#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_heap_WII.h"

namespace ITF
{

#ifdef _HEAP_DEBUG_
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::CheckAllocatedPointers()
{
    ITF_ASSERT_CRASH(m_uTotalSize == m_uAllocSize + m_uAllocPoolSize + m_uFreeSize + m_uFreePoolSize, "Memory size problem detected");
	ITF_ASSERT_CRASH(m_uTotalSize + sizeof(MEM_HeaderAlloc) == (mpu8_EndMemory - mpu8_BegMemory), "Memory heap problem detected");
    ITF_ASSERT_CRASH(m_uPoolNb * sizeof(MEM_Pool) + mpu8_PoolMemory == mpu8_EndMemory, "Memory heap problem detected");
    u32 uLastFreeSize = u_GetLastFreeSize();
    ITF_ASSERT_CRASH(m_uFreeInHolesSize + uLastFreeSize == m_uFreeSize, "Memory size problem detected");

    if (m_bCheckAllBlocks)
    {
        CheckAllBlocks();
    }

    if (m_bCheckFreeBlocks)
    {
        CheckFreeBlocks();
    }

    if (m_bCheckPools)
    {
        CheckPools();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::CheckFreeBlocks()
{
    u32 uFreeBlockNb = 0;
    u32 uTotalFree = 0;

    u32 uIndex = 0;
    for (uIndex=0; uIndex<MEM_FREE_BLOCKS_NB; uIndex++)
    {
        MEM_FreeBlock *pFreeBlock = m_apFreeBlocks[uIndex];
        
	    u32 uIntIndex = uIndex>>5; 					
	    u32 uBitIndex = uIndex - (uIntIndex<<5);
	    u32 uBitInInt = 31 - uBitIndex;

	    u32 uFlagWithBit = 1<<uBitInInt;
	    u32 uIntFlag = m_auFreeFlags[uIntIndex];        
                
        ITF_ASSERT_CRASH((pFreeBlock == NULL) == !(uIntFlag & uFlagWithBit), "Wrong flag");

		u32 uMinSize = uIndexToSize(uIndex);
		u32 uMaxSize = (uIndex == MEM_FREE_BLOCKS_NB-1 ? 0xFFFFFFFF : uIndexToSize(uIndex+1)-4);
        ITF_ASSERT_CRASH(uMinSize <= uMaxSize, "Wrong size");
        while (pFreeBlock)
        {
            uFreeBlockNb++;
            u32 uSize = pFreeBlock->m_Header.GetSize();
            uTotalFree += uSize;
            
			// Is it really a free block ?
	        ITF_ASSERT_CRASH(pFreeBlock->m_Header.m_bIsFree, "Wrong header");
            
            // Is the size of the block correct ?
            ITF_ASSERT_CRASH(!(uSize & 3), "Wrong size");
            ITF_ASSERT_CRASH(uSize >= uMinSize, "Wrong size");
            ITF_ASSERT_CRASH(uSize <= uMaxSize, "Wrong size");
    
            // Is the address at end of free block correct ?
            ITF_ASSERT_CRASH((MEM_HeaderFree *)pFreeBlock == *(MEM_HeaderFree **)(((u8 *)pFreeBlock) + pFreeBlock->m_Header.GetSize() - sizeof(MEM_HeaderFree **)), "Wrong header");
            
            if (uSize > MEM_MIN_BLOCK_SIZE)
            {
                if (m_bFillFreeBlocks)
                {
                    // Check beginning of free buffer
                    void *pFirstFree = (void *)(((u8 *)pFreeBlock) + sizeof(MEM_FreeBlock));
                    CheckFillFree(pFirstFree);

                    // Check end of free buffer
                    void *pLastFree = (void *)(((u8 *)pFreeBlock) + uSize - (sizeof(MEM_FreeBlock*)+ sizeof(u32)));
                    CheckFillFree(pLastFree);
                }

                // Check that inside pages of free blocks are decommitted
                CheckDecommitted(((u8 *)pFreeBlock) + sizeof(MEM_FreeBlock),uSize-MEM_MIN_BLOCK_SIZE);
            }

            pFreeBlock = pFreeBlock->m_pNext;
        }
    }
    ITF_ASSERT_CRASH(uFreeBlockNb == m_uFreeNb, "Wrong block number");
    ITF_ASSERT_CRASH(uTotalFree == m_uFreeSize, "Wrong free size");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::CheckPools()
{
    u32 uEmptyPoolNb = 0;
    u32 uFreeNbPerSize[MEM_BLOCK_SIZE_NB];
    u32 uAllocNbPerSize[MEM_BLOCK_SIZE_NB];

    ITF_MemSet(uFreeNbPerSize, 0, sizeof(uFreeNbPerSize));
    ITF_MemSet(uAllocNbPerSize, 0, sizeof(uAllocNbPerSize));

    u32 uFreeBuffer[MEM_BLOCK_SIZE_NB];
    ITF_MemSet(uFreeBuffer, M_MEM_FILL_FREE, sizeof(uFreeBuffer));

	// Parse all pools
    u32 uPoolIndex;
    for (uPoolIndex = 0; uPoolIndex<m_uPoolNb; uPoolIndex ++)
    {
        MEM_Pool *pPool = pGetPool(uPoolIndex);
        
        if (pPool->b_IsPoolEmpty())
        {
            uEmptyPoolNb++;
        }
        else
        {
            u32 uIndexInPool;
            u32 uBlockNb = pPool->GetBlockNb();
            u32 uSizeIndex = pPool->uGetSizeIndex();
            for (uIndexInPool = 0; uIndexInPool < uBlockNb; uIndexInPool++)
            {
                bbool bIsFree = pPool->b_IsBlockFree(uIndexInPool);
                if (bIsFree)
                {
                    uFreeNbPerSize[uSizeIndex]++;

                    if (m_bFillFreeBlocks)
                    {
                        ITF_ASSERT_CRASH(0 == memcmp(pPool->p_GetBlockAddress(uIndexInPool),uFreeBuffer,pPool->uGetBlockSize()), "Wrong block data");
                    }
                }
                else
                {
                    uAllocNbPerSize[uSizeIndex]++;
                    if (m_bAddGuards)
                    {
                    	void *pUserAddress = ((u32 *)pPool->p_GetBlockAddress(uIndexInPool))+1;
                    	
                   		ITF_ASSERT_CRASH(*pGetBeginGuard(pUserAddress) == M_MEM_BEGIN_GUARD, "Wrong block start");
			    		ITF_ASSERT_CRASH(*pGetEndGuard(pUserAddress,pPool->uGetBlockSize() - 2 * sizeof(u32)) == M_MEM_END_GUARD, "Wrong block end");
                    }
                }
            }
        }
    }

	// Parse partially filled pools
    u32 uSizeIndex;
    u32 uAllocPool = 0;
    u32 uPoolNb = 0;

    for (uSizeIndex=0; uSizeIndex<MEM_BLOCK_SIZE_NB; uSizeIndex++)
    {
        u32 uAllocPerSize = uAllocNbPerSize[uSizeIndex] * ((uSizeIndex+1)<<2);
        uAllocPool += uAllocPerSize;

        ITF_ASSERT_CRASH(m_uAllocSizePerSize[uSizeIndex] == uAllocPerSize, "Wrong size");

        u32 uPoolIndex = m_AvailablePools[uSizeIndex];
        while (uPoolIndex != MEM_POOL_INVALID)
        {
            uPoolNb ++;
            MEM_Pool *pPool = pGetPool(uPoolIndex);
            uPoolIndex = pPool->m_Footer.m_uNext;
        }
    }

	// Parse full pools
    uPoolIndex = m_FullPools;
    while (uPoolIndex != MEM_POOL_INVALID)
    {
        uPoolNb ++;
        MEM_Pool *pPool = pGetPool(uPoolIndex);
        uPoolIndex = pPool->m_Footer.m_uNext;
    }

	// Parse empty pools
    uPoolIndex = m_EmptyPools;
    u32 uEmptyPoolNbInList = 0;
    while (uPoolIndex != MEM_POOL_INVALID)
    {
        uPoolNb ++;
        uEmptyPoolNbInList++;
		MEM_Pool *pPool = pGetPool(uPoolIndex);
        uPoolIndex = pPool->m_Footer.m_uNext;
    }

	// Check that all numbers are coherent. 
    ITF_ASSERT_CRASH(uEmptyPoolNb == uEmptyPoolNbInList, "Wrong number");
    ITF_ASSERT_CRASH(m_uPoolNb == uPoolNb, "Wrong number");
    ITF_ASSERT_CRASH(m_uAllocPoolSize == uAllocPool, "Wrong size");
    ITF_ASSERT_CRASH(m_uFreePoolSize+m_uAllocPoolSize == m_uPoolNb * MEM_POOL_SIZE, "Wrong size");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::CheckAllBlocks()
{
    u32 uFree = 0;
    u32 uAlloc = 0;
    u32 uFreeNb = 0;
    u32 uAllocNb = 0;

	bbool bBlockBeforeIsFree = bfalse;
	
    // Parse all pointers
    MEM_Header *pHeader = (MEM_Header *)mpu8_BegMemory;

    while (((u8 *)pHeader) < mpu8_PoolMemory - sizeof(MEM_HeaderAlloc))
    {
        u32 uSize;
        if (pHeader->m_bIsFree)
        {
            uFreeNb++;
            uSize = ((MEM_HeaderFree *)pHeader)->GetSize();
            uFree += uSize;

            // Is the address at end of free block correct ?
            ITF_ASSERT_CRASH((MEM_HeaderFree *)pHeader == *(MEM_HeaderFree **)(((u8 *)pHeader) + uSize - sizeof(MEM_HeaderFree **)), "Wrong header");
            bBlockBeforeIsFree = btrue;
        }
        else
        {
            uAllocNb++;
            uSize = ((MEM_HeaderAlloc *)pHeader)->GetSize();
            uAlloc += uSize;
            
            if (m_bAddGuards)
            {
                ITF_ASSERT_CRASH((*(u32 *)(((u8 *)pHeader) + sizeof(MEM_HeaderAlloc))) == M_MEM_BEGIN_GUARD, "Wrong block start");
                ITF_ASSERT_CRASH((*(u32 *)(((u8 *)pHeader) + uSize - sizeof(u32))) == M_MEM_END_GUARD, "Wrong block end");
            }
            
            // Check bit "IsBlockBeforeFree"
            bbool bHeaderIsBlockBeforeFree = ((MEM_HeaderAlloc *)pHeader)->m_bIsBeforeFree ? btrue : bfalse;
            ITF_ASSERT_CRASH(bHeaderIsBlockBeforeFree == bBlockBeforeIsFree, "Wrong header");
            
            bBlockBeforeIsFree = bfalse;
        }
        ITF_ASSERT_CRASH(!(uSize & 3), "Wrong size");
        pHeader = (MEM_Header *)(((u8 *)pHeader) + uSize);
    }
    ITF_ASSERT_CRASH(uFree == m_uFreeSize, "Wrong size");
    ITF_ASSERT_CRASH(uAlloc == m_uAllocSize, "Wrong size");
    ITF_ASSERT_CRASH(uFreeNb == m_uFreeNb, "Wrong number");
    ITF_ASSERT_CRASH(uAllocNb == m_uAllocNb, "Wrong number");
}

#endif //_HEAP_DEBUG_

} // namespace ITF
