#include "texture.h"

#include <iostream>

// Globals.... Everywhere.... what have I done

#ifdef WIN32
//#include "WinAPIComponent.h"
#endif


int main() {
    CEGUI::MEMORY::INIT_MEM();

    CEGUI::MEMORY::TEXTURE::MTextureBuffer temp_texture("Test", 100, 300);

		temp_texture.allocate("text.stuff");
	  temp_texture.GetHeaderToolkit().dump();
		std::cout << '\n';
		temp_texture.GetTextureToolkit().dump();

		std::cout << '\n';
    temp_texture.allocate("aaaahhhhh.amogus");
		temp_texture.GetHeaderToolkit().dump();
		std::cout << '\n';
		temp_texture.GetTextureToolkit().dump();

    CEGUI::MEMORY::END();

	return 0;
}