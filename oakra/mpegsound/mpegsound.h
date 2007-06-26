/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Mpegsound.h
//   This is typeset for functions in MPEG Sound library.
//   Now, it's for only linux-pc-?86

/*
 * Chad Austin, 2003.06.01
 *
 * Build the MP3 decoder source files with custom optimizations in
 * VC6.  Some files have static when all of the optimizations are
 * enabled.
 *
 * I've also tested this code on Linux and SGI.
 */

/************************************/
/* Inlcude default library packages */
/************************************/
#include <stdio.h>
#include <sys/types.h>
#include <string.h>


#ifndef _L__SOUND__
#define _L__SOUND__

/****************/
/* Sound Errors */
/****************/
// General error
#define SOUND_ERROR_OK                0
#define SOUND_ERROR_FINISH           -1

// Sound file (for reader)
#define SOUND_ERROR_FILEOPENFAIL      5
#define SOUND_ERROR_FILEREADFAIL      6

// Miscellneous (for translater)
#define SOUND_ERROR_MEMORYNOTENOUGH  15
#define SOUND_ERROR_EOF              16
#define SOUND_ERROR_BAD              17

//#define SOUND_ERROR_THREADFAIL       18

#define SOUND_ERROR_UNKNOWN          19


/**************************/
/* Define values for MPEG */
/**************************/
#define SCALEBLOCK     12
#define CALCBUFFERSIZE 512
#define MAXSUBBAND     32
#define MAXCHANNEL     2
#define MAXTABLE       2
#define SCALE          32768
#define MAXSCALE       (SCALE-1)
#define MINSCALE       (-SCALE)
#define RAWDATASIZE    (2*2*32*SSLIMIT)

#define LS 0
#define RS 1

#define SSLIMIT      18
#define SBLIMIT      32

#define WINDOWSIZE    4096

// Huffmancode
#define HTN 34

#define MODE_MONO   0
#define MODE_STEREO 1

/********************/
/* Type definitions */
/********************/
typedef float REAL;

typedef struct
{
  bool         generalflag;
  unsigned int part2_3_length;
  unsigned int big_values;
  unsigned int global_gain;
  unsigned int scalefac_compress;
  unsigned int window_switching_flag;
  unsigned int block_type;
  unsigned int mixed_block_flag;
  unsigned int table_select[3];
  unsigned int subblock_gain[3];
  unsigned int region0_count;
  unsigned int region1_count;
  unsigned int preflag;
  unsigned int scalefac_scale;
  unsigned int count1table_select;
}layer3grinfo;

typedef struct
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct
  {
    unsigned scfsi[4];
    layer3grinfo gr[2];
  }ch[2];
}layer3sideinfo;

typedef struct
{
  int l[23];            /* [cb] */
  int s[3][13];         /* [window][cb] */
}layer3scalefactor;     /* [ch] */

typedef struct
{
  int tablename;
  unsigned int xlen,ylen;
  unsigned int linbits;
  unsigned int treelen;
  const unsigned int (*val)[2];
}HUFFMANCODETABLE;


class Errorbase
{
public:
  Errorbase()          { __errorcode = SOUND_ERROR_OK; }
  virtual ~Errorbase() { }
  int geterrorcode()   { return __errorcode; }

protected:
  void seterrorcode(int code) { __errorcode = code; }

private:
  int __errorcode;
};


/*********************************/
/* Sound input interface classes */
/*********************************/
// Superclass for Inputbitstream // Yet, Temporary
class Soundinputstream : public Errorbase
{
public:
  virtual int  getbytedirect()                   =0;
  virtual bool _readbuffer(char *buffer,int size)=0;
  virtual bool eof()                             =0;
};

/**********************************/
/* Sound player interface classes */
/**********************************/
// Superclass for player
class Soundplayer : public Errorbase
{
public:
  virtual bool setsoundtype(int stereo,int samplesize,int speed)=0;
  virtual bool putblock(void *buffer,int size)                  =0;
};



/*********************************/
/* Data format converter classes */
/*********************************/
// Class for Mpeg layer3
class Mpegbitwindow
{
public:
  Mpegbitwindow(){bitindex=point=0;};

