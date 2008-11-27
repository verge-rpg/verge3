/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.

//MBG TODO - this is an old file. it sucks. do something better.


#ifndef _HANDLE_H_
#define _HANDLE_H_

#define HANDLE_TYPE_MOVIE (0)
#define HANDLE_TYPE_IMAGE (1)
#define HANDLE_TYPE_DICT  (2)
#define HANDLE_TYPE_AUDCHN  (3)
#define HANDLE_TYPE_MAX (4)

#include <vector>
#include <stack>

class Handle
{
public:
	template<class T>
	class HandleSet {
	public:
		std::stack<int> handles;
		std::vector<T*> handleVals;
		int handleCount;
		HandleSet() {
			handleCount = 0;
		}
		void forceAlloc(int count) {
			handleCount = count;
			for(int i=0;i<count;i++)
				handleVals.push_back(0);
		}
		int alloc(T* ptr) {
			//expand if necessary
			if(handles.empty())
				for(int i=0;i<16;i++,handleCount++) {
					handles.push(handleCount);
					handleVals.push_back(0);
				}

			int handle = handles.top();
			handles.pop();
			handleVals[handle] = ptr;
			return handle;
		}
		void free(int handle) {
			//can we do vector reserve() to shrink?
			handleVals[handle] = 0;
			handles.push(handle);
		}

		T* getPointer(int handle) { return handleVals[handle]; }
		void setPointer(int handle, T* ptr) { handleVals[handle] = ptr; }
		int getHandleCount() { return handleCount; }
		bool isValid(int handle) { return handle < getHandleCount() && handle >= 0; }

		T* operator[](int handle) { return getPointer(handle); }


		class iterator {
		public:
			iterator(HandleSet<T> *_set)
				: set(_set)
				, index(0)
			{
			}
			iterator(HandleSet<T> *_set, int _index) 
				: set(_set)
				, index(_index)
			{
			}

		
			T* operator*() { return set->handleVals[index]; }

			bool operator==(const iterator& other) {
				return set == other.set && index == other.index;
			}

			bool operator!=(const iterator& other) {
				return !(*this == other);
			}

			iterator operator++() {
				if(index != set->getHandleCount()) {
					index++;
					advance();
				}
				return *this;
			}

			iterator operator++(int) {
				iterator temp = *this;
				if(index != set->getHandleCount()) {
					index++;
					advance();
				}
				return temp;
			}

			void advance() {
				while(!set->getPointer(index) && index != set->getHandleCount())
					index++;
			}

		private:
			HandleSet<T> *set;
			int index;
		};

		iterator begin() { return iterator(this); }
		iterator end() { return iterator(this,getHandleCount()); }
	};

	typedef HandleSet<void> VoidHandleSet;
	static std::vector<VoidHandleSet> handleTypes;
	


public:

	static void init() {
		for(int i=0;i<=HANDLE_TYPE_MAX;i++)
			handleTypes.push_back(VoidHandleSet());
	}

	static int alloc(int type, void *ptr)
	{
		return handleTypes[type].alloc(ptr);
	}

	static void free(int type, int handle) {
		handleTypes[type].free(handle);
	}

	static void *getPointer(int type, int handle) { return handleTypes[type].handleVals[handle]; }
	static void setPointer(int type, int handle, void *ptr) { handleTypes[type].handleVals[handle] = ptr; }
	static int getHandleCount(int type) { return handleTypes[type].handleCount; }
	static bool isValid(int type, int handle) { return handle < getHandleCount(type) && handle >= 0; }

	//note:
	//do this before any allocations!
	//this will reserve handles 0,1,etc...
	static void forceAlloc(int type, int count) { handleTypes[type].forceAlloc(count); }

	class iterator : public VoidHandleSet::iterator {};

	static iterator begin(int type) { return *reinterpret_cast<iterator*>(&handleTypes[type].begin()); }
	static iterator end(int type) { return *reinterpret_cast<iterator*>(&handleTypes[type].end()); }
};


#endif
