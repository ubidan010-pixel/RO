#ifndef MEMMNG_PRIVATE_PLATFORM_NINTENDO_H_
#define MEMMNG_PRIVATE_PLATFORM_NINTENDO_H_

#pragma once

#include <stdlib.h>
#include <malloc.h>

#include "MemMng.h"

namespace ITF
{

    namespace MemMngTools
    {
        //////////////////////////////////////////////////////////////////////////
        // Atomic operations
        // 
        // return previous value...

        inline u64 AtomicAdd(volatile u64 & _variable, u64 _value)
        {
            return __atomic_add_fetch(&_variable, _value, __ATOMIC_SEQ_CST);
            //return sceAtomicAdd64(reinterpret_cast<volatile i64*>(&var), i64(_value));
        }

        inline u64 AtomicSub(volatile u64 & _variable, u64 _value)
        {
            return __atomic_sub_fetch(&_variable, _value, __ATOMIC_SEQ_CST);
            //return sceAtomicAdd64(reinterpret_cast<volatile i64*>(&var), -i64(_value));
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

    namespace MemMngLowLevelAlloc
    {
        inline void Init()
        {
        }

        inline void GetRange(void * & baseAddress, void * & limitAddress)
        {
            baseAddress  = (void*) (4*1024); // just advance a bit to not get 0 address inside it
            limitAddress = (void*) (u64(-1));
        }

        inline void * Malloc(u32 iSize)
        {
            return malloc((size_t)iSize);
        }

        inline void * MallocAligned(u32 iSize, u32 uAlign)
        {
            return memalign((size_t)uAlign, (size_t)iSize);
        }

        inline void	Free(void * pData)
        {
            free(pData);

        }

        inline unsigned int MSize(void * pData)
        {
            return malloc_usable_size(pData);
        }

    } // namespace MemMngTools

} // namespace ITF

#endif // MEMMNG_PRIVATE_PLATFORM_NINTENDO_H_
