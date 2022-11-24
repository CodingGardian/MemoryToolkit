#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

namespace CEGUI {
		namespace MEMORY {
				// linked list memory pool, where headers are stored with memory (inspired by DOOM 1993 system)
				class MemoryPool {
				public:
					MemoryPool(int bytes);
					~MemoryPool();

					void* allocate(int bytes);
					void deallocate(void* ptr);

					void PrintMemory();

					void CheckMemory();
				private:
					struct header {
						header* next;
						header* before;
						int size;
						int id; // using an int to keep byte allignment
					};

					void* m_ptrStart;
					header* m_ptrStartLook;
					int m_size;

					int m_numEntries;

				};
				
		}
}

/* Old header
struct header {
						header* next; // for free blocks, next will point to the next free block. In filled blocks it does what youd expect
						header* before; // Proposed system, before will always point to an occupied block in free blocks. In occupied blocks it will point to the block right before this, filled or not
						header* freebefore; // points to the closest free block before this one
						int size;
						int __pad; // keep aligned to 8 bytes
					};

*/


#endif