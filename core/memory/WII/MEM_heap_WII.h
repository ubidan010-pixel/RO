#ifndef __MEM_HEAP_WII_H__
#define __MEM_HEAP_WII_H__

#include "core/memory/WII/MEM_consts_WII.h"
#include "core/memory/WII/MEM_structs_WII.h"

namespace ITF
{

class MEM_Heap
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 	PUBLIC INTERFACE
 -----------------------------------------------------------------------------------------------------------------------
 */

public :
	// Creation/Destruction
    MEM_Heap(void);
	~MEM_Heap(void);
	
	void	Create(u32 _uHeapSize, u8 *_pBeginAddress, bbool _bVirtual = btrue);
	void	Destroy(void);

	// Alloc/realloc/free
	void	*p_Alloc(u32 _uSize);
	void	*p_Realloc(void *_p, u32 _uSize, bbool _bAllowReallocInPlace = bfalse);
	void	Free(void *_p);

	// Alloc/realloc (align version). Use Free to free memory allocated by align.
	void	*p_AllocAlign(u32 _uSize, u32 _uAlign);
	void	*p_ReallocAlign(void *_p, u32 _uSize, u32 _uAlign, bbool _bAllowReallocInPlace = bfalse);

    // Alloc not aligned (aligned on 4 bytes only), used to debug alignment.
    void    *p_AllocNotAlign(u32 _uSize);
	
	// Validity functions
    bbool b_IsPointerInHeap(void *_pAddress) {return (_pAddress >= mpu8_BegMemory) && (_pAddress <mpu8_EndMemory);}
    bbool b_IsPointerValid(void *_pAddress);
    bbool b_IsPointerInPool(void *_pAddress) {return (_pAddress >= mpu8_PoolMemory) && (_pAddress <mpu8_EndMemory);}
    
    
    // Size functions
	u32 uGetUserSizeFromUserAddress(void *_p_Address);	
	u32 uGetSizeFromUserAddress(void *_p_Address);	
	u32 u_GetLastFreeSize();
	u32 u_GetBiggestAllocPossible();

		
	// Management function
	void ReallocCopy(void *_pOldAddress, void *_pNewAddress);
	
#ifdef _HEAP_DEBUG_
	void 	CheckAllocatedPointers();

	u32 u_GetFreeSize()			{ return m_uFreeSize + m_uFreePoolSize;}
	u32 u_GetFreeCommittedSize(){ return u_GetFreeSize() - u_GetUncommittedSize();}
	u32 u_GetAllocNb() 			{ return m_uAllocNb + m_uAllocPoolNb;}
	u32 u_GetAllocPoolNb()		{ return m_uAllocPoolNb;}
	u32 u_GetFreeNb()  			{ return m_uFreeNb;}
	
    // Debug behavior.
    static bbool m_bFillFreeBlocks;
    static bbool m_bFillAllocBlocks;
    static bbool m_bAddGuards;	
#endif //_HEAP_DEBUG_

#ifdef _HEAP_SIZE_DEBUG_
    u32 u_GetTotalSize()		{ return m_uTotalSize;}
    u32 u_GetCommittedSize()	{ return m_uCommittedSize;}
    u32 u_GetUncommittedSize()	{ return m_uTotalSize-m_uCommittedSize;}
    u32 u_GetAllocSize()		{ return m_uAllocSize + m_uAllocPoolSize;}
    u32 u_GetAllocPoolSize()	{ return m_uAllocPoolSize;}

    u32 u_GetPoolSize()			{ return (u32)(mpu8_EndMemory-mpu8_PoolMemory);}
#endif // _HEAP_SIZE_DEBUG_

	void *pGetBegMemory()	{return mpu8_BegMemory;}
	void *pGetEndMemory()	{return mpu8_EndMemory;}
	void *pGetPoolMemory()	{return mpu8_PoolMemory;}
#ifdef _DEBUG_ALLOC_
	
	const MEM_FreeBlock *pGetFreeBlock(u32 _uIndex) const {ITF_ASSERT_CRASH(_uIndex<MEM_FREE_BLOCKS_NB, "Bad Index"); return m_apFreeBlocks[_uIndex];}
#endif // _DEBUG_ALLOC_

