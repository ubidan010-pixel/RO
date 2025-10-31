namespace ITF
{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
    void* defaultMallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS id)
    {
        ITF_ASSERT(id> eXALLOCAllocatorId_GameMin);
        ITF_ASSERT(id< eXALLOCAllocatorId_MiddlewareReservedMax );

        //
        DWORD dwHeapMemoryAttributes = MAKE_XALLOC_ATTRIBUTES
            (
            0,                                  // ObjectType
            1,                                  // HeapTracksAttributes
            0,                                  // MustSucceed
            0,                                  // FixedSize
            id,         // AllocatorID
            XALLOC_ALIGNMENT_DEFAULT,           // Alignment
            XALLOC_MEMPROTECT_READWRITE,        // MemoryProtection
            0,                                  // ZeroInit
            XALLOC_MEMTYPE_HEAP                 // MemoryType
            );


        void* ptr = XMemAlloc(size,dwHeapMemoryAttributes);
        XSetAttributesOnHeapAlloc(ptr,dwHeapMemoryAttributes);
        return ptr;
    }


    void* defaultMalloc(size_t size)
    {
        DWORD dwHeapMemoryAttributes = MAKE_XALLOC_ATTRIBUTES
            (
            0,                                  // ObjectType
            0,                                  // HeapTracksAttributes
            0,                                  // MustSucceed
            0,                                  // FixedSize
            MemoryId::mId_Unknow,                 // AllocatorID
            XALLOC_ALIGNMENT_DEFAULT,           // Alignment
            XALLOC_MEMPROTECT_READWRITE,        // MemoryProtection
            0,                                  // ZeroInit
            XALLOC_MEMTYPE_HEAP                 // MemoryType
            );

        return XMemAlloc(size,dwHeapMemoryAttributes);
    }

    void defaultFree(void* ptr)
    {
        return XMemFree(ptr,0);
    }

    size_t defaultSize(void* ptr)
    {
        DWORD dwHeapMemoryAttributes = MAKE_XALLOC_ATTRIBUTES
            (
            0,                                  // ObjectType
            0,                                  // HeapTracksAttributes
            0,                                  // MustSucceed
            0,                                  // FixedSize
            MemoryId::mId_Unknow,                 // AllocatorID
            XALLOC_ALIGNMENT_DEFAULT,           // Alignment
            XALLOC_MEMPROTECT_READWRITE,        // MemoryProtection
            0,                                  // ZeroInit
            XALLOC_MEMTYPE_HEAP                 // MemoryType
            );
        return XMemSize(ptr, dwHeapMemoryAttributes );
    }




    void* defaultRealloc(void* ptr, size_t size)
    {
        if (ptr!=0) 
        {
            DWORD dwAllocAttributes = XGetAttributesOnHeapAlloc(ptr);
            const XALLOC_ATTRIBUTES& AllocAttributes = *( XALLOC_ATTRIBUTES*) &dwAllocAttributes;
            u32 m_oldSize = XMemSize(ptr, 0 );
            void* newPtr = defaultMallocCategory(size,(MemoryId::ITF_ALLOCATOR_IDS) AllocAttributes.dwAllocatorId );

            ITF_Memcpy(newPtr,ptr,std::min(m_oldSize,size));

            defaultFree(ptr);
            return newPtr;
        }


        return defaultMallocCategory(size,MemoryId::mId_Unknow);
    }
#else
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

    size_t defaultSize(void* ptr)
    {
        return ::_msize(ptr);
    }

#endif  //ITF_TRACKING_MEMORY_ENABLE

}

#include "Memory_Align.inl"
