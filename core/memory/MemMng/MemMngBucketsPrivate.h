#ifndef ITF_CORE_MEMMNGBUCKETSPRIVATE_H_
#define ITF_CORE_MEMMNGBUCKETSPRIVATE_H_

#pragma once

#include "MemMngBucketsPlatform.h"

namespace ITF
{

// check plateform configuration
ITF_COMPILE_TIME_ASSERT(Nb_Bucket_Size_Too_Low, MemMngBucketsPlatform::BUCKET_NB_SIZE>=1);


/*
"Bucket" is used to designate a pool of memory where we want to allocate several same size memory blocks
"Element" or the acronym "Elt" is used to designate a memory block in a bucket
"BucketHeader" is a header describing a bucket. BucketHeader maps the underlying memory area with the
granularity of our bucket pages.
Free elements are simply linked, using the allocation space.
Full buckets are doubly linked together in what we will call FullList (1 FullList in all the allocated stuff).
Buckets not full of same size are doubly linked together.
*/
namespace MemMngBuckets
{
    // locked when modifying the page list
    #define MEMMNG_AUTO_BUCKET_LOCK() ScopeLockMutex AL_csLock_(*g_pBucketMutex)
    extern Mutex * g_pBucketMutex;

    #define MEMMNG_LOCK_BY_SIZE
    #ifndef MEMMNG_LOCK_BY_SIZE
        #define MEMMNG_AUTO_LOCK_BY_SIZE(Bucket) MEMMNG_AUTO_BUCKET_LOCK()
    #else
        #define MEMMNG_AUTO_LOCK_BY_SIZE(Bucket) ScopeLockMutex AL_csLock_(*Bucket.GetLock())
    #endif

	// platform types 
	typedef MemMngBucketsPlatform::PageIndex	PageIndex;
	typedef MemMngBucketsPlatform::SizeIndex	SizeIndex;
	typedef MemMngBucketsPlatform::EltIndex		EltIndex;
	typedef MemMngBucketsPlatform::EltSize		EltSize;
    typedef MemMngBucketsPlatform::PageOffset   PageOffset;

	static const PageIndex	INVALID_PAGE_INDEX= MemMngBucketsPlatform::INVALID_PAGE_INDEX;
	static const EltIndex	INVALID_ELT_INDEX= MemMngBucketsPlatform::INVALID_ELT_INDEX;
	static const SizeIndex	INVALID_SIZE_INDEX= MemMngBucketsPlatform::INVALID_SIZE_INDEX;
    static const PageOffset INVALID_OFFSET= MemMngBucketsPlatform::INVALID_OFFSET;

//////////////////////////////////////////////////////////////////////////
// BucketHeader declaration
//////////////////////////////////////////////////////////////////////////
// We got 1 instance of class BucketHeader for each page of physical allocation
// These instance are in a static array in the class
class BucketHeader : public MemMngBucketsPlatform::BucketHeader
{
public :

	// elt
	EltSize	GetEltSize();
	u32 GetNbEltMax();

	// address <-> index
	PageIndex		GetPageIndex();
	char *			GetPageAddress();

	void 			SetOffsetFromAddress(void * addr);

	// Address of elt <-> index of elt 
	EltIndex		GetEltIndex(void * pAdrs); // pAdrs should be in the page
	void *			GetEltAdress(EltIndex iElt);

	// page management
	void			InitAfterPageAlloc(SizeIndex iSizeIndex);

	// Page links. These methods do not update m_pFirstFullList or pFirstNotFull (initial head ptr)
	// responsibility of this is in SizeBucketHeader
	void			Unlink();
	void			LinkBefore(PageIndex iNext); // should have been unlinked
	void			LinkAfter(PageIndex iPrev); // should have been unlinked
	void			MoveUpInList();
	void			MoveDownInList();

	// elt alloc/free -> these 2 methods update elt FreeList and change the page link
	void *			AllocElt();
	void			FreeElt(void *);

	// check
	void			CheckValidity();
	// dump mem
	void			DumpUsage(FILE * file);
	// DumpRequestedSize
	void			DumpRequestedSize(FILE * file);
	// DumpMemoryTracer
	void			DumpMemoryTracer(FILE * file);
	// ClearMemoryTracer
	void			ClearMemoryTracer();

	// static follow
public :
	static PageIndex GetBucketPageIndex(void * pAdrs);
	static void InitBucketHeaderTable();
	static BucketHeader & GetBucketHeaderFromAddress(void * pAdrs);

    static BucketHeader * GetBucketHeaderFromEltAddress(void * pAdrs);

	static BucketHeader * CreateBucket(SizeIndex iSizeIndex);
	static BucketHeader & GetBucketHeader(PageIndex iPage);

    static PageIndex GetFirstFreePage() {return m_nFirstFreePage; }
    static void SetFirstFreePage(PageIndex page) { m_nFirstFreePage = page; }

	static void	CheckArrayValidity();

	static const u32 NB_MAX_PAGE= MemMngBucketsPlatform::BUCKET_MEMORY_ADRESSABLE/MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY;

private:
	static BucketHeader m_aBucketHeaderTable[];		// it can be a big table
    static PageIndex m_nFirstFreePage;
};


//////////////////////////////////////////////////////////////////////////
// SizeBucketHeader declaration
//////////////////////////////////////////////////////////////////////////
// we got 1 instance of class SizeBucketHeader for each different size of buckets
// available.
// These instances are in a static array in the class
class SizeBucketHeader
{
private:
	friend struct LessSizeBucketHeaderFunc;

    Mutex * m_lock;
	PageIndex m_pFirstNotFull;
	EltSize m_nEltSize;

public:
	SizeIndex		GetIndex();
	BucketHeader *	GetBestBucketHeader(); // eventually allocate if none found
	void			InsertFullAsHeadInNotFullList(BucketHeader & headerBucket); // headerBucket was in full list, put it in head of the NotFullList

    void            DumpStats();
	void			CheckValidity();
    Mutex *         GetLock();

	// static
	static void						InitSizeBucketHeaderTable();

	static SizeBucketHeader *		GetBestBucketHeaderForSize(u32 iSize, u32 uAlign);

	static void						RemovePageFromAnyList(BucketHeader & headerBucket);
	static void						InsertHeadInFullList(BucketHeader & headerBucket);
	static void						MoveUpInNotFullList(BucketHeader & headerBucket);
	static void						MoveDownInNotFullList(BucketHeader & headerBucket);

	static u32					    GetSizeFromIndex(SizeIndex index);
    static SizeBucketHeader &		GetSizeBucketHeaderFromIndex(SizeIndex index);
	static SizeBucketHeader &		GetSizeBucketHeader(BucketHeader & headerBucket);

    static void                     DumpArrayStats();
	static void						CheckArrayValidity();
	static void						DumpUsage(FILE * file);
	static void						DumpRequestedSize(FILE * file);
	static void						DumpMemoryTracer(FILE * file);
	static void						ClearMemoryTracer();

private:
	static SizeBucketHeader m_aSizeBucketList[];
	static PageIndex m_pFirstFullList;
};

} // namespace MemMngBuckets

} // namespace ITF

#endif
