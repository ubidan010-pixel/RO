#ifndef ITF_CORE_MEMMNGBUCKETSPLATFORM_H_
#define ITF_CORE_MEMMNGBUCKETSPLATFORM_H_

#pragma once

#include "MemMngPrivate.h"
#include "MemMngStdAlloc.h"
#include "MemMngPage.h"

namespace ITF
{

#if !defined(MEMMNG_BUCKET_BUILT_ON_MALLOC) || MEMMNG_BUCKET_BUILT_ON_MALLOC==0

namespace MemMngBucketsPlatform
{
    // up to 512MB of virtual space reserved for buckets
    static const u32 BUCKET_MEMORY_ADRESSABLE = MEMMNG_BUCKET_MEMORY_ADRESSABLE;
    static const u32 BUCKET_PAGE_SIZE_GRANULARITY = MEMMNG_BUCKET_PAGE_SIZE_GRANULARITY;

    // everyone in 32 bits
    typedef unsigned int    PageIndex;
    typedef unsigned int    SizeIndex;
    typedef unsigned int    EltIndex;
    typedef unsigned int    EltSize;
    typedef unsigned int    PageOffset;

    static const SizeIndex  INVALID_SIZE_INDEX = SizeIndex(-1);
    static const PageIndex  INVALID_PAGE_INDEX = PageIndex(-1);
    static const EltIndex   INVALID_ELT_INDEX = EltIndex(-1);
    static const PageOffset INVALID_OFFSET = PageOffset(-1);

    class BucketHeader
    {
    private:
        u32 m_pPrev;
        u32 m_pFirstFreeElt;
        u32 m_pNext;
        u32 m_nEltCount;
        u32 m_iSizeIndex;

    public :
        // accessors
        PageIndex       GetNextPage() const { return PageIndex(m_pNext); }
        void            SetNextPage(PageIndex next) { m_pNext= next; }
        PageIndex       GetPrevPage() const { return PageIndex(m_pPrev); }
        void            SetPrevPage(PageIndex prev) { m_pPrev= prev; }
        SizeIndex       GetSizeIndex() const { return SizeIndex(m_iSizeIndex); }
        void            SetSizeIndex(SizeIndex iSize) { m_iSizeIndex= iSize; }
        EltIndex        GetFirstFreeElt() const { return EltIndex(m_pFirstFreeElt); }
        void            SetFirstFreeElt(EltIndex iFirstFreeElt) { m_pFirstFreeElt= iFirstFreeElt; }
        EltIndex        GetEltCount() const { return EltIndex(m_nEltCount); }
        void            SetEltCount(EltIndex nEltCount) { m_nEltCount= nEltCount; }
        void            IncEltCount() { m_nEltCount++; } // at alloc
        void            DecEltCount() { m_nEltCount--; } // at free
        PageOffset      GetOffsetInPage() const { return 0; }
        void            SetOffsetInPage(PageOffset offset) { }

        void            SetAsInvalid()
        {
            SetSizeIndex(INVALID_SIZE_INDEX);
            SetFirstFreeElt(INVALID_ELT_INDEX);
        }

        bool            IsABucketHeader() const
        {
            MEMMNG_ASSERT(GetSizeIndex()!=INVALID_SIZE_INDEX
                || (GetSizeIndex()==INVALID_SIZE_INDEX && GetFirstFreeElt()==INVALID_ELT_INDEX));
            return GetSizeIndex()!=INVALID_SIZE_INDEX; // count 0, should have been be freed
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Buckets size
    //////////////////////////////////////////////////////////////////////////
    // To get correct alignment handling, all power of 2 value until BUCKET_PAGE_SIZE_GRANULARITY should be there
    static const unsigned int nFirstSize= 4;
    static const unsigned int aSizeList[]= { nFirstSize, 8, 16, 24, 32, 48, 64, 80, 96, 128, 160, 192, 224, 256, 512, 816, 1024, 1360, 2048, 4096, 8192, 16384, 32768, BUCKET_PAGE_SIZE_GRANULARITY };

    // address mapping info
    extern void * baseAddress;
    extern void * limitAddress;

    extern MemoryPageManager * g_bucketPageMgr;

    // base page allocation for buckets
    inline void * SystemPageAlloc(void * suggestedAddress, u32 nSize, u32 uAlign, u32 iEltSize)
    {
        u8 * pageAddress = g_bucketPageMgr->CommitPages((u8 *)suggestedAddress, BUCKET_PAGE_SIZE_GRANULARITY);
        MEMMNG_ASSERT(pageAddress<(u8*)limitAddress);
        MEMMNG_ASSERT(pageAddress>=(u8*)baseAddress);
        return pageAddress;
    }

    // system phys alloc
    inline void SystemPageFree(void * pAdrs)
    {
        bbool ret = g_bucketPageMgr->DecommitPages((u8*)pAdrs, BUCKET_PAGE_SIZE_GRANULARITY);
        MEMMNG_ASSERT(ret!=bfalse);
    }

    inline u32 SystemPageSize(void * pAdrs)
    {
        return u32(BUCKET_PAGE_SIZE_GRANULARITY);
    }

    void Init();
    void Close();

    // constant

    ITF_COMPILE_TIME_ASSERT(Unexpected_BucketHeader_Size, sizeof(MemMngBucketsPlatform::BucketHeader)==20);

    static const u32 BUCKET_NB_SIZE= sizeof(aSizeList)/sizeof(aSizeList[0]);

    inline bool BucketSystemAvailable()
    {
        return true;
    }

    inline bool IsBucketRangeAddress(void * pData)
    {
        // check if we are in reserved area
        return (pData>=baseAddress && pData<=limitAddress);
    }

    inline u32 ConvertAlignment(u32 iSize, u32 uAlign)
    {
        return uAlign;
    }

    inline char * GetAddressFromPageIndex(u32 iPage)
    {
        return ((char *)baseAddress)+(iPage*BUCKET_PAGE_SIZE_GRANULARITY);
    }

    inline u32 GetPageIndexFromAddress(void * pAdrs)
    {
        return u32((char *)pAdrs-(char *)baseAddress)/BUCKET_PAGE_SIZE_GRANULARITY;
    }
}

#else // MEMMNG_BUCKET_BUILT_ON_MALLOC!=0
// Buckets are allocating using StdAlloc

namespace MemMngBucketsPlatform
{
    // when built on std alloc we have to get a range of addresses returned by the std allocator
    static const u32 BUCKET_MEMORY_ADRESSABLE = 256*1024*1024;

    // One advantage of being based on std alloc is that we can get lower size pages like 4K pages
    static const u32 BUCKET_PAGE_SIZE_GRANULARITY = 4*1024;

    inline char * GetAddressFromPageIndex(u32 iPage);
    inline u32 GetPageIndexFromAddress(void * pAdrs);

    static const u32 PAGE_INDEX_NB_BITS= 16;
    static const u32 ELT_INDEX_NB_BITS= 16;
    static const u32 ELT_COUNT_NB_BITS= 11; 
    static const u32 SIZE_INDEX_NB_BITS= 6;
    static const u32 OFFSET_NB_BITS= 10;

    // everyone in 32 bits
    typedef unsigned int	PageIndex;
    typedef unsigned int	SizeIndex;
    typedef unsigned int	EltIndex;
    typedef unsigned int	EltSize;
    typedef unsigned int	PageOffset;

    static const SizeIndex	INVALID_SIZE_INDEX= SizeIndex(1 << SIZE_INDEX_NB_BITS) - 1;
    static const PageIndex	INVALID_PAGE_INDEX= PageIndex(1 << PAGE_INDEX_NB_BITS) - 1;
    static const EltIndex	INVALID_ELT_INDEX= EltIndex(1 << ELT_INDEX_NB_BITS) - 1;
    static const PageOffset INVALID_OFFSET = (PageOffset(1 << OFFSET_NB_BITS) - 1) * 4u;

    // Bucket header is tightly packed

    class BucketHeader
    {
    private:
        u16 m_pPrev;
        u16 m_pNext;
        u16 m_pFirstFreeElt;
        u16 m_nEltCount;
        u16 m_iSizeIndex : SIZE_INDEX_NB_BITS; // 64 size allowed
        u16 m_offsetInPage : OFFSET_NB_BITS; // 1024 (*4) offset

    public :
        // accessors
        PageIndex		GetNextPage() { return PageIndex(m_pNext); }
        void			SetNextPage(PageIndex next) { m_pNext= next; }
        PageIndex		GetPrevPage() { return PageIndex(m_pPrev); }
        void			SetPrevPage(PageIndex prev) { m_pPrev= prev; }
        SizeIndex		GetSizeIndex() { return SizeIndex(m_iSizeIndex); }
        void			SetSizeIndex(SizeIndex iSize) { m_iSizeIndex= iSize; }
        EltIndex		GetFirstFreeElt() { return EltIndex(m_pFirstFreeElt); }
        void			SetFirstFreeElt(EltIndex iFirstFreeElt) { m_pFirstFreeElt= iFirstFreeElt; }
        EltIndex		GetEltCount() { return EltIndex(m_nEltCount); }
        void			SetEltCount(EltIndex nEltCount) { m_nEltCount= nEltCount; }
        void			IncEltCount() { m_nEltCount++; } // at alloc
        void			DecEltCount() { m_nEltCount--; } // at free
        PageOffset		GetOffsetInPage() { return PageOffset(m_offsetInPage)*4u; }
        void            SetOffsetInPage(PageOffset offset) { m_offsetInPage = u16(offset >> 2); }

        void            SetAsInvalid()
        {
            SetOffsetInPage(INVALID_OFFSET);
            SetSizeIndex(INVALID_SIZE_INDEX);
            SetFirstFreeElt(INVALID_ELT_INDEX);
        }

        bool            IsABucketHeader()
        {
            return GetOffsetInPage()!=INVALID_OFFSET;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Buckets size
    //////////////////////////////////////////////////////////////////////////
    // To get correct alignment handling, all power of 2 value until BUCKET_PAGE_SIZE_GRANULARITY should be there
    static const unsigned int nFirstSize= 4;
    static const unsigned int aSizeList[]= { nFirstSize, 8, 16, 24, 32, 48, 64, 80, 96, 128, 160, 192, 224, 256, 512, 816, 1024, 1360, 2048, 4096, 8192, 16384, 32768};
    static const unsigned int aAlignList[]= { nFirstSize,8, 16,  8, 16, 16, 16, 16, 16, 128,  16,  16,  16, 128, 128,  16,  128,   16,  128,  128,  128,   128,   128};

    // Address mapping info (set by Init)
    extern void * baseAddress;
    extern void * limitAddress;

    // base page allocation for buckets
    inline void * SystemPageAlloc(void * suggestedAddress, u32 nSize, u32 uAlign, u32 iEltSize)
    {
        return MemMngStd::Alloc(nSize, aAlignList[iEltSize]);
    }

    // system phys alloc
    inline void SystemPageFree(void * pAdrs)
    {
        return MemMngStd::Free(pAdrs);
    }

    inline u32 SystemPageSize(void * pAdrs)
    {
        return u32(BUCKET_PAGE_SIZE_GRANULARITY);
    }

    void Init();
    void Close();

    // constant

    ITF_COMPILE_TIME_ASSERT(Unexpected_BucketHeader_Size, sizeof(MemMngBucketsPlatform::BucketHeader)==10);
    ITF_COMPILE_TIME_ASSERT(Adressable_Memory_Not_Multiple_Of_256M, (BUCKET_MEMORY_ADRESSABLE/(256*1024*1024))*256*1024*1024==BUCKET_MEMORY_ADRESSABLE); // 256M for reserved mem

    static const u32 BUCKET_NB_SIZE = sizeof(aSizeList)/sizeof(aSizeList[0]);

    inline bool BucketSystemAvailable()
    {
        return true;
    }

    inline bool IsBucketRangeAddress(void * pData)
    {
        // check if we are in reserved area
        return (pData>=baseAddress && pData<=limitAddress);
    }

    inline u32 ConvertAlignment(u32 iSize, u32 uAlign)
    {
        if (uAlign>16)
        {
            return 128;
        }
        else
        {
            return uAlign;
        }
    }

    inline char * GetAddressFromPageIndex(u32 iPage)
    {
        return ((char *)baseAddress)+(iPage*BUCKET_PAGE_SIZE_GRANULARITY);
    }

    inline u32 GetPageIndexFromAddress(void * pAdrs)
    {
        return u32((char *)pAdrs-(char *)baseAddress)/BUCKET_PAGE_SIZE_GRANULARITY;
    }

}

#endif

} // namespace ITF

#endif // #ifndef ITF_CORE_MEMMNGBUCKETSPLATFORM_H_
