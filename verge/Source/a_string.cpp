/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/******************************************************************
 * verge3: a_string.cpp                                           *
 * string class by aen!                                           *
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifndef __APPLE__

#define strlwr _strlwr
#define strupr _strupr

#endif

#define s8  char
#define u8  unsigned char
#define s16 short
#define u16 unsigned short
#define s32 int
#define u32 unsigned int

/******** aen's RAWMEM class ********/

class rangenum
{
	s32 lo,hi;
	s32 n;

public:
	rangenum(s32 l=0, s32 h=0)
	{
		set_limits(l,h);
		set(0);
	}

	void set_limits(s32 l, s32 h)
	{
		lo=l;
		hi=h;
	}
	void set(s32 x)
	{
		if (x<lo || x>hi)
			return;
		n=x;
	}
	void inc(s32 x)
	{
		set(get()+x);
	}
	s32 get() const { return n; }
	s32 get_lo() const { return lo; }
	s32 get_hi() const { return hi; }
};

#define HUNK 32
#define HUNK_COUNT(x) (((x)+HUNK-1)/HUNK)
#define RAWMEM_USAGE_STRING_LENGTH 256

extern void die_on_violation(u32 flag);

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
	rawptr(){}
	rawptr(rawmem* r, u32 pos=0) {
		point_to(r);
		set_pos(pos);
	}

	void touch(u32 count) {
		if (!raw) return;
		raw->touch(get_pos()+count);
	}

	void point_to(rawmem* r) {
		if (!r) return;
		raw=r;
		offset.set_limits(0,r->length()-1);
	}
	//rawmem* pointing_to() const { return raw; }

	u8 *get(u32 count) {
		if (!raw) return 0L;
		return raw->get(get_pos(),count);
	}

	void set_pos(u32 x) {
		offset.set(x);
	}
	s32 get_pos() const { return offset.get(); }
	void inc_pos(s32 x) { offset.inc(x); }

	void set_start() {
		set_pos(0);
	}
	void set_end() {
		if (!raw) return;
		set_pos(raw->length()-1);
	}
};

static u32 die_on_violate=1;
void die_on_violation(u32 flag) {
	die_on_violate=flag;
}

// ctor
rawmem::rawmem(s32 requested, const char *use) {
	zero_all();
	resize(requested,use);
}
// dtor
rawmem::~rawmem(){
	destroy();
}

u32 rawmem::touched() const {
	return m_touched;
}
void rawmem::touch(u32 rhand) {
	if (rhand>length())
		rhand=length();
	if (rhand>touched())
		m_touched = rhand;
}

void rawmem::untouch() {
	m_touched=0;
}

// used by ctors
void rawmem::zero_all() {
	m_data=0L;
	m_length=m_hunks=m_touched=0;
}
char* rawmem::get_use() const {
	static char temp[RAWMEM_USAGE_STRING_LENGTH +1];
	if (strlen((char *)m_use)<1)
		strcpy(temp, "*UNKNOWN*");
	else strcpy(temp, (char *)m_use);
	return temp;
}

void rawmem::set_use(const char *use) {
	if (!use || strlen((char *)use)<1) return;
	else strncpy((char *)m_use, (char *)use, 256);
}

void rawmem::become_string(const char* text) {
// resize to accomodate a C-string (zero-terminated), then copy the string in and touch all
	resize(strlen((char *)text)+1);
	memcpy(get(0,length()), (char *)text, length());
	touch(length());
}

u32 rawmem::length() const { return m_length; }
u32 rawmem::hunks() const { return m_hunks; }
// total amount of memory this rawmem object currently contains
u32 rawmem::bytes_allocated() const { return hunks() * HUNK; }

// free all memory and zero all members; also effectively unmaps a device, if currently mapped
void rawmem::destroy() {
	if (m_data) {
		delete [] m_data;
		m_data = 0L;
	}
	m_length=m_hunks=m_touched=0;
}

