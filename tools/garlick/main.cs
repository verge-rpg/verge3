using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;

//the garlick file format is very simple:
//to start with, write the logical length of the file as an int64.
//then, repeat this until you're finished:
//write an int value describing how many bytes will be encoded in the next batch.
//if bit 31 is set, then the bits come from the library.  
// - write a byte describing the encoding format
// - write 64bit pointer into the library describing the offset to those bytes.
//if bit31 is clear, then those bytes are are simply N bytes in the input file.
//the end.
//
//mod will be pains to decode, due to variability
//we will do a very dumb packing, for now, in the interest of obfuscation, and just put the whole thing in the library file
//
//we should consider trying to intelligently dice spans when there are significant but imperfect matches
//(maybe this happens a lot? collect statistics.)
//
//todO: support IT sample compression?
//probably not.

class main {
	static void Main(string[] args) {
		
		pr2.Garlick.Garlicker g = new pr2.Garlick.Garlicker();

		string libname = "library";
		for(int i = 0; i < args.Length; i++) {
			if(args[i].ToLower() == "-flac") g.flac = true;
			else libname = args[i];
		}

		//g.flac = true;

		string str = null;
		while((str = Console.ReadLine()) != null) {
		//foreach(FileInfo fi in new DirectoryInfo(Directory.GetCurrentDirectory()).GetFiles("*.*")) {
		//    string str = fi.Name;
		    if(str == "library") continue;
			string ext = Path.GetExtension(str).ToLower();
			if(ext[ext.Length - 1] == '_') continue;
			Console.WriteLine("- " + str);  
			switch(ext) {
				case ".it": g.processIT(str); break;
				case ".s3m": g.processS3M(str); break;
				case ".xm": g.processXM(str); break;
				case ".mod":
					g.processSimple(str); break;
				default: Console.WriteLine(" * ignoring");  break;
			}
		}

		

		g.writeLibrary(libname);

		//pr2.Garlick.UnGarlicker ug = new pr2.Garlick.UnGarlicker();
		//foreach(string str in new string[] { "bj-smell.s3m", "CUTE.S3M", "BLANK.S3M", "four.s3m", "pitfall.s3m", "sword.s3m", "ZCVBOSS.S3M", "ZCVBOSS2.S3M", "ZFNLBATL.S3M" }) {
		//foreach(string str in new string[] { "bj-smell.s3m" }) {
		//	ug.expand(str + "_", str + "_.s3m", "library");
		//}

		//pr2.Garlick.UnGarlicker ug = new pr2.Garlick.UnGarlicker();
		//ug.expand(@"C:\Documents and Settings\Administrator\Desktop\zeta\zw-rainforest.it_", @"C:\Documents and Settings\Administrator\Desktop\zeta\zw-rainforest.it_.it", @"C:\Documents and Settings\Administrator\Desktop\zeta\library");
	}
}

//todo: consider whether it is worth restructuring so that the library is not used for chunks which only appear once
//wont make much of a difference in the compression ratio or decoding speed, although it will change them somewhat --uncertain whether for good or ill.
//at any rate, it wont change the disk protocol so nobody will ever have to know
namespace pr2.Garlick {

	/// <summary>
	/// creates garlick files and libraries. only supports IT > 2.00; shouldnt be a problem.
	/// does not support compressed samples.
	/// this class is not thread-safe
	/// </summary>
	public class Garlicker {

		/// <summary>
		/// indicates whether flaccing is allowed
		/// </summary>
		public bool flac = false;


		/// <summary>
		/// describes a scan of bytes.
		/// </summary>
		class Span {
			public Span(long start, long length) { this.start = start; this.length = length; type = Type.Raw;  }
			public long start, length;
			
			public Type type;
			[Flags]
			public enum Type : byte {
				Raw = 0, 
				Flac = 1,
				Stereo = 2, 
				Bits16 = 4,
				//8 = stereo Planar bit
				StereoPlanar = 10,
				Unsigned = 16,
				XMDelta = 32
			}
		}

