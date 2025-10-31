#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_macros_WII.h"
#include "memory/WII/MEM_heap_WII.h"
#include "math/WII/MTH_int_WII.h"

namespace ITF
{

#ifdef _HEAP_DEBUG_
bbool MEM_Heap::m_bFillFreeBlocks = bfalse;
bbool MEM_Heap::m_bFillAllocBlocks = bfalse;
bbool MEM_Heap::m_bAddGuards = bfalse;

bbool MEM_Heap::m_bCheckFreeBlocks = btrue;
bbool MEM_Heap::m_bCheckPools = btrue;
bbool MEM_Heap::m_bCheckAllBlocks = btrue;
#endif /* _HEAP_DEBUG_ */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_Heap::MEM_Heap()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_Heap::~MEM_Heap()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::Create(u32 _uHeapSize, u8 *_pBeginAddress, bbool _bVirtual)
{
    u32 i;

	// Heap pointers
	mpu8_BegMemory = _pBeginAddress;
	mpu8_PoolMemory = _pBeginAddress + _uHeapSize;
	mpu8_EndMemory = mpu8_PoolMemory;

#ifdef _HEAP_SIZE_DEBUG_
    // Debug management : measures
    m_uTotalSize = _uHeapSize - sizeof(MEM_HeaderAlloc); // header used by pseudo-alloc.
    m_uCommittedSize = m_uTotalSize;
    m_uAllocSize = 0; 
    m_uAllocPoolSize = 0; 
#endif // _HEAP_SIZE_DEBUG_

#ifdef _HEAP_DEBUG_
    // Debug management : measures
    m_uFreeSize = 0;
    m_uFreeInHolesSize = 0;
    m_uFreePoolSize = 0;
    m_uFreeNb = 0;
    m_uAllocNb = 0;

    for (i=0; i<MEM_BLOCK_SIZE_NB; i++)
    {
        m_uFreeSizePerSize[i] = 0;
        m_uAllocSizePerSize[i] = 0;
        m_uTotalSizePerSize[i] = 0;
    }
#endif //_HEAP_DEBUG_

    bCommit(_pBeginAddress,_uHeapSize);

	// Clear free block tables.
	ITF_MemSet(m_apFreeBlocks, 0, sizeof(m_apFreeBlocks));
	ITF_MemSet(m_auFreeFlags, 0, sizeof(m_auFreeFlags));
	
    // Init pool buffers : empty for now
    m_EmptyPools = MEM_POOL_INVALID;
    m_FullPools = MEM_POOL_INVALID;
    for (i=0; i<MEM_BLOCK_SIZE_NB; i++)
        m_AvailablePools[i] = MEM_POOL_INVALID;

    // Pseudo-alloc block is a header of an alloc block at end of normal stack, with size 0.
    CreatePseudoAllocBlock();

    // Commit start of free block.
    bCommit(_pBeginAddress,sizeof(MEM_FreeBlock));

    // Add free block
    AddFreeBlock(mpu8_BegMemory,_uHeapSize-sizeof(MEM_HeaderAlloc),NULL,0);

#ifdef _HEAP_DEBUG_
    if (m_bFillFreeBlocks)
    {
        FillFreeMemory(((u8 *)mpu8_BegMemory)+sizeof(MEM_FreeBlock),_uHeapSize-(sizeof(MEM_FreeBlock)+sizeof(MEM_FreeBlock * ) + sizeof(MEM_HeaderAlloc)));
    }
#endif //_HEAP_DEBUG_
}

// Transforms an address range (address + size) to the biggest page address range that is totally included in the address range.
inline void MEM_ComputeIncludedPages(void *_pBegin, u32 _uSize, u32 _uPageSize, void **_ppPageIncludedBegin, i32 *_piPageIncludedSize)
{
    u32 uPageSize_1 = _uPageSize-1;
    u32 uMask = ~uPageSize_1;
    u32 uPageBegin = (((u32)_pBegin) + uPageSize_1) & uMask;
    *_ppPageIncludedBegin = (void *)uPageBegin;

    u32 uEndAddress = ((u32)_pBegin) + _uSize;
    u32 uPageEnd = uEndAddress & uMask;
    *_piPageIncludedSize = uPageEnd-uPageBegin;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::Destroy(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool MEM_Heap::AddFreeBlock(void *_pAddress, u32 _size, void *_pNoDecommitAddress, u32 _uNoDecommitSize)
{
    // Find index corresponding to size.
	u32 uIndex = uSizeToIndex(_size);
	MEM_FreeBlock *pFreeBlockInList = m_apFreeBlocks[uIndex];

    // Flag management 
    u32 uIntIndex = uIndex>>5; 					
    u32 uBitIndex = uIndex - (uIntIndex<<5);
    u32 uBitInInt = 31 - uBitIndex;

    u32 uFlagWithBit = 1<<uBitInInt;
    u32 uIntFlag = m_auFreeFlags[uIntIndex];
    
    // Check coherency between flags and lists.
    ITF_ASSERT_CRASH(!(uIntFlag & uFlagWithBit) == (pFreeBlockInList == NULL), "Wrong flags");

    // Add flag
    m_auFreeFlags[uIntIndex] |= uFlagWithBit;

    // Create free block
    MEM_FreeBlock *pFreeBlock = (MEM_FreeBlock *)_pAddress;

    // Last 4 bytes of free block point to beginning of free block
    MEM_FreeBlock **ppEndAddress = (MEM_FreeBlock **) (((u8 *)_pAddress) + (_size-4));

    if (!(bCommit(pFreeBlock,sizeof(MEM_FreeBlock)) && 
        bCommit(ppEndAddress,sizeof(MEM_FreeBlock *)+sizeof(MEM_HeaderAlloc))))
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
    MEM_HeaderAlloc *pNextHeader = ((MEM_HeaderAlloc *)ppEndAddress) + 1;
	ITF_ASSERT_CRASH(!pNextHeader->m_bIsFree, "Wrong header");
	pNextHeader->m_bIsBeforeFree = 1;

#ifdef _HEAP_DEBUG_
    // Debug management : update measures
    m_uFreeNb ++;
    m_uFreeSize += _size;
    if (pNextHeader != (MEM_HeaderAlloc *)(mpu8_PoolMemory - sizeof(MEM_HeaderAlloc)))
        m_uFreeInHolesSize += _size;
#endif //_HEAP_DEBUG_
    return btrue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::pAddAllocBlock(void *_pAddress, u32 _uSize, u32 _uUserSize, bbool _bIsBlockBeforeFree, bbool _bAllocTmp)
{
    if (!bCommit(_pAddress,_uSize))
        return NULL;

    MEM_HeaderAlloc *pHeader = (MEM_HeaderAlloc *)_pAddress;
    u32 uWastedSize = _uSize - _uUserSize;
    ITF_ASSERT_CRASH(uWastedSize < 32, "Wrong size");
    pHeader->m_bIsFree = 0;
    pHeader->m_bIsBeforeFree = (_bIsBlockBeforeFree ? 1 : 0);
    pHeader->SetSize(_uSize);
    pHeader->m_uWastedSize = uWastedSize;

	if (!_bAllocTmp)
	{
	    // Update the next block's 'm_bIsBeforeFree' flag (if alloc block)
	    MEM_HeaderAlloc *pNextHeader = (MEM_HeaderAlloc *)pGetBlockAfterAllocBlock(pHeader);
	    if (!pNextHeader->m_bIsFree)
	    {
	    	pNextHeader->m_bIsBeforeFree = 0;
	    }
	}

    void *pUserAddress = (((MEM_HeaderAlloc *)_pAddress) + 1);

#ifdef _HEAP_DEBUG_
    // Debug management : fill alloc block
    if (m_bFillAllocBlocks)
    {
        u32 uUserSize = _uSize - sizeof(MEM_HeaderAlloc);
        ITF_MemSet(pUserAddress, M_MEM_FILL_ALLOC, uUserSize);
    }

    // Debug management : add guards
    if (m_bAddGuards)
    {
        u32 *pBeginGuard = (u32 *)pUserAddress;
        *pBeginGuard = M_MEM_BEGIN_GUARD;

        u32 *pEndGuard = (u32 *)(((u8 *) _pAddress) + (_uSize - sizeof(u32)));
        *pEndGuard = M_MEM_END_GUARD;

        pUserAddress = ((u8 *) pUserAddress) + sizeof(u32);
    }

    // Debug management : update measures
    m_uAllocNb ++;
#endif //_HEAP_DEBUG_
#ifdef _HEAP_SIZE_DEBUG_
    // Debug management : update measures
    m_uAllocSize += _uSize;
#endif // _HEAP_SIZE_DEBUG_

    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::RemoveAllocBlock(void *_pAddress)
{
    MEM_HeaderAlloc *pHeader = (MEM_HeaderAlloc *)_pAddress;

    ITF_ASSERT_CRASH(!pHeader->m_bIsFree, "Wrong header");

#ifdef _HEAP_DEBUG_
    // Debug management : update measures
    m_uAllocNb --;
#endif //_HEAP_DEBUG_
#ifdef _HEAP_SIZE_DEBUG_
    // Debug management : update measures
    m_uAllocSize -= pHeader->GetSize();
#endif // _HEAP_SIZE_DEBUG_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_FreeBlock *MEM_Heap::pGetBestBlockForSize(MEM_FreeBlock *_pBlock, u32 _u32_StopSize)
{
    if (!_pBlock)
        return NULL;

    if (_pBlock->m_Header.GetSize() == _u32_StopSize)
        return _pBlock;

    MEM_FreeBlock *pBestBlock = NULL;

	MEM_FreeBlock *pInitialBlock = _pBlock;

    while (_pBlock)
    {
        // Block can be used only if it big enough
        // Let enough free space to new block+header greater than max pool size
        if ((_pBlock->m_Header.GetSize() == _u32_StopSize) || (_pBlock->m_Header.GetSize() >= (_u32_StopSize+MEM_BLOCK_SIZE_NB*4+MEM_MIN_BLOCK_SIZE)))
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
MEM_FreeBlock *MEM_Heap::pSearchAndRemoveFreeBlock(u32 _u32_Size)
{ 
    MEM_FreeBlock *pBlock = pSearchFreeBlock(_u32_Size);
	if(!pBlock)
		return NULL;
 
    // If the best block is the last block, search for a bigger block 
    // in order to avoid to use the last block if possible 
    // so that the last block stays available for pools.
    if (pBlock == pGetLastFreeBlock())
    {
        MEM_FreeBlock *pBiggerBlock = pSearchFreeBlock(pBlock->m_Header.GetSize()+4);
        if (pBiggerBlock)
            pBlock = pBiggerBlock;
    }

    // Remove block from free blocks.
	if (pBlock)
	{
		ITF_ASSERT_CRASH(pBlock->m_Header.GetSize() >= _u32_Size,"Memory error: bad free block header size");
        RemoveFreeBlock(pBlock);
	}
	
	return pBlock;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MEM_FreeBlock *MEM_Heap::pSearchFreeBlock(u32 _u32_Size)
{
    // Are there free blocks at uIndex ?
	u32 uIndex = uSizeToIndex(_u32_Size);
	MEM_FreeBlock *pBlock = m_apFreeBlocks[uIndex];
    MEM_FreeBlock *pBestBlock = pGetBestBlockForSize(pBlock,_u32_Size);

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
            uBitIndex = u32_CountLeadingZero(uMaskedIntFlag);
        }
        else
        {
            // Search free block in other ints.
            uIntIndex++;
            while (uIntIndex < MEM_FREE_FLAG_NB)
            {
			    u32 uIntFlag = m_auFreeFlags[uIntIndex];
			    
                if (uIntFlag)
                {
		            uBitIndex = u32_CountLeadingZero(uIntFlag);
                    break;
                }
                uIntIndex ++;
            }
            if (uIntIndex >= MEM_FREE_FLAG_NB)
            {
                // Could not find a free block.
                return NULL;
            }
        }
        
        uIndex = uBitIndex + (uIntIndex<<5);
        u32 uStopSize = uIndexToSize(uIndex);
        ITF_ASSERT_CRASH(uStopSize >= _u32_Size, "Wrong header");
        pBestBlock = pGetBestBlockForSize(m_apFreeBlocks[uIndex],uStopSize);
    }
    return pBestBlock;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::RemoveFreeBlock(MEM_FreeBlock *_pFreeBlock)
{
    ITF_ASSERT_CRASH(_pFreeBlock->m_Header.m_bIsFree, "Wrong header");
    
    // Find index corresponding to size.
	u32 uIndex = uSizeToIndex(_pFreeBlock->m_Header.GetSize());
	MEM_FreeBlock *pFreeBlockList = m_apFreeBlocks[uIndex];
    
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
        ITF_ASSERT_CRASH(_pFreeBlock->m_pPrev, "Wrong block");
        _pFreeBlock->m_pPrev->m_pNext = _pFreeBlock->m_pNext;
        if (_pFreeBlock->m_pNext)
        {
            _pFreeBlock->m_pNext->m_pPrev = _pFreeBlock->m_pPrev;
        }
    }

#ifdef _HEAP_DEBUG_
    // Debug management : Update measures
    u32 size = _pFreeBlock->m_Header.GetSize();
    m_uFreeNb --;
    m_uFreeSize -= size;
    MEM_Header *pNextHeader = (MEM_Header *) pGetBlockAfterFreeBlock((MEM_HeaderFree *)_pFreeBlock);
    if (((u8 *)pNextHeader) != (mpu8_PoolMemory - sizeof(MEM_HeaderAlloc)))
        m_uFreeInHolesSize -= size;

    // Debug management : check that free data is filled with correct data (check begin and end of buffer)
    if (m_bFillFreeBlocks && (size > MEM_MIN_BLOCK_SIZE))
    {
        u32 *pBeginFreeBuffer = (u32 *)(((u8 *)_pFreeBlock) + 3 * sizeof(u32));
        CheckFillFree(pBeginFreeBuffer);

        u32 *pEndFreeBuffer = (u32 *)(((u8 *)_pFreeBlock) + size - 2 * sizeof(u32));
        CheckFillFree(pEndFreeBuffer);
    }
#endif //_HEAP_DEBUG_
}

 /*
 =======================================================================================================================
 _i32_Size is signed because mpu8_PoolMemory can go forward (_i32_Size > 0) or backward (_i32_Size < 0)
 =======================================================================================================================
 */
u8 *MEM_Heap::pMovePoolPointer(i32 _i32_Size)
{
    ITF_ASSERT_CRASH(mpu8_PoolMemory + _i32_Size <= mpu8_EndMemory, "Bad move");

    // Get last free buffer.
    MEM_FreeBlock *pLastFree = pGetLastFreeBlock();

    // Check for enough memory
    i32 iNewFreeBlockSize = (pLastFree ? pLastFree->m_Header.GetSize() : 0) + _i32_Size;
    if (iNewFreeBlockSize < MEM_MIN_BLOCK_SIZE)
    {
        return NULL;
    }
    
#ifdef _HEAP_DEBUG_
	bbool b_IsNewBlock = (pLastFree == NULL);
#endif //_HEAP_DEBUG_

    // Resize free block (or create it if it does not exist).
    if (pLastFree)
    {
	    RemoveFreeBlock(pLastFree);
	}
	else
	{
		pLastFree = (MEM_FreeBlock *)(mpu8_PoolMemory-sizeof(MEM_HeaderAlloc));
	}

	// Create alloc block
	mpu8_PoolMemory += _i32_Size;
	
	if (!CreatePseudoAllocBlock())
        return NULL;

    if (!AddFreeBlock(pLastFree,(u32)iNewFreeBlockSize,((u8*)pLastFree)+sizeof(MEM_FreeBlock),pLastFree->m_Header.GetSize()-MEM_MIN_BLOCK_SIZE))
        return NULL;

#ifdef _HEAP_DEBUG_
    // Debug management : fill free block (only necessary data)
    if (m_bFillFreeBlocks && (_i32_Size>0))
    {
        if(b_IsNewBlock)
            FillFreeMemory(mpu8_PoolMemory - _i32_Size + 2*sizeof(u32),_i32_Size - 4*sizeof(u32));
        else
            FillFreeMemory(mpu8_PoolMemory - _i32_Size - 2*sizeof(u32),_i32_Size);
    }
#endif //_HEAP_DEBUG_

    return mpu8_PoolMemory;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 MEM_Heap::uSizeToIndex(u32 _uSize)
{
    ITF_ASSERT_CRASH(_uSize >= MEM_MIN_BLOCK_SIZE, "Bad size");
    ITF_ASSERT_CRASH(_uSize < MEM_MAX_BLOCK_SIZE, "Bad size");
    
	if (_uSize < MEM_SMALL_BLOCK_SIZE)
	{
		// 16 -> 0, 20 -> 1, ...
		return (_uSize-MEM_MIN_BLOCK_SIZE)>>2;
	}
	else
	{
		// size is between 2^(i+N) + jP * 2^(i+N)/P and 2^(i+N) + (j+1)P * 2^(i+N)/P 
		// where N is MEM_SMALL_SIZE_POW2 and P is MEM_BUCKET_NB.
	
	    u32 uPowOf2 = MTH_PowerOfTwo(_uSize);
	    u32 i = uPowOf2 - MEM_SMALL_SIZE_POW2;
	    ITF_ASSERT_CRASH(i<MEM_MAX_SIZE_POW2, "Wrong size");
	    
	    u32 uSizeAtThisPowOf2 = 1<<uPowOf2;
	    
	    u32 uDeltaSize = _uSize - uSizeAtThisPowOf2;
	    ITF_ASSERT_CRASH(uDeltaSize < uSizeAtThisPowOf2, "Wrong size");

        u32 uDenominator = uSizeAtThisPowOf2 >> MEM_BUCKET_POW2;	    
	    u32 j = uDeltaSize / uDenominator;  // To avoid int overrun with (uDeltaSize*MEM_BUCKET_NB)/uDenominator

	    ITF_ASSERT_CRASH(j < MEM_BUCKET_NB, "Wrong number");
	    
	    return MEM_SMALL_BLOCK_INDEX + i*MEM_BUCKET_NB + j;		
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 MEM_Heap::uIndexToSize(u32 _uIndex)
{
	if (_uIndex < MEM_SMALL_BLOCK_INDEX)
	{
		// 0 -> 16, 1-> 20 ...
		return (_uIndex<<2)+MEM_MIN_BLOCK_SIZE;
	}
	else
	{
		u32 uIndexAfterSmall = _uIndex-MEM_SMALL_BLOCK_INDEX;
		u32 i = (uIndexAfterSmall >> MEM_BUCKET_POW2); // Better than (uIndexAfterSmall/MEM_BUCKET_NB); 		// Div
		ITF_ASSERT_CRASH(i<MEM_MAX_SIZE_POW2, "Wrong size");
		
		u32 j = uIndexAfterSmall - (i*MEM_BUCKET_NB);	// Mod
		ITF_ASSERT_CRASH(j < MEM_BUCKET_NB, "Wrong number");
		
		u32 uPowOf2 = MEM_SMALL_SIZE_POW2 + i;
		u32 uSizeAtThisPowOf2 = (1<<uPowOf2);
		
		u32 uDeltaSize = j * (uSizeAtThisPowOf2 >> MEM_BUCKET_POW2);
		
		return uSizeAtThisPowOf2 + uDeltaSize;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool MEM_Heap::CreatePseudoAllocBlock()
{
	MEM_HeaderAlloc *pHeader = (MEM_HeaderAlloc *)(mpu8_PoolMemory - sizeof (MEM_HeaderAlloc));
    if (!bCommit(pHeader,4))
    {
        return bfalse;
    }
    else
    {
        pHeader->m_bIsFree = 0;
        pHeader->m_bIsBeforeFree = 1;
        pHeader->SetSize(sizeof (MEM_HeaderAlloc));
        pHeader->m_uWastedSize = sizeof (MEM_HeaderAlloc);
        return btrue;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bbool MEM_Heap::b_IsPointerValid(void *_pAddress)
{
	if (b_IsPointerInHeap(_pAddress))
	{
		if (!(((u32)_pAddress) & 3))
		{
			if (_pAddress < mpu8_PoolMemory)
			{
				MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pAddress);
				
				return (!pHeader->m_bIsFree) && bAreGuardsValid(_pAddress,pHeader->GetSize() - 3 * sizeof(u32));
			}
			else
			{
				void *pShiftedAddress = pGuardShiftPoolAddress(_pAddress);
				u32 uPool = uGetPoolIndex((u8 *)pShiftedAddress);
				MEM_Pool *pPool = pGetPool(uPool);
				
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
u32 MEM_Heap::u_GetBiggestAllocPossible()
{
 	u32 uIndex = 0xFFFFFFFF;
 	i32 IntIndex;
 	for (IntIndex=MEM_FREE_FLAG_NB-1; IntIndex>=0; IntIndex--)
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
 		
	uIndex = Min(uIndex, (u32)(MEM_FREE_BLOCKS_NB-1));
 	
	MEM_FreeBlock *pFreeBlockInList = m_apFreeBlocks[uIndex];
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
	
	u32 uHeaderSize = sizeof(MEM_HeaderAlloc);
	AddGuardSize(&uHeaderSize);
	return uBiggestSize - uHeaderSize;
}


#ifdef _HEAP_DEBUG_
void MEM_Heap::FillFreeMemory(void *_pFillAddress,u32 _uFillSize)
{
	ITF_MemSet(_pFillAddress, M_MEM_FILL_FREE, _uFillSize);
}

void MEM_Heap::CheckFillFree(void *_p)
{
    ITF_ASSERT_CRASH(M_MEM_FILL_FREE == *(u32 *)_p, "Data error detected");
}

void MEM_Heap::CheckDecommitted(void *_pBegin,u32 _uSize)
{
}
#endif //_HEAP_DEBUG_

} // namespace ITF
