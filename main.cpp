#include "texture.h"

#include <iostream>

// Globals.... Everywhere.... what have I done

#ifdef WIN32
//#include "WinAPIComponent.h"
#endif


int main() {
		using namespace CEGUI::MEMORY::TEXTURE;
    std::cout << "Hello World";
    CEGUI::MEMORY::INIT_MEM();

    CEGUI::MEMORY::TEXTURE::MTextureBuffer temp_texture("Test", 5, 250);

    temp_texture.allocate("text.stuff");
   	TextureNode* node = temp_texture.allocate("aaaahhhhh.amogus");
		temp_texture.allocate("text.stuff");
    /*CEGUI::MEMORY::TEXTURE::INIT_TEXTURE(110, 100);
    CEGUI::MEMORY::TEXTURE::allocate("text.stuff");
    CEGUI::MEMORY::TEXTURE::allocate("aaaahhhhh.amogus");

    std::cout << "\n";
    CEGUI::MEMORY::TEXTURE::mt->doit();
*/
    std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();

		temp_texture.deallocate(node);

	 	std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();


		TextureNode* temp_node = temp_texture.allocate("text.stuff");

		std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().dump();
		temp_texture.GetTextureToolkit().dump();

		std::cout '\n';
		std::cout << temp_node->
		
    CEGUI::MEMORY::END();

	return 0;
}