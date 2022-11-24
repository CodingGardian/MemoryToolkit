#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>

//  TODO:
//      Finish deletion function
//      Implement handles
//      
//


// Idea: Create interface for window class

#include "ram.h"

#ifdef WIN32
#include "WinAPIComponent.h"
#endif

namespace CEGUI {
    namespace MEMORY {
        namespace TEXTURE { // this was originally to be used to store textures, but has enough potential to be used with other things
						
						// TODO: implement a handle for TextureNode (I think this will work nicely with the hash table)


            struct TextureNode { // a header to a texture that stores metadata in a linked list in a specified buffer
                // when a block of memory is not enogh size  (gaps in memory being used to store new textures)
            		// but there is space at another block (at the end of the texture heap or space between two other blocks)
								TextureNode* next; // the next block if the data is split, nullptr if this is the whole block

                TextureNode* seqnext; // The next block, headers are not stored sequentially in the header block, but their trail of ptrs (head --> tail) do
								TextureNode* seqbefore;
				
                char* mem;

                int vspan; // height (in pixels or whatever)
                int width; // width (in pixels or whatever)

                int len; // size of the block (in pixels), can be less than vspan*width because of continued memory

								

            };

            // not meant to be inherited from, but if you want this functionality without writing much code, go for it
            class MTextureBuffer {
            private:
                // functions
                void aux_allocateheader(TextureNode* at = nullptr);
                TextureNode* aux_getOpenBlock(TextureNode* t = nullptr);
                TextureNode* aux_getAt(void* pos);
                TextureNode* aux_getBefore(TextureNode* t);
            public: 
                
                MTextureBuffer(const char* name, short num_headers, int texture_size);
                ~MTextureBuffer();

                TextureNode* allocate(const char* filename);
                void deallocate(TextureNode* t);
                void resize(TextureNode* t);

                int CleanUp() { return 0; }

                bool operator==(IMemoryBuffer* other) { return false; }

								const MemoryToolkit& GetHeaderToolkit() {return *m_header_toolkit;}
								const MemoryToolkit& GetTextureToolkit() {return *m_texture_toolkit;}

							// TODO: implement functions for THandle

            private:
                CEGUI::MEMORY::MemoryToolkit* m_header_toolkit;
                CEGUI::MEMORY::MemoryToolkit* m_texture_toolkit;

                char* m_texture_buffer;
                short m_total_texture_size;

                char* m_header_buffer;
                short m_sizeHeaderBuffer;

                char* m_unalloc_start_texture;

                char* m_start;
                const char* m_name;

								int m_allocatedHeaders;
            		int m_texture_unallocated;

								TextureNode* m_head; // head node ALWAYS points to start of memory block, and ALWAYS at the start of header data block
								TextureNode* m_tail; // points to the end of the list

								bool m_check_block; // set to true when a block is deleted

								TextureNode** m_ptrFreeNodes;
								short m_FreeNodeTop;
            };
            

            // int* aux_getOpenBlock(); returns the first open space of memory
        };
    };
};



#endif