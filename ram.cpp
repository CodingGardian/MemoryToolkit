//#include <base/memory/ram.h>
#include "ram.h"
#include <iostream>
#include <utility>

using namespace CEGUI::MEMORY;

#define DEFAULT_MEMORY_SIZE 100 * MEGABYTE

#define DEFAULT_CLASS_BUFFER_SIZE 5 * MEGABYTE

// map allocates on heap, so should be fine to just stick them here
std::map<const char*, IMemoryBuffer*> name_class_table;
std::map<const char*, unknown&> class_ptr_table;
std::map<const char*, int> memory_alloc; // amout allocated

bool MEM_INITIALIZED = false;

int total_bytes = 0;
int unallocated = 0;
char* buff = nullptr;


class _CORE_CLASS_LIST_TEMP {
private:
		_CORE_CLASS_LIST_RM();

		// to lazy to create triple container, heck off
		std::vector<std::pair<const char*, int>> m_list;
		std::vector<CoreBuffCreateFn> m_funcs;
		friend class _CREATE_CORE_MEMORY_BUFFER;
};

_CORE_CLASS_LIST_RM* CoreClassContainer = nullptr;


IMemoryBuffer::~IMemoryBuffer() { }

CEGUI::MEMORY::_CREATE_CORE_MEMORY_BUFFER(const char* name, CoreBuffCreateFn fn, int size) {
		if (!CoreClassContainer) {CoreClassContainer = new _CORE_CLASS_LIST_TEMP;}

		CoreClassContainer->m_list.push_back(std::pair<const char*, int>{name, size});
		CoreClassContainer->m_funcs.push_back(fn);
		
}


int INITIALIZE_ALL_CORE_BUFFERS() {
	// TODO!!!!!!!!!!
	
	// loop through all core classes in container
	// for each one, allocate the memory and increment the start pointer for the class buffer

	delete CoreClassContainer;
	return 0;
}

// Defined in ClassBuffer.cpp
extern int CREATE_CLASS_BUFFER(void*, int);

void CEGUI::MEMORY::INIT_MEM(int overload_bytes) {
    if (overload_bytes == 0) {
        buff = new char[DEFAULT_MEMORY_SIZE];
				/*if (buff == nullptr) {
					std::cout << "heck" << std::endl;
					exit(-1);
				}*/
				// TODO: Catch exeption and exit instead of checking for nullptr
        unallocated = DEFAULT_MEMORY_SIZE;
        total_bytes = DEFAULT_MEMORY_SIZE;
    }
    else {
        if (overload_bytes < MEGABYTE) {std::cerr << "Really? under 1 MB of space?" << std::endl;}
        buff = new char[overload_bytes];
				/*if (buff == nullptr) {
					std::cout << "heck1" << std::endl;
					exit(-1);
				}*/
				// TODO: Catch exeption and exit instead of checking for nullptr
        unallocated = overload_bytes;
        total_bytes = overload_bytes;
    }

		//TODO: Create global MemoryToolkit for the WHOLE memory pool
	
		if (CREATE_CLASS_BUFFER(buff, DEFAULT_CLASS_BUFFER_SIZE)) {
			// uh oh
		}
	
		if (INITIALIZE_ALL_CORE_BUFFERS()) {
			// get last memory error and return
		}
	
    MEM_INITIALIZED = true;
}

// return: 0, everything is fine. Anything else is bad
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

void CEGUI::MEMORY::END() {
		// TODO: check if this actually works
    for (const std::pair<const char*, IMemoryBuffer*>& entry : name_class_table) {
        entry.second->CleanUp(); // clean up everything
    }

    delete[] buff;
}

template<typename T>
T* GetMemoryBuffer(const char* name) {
    return dynamic_cast<T*>(name_class_table[name]);
}

IMemoryBuffer* GetVaugeBuffer(const char* name) {
    return name_class_table[name];
}

char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// had to add scope to keep the compiler from complaining
std::string CEGUI::MEMORY::byte_to_hex(byte byteval) {
    std::string returnVal = "";
    returnVal += hex[(byteval>>4)];
    returnVal += hex[(byteval & 0x0F)];

    return returnVal;
}

// "throws up" the group specified in (10 bytes per row)
void MemoryToolkit::dump() const {
		std::cout << m_name << std::endl;
    int size_bytes = m_span.end - m_span.start;
		char* mem1 = m_span.start;
    for (int i=0; i<size_bytes; i++) {
        std::cout << byte_to_hex(*mem1) << ' ';
        mem1++;

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

/*void SMemoryToolkit::ReportError(std::string FUNCTION) {
    std::cerr << "MEMORY ERROR: "

}*/