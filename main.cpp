#include "texture.h"
#include "ram.h"

#include <iostream>

// Globals.... Everywhere.... what have I done

#ifdef WIN32
#include "WinAPIComponent.h"
#endif

int main() {
    CEGUI::MEMORY::INIT_MEM();

    CEGUI::MEMORY::TEXTURE::INIT_TEXTURE(110, 100);
    CEGUI::MEMORY::TEXTURE::allocate("text.stuff");
    CEGUI::MEMORY::TEXTURE::allocate("aaaahhhhh.amogus");

    std::cout << "\n";
    CEGUI::MEMORY::TEXTURE::mt->doit();

    CEGUI::MEMORY::END();

	return 0;
}