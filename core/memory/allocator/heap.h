#pragma once
#ifndef _ITF_HEAP_H_
#define _ITF_HEAP_H_

#ifdef ENABLE_ALLOCATOR
#ifndef _ITF_ALLOCATOR_CONSTS_H_
#include "core/memory/allocator/allocator_consts.h"
#endif //_ITF_ALLOCATOR_CONSTS_H_

#ifndef _ITF_POOL_H_
#include "core/memory/allocator/pool.h"
#endif //_ITF_POOL_H_

//#include "THreads/THRead_sync.h"

namespace ITF
{

// Beginning of a memory block (alloc or free).
// Only 1 bit is common to both header types, to know which header type we have.
struct Header
{
	u32 m_bIsFree:1;         // Is the current block free ?
	u32 m_uDummy:31;         // Should not be used (type of header is unknown)
};

// Header of an allocated block
struct HeaderAlloc
{
	u32 GetSize() const {return m_uSizeDiv4<<2;}
	void SetSize(u32 _uSize) {m_uSizeDiv4 = _uSize>>2;}

	u32 m_bIsFree:1;         // Is the current block free ?
	u32 m_bIsBeforeFree:1;   // Is the block just before in the heap free ?
private:
	u32 m_uSizeDiv4:30;      // Total size of the block (including header, guards, etc.), divided by 4
};

// Header of a temp allocated block
struct HeaderAllocTmp
{
	u32 GetSize() const {return m_uSizeDiv4<<2;}
	void SetSize(u32 _uSize) {m_uSizeDiv4 = _uSize>>2;}

	u32 m_bIsFree:1;         // Is the current block free ?
	u32 m_bIsFreeable:1;     // Can the block be freed ?
private:
	u32 m_uSizeDiv4:30;      // Total size of the block (including header, guards, etc.), divided by 4
};

// Header of a free block
struct HeaderFree
{
	u32 GetSize() const {return m_uSizeDiv4<<2;}
	void SetSize(u32 _uSize) {m_uSizeDiv4 = _uSize>>2;}

	u32 m_bIsFree:1;         // Is the current block free ?
private:
	u32 m_uSizeDiv4:31;      // Total size of the block (including header, guards, etc.), divided by 4
};

// -----------------------------------------------------------------------------------------------------------------------
// Beginning of a free block. 
// The last 4 bytes of a free block is the address of the beginning of the block.
// The minimum size for a free block is 16 bytes.
// -----------------------------------------------------------------------------------------------------------------------
struct FreeBlock
{
    HeaderFree m_Header;
    FreeBlock *m_pPrev;
    FreeBlock *m_pNext;
};

// Manages a memory heap. 
class Heap
{
// -----------------------------------------------------------------------------------------------------------------------
// 	PUBLIC INTERFACE
// -----------------------------------------------------------------------------------------------------------------------
public :
	// Creation/Destruction
    Heap(void);
	~Heap(void);
	
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
	
	// Alloc/free (tmp version)
	void	*p_AllocTmp(u32 _uSize);

	// Validity functions
    bbool b_IsPointerInHeap(void *_pAddress) {return (_pAddress >= mpu8_BegMemory) && (_pAddress <mpu8_EndMemory);}
    bbool b_IsPointerValid(void *_pAddress);
    bbool b_IsPointerInPool(void *_pAddress) {return (_pAddress >= mpu8_PoolMemory) && (_pAddress <mpu8_EndMemory);}
    
    // Size functions
	u32 uGetUserSizeFromUserAddress(void *_p_Address);	
	u32 uGetSizeFromUserAddress(void *_p_Address);	
	u32 u_GetLastFreeSize() { FreeBlock *pLastBlock = pGetLastFreeBlock(); return pLastBlock ? pLastBlock->m_Header.GetSize() : 0;}
	u32 u_GetBiggestAllocPossible();

		
	// Management function
	void ReallocCopy(void *_pOldAddress, void *_pNewAddress);
	
#ifdef ITF_MEM_HEAP_DEBUG_
	void 	CheckAllocatedPointers();