    bbool bIsEmpty() {return (u_GetLastFreeSize() + 4) ==  (mpu8_EndMemory-mpu8_BegMemory); }

private :
	void FreeInPools(void *_pUserAddress);
	void FreeInBlocks(void *_pUserAddress);
	void FreeInTmp(void *_pUserAddress);

/*
 -----------------------------------------------------------------------------------------------------------------------
 	HEAP BOUNDARIES - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private :
    // Classic heap : between Beg and Tmp
    // Tmp allocs : between Tmp and Pool
    // Pools : between Pool and End
	u8	*mpu8_BegMemory;	    // First valid address in heap
	u8	*mpu8_PoolMemory;	    // Beginning of pools. 
	u8	*mpu8_EndMemory;    	// First non-valid address in heap 

/*
 -----------------------------------------------------------------------------------------------------------------------
 	CLASSIC ALLOC/FREE - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	// Search, add and remove blocks	
	bbool AddFreeBlock(void *_pAddress, u32 _size, void *_pNoDecommitAddress, u32 _uNoDecommitSize);
	void *pAddAllocBlock(void *_pAddress, u32 _uSize, u32 _uUserSize, bbool _bIsBlockBeforeFree, bbool _bAllocTmp);
	void RemoveAllocBlock(void *_pAddress);
	MEM_FreeBlock *pGetBestBlockForSize(MEM_FreeBlock *_pBlock, u32 _u32_StopSize);
	MEM_FreeBlock *pSearchAndRemoveFreeBlock(u32 _u32_Size);
	MEM_FreeBlock *pSearchFreeBlock(u32 _u32_Size);
	void RemoveFreeBlock(MEM_FreeBlock *_pFreeBlock);

    // Convert size to index in m_apFreeBlocks.
    u32 uSizeToIndex(u32 _uSize);
    
    // Other way around.
	u32 uIndexToSize(u32 _uIndex);

	// Various internal methods
	u32 uUserToAllocSize(u32 _uUserSize);
	u32 uUserToPoolAllocSize(u32 _uUserSize);
	u8 *pMovePoolPointer(i32 _i32_Size);
	void *p_ReallocInPlace(void *_pUserAddress, u32 _u32_Size);
	bbool CreatePseudoAllocBlock();

	// Private accessors
    MEM_FreeBlock *pGetBlockBefore(void *_pAddress) {return *(((MEM_FreeBlock **)_pAddress)-1);}
    MEM_Header *pGetBlockAfterAllocBlock(MEM_HeaderAlloc *_pHeader) {return (MEM_Header *)(((u8 *)_pHeader) + _pHeader->GetSize());}
    MEM_Header *pGetBlockAfterFreeBlock(MEM_HeaderFree *_pHeader)   {return (MEM_Header *)(((u8 *)_pHeader) + _pHeader->GetSize());}
    MEM_HeaderFree *pGetFreeBlockBeforeAllocBlock(MEM_HeaderAlloc *_pHeader) {ITF_ASSERT_CRASH(_pHeader->m_bIsBeforeFree, "Wrong header"); return *(MEM_HeaderFree **)(((u8 *)_pHeader) - sizeof(MEM_HeaderFree **));}
    inline MEM_FreeBlock *pGetLastFreeBlock();
    
private:
	// Lists of free blocks, stored by size for fast removal/insertion.
    MEM_FreeBlock *m_apFreeBlocks[MEM_FREE_BLOCKS_NB];

    // This bit field contains which of the pointers in m_apFreeBlocks are NULL, for fast free block search.
    u32 m_auFreeFlags[MEM_FREE_FLAG_NB];

/*
 -----------------------------------------------------------------------------------------------------------------------
 	POOLS (small allocs) - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void *p_AllocInPools(u32 _u32_Size);
	void *p_AllocAlignInPools(u32 _u32_Size, u32 _u32_Align);
	void MovePool(MEM_tdPoolIndex _uPool, MEM_tdPoolIndex *_puDst, MEM_tdPoolIndex *_puSrc);
		
    // Pools can contain small blocks, and use up less header space than normal alloc
    MEM_tdPoolIndex m_uPoolNb;

    // Empty pools can be instantiated at any size.
    MEM_tdPoolIndex m_EmptyPools;

    // Totally full pools.
    MEM_tdPoolIndex m_FullPools;

    // These pools are neither full or empty. 
    // The size of the blocks of the pools of the list i is 4*i.
    MEM_tdPoolIndex m_AvailablePools[MEM_BLOCK_SIZE_NB];   
	
	// Access.
    inline MEM_Pool *pGetPool(u32 _uPoolIndex);
    u32 uGetPoolIndex(void *_pAddress) {u32 uIndex = (mpu8_EndMemory - (((u8 *)_pAddress) + 1))/sizeof(MEM_Pool); ITF_ASSERT_CRASH(uIndex < m_uPoolNb, "Wrong Index"); return uIndex;}

/*
 -----------------------------------------------------------------------------------------------------------------------
 	GUARDS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	// [OA] used in memory report, done in DES_LynController.cpp, Extern, DebugServer
#ifdef _HEAP_DEBUG_
    static void *pGetRealAddressFromUserAddress(void *_pUserAddress) {return (MEM_HeaderAlloc *)_pUserAddress - (m_bAddGuards ? 2 : 1);}
#else //_HEAP_DEBUG_
    static void *pGetRealAddressFromUserAddress(void *_pUserAddress) {return ((MEM_HeaderAlloc *)_pUserAddress) - 1;}
#endif // _HEAP_DEBUG_

private:
#ifdef _HEAP_DEBUG_
    static MEM_HeaderAlloc *pGetHeaderFromUserAddress(void *_pUserAddress) {return (MEM_HeaderAlloc *)_pUserAddress - (m_bAddGuards ? 2 : 1);}
    
	static inline void AddGuardSize(u32 *_pSize);
	static inline void CheckGuards(void *_pUserAddress);
	static inline bbool bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser);
		
	static u32 *pGetBeginGuard(void *_pUserAddress) {return (u32 *)_pUserAddress - 1;}
	static u32 *pGetEndGuard(void *_pUserAddress, u32 _uSizeUser) 	{return (u32 *)(((u8 *)_pUserAddress) + _uSizeUser);}

	static void *pGuardShiftPoolAddress(void *_pUserAddress) {return (m_bAddGuards ? ((u32 *)_pUserAddress) - 1 : _pUserAddress);}

#else //_HEAP_DEBUG_
    static MEM_HeaderAlloc *pGetHeaderFromUserAddress(void *_pUserAddress) {return ((MEM_HeaderAlloc *)_pUserAddress) - 1;}
	static void AddGuardSize(u32 *) {}
    static void CheckGuards(void *) {}
	static bbool bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser) {return btrue;}
	static void *pGuardShiftPoolAddress(void *_pUserAddress) {return _pUserAddress;}
#endif // _HEAP_DEBUG_


 /*
 -----------------------------------------------------------------------------------------------------------------------
 	VIRTUAL ADDRESSING - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:

    // No virtual addressing on Wii
    bbool bCommit(void *, u32 ) {return btrue;}
    void Decommit(void *, u32 ) {}

#ifdef _HEAP_DEBUG_
    // Fill memory with magic number, except for pages that are not committed to memory
    void    FillFreeMemory(void *_pFillAddress,u32 _uFillSize); 
    // Check that the address is filled with magic number
    void    CheckFillFree(void *_p); 
    // Check that inside the range of addresses, the memory is decommitted
    void CheckDecommitted(void *_pBegin,u32 _uSize);
#endif //_HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 	MEASURES / DEBUG - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */

private:

	void vCheckAligned(void *_pUserAddress, u32 _u32_Align) {ITF_ASSERT_CRASH(!(((u32)_pUserAddress) & (_u32_Align-1)), "Bad alignment");}

#ifdef _HEAP_DEBUG_
    // Measures.

    // Free memory
    u32 m_uFreeSize;            // Number of free bytes (not in pools)
    u32 m_uFreeInHolesSize;     // Number of free bytes not in last block
    u32 m_uFreePoolSize;        // Number of free bytes in pool
    u32 m_uFreeNb;           	// Number of free blocks (not in pools)

    // Allocated memory (really used)
    u32 m_uAllocNb; 	         // Number of alloc blocks (neither in pools nor tmp).
    u32 m_uAllocPoolNb;        	 // Number of allocated blocks in pools

    u32 m_uFreeSizePerSize[MEM_BLOCK_SIZE_NB];  // Free memory in pools, per alloc size. 
    u32 m_uAllocSizePerSize[MEM_BLOCK_SIZE_NB]; // Allocated memory in pools, per alloc size
    u32 m_uTotalSizePerSize[MEM_BLOCK_SIZE_NB]; // Total memory in pools, per alloc size

	void CheckFreeBlocks();
	void CheckPools();
	void CheckAllBlocks();

    static bbool m_bCheckFreeBlocks;
    static bbool m_bCheckPools;
    static bbool m_bCheckAllBlocks;
#endif //_HEAP_DEBUG_
#ifdef _HEAP_SIZE_DEBUG_
    // Measures.
    u32 m_uTotalSize;            // Total heap size (m_uAllocSize + m_uAllocPoolSize + m_uFreeSize + m_uFreePoolSize)	
    u32 m_uCommittedSize;        // Size in pages that are committed. Equals to m_uTotalSize for Wii. 
    u32 m_uAllocSize;            // Number of allocated bytes (not in pools). Includes wasted bytes.
    u32 m_uAllocPoolSize;        // Number of allocated bytes in pools
#endif // _HEAP_SIZE_DEBUG_
};

