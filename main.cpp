#include "texture.h"

#include <iostream>

// Globals.... Everywhere.... what have I done

#ifdef WIN32
//#include "WinAPIComponent.h"
#endif


int main() {
    std::cout << "Hello World" << std::endl;
    CEGUI::MEMORY::INIT_MEM();

    CEGUI::MEMORY::TEXTURE::MTextureBuffer temp_texture("Test", 5, 100);

    temp_texture.allocate("text.stuff");
    //temp_texture.allocate("aaaahhhhh.amogus");
    /*CEGUI::MEMORY::TEXTURE::INIT_TEXTURE(110, 100);
    CEGUI::MEMORY::TEXTURE::allocate("text.stuff");
    CEGUI::MEMORY::TEXTURE::allocate("aaaahhhhh.amogus");

    std::cout << "\n";
    CEGUI::MEMORY::TEXTURE::mt->doit();
*/
    std::cout << '\n' << std::endl;
    temp_texture.GetHeaderToolkit().doit();



    CEGUI::MEMORY::END();

	return 0;
}