	u32 u_GetAllocSize()		{ return m_uAllocSize + m_uAllocPoolSize + m_uAllocTmpSize;}
	u32 u_GetAllocPoolSize()	{ return m_uAllocPoolSize;}
	u32 u_GetFreeSize()			{ return m_uFreeSize + m_uFreePoolSize;}
	u32 u_GetFreeCommittedSize(){ return u_GetFreeSize() - u_GetUncommittedSize();}
	u32 u_GetTotalSize()		{ return m_uTotalSize;}
	u32 u_GetCommittedSize()	{ return m_uCommittedSize;}
	u32 u_GetUncommittedSize()	{ return m_uTotalSize-m_uCommittedSize;}
	u32 u_GetTmpSize() 			{ return m_uAllocTmpSize;}
	u32 u_GetAllocNb() 			{ return m_uAllocNb + m_uAllocPoolNb + m_uAllocTmpNb;}
	u32 u_GetAllocHeapNb() 		{ return m_uAllocNb;}
	u32 u_GetAllocPoolNb()		{ return m_uAllocPoolNb;}
	u32 u_GetFreeNb()  			{ return m_uFreeNb;}
	u32 u_GetPoolSize()			{ return (u32)(mpu8_EndMemory-mpu8_PoolMemory);}
	
    // Debug behavior.
    static bbool m_bFillFreeBlocks;
    static bbool m_bFillAllocBlocks;
    static bbool m_bAddGuards;	
#endif //ITF_MEM_HEAP_DEBUG_

	void *pGetBegMemory() {return mpu8_BegMemory;}
	void *pGetEndMemory() {return mpu8_EndMemory;}
#ifdef ITF_DEBUG_ALLOC_
	void *pGetTmpMemory() {return mpu8_TmpMemory;}
	void *pGetPoolMemory() {return mpu8_PoolMemory;}
	
	const FreeBlock *pGetFreeBlock(u32 _uIndex) const {ITF_ASSERT(_uIndex<ITF_MEM_FREE_BLOCKS_NB); return m_apFreeBlocks[_uIndex];}
#endif // ITF_DEBUG_ALLOC_

    bbool bIsEmpty() {return (mpu8_TmpMemory == mpu8_EndMemory);}

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
	u8	*mpu8_TmpMemory;	    // Beginning of tmp allocs. The block before that is a pseudo alloc block that ends the "classic" heap 
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
	FreeBlock *pGetBestBlockForSize(FreeBlock *_pBlock, u32 _u32_StopSize);
	FreeBlock *pSearchAndRemoveFreeBlock(u32 _u32_Size);
	FreeBlock *pSearchFreeBlock(u32 _u32_Size);
	void RemoveFreeBlock(FreeBlock *_pFreeBlock);

    // Convert size to index in m_apFreeBlocks.
    u32 uSizeToIndex(u32 _uSize);
    
    // Other way around.
	u32 uIndexToSize(u32 _uIndex);

	// Various internal methods
	u32 uUserToAllocSize(u32 _uUserSize);
	u32 uUserToPoolAllocSize(u32 _uUserSize);
	HeaderAllocTmp *pMoveTmpPointer(i32 _i32_Size);
	void *p_ReallocInPlace(void *_pUserAddress, u32 _u32_Size);
	bbool CreatePseudoAllocBlock();

	// Private accessors
    FreeBlock *pGetBlockBefore(void *_pAddress) {return *(((FreeBlock **)_pAddress)-1);}
    Header *pGetBlockAfterAllocBlock(HeaderAlloc *_pHeader) {return (Header *)(((u8 *)_pHeader) + _pHeader->GetSize());}
    Header *pGetBlockAfterFreeBlock(HeaderFree *_pHeader)   {return (Header *)(((u8 *)_pHeader) + _pHeader->GetSize());}
    HeaderFree *pGetFreeBlockBeforeAllocBlock(HeaderAlloc *_pHeader) {ITF_ASSERT(_pHeader->m_bIsBeforeFree); return *(HeaderFree **)(((u8 *)_pHeader) - sizeof(HeaderFree **));}
    inline FreeBlock *pGetLastFreeBlock();
    
private:
	// Lists of free blocks, stored by size for fast removal/insertion.
    FreeBlock *m_apFreeBlocks[ITF_MEM_FREE_BLOCKS_NB];