		/// <summary>
		/// describs a blob in the library
		/// </summary>
		struct LibraryBlob {
			public long start;
			public byte[] blob;
			public byte[] blob_encoded;
		}

		List<LibraryBlob> libraryBlobs = new List<LibraryBlob>();
		long libraryCursor = 0;

		bool areBuffersEqual(byte[] x, byte[] y) {
			if(x.Length != y.Length) return false;
			int len = x.Length;
			for(int i = 0; i < len; i++)
				if(x[i] != y[i]) {
					if(len > 1000) {
						decimal ratio = (decimal)i / (decimal)len;
						if(ratio > 0.10M)
							Console.WriteLine("({0}/{1}) Significant but imperfect match: {2}", i,len, ratio);
					}
					return false;
				}
			return true;
		}

		/// <summary>
		/// attempts to find a match for the provided data in the library.
		/// if it is present, it writes the long pointer to the location record in the library
		/// if it is absent, it adds a new record to the library and returns a pointer to that
		/// </summary>
		void matchOrAdd(BinaryWriter outfile, byte[] data, Span.Type spanType) {
			foreach(LibraryBlob lb in libraryBlobs) {
				if(areBuffersEqual(data, lb.blob)) {
					//if(lb.blob.Length > 1000) Console.WriteLine("{0} long match!",lb.blob.Length);
					outfile.Write(lb.start);
					return;
				}
			}

			MemoryStream mstemp = new MemoryStream();
			BinaryWriter bw = new BinaryWriter(mstemp);
			

			//didnt find a match.. add to library
			//perhaps encode the blob
			if((spanType & Span.Type.Flac) != 0) {
				byte[] flacdata = FLACDotNet.FLACWriter.Encode(data,
					(spanType & Span.Type.Bits16) != 0 ? 16 : 8,
					(spanType & Span.Type.Stereo) != 0 ? 2 : 1,
					(spanType & Span.Type.StereoPlanar) != 0,
					(spanType & Span.Type.Unsigned) != 0,
					(spanType & Span.Type.XMDelta) != 0
					);

				//sometimes flac will do a bad job. write the raw bytes out in that case
				if(flacdata.Length >= data.Length) {
					Console.WriteLine("Dumping {0} raw sample bytes with bad flac performance", data.Length);
					bw.Write((byte)0); bw.Flush(); //write the encoding type
					mstemp.Write(data, 0, data.Length);
				} else {
					bw.Write((byte)spanType); bw.Flush(); //write the encoding type
					bw.Write(flacdata.Length); bw.Flush(); //write the encoded length
					mstemp.Write(flacdata, 0, flacdata.Length);
					Console.WriteLine("Flac [{0}]: {1} ({2})", spanType, (decimal)flacdata.Length / (decimal)data.Length, data.Length);
				}
			} else {
				bw.Write((byte)0); bw.Flush(); //write the encoding type
				mstemp.Write(data,0,data.Length);
			}

			LibraryBlob newBlob = new LibraryBlob();
			newBlob.start = libraryCursor;
			newBlob.blob = (byte[])data.Clone();
			newBlob.blob_encoded = mstemp.ToArray(); ;
			libraryCursor += newBlob.blob_encoded.Length;
			libraryBlobs.Add(newBlob);
			outfile.Write(newBlob.start);
		}

		byte[] tempbuf = new byte[1024];
		void copy(FileStream fsin, FileStream fsout, int count) {
			while(count > 0) {
				int todo = Math.Min(count, tempbuf.Length);
				fsin.Read(tempbuf, 0, todo);
				fsout.Write(tempbuf, 0, todo);
				count -= todo;
			}
		}

		/// <summary>
		/// writes the library to the specified file
		/// </summary>
		public void writeLibrary(string fname) {
			using(FileStream fsout = new FileStream(fname, FileMode.Create, FileAccess.Write, FileShare.Read)) {
				foreach(LibraryBlob lb in libraryBlobs)
					fsout.Write(lb.blob_encoded, 0, lb.blob_encoded.Length);
			}
		}

