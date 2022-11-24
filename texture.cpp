/*#include <base/memory/texture.h>
#include <base/memory/ram.h>
#include <base/fileio.h>
*/

#define _CRT_SECURE_NO_WARNINGS

// TODO: Maybe create handle system instead of pointers (2 byte shorts used instead of 4-8 byte ptrs)
// TODO: Definitley need a hash table handle system, without it moving memory woulbe be a nightmare

#include "texture.h"

#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <fstream>


#define TEXTURE_SIZE 8 * MEGABYTE
#define HEADER_SIZE 1 * MEGABYTE

using namespace CEGUI::MEMORY::TEXTURE;

using CEGUI::MEMORY::unknown;

/*char* texture_buffer;
int total_texture_size; // total bytes

char* header_buffer;
int total_header_size; // total bytes

char* unalloc_start_metadata; // points to the start of the unallocated end in the metadata block (in bytes)
char* unalloc_start_texture; // points to the start of the unallocated end in the metadata block (in bytes)

bool check_block; // set to true when a block is deleted*/

MTextureBuffer::~MTextureBuffer() {}

MTextureBuffer::MTextureBuffer(const char* name, short num_headers, int texture_size) : m_name(name), m_total_texture_size(texture_size) {

    m_sizeHeaderBuffer = num_headers;
	m_ptrFreeNodes = new TextureNode*[m_sizeHeaderBuffer];

    m_start = (char*)malloc( (m_sizeHeaderBuffer * sizeof(TextureNode) ) + texture_size);
	
	if (m_start == nullptr) {std::cout << "AHHHHHH" << std::endl; exit(-1);} // Maybe get rid of all these abrupt exits?

	m_total_texture_size = texture_size;
	
    m_texture_unallocated = m_total_texture_size;

    m_header_buffer = m_start;
    m_texture_buffer = m_start + m_sizeHeaderBuffer * sizeof(TextureNode);

	m_unalloc_start_texture = m_texture_buffer;
	
	m_head = nullptr;
    m_tail = nullptr;

	m_check_block = false;

		// fill up free node list
	TextureNode* temp = (TextureNode*)m_header_buffer + ( (m_sizeHeaderBuffer - 1) );
	for (int i=1; i<=m_sizeHeaderBuffer; i++) {
		m_ptrFreeNodes[i-1] = temp;
		temp -= 1;
	}
	m_FreeNodeTop = m_sizeHeaderBuffer-1;
	
		// TODO: make this not bad
    std::string stemp(m_name), stemp2(m_name);
    stemp += "_header";
    stemp2 += "_texture";

    char* s1 = new char[stemp.length() + 1];
    char* s2 = new char[stemp2.length() + 1];
    strncpy(s1, stemp.c_str(), stemp.length()+1);
    strncpy(s2, stemp2.c_str(), stemp2.length()+1);
    
    memspan m1 = { (unknown)m_start, (unknown)(m_start + m_sizeHeaderBuffer*sizeof(TextureNode)) };
    memspan m2 = { (unknown)m_texture_buffer, (unknown)(m_texture_buffer + m_total_texture_size) };
		
    m_header_toolkit = GetMemToolkit(s1,  m1);
    m_texture_toolkit = GetMemToolkit(s2,  m2);

	m_allocatedHeaders = 0;
	
}

// void MTextureBuffer::start() { // CEGUI::MEMORY::AddEntry(this, m_name, (void*&)m_start, (m_total_header_size + m_total_texture_size)); }


/*void CEGUI::MEMORY::TEXTURE::INIT_TEXTURE(int overload_bytes_texture, int overload_bytes_header) {
    if (overload_bytes_header == 0) { // header buffer contains linked list
        header_buffer = CEGUI::MEMORY::ADD_ENTRY("texture.metadata", HEADER_SIZE);
        unalloc_start_metadata = header_buffer;
        total_header_size = HEADER_SIZE;
        header_unallocated = HEADER_SIZE;
    }
    else {
        header_buffer = CEGUI::MEMORY::ADD_ENTRY("texture.metadata", overload_bytes_header);
        unalloc_start_metadata = header_buffer;
        total_header_size = overload_bytes_header;
        header_unallocated = overload_bytes_header;
    }

    head = nullptr;
    
    if (overload_bytes_texture == 0) {
        texture_buffer = CEGUI::MEMORY::ADD_ENTRY("texture", TEXTURE_SIZE);
        unalloc_start_texture = texture_buffer;
        total_texture_size = TEXTURE_SIZE;
        texture_unallocated = TEXTURE_SIZE;
    }
    else {
        texture_buffer = CEGUI::MEMORY::ADD_ENTRY("texture", overload_bytes_texture);
        unalloc_start_texture = texture_buffer;
        total_texture_size = overload_bytes_texture;
        texture_unallocated = overload_bytes_texture;
    }

    mt_header = new CEGUI::MEMORY::MemoryToolkit("texture.metadata");
    mt = new CEGUI::MEMORY::MemoryToolkit("texture");
    check_block = false;
}*/