  void initialize()        {bitindex=point=0;};
  int  gettotalbit() const {return bitindex;};
  void putbyte(int c)      {buffer[point&(WINDOWSIZE-1)]=c;point++;};
  void wrap();
  void rewind(int bits)    {bitindex-=bits;};
  void forward(int bits)   {bitindex+=bits;};
  int  getbit();
  int  getbits9(int bits);
  int  getbits(int bits);

private:
  int  point;
  int  bitindex;
  char buffer[2*WINDOWSIZE];
};

inline void Mpegbitwindow::wrap()
{
  int p=bitindex>>3;
  point&=(WINDOWSIZE-1);

  if(p>=point)
  {
    for(register int i=4;i<point;i++)
      buffer[WINDOWSIZE+i]=buffer[i];
  }
  *((int *)(buffer+WINDOWSIZE))=*((int *)buffer);
}

inline int Mpegbitwindow::getbit()
{
//  register int r=(buffer[(bitindex>>3)&(WINDOWSIZE-1)]>>(7-(bitindex&7)))&1;
  register int r=(buffer[bitindex>>3]>>(7-(bitindex&7)))&1;
  bitindex++;
  return r;
};

inline int Mpegbitwindow::getbits9(int bits)
{
  register unsigned short a;
  //    int offset=(bitindex>>3)&(WINDOWSIZE-1);
  int offset=bitindex>>3;

  a=(((unsigned char)buffer[offset])<<8) | ((unsigned char)buffer[offset+1]);

  a<<=(bitindex&7);
  bitindex+=bits;
  return (int)((unsigned int)(a>>(16-bits)));
}



// Class for converting mpeg format to raw format
class Mpegtoraw : public Errorbase
{
  /*****************************/
  /* Constant tables for layer */
  /*****************************/
private:
  static const int bitrate[2][3][15],frequencies[2][3];
  static const REAL scalefactorstable[64];
  static const HUFFMANCODETABLE ht[HTN];
  static const REAL filter[512];
  static REAL hcos_64[16],hcos_32[8],hcos_16[4],hcos_8[2],hcos_4;

  /*************************/
  /* MPEG header variables */
  /*************************/
private:
  int layer,protection,bitrateindex,padding,extendedmode;
  enum _mpegversion  {mpeg1,mpeg2}                               version;
  enum _mode    {fullstereo,joint,dual,single}                   mode;
  enum _frequency {frequency44100,frequency48000,frequency32000} frequency;

  /*******************************************/
  /* Functions getting MPEG header variables */
  /*******************************************/
public:
  // General
  int  getversion()   const {return version;};
  int  getlayer()     const {return layer;};
  bool getcrccheck()  const {return (!protection);};
  // Stereo or not
  int  getmode()      const {return mode;};
  bool isstereo()     const {return (getmode()!=single);};
  // Frequency and bitrate
  int  getfrequency() const {return frequencies[version][frequency];};
  int  getbitrate()   const {return bitrate[version][layer-1][bitrateindex];};

  /***************************************/
  /* Interface for setting music quality */
  /***************************************/
private:
  bool forcetomonoflag;
  int  downfrequency;

public:
  void setforcetomono(bool flag);
  void setdownfrequency(int value);

  /******************************************/
  /* Functions getting other MPEG variables */
  /******************************************/
public:
  bool getforcetomono();
  int  getdownfrequency();
  int  getpcmperframe();

  /******************************/
  /* Frame management variables */
  /******************************/
private:
  int currentframe,totalframe;
  int decodeframe;
  int *frameoffsets;

  /******************************/
  /* Frame management functions */
  /******************************/
public:
  int  getcurrentframe() const {return currentframe;};
  int  gettotalframe()   const {return totalframe;};

  /***************************************/
  /* Variables made by MPEG-Audio header */
  /***************************************/
private:
  int tableindex,channelbitrate;
  int stereobound,subbandnumber,inputstereo,outputstereo;
  REAL scalefactor;
  int framesize;

  /*******************/
  /* Mpegtoraw class */
  /*******************/
public:
  Mpegtoraw(Soundinputstream *loader,Soundplayer *player);
  ~Mpegtoraw();
  void initialize();
  bool run(int frames);
  void clearbuffer();

