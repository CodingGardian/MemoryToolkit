#ifndef RAM_H
#define RAM_H

#define KILOBYTE 1000
#define MEGABYTE 1000000

#include <map>
#include <string>
#include <vector>

namespace CEGUI {
    namespace MEMORY { //TODO: maybe create a virtual class for other memory partions
        
        extern bool MEM_INITIALIZED;

        extern int total_bytes;
        extern int unallocated;
        extern char* buff;
        extern std::map<const char*, int> memory_alloc; // amout allocated
        extern std::map<const char*, char*> memory_ptr; // pointer for each

        void INIT_MEM(int overload_bytes = 0);

        char* ADD_ENTRY(const char* name, int size);
        
        void END();

        std::string byte_to_hex(char byte);

        class MemoryToolkit {
        private:
            //std::vector<std::string> section; // gives a way of naming sections of allocated blocks
            const char* name;

        public:
            MemoryToolkit() = delete;
            MemoryToolkit(const char* name);
            ~MemoryToolkit();

            // prints the error and stops the program, as well as dumps the memory of the section
            void ReportError(std::string FUNCTION, char flag);

            // moves the memory from the source to the destination. And fills src with zero NOT A COPY! You have memcpy for that
            void z_mov(char* src, char* dest, size_t size);

            void doit();
        };
        
    };
};
#endif