    // This bit field contains which of the pointers in m_apFreeBlocks are NULL, for fast free block search.
    u32 m_auFreeFlags[ITF_MEM_FREE_FLAG_NB];

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
    MEM_tdPoolIndex m_AvailablePools[ITF_MEM_BLOCK_SIZE_NB];   
	
public:
	// Access.
	MEM_tdPoolIndex getNbPool() const { return m_uPoolNb; }
    inline Pool *pGetPool(u32 _uPoolIndex);
    u32 uGetPoolIndex(void *_pAddress) {u32 uIndex = (mpu8_EndMemory - (((u8 *)_pAddress) + 1))/sizeof(Pool); ITF_ASSERT(uIndex < m_uPoolNb); return uIndex;}

/*
 -----------------------------------------------------------------------------------------------------------------------
 	GUARDS - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	// [OA] used in memory report, done in DES_LynController.cpp, Extern, DebugServer
#ifdef ITF_MEM_HEAP_DEBUG_
	static void *pGetRealAddressFromUserAddress(void *_pUserAddress) {return (HeaderAlloc *)_pUserAddress - (m_bAddGuards ? 2 : 1);}
	static void *pGetUserAddressFromRealAddress(void *_pRealAddress) {return (HeaderAlloc *)_pRealAddress + (m_bAddGuards ? 2 : 1);}
	static void *pGetUserAddressFromPoolRealAddress(void *_pRealAddress) {return (u32 *)_pRealAddress + (m_bAddGuards ? 1 : 0);}
#else //ITF_MEM_HEAP_DEBUG_
	static void *pGetRealAddressFromUserAddress(void *_pUserAddress) {return ((HeaderAlloc *)_pUserAddress) - 1;}
	static void *pGetUserAddressFromRealAddress(void *_pRealAddress) {return ((HeaderAlloc *)_pRealAddress) + 1;}
	static void *pGetUserAddressFromPoolRealAddress(void *_pRealAddress) {return _pRealAddress;}
#endif // ITF_MEM_HEAP_DEBUG_

private:
#ifdef ITF_MEM_HEAP_DEBUG_
    static HeaderAlloc *pGetHeaderFromUserAddress(void *_pUserAddress) {return (HeaderAlloc *)_pUserAddress - (m_bAddGuards ? 2 : 1);}
    
	static inline void AddGuardSize(u32 *_pSize);
	static void CheckGuards(void *_pUserAddress);
	static inline bbool bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser);
		
	static u32 *pGetBeginGuard(void *_pUserAddress) {return (u32 *)_pUserAddress - 1;}
	static u32 *pGetEndGuard(void *_pUserAddress, u32 _uSizeUser) 	{return (u32 *)(((u8 *)_pUserAddress) + _uSizeUser);}

	static void *pGuardShiftPoolAddress(void *_pUserAddress) {return (m_bAddGuards ? ((u32 *)_pUserAddress) - 1 : _pUserAddress);}

#else //ITF_MEM_HEAP_DEBUG_
    static HeaderAlloc *pGetHeaderFromUserAddress(void *_pUserAddress) {return ((HeaderAlloc *)_pUserAddress) - 1;}
	static void AddGuardSize(u32 *) {}
    static void CheckGuards(void *) {}
	static bbool bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser) {return btrue;}
	static void *pGuardShiftPoolAddress(void *_pUserAddress) {return _pUserAddress;}
#endif // ITF_MEM_HEAP_DEBUG_


 /*
 -----------------------------------------------------------------------------------------------------------------------
 	VIRTUAL ADDRESSING - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */
private:

#ifdef ITF_WII
    // No virtual addressing on Wii
    bbool bCommit(void *, ux ) {return btrue;}
    void Decommit(void *, ux ) {}
#else // ITF_WII

