#pragma once
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _ITF_ALLOCATOR_H_
#define _ITF_ALLOCATOR_H_

#ifdef ENABLE_ALLOCATOR

#ifndef _ITF_ALLOCATOR_CONSTS_H_
#include "core/memory/allocator/allocator_consts.h"
#endif //_ITF_ALLOCATOR_CONSTS_H_

#ifndef _ITF_HEAP_H_
#include "core/memory/allocator/heap.h"
#endif //_ITF_HEAP_H_

#ifndef _ITF_PAGE_H_
#include "core/memory/allocator/page.h"
#endif //_ITF_PAGE_H_

//#include "THreads/THR_LocalStorage.h"
//#include "THreads/THR_Const.h"
//#include "THreads/THread_sync.h"
//#include <map>

//class MEM_Debug;
//class MEM_tt_Trace;
//class MEM_tt_TraceContext;

#define ITF_MEM_DEFAULT_ALIGN 0 // 16 : for debug

namespace ITF
{

struct FlagStack
{
	u32 m_Stack[ITF_MEM_DEFAULT_FLAG_STACK_SIZE];
	u32 m_Index;
	u32 m_Flag;
	bbool m_bIsUsed;
};

class Allocator 
{

// -----------------------------------------------------------------------------------------------------------------------
// 	PUBLIC INTERFACE
// -----------------------------------------------------------------------------------------------------------------------

public:
	// Init/Destroy
	Allocator();
	~Allocator();

	static void initStatics();

    static Allocator *pGetInstance();
	static bbool		bIsAvailable();

	static void s_Create();
	static void s_Destroy();

	// Alloc, realloc, free. 
	// Modes : normal, aligned, tmp.
	void *p_Alloc(u32 _uSize ITF_MEM_DebugArgsTypesDeclAlloc) 						{return p_AllocAlignExt(_uSize, ITF_MEM_DEFAULT_ALIGN, pGetFlagStack()->m_Flag ITF_MEM_DebugArgs);}
	void *p_AllocExt(u32 _uSize, u32 _uType ITF_MEM_DebugArgsTypesDeclAlloc)			{return p_AllocAlignExt(_uSize, ITF_MEM_DEFAULT_ALIGN, _uType ITF_MEM_DebugArgs);}
	void *p_Realloc(void *_p_OldAddress, u32 _uSize ITF_MEM_DebugArgsTypesDeclAlloc)	{return p_ReallocAlign(_p_OldAddress,_uSize,ITF_MEM_DEFAULT_ALIGN ITF_MEM_DebugArgs);}

	void Free(void *_p ITF_MEM_FreeArgsTypesDecl);
	
	void *p_AllocAlign(u32 _uSize, u32 _uAlign ITF_MEM_DebugArgsTypesDeclAligned) {return p_AllocAlignExt(_uSize, _uAlign, pGetFlagStack()->m_Flag ITF_MEM_DebugArgs);}
	void *p_AllocAlignExt(u32 _uSize, u32 _uAlign, u32 _uType ITF_MEM_DebugArgsTypesDeclAligned);
	void *p_ReallocAlign(void *_pOldAddress, u32 _uSize, u32 _uAlign ITF_MEM_DebugArgsTypesDeclAligned);

	void *p_AllocTmp(u32 _uSize ITF_MEM_DebugArgsTypesDeclAlloc);
	void FreeTmp(void *_p) {Free(_p ITF_MEM_FreeTmpArgs);}

	// Create container (for PS3)
#ifdef ITF_PS3
	int ContainerCreate(sys_memory_container_t *_pContainer,u32 _uSize ITF_ITF_MEM_ContainerCreateDecl);
	int ContainerDestroy(sys_memory_container_t _container);
	int ContainerGetSize(sys_memory_container_t _container);

#ifdef ITF_MEM_HEAP_DEBUG_
	// Total size and number of containers.
	u32 m_uContainerSize;
	u32 m_uContainerNb;
#endif //ITF_MEM_HEAP_DEBUG_
#endif // ITF_PS3
	
#ifdef ITF_USEFLAGSTACK
	void RegisterThread(); // Call this at beginning of thread
	void UnregisterThread(); // Call this at end of thread
#endif  //ITF_USEFLAGSTACK
	
