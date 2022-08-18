//#include <base/memory/ram.h>
#include "ram.h"
#include <iostream>
#include <utility>

using namespace CEGUI::MEMORY;

#define INITIAL_MEMORY 100 * MEGABYTE

std::map<const char*, IMemoryBuffer*> name_class_table;
std::map<const char*, unknown&> class_ptr_table;
std::map<const char*, int> memory_alloc; // amout allocated

bool MEM_INITIALIZED = false;

int total_bytes = 0;
int unallocated = 0;
char* buff = nullptr;

std::map<const char*, char*> memory_ptr; // pointer for each

IMemoryBuffer::~IMemoryBuffer() { }


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

// return: 0, everything is file. Anything else is bad
// TODO: create functions to handle std::map return code
int CEGUI::MEMORY::AddEntry(IMemoryBuffer* obuff, const char* name, unknown& ptr, int size) {
    if (!MEM_INITIALIZED) { INIT_MEM(); }

    if (unallocated < size) { return INSUFFICIENT_SPACE; }

    ptr = buff + (total_bytes - unallocated);
 

    name_class_table.insert(std::pair<const char*, IMemoryBuffer*>{name, obuff});
    class_ptr_table.insert(std::pair<const char*, unknown&>{name, ptr});
    memory_alloc.insert(std::pair<const char*, int>{name, size});

    unallocated -= size;
    return 0;
}

int CEGUI::MEMORY::AddGlobalEntry(unknown& ptr, const char* name, int size) {
    if (!MEM_INITIALIZED) { INIT_MEM(); }

    if (unallocated < size) { return INSUFFICIENT_SPACE; }

    ptr = buff + (total_bytes - unallocated);

    name_class_table.insert(std::pair<const char*, IMemoryBuffer*>{name, NO_INTERFACE});
    class_ptr_table.insert(std::pair<const char*, unknown&>{name, ptr});
    memory_alloc.insert(std::pair<const char*, int>{name, size});

    unallocated -= size;
    return 0;
}

int get_total_memory_size() {
    return total_bytes;
}

int get_unallocated_size() {
    return unallocated;
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
    for (const std::pair<const char*, IMemoryBuffer*>& entry : name_class_table) {
        entry.second->CleanUp(); // clean up everything
    }

    delete[] buff;
}

template<typename T>
T* GetMemoryBuffer(const char* name) {
    return (T*)name_class_table[name];
}

IMemoryBuffer* GetVaugeBuffer(const char* name) {
    return name_class_table[name];
}

char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// had to add scope to keep the compiler from complaining
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

MemoryToolkit::MemoryToolkit(const char* name, memspan span) : m_name(name), m_span(span) {}

MemoryToolkit::MemoryToolkit(const char* name) : m_name(name) {}

MemoryToolkit* CEGUI::MEMORY::GetMemToolkit(const char* name, memspan span) {
    return new MemoryToolkit(name, span);
}

void MemoryToolkit::doit() const {
    dump(m_name);
}

/*void SMemoryToolkit::ReportError(std::string FUNCTION) {
    std::cerr << "MEMORY ERROR: "

}*/