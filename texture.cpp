/*#include <base/memory/texture.h>
#include <base/memory/ram.h>
#include <base/fileio.h>
*/

//#define _CRT_SECURE_NO_WARNINGS

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

MTextureBuffer::MTextureBuffer(const char* name, int header_size, int texture_size) : m_name(name), m_total_header_size(header_size), m_total_texture_size(texture_size) { 
    if (CEGUI::MEMORY::AddEntry(this, m_name, (unknown&)m_start, (m_total_header_size + m_total_texture_size)) == INSUFFICIENT_SPACE)
    {
        std::cout << "Insufficient space for memory allocation in MtextureBuffer allocation" << std::endl;
        exit(-1);
    }
    m_header_unallocated = m_total_header_size;
    m_texture_unallocated = m_total_texture_size;

    m_header_buffer = m_start;
    m_texture_buffer = m_start + m_total_header_size;

		m_unalloc_start_header = m_header_buffer;
		m_unalloc_start_texture = m_texture_buffer;
	
		m_head = nullptr;
		m_tail = nullptr;

		m_check_block = false;

		// TODO: make this not bad
    std::string stemp(m_name), stemp2(m_name);
    stemp += "_header";
    stemp2 += "_texture";

		char* s1 = new char[stemp.length() + 1];
		char* s2 = new char[stemp2.length() + 1];
		strncpy(s1, stemp.c_str(), stemp.length()+1);
		strncpy(s2, stemp2.c_str(), stemp2.length()+1);

		memspan m1 = { (unknown)m_start, (unknown)(m_start + m_total_header_size) };
		memspan m2 = { (unknown)m_texture_buffer, (unknown)(m_texture_buffer + m_total_texture_size) };
		
    m_header_toolkit = GetMemToolkit(s1,  m1);
    m_texture_toolkit = GetMemToolkit(s2,  m2);
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
    if (m_header_unallocated < sizeof(TextureNode)) {
        std::cerr << "MEMORY ERROR: CRITICAL HEADER MEMORY IN CLASSNAME " << m_name << std::endl;
        exit(-1);
    }
    // allocate at end of metadata block
    m_header_unallocated -= sizeof(TextureNode);
    TextureNode* returnVal = (TextureNode*)m_unalloc_start_header;
    m_unalloc_start_header += sizeof(TextureNode);
    // only runs when the list is empty
    if (m_head == nullptr) {
        m_head = returnVal;
        m_head->next = nullptr;
        m_head->seqnext = m_head;
				
        m_head->len = 0;
        m_head->vspan = 0;
        m_head->width = 0;
        m_head->mem = nullptr;

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
            int* temp = t->mem + t->len;
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
    TextureNode* temp = m_head;
    do {
        if (temp->mem == static_cast<int*>(pos)) {
            return temp;
        }
        else { temp = temp->seqnext; }


    } while (temp->seqnext != m_head);

    return nullptr;
}

TextureNode* MTextureBuffer::aux_getBefore(TextureNode* t) { // avoid use, O(n) time
    for (TextureNode* i=m_head; (char*)i < m_unalloc_start_header; i = i->seqnext) {
        if (i->seqnext == t) {
            return i;
        }
    }

    std::cerr << "MEMORY ERROR: UNEXPECTED FAIL IN FUNCTION: " << /*__PRETTY_FUNCTION__ <<*/ std::endl;
    exit(-1);
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

// this is pain (fix dependancy, do bridge method)
// 48 bytes (should be) allocated in the header space for every texture of any size. Less if working with a 32 bit CPU (which is likely the case with replit)
TextureNode* MTextureBuffer::allocate(const char* filename) {
    int size=0;
#ifndef WIN32
    struct stat tempstat;

    if (stat(filename, &tempstat) == 0) {
        size = tempstat.st_size;
    }
    else {
        std::cout << "FILE IO ERROR: STAT FUNCTION FAILED IN FUNCTION " << std::endl;
    }
    size -= 8; // two integers at the start of the file describe the length and width of the file
	
#endif

#ifdef WIN32
    HANDLE hfile = fIOthing.OpenFile(GetWC(filename));
    size = fIOthing.GetFileSize2(hfile);
    // windows API can be so much cleaner sometimes...
    CloseHandle(hfile);

    size -= 8;
#endif
	
    std::ifstream temp_fstream(filename, std::ios::binary | std::ios::in);
    if (!temp_fstream.good()) {
        std::cerr << "MEMORY ERROR: FAILED TO OPEN " << filename << " IN FUNCTION " << std::endl;
        exit(-1);
}

    if (size > m_texture_unallocated) {
        std::cerr << "MEMORY ERROR: CRITICAL texture MEMORY IN FUNCTION " << std::endl;
        exit(-1);
    }

    m_texture_unallocated -= size;
    TextureNode* temp_block = aux_getOpenBlock();
    size /= 4;
    
    int* l_w = new int[2];
    temp_fstream.read((char*)l_w, 8); // read length and width into file
		
    if (!temp_block) {
        aux_allocateheader(); // new header is pointed to by tail

        // allocate tail
        m_tail->mem = (int*)m_unalloc_start_texture;
        m_tail->len = size;
        m_tail->vspan = l_w[0];
        m_tail->width = l_w[1];

        m_unalloc_start_texture += size*4;

        // fill with data
        temp_fstream.read((char*)m_tail->mem, (size)*4);
				
        return m_tail;
    }
    else {
        // allocate header & texture metadata
        aux_allocateheader(temp_block);

        // start texture data at block
        TextureNode* temp_next = temp_block->seqnext;
        temp_next->mem = temp_block->mem + temp_block->len;

        int* start = temp_next->mem;
        int* end = temp_next->seqnext->mem;
			
        // if block is not big enough, still fill with data and repeat aux_getOpenBlock(temp_block->next)
        if (end - start < size) {
            size -= (end - start);

            TextureNode* temp_prev = temp_next;

            // fill with data
            temp_fstream.read((char*)temp_next->mem, (end-start)*4);

            while(1) {
                // check if another block is open
                TextureNode* temp_loop = aux_getOpenBlock(temp_next);
                
                if (temp_loop == nullptr) {
                    // block is not open, allocate at end of list
                    TextureNode* temp_end = m_tail;

                    aux_allocateheader();

                    m_tail->mem = (int*)m_unalloc_start_texture;
                    temp_prev->next = m_tail;
                    
                    // fill up space
                    temp_fstream.read((char*)m_tail->mem,(size)*4);

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
                        temp_fstream.read((char*)temp_next->mem, (end-start)*4);

                        temp_next->len = (end-start);
                        temp_next->vspan = l_w[0];
                        temp_next->width = l_w[1];

                    }
                    else {
                        
                        temp_fstream.read((char*)temp_next->mem, (size)*4);

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
            temp_fstream.read((char*)temp_next->mem, (size_t)(size)*4);

            temp_next->len = size;
            temp_next->vspan = l_w[0];
            temp_next->width = l_w[1];

						return temp_next;
            
        } 
    }
    
    delete[] l_w;
}


// NOTE: decrement linked list texture header pointers and such to account for the memory being moved (this is the most time expensive "basic" function)
void MTextureBuffer::deallocate(TextureNode* node) {
    // this will be hard
    //  no need to delete memory since it will be overwritten
    if (node == head) {std::cerr << "H E C K  O F F" << std::endl; exit(-1);}
		
		// IF NOT SPLIT
		if (node->next == nullptr) {
				// find block behind & after this one
				TextureNode* node_before = aux_getBefore(node);
				TextureNode* node_after = node->seqnext;
				// delete this block (remove header and move other mem)
				
				// set seqnext appropriatley
		}
		
		// IF SPLIT:
		// find block behing and after this one and continued block
		// delete block (remove header and move other mem)
		// set seqnext approproatley
		// REPEAT FOR CONTINUED BLOCK UNTIL NO MORE LEFT

    

   
}

void resize(TextureNode* t) { // NO MORE TORTURE PLEASE IM LITERALLY DYING ON THE INSIDE HECK
    // AAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHH
<<<<<<< Updated upstream
}*/
=======
}
>>>>>>> Stashed changes
