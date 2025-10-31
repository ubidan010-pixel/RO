#ifndef __MEM_STRUCTS_WII_H__
#define __MEM_STRUCTS_WII_H__

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include "core/memory/WII/MEM_consts_WII.h"

namespace ITF
{

/*
 -----------------------------------------------------------------------------------------------------------------------
 Beginning of a memory block (alloc or free).
 Only 1 bit is common to both header types, to know which header type we have.
 -----------------------------------------------------------------------------------------------------------------------
 */
struct MEM_Header
{
    u32 m_bIsFree:1;         // Is the current block free ?
    u32 m_uDummy:31;         // Should not be used (type of header is unknown)
};

struct MEM_HeaderAlloc
{
    u32 GetSize() const {return m_uSizeDiv4<<2;}
    void SetSize(u32 _uSize) {m_uSizeDiv4 = _uSize>>2;}

    u32 m_bIsFree:1;         // Is the current block free ?
    u32 m_bIsBeforeFree:1;   // Is the block just before in the heap free ?
    u32 m_uWastedSize:5;     // Difference between size asked by user and size really used.
private:
    u32 m_uSizeDiv4:(MEM_MAX_ALLOC_SIZE_POW2-2);      // Total size of the block (including header, guards, etc.), divided by 4
};

struct MEM_HeaderFree
{
    u32 GetSize() const {return m_uSizeDiv4<<2;}
    void SetSize(u32 _uSize) {m_uSizeDiv4 = _uSize>>2;}
    
    u32 m_bIsFree:1;         // Is the current block free ?
private:
    u32 m_uSizeDiv4:31;      // Total size of the block (including header, guards, etc.), divided by 4
};

/*
 -----------------------------------------------------------------------------------------------------------------------
 Beginning of a free block. 
 The last 4 bytes of a free block is the address of the beginning of the block.
 The minimum size for a free block is 16 bytes.
 -----------------------------------------------------------------------------------------------------------------------
 */
struct MEM_FreeBlock
{
    MEM_HeaderFree m_Header;
    MEM_FreeBlock *m_pPrev;
    MEM_FreeBlock *m_pNext;
};

/*
 -----------------------------------------------------------------------------------------------------------------------
 A pool contains small blocks, 4, 8, 12, 16, 20 or 24 ... 72 bytes long.
 It's size is always 720 bytes long, so that it can contain 180*4, 90*8, 60*12, 45*16, 36*20, 30*24, 15*48, 10*72 bytes blocks.
 The fixed size make it possible to change the size of the block size when it's empty.
 The other data (flags,prev,next,size) use 16 bytes -> the pool is 736 bytes long, which respects alignement.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef u32 MEM_tdPoolIndex;

#define MEM_SIZE_INDEX_BIT_NB 5
#define MEM_BLOCK_SIZE_MAX 128 // Blocks can be 4 to 128 bytes long. Should not be bigger than 4 * (2^MEM_SIZE_INDEX_BIT_NB)
#define MEM_BLOCK_SIZE_NB  (MEM_BLOCK_SIZE_MAX >> 2)   
#define MEM_POOL_SIZE     4096 // Aligned for cache and for alloc aligned in pools
#define MEM_POOL_INVALID    (0xFFFFFFFF>>MEM_SIZE_INDEX_BIT_NB) // Used for invalid "prev" and "next"

// Block number in the pool (computed so that it fits in M_MEM_POOL_SIZE minus the footer and the flag buffer)
const u32 MEM_BLOCK_NB[MEM_BLOCK_SIZE_NB] = {991, 503, 337, 253, 203, 169, 145, 127, 113, 101, 92, 84, 78, 72, 67, 63, 60, 56, 53, 51, 48, 46, 44, 42, 40, 39, 37, 36, 35, 34, 32, 31};

// Number of flag fields. A flag field contains up to 32 flags (bits).
const u32 MEM_FLAG_FIELD_NB[MEM_BLOCK_SIZE_NB] = {31, 16, 11, 8, 7, 6, 5, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};

struct MEM_PoolFooter
{
	//---------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	//---------------------------------------------------------------------------------------------------
public:

	// Index of previous pool in list
	u32 m_uPrev:(32-MEM_SIZE_INDEX_BIT_NB);

	u32 m_uDummy:MEM_SIZE_INDEX_BIT_NB; 

	// Index of next pool in list        
	u32 m_uNext:(32-MEM_SIZE_INDEX_BIT_NB);

	// Index corresponding to the size of a block (0->4, 1->8, i->4*(i+1), ..., 31->128). Need at least 5 bits because the max block size is 128.
	u32 m_uSizeIndex:MEM_SIZE_INDEX_BIT_NB; 
};

struct MEM_Pool
{
	//---------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	//---------------------------------------------------------------------------------------------------
public:
    // m_auBuffer contains 2 parts :
	// -> blocks (allocated data), which are first in order to respect memory alignment
	// -> flags (1 bit per block that tells if the block is free or allocated).
	// The respective size of both parts depends on the block size, so that we fit as many blocks in the block part as possible. 
    u8  m_auBuffer[MEM_POOL_SIZE-sizeof(MEM_PoolFooter)];
    
	// The footer is at the end of the pool, and contains size + pointers for a double linked chain.
    MEM_PoolFooter m_Footer;

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

	// Gets the index of the first free block (-1 if the pool is full)
    u32   u_GetFirstFreeIndex();

    bbool b_IsPoolEmpty();
	bbool b_IsPoolFull();
   
	// Gets the address of the block 
    void *p_GetBlockAddress(u32 _uIndex) 	{ return m_auBuffer + _uIndex * uGetBlockSize();}
    
	// Gets the index of the block
    u32 uGetIndexFromAddress(u8 *_pAdd) 	{ u32 uIndex = (_pAdd - m_auBuffer)/uGetBlockSize(); ITF_ASSERT_CRASH(uIndex * uGetBlockSize() < uGetBlockBufferSize(), "Wrong Index"); return uIndex;}
    
	// Gets the size of a block
    u32 uGetBlockSize() 					{ return uSizeIndexToSize(m_Footer.m_uSizeIndex);}

	// Gets the index that corresponds to the block size
    u32 uGetSizeIndex()						{ return m_Footer.m_uSizeIndex;}
   
	// Functions to transform index to size and size to index. 
    static u32 uSizeToSizeIndex(u32 _uSize) 		{ return (_uSize - 4)>>2;}
    static u32 uSizeIndexToSize(u32 _uSizeIndex) 	{ return (_uSizeIndex<<2) + 4;}
        
};

struct MEM_FlagStack
{
	u32 m_Stack[MEM_DefaultFlagStackSize];
	u32 m_Index;
	u32 m_Flag;
	bbool m_bIsUsed;
};

} // namespace ITF

#endif // __MEM_STRUCTS_WII_H__
