#include "precompiled_core.h"

#include "memory/WII/MEM_consts_WII.h"
#include "memory/WII/MEM_heap_WII.h"
#include "memory/WII/MEM_macros_WII.h"

namespace ITF
{

#define ROUND_UP_4(a) (4 + ((a)-1) & 0xFFFFFFFC ) // Return smallest multiple of 4 equal or bigger than a.

//#define MEM_NO_POOLS
//#define MEM_NO_REALLOC_IN_PLACE

u32 MEM_Heap::uUserToAllocSize(u32 _uUserSize)
{
	// Alloc must be aligned on 4.
    u32 uAllocSize = ROUND_UP_4(_uUserSize);
    
	// Add header size
    uAllocSize += sizeof(MEM_HeaderAlloc);
    
    // Add guards (if necessary)
	AddGuardSize(&uAllocSize);
	
    // Min alloc : MEM_MIN_BLOCK_SIZE. 
    uAllocSize = (uAllocSize < MEM_MIN_BLOCK_SIZE ? MEM_MIN_BLOCK_SIZE : uAllocSize);
	
	return uAllocSize;
}

u32 MEM_Heap::uUserToPoolAllocSize(u32 _uUserSize)
{
	// Alloc must be aligned on 4.
    u32 uAllocSize = ROUND_UP_4(_uUserSize);
        
    // Add guards (if necessary)
	AddGuardSize(&uAllocSize);
		
	return uAllocSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_Alloc(u32 _u32_Size)
{
	if(_u32_Size == 0) 
        return NULL;

	if (_u32_Size >= MEM_MAX_ALLOC_SIZE)
		return NULL;

    u32 uAllocSize = uUserToPoolAllocSize(_u32_Size);

    // Try to put small alloc in pools
#ifndef MEM_NO_POOLS
    if (uAllocSize <= MEM_BLOCK_SIZE_MAX)
    {
        void *pAddress = p_AllocInPools(uAllocSize);
        
        if (pAddress)
        {
            return pAddress;
        }
    }
#endif //MEM_NO_POOLS

    MEM_CHECK(MEM_CHECK_BEFORE_ALLOC);

	uAllocSize = uUserToAllocSize(_u32_Size);
	
    // Search free block of size big enough for alloc, and remove it for tables.
    MEM_FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uAllocSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();
    ITF_ASSERT_CRASH(uFreeSize>=uAllocSize, "Bad block size");
    u32 uLeftoverSize = uFreeSize - uAllocSize;

    // If leftover size is big enough, create a smaller free block.
    if (uLeftoverSize >=  MEM_MIN_BLOCK_SIZE)
    {
        AddFreeBlock((((u8 *)pFreeBlock)+uAllocSize),uLeftoverSize,MEM_NO_DECOMMIT,0);
    }
    else
    {
        // Set the size of the alloc block to the size of the free block.
        uAllocSize = uFreeSize;
        uLeftoverSize = 0;
    }

    // Create the alloc block, and return the user address.
    void *pUserAddress = pAddAllocBlock(pFreeBlock,uAllocSize,_u32_Size, bfalse, bfalse);

    if (pUserAddress)
        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);

    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::Free(void *_pUserAddress)
{
	// Free of NULL is allowed by ANSI.
	if(!_pUserAddress) 
		return;

    ITF_ASSERT_CRASH(b_IsPointerValid(_pUserAddress), "Pointer already freed ?");

	MEM_CHECK(MEM_CHECK_BEFORE_FREE);
    
    // The function used for free depends of the address.
	if (_pUserAddress >= mpu8_PoolMemory)
		FreeInPools(_pUserAddress);
	else
		FreeInBlocks(_pUserAddress);

    MEM_CHECK(MEM_CHECK_AFTER_FREE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::FreeInBlocks(void *_pUserAddress)
{
    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);

    // Debug management : check guards
    CheckGuards(_pUserAddress);
    
    // Remove alloc block.
    RemoveAllocBlock(pHeader);

    // The new free block is a merge of the alloc block that is being freed
    // and of its neighbors if they are free blocks.
    void *pMergedFreeBlock = pHeader;
    u32 uMergedFreeSize = pHeader->GetSize();

    // Is block before free ?
    if (pHeader->m_bIsBeforeFree)
    {
        // The last 4 bytes of a free block contain the address of the beginning of the free block.
        MEM_FreeBlock *pBlockBefore = pGetBlockBefore(pHeader);
        RemoveFreeBlock(pBlockBefore);

        pMergedFreeBlock = pBlockBefore;
        uMergedFreeSize += pBlockBefore->m_Header.GetSize();
    }

    // Is block after free ?
    MEM_HeaderFree *pBlockAfter = (MEM_HeaderFree *)pGetBlockAfterAllocBlock(pHeader);
    void *pNoDecommit;
    u32 uNoDecommitSize;
    if (pBlockAfter->m_bIsFree)
    {
        pNoDecommit = ((u8*)pBlockAfter) + sizeof(MEM_FreeBlock);
        uNoDecommitSize = pBlockAfter->GetSize()-MEM_MIN_BLOCK_SIZE;

        RemoveFreeBlock((MEM_FreeBlock *)pBlockAfter);

        uMergedFreeSize += pBlockAfter->GetSize();
    }
    else
    {
        pNoDecommit = NULL;
        uNoDecommitSize = 0;
    }

#ifdef _HEAP_DEBUG_
    // Debug management : fill free block (only necessary data)
    if (m_bFillFreeBlocks)
    {
        void *pFillAddress = pHeader;
        u32 uFillSize = pHeader->GetSize();

        if (pHeader->m_bIsBeforeFree)
        {
            pFillAddress = ((u8 *)pHeader) - sizeof(MEM_FreeBlock **);
        }
        else 
        {
            pFillAddress = ((u8 *)pHeader) + sizeof(MEM_FreeBlock);
            uFillSize -= (sizeof(MEM_FreeBlock) + sizeof(MEM_FreeBlock **));
        }

        if (pBlockAfter->m_bIsFree)
        {
            uFillSize += sizeof(MEM_FreeBlock) + sizeof(MEM_FreeBlock **);
        }

        FillFreeMemory(pFillAddress,uFillSize);
    }
#endif //_HEAP_DEBUG_

    // Add merged free block to tables.
    AddFreeBlock(pMergedFreeBlock,uMergedFreeSize,pNoDecommit,uNoDecommitSize);
}

void *MEM_Heap::p_ReallocInPlace(void *_pUserAddress, u32 _u32_Size)
{	
	ITF_ASSERT_CRASH(_pUserAddress && _u32_Size, "Bad realloc call");
	u32 uAllocSize = uUserToAllocSize(_u32_Size);
	
    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
    if (pHeader->GetSize() >= uAllocSize)
    {
        // If we can't create a new free block at end of block, just return the same address.
        if (pHeader->GetSize() < uAllocSize + MEM_MIN_BLOCK_SIZE)
        {
        	return _pUserAddress;
        }
     
#ifdef MEM_NO_REALLOC_IN_PLACE
		return NULL;
#endif //MEM_NO_REALLOC_IN_PLACE

       // Realloc in place.
        MEM_CHECK(MEM_CHECK_BEFORE_ALLOC);

        // Debug management : check guards
        CheckGuards(_pUserAddress);

        // Remove alloc block.
        RemoveAllocBlock(pHeader);

        // New free block address
        MEM_HeaderFree *pNewFreeBlock = (MEM_HeaderFree *)(((u8 *)pHeader) + uAllocSize);
        u32 uUnmergedFreeSize = pHeader->GetSize() - uAllocSize;
		u32 uMergedFreeSize = uUnmergedFreeSize;

        // Is there a free block after the alloc block ?
        MEM_HeaderFree *pBlockAfter = (MEM_HeaderFree *)pGetBlockAfterAllocBlock(pHeader);
        void *pNoDecommit;
        u32 uNoDecommitSize;
        if (pBlockAfter->m_bIsFree)
        {
            pNoDecommit = ((u8*)pBlockAfter) + sizeof(MEM_FreeBlock);
            uNoDecommitSize = pBlockAfter->GetSize()-MEM_MIN_BLOCK_SIZE;
            RemoveFreeBlock((MEM_FreeBlock *)pBlockAfter);

            // Merge free blocks
			uMergedFreeSize += pBlockAfter->GetSize();
        }
        else
        {
            pNoDecommit = NULL;
            uNoDecommitSize = 0;
        }

        // Create free block (before alloc block creation because of update of 'IsBlockBeforeFree' bit)
        AddFreeBlock(pNewFreeBlock,uMergedFreeSize,pNoDecommit,uNoDecommitSize);
        
#ifdef _HEAP_DEBUG_
        bbool bSaveFillAllocBlocks = m_bFillAllocBlocks;
        m_bFillAllocBlocks = bfalse;
#endif //_HEAP_DEBUG_
        
        // Create alloc block
        void *pUserAddress = pAddAllocBlock(pHeader,uAllocSize,_u32_Size,pHeader->m_bIsBeforeFree, bfalse);
#ifdef _HEAP_DEBUG_
        m_bFillAllocBlocks = bSaveFillAllocBlocks;
#endif //_HEAP_DEBUG_

#ifdef _HEAP_DEBUG_
        if (pUserAddress)
        {
	        // Debug management : fill free block.
	        if (m_bFillFreeBlocks)
	        {
				FillFreeMemory(((u8 *)pNewFreeBlock)+sizeof(MEM_FreeBlock),uMergedFreeSize-(sizeof(MEM_FreeBlock)+sizeof(MEM_FreeBlock*)));
	        }
            MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
        }
#endif //_HEAP_DEBUG_

        return pUserAddress;
    }
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_Realloc(void *_pUserAddress, u32 _u32_Size, bbool _bAllowReallocInPlace)
{
    ITF_ASSERT_CRASH(!_pUserAddress || b_IsPointerValid(_pUserAddress), "Bad pointer");

	void *pNewAddress;
    // If address is not in pool area, try realloc in place.
    if (_bAllowReallocInPlace && _pUserAddress && (_pUserAddress < mpu8_PoolMemory) && _u32_Size)
    {
	    pNewAddress = p_ReallocInPlace(_pUserAddress,_u32_Size);
	    if (pNewAddress) 
		{
	        return pNewAddress;
		}
	}

    // Not in place
    pNewAddress = p_Alloc(_u32_Size);

	if (_pUserAddress && pNewAddress)
	{
        ReallocCopy(_pUserAddress,pNewAddress);
		Free(_pUserAddress);
	}
    return pNewAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Heap::ReallocCopy(void *_pOldAddress, void *_pNewAddress)
{
    u32 uOldSize = uGetUserSizeFromUserAddress(_pOldAddress);
    u32 uNewSize = uGetUserSizeFromUserAddress(_pNewAddress);

    u32 uCopySize = Min(uOldSize,uNewSize);

    ITF_Memcpy(_pNewAddress,_pOldAddress,uCopySize);
}


/*
 =======================================================================================================================
 Récupère la taille utilisateur d'une adresse
 =======================================================================================================================
 */
u32 MEM_Heap::uGetUserSizeFromUserAddress(void *_pUserAddress)
{
	u32 uSize;
	u32 uWastedSize;
    if (_pUserAddress < mpu8_PoolMemory)
    {
	    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
	    ITF_ASSERT_CRASH(!pHeader->m_bIsFree, "Wrong header");
	    uSize = pHeader->GetSize();
	    uWastedSize = sizeof(MEM_HeaderAlloc);
    }
    else
    {
    	_pUserAddress = pGuardShiftPoolAddress(_pUserAddress);
    	u32 uPoolIndex = uGetPoolIndex(_pUserAddress);
    	MEM_Pool *pPool = pGetPool(uPoolIndex);
    	uSize = pPool->uGetBlockSize();
	    uWastedSize = 0;
    }
    AddGuardSize(&uWastedSize);
    return uSize-uWastedSize;
}

/*
 =======================================================================================================================
 Récupère la taille complète d'une adresse (y compris les headers et guards).
 =======================================================================================================================
 */
u32 MEM_Heap::uGetSizeFromUserAddress(void *_pUserAddress)
{
	u32 uSize;
    if (_pUserAddress < mpu8_PoolMemory)
    {
	    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
	    ITF_ASSERT_CRASH(!pHeader->m_bIsFree, "Wrong header");
	    uSize = pHeader->GetSize();
    }
    else
    {
    	_pUserAddress = pGuardShiftPoolAddress(_pUserAddress);
    	u32 uPoolIndex = uGetPoolIndex(_pUserAddress);
    	MEM_Pool *pPool = pGetPool(uPoolIndex);
    	uSize = pPool->uGetBlockSize();
    }
    return uSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_AllocAlign(u32 _u32_Size, u32 _u32_Align)
{
	if(_u32_Size == 0) 
        return NULL;

	if (_u32_Align <= 4)
		return p_Alloc(_u32_Size);

	if (_u32_Size >= MEM_MAX_ALLOC_SIZE)
		return NULL;

	u32 uAllocSize = uUserToPoolAllocSize(_u32_Size);

	// Try to put small alloc in pools
#ifndef MEM_NO_POOLS
	if (uAllocSize <= MEM_BLOCK_SIZE_MAX 
#ifdef _HEAP_DEBUG_		
		&& !m_bAddGuards // Guards are incompatible with p_AllocAlignInPools (they break the alignment)
#endif // _HEAP_DEBUG_
		)
	{
		void *pAddress = p_AllocAlignInPools(uAllocSize, _u32_Align);

		if (pAddress)
		{
			return pAddress;
		}
	}
#endif //MEM_NO_POOLS

    MEM_CHECK(MEM_CHECK_BEFORE_ALLOC);

	// uAllocSize is the (minimum) size of the user block (the block that will be returned to the user).
	uAllocSize = uUserToAllocSize(_u32_Size);

    // uNeededSize is the (minimum) size of the free block that is searched for.
	// It is bigger than uAllocSize because :
	// -> we create a new free block before the user block.
	// -> the alignement of the user block moves the address of the user of a value 
	// 		between 0 and (_u32_Align - sizeof(MEM_HeaderAlloc))
    u32 uNeededSize = uAllocSize + _u32_Align + MEM_MIN_BLOCK_SIZE - sizeof(MEM_HeaderAlloc);

    // Search free block of size big enough for alloc, and remove it for tables.
    MEM_FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uNeededSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();

    // Compute aligned alloc user address.
    u32 uUserAlignedAddress = (u32) (((u8 *)pFreeBlock) + _u32_Align + MEM_MIN_BLOCK_SIZE);
#ifdef _HEAP_DEBUG_
    if (m_bAddGuards)
    	uUserAlignedAddress += 4;
#endif //_HEAP_DEBUG_    	
    u32 uAlignMask = ~(_u32_Align-1);

	// Alignement moves the user address of 0 to (_u32_Align - 4).
    ITF_ASSERT_CRASH(uUserAlignedAddress - (uUserAlignedAddress & uAlignMask) <= _u32_Align - sizeof(MEM_HeaderAlloc), "Bad alignment");
    uUserAlignedAddress &= uAlignMask;

    // Align address (minus header)
    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress((void *)uUserAlignedAddress);

    // Compute size of free block before alloc block.
    u32 uSizeBeforeAlloc = ((u32)pHeader) - ((u32)pFreeBlock);

	// Create free block after alloc block ? 
	ITF_ASSERT_CRASH(uFreeSize >= (uSizeBeforeAlloc + uAllocSize), "Wrong block size");
    u32 uLeftoverSize = uFreeSize - uSizeBeforeAlloc - uAllocSize;

    if (uLeftoverSize >=  MEM_MIN_BLOCK_SIZE)
    {
        // Leftover size is big enough -> create a smaller free block after alloc.
        u8 *pFreeBlockAfter = ((u8 *)pHeader) + uAllocSize;
        AddFreeBlock(pFreeBlockAfter,uLeftoverSize,MEM_NO_DECOMMIT,0);
    }
    else
    {
        // Leftover size is too small -> increase alloc size so that there is no free block after.
        uAllocSize = uFreeSize - uSizeBeforeAlloc;
    }

    // Create the alloc block, and get the user address.
    void *pUserAddress = pAddAllocBlock(pHeader,uAllocSize,_u32_Size, btrue, bfalse);
    if (pUserAddress)
    {
        ITF_ASSERT_CRASH(uUserAlignedAddress == (u32)pUserAddress, "Allocation error");
        
        // Create free block before alloc block (do it after alloc block has been added, because we need to change its flag).
        AddFreeBlock(pFreeBlock,uSizeBeforeAlloc,MEM_NO_DECOMMIT,0);

        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
        
        vCheckAligned(pUserAddress,_u32_Align);
    }

    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_ReallocAlign(void *_pUserAddress, u32 _u32_Size, u32 _u32_Align, bbool _bAllowReallocInPlace)
{
    ITF_ASSERT_CRASH(!_pUserAddress || b_IsPointerValid(_pUserAddress), "Bad pointer");

	if (_u32_Align <= 4)
		return p_Realloc(_pUserAddress, _u32_Size, _bAllowReallocInPlace);

	void *pNewAddress;
    // If address is not in pool area, try realloc in place.
    if (_bAllowReallocInPlace && _pUserAddress && (_pUserAddress < mpu8_PoolMemory) && _u32_Size)
    {
    	// realloc in place works only if address is already aligned.
    	if (!(((u32)_pUserAddress) & (_u32_Align-1)))
    	{
		    pNewAddress = p_ReallocInPlace(_pUserAddress,_u32_Size);
		    if (pNewAddress) 
		    {
		        vCheckAligned(pNewAddress,_u32_Align);
		        return pNewAddress;
		    }
		}
	}

    // Not in place
    pNewAddress = p_AllocAlign(_u32_Size,_u32_Align);

	if (_pUserAddress && pNewAddress)
	{
        ReallocCopy(_pUserAddress,pNewAddress);
		Free(_pUserAddress);
	}
	vCheckAligned(pNewAddress,_u32_Align);
    return pNewAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_Heap::p_AllocNotAlign(u32 _u32_Size)
{
    // The returned address should be aligned on 4 bytes, but not on 8 bytes (and thus higher values) 
    const u32 uAligned = 4;
    const u32 uNotAligned = 8;

	if(_u32_Size == 0) 
        return NULL;

	if (_u32_Size >= MEM_MAX_ALLOC_SIZE)
		return NULL;

    MEM_CHECK(MEM_CHECK_BEFORE_ALLOC);

	// uAllocSize is the (minimum) size of the user block (the block that will be returned to the user).
	u32 uAllocSize = uUserToAllocSize(_u32_Size);

    // uNeededSize is the (minimum) size of the free block that is searched for.
	// It is bigger than uAllocSize because we create a new free block before the user block, of size MEM_MIN_BLOCK_SIZE or MEM_MIN_BLOCK_SIZE + 4, in order to avoid alignment on 8 bytes.
    u32 uNeededSize = uAllocSize + uNotAligned + MEM_MIN_BLOCK_SIZE - sizeof(MEM_HeaderAlloc);

    // Search free block of size big enough for alloc, and remove it for tables.
    MEM_FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uNeededSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();

    // Compute aligned alloc user address.
    u32 uUserAddressBeforeAlignment = (u32) (((u8 *)pFreeBlock) + uAligned + sizeof(MEM_HeaderAlloc) + MEM_MIN_BLOCK_SIZE);
#ifdef _HEAP_DEBUG_
    if (m_bAddGuards)
    	uUserAddressBeforeAlignment += 4;
#endif //_HEAP_DEBUG_    	
    const u32 uAlignMask = ~(uNotAligned-1);

	// Alignment removes 0 to (uNotAligned - 4) to the address.
    u32 uUserAlignedAddress = uAlignMask & uUserAddressBeforeAlignment;
    ITF_ASSERT_CRASH(uUserAddressBeforeAlignment - uUserAlignedAddress <= uNotAligned - sizeof(MEM_HeaderAlloc), "Bad alignment");

    // uUserAlignedAddress is aligned on 8 bytes.
    // In order to have an address not aligned on 8 bytes, we either add or remove 4 bytes
    u32 uUserNotAlignedAddress = ((uUserAddressBeforeAlignment == uUserAlignedAddress) ? uUserAlignedAddress - 4 : uUserAlignedAddress + 4);

    // Align address (minus header)
    MEM_HeaderAlloc *pHeader = pGetHeaderFromUserAddress((void *)uUserNotAlignedAddress);

    // Compute size of free block before alloc block.
    u32 uSizeBeforeAlloc = ((u32)pHeader) - ((u32)pFreeBlock);

    ITF_ASSERT_CRASH(uSizeBeforeAlloc >= MEM_MIN_BLOCK_SIZE, "Wrong block size");

	// Create free block after alloc block ? 
	ITF_ASSERT_CRASH(uFreeSize >= (uSizeBeforeAlloc + uAllocSize), "Wrong block size");
    u32 uLeftoverSize = uFreeSize - uSizeBeforeAlloc - uAllocSize;

    if (uLeftoverSize >=  MEM_MIN_BLOCK_SIZE)
    {
        // Leftover size is big enough -> create a smaller free block after alloc.
        u8 *pFreeBlockAfter = ((u8 *)pHeader) + uAllocSize;
        AddFreeBlock(pFreeBlockAfter,uLeftoverSize,MEM_NO_DECOMMIT,0);
    }
    else
    {
        // Leftover size is too small -> increase alloc size so that there is no free block after.
        uAllocSize = uFreeSize - uSizeBeforeAlloc;
    }

    // Create the alloc block, and get the user address.
    void *pUserAddress = pAddAllocBlock(pHeader,uAllocSize,_u32_Size, btrue, bfalse);
    if (pUserAddress)
    {
        ITF_ASSERT_CRASH(uUserNotAlignedAddress == (u32)pUserAddress, "Allocation error");
        
        // Create free block before alloc block (do it after alloc block has been added, because we need to change its flag).
        AddFreeBlock(pFreeBlock,uSizeBeforeAlloc,MEM_NO_DECOMMIT,0);

        MEM_CHECK(MEM_CHECK_AFTER_ALLOC);
    }
    
    return pUserAddress;
}

} // namespace ITF
