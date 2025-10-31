#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR

#ifndef _ITF_ALLOCATOR_H_
#include "core/memory/allocator/allocator.h"
#endif //_ITF_ALLOCATOR_H_

#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifdef ITF_PS3
#include <sys/memory.h>
#endif //ITF_PS3

#ifdef ITF_CTR
#include <nn/gx.h>
#include <nn/os.h>
#endif

//#include "ALLOCATOR/MEM_opt_debug.h"
//#include "ALLOCATOR/MEM_callstack.h"
//#include "ALLOCATOR/MEM_log.h"
//#include "ALLOCATOR/MEM_raster.h"
//#include "THReads/THR_Thread.h"

namespace ITF
{

/*
 =======================================================================================================================
 Define the behavior of the manager here (avoid long compiles)
 =======================================================================================================================
 */
#ifdef _DEBUG_ 
#define MEM_FILL_ALLOC
#define MEM_FILL_FREE 
#define MEM_USE_GUARDS
//#define MEM_NOT_ALIGNED // All memory that is not specifically asked as aligned is not aligned (at least only on 4 bytes).
#endif //_DEBUG_ 

#ifdef  ITF_DEBUG_ALLOC_
#   define     MEM_USE_DEBUG   // Undef MEM_USE_DEBUG if you don't want to track allocs
#   ifdef      MEM_USE_DEBUG
#       if defined(ITF_CTR)
#           define         MEM_DEBUG_HEAP_SIZE ((32-6)*1024*1024 ) // 32 MB is maximum allowed
#       elif defined(ITF_WII)
#           define         MEM_DEBUG_HEAP_SIZE (45*1024*1024)	// 45Mo in debug heap -> this leaves 19 extra Mo in mem2.
#		elif defined(ITF_WINDOWS)
#           define         MEM_DEBUG_HEAP_SIZE_SEVEN (250*1024*1024)  // 250Mo for Seven . Needed when doing up to 6 million allocs, during binarization.
#           define         MEM_DEBUG_HEAP_SIZE_XP (80*1024*1024)	  // 80Mo for XP (no need for so many allocs)
#		else
#           define         MEM_DEBUG_HEAP_SIZE (80*1024*1024) // 80 MB on PC/PS3/Xbox 360.
#       endif          //!ITF_WII
#   else       //MEM_USE_DEBUG
#       define     MEM_DEBUG_HEAP_SIZE 0 
#   endif      //MEM_USE_DEBUG
#else //ITF_DEBUG_ALLOC_
#   define     MEM_DEBUG_HEAP_SIZE 0 
#endif  //ITF_DEBUG_ALLOC_

// Heap size for small physical allocs (for GPU and sound on Xbox 360)
#define MEM_PHYSICAL_HEAP_WRITE_COMBINE_SIZE          5*1024*1024 
#define MEM_PHYSICAL_HEAP_NO_WRITE_COMBINE_SIZE       5*1024*1024 

#ifdef ITF_CTR
	#define	MEM_CTR_DEVICE_MEM_SIZE	( (16 + 5)*1024*1024 )							// 10MB (max is 32MB)
	#ifndef _FINAL_
		#define	MEM_CTR_HEAP_SIZE		( 96*1024*1024 - MEM_CTR_DEVICE_MEM_SIZE )	// (max is 96 in devkits including device mem)
	#else
		#define	MEM_CTR_HEAP_SIZE		( 64*1024*1024 - MEM_CTR_DEVICE_MEM_SIZE )	// (max is 64 including device mem)
	#endif
#endif

// Threshold on Xbox 360 to decide if physical alloc should go in physical heap or directly in XPhysicalAlloc (knowing that XPhysicalAlloc allocates by pages of at least 4ko).
#define MEM_PHYSICAL_SMALL_ALLOC_LIMIT  8*1024

// Modify ITF_MEM_MAX_SIZE_POW2 if you change this
#if defined(ITF_X360) 
#define MEM_VIRTUAL_HEAP_SIZE (512*1024*1024)
#elif defined(ITF_PS3)
// For PS3, size must be a multiple of 256Mo 
#define MEM_VIRTUAL_HEAP_SIZE (512*1024*1024)
#else
#define MEM_VIRTUAL_HEAP_SIZE_SEVEN	0x7FD00000 // 2Go - 3Mo
#define MEM_VIRTUAL_HEAP_SIZE_XP	0x20000000 // 512Mo
#endif

#ifdef ITF_WII
#define MEM_DefaultHeapAlloc	 ITF_MEM_HEAP2
#define MEM_DefaultHeapAllocPool ITF_MEM_HEAP1
#define MEM_DefaultHeapAllocTmp  ITF_MEM_HEAP2
#endif

#ifdef ITF_CTR
#define MEM_DefaultHeap ITF_MEM_HEAP1
#endif

// To decide if an alloc will be a page alloc, 
// we compute the ratio of the size of the alloc over the size that a page alloc would waste.
// If this ratio is higher than MEM_USE_PAGE_RATIO, do a page alloc. 


#define MEM_USE_PAGE_RATIO 100	// less than 1% of the alloc is wasted. 
//#define MEM_USE_PAGE_RATIO 2	// 2 : all allocs over 2ko go in pages. Useful to detect code that write in big freed memory areas. 
//#define MEM_USE_PAGE_RATIO 0	// 0 : all allocs go in pages. Useful to detect code that write in big freed memory areas. 

// The memory manager
Allocator Allocator::ms_Instance;
Allocator *Allocator::ms_pInstance = NULL;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
Allocator::Allocator()
{
#ifdef ITF_CTR
	CreateCriticalSection();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
Allocator::~Allocator()
{
}
		
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::initStatics()
{
    #ifdef ITF_MEM_HEAP_DEBUG_
    #   ifdef MEM_FILL_ALLOC
        Heap::m_bFillAllocBlocks = btrue;
    #   else //MEM_FILL_ALLOC
        Heap::m_bFillAllocBlocks = bfalse;
    #   endif //MEM_FILL_ALLOC

    #   ifdef MEM_FILL_FREE
        Heap::m_bFillFreeBlocks = btrue;
    #   else //MEM_FILL_FREE
        Heap::m_bFillFreeBlocks = bfalse;
    #   endif //MEM_FILL_FREE

    #   ifdef MEM_USE_GUARDS
        Heap::m_bAddGuards = btrue;
    #   else //MEM_USE_GUARDS
        Heap::m_bAddGuards = bfalse;
    #   endif //MEM_USE_GUARDS
    #endif /* ITF_MEM_HEAP_DEBUG_ */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::s_Create()
{
    ITF_ASSERT(!ms_pInstance);
    ms_pInstance = &ms_Instance;
    initStatics();
    ms_Instance.Create();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::s_Destroy()
{
    ITF_ASSERT(ms_pInstance);
    ms_Instance.Destroy();
    ms_pInstance = NULL;
}

#ifdef ITF_X360
u8 *MEM_SystemHeapCreate(u32 _uSize, bbool _bVirtual = btrue, bbool _bWriteCombine = btrue)
{
    u8 *pResult;
    if (_bVirtual)
        pResult = (u8 *)VirtualAlloc(NULL,_uSize,MEM_RESERVE|MEM_NOZERO|MEM_LARGE_PAGES,PAGE_READWRITE);
    else
    {   
        if (_bWriteCombine)
            pResult = (u8 *)XPhysicalAlloc(_uSize,MAXULONG_PTR,0,PAGE_READWRITE|PAGE_WRITECOMBINE|MEM_LARGE_PAGES);
        else
            pResult = (u8 *)XPhysicalAlloc(_uSize,MAXULONG_PTR,0,PAGE_READWRITE|MEM_LARGE_PAGES);
    }
    ITF_ASSERT(pResult);
    return pResult;
}

void MEM_SystemHeapDestroy(void *_pBuffer, bbool _bVirtual = btrue)
{
    if (_bVirtual)
        VirtualFree(_pBuffer,0,MEM_RELEASE);
    else
        XPhysicalFree(_pBuffer);
}

#elif defined ITF_PS3 
u8 *MEM_SystemHeapCreate(u32 _uSize)
{
    sys_addr_t pointer;
    int errorCode = sys_mmapper_allocate_address(
        _uSize,
        SYS_MEMORY_ACCESS_RIGHT_ANY | SYS_MEMORY_PAGE_SIZE_1M, // or SYS_MEMORY_PAGE_SIZE_64K
        0x10000000, // 256MB multiple
        &pointer
        );

    ITF_ASSERT(CELL_OK == errorCode);

    return (u8 *)pointer;
}

void MEM_SystemHeapDestroy(void *_pBuffer)
{
    int errorCode = sys_mmapper_free_address((sys_addr_t)_pBuffer);
    ITF_ASSERT(CELL_OK == errorCode);
}

#elif defined ITF_WII
u8 *MEM_SystemHeapCreate(u32 *_puSize, u32 _uMem)
{
	u8 *pAddress;

    if (_uMem == 1)
    {
        *_puSize = (u32) OSGetMEM1ArenaHi() - (u32) OSGetMEM1ArenaLo();
        ITF_ASSERT(!(*_puSize & 31)); // Are we aligned on 32 bytes ?
	    pAddress = (u8 *) OSAllocFromMEM1ArenaLo(*_puSize, 32);
    }
    else
    {
	    *_puSize = (u32) OSGetMEM2ArenaHi() - (u32) OSGetMEM2ArenaLo();
        ITF_ASSERT(!(*_puSize & 31)); // Are we aligned on 32 bytes ?
	    pAddress = (u8 *) OSAllocFromMEM2ArenaLo(*_puSize, 32);
    }

    return pAddress;
}
#elif defined ITF_CTR
u8 *MEM_SystemHeapCreate(u32 *_puSize, u32 _uMem)
{
	u8 *pAddress = NULL;

	if (_uMem == 1)
	{
		const size_t uiAssignment		= nn::os::GetAppMemorySize();
		const size_t uiCurrentlyUsing	= nn::os::GetUsingMemorySize();

		const size_t uiAvailable		= uiAssignment - uiCurrentlyUsing;
		const size_t uiHeapSize			= uiAvailable - MEM_CTR_DEVICE_MEM_SIZE;

		// First time initialization
		nn::os::SetDeviceMemorySize( MEM_CTR_DEVICE_MEM_SIZE ); 
		nn::os::SetHeapSize( uiHeapSize ); 

		// Heap
		*_puSize = nn::os::GetHeapSize();
		pAddress = ( u8* )nn::os::GetHeapAddress();
	}
	else
	{
		if (_uMem == 2)
		{
			// FCRAM
			*_puSize = nn::os::GetDeviceMemorySize();
			pAddress = ( u8* )nn::os::GetDeviceMemoryAddress();
		}
	}
    return pAddress;
}

void MEM_SystemHeapDestroy(void *_pBuffer, bbool _bVirtual = btrue)
{
}

#else // PC
u8 *MEM_SystemHeapCreate(u32 _uSize)
{
    void *result = VirtualAlloc(NULL,_uSize,MEM_RESERVE,PAGE_READWRITE);
    return (u8 *)result;
}

void MEM_SystemHeapDestroy(void *_pBuffer)
{
    VirtualFree(_pBuffer,0,MEM_RELEASE);
}
#endif 

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static bbool MEM_Manager_bInitHasBeenDone = bfalse;
void Allocator::Create()
{
	if (MEM_Manager_bInitHasBeenDone)
		return; 
	MEM_Manager_bInitHasBeenDone = btrue;

#ifdef ITF_DEBUG_ALLOC_
	m_pDebug = NULL;
#endif // ITF_DEBUG_ALLOC_

    // Create heaps (platform dependant).
    u8 *pBegin;
    u32 uSize;
	bbool bBigAllocNb = bfalse;
#ifdef ITF_WII
    pBegin = MEM_SystemHeapCreate(&uSize, 1); // MEM 1 Heap
	m_Heaps[ITF_MEM_HEAP1].Create(uSize,pBegin);
    pBegin = MEM_SystemHeapCreate(&uSize, 2); // MEM 2 Heap
	m_Heaps[ITF_MEM_HEAP2].Create(uSize-MEM_DEBUG_HEAP_SIZE,pBegin);
#else //ITF_WII

#ifdef ITF_X360
    // Create physical memory heap (used by GPU and Sound for small physical allocs, only on Xbox)
    pBegin = MEM_SystemHeapCreate(MEM_PHYSICAL_HEAP_WRITE_COMBINE_SIZE, bfalse,btrue);
	m_Heaps[ITF_MEM_HEAP_WC].Create(MEM_PHYSICAL_HEAP_WRITE_COMBINE_SIZE,pBegin,bfalse);

    pBegin = MEM_SystemHeapCreate(MEM_PHYSICAL_HEAP_NO_WRITE_COMBINE_SIZE, bfalse, bfalse);
	m_Heaps[ITF_MEM_HEAP_NWC].Create(MEM_PHYSICAL_HEAP_NO_WRITE_COMBINE_SIZE,pBegin,bfalse);
#endif //ITF_X360

#ifdef ITF_CTR
	u8 *pBeginMainHeap;
	u32 uSizeMainHeap;
	pBegin = MEM_SystemHeapCreate( &uSize, 1 ); // Normal Heap
	m_Heaps[ ITF_MEM_HEAP1 ].Create( uSize - MEM_DEBUG_HEAP_SIZE, pBegin ); // Create a smaller heap to reuse part of it for the debug heap
	pBeginMainHeap = pBegin;
	uSizeMainHeap = uSize;
	pBegin = MEM_SystemHeapCreate( &uSize, 2 ); // FCRAM Heap (Device Memory)
	m_Heaps[ ITF_MEM_HEAP2 ].Create( uSize, pBegin );
	pBegin = pBeginMainHeap;
	uSize = uSizeMainHeap;
#endif


#ifndef ITF_CTR // don't create virtual memory heap in CTR neither
    // Create virtual memory heap (used by standard allocs on PC, PS3, Xbox)
#ifdef ITF_WINDOWS
	m_bAllocInPages = bfalse;

	// On PC, the debug heap is allocated independently from the main heap.

	// We first try a size that we know to pass on Windows Seven but fail on Windows XP, and then try with a size that passes on Windows XP .
	uSize = MEM_VIRTUAL_HEAP_SIZE_SEVEN;
	pBegin = MEM_SystemHeapCreate(uSize);
	if (pBegin)
		bBigAllocNb = btrue;
	else
	{
		m_bAllocInPages = btrue;
		uSize = MEM_VIRTUAL_HEAP_SIZE_XP;
		pBegin = MEM_SystemHeapCreate(uSize);
	}

	u32 uMainHeapSize = uSize;
#else //ITF_WINDOWS
	uSize = MEM_VIRTUAL_HEAP_SIZE;
	pBegin = MEM_SystemHeapCreate(uSize);
	u32 uMainHeapSize = uSize-MEM_DEBUG_HEAP_SIZE;
#endif //ITF_WINDOWS
	m_Heaps[ITF_MEM_HEAP1].Create(uMainHeapSize,pBegin);
#endif

#endif //ITF_WII

#ifdef ITF_DEBUG_ALLOC_
	u32 uDebugHeapSize; 
#ifdef ITF_WINDOWS
	uDebugHeapSize = MEM_DEBUG_HEAP_SIZE_XP;
	if (bBigAllocNb)
		uDebugHeapSize = MEM_DEBUG_HEAP_SIZE_SEVEN;
	u8 *pDebugBegin = MEM_SystemHeapCreate(uDebugHeapSize);
#else // ITF_WINDOWS
	uDebugHeapSize = MEM_DEBUG_HEAP_SIZE;
	u8 *pDebugBegin = pBegin+(uSize-MEM_DEBUG_HEAP_SIZE);
#endif // ITF_WINDOWS

	// In debug, use another heap for debug allocs.	
	m_Heaps[ITF_MEM_HEAP_DEBUG].Create(uDebugHeapSize,pDebugBegin);
#endif // ITF_DEBUG_ALLOC_

	CreateCriticalSection();
	
	ITF_MemSet(m_aFlags, 0, sizeof(m_aFlags));

#ifdef ITF_USEFLAGSTACK
    RegisterThread();
#endif //ITF_USEFLAGSTACK
					
	ITF_MEM_PushExt(ITF_MEM_DEBUG_ALLOC);
#ifdef MEM_USE_DEBUG
	m_pDebug = new MEM_Debug;	
	m_pDebug->Create(bBigAllocNb);

	MEM_CallStackTable::CreateTable (); 
#endif // MEM_USE_DEBUG
	ITF_MEM_PopExt();
	
#if !defined(ITF_WII) && !defined(ITF_CTR)
	m_Pages.Create();
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

#if (defined ITF_PS3) && (defined ITF_MEM_HEAP_DEBUG_)
	m_uContainerSize = 0;
	m_uContainerNb = 0;
#endif

	// Add a raster for system memory
#ifdef ITF_MEM_RASTERS
    PUSH_MEM_RASTER_SCOPE("Engine/System");

	MEM_DebugData dbgData;
	MEM_raster::SetAlloc(&dbgData,ITF_MEM_GetSystemPhysicalMemory());
#endif //ITF_MEM_RASTERS

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void Allocator::Destroy()
{
#if !defined(ITF_WII) && !defined (ITF_CTR)
	m_Pages.Destroy();
#endif //!defined(ITF_WII) && !defined ITF_CTR)

	ITF_MEM_TrackLeaks();

#ifdef ITF_DEBUG_ALLOC_	
	MEM_CallStackTable::DestroyTable (); 
#endif //ITF_DEBUG_ALLOC_	

#ifdef MEM_USE_DEBUG
	m_pDebug->Destroy();
	delete m_pDebug;
#endif //MEM_USE_DEBUG


#ifdef ITF_WII
	m_Heaps[ITF_MEM_HEAP1].Destroy();
	m_Heaps[ITF_MEM_HEAP2].Destroy();
#else //ITF_WII
    m_Heaps[ITF_MEM_HEAP1].Destroy();
#ifdef ITF_X360
    m_Heaps[ITF_MEM_HEAP_WC].Destroy();
    m_Heaps[ITF_MEM_HEAP_NWC].Destroy();
#endif //ITF_X360
#endif  //ITF_WII
#ifdef ITF_CTR
	m_Heaps[ITF_MEM_HEAP2].Destroy();
#endif

#ifdef ITF_DEBUG_ALLOC_
	m_Heaps[ITF_MEM_HEAP_DEBUG].Destroy();
#endif //ITF_DEBUG_ALLOC_	


    // Destroy heaps (platform dependant).
#ifdef ITF_WII
    //MEM_SystemHeapDestroy
#else //ITF_WII
#ifndef ITF_CTR
    MEM_SystemHeapDestroy(m_Heaps[ITF_MEM_HEAP1].pGetBegMemory());
#endif
#ifdef ITF_X360
    MEM_SystemHeapDestroy(m_Heaps[ITF_MEM_HEAP_WC].pGetBegMemory(),bfalse);
    MEM_SystemHeapDestroy(m_Heaps[ITF_MEM_HEAP_NWC].pGetBegMemory(),bfalse);
#endif //ITF_X360
#if (defined ITF_WINDOWS) && (defined ITF_DEBUG_ALLOC_)
	MEM_SystemHeapDestroy(m_Heaps[ITF_MEM_HEAP_DEBUG].pGetBegMemory());
#endif
#endif //ITF_WII

	DestroyCriticalSection();	
	MEM_Manager_bInitHasBeenDone = bfalse;
}

#ifdef ITF_PS3
int Allocator::ContainerCreate(sys_memory_container_t *_pContainer, u32 _uSize ITF_ITF_MEM_ContainerCreateDef)
{
	int errorCode = sys_memory_container_create(_pContainer, _uSize);
	if ((errorCode == ENOMEM) || (errorCode == EAGAIN))
	{
#ifdef ITF_MEM_HEAP_DEBUG_
		VerifyAllocResult(NULL, _uSize,bfalse);
#endif /* ITF_MEM_HEAP_DEBUG_ */
		return errorCode;
	}
	ITF_ASSERT(CELL_OK == errorCode);

	ITF_ASSERT(ContainerGetSize(*_pContainer) == _uSize);

#ifdef ITF_MEM_HEAP_DEBUG_
	m_uContainerSize += _uSize;
	m_uContainerNb++;
#endif //ITF_MEM_HEAP_DEBUG_

#ifdef MEM_USE_DEBUG
	EnterCriticalSection();
	m_pDebug->AddID((*_pContainer),_uSize,m_LS.iGetValue() ITF_ITF_MEM_ContainerCreateArg , bfalse);
	LeaveCriticalSection();
#endif // MEM_USE_DEBUG

	return errorCode;
}

int Allocator::ContainerDestroy(sys_memory_container_t _container)
{
#ifdef MEM_USE_DEBUG
	EnterCriticalSection();
	if (m_pDebug)
		m_pDebug->RemoveID(_container,bfalse);
	LeaveCriticalSection();
#endif //MEM_USE_DEBUG

#ifdef ITF_MEM_HEAP_DEBUG_
	m_uContainerSize -= ContainerGetSize(_container);
	m_uContainerNb--;
#endif //ITF_MEM_HEAP_DEBUG_

	int errorCode = sys_memory_container_destroy(_container);
	ITF_ASSERT(CELL_OK == errorCode);

	return errorCode;
}

int Allocator::ContainerGetSize(sys_memory_container_t _container)
{
	sys_memory_info_t minfo;
	int errorCode = sys_memory_container_get_size(&minfo,_container);
	ITF_ASSERT(errorCode == CELL_OK);

	return minfo.total_user_memory;
}

#endif // ITF_PS3


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ITF_USEFLAGSTACK
void Allocator::RegisterThread()
{
	u32 i;
	// i == 0 : default context, used during interruptions.
	for(i=1; i < eThreadId_count ; i++)
	{
		if(!m_aFlags[i].m_bIsUsed)
		{			
			m_aFlags[i].m_bIsUsed = btrue;
			// Save this index (i) in local storage
			m_LS.setValue(i);
			break;
		}		
	}
	ITF_ASSERT_CRASH(i<eThreadId_count, "Wrong thread ID");

#ifdef ITF_WII
	OSInitFastCast();
	OSSetGQR6(OS_GQR_S16, OS_GQR_SCALE_32768);
#endif //ITF_WII
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::UnregisterThread()
{
	#ifdef ITF_MEM_RASTERS
		MEM_raster::ReleaseThreadStack(THR_GetThreadId());
	#endif

	i32 iValue = m_LS.getValue();
	ITF_ASSERT(iValue>0 && iValue<eThreadId_count);
	ITF_ASSERT(m_aFlags[iValue].m_bIsUsed);
	m_aFlags[iValue].m_bIsUsed = bfalse;
}

#endif //ITF_USEFLAGSTACK

#ifdef ITF_USE_TEMP_HEAP
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::CreateTempHeap(ITF_MEM_TempHeap _Heap,void *_pBeginAddress, u32 _uHeapSize)
{
    ITF_ASSERT(_Heap < ITF_MEM_TempHeapNb);
    ITF_ASSERT(!m_TempHeaps[_Heap]);
    
    // Is the buffer aligned on 32 bytes (necessary for pools) ?
    ITF_ASSERT(!(((u32)_pBeginAddress) & 31));
    ITF_ASSERT(!(_uHeapSize & 31));
    
	m_TempHeaps[_Heap] = (Heap *)ITF_MEM_Alloc(sizeof(Heap));
	ITF_MemSet(m_TempHeaps[_Heap],0,sizeof(Heap));
	m_TempHeaps[_Heap]->Create(_uHeapSize, (u8 *)_pBeginAddress, bfalse);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool Allocator::bDestroyTempHeap(ITF_MEM_TempHeap _Heap)
{
    ITF_ASSERT(_Heap < ITF_MEM_TempHeapNb);
    ITF_ASSERT(m_TempHeaps[_Heap]);
    
    if (!m_TempHeaps[_Heap]->bIsEmpty())
        return bfalse;
    
    DCFlushRange(m_TempHeaps[_Heap]->pGetBegMemory(),(u8*)m_TempHeaps[_Heap]->pGetEndMemory()-(u8*)m_TempHeaps[_Heap]->pGetBegMemory());
        
	m_TempHeaps[_Heap]->Destroy();
	ITF_MEM_Free(m_TempHeaps[_Heap]);
	m_TempHeaps[_Heap] = NULL;
	return btrue;
}

u32 Allocator::uGetTempHeapFromAddress(void *_p_Address) 
{
    ux i;
    for (i=0; i<ITF_MEM_TempHeapNb; i++)
    {
        if (m_TempHeaps[i] && m_TempHeaps[i]->b_IsPointerInHeap(_p_Address))
            return i;
    }
    return ITF_MEM_HeapInvalid;
}
#endif //ITF_USE_TEMP_HEAP

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ITF_MEM_HEAP_DEBUG_
#ifdef _WIN32
__declspec(noinline) 
#endif // _WIN32
void Allocator::VerifyAllocResult(void *_pResult, u32 _uSize, bbool _bDebugAlloc)
{
	if (!_pResult)
	{
		char msg[128];
		sprintf(msg,"Not enough memory to allocate %u bytes%s. Total alloc size : %u. Total alloc Nb : %u\n",_uSize, (_bDebugAlloc? " for debug" : ""), u_GetAllocSize(),u_GetAllocNb());
		LOG(msg);

		/*
		// Display mem usage
		MEM_LogUsageToDebugger();

#ifndef _WIN32_		
        MOD::TRCDraw_GRP drawer;
		MOD::TRCDisplay::getSingleton()->setDisplayPlatform(MOD::TRCDisplay::BasicStackIndex, &drawer,bfalse); 
		LAG::ErrorMsg& errorMsg = MOD::TRCDisplay::getSingleton()->getNoMemoryMessage();
		errorMsg.getMain().assign(msg);
		MOD::TRCDisplay::getSingleton()->displayAlone(MOD::TRCDisplay::BasicStackIndex, errorMsg);
#endif
		// Dump memory
#if !defined(ITF_CTR)
		if (!_bDebugAlloc)	
			MEM_LogDumpToDebugger(ITF_MEM_LOG_DUMP_ALLOC);
#endif*/

		ITF_ASSERT_CRASH(0,msg);
		*(u32 *)NULL = 0; // Crash here !
	}
}
#endif /* ITF_MEM_HEAP_DEBUG_ */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Allocator::p_AllocAlignExt(u32 _uSize, u32 _uAlign, u32 _uType ITF_MEM_DebugArgsTypesDef)
{
	if(!_uSize) 
		return NULL;

    EnterCriticalSection();
	ITF_MEM_dbgAllocExtendedCheckType(_uType);
	_uType = UpdateType(_uType);
	void *pResult = NULL;
	bbool bTrackAddress = btrue;
	
#ifdef ITF_DEBUG_ALLOC_
	if (_uType & ITF_MEM_DEBUG_ALLOC)
	{
		pResult = m_Heaps[ITF_MEM_HEAP_DEBUG].p_AllocAlign(_uSize, _uAlign);
		bTrackAddress = bfalse;
	}
	else
#endif // ITF_DEBUG_ALLOC_
	{
#ifdef ITF_USE_TEMP_HEAP
        if (_uType & (ITF_MEM_FRAMEBUFFER_ALLOC | ITF_MEM_AFTERFX_ALLOC))
        {
            u32 uHeap = (_uType & ITF_MEM_FRAMEBUFFER_ALLOC ? ITF_MEM_TempHeapFrameBuffer : ITF_MEM_TempHeapAfterFX);
            Heap *pHeap = m_TempHeaps[uHeap];
            if (pHeap)
            {
        		pResult = pHeap->p_AllocAlign(_uSize,_uAlign);
        		if (pResult)
        		{
        		    //OSReport("#####ALLOC 0x%08x : %s(%d), size %d\n",pResult,_sFile, _uLine,_uSize);
            		bTrackAddress = bfalse;
            	}
            }
        }
#endif //ITF_USE_TEMP_HEAP
	
	    if (!pResult)
	    {
#ifdef ITF_WII
    		u32	uHeap = (_uSize <= ITF_MEM_BLOCK_SIZE_MAX ? MEM_DefaultHeapAllocPool : MEM_DefaultHeapAlloc);
    		uHeap = (_uType & ITF_MEM_MEM1_ALLOC ? ITF_MEM_HEAP1 : (_uType & ITF_MEM_MEM2_ALLOC ? ITF_MEM_HEAP2 : uHeap));

    		pResult = m_Heaps[uHeap].p_AllocAlign(_uSize,_uAlign );
    		
    		#ifdef ITF_DEBUG_ALLOC_
    		/*if(pResult && (uHeap == ITF_MEM_HEAP1) && (_uSize > 1024))
    		{
    			char msg[128];
    			if(_uType & ITF_MEM_HARD_ALLOC)
    				STD_sprintf(msg,"Specific allocation %u bytes in MEM1 at %s(%d) - %s.\n",_uSize, _sFile, _uLine, _sTag);
    			else
    				STD_sprintf(msg,"Classic allocation %u bytes in MEM1 at %s(%d) - %s.\n",_uSize, _sFile, _uLine, _sTag);
    			BAS_TraceEngine(msg);		
    		}*/
    		#endif
    		if(!pResult && !(_uType & ITF_MEM_HARD_ALLOC))
    		{
    			ITF_ASSERT(ITF_MEM_HEAP_NB == 2);
    			pResult = m_Heaps[1 - uHeap].p_AllocAlign(_uSize,_uAlign);
    		}
#else //ITF_WII
#ifdef ITF_CTR
			// In which heap do we want to make the alloc ? 
		u32	uHeap = (_uType & ITF_MEM_CTR_FCRAM_ALLOC ? ITF_MEM_HEAP2 : MEM_DefaultHeap );
		pResult = m_Heaps[uHeap].p_AllocAlign(_uSize, _uAlign);
#else
#ifdef ITF_X360
            if (_uType & ITF_MEM_XB360_PHYSICAL_ALLOC) 
            {
                if (_uSize < MEM_PHYSICAL_SMALL_ALLOC_LIMIT)
                {
                    ux uHeap = (_uType & ITF_MEM_XB360_WRITE_COMBINE ? ITF_MEM_HEAP_WC : ITF_MEM_HEAP_NWC);
                    pResult = m_Heaps[uHeap ].p_AllocAlign(_uSize,_uAlign);
                }

                if (!pResult)
                {
    				pResult = m_Pages.pAllocPage(ITF_MEM_PageSizeBEST,_uSize,_uAlign,_uType & ITF_MEM_XB360_WRITE_COMBINE);
                }
            }
            else
#endif // ITF_X360
    		{
                // A page alloc wastes too much -> try a heap alloc
                pResult = m_Heaps[ITF_MEM_HEAP1].p_AllocAlign(_uSize,_uAlign);
                if (!pResult && !(_uType & ITF_MEM_NOPAGE_ALLOC))
                {
                    // Oups, not enough memory in heap -> try a page alloc.
                    pResult = m_Pages.pAllocPage(ITF_MEM_PageSizeBEST,_uSize,_uAlign,bfalse);
                }

    		}

#endif //ITF_CTR
#endif //ITF_WII
        }
	}
	
#ifdef ITF_MEM_HEAP_DEBUG_
	VerifyAllocResult(pResult, _uSize, _uType & ITF_MEM_DEBUG_ALLOC);
#endif /* ITF_MEM_HEAP_DEBUG_ */

#ifdef MEM_USE_DEBUG
	if (bTrackAddress)
		m_pDebug->AddAddress(pResult,m_LS.iGetValue() ITF_MEM_DebugArgs);
#endif // MEM_USE_DEBUG

    LeaveCriticalSection();
	return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef MEM_USE_DEBUG
void Allocator::vAddVRAMAddress(u32 _ID, u32 _uSize ITF_ITF_MEM_ContainerCreateDef)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->AddID(_ID,_uSize,m_LS.iGetValue() ITF_ITF_MEM_ContainerCreateArg , btrue);
		LeaveCriticalSection();
	}
}

void Allocator::vRemoveVRAMAddress(u32 _ID)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->RemoveID(_ID, btrue);
		LeaveCriticalSection();
	}
}
#endif //MEM_USE_DEBUG

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Allocator::p_AllocTmp(u32 _uSize ITF_MEM_DebugArgsTypesDef)
{
	if(!_uSize) 
		return NULL;

    EnterCriticalSection();

	void *pResult = NULL;

    // No temp alloc for weird allocs. 
    u32 uFlag = ITF_MEM_GetFlag();
    ITF_ASSERT(!(uFlag & (ITF_MEM_FRAMEBUFFER_ALLOC | ITF_MEM_AFTERFX_ALLOC | ITF_MEM_XB360_PHYSICAL_ALLOC)));
#ifdef ITF_WII
	u32	uHeap = (uFlag & ITF_MEM_MEM1_ALLOC ? ITF_MEM_HEAP1 : (uFlag & ITF_MEM_MEM2_ALLOC ? ITF_MEM_HEAP2 : MEM_DefaultHeapAllocTmp));

	pResult = m_Heaps[uHeap].p_AllocTmp(_uSize);
	if(!pResult)
	{
		// If alloc tmp failed -> use normal alloc
		pResult = m_Heaps[uHeap].p_Alloc(_uSize);
		if(!pResult)
		{
			pResult = m_Heaps[1 - uHeap].p_Alloc(_uSize);
		}
	}
#else //ITF_WII
#ifdef ITF_CTR
	// In which heap do we want to make the alloc ? 
	u32	uHeap = (uFlag & ITF_MEM_CTR_FCRAM_ALLOC ? ITF_MEM_HEAP2 : MEM_DefaultHeap );
	pResult = m_Heaps[uHeap].p_AllocTmp(_uSize);

	 // If alloc tmp failed -> use normal alloc
	if (!pResult)
		pResult = m_Heaps[uHeap].p_Alloc(_uSize);
#else
	// Alloc Tmp -> try a heap alloc, to avoid blocking pool allocs.
	pResult = m_Heaps[ITF_MEM_HEAP1].p_Alloc(_uSize);

	if (!pResult)
	{
		// Not enough memory in heap -> try a page alloc (much slower).
		pResult = m_Pages.pAllocPage(ITF_MEM_PageSizeBEST,_uSize,0, bfalse);
	}
#endif ITF_CTR
#endif //ITF_WII
			
#ifdef ITF_MEM_HEAP_DEBUG_
	VerifyAllocResult(pResult, _uSize, bfalse);
#endif /* ITF_MEM_HEAP_DEBUG_ */

#ifdef MEM_USE_DEBUG
	m_pDebug->AddAddress(pResult,m_LS.iGetValue() ITF_MEM_DebugArgs);
#endif // MEM_USE_DEBUG

    LeaveCriticalSection();
	return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::Free(void *_p_Address ITF_MEM_FreeArgsTypesDef)
{
    EnterCriticalSection();
	if(_p_Address) 
	{
#ifdef ITF_DEBUG_ALLOC_
		if (m_Heaps[ITF_MEM_HEAP_DEBUG].b_IsPointerInHeap(_p_Address))
		{
			m_Heaps[ITF_MEM_HEAP_DEBUG].Free(_p_Address);
		}
		else
#endif // ITF_DEBUG_ALLOC_
        {
#ifdef ITF_USE_TEMP_HEAP
            // First search in temporary heaps. 
    		u32 uTempHeap = uGetTempHeapFromAddress(_p_Address);
    		if (uTempHeap != ITF_MEM_HeapInvalid)
    		{
        		//OSReport("#####FREE 0x%08x\n",_p_Address);
        		m_TempHeaps[uTempHeap]->Free(_p_Address);
            }
            else
#endif //ITF_USE_TEMP_HEAP
            {
#ifdef MEM_USE_DEBUG
                if (m_pDebug)
            		m_pDebug->RemoveAddress(_p_Address,_bIsNew);
#endif //MEM_USE_DEBUG

#if !defined(ITF_WII ) && !defined(ITF_CTR)
        		// If the address is not in heaps, it must has been allocated by m_Page.pAllocPage
        		u32 uHeap = uGetHeapFromAddress(_p_Address);
        		if (uHeap == ITF_MEM_HeapInvalid)
        		{
                    if (m_Pages.bIsValidPointer(_p_Address))
            			m_Pages.FreePage(_p_Address);
                    else 
                        free(_p_Address);
        		}
        		else
#else 
        		u32 uHeap = uSafeGetHeapFromAddress(_p_Address);
#endif 
                {
            		m_Heaps[uHeap].Free(_p_Address);
                }
            }
        }
	}
    LeaveCriticalSection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Allocator::p_ReallocAlign(void *_p_OldAddress, u32 _uSize, u32 _uAlign ITF_MEM_DebugArgsTypesDef)
{
	void	*pResult = NULL;

	// Realloc is an alloc
	if(!_p_OldAddress)
	{
		return p_AllocAlign(_uSize,_uAlign ITF_MEM_DebugArgs);
	}
	
	// Realloc is a free
	if (!_uSize)
	{
		Free(_p_OldAddress);
		return NULL;
	}
	
    EnterCriticalSection();
#ifdef ITF_DEBUG_ALLOC_
	if (m_Heaps[ITF_MEM_HEAP_DEBUG].b_IsPointerInHeap(_p_OldAddress))
	{
		pResult = m_Heaps[ITF_MEM_HEAP_DEBUG].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign);
		VerifyAllocResult(pResult, _uSize, bfalse);
	}
	else
#endif // ITF_DEBUG_ALLOC_
	{
	
#ifdef MEM_USE_DEBUG
		if (m_pDebug)
		{
    		m_pDebug->RemoveAddress(_p_OldAddress,bfalse);
		}
#endif // MEM_USE_DEBUG

#ifdef ITF_WII
		// Try to realloc in same heap	
		u32 uHeap = uSafeGetHeapFromAddress(_p_OldAddress);
		pResult = m_Heaps[uHeap].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign, pGetFlagStack()->m_Flag & ITF_MEM_REALLOC_IN_PLACE);

		if(!pResult)
		{
			// Try to realloc in other heap.
			pResult = m_Heaps[1-uHeap].p_AllocAlign(_uSize, _uAlign);
		
			if(pResult)
			{
				ReallocCopy(_p_OldAddress,pResult,&m_Heaps[uHeap],&m_Heaps[1-uHeap]);
			}
		    m_Heaps[uHeap].Free(_p_OldAddress);
		}
#elif !defined(ITF_CTR) //ITF_WII
        u32 uHeap = uGetHeapFromAddress(_p_OldAddress);
		if (ITF_MEM_HEAP1 == uHeap)
		{
			u32 uType = pGetFlagStack()->m_Flag;
#ifdef ITF_WINDOWS
			if (m_bAllocInPages)
#endif
			{
				if (!(uType & ITF_MEM_NOPAGE_ALLOC))
				{
					u32 uWastedByPageAlloc = m_Pages.SizeWasted(_uSize);
					if (uWastedByPageAlloc * MEM_USE_PAGE_RATIO < _uSize)
					{
						// We don't waste much with a page alloc -> try a page alloc
						pResult = m_Pages.pAllocPage(ITF_MEM_PageSizeBEST,_uSize,_uAlign,bfalse);
						if (pResult)
							ReallocCopyHeapToPage(_p_OldAddress,uHeap,pResult);
					}
				}
			}
			if (!pResult)
			{
				// Try to realloc in heap.
				pResult = m_Heaps[uHeap].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign, pGetFlagStack()->m_Flag & ITF_MEM_REALLOC_IN_PLACE);
			}
		}
		if (ITF_MEM_HeapInvalid == uHeap)
		{
			// First try to realloc with system alloc
			pResult = m_Pages.pReallocPage(_p_OldAddress,ITF_MEM_PageSizeBEST, _uSize, _uAlign,bfalse);
			if (!pResult)
			{
				// No more system memory -> try to use heap alloc
				pResult = m_Heaps[ITF_MEM_HEAP1].p_AllocAlign(_uSize,_uAlign);
				if (pResult)
					ReallocCopyPageToHeap(_p_OldAddress,pResult,_uSize);
			}
		}
#else	// ITF_CTR
	u32 uHeap = uSafeGetHeapFromAddress(_p_OldAddress);
	pResult = m_Heaps[uHeap].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign, pGetFlagStack()->m_Flag & ITF_MEM_REALLOC_IN_PLACE);
#endif //ITF_WII

#ifdef ITF_MEM_HEAP_DEBUG_
		VerifyAllocResult(pResult, _uSize, bfalse);
#endif /* ITF_MEM_HEAP_DEBUG_ */

#ifdef MEM_USE_DEBUG
		if (m_pDebug)
		{
			m_pDebug->AddAddress(pResult,m_LS.iGetValue() ITF_MEM_DebugArgs);
		}
#endif // MEM_USE_DEBUG
	}

    LeaveCriticalSection();
	return pResult;
}


#ifdef ITF_DEBUG_ALLOC_
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Allocator::p_AllocForDebug(u32 _uSize)
{
    EnterCriticalSection();
	void * pResult = m_Heaps[ITF_MEM_HEAP_DEBUG].p_Alloc(_uSize);
	VerifyAllocResult(pResult, _uSize, btrue);
    LeaveCriticalSection();
    return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Allocator::p_ReallocForDebug(void *_p_Address, u32 _uSize)
{
    EnterCriticalSection();
	void * pResult = m_Heaps[ITF_MEM_HEAP_DEBUG].p_Realloc(_p_Address, _uSize);
	VerifyAllocResult(pResult, _uSize, btrue);
    LeaveCriticalSection();
    return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::FreeForDebug(void *_p_Address)
{
    EnterCriticalSection();
	m_Heaps[ITF_MEM_HEAP_DEBUG].Free(_p_Address);
    LeaveCriticalSection();
}

#endif //ITF_DEBUG_ALLOC_


/*
 =======================================================================================================================
 // Copies the data for a realloc from a LyN heap address to another LyN heap address
 =======================================================================================================================
 */
void Allocator::ReallocCopy(void *_pOldAddress, void *_pNewAddress, Heap *_pOldHeap, Heap *_pNewHeap)
{
    u32 uOldSize = _pOldHeap->uGetUserSizeFromUserAddress(_pOldAddress);
    u32 uNewSize = _pNewHeap->uGetUserSizeFromUserAddress(_pNewAddress);

    u32 uCopySize = ITF::Min<u32>(uOldSize,uNewSize);

    ITF_Memcpy(_pNewAddress,_pOldAddress,uCopySize);
}

#if !defined(ITF_WII) && !defined(ITF_CTR)
/*
 =======================================================================================================================
 // Copies the data for a realloc from a system address to a heap address, and frees the system address
 =======================================================================================================================
 */
void Allocator::ReallocCopyPageToHeap(void *_p_OldAddress, void * _pNewAddress, u32 _uNewSize)
{
	u32 oldSize = m_Pages.uGetPageSize(_p_OldAddress);

	// Compute the size that should be copied, and copy
	u32 uCopySize = ITF::Min<u32>((u32)oldSize,_uNewSize);
	ITF_Memcpy(_pNewAddress,_p_OldAddress,uCopySize);

	m_Pages.FreePage(_p_OldAddress);
}

/*
 =======================================================================================================================
 // Copies the data for a realloc from a heap address to a system address, and frees the heap address
 =======================================================================================================================
 */
void Allocator::ReallocCopyHeapToPage(void *_p_OldAddress, u32 _uOldHeap, void *_pNewAddress)
{
	// Get the user size from the heap address
	u32 uOldSize = m_Heaps[_uOldHeap].uGetUserSizeFromUserAddress(_p_OldAddress);

	// Compute the size that should be copied, and copy
	u32 uCopySize = ITF::Min<u32>(uOldSize,m_Pages.uGetPageSize(_pNewAddress));
	ITF_Memcpy(_pNewAddress,_p_OldAddress,uCopySize);

	// Free the heap address
	m_Heaps[_uOldHeap].Free(_p_OldAddress);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void *Allocator::p_AllocPage(ITF_MEM_PageSize _ePageSize, u32 _uSize, u32 _uAlign, bbool _bWriteCombine ITF_MEM_DebugArgsTypesDef)
{
	EnterCriticalSection();
	void * pResult = m_Pages.pAllocPage(_ePageSize,_uSize,_uAlign, _bWriteCombine);
#ifdef ITF_MEM_HEAP_DEBUG_
	VerifyAllocResult(pResult, _uSize, bfalse);
#endif //ITF_MEM_HEAP_DEBUG_

#ifdef MEM_USE_DEBUG
	if (m_pDebug)
		m_pDebug->AddAddress(pResult,m_LS.iGetValue() ITF_MEM_DebugArgs);
#endif // MEM_USE_DEBUG

	LeaveCriticalSection();
	return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::FreePage(void *_pPage) 
{
	EnterCriticalSection();

#ifdef MEM_USE_DEBUG
	if (m_pDebug)
		m_pDebug->RemoveAddress(_pPage,bfalse);
#endif //MEM_USE_DEBUG

	m_Pages.FreePage(_pPage);
	LeaveCriticalSection();
}
#endif //!defined(ITF_WII) && !defined(ITF_CTR)


/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool Allocator::b_IsPointerInHeap(void *_p_Address)
{
	if(!_p_Address) 
		return bfalse;

	// Is it in normal heaps ? 
	ux i;
	for(i = 0; i < ITF_MEM_HEAP_NB; i++)
	{
		if(m_Heaps[i].b_IsPointerInHeap(_p_Address))
		{
			return btrue;
		}
	}
	return bfalse;
}

bbool Allocator::b_IsValidPointer(void *_p_Address)
{
	if (!_p_Address) 
		return bfalse;

	if (b_IsPointerInHeap(_p_Address))
		return btrue;

#if !defined(ITF_WII) && !defined(ITF_CTR)
	if (m_Pages.bIsValidPointer(_p_Address))
		return btrue;
#endif // !defined(ITF_WII) && !defined(ITF_CTR)

	return bfalse;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32 Allocator::uGetHeapFromAddress(void *_p_Address) 
{
    ux i;
    for (i=0; i<ITF_MEM_HEAP_TOTAL_NB; i++)
    {
        if (m_Heaps[i].b_IsPointerInHeap(_p_Address))
            return i;
    }
    return ITF_MEM_HeapInvalid;
}

#ifdef ITF_MEM_HEAP_DEBUG_


/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetAllocSize() 
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
        result += m_Heaps[uHeap].u_GetAllocSize();
    }
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetFreeSize()
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	result += m_Heaps[uHeap].u_GetFreeSize();
    }
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetFreeCommittedSize()
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
		result += m_Heaps[uHeap].u_GetFreeCommittedSize();
    }
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetTotalSize()
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
	    result += m_Heaps[uHeap].u_GetTotalSize();
    }
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetTotalCommittedSize()
{
	ux uHeap;
	u32 result = 0;
	for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
	{
		result += u_GetHeapCommittedSize(uHeap);
	}
#if !defined(ITF_WII) && !defined(ITF_CTR)
	result += u_GetPageSize();
#endif // !defined(ITF_WII) && !defined(ITF_CTR)
#ifdef ITF_PS3
	result += u_GetContainerSize();
#endif //ITF_PS3
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetHeapCommittedSize(u32 _uHeap)
{
	EnterCriticalSection();
	u32 result = m_Heaps[_uHeap].u_GetCommittedSize();
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetLastFreeSize()  	
{
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	result += m_Heaps[uHeap].u_GetLastFreeSize();
    }
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetTmpSize()
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
	    result += m_Heaps[uHeap].u_GetTmpSize();
    }
	LeaveCriticalSection();
	return result;
}
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetBiggestAllocPossible() 
{ 
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
        u32 uBiggestAllocPossible = m_Heaps[uHeap].u_GetBiggestAllocPossible();
        if (result < uBiggestAllocPossible)
            result = uBiggestAllocPossible;
    }
    LeaveCriticalSection();
	return result;
}
	
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetAllocNb()
{
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	result += m_Heaps[uHeap].u_GetAllocNb();
    }

#if !defined(ITF_WII) && !defined(ITF_CTR)
	result += u_GetPageNb();
#endif //ITF_WII
#ifdef ITF_PS3
	result += u_GetContainerNb();
#endif //ITF_PS3

	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetFreeNb()
{
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	result += m_Heaps[uHeap].u_GetFreeNb();
    }
	LeaveCriticalSection();
	return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::u_GetPoolSize()
{
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	result += m_Heaps[uHeap].u_GetPoolSize();
    }
	LeaveCriticalSection();
	return result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::CheckPointer(void *_p)
{
	u32 uHeap = uGetHeapFromAddress(_p);
	ITF_ASSERT((uHeap != ITF_MEM_HeapInvalid) && m_Heaps[uHeap].b_IsPointerValid(_p));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::CheckAllocatedPointers()
{
	EnterCriticalSection();
    ux uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<ITF_MEM_HEAP_TOTAL_NB; uHeap++)
    {
    	m_Heaps[uHeap].CheckAllocatedPointers();
    }
#ifdef ITF_USE_TEMP_HEAP
	for (uHeap = 0; uHeap<ITF_MEM_TempHeapNb; uHeap++)
	{
		if (m_TempHeaps[uHeap])
		{
		  	m_TempHeaps[uHeap]->CheckAllocatedPointers();
		}
	}
        
#endif //ITF_USE_TEMP_HEAP
    
	LeaveCriticalSection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::AssertMemPointer(bbool _bCondition, void *_pAddress, const char *_sMsg)
{
#ifdef ITF_DEBUG_ALLOC_	
	if (!_bCondition)
	{
		MEM_DebugData *pDebugData = m_pDebug->pGetDebugData(_pAddress);
		if (pDebugData)
			BAS_TraceEngine("%s(%d): %s\n", pDebugData->m_pContext->m_sFile, pDebugData->m_pContext->m_uLine, _sMsg);
		else 
			BAS_TraceEngine("UNKNOWN FILE: %s\n", pDebugData->m_pContext->m_sFile, pDebugData->m_pContext->m_uLine, _sMsg);
	}
#endif // ITF_DEBUG_ALLOC_
	ITF_ASSERT_CRASH(_bCondition,_sMsg);
}
#endif //ITF_MEM_HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  Allocator::GetSize(void *_pAddress)
{
	EnterCriticalSection();

	u32 uSize=0;
	ux i;
	for (i=0; i<ITF_MEM_HEAP_TOTAL_NB; i++)
	{
		if (m_Heaps[i].b_IsPointerInHeap(_pAddress))
		{
			uSize = m_Heaps[i].uGetSizeFromUserAddress(_pAddress);
			break;
		}
	}
#if defined(ITF_WII) || defined(ITF_CTR)
	ITF_ASSERT_CRASH(i<ITF_MEM_HEAP_TOTAL_NB,"Unvalid address");
#else // defined(ITF_WII) || defined(ITF_CTR)
	if (i == ITF_MEM_HEAP_TOTAL_NB)
	{
		uSize = m_Pages.uGetPageSize(_pAddress);
	}
#endif // defined(ITF_WII) || defined(ITF_CTR)

	LeaveCriticalSection();
	return uSize;
}

#ifdef ITF_DEBUG_ALLOC_


#ifdef MEM_USE_DEBUG



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::MemTrace(std::multimap<u32,MEM_tt_Trace>& _map, u32 _uFlags)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->MemTrace(_map, _uFlags);
		LeaveCriticalSection();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 Allocator::BeginMemDump(MEM_tt_Trace **_paTraceArray, u32 *_puArraySize, u32 _uFlags)
{
	u32 result = 0;
	if (m_pDebug)
	{
		EnterCriticalSection();
		result = m_pDebug->BeginMemDump(_paTraceArray, _puArraySize,_uFlags);
		LeaveCriticalSection();
	}
	return result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::EndMemDump(MEM_tt_Trace *_aTraceArray)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->EndMemDump(_aTraceArray);
		LeaveCriticalSection();
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::SetStep(ITF_MEM_Step _Step)
{
	if (m_pDebug)
	{
	    EnterCriticalSection();
		m_pDebug->SetStep(m_LS.iGetValue(),_Step);
 	    LeaveCriticalSection();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Allocator::SetFileKey(u32 _FileKey)
{
	if (m_pDebug)
	{
	    EnterCriticalSection();
		m_pDebug->SetFileKey(m_LS.iGetValue(),_FileKey);
 	    LeaveCriticalSection();
	}
}

void Allocator::StartFreedBlocksTracking()
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->StartFreedBlocksTracking();
		LeaveCriticalSection();
	}
}

void Allocator::StopFreedBlocksTracking()
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->StopFreedBlocksTracking();
		LeaveCriticalSection();
	}
}

void Allocator::ComputeFreedMem(MEM_Snapshot *_pFreedMem)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->ComputeFreedMem(_pFreedMem);
		LeaveCriticalSection();
	}
}


