#ifndef __MEM_MANAGER_WII_H__
#define __MEM_MANAGER_WII_H__

#include "core/memory/WII/MEM_consts_WII.h"
#include "core/memory/WII/MEM_heap_WII.h"
#include "core/memory/WII/MEM_structs_WII.h"

#include <map>

namespace ITF
{

class MEM_Debug;
class MEM_tt_Trace;

#define MEM_DEFAULT_ALIGN	0

class MEM_Manager
{
public:
	// Init/Destroy
	MEM_Manager();
	~MEM_Manager();

	static void initStatics();

    static MEM_Manager *pGetInstance();

	static void s_Create();
	static void s_Destroy();

	// Alloc, realloc, free. 
	// Modes : normal, aligned, tmp.
	void *p_Alloc(u32 _uSize MEM_M_DebugArgsTypesDeclAlloc) 						{return p_AllocAlignExt(_uSize, MEM_DEFAULT_ALIGN, pGetFlagStack()->m_Flag MEM_M_DebugArgs);}
	void *p_AllocExt(u32 _uSize, u32 _uType MEM_M_DebugArgsTypesDeclAlloc)			{return p_AllocAlignExt(_uSize, MEM_DEFAULT_ALIGN, _uType MEM_M_DebugArgs);}
	void *p_Realloc(void *_p_OldAddress, u32 _uSize MEM_M_DebugArgsTypesDeclAlloc)	{return p_ReallocAlign(_p_OldAddress,_uSize,MEM_DEFAULT_ALIGN MEM_M_DebugArgs);}

	void Free(void *_p MEM_M_FreeArgsTypesDecl);
	
	void *p_AllocAlign(u32 _uSize, u32 _uAlign MEM_M_DebugArgsTypesDeclAligned) {return p_AllocAlignExt(_uSize, _uAlign, pGetFlagStack()->m_Flag MEM_M_DebugArgs);}
	void *p_AllocAlignExt(u32 _uSize, u32 _uAlign, u32 _uType MEM_M_DebugArgsTypesDeclAligned);
	void *p_ReallocAlign(void *_pOldAddress, u32 _uSize, u32 _uAlign MEM_M_DebugArgsTypesDeclAligned);
	
	void RegisterThread(); // Call this at beginning of thread
	void UnregisterThread(); // Call this at end of thread
	
	MEM_FlagStack *pGetFlagStack();

#ifdef USE_TEMP_HEAP
/*
 -----------------------------------------------------------------------------------------------------------------------
 	TEMP HEAPS
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    // Create a temporary heap.
    void CreateTempHeap(MEM_TempHeap _Heap,void *_pBeginAddress, u32 _uHeapSize);

    // Destroys a temporary heap if it is empty.    
    bbool bDestroyTempHeap(MEM_TempHeap _Heap);
    
    // If the address is in a temporary heap, get its index.
	u32 uGetTempHeapFromAddress(void *_p_Address);
	
	// Is the temporary heap available for allocs ? 
	bbool bIsTempHeapAvailable(MEM_TempHeap _Heap) {return m_TempHeaps[_Heap] != NULL;}
    
private:    
    // Temporary heaps (heaps that are created dynamically to take advantage of memory available temporarily).
	MEM_Heap 	*m_TempHeaps[MEM_TempHeapNb];

#endif //USE_TEMP_HEAP

public:
#ifdef _DEBUG_HEAP_
	// Alloc/free that can be used only in debug
	void *p_AllocForDebug(u32 _uSize);
	void *p_ReallocForDebug(void *_p,u32 _uSize);
	void FreeForDebug(void *_p);
#endif // _DEBUG_HEAP_
	
#ifdef _DEBUG_ALLOC_
	void *pGetBegMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetBegMemory();}
	void *pGetPoolMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetPoolMemory();}
	void *pGetEndMemory(u32 _uHeap) {return m_Heaps[_uHeap].pGetEndMemory();}
	
	// Used to parse the free blocks
	const MEM_FreeBlock *pGetFreeBlock(u32 _uHeap, u32 _uIndex) const {ITF_ASSERT_CRASH(_uIndex<MEM_FREE_BLOCKS_NB, "Bad index"); return m_Heaps[_uHeap].pGetFreeBlock(_uIndex);}
#endif // _DEBUG_ALLOC_

	// Validity functions
	bbool b_IsPointerInHeap(void *_p);
	bbool b_IsValidPointer(void *_p);
	bbool b_IsPointerInHeap(void *_p,u32 _uHeap) { return m_Heaps[_uHeap].b_IsPointerInHeap(_p);}
	bbool b_IsPointerInPool(void *_p,u32 _uHeap) {return m_Heaps[_uHeap].b_IsPointerInPool(_p);}
	bbool b_IsPointerValid(void *_p)
	{
		const u32 uHeap = uGetHeapFromAddress(_p);
		if(uHeap == MEM_HeapInvalid)
			return bfalse;
		return m_Heaps[uHeap].b_IsPointerValid(_p);
	}

#ifdef _HEAP_DEBUG_
	void CheckPointer(void *_p);
	void CheckAllocatedPointers();
#endif /* _HEAP_DEBUG_ */

