/*#include <base/memory/texture.h>
#include <base/memory/ram.h>
#include <base/fileio.h>
*/


#include "texture.h"
#include "ram.h"

#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include "WinAPIComponent.h"
#endif

#define TEXTURE_SIZE 8 * MEGABYTE
#define HEADER_SIZE 1 * MEGABYTE

using namespace CEGUI::MEMORY::TEXTURE;

CEGUI::MEMORY::MemoryToolkit* CEGUI::MEMORY::TEXTURE::mt_header;
CEGUI::MEMORY::MemoryToolkit* CEGUI::MEMORY::TEXTURE::mt;

int CEGUI::MEMORY::TEXTURE::header_unallocated = 0;
int CEGUI::MEMORY::TEXTURE::texture_unallocated = 0;

char* texture_buffer;
int total_texture_size; // total bytes

char* header_buffer;
int total_header_size; // total bytes

char* unalloc_start_metadata; // points to the start of the unallocated end in the metadata block (in bytes)
char* unalloc_start_texture; // points to the start of the unallocated end in the metadata block (in bytes)

bool check_block; // set to true when a block is deleted

TextureNode* head; // head node ALWAYS points to start of memory block, and ALWAYS at the start of header data block
TextureNode* tail; // points to the end of the list

void CEGUI::MEMORY::TEXTURE::INIT_TEXTURE(int overload_bytes_texture, int overload_bytes_header) {
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

    // why :(
    mt_header = new CEGUI::MEMORY::MemoryToolkit("texture.metadata");
    mt = new CEGUI::MEMORY::MemoryToolkit("texture");
    check_block = false;
}

