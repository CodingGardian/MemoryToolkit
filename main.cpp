#include "texture.h"

#include <iostream>
#include <fstream>

// Globals.... Everywhere.... what have I done

#ifdef WIN32
//#include "WinAPIComponent.h"
#endif


int main() {
		using namespace CEGUI::MEMORY::TEXTURE;
	
    CEGUI::MEMORY::INIT_MEM();

    CEGUI::MEMORY::TEXTURE::MTextureBuffer temp_texture("Test", 10, 2500);

    TextureNode* node1 = temp_texture.allocate("text.stuff");
   	TextureNode* node2 = temp_texture.allocate("aaaahhhhh.amogus");

		std::cout << "\n" << node1 << " " << node2->seqbefore;
	
		TextureNode* node3 = temp_texture.allocate("text.stuff");
    std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();

		std::cout << "\n";
		temp_texture.deallocate(node2);
		std::cout << node1->seqnext << node3;
	
	 	std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();
		std::cout << '\n' << std::endl;
		
		TextureNode* temp_node = temp_texture.allocate("aaaahhhhh2.amogus");

		std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();
		std::cout << "\n";
		std::cout << node3->seqnext << temp_node->next;
	
    CEGUI::MEMORY::END();

	return 0;
}