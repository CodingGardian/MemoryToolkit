#include "MemoryPool.h"
#include <iostream>

#define MINNEWBLOCK sizeof(header) + 4

#define FREEBLOCK 1;
#define FILLEDBLOCK 0;

// NEVER GET header->freebefore->next or header->freebefore->before UNLESS YOU ARE SURE THAT IT IS NOT NULL


CEGUI::MEMORY::MemoryPool::MemoryPool(int bytes) {
	/*m_ptrStart = new char[bytes];
	header* tempheader = (header*)m_ptrStart;
	tempheader->next = tempheader;
	tempheader->size = 0;
	
	m_ptrFreeMemoryList = tempheader+1;
	m_ptrFreeMemoryList->next = m_ptrFreeMemoryList; // temp header acts as end of list
	m_ptrFreeMemoryList->before = tempheader;
	m_ptrFreeMemoryList->size = bytes - sizeof(header)*2;
	m_ptrFreeMemoryList->freebefore = nullptr; // free blocks don't need this
	m_ptrFreeMemoryListTail = m_ptrFreeMemoryList;
	tempheader->before = m_ptrFreeMemoryList;
	tempheader->freebefore = nullptr;
	
	m_numEntries = 1;
	m_size = bytes;*/
	bytes = (bytes + 7) & ~7;
	std::cout << bytes << '\n';
	m_ptrStart = new char[bytes];
	header* temp = (header*)m_ptrStart;
	temp->size = 0;
	temp->id = FILLEDBLOCK;
	temp->next = (temp+1);
	temp->before = (temp+1);
	temp += 1;

	temp->size = bytes - sizeof(header)*2;
	temp->id = FREEBLOCK;
	temp->next = temp-1;
	temp->before = temp-1;
	
	m_ptrStartLook = temp;
	
	m_numEntries = 1;
	m_size = bytes;
	
}

CEGUI::MEMORY::MemoryPool::~MemoryPool() {
	delete[] m_ptrStart;
}

// ASSUMPTION 1: THERE WILL NEVER BE TWO OR MORE ADJACENT FREE BLOCKS
void* CEGUI::MEMORY::MemoryPool::allocate(int bytes) {
	/*header* prev, *current;
	current = m_ptrFreeMemoryList;
	prev = m_ptrFreeMemoryListTail;

	bytes = (bytes + 7) & ~7; // multiple of 8 (for byte alignment)
		
	do {
		if (current->size >= bytes) {
			break;
		}
		prev = current;
		current = current->next; // only iterating through free space, super efficient
		if (current == m_ptrFreeMemoryList) {return nullptr;} // no free space :(
	} while(true);
	
	int diff = current->size - bytes;
	
	if (diff > MINBLOCKSIZE) {
		// will be room for another header
		header* newcurrent = (header*)((unsigned char*)(current+1) + bytes);

		if (prev == current) {newcurrent->next = newcurrent;}
		else {newcurrent->next = current->next; prev->next = newcurrent;}
		
		current->size = bytes;
		
		current->next = current->before->next;
		current->before->next = current;

		
		if (current == m_ptrFreeMemoryList) {m_ptrFreeMemoryList = newcurrent; prev = nullptr;}
		if (current == m_ptrFreeMemoryListTail) {m_ptrFreeMemoryListTail = newcurrent;}

		
		current->freebefore = prev;

		newcurrent->size = diff - sizeof(header);
		newcurrent->before = current;
		
		return (void*)(current+1);
	}

	
	if (current == m_ptrFreeMemoryListTail) {m_ptrFreeMemoryListTail = prev;}

	
	if (current == m_ptrFreeMemoryList) {m_ptrFreeMemoryList = current->next; prev = nullptr;}


	
	prev->next = current->next;
	current->next = current->before->next;
	current->frebefore = prev;
	
	return (void*)(current+1);*/
	bytes = (bytes + 7) & ~7;
	header* temp = m_ptrStartLook;

	do {
		if (temp->size > bytes && temp->id) {
			break;
		}
		temp = temp->next;
		if (temp==m_ptrStartLook) {return nullptr;}
	} while(true);
	
	m_ptrStartLook = temp;
	
	int diff = temp->size - bytes;
	if (diff < MINNEWBLOCK) {
		// no new block
		temp->id = FILLEDBLOCK;
		return(void*)(temp+1);
	}

	header* newheader = (header*)((char*)temp + bytes + sizeof(header));
	newheader->next = temp->next;
	newheader->before = temp;
	
	temp->next->before = newheader;
	temp->next = newheader;
	newheader->id = FREEBLOCK;
	newheader->size = diff - sizeof(header);
	temp->size = bytes;
	temp->id = FILLEDBLOCK;

	return (void*)(temp+1);
	
}


