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

#define s8  char
#define u8  unsigned char
#define s16 short
#define u16 unsigned short
#define s32 int
#define u32 unsigned int

#define HUNK 32
#define HUNK_COUNT(x) (((x)+HUNK-1)/HUNK)
#define RAWMEM_USAGE_STRING_LENGTH 256

class rangenum
{
	s32 lo,hi;
	s32 n;

public:
	rangenum(s32 l=0, s32 h=0);
	void set_limits(s32 l, s32 h);
	void set(s32 x);
	void inc(s32 x);
	s32 get();
	s32 get_lo();
	s32 get_hi();
};

class rawptr;
class rawmem
{
	// what are we using this memory for?
	char m_use[RAWMEM_USAGE_STRING_LENGTH +1];

	u8* m_data;			// the raw buffer
	u32 m_length;		// user size request for buffer; mostly for suffix corruption detection
	u32 m_hunks;		// hunks currently allocated
	u32 m_touched;		// byte total of memory in-use

	// used by ctors
	void zero_all();
	// total amount of memory this rawmem object currently contains
	u32 bytes_allocated() const;

public:
	rawmem(s32 requested=0, const char *use=0L);
	~rawmem();

	u32 hunks() const;
	u32 touched() const;
	void touch(u32 rhand);
	void untouch();
	u32 length() const;

	void destroy();
	void resize(s32 requested, const char *use=0L);
	void become_string(const char* text);

	// reallocate if hunks required to hold length() bytes is less than
	// the currently allocated number of blocks
	void consolidate();
	void report();

	u8* get(s32 n, u32 z) const;
	u32* get_u32(s32 n) const;
	u16* get_u16(s32 n) const;
	u8* get_u8(s32 n) const;
	u8& operator[](s32 n) const;

	void set_use(const char *use);
	char* get_use() const;

	friend void rawmem_fill_u8(rawptr& dest, u32, s32);
	friend void rawmem_xfer(rawptr& dest, rawptr& source, s32);
};

class rawptr {
	rawmem *raw;
	rangenum offset;
public:
	rawptr();
	rawptr(rawmem* r, u32 pos=0);
	void touch(u32 count);
	void point_to(rawmem* r);
	u8 *get(u32 count);
	void set_pos(u32 x);
	s32 get_pos();
	void inc_pos(s32 x);
	void set_start();
	void set_end();
};


class linked_node
{
	linked_node* nex;
	linked_node* pre;

public:
	linked_node* next() { return nex; }
	linked_node* prev() { return pre; }
	void set_next(linked_node* pn) { nex=pn; }
	void set_prev(linked_node* pn) { pre=pn; }

	virtual int compare(void* c) { c=c; return 0; }

	virtual ~linked_node() {}
	linked_node() { nex=pre=0; }
};

class linked_list
{
	linked_node* fn;
	linked_node* cn;
	int nn;

public:
	~linked_list();
	linked_list(linked_node *first=0);

	void insert_head(linked_node* pn);
	void insert_tail(linked_node* pn);
	void insert_before_current(linked_node* pn);
	void insert_after_current(linked_node* pn);
	void insert(linked_node* pn);

	linked_node* current() const { return cn; }
	linked_node* head() const { return fn; }
	linked_node* tail() const { return head()->prev(); }

	linked_node* get_node(int x);

	void set_current(linked_node* pn) { cn=pn; }

	void go_head() { cn=head(); }
	void go_tail() { cn=head()->prev(); }
	void go_next() { cn=current()->next(); }
	void go_prev() { cn=current()->prev(); }

	int number_nodes() const { return nn; }
	int node_number(linked_node* pn);

	int unlink(linked_node* pn);
};

/*********** aen's string class ************/

class string
	: public linked_node
{
	rawmem m_data;

public:
	int compare(void* n1);

	// !!! NO MEMORY PROTECTION HERE, BE CAREFUL !!!
	char* c_str() const;

	u32 length() const;

	// create & destroy
	void destroy();
	// create via C-string
	void assign(const char* text);
	// create via single char
	void assign(char ch);

	// def ctor
	string(const char *text = 0);
	// single character ctor
	string(char ch);
	// copy ctor
	string(const string& s);
	// dtor
	~string();

    // assignment op
	string& operator =(const string& s);
	// concatenation
	string& operator+=(const string& s);
	string& operator+ (const string& s);

	u32 touched() const { return m_data.touched(); }

	// indexed char retrieval
	char operator[](s32 n) const;

	// transformers
	string upper();
	string lower();

	// substring extraction; auto-clipping due to operator[]
	string operator()(s32 pos, s32 len); // const;
	// whee!
	string left (s32 len);
	string right(s32 len);
	string mid  (s32 pos, s32 len);

	// locators; return -1 on failure, otherwise the located index
	u32 firstindex(char c) const;
	u32 lastindex (char c) const;

	// equality ops
	bool operator ==(const string& s) const;
	bool operator < (const string& s) const;
	bool operator <=(const string& s) const;
	bool operator > (const string& s) const;
	bool operator >=(const string& s) const;

	void report();
	void consolidate();

	void become_raw(rawmem& rm);
};

#undef s8
#undef u8
#undef s16
#undef u16
#undef s32
#undef u32

#endif
