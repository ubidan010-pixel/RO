#include "precompiled_core.h"
#include "MemMngPrivate.h"
#include "MemMngBucketsPrivate.h"

namespace ITF
{

namespace MemMngBuckets
{

	//////////////////////////////////////////////////////////////////////////
	// validity checks
	//////////////////////////////////////////////////////////////////////////

	void BucketHeader::CheckArrayValidity()
	{
		for(u32 i= 0; i<BucketHeader::NB_MAX_PAGE; i++)
		{
			m_aBucketHeaderTable[i].CheckValidity();
		}
	}

	void BucketHeader::CheckValidity()
	{
		MEMMNG_ASSERT(GetSizeIndex()!=INVALID_SIZE_INDEX
			|| (GetEltCount()==0 && GetSizeIndex()==INVALID_SIZE_INDEX && GetFirstFreeElt()==INVALID_ELT_INDEX));

		// check freelist ending
		u32 i= 0;
		EltIndex iElt= GetFirstFreeElt();
		MEMMNG_ASSERT(iElt==INVALID_ELT_INDEX || iElt<GetNbEltMax());
        while(iElt!=INVALID_ELT_INDEX && i<MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY/MemMngBucketsPlatform::aSizeList[0])
		{
			i++;
			EltIndex ei= iElt;
			iElt= *((EltIndex *)GetEltAdress(iElt));
			MEMMNG_ASSERT(iElt==INVALID_ELT_INDEX || iElt<GetNbEltMax());
			if (iElt!=INVALID_ELT_INDEX && iElt>=GetNbEltMax())
			{
				fprintf(stdout, "Error on element %d in the freelist\n", i);
				fprintf(stdout, "EltSizeIndex= %d EltCount=%d FirstFreeElt=%d\n", GetEltSize(), GetEltCount(), GetFirstFreeElt());
				fprintf(stdout, "Probably a write at address 0x%p with the 32b Word 0x%X\n", GetEltAdress(ei), *((u32 *)(GetEltAdress(ei))));
			}
		};
		MEMMNG_ASSERT(iElt==INVALID_ELT_INDEX);

		// check elt count
		if (GetSizeIndex()!=INVALID_SIZE_INDEX)
		{
			MEMMNG_ASSERT(GetEltCount()<=GetNbEltMax());
		}

		// check size index
		if (GetSizeIndex()!=INVALID_SIZE_INDEX)
		{
			MEMMNG_ASSERT(GetSizeIndex()<MemMngBucketsPlatform::BUCKET_NB_SIZE);
		}

		// check prev->next==this and next->prev==this
		MEMMNG_ASSERT(GetPrevPage()==INVALID_PAGE_INDEX || GetBucketHeader(GetPrevPage()).GetNextPage()==GetPageIndex());
		MEMMNG_ASSERT(GetNextPage()==INVALID_PAGE_INDEX || GetBucketHeader(GetNextPage()).GetPrevPage()==GetPageIndex());
	}

    void    SizeBucketHeader::DumpArrayStats()
    {
        u32 i;
        for (i= 0; i<MemMngBucketsPlatform::BUCKET_NB_SIZE; i++)
        {
            m_aSizeBucketList[i].DumpStats();
        }
    }

	void	SizeBucketHeader::CheckArrayValidity()
	{
		// array
		u32 i;
		for (i= 0; i<MemMngBucketsPlatform::BUCKET_NB_SIZE; i++)
		{
			m_aSizeBucketList[i].CheckValidity();
		}

		// full list
        {
		    i= 0;
		    PageIndex iFullPageCurrent= m_pFirstFullList;
		    PageIndex iFullPagePrev= INVALID_PAGE_INDEX;
		    while(iFullPageCurrent!=INVALID_PAGE_INDEX && i<BucketHeader::NB_MAX_PAGE)
		    {
			    BucketHeader & headerBucket= BucketHeader::GetBucketHeader(iFullPageCurrent);
			    MEMMNG_ASSERT(headerBucket.GetPrevPage()==iFullPagePrev);
			    MEMMNG_ASSERT(headerBucket.GetEltCount()==headerBucket.GetNbEltMax());
			    iFullPagePrev= iFullPageCurrent;
			    iFullPageCurrent= headerBucket.GetNextPage();
			    i++;
		    }
		    MEMMNG_ASSERT(iFullPageCurrent==INVALID_PAGE_INDEX);
        }
	}

    void	SizeBucketHeader::DumpStats()
    {
        u32 memAsked = 0;
        u32 memUsed = 0;

        PageIndex iNotFullPageCurrent= m_pFirstNotFull;
        u32 i= 0;
        while(iNotFullPageCurrent!=INVALID_PAGE_INDEX && i<BucketHeader::NB_MAX_PAGE)
        {
            BucketHeader & headerBucket= BucketHeader::GetBucketHeader(iNotFullPageCurrent);
            
            memAsked += headerBucket.GetEltCount()*headerBucket.GetEltSize();
            memUsed += MemMngBucketsPlatform::BUCKET_PAGE_SIZE_GRANULARITY;
            iNotFullPageCurrent= headerBucket.GetNextPage();
            ++i;
        }

        fprintf(stdout, "%d  asked : %d  used : %d\n", m_nEltSize, memAsked, memUsed);
    }

	void	SizeBucketHeader::CheckValidity()
	{
		MEMMNG_ASSERT(m_nEltSize<=MemMngBucketsPlatform::aSizeList[MemMngBucketsPlatform::BUCKET_NB_SIZE-1]);
		u32 i= 0;
		PageIndex iNotFullPageCurrent= m_pFirstNotFull;
		PageIndex iNotFullPagePrev= INVALID_PAGE_INDEX;
		while(iNotFullPageCurrent!=INVALID_PAGE_INDEX && i<BucketHeader::NB_MAX_PAGE)
		{
			BucketHeader & headerBucket= BucketHeader::GetBucketHeader(iNotFullPageCurrent);
			MEMMNG_ASSERT(headerBucket.GetPrevPage()==iNotFullPagePrev); // test prev
/*			MEMMNG_ASSERT(	iNotFullPagePrev==INVALID_PAGE_INDEX ||
				headerBucket.GetEltCount()<=BucketHeader::GetBucketHeader(iNotFullPagePrev).GetEltCount());*/
			iNotFullPagePrev= iNotFullPageCurrent;
			iNotFullPageCurrent= headerBucket.GetNextPage();
			i++;
		}
		MEMMNG_ASSERT(iNotFullPageCurrent==INVALID_PAGE_INDEX);
	}
} // namespace MemMngBuckets
} // namespace ITF