// ASSUMPTION 1: THERE WILL NEVER BE TWO OR MORE ADJACENT FREE BLOCKS
// #1 RULE: NEVER LEAVE TWO FREE BLOCKS NEXT TO EACHOTHER, ALWAYS COMBINE THEM!!!!
void CEGUI::MEMORY::MemoryPool::deallocate(void* ptr) {
	/*header* ptrheader = (header*)(ptr) - 1;

	// TODO: FINSIH AND MAKE PRETTY AND SMALL :)
	
	// I don't like all these if's, but what can ya do :/
	
	if (m_ptrFreeMemoryList->before == ptrheader) {
		// if next is at end of list, no way there could be a node before. Delete node, combine & return
		ptrheader->size += m_ptrFreeMemoryList->size;
		//ptrheader->before = m_ptrFreeMemoryList->before;
		ptrheader->next->before = ptrheader;
		ptrheader->before->next = ptrheader->next;

		ptrheader->next = m_ptrFreeMemoryList->next;
		m_ptrFreeMemoryList = ptrheader;
		return;
	}

	if (ptrheader->before == m_ptrFreeMemoryListTail) {
		// now way there could be a free node after. Delete node & return
		m_ptrFreeMemoryListTail += ptrheader->size;
		
		
		ptrheader->next->before = m_ptrFreeMemoryListTail;
		ptrheader->freebefore = nullptr;

		m_ptrFreeMemoryListTail->before->next = ptrheader->next;
		return;
	}
	// freebefore SHOULD NOT BE NULL IF THESE IF's ARE PASSED
	
	if (ptrheader->freebefore == ptrheader->before) {
		// node before

		// Steps
		// next->before needs to = freebefore
		// filled node before ->next = next
		// don't forget to increment size
		
	}
	
	if (ptrheader->freebefore->next < ptrheader->next) {
		// node after
		
		// Steps
		// freebefore->next = ptrheader
		// filled node before ->next = next 
		// dont forget to increment size
		
	}

	// no node before or after
	// ptrheader->next = ptrheader->freebefore->next->next;
	// ptrheader->freebefore->next = ptrheader;
	// ptrheader->freebefore = nullptr;
	
	
	*/

	header* ptrheader = (header*)ptr - 1;
	if (ptrheader->id) {std::cout << "TRIED TO DOUBLE FREE BLOCK, EXITIING"; return;}
	
	if (ptrheader->before->id) {
		// free block before
		ptrheader->before->size += ptrheader->size + sizeof(header);
		ptrheader->before->next = ptrheader->next;
		ptrheader->next->before = ptrheader->before;
		
		if (m_ptrStartLook == ptrheader) {m_ptrStartLook = ptrheader->before;}
		
		ptrheader = ptrheader->before;
		
	}

	if (ptrheader->next->id) {
		// free block after
		ptrheader->size += ptrheader->next->size + sizeof(header);
		ptrheader->next = ptrheader->next->next;
		ptrheader->next->before = ptrheader;

	
	}

	ptrheader->id = FREEBLOCK;
	
}

// ASSUMPTION 1: THERE WILL NEVER BE TWO OR MORE ADJACENT FREE BLOCKS
void CEGUI::MEMORY::MemoryPool::PrintMemory() {
	header* temp = (header*)m_ptrStart;
	std::cout << "\n";
	do {
		std::cout << "ID:" << temp->id << " addr:" << temp << " before:" << temp->before << " next:" << temp->next << " size:" << temp->size << "\n";
		
		temp = temp->next;
	} while(temp != (header*)m_ptrStart);
	
}

void CEGUI::MEMORY::MemoryPool::CheckMemory() {
	header* temp = (header*)m_ptrStart;
	std::cout << "\n";
	while (temp->next != (header*)m_ptrStart) {
		header* tempcalc = (header*)((char*)(temp + 1) + temp->size);
		if (tempcalc != temp->next) {
			std::cout << "Check memory failed for " << temp << "->next!" << "\n";
		}
		temp = temp->next;
	}
	header* end = temp;
	while (temp->before != (header*)end) {
		header* tempcalc = (header*)((char*)(temp-1) - temp->before->size);
		if (tempcalc != temp->before) {
			std::cout << "Check memory failed for " << temp << "->before!" << "\n";
		}
		temp = temp->before;
	}
}