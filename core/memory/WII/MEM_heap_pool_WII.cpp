#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_heap_WII.h"
#include "memory/WII/MEM_macros_WII.h"
#include "math/WII/MTH_int_WII.h"

namespace ITF
{

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_AllocInPools(u32 _u32_Size)
{
	u32 uPoolSizeIndex = MEM_Pool::uSizeToSizeIndex(_u32_Size);
	ITF_ASSERT_CRASH(uPoolSizeIndex < MEM_BLOCK_SIZE_NB, "Wrong size");

	MEM_CHECK(MEM_CHECK_BEFORE_ALLOC);

	MEM_tdPoolIndex uPool = m_AvailablePools[uPoolSizeIndex];

	MEM_Pool *pPool = pGetPool(uPool);

	if (!pPool)
	{
		// Is there any empty pool available ? 
		if (m_EmptyPools == MEM_POOL_INVALID)
		{
			// Try to get a new pool.
			if (m_uPoolNb < MEM_POOL_INVALID - 1)
			{   
				u8 *pAddress = pMovePoolPointer(-((i32)sizeof(MEM_Pool)));
				if (pAddress)
				{
					pPool = (MEM_Pool *)pAddress;
					mpu8_PoolMemory = pAddress;

					m_uPoolNb++;
					m_EmptyPools = uGetPoolIndex(mpu8_PoolMemory);

					// Init pool.
					pPool->SetSize(_u32_Size);
					pPool->m_Footer.m_uNext = MEM_POOL_INVALID;
					pPool->m_Footer.m_uPrev = MEM_POOL_INVALID;

#ifdef _HEAP_DEBUG_
					// Debug management : fill free blocks.
					if (m_bFillFreeBlocks)
					{
						ITF_MemSet(pPool->m_auBuffer, M_MEM_FILL_FREE, pPool->uGetBlockBufferSize());
					}

					// Debug management : update measures
					m_uFreePoolSize += MEM_POOL_SIZE;
#endif //_HEAP_DEBUG_
				}
			}

			if (m_EmptyPools == MEM_POOL_INVALID)
			{
				MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
				return NULL;
			}
		}

		ITF_ASSERT_CRASH(m_EmptyPools != MEM_POOL_INVALID, "Pool error");

		MovePool(m_EmptyPools,&m_AvailablePools[uPoolSizeIndex],&m_EmptyPools);
		uPool = m_AvailablePools[uPoolSizeIndex];

		pPool = pGetPool(uPool);
		ITF_ASSERT_CRASH(pPool, "Pool error");
		pPool->SetSize(_u32_Size);
        
#ifdef _HEAP_DEBUG_
		m_uFreeSizePerSize[uPoolSizeIndex] += MEM_POOL_SIZE;
		m_uTotalSizePerSize[uPoolSizeIndex] += MEM_POOL_SIZE;
#endif //_HEAP_DEBUG_
	}

    u32 uIndexBlock = pPool->u_GetFirstFreeIndex();
    ITF_ASSERT_CRASH(uIndexBlock < pPool->GetBlockNb(), "Wrong number");

    // Free block is now allocated
    pPool->SetBlockAllocated(uIndexBlock);

    // If pool is full, change the list it belongs to.
    if (pPool->b_IsPoolFull())
    {
        MovePool(uPool,&m_FullPools,&m_AvailablePools[uPoolSizeIndex]);
    }

    void *pAddress = pPool->p_GetBlockAddress(uIndexBlock);

#ifdef _HEAP_SIZE_DEBUG_
    u32 uBlockSize = pPool->uGetBlockSize();
    m_uAllocPoolSize += uBlockSize;
#ifdef _HEAP_DEBUG_
    if (m_bFillAllocBlocks)
    {
        ITF_MemSet(pAddress, M_MEM_FILL_ALLOC, uBlockSize);
    }

    m_uFreeSizePerSize[uPoolSizeIndex] -= uBlockSize;
    m_uAllocSizePerSize[uPoolSizeIndex] += uBlockSize;
    m_uFreePoolSize -= uBlockSize;
    m_uAllocPoolNb++;
    
	if (m_bAddGuards)
	{
		*((u32 *)pAddress) = M_MEM_BEGIN_GUARD;
		*((u32 *) (((u8 *)pAddress)+_u32_Size-4)) = M_MEM_END_GUARD;
		pAddress = ((u32 *)pAddress) + 1;
	}    
#endif //_HEAP_DEBUG_
#endif // _HEAP_SIZE_DEBUG_

    MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
    return pAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_AllocAlignInPools(u32 _u32_Size, u32 _u32_Align)
{
	// Compute size that :
	// -> is aligned on _u32_Align
	// -> is bigger or equal than _u32_Size
	u32 uAlignedSize = _u32_Align * (1 + ((_u32_Size-1) / _u32_Align));

	if (uAlignedSize <= MEM_BLOCK_SIZE_MAX)
		return p_AllocInPools(uAlignedSize);
	else 
		return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::FreeInPools(void *_pUserAddress)
{
	_pUserAddress = pGuardShiftPoolAddress(_pUserAddress);

    u32 uPool = uGetPoolIndex((u8 *)_pUserAddress);
    ITF_ASSERT_CRASH(uPool < m_uPoolNb, "Wrong index");

    MEM_Pool *pPool = pGetPool(uPool);

    u32 uPoolSizeIndex = pPool->uGetSizeIndex();

#ifdef _HEAP_SIZE_DEBUG_
    u32 uBlockSize = pPool->uGetBlockSize();
    m_uAllocPoolSize -= uBlockSize;
#ifdef _HEAP_DEBUG_
	if (m_bAddGuards)
	{
		ITF_ASSERT_CRASH(*((u32 *)_pUserAddress) == M_MEM_BEGIN_GUARD, "Begin guard error");
		ITF_ASSERT_CRASH(*((u32 *) (((u8 *)_pUserAddress)+uBlockSize-4)) == M_MEM_END_GUARD, "End guard error");
	}

    if (m_bFillFreeBlocks)
    {
        FillFreeMemory(_pUserAddress, uBlockSize);
    }
        
    m_uFreeSizePerSize[uPoolSizeIndex] += uBlockSize;
    m_uAllocSizePerSize[uPoolSizeIndex] -= uBlockSize;        
    m_uFreePoolSize += uBlockSize;
    m_uAllocPoolNb--;
#endif //_HEAP_DEBUG_
#endif // _HEAP_SIZE_DEBUG_

    // If pool is full, move it to available pools.
    if (pPool->b_IsPoolFull())
    {
        MovePool(uPool,&m_AvailablePools[uPoolSizeIndex],&m_FullPools);
    }

	// Set block as free
    u32 uIndexBlock = pPool->uGetIndexFromAddress((u8 *)_pUserAddress);
    pPool->SetBlockFree(uIndexBlock);

    // If pool is now empty, move it to empty pools.
    if (pPool->b_IsPoolEmpty())
    {
        MovePool(uPool,&m_EmptyPools,&m_AvailablePools[uPoolSizeIndex]);

#ifdef _HEAP_DEBUG_
        // Debug management : update measures
        m_uFreeSizePerSize[uPoolSizeIndex] -= MEM_POOL_SIZE;
        m_uTotalSizePerSize[uPoolSizeIndex] -= MEM_POOL_SIZE;
#endif //_HEAP_DEBUG_

        // Try to reduce the space used by empty pools.
        MEM_Pool *pPool = (MEM_Pool *)mpu8_PoolMemory;
        while ((((u8 *)pPool) < mpu8_EndMemory) && pPool->b_IsPoolEmpty())
        {
#ifdef _HEAP_DEBUG_
            // Debug management : update measures
            m_uFreePoolSize -= MEM_POOL_SIZE;
#endif //_HEAP_DEBUG_

            MEM_tdPoolIndex uTheBigNowhere = MEM_POOL_INVALID;
			MovePool(m_uPoolNb-1,&uTheBigNowhere,&m_EmptyPools);

            m_uPoolNb--;
            mpu8_PoolMemory = pMovePoolPointer(sizeof(MEM_Pool));
			ITF_ASSERT_CRASH(mpu8_PoolMemory, "Move error");

            pPool = (MEM_Pool *)mpu8_PoolMemory;
        }
    }
}

void MEM_Heap::MovePool(MEM_tdPoolIndex _uPool, MEM_tdPoolIndex *_puDst, MEM_tdPoolIndex *_puSrc)
{
    MEM_Pool *pPool = pGetPool(_uPool);

    // 1) Remove _Pool from _pSrc

    if (*_puSrc == _uPool)
    {
        // _Pool is at beginning of _pSrc
        ITF_ASSERT_CRASH(pPool->m_Footer.m_uPrev == MEM_POOL_INVALID, "Wrong block");

        *_puSrc = pPool->m_Footer.m_uNext;
        if (pPool->m_Footer.m_uNext != MEM_POOL_INVALID)
            pGetPool(pPool->m_Footer.m_uNext)->m_Footer.m_uPrev = MEM_POOL_INVALID;
    }
    else 
    {
        // _Pool has a previous pool
        ITF_ASSERT_CRASH(pPool->m_Footer.m_uPrev != MEM_POOL_INVALID, "Wrong block");
        pGetPool(pPool->m_Footer.m_uPrev)->m_Footer.m_uNext = pPool->m_Footer.m_uNext;
        if (pPool->m_Footer.m_uNext != MEM_POOL_INVALID)
            pGetPool(pPool->m_Footer.m_uNext)->m_Footer.m_uPrev = pPool->m_Footer.m_uPrev;
    }

    // 2) Add _Pool to _pDst (at beginning)
    if (*_puDst != MEM_POOL_INVALID)
    {
        ITF_ASSERT_CRASH(pGetPool(*_puDst)->m_Footer.m_uPrev == MEM_POOL_INVALID, "Wrong block");
        pGetPool(*_puDst)->m_Footer.m_uPrev = _uPool;
    }

    pPool->m_Footer.m_uNext = *_puDst;
    pPool->m_Footer.m_uPrev = MEM_POOL_INVALID;
    *_puDst = _uPool;
}

u32 MEM_Pool::GetEmptyFlag(u32 _uFlagIndex)	
{   
	u32 uSizeIndex = m_Footer.m_uSizeIndex;
	u32 uFlagFieldMaxIndex = MEM_FLAG_FIELD_NB[uSizeIndex]-1;
	if (_uFlagIndex < uFlagFieldMaxIndex || !(MEM_BLOCK_NB[uSizeIndex] & 0x1F))
 	{
 		return 0xFFFFFFFF;
 	}
 	else if (_uFlagIndex == uFlagFieldMaxIndex)
 	{
		// Number of blocks in the last flag
		u32 uLastFlagBlockNb = (MEM_BLOCK_NB[uSizeIndex] - ((uFlagFieldMaxIndex)<<5));

		// The last flag field has '1's for free blocks and '0's for the rest, 
		// so that u32_CountLeadingZero gives the index of the first free block.
		return ~(0xFFFFFFFF >> uLastFlagBlockNb);
 	}
 	else
 		return 0;
}
										
void MEM_Pool::SetSize(u32 _uSize)
{
	u32 uSizeIndex = uSizeToSizeIndex(_uSize); 
	m_Footer.m_uSizeIndex = uSizeIndex;
	u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[uSizeIndex];
	u32 *pFlagField = aGetFlagFields();

	// clean buffer
#ifdef _HEAP_DEBUG_
	if (MEM_Heap::m_bFillFreeBlocks)
		ITF_MemSet(m_auBuffer, M_MEM_FILL_FREE, sizeof(m_auBuffer));
#endif

	// First flag fields are filled with '1' bits.
	ITF_MemSet(pFlagField, 0xFFFFFFFF, 4*(uFlagFieldNb-1));

	// Last flag field is special (only the first bits are set to '1'). 
	pFlagField[uFlagFieldNb-1] = GetEmptyFlag(uFlagFieldNb-1);
}

u32 MEM_Pool::u_GetFirstFreeIndex()
{
	u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex];
	u32 *pFlagField = aGetFlagFields();

	for(u32 i =0; i< uFlagFieldNb; i++)
	{
		u32 uFlagField = pFlagField[i];
		if(uFlagField)
		{
			return 32*i + u32_CountLeadingZero(uFlagField);
		}
	}
	return -1;
}

bbool MEM_Pool::b_IsPoolEmpty()
{
	u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex];
	u32 *pFlagField = aGetFlagFields();

	for(u32 i =0; i< uFlagFieldNb-1; i++)
	{
		if (pFlagField[i] != 0xFFFFFFFF)
			return bfalse;
	}
	return pFlagField[uFlagFieldNb-1] == GetEmptyFlag(uFlagFieldNb-1);
}

bbool MEM_Pool::b_IsPoolFull()
{
	u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex];
	u32 *pFlagField = aGetFlagFields();
	for(u32 i =0; i<uFlagFieldNb; i++)
	{
		if (pFlagField[i])
			return bfalse;
	}
	return btrue;
}

} // namespace ITF
