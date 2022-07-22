#include <base/memory/ram.h>

#include <iostream>
#include <utility>

using namespace CEGUI::MEMORY;

#define INITIAL_MEMORY 100 * MEGABYTE

bool CEGUI::MEMORY::MEM_INITIALIZED = false;

int CEGUI::MEMORY::total_bytes = 0;
int CEGUI::MEMORY::unallocated = 0;
char* CEGUI::MEMORY::buff = nullptr;
std::map<const char*, int> CEGUI::MEMORY::memory_alloc; // amout allocated
std::map<const char*, char*> CEGUI::MEMORY::memory_ptr; // pointer for each

void CEGUI::MEMORY::INIT_MEM(int overload_bytes) {
    if (overload_bytes == 0) {
        buff = new char[INITIAL_MEMORY];
        unallocated = INITIAL_MEMORY;
        total_bytes = INITIAL_MEMORY;
    }
    else {
        if (overload_bytes < MEGABYTE) {std::cerr << "Really? under 1 MB of space?" << std::endl;}
        buff = new char[overload_bytes];
        unallocated = overload_bytes;
        total_bytes = overload_bytes;
    }
    MEM_INITIALIZED = true;
}

char* CEGUI::MEMORY::ADD_ENTRY(const char* name, int size) {
    if (!MEM_INITIALIZED) {INIT_MEM();}

    if (unallocated < size) {
        std::cerr << "ADD_ENTRY ERR: OVERFLOW OPERATION CAUGHT" << std::endl;
        exit(-1);
    }
    
    memory_alloc.insert(std::pair<const char*, int>{name, size});

    char* temp = buff + (total_bytes - unallocated);
    memory_ptr.insert(std::pair<const char*, char*>{name, temp});

    unallocated -= size;
    return temp;
}

void CEGUI::MEMORY::END() {
    delete[] buff;
}

char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// had to add scope to keep the IDE from complaining
std::string CEGUI::MEMORY::byte_to_hex(char byte) {
    std::string returnVal = "";
    returnVal += hex[(byte>>4)];
    returnVal += hex[(byte & 0x0F)];

    return returnVal;
}

// "throws up" the group specified in (10 bytes per row)
void dump(const char* name) {
    char* mem = memory_ptr[name];
    int size_bytes = memory_alloc[name];

    for (int i=0; i<size_bytes; i++) {
        std::cout << byte_to_hex(*mem) << ' ';
        mem++;

        if (((i+1) % 10) == 0) {
            std::cout << '\n';
        }
    }
}

MemoryToolkit::MemoryToolkit(const char* name) : name(name) {}

void MemoryToolkit::doit() {
    dump(name);
}

void MemoryToolkit::ReportError(std::string FUNCTION) {
    std::cerr << "MEMORY ERROR: "

}