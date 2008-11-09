#include "xerxes.h"
#include <stdio.h>

//the order of these matters: StringRefs cant be constructed until the pool is created
boost::object_pool<_StringRef> _StringRef_allocator;
StringRef StringRef::empty_string = "";

//#ifdef __WIN32__
//
//void* operator new(size_t n) 
//{
//	static int ctr = 0;
//	ctr++;
//	if(ctr%1000==0)
//		printf("%d\n",ctr);
//	return malloc(n);
//}
//
//void operator delete(void* mem) throw() {
//	free(mem);
//}
//
//#endif


int strcasecmp(std::string s1, std::string s2) {
	return strcasecmp(s1.c_str(),s2.c_str());
}

//a vc-style substring operation (very kind and lenient)
StringRef vc_strsub(const StringRef &str, int pos, int len) {
	int strlen = str.size();
	
	if(strlen==0) return str; //empty strings always return empty strings
	if(pos>=strlen) return str; //if you start past the end of the string, return the entire string. this is unusual, but there you have it

	//clipping
	if(pos<0) {
		len += pos;
		pos = 0;
	}

	if (pos+len>=strlen)
		len=strlen-pos+1;
	
	return str.str().substr(pos,len);
}

StringRef vc_strmid(const StringRef &str, int pos, int len) { return vc_strsub(str,pos,len); }
StringRef vc_strleft(const StringRef &str, int len) { return vc_strsub(str,0,len); }
// Overkill (2007-08-25): Fixed a bug in right() where len of 0 returns str, instead of "".
StringRef vc_strright(const StringRef &str, int len) { return len ? vc_strsub(str,str.size()-len,len) : ""; }

// Overkill: 2005-12-28
// Thank you, Zip.
StringRef strovr(const StringRef& source, const StringRef& rep, int offset)
// Pass: The offset in the source to overwrite from, the string to overwrite with, the source string
// Return: The string after overwrite
// Assmes: Offset is less than source length
{
	int length = source.length();
	int replen = rep.length();
	if (length < replen + offset) return vc_strleft(source,offset).str() + rep.str();
	return vc_strleft(source,offset).str() + rep.str() + vc_strright(source,length - offset - replen).str();
}

//VI.b. String Functions
//helper
bool isdelim(char c, const StringRef& s) {
	for (int i=0; i<s.length(); i++)
		if (c==s[i])
			return true;
	return false;
}


quad FastHash( char const * const s, quad seed )
{
	int result = seed;

	if ( s == NULL ) // should this assert(false) ?
	{
		return -1;
	}

	const char * pStr = s;

	// this hash collapses:
	// numerics and punctuation onto control characters (except as specified)
	// lower onto upper case letters.
	// ` == @
	// { == [
	// } == }
	// \ == |
	// ~ == ^
	// It's probably best not to use those characters with this hash :)

	while ( *pStr )
	{
		if ( *pStr == '*' )
		{
			result = result ^ FastHash( pStr+ 1 );
			break;
		}

		result = result * 31 + ( *pStr & ( char )( 0x5F ) );
		++pStr;
	}

	return result;
}

quad FastHash( const std::string& s, quad seed )
{
	return FastHash( s.c_str(), seed );
}
