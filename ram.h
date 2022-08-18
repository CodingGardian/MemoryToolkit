#ifndef RAM_H
#define RAM_H

#define KILOBYTE 1000
#define MEGABYTE 1000000

#include <map>
#include <string>
#include <vector>

#define INSUFFICIENT_SPACE 1
#define ALLOCATION_ERROR 2

#define NO_INTERFACE nullptr // haha funny number

// TODO: make IMemoryBuffer as idiot-proof as possible 
// (no offense to any other programmers planning to use this libray, I am an idiot sometimes and need this too)


// TODO: write an buffer that can be used as a template allocator
namespace CEGUI {
    namespace MEMORY { //TODO: maybe create a virtual class for other memory partions

        // TODO: Give unknown a better name maybe (probably definetly)
        typedef void* unknown;

        class IMemoryBuffer {
        public:
            virtual ~IMemoryBuffer() = 0;
            
            //virtual unknown GetPtr() = 0; // pointer to memory block (DANGEROUS, I DON"T LIKE THIS BUT I MIGHT NEED TO IMPLEMENT IT)

            virtual int CleanUp() = 0; // delete memory and do other things like closing threads


            virtual bool operator==(IMemoryBuffer* other) = 0; // must be overloaded for map to work
        };

        struct memspan {
            unknown start, end;
        };

        template<typename T>
        T* GetMemoryBuffer(const char*);

        IMemoryBuffer* GetVaugeBuffer(const char*); // If you don't know the type of memorybuffer your getting

        int AddEntry(IMemoryBuffer*, const char*, unknown&, int);

        int AddGlobalEntry(unknown&, const char*, int); // if you don't wan't to use a class


        // int GetAmountAlloc(const char*);
        

       
        void INIT_MEM(int overload_bytes = 0);

        char* ADD_ENTRY(const char* name, int size);

        // Both return allocated memory in bytes
        int get_total_memory_size();
        int get_unallocated_size();

        void END();

        std::string byte_to_hex(char byte);

        // manage a chunck of memory called a span
        class MemoryToolkit {
        private:
            const char* m_name;
            memspan m_span;

        public:
            MemoryToolkit() = delete;
            MemoryToolkit(const char* name, memspan memspan);
            MemoryToolkit(const char* name);
            ~MemoryToolkit();

            // prints the error and stops the program, as well as dumps the memory of the section
            void ReportError(std::string FUNCTION, char flag);

            // moves the memory from the source to the destination. And fills src with zero NOT A COPY! You have memcpy for that
            //void z_mov(char* src, char* dest, size_t size);

            void doit() const;
        };

        
        MemoryToolkit* GetMemToolkit(const char*, memspan);
        
    };
};
#endif