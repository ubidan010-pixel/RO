
// default version of the memory aligned functions, adding a header with size and offset

namespace ITF
{

#define ITF_IS_POWER_OF_2(n) (((n) & ((n)-1)) == 0)
#define ITF_PREVIOUS_MULTIPLE_OF_M(n, m) ((n) & ~((m)-1))
#define ITF_MIN(n, m) ((n) < (m) ? (n) : (m))

    inline void* defaultAlignedMalloc(size_t size, size_t align)
    {
        if (!ITF_IS_POWER_OF_2(align))
        {	
            // alignement must be power of 2
            ITF_ASSERT(ITF_IS_POWER_OF_2(align));
            return NULL;
        }

        uPtr padding = align -1 + sizeof(void*) + sizeof(size_t);
        uPtr block = reinterpret_cast<uPtr>(Memory::malloc(padding + size));
        if (!block)
        {
            return NULL;
        }

        uPtr data = ITF_PREVIOUS_MULTIPLE_OF_M(block + padding, align);

        // store the original block address before the data
        uPtr* header = reinterpret_cast<uPtr*>(data - sizeof(void*));
        *header = block;

        // store the size of the original block before the block address (to know size at realloc)
        header = reinterpret_cast<uPtr*>(data - sizeof(void*) - sizeof(size_t));
        *header = size;

        return reinterpret_cast<void*>(data);
    }


    inline void* defaultAlignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id)
    {
        if (!ITF_IS_POWER_OF_2(align))
        {	
            // alignement must be power of 2
            ITF_ASSERT(ITF_IS_POWER_OF_2(align));
            return NULL;
        }

        uPtr padding = align -1 + sizeof(void*) + sizeof(size_t);
        uPtr block = reinterpret_cast<uPtr>(Memory::mallocCategory(padding + size, id));
        if (!block)
        {
            return NULL;
        }

        uPtr data = ITF_PREVIOUS_MULTIPLE_OF_M(block + padding, align);

        // store the original block address before the data
        uPtr* header = reinterpret_cast<uPtr*>(data - sizeof(void*));
        *header = block;

        // store the size of the original block before the block address (to know size at realloc)
        header = reinterpret_cast<uPtr*>(data - sizeof(void*) - sizeof(size_t));
        *header = size;

        return reinterpret_cast<void*>(data);
    }

    inline void defaultAlignedFree(void* ptr)
    {
		if (!ptr) return;

        ITF_MemoryBarrier(); // be sure to get the header stuff up to date (in case of alloc and free on different thread)
        uPtr data = reinterpret_cast<uPtr>(ptr);
        uPtr* header =  reinterpret_cast<uPtr*>(data - sizeof(void*));
        void* block = reinterpret_cast<void*>(*header);
        Memory::free(block);
    }

    inline void* defaultAlignedRealloc(void* ptr, size_t size, size_t align)
    {
        size_t oldSize;
        void* data = defaultAlignedMalloc(size, align);
        ITF_Memcpy(&oldSize, (char*)ptr - sizeof(void*) - sizeof(size_t), sizeof(size_t));
        ITF_Memcpy(data, ptr, ITF_MIN(size, oldSize));
        defaultAlignedFree(ptr);
        return data;
    }

    inline size_t defaultAlignedSize(void* ptr)
    {
        ITF_MemoryBarrier();
        uPtr data = reinterpret_cast<uPtr>(ptr);
        uPtr* header =  reinterpret_cast<uPtr*>(data - sizeof(void*));
        void* block = reinterpret_cast<void*>(*header);
        return defaultSize(block);
    }
}