    // FilteredDecommit reduces the span of addresses that we try to decommit,
    // because we know that the "no decommit" addresses are already decommitted.
    void FilteredDecommit(u8 *_pDecommitBegin, ux _uDecommitSize, u8 *_pNoDecommitBegin, ux _uNoDecommitSize);
    bbool bCommit(void *_pBegin, ux _uSize);
    void Decommit(void *_pBegin, ux _uSize);

    u32 m_PageSize;
#ifdef ITF_X360
    // On Xbox 360, the heap can be allocated with virtual or physical addresses
    bbool m_bVirtual;     
#elif defined ITF_PS3
    // On PS3, we must remember which physical pages are used (-1 : not attributed)
    sys_memory_t  m_aMemoryID[ITF_MEM_PS3MaxPageNb]; 
#endif //ITF_PS3

#ifdef ITF_MEM_HEAP_DEBUG_
    // Keep track of the pages that are commit in order to fill 
    // the freshly committed pages with ITF_MEM_FILL_FREE.
    void AfterCommit(void *_pBegin, ux _uSize);
    void AfterDecommit(void *_pBegin, ux _uSize);

    bbool bIsAddressCommitted(void *_pAddress);
    bbool bIsPageCommitted(u32 _page);
    void SetPageCommitted(u32 _page);
    void SetPageDecommitted(u32 _page);

    u32 m_PageCommitted[ITF_MEM_MaxPageNb>>5];
#else //ITF_MEM_HEAP_DEBUG_
	void AfterCommit(void *, ux ) {}
	void AfterDecommit(void *, ux ) {}
#endif //ITF_MEM_HEAP_DEBUG_

#endif // ITF_WII

#ifdef ITF_MEM_HEAP_DEBUG_
    // Fill memory with magic number, except for pages that are not committed to memory
    void    FillFreeMemory(void *_pFillAddress,u32 _uFillSize); 
    // Check that the address is filled with magic number
    void    CheckFillFree(void *_p); 
    // Check that inside the range of addresses, the memory is decommitted
    void CheckDecommitted(void *_pBegin,u32 _uSize);
#endif //ITF_MEM_HEAP_DEBUG_

/*
 -----------------------------------------------------------------------------------------------------------------------
 	MEASURES / DEBUG - internal management
 -----------------------------------------------------------------------------------------------------------------------
 */

private:

	void vCheckAligned(void *_pUserAddress, u32 _u32_Align) {ITF_ASSERT(!(((u32)_pUserAddress) & (_u32_Align-1)));}

#ifdef ITF_MEM_HEAP_DEBUG_
    // Measures.

    // Free memory
    u32 m_uFreeSize;            // Number of free bytes (not in pools)
    u32 m_uFreeInHolesSize;     // Number of free bytes not in last block
    u32 m_uFreePoolSize;        // Number of free bytes in pool
    u32 m_uFreeNb;           	// Number of free blocks (not in pools)
    u32 m_uFreeableTmpNb;    	// Number of tmp blocks that have been marked as freeable but cannot really be freed because another unfreed tmp block is before them. 

    // Allocated memory (really used)
    u32 m_uAllocSize;            // Number of allocated bytes (not in pools). Includes wasted bytes.
    u32 m_uAllocPoolSize;        // Number of allocated bytes in pools
    u32 m_uAllocTmpSize;         // Number of allocated bytes in tmp zone (includes freeable tmp blocks)
    u32 m_uAllocNb; 	         // Number of alloc blocks (neither in pools nor tmp).
    u32 m_uAllocPoolNb;        	 // Number of allocated blocks in pools
    u32 m_uAllocTmpNb;  	     // Number of tmp alloc blocks

