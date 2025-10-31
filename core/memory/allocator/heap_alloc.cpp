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

#define ROUND_UP_4(a) (4 + ((a)-1) & 0xFFFFFFFC ) // Return smallest multiple of 4 equal or bigger than a.

//#define ITF_MEM_NO_POOLS
#define ITF_MEM_NO_TMP
//#define ITF_MEM_NO_REALLOC_IN_PLACE

u32 Heap::uUserToAllocSize(u32 _uUserSize)
{
	// Alloc must be aligned on 4.
    u32 uAllocSize = ROUND_UP_4(_uUserSize);
    
	// Add header size
    uAllocSize += sizeof(HeaderAlloc);
    
    // Add guards (if necessary)
	AddGuardSize(&uAllocSize);
	
    // Min alloc : ITF_MEM_MIN_BLOCK_SIZE. 
    uAllocSize = (uAllocSize < ITF_MEM_MIN_BLOCK_SIZE ? ITF_MEM_MIN_BLOCK_SIZE : uAllocSize);
	
	return uAllocSize;
}

u32 Heap::uUserToPoolAllocSize(u32 _uUserSize)
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
void *Heap::p_Alloc(u32 _u32_Size)
{
	if(_u32_Size == 0) 
        return NULL;

    u32 uAllocSize = uUserToPoolAllocSize(_u32_Size);

    // Try to put small alloc in pools
#ifndef ITF_MEM_NO_POOLS
    if (uAllocSize <= ITF_MEM_BLOCK_SIZE_MAX)
    {
        void *pAddress = p_AllocInPools(uAllocSize);
        
        if (pAddress)
        {
            return pAddress;
        }
    }
#endif //ITF_MEM_NO_POOLS

    ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

	uAllocSize = uUserToAllocSize(_u32_Size);
	
    // Search free block of size big enough for alloc, and remove it for tables.
    FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uAllocSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();
    ITF_ASSERT(uFreeSize>=uAllocSize);
    u32 uLeftoverSize = uFreeSize - uAllocSize;

    // If leftover size is big enough, create a smaller free block.
    if (uLeftoverSize >=  ITF_MEM_MIN_BLOCK_SIZE)
    {
        AddFreeBlock((((u8 *)pFreeBlock)+uAllocSize),uLeftoverSize,ITF_MEM_NO_DECOMMIT,0);
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
	{
        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
	}

    return pUserAddress;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::Free(void *_pUserAddress)
{
	// Free of NULL is allowed by ANSI.
	if(!_pUserAddress) 
		return;

    ITF_ASSERT(b_IsPointerValid(_pUserAddress));

	ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_FREE);
    
    // The function used for free depends of the address.
    if (_pUserAddress >= mpu8_PoolMemory)
        FreeInPools(_pUserAddress);
	else if (_pUserAddress < mpu8_TmpMemory)
        FreeInBlocks(_pUserAddress);
	else
		FreeInTmp(_pUserAddress);

    ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_FREE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::FreeInBlocks(void *_pUserAddress)
{
    HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);

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
        FreeBlock *pBlockBefore = pGetBlockBefore(pHeader);
        RemoveFreeBlock(pBlockBefore);

        pMergedFreeBlock = pBlockBefore;
        uMergedFreeSize += pBlockBefore->m_Header.GetSize();
    }

    // Is block after free ?
    HeaderFree *pBlockAfter = (HeaderFree *)pGetBlockAfterAllocBlock(pHeader);
    void *pNoDecommit;
    u32 uNoDecommitSize;
    if (pBlockAfter->m_bIsFree)
    {
        pNoDecommit = ((u8*)pBlockAfter) + sizeof(FreeBlock);
        uNoDecommitSize = pBlockAfter->GetSize()-ITF_MEM_MIN_BLOCK_SIZE;

        RemoveFreeBlock((FreeBlock *)pBlockAfter);

        uMergedFreeSize += pBlockAfter->GetSize();
    }
    else
    {
        pNoDecommit = NULL;
        uNoDecommitSize = 0;
    }

#ifdef ITF_MEM_HEAP_DEBUG_
    // Debug management : fill free block (only necessary data)
    if (m_bFillFreeBlocks)
    {
        void *pFillAddress = pHeader;
        u32 uFillSize = pHeader->GetSize();

        if (pHeader->m_bIsBeforeFree)
        {
            pFillAddress = ((u8 *)pHeader) - sizeof(FreeBlock **);
        }
        else 
        {
            pFillAddress = ((u8 *)pHeader) + sizeof(FreeBlock);
            uFillSize -= (sizeof(FreeBlock) + sizeof(FreeBlock **));
        }

        if (pBlockAfter->m_bIsFree)
        {
            uFillSize += sizeof(FreeBlock) + sizeof(FreeBlock **);
        }

        FillFreeMemory(pFillAddress,uFillSize);
    }
#endif //ITF_MEM_HEAP_DEBUG_

    // Add merged free block to tables.
    AddFreeBlock(pMergedFreeBlock,uMergedFreeSize,pNoDecommit,uNoDecommitSize);
}