void MTextureBuffer::aux_allocateheader(TextureNode* at) { // allocates a header. Always set up as a tail unless at is not nullptr, in which case it will be insertes infront of specified header (seqnext of ptr at will be set to new header). Headers are allways allocated at the end of header block
    //std::cout << "AAAAHHH" << std::endl;
    if (m_allocatedHeaders == m_sizeHeaderBuffer) {
        std::cerr << "MEMORY ERROR: CRITICAL HEADER MEMORY IN CLASSNAME " << m_name << std::endl;
        exit(-1);
    }
		
    // allocate at end of metadata block
    m_allocatedHeaders += 1;
		TextureNode* returnVal = m_ptrFreeNodes[m_FreeNodeTop];
		m_FreeNodeTop--;
	
    // only runs when the list is empty
    if (m_head == nullptr) {
        m_head = returnVal;
        m_head->next = nullptr;
        m_head->seqnext = m_head;
				
        m_head->len = 0;
        m_head->vspan = 0;
        m_head->width = 0;
        m_head->mem = nullptr;
			
				m_head->seqbefore = m_head;
        m_tail = m_head;
        return;
    }


    if (at == nullptr) {
        // fill it up
        returnVal->seqnext = m_head;
        returnVal->next = nullptr;

        returnVal->len = 0;
        returnVal->mem = nullptr;
        returnVal->width = 0;
        returnVal->vspan = 0;
			
				m_head->seqbefore = returnVal;

				returnVal->seqbefore = m_tail;
        m_tail->seqnext = returnVal;
        m_tail = returnVal;
    }
    else {
        returnVal->next = nullptr;

        returnVal->len = 0;
        returnVal->mem = nullptr;
        returnVal->width = 0;
        returnVal->vspan = 0;

        returnVal->seqnext = at->seqnext;
				returnVal->seqbefore = at;
			
				at->seqnext->seqbefore = returnVal;
        at->seqnext = returnVal;
    }
}
// O(n) time, use sparingly
// returns block behind open space
TextureNode* MTextureBuffer::aux_getOpenBlock(TextureNode* t) {
    if (!m_check_block) { // what is this?
        m_check_block = true;
        return nullptr;
    }
    else {
        if (t == nullptr) {t = m_head;}
        while(1) {
            if (t->seqnext == m_head) {break;}
            char* temp = t->mem + t->len;
            if (temp != t->seqnext->mem) {
                return t;
            }
            else {t = t->seqnext;}
        }
    }

    return nullptr;
}

// gets a texture at memory position, O(n) time, use sparingly. Returns null on fail

TextureNode* MTextureBuffer::aux_getAt(void* pos) {
  // this is a very bad idea  
	/*TextureNode* temp = m_head;
    do {
        if (temp->mem == static_cast<int*>(pos)) {
            return temp;
        }
        else { temp = temp->seqnext; }


    } while (temp->seqnext != m_head);

    return nullptr;*/
    return nullptr;
}

TextureNode* MTextureBuffer::aux_getBefore(TextureNode* t) { // avoid use, O(n) time
    /*for (TextureNode* i=m_head; (char*)i < m_unalloc_start_header; i = i->seqnext) {
        if (i->seqnext == t) {
            return i;
        }
    }

    std::cerr << "MEMORY ERROR: UNEXPECTED FAIL IN FUNCTION: " << /*__PRETTY_FUNCTION__ <<*/ //std::endl;
    //exit(-1);
    return nullptr;
}

#ifdef WIN32
const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

WAPIFileIO fIOthing;
#endif

int FileSize(const char* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	file.seekg(0, std::ios::end);
	return file.tellg();
}