// aen <21dec99> Allocation management core.
void rawmem::resize(s32 requested, const char *use) {
	set_use(use);
	//if (requested<1)
	//	requested=1;
	// do we need more blocks?
	if (requested && HUNK_COUNT(requested) > (signed) hunks()) {
		m_hunks = HUNK_COUNT(requested);			// reset hunk count
		u8* buf = new u8 [bytes_allocated()];	// allocate a new buffer w/ more needed hunks
		// preservation requested? if so, copy in touched bytes
		if (touched()) {
			memcpy(buf, m_data, touched());
		}
		// destroy current buffer, and reassign pointer to new buffer
		delete [] m_data;
		m_data = buf;
	// forgot how this works..
	} else if (requested < (signed) length()) {
		touch(requested);
	}

	// zero-out all unused bytes during this resize
	if (touched() < bytes_allocated()) {
		memset(m_data+touched(), 0, bytes_allocated()-touched());
	}
	// reset length
	m_length=requested;
}

// reallocate if hunks required to hold length() bytes is less than
// the currently allocated number of blocks
void rawmem::consolidate() {
	// do we really need this many hunks?
	if (HUNK_COUNT(length()) < hunks()) {
		m_hunks = HUNK_COUNT(length());				// reset hunk count
		u8* buf = new u8 [bytes_allocated()];	// allocate a new buffer w/ less hunks
		// any memory in-use? if so, copy it in
		if (touched()) {
			memcpy(buf, m_data, touched());
		}
		// destroy current buffer, and reassign pointer to new buffer
		delete [] m_data;
		m_data = buf;
	}
}

void rawmem::report() {
	printf("$ MEM REPORT <%s> # ", get_use());
	printf("<L%d:%d%%> <H%d> <T%d:%d%%>\n",
		length(),
		(length()*100)/bytes_allocated(),
		hunks(),
		touched(),
		(touched()*100)/length()
	);
}

// aen <9nov99> : Yeah baby! The core of all protection.
u8* rawmem::get(s32 n, u32 z) const {
	// all protection for all requests originates here
	if (n<0) {
		if (die_on_violate) {
			n=0-n;
			printf("rawmem$get: %s: prefix violation (offset=%d,count=%d)\n", get_use(), n, z);
			exit(-1);
		}
		n=0;
	// we want things to be okay if length (z) is zero; if we don't check for length,
	// it will trigger the violation code, even though a violation hasn't occurred.
	} else if (z && n+z-1 >= length()) {
		if (die_on_violate) {
			n=n+z-1 -length() +1;
			printf("rawmem$get: %s: suffix violation (offset=%d,count=%d)\n", get_use(), n, z);
			exit(-1);
		}
		n=0;
	}
	return &m_data[n];
}

u32* rawmem::get_u32(s32 n) const { return (u32*)get(n, sizeof(u32)); }
u16* rawmem::get_u16(s32 n) const { return (u16*)get(n, sizeof(u16)); }
u8* rawmem::get_u8(s32 n) const { return get(n, sizeof(u8)); }
u8& rawmem::operator [](s32 n) const { return *get(n, sizeof(u8)); }

void rawmem_fill_u8(rawptr& dest, u32 color, s32 count) {
	if (count<0) return;
	u8* d=dest.get(count);
	if (d)
		memset(d,color,count);
	dest.touch(count);
}

void rawmem_xfer(rawptr& dest, rawptr& source, s32 count) {
	if (count<0) return;
	u8 *s=source.get(count);
	u8 *d=dest.get(count);
	if (s && d)
		memcpy(d,s,count);
	source.touch(count);
	dest.touch(count);
}

#undef HUNK

/*********** aen's linked-list class ***********/

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

void linked_list::insert_before_current(linked_node* pn)
{
	pn->set_prev(current()->prev());
	pn->set_next(current());

	current()->prev()->set_next(pn);
	current()->set_prev(pn);

	++nn;
}

void linked_list::insert_after_current(linked_node* pn)
{
	go_next();
	insert_before_current(pn);
	go_prev();
}

