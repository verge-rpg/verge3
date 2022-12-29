#include "xerxes.h"
#include <stdio.h>
#include <string.h>

CStringRef empty_string = StringRef::empty_string();

const StringRef& StringRef::empty_string() {
	static StringRef empty_string = "";
	return empty_string;
}

#ifdef __WIN32__

void* operator new(size_t n) 
{
	static int ctr = 0;
	ctr++;
	//if(ctr%1000==0)
		//printf("news: %d x1000\n",ctr);
	return malloc(n);
}

void operator delete(void* mem) throw() {
	free(mem);
}

#endif

std::string to_upper_copy(const std::string& s) {
	auto result = s;
	to_upper(result);
	return result;
}

void to_upper(std::string& s) {
	for (std::size_t i = 0; i != s.size(); ++i) {
		s[i] = toupper(s[i]);
	}
}

std::string to_lower_copy(const std::string& s) {
	auto result = s;
	to_upper(result);
	return result;
}

void to_lower(std::string& s) {
	for (std::size_t i = 0; i != s.size(); ++i) {
		s[i] = tolower(s[i]);
	}
}

void replace_all(std::string& s, char match, char replacement) {
	for (std::size_t i = 0; i != s.size(); ++i) {
		if (s[i] == match) {
			s[i] = replacement;
		}
	}
}

int strcasecmp(CStringRef s1, CStringRef s2) {
	return strcasecmp(s1.c_str(), s2.c_str());
}

int strcasecmp(std::string_view s1, std::string_view s2) {
	return strcasecmp(s1.data(), s2.data());
}

//a vc-style substring operation (very kind and lenient)
StringRef vc_strsub(CStringRef str, int pos, int len)
{
	int strlen = str.size();
	
	if (strlen == 0) return str; // empty strings always return empty strings
	if (pos >= strlen) return str; // if you start past the end of the string, return the entire string. this is unusual, but there you have it

	if(pos < 0)
	{
		len += pos;
		pos = 0;
	}

	if (pos + len >= strlen) len = strlen - pos + 1;
	if (len <= 0) return empty_string;
	
	return str.substr(pos, len);
}

StringRef vc_strmid(CStringRef str, int pos, int len) { return vc_strsub(str,pos,len); }
StringRef vc_strleft(CStringRef str, int len) { return vc_strsub(str,0,len); }
// Overkill (2007-08-25): Fixed a bug in right() where len of 0 returns str, instead of "".
StringRef vc_strright(CStringRef str, int len) { return len > 0 ? vc_strsub(str,str.size()-len,len) : empty_string; }

// Overkill: 2005-12-28
// Thank you, Zip.
StringRef strovr(CStringRef source, const StringRef& rep, int offset)
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
bool isdelim(char c, CStringRef s) {
	for (int i=0; i<s.length(); i++)
		if (c==s[i])
			return true;
	return false;
}

unsigned int FastHash(bool _tolower, char const * const s, unsigned int seed) {
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

		if(_tolower)
			result = result * 31 + ( tolower(*pStr) & ( char )( 0x5F ) );
		else
			result = result * 31 + ( *pStr & ( char )( 0x5F ) );
		++pStr;
	}

	return result;
}

unsigned int FastHash( char const * const s, unsigned int seed )
{
	return FastHash(false,s,seed);
}

unsigned int FastHash( const std::string& s, unsigned int seed )
{
	return FastHash( s.c_str(), seed );
}
