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
#include <string_view>
#include <memory>
#include <utility>

class StringRef {
public:
	static const StringRef& empty_string();

	StringRef()
	: ptr(empty_string().ptr) {}

	StringRef(const StringRef&) = default;
	StringRef(StringRef&&) = default;

	StringRef(const std::string& stdstr)
	: ptr(std::make_shared<std::string>(stdstr)) {}

	StringRef(std::string&& stdstr)
	: ptr(std::make_shared<std::string>(std::move(stdstr))) {}

	StringRef(const char* cstr)
	: ptr(std::make_shared<std::string>(cstr)) {}

	StringRef(const char* cstr, const char* cstrend)
	: ptr(std::make_shared<std::string>(cstr, cstrend)) {}

	StringRef(std::string_view view)
	: ptr(std::make_shared<std::string>(view.data(), view.length())) {}	

	FORCEINLINE const std::string* get() const { return ptr.get(); }
	FORCEINLINE const std::string& str() const { return *ptr; }
	FORCEINLINE std::string_view view() const { return std::string_view(ptr->c_str(), ptr->length()); }
	FORCEINLINE const char* c_str() const { return ptr->c_str(); }
	FORCEINLINE unsigned int length() const { return ptr->length(); }
	FORCEINLINE unsigned int size() const { return ptr->size(); }
	FORCEINLINE bool empty() const { return size() == 0; }

	StringRef& operator =(const StringRef&) = default;
	StringRef& operator =(StringRef&&) = default;
	FORCEINLINE const char& operator [](int index) const { return str()[index]; }

	//same as std::string, but saves a string allocation
	StringRef substr(int pos, int len) const {
		pos = std::min(pos, (int)length());
		len = std::max(len, 0);
		len = std::min(len, (int)length() - pos);
		return StringRef(c_str() + pos, c_str() + pos + len);
	}

	//peeks at the underlying string. use with caution, you will be changing it for all associated StringRefs
	FORCEINLINE std::string& dangerous_peek() { return *ptr; }

private:
	std::shared_ptr<std::string> ptr;
};

typedef const StringRef& CStringRef;

FORCEINLINE bool operator ==(const StringRef& lhs, const StringRef& rhs) { return lhs.str() == rhs.str(); }
FORCEINLINE bool operator !=(const StringRef& lhs, const StringRef& rhs) { return lhs.str() != rhs.str(); }
FORCEINLINE bool operator <(const StringRef& lhs, const StringRef& rhs) { return lhs.str() < rhs.str(); }
FORCEINLINE bool operator >(const StringRef& lhs, const StringRef& rhs) { return lhs.str() > rhs.str(); }
FORCEINLINE bool operator <=(const StringRef& lhs, const StringRef& rhs) { return lhs.str() <= rhs.str(); }
FORCEINLINE bool operator >=(const StringRef& lhs, const StringRef& rhs) { return lhs.str() >= rhs.str(); }

FORCEINLINE bool operator ==(std::string_view lhs, const StringRef& rhs) { return lhs == rhs.str(); }
FORCEINLINE bool operator !=(std::string_view lhs, const StringRef& rhs) { return lhs != rhs.str(); }
FORCEINLINE bool operator <(std::string_view lhs, const StringRef& rhs) { return lhs < rhs.str(); }
FORCEINLINE bool operator >(std::string_view lhs, const StringRef& rhs) { return lhs > rhs.str(); }
FORCEINLINE bool operator <=(std::string_view lhs, const StringRef& rhs) { return lhs <= rhs.str(); }
FORCEINLINE bool operator >=(std::string_view lhs, const StringRef& rhs) { return lhs >= rhs.str(); }

FORCEINLINE bool operator ==(const StringRef& lhs, std::string_view rhs) { return lhs.str() == rhs; }
FORCEINLINE bool operator !=(const StringRef& lhs, std::string_view rhs) { return lhs.str() != rhs; }
FORCEINLINE bool operator <(const StringRef& lhs, std::string_view rhs) { return lhs.str() < rhs; }
FORCEINLINE bool operator >(const StringRef& lhs, std::string_view rhs) { return lhs.str() > rhs; }
FORCEINLINE bool operator <=(const StringRef& lhs, std::string_view rhs) { return lhs.str() <= rhs; }
FORCEINLINE bool operator >=(const StringRef& lhs, std::string_view rhs) { return lhs.str() >= rhs; }

extern CStringRef empty_string;

std::string to_upper_copy(const std::string& s);
void to_upper(std::string& s);
std::string to_lower_copy(const std::string& s);
void to_lower(std::string& s);
void replace_all(std::string& s, char match, char replacement);

using std::string;

int strcasecmp(CStringRef s1, CStringRef s2);
int strcasecmp(std::string_view s1, std::string_view s2);
StringRef vc_strsub(CStringRef str, int pos, int len);
StringRef vc_strmid(CStringRef str, int pos, int len);
StringRef vc_strleft(CStringRef str, int len);
StringRef vc_strright(CStringRef str, int len);
StringRef strovr(CStringRef source, CStringRef rep, int offset);
bool isdelim(char c, CStringRef s);

unsigned int FastHash(bool tolower, char const * const s, unsigned int seed = 0);
unsigned int FastHash(char const * const s, unsigned int seed = 0);
unsigned int FastHash(const std::string& s, unsigned int seed = 0);


#endif