	FlagStack *pGetFlagStack();

#ifdef ITF_USE_TEMP_HEAP
/*
 -----------------------------------------------------------------------------------------------------------------------
 	TEMP HEAPS
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    // Create a temporary heap.
    void CreateTempHeap(ITF_MEM_TempHeap _Heap,void *_pBeginAddress, u32 _uHeapSize);

    // Destroys a temporary heap if it is empty.    
    bbool bDestroyTempHeap(ITF_MEM_TempHeap _Heap);
    
    // If the address is in a temporary heap, get its index.
	u32 uGetTempHeapFromAddress(void *_p_Address);
	
	// Is the temporary heap available for allocs ? 
	bbool bIsTempHeapAvailable(ITF_MEM_TempHeap _Heap) {return m_TempHeaps[_Heap] != NULL;}
    
private:    
    // Temporary heaps (heaps that are created dynamically to take advantage of memory available temporarily).
	Heap 	*m_TempHeaps[ITF_MEM_TempHeapNb];

#endif //ITF_USE_TEMP_HEAP
    
/*
 -----------------------------------------------------------------------------------------------------------------------
 	PAGE ALLOCS
 -----------------------------------------------------------------------------------------------------------------------
 */
#if !defined(ITF_WII) && !defined(ITF_CTR)
public:
	void *p_AllocPage(ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlign, bbool _bWriteCombine ITF_MEM_DebugArgsTypesDeclPage);
	void FreePage(void *_pPage);
	u32 GetPageSize() {return m_Pages.uGetDefaultPageSize();}

private:
	void ReallocCopyPageToHeap(void *_p_OldAddress, void * _pNewAddress, u32 _uNewSize);
	void ReallocCopyHeapToPage(void *_p_OldAddress, u32 _uOldHeap, void *_pNewAddress);
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

#ifdef ITF_WINDOWS
	bbool m_bAllocInPages;
#endif

public:
#ifdef ITF_DEBUG_ALLOC_
	// Alloc/free that can be used only in debug
	void *p_AllocForDebug(u32 _uSize);
	void *p_ReallocForDebug(void *_p,u32 _uSize);
	void FreeForDebug(void *_p);
	
	void *pGetBegMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetBegMemory();}
	void *pGetTmpMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetTmpMemory();}
	void *pGetPoolMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetPoolMemory();}
	void *pGetEndMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetEndMemory();}
	
	// Used to parse the free blocks
	const FreeBlock *pGetFreeBlock(u32 _uHeap, u32 _uIndex) const {ITF_ASSERT(_uIndex<ITF_MEM_FREE_BLOCKS_NB); return m_Heaps[_uHeap].pGetFreeBlock(_uIndex);}

	// Debug infos
	void PushBFKey(u32 _uKey);
	void PopBFKey();
	void PushWorKey(u32 _uKey);
	void PopWorKey();
	void PushFileExt(const char *_sName);
	void PopFileExt();
	void PushCategory(const char *_sName);
	void PopCategory();
#endif // ITF_DEBUG_ALLOC_

	// Validity functions
	bbool b_IsPointerInHeap(void *_p);
	bbool b_IsValidPointer(void *_p);
	bbool b_IsPointerInHeap(void *_p,u32 _uHeap) { return m_Heaps[_uHeap].b_IsPointerInHeap(_p);}
	bbool b_IsPointerInPool(void *_p,u32 _uHeap) {return m_Heaps[_uHeap].b_IsPointerInPool(_p);}
	bbool b_IsPointerValid(void *_p)
	{
		const u32 uHeap = uGetHeapFromAddress(_p);
		if(uHeap == ITF_MEM_HeapInvalid)
			return bfalse;
		return m_Heaps[uHeap].b_IsPointerValid(_p);
	}

	// Track allocs code
#ifdef ITF_DEBUG_ALLOC_
	// For alloc blocks dumps
	void MemTrace(std::multimap<u32,MEM_tt_Trace>& _map, u32 _uFlags);
	u32 BeginMemDump(MEM_tt_Trace **_paTraceArray, u32 *_puArraySize, u32 _uFlags);
	void EndMemDump(MEM_tt_Trace *_aTraceArray);
	void SetStep(ITF_MEM_Step _Step); 
	void SetFileKey(u32 _FileKey);

	// For freed blocks dump
	void StartFreedBlocksTracking();
	void StopFreedBlocksTracking();
	void ComputeFreedMem(MEM_Snapshot *_pFreedMem);

	// For map leak tracking (compares allocations before synchronous map loadings)
	void ComputeMapLeaks(MEM_Snapshot *_pLeaks);
	void ComputeMapDynamic(MEM_Snapshot *_pLeaks);
    void StoreAfterLoading();

	// Used to parse the alloc block data
    u32 GetFirst() const;
    u32 GetNext(u32 _it) const;
    bbool bIsLast(u32 _it) const;
    void *pGetCallStack(u32 _it) const;
    void *pGetAddress(u32 _it) const;