		/// <summary>
		/// do not give me overlapping spans. that would be cruel.
		/// </summary>
		void process(string fnin, FileStream fsin) {
			//reset input file
			fsin.Position = 0;

			string fnout = fnin + "_";
			//sort spans
			spans.Sort(delegate(Span x, Span y) { long diff = x.start - y.start; if(diff < 0) return -1; else if(diff > 0) return 1; else return 0; });

			//undo flac if it is forbidden
			if(!flac)
				spans.ForEach(delegate(Span span) { span.type = span.type & ~Span.Type.Flac; });

			//prep outfile
			using(FileStream fsout = new FileStream(fnout, FileMode.Create, FileAccess.Write, FileShare.Read)) {
				try {
					//write logical length
					BinaryWriter bw = new BinaryWriter(fsout);
					long fsinlen = fsin.Length;
					bw.Write(fsinlen); bw.Flush();
					
					long cursor = 0;
					foreach(Span span in spans) {
						//Console.WriteLine("{0:x4} {1}", span.start, span.length);

						long next = span.start;
						long todo = next - cursor;
						cursor += todo;
						//write a raw byte record, if we have any raw bytes
						while(todo > 0) {
							int chunk = (int)Math.Min((long)int.MaxValue, todo);
							todo -= chunk;
							//write the length of the raw chunk
							bw.Write(chunk); bw.Flush();
							//write the raw chunk
							copy(fsin, fsout, chunk);
						};
						//write a library record
						todo = span.length;
						cursor += todo;
						do {
							int chunk = (int)Math.Min((long)int.MaxValue, todo);
							todo -= chunk;
							//write the logicallength of the libraried chunk
							bw.Write((uint)chunk|0x80000000); bw.Flush();
							//read the data to be libraried
							byte[] temp = new byte[chunk];
							fsin.Read(temp, 0, chunk);
							//write the pointer into the library
							//bw.Write((byte)span.type);
							//bw.Write(matchOrAdd(temp, span.type)); bw.Flush();
							matchOrAdd(bw, temp, span.type);
							bw.Flush();
						} while(todo > 0) ;
					}
					
					//write one final raw chunk
					{
						long todo = fsinlen - cursor;
						while(todo > 0) {
							int chunk = (int)Math.Min((long)int.MaxValue, todo);
							todo -= chunk;
							//write the length of the raw chunk
							bw.Write(chunk); bw.Flush();
							//write the raw chunk
							copy(fsin, fsout, chunk);
						}
					}

				} catch { throw; } finally { try { File.Delete(fnout); } catch { } }
			} 
		}

		List<Span> spans = new List<Span>();
		void span(long start, long length) {
			spans.Add(new Span(start, length));
		}

		void span(Span.Type type, long start, long length) {
			Span s = new Span(start, length);
			s.type = type;
			spans.Add(s);
		}

		//just stuff it in the library file
		public void processSimple(string fname) {
			spans.Clear();
			using(FileStream fsin = new FileStream(fname, FileMode.Open, FileAccess.Read, FileShare.Read)) {
				span(0, fsin.Length);
				process(fname, fsin);
			}
		}