inline MEM_FreeBlock *MEM_Heap::pGetLastFreeBlock() 
{
	MEM_HeaderAlloc *pHeader = (MEM_HeaderAlloc *)(mpu8_PoolMemory - sizeof(MEM_HeaderAlloc));
	return (pHeader->m_bIsBeforeFree ? pGetBlockBefore(pHeader) : NULL);
} 

inline u32 MEM_Heap::u_GetLastFreeSize()
{ 
	MEM_FreeBlock *pLastBlock = pGetLastFreeBlock(); 
	return pLastBlock ? pLastBlock->m_Header.GetSize() : 0;
}

 
inline MEM_Pool *MEM_Heap::pGetPool(u32 _uPoolIndex) 
{
	ITF_ASSERT_CRASH((MEM_POOL_INVALID == _uPoolIndex) || (_uPoolIndex < m_uPoolNb), "Bad index"); 
	return (MEM_POOL_INVALID == _uPoolIndex ? 
			NULL : 
			(MEM_Pool *)(mpu8_EndMemory - ((1+_uPoolIndex)*sizeof(MEM_Pool))));
}

#ifdef _HEAP_DEBUG_

inline void MEM_Heap::AddGuardSize(u32 *_pSize) 
{
	if (m_bAddGuards) 
		*_pSize += 2 * sizeof(u32);
}
    
inline void MEM_Heap::CheckGuards(void *_pUserAddress)
{
#ifndef ITF_FINAL
    if (m_bAddGuards)
    {
    	MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);

	    ITF_ASSERT_CRASH(*pGetBeginGuard(_pUserAddress) == M_MEM_BEGIN_GUARD, "Can't find begin guard");
	    ITF_ASSERT_CRASH(*pGetEndGuard(_pUserAddress,pHeader->GetSize() - 3 * sizeof(u32)) == M_MEM_END_GUARD, "Can't find end guard");
    }
#endif // ITF_FINAL
}

inline bbool MEM_Heap::bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser)
{
	return (!m_bAddGuards) || 
	   ((*pGetBeginGuard(_pUserAddress) == M_MEM_BEGIN_GUARD) &&
		(*pGetEndGuard(_pUserAddress,_uSizeUser) == M_MEM_END_GUARD  ));
}

#endif // _HEAP_DEBUG_

} // namespace ITF

#endif //__MEM_HEAP_WII_H__