#endif /* ITF_DEBUG_ALLOC_ */

#ifdef ITF_MEM_HEAP_DEBUG_
	void CheckPointer(void *_p);
	void CheckAllocatedPointers();
	void AssertMemPointer(bbool _bCondition, void *_pAddress, const char *_sMsg);
#endif /* ITF_MEM_HEAP_DEBUG_ */

	// Get sizes
#ifdef ITF_MEM_HEAP_DEBUG_
	u32  u_GetAllocSize();
	u32  u_GetFreeSize();
	u32  u_GetFreeCommittedSize();
	u32  u_GetTotalSize();
	u32  u_GetTotalCommittedSize();
	u32  u_GetHeapCommittedSize(u32 _uHeap);
	u32  u_GetLastFreeSize();
	u32  u_GetTmpSize();
	u32  u_GetBiggestAllocPossible();
	u32  u_GetAllocNb();
	u32  u_GetFreeNb();
	u32  u_GetPoolSize();	
#if !defined(ITF_WII) && !defined(ITF_CTR)
	u32  u_GetPageSize()		{return m_Pages.uGetPageSize();}
	u32  u_GetPageNb()			{return m_Pages.uGetPageNb();}
#endif // !defined(ITF_WII) && !defined(ITF_CTR)
#ifdef ITF_PS3
	u32  u_GetContainerSize()	{return m_uContainerSize;}
	u32  u_GetContainerNb()		{return m_uContainerNb;}
#endif //ITF_PS3
	
	inline u32  u_GetHeapAllocSize(u32 _uHeap);
	inline u32  u_GetHeapAllocPoolSize(u32 _uHeap);
	inline u32  u_GetHeapFreeSize(u32 _uHeap);
	inline u32  u_GetHeapFreeCommittedSize(u32 _uHeap);
	inline u32  u_GetHeapTotalSize(u32 _uHeap);
	inline u32  u_GetHeapLastFreeSize(u32 _uHeap);
	inline u32  u_GetHeapTmpSize(u32 _uHeap);
	inline u32  u_GetHeapBiggestAllocPossible(u32 _uHeap);
	inline u32  u_GetHeapAllocNb(u32 _uHeap);
	inline u32  u_GetHeapAllocPoolNb(u32 _uHeap);
	inline u32  u_GetHeapFreeNb(u32 _uHeap);
	inline u32  u_GetHeapPoolSize(u32 _uHeap);	
#endif /* ITF_MEM_HEAP_DEBUG_ */
	u32  GetSize(void *_p);

	inline u32 uSafeGetHeapFromAddress(void *_p_Address);
	u32 uGetHeapFromAddress(void *_p_Address);
	
	inline void EnterCriticalSection();
	inline void LeaveCriticalSection();
	

/*
 -----------------------------------------------------------------------------------------------------------------------
 	STATIC INSTANCE
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    void Create();
    void Destroy();

    static Allocator *ms_pInstance;
    static Allocator ms_Instance;

/*
 -----------------------------------------------------------------------------------------------------------------------
 	NORMAL ALLOCS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	u32 UpdateType(u32 _uType) { return (_uType & ITF_MEM_IA_ALLOC ? ((_uType | ITF_MEM_MEM1_ALLOC) & ~ITF_MEM_HARD_ALLOC) : _uType);}
	void ReallocCopy(void *_pOldAddress, void *_pNewAddress,Heap *_pOldHeap, Heap *_pNewHeap);


#ifdef ITF_MEM_HEAP_DEBUG_
	void VerifyAllocResult(void *_pResult, u32 _uSize, bbool _bDebugAlloc);
#endif /* ITF_MEM_HEAP_DEBUG_ */

