#ifndef CEGUIARRAY_H
#define CEGUIARRAY_H

namespace CEGUI {
		namespace MEMORY {
				namespace ARRAY {
						// Variable array doubles in size when runs out of space
						template<typename tDATATYPE, int tDATASIZE, template tALLOCATOR>
						class Varray : public IMemoryBuffer {
						public:
								VArray();
								~VArray();

								// Will use = operator. IF DATATYPE IS A CLASS, THE = OPERATOR MUST BE IMPLEMENTED AS A COPY
								DATATYPE* AddEntry(DATATYPE add);
								DATATYPE* AddEntryPtr(DATATYPE* add); 
								// will copy data at ptr, but will use SIZE passed in template arg

								


						private:
								int m_size;
								DATATYPE* m_mem;
						}

				};
		}
};




#define