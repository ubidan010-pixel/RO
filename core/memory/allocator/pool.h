
#pragma once
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef ITF_CORE_BITTWEAK_H_
#include "core/math/BitTweak.h"
#endif //ITF_CORE_BITTWEAK_H_

#ifndef _ITF_POOL_H_
#define _ITF_POOL_H_

#ifdef ENABLE_ALLOCATOR
namespace ITF
{

/*
 -----------------------------------------------------------------------------------------------------------------------
 A pool buffer contains small blocks of 4, 8, 12, ... 128 bytes long, with a footer at the end of the buffer. 
 The footer has a part (the flag buffer) whose size depends on the size of the blocks, because it uses 1 bit per block (it is bigger for small blocks).
 The size of pool buffers is always 4096 bytes long.
 The fix buffer size make it possible to change the block size when it's empty.
 The fact that the size is a power of two keeps the alignement of the pool buffers with the alignement of the end of the heap.
 And because the pool buffers are aligned, the blocks in it that are a power of two are also aligned with their size (32 bytes blocks are aligned on 32 bytes for example).
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef u32 MEM_tdPoolIndex;

#define ITF_MEM_SIZE_INDEX_BIT_NB	5
#define ITF_MEM_BLOCK_SIZE_MAX		128 // Blocks can be 4 to 128 bytes long. Should not be bigger than 4 * (2^ITF_MEM_SIZE_INDEX_BIT_NB)
#define ITF_MEM_BLOCK_SIZE_NB		(ITF_MEM_BLOCK_SIZE_MAX >> 2)   
#define ITF_MEM_POOL_SIZE			4096 // Aligned for cache and for alloc aligned in pools
#define ITF_MEM_POOL_INVALID		(0xFFFFFFFF>>ITF_MEM_SIZE_INDEX_BIT_NB) // Used for invalid "prev" and "next"

// Block number in the pool (computed so that it fits in M_MEM_POOL_SIZE minus the footer and the flag buffer)
const u32 MEM_BLOCK_NB[ITF_MEM_BLOCK_SIZE_NB] = {991, 503, 337, 253, 203, 169, 145, 127, 113, 101, 92, 84, 78, 72, 67, 63, 60, 56, 53, 51, 48, 46, 44, 42, 40, 39, 37, 36, 35, 34, 32, 31};

// Number of flag fields. A flag field contains up to 32 flags (bits).
const u32 MEM_FLAG_FIELD_NB[ITF_MEM_BLOCK_SIZE_NB] = {31, 16, 11, 8, 7, 6, 5, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};

struct PoolFooter
{
	//---------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	//---------------------------------------------------------------------------------------------------
public:

	// Index of previous pool in list
	u32 m_uPrev:(32-ITF_MEM_SIZE_INDEX_BIT_NB);

	u32 m_uDummy:ITF_MEM_SIZE_INDEX_BIT_NB; 

	// Index of next pool in list        
	u32 m_uNext:(32-ITF_MEM_SIZE_INDEX_BIT_NB);

	// Index corresponding to the size of a block (0->4, 1->8, i->4*(i+1), ..., 31->128). Need at least 5 bits because the max block size is 128.
	u32 m_uSizeIndex:ITF_MEM_SIZE_INDEX_BIT_NB; 
};

class Pool
{
	//---------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	//---------------------------------------------------------------------------------------------------
public:
    // m_auBuffer contains 2 parts :
	// -> blocks (allocated data), which are first in order to respect memory alignment
	// -> flags (1 bit per block that tells if the block is free or allocated).
	// The respective size of both parts depends on the block size, so that we fit as many blocks in the block part as possible. 
    u8  m_auBuffer[ITF_MEM_POOL_SIZE-sizeof(PoolFooter)];
    
	// The footer is at the end of the pool, and contains size + pointers for a double linked chain.
    PoolFooter m_Footer;

	//---------------------------------------------------------------------------------------------------
	// METHODS
	//---------------------------------------------------------------------------------------------------
public:
	// Accessors for the flag bits. 
    bbool b_IsBlockFree(u32 _uIndex) 		{ return (aGetFlagFields()[_uIndex >> 5] & (0x80000000 >> (_uIndex & 0x0000001F)));}    
    void  SetBlockFree(u32 _uIndex) 		{ aGetFlagFields()[_uIndex >> 5] |= (0x80000000 >> (_uIndex & 0x0000001F));}
    void  SetBlockAllocated(u32 _uIndex) 	{ aGetFlagFields()[_uIndex >> 5] &= ~(0x80000000 >> (_uIndex & 0x0000001F));}
    
	// Gets the number of blocks in the pool
    u32   GetBlockNb() 						{ return MEM_BLOCK_NB[m_Footer.m_uSizeIndex];}

	// Address of the flag field array
	u32  *aGetFlagFields()					{ return (u32 *) (((u8 *)this) + uGetBlockBufferSize()) ;}

	// Size of the data buffer
	u32   uGetBlockBufferSize()				{ return sizeof(m_auBuffer)-uGetFlagBufferSize();}

	// Size of the flag buffer
	u32   uGetFlagBufferSize()				{ return MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex] << 2;}

	// Get the flag field that indicates that the flag field is empty. 
	u32   GetEmptyFlag(u32 _uFlagIndex);

	// Sets the size of the pool. 
    void  SetSize(u32 _uSize);

    ITF_FORCE_INLINE bbool b_IsPoolFull()
    {
        const u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex];
        const u32 *pFlagField = aGetFlagFields();
        for(u32 i =0; i<uFlagFieldNb; i++)
        {
            if (pFlagField[i])
                return bfalse;
        }
        return btrue;
    }

    // Gets the index of the first free block (-1 if the pool is full)
    ITF_FORCE_INLINE u32 u_GetFirstFreeIndex()
    {
        u32 uFlagFieldNb = MEM_FLAG_FIELD_NB[m_Footer.m_uSizeIndex];
        u32 *pFlagField = aGetFlagFields();

        for(u32 i =0; i< uFlagFieldNb; i++)
        {
            u32 uFlagField = pFlagField[i];
            if(uFlagField)
            {
                return 32*i + BitTweak::countLeadingZero(uFlagField);
            }
        }
        return 0xFFFFFFFF;
    }

    ITF_FORCE_INLINE bbool Pool::b_IsPoolEmpty()
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

   
	// Gets the address of the block 
    void *p_GetBlockAddress(u32 _uIndex) 	{ return m_auBuffer + _uIndex * uGetBlockSize();}
    
	// Gets the index of the block
    u32 uGetIndexFromAddress(u8 *_pAdd) 	{ u32 uIndex = (_pAdd - m_auBuffer)/uGetBlockSize(); ITF_ASSERT(uIndex * uGetBlockSize() < uGetBlockBufferSize()); return uIndex;}
    
	// Gets the size of a block
    u32 uGetBlockSize() 					{ return uSizeIndexToSize(m_Footer.m_uSizeIndex);}

	// Gets the index that corresponds to the block size
    u32 uGetSizeIndex()						{ return m_Footer.m_uSizeIndex;}
   
	// Functions to transform index to size and size to index. 
    static u32 uSizeToSizeIndex(u32 _uSize) 		{ return (_uSize - 4)>>2;}
    static u32 uSizeIndexToSize(u32 _uSizeIndex) 	{ return (_uSizeIndex<<2) + 4;}
        
};

} // namespace ITF
#endif //ENABLE_ALLOCATOR
#endif //_ITF_POOL_H_