void Allocator::ComputeMapLeaks(MEM_Snapshot *_pLeaks)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->ComputeMapLeaks(_pLeaks);
		LeaveCriticalSection();
	}
}

void Allocator::ComputeMapDynamic(MEM_Snapshot *_pLeaks)
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->ComputeMapDynamic(_pLeaks);
		LeaveCriticalSection();
	}
}

void Allocator::StoreAfterLoading()
{
	if (m_pDebug)
	{
		EnterCriticalSection();
		m_pDebug->StoreAfterLoading();
		LeaveCriticalSection();
	}
}

#endif //MEM_USE_DEBUG
	
/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 Allocator::GetFirst() const 
{
#ifdef MEM_USE_DEBUG
	return m_pDebug->GetFirst();
#else //MEM_USE_DEBUG
    return 0;
#endif //MEM_USE_DEBUG
}

u32 Allocator::GetNext(u32 _it) const 
{
#ifdef MEM_USE_DEBUG
	return m_pDebug->GetNext(_it);
#else //MEM_USE_DEBUG
    return 0;
#endif //MEM_USE_DEBUG
}

bbool Allocator::bIsLast(u32 _it) const 
{
#ifdef MEM_USE_DEBUG
	return m_pDebug->bIsLast(_it);
#else //MEM_USE_DEBUG
    return btrue;
#endif //MEM_USE_DEBUG
}

void *Allocator::pGetCallStack(u32 _it) const 
{
#ifdef MEM_USE_DEBUG
	return m_pDebug->pGetCallStack(_it);
#else //MEM_USE_DEBUG
    return NULL;
#endif //MEM_USE_DEBUG
}

void *Allocator::pGetAddress(u32 _it) const 
{ 
#ifdef MEM_USE_DEBUG
	return m_pDebug->pGetAddress(_it);
#else //MEM_USE_DEBUG
    return NULL;
#endif //MEM_USE_DEBUG
}

#endif // ITF_DEBUG_ALLOC_


} //namespace ITF
#endif //ENABLE_ALLOCATOR
