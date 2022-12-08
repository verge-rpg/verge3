/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	a_common.cpp
 ****************************************************************/

#include "xerxes.h"

/***************************** data *****************************/

FILE *logfile, *conf;
bool log_on = false;
bool logconsole = false;

/***************************** code *****************************/

byte* ImageTo24bpp(image *img)
{
	int r, g, b;

	// Tripled, because we want a seperate byte for each R, G, or B channel.
	byte *dest = new byte[img->width * img->height * 3];
	byte *out = dest;

	for (int y=img->cy1; y<=img->cy2; y++)
	{
		for (int x=img->cx1; x<=img->cx2; x++)
		{
			GetColor(ReadPixel(x, y, img), r, g, b);
			//dest[i] = MakeColor(r,g,b);
			*dest++ = r;
			*dest++ = g;
			*dest++ = b;
		}
	}
	return out;
}


void log_Init(bool on)
{
#ifndef PROHIBIT_DISK_LOG
	remove(LOGFILE);
#endif
	log_on = on;
}


void log(const char *text, ...)
{
	va_list argptr;
	char msg[4096];

	va_start(argptr, text);
	vsprintf(msg, text, argptr);
	va_end(argptr);

#ifdef __EMSCRIPTEN__
    EM_ASM({
        console.log(UTF8ToString($0));
#ifdef VERGE_EMSCRIPTEN_DEBUG     
        console.log(stackTrace());
#endif
    }, msg);   

#else
	if (!log_on)
    {
        return;
    }

#ifndef PROHIBIT_DISK_LOG
	logfile = fopen(LOGFILE,"a");
#endif
	if (logfile)
	{
		fprintf(logfile,"%s\n",msg);
		fflush(logfile);
		fclose(logfile);
	}
	else
	{
		fprintf(stdout, "%s\n", msg);
		fflush(stdout);
	}

	if (logconsole)
    {
		printf("%s\n",msg);
    }
#endif    
}


char *va(char* format, ...)
{
	va_list argptr;
    static char string[4096];

    va_start(argptr, format);
    vsprintf(string, format, argptr);
    va_end(argptr);

	return string;
}

#ifdef __WIN32__
int strcasecmp(const char *s1, const char *s2)
{	
	while (true)
	{
		if (tolower(*s1) != tolower(*s2))
			return -1;
		if (!*s1)
			return 0;
		s1++;
		s2++;
	}
	return -1;
}
#endif

char mycharcmp(char c)
{
	char z = tolower(c);
	if (z=='\\') z = '/';
	return z;
}

int fncmp(const char *s1, const char *s2)
{
	while (true)
	{
		if (mycharcmp(*s1) != mycharcmp(*s2))
			return -1;
		if (!*s1)
			return 0;
		s1++;
		s2++;
	}
	return -1;
}

bool isletter(char c)
{
	if (tolower(c) < 'a' || tolower(c) > 'z') return false;
	return true;
}


void strclean(char *s)
{
	while (*s)
	{
		if (*s == 10 || *s == 13)
		{
			*s = 0;
			return;
		}
		s++;
	}
}


int sgn(int a)
{
	if (a<0)
		return -1;
	else if (a>0)
		return +1;
	return 0;
}


char *stripext(const char *s)
{
	static char strbuf[512];
	strcpy(strbuf, s);
	int i = strlen(strbuf);
	while (i>=0 && strbuf[i] != '.')
		i--;
	strbuf[i]=0;
	return strbuf;
}


bool ExtensionIs(const char *f, const char *e)
{
	const char *spt = f;
	while (*spt && *spt != '.')
		spt++;
	spt++;
	return (!strcasecmp(spt, e));
}

//see
//http://www.math.keio.ac.jp/~matumoto/cokus.c
//for comments and attributation

#define N              (624)                 // length of state vector
#define M              (397)                 // a period parameter
#define K              (0x9908B0DFU)         // a magic constant
#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

namespace namespace_a_rand
{

static unsigned int   state[N+1];     // state vector + 1 extra to not violate ANSI C
static unsigned int   *next;          // namespace_a_rand::next random value is computed from here
static int      left = -1;      // can *namespace_a_rand::next++ this many times before reloading
static unsigned int currSeed = timeGetTime();

}

void a_srand()
{
    namespace_a_rand::currSeed++;

    unsigned int x = (namespace_a_rand::currSeed | 1U) & 0xFFFFFFFFU, *s = namespace_a_rand::state;
    int j;

	for(namespace_a_rand::left=0, *s++=x, j=N; --j;
		*s++ = (x*=69069U) & 0xFFFFFFFFU) {}
}


unsigned int a_rand_reloadMT(void)
{
    unsigned int *p0=namespace_a_rand::state, *p2=namespace_a_rand::state+2, *pM=namespace_a_rand::state+M, s0, s1;
    int j;

    if(namespace_a_rand::left < -1)
        a_srand();

    namespace_a_rand::left=N-1, namespace_a_rand::next=namespace_a_rand::state+1;

    for(s0=namespace_a_rand::state[0], s1=namespace_a_rand::state[1], j=N-M+1; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);

    for(pM=namespace_a_rand::state, j=M; --j; s0=s1, s1=*p2++)
        *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);

    s1=namespace_a_rand::state[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    return(s1 ^ (s1 >> 18));
}


unsigned int a_rand()
{
    unsigned int y;

    if(--namespace_a_rand::left < 0)
        return(a_rand_reloadMT());

    y  = *namespace_a_rand::next++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    return(y ^ (y >> 18));
}

void arandseed(unsigned int seed)
{
	namespace_a_rand::currSeed = seed ? seed : timeGetTime();
	namespace_a_rand::left = -1;
}

int rnd(int lo, int hi)
{
	int range = hi - lo + 1;
	int i = a_rand() % range;
	return i + lo;
}