  /*****************************/
  /* Loading MPEG-Audio stream */
  /*****************************/
private:
  Soundinputstream *loader;   // Interface
  union
  {
    unsigned char store[4];
    unsigned int  current;
  }u;
  char buffer[4096];
  int  bitindex;
  bool fillbuffer(int size){bitindex=0;return loader->_readbuffer(buffer,size);};
  void sync()  {bitindex=(bitindex+7)&0xFFFFFFF8;};
  bool issync(){return (bitindex&7) != 0;};
  int getbyte();
  int getbits(int bits);
  int getbits9(int bits);
  int getbits8();
  int getbit();

  /********************/
  /* Global variables */
  /********************/
private:
  int lastfrequency,laststereo;

  // for Layer3
  int layer3slots,layer3framestart,layer3part2start;
  REAL prevblck[2][2][SBLIMIT][SSLIMIT];
  int currentprevblock;
  layer3sideinfo sideinfo;
  layer3scalefactor scalefactors[2];

  Mpegbitwindow bitwindow;
  int wgetbit();
  int wgetbits9(int bits);
  int wgetbits(int bits);


  /*************************************/
  /* Decoding functions for each layer */
  /*************************************/
private:
  bool loadheader();

  //
  // Subbandsynthesis
  //
  REAL calcbufferL[2][CALCBUFFERSIZE],calcbufferR[2][CALCBUFFERSIZE];
  int  currentcalcbuffer,calcbufferoffset;

  void computebuffer(REAL *fraction,REAL buffer[2][CALCBUFFERSIZE]);
  void generatesingle();
  void generate();
  void subbandsynthesis(REAL *fractionL,REAL *fractionR);

  void computebuffer_2(REAL *fraction,REAL buffer[2][CALCBUFFERSIZE]);
  void generatesingle_2();
  void generate_2();
  void subbandsynthesis_2(REAL *fractionL,REAL *fractionR);

  // Extarctor
  void extractlayer1();    // MPEG-1
  void extractlayer2();
  void extractlayer3();
  void extractlayer3_2();  // MPEG-2


  // Functions for layer 3
  void layer3initialize();
  bool layer3getsideinfo();
  bool layer3getsideinfo_2();
  void layer3getscalefactors(int ch,int gr);
  void layer3getscalefactors_2(int ch);
  void layer3huffmandecode(int ch,int gr,int out[SBLIMIT][SSLIMIT]);
  REAL layer3twopow2(int scale,int preflag,int pretab_offset,int l);
  REAL layer3twopow2_1(int a,int b,int c);
  void layer3dequantizesample(int ch,int gr,int   in[SBLIMIT][SSLIMIT],
			                    REAL out[SBLIMIT][SSLIMIT]);
  void layer3fixtostereo(int gr,REAL  in[2][SBLIMIT][SSLIMIT]);
  void layer3reorderandantialias(int ch,int gr,REAL  in[SBLIMIT][SSLIMIT],
				               REAL out[SBLIMIT][SSLIMIT]);

  void layer3hybrid(int ch,int gr,REAL in[SBLIMIT][SSLIMIT],
		                  REAL out[SSLIMIT][SBLIMIT]);
  
  void huffmandecoder_1(const HUFFMANCODETABLE *h,int *x,int *y);
  void huffmandecoder_2(const HUFFMANCODETABLE *h,int *x,int *y,int *v,int *w);

public:
	//mbg 9/17/05
	//feed me a rawbuffer
	//i will not own this buffer!
  int getRawBufferSize() { return RAWDATASIZE; }
  void setRawBuffer(short *ptr) { rawdata = ptr; }


  /********************/
  /* Playing raw data */
  /********************/
private:
  Soundplayer *player;       // General playing interface
  int       rawdataoffset;
  //short int rawdata[RAWDATASIZE];
  short *rawdata;

  void clearrawdata()    {rawdataoffset=0;};
  void putraw(short int pcm) {
	  rawdata[rawdataoffset++]=pcm;
  };
  void flushrawdata();
};


#endif