void aux_allocateheader(TextureNode* at = nullptr) { // allocates a header, which is always at the end of the metadata block (headers are not store sequentially with memory blocks)
    //std::cout << "AAAAHHH" << std::endl;
    if (header_unallocated < sizeof(TextureNode)) {
        std::cerr << "MEMORY ERROR: CRITICAL texture.metadata MEMORY IN FUNCTION " << /*__PRETTY_FUNCTION__ <<*/ std::endl;
        exit(-1);
    }
    // allocate at end of metadata block
    header_unallocated -= sizeof(TextureNode);
    TextureNode* returnVal = (TextureNode*)unalloc_start_metadata;
    unalloc_start_metadata += sizeof(TextureNode);
    // only runs when the list is empty
    if (head == nullptr) {
        head = returnVal;
        head->next = nullptr;
        head->seqnext = head;

        head->len = 0;
        head->vspan = 0;
        head->width = 0;
        head->mem = nullptr;

        tail = head;
        tail->seqnext = head;
        return;
    }

    if (at == nullptr) {
        // fill it up
        returnVal->seqnext = head;
        returnVal->next = nullptr;

        returnVal->len = 0;
        returnVal->mem = nullptr;
        returnVal->width = 0;
        returnVal->vspan = 0;
        

        tail->seqnext = returnVal;
        tail = returnVal;
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

TextureNode* aux_getOpenBlock(TextureNode* t = nullptr) {
    if (check_block == false) {
        check_block = true;
        return nullptr;
    }
    else {
        if (t == nullptr) {t = head;}
        while(1) {
            if (t->seqnext == head) {break;}
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
TextureNode* aux_getAt(void* pos) {
    TextureNode* temp = head;
    do {
        if (temp->mem == static_cast<int*>(pos)) {
            return temp;
        }
        else { temp = temp->seqnext; }


    } while (temp->seqnext != head);

    return nullptr;
}

TextureNode* aux_getBefore(TextureNode* t) { // avoid use, O(n) time
    for (TextureNode* i=head; (char*)i < unalloc_start_metadata; i = i->seqnext) {
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

// this is pain
// 48 bytes (should be) allocated in the header space for every texture of any size. Less if working with a 32 or 16 bit CPU
TextureNode* CEGUI::MEMORY::TEXTURE::allocate(const char* filename) {
    int size;
#ifndef WIN32
    struct stat tempstat;
    if (stat(filename, &tempstat) == 0) {
        size = tempstat.st_size;   
    }
    else {
        std::cerr << "FILE IO ERROR: STAT FUNCTION FAILED IN FUNCTION " << /*__PRETTY_FUNCTION__ <<*/ std::endl;
    }
    size -= 8; // two integers at the start of the file describe the length and width of the file
#endif

#ifdef WIN32
    //fIOthing.OpenFile(filename); fix wchar_t

#endif

    std::ifstream temp_fstream;
    temp_fstream.open( filename, std::ios::binary | std::ios::in );
    if (!temp_fstream.good()) {
        std::cerr << "MEMORY ERROR: FAILED TO OPEN " << filename << " IN FUNCTION " << /*__PRETTY_FUNCTION__ <<*/ std::endl;
        exit(-1);
    }

    if (size > texture_unallocated) {
        std::cerr << "MEMORY ERROR: CRITICAL texture MEMORY IN FUNCTION " << /*__PRETTY_FUNCTION__ <<*/ std::endl;
        exit(-1);
    }

    texture_unallocated -= size;
    TextureNode* temp_block = aux_getOpenBlock();
    size /= 4;
    
    int* l_w = new int[2];
    temp_fstream.read((char*)l_w, 8); // read length and width into file
    if (temp_block == nullptr) {
        //std::cout << "here" << std::endl;
        TextureNode* temp_end = tail;
        
        aux_allocateheader(); // new header is pointed to by tail

        // allocate tail
        tail->mem = (int*)unalloc_start_texture;
        tail->len = size;
        tail->vspan = l_w[0];
        tail->width = l_w[1];

        unalloc_start_texture += size*4;

        // fill with data
        temp_fstream.read((char*)tail->mem, (size)*4);
        return tail;
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
                    TextureNode* temp_end = tail;

                    aux_allocateheader();

                    tail->mem = (int*)unalloc_start_texture;
                    temp_prev->next = tail;
                    
                    // fill up space
                    temp_fstream.read((char*)tail->mem,(size)*4);

                    // fill up rest of node
                    temp_end->len = size;
                    temp_end->vspan = l_w[0];
                    temp_end->width = l_w[1];

                    // increment unalloc_start_texture
                    unalloc_start_texture += size;
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
            
        } 
    }
    
    delete[] l_w;
}

// NOTE: decrement linked list texture header pointers and such to account for the memory being moved
// this is not efficient, potential solutions:
// hash table (longer allocation, but allows memory deletion to have more chance of being O(1) best case)
// ?
/*void deallocate(TextureNode* node) {
    // this will be hard
    //  no need to delete memory since it will be overwritten
    if (node == head) {std::cerr << "H E C K  O F F" << std::endl; exit(-1);}
    if (node == tail) { TextureNode* temp = aux_getBefore(tail); temp->next = nullptr; tail = temp; return; }
    // This checks if memory is split
    if (node->next == nullptr) {
        // memory is not split

        int temp_len = node->len;
        TextureNode* temp = node;

        do {
            temp = aux_getAt(temp + temp->len); // get the next memory block
            memcpy(temp - temp->len, temp, temp->len);

        } while (temp != tail);
      
        

        
    }
    else {
        // if it is:
        // 1. start at beginning block
        // LOOP UNTIL NO MORE BLOCKS LEFT
        // 2. save next block
        // 3. delete the block in texture buffer
        // 4. delete texture metadata in texture header buffer
        // 4b. move memory over (DECREMENT!!! SEE NOTE)
        // 5. delete texture node in texture header buffer
        // 5b. move memory over (DECREMENT!!! SEE NOTE)
        // 6. go to saved block and repeat


    }

   
}

void resize(TextureNode* t) { // NO MORE TORTURE PLEASE IM LITERALLY DYING ON THE INSIDE HECK
    // AAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHH
}*/