#ifndef MEMORY_NINTENDO_H_
#define MEMORY_NINTENDO_H_

#pragma once

#include <stdlib.h>

// Need malloc.h for malloc_usable_size
#include <malloc.h>

#include "MemMng/MemMng.h"



namespace ITF
{
    inline void* defaultMallocCategory(size_t size, MemoryId::ITF_ALLOCATOR_IDS id)
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
        // Simulate a _aligned_realloc function
        void* newPtr = ::memalign(align, size);
        if (newPtr != nullptr && ptr != nullptr)
        {
            size_t prevSize = ::malloc_usable_size(ptr);
            std::memcpy(newPtr, ptr, std::min(prevSize, size));
            ::free(ptr);
        }
        return newPtr;
    }

    inline size_t defaultAlignedSize(void* ptr)
    {
        return defaultSize(ptr);
    }

    // insulate the memmng from the category enum
    inline void* MallocCategory(size_t size, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return MemMng::MallocCategory(size, id);
    }

    inline void* AlignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return MemMng::AlignedMallocCategory(size, align, id);
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
}

#endif // MEMORY_NINTENDO_H_
