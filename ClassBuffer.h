// singleton buffer for IMemoryBuffers to be allocated...
// Lets get this done before everything else...
#ifndef CLASSBUFFER_H
#define CLASSBUFFER_H

// 20,000 bytes
#define CLASS_BUFFER_SIZE 20000

namespace CEGUI {
		namespace MEMORY {

				// MemoryToolkits and all classes
				class CLASS_DATA_BUFFER_S {
				public:
						~CLASS_DATA_BUFFER_S();
				private:
						ClASS_DATA_BUFFER_S();

						int m_numclasses;
						int m_freespace;
						int m_usedspace;

						void* m_startptr;

						MemoryToolkit* m_Toolkit;
						// HMMTable* table
				};

		};
};

#endif