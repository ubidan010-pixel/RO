#ifndef __MEM_MACROS_WII_H__
#define __MEM_MACROS_WII_H__

#include "core/memory/WII/MEM_consts_WII.h"
#include "core/memory/WII/MEM_manager_WII.h"
#include "core/memory/WII/MEM_log_WII.h"
 
/*
 =======================================================================================================================
 Common macros
 =======================================================================================================================
 */

namespace ITF
{

#define MEMory()			MEM_Manager::pGetInstance()

inline void  MEM_M_Create()								{MEM_Manager::s_Create();}
inline void  MEM_M_Destroy()							{MEM_Manager::s_Destroy();}
inline bbool MEM_M_IsPointerInHeap(void *p)				{return MEMory()->b_IsPointerInHeap(p);}
inline bbool MEM_M_IsPointerInHeap(void *p, u32 _uHeap)	{return MEMory()->b_IsPointerInHeap(p,_uHeap);}
inline u32   MEM_M_GetSize(void *_p_Address)			{return MEMory()->GetSize(_p_Address);}
inline bbool MEM_M_IsValidPointer(void *p)				{return MEMory()->b_IsValidPointer(p);}

#ifdef _HEAP_SIZE_DEBUG_
// Get the total size of the physical memory (in bytes) : available, committed by ITF or by the system.
extern u32 MEM_GetTotalPhysicalMemory();

// Get the total size of the physical memory (in bytes), on retail systems
extern u32 MEM_GetTotalRetailPhysicalMemory();

// Get the number of bytes that have been committed by the system (exe, external libs, operating system, etc.).
extern u32 MEM_GetSystemPhysicalMemory();

// Get the number of bytes that have been committed by ITF, all in all.
inline u32 MEM_GetITFTotalCommittedMemory()		{return MEMory()->u_GetTotalCommittedSize();}

inline u32 MEM_M_AllocSize()			        {return MEMory()->u_GetAllocSize();}
inline u32 MEM_GetTotalSize(u32 _uHeap)			{return MEMory()->u_GetHeapTotalSize(_uHeap);}
#endif // _HEAP_SIZE_DEBUG_

#ifdef _HEAP_DEBUG_
inline u32 MEM_M_LastFreeSize()			{return MEMory()->u_GetLastFreeSize();}
inline u32 MEM_M_TotalSize()			{return MEMory()->u_GetTotalSize();}
inline u32 MEM_M_FreeSize()				{return MEMory()->u_GetFreeSize();}

inline u32 MEM_GetFreeCommittedSize()	{return MEMory()->u_GetFreeCommittedSize();}

// Get the number of bytes that have been committed by ITF in a given heap.
inline u32 MEM_GetITFHeapCommittedMemory(u32 _uHeap)	{return MEMory()->u_GetHeapCommittedSize(_uHeap);}

inline u32 MEM_GetAllocSize(u32 _uHeap)			{return MEMory()->u_GetHeapAllocSize(_uHeap);}
inline u32 MEM_GetLastFreeSize(u32 _uHeap)		{return MEMory()->u_GetHeapLastFreeSize(_uHeap);}
inline u32 MEM_GetFreeSize(u32 _uHeap)			{return MEMory()->u_GetHeapFreeSize(_uHeap);}
inline u32 MEM_GetFreeCommittedSize(u32 _uHeap) {return MEMory()->u_GetHeapFreeCommittedSize(_uHeap);}
inline u32 MEM_GetAllocPoolSize(u32 _uHeap)		{return MEMory()->u_GetHeapAllocPoolSize(_uHeap);}

inline u32 MEM_GetBiggestAllocPossible(u32 _uHeap)	{return MEMory()->u_GetHeapBiggestAllocPossible(_uHeap);}
inline u32 MEM_GetAllocNb(u32 _uHeap)				{return MEMory()->u_GetHeapAllocNb(_uHeap);}
inline u32 MEM_GetAllocPoolNb(u32 _uHeap)			{return MEMory()->u_GetHeapAllocPoolNb(_uHeap);}
inline u32 MEM_GetFreeNb(u32 _uHeap)				{return MEMory()->u_GetHeapFreeNb(_uHeap);}

#define MEM_u_TotalSize 			MEM_M_TotalSize
#define MEM_u_GetTotalFreeSpaceSize MEM_M_FreeSize
#define MEM_u_GetTotalFreeSize		MEM_M_FreeSize
#define MEM_u_GetTotalSortedSize	MEM_M_FreeSize
#endif /* _HEAP_DEBUG_ */

inline void MEM_BeginCriticalSection() 	{ MEMory()->EnterCriticalSection();}
inline void MEM_EndCriticalSection() 	{ MEMory()->LeaveCriticalSection();}

#ifdef _HEAP_DEBUG_
#define MEM_M_CheckAllocatedPointers() 	MEMory()->CheckAllocatedPointers()
#else /* _HEAP_DEBUG_ */
#define MEM_M_CheckAllocatedPointers() 	
#endif /* _HEAP_DEBUG_ */

/*
 =======================================================================================================================
 Alloc macros
 =======================================================================================================================
 */
 
#define MEM_M_Alloc(__S)							MEMory()->p_Alloc((__S))
#define MEM_M_AllocExt(__S, __T)					MEMory()->p_AllocExt((__S), (__T))

#define MEM_M_AllocAlign(__S, __A)					MEMory()->p_AllocAlign((__S), (__A))
#define MEM_M_AllocAlignExt(__S, __A, __T)			MEMory()->p_AllocAlignExt((__S), (__A), (__T))

#define MEM_M_Realloc(__P, __S)						MEMory()->p_Realloc((__P), (__S))
#define MEM_M_ReallocAlign(__P, __S, __A)			MEMory()->p_ReallocAlign((__P), (__S), (__A))

#define MEM_M_Free(__P)								MEMory()->Free(__P)
#define MEM_M_FreeAlign(__P)						MEMory()->Free(__P)

#define MEM_M_Size(__P)						        MEMory()->GetSize(__P)

#define MEM_M_dbgAllocExtendedCheckType(_u32_Type) \
	ITF_ASSERT_CRASH((_u32_Type & (MEM_C_MEM1_ALLOC | MEM_C_MEM2_ALLOC)) != (MEM_C_MEM1_ALLOC | MEM_C_MEM2_ALLOC), "Type error")

/*
 =======================================================================================================================
 DEFAULT FLAG MANAGEMENT
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MEM_M_PushExt(u32 _uType)
{
	MEM_FlagStack *pFlagStack = MEMory()->pGetFlagStack();
	
	ITF_ASSERT_CRASH(pFlagStack->m_Index + 1 < MEM_DefaultFlagStackSize, "Wrong stack"); 
	pFlagStack->m_Flag = _uType;
	pFlagStack->m_Stack[++pFlagStack->m_Index] = _uType; 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MEM_M_PopExt()
{
	MEM_FlagStack *pFlagStack = MEMory()->pGetFlagStack();
	
	if(pFlagStack->m_Index <= 0) 
	{ 
		pFlagStack->m_Flag = 0; 
		pFlagStack->m_Index = (u32)-1; 
	} 
	else 
	{
		pFlagStack->m_Flag = pFlagStack->m_Stack[--pFlagStack->m_Index]; 
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline u32 MEM_GetFlag()
{
	MEM_FlagStack *pFlagStack = MEMory()->pGetFlagStack();
	ITF_ASSERT_CRASH(pFlagStack, "Wrong stack");
	return pFlagStack->m_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MEM_RegisterThread()
{
	MEMory()->RegisterThread();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void MEM_UnregisterThread()
{
	MEMory()->UnregisterThread();
}

/*
 =======================================================================================================================
 OPTIMIZED CLEARED FUNCTION
 =======================================================================================================================
 */

/*!
Clear dest.
*/
inline void MEM_M_DCZeroRangeSafe(void *_p_Dest, u32 _u32_Size)
{
    void       *pDestUp  = (void*)OSRoundUp32B(_p_Dest);
    const i32  uSizeDown = (i32) OSRoundDown32B(_u32_Size - ((u32)pDestUp-(u32)_p_Dest));
    
    if(uSizeDown>32)
        DCZeroRange(pDestUp, (u32)uSizeDown);
}

/*!
Clear dest and put it in cache.
*/
inline void MEM_M_clearmemDC(void *_p_Dest, u32 _u32_Size)
{
    void       *pDestUp  = (void*)OSRoundUp32B(_p_Dest);
    const i32  uSizeDown = (i32) OSRoundDown32B(_u32_Size - ((u32)pDestUp-(u32)_p_Dest));
    
    if(uSizeDown>32)
    {
        const u32 dDest = (u32)pDestUp - (u32)_p_Dest;        
        DCZeroRange(pDestUp, (u32)uSizeDown);
        
        for(u32 i = 0; i < dDest; ++i)
            ((u8*)_p_Dest)[i] = 0;
            
        for(u32 i = (u32)uSizeDown; i < _u32_Size; ++i)
            ((u8*)_p_Dest)[i] = 0;
    }
    else
        ITF_MemSet(_p_Dest, 0, _u32_Size);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef _HEAP_DEBUG_
#define MEM_CHECK_BEFORE_ALLOC  0x00000001
#define MEM_CHECK_BEFORE_FREE   0x00000002
#define MEM_CHECK_AFTER_ALLOC   0x00000004
#define MEM_CHECK_AFTER_FREE    0x00000008
extern u32 MEM_uCheckState;
#define MEM_CHECK(pos) { if (pos & MEM_uCheckState) MEM_M_CheckAllocatedPointers();}
inline void  MEM_SetCheckStatus(u32 _Status) { MEM_uCheckState = _Status;}
#else //_HEAP_DEBUG_
#define MEM_CHECK(pos) 
#define MEM_SetCheckStatus(_Status)
#endif //_HEAP_DEBUG_

} // namespace ITF

#endif // __MEM_MACROS_WII_H__