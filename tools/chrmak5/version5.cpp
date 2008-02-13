#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "a_codec/a_codec.h"
#include "a_config.h"

#include "corona/corona.h"

#define CHR5_FLAG_ALPHA 1
#define COMPRESSION_CODEC_ZLIB 1


typedef unsigned int Color;
typedef unsigned short int Color16;
#define COLOR(r,g,b) ((((unsigned int)r)<<16)|(((unsigned int)g)<<8)|(((unsigned int)b)))
#define COLORALPHA(r,g,b,a) ((((unsigned int)a)<<24)|(((unsigned int)r)<<16)|(((unsigned int)g)<<8)|(((unsigned int)b)))
#define COLORALPHAFROMCOLOR(c,a) ((((unsigned int)a)<<24)|(c))
#define COLOR16FROMCOLOR(c) ((((c)&0x00F80000)>>8)|(((c)&0x0000FC00)>>5)|(((c)&0x000000F8)>>3))
#define COLORFROMCOLOR16(c) ((((c)&0xF800)<<8)|(((c)&0x07E0)<<5)|(((c)&0x001F)<<3))
#define COLOR16(r,g,b) ((((unsigned int)(r)&0xF8)<<8)|(((unsigned int)(g)&0xFC)<<3)|((unsigned int)(b)>>3))



static int encode_v1_byte(unsigned char *src, int len, unsigned char *outbuf);


corona::Image *imgLoad(char *filename, int bpp)
{
	corona::PixelFormat pf;
	if(bpp == 8)
		pf = corona::PF_DONTCARE;
	else
		pf = corona::PF_R8G8B8A8;

	corona::Image *corimg = corona::OpenImage(filename,corona::FF_AUTODETECT,pf);

	if(!corimg)
		return 0;

	return corimg;
}