		public void processXM(string fname) {
			spans.Clear();
			using(FileStream fsin = new FileStream(fname, FileMode.Open, FileAccess.Read, FileShare.Read)) {
				BinaryReader br = new BinaryReader(fsin);
				fsin.Position = 60;

				uint headerlength = br.ReadUInt32();
				ushort length = br.ReadUInt16();
				ushort restart = br.ReadUInt16();
				ushort numchannels = br.ReadUInt16();
				ushort numpatterns = br.ReadUInt16();
				ushort numinstruments = br.ReadUInt16();
				fsin.Position = 60 + headerlength;

				//skip patterns
				for(int i = 0; i < numpatterns; i++) {
					long start = fsin.Position;
					uint patheaderlength = br.ReadUInt32();
					byte packing = br.ReadByte();
					ushort numrows = br.ReadUInt16();
					ushort packedsize = br.ReadUInt16();
					//skip
					fsin.Position = start + patheaderlength + packedsize;
				}

				for(int i = 0; i < numinstruments; i++) {
					//Console.WriteLine("inst#{0}", i);
					long start = fsin.Position;
					uint size = br.ReadUInt32();
					fsin.Position += 22; //skip name;
					fsin.Position++; //skip type
					ushort numsamples = br.ReadUInt16();
					if(numsamples == 0) {
						span(start, size);
						fsin.Position = start + size;
						continue;
					}
					span(start, size);
					uint sampheadersize = br.ReadUInt32();
					fsin.Position = start + size;
					List<uint> lengths = new List<uint>();
					List<byte> flags = new List<byte>();
					for(int j = 0; j < numsamples; j++) {
						//Console.WriteLine("- samp#{0}", j);
						long sampstart = fsin.Position;
						span(sampstart, sampheadersize);
						lengths.Add(br.ReadUInt32());
						fsin.Position = sampstart + 14;
						flags.Add(br.ReadByte());
						fsin.Position = sampstart + sampheadersize;
					}
					for(int j = 0; j < numsamples; j++) {
						if(lengths[j] == 0) continue;
						//Console.WriteLine("* samp#{0} [{1}]", j, lengths[j]);
						long sampstart = fsin.Position;
						byte flag = flags[j];
						uint samplength = lengths[j];
						if((flag & 0x10) != 0)
							span(Span.Type.Flac | Span.Type.Bits16 | Span.Type.XMDelta, sampstart, samplength);
						else
							span(Span.Type.Flac | Span.Type.XMDelta, sampstart, samplength);
						Console.WriteLine("FLAG : " + flag + " (watch out for values >19)");
						fsin.Position = sampstart + samplength;
					}
				}

				process(fname, fsin);
			}
		}

		public void processS3M(string fname) {
			spans.Clear();
			using(FileStream fsin = new FileStream(fname, FileMode.Open, FileAccess.Read, FileShare.Read)) {
				BinaryReader br = new BinaryReader(fsin);

				fsin.Seek(0x20, SeekOrigin.Begin);
				ushort ordnum = br.ReadUInt16();
				ushort insnum = br.ReadUInt16();
				ushort patnum = br.ReadUInt16();

				//skip to instrument offsets
				fsin.Position = 0x60 + ordnum;
				//read instrument pointers
				uint[] insptrs = new uint[insnum];
				for(int i = 0; i < insnum; i++)
					insptrs[i] = ((uint)br.ReadUInt16())*16;

				//process instruments
				for(int i = 0; i < insnum; i++) {
					//add span for the sample header
					span(insptrs[i] + 0x10, 0x30);
					
					//check to see if the sample is used
					fsin.Position = insptrs[i];
					if(fsin.ReadByte() != 1) continue;

					//skip to sampledata ptr
					fsin.Position = insptrs[i] + 0x0E;
					uint ptr = (uint)br.ReadUInt16()*16;
					//read length
					ushort length = br.ReadUInt16();
					//skip to flags
					fsin.Position = insptrs[i] + 0x1E;
					byte pack = br.ReadByte();
					byte flags = br.ReadByte();

					bool bstereo = (flags & 2) != 0;
					bool b16 = (flags & 4) != 0;
					if(b16)
						if(bstereo) span(Span.Type.Flac | Span.Type.StereoPlanar | Span.Type.Bits16 | Span.Type.Unsigned, ptr, length * 4);
						else span(Span.Type.Flac | Span.Type.Bits16 | Span.Type.Unsigned, ptr, length * 2);
					else
						if(bstereo) span(Span.Type.Flac | Span.Type.StereoPlanar | Span.Type.Unsigned, ptr, length * 2);
						else span(Span.Type.Flac | Span.Type.Unsigned, ptr, length);
					
					////old
					//span(ptr, length);
					////handle stereo
					//if((flags & 0x02) != 0)
					//    span(ptr+length, length);
				}

				process(fname, fsin);
			}
		}

