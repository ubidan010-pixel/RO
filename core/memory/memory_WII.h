#ifndef __MEMORY_WII_H__
#define __MEMORY_WII_H__

#include "core/memory/WII/MEM_macros_WII.h"

#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_
//#define FORCE_ALIGN 0
namespace ITF
{
#if !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)
    struct CMemStats
    {
        CMemStats()
        {
            for(int i = 0; i < MemoryId::mId_Max; ++i)
            {
                statsId[i].totcount = 0;
                statsId[i].alloc = 0;
                statsId[i].breakAtCount = 0;
            }
        }

        struct IdStats
        {
            u32 totcount;
            u32 alloc;
            u32 breakAtCount;
        };

        IdStats statsId[MemoryId::mId_Max];
    };
    extern CMemStats g_dbgMemStats;
#endif // !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)

    extern u32 ComputeMemFlagStrategie(size_t _size, MemoryId::ITF_ALLOCATOR_IDS _id);
	inline void* defaultMallocCategory(size_t _size, MemoryId::ITF_ALLOCATOR_IDS _id)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		INIT_ALLOCATION
#ifdef FORCE_ALIGN
		return ADD_ALLOCATION_ALIGN(MEM_M_AllocAlignExt(GET_OVERHEADSIZE_ALIGN(_size,FORCE_ALIGN), FORCE_ALIGN,ComputeMemFlagStrategie(_size,_id)), _size, FORCE_ALIGN, _id);
#else
		return ADD_ALLOCATION(MEM_M_AllocExt(GET_OVERHEADSIZE(_size), ComputeMemFlagStrategie(_size,_id)), _size, _id);		
#endif
#else // ITF_CATEGORY_MEMORY_ENABLE
        return MEM_M_AllocExt(_size, ComputeMemFlagStrategie(_size,_id));
#endif // ITF_CATEGORY_MEMORY_ENABLE        
	}

	inline void* wii_Realloc(void* _ptr, size_t _size)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		INIT_ALLOCATION
        void* realPtr = REMOVE_ALLOCATION(_ptr);
#ifdef FORCE_ALIGN 
        MemoryId::ITF_ALLOCATOR_IDS id = (MemoryId::ITF_ALLOCATOR_IDS)(((HeaderMem*)realPtr)->m_attribute.dwAllocatorId);
		return ADD_ALLOCATION_ALIGN(MEM_M_ReallocAlign(realPtr, GET_OVERHEADSIZE_ALIGN(_size,FORCE_ALIGN), FORCE_ALIGN), _size, FORCE_ALIGN, id);
#else
        MemoryId::ITF_ALLOCATOR_IDS id = (MemoryId::ITF_ALLOCATOR_IDS)(((HeaderMem*)realPtr)->m_attribute.dwAllocatorId);
		return ADD_ALLOCATION(MEM_M_Realloc(realPtr, GET_OVERHEADSIZE(_size)), _size, id);
#endif

#else // ITF_CATEGORY_MEMORY_ENABLE
        return MEM_M_Realloc(_ptr, _size);
#endif // ITF_CATEGORY_MEMORY_ENABLE
	}

	inline void* wii_AlignedMallocCategory(size_t _size, size_t _align, MemoryId::ITF_ALLOCATOR_IDS _id)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		INIT_ALLOCATION
		return ADD_ALLOCATION_ALIGN(MEM_M_AllocAlignExt(GET_OVERHEADSIZE_ALIGN(_size,_align), _align,ComputeMemFlagStrategie(_size,_id)), _size, _align, _id);
#else // ITF_CATEGORY_MEMORY_ENABLE
		return MEM_M_AllocAlignExt(_size, _align,ComputeMemFlagStrategie(_size,_id));
#endif // ITF_CATEGORY_MEMORY_ENABLE
	}

	inline void* wii_AlignedMalloc(size_t _size, size_t _align)
	{
        return wii_AlignedMallocCategory(_size, _align, MemoryId::mId_Unknow);
	}

	inline void* wii_AlignedRealloc(void* _ptr, size_t _size, size_t _align)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		INIT_ALLOCATION
        void* realPtr = REMOVE_ALLOCATION(_ptr);
        MemoryId::ITF_ALLOCATOR_IDS id = (MemoryId::ITF_ALLOCATOR_IDS)(((HeaderMem*)realPtr)->m_attribute.dwAllocatorId);
		return ADD_ALLOCATION_ALIGN(MEM_M_ReallocAlign(realPtr, GET_OVERHEADSIZE_ALIGN(_size,_align), _align), _size, _align, id);
#else // ITF_CATEGORY_MEMORY_ENABLE
		return MEM_M_ReallocAlign(_ptr, _size, _align);
#endif // ITF_CATEGORY_MEMORY_ENABLE
	}

	inline void wii_AlignedFree(void* _ptr)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		MEM_M_FreeAlign(REMOVE_ALLOCATION(_ptr));
#else // ITF_CATEGORY_MEMORY_ENABLE
		MEM_M_FreeAlign(_ptr);
#endif // ITF_CATEGORY_MEMORY_ENABLE
	}


	inline void* defaultMalloc(size_t _size)
	{
        return defaultMallocCategory(_size, MemoryId::mId_Unknow);
	}

    inline void* defaultRealloc(void* ptr, size_t size)
    {
        return wii_Realloc(ptr, size);
    }

	static void defaultFree(void* _ptr)
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
#ifdef FORCE_ALIGN
		MEM_M_FreeAlign(REMOVE_ALLOCATION(_ptr));
#else
		MEM_M_Free(REMOVE_ALLOCATION(_ptr));
#endif
#else // ITF_CATEGORY_MEMORY_ENABLE
        MEM_M_Free(_ptr);
#endif // ITF_CATEGORY_MEMORY_ENABLE
	}


    static size_t defaultSize(void* _ptr)
    {
        return MEM_M_Size(_ptr);
    }

	//----

	inline void CreateMemoryManager()
	{
		MEM_M_Create();

		Memory::setReallocFunction(wii_Realloc);
		Memory::setAlignedMallocFunction(wii_AlignedMalloc);
		Memory::setAlignedMallocCategoryFunction(wii_AlignedMallocCategory);
		Memory::setAlignedReallocFunction(wii_AlignedRealloc);
		Memory::setAlignedFreeFunction(wii_AlignedFree);
	}

	inline void DestroyMemoryManager()
	{
		MEM_M_Destroy();
	}
}

#include "Memory_Align.inl"

#endif // __MEMORY_WII_H__
