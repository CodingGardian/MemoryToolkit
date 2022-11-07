#ifndef DEFAULTALLOCATOR_H
#define DEFAULTALLOCATOR_H

// varyants maybe? could be a good idea if different allocators need different stuff
#define DECLARE_ALLOCATOR_TARGET()             \
		class _LOCAL_ALLOCATOR_TARGET_DATA {       \
		public:                          					 \

//#define ALLOCATOR_TARGET_PTR(handle) \
				//void* GetPtr() {return (return the ptr by accessing global handle fund)}

#define END_ALLOCATOR_TARGET };


namespace CEGUI {
	namespace MEMORY {

			template<typename DATATYPE>
			class IBaseAllocator {
			public:
				virtual DATATYPE* allocate(size_t size);
				virtual DATATYPE* deallocate(size_t size);
			private:

				
			};

			// allocates to a target that needs to hve DECLARE_ALLOCATOR_TARGET at the end of class
			template<typename DATATYPE, typename BUFFERCLASS, int SIZE>
			class IBufferAllocator : public IBaseAllocator<DATATYPE>, public BUFFERCLASS::_LOCAL_ALLOCATOR_TARGET_DATA 
			{
			public:
				
			private:
			
			}


			// creates an entry in the table of entries in the main memory block used by the application. Slow initialization and deletion
			template<typename DATATYPE, int SIZE>
			class HeavyDefaultAllocator : public IBufferAllocator {
			public:

			private:
				
			};

			// allocates in an unmamaged space in heap (wrapper for new and delete)
			template<typename DATATYPE, int SIZE>
			class NoHeaderHeapAllocator : public IBaseAllocator<DATATYPE> {
			public:
				
			private:


			};
	};
};

#endif