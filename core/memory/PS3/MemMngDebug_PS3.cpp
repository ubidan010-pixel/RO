#include "precompiled_core.h"
#include "MemMng_PS3.h"
#include "MemMngPrivate_PS3.h"
#include "MemMngTracer_PS3.h"

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

} // namespace ITF
