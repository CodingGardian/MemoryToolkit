#ifndef VARIABLEARRAY_H
#define VARIABLEARRAY_H

namespace CEGUI {
	namespace MEMORY {
		// an array that increases by a factor of GrowthFactor
		template<typename EntryType, int GrowthFactor>
		class VariableSizeArray {
		public:
			VariableSizeArray();
			~VariableSizeArray();



			// implement operators =, move contructors, etc.


			EntryType AddEntry(EntryType e);
			

		private:

		};
	};
};



#endif