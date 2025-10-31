#ifndef MEMORY_PS3_H_
#define MEMORY_PS3_H_

#define ITF_USE_CUTOM_PS3_MEMORY_MANAGER

#include "PS3/MemMng_PS3.h"

namespace ITF
{
    inline void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return ::malloc(size);
    }

    inline void* defaultMalloc(size_t size)
    {
       return ::malloc(size);
    }

    inline void* defaultRealloc(void* ptr, size_t size)
    {
        return ::realloc(ptr, size);
    }

    inline void defaultFree(void* ptr)
    {
        return ::free(ptr);
    }

    inline size_t defaultSize(void* ptr)
    {
        return ::malloc_usable_size(ptr);
    }

    inline void* defaultAlignedMalloc(size_t size, size_t align)
    {
        return ::memalign(align, size);
    }

    inline void* defaultAlignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return ::memalign(align, size);
    }

    inline void defaultAlignedFree(void* ptr)
    {
        defaultFree(ptr);
    }

    inline void* defaultAlignedRealloc(void* ptr, size_t size, size_t align)
    {
        return ::reallocalign(ptr, size, align);
    }

    inline size_t defaultAlignedSize(void* ptr)
    {
        return defaultSize(ptr);
    }

#ifdef ITF_USE_CUTOM_PS3_MEMORY_MANAGER

    // insulate the memmng from the category enum
    inline void * MallocCategory(size_t size, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return MemMng::MallocCategory(size, u32(id));
    }

    inline void * AlignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return MemMng::AlignedMallocCategory(size, align, u32(id));
    }

    inline void CreateMemoryManager()
    {
        Memory::setMallocFunction(MemMng::Malloc);
        Memory::setMallocCategoryFunction(MallocCategory);
        Memory::setReallocFunction(MemMng::Realloc);
        Memory::setSizeFunction(MemMng::Size);
        Memory::setFreeFunction(MemMng::Free);

        Memory::setAlignedMallocFunction(MemMng::AlignedMalloc);
        Memory::setAlignedMallocCategoryFunction(AlignedMallocCategory);
        Memory::setAlignedReallocFunction(MemMng::AlignedRealloc);
        Memory::setAlignedFreeFunction(MemMng::AlignedFree);
        Memory::setAlignedSizeFunction(MemMng::AlignedSize);
        MemMng::Init();
    }

    inline void DestroyMemoryManager()
    {
        // Do nothing
    }
#endif
}

#endif // MEMORY_PS3_H_