void chrmak_v5_v2(char *fname,int version_to_export)
{
	char *mandatorykeys[]={"in","frame_w","frame_h","frames_per_row","frame_rows","d_idle","u_idle","l_idle","r_idle","d_walk","u_walk","l_walk","r_walk"};
	//optionalkeys: in_alpha, hotspot_x, hotspot_y, hotspot_w, hotspot_h, unpadded, tcolor_r, tcolor_g, tcolor_b, force32bpp, compressionflags, ul_walk, ur_walk, dl_walk, dr_walk
	int i;
	int version;
	corona::Image *s =0, *sa =0;
	FILE *outf=0;

	int compressionflags;
	int frame_w,frame_h;
	int hotspot_x,hotspot_y,hotspot_w,hotspot_h;
	int flags=0;
	int frame_count;
	int frames_per_row;
	int frame_rows;
	int d_idle,u_idle,l_idle,r_idle;
	int pad_add;
	int bpp=24;
	int num_customs;
	char custom_key[15];
	Color tcol;

	printf("Processing makfile...\n");
	
	//make sure mandatory keys are present in makfile
	for(i=0;i<(sizeof mandatorykeys)/4;i++)
	{
		if(!cfg_KeyPresent(mandatorykeys[i]))
		{
			printf("ERROR: version5/2: mandatory command [%s] not present. <%s>\n",mandatorykeys[i],fname);
			exit(0);
		}
	}

	//set default key values for optional keys
	cfg_SetDefaultKeyValue("hotspot_w", "16");
	cfg_SetDefaultKeyValue("hotspot_h", "16");
	cfg_SetDefaultKeyValue("hotspot_x","0");
	cfg_SetDefaultKeyValue("hotspot_y","16");
	cfg_SetDefaultKeyValue("tcolor_r","255");
	cfg_SetDefaultKeyValue("tcolor_g","0");
	cfg_SetDefaultKeyValue("tcolor_b","255");
	cfg_SetDefaultKeyValue("compressionflags","1");
	cfg_SetDefaultKeyValue("num_custom","0");
	cfg_SetDefaultKeyValue("ul_walk",cfg_GetKeyValue("l_walk"));
	cfg_SetDefaultKeyValue("ur_walk",cfg_GetKeyValue("r_walk"));
	cfg_SetDefaultKeyValue("dl_walk",cfg_GetKeyValue("l_walk"));
	cfg_SetDefaultKeyValue("dr_walk",cfg_GetKeyValue("r_walk"));


	//read necessary config keys
	compressionflags=atoi(cfg_GetKeyValue("compressionflags"));
	frame_w=atoi(cfg_GetKeyValue("frame_w"));
	frame_h=atoi(cfg_GetKeyValue("frame_h"));
	hotspot_x=atoi(cfg_GetKeyValue("hotspot_x"));
	hotspot_y=atoi(cfg_GetKeyValue("hotspot_y"));
	hotspot_w=atoi(cfg_GetKeyValue("hotspot_w"));
	hotspot_h=atoi(cfg_GetKeyValue("hotspot_h"));
	frames_per_row=atoi(cfg_GetKeyValue("frames_per_row"));
	frame_rows=atoi(cfg_GetKeyValue("frame_rows"));
	frame_count=frames_per_row*frame_rows;
	d_idle=atoi(cfg_GetKeyValue("d_idle"));
	u_idle=atoi(cfg_GetKeyValue("u_idle"));
	l_idle=atoi(cfg_GetKeyValue("l_idle"));
	r_idle=atoi(cfg_GetKeyValue("r_idle"));
	tcol=COLOR(atoi(cfg_GetKeyValue("tcolor_r")),atoi(cfg_GetKeyValue("tcolor_g")),atoi(cfg_GetKeyValue("tcolor_b")));
	pad_add=cfg_KeyPresent("unpadded")?0:1;
	
	if(cfg_KeyPresent("force32bpp"))
	{
		bpp=32;
	}
	num_customs=atoi(cfg_GetKeyValue("num_custom"));

	

	//attempt to open the outfile
	outf=fopen(cfg_GetKeyValue("out"),"wb");
	if(!outf)
	{
		printf("ERROR: version5/2: could not open specified OUT file [%s]. <%s>\n",cfg_GetKeyValue("out"),fname);
		exit(0);
	}

	printf("Loading image(s)...\n");

	//attempt to load the IN file
	if(version_to_export==5)
	{
		s = imgLoad(cfg_GetKeyValue("in"),24);
		if(!s)
		{
			printf("ERROR: version5: could not open specified IN file [%s]. unreadable format or incorrect filename? <%s>\n",cfg_GetKeyValue("in"),fname);
			exit(0);
		}
	}
	else
	{
		s = imgLoad(cfg_GetKeyValue("in"),8);
		if(!s)
		{
			printf("ERROR: version2: could not open specified IN file [%s]. unreadable format or incorrect filename? <%s>\n",cfg_GetKeyValue("in"),fname);
			exit(0);
		}
		if(s->getFormat()!=corona::PF_I8)
		{
			printf("ERROR: version2: specified IN file [%s] is not 8bpp. <%s>",cfg_GetKeyValue("in"),fname);
			exit(0);
		}
	}


	//if we need to ,attempt to load the IN_ALPHA file
	if(version_to_export==5)
	{
		if(cfg_KeyPresent("in_alpha"))
		{
			/*flags|=CHR5_FLAG_ALPHA;
			bpp=32;
			if(!sa.Load(cfg_GetKeyValue("in_alpha")))
			{
				printf("ERROR: version5: could not open specified IN_ALPHA file [%s]. unreadable format or incorrect filename? <%s>\n",cfg_GetKeyValue("in_alpha"),fname);
				exit(0);
			}*/
			
			printf("ERROR: version2: IN_ALPHA file [%s] is not presently supported in version5 chrfiles. <%s>\n",cfg_GetKeyValue("in_alpha"),fname);
		}
	}
	else
	{
		if(cfg_KeyPresent("in_alpha"))
		{
			printf("ERROR: version2: IN_ALPHA file [%s] is not supported in version2 chrfiles. <%s>\n",cfg_GetKeyValue("in_alpha"),fname);
			exit(0);
		}
	}


	//ensure that IN and IN_ALPHA file dimensions are sufficent
	if(version_to_export==5)
	{
		if(s->getWidth()<frames_per_row*(frame_w+pad_add)||s->getHeight()<frame_rows*(frame_h+pad_add))
		{
			printf("ERROR: version5: actual dimensions of IN file [%s] are smaller than .makfile specifications demand. <%s>\n",cfg_GetKeyValue("in"),fname);
			exit(0);
		}
		/*if(flags&CHR5_FLAG_ALPHA)
			if(sa.nWidth<frames_per_row*(frame_w+pad_add)||sa.nHeight<frame_rows*(frame_h+pad_add))
			{
				printf("ERROR: version5: actual dimensions of IN_ALPHA file [%s] are smaller than .makfile specifications demand. <%s>\n",cfg_GetKeyValue("in_alpha"),fname);
				exit(0);
			}*/
	}


	if(version_to_export==2)
	{
		printf("Exporting version2 chrfile...\n");

		printf("Writing chr header...\n");

		//write the chr header
		version=2;
		fwrite(&version,1,1,outf);
		fwrite(&frame_w,2,1,outf);
		fwrite(&frame_h,2,1,outf);
		fwrite(&hotspot_x,2,1,outf);
		fwrite(&hotspot_y,2,1,outf);
		fwrite(&hotspot_w,2,1,outf);
		fwrite(&hotspot_h,2,1,outf);
		fwrite(&frame_count,2,1,outf);

		unsigned char *framebuf=(unsigned char*)malloc(frame_count*frame_w*frame_h);
		unsigned char *fartbreath=framebuf;

		int x,y,px,py,cp = 0;
		Color c;
		for(y=0;y<frame_rows;y++)
		{
			for(x=0;x<frames_per_row;x++)
			{
				for(py=0;py<frame_h;py++)
				{
					for(px=0;px<frame_w;px++)
					{
						//c=((Color *)s.pData)                        [(y*(frame_h+pad_add)+pad_add+py)*s.nPitch+x*(frame_w+pad_add)+pad_add+px];
						*fartbreath=((unsigned char *)s->getPixels())[(y*(frame_h+pad_add)+pad_add+py)*s->getWidth()+x*(frame_w+pad_add)+pad_add+px];
						//if((px&1)^(py&1))
						//	*fartbreath=139;
						//else
						//	*fartbreath=55;;
						fartbreath++;
					}
				}
			}
		}

		unsigned char *outbuf=(unsigned char* )malloc(s->getWidth()*s->getHeight()*2);

		int poocrap=encode_v1_byte((unsigned char *)framebuf,frame_count*frame_w*frame_h,outbuf);
		fwrite(&poocrap,4,1,outf);
		fwrite(outbuf,1,poocrap,outf);
		free(outbuf);
		free(framebuf);

		fwrite(&d_idle,4,1,outf);
		fwrite(&u_idle,4,1,outf);
		fwrite(&l_idle,4,1,outf);
		fwrite(&r_idle,4,1,outf);	

		i=strlen(cfg_GetKeyValue("l_walk"))+1;
		if(i>99)
		{
			printf("ERROR: version2: leftwalk animstring too long");
			exit(0);
		}
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("l_walk"),i,1,outf);
		i=strlen(cfg_GetKeyValue("r_walk"))+1;
		if(i>99)
		{
			printf("ERROR: version2: rightwalk animstring too long");
			exit(0);
		}
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("r_walk"),i,1,outf);
		i=strlen(cfg_GetKeyValue("u_walk"))+1;
		if(i>99)
		{
			printf("ERROR: version2: upwalk animstring too long");
			exit(0);
		}
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("u_walk"),i,1,outf);
		i=strlen(cfg_GetKeyValue("d_walk"))+1;
		if(i>99)
		{
			printf("ERROR: version2: downwalk animstring too long");
			exit(0);
		}
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("d_walk"),i,1,outf);

	}

	if(version_to_export==5)
	{
		printf("Exporting version5 chrfile...\n");
		
		printf("Writing chr header...\n");

		//write the chr header
		fwrite("CHR",1,4,outf);
		version=5;
		fwrite(&version,4,1,outf);
		fwrite(&bpp,4,1,outf);
		fwrite(&flags,4,1,outf);
		fwrite(&tcol,4,1,outf);
		fwrite(&hotspot_x,4,1,outf);
		fwrite(&hotspot_y,4,1,outf);
		fwrite(&hotspot_w,4,1,outf);
		fwrite(&hotspot_h,4,1,outf);
		fwrite(&frame_w,4,1,outf);
		fwrite(&frame_h,4,1,outf);
		fwrite(&frame_count,4,1,outf);
		fwrite(&d_idle,4,1,outf);
		fwrite(&u_idle,4,1,outf);
		fwrite(&l_idle,4,1,outf);
		fwrite(&r_idle,4,1,outf);
		i=strlen(cfg_GetKeyValue("d_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("d_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("u_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("u_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("l_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("l_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("r_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("r_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("ul_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("ul_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("ur_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("ur_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("dl_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("dl_walk"),i+1,1,outf);
		i=strlen(cfg_GetKeyValue("dr_walk"));
		fwrite(&i,4,1,outf);
		fwrite(cfg_GetKeyValue("dr_walk"),i+1,1,outf);

		
	  
		printf("Writing custom scripts...\n");

		fwrite(&num_customs, 4, 1, outf);
		for(int q=0;q < num_customs;q++)
		{
			sprintf(custom_key, "custom%i", q);
			i=strlen(cfg_GetKeyValue(custom_key));
			fwrite(&i, 4, 1, outf);
			fwrite(cfg_GetKeyValue(custom_key), i+1, 1, outf);
		}

		printf("Dumping frames...\n");

		//dump the frames
		int uncomp_size = frame_count * frame_h * frame_w * (bpp / 8);
		unsigned char* framebuf = new unsigned char[uncomp_size];
		int x,y,px,py,cp = 0;
		Color c;
		for(y=0;y<frame_rows;y++)
		{
			for(x=0;x<frames_per_row;x++)
			{
				for(py=0;py<frame_h;py++)
				{
					for(px=0;px<frame_w;px++)
					{
						c=((Color *)s->getPixels())[(y*(frame_h+pad_add)+pad_add+py)*s->getWidth()+x*(frame_w+pad_add)+pad_add+px];
						/*if(flags&CHR5_FLAG_ALPHA)
						{
							c=COLORALPHAFROMCOLOR(c,((Color *)sa.pData)[(y*(frame_h+pad_add)+pad_add+py)*sa.nPitch+x*(frame_w+pad_add)+pad_add+px]);
						}*/
						if(bpp==32)
							memcpy(framebuf+(cp++), &c, 4);
						if(bpp==24)
						{
							framebuf[cp++]=((unsigned char *)&c)[0];//(c&16711680)>>16;
							framebuf[cp++]=((unsigned char *)&c)[1];//(c&65280)>>8;
							framebuf[cp++]=((unsigned char *)&c)[2];//c&255;
						}
					}
				}
			}
		}
	
		fwrite(&compressionflags,4,1,outf);

		if(compressionflags)
		cfwrite(framebuf, uncomp_size, 1, outf);
		else
		fwrite(framebuf,uncomp_size,1,outf);

	}//end ver5


	//clean up
	if(s) delete s;
	if(sa) delete sa;

	fclose(outf);

}


static int encode_v1_byte(unsigned char *src, int len, unsigned char *outbuf)
{
	int samect;
	int i;
	int data;
	int outindex;

	i=0;
	outindex=0;
	do
	{
		data=src[i++];
		samect=1;
		while(samect<254&&i<len&&data==src[i])
		{
			samect++;
			i++;
		}

		if(data!=255)
		{
			if(samect==2)
				outbuf[outindex++]=data;
			if(samect==3)
			{
				outbuf[outindex++]=data;
				outbuf[outindex++]=data;
			}
		}
		if(samect>3||data==255)
		{
			outbuf[outindex++]=255;
			outbuf[outindex++]=samect;
		}
		outbuf[outindex++]=data;
	} while(i<len);

	return outindex;
}
