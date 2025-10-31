#include "precompiled_core.h"
#include "MemMngPrivate.h"
#include "MemMngBuckets.h"
#include "MemMngBucketsPrivate.h"
#include "MemMngTracer.h"
#include "Core/Math/BitTweak.h"

#define FREEMEM_PATTERN_4BYTES (FREEMEM_PATTERN|(FREEMEM_PATTERN<<8)|(FREEMEM_PATTERN<<16)|(FREEMEM_PATTERN<<24))

namespace ITF
{

namespace MemMngBuckets
{
    // lookup table to speed up bucket size calculations
    static const u32 FASTLOOKUP_SIZE = 256;  // covers sizes up to 4096 bytes
    static u8 aFastLookup[FASTLOOKUP_SIZE] __attribute__((aligned(128)));

    // mutex
    Mutex * g_pBucketMutex = 0;

// base allocation for buckets
static inline void * PageAlloc(void * suggestedAddress, u32 nSize, u32 uAlign, u32 iEltSize)
{
    void * pRet;
    pRet= MemMngBucketsPlatform::SystemPageAlloc(suggestedAddress, nSize, uAlign, iEltSize);
    DECL_MEM_USAGE(	pRet!=NULL?MemMngBucketsPlatform::SystemPageSize(pRet):0);
    COUNT_ALLOC(BucketPageMemAllocated);
    return pRet;
}

static inline void PageFree(void * pAdrs)
{
    DECL_MEM_USAGE(MemMngBucketsPlatform::SystemPageSize(pAdrs));
    COUNT_FREE(BucketPageMemAllocated);
    MemMngBucketsPlatform::SystemPageFree(pAdrs);
}

//////////////////////////////////////////////////////////////////////////
// BucketHeader implementation
//////////////////////////////////////////////////////////////////////////
// data
BucketHeader BucketHeader::m_aBucketHeaderTable[BucketHeader::NB_MAX_PAGE];

PageIndex MemMngBuckets::BucketHeader::m_nFirstFreePage = 0;

// methods
// Get the Elt size of a BucketHeader
EltSize BucketHeader::GetEltSize()
{
    return SizeBucketHeader::GetSizeFromIndex(GetSizeIndex());
}

PageIndex BucketHeader::GetPageIndex()
{
    return PageIndex(this-m_aBucketHeaderTable);
}

char * BucketHeader::GetPageAddress()
{
    u32 iPage= GetPageIndex();
    return MemMngBucketsPlatform::GetAddressFromPageIndex(iPage) + GetOffsetInPage();
}

void BucketHeader::SetOffsetFromAddress(void * addr)
{
    u32 iPage= GetPageIndex();
    char * pageStart = MemMngBucketsPlatform::GetAddressFromPageIndex(iPage);
    SetOffsetInPage(((char*)addr) - pageStart);
}

EltSize BucketHeader::GetEltIndex(void * pAdrs)
{
    return ((char *)pAdrs-GetPageAddress())/GetEltSize();
}

void *	BucketHeader::GetEltAdress(EltIndex iElt)
{
    return GetPageAddress()+u32(iElt)*u32(GetEltSize());
}

u32 BucketHeader::GetNbEltMax()
{
    return MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY/u32(GetEltSize()); // could be store into SizeBucketHeader
}

#if 0
void BucketHeader::InitAfterPageAlloc(SizeIndex iSizeIndex)
{
    if (GetPageIndex() == BucketHeader::GetFirstFreePage())
    {
        // it's the first one, change it to the next (the platform has accepted the suggested address)
        MEMMNG_ASSERT(GetPrevPage()==INVALID_PAGE_INDEX); // we are the first also according to the linking
        BucketHeader::SetFirstFreePage(GetNextPage());
    }
    Unlink();
    
    // init page members
    SetSizeIndex(iSizeIndex);
    SetEltCount(0);
    // lets init the FreeList
    u32 nEltSize= GetEltSize();
    u32 nbMaxElt= GetNbEltMax();
    u32 nFirstFreeElt = -1;
    char * pCurrent= GetPageAddress();
    MEMMNG_ASSERT(nbMaxElt>=1);
    for(u32 i= 0; i<nbMaxElt-1; i++, pCurrent+= nEltSize)
    {
        if (nFirstFreeElt==-1)
        {
            nFirstFreeElt = i;
        }

#if MEMMNG_ENABLE_ASSERTS==1
        FILL_FREE(pCurrent, GetEltSize());
#endif //MEMMNG_ENABLE_ASSERTS==1

        bool exit = true;

        for(u32 j=i+1; j<nbMaxElt-1; j++)
        {
            *((EltIndex *)(pCurrent))= j;

#if MEMMNG_ENABLE_ASSERTS==1
            // Fill the first two elt indices with next elt index
            if (nEltSize >= 16)
            {
                EltIndex* indices = (EltIndex*)pCurrent;
                indices[1] = j;
            }
#endif //MEMMNG_ENABLE_ASSERTS==1

            exit = false;
            break;
        }

        if (exit)
            break;
    }

    *((EltIndex *)(pCurrent))= INVALID_ELT_INDEX;

#if MEMMNG_ENABLE_ASSERTS==1
    // Fill the first two elt indices with next elt index
    if (nEltSize >= 16)
    {
        EltIndex* indices = (EltIndex*)pCurrent;
        indices[1] = INVALID_ELT_INDEX;
    }
#endif //MEMMNG_ENABLE_ASSERTS==1

    // init first ptr
    SetFirstFreeElt(nFirstFreeElt);
}
#else

void BucketHeader::InitAfterPageAlloc(SizeIndex iSizeIndex)
{
    if (GetPageIndex() == BucketHeader::GetFirstFreePage())
    {
        // it's the first one, change it to the next (the platform has accepted the suggested address)
        MEMMNG_ASSERT(GetPrevPage()==INVALID_PAGE_INDEX); // we are the first also according to the linking
        BucketHeader::SetFirstFreePage(GetNextPage());
    }
    Unlink();

    // init page members
    SetSizeIndex(iSizeIndex);
    SetEltCount(0);
    // lets init the FreeList
    u32 nEltSize= GetEltSize();
    u32 nbMaxElt= GetNbEltMax();
    char * pCurrent= GetPageAddress();
    MEMMNG_ASSERT(nbMaxElt>=1);
    for(ux i = 0; i<nbMaxElt-1; i++, pCurrent += nEltSize)
    {
#if MEMMNG_ENABLE_ASSERTS==1
        FILL_FREE(pCurrent, GetEltSize());
#endif //MEMMNG_ENABLE_ASSERTS==1

        EltIndex* indices = (EltIndex*)pCurrent;
        indices[0] = i+1;

#if MEMMNG_ENABLE_ASSERTS==1
        // Fill the first two elt indices with next elt index
        if (nEltSize >= 16)
        {
            indices[1] = i+1;
        }
#endif //MEMMNG_ENABLE_ASSERTS==1
    }

#if MEMMNG_ENABLE_ASSERTS==1
    FILL_FREE(pCurrent, GetEltSize());
#endif //MEMMNG_ENABLE_ASSERTS==1

    EltIndex* indices = (EltIndex*)pCurrent;
    indices[0] = INVALID_ELT_INDEX;
#if MEMMNG_ENABLE_ASSERTS==1
    // Fill the first two elt indices with next elt index
    if (nEltSize >= 16)
    {
        indices[1] = INVALID_ELT_INDEX;
    }
#endif //MEMMNG_ENABLE_ASSERTS==1

    SetFirstFreeElt(0);
}
#endif


void *	BucketHeader::AllocElt()
{
    MEMMNG_ASSERT(GetFirstFreeElt() != INVALID_ELT_INDEX); // can be a memory manager bug
    MEMMNG_ASSERT(GetFirstFreeElt() < GetNbEltMax()); // probably a corruption

    // get address and update freelist
    void* pRet = GetEltAdress(GetFirstFreeElt());

#if MEMMNG_ENABLE_ASSERTS==1 && MEMMNG_FILL_MEMORY_AT_FREE==1
    if (GetEltSize() >= 16)
    {
        // Validate pRet
        u32* valuesBuffer = (u32*)pRet;
        MEMMNG_ASSERT(valuesBuffer[0] == valuesBuffer[1]);
        MEMMNG_ASSERT(valuesBuffer[2] == FREEMEM_PATTERN_4BYTES);
        MEMMNG_ASSERT(valuesBuffer[3] == FREEMEM_PATTERN_4BYTES);

        // And validate the nextfreeelt we are storing (if not invalid)
        if (*((EltIndex*)pRet) != INVALID_ELT_INDEX)
        {
            u32* nextFreeElt = (u32*)GetEltAdress(*((EltIndex*)pRet));
            MEMMNG_ASSERT(nextFreeElt[0] == nextFreeElt[1]);
            MEMMNG_ASSERT(nextFreeElt[2] == FREEMEM_PATTERN_4BYTES);
            MEMMNG_ASSERT(nextFreeElt[3] == FREEMEM_PATTERN_4BYTES);
        }
    }
#endif //MEMMNG_ENABLE_ASSERTS==1

    // get next from the first elt memory block (first freelist elt)
    u32 nextFreeEltIndex = *((EltIndex*)pRet);

    MEMMNG_ASSERT((nextFreeEltIndex == INVALID_ELT_INDEX) || (nextFreeEltIndex < GetNbEltMax()));

    SetFirstFreeElt(nextFreeEltIndex);
    IncEltCount();

    MEMMNG_ASSERT((GetEltCount() < GetNbEltMax()) || (GetFirstFreeElt() == INVALID_ELT_INDEX));
    MEMMNG_ASSERT(GetEltCount()<=GetNbEltMax());
    MEMMNG_ASSERT(GetFirstFreeElt() != INVALID_ELT_INDEX || GetEltCount() == GetNbEltMax());

    // update linked list
    if (GetFirstFreeElt()==INVALID_ELT_INDEX)
    {
        MEMMNG_AUTO_BUCKET_LOCK();
        SizeBucketHeader::InsertHeadInFullList(*this);
    }
// Sort the list in the alloc is not needed because this won't change the order
//	else
//	{
//		SizeBucketHeader::MoveUpInNotFullList(*this);
//	}

    FILL_ALLOC(pRet, GetEltSize());
    return pRet;
}

void	BucketHeader::FreeElt(void * pAdrs)
{
    // elt free list update
    bool bWasFull = (GetFirstFreeElt()==INVALID_ELT_INDEX);
    MEMMNG_ASSERT(!bWasFull || GetEltCount()==GetNbEltMax());
    MEMMNG_ASSERT(bWasFull || GetEltCount()<GetNbEltMax());
    MEMMNG_ASSERT(bWasFull || GetFirstFreeElt()<GetNbEltMax());

#if MEMMNG_ENABLE_ASSERTS==1 && MEMMNG_FILL_MEMORY_AT_FREE==1
    if (GetEltSize() >= 16)
    {
        // Validate pAdrs for double-free
        u32* valuesBuffer = (u32*)pAdrs;

        MEMMNG_ASSERT(!((valuesBuffer[0] == valuesBuffer[1]) &&
                        (valuesBuffer[1] != FREEMEM_PATTERN_4BYTES) &&
                        (valuesBuffer[2] == FREEMEM_PATTERN_4BYTES) &&
                        (valuesBuffer[3] == FREEMEM_PATTERN_4BYTES)));
    }
#endif //MEMMNG_ENABLE_ASSERTS==1

    FILL_FREE(pAdrs, GetEltSize());

    *((EltIndex *)pAdrs)= GetFirstFreeElt();

#if MEMMNG_ENABLE_ASSERTS==1
    // Fill the first two elt indices with next elt index
    if (GetEltSize() >= 16)
    {
        EltIndex* indices = (EltIndex*)pAdrs;
        indices[1] = GetFirstFreeElt();
    }
#endif //MEMMNG_ENABLE_ASSERTS==1

    SetFirstFreeElt(GetEltIndex(pAdrs));
    MEMMNG_ASSERT(GetFirstFreeElt()<GetNbEltMax());
    // now update linking
    if (GetEltCount()==1)
    {
        MEMMNG_AUTO_BUCKET_LOCK();
        DecEltCount();
        // lets free it
        PageFree(GetPageAddress());
        MEMMNG_ASSERT(GetFirstFreeElt()!=INVALID_ELT_INDEX);
        // update links
        SizeBucketHeader::RemovePageFromAnyList(*this);
        // set it has invalid
        SetAsInvalid();
        // link it to the free pages list
        if (GetFirstFreePage()!=INVALID_PAGE_INDEX)
        {
            LinkBefore(GetFirstFreePage());
        }
        SetFirstFreePage(GetPageIndex());
    }
    else
    {
        DecEltCount();
        if (bWasFull)
        {
            MEMMNG_AUTO_BUCKET_LOCK();
            // was previously full so link it head
            SizeBucketHeader::GetSizeBucketHeader(*this).InsertFullAsHeadInNotFullList(*this);
        }
        else
        {
            // optim [PZ] : divide by 8 the number of time we have to sort the list
            if ( (GetEltCount() & 7) == 7)
            {
                SizeBucketHeader::MoveDownInNotFullList(*this);
            }
        }
    }
}


void	BucketHeader::Unlink()
{
    if (GetPrevPage()!=INVALID_PAGE_INDEX)
    {
        BucketHeader & prev= m_aBucketHeaderTable[GetPrevPage()];
        MEMMNG_ASSERT(prev.GetNextPage()==GetPageIndex());
        prev.SetNextPage(GetNextPage());
    }
    if (GetNextPage()!=INVALID_PAGE_INDEX)
    {
        BucketHeader & next= m_aBucketHeaderTable[GetNextPage()];
        MEMMNG_ASSERT(next.GetPrevPage()==GetPageIndex());
        next.SetPrevPage(GetPrevPage());
    }
    SetPrevPage(INVALID_PAGE_INDEX);
    SetNextPage(INVALID_PAGE_INDEX);
}

void	BucketHeader::MoveUpInList()
{
    if (GetPrevPage()==INVALID_PAGE_INDEX)
    {
        return; // we are in head, that's all
    }
    BucketHeader * pCurrent= &m_aBucketHeaderTable[GetPrevPage()];
    while(pCurrent->GetEltCount()<GetEltCount() && pCurrent->GetPrevPage()!=INVALID_PAGE_INDEX)
    {
        pCurrent= &m_aBucketHeaderTable[pCurrent->GetPrevPage()];
    }
    Unlink();
    if (pCurrent->GetEltCount()>=GetEltCount())
    {
        LinkAfter(pCurrent->GetPageIndex());
    }
    else
    {
        LinkBefore(pCurrent->GetPageIndex());
    }
}

void	BucketHeader::MoveDownInList()
{
    if (GetNextPage()==INVALID_PAGE_INDEX)
    {
        return; // we are in queue, that's all
    }
    BucketHeader * pCurrent= &m_aBucketHeaderTable[GetNextPage()];
    while(pCurrent->GetEltCount()>GetEltCount() && pCurrent->GetNextPage()!=INVALID_PAGE_INDEX)
    {
        pCurrent= &m_aBucketHeaderTable[pCurrent->GetNextPage()];
    }
    Unlink();
    if (pCurrent->GetEltCount()<=GetEltCount())
    {
        LinkBefore(pCurrent->GetPageIndex());
    }
    else
    {
        LinkAfter(pCurrent->GetPageIndex());
    }
}

void	BucketHeader::LinkBefore(PageIndex pNext)
{
    MEMMNG_ASSERT(pNext!=INVALID_PAGE_INDEX);
    MEMMNG_ASSERT(GetPrevPage()==INVALID_PAGE_INDEX && GetNextPage()==INVALID_PAGE_INDEX);
    BucketHeader & headerNext= GetBucketHeader(pNext);
    SetPrevPage(headerNext.GetPrevPage());
    SetNextPage(pNext);
    headerNext.SetPrevPage(GetPageIndex());
    // now the prev
    if (GetPrevPage()!=INVALID_PAGE_INDEX)
    {
        BucketHeader & headerPrev= GetBucketHeader(GetPrevPage());
        headerPrev.SetNextPage(GetPageIndex());
    }
}

void	BucketHeader::LinkAfter(PageIndex pPrev)
{
    MEMMNG_ASSERT(pPrev!=INVALID_PAGE_INDEX);
    MEMMNG_ASSERT(GetPrevPage()==INVALID_PAGE_INDEX && GetNextPage()==INVALID_PAGE_INDEX);
    BucketHeader & headerPrev= GetBucketHeader(pPrev);
    SetPrevPage(pPrev);
    SetNextPage(headerPrev.GetNextPage());
    headerPrev.SetNextPage(GetPageIndex());
    // now update the next
    if (GetNextPage()!=INVALID_PAGE_INDEX)
    {
        BucketHeader & headerNext= GetBucketHeader(GetNextPage());
        headerNext.SetPrevPage(GetPageIndex());
    }
}

// static methods

PageIndex BucketHeader::GetBucketPageIndex(void * pAdrs)
{
    return PageIndex(MemMngBucketsPlatform::GetPageIndexFromAddress(pAdrs));
}

void BucketHeader::InitBucketHeaderTable()
{
    BucketHeader::SetFirstFreePage(0);

    for (u32 i= 0; i<NB_MAX_PAGE; i++)
    {
        BucketHeader & header= m_aBucketHeaderTable[i];
        header.SetSizeIndex(INVALID_SIZE_INDEX);
        header.SetEltCount(0);
        header.SetFirstFreeElt(INVALID_ELT_INDEX);
        header.SetOffsetInPage(INVALID_OFFSET);
        // Free pages management
        header.SetPrevPage(i==0?INVALID_PAGE_INDEX:i-1);
        header.SetNextPage(i==NB_MAX_PAGE-1?INVALID_PAGE_INDEX:i+1);
    }
}

BucketHeader & BucketHeader::GetBucketHeaderFromAddress(void * pPageAdrs)
{
    return m_aBucketHeaderTable[GetBucketPageIndex(pPageAdrs)];
}

BucketHeader * BucketHeader::GetBucketHeaderFromEltAddress(void * pData)
{
#if defined(MEMMNG_BUCKET_BUILT_ON_MALLOC) && MEMMNG_BUCKET_BUILT_ON_MALLOC!=0
    // the caller give us a pointer in the range
    BucketHeader * headerBucket= &BucketHeader::GetBucketHeaderFromAddress(pData);
    PageOffset offset = headerBucket->GetOffsetInPage();
    if (offset!=INVALID_OFFSET && ((char *)pData) >= MemMngBucketsPlatform::GetAddressFromPageIndex(headerBucket->GetPageIndex()) + offset)
    {
        return headerBucket; // canonical page is a good guess
    }

    // ok, the canonic page is not valid, lets look if the page below is valid and
    // if it's in the range
    PageIndex idxPage = headerBucket->GetPageIndex();
    if (idxPage>=1) // beware of page 0
    {
        headerBucket = &BucketHeader::GetBucketHeader(idxPage-1);
        offset = headerBucket->GetOffsetInPage();
        if (offset!=INVALID_OFFSET
            && pData < MemMngBucketsPlatform::GetAddressFromPageIndex(headerBucket->GetPageIndex()) + offset + MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY)
        {
            return headerBucket; // well done it's the below page
        }
    }
    return 0; // definitely not available
#else
    return & (GetBucketHeaderFromAddress(pData));
#endif
}

BucketHeader & BucketHeader::GetBucketHeader(PageIndex iPage)
{
    return m_aBucketHeaderTable[iPage];
}

BucketHeader * BucketHeader::CreateBucket(SizeIndex iSizeIndex)
{
    PageIndex indexFree = GetFirstFreePage();

    void * pRet = 0;
    if (indexFree!=INVALID_PAGE_INDEX) // OUT OF MEMORY ?
    {
        pRet = PageAlloc(	MemMngBucketsPlatform::GetAddressFromPageIndex(indexFree),
                            MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY,
                            MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY,
                            iSizeIndex);
    }

    if (pRet!=NULL)
    {
        MEMMNG_ASSERT(!BucketHeader::GetBucketHeaderFromAddress(pRet).IsABucketHeader());
        BucketHeader & headerBucket= GetBucketHeaderFromAddress(pRet);
        headerBucket.SetOffsetFromAddress(pRet);
        headerBucket.InitAfterPageAlloc(iSizeIndex);
        return &headerBucket;
    }
    else
    {
        return NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// SizeBucketHeader implementation
//////////////////////////////////////////////////////////////////////////

// data
SizeBucketHeader SizeBucketHeader::m_aSizeBucketList[MemMngBucketsPlatform::BUCKET_NB_SIZE];
PageIndex SizeBucketHeader::m_pFirstFullList= INVALID_PAGE_INDEX;

// methods
BucketHeader * SizeBucketHeader::GetBestBucketHeader()
{
    if (m_pFirstNotFull==INVALID_PAGE_INDEX)
    {
        MEMMNG_AUTO_BUCKET_LOCK();

        BucketHeader * pheaderBucket= BucketHeader::CreateBucket(GetIndex());
        if (pheaderBucket!=NULL)
        {
            // insert in head
            m_pFirstNotFull= pheaderBucket->GetPageIndex();
            return pheaderBucket;
        }
        else
        {
            return NULL;
        }
    }
    return &BucketHeader::GetBucketHeader(m_pFirstNotFull);
}

SizeIndex SizeBucketHeader::GetIndex()
{
    return (this-m_aSizeBucketList);
}

Mutex * SizeBucketHeader::GetLock()
{
    return m_lock;
}

// static 
SizeBucketHeader * SizeBucketHeader::GetBestBucketHeaderForSize(u32 iSize, u32 uAlign)
{
    uAlign = MemMngBucketsPlatform::ConvertAlignment(iSize, uAlign); // choose the best alignment according to the platform page system

    MEMMNG_ASSERT(uAlign==0 || BitTweak::IsPow2(uAlign)); // (pow2 aligned is the only thing we look on for alignment of course)

    const unsigned int nbBuckets = sizeof(MemMngBucketsPlatform::aSizeList)/sizeof(MemMngBucketsPlatform::aSizeList[0]);
    const unsigned int * sizeListEnd = MemMngBucketsPlatform::aSizeList + nbBuckets;
    ITF_UNUSED(sizeListEnd);

    const unsigned int * sizeFound = MemMngBucketsPlatform::aSizeList + aFastLookup[Min((iSize>>4), FASTLOOKUP_SIZE)];

    while (iSize > *sizeFound)
    {
        sizeFound++;
    }

    MEMMNG_ASSERT(sizeFound < sizeListEnd);

    if (uAlign!=0)
    {
        // find next bucket aligned to uAlign size.
        while (*sizeFound & (uAlign-1))
        {
            sizeFound++;
            MEMMNG_ASSERT(sizeFound < sizeListEnd);
        }
        MEMMNG_ASSERT(BitTweak::GetAboveAlign(*sizeFound, uAlign) == *sizeFound);
    }

    return m_aSizeBucketList + (sizeFound - MemMngBucketsPlatform::aSizeList);
}

void SizeBucketHeader::InitSizeBucketHeaderTable()
{
    static Mutex locks[MemMngBucketsPlatform::BUCKET_NB_SIZE];

    for(int i= 0; i<MemMngBucketsPlatform::BUCKET_NB_SIZE; i++)
    {
        SizeBucketHeader & item = m_aSizeBucketList[i];

        item.m_pFirstNotFull= INVALID_PAGE_INDEX;
        item.m_nEltSize= MemMngBucketsPlatform::aSizeList[i];
        item.m_lock= &locks[i];
    }

    m_pFirstFullList= INVALID_PAGE_INDEX;
}

// get the size of an index of an entry in the size table
u32 SizeBucketHeader::GetSizeFromIndex(SizeIndex index)
{
    return m_aSizeBucketList[index].m_nEltSize;
}

void	SizeBucketHeader::RemovePageFromAnyList(BucketHeader & headerBucket)
{
    PageIndex iPage= headerBucket.GetPageIndex();
    if (m_pFirstFullList==iPage)
    {
        m_pFirstFullList= headerBucket.GetNextPage();
    }
    else
    {
        SizeBucketHeader &	sheaderBucket= SizeBucketHeader::GetSizeBucketHeader(headerBucket);
        if (sheaderBucket.m_pFirstNotFull==iPage)
        {
            sheaderBucket.m_pFirstNotFull= headerBucket.GetNextPage();
        }
    }
    headerBucket.Unlink();
}

void	SizeBucketHeader::InsertHeadInFullList(BucketHeader & headerBucket)
{
    RemovePageFromAnyList(headerBucket);
    MEMMNG_ASSERT(headerBucket.GetEltCount()==headerBucket.GetNbEltMax());
    if (m_pFirstFullList!=INVALID_PAGE_INDEX)
    {
        headerBucket.LinkBefore(m_pFirstFullList);
    }
    m_pFirstFullList= headerBucket.GetPageIndex();
}

void	SizeBucketHeader::MoveUpInNotFullList(BucketHeader & headerBucket)
{
    headerBucket.MoveUpInList();
    SizeBucketHeader &	sheaderBucket= SizeBucketHeader::GetSizeBucketHeader(headerBucket);
    if (sheaderBucket.m_pFirstNotFull==headerBucket.GetNextPage())
    {
        sheaderBucket.m_pFirstNotFull= headerBucket.GetPageIndex();
    }
}

void	SizeBucketHeader::MoveDownInNotFullList(BucketHeader & headerBucket)
{
    SizeBucketHeader &	sheaderBucket= SizeBucketHeader::GetSizeBucketHeader(headerBucket);
    bool bWasInHead= sheaderBucket.m_pFirstNotFull==headerBucket.GetPageIndex();
    PageIndex iNext= headerBucket.GetNextPage();
    headerBucket.MoveDownInList();
    if (bWasInHead && headerBucket.GetPrevPage()!=INVALID_PAGE_INDEX)
    {
        sheaderBucket.m_pFirstNotFull= iNext;
    }
}

void	SizeBucketHeader::InsertFullAsHeadInNotFullList(BucketHeader & headerBucket)
{
    RemovePageFromAnyList(headerBucket);
    // now put it in head
    if (m_pFirstNotFull!=INVALID_PAGE_INDEX)
    {
        headerBucket.LinkBefore(m_pFirstNotFull);
    }
    m_pFirstNotFull= headerBucket.GetPageIndex();
}

SizeBucketHeader & SizeBucketHeader::GetSizeBucketHeaderFromIndex(SizeIndex index)
{
    return m_aSizeBucketList[index];
}

SizeBucketHeader &	SizeBucketHeader::GetSizeBucketHeader(BucketHeader & headerBucket)
{
    return m_aSizeBucketList[headerBucket.GetSizeIndex()];
}


//////////////////////////////////////////////////////////////////////////
// Check validity
//////////////////////////////////////////////////////////////////////////

u32 g_ValidityCheckPeriod = 100;

// should only be called if the headerBucket size lock is locked
void	GlobalBucketCheckValidity(BucketHeader & headerBucket)
{
#if CHECK_BUCKETS_POLICY==1
    static i32 nCount = 0;
    if (((MemMngTools::AtomicAdd(nCount, 1)+1u) % g_ValidityCheckPeriod) == 0)
    {
        headerBucket.CheckValidity();
    }
#endif
}

void    FullCheckValidity()
{
    // for full check validity we need to lock every size lock

    for (ux i= 0; i<MemMngBucketsPlatform::BUCKET_NB_SIZE; i++)
    {
        SizeBucketHeader::GetSizeBucketHeaderFromIndex(i).GetLock()->lock();
    }

    {
        MEMMNG_AUTO_BUCKET_LOCK(); // to avoid deadlock -> size lock should never happen inside MEMMNG_AUTO_BUCKET_LOCK blocks
        SizeBucketHeader::CheckArrayValidity();
        BucketHeader::CheckArrayValidity();
    }

    for (ux i= 0; i<MemMngBucketsPlatform::BUCKET_NB_SIZE; i++)
    {
        SizeBucketHeader::GetSizeBucketHeaderFromIndex(i).GetLock()->unlock();
    }
}

void    GlobalFullCheckValidity() // should not be called if any mutex is locked
{
#if CHECK_BUCKETS_POLICY==2
    static i32 nCount= 0;
    if (((MemMngTools::AtomicAdd(nCount, 1)+1u) % g_ValidityCheckPeriod) == 0)
    {
        FullCheckValidity();
    }
#endif // CHECK_BUCKETS_POLICY==2
}

void    DumpStats()
{
    MEMMNG_AUTO_BUCKET_LOCK();
    SizeBucketHeader::DumpArrayStats();
}


//*******************************************************************************************
// Public Interface
//*******************************************************************************************

void* Alloc(u32 iSize, u32 uAlign)
{
    GlobalFullCheckValidity();
    void * pRet = NULL;
    {
        SizeBucketHeader & psheaderBucket = *SizeBucketHeader::GetBestBucketHeaderForSize(iSize, uAlign);
        MEMMNG_AUTO_LOCK_BY_SIZE(psheaderBucket);

        BucketHeader * pheaderBucket = psheaderBucket.GetBestBucketHeader();
        if (pheaderBucket!=NULL)
        {
            GlobalBucketCheckValidity(*pheaderBucket);
            pRet= pheaderBucket->AllocElt();
            MEMMNG_ASSERT(pRet!=NULL); // Out of mem in buckets ! should not happen there (pheaderBucket should have been null)
            MEMMNG_ASSERT(MemMngTools::GetAlignFromAdrs(pRet, uAlign)==0);
            GlobalBucketCheckValidity(*pheaderBucket);
        }
    }
    GlobalFullCheckValidity();
    return pRet; // no out of mem report here (caller's job)
}

void Free(void *pData)
{
    GlobalFullCheckValidity();
    {
        BucketHeader * headerBucket= BucketHeader::GetBucketHeaderFromEltAddress(pData);
        MEMMNG_ASSERT(headerBucket);
        SizeBucketHeader & psheaderBucket = SizeBucketHeader::GetSizeBucketHeader(*headerBucket);
        MEMMNG_AUTO_LOCK_BY_SIZE(psheaderBucket);

        GlobalBucketCheckValidity(*headerBucket);
        MEMMNG_ASSERT(headerBucket->IsABucketHeader());
        if (headerBucket->IsABucketHeader())
        {
            headerBucket->FreeElt(pData);
        }	
        GlobalBucketCheckValidity(*headerBucket);
    }
    GlobalFullCheckValidity();
}

uPtr MSize(void * pData)
{
    if (pData==NULL)
    {
        return 0;
    }

    GlobalFullCheckValidity();

    BucketHeader * headerBucket= BucketHeader::GetBucketHeaderFromEltAddress(pData);

    if (headerBucket)
    {
        SizeBucketHeader & psheaderBucket = SizeBucketHeader::GetSizeBucketHeader(*headerBucket);
        MEMMNG_AUTO_LOCK_BY_SIZE(psheaderBucket);

        uPtr nSize = uPtr(headerBucket->GetEltSize());
        GlobalBucketCheckValidity(*headerBucket);
        return nSize;
    }

    return 0;
}

bool BucketSystemAvailable()
{
    return MemMngBucketsPlatform::BucketSystemAvailable();
}

bool IsBucketRangeAddress(void * pData)
{
#if MEMMNG_BUCKET_SPECIFIC_ADDRESS_RANGE==0
    return IsBucketAddress(pData); // Our buckets are built above another allocator
#else
    // faster version by just comparing values
    return MemMngBucketsPlatform::IsBucketRangeAddress(pData);
#endif
}

bool IsBucketAddress(void * pData)
{
    if (!MemMngBucketsPlatform::IsBucketRangeAddress(pData)) return false;
    BucketHeader * headerBucket= BucketHeader::GetBucketHeaderFromEltAddress(pData);
    bool bRet= headerBucket && headerBucket->IsABucketHeader();
    return bRet;
}

void InitFastLookupTable()
{
    const unsigned int nbSizes = sizeof(MemMngBucketsPlatform::aSizeList)/sizeof(MemMngBucketsPlatform::aSizeList[0]);
    ITF_UNUSED(nbSizes);
    unsigned int curSize = 0;

    for (unsigned int i = 0; i<FASTLOOKUP_SIZE; ++i)
    {
        while ((curSize < 255) && ((i<<4) > MemMngBucketsPlatform::aSizeList[curSize]))
        {
            MEMMNG_ASSERT(curSize < nbSizes);
            MEMMNG_ASSERT(MemMngBucketsPlatform::aSizeList[curSize+1] >= MemMngBucketsPlatform::aSizeList[curSize]);
            ++curSize;
        }

        aFastLookup[i] = (u8)curSize;
    }
}

bool Init()
{
    static Mutex mutex;
    g_pBucketMutex = &mutex;

    MemMngBucketsPlatform::Init();
    InitFastLookupTable();
    MemMngBuckets::BucketHeader::InitBucketHeaderTable();
    MemMngBuckets::SizeBucketHeader::InitSizeBucketHeaderTable();
    #if CHECK_BUCKETS_POLICY==2
        SizeBucketHeader::CheckArrayValidity();
        BucketHeader::CheckArrayValidity();
    #endif
    for(u32 i = BitTweak::GetLog2(BitTweak::GetCeilPow2(MemMngBucketsPlatform::aSizeList[0])),
        curPow2 = 1u << i;
        curPow2<MemMngBucketsPlatform::aSizeList[MemMngBucketsPlatform::BUCKET_NB_SIZE-1];
        i++, curPow2 = curPow2 << 1u)
    {
        // pow2 there ?
        MEMMNG_ASSERT(&MemMngBucketsPlatform::aSizeList[MemMngBucketsPlatform::BUCKET_NB_SIZE]!=std::find(&MemMngBucketsPlatform::aSizeList[0], &MemMngBucketsPlatform::aSizeList[MemMngBucketsPlatform::BUCKET_NB_SIZE], curPow2));
    }
    return true;
}

void Close()
{
    // todo
    MemMngBucketsPlatform::Close();
}

} // end of MemMngBuckets namespace

} // end of ITF namespace
