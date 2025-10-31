#include "precompiled_core.h"

#include "memory/memory_WII.h"
#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_macros_WII.h"
#include "memory/WII/MEM_manager_WII.h"
#include "memory/WII/MEM_log_WII.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#include <revolution/os.h>

namespace ITF
{
#if !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)
    CMemStats g_dbgMemStats;
#endif // !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)

u32 ComputeMemFlagStrategie(size_t _size, MemoryId::ITF_ALLOCATOR_IDS _id)
{
#if !defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)
    g_dbgMemStats.statsId[_id].totcount ++;
    g_dbgMemStats.statsId[_id].alloc = _size + memoryStatsManager::get().getCategoryCurrentAllocated(_id);
    if(g_dbgMemStats.statsId[_id].totcount == g_dbgMemStats.statsId[_id].breakAtCount)
    {
        ITF_DEBUG_BREAK();
    }

    if(_id == MemoryId::mId_Blob ) // used tu put a breakpoint
    {
        MEM_FlagStack *pFlagStack = MEMory()->pGetFlagStack();
        if(pFlagStack->m_Flag)
            return pFlagStack->m_Flag;
        return MEM_C_MEM2_ALLOC;
    }
#endif //!defined(ITF_FINAL) && defined(ITF_CATEGORY_MEMORY_ENABLE)

    MEM_FlagStack *pFlagStack = MEMory()->pGetFlagStack();
    if(pFlagStack->m_Flag)
        return pFlagStack->m_Flag;

    switch(_id)
    {
        case MemoryId::mId_Unknow:
        case MemoryId::mId_String8:
        case MemoryId::mId_String16:
        case MemoryId::mId_Blob:
        case MemoryId::mId_IDServer:
        case MemoryId::mId_StringIDDatabase:
        case MemoryId::mId_LogicDataBase:        
        case MemoryId::mId_Misc:
        case MemoryId::mId_Allocator:
        case MemoryId::mId_TinyXML:
        case MemoryId::mId_UI:
        case MemoryId::mId_Resource:
        case MemoryId::mId_Editor:
        case MemoryId::mId_SavegameManager:
        case MemoryId::mId_Plugin:
        case MemoryId::mId_Animation:
        case MemoryId::mId_Phys:        
        case MemoryId::mId_Frieze:
        case MemoryId::mId_List:        
        case MemoryId::mId_Vector:
        case MemoryId::mId_GfxAdapter:
        case MemoryId::mId_VectorAnimation:
        case MemoryId::mId_VectorAnimTrackBone:
        case MemoryId::mId_VectorAnimTrackBonePAS:
        case MemoryId::mId_VectorAnimTrackBoneZAL:
        case MemoryId::mId_AudioData:            
        case MemoryId::mId_AudioStreamBuffers:            
        case MemoryId::mId_Textures:
        case MemoryId::mId_VertexBufferStatic:
            return MEM_C_MEM2_ALLOC;
        case MemoryId::mId_Scene:
        case MemoryId::mId_Gameplay:
        case MemoryId::mId_GameplayInputCriteria:            
        case MemoryId::mId_GameplayTemplate:
        case MemoryId::mId_GameplayFactory:
        case MemoryId::mId_GameplayMisc:
        case MemoryId::mId_SafeArray:        
        case MemoryId::mId_SafeArrayCache:        
        case MemoryId::mId_VertexBuffer:
        case MemoryId::mId_Particle:
        case MemoryId::mId_Serialization:
        case MemoryId::mId_Actor:
        case MemoryId::mId_AudioEngine:
        case MemoryId::mId_Polyline:
		case MemoryId::mId_Singleton:
            return MEM_C_MEM1_ALLOC;

        case MemoryId::mId_IndexBuffer:
        case MemoryId::mId_Mesh:        
        case MemoryId::mId_Vertex:
        case MemoryId::mId_Temporary:
        case MemoryId::mId_VectorInt:
        case MemoryId::mId_Map:
        case MemoryId::mId_System:
            return MEM_C_MEM2_ALLOC;
        default:
            return MEM_C_MEM2_ALLOC;
    }
}

/*
 =======================================================================================================================
 Define the behavior of the manager here (avoid long compiles)
 =======================================================================================================================
 */
#ifdef _DEBUG
#define MEM_FILL_ALLOC
#define MEM_FILL_FREE
#define MEM_USE_GUARDS
#endif // _DEBUG 

#ifdef _DEBUG_HEAP_
    #define MEM_DEBUG_HEAP_SIZE        (30*1024*1024) // 30 MB for debug infos. This leaves 34 extra Mo in mem2. 
#else // _DEBUG_HEAP_
    #define MEM_DEBUG_HEAP_SIZE        0
#endif // _DEBUG_HEAP_

// Modify MEM_MAX_SIZE_POW2 if you change this
#define MEM_VIRTUAL_HEAP_SIZE (512*1024*1024)

#define MEM_DefaultHeapAlloc     MEM_Heap2
#define MEM_DefaultHeapAllocPool MEM_Heap1

// The memory manager
MEM_Manager MEM_Manager::ms_Instance;
MEM_Manager *MEM_Manager::ms_pInstance = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_Manager::MEM_Manager()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_Manager::~MEM_Manager()
{
}
        
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::initStatics()
{
    #ifdef _HEAP_DEBUG_
    #   ifdef MEM_FILL_ALLOC
        MEM_Heap::m_bFillAllocBlocks = btrue;
    #   else //MEM_FILL_ALLOC
        MEM_Heap::m_bFillAllocBlocks = bfalse;
    #   endif //MEM_FILL_ALLOC

    #   ifdef MEM_FILL_FREE
        MEM_Heap::m_bFillFreeBlocks = btrue;
    #   else //MEM_FILL_FREE
        MEM_Heap::m_bFillFreeBlocks = bfalse;
    #   endif //MEM_FILL_FREE

    #   ifdef MEM_USE_GUARDS
        MEM_Heap::m_bAddGuards = btrue;
    #   else //MEM_USE_GUARDS
        MEM_Heap::m_bAddGuards = bfalse;
    #   endif //MEM_USE_GUARDS
    #endif /* _HEAP_DEBUG_ */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::s_Create()
{
    ITF_ASSERT_CRASH(!ms_pInstance, "Must create manager first");
    ms_pInstance = &ms_Instance;
    initStatics();
    ms_Instance.Create();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::s_Destroy()
{
    ITF_ASSERT_CRASH(ms_pInstance, "Must create manager first");
    ms_Instance.Destroy();
    ms_pInstance = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u8 *MEM_SystemHeapCreate(u32 *_puSize, u32 _uMem)
{
    u8 *pAddress;

    if (_uMem == 1)
    {
        *_puSize = (u32) OSGetMEM1ArenaHi() - (u32) OSGetMEM1ArenaLo();
        ITF_ASSERT_CRASH(!(*_puSize & 31), "Bad alignment"); // Are we aligned on 32 bytes ?
        pAddress = (u8 *) OSAllocFromMEM1ArenaLo(*_puSize, 32);
    }
    else
    {
        *_puSize = (u32) OSGetMEM2ArenaHi() - (u32) OSGetMEM2ArenaLo();
        ITF_ASSERT_CRASH(!(*_puSize & 31), "Bad alignment"); // Are we aligned on 32 bytes ?
        pAddress = (u8 *) OSAllocFromMEM2ArenaLo(*_puSize, 32);
    }

    return pAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static bbool MEM_Manager_bInitHasBeenDone = bfalse;
void MEM_Manager::Create()
{
    if (MEM_Manager_bInitHasBeenDone)
        return; 
    MEM_Manager_bInitHasBeenDone = btrue;

    // Create heaps (platform dependant).
    u8 *pBegin;
    u32 uSize;

    pBegin = MEM_SystemHeapCreate(&uSize, 1); // MEM 1 Heap
    m_Heaps[MEM_Heap1].Create(uSize,pBegin);
    pBegin = MEM_SystemHeapCreate(&uSize, 2); // MEM 2 Heap
    m_Heaps[MEM_Heap2].Create(uSize-MEM_DEBUG_HEAP_SIZE,pBegin);
    
#ifdef _DEBUG_HEAP_
    // In debug, use another heap for debug allocs.    
    m_Heaps[MEM_HeapDebug].Create(MEM_DEBUG_HEAP_SIZE,pBegin+(uSize-MEM_DEBUG_HEAP_SIZE));
#endif // _DEBUG_HEAP_

    CreateCriticalSection();
    
    ITF_MemSet(m_aFlags, 0, sizeof(m_aFlags));

    RegisterThread();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 void MEM_Manager::Destroy()
{
    m_Heaps[MEM_Heap1].Destroy();
    m_Heaps[MEM_Heap2].Destroy();

#ifdef _DEBUG_HEAP_
    m_Heaps[MEM_HeapDebug].Destroy();
#endif //_DEBUG_HEAP_    

    DestroyCriticalSection();    
    MEM_Manager_bInitHasBeenDone = bfalse;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::RegisterThread()
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

    OSInitFastCast();
    OSSetGQR6(OS_GQR_S16, OS_GQR_SCALE_32768);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::UnregisterThread()
{
    const i32 iValue = m_LS.getValue();
    ITF_ASSERT_CRASH(iValue>0 && iValue<eThreadId_count, "Wrong local storage");
    ITF_ASSERT_CRASH(m_aFlags[iValue].m_bIsUsed, "Wrong local storage");
    m_aFlags[iValue].m_bIsUsed = bfalse;
}

#ifdef USE_TEMP_HEAP
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::CreateTempHeap(MEM_TempHeap _Heap,void *_pBeginAddress, u32 _uHeapSize)
{
    ITF_ASSERT(_Heap < MEM_TempHeapNb);
    ITF_ASSERT(!m_TempHeaps[_Heap]);
    
    // Is the buffer aligned on 32 bytes (necessary for pools) ?
    ITF_ASSERT(!(((u32)_pBeginAddress) & 31));
    ITF_ASSERT(!(_uHeapSize & 31));
    
	m_TempHeaps[_Heap] = (MEM_Heap *)MEM_M_Alloc(sizeof(MEM_Heap));
	ITF_MemSet(m_TempHeaps[_Heap],0,sizeof(MEM_Heap));
	m_TempHeaps[_Heap]->Create(_uHeapSize, (u8 *)_pBeginAddress, bfalse);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool MEM_Manager::bDestroyTempHeap(MEM_TempHeap _Heap)
{
    ITF_ASSERT(_Heap < MEM_TempHeapNb);
    ITF_ASSERT(m_TempHeaps[_Heap]);
    
    if (!m_TempHeaps[_Heap]->bIsEmpty())
        return bfalse;
    
    DCFlushRange(m_TempHeaps[_Heap]->pGetBegMemory(),(u8*)m_TempHeaps[_Heap]->pGetEndMemory()-(u8*)m_TempHeaps[_Heap]->pGetBegMemory());
        
	m_TempHeaps[_Heap]->Destroy();
	MEM_M_Free(m_TempHeaps[_Heap]);
	m_TempHeaps[_Heap] = NULL;
	return btrue;
}

u32 MEM_Manager::uGetTempHeapFromAddress(void *_p_Address) 
{
    ux i;
    for (i=0; i<MEM_TempHeapNb; i++)
    {
        if (m_TempHeaps[i] && m_TempHeaps[i]->b_IsPointerInHeap(_p_Address))
            return i;
    }
    return MEM_HeapInvalid;
}
#endif //USE_TEMP_HEAP


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef _HEAP_DEBUG_
void MEM_Manager::VerifyAllocResult(void *_pResult, u32 _uSize, bbool _bDebugAlloc)
{
    if (!_pResult)
    {
        ITF_WARNING(NULL, 0, "Not enough memory to allocate %u bytes%s. Total alloc size : %u. Total alloc Nb : %u\n",_uSize, (_bDebugAlloc? " for debug" : ""), u_GetAllocSize(),u_GetAllocNb());
        
        // Display mem usage
        MEM_LogUsageDebug();

        ITF_ASSERT_MSG(0,"Not enough memory");
        *(u32 *)NULL = 0; // Crash here !
    }
}
#endif /* _HEAP_DEBUG_ */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Manager::p_AllocAlignExt(u32 _uSize, u32 _uAlign, u32 _uType MEM_M_DebugArgsTypesDef)
{
    if(!_uSize) 
        return NULL;

    EnterCriticalSection();
    MEM_M_dbgAllocExtendedCheckType(_uType);
    void *pResult = NULL;
    
#ifdef _DEBUG_HEAP_
    if (_uType & MEM_C_DEBUG_ALLOC)
    {
        pResult = m_Heaps[MEM_HeapDebug].p_AllocAlign(_uSize, _uAlign);
    }
    else
#endif // _DEBUG_HEAP_
    {
#ifdef USE_TEMP_HEAP
		if (_uType & (MEM_C_FRAMEBUFFER_ALLOC | MEM_C_AFTERFX_ALLOC))
		{
			u32 uHeap = (_uType & MEM_C_FRAMEBUFFER_ALLOC ? MEM_TempHeapFrameBuffer : MEM_TempHeapAfterFX);
			MEM_Heap *pHeap = m_TempHeaps[uHeap];
			if (pHeap)
				pResult = pHeap->p_AllocAlign(_uSize,_uAlign);
		}
#endif //USE_TEMP_HEAP

		if (!pResult)
		{
			u32    uHeap = (_uSize <= MEM_BLOCK_SIZE_MAX ? MEM_DefaultHeapAllocPool : MEM_DefaultHeapAlloc);
			uHeap = (_uType & MEM_C_MEM1_ALLOC ? MEM_Heap1 : (_uType & MEM_C_MEM2_ALLOC ? MEM_Heap2 : uHeap));

			pResult = m_Heaps[uHeap].p_AllocAlign(_uSize,_uAlign );
	        
			if(!pResult && !(_uType & MEM_C_HARD_ALLOC))
			{
				ITF_ASSERT_CRASH(MEM_HeapNb == 2, "Wrong heap number");
				pResult = m_Heaps[1 - uHeap].p_AllocAlign(_uSize,_uAlign);
			}
		}
    }
    
#ifdef _HEAP_DEBUG_
    VerifyAllocResult(pResult, _uSize, _uType & MEM_C_DEBUG_ALLOC);
#endif /* _HEAP_DEBUG_ */

    LeaveCriticalSection();
    return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::Free(void *_p_Address MEM_M_FreeArgsTypesDef)
{
    EnterCriticalSection();
    if(_p_Address) 
    {
#ifdef _DEBUG_HEAP_
        if (m_Heaps[MEM_HeapDebug].b_IsPointerInHeap(_p_Address))
        {
            m_Heaps[MEM_HeapDebug].Free(_p_Address);
            LeaveCriticalSection();
            return;
        }
#endif // _DEBUG_HEAP_

#ifdef USE_TEMP_HEAP
		// First search in temporary heaps. 
		u32 uTempHeap = uGetTempHeapFromAddress(_p_Address);
		if (uTempHeap != MEM_HeapInvalid)
		{
			//OSReport("#####FREE 0x%08x\n",_p_Address);
			m_TempHeaps[uTempHeap]->Free(_p_Address);
		}
		else
#endif //USE_TEMP_HEAP
		{
			u32 uHeap = uSafeGetHeapFromAddress(_p_Address);
			m_Heaps[uHeap].Free(_p_Address);
		}
    }
    LeaveCriticalSection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Manager::p_ReallocAlign(void *_p_OldAddress, u32 _uSize, u32 _uAlign MEM_M_DebugArgsTypesDef)
{
    void    *pResult = NULL;

    // Realloc is an alloc
    if(!_p_OldAddress)
    {
        return p_AllocAlign(_uSize,_uAlign MEM_M_DebugArgs);
    }
    
    // Realloc is a free
    if (!_uSize)
    {
        Free(_p_OldAddress);
        return NULL;
    }
    
    EnterCriticalSection();
#ifdef _DEBUG_HEAP_
    if (m_Heaps[MEM_HeapDebug].b_IsPointerInHeap(_p_OldAddress))
    {
        pResult = m_Heaps[MEM_HeapDebug].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign);
#ifdef _HEAP_DEBUG_
        VerifyAllocResult(pResult, _uSize, bfalse);
#endif // _HEAP_DEBUG_
    }
    else
#endif // _DEBUG_HEAP_
    {
        // Try to realloc in same heap    
        u32 uHeap = uSafeGetHeapFromAddress(_p_OldAddress);
        pResult = m_Heaps[uHeap].p_ReallocAlign(_p_OldAddress, _uSize, _uAlign, pGetFlagStack()->m_Flag & MEM_C_REALLOC_IN_PLACE);

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

#ifdef _HEAP_DEBUG_
        VerifyAllocResult(pResult, _uSize, bfalse);
#endif /* _HEAP_DEBUG_ */
    }

    LeaveCriticalSection();
    return pResult;
}


#ifdef _DEBUG_HEAP_
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Manager::p_AllocForDebug(u32 _uSize)
{
    EnterCriticalSection();
    void * pResult = m_Heaps[MEM_HeapDebug].p_Alloc(_uSize);
#ifdef _HEAP_DEBUG_
    VerifyAllocResult(pResult, _uSize, btrue);
#endif // _HEAP_DEBUG_
    LeaveCriticalSection();
    return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Manager::p_ReallocForDebug(void *_p_Address, u32 _uSize)
{
    EnterCriticalSection();
    void * pResult = m_Heaps[MEM_HeapDebug].p_Realloc(_p_Address, _uSize);
#ifdef _HEAP_DEBUG_
    VerifyAllocResult(pResult, _uSize, btrue);
#endif // _HEAP_DEBUG_
    LeaveCriticalSection();
    return pResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::FreeForDebug(void *_p_Address)
{
    EnterCriticalSection();
    m_Heaps[MEM_HeapDebug].Free(_p_Address);
    LeaveCriticalSection();
}
#endif //_DEBUG_HEAP_

/*
 =======================================================================================================================
 // Copies the data for a realloc from a LyN heap address to another LyN heap address
 =======================================================================================================================
 */
void MEM_Manager::ReallocCopy(void *_pOldAddress, void *_pNewAddress, MEM_Heap *_pOldHeap, MEM_Heap *_pNewHeap)
{
    u32 uOldSize = _pOldHeap->uGetUserSizeFromUserAddress(_pOldAddress);
    u32 uNewSize = _pNewHeap->uGetUserSizeFromUserAddress(_pNewAddress);

    u32 uCopySize = Min(uOldSize,uNewSize);

    ITF_Memcpy(_pNewAddress,_pOldAddress,uCopySize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool MEM_Manager::b_IsPointerInHeap(void *_p_Address)
{
    if(!_p_Address) 
        return bfalse;

    // Is it in normal heaps ? 
    u32 i;
    for(i = 0; i < MEM_HeapNb; i++)
    {
        if(m_Heaps[i].b_IsPointerInHeap(_p_Address))
        {
            return btrue;
        }
    }
    return bfalse;
}

bbool MEM_Manager::b_IsValidPointer(void *_p_Address)
{
    if (!_p_Address) 
        return bfalse;

    if (b_IsPointerInHeap(_p_Address))
        return btrue;

    return bfalse;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32 MEM_Manager::uGetHeapFromAddress(void *_p_Address) 
{
    u32 i;
    for (i=0; i<MEM_HeapTotalNb; i++)
    {
        if (m_Heaps[i].b_IsPointerInHeap(_p_Address))
            return i;
    }
    return MEM_HeapInvalid;
}

#ifdef _HEAP_DEBUG_
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::u_GetFreeSize()
{ 
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetFreeCommittedSize()
{ 
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetTotalSize()
{ 
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetLastFreeSize()      
{
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetBiggestAllocPossible() 
{ 
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetAllocNb()
{
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
    {
        result += m_Heaps[uHeap].u_GetAllocNb();
    }
    LeaveCriticalSection();
    return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::u_GetFreeNb()
{
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
    {
        result += m_Heaps[uHeap].u_GetFreeNb();
    }
    LeaveCriticalSection();
    return result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::CheckPointer(void *_p)
{
    u32 uHeap = uGetHeapFromAddress(_p);
    ITF_ASSERT_CRASH((uHeap != MEM_HeapInvalid) && m_Heaps[uHeap].b_IsPointerValid(_p), "Invalid pointer");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Manager::CheckAllocatedPointers()
{
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
    {
        m_Heaps[uHeap].CheckAllocatedPointers();
    }
#ifdef USE_TEMP_HEAP
	for (uHeap = 0; uHeap<MEM_TempHeapNb; uHeap++)
	{
		if (m_TempHeaps[uHeap])
		{
			m_TempHeaps[uHeap]->CheckAllocatedPointers();
		}
	}
#endif //USE_TEMP_HEAP
    LeaveCriticalSection();
}
#endif //_HEAP_DEBUG_

#ifdef _HEAP_SIZE_DEBUG_
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::u_GetAllocSize() 
{ 
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
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
u32  MEM_Manager::u_GetPoolSize()
{
    EnterCriticalSection();
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
    {
        result += m_Heaps[uHeap].u_GetPoolSize();
    }
    LeaveCriticalSection();
    return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::u_GetTotalCommittedSize()
{
    u32 uHeap;
    u32 result = 0;
    for (uHeap = 0; uHeap<MEM_HeapTotalNb; uHeap++)
    {
        result += u_GetHeapCommittedSize(uHeap);
    }
    return result;
}

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::u_GetHeapCommittedSize(u32 _uHeap)
{
    EnterCriticalSection();
    u32 result = m_Heaps[_uHeap].u_GetCommittedSize();
    LeaveCriticalSection();
    return result;
}
#endif // _HEAP_SIZE_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
u32  MEM_Manager::GetSize(void *_pAddress)
{
    EnterCriticalSection();

    u32 uSize=0;
    u32 i;
    for (i=0; i<MEM_HeapTotalNb; i++)
    {
        if (m_Heaps[i].b_IsPointerInHeap(_pAddress))
        {
            uSize = m_Heaps[i].uGetSizeFromUserAddress(_pAddress);
            break;
        }
    }
    ITF_ASSERT_MSG(i<MEM_HeapTotalNb,"Unvalid address");

    LeaveCriticalSection();
    return uSize;
}

} // namespace ITF
