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

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_AllocInPools(u32 _u32_Size)
{
    u32 uPoolSizeIndex = Pool::uSizeToSizeIndex(_u32_Size);
    ITF_ASSERT(uPoolSizeIndex < ITF_MEM_BLOCK_SIZE_NB);

    ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

    MEM_tdPoolIndex uPool = m_AvailablePools[uPoolSizeIndex];

    Pool *pPool = pGetPool(uPool);

	u32 uPath = 0;

    if (!pPool)
    {
		uPath = 1;
        // Is there any empty pool available ? 
        if (m_EmptyPools == ITF_MEM_POOL_INVALID)
        {
			uPath = 2;
            // Try to get a pool from tmp space. 
            if ((m_uPoolNb < ITF_MEM_POOL_INVALID - 1) &&
                (mpu8_TmpMemory == mpu8_PoolMemory))
            {   
				uPath = 3;
                void *pAddress = pMoveTmpPointer(-((i32)sizeof(Pool)));
                if (pAddress)
                {
                    pPool = (Pool *)pAddress;
                    mpu8_PoolMemory = mpu8_TmpMemory;

                    m_uPoolNb++;
                    m_EmptyPools = uGetPoolIndex(mpu8_PoolMemory);

                    // Init pool.
                    pPool->SetSize(_u32_Size);
                    pPool->m_Footer.m_uNext = ITF_MEM_POOL_INVALID;
                    pPool->m_Footer.m_uPrev = ITF_MEM_POOL_INVALID;

#ifdef ITF_MEM_HEAP_DEBUG_
                    // Debug management : fill free blocks.
                    if (m_bFillFreeBlocks)
                    {
                        ITF_MemSet(pPool->m_auBuffer,ITF_MEM_FILL_FREE,pPool->uGetBlockBufferSize());
                    }

                    // Debug management : update measures
                    m_uFreePoolSize += ITF_MEM_POOL_SIZE;
#endif //ITF_MEM_HEAP_DEBUG_
                }
            }

            if (m_EmptyPools == ITF_MEM_POOL_INVALID)
            {
                ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
                return NULL;
            }
        }

        ITF_ASSERT(m_EmptyPools != ITF_MEM_POOL_INVALID);

        MovePool(m_EmptyPools,&m_AvailablePools[uPoolSizeIndex],&m_EmptyPools);
        uPool = m_AvailablePools[uPoolSizeIndex];

        pPool = pGetPool(uPool);
        ITF_ASSERT(pPool);
        pPool->SetSize(_u32_Size);
        
#ifdef ITF_MEM_HEAP_DEBUG_
        m_uFreeSizePerSize[uPoolSizeIndex] += ITF_MEM_POOL_SIZE;
        m_uTotalSizePerSize[uPoolSizeIndex] += ITF_MEM_POOL_SIZE;
#endif //ITF_MEM_HEAP_DEBUG_
    }

    u32 uIndexBlock = pPool->u_GetFirstFreeIndex();
    ITF_ASSERT(uIndexBlock < pPool->GetBlockNb());

    // Free block is now allocated
    pPool->SetBlockAllocated(uIndexBlock);

    // If pool is full, change the list it belongs to.
    if (pPool->b_IsPoolFull())
    {
        MovePool(uPool,&m_FullPools,&m_AvailablePools[uPoolSizeIndex]);
    }

    void *pAddress = pPool->p_GetBlockAddress(uIndexBlock);

#ifdef ITF_MEM_HEAP_DEBUG_
	u32 uBlockSize = pPool->uGetBlockSize();
    if (m_bFillAllocBlocks)
    {
        ITF_MemSet(pAddress,ITF_MEM_FILL_ALLOC,uBlockSize);
    }

    m_uFreeSizePerSize[uPoolSizeIndex] -= uBlockSize;
    m_uAllocSizePerSize[uPoolSizeIndex] += uBlockSize;
    m_uAllocPoolSize += uBlockSize;
    m_uFreePoolSize -= uBlockSize;
    m_uAllocPoolNb++;
    
	if (m_bAddGuards)
	{
		*((u32 *)pAddress) = ITF_MEM_BEGIN_GUARD;
		*((u32 *) (((u8 *)pAddress)+_u32_Size-4)) = ITF_MEM_END_GUARD;
		pAddress = ((u32 *)pAddress) + 1;
	}    
#endif //ITF_MEM_HEAP_DEBUG_


    ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
    return pAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_AllocAlignInPools(u32 _u32_Size, u32 _u32_Align)
{
	// Compute size that :
	// -> is aligned on _u32_Align
	// -> is bigger or equal than _u32_Size
	u32 uAlignedSize = _u32_Align * (1 + ((_u32_Size-1) / _u32_Align));

	if (uAlignedSize <= ITF_MEM_BLOCK_SIZE_MAX)
		return p_AllocInPools(uAlignedSize);
	else 
		return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::FreeInPools(void *_pUserAddress)
{
	void *pRealAddress = pGuardShiftPoolAddress(_pUserAddress);

    u32 uPool = uGetPoolIndex((u8 *)pRealAddress);
    ITF_ASSERT(uPool < m_uPoolNb);

    Pool *pPool = pGetPool(uPool);

    u32 uPoolSizeIndex = pPool->uGetSizeIndex();

#ifdef ITF_MEM_HEAP_DEBUG_
	u32 uBlockSize = pPool->uGetBlockSize();

	if (m_bAddGuards)
	{
		ALLOCATOR()->AssertMemPointer ((*((u32 *)pRealAddress) == ITF_MEM_BEGIN_GUARD), _pUserAddress,"begin guard overwritten");
		ALLOCATOR()->AssertMemPointer ((*((u32 *) (((u8 *)pRealAddress)+uBlockSize-4)) == ITF_MEM_END_GUARD), _pUserAddress, "end guard overwritten");
	}

    if (m_bFillFreeBlocks)
    {
        FillFreeMemory(pRealAddress, uBlockSize);
    }
        
    m_uFreeSizePerSize[uPoolSizeIndex] += uBlockSize;
    m_uAllocSizePerSize[uPoolSizeIndex] -= uBlockSize;        
    m_uAllocPoolSize -= uBlockSize;
    m_uFreePoolSize += uBlockSize;
    m_uAllocPoolNb--;
#endif //ITF_MEM_HEAP_DEBUG_

    // If pool is full, move it to available pools.
    if (pPool->b_IsPoolFull())
    {
        MovePool(uPool,&m_AvailablePools[uPoolSizeIndex],&m_FullPools);
    }

	// Set block as free
    u32 uIndexBlock = pPool->uGetIndexFromAddress((u8 *)pRealAddress);
    pPool->SetBlockFree(uIndexBlock);

    // If pool is now empty, move it to empty pools.
    if (pPool->b_IsPoolEmpty())
    {
        MovePool(uPool,&m_EmptyPools,&m_AvailablePools[uPoolSizeIndex]);

#ifdef ITF_MEM_HEAP_DEBUG_
        // Debug management : update measures
        m_uFreeSizePerSize[uPoolSizeIndex] -= ITF_MEM_POOL_SIZE;
        m_uTotalSizePerSize[uPoolSizeIndex] -= ITF_MEM_POOL_SIZE;
#endif //ITF_MEM_HEAP_DEBUG_
        // Try to reduce the space used by empty pools.
        if (mpu8_PoolMemory == mpu8_TmpMemory)
        {
            Pool *pPool = (Pool *)mpu8_PoolMemory;
            while ((((u8 *)pPool) < mpu8_EndMemory) && pPool->b_IsPoolEmpty())
            {
#ifdef ITF_MEM_HEAP_DEBUG_
                // Debug management : update measures
                m_uFreePoolSize -= ITF_MEM_POOL_SIZE;
#endif //ITF_MEM_HEAP_DEBUG_

                
                MEM_tdPoolIndex uTheBigNowhere = ITF_MEM_POOL_INVALID;
				MovePool(m_uPoolNb-1,&uTheBigNowhere,&m_EmptyPools);

                m_uPoolNb--;
                pPool = (Pool *)pMoveTmpPointer(sizeof(Pool));
            }
            mpu8_PoolMemory = mpu8_TmpMemory;
        }
    }
}

void Heap::MovePool(MEM_tdPoolIndex _uPool, MEM_tdPoolIndex *_puDst, MEM_tdPoolIndex *_puSrc)
{
    Pool *pPool = pGetPool(_uPool);

    // 1) Remove _Pool from _pSrc

    if (*_puSrc == _uPool)
    {
        // _Pool is at beginning of _pSrc
        ITF_ASSERT(pPool->m_Footer.m_uPrev == ITF_MEM_POOL_INVALID);

        *_puSrc = pPool->m_Footer.m_uNext;
        if (pPool->m_Footer.m_uNext != ITF_MEM_POOL_INVALID)
            pGetPool(pPool->m_Footer.m_uNext)->m_Footer.m_uPrev = ITF_MEM_POOL_INVALID;
    }
    else 
    {
        // _Pool has a previous pool
        ITF_ASSERT(pPool->m_Footer.m_uPrev != ITF_MEM_POOL_INVALID);
        pGetPool(pPool->m_Footer.m_uPrev)->m_Footer.m_uNext = pPool->m_Footer.m_uNext;
        if (pPool->m_Footer.m_uNext != ITF_MEM_POOL_INVALID)
            pGetPool(pPool->m_Footer.m_uNext)->m_Footer.m_uPrev = pPool->m_Footer.m_uPrev;
    }

    // 2) Add _Pool to _pDst (at beginning)
    if (*_puDst != ITF_MEM_POOL_INVALID)
    {
        ITF_ASSERT(pGetPool(*_puDst)->m_Footer.m_uPrev == ITF_MEM_POOL_INVALID);
        pGetPool(*_puDst)->m_Footer.m_uPrev = _uPool;
    }

    pPool->m_Footer.m_uNext = *_puDst;
    pPool->m_Footer.m_uPrev = ITF_MEM_POOL_INVALID;
    *_puDst = _uPool;
}


} // namespace ITF

#endif //ENABLE_ALLOCATOR
