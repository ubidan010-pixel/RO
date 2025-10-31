#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR
//#include "BASe/BAS_extern.h"
//#include "STandard/STD_mem.h"

#ifndef _ITF_HEAP_H_
#include "core/memory/allocator/heap.h"
#endif //_ITF_HEAP_H_


#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_

namespace ITF
{

#ifdef ITF_MEM_HEAP_DEBUG_
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::CheckAllocatedPointers()
{
    ITF_ASSERT(m_uTotalSize == m_uAllocSize + m_uAllocPoolSize + m_uAllocTmpSize + m_uFreeSize + m_uFreePoolSize);
	ITF_ASSERT(m_uTotalSize + sizeof(HeaderAlloc) == (u32)(mpu8_EndMemory - mpu8_BegMemory));
    ITF_ASSERT(mpu8_TmpMemory+m_uAllocTmpSize == mpu8_PoolMemory);
    ITF_ASSERT(m_uPoolNb * sizeof(Pool) + mpu8_PoolMemory == mpu8_EndMemory);
    u32 uLastFreeSize = u_GetLastFreeSize();
    ITF_ASSERT(m_uFreeInHolesSize + uLastFreeSize == m_uFreeSize);

    if (m_bCheckTmp)
    {
        CheckTmp();
    }

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

    // Check that commit info is correct.
/*#ifdef ITF_MEM_HEAP_DEBUG_
#ifdef ITF_X360
    if (m_bVirtual)
    {
        u8 *pAddress = mpu8_BegMemory;
        while (pAddress < mpu8_EndMemory)
        {
            ux page = (pAddress - mpu8_BegMemory)/m_PageSize;
            bbool bMemCommitted = (bIsPageCommitted(page) != bfalse);

            MEMORY_BASIC_INFORMATION memInfo;
            VirtualQuery(pAddress,&memInfo,sizeof(MEMORY_BASIC_INFORMATION));
            ITF_ASSERT(memInfo.State != MEM_FREE);
            bbool bSysCommitted = (memInfo.State == MEM_COMMIT);

            ITF_ASSERT(bMemCommitted == bSysCommitted);
            pAddress += m_PageSize;
        }
    }
#endif //ITF_X360
#endif //ITF_MEM_HEAP_DEBUG_*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::CheckFreeBlocks()
{
    u32 uFreeBlockNb = 0;
    u32 uTotalFree = 0;

    u32 uIndex = 0;
    for (uIndex=0; uIndex<ITF_MEM_FREE_BLOCKS_NB; uIndex++)
    {
        FreeBlock *pFreeBlock = m_apFreeBlocks[uIndex];
        
	    u32 uIntIndex = uIndex>>5; 					
	    u32 uBitIndex = uIndex - (uIntIndex<<5);
	    u32 uBitInInt = 31 - uBitIndex;

	    u32 uFlagWithBit = 1<<uBitInInt;
	    u32 uIntFlag = m_auFreeFlags[uIntIndex];        
                
        ITF_ASSERT( (pFreeBlock == NULL) == !(uIntFlag & uFlagWithBit));

		u32 uMinSize = uIndexToSize(uIndex);
		u32 uMaxSize = (uIndex == ITF_MEM_FREE_BLOCKS_NB-1 ? 0xFFFFFFFF : uIndexToSize(uIndex+1)-4);
        ITF_ASSERT(uMinSize <= uMaxSize);
        while (pFreeBlock)
        {
            uFreeBlockNb++;
            u32 uSize = pFreeBlock->m_Header.GetSize();
            uTotalFree += uSize;
            
			// Is it really a free block ?
	        ITF_ASSERT(pFreeBlock->m_Header.m_bIsFree);
            
            // Is the size of the block correct ?
            ITF_ASSERT(!(uSize & 3));
            ITF_ASSERT(uSize >= uMinSize);
            ITF_ASSERT(uSize <= uMaxSize);
    
            // Is the address at end of free block correct ?
            ITF_ASSERT((HeaderFree *)pFreeBlock == *(HeaderFree **)(((u8 *)pFreeBlock) + pFreeBlock->m_Header.GetSize() - sizeof(HeaderFree **)));
            
#ifdef ITF_MEM_HEAP_DEBUG_
            if (uSize > ITF_MEM_MIN_BLOCK_SIZE)
            {
                if (m_bFillFreeBlocks)
                {
                    // Check beginning of free buffer
                    void *pFirstFree = (void *)(((u8 *)pFreeBlock) + sizeof(FreeBlock));
                    CheckFillFree(pFirstFree);

                    // Check end of free buffer
                    void *pLastFree = (void *)(((u8 *)pFreeBlock) + uSize - (sizeof(FreeBlock*)+ sizeof(u32)));
                    CheckFillFree(pLastFree);
                }

                // Check that inside pages of free blocks are decommitted
                CheckDecommitted(((u8 *)pFreeBlock) + sizeof(FreeBlock),uSize-ITF_MEM_MIN_BLOCK_SIZE);
            }
#endif //ITF_MEM_HEAP_DEBUG_

            pFreeBlock = pFreeBlock->m_pNext;
        }
    }
    ITF_ASSERT(uFreeBlockNb == m_uFreeNb);
    ITF_ASSERT(uTotalFree == m_uFreeSize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::CheckPools()
{
    u32 uEmptyPoolNb = 0;
    u32 uFreeNbPerSize[ITF_MEM_BLOCK_SIZE_NB];
    u32 uAllocNbPerSize[ITF_MEM_BLOCK_SIZE_NB];

    ITF_MemSet(uFreeNbPerSize,0,sizeof(uFreeNbPerSize));
    ITF_MemSet(uAllocNbPerSize,0,sizeof(uAllocNbPerSize));

    u32 uFreeBuffer[ITF_MEM_BLOCK_SIZE_NB];
    ITF_MemSet(uFreeBuffer,ITF_MEM_FILL_FREE,sizeof(uFreeBuffer));

	// Parse all pools
    u32 uPoolIndex;
    for (uPoolIndex = 0; uPoolIndex<m_uPoolNb; uPoolIndex ++)
    {
        Pool *pPool = pGetPool(uPoolIndex);
        
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
                        ITF_ASSERT(0 == memcmp(pPool->p_GetBlockAddress(uIndexInPool),uFreeBuffer,pPool->uGetBlockSize()));
                    }
                }
                else
                {
                    uAllocNbPerSize[uSizeIndex]++;
                    if (m_bAddGuards)
                    {
                    	void *pUserAddress = ((u32 *)pPool->p_GetBlockAddress(uIndexInPool))+1;
                    	
                   		ALLOCATOR()->AssertMemPointer (*pGetBeginGuard(pUserAddress) == ITF_MEM_BEGIN_GUARD,pUserAddress,"begin guard overwritten");
			    		ALLOCATOR()->AssertMemPointer (*pGetEndGuard(pUserAddress,pPool->uGetBlockSize() - 2 * sizeof(u32)) == ITF_MEM_END_GUARD  ,pUserAddress,"end guard overwritten");
                    }
                }
            }
        }
    }

	// Parse partially filled pools
    u32 uSizeIndex;
    u32 uAllocPool = 0;
    u32 uPoolNb = 0;

    for (uSizeIndex=0; uSizeIndex<ITF_MEM_BLOCK_SIZE_NB; uSizeIndex++)
    {
        u32 uAllocPerSize = uAllocNbPerSize[uSizeIndex] * ((uSizeIndex+1)<<2);
        uAllocPool += uAllocPerSize;

        ITF_ASSERT(m_uAllocSizePerSize[uSizeIndex] == uAllocPerSize);

        u32 uPoolIndex = m_AvailablePools[uSizeIndex];
        while (uPoolIndex != ITF_MEM_POOL_INVALID)
        {
            uPoolNb ++;
            Pool *pPool = pGetPool(uPoolIndex);
            uPoolIndex = pPool->m_Footer.m_uNext;
        }
    }

	// Parse full pools
    uPoolIndex = m_FullPools;
    while (uPoolIndex != ITF_MEM_POOL_INVALID)
    {
        uPoolNb ++;
        Pool *pPool = pGetPool(uPoolIndex);
        uPoolIndex = pPool->m_Footer.m_uNext;
    }

	// Parse empty pools
    uPoolIndex = m_EmptyPools;
    u32 uEmptyPoolNbInList = 0;
    while (uPoolIndex != ITF_MEM_POOL_INVALID)
    {
        uPoolNb ++;
        uEmptyPoolNbInList++;
		Pool *pPool = pGetPool(uPoolIndex);
        uPoolIndex = pPool->m_Footer.m_uNext;
    }

	// Check that all numbers are coherent. 
    ITF_ASSERT(uEmptyPoolNb == uEmptyPoolNbInList);
    ITF_ASSERT(m_uPoolNb == uPoolNb);
    ITF_ASSERT(m_uAllocPoolSize == uAllocPool);
    ITF_ASSERT(m_uFreePoolSize+m_uAllocPoolSize == m_uPoolNb * ITF_MEM_POOL_SIZE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::CheckAllBlocks()
{
    u32 uFree = 0;
    u32 uAlloc = 0;
    u32 uFreeNb = 0;
    u32 uAllocNb = 0;

	bbool bBlockBeforeIsFree = bfalse;
	
    // Parse all pointers
    Header *pHeader = (Header *)mpu8_BegMemory;

    while (((u8 *)pHeader) < mpu8_TmpMemory - sizeof(HeaderAlloc))
    {
        u32 uSize;
        if (pHeader->m_bIsFree)
        {
            uFreeNb++;
            uSize = ((HeaderFree *)pHeader)->GetSize();
            uFree += uSize;

            // Is the address at end of free block correct ?
            ITF_ASSERT((HeaderFree *)pHeader == *(HeaderFree **)(((u8 *)pHeader) + uSize - sizeof(HeaderFree **)));
            bBlockBeforeIsFree = btrue;
        }
        else
        {
            uAllocNb++;
            uSize = ((HeaderAlloc *)pHeader)->GetSize();
            uAlloc += uSize;
            
            if (m_bAddGuards)
            {
				void *pUserAddress = pGetUserAddressFromRealAddress(pHeader);
                ALLOCATOR()->AssertMemPointer (((*(u32 *)(((u8 *)pHeader) + sizeof(HeaderAlloc))) == ITF_MEM_BEGIN_GUARD), pUserAddress, "begin guard overwritten");
                ALLOCATOR()->AssertMemPointer (((*(u32 *)(((u8 *)pHeader) + uSize - sizeof(u32))) == ITF_MEM_END_GUARD), pUserAddress, "end guard overwritten");
            }
            
            // Check bit "IsBlockBeforeFree"
            bbool bHeaderIsBlockBeforeFree = ((HeaderAlloc *)pHeader)->m_bIsBeforeFree ? btrue : bfalse;
            ITF_ASSERT(bHeaderIsBlockBeforeFree == bBlockBeforeIsFree);
            
            bBlockBeforeIsFree = bfalse;
        }
        ITF_ASSERT(!(uSize & 3));
        pHeader = (Header *)(((u8 *)pHeader) + uSize);
    }
    ITF_ASSERT(uFree == m_uFreeSize);
    ITF_ASSERT(uAlloc == m_uAllocSize);
    ITF_ASSERT(uFreeNb == m_uFreeNb);
    ITF_ASSERT(uAllocNb == m_uAllocNb);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::CheckTmp()
{
    HeaderAllocTmp *pHeader = (HeaderAllocTmp *)mpu8_TmpMemory;

    u32 uAllocTmp = 0;
    u32 uAllocTmpNb = 0;

    while (((u8 *)pHeader) < mpu8_PoolMemory)
    {
        ITF_ASSERT(!pHeader->m_bIsFree);

        u32 uSize = pHeader->GetSize();
        ITF_ASSERT(!(uSize&3));
        uAllocTmp += uSize;
        uAllocTmpNb ++;

        pHeader = (HeaderAllocTmp *)(((u8 *)pHeader) + uSize);
    }
    ITF_ASSERT(uAllocTmp == m_uAllocTmpSize);
    ITF_ASSERT(uAllocTmpNb == m_uAllocTmpNb);
    ITF_ASSERT(m_uAllocTmpSize == (u32)(mpu8_PoolMemory - mpu8_TmpMemory));
}

#ifndef _FINAL_
void Heap::CheckGuards(void *_pUserAddress)
{
	if (m_bAddGuards)
	{
		HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);

		ALLOCATOR()->AssertMemPointer ((*pGetBeginGuard(_pUserAddress) == ITF_MEM_BEGIN_GUARD), _pUserAddress, "begin guard overwritten");
		ALLOCATOR()->AssertMemPointer ((*pGetEndGuard(_pUserAddress,pHeader->GetSize() - 3 * sizeof(u32)) == ITF_MEM_END_GUARD  ), _pUserAddress, "end guard overwritten");
	}
}
#endif //_FINAL_

#endif //ITF_MEM_HEAP_DEBUG_

}//namespace ITF
#endif //ENABLE_ALLOCATOR
