#include "precompiled_core.h"
#include "MemMng.h"
#include "MemMngPrivate.h"
#include "MemMngTracer.h"

namespace ITF
{

//////////////////////////////////////////////////////////////////////////
// Out Of Mem
//////////////////////////////////////////////////////////////////////////
static MemMng::OutOfMemCB aOutOfMemCB[]= {0, 0, 0, 0};
static const u32 MAX_OUT_OF_MEM_CB = sizeof(aOutOfMemCB)/sizeof(aOutOfMemCB[0]);

bool MemMng::RegisterOutOfMemCallBack(OutOfMemCB pOOM)
{
	// 1 - search if not already there
	for(u32 i= 0; i<MAX_OUT_OF_MEM_CB; ++i)
	{
		if (aOutOfMemCB[i]==pOOM)
		{
			return false; // lets say we have not added it
		}
	}

	// 2 - insert
	for(u32 i= 0; i<MAX_OUT_OF_MEM_CB; ++i)
	{
		if (aOutOfMemCB[i]==NULL)
		{
			aOutOfMemCB[i]= pOOM;
			return true;
		}
	}

	// full
	return false;
}

bool MemMng::UnRegisterOutOfMemCallBack(OutOfMemCB pOOM)
{
	// 1 - search if not already there
	for(u32 i= 0; i<MAX_OUT_OF_MEM_CB; ++i)
	{
		if (aOutOfMemCB[i]==pOOM)
		{
			aOutOfMemCB[i]= NULL;
			return true; // lets say we have not added it
		}
	}
	// not found
	return false;
}



namespace MemMngDebug
{
	//////////////////////////////////////////////////////////////////////////
	// What to do when OOM ?
	//////////////////////////////////////////////////////////////////////////
#if MEMNG_TEST_OUT_OF_MEM==1
	void OutOfMem(u32 nSize)
	{
#if MEMMNG_MEMORY_TRACER==1
        MemMngTracer::Dump();
        MemMngTracer::FlushLog(true);
#endif
		fprintf(stdout, "Out of mem size=%d\n", nSize);
		MEMMNG_ASSERT(0); // OUT OF MEM
		fprintf(stdout, "Dumping mem usage\n");
		for(u32 i= 0; i<MAX_OUT_OF_MEM_CB; ++i)
		{
			if (aOutOfMemCB[i]!=NULL)
			{
				(*aOutOfMemCB[i])(nSize);
			}
		}
	}
#endif
}


//////////////////////////////////////////////////////////////////////////
// Memory counts access
//////////////////////////////////////////////////////////////////////////

MemMng::MemCounts * g_MemUsageStat; // global to get easy debug access

const MemMng::MemCounts & MemMng::GetMemCounts()
{
    return *g_MemUsageStat;
}

#if MEMMNG_MEMORY_TRACER==1
    void	MemMng::DumpMemoryTracer()
    {
        MemMngTracer::Dump();
    }
#else // MEMMNG_MEMORY_TRACER
    void	MemMng::DumpMemoryTracer()
    {
    }
#endif // MEMMNG_MEMORY_TRACER



    //////////////////////////////////////////////////////////////////////////
    // QUARANTINE
    //////////////////////////////////////////////////////////////////////////
    namespace MemMngDebug
    {
    #if MEMMNG_QUARANTINE==1

        // init stuff for the current policy in the quarantine
        static const size_t nQuarantineAleaSizeArray[] = { 0, 4, 8, 16, 18, 20, 24, 32, 48, 64, 80, 96, 128, 160, 192, 224, 256, 512, 1024, 2048, 4096, 8192, 16 * 1024 };
        static unsigned int nQuarantineSizeIndex;

        static const size_t nPageSize = 16 * 1024u; // keep it power of 2

        MemoryPageManager* g_quarantineMemMng = nullptr;

        // init quarantine stuff
        void InitQuarantine()
        {
            srand((unsigned int)(time(NULL)));
            nQuarantineSizeIndex = 1 + rand() % ((sizeof(nQuarantineAleaSizeArray) - 1) / sizeof(nQuarantineAleaSizeArray[0])); // avoid 0

            static MemoryPageManager quarantineMemMng;
            g_quarantineMemMng = &quarantineMemMng;
            g_quarantineMemMng->Init(MemoryPageManager::InitParams().setReserveVirtualSpace(bfalse));
        }

        void* AllocInQuarantine(u32 iSize, u32 uAlign)
        {
            // exclude incorrect page size and alignment
            if (iSize > nPageSize || uAlign != 0)
                return nullptr;

            // policy to alloc
            if (iSize <= nQuarantineAleaSizeArray[nQuarantineSizeIndex - 1] || iSize > nQuarantineAleaSizeArray[nQuarantineSizeIndex])
                return nullptr;

            u8* pageAddr = g_quarantineMemMng->AllocArea(nPageSize * 2);
            if (!pageAddr)
                return nullptr;
            uPtr uPageAddr = reinterpret_cast<uPtr>(pageAddr);
            uPtr uBlockAddr = uPageAddr + nPageSize - iSize;
            uBlockAddr = BitTweak::GetBelowAlign(uBlockAddr, uPtr(uAlign));
            // now remove read/write access to the second page
            bbool ret = g_quarantineMemMng->setMemoryProtection(pageAddr + nPageSize, nPageSize, 0);
            MEMMNG_ASSERT(ret);
            return reinterpret_cast<u8*>(uBlockAddr);
        }

        void FreeInQuarantine(void* pDataInQuarantine)
        {
            u8* pageAddr = BitTweak::GetBelowAlign((u8*)pDataInQuarantine, nPageSize);
            g_quarantineMemMng->DecommitPages(pageAddr, nPageSize * 2);
        }

        unsigned int SizeInQuarantine(void* pDataInQuarantine)
        {
            return 2 * nPageSize;
        }

        bool FindInQuarantine(void* pDataInQuarantine)
        {
            return g_quarantineMemMng->isInRange((u8*)pDataInQuarantine);
        }

    #endif // MEMMNG_QUARANTINE==1
    } // namespace MemMngDebug

} // namespace ITF