		public void processIT(string fname) {
			spans.Clear();
			using(FileStream fsin = new FileStream(fname, FileMode.Open, FileAccess.Read, FileShare.Read)) {
				BinaryReader br = new BinaryReader(fsin);

				fsin.Seek(0x20, SeekOrigin.Begin);
				ushort ordnum = br.ReadUInt16();
				ushort insnum = br.ReadUInt16();
				ushort smpnum = br.ReadUInt16();

				//add spans for module header..? nah its lame
				//spans.Add(new Span(0x00, 0x20));
				//spans.Add(new Span(0x40, 0x40));
				//spans.Add(new Span(0x80, 0x40));
				
				//skip to instrument offsets
				fsin.Position = 0xC0 + ordnum;
			
				//read instrument header offsets and add spans
				//this saves ~100k in troupes zeux modpack of 15.0M
				for(int i = 0; i < insnum; i++) {
					int insofs = br.ReadInt32();
					span(insofs, 554);
				}

				//read sample header offsets
				int[] smpofs = new int[smpnum];
				for(int i = 0; i < smpnum; i++) smpofs[i] = br.ReadInt32();
				
				//process each sample
				for(int i = 0; i < smpnum; i++) {
					//add a span for the sample header
					//this saves ~10K in troupes zeux modpack of 15.0M
					span(smpofs[i], 0x30);
					

					//fetch flag
					fsin.Position = smpofs[i] + 0x12;
					byte flags = br.ReadByte();
					fsin.Position = smpofs[i] + 0x30;
					int length = br.ReadInt32();
					fsin.Position = smpofs[i] + 0x48;
					int ptr = br.ReadInt32();
					
					//these appear to be empty sample records
					if((flags & 1) == 0) continue;
					if(length == 0) continue; //bogus also

					//calculate the sample length
					//if((flags&1)!=0) ?
					bool b16 = (flags & 2) != 0;
					bool bstereo = (flags & 4) != 0;
					bool bcompr = (flags & 8) != 0;
					//if((flags & 2) != 0) length *= 2; //16bit
					//if((flags & 4) != 0) length *= 2; //stereo
					if(bcompr) { }
					else 
						if(b16)
							if(bstereo) span(Span.Type.Flac | Span.Type.Stereo | Span.Type.Bits16, ptr, length * 4);
							else span(Span.Type.Flac | Span.Type.Bits16, ptr, length * 2);
						else
							if(bstereo) span(Span.Type.Flac | Span.Type.Stereo, ptr, length * 2);
							else span(Span.Type.Flac, ptr, length);
				}

				process(fname, fsin);
			}
		}
	}

	/// <summary>
	/// tests the expanding algorithm
	/// </summary>
	public class UnGarlicker {

		byte[] tempbuf = new byte[1024];
		void copy(FileStream fsin, FileStream fsout, int count) {
			while(count > 0) {
				int todo = Math.Min(count, tempbuf.Length);
				fsin.Read(tempbuf, 0, todo);
				fsout.Write(tempbuf, 0, todo);
				count -= todo;
			}
		}

		public void expand(string fnin, string fnout, string fnlib) {
			using(FileStream fsin = File.OpenRead(fnin)) {
				using(FileStream fsout = File.OpenWrite(fnout)) {
					using(FileStream fslib = File.OpenRead(fnlib)) {
						BinaryReader br = new BinaryReader(fsin);
						long len = br.ReadInt64();
						long todo = len;
						while(todo > 0) {
							//do a raw chunk
							int chunk = br.ReadInt32() & 0x7FFFFFFF;
							todo -= chunk;
							copy(fsin, fsout, chunk);
							if(todo == 0) break;
							//do a library chunk
							chunk = br.ReadInt32() & 0x7FFFFFFF;
							byte mode = br.ReadByte();
							todo -= chunk;
							fslib.Position = br.ReadInt64();
							copy(fslib, fsout, chunk);
						}
					}
				}
			}
		}
	}

}