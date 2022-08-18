#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>

//  TODO:
//      Finish deletion function
//      Decide if hash table is a good thing std::vector
//      Get rid of globals (what the fuck was I thinking?)
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

            struct TextureNode { // a header to a texture that stores metadata in a linked list in a specified buffer
                TextureNode* next; // the next block if the data is split, nullptr if this is the whole block
                TextureNode* seqnext; // the header of the next block. TextureNodes are not stored sequentially in memory.

                int* mem;
                int vspan; // height (in pixels or whatever)
                int width; // width (in pixels or whatever)

                int hashpos; // position within the hash table

                int len; // size of the block (in pixels), can be less than vspan*width because of continued memory 
            };

            // not meant to be inherited from, but if you want this functionality without writing much code, go for it
            class MTextureBuffer : public IMemoryBuffer {
            private:
                // functions
                void aux_allocateheader(TextureNode* at = nullptr);
                TextureNode* aux_getOpenBlock(TextureNode* t = nullptr);
                TextureNode* aux_getAt(void* pos);
                TextureNode* aux_getBefore(TextureNode* t);

                // TODO: implement hash table that stores headers sequentially
                std::vector<TextureNode*> nodehash;
            public: 
                
                MTextureBuffer(const char* name, int header_size, int texture_size);
                ~MTextureBuffer();

                TextureNode* allocate(const char* filename);
                void deallocate(TextureNode* t);
                void resize(TextureNode* t);

                int CleanUp() { return 0; }

                bool operator==(IMemoryBuffer* other) { return false; }

                const MemoryToolkit& GetMemoryToolkit(int i) {
                    if (!i) { return *m_header_toolkit; }
                    else { return *m_texture_toolkit; }
                } // 0 is header, 1 is texture

            private:
                CEGUI::MEMORY::MemoryToolkit* m_header_toolkit;
                CEGUI::MEMORY::MemoryToolkit* m_texture_toolkit;

                char* m_texture_buffer;
                int m_total_texture_size;

                char* m_header_buffer;
                int m_total_header_size;

                char* m_unalloc_start_header;
                char* m_unalloc_start_texture;

                char* m_start;
                const char* m_name;

            public:

                int m_header_unallocated;
                int m_texture_unallocated;
               
            };

            extern int header_unallocated;
            extern int texture_unallocated;

            extern CEGUI::MEMORY::MemoryToolkit* mt_header;
            extern CEGUI::MEMORY::MemoryToolkit* mt;

            // when a block of memory is not enogh size  (gaps in memory being used to store new textures)
            // but there is space at another block (at the end of the texture heap or space between two other blocks)
            // texture metadata


            // Texture header
            /*struct TextureNode {
                TextureNode* next; // the next block if the data is split, nullptr if this is the whole block
                TextureNode* seqnext; // the header of the next block. TextureNodes are not stored sequentially in memory.

                int* mem;
                int vspan; // height (in pixels)
                int width; // width (in pixels)

                int hashpos; // position within the hash table

                int len; // size of the block (in pixels), can be less than vspan*width because of continued memory 
            };*/

            void INIT_TEXTURE(int overload_bytes_texture=0, int overload_bytes_header=0); // originally allocated 8 megabytes for actual textures, 1 megabytes for metadata (linked list) but can take an overload in bytes
            
            TextureNode* allocate(const char* filename);
            void deallocate(TextureNode* t);
            void resize(TextureNode* t);

            // int* aux_getOpenBlock(); returns the first open space of memory
        };
    };
};



#endif