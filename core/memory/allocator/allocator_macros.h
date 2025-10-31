#pragma once

#ifndef _ITF_ALLOCATOR_MACROS_H_
#define _ITF_ALLOCATOR_MACROS_H_

#ifdef ENABLE_ALLOCATOR
//#include "STanDard/STD_mem.h"

#ifndef _ITF_ALLOCATOR_H_
#include "core/memory/allocator/allocator.h"
#endif //_ITF_ALLOCATOR_H_

#ifndef _ITF_ALLOCATOR_CONSTS_H_
#include "core/memory/allocator/allocator_consts.h"
#endif //_ITF_ALLOCATOR_CONSTS_H_

//#include "ALLOCATOR/MEM_log.h"

namespace ITF
{

/*
 =======================================================================================================================
 Common macros
 =======================================================================================================================
 */

#define ALLOCATOR()				Allocator::pGetInstance()
#define ALLOCATOR_Available()	Allocator::bIsAvailable()

inline void  ITF_MEM_Create()								{Allocator::s_Create();}
inline void  ITF_MEM_Destroy()								{Allocator::s_Destroy();}
inline bbool ITF_MEM_IsPointerInHeap(void *p)				{return ALLOCATOR()->b_IsPointerInHeap(p);}
inline bbool ITF_MEM_IsPointerInHeap(void *p, u32 _uHeap)	{return ALLOCATOR()->b_IsPointerInHeap(p,_uHeap);}
inline u32   ITF_MEM_GetSize(void *_p_Address)			    {return ALLOCATOR()->GetSize(_p_Address);}
inline bbool ITF_MEM_IsValidPointer(void *p)				{return ALLOCATOR()->b_IsValidPointer(p);}

// Get the total size of the physical memory (in bytes) : available, committed by LyN or by the system.
extern u32 ITF_MEM_GetTotalPhysicalMemory();

// Get the total size of the physical memory (in bytes), on retail systems
extern u32 ITF_MEM_GetTotalRetailPhysicalMemory();

// Get the number of bytes that can be used (that are in uncommitted pages).
extern u32 ITF_MEM_GetAvailablePhysicalMemory();

// Get the number of bytes that have been committed by the system (exe, external libs, operating system, etc.).
extern u32 ITF_MEM_GetSystemPhysicalMemory();

#ifdef ITF_MEM_HEAP_DEBUG_
inline u32 ITF_MEM_AllocSize()				{return ALLOCATOR()->u_GetAllocSize();}
inline u32 ITF_MEM_LastFreeSize()			{return ALLOCATOR()->u_GetLastFreeSize();}
inline u32 ITF_MEM_TotalSize()				{return ALLOCATOR()->u_GetTotalSize();}
inline u32 ITF_MEM_FreeSize()				{return ALLOCATOR()->u_GetFreeSize();}

inline u32 ITF_MEM_GetFreeCommittedSize()	{return ALLOCATOR()->u_GetFreeCommittedSize();}

// Get the number of bytes that have been committed by LyN, all in all.
inline u32 ITF_MEM_GetTotalCommittedMemory()			{return ALLOCATOR()->u_GetTotalCommittedSize();}

// Get the number of bytes that have been committed by LyN in a given heap.
inline u32 ITF_MEM_GetHeapCommittedMemory(u32 _uHeap)	{return ALLOCATOR()->u_GetHeapCommittedSize(_uHeap);}

#if !defined(ITF_WII) && !defined(ITF_CTR)
// Get memory that have been committed by LyN in pages.
inline u32 ITF_MEM_GetPageSize()				{return ALLOCATOR()->u_GetPageSize();}
inline u32 ITF_MEM_GetPageNb()				{return ALLOCATOR()->u_GetPageNb();}
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

// Get memory that have been committed by LyN in containers.
#ifdef ITF_PS3
inline u32 ITF_MEM_GetContainerSize()		{return ALLOCATOR()->u_GetContainerSize();}
inline u32 ITF_MEM_GetContainerNb()			{return ALLOCATOR()->u_GetContainerNb();}
#endif //ITF_PS3

inline u32 ITF_MEM_GetTotalSize(u32 _uHeap)		{return ALLOCATOR()->u_GetHeapTotalSize(_uHeap);}
inline u32 ITF_MEM_GetAllocSize(u32 _uHeap)		{return ALLOCATOR()->u_GetHeapAllocSize(_uHeap);}
inline u32 ITF_MEM_GetLastFreeSize(u32 _uHeap)	{return ALLOCATOR()->u_GetHeapLastFreeSize(_uHeap);}
inline u32 ITF_MEM_GetFreeSize(u32 _uHeap)		{return ALLOCATOR()->u_GetHeapFreeSize(_uHeap);}
inline u32 ITF_MEM_GetFreeCommittedSize(u32 _uHeap) {return ALLOCATOR()->u_GetHeapFreeCommittedSize(_uHeap);}
inline u32 ITF_MEM_GetAllocPoolSize(u32 _uHeap)	{return ALLOCATOR()->u_GetHeapAllocPoolSize(_uHeap);}

inline u32 ITF_MEM_GetBiggestAllocPossible(u32 _uHeap)	{return ALLOCATOR()->u_GetHeapBiggestAllocPossible(_uHeap);}
inline u32 ITF_MEM_GetAllocNb(u32 _uHeap)		{return ALLOCATOR()->u_GetHeapAllocNb(_uHeap);}
inline u32 ITF_MEM_GetAllocPoolNb(u32 _uHeap)	{return ALLOCATOR()->u_GetHeapAllocPoolNb(_uHeap);}
inline u32 ITF_MEM_GetFreeNb(u32 _uHeap)		{return ALLOCATOR()->u_GetHeapFreeNb(_uHeap);}

#define MEM_u_TotalSize 			ITF_MEM_TotalSize
#define MEM_u_GetTotalFreeSpaceSize ITF_MEM_FreeSize
#define MEM_u_GetTotalFreeSize		ITF_MEM_FreeSize
#define MEM_u_GetTotalSortedSize	ITF_MEM_FreeSize

inline u32 MEM_u_GetTotalTempSize()		{return ALLOCATOR()->u_GetTmpSize();}
inline u32 MEM_u_GetTotalUnsortedSize() {return 0;}
#endif /* ITF_MEM_HEAP_DEBUG_ */

inline void MEM_BeginCriticalSection() 	{ ALLOCATOR()->EnterCriticalSection();}
inline void MEM_EndCriticalSection() 	{ ALLOCATOR()->LeaveCriticalSection();}

#ifdef ITF_MEM_HEAP_DEBUG_
#define ITF_MEM_CheckAllocatedPointers() 	ALLOCATOR()->CheckAllocatedPointers()
#else /* ITF_MEM_HEAP_DEBUG_ */
#define ITF_MEM_CheckAllocatedPointers() 	
#endif /* ITF_MEM_HEAP_DEBUG_ */

#ifdef ITF_DEBUG_ALLOC_
#define ITF_MEM_SetStep(s) 				ALLOCATOR()->SetStep(s)
#define ITF_MEM_SetFileKey(k) 		    ALLOCATOR()->SetFileKey(k)
#else /* MEM_USE_DEBUG */
#define ITF_MEM_SetStep(s) 				
#define ITF_MEM_SetFileKey(k)
#endif /* ITF_DEBUG_ALLOC_ */

#define ITF_MEM_TrackLeaks()	//			MEM_LogDumpToDebugger(ITF_MEM_LOG_DUMP_LEAKS)

#define ITF_MEM_Delete			delete
#define ITF_MEM_DeleteArray		delete []
#define ITF_MEM_NewPlacement(pt)	new(pt)


/*
 =======================================================================================================================
 Macros for macros
 =======================================================================================================================
 */
#ifdef ITF_DEBUG_ALLOC_

extern const char	*ITF_MEM_s_FILE;
extern u32			ITF_MEM_u_LINE;

#define ITF_FILE()					(ITF_MEM_s_FILE ? ITF_MEM_s_FILE : __FILE__)
#define ITF_LINE()					(ITF_MEM_s_FILE ? ITF_MEM_u_LINE : __LINE__)

#define ITF_MEM_NoTag()					,ITF_FILE(),ITF_LINE(), NULL
#define ITF_MEM_Tag(__I)				,ITF_FILE(),ITF_LINE(), __I
#define ITF_MEM_AllTag(__F,__L,__I)		,__F,__L,__I

#ifdef ITF_WII
// New / delete. Should be used only for debug purposes (cannot track debug data)
void *operator	new(size_t _ui_Size); 
void operator	delete(void *_p_Address); 
#endif //ITF_WII

#else // ITF_DEBUG_ALLOC_

#define ITF_MEM_NoTag()	
#define ITF_MEM_Tag(__I)
#define ITF_MEM_AllTag(__F,__L,__I)	


#endif // ITF_DEBUG_ALLOC_


/*
 =======================================================================================================================
 Alloc macros
 =======================================================================================================================
 */
 


#define ITF_MEM_Alloc(__S)								ALLOCATOR()->p_Alloc((__S) ITF_MEM_NoTag())
#define ITF_MEM_AllocTag(__S, __I)						ALLOCATOR()->p_Alloc((__S) ITF_MEM_Tag(__I))
#define ITF_MEM_AllocFullTag(__S, __F, __L, __I)		ALLOCATOR()->p_Alloc((__S) ITF_MEM_AllTag((__F), (__L), (__I)))

#define ITF_MEM_AllocExt(__S, __T)						ALLOCATOR()->p_AllocExt((__S), (__T) ITF_MEM_NoTag())
#define ITF_MEM_AllocExtTag(__S, __T, __I)				ALLOCATOR()->p_AllocExt((__S), (__T) ITF_MEM_Tag(__I))

#define ITF_MEM_AllocAlign(__S, __A)					ALLOCATOR()->p_AllocAlign((__S), (__A) ITF_MEM_NoTag())
#define ITF_MEM_AllocAlignTag(__S, __A, __I)			ALLOCATOR()->p_AllocAlign((__S), (__A) ITF_MEM_Tag(__I))
#define ITF_MEM_AllocAlignExt(__S, __A, __T)			ALLOCATOR()->p_AllocAlignExt((__S), (__A), (__T) ITF_MEM_NoTag())
#define ITF_MEM_AllocAlignExtTag(__S, __A, __T, __I)	ALLOCATOR()->p_AllocAlignExt((__S), (__A), (__T) ITF_MEM_Tag(__I))

#define ITF_MEM_AllocTmp(__S)							ALLOCATOR()->p_AllocTmp((__S) ITF_MEM_NoTag())

#define ITF_MEM_Realloc(__P, __S)						ALLOCATOR()->p_Realloc((__P), (__S) ITF_MEM_NoTag())
#define ITF_MEM_ReallocTag(__P, __S, __I)				ALLOCATOR()->p_Realloc((__P), (__S) ITF_MEM_Tag(__I))
#define ITF_MEM_ReallocAlign(__P, __S, __A)				ALLOCATOR()->p_ReallocAlign((__P),(__S), (__A) ITF_MEM_NoTag())
#define ITF_MEM_ReallocAlignTag(__P, __S, __A, __I)		ALLOCATOR()->p_ReallocAlign((__P),(__S), (__A) ITF_MEM_Tag(__I))

#define ITF_MEM_Free(__P)								ALLOCATOR()->Free(__P)
#define ITF_MEM_FreeAlign(__P)							ALLOCATOR()->Free(__P)
#define ITF_MEM_FreeTmp(__P)							ALLOCATOR()->FreeTmp(__P)

#if !defined(ITF_WII) && !defined(ITF_CTR)
#define ITF_MEM_AllocPage(__E,__S,__A,__W)				ALLOCATOR()->p_AllocPage(__E,__S,__A,__W ITF_MEM_NoTag())
#define ITF_MEM_AllocPageTag(__E,__S,__A,__W,__I)		ALLOCATOR()->p_AllocPage(__E,__S,__A,__W ITF_MEM_Tag(__I))
#define ITF_MEM_FreePage(__P)							ALLOCATOR()->FreePage(__P)
#define ITF_MEM_GetPageSize()							ALLOCATOR()->GetPageSize()
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

#ifdef ITF_PS3
#define ITF_MEM_ContainerCreate(__ID,__S)				ALLOCATOR()->ContainerCreate(__ID,__S ITF_MEM_NoTag())
#define ITF_MEM_ContainerDestroy(__ID)					ALLOCATOR()->ContainerDestroy(__ID)
#endif //ITF_PS3

#ifdef ITF_PS3
#define ITF_MEM_ContainerCreate(__ID,__S)				ALLOCATOR()->ContainerCreate(__ID,__S ITF_MEM_NoTag())
#define ITF_MEM_ContainerDestroy(__ID)					ALLOCATOR()->ContainerDestroy(__ID)
#endif //ITF_PS3

#ifdef ITF_DEBUG_ALLOC_

#define ITF_New(_Class)							new(ITF_FILE(), ITF_LINE(), "Class New", 1.f) _Class
#define ITF_NewAligned(_Class,_Align)			new(_Align, ITF_FILE(), ITF_LINE(), "Class New",1.0f) _Class
#define ITF_NewArray(_Class, _Count)			new(ITF_FILE(), ITF_LINE(), "Class New", 1.f) _Class[_Count]

#else /* !ITF_DEBUG_ALLOC_ */

#define ITF_New(_Class)							new _Class
#define ITF_NewAligned(_Class,_Align)		    new(_Align) _Class
#define ITF_NewArray(_Class, _Count)			new _Class[_Count]

#endif /* !ITF_DEBUG_ALLOC_ */


#define ITF_MEM_dbgAllocExtendedCheckType(_u32_Type) \
	ITF_ASSERT((_u32_Type & (ITF_MEM_MEM1_ALLOC | ITF_MEM_MEM2_ALLOC)) != (ITF_MEM_MEM1_ALLOC | ITF_MEM_MEM2_ALLOC))

/*
 =======================================================================================================================
 DEFAULT FLAG MANAGEMENT
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void ITF_MEM_PushExt(u32 _uType)
{
	FlagStack *pFlagStack = ALLOCATOR()->pGetFlagStack();
	
	ITF_ASSERT(pFlagStack->m_Index + 1 < ITF_MEM_DEFAULT_FLAG_STACK_SIZE); 
	pFlagStack->m_Flag = _uType;
	pFlagStack->m_Stack[++pFlagStack->m_Index] = _uType; 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void ITF_MEM_PopExt()
{
	FlagStack *pFlagStack = ALLOCATOR()->pGetFlagStack();
	
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
inline u32 ITF_MEM_GetFlag()
{
	FlagStack *pFlagStack = ALLOCATOR()->pGetFlagStack();
	ITF_ASSERT(pFlagStack);
	return pFlagStack->m_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ITF_USEFLAGSTACK
inline void ITF_MEM_RegisterThread()
{
	ALLOCATOR()->RegisterThread();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void ITF_MEM_UnregisterThread()
{
	ALLOCATOR()->UnregisterThread();
}
#endif //ITF_USEFLAGSTACK
/*
 =======================================================================================================================
 =======================================================================================================================
 */

#define ITF_MEM_StopAllocTracking() \
{ \
	ITF_MEM_PushExt(ITF_MEM_DEBUG_ALLOC); \
}

#define ITF_MEM_StartAllocTracking() \
{ \
	ITF_MEM_PopExt(); \
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef ITF_MEM_HEAP_DEBUG_
#define ITF_MEM_CHECK_BEFORE_ALLOC  0x00000001
#define ITF_MEM_CHECK_BEFORE_FREE   0x00000002
#define ITF_MEM_CHECK_AFTER_ALLOC   0x00000004
#define ITF_MEM_CHECK_AFTER_FREE    0x00000008
extern u32 MEM_uCheckState;
#define ITF_MEM_CHECK(pos) { if (pos & MEM_uCheckState) ITF_MEM_CheckAllocatedPointers();}
inline void  ITF_MEM_SetCheckStatus(u32 _Status) { MEM_uCheckState = _Status;}
#else //ITF_MEM_HEAP_DEBUG_
#define ITF_MEM_CHECK(pos) 
#define ITF_MEM_SetCheckStatus(_Status)
#endif //ITF_MEM_HEAP_DEBUG_

} //namespace ITF

#endif //ENABLE_ALLOCATOR
#endif //_ITF_ALLOCATOR_MACROS_H_