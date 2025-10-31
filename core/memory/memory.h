#ifndef ITF_MEMORY_H_
#define ITF_MEMORY_H_

#pragma once

// Do not include core/types.h in this file, it would lead to a circular dependency
// As a consequence, do not use the ITF types

#include <stdlib.h>
#include "core/memory/memoryId.h"

// Allocates memory on the stack. Be careful not to allocate too much memory, or it would lead to a stack overflow.
// It is freed automatically when exiting the function.
// Consider it as deprecated and bad usage.
#if defined ITF_MICROSOFT
#include <malloc.h>
#define ITF_Alloca(size) _alloca(size)
#else
#define ITF_Alloca(size) alloca(size)
#endif 

//#define USE_MEMORY_OVERRIDE
//#define USE_MEMORY_CAPTURESTACK
//#define ITF_DEBUG_MEMORY    // this is optional

namespace ITF
{

    struct ITF_AllocatorAttributes
    {
        unsigned int dwObjectType : 13;
        unsigned int dwHeapTracksAttributes : 1;
        unsigned int dwMustSucceed : 1;
        unsigned int dwFixedSize : 1;
        unsigned int dwAllocatorId : 8;
        unsigned int dwAlignment : 4;
        unsigned int dwMemoryProtect : 2;
        unsigned int dwZeroInitialize : 1;
        unsigned int dwMemoryType : 1;
    };
    using PITF_AllocatorAttributes = ITF_AllocatorAttributes*;

    struct HeaderMem
    {
        size_t m_size;
        size_t m_Ptr;

        union
        {
            ITF_AllocatorAttributes m_attribute;
            unsigned int            m_dwAttribute;
        };
    };

    struct HeaderPtr
    {
        size_t m_Ptr;
    };

    class Memory
    {

        public:
        ///
        /// Defines a function pointer type for unaligned memory allocation
        /// @param size is the size of the memory block to allocate
        /// @return a pointer to the block of allocated memory.
        ///
        using MallocFunction = void* (*) (size_t size);

        using MallocCategoryFunction = void* (*) (size_t size,MemoryId::ITF_ALLOCATOR_IDS id);

        ///
        /// Defines a function pointer type for unaligned memory reallocation
        /// @param ptr old pointer to realocate
        /// @param size is the size of the memory block to allocate
        /// @return a pointer to the block of allocated memory.
        ///
        using ReallocFunction = void* (*) (void* ptr, size_t size);

        ///
        /// Defines a function pointer type for unaligned memory deallocation
        /// @param ptr is the pointer to free
        ///
        using FreeFunction = void (*) (void* ptr);

        ///
        /// Defines a function pointer type for memory block size query
        /// @param ptr is the pointer to query size on
        /// @return the size of the block of allocated memory.
        ///
        using SizeFunction = size_t (*)(void* ptr);

        ///
        /// Defines a function pointer type for aligned memory allocation
        /// @param size is the size of the memory block to allocate
        /// @param align is the alignement of the memory block required, must be power of 2
        /// @return a pointer to the block of allocated memory.
        ///
        using AlignedMallocFunction = void* (*) (size_t size, size_t align);

        using AlignedMallocCategoryFunction = void* (*) (size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id);

        ///
        /// Defines a function pointer type for aligned memory reallocation
        /// @param ptr old pointer to realocate
        /// @param size is the size of the memory block to allocate
        /// @param align is the alignement of the memory block required, must be power of 2
        /// @return a pointer to the block of allocated memory.
        ///
        using AlignedReallocFunction = void* (*) (void* ptr, size_t size, size_t align);

        ///
        /// Defines a function pointer type for aligned memory deallocation
        /// @param ptr is the pointer to free
        ///
        using AlignedFreeFunction = void (*) (void* ptr);

        ///
        /// Defines a function pointer type for aligned memory block size query
        /// @param ptr is the pointer to query size on
        /// @return the size of the block of allocated memory.
        ///
        using AlignedSizeFunction = size_t (*) (void* ptr);


        ///
        /// Set the function used for unaligned memory allocation
        /// @param funct a pointer to the allocation function
        ///
        static void setMallocFunction(MallocFunction funct);

        static void setMallocCategoryFunction(MallocCategoryFunction funct);

        ///
        /// Set the function used for unaligned memory reallocation
        /// @param funct a pointer to the reallocation function
        ///
        static void setReallocFunction(ReallocFunction funct);

        ///
        /// Set the function used for unaligned memory deallocation
        /// @param funct a pointer to the deallocation function
        ///
        static void setFreeFunction(FreeFunction funct);

        ///
        /// Set the function used for memory block size query
        /// @param funct a pointer to the size query function
        ///
        static void setSizeFunction(SizeFunction funct);
        
        ///
        /// Set the function used for aligned memory allocation
        /// @param funct a pointer to the allocation function
        ///
        static void setAlignedMallocFunction(AlignedMallocFunction funct);

        static void setAlignedMallocCategoryFunction(AlignedMallocCategoryFunction funct);
        
        ///
        /// Set the function used for aligned memory reallocation
        /// @param funct a pointer to the reallocation function
        ///
        static void setAlignedReallocFunction(AlignedReallocFunction funct);
        
        ///
        /// Set the function used for aligned memory deallocation
        /// @param funct a pointer to the deallocation function
        ///
        static void setAlignedFreeFunction(AlignedFreeFunction funct);

        ///
        /// Set the function used for aligned block size query
        /// @param funct a pointer to the size query function
        ///
        static void setAlignedSizeFunction(AlignedSizeFunction funct);

        ///
        /// Every time a allocation function is set, the setup version increase.
        /// @return the current allocator setup version
        ///
        static int getAllocatorSetupVersion();


        ///
        /// Get the function used for unaligned memory allocation
        /// @return funct a pointer to the allocation function
        ///
        static MallocFunction getMallocFunction();

        ///
        /// Get the function used for unaligned memory reallocation
        /// @return funct a pointer to the reallocation function
        ///
        static ReallocFunction getReallocFunction();

        static  MallocCategoryFunction getMallocCategoryFunction();

        ///
        /// Get the function used for unaligned memory deallocation
        /// @return funct a pointer to the deallocation function
        ///
        static FreeFunction getFreeFunction();

        ///
        /// Get the function used for unaligned block size query
        /// @return funct a pointer to the size query function
        ///
        static SizeFunction getSizeFunction();

        ///
        /// Get the function used for aligned memory allocation
        /// @return funct a pointer to the allocation function
        ///
        static AlignedMallocFunction getAlignedMallocFunction();

        static AlignedMallocCategoryFunction getAlignedMallocCategoryFunction();
        
        ///
        /// Get the function used for aligned memory reallocation
        /// @return funct a pointer to the reallocation function
        ///
        static AlignedReallocFunction getAlignedReallocFunction();
        
        ///
        /// Get the function used for aligned memory deallocation
        /// @return funct a pointer to the deallocation function
        ///
        static AlignedFreeFunction getAlignedFreeFunction();

        ///
        /// Get the function used for aligned block size query
        /// @return funct a pointer to the size query function
        ///
        static AlignedSizeFunction getAlignedSizeFunction();

        ///
        /// allocate unaligned memory block
        /// @param size size of memory block to allocate
        /// @return pointer to the allocated memory
        ///
        static void* malloc(size_t size);

        static void* mallocCategory(size_t size,MemoryId::ITF_ALLOCATOR_IDS);

        ///
        /// Helper function to allocate memory already typecasted in your specified type
        ///
        template <class T> static T* typedMalloc(size_t size);

        ///
        /// reallocate unaligned memory block
        /// @param ptr pointer to the memory block to reallocate
        /// @param size size of memory block to allocate
        /// @return pointer to the reallocated memory
        ///
        static void* realloc(void* ptr, size_t size);

        ///
        /// free unaligned memory block
        /// @param pointer to the memory block to deallocate
        ///
        static void free(void* ptr);

        ///
        /// query the size of a memory block
        /// @param pointer to the memory block to get size on
        ///
        static size_t size(void* ptr);

        ///
        /// allocate aligned memory block
        /// @param size size of memory block to allocate
        /// @param align is the alignment of the memory block required, must be power of 2
        /// @return pointer to the allocated memory
        ///
        static void* alignedMalloc(size_t size, size_t align);

        static void* alignedMallocCategory(size_t size, size_t align, MemoryId::ITF_ALLOCATOR_IDS id);

        ///
        /// reallocate aligned memory block
        /// @param ptr pointer to the memory block to reallocate
        /// @param size size of memory block to allocate
        /// @param align is the alignment of the memory block required, must be power of 2
        /// @return pointer to the reallocated memory
        ///
        static void* alignedRealloc(void* ptr, size_t size, size_t align);

        ///
        /// free aligned memory block
        /// @param pointer to the memory block to deallocate
        ///
        static void alignedFree(void* ptr);

        ///
        /// query the size of an aligned memory block
        /// @param pointer to the memory block to get size on
        ///
        static size_t alignedSize(void* ptr);

