#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>

//  TODO:
//      Finish deletion function
//      Decide if hash table is a good thing std::vector
//      Get rid of globals (what the fuck was I thinking?)
//      
//

namespace CEGUI {
    namespace MEMORY {
        class MemoryToolkit;
        namespace TEXTURE {

            extern int header_unallocated;
            extern int texture_unallocated;

            extern CEGUI::MEMORY::MemoryToolkit* mt_header;
            extern CEGUI::MEMORY::MemoryToolkit* mt;

            // when a block of memory is not enogh size  (gaps in memory being used to store new textures)
            // but there is space at another block (at the end of the texture heap or space between two other blocks)
            // texture metadata

            // Texture header
            struct TextureNode {
                TextureNode* next; // the next block if the data is split, nullptr if this is the whole block
                TextureNode* seqnext; // the header of the next block. TextureNodes are not stored sequentially in memory.

                int* mem;
                int vspan; // height (in pixels)
                int width; // width (in pixels)

                int len; // size of the block (in pixels), can be less than vspan*width because of continued memory 
            };

            void INIT_TEXTURE(int overload_bytes_texture=0, int overload_bytes_header=0); // originally allocated 8 megabytes for actual textures, 1 megabytes for metadata (linked list) but can take an overload in bytes
            
            TextureNode* allocate(const char* filename);
            void deallocate(TextureNode* t);
            void resize(TextureNode* t);

            // int* aux_getOpenBlock(); returns the first open space of memory
        };
    };
};



#endif