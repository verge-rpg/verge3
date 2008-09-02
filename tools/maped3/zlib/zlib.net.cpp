namespace c
{
	#include <stdlib.h>
	#include <string.h>
	#include <windows.h>
	#include <stdio.h>
}

#using <mscorlib.dll>
using namespace System;

namespace zlib
{
	#include "zlib.h"
}

public __gc class ZLIB
{
public:

	__gc class  Exception : public System::ApplicationException
	{
	private:
		int m_errorcode;
	public:

		__property virtual System::String __gc * get_Message()
		{
			switch(m_errorcode)
			{
			case Z_MEM_ERROR:
				return new System::String(S"ZLIB: Z_MEM_ERROR");
			case Z_BUF_ERROR:
				return new System::String(S"ZLIB: Z_BUF_ERROR");
			case Z_STREAM_ERROR:
				return new System::String(S"ZLIB: Z_STREAM_ERROR");
			}
		}

		Exception(int errorcode)
		{
			m_errorcode=errorcode;
		}

	};

public:

	static Byte Encode(unsigned char *srcbuf, int srclen) []
	{
		int i;

		if(srclen==0)
		{
			Byte  dest __gc []=__gc new Byte[0];
			return dest;
		}


		unsigned int destlen=(int)((double)srclen*1.001+0.5)+12;
		unsigned char *destbuf=(unsigned char *)c::malloc(destlen);
		
		int ok=zlib::compress(destbuf,(zlib::uLongf *)&destlen,srcbuf,srclen);
		if(ok!=Z_OK)
		{
			c::free(destbuf);
			throw( new ZLIB::Exception(ok));
		}
		
		
		Byte  dest __gc []=__gc new Byte[destlen];
		for(i=0;i<destlen;i++)
			dest[i]=destbuf[i];

		c::free(destbuf);

		return dest;		
	}

	static Byte Encode(Byte src __gc []) []
	{
		if(src->Length == 0)
			return __gc new Byte[0];

		unsigned char __pin *srcbuf=&src[0];
		return Encode(srcbuf,src->Length);
	}

private:
	static void _decode(unsigned char *srcbuf, int srclen, unsigned char *destbuf, int destlen)
	{
		int _destlen = destlen;
		int ok=zlib::uncompress(destbuf,(zlib::uLongf *)&destlen,srcbuf,srclen);
		if(ok!=Z_OK)
			throw( new ZLIB::Exception(ok));
	}

public:
	static Byte Decode(Byte src __gc [], int size) []
	{
		int i;

		if(size==0)
		{
			Byte dest __gc []=__gc new Byte[0];
			return dest;
		}

		unsigned char __pin* srcbuf=&src[0];
		Byte dest __gc []=__gc new Byte[size];
		unsigned char __pin* destbuf=&dest[0];

		_decode(srcbuf,src->Length,destbuf,size);

		return dest;
	}



	static int DecodeInts(Byte src __gc [], int size) __gc[]
	{
		if((size&3) != 0)
			throw __gc new ArgumentException("zlib.net: DecodeInts -- size argument must be multiple of 4");

		if(size==0)
		{
			Int32 dest __gc []=__gc new Int32[0];
			return dest;
		}
		
		unsigned char __pin* srcbuf=&src[0];

		int intArr __gc[] = new int __gc[size/4];
		int __pin* destbuf=&intArr[0];
		unsigned char __pin* _destbuf= (unsigned char*)destbuf;

		_decode(srcbuf,src->Length,_destbuf,size);
		
		return intArr;
	}

	static Int16 DecodeShorts(Byte src __gc [], int size) __gc[]
	{
		if((size&1) != 0)
			throw __gc new ArgumentException("zlib.net: DecodeShorts -- size argument must be multiple of 2");

		if(size==0)
		{
			Int16 dest __gc []=__gc new Int16[0];
			return dest;
		}

		unsigned char __pin* srcbuf=&src[0];

		Int16 shortArr __gc[] = new Int16 __gc[size/2];
		Int16 __pin* destbuf=&shortArr[0];
		unsigned char __pin* _destbuf= (unsigned char*)destbuf;

		_decode(srcbuf,src->Length,_destbuf,size);
		
		return shortArr;
	}

	
}; 