// this is pain (fix dependancy, do bridge method)
// 48 bytes (should be) allocated in the header space for every texture of any size. Less if working with a 32 bit CPU (which is likely the case with replit)
// allocation is much slower compared to memory pool, better to use this for large data
TextureNode* MTextureBuffer::allocate(const char* filename) {
/*#ifdef WIN32
    /*HANDLE hfile = fIOthing.OpenFile(GetWC(filename));
    size = fIOthing.GetFileSize2(hfile);
    // windows API can be so much cleaner sometimes...
    CloseHandle(hfile);

    size -= 8;
#endif*/
		// se know that this works, but will have to investigate fstream crash...
    std::ifstream temp_fstream(filename, std::ios::binary | std::ios::in);
    if (!temp_fstream.good()) {
        std::cerr << "MEMORY ERROR: FAILED TO OPEN " << filename << " IN FUNCTION " << std::endl;
        exit(-1);
		}

    
    int* l_w = new int[2];
    temp_fstream.read((char*)l_w, 8); // read length and width into file
		int size = l_w[0] * l_w[1];
		
		if (size > m_texture_unallocated) {
        std::cerr << "MEMORY ERROR: CRITICAL texture MEMORY IN FUNCTION " << std::endl;
        exit(-1);
    }
	
		m_texture_unallocated -= size;
	
    std::cout << l_w[0] * l_w[1] << " ";

		TextureNode* temp_block = aux_getOpenBlock();

    if (!temp_block) {
        aux_allocateheader(); // new header is pointed to by tail
	
        // allocate tail
        m_tail->mem = m_unalloc_start_texture;
        m_tail->len = size;
        m_tail->vspan = l_w[0];
        m_tail->width = l_w[1];

        m_unalloc_start_texture += size;

        // fill with data
        temp_fstream.read(m_tail->mem, size);
				
        return m_tail;
    }
    else {
        // allocate header & texture metadata
        aux_allocateheader(temp_block);

        // start texture data at block
        TextureNode* temp_next = temp_block->seqnext;
        temp_next->mem = temp_block->mem + temp_block->len;

        char* start = temp_next->mem;
        char* end = temp_next->seqnext->mem;
			
        // if block is not big enough, still fill with data and repeat aux_getOpenBlock(temp_block->next)
        if (end - start < size) {
            size -= (end - start);
            std::cout << "Here" << std::endl;

            TextureNode* temp_prev = temp_next;

            // fill with data
            temp_fstream.read(temp_next->mem, end-start);

            while(1) {
                // check if another block is open
                TextureNode* temp_loop = aux_getOpenBlock(temp_next);
                
                if (temp_loop == nullptr) {
                    // block is not open, allocate at end of list
                    TextureNode* temp_end = m_tail;

                    aux_allocateheader();

                    m_tail->mem = m_unalloc_start_texture;
                    temp_prev->next = m_tail;
                    
                    // fill up space
                    temp_fstream.read((char*)m_tail->mem, size);

                    // fill up rest of node
                    temp_end->len = size;
                    temp_end->vspan = l_w[0];
                    temp_end->width = l_w[1];

                    // increment unalloc_start_texture
                    m_unalloc_start_texture += size;
                    break;
                }
                else {
                    aux_allocateheader(temp_loop);

                    temp_next = temp_loop->seqnext;
                    temp_next->mem = temp_loop->mem + temp_loop->len;

                    start = temp_next->mem;
                    end = temp_next->seqnext->mem;

                    temp_prev->next = temp_next;

                    if (end - start < size) {
                        size -= (end-start);

                        // fill with data
                        temp_fstream.read(temp_next->mem, end-start);

                        temp_next->len = (end-start);
                        temp_next->vspan = l_w[0];
                        temp_next->width = l_w[1];

                    }
                    else {
                        
                        temp_fstream.read(temp_next->mem, size);

                        temp_next->len = size;
                        temp_next->vspan = l_w[0];
                        temp_next->width = l_w[1];
											
                        break;
                    }
                }
            }
        }
        else {
            // fill with data
            temp_fstream.read((char*)temp_next->mem, size);

            temp_next->len = size;
            temp_next->vspan = l_w[0];
            temp_next->width = l_w[1];

						return temp_next;
            
        } 
    }
    
    delete[] l_w;
}


// NOTE: decrement linked list texture header pointers and such to account for the memory being moved (this is the most time expensive "basic" function)
// LETS GO BOYS O(1) TIMEEEEEEEEE
void MTextureBuffer::deallocate(TextureNode* node) {
	  // this will be hard
    //  no need to delete memory since it will be overwritten
    if (node == m_head) {std::cerr << "H E C K  O F F" << std::endl; exit(-1);}
		
		// IF NOT SPLIT
		if (node->next == nullptr) {
				// get all the blocks I need
				TextureNode* tempbefore = node->seqbefore;
				TextureNode* tempafter = node->seqnext;
				
				// set seqnext & seqbefore appropriatley
				tempbefore->seqnext = tempafter;
				tempafter->seqbefore = tempbefore;

				// add to list of free nodes
				m_ptrFreeNodes[++m_FreeNodeTop] = node;
		}
		else {
				// IF SPLIT:
				// find block behing and after this one and continued block
				// set seqnext approproatley
				// REPEAT FOR CONTINUED BLOCK UNTIL NO MORE LEFT

				while (node->next != nullptr) {
						TextureNode* tempbefore = node->seqbefore;
						TextureNode* tempafter = node->seqnext;
				
						// set seqnext & seqbefore appropriatley
						tempbefore->seqnext = tempafter;
						tempafter->seqbefore = tempbefore;

						// add to list of free nodes
						m_ptrFreeNodes[++m_FreeNodeTop] = node;

						node = node->next;
				}
		}
		
		

    

   
}

void resize(TextureNode* t) { // NO MORE TORTURE PLEASE IM LITERALLY DYING ON THE INSIDE HECK
    // AAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHH
	
}
