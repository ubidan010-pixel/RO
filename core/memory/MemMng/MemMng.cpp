#include "precompiled_core.h"
#include "MemMng.h"
#include "MemMngPrivate.h"
#include "MemMngStdAlloc.h"
#include "MemMngBigAlloc.h"
#include "MemMngBuckets.h"
#include "MemMngBucketsPlatform.h"
#include "MemMngTracer.h"
#include "Core/Math/BitTweak.h"

namespace ITF
{

bool    MemMng::m_bInitialized= false;

void    MemMng::Init()
{
    // check already init
    MEMMNG_ASSERT(!m_bInitialized);
    if (m_bInitialized)
    {
        return;
    }

    m_bInitialized = true;

    static MemCounts counters;
    g_MemUsageStat = &counters;
    g_MemUsageStat->memoryTakenAtInit = MemMngDebug::GetCurrentPhysicalMemoryUsed();
    g_MemUsageStat->userMemoryAvailable = MemMngDebug::GetTotalPhysicalMemory();

    // sub allocator init
    if (!MemMngStd::Init())
    {
        return;
    }

    if (!MemMngBuckets::Init())
    {
        return;
    }

    // quarantine
    MemMngDebug::InitQuarantine();

    // big alloc
    MemMngBigAlloc::Init();

    // callstack memory tracer
    MemMngDebug::InitMemoryTracer();
    MEMMNG_TRACER_INIT;
}

void*    MemMng::Malloc(u64 _size)		// from u32
{
    return AlignedMalloc(_size, 1);
}

void*    MemMng::MallocCategory(u32 _size, u32 _idCat)
{
    return AlignedMallocCategory(_size, 1, _idCat);
}

void*    MemMng::Realloc(void * _ptr, u64 _newSize)  // from u32
{
    return AlignedRealloc(_ptr, _newSize, 1);
}

u64    MemMng::Size(void * _ptr)		// from u32
{
    return AlignedSize(_ptr);
}

void    MemMng::Free(void * _ptr)
{
    AlignedFree(_ptr);
}


void* MemMng::AlignedMalloc(u64 iSize, u64 uAlign)  // from u32, u32
{
    MEMMNG_ASSERT(m_bInitialized);
    
    if(!iSize)
    {
        return NULL;
    }

    MEMMNG_TRACER_ASKEDSIZE(iSize);

    void * pRet;

    // Zero alignment is NOT tolerated (anyway, it's not compatible with the "BitTweak::GetAboveAlign" function called below)
    uAlign = Max(u64(1), uAlign);

    // Sizes up to MEMMNG_DEFAULT_ALIGNMENT should be aligned on their size alignment
    if (iSize <= MEMMNG_DEFAULT_ALIGNMENT)
    {
        u32 sizeAlignment = BitTweak::GetPow2Alignment(u32(iSize));
        uAlign = Max(uAlign, u64(sizeAlignment));
    }
    else
    {
        u32 sizeAlignment = BitTweak::GetPow2Alignment(u32(iSize));
        sizeAlignment = Min(sizeAlignment, u32(MEMMNG_DEFAULT_ALIGNMENT)); // Do not force alignment above 32 bytes.
        uAlign = Max(uAlign, u64(sizeAlignment));
    }

    // Align size too
    u32 iAlignedSize = BitTweak::GetAboveAlign(iSize, uAlign);
    if (iAlignedSize < iSize)
    {
        return NULL; // alignment overflow iSize, so there is no way we can handle the allocation
    }
    iSize = iAlignedSize;

    // ok, lets look if go in quarantine
    pRet= MemMngDebug::AllocInQuarantine(iSize, uAlign);
    if (pRet!=NULL)
    {
        DECL_MEM_USAGE(0);
        GET_MEM_USAGE(MemMngDebug::SizeInQuarantine(pRet));
        COUNT_ALLOC(Quarantine);
        return pRet; // jump over mem counting for quarantine memory, as it dos not mean anything
    }

    // else lets go to the normal allocation scheme
    DECL_MEM_USAGE(0);

    if (iSize<=MEMMNG_BUCKET_SIZE_MAX && MemMngBuckets::BucketSystemAvailable())
    {
        pRet = MemMngBuckets::Alloc(iSize, uAlign);
        GET_MEM_USAGE(pRet!=NULL?MemMngBuckets::MSize(pRet):0);
        COUNT_ALLOC(BucketMemUsed);
        MEMMNG_TRACER_ALLOC(pRet, NMALLOC_BUCKET);
    }
    // bucket system has failed ? Alloc in big
    if (pRet==NULL && iSize>=MEMMNG_BIGALLOC_SIZE_MIN && uAlign<=MemMngBigAlloc::GetPageSize())
    {
        pRet = MemMngBigAlloc::Alloc(iSize); // uAlign ignored -> aligned on page size
        GET_MEM_USAGE(pRet!=NULL?MemMngBigAlloc::MSize(pRet):0);
        COUNT_ALLOC(BigMemUsed);
        MEMMNG_TRACER_ALLOC(pRet, NMALLOC_BIG);
    }
    // finally try std
    if (pRet==NULL)
    {
        pRet= MemMngStd::Alloc(iSize, uAlign);
        GET_MEM_USAGE(pRet!=NULL?MemMngStd::MSize(pRet):0);
        COUNT_ALLOC(MallocMemUsed);
        MEMMNG_TRACER_ALLOC(pRet, NMALLOC_STD);
    }

    COUNT_ALLOC(TotalMemUsed);
    MEMNG_OUT_OF_MEM(pRet, iSize);

    return pRet;
}

void* MemMng::AlignedMallocCategory(u32 iSize, u32 uAlign, u32 _idCat)
{
    // TODO -> do not ignore category
    return AlignedMalloc(iSize, uAlign);
}

void * MemMng::AlignedRealloc(void* pData, u64 iNewSize, u64 uAlign)    // from u32, u32
{
    MEMMNG_ASSERT(m_bInitialized);

    // 2 special case, pure free and pure alloc
    if (iNewSize==0)
    {
        Free(pData);
        return NULL;
    }
    if (!pData)
    {
        return AlignedMalloc(iNewSize, uAlign);
    }

    // classic, get the oldsize, alloc, copy and free
    u64 iOldSize = Size(pData);   /// from u32
    void* pRet = AlignedMalloc(iNewSize, uAlign);
    if (pRet)
    {
        memmove(pRet, pData, Min(iNewSize, iOldSize));
    }
    AlignedFree(pData);
    return pRet;
}

u64 MemMng::AlignedSize(void * pData)
{
    if (pData==0)
    {
        return 0;
    }

    if (MemMngDebug::FindInQuarantine(pData))
    {
        return MemMngDebug::SizeInQuarantine(pData);
    }
    if (MemMngBuckets::IsBucketRangeAddress(pData))
    {
        return MemMngBuckets::MSize(pData);
    }
    else if (MemMngBigAlloc::IsInRange(pData))
    {
        return MemMngBigAlloc::MSize(pData);
    }
    else 
    {
        return MemMngStd::MSize(pData);
    }
}

void MemMng::AlignedFree(void *pData)
{
    if (pData==0)
    {
        return;
    }

    if (MemMngDebug::FindInQuarantine(pData))
    {
        DECL_MEM_USAGE(0);
        GET_MEM_USAGE(MemMngDebug::SizeInQuarantine(pData));
        COUNT_FREE(Quarantine);
        MemMngDebug::FreeInQuarantine(pData);
        return;
    }

    DECL_MEM_USAGE(0);

    if (MemMngBuckets::IsBucketRangeAddress(pData))
    {
        GET_MEM_USAGE(MemMngBuckets::MSize(pData));
        COUNT_FREE(BucketMemUsed);
        MemMngBuckets::Free(pData);
    }
    else if (MemMngBigAlloc::IsInRange(pData))
    {
        GET_MEM_USAGE(MemMngBigAlloc::MSize(pData));
        COUNT_FREE(BigMemUsed);
        MemMngBigAlloc::Free(pData);
    }
    else
    {
        GET_MEM_USAGE(MemMngStd::MSize(pData));
        COUNT_FREE(MallocMemUsed);
        MemMngStd::Free(pData);
    }

    COUNT_FREE(TotalMemUsed);
    MEMMNG_TRACER_FREE(pData, LAST_MEM_USAGE);
}

void	MemMng::Close	()
{
    m_bInitialized= false;
    MemMngBuckets::Close();
    MEMMNG_TRACER_CLOSE;
}

}