/*
 -----------------------------------------------------------------------------------------------------------------------
 	CRITICAL SECTION - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	// Critical section management
	inline void CreateCriticalSection();
	inline void DestroyCriticalSection();

	ITF_THREAD_CRITICAL_SECTION m_MemCriticalSection;
/*
 -----------------------------------------------------------------------------------------------------------------------
 	FLAGS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	// Flag Stack. 0 : default stack (used during interruptions). 1 : main thread, 2 and above : other threads.
	FlagStack				 m_aFlags[eThreadId_count];
	
	// Thread Specific data access
	ThreadLocalStorage		 m_LS;
/*
 -----------------------------------------------------------------------------------------------------------------------
 	NORMAL ALLOCS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	// All memory heaps
	Heap 	m_Heaps[ITF_MEM_HEAP_TOTAL_NB];

public:
	Heap* getHeaps() { return m_Heaps; }

private:

#if !defined(ITF_WII) && !defined(ITF_CTR)
	// Pages (system allocations that use physical memory pages)
	Page	m_Pages;
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

/*
 -----------------------------------------------------------------------------------------------------------------------
 	DEBUG - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef ITF_MEM_HEAP_DEBUG_
#ifdef ITF_DEBUG_ALLOC_	
	// Where the debug information is stored
	MEM_Debug 	*m_pDebug;

public:
	MEM_Debug 	*pGetDebug() { return m_pDebug;}

	void vAddVRAMAddress(u32 _ID, u32 _uSize ITF_ITF_MEM_ContainerCreateDecl);
	void vRemoveVRAMAddress(u32 _ID);
#endif //ITF_DEBUG_ALLOC_	

    // Used by error management, to access memory in an interrupt. 
    bbool m_bIgnoreCriticalSections;
    
public :
    void SetIgnoreCriticalSections(bbool _bIgnoreCS) {m_bIgnoreCriticalSections = _bIgnoreCS;}
#endif // ITF_MEM_HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 	SYSTEM ALLOCS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
#ifdef ITF_WINDOWS
	HANDLE m_SystemHeap;
	void *pSystemAlloc(u32 _uSize, u32 _uAlign);
	void SystemFree(void *_pAddress);
	void *pSystemRealloc(void *_pOldAddress, u32 _uSize, u32 _uAlign);
#elif defined ITF_X360
	void *pSystemAlloc(u32 _uSize, u32 _uAlign, bbool _bWriteCombine, bbool _bLargePages);
	void SystemFree(void *_pAddress);
#endif

#ifdef MEM_TRACK_SYSTEM_ALLOCS
	MEM_SystemHeader *m_pFirstSysHeader; // Double linked list
#endif 

	void *pPostAlloc(void *_pSystemAddress, u32 _uSize, u32 _uAlign);
	void *pPreFree(void *_pAddress);
	void *pGetSystemAddress(void *_pAddress);
};


/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline Allocator *Allocator::pGetInstance() 
{ 
    ITF_ASSERT(ms_pInstance);
    return ms_pInstance;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline bbool Allocator::bIsAvailable() 
{ 
    return ms_pInstance != NULL;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline 	u32 Allocator::uSafeGetHeapFromAddress(void *_p_Address) 
{
	u32 uHeap = uGetHeapFromAddress(_p_Address);
	ITF_ASSERT_CRASH(uHeap != ITF_MEM_HeapInvalid,"ERROR : pointer 0x%08x is not in heaps", (u32) _p_Address);
	return uHeap;
}



/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline FlagStack *Allocator::pGetFlagStack()
{
	i32 iValue = m_LS.getValue();
	return &m_aFlags[iValue];
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void Allocator::CreateCriticalSection()
{
#ifdef ITF_MEM_HEAP_DEBUG_
    m_bIgnoreCriticalSections = bfalse;
#endif // ITF_MEM_HEAP_DEBUG_
	Synchronize::createCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void Allocator::DestroyCriticalSection()
{
	Synchronize::destroyCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void Allocator::EnterCriticalSection()
{
	Synchronize::enterCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void Allocator::LeaveCriticalSection()
{
	Synchronize::leaveCriticalSection(&m_MemCriticalSection);
} 
 
#ifdef ITF_MEM_HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapAllocSize(u32 _uHeap) 
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetAllocSize();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapAllocPoolSize(u32 _uHeap) 
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetAllocPoolSize();
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapFreeSize(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetFreeSize();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapFreeCommittedSize(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetFreeCommittedSize();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapTotalSize(u32 _uHeap) 		
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetTotalSize();
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapLastFreeSize(u32 _uHeap)  
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetLastFreeSize();
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapTmpSize(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetTmpSize();
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapBiggestAllocPossible(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetBiggestAllocPossible();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapAllocNb(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetAllocNb();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapAllocPoolNb(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetAllocPoolNb();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapFreeNb(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetFreeNb();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  Allocator::u_GetHeapPoolSize(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetPoolSize();
	LeaveCriticalSection();
	return result;
}

#endif /* ITF_MEM_HEAP_DEBUG_ */

} // namespace ITF

#endif //ENABLE_ALLOCATOR
#endif //_ITF_ALLOCATOR_H_
