#include "precompiled_core.h"

#ifdef ENABLE_ALLOCATOR
#ifndef _ITF_POOL_H_
#include "core/memory/allocator/pool.h"
#endif //_ITF_POOL_H_

#ifndef _ITF_ALLOCATOR_MACROS_H_
#include "core/memory/allocator/allocator_macros.h"
#endif //_ITF_ALLOCATOR_MACROS_H_



namespace ITF
{

u32 Pool::GetEmptyFlag(u32 _uFlagIndex)	
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

void Pool::SetSize(u32 _uSize)
{
	u32 uSizeIndex = uSizeToSizeIndex(_uSize); 
	m_Footer.m_uSizeIndex = uSizeIndex;
	u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[uSizeIndex];
	u32 *pFlagField = aGetFlagFields();

	// clean buffer
#ifdef ITF_MEM_HEAP_DEBUG_
	if (Heap::m_bFillFreeBlocks)
		ITF_MemSet( m_auBuffer, ITF_MEM_FILL_FREE, sizeof(m_auBuffer));
#endif

	// First flag fields are filled with '1' bits.
	ITF_MemSet(pFlagField,0xFFFFFFFF,4*(uFlagFieldNb-1));

	// Last flag field is special (only the first bits are set to '1'). 
	pFlagField[uFlagFieldNb-1] = GetEmptyFlag(uFlagFieldNb-1);
}




} //	namespace ITF
#endif //ENABLE_ALLOCATOR
