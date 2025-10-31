#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_log_WII.h"
#include "memory/WII/MEM_macros_WII.h"

namespace ITF
{

#ifdef _HEAP_DEBUG_
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static u32 CeilPowerOf(f32 _num, f32 _power)
{
	f32 res = 0;
	for(u32 i=1 ; res<_num ; ++i) res = powf(_power,(f32)i);
	return (u32)res;
}

const f32 f1Mo = 1024.f*1024.f;
const u32 u1Mo = 1024*1024;

static void MEM_LogHeap(MEM_tfn_LogCallback _pLogCb, u32 _uHeap)
{
	char heapName[16];

	switch(_uHeap)
	{
	case MEM_Heap1:
		strcpy(heapName,"MEM1   ");
		break;
	case MEM_Heap2:
		strcpy(heapName,"MEM2   ");
		break;
#ifdef _DEBUG_HEAP_	
	case MEM_HeapDebug:
		strcpy(heapName,"DEBUG  ");
		break;
#endif //_DEBUG_HEAP_	
	}

	f32 allocSize = (f32)MEM_GetAllocSize(_uHeap) / f1Mo;
	f32 allocPoolSize = (f32)MEM_GetAllocPoolSize(_uHeap) / f1Mo;
	f32 freeSize = (f32)MEM_GetFreeCommittedSize(_uHeap) / f1Mo;
	f32 freeBiggest = (f32)MEM_GetBiggestAllocPossible(_uHeap) / f1Mo;
	f32 freeLast = (f32)MEM_GetLastFreeSize(_uHeap)/ f1Mo;
	f32 uncommitted = (f32)(MEM_GetTotalSize(_uHeap)- MEM_GetITFHeapCommittedMemory (_uHeap))/ f1Mo;
	f32 holes = freeSize+uncommitted-freeBiggest;
	f32 totalSize = allocSize+freeSize;

	u32 allocNb = MEM_GetAllocNb(_uHeap);
	u32 allocPoolNb = MEM_GetAllocPoolNb(_uHeap);
	u32 freeNb = MEM_GetFreeNb(_uHeap);

	MEM_LogPrint(_pLogCb,"%s:%6.2f=%6.2f(%6d,%6.2f,%6d)+%6.2f(%6d,%6.2f,",
		heapName,
		totalSize,
		allocSize,allocNb,allocPoolSize,allocPoolNb,
		freeSize,freeNb,holes);

	MEM_LogPrint(_pLogCb,"%6.2f)\n",freeLast);
}

void MEM_LogUsage(MEM_tfn_LogCallback _pLogCb, u32 _uFlag)
{
	u32 i;
	bbool bDebug = (MEM_HeapTotalNb != MEM_HeapNb);

	if (_uFlag & MEM_C_LOG_USAGE_TITLE)
		MEM_LogPrint(_pLogCb,"*************** MEMORY USAGE **************\n");

	f32 allocSize,freeSize;
	f32 available=0;
	f32 debugSize = 0;
	u32 uTotalPhysicalMemory = MEM_GetTotalPhysicalMemory();
	u32 uSystemMemory = MEM_GetSystemPhysicalMemory();
	u32 debugSystemMemory = 0;

	if (bDebug)
		debugSize = MEM_GetTotalSize(2);
	allocSize = (MEM_GetAllocSize(MEM_Heap1) + MEM_GetAllocSize(MEM_Heap2));
	freeSize = (MEM_GetFreeSize(MEM_Heap1) + MEM_GetFreeSize(MEM_Heap2));
	if (_uFlag & MEM_C_LOG_USAGE_SUMMARY)
	{
		MEM_LogPrint(_pLogCb,"TOTAL = SYSTEM + ALLOC  +  FREE  ");
		if (bDebug)
			MEM_LogPrint(_pLogCb,"+  DEBUG");
		MEM_LogPrint(_pLogCb,"\n");
		MEM_LogPrint(_pLogCb,"%3dMB = %6.2f + %6.2f + %6.2f ",
			uTotalPhysicalMemory/u1Mo,
			uSystemMemory/f1Mo,
			allocSize/f1Mo,
			freeSize/f1Mo);
	}

	if (_uFlag & MEM_C_LOG_USAGE_SUMMARY)
	{
		if (bDebug)
			MEM_LogPrint(_pLogCb,"+ %6.2f\n",debugSize/f1Mo);
		else 
			MEM_LogPrint(_pLogCb,"\n");
	}

	if (_uFlag & MEM_C_LOG_USAGE_AVAILABLE)
	{
		f32 retailAvailable = MEM_GetTotalRetailPhysicalMemory() - (allocSize+uSystemMemory);
		MEM_LogPrint(_pLogCb,"AVAILABLE IN RETAIL = %6.2f\n",retailAvailable/f1Mo);
	}

	if (_uFlag & MEM_C_LOG_USAGE_HEAP)
	{
		// Display various heaps
		MEM_LogPrint(_pLogCb,"\nHEAP   : TOTAL= ALLOC(number,  pool,poolnb)+  FREE(number, holes,  last)\n");

		for(i=0 ; i<MEM_HeapTotalNb ; ++i)
		{
			MEM_LogHeap(_pLogCb,i);
		}
	}

	// Display system repartition (MEM1 vs MEM2)

	// MEM1 / MEM2
	u32 realRamSize[MEM_HeapNb]; // (24 / 64 or 128)
	f32 powers[MEM_HeapNb] = {24.f, 2.f};  //Highly specific to Wii !!!
	f32 sysSize[MEM_HeapNb]; 	// ram taken by system.

	for(i=0 ; i<MEM_HeapNb ; ++i)
	{
		f32 ramSize = (f32)MEM_GetTotalSize(i) / f1Mo;
		if (i==1 && MEM_HeapNb != MEM_HeapTotalNb)
			ramSize += (f32)MEM_GetTotalSize(2) / f1Mo; // Add debug heap size

		realRamSize[i] = CeilPowerOf(ramSize,powers[i]);
		sysSize [i] = (f32)realRamSize[i] - ramSize;
	}
	if (_uFlag & MEM_C_LOG_USAGE_SYSTEM)
	{
		MEM_LogPrint(_pLogCb,"\nSYSTEM=  MEM1+  MEM2\n");
		MEM_LogPrint(_pLogCb,"%6.2f=%6.2f+%6.2f\n",sysSize[0]+sysSize[1],sysSize[0],sysSize[1]);
	}
}

#endif // _HEAP_DEBUG_

} // namespace ITF
