#include "texture.h"
#include "MemoryPool.h"

#include <iostream>
#include <fstream>
#include <chrono>

// Globals.... Everywhere.... what have I done

//run = "g++ -g main.cpp ram.cpp texture.cpp MemoryPool.cpp -o debugmain; gdb debugmain;" debugging

#ifdef WIN32
//#include "WinAPIComponent.h"
#endif

struct MemoryPoolTest {
	int x[10];
	float y[20];
};


int main() {
	using namespace CEGUI::MEMORY;

	using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;

	MemoryPool p(312);

	/*auto t1new = high_resolution_clock::now();
	MemoryPoolTest* x1 = new MemoryPoolTest;
	auto t2new = high_resolution_clock::now();

	duration<double, std::milli> newalloctime = t2new - t1new;

	auto t1malloc = high_resolution_clock::now();
	MemoryPoolTest* x2 = (MemoryPoolTest*)malloc(sizeof(MemoryPoolTest));
	auto t2malloc = high_resolution_clock::now();

	duration<double, std::milli> malloctime = t2malloc - t1malloc;
	
	auto t1pool = high_resolution_clock::now();
	MemoryPoolTest* test1 = (MemoryPoolTest*)p.allocate(sizeof(MemoryPoolTest));
	auto t2pool = high_resolution_clock::now();

	duration<double, std::milli> poolalloctime = t2pool - t1pool;

	std::cout << "allocate with new time: " << newalloctime.count() << "\n";

	std::cout << "allocate with malloc time: " << malloctime.count() << "\n";

	std::cout << "allocate with pool time: " << poolalloctime.count() << "\n";
	*/
	
	MemoryPoolTest* test1 = (MemoryPoolTest*)p.allocate(sizeof(MemoryPoolTest));
	MemoryPoolTest* test2 = (MemoryPoolTest*)p.allocate(sizeof(MemoryPoolTest));
	//MemoryPoolTest* test3 = (MemoryPoolTest*)p.allocate(sizeof(MemoryPoolTest));
	for (int i=0; i<10; i++) {
		test1->x[i] = 34843579;
		test1->y[i] = 23042.2349334;
	}
	p.PrintMemory();
	p.CheckMemory();

	p.deallocate(test1);
	p.PrintMemory();
	p.CheckMemory();

	int* testint = (int*)p.allocate(sizeof(int));
	p.PrintMemory();
	p.CheckMemory();
	
  	/*CEGUI::MEMORY::TEXTURE::MTextureBuffer temp_texture("Test", 10, 2500);

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
		std::cout << "\n";*/

		
	return 0;
}