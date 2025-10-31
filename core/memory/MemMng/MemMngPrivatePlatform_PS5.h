#ifndef MEMMNG_PRIVATE_PLATFORM_PS5_H_
#define MEMMNG_PRIVATE_PLATFORM_PS5_H_

#pragma once

#include <stdlib.h>
#include <sce_atomic.h>

#include "MemMng.h"

namespace ITF
{

namespace MemMngTools
{
    //////////////////////////////////////////////////////////////////////////
    // Atomic operations
    // 
    // return previous value...

    inline u64 AtomicAdd(volatile u64 & var, u64 _value)
    {
        return sceAtomicAdd64(reinterpret_cast<volatile i64*>(&var), i64(_value));
    }

    inline u64 AtomicSub(volatile u64 & var, u64 _value)
    {
        return sceAtomicAdd64(reinterpret_cast<volatile i64*>(&var), -i64(_value));
    }

    inline void MemCountAtomicAdd(MemMng::MemCounts::CountDetail & count, u64 nAllocated)
    {
#if MEMMNG_MEM_COUNT == 1
        MEMMNG_AUTO_COUNT_LOCK();
        count.current += nAllocated;
        count.peak= Max(count.peak, count.current);
#else
        AtomicAdd(count.current, nAllocated);
#endif
    }

    inline void MemCountAtomicSub(MemMng::MemCounts::CountDetail & count, u64 nAllocated)
    {
#if MEMMNG_MEM_COUNT == 1
        MEMMNG_AUTO_COUNT_LOCK();
        count.current -= nAllocated;
#else
        AtomicSub(count.current, nAllocated);
#endif
    }
}

//////////////////////////////////////////////////////////////////////////
// malloc "std" wrapping

//#define WRAP_ALLOC 1
#define WRAP_ALLOC 0

#if WRAP_ALLOC
extern "C"
{
    extern void *__real_malloc (size_t size);
    extern void __real_free (void *p);
    extern void	*__real_memalign (size_t align, size_t size);
    extern int	__real_malloc_usable_size (void *data);
}
#else //#if WRAP_ALLOC
# define __real_malloc					malloc
# define __real_free					free
# define __real_memalign				memalign
# define __real_malloc_usable_size		malloc_usable_size
#endif //#if WRAP_ALLOC

namespace MemMngLowLevelAlloc
{
    inline void Init()
    {

    }

    inline void GetRange(void * & baseAddress, void * & limitAddress)
    {
        baseAddress  = (void*) (4*1024);
        limitAddress = (void*) (u64(-1));
    }

    inline void * Malloc(u32 iSize)
    {
        return __real_malloc((size_t)iSize);
    }

    inline void * MallocAligned(u32 iSize, u32 uAlign)
    {
        return __real_memalign((size_t)uAlign, (size_t)iSize);
    }

    inline void	Free(void * pData)
    {
        __real_free(pData);

    }

    inline unsigned int MSize(void * pData)
    {
        return __real_malloc_usable_size(pData);
    }

} // namespace MemMngTools

} // namespace ITF

#endif // MEMMNG_PRIVATE_PLATFORM_ORBIS_H_
