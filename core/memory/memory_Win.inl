#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_CRC_H_
#include "core/crc.h"
#endif //_ITF_CRC_H_

namespace ITF
{

#ifdef ITF_MEMORY_PATTERNS
    // values studied for float ->
    // Free  = -1.48513959e+029 (Big Negative)
    // Alloc = 3.39615136e+038(Big Positive)
    #define	ITF_FREEMEM_PATTERN		0xfe		// Used to tag free memory
    #define	ITF_ALLOCMEM_PATTERN	0x7f		// Used to tag newly allocated memory
    #define ITF_FILL_PATTERN_ALLOC(pBlock, nSize)	memset((pBlock), ITF_ALLOCMEM_PATTERN, (nSize))
    #define ITF_FILL_PATTERN_FREE(pBlock, nSize)	memset((pBlock), ITF_FREEMEM_PATTERN, (nSize))
#else
    #define ITF_FILL_PATTERN_ALLOC(pBlock, nSize)	((void)0)
    #define ITF_FILL_PATTERN_FREE(pBlock, nSize)	((void)0)
#endif

#ifdef ITF_CATEGORY_MEMORY_ENABLE
    void* defaultMallocCategoryTracking(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
		INIT_ALLOCATION
        uPtr newsize = GET_OVERHEADSIZE(size);
        void* ptrRealAlloc = ::malloc(newsize);
        ITF_FILL_PATTERN_ALLOC(ptrRealAlloc, newsize);
     	void* ptr = ADD_ALLOCATION(ptrRealAlloc,size,id);

#ifdef ITF_TRACKING_MEMORY_ENABLE
        if (threadAllocatorTracker::getInstance() && threadAllocatorTracker::getInstance()->isEnable())
        {
            enum { FRAMETOSKIP = 0};

			ITF_AllocatorAttributes attribute = {0};
			attribute.dwAllocatorId = id;

            TraceInfo info;
            info.m_attributes =  *( u32* )& attribute;

            ITF_MemSet(info.m_stack,0,sizeof(info.m_stack));
            threadAllocatorTracker::getInstance()->getStackWalker().retrieveStackFast(FRAMETOSKIP, &info.m_stack[0],threadAllocatorTracker::MAX_STACK_DEPTH);

            u_long  crc = CRC::compute32((u_char*)&info.m_stack,sizeof(info.m_stack));    

            threadAllocatorTracker::getInstance()->addAllocation(attribute,size,(ITF::uPtr)ptr,crc,info);
        }
#endif //ITF_TRACKING_MEMORY_ENABLE
        return ptr;
    }

	void* defaultMallocTracking(size_t size)
	{
		return defaultMallocCategoryTracking(size,MemoryId::mId_Unknow);
	}

	void defaultFreeTracking(void* ptr)
    {
     	void* realPtr = REMOVE_ALLOCATION(ptr);

#ifdef ITF_TRACKING_MEMORY_ENABLE
        if (threadAllocatorTracker::getInstance() && threadAllocatorTracker::getInstance()->isEnable())
            threadAllocatorTracker::getInstance()->removeAllocation((ITF::uPtr)ptr,0);
#endif //ITF_TRACKING_MEMORY_ENABLE
        ITF_FILL_PATTERN_FREE((void*)realPtr, _msize(realPtr));
		::free((void*)realPtr);

	}

	void* defaultReallocCategoryTracking(void* ptr, size_t size)
	{
		if (ptr!=0) 
		{

			uPtr realOldPtr = (uPtr) GET_HEADER_FROM_USER_PTR(ptr);
			HeaderMem* oldHeader = (HeaderMem*)realOldPtr;
			ITF_ASSERT(oldHeader->m_Ptr==(uPtr)ptr);

			u32 AllocatorId		= oldHeader->m_attribute.dwAllocatorId;
			uPtr m_oldSize		= oldHeader->m_size;

			void* newPtr = defaultMallocCategoryTracking(size,(MemoryId::ITF_ALLOCATOR_IDS) AllocatorId);

			ITF_Memcpy(newPtr,ptr, std::min(static_cast<size_t>(m_oldSize), size));

			defaultFreeTracking(ptr);
			return newPtr;
		}

		
		return defaultMallocCategoryTracking(size,MemoryId::mId_Unknow);
	}

#endif //ITF_CATEGORY_MEMORY_ENABLE

    ///---------------------------------------------------------------------------------------------------------------------

    void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
#ifdef ITF_TRACKING_MEMORY_ENABLE
        if (threadAllocatorTracker::getInstance() && threadAllocatorTracker::getInstance()->isEnable())
        {
            Memory::MallocCategoryFunction m_mallocCat = Memory::getMallocCategoryFunction();
            return m_mallocCat(size,id);
        }
#endif //ITF_TRACKING_MEMORY_ENABLE

        void* ptr = ::malloc(size);
        ITF_FILL_PATTERN_ALLOC(ptr, size);
        return (ptr);
    }


    void* defaultMalloc(size_t size)
    {
        return defaultMallocCategory(size,MemoryId::mId_Unknow);
    }

    void* defaultRealloc(void* ptr, size_t size)
    {
        return ::realloc(ptr, size);
    }

    void defaultFree(void* ptr)
    {
        ITF_FILL_PATTERN_FREE(ptr, _msize(ptr));
       ::free(ptr);
    }


    size_t defaultSize(void* ptr)
    {
        return ::_msize(ptr);
    }
}

#include "Memory_Align.inl"