	// Get sizes
#ifdef _HEAP_DEBUG_
	u32  u_GetFreeSize();
	u32  u_GetFreeCommittedSize();
	u32  u_GetTotalSize();
	u32  u_GetLastFreeSize();
	u32  u_GetBiggestAllocPossible();
	u32  u_GetAllocNb();
	u32  u_GetFreeNb();
	
	inline u32  u_GetHeapAllocSize(u32 _uHeap);
	inline u32  u_GetHeapAllocPoolSize(u32 _uHeap);
	inline u32  u_GetHeapFreeSize(u32 _uHeap);
	inline u32  u_GetHeapFreeCommittedSize(u32 _uHeap);
	inline u32  u_GetHeapLastFreeSize(u32 _uHeap);
	inline u32  u_GetHeapBiggestAllocPossible(u32 _uHeap);
	inline u32  u_GetHeapAllocNb(u32 _uHeap);
	inline u32  u_GetHeapAllocPoolNb(u32 _uHeap);
	inline u32  u_GetHeapFreeNb(u32 _uHeap);
	inline u32  u_GetHeapPoolSize(u32 _uHeap);	
#endif /* _HEAP_DEBUG_ */

#ifdef _HEAP_SIZE_DEBUG_
    u32  u_GetAllocSize();
    u32  u_GetPoolSize();	

    u32  u_GetTotalCommittedSize();
    u32  u_GetHeapCommittedSize(u32 _uHeap);

    inline u32  u_GetHeapTotalSize(u32 _uHeap);
#endif // _HEAP_SIZE_DEBUG_

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

    static MEM_Manager *ms_pInstance;
    static MEM_Manager ms_Instance;

/*
 -----------------------------------------------------------------------------------------------------------------------
 	NORMAL ALLOCS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void ReallocCopy(void *_pOldAddress, void *_pNewAddress,MEM_Heap *_pOldHeap, MEM_Heap *_pNewHeap);

#ifdef _HEAP_DEBUG_
	void VerifyAllocResult(void *_pResult, u32 _uSize, bbool _bDebugAlloc);
#endif /* _HEAP_DEBUG_ */

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
	MEM_FlagStack			 m_aFlags[eThreadId_count];
	
	// Thread Specific data access
	ThreadLocalStorage		 m_LS;

/*
 -----------------------------------------------------------------------------------------------------------------------
 	NORMAL ALLOCS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	// All memory heaps
	MEM_Heap 	m_Heaps[MEM_HeapTotalNb];
};

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline MEM_Manager *MEM_Manager::pGetInstance() 
{ 
    ITF_ASSERT_CRASH(ms_pInstance, "Must create manager first");
    return ms_pInstance;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline 	u32 MEM_Manager::uSafeGetHeapFromAddress(void *_p_Address) 
{
	u32 uHeap = uGetHeapFromAddress(_p_Address);
	ITF_ASSERT_MSG(uHeap != MEM_HeapInvalid, "ERROR : pointer 0x%08x is not in heaps", (u32) _p_Address);
	return uHeap;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline MEM_FlagStack *MEM_Manager::pGetFlagStack()
{
	return &m_aFlags[ m_LS.getValue() ];
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void MEM_Manager::CreateCriticalSection()
{
	ITF_ASSERT_CRASH(!Synchronize::IsInterupt(), "Forbid call");
	Synchronize::createCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void MEM_Manager::DestroyCriticalSection()
{
	ITF_ASSERT_CRASH(!Synchronize::IsInterupt(), "Forbid call");
	Synchronize::destroyCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void MEM_Manager::EnterCriticalSection()
{
	ITF_ASSERT_CRASH(!Synchronize::IsInterupt(), "Forbid call");
	Synchronize::enterCriticalSection(&m_MemCriticalSection);
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline void MEM_Manager::LeaveCriticalSection()
{
	ITF_ASSERT_CRASH(!Synchronize::IsInterupt(), "Forbid call");
	Synchronize::leaveCriticalSection(&m_MemCriticalSection);
} 
 
#ifdef _HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  MEM_Manager::u_GetHeapAllocSize(u32 _uHeap) 
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
inline u32  MEM_Manager::u_GetHeapAllocPoolSize(u32 _uHeap) 
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
inline u32  MEM_Manager::u_GetHeapFreeSize(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapFreeCommittedSize(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapLastFreeSize(u32 _uHeap)  
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
inline u32  MEM_Manager::u_GetHeapBiggestAllocPossible(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapAllocNb(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapAllocPoolNb(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapFreeNb(u32 _uHeap)
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
inline u32  MEM_Manager::u_GetHeapPoolSize(u32 _uHeap)
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetPoolSize();
	LeaveCriticalSection();
	return result;
}

#endif /* _HEAP_DEBUG_ */

#ifdef _HEAP_SIZE_DEBUG_
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
inline u32  MEM_Manager::u_GetHeapTotalSize(u32 _uHeap) 		
{ 
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetTotalSize();
	LeaveCriticalSection();
	return result;
}
#endif // _HEAP_SIZE_DEBUG_

} // namespace ITF

#endif // __MEM_MANAGER_WII_H__