// unlink take a node out of the linked list, but does not dispose of the memory
int linked_list::unlink(linked_node* pn)
{
	if (!head()) return 0;

	// if they want to unlink the first node
	if (head() == pn)
	{
		head()->prev()->set_next(head()->next());	// set the first's last's next to the first's next
		head()->next()->set_prev(head()->prev());	// set the first next's last to the first last

		// if there is only one node
		if (head()->next() == head())
		{
			// clear the list
			fn=0;
			cn=0;
		}
		else
		{
			// else point the first pointer to the next node
			fn=pn->next();
		}
		// decrement the number of nodes in the list
		--nn;

		return 1;
	}
	else
	{
		// find the node in the list
		go_head();
		go_next();
		while (current() != pn && current() != head())
		{
			go_next();
		}
		// is it in the list at all?
		if (current() != head())
		{
			// yes unlink the pointers
			current()->prev()->set_next(current()->next());
			current()->next()->set_prev(current()->prev());

			// decrement the number of nodes
			--nn;

			return 1;
		}
	}

	return 0;
}

// this function clears all the nodes in a linked list and dispose of the
// memory for each one by calling is't destructor
linked_list::~linked_list()
{
	if (head())
	{
		// set the last nodes next to NULL
		// so we can go until we hit NULL
		head()->prev()->set_next(0);
	}

	// repeat until we get to that node
	while (fn != 0)
	{
		go_head();
		go_next();
		//cn=head()->next();
		delete fn;	// delete the old node
		fn=current();
	}
	// clear the list
	cn=0;
	// set the number of nodes to 0
	nn=0;
}

// this function returns the node number a node is in a linked list
// it start at the node and goes backwards until it reaches the first
// node
int linked_list::node_number(linked_node* pn)
{
	int x=1;
	while (pn != head())
	{
		x++;
		pn=pn->prev();
	}
	return x;
}

// this function returns a pointer to the xth node
linked_node* linked_list::get_node(int x)
{
	// start at the first node
	go_head();

	if (x>0 && nn) {
		x--;
		x%=nn;
		x++;
		// go forward X-1 nodes
		while (x-- > 1)
		{
			go_next();
		}
	}

	return current();
}

// this function adds a node to the end of a linked_list
void linked_list::insert_tail(linked_node* pn)
{
	// if there are no nodes, then this one becomes the first
	if (0 == head())
	{
		fn=pn;
		// and it poits to itself for first and last
		head()->set_next(head());
		head()->set_prev(head());

		++nn;
	}
	else
	{
		go_head();
		insert_before_current(pn);
	}
}

// to add a node at the fron of the list, just add it at the end and set
// the first pointer to it
void linked_list::insert_head(linked_node* pn)
{
	insert_tail(pn);
	fn=pn;
}

// insert adds a node in the list according to is sort value
void linked_list::insert(linked_node* pn)
{
	// if there are nodes, or it belongs at the beginin call add front
	if ((0==head()) || (pn->compare(head()) > 0))
	{
		insert_head(pn);
	}
	// else if it goes at the ned call add_end
	else if (pn->compare(head()->prev()) <= 0)
	{
		insert_tail(pn);
	}
	// otherwise we have to find the right spot for it.
	else
	{
		// iter starts at head
		go_head();
		while (current() != head()->prev())
		{
			go_next();
			// repeat until we find a value greater than the one we are inserting
			if (pn->compare(current()) > 0)
			{
				insert_before_current(pn);

				break;
			}
		}
	}
}

linked_list::linked_list(linked_node* first)
{
	fn=first;
	cn=first;
	nn=0;

	if (first)
	{
		linked_node* prev;

		go_head();
		do
		{
			++nn;
			prev=current();
			go_next();
		} while (current() && current() != head());

		if (0 == current())
		{
			head()->set_prev(prev);
			prev->set_next(head());
		}
		go_head();
	}
}

/*********** aen's rawstr class ************/

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

int string::compare(void* c)
{
        return (*((string *)c) > *this);
}

// !!! NO MEMORY PROTECTION HERE, BE CAREFUL !!!
char* string::c_str() const {
	return (char*)m_data.get(0,m_data.length());
}

u32 string::length() const {
	return m_data.length() > 0 ? m_data.length() -1 : 0;
}

// create & destroy
void string::destroy() {
	m_data.destroy();
}
// create via C-string
void string::assign(const char* text) {
	// exit gracefully, if no string
	if (!text) {
		return;
	}
	// free mem & resize
	destroy();
	// copy the string in
	m_data.become_string(text);
}
// create via single char
void string::assign(char ch) {
	char text[2] = {ch,'\0'};

	assign(text);
}

