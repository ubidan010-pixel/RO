#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR
#ifndef _ITF_HEAP_H_
#include "core/memory/allocator/heap.h"
#endif //_ITF_HEAP_H_


#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_

#ifndef ITF_CORE_BITTWEAK_H_
#include "core/math/BitTweak.h"
#endif //ITF_CORE_BITTWEAK_H_

//#include "STanDard/STD_mem.h"

#ifdef ITF_PS3
#include <sys/memory.h>
#endif //ITF_PS3

namespace ITF
{

#ifdef ITF_MEM_HEAP_DEBUG_
bbool Heap::m_bFillFreeBlocks = bfalse;
bbool Heap::m_bFillAllocBlocks = bfalse;
bbool Heap::m_bAddGuards = bfalse;

bbool Heap::m_bCheckFreeBlocks = btrue;
bbool Heap::m_bCheckPools = btrue;
bbool Heap::m_bCheckAllBlocks = btrue;
bbool Heap::m_bCheckTmp = btrue;
#endif /* ITF_MEM_HEAP_DEBUG_ */

#define ITF_MEM_UNUSED_ID ((sys_memory_t)-1)

#ifdef ITF_PS3
#define ITF_MEM_PAGE_SIZE   (1024*1024)
#elif ITF_X360
#define ITF_MEM_PAGE_SIZE   (64*1024)
#endif //ITF_PS3

#ifdef ITF_WII
// If defined, all memory pages are committed to memory
// Else, only used memory pages are committed to memory.
#define ITF_MEM_COMMIT_ALL // MUST BE DEFINED FOR WII
#endif //ITF_WII

#ifdef ITF_CTR
// If defined, all memory pages are committed to memory
// Else, only used memory pages are committed to memory.
#define ITF_MEM_COMMIT_ALL // MUST BE DEFINED FOR CTR
#endif //ITF_CTR


/*
 =======================================================================================================================
 =======================================================================================================================
 */
Heap::Heap()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
Heap::~Heap()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::Create(u32 _uHeapSize, u8 *_pBeginAddress, bbool _bVirtual)
{
    u32 i;

    // Heap pointers
    mpu8_BegMemory = _pBeginAddress;
    mpu8_TmpMemory = _pBeginAddress + _uHeapSize;
    mpu8_PoolMemory = mpu8_TmpMemory;
    mpu8_EndMemory = mpu8_TmpMemory;

#ifdef ITF_X360
    m_bVirtual = _bVirtual;
    m_PageSize = ITF_MEM_PAGE_SIZE;
#elif defined ITF_WINDOWS
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    m_PageSize = systemInfo.dwPageSize;
#elif defined ITF_PS3
    m_PageSize = ITF_MEM_PAGE_SIZE;
    for (i=0; i<1024; i++)
        m_aMemoryID[i] = ITF_MEM_UNUSED_ID; 
#endif //ITF_PS3

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : measures
    m_uTotalSize = _uHeapSize - sizeof(HeaderAlloc); // header used by pseudo-alloc.
    m_uFreeSize = 0;
    m_uFreeInHolesSize = 0;
    m_uFreePoolSize = 0;
    m_uFreeNb = 0;
    m_uFreeableTmpNb = 0; 
    m_uAllocSize = 0; 
    m_uAllocPoolSize = 0; 
    m_uAllocTmpSize = 0;
    m_uAllocNb = 0;
    m_uAllocTmpNb = 0;

    for (i=0; i<ITF_MEM_BLOCK_SIZE_NB; i++)
    {
        m_uFreeSizePerSize[i] = 0;
        m_uAllocSizePerSize[i] = 0;
        m_uTotalSizePerSize[i] = 0;
    }
#if !defined(ITF_WII) && !defined (ITF_CTR)
	ITF_MemSet(m_PageCommitted,0,sizeof(m_PageCommitted));
	m_uCommittedSize = 0;
#else //_WII
	m_uCommittedSize = m_uTotalSize;
#endif //_WII

#endif //ITF_MEM_HEAP_DEBUG_

#ifdef ITF_MEM_COMMIT_ALL
    bCommit(_pBeginAddress,_uHeapSize);
#endif //ITF_MEM_COMMIT_ALL


	// Clear free block tables.
	ITF_MemSet(m_apFreeBlocks,0,sizeof(m_apFreeBlocks));
	ITF_MemSet(m_auFreeFlags,0,sizeof(m_auFreeFlags));
	
    // Init pool buffers : empty for now
    m_EmptyPools = ITF_MEM_POOL_INVALID;
    m_FullPools = ITF_MEM_POOL_INVALID;
    for (i=0; i<ITF_MEM_BLOCK_SIZE_NB; i++)
        m_AvailablePools[i] = ITF_MEM_POOL_INVALID;

    // Pseudo-alloc block is a header of an alloc block at end of normal stack, with size 0.
    CreatePseudoAllocBlock();

    // Commit start of free block.
    bCommit(_pBeginAddress,sizeof(FreeBlock));

    // Add free block
    AddFreeBlock(mpu8_BegMemory,_uHeapSize-sizeof(HeaderAlloc),NULL,0);

#ifdef ITF_MEM_HEAP_DEBUG_
    if (m_bFillFreeBlocks)
    {
        FillFreeMemory(((u8 *)mpu8_BegMemory)+sizeof(FreeBlock),_uHeapSize-(sizeof(FreeBlock)+sizeof(FreeBlock * ) + sizeof(HeaderAlloc)));
    }
#endif //ITF_MEM_HEAP_DEBUG_
}

// Transforms an address range (address + size) to the biggest page address range that is totally included in the address range.
inline void ITF_MEM_ComputeIncludedPages(void *_pBegin, u32 _uSize, u32 _uPageSize, void **_ppPageIncludedBegin, i32 *_piPageIncludedSize)
{
    u32 uPageSize_1 = _uPageSize-1;
    u32 uMask = ~uPageSize_1;
    u32 uPageBegin = (((u32)_pBegin) + uPageSize_1) & uMask;
    *_ppPageIncludedBegin = (void *)uPageBegin;

    u32 uEndAddress = ((u32)_pBegin) + _uSize;
    u32 uPageEnd = uEndAddress & uMask;
    *_piPageIncludedSize = uPageEnd-uPageBegin;
}

#if (defined ITF_MEM_HEAP_DEBUG_) && !(defined ITF_WII) && !(defined ITF_CTR)

void Heap::AfterCommit(void *_pBegin, ux _uSize)
{
    // Fill blocks that are committed with magic number, and remember which blocks are committed
    ux i;
    ux begin = (ux)(((u8 *)_pBegin) - mpu8_BegMemory);
    ux firstPage = (begin & ~(m_PageSize-1))/m_PageSize;
    ux end = begin + _uSize;
    ux lastPage = ((end-1) & ~(m_PageSize-1))/m_PageSize;
    for(i = firstPage; i <= lastPage; i++)
    {
        if (!bIsPageCommitted(i))
        {
            SetPageCommitted(i);
			if(m_bFillFreeBlocks)
				ITF_MemSet(mpu8_BegMemory + i * m_PageSize,ITF_MEM_FILL_FREE,m_PageSize);
			m_uCommittedSize += m_PageSize;
        }
    }
}

void Heap::AfterDecommit(void *_pPageAddress, ux _uSize)
{
    // Remember which blocks are decommitted
    ux i;
    ux firstPage = (ux)((((u8 *)_pPageAddress) - mpu8_BegMemory)/m_PageSize);
    ux lastPage = firstPage + (_uSize/m_PageSize);
    for(i = firstPage; i < lastPage; i++)
    {
        if (bIsPageCommitted(i))
        {
            SetPageDecommitted(i);
			m_uCommittedSize -= m_PageSize;
        }
    }
}

#endif //#if (defined ITF_MEM_HEAP_DEBUG_) && !(defined ITF_WII) && !(defined ITF_CTR)

#ifdef ITF_X360
bbool Heap::bCommit(void *_pBegin, ux _uSize)
{
    if (!m_bVirtual)
        return btrue;
    else
    {
        void *pFirstPage = (void *)(((u32)_pBegin) & ~(ITF_MEM_PAGE_SIZE-1));
        ux uSize = _uSize + (((u8 *)_pBegin) - ((u8 *)pFirstPage));
        void *pBaseAddress = VirtualAlloc(pFirstPage,uSize,MEM_COMMIT | MEM_NOZERO | MEM_LARGE_PAGES,PAGE_READWRITE);
		if (!pBaseAddress)
			return bfalse;
        AfterCommit(_pBegin,_uSize);
        return btrue;
    }
}

void Heap::Decommit(void *_pBegin, ux _uSize)
{
    if (m_bVirtual)
    {
        void *pPageAddress;
        i32 iPageSize;
        ITF_MEM_ComputeIncludedPages(_pBegin,_uSize,ITF_MEM_PAGE_SIZE,&pPageAddress,&iPageSize);
        if (iPageSize > 0)
        {
            BOOL bSuccess = VirtualFree(pPageAddress,iPageSize,MEM_DECOMMIT);
            ITF_ASSERT(bSuccess);
            AfterDecommit(pPageAddress,iPageSize);
        }
    }
}
#elif (defined ITF_WINDOWS)
bbool Heap::bCommit(void *_pBegin, ux _uSize)
{
	ux i;
	ux begin = (ux)(((u8 *)_pBegin) - mpu8_BegMemory);
	ux firstPage = (begin & ~(m_PageSize-1))/m_PageSize;
	ux end = begin + _uSize;
	ux lastPage = ((end-1) & ~(m_PageSize-1))/m_PageSize;
	for(i = firstPage; i <= lastPage; i++)
	{
		void *pBaseAddress = VirtualAlloc(_pBegin,_uSize,MEM_COMMIT, PAGE_READWRITE);
		if (pBaseAddress == NULL)
			return bfalse;
		AfterCommit(_pBegin,_uSize);
		break;
	}
    
    return btrue;
}

void Heap::Decommit(void *_pBegin, ux _uSize)
{
    void *pPageAddress;
    i32 pageSize;
    ITF_MEM_ComputeIncludedPages(_pBegin,_uSize,m_PageSize,&pPageAddress,&pageSize);
    if (pageSize > 0)
    {
        BOOL bSuccess = VirtualFree(pPageAddress,pageSize,MEM_DECOMMIT);
        ITF_ASSERT(bSuccess);
        AfterDecommit(pPageAddress,pageSize);
    }
}
#elif (defined ITF_PS3)

bbool Heap::bCommit(void *_pBegin, ux _uSize)
{
    ux i;
    ux begin = (ux)(((u8 *)_pBegin) - mpu8_BegMemory);
    ux firstPage = (begin & ~(ITF_MEM_PAGE_SIZE-1))/ITF_MEM_PAGE_SIZE;
    ux end = begin + _uSize;
    ux lastPage = ((end-1) & ~(ITF_MEM_PAGE_SIZE-1))/ITF_MEM_PAGE_SIZE;
    for(i = firstPage; i <= lastPage; i++)
    {
        if (m_aMemoryID[i] == ITF_MEM_UNUSED_ID)
        {
            //printf("HEAP[0x%x]: Commit page %d\n",(u32)this,i);
            // Allocate physical memory
            int error = sys_mmapper_allocate_memory(ITF_MEM_PAGE_SIZE,SYS_MEMORY_GRANULARITY_1M,&m_aMemoryID[i]);
            if (error != CELL_OK)
            {
                m_aMemoryID[i] = ITF_MEM_UNUSED_ID;
                return bfalse;
            }

            // Map physical memory to virtual address.
            error = sys_mmapper_map_memory((sys_addr_t)(mpu8_BegMemory+i*ITF_MEM_PAGE_SIZE),m_aMemoryID[i],SYS_MEMORY_PROT_READ_WRITE);
            ITF_ASSERT(error == CELL_OK);
        }
    }
    AfterCommit(_pBegin,_uSize);
    return btrue;
}

void Heap::Decommit(void *_pBegin, ux _uSize)
{
    void *pPageAddress;
    i32 iPageSize;
    ITF_MEM_ComputeIncludedPages(_pBegin,_uSize,ITF_MEM_PAGE_SIZE,&pPageAddress,&iPageSize);

    ux i;
    ux firstPage = (ux)((((u8 *)pPageAddress) - mpu8_BegMemory)/ITF_MEM_PAGE_SIZE);
    ux lastPage = firstPage + (iPageSize/ITF_MEM_PAGE_SIZE);
    for(i = firstPage; i < lastPage; i++)
    {
        if (m_aMemoryID[i] != ITF_MEM_UNUSED_ID)
        {
            //printf("HEAP[0x%x]: Decommit page %d\n",(u32)this,i);
            // Unmap physical memory from virtual address
            sys_memory_t mem_id;
            sys_addr_t address = (sys_addr_t) (((u8 *)mpu8_BegMemory) + i * ITF_MEM_PAGE_SIZE);
            int error = sys_mmapper_unmap_memory(address,&mem_id);
            ITF_ASSERT(error == CELL_OK);
            ITF_ASSERT(mem_id == m_aMemoryID[i]);
            m_aMemoryID[i] = ITF_MEM_UNUSED_ID;

            // Free physical memory
            error = sys_mmapper_free_memory(mem_id);
            ITF_ASSERT(error == CELL_OK);
        }
    }
    if (iPageSize>0)
        AfterDecommit(pPageAddress,iPageSize);
}
#elif defined (ITF_CTR)
// No virtual addressing on CTR
bbool Heap::bCommit(void *_pBegin, ux _uSize)
{
    return (TRUE);
}

void Heap::Decommit(void *_pBegin, ux _uSize)
{
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::Destroy(void)
{
#ifdef ITF_PS3
	Decommit(mpu8_BegMemory,mpu8_EndMemory-mpu8_BegMemory);
#endif //ITF_PS3
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool Heap::AddFreeBlock(void *_pAddress, u32 _size, void *_pNoDecommitAddress, u32 _uNoDecommitSize)
{
    // Find index corresponding to size.
	u32 uIndex = uSizeToIndex(_size);
	FreeBlock *pFreeBlockInList = m_apFreeBlocks[uIndex];

    // Flag management 
    u32 uIntIndex = uIndex>>5; 					
    u32 uBitIndex = uIndex - (uIntIndex<<5);
    u32 uBitInInt = 31 - uBitIndex;

    u32 uFlagWithBit = 1<<uBitInInt;
    u32 uIntFlag = m_auFreeFlags[uIntIndex];
    
    // Check coherency between flags and lists.
    ITF_ASSERT(!(uIntFlag & uFlagWithBit) == (pFreeBlockInList == NULL));

    // Add flag
    m_auFreeFlags[uIntIndex] |= uFlagWithBit;

    // Create free block
    FreeBlock *pFreeBlock = (FreeBlock *)_pAddress;

    // Last 4 bytes of free block point to beginning of free block
    FreeBlock **ppEndAddress = (FreeBlock **) (((u8 *)_pAddress) + (_size-4));

    if (!(bCommit(pFreeBlock,sizeof(FreeBlock)) && 
        bCommit(ppEndAddress,sizeof(FreeBlock *)+sizeof(HeaderAlloc))))
        return bfalse;

    pFreeBlock->m_Header.m_bIsFree = 1;
    pFreeBlock->m_Header.SetSize(_size);

    // Add free block in list 
    pFreeBlock->m_pNext = pFreeBlockInList;
    pFreeBlock->m_pPrev = NULL;
    if (pFreeBlockInList)
    {
        pFreeBlockInList->m_pPrev = pFreeBlock;
    }
    m_apFreeBlocks[uIndex] = pFreeBlock;

    // Write end address
    (*ppEndAddress) = pFreeBlock;

    // Inform next block that this block is free.
    HeaderAlloc *pNextHeader = ((HeaderAlloc *)ppEndAddress) + 1;
	ITF_ASSERT(!pNextHeader->m_bIsFree);
	pNextHeader->m_bIsBeforeFree = 1;

    // Content of free block should be decommitted
#ifndef ITF_MEM_COMMIT_ALL
    if (_pNoDecommitAddress != ITF_MEM_NO_DECOMMIT)
    {
        FilteredDecommit((u8*)_pAddress+sizeof(FreeBlock),_size-ITF_MEM_MIN_BLOCK_SIZE,(u8*)_pNoDecommitAddress, _uNoDecommitSize);
    }
#endif //ITF_MEM_COMMIT_ALL

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : update measures
    m_uFreeNb ++;
    m_uFreeSize += _size;
    if (pNextHeader != (HeaderAlloc *)(mpu8_TmpMemory - sizeof(HeaderAlloc)))
        m_uFreeInHolesSize += _size;
#endif //ITF_MEM_HEAP_DEBUG_
    return btrue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::pAddAllocBlock(void *_pAddress, u32 _uSize, u32 _uUserSize, bbool _bIsBlockBeforeFree, bbool _bAllocTmp)
{
    if (!bCommit(_pAddress,_uSize))
        return NULL;

    HeaderAlloc *pHeader = (HeaderAlloc *)_pAddress;
    pHeader->m_bIsFree = 0;
    pHeader->m_bIsBeforeFree = (_bIsBlockBeforeFree ? 1 : 0);
    pHeader->SetSize(_uSize);

	if (!_bAllocTmp)
	{
	    // Update the next block's 'm_bIsBeforeFree' flag (if alloc block)
	    HeaderAlloc *pNextHeader = (HeaderAlloc *)pGetBlockAfterAllocBlock(pHeader);
	    if (!pNextHeader->m_bIsFree)
	    {
	    	pNextHeader->m_bIsBeforeFree = 0;
	    }
	}

    void *pUserAddress = (((HeaderAlloc *)_pAddress) + 1);

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : fill alloc block
    if (m_bFillAllocBlocks)
    {
        u32 uUserSize = _uSize - sizeof(HeaderAlloc);
        ITF_MemSet(pUserAddress,ITF_MEM_FILL_ALLOC,uUserSize);
    }

    // Debug management : add guards
    if (m_bAddGuards)
    {
        u32 *pBeginGuard = (u32 *)pUserAddress;
        *pBeginGuard = ITF_MEM_BEGIN_GUARD;

        u32 *pEndGuard = (u32 *)(((u8 *) _pAddress) + (_uSize - sizeof(u32)));
        *pEndGuard = ITF_MEM_END_GUARD;

        pUserAddress = ((u8 *) pUserAddress) + sizeof(u32);
    }

    // Debug management : update measures
    m_uAllocSize += _uSize;
    m_uAllocNb ++;
#endif //ITF_MEM_HEAP_DEBUG_

    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::RemoveAllocBlock(void *_pAddress)
{
    HeaderAlloc *pHeader = (HeaderAlloc *)_pAddress;

    ITF_ASSERT(!pHeader->m_bIsFree);

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : update measures
    m_uAllocSize -= pHeader->GetSize();
    m_uAllocNb --;
#endif //ITF_MEM_HEAP_DEBUG_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
FreeBlock *Heap::pGetBestBlockForSize(FreeBlock *_pBlock, u32 _u32_StopSize)
{
    if (!_pBlock)
        return NULL;

    if (_pBlock->m_Header.GetSize() == _u32_StopSize)
        return _pBlock;

    FreeBlock *pBestBlock = NULL;

	FreeBlock *pInitialBlock = _pBlock;

    while (_pBlock)
    {
        // Block can be used only if it big enough
        // Let enough free space to new block+header greater than max pool size
        if ((_pBlock->m_Header.GetSize() == _u32_StopSize) || (_pBlock->m_Header.GetSize() >= (_u32_StopSize+ITF_MEM_BLOCK_SIZE_NB*4+ITF_MEM_MIN_BLOCK_SIZE)))
        {
            //best size and lower address
            if (pBestBlock && (_pBlock < pBestBlock) && (_pBlock->m_Header.GetSize() <= (100+pBestBlock->m_Header.GetSize())))
            	pBestBlock = NULL;	
            
            if (!pBestBlock || (_pBlock->m_Header.GetSize() < pBestBlock->m_Header.GetSize()))
            {
                pBestBlock = _pBlock;

                // Stop search if size cannot be smaller
                if (pBestBlock->m_Header.GetSize() == _u32_StopSize)
                    return pBestBlock;
            }
        }
        _pBlock = _pBlock->m_pNext;
    }
    if (pBestBlock)
    	return pBestBlock;
   
    
    //Downgraded situation
	_pBlock = pInitialBlock;
	
    while (_pBlock)
    {
        // Block can be used only if it big enough
        if (_pBlock->m_Header.GetSize() >= _u32_StopSize)
        {
            //lower address
            //if (!pBestBlock || (_pBlock < pBestBlock))
            
            //best size
            if (!pBestBlock || (_pBlock->m_Header.GetSize() < pBestBlock->m_Header.GetSize()))
            {
                pBestBlock = _pBlock;

                // Stop search if size cannot be smaller
                if (pBestBlock->m_Header.GetSize() == _u32_StopSize)
                    return pBestBlock;
            }
        }
        _pBlock = _pBlock->m_pNext;
    }
    return pBestBlock;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
FreeBlock *Heap::pSearchAndRemoveFreeBlock(u32 _u32_Size)
{ 
    FreeBlock *pBlock = pSearchFreeBlock(_u32_Size);
    if(!pBlock)
    	return NULL;
 
    // If the best block is the last block, search for a bigger block 
    // in order to avoid to use the last block if possible 
    // so that the last block stays available for pools.
    if (pBlock == pGetLastFreeBlock())
    {
        FreeBlock *pBiggerBlock = pSearchFreeBlock(pBlock->m_Header.GetSize()+4);
        if (pBiggerBlock)
            pBlock = pBiggerBlock;
    }

    // Remove block from free blocks.
	if (pBlock)
	{
        ITF_ASSERT(pBlock->m_Header.GetSize() >= _u32_Size);
        RemoveFreeBlock(pBlock);
	}
	
	return pBlock;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
FreeBlock *Heap::pSearchFreeBlock(u32 _u32_Size)
{
    // Are there free blocks at uIndex ?
	u32 uIndex = uSizeToIndex(_u32_Size);
	FreeBlock *pBlock = m_apFreeBlocks[uIndex];
    FreeBlock *pBestBlock = pGetBestBlockForSize(pBlock,_u32_Size);

    if (!pBestBlock)
    {
        // Search in current table, but only for big enough blocks (remove from search too small blocks with mask).
	    u32 uIntIndex = uIndex>>5; 					
	    u32 uBitIndex = uIndex - (uIntIndex<<5);
	    u32 uIntFlag = m_auFreeFlags[uIntIndex];
        
        u32 uFlagMask = (uBitIndex == 31 ? 0 : (0xFFFFFFFF >> (uBitIndex+1)));
        u32 uMaskedIntFlag = (uIntFlag & uFlagMask);

        if (uMaskedIntFlag)
        {
            // Free block found for current int
			uBitIndex = ITF::BitTweak::countLeadingZero(uMaskedIntFlag);
        }
        else
        {
            // Search free block in other ints.
            uIntIndex++;
            while (uIntIndex < ITF_MEM_FREE_FLAG_NB)
            {
			    u32 uIntFlag = m_auFreeFlags[uIntIndex];
			    
                if (uIntFlag)
                {
		            uBitIndex = ITF::BitTweak::countLeadingZero(uIntFlag);
                    break;
                }
                uIntIndex ++;
            }
            if (uIntIndex >= ITF_MEM_FREE_FLAG_NB)
            {
                // Could not find a free block.
                return NULL;
            }
        }
        
        uIndex = uBitIndex + (uIntIndex<<5);
        u32 uStopSize = uIndexToSize(uIndex);
        ITF_ASSERT(uStopSize >= _u32_Size);        
        pBestBlock = pGetBestBlockForSize(m_apFreeBlocks[uIndex],uStopSize);
    }
        
    return pBestBlock;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::RemoveFreeBlock(FreeBlock *_pFreeBlock)
{
    ITF_ASSERT(_pFreeBlock->m_Header.m_bIsFree);
    
    // Find index corresponding to size.
	u32 uIndex = uSizeToIndex(_pFreeBlock->m_Header.GetSize());
	FreeBlock *pFreeBlockList = m_apFreeBlocks[uIndex];
    
    // Remove bit in flag ?
    if ((pFreeBlockList == _pFreeBlock) && (!_pFreeBlock->m_pNext))
    {
	    u32 uIntIndex = uIndex>>5; 					
	    u32 uBitIndex = uIndex - (uIntIndex<<5);
	    u32 uBitInInt = 31 - uBitIndex;

	    u32 uFlagWithBit = 1<<uBitInInt;
	    m_auFreeFlags[uIntIndex] &= ~uFlagWithBit;
    }
    
    // Remove block from table.
    if (pFreeBlockList == _pFreeBlock)
    {
        // _pFreeBlock is the first block in the list.
        m_apFreeBlocks[uIndex] = _pFreeBlock->m_pNext;
        if (m_apFreeBlocks[uIndex])
        {
            m_apFreeBlocks[uIndex]->m_pPrev = NULL;
        }        
    }
    else
    {
        // _pFreeBlock is not the first block in the list.
        ITF_ASSERT(_pFreeBlock->m_pPrev);
        _pFreeBlock->m_pPrev->m_pNext = _pFreeBlock->m_pNext;
        if (_pFreeBlock->m_pNext)
        {
            _pFreeBlock->m_pNext->m_pPrev = _pFreeBlock->m_pPrev;
        }
    }

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : Update measures
    u32 size = _pFreeBlock->m_Header.GetSize();
    m_uFreeNb --;
    m_uFreeSize -= size;
    Header *pNextHeader = (Header *) pGetBlockAfterFreeBlock((HeaderFree *)_pFreeBlock);
    if (((u8 *)pNextHeader) != (mpu8_TmpMemory - sizeof(HeaderAlloc)))
        m_uFreeInHolesSize -= size;

    // Debug management : check that free data is filled with correct data (check begin and end of buffer)
    if (m_bFillFreeBlocks && (size > ITF_MEM_MIN_BLOCK_SIZE))
    {
        u32 *pBeginFreeBuffer = (u32 *)(((u8 *)_pFreeBlock) + 3 * sizeof(u32));
        CheckFillFree(pBeginFreeBuffer);

        u32 *pEndFreeBuffer = (u32 *)(((u8 *)_pFreeBlock) + size - 2 * sizeof(u32));
        CheckFillFree(pEndFreeBuffer);
    }
#endif //ITF_MEM_HEAP_DEBUG_
}
 
/*
 =======================================================================================================================
 _i32_Size is signed because mpu8_TmpMemory can go forward (_i32_Size > 0) or backward (_i32_Size < 0)
 =======================================================================================================================
 */
HeaderAllocTmp * Heap::pMoveTmpPointer(i32 _i32_Size)
{
    ITF_ASSERT(mpu8_TmpMemory + _i32_Size <= mpu8_EndMemory);

    // Get last free buffer.
    FreeBlock *pLastFree = pGetLastFreeBlock();

    // Check for enough memory
    i32 iNewFreeBlockSize = (pLastFree ? pLastFree->m_Header.GetSize() : 0) + _i32_Size;
    if (iNewFreeBlockSize < ITF_MEM_MIN_BLOCK_SIZE)
    {
        return NULL;
    }
    
#ifdef ITF_MEM_HEAP_DEBUG_
	bbool b_IsNewBlock = (pLastFree == NULL);
#endif //ITF_MEM_HEAP_DEBUG_

    // Resize free block (or create it if it does not exist).
    if (pLastFree)
    {
	    RemoveFreeBlock(pLastFree);
	}
	else
	{
		pLastFree = (FreeBlock *)(mpu8_TmpMemory-sizeof(HeaderAlloc));
	}

	// Create alloc block
	mpu8_TmpMemory += _i32_Size;
	
	if (!CreatePseudoAllocBlock())
        return NULL;

    if (!AddFreeBlock(pLastFree,(u32)iNewFreeBlockSize,((u8*)pLastFree)+sizeof(FreeBlock),pLastFree->m_Header.GetSize()-ITF_MEM_MIN_BLOCK_SIZE))
        return NULL;

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : fill free block (only necessary data)
    if (m_bFillFreeBlocks && (_i32_Size>0))
    {
        if(b_IsNewBlock)
            FillFreeMemory(mpu8_TmpMemory - _i32_Size + 2*sizeof(u32),_i32_Size - 4*sizeof(u32));
        else
            FillFreeMemory(mpu8_TmpMemory - _i32_Size - 2*sizeof(u32),_i32_Size);
    }
#endif //ITF_MEM_HEAP_DEBUG_


    return (HeaderAllocTmp *)mpu8_TmpMemory;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 Heap::uSizeToIndex(u32 _uSize)
{
    ITF_ASSERT(_uSize >= ITF_MEM_MIN_BLOCK_SIZE);
    ITF_ASSERT(_uSize < ITF_MEM_MAX_BLOCK_SIZE);
    
	if (_uSize < ITF_MEM_SMALL_BLOCK_SIZE)
	{
		// 16 -> 0, 20 -> 1, ...
		return (_uSize-ITF_MEM_MIN_BLOCK_SIZE)>>2;
	}
	else
	{
		// size is between 2^(i+N) + jP * 2^(i+N)/P and 2^(i+N) + (j+1)P * 2^(i+N)/P 
		// where N is ITF_MEM_SMALL_SIZE_POW2 and P is ITF_MEM_BUCKET_NB.
	
		u32 uPowOf2 = ITF::BitTweak::powerOfTwo(_uSize);
	    u32 i = uPowOf2 - ITF_MEM_SMALL_SIZE_POW2;
	    ITF_ASSERT(i<ITF_MEM_MAX_SIZE_POW2);
	    
	    u32 uSizeAtThisPowOf2 = 1<<uPowOf2;
	    
	    u32 uDeltaSize = _uSize - uSizeAtThisPowOf2;
	    ITF_ASSERT(uDeltaSize < uSizeAtThisPowOf2);

        u32 uDenominator = uSizeAtThisPowOf2 >> ITF_MEM_BUCKET_POW2;	    
	    u32 j = uDeltaSize / uDenominator;  // To avoid int overrun with (uDeltaSize*ITF_MEM_BUCKET_NB)/uDenominator

	    ITF_ASSERT(j < ITF_MEM_BUCKET_NB);
	    
	    return ITF_MEM_SMALL_BLOCK_INDEX + i*ITF_MEM_BUCKET_NB + j;		
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 Heap::uIndexToSize(u32 _uIndex)
{
	if (_uIndex < ITF_MEM_SMALL_BLOCK_INDEX)
	{
		// 0 -> 16, 1-> 20 ...
		return (_uIndex<<2)+ITF_MEM_MIN_BLOCK_SIZE;
	}
	else
	{
		u32 uIndexAfterSmall = _uIndex-ITF_MEM_SMALL_BLOCK_INDEX;
		u32 i = (uIndexAfterSmall >> ITF_MEM_BUCKET_POW2); // Better than (uIndexAfterSmall/ITF_MEM_BUCKET_NB); 		// Div
		ITF_ASSERT(i<ITF_MEM_MAX_SIZE_POW2);
		
		u32 j = uIndexAfterSmall - (i*ITF_MEM_BUCKET_NB);	// Mod
		ITF_ASSERT(j < ITF_MEM_BUCKET_NB);
		
		u32 uPowOf2 = ITF_MEM_SMALL_SIZE_POW2 + i;
		u32 uSizeAtThisPowOf2 = (1<<uPowOf2);
		
		u32 uDeltaSize = j * (uSizeAtThisPowOf2 >> ITF_MEM_BUCKET_POW2);
		
		return uSizeAtThisPowOf2 + uDeltaSize;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool Heap::CreatePseudoAllocBlock()
{
	HeaderAlloc *pHeader = (HeaderAlloc *)(mpu8_TmpMemory - sizeof (HeaderAlloc));
    if (!bCommit(pHeader,4))
    {
        return bfalse;
    }
    else
    {
        pHeader->m_bIsFree = 0;
        pHeader->m_bIsBeforeFree = 1;
        pHeader->SetSize(sizeof (HeaderAlloc));
        return btrue;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool Heap::b_IsPointerValid(void *_pAddress)
{
#if (defined ITF_MEM_HEAP_DEBUG_) && !(defined ITF_WII) && !(defined ITF_CTR)
	if (
#ifdef ITF_X360		
		m_bVirtual && 
#endif //ITF_X360		
		!bIsAddressCommitted(_pAddress))
	{
		return bfalse;
	}
#endif // ITF_MEM_HEAP_DEBUG_

	if (b_IsPointerInHeap(_pAddress))
	{
		if (!(((u32)_pAddress) & 3))
		{
			if (_pAddress < mpu8_PoolMemory)
			{
				HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pAddress);
				
				return (!pHeader->m_bIsFree) && bAreGuardsValid(_pAddress,pHeader->GetSize() - 3 * sizeof(u32));
			}
			else
			{
				void *pShiftedAddress = pGuardShiftPoolAddress(_pAddress);
				u32 uPool = uGetPoolIndex((u8 *)pShiftedAddress);
				Pool *pPool = pGetPool(uPool);
				
				u32 uIndexInPool = pPool->uGetIndexFromAddress((u8 *)pShiftedAddress);
				void *pAddressInPool = pPool->p_GetBlockAddress(uIndexInPool);
				return (pAddressInPool == pShiftedAddress) && bAreGuardsValid(_pAddress,pPool->uGetBlockSize()- 2 * sizeof(u32));
			}
		}
	}
	return bfalse;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 Heap::u_GetBiggestAllocPossible()
{
 	u32 uIndex = 0xFFFFFFFF;
 	i32 IntIndex;
 	for (IntIndex=ITF_MEM_FREE_FLAG_NB-1; IntIndex>=0; IntIndex--)
 	{
 		u32 uIntFlag = m_auFreeFlags[IntIndex];
 		if (uIntFlag)
 		{
 			uIndex = 31 + (IntIndex<<5);
 			break;
 		}
 	}
 	if (uIndex == 0xFFFFFFFF)
 		return 0;
 		
	uIndex = ITF::Min<u32>(uIndex,ITF_MEM_FREE_BLOCKS_NB-1);
 	
	FreeBlock *pFreeBlockInList = m_apFreeBlocks[uIndex];
	while (!pFreeBlockInList)
	{
		uIndex--;
		pFreeBlockInList = m_apFreeBlocks[uIndex];
	}

	u32 uBiggestSize = pFreeBlockInList->m_Header.GetSize();
	while (pFreeBlockInList)
	{
		if (uBiggestSize < pFreeBlockInList->m_Header.GetSize())
		{
			uBiggestSize = pFreeBlockInList->m_Header.GetSize();
		}
		pFreeBlockInList = pFreeBlockInList->m_pNext;
	}
	
	u32 uHeaderSize = sizeof(HeaderAlloc);
	AddGuardSize(&uHeaderSize);
	return uBiggestSize - uHeaderSize;
}


#ifdef ITF_MEM_HEAP_DEBUG_

#ifdef ITF_MEM_COMMIT_ALL
void Heap::FillFreeMemory(void *_pFillAddress,u32 _uFillSize)
{
    ITF_MemSet(_pFillAddress,ITF_MEM_FILL_FREE,_uFillSize);
}
void Heap::CheckFillFree(void *_p)
{
    ITF_ASSERT(ITF_MEM_FILL_FREE == *(u32 *)_p);
}

void Heap::CheckDecommitted(void *_pBegin,u32 _uSize)
{
}
#else  // ITF_MEM_COMMIT_ALL
void Heap::FillFreeMemory(void *_pFillAddress,u32 _uFillSize)
{
    // Does <_pFillAddress,_uFillSize> include a region which can be not committed ? 
    void *pPageAddress;
    i32 iPageSize;
    ITF_MEM_ComputeIncludedPages(_pFillAddress,_uFillSize,m_PageSize,&pPageAddress,&iPageSize);
    if (iPageSize<0)
    {
        // <_pFillAddress,_uFillSize> is entirely committed to memory
        ITF_MemSet(_pFillAddress,ITF_MEM_FILL_FREE,_uFillSize);
    }
    else
    {
        ux firstPage = (ux)((((u8 *)pPageAddress) - mpu8_BegMemory)/m_PageSize);
        ux lastPage = firstPage + (iPageSize/m_PageSize);

        // First page
        i32 beforeSize = ((u8*)pPageAddress) - ((u8*)_pFillAddress);
        if (beforeSize > 0 &&  bIsPageCommitted(firstPage-1))
        {
            ITF_MemSet(_pFillAddress,ITF_MEM_FILL_FREE,beforeSize);
        }

        // Last Page
        void *pEndPageAddress = (void *)(((u8*)pPageAddress) + iPageSize);
        i32 afterSize = (_uFillSize - (iPageSize+beforeSize));
        if (afterSize > 0 && bIsPageCommitted(lastPage))
            ITF_MemSet(pEndPageAddress,ITF_MEM_FILL_FREE,afterSize);
    }
}

void Heap::CheckFillFree(void *_p)
{
    // Don't do the check if the address is not committed.
    ITF_ASSERT((!bIsAddressCommitted(_p)) || (ITF_MEM_FILL_FREE == *(u32 *)_p));
}

// Check that inside the range of addresses, the memory is decommitted
void Heap::CheckDecommitted(void *_pBegin,u32 _uSize)
{
    void *pPageAddress;
    i32 iPageSize;
    ITF_MEM_ComputeIncludedPages(_pBegin,_uSize,m_PageSize,&pPageAddress,&iPageSize);
    if (iPageSize>0)
    {
        ux firstPage = (ux)((((u8 *)pPageAddress) - mpu8_BegMemory)/m_PageSize);
        ux lastPage = firstPage + (iPageSize/m_PageSize);
        ux page;
        for (page = firstPage ; page < lastPage; page++)
        {
            ITF_ASSERT(!bIsPageCommitted(page));
        }
    }
}


#endif // ITF_MEM_COMMIT_ALL
#endif //ITF_MEM_HEAP_DEBUG_

#ifndef ITF_MEM_COMMIT_ALL
void Heap::FilteredDecommit(u8 *_pDecommitBegin, ux _uDecommitSize, u8 *_pNoDecommitBegin, ux _uNoDecommitSize)
{
	if (_pNoDecommitBegin == NULL)
		Decommit(_pDecommitBegin,_uDecommitSize);
	else
	{
		// FilteredDecommit reduces the span of addresses that we try to decommit,
		// because we know that the "no decommit" addresses are already decommitted.
		u8 *pDecommitEnd = _pDecommitBegin + _uDecommitSize;
		u8 *pNoDecommitEnd = _pNoDecommitBegin + _uNoDecommitSize;

		// Reduce the "no decommit" area to pages included in the area
		u32 uPageSize_1 = m_PageSize-1;
		u32 uMask = ~uPageSize_1;
		_pNoDecommitBegin = (u8 *)( ((u32)(_pNoDecommitBegin + uPageSize_1)) & uMask);
		pNoDecommitEnd = (u8 *)(((u32)pNoDecommitEnd) & uMask);

		if ((pDecommitEnd < _pNoDecommitBegin) || (_pDecommitBegin > pNoDecommitEnd))
		{
			Decommit(_pDecommitBegin,pDecommitEnd-_pDecommitBegin);
		}
		else
		{
			if (_pDecommitBegin < _pNoDecommitBegin)
				Decommit(_pDecommitBegin,_pNoDecommitBegin-_pDecommitBegin);
			if (pNoDecommitEnd < pDecommitEnd)
				Decommit(pNoDecommitEnd,pDecommitEnd-pNoDecommitEnd);
		}
	}
}
#endif //ITF_MEM_COMMIT_ALL

} //namespace ITF

#endif //ENABLE_ALLOCATOR
