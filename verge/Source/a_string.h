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
		int zzz=9;
	}

	_StringRef(const char* cstr)
		: str(cstr)
		, mRefcount(0)
	{}

	const char* c_str() { return str.c_str(); }

	std::string str;

	int mRefcount;
};

extern boost::object_pool<_StringRef> _StringRef_allocator;


namespace boost
{
	inline void intrusive_ptr_add_ref(_StringRef * ptr) { 
		++ptr->mRefcount; 
	}

	inline void intrusive_ptr_release(_StringRef * ptr) { 
		if(--ptr->mRefcount == 0)
			_StringRef_allocator.destroy(ptr);
	}
};

typedef boost::intrusive_ptr<_StringRef> X_StringRef;
class StringRef : public X_StringRef {
public:
	
	static StringRef empty_string;

	StringRef()
		: X_StringRef(empty_string.get())
	{
		//just adds a reference to the empty string
	}

	StringRef(const std::string &stdstr)
		: X_StringRef(_StringRef_allocator.construct(stdstr))
	{}

	StringRef(const char* cstr)
		: X_StringRef(_StringRef_allocator.construct(cstr))
	{}

	std::string const & str() const { return get()->str; }
	operator std::string const &() const { return str(); }
	const char* c_str() const { return str().c_str(); }
	unsigned int length() const { return str().length(); }
	unsigned int size() const { return str().size(); }
	const char& operator[](int index) const { return str()[index]; }
	bool operator==(const StringRef& rhs) const { return str() == rhs.str(); }
	bool operator==(const char* rhs) const { return str() == rhs; }
};


inline StringRef empty_string() { return StringRef::empty_string; }

#include <boost/algorithm/string.hpp>
using boost::algorithm::to_upper_copy;
using boost::algorithm::to_upper;
using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;
using boost::algorithm::equals;
using std::string;
//ex: std::string x = to_lower_copy(std::string("HI"));

int strcasecmp(std::string s1, std::string s2);
StringRef vc_strsub(const StringRef &str, int pos, int len);
StringRef vc_strmid(const StringRef &str, int pos, int len);
StringRef vc_strleft(const StringRef &str, int len);
StringRef vc_strright(const StringRef &str, int len);
StringRef strovr(const StringRef& source, const StringRef& rep, int offset);
bool isdelim(char c, const StringRef& s);

quad FastHash( char const * const s, quad seed = 0);
quad FastHash( const std::string& s, quad seed = 0);


#endif
