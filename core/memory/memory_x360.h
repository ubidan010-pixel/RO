#ifndef MEMORY_X360_H_
#define MEMORY_X360_H_

#ifdef ENABLE_ALLOCATOR
#include "core/memory/allocator/allocator.h"
#include "core/memory/allocator/allocator_macros.h"

namespace ITF
{
    inline void* defaultMallocCategory(size_t _size, MemoryId::ITF_ALLOCATOR_IDS _id)
    {
        return (ALLOCATOR_Available() ? ITF_MEM_Alloc(_size) : malloc(_size));
    }

    inline void* x360_Realloc(void* _ptr, size_t _size)
    {
        return (ALLOCATOR_Available() ? ITF_MEM_Realloc(_ptr, _size) : realloc(_ptr,_size));
    }

    inline void* x360_AlignedMallocCategory(size_t _size, size_t _align, MemoryId::ITF_ALLOCATOR_IDS _id)
    {
        ITF_ASSERT(ALLOCATOR_Available());
        return ITF_MEM_AllocAlign(_size, _align);
    }

    inline void* x360_AlignedMalloc(size_t _size, size_t _align)
    {
        ITF_ASSERT(ALLOCATOR_Available());
        return x360_AlignedMallocCategory(_size, _align, MemoryId::mId_Unknow);
    }

    inline void* x360_AlignedRealloc(void* _ptr, size_t _size, size_t _align)
    {
        ITF_ASSERT(ALLOCATOR_Available());
        return ITF_MEM_ReallocAlign(_ptr, _size, _align);
    }

    inline void x360_AlignedFree(void* _ptr)
    {
        ITF_ASSERT(ALLOCATOR_Available());
        ITF_MEM_Free(_ptr);
    }

    inline void* defaultMalloc(size_t _size)
    {
        return (ALLOCATOR_Available() ? defaultMallocCategory(_size, MemoryId::mId_Unknow) : malloc(_size));
    }

    inline void* defaultRealloc(void* ptr, size_t size)
    {
        return (ALLOCATOR_Available() ? x360_Realloc(ptr, size) : realloc(ptr,size));
    }

    inline void defaultFree(void* _ptr)
    {
        if (ALLOCATOR_Available())
            ITF_MEM_Free(_ptr);
        else
            free(_ptr);
    }
    

    inline size_t defaultSize(void* _ptr)
    {
        return ITF_MEM_GetSize(_ptr);
    }

    inline void CreateMemoryManager()
    {

        ITF_MEM_Create();

        Memory::setReallocFunction(x360_Realloc);
        Memory::setAlignedMallocFunction(x360_AlignedMalloc);
        Memory::setAlignedMallocCategoryFunction(x360_AlignedMallocCategory);
        Memory::setAlignedReallocFunction(x360_AlignedRealloc);
        Memory::setAlignedFreeFunction(x360_AlignedFree);


        
    }

    inline void DestroyMemoryManager()
    {
        // Do nothing
    }
}

#include "Memory_Align.inl"

#else

#include "memory_x360.inl"

#endif //ENABLE_ALLOCATOR

#endif // MEMORY_X360_H_