void *Heap::p_ReallocInPlace(void *_pUserAddress, u32 _u32_Size)
{	
	ITF_ASSERT(_pUserAddress && _u32_Size);
	u32 uAllocSize = uUserToAllocSize(_u32_Size);
	
    HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
    if (pHeader->GetSize() >= uAllocSize)
    {
        // If we can't create a new free block at end of block, just return the same address.
        if (pHeader->GetSize() < uAllocSize + ITF_MEM_MIN_BLOCK_SIZE)
        {
        	return _pUserAddress;
        }
     
#ifdef ITF_MEM_NO_REALLOC_IN_PLACE
		return NULL;
#endif //ITF_MEM_NO_REALLOC_IN_PLACE

       // Realloc in place.
        ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

        // Debug management : check guards
        CheckGuards(_pUserAddress);

        // Remove alloc block.
        RemoveAllocBlock(pHeader);

        // New free block address
        HeaderFree *pNewFreeBlock = (HeaderFree *)(((u8 *)pHeader) + uAllocSize);
        u32 uUnmergedFreeSize = pHeader->GetSize() - uAllocSize;
		u32 uMergedFreeSize = uUnmergedFreeSize;

        // Is there a free block after the alloc block ?
        HeaderFree *pBlockAfter = (HeaderFree *)pGetBlockAfterAllocBlock(pHeader);
        void *pNoDecommit;
        u32 uNoDecommitSize;
        if (pBlockAfter->m_bIsFree)
        {
            pNoDecommit = ((u8*)pBlockAfter) + sizeof(FreeBlock);
            uNoDecommitSize = pBlockAfter->GetSize()-ITF_MEM_MIN_BLOCK_SIZE;
            RemoveFreeBlock((FreeBlock *)pBlockAfter);

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
        
#ifdef ITF_MEM_HEAP_DEBUG_
        bbool bSaveFillAllocBlocks = m_bFillAllocBlocks;
        m_bFillAllocBlocks = bfalse;
#endif //ITF_MEM_HEAP_DEBUG_
        
        // Create alloc block
        void *pUserAddress = pAddAllocBlock(pHeader,uAllocSize,_u32_Size,pHeader->m_bIsBeforeFree, bfalse);
#ifdef ITF_MEM_HEAP_DEBUG_
        m_bFillAllocBlocks = bSaveFillAllocBlocks;
#endif //ITF_MEM_HEAP_DEBUG_

#ifdef ITF_MEM_HEAP_DEBUG_
        if (pUserAddress)
        {
	        // Debug management : fill free block.
	        if (m_bFillFreeBlocks)
	        {
				FillFreeMemory(((u8 *)pNewFreeBlock)+sizeof(FreeBlock),uMergedFreeSize-(sizeof(FreeBlock)+sizeof(FreeBlock*)));
	        }
            ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
        }
#endif //ITF_MEM_HEAP_DEBUG_

        return pUserAddress;
    }
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_Realloc(void *_pUserAddress, u32 _u32_Size, bbool _bAllowReallocInPlace)
{
    ITF_ASSERT(!_pUserAddress || b_IsPointerValid(_pUserAddress));

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
void Heap::ReallocCopy(void *_pOldAddress, void *_pNewAddress)
{
    u32 uOldSize = uGetUserSizeFromUserAddress(_pOldAddress);
    u32 uNewSize = uGetUserSizeFromUserAddress(_pNewAddress);

	u32 uCopySize = ITF::Min<u32>(uOldSize,uNewSize);

    ITF_Memcpy(_pNewAddress,_pOldAddress,uCopySize);
}


/*
 =======================================================================================================================
 Récupère la taille utilisateur d'une adresse
 =======================================================================================================================
 */
u32 Heap::uGetUserSizeFromUserAddress(void *_pUserAddress)
{
	u32 uSize;
	u32 uWastedSize;
    if (_pUserAddress < mpu8_PoolMemory)
    {
	    HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
	    ITF_ASSERT(!pHeader->m_bIsFree);
	    uSize = pHeader->GetSize();
	    uWastedSize = sizeof(HeaderAlloc);
    }
    else
    {
    	_pUserAddress = pGuardShiftPoolAddress(_pUserAddress);
    	u32 uPoolIndex = uGetPoolIndex(_pUserAddress);
    	Pool *pPool = pGetPool(uPoolIndex);
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
u32 Heap::uGetSizeFromUserAddress(void *_pUserAddress)
{
	u32 uSize;
    if (_pUserAddress < mpu8_PoolMemory)
    {
	    HeaderAlloc *pHeader = pGetHeaderFromUserAddress(_pUserAddress);
	    ITF_ASSERT(!pHeader->m_bIsFree);
	    uSize = pHeader->GetSize();
    }
    else
    {
    	_pUserAddress = pGuardShiftPoolAddress(_pUserAddress);
    	u32 uPoolIndex = uGetPoolIndex(_pUserAddress);
    	Pool *pPool = pGetPool(uPoolIndex);
    	uSize = pPool->uGetBlockSize();
    }
    return uSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_AllocAlign(u32 _u32_Size, u32 _u32_Align)
{
	if(_u32_Size == 0) 
        return NULL;

	if (_u32_Align <= 4)
		return p_Alloc(_u32_Size);

	u32 uAllocSize = uUserToPoolAllocSize(_u32_Size);

	// Try to put small alloc in pools
#ifndef ITF_MEM_NO_POOLS
	if (uAllocSize <= ITF_MEM_BLOCK_SIZE_MAX 
#ifdef ITF_MEM_HEAP_DEBUG_		
		&& !m_bAddGuards // Guards are incompatible with p_AllocAlignInPools (they break the alignment)
#endif // ITF_MEM_HEAP_DEBUG_
		)
	{
		void *pAddress = p_AllocAlignInPools(uAllocSize, _u32_Align);

		if (pAddress)
		{
			return pAddress;
		}
	}
#endif //ITF_MEM_NO_POOLS

    ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

	// uAllocSize is the (minimum) size of the user block (the block that will be returned to the user).
	uAllocSize = uUserToAllocSize(_u32_Size);

    // uNeededSize is the (minimum) size of the free block that is searched for.
	// It is bigger than uAllocSize because :
	// -> we create a new free block before the user block.
	// -> the alignement of the user block moves the address of the user of a value 
	// 		between 0 and (_u32_Align - sizeof(HeaderAlloc))
    u32 uNeededSize = uAllocSize + _u32_Align + ITF_MEM_MIN_BLOCK_SIZE - sizeof(HeaderAlloc);

    // Search free block of size big enough for alloc, and remove it for tables.
    FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uNeededSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();

    // Compute aligned alloc user address.
    u32 uUserAlignedAddress = (u32) (((u8 *)pFreeBlock) + _u32_Align + ITF_MEM_MIN_BLOCK_SIZE);
#ifdef ITF_MEM_HEAP_DEBUG_
    if (m_bAddGuards)
    	uUserAlignedAddress += 4;
#endif //ITF_MEM_HEAP_DEBUG_    	
    u32 uAlignMask = ~(_u32_Align-1);

	// Alignement moves the user address of 0 to (_u32_Align - 4).
    ITF_ASSERT(uUserAlignedAddress - (uUserAlignedAddress & uAlignMask) <= _u32_Align - sizeof(HeaderAlloc));
    uUserAlignedAddress &= uAlignMask;

    // Align address (minus header)
    HeaderAlloc *pHeader = pGetHeaderFromUserAddress((void *)uUserAlignedAddress);

    // Compute size of free block before alloc block.
    u32 uSizeBeforeAlloc = ((u32)pHeader) - ((u32)pFreeBlock);

	// Create free block after alloc block ? 
	ITF_ASSERT(uFreeSize >= (uSizeBeforeAlloc + uAllocSize));
    u32 uLeftoverSize = uFreeSize - uSizeBeforeAlloc - uAllocSize;

    if (uLeftoverSize >=  ITF_MEM_MIN_BLOCK_SIZE)
    {
        // Leftover size is big enough -> create a smaller free block after alloc.
        u8 *pFreeBlockAfter = ((u8 *)pHeader) + uAllocSize;
        AddFreeBlock(pFreeBlockAfter,uLeftoverSize,ITF_MEM_NO_DECOMMIT,0);
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
        ITF_ASSERT(uUserAlignedAddress == (u32)pUserAddress);
        
        // Create free block before alloc block (do it after alloc block has been added, because we need to change its flag).
        AddFreeBlock(pFreeBlock,uSizeBeforeAlloc,ITF_MEM_NO_DECOMMIT,0);

        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
        
        vCheckAligned(pUserAddress,_u32_Align);
    }

    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_ReallocAlign(void *_pUserAddress, u32 _u32_Size, u32 _u32_Align, bbool _bAllowReallocInPlace)
{
    ITF_ASSERT(!_pUserAddress || b_IsPointerValid(_pUserAddress));

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
void *Heap::p_AllocNotAlign(u32 _u32_Size)
{
    // The returned address should be aligned on 4 bytes, but not on 8 bytes (and thus higher values) 
    const u32 uAligned = 4;
    const u32 uNotAligned = 8;

	if(_u32_Size == 0) 
        return NULL;

    ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

	// uAllocSize is the (minimum) size of the user block (the block that will be returned to the user).
	u32 uAllocSize = uUserToAllocSize(_u32_Size);

    // uNeededSize is the (minimum) size of the free block that is searched for.
	// It is bigger than uAllocSize because we create a new free block before the user block, of size ITF_MEM_MIN_BLOCK_SIZE or ITF_MEM_MIN_BLOCK_SIZE + 4, in order to avoid alignment on 8 bytes.
    u32 uNeededSize = uAllocSize + uNotAligned + ITF_MEM_MIN_BLOCK_SIZE - sizeof(HeaderAlloc);

    // Search free block of size big enough for alloc, and remove it for tables.
    FreeBlock *pFreeBlock = pSearchAndRemoveFreeBlock(uNeededSize);

    // Manage the case where no free block was found
    if (!pFreeBlock)
    {
        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
        return NULL;
    }

    // What is the size of the free block we just retrieved ?
    u32 uFreeSize = pFreeBlock->m_Header.GetSize();

    // Compute aligned alloc user address.
    u32 uUserAddressBeforeAlignment = (u32) (((u8 *)pFreeBlock) + uAligned + sizeof(HeaderAlloc) + ITF_MEM_MIN_BLOCK_SIZE);
#ifdef ITF_MEM_HEAP_DEBUG_
    if (m_bAddGuards)
    	uUserAddressBeforeAlignment += 4;
#endif //ITF_MEM_HEAP_DEBUG_    	
    const u32 uAlignMask = ~(uNotAligned-1);

	// Alignment removes 0 to (uNotAligned - 4) to the address.
    u32 uUserAlignedAddress = uAlignMask & uUserAddressBeforeAlignment;
    ITF_ASSERT(uUserAddressBeforeAlignment - uUserAlignedAddress <= uNotAligned - sizeof(HeaderAlloc));

    // uUserAlignedAddress is aligned on 8 bytes.
    // In order to have an address not aligned on 8 bytes, we either add or remove 4 bytes
    u32 uUserNotAlignedAddress = ((uUserAddressBeforeAlignment == uUserAlignedAddress) ? uUserAlignedAddress - 4 : uUserAlignedAddress + 4);

    // Align address (minus header)
    HeaderAlloc *pHeader = pGetHeaderFromUserAddress((void *)uUserNotAlignedAddress);

    // Compute size of free block before alloc block.
    u32 uSizeBeforeAlloc = ((u32)pHeader) - ((u32)pFreeBlock);

    ITF_ASSERT(uSizeBeforeAlloc >= ITF_MEM_MIN_BLOCK_SIZE);

	// Create free block after alloc block ? 
	ITF_ASSERT(uFreeSize >= (uSizeBeforeAlloc + uAllocSize));
    u32 uLeftoverSize = uFreeSize - uSizeBeforeAlloc - uAllocSize;

    if (uLeftoverSize >=  ITF_MEM_MIN_BLOCK_SIZE)
    {
        // Leftover size is big enough -> create a smaller free block after alloc.
        u8 *pFreeBlockAfter = ((u8 *)pHeader) + uAllocSize;
        AddFreeBlock(pFreeBlockAfter,uLeftoverSize,ITF_MEM_NO_DECOMMIT,0);
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
        ITF_ASSERT(uUserNotAlignedAddress == (u32)pUserAddress);
        
        // Create free block before alloc block (do it after alloc block has been added, because we need to change its flag).
        AddFreeBlock(pFreeBlock,uSizeBeforeAlloc,ITF_MEM_NO_DECOMMIT,0);

        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
    }
    
    return pUserAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *Heap::p_AllocTmp(u32 _u32_Size)
{
	if(_u32_Size == 0) 
        return NULL;
        
#ifdef ITF_MEM_NO_TMP

	return NULL;

#else //ITF_MEM_NO_TMP

    ITF_MEM_CHECK(ITF_MEM_CHECK_BEFORE_ALLOC);

	u32 uAllocSize = uUserToAllocSize(_u32_Size);

    void *pAddress = pMoveTmpPointer(-((i32)uAllocSize));
    if (!pAddress)
	{
        // Not enough memory at end of heap : the calling function must deal with this 
        // and allocate the memory elsewhere (another heap on wii, or with a normal alloc on PC).
		return NULL;
	}

    void *pUserAddress = pAddAllocBlock(pAddress,uAllocSize,_u32_Size, bfalse, btrue);
    if (pUserAddress)
    {
#ifdef ITF_MEM_HEAP_DEBUG_
        // Debug management : update measures
        m_uAllocTmpSize += uAllocSize;
        m_uAllocSize -= uAllocSize;
        m_uAllocTmpNb ++;
        m_uAllocNb --;
#endif //ITF_MEM_HEAP_DEBUG_

        ITF_MEM_CHECK(ITF_MEM_CHECK_AFTER_ALLOC);
    }

	// return address
	return pUserAddress;

#endif //ITF_MEM_NO_TMP
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Heap::FreeInTmp(void *_pUserAddress)
{
	HeaderAllocTmp *pHeader = (HeaderAllocTmp *)pGetHeaderFromUserAddress(_pUserAddress);
    pHeader->m_bIsFreeable = 1;
#ifdef ITF_MEM_HEAP_DEBUG_
    m_uFreeableTmpNb++;
#endif /* ITF_MEM_HEAP_DEBUG_ */

    // Try to free as many tmp blocks as possible
    pHeader = (HeaderAllocTmp *)mpu8_TmpMemory;

    while (pHeader->m_bIsFreeable && (((u8 *)pHeader) != mpu8_PoolMemory))
    {
#ifdef ITF_MEM_HEAP_DEBUG_
        // Debug management : update measures
        m_uFreeableTmpNb--;
        m_uAllocTmpSize -= pHeader->GetSize();
        m_uAllocTmpNb --;
#endif //ITF_MEM_HEAP_DEBUG_

        pHeader = pMoveTmpPointer(pHeader->GetSize());
    }
}

} //namespace ITF
#endif //ENABLE_ALLOCATOR
