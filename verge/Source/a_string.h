/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef A_STRING
#define A_STRING

//std::string utility methods

#include <boost/algorithm/string.hpp>
using boost::algorithm::to_upper_copy;
using boost::algorithm::to_upper;
using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;
using boost::algorithm::equals;
using std::string;
//ex: std::string x = to_lower_copy(std::string("HI"));

inline int strcasecmp(std::string s1, std::string s2) {
	return strcasecmp(s1.c_str(),s2.c_str());
}

//a vc-style substring operation (very kind and lenient)
inline std::string vc_strsub(const std::string &str, int pos, int len) {
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
	
	return str.substr(pos,len);
}

inline std::string vc_strmid(const std::string &str, int pos, int len) { return vc_strsub(str,pos,len); }
inline std::string vc_strleft(const std::string &str, int len) { return vc_strsub(str,0,len); }
// Overkill (2007-08-25): Fixed a bug in right() where len of 0 returns str, instead of "".
inline std::string vc_strright(const std::string &str, int len) { return len ? vc_strsub(str,str.size()-len,len) : ""; }

// Overkill: 2005-12-28
// Thank you, Zip.
inline std::string strovr(std::string source, std::string rep, int offset)
// Pass: The offset in the source to overwrite from, the string to overwrite with, the source string
// Return: The string after overwrite
// Assmes: Offset is less than source length
{
	int length = source.length();
	int replen = rep.length();
	if (length < replen + offset) return vc_strleft(source,offset) + rep;
	return vc_strleft(source,offset) + rep + vc_strright(source,length - offset - replen);
}

// Overkill: 2005-12-28
// Helper function.
inline int strpos(std::string source, std::string sub, int start)
{
	int i;
	int count = 0;
	int lensub = sub.length();
	int lensource = source.length();
	for (i = start; i < lensource; i++)
	{
		if (!strcmp(sub.c_str(), vc_strmid(source, i, lensub).c_str())) return i;
	}
	return lensource;
}


#endif