    protected:
        static MallocFunction m_mallocFunction;
        static MallocCategoryFunction m_mallocCategoryFunction;
        static ReallocFunction m_reallocFunction;
        static FreeFunction m_freeFunction;
        static SizeFunction m_sizeFunction;
        static AlignedMallocFunction m_alignedMallocFunction;
        static AlignedMallocCategoryFunction m_alignedMallocCategoryFunction;
        static AlignedReallocFunction m_alignedReallocFunction;
        static AlignedFreeFunction m_alignedFreeFunction;
        static AlignedSizeFunction m_alignedSizeFunction;
        static int m_allocatorSetupVersion;
    };

    ///
    /// Helper function to allocate memory already typecasted in your specified type
    ///
    template <class T> T* Memory::typedMalloc(size_t size)
    {
        return static_cast<T*>(Memory::malloc(sizeof(T) * size));
    }

    ///
    /// A macro to define class specific new and delete operators.
    ///
    #define ITF_DECLARE_NEW_AND_DELETE_OPERATORS                \
            inline void* operator new(size_t size)                \
            {                                                    \
                return ITF::Memory::malloc(size);               \
            }                                                    \
                                                                \
            inline void operator delete(void* ptr)                \
            {                                                    \
                if (ptr)                                        \
                {                                               \
                    ITF::Memory::free(ptr);                     \
                }                                               \
            }                                                    \
                                                                \
            inline void* operator new[](size_t size)            \
            {                                                    \
                return ITF::Memory::malloc(size);                \
            }                                                    \
                                                                \
            inline void operator delete[](void* ptr)            \
            {                                                    \
                if (ptr)                                        \
                {                                               \
                    ITF::Memory::free(ptr);                        \
                }                                               \
            }                                                    \
                                                                \
            inline void* operator new(size_t, void* ptr)        \
            {                                                    \
                return ptr;                                        \
            }                                                    \
                                                                \
            inline void operator delete(void*, void*)            \
            {                                                    \
            }                                                    \
                                                                \
            inline void* operator new[](size_t,    void* ptr)        \
            {                                                    \
                return ptr;                                        \
            }                                                    \
                                                                \
            inline void operator delete[](void*, void*)            \
            {                                                    \
            }


#define ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(idAllocator)               \
            inline void* operator new(size_t size)                                \
            {                                                                    \
                return ITF::Memory::mallocCategory(size,MemoryId::idAllocator); \
            }                                                                    \
            \
            inline void operator delete(void* ptr)                \
            {                                                    \
                if (ptr)                                        \
                {                                               \
                    ITF::Memory::free(ptr);                     \
                }                                               \
            }                                                    \
            \
            inline void* operator new[](size_t size)                            \
            {                                                                    \
                return ITF::Memory::mallocCategory(size,MemoryId::idAllocator); \
            }                                                                    \
            \
            inline void operator delete[](void* ptr)            \
            {                                                    \
                if (ptr)                                        \
                {                                               \
                    ITF::Memory::free(ptr);                        \
                }                                               \
            }                                                    \
            \
            inline void* operator new(size_t size, MemoryId::ITF_ALLOCATOR_IDS )    \
            {                                                                       \
                return ITF::Memory::mallocCategory(size, MemoryId::idAllocator);    \
            }                                                                       \
            \
            inline void operator delete(void* ptr, MemoryId::ITF_ALLOCATOR_IDS )    \
            {                                                                       \
                if (ptr)                                                            \
                {                                                                   \
                    ITF::Memory::free(ptr);                                            \
                }                                                                   \
            }                                                                       \
            \
            inline void* operator new(size_t, void* ptr)        \
            {                                                    \
                return ptr;                                        \
            }                                                    \
            \
            inline void operator delete(void*, void*)            \
            {                                                    \
            }                                                    \
            \
            inline void* operator new[](size_t,    void* ptr)        \
            {                                                    \
                return ptr;                                        \
            }                                                    \
            \
            inline void operator delete[](void*, void*)            \
            {                                                    \
            }
} // namespace ITF

#ifndef ITF_FINAL
#define new_array(type, count, category) new (MemoryId::category) type[count]
#define newAlloc(category, expression)  new (ITF::MemoryId::category) expression
#else
#define new_array(type, count, category) new type[count]
#define newAlloc(category, expression)  new expression
#endif //ITF_X360


#define ITF_NEW(type) new type
#define ITF_DELETE(ptr) delete ptr
#define ITF_DELETE_ARRAY(ptr) delete[] ptr

void* operator new(size_t size, ITF::MemoryId::ITF_ALLOCATOR_IDS allocId);
void operator delete(void* ptr, ITF::MemoryId::ITF_ALLOCATOR_IDS allocId);

void* operator new[](size_t size, ITF::MemoryId::ITF_ALLOCATOR_IDS allocId);
void operator delete[](void* ptr, ITF::MemoryId::ITF_ALLOCATOR_IDS allocId);

#endif // ITF_MEMORY_H_
