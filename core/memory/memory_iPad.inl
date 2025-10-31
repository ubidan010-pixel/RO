namespace ITF
{
    void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
        return ::malloc(size);
    }


    void* defaultMalloc(size_t size)
    {
       return ::malloc(size);
    }

    void* defaultRealloc(void* ptr, size_t size)
    {
        return ::realloc(ptr, size);
    }

    void defaultFree(void* ptr)
    {
        return ::free(ptr);
    }
}

#include "Memory_Align.inl"