// def ctor
string::string(const char *text)
: m_data(0,"string") {
	assign(text ? text : "");
}
// single character ctor
string::string(char ch)
: m_data(0,"string") {
	assign(ch);
}
// copy ctor
string::string(const string& s)
: m_data(0,"string") {
	*this = s;
}
// dtor
string::~string() {
	destroy();
}

// assignment op
string& string::operator=(const string& s) {
	if (&s != this) {
		assign((char *)s.m_data.get(0,s.m_data.length()));
	}
	return *this;
}
// concatenation
string& string::operator+=(const string& s) {
	int z = length();

	m_data.resize(z + s.length() +1);

	/*
	extern void Log(char* message);
	char shit[1024];
	sprintf(shit, "+= B4 %s", (char *)m_data.get(0,1));
	Log(shit);
	*/

	//V_memcpy(m_data.m_data+z, s.m_data, s.length());
	//m_data.m_data[z]='\0';
	rawptr source((rawmem *)&s.m_data), dest(&m_data,z);
	rawmem_xfer(dest, source, s.length());
	//(*this)[z+s.length()]='\0';
	m_data[z+s.length()]='\0';
	/*
	sprintf(shit, "+= A %s", (char *)m_data.get(0,1));
	Log(shit);
	*/

	return *this;
}
string& string::operator+(const string& s) {
	return *this += s;
}

// indexed char retrieval
char string::operator[](s32 n) const {
	return (n<0 || n>=(signed)length()) ? (char)0 : c_str()[n];
}

// transformers
string string::upper() {
	string s = *this;
	strupr((char *)s.m_data.get(0,s.m_data.length()));
	return s;
}
string string::lower() {
	string s = *this;
	strlwr((char *)s.m_data.get(0,s.m_data.length()));
	return s;
}

// substring extraction; auto-clipping due to operator[]
string string::operator()(s32 pos, s32 len) { //const {
	string s;

	// bogus length
	if (len<1 || pos>=(signed)length()) {
		return s;
	}

	// clipping
	if (pos<0) {
		len+=pos;
		pos=0;
	}
	if (pos+len>=(signed)length())
		len=length()-pos+1;

	// blap!
	s.m_data.resize(len +1);
	rawptr source(&m_data,pos), dest(&s.m_data,0);
	rawmem_xfer(dest, source, len);

	s.m_data[len]='\0';

	/*
	// slow method
	int n = 0;
	while (n < len) {
		s += operator[](pos + n);
		n++;
	}
	*/
	return s;
}
// whee!
string string::left(s32 len) { return operator()(0, len); }
string string::right(s32 len) { return operator()(length()-len, len); }
string string::mid(s32 pos, s32 len) { return operator()(pos, len); }

// locators; return -1 on failure, otherwise the located index
u32 string::firstindex(char c) const {
	/*
	char *found = strchr(c_str(), c);
	return found ? found - c_str() : -1;
	*/
	return 0;
}
u32 string::lastindex(char c) const {
	/*
	char *found = strrchr(c_str(), c);
	return found ? found - c_str() : -1;
	*/
	return 0;
}

// equality ops
bool string::operator==(const string& s) const { return strcmp(c_str(), s.c_str()) == 0; }
bool string::operator< (const string& s) const { return strcmp(c_str(), s.c_str()) <  0; }
bool string::operator<=(const string& s) const { return strcmp(c_str(), s.c_str()) <= 0; }
bool string::operator> (const string& s) const { return strcmp(c_str(), s.c_str()) >  0; }
bool string::operator>=(const string& s) const { return strcmp(c_str(), s.c_str()) >= 0; }

void string::report() {	m_data.report(); }
void string::consolidate() { m_data.consolidate(); }

void string::become_raw(rawmem& rm) {
	u32 z=strlen((char *)rm.get(0,rm.length()));
	m_data.resize(z+1);

	rawptr source(&rm), dest(&m_data);
	rawmem_xfer(dest, source, z);

	m_data[z]='\0';
}

/*******************************************/