    u32 m_uFreeSizePerSize[ITF_MEM_BLOCK_SIZE_NB];  // Free memory in pools, per alloc size. 
    u32 m_uAllocSizePerSize[ITF_MEM_BLOCK_SIZE_NB]; // Allocated memory in pools, per alloc size
    u32 m_uTotalSizePerSize[ITF_MEM_BLOCK_SIZE_NB]; // Total memory in pools, per alloc size

    u32 m_uTotalSize;            // Total heap size (m_uAllocSize + m_uAllocPoolSize + m_uAllocTmpSize + m_uFreeSize + m_uFreePoolSize)	

	u32 m_uCommittedSize;        // Size in pages that are committed. Equals to m_uTotalSize for Wii. 

	void CheckFreeBlocks();
	void CheckPools();
	void CheckAllBlocks();
	void CheckTmp();

    static bbool m_bCheckFreeBlocks;
    static bbool m_bCheckPools;
    static bbool m_bCheckAllBlocks;
    static bbool m_bCheckTmp;
#endif //ITF_MEM_HEAP_DEBUG_
};

inline FreeBlock *Heap::pGetLastFreeBlock() 
{
	HeaderAlloc *pHeader = (HeaderAlloc *)(mpu8_TmpMemory - sizeof(HeaderAlloc));
	return (pHeader->m_bIsBeforeFree ? pGetBlockBefore(pHeader) : NULL);
} 
 
inline Pool *Heap::pGetPool(u32 _uPoolIndex) 
{
	ITF_ASSERT((ITF_MEM_POOL_INVALID == _uPoolIndex) || (_uPoolIndex < m_uPoolNb)); 
	return (ITF_MEM_POOL_INVALID == _uPoolIndex ? 
			NULL : 
			(Pool *)(mpu8_EndMemory - ((1+_uPoolIndex)*sizeof(Pool))));
}

#ifdef ITF_MEM_HEAP_DEBUG_

inline void Heap::AddGuardSize(u32 *_pSize) 
{
	if (m_bAddGuards) 
		*_pSize += 2 * sizeof(u32);
}
    
#ifdef _FINAL_
inline void Heap::CheckGuards(void *_pUserAddress)
{
}
#endif //_FINAL_

inline bbool Heap::bAreGuardsValid(void *_pUserAddress, u32 _uSizeUser)
{
	return (!m_bAddGuards) || 
	   ((*pGetBeginGuard(_pUserAddress) == ITF_MEM_BEGIN_GUARD) &&
		(*pGetEndGuard(_pUserAddress,_uSizeUser) == ITF_MEM_END_GUARD  ));
}

#if !defined (ITF_WII) && !defined (ITF_CTR)

inline bbool Heap::bIsPageCommitted(u32 _page) 
{
    ux index = _page>>5; 
    ux indexInBitfield = _page - (index<<5);
    return m_PageCommitted[index] & (1<<indexInBitfield);
}

inline void Heap::SetPageCommitted(u32 _page) 
{
    ux index = _page>>5; 
    ux indexInBitfield = _page - (index<<5);
    m_PageCommitted[index] |= (1<<indexInBitfield);
}

inline void Heap::SetPageDecommitted(u32 _page) 
{
    ux index = _page>>5; 
    ux indexInBitfield = _page - (index<<5);
    m_PageCommitted[index] &= ~(1<<indexInBitfield);
}

inline bbool Heap::bIsAddressCommitted(void *_pAddress)
{
    return bIsPageCommitted((((ux)(((u8 *)_pAddress) - mpu8_BegMemory)) & ~(m_PageSize-1))/m_PageSize);
}

#endif //!defined (ITF_WII) && !defined (ITF_CTR)
#endif // ITF_MEM_HEAP_DEBUG_

} // namespace ITF

#endif //ENABLE_ALLOCATOR
#endif //_ITF_HEAP_H_