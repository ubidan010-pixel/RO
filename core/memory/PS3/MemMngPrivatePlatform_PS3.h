#ifndef MEMMNG_PRIVATE_PLATFORM_PS3_H_
#define MEMMNG_PRIVATE_PLATFORM_PS3_H_

#include <stdlib.h>
#include <cell/atomic.h>
#include <sys/fixed_addr.h>
#include "MemMng_PS3.h"

namespace ITF
{

namespace MemMngTools
{
    //////////////////////////////////////////////////////////////////////////
    // Atomic operations

	inline void AtomicAdd(u32 & var, u32 value)
	{
        cellAtomicAdd32(&var, value);
	}
	inline void AtomicAdd(int & var, int value)
	{
		cellAtomicAdd32(reinterpret_cast<u32*>(&var), u32(value));
	}

	inline void AtomicSub(u32 & var, u32 value)
	{
		cellAtomicSub32(&var, value);
	}

	inline void AtomicSub(int & var, int value)
	{
		cellAtomicSub32(reinterpret_cast<u32*>(&var), u32(value));
	}

	inline void MemCountAtomicAdd(MemMng::MemCounts::CountDetail & count, u32 nAllocated)
	{
#if MEMMNG_MEM_COUNT == 1
		MEMMNG_AUTO_COUNT_LOCK();
		count.current+= nAllocated;
		count.peak= Max(count.peak, count.current);
#else
		AtomicAdd(count.current, nAllocated);
#endif
	}

	inline void MemCountAtomicSub(MemMng::MemCounts::CountDetail & count, u32 nAllocated)
	{
#if MEMMNG_MEM_COUNT == 1
		MEMMNG_AUTO_COUNT_LOCK();
		count.current-= nAllocated;
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
        baseAddress  = (void *)(TEXT_SEGMENT_BASE_ADDR);
        limitAddress = (void *)(RSX_FB_BASE_ADDR - 1u);
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

#endif // MEMMNG_PRIVATE_PLATFORM_PS3_H_
