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

#include <string>
#include <boost/intrusive_ptr.hpp>
#include <boost/pool/object_pool.hpp>

//std::string utility methods

class _StringRef {
public:
	_StringRef()
		: mRefcount(0)
	{}

	_StringRef(const std::string &stdstr)
		: str(stdstr)
		, mRefcount(0)
	{
	}

	_StringRef(const char* cstr)
		: str(cstr)
		, mRefcount(0)
	{}

	_StringRef(const char* cstr, const char* cstrend)
		: str(cstr,cstrend)
		, mRefcount(0)
	{}

	const char* c_str() { return str.c_str(); }

	std::string str;

	int mRefcount;
};


typedef boost::intrusive_ptr<_StringRef> X_StringRef;
class StringRef : public X_StringRef {
public:
	
	static const StringRef& empty_string();
	static boost::object_pool<_StringRef> & get_StringRef_allocator();

	StringRef()
		: X_StringRef(empty_string().get())
	{
		//just adds a reference to the empty string
	}

	StringRef(const std::string &stdstr)
		: X_StringRef(get_StringRef_allocator().construct(stdstr))
	{}

	StringRef(const char* cstr)
		: X_StringRef(get_StringRef_allocator().construct(cstr))
	{}

	StringRef(const char* cstr, const char* cstrend)
		: X_StringRef(get_StringRef_allocator().construct(cstr,cstrend))
	{}

	std::string const & str() const { return get()->str; }
	//operator std::string const &() const { return str(); }
	const char* c_str() const { return str().c_str(); }
	unsigned int length() const { return str().length(); }
	unsigned int size() const { return str().size(); }
	const char& operator[](int index) const { return str()[index]; }
	bool operator==(const StringRef& rhs) const { return str() == rhs.str(); }
	bool operator==(const char* rhs) const { return str() == rhs; }
	bool empty() const { return size()==0; }

	bool operator < (StringRef const & rhs) const {
		return str() < rhs.str();
	}

	//same as std::string, but saves a string allocation
	StringRef substr(int pos, int len) const {
		pos = std::min(pos,(int)length());
		len = std::max(len,0);
		len = std::min(len,(int)length()-pos);
		return StringRef(c_str()+pos,c_str()+pos+len);
	}

	//peeks at the underlying string. use with caution, you will be changing it for all associated StringRefs
	std::string & dangerous_peek() { return get()->str; }
};


namespace boost
{
	inline void intrusive_ptr_add_ref(_StringRef * ptr) { 
		++ptr->mRefcount; 
	}

	inline void intrusive_ptr_release(_StringRef * ptr) { 
		if(--ptr->mRefcount == 0)
			StringRef::get_StringRef_allocator().destroy(ptr);
	}
};


typedef const StringRef& CStringRef;

extern CStringRef empty_string;

#include <boost/algorithm/string.hpp>
using boost::algorithm::to_upper_copy;
using boost::algorithm::to_upper;
using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;
using boost::algorithm::equals;
using std::string;
//ex: std::string x = to_lower_copy(std::string("HI"));

int strcasecmp(CStringRef s1, CStringRef s2);
StringRef vc_strsub(CStringRef str, int pos, int len);
StringRef vc_strmid(CStringRef str, int pos, int len);
StringRef vc_strleft(CStringRef str, int len);
StringRef vc_strright(CStringRef str, int len);
StringRef strovr(CStringRef source, CStringRef rep, int offset);
bool isdelim(char c, CStringRef s);

unsigned int FastHash(bool tolower, char const * const s, unsigned int seed = 0);
unsigned int FastHash( char const * const s, unsigned int seed = 0);
unsigned int FastHash( const std::string& s, unsigned int seed = 0);


#endif
