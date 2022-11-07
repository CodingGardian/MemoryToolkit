#ifndef RAM_H
#define RAM_H

#define KILOBYTE 1000
#define MEGABYTE 1000000

#include <map>
#include <string>
#include <vector>
#include <stack>

#include "memoryerr.h"

#define INSUFFICIENT_SPACE 1
#define ALLOCATION_ERROR 2

#define NO_INTERFACE nullptr

// TODO: make IMemoryBuffer as idiot-proof as possible 
// (no offense to any other programmers planning to use this libray, I am an idiot sometimes and need this too)

// TODO: Change AddEntry function to return bit array of flags (int that is & with specific value)
// Maybe make a struct to put in function paramaters for AddEntry?

// TODO: buffers that can be used with template allocators

// TODO: Core buffers are singletons that should be allocated a the start of the memory buffer class block. They cannot be moved and are deleted ONLY at program shutdown

#define DECLARE_CORE_MEMORY_BUFFER(ClassName, CreateFn, Size) \
				_CORE_MEMORY_BUFFER ClassName##CORE_HEADER = new _CORE_MEMORY_BUFFER(ClassName, CreateFn, Size);

// Blocks to add:
//    Template allocator
//    Fast stack and queue
//    Reference counter cache (maybe derrive from other MemoryBuffers)

// TODO: How to define core buffer?
//       User defined function creates a class in cache that adds function to list and destroys itself

namespace CEGUI {
    namespace MEMORY { //TODO: maybe create a virtual class for other memory partions

        // TODO: Give unknown a better name maybe (probably definetly)
				// dont use known, just use whatever pointer you feel like :/ (char*, void*, etc.)
				// keeping for now because I already use this in quite a few places
        typedef char* unknown;
				typedef unsigned char byte;

        class IMemoryBuffer {
        public:
            virtual ~IMemoryBuffer() = 0;
            
            //virtual unknown GetPtr() = 0; // pointer to memory block (DANGEROUS, I DON"T LIKE THIS BUT I MIGHT NEED TO IMPLEMENT IT)

            virtual int CleanUp() = 0; // delete memory and do other things like closing threads (idk why would open a thread, just giving an example)
						
            //virtual bool operator==(IMemoryBuffer* other) = 0; // might need be overloaded for map to work, im only using ptrs tho, so probably wont be a problem
        };

        struct memspan {
            unknown start, end;
        };

        template<typename T>
        T* GetMemoryBuffer(const char*);

        IMemoryBuffer* GetVaugeBuffer(const char*); // If you don't know the type of memorybuffer your getting

				// TODO: IMPLEMEMT THIS AFTER CLASS BUFFER!!!!
				template<typename T>
				T* CreateMemoryBuffer(T* MemoryBuff); // creates a memory buffer USE THIS FOR ALL MEMORY BUFFER CLASSES!

				// TODO: Create a memory buffer fn to use with allocators
				

        int AddEntry(IMemoryBuffer*, const char*, unknown&, int);

        int AddGlobalEntry(unknown&, const char*, int); // if you don't wan't to use a class

		
				typedef int (*CoreBuffCreateFn)(void*, int, const char*);

				class _CORE_MEMORY_BUFFER {
				public:
						_CORE_MEMORY_BUFFER(const char* classname, CoreBuffCreateFn fn, int size);

						// I know, I know, this basically does nothing after initialization but all other options point to memory leaks
				};
				
				
        // int GetAmountAlloc(const char*);

				// USE THIS INSTEAD OF LIBRARY MOVE FUNCTIONS
				// THIS IS WHAT MAINTAINES HMM TABLES
				// TODO: IMPLEMENT THIS
				int MoveMemory(IMemoryBuffer table, void* src, void* dest, size_t len);

				//TODO: NEED TO MAKE MEMORY BUFFER FOR GET ENTRY <-- wut?
				//HMMTable* GetTable(unsigned char pos);

        void INIT_MEM(int overload_bytes = 0);

        char* ADD_ENTRY(const char* name, int size);

        // Both return allocated memory in bytes
        int get_total_memory_size();
        int get_unallocated_size();

        void END();

        std::string byte_to_hex(unsigned char byte);

        // manage a chunck of memory called a span
        class MemoryToolkit {
        private:
            const char* m_name;
            memspan m_span;

						// TODO: In house error stack
						//std::stack<int> ErrorStack;

        public:
            MemoryToolkit() = delete;
            MemoryToolkit(const char* name, memspan memspan);
            MemoryToolkit(const char* name);
            ~MemoryToolkit();

            // prints the error and stops the program, as well as dumps the memory of the section
            void ReportError(std::string FUNCTION, char flag);

            void dump() const;

						int GetLastError();
						
        };

        
        MemoryToolkit* GetMemToolkit(const char*, memspan);
        
    };
};
#endif