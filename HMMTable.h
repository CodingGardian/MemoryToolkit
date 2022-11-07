#ifndef HMMTABLE_H
#define HMMTABLE_H

namespace CEGUI {
		namepsace MEMORY {
					
					class HMMTable_Buffer_S : public IMemoryBuffer { // _S means singleton
					public:
							~HMMTable_Buffer_S();


			
					private:
							HMMTable_Buffer_S();

			
					};



			
					// Hanle Mapped Memory Table (keeps an entry of keys and maps them to distinct entries stored in memory)
					// template<typename tHANDLE = unsigned int> do I really need this????
					class HMMTable {
					public:
							
					private:
						
					}
		};



};

#endif