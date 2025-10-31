#include "precompiled_GFXAdapter_3DS_GR.h"

#ifndef _ITF_3DS_VRAMALLOCATOR_H_
#include "adapters/GFXAdapter_3DS_GR/VramAllocator.h"
#endif //_ITF_3DS_VRAMALLOCATOR_H_

namespace ITF
{

VramAllocator::VramAllocator()
	:m_startAddress(0)
	,m_endAddress(0)
	,m_blockCount(0)
	,m_usedChains(NULL)
	,m_usedChainCount(0)
	,m_freeBlockCount(0)
	,m_blockSize(0)
{
}

//------------------------------------------------------------------------
VramAllocator::~VramAllocator()
{
	Finalize();
}

//------------------------------------------------------------------------
void VramAllocator::Initialize(uptr vramStartAddress, std::size_t vramSize)
{
	if (m_usedChains != NULL)
	{
		//already init
		return;
	}

	GFX_ASSERT(vramSize <= STATIC_VRAM_SIZE, "The chosen static vram size is too low\n");

	m_blockSize = BLOCK_SIZE;
	m_startAddress = vramStartAddress;
	m_endAddress = vramStartAddress + vramSize;
	m_blockCount = vramSize / m_blockSize;

	m_blocksUsed.reset();
	m_freeBlockCount = m_blockCount;

	m_usedChains = newAlloc(mId_System ,BlockChain[m_blockCount]);
	m_usedChainCount = 0;
}

//------------------------------------------------------------------------
void VramAllocator::Finalize()
{
	delete[] m_usedChains;
	m_usedChains = NULL;
}

//------------------------------------------------------------------------
void* VramAllocator::Alloc(u32 size, E_AllocDirection direction)
{
	GFX_ASSERT(m_usedChains, "VramArea: trying to allocate before initializing the area!");

	//compute number of needed blocks
	u16 neededBlockCount = size/m_blockSize;
	if (size%m_blockSize)
		neededBlockCount++;

	if (neededBlockCount == 0)
	{
		GFX_ASSERT(size == 0, "VramArea: Bad computation of required block count!");
		return NULL;
	}

	if (neededBlockCount > m_freeBlockCount)
	{
		GFX_ASSERT(false, "No more available VRAM! (%d free blocks, needed %d)", m_freeBlockCount, neededBlockCount);
		return NULL;
	}

	//look for a free block chain that's long enough to host the allocation (best fit)
	BlockChain currentChain;
	currentChain.blockCount = 0;
	currentChain.startIndex = 0;

	BlockChain bestChain;
	bestChain.blockCount = m_blockCount+1;
	bestChain.startIndex = 0;

	for (u16 block = 0; block < m_blockCount; ++block)
	{
		if (!m_blocksUsed[block])
		{
			if (block > 0 && m_blocksUsed[block-1])
			{
				//chain start
				currentChain.startIndex = block;
				currentChain.blockCount = 0;
			}
			currentChain.blockCount++;
		}

		if (m_blocksUsed[block] || block == (m_blockCount-1))
		{
			//chain end
			if (direction == AllocDirection_FromEnd)
			{
				if (currentChain.blockCount >= neededBlockCount && currentChain.blockCount <= bestChain.blockCount)
				{
					bestChain = currentChain;
				}
			}
			else
			{
				if (currentChain.blockCount >= neededBlockCount && currentChain.blockCount < bestChain.blockCount)
				{
					bestChain = currentChain;
				}
			}
		}
	}

	if (bestChain.blockCount > m_blockCount)
	{
		//couldn't find a long enough chain to allocate the required size
		GFX_ASSERT(false, "VRAM too fragmented to accomodate this allocation! (needed %d contiguous blocks)", neededBlockCount);
		return NULL;
	}

	GFX_ASSERT(neededBlockCount <= m_freeBlockCount, "VramArea allocation bug!!!");
	GFX_ASSERT(bestChain.blockCount <= m_freeBlockCount, "VramArea allocation bug!!!");
	GFX_ASSERT(bestChain.startIndex+bestChain.blockCount <= m_blockCount, "VramArea allocation bug!!!");

	//add newly allocated chain to used chain list
	u16 newChainStart = bestChain.startIndex;
	if (direction == AllocDirection_FromEnd)
	{
		//take the end of the chain, not the beginning
		newChainStart += (bestChain.blockCount-neededBlockCount);
	}

	u16 chainIndex = m_usedChainCount;
	for (; chainIndex > 0 && m_usedChains[chainIndex-1].startIndex > newChainStart; --chainIndex)
	{
		m_usedChains[chainIndex] = m_usedChains[chainIndex-1];
	}
	m_usedChains[chainIndex].startIndex = newChainStart;
	m_usedChains[chainIndex].blockCount = neededBlockCount;
	m_usedChainCount++;

	//remove newly allocated blocks from free block list
	m_freeBlockCount -= neededBlockCount;
	for (u16 i = 0; i < neededBlockCount; ++i)
	{
		m_blocksUsed[newChainStart+i] = true;
	}

	return reinterpret_cast<void*>(m_startAddress + (newChainStart*m_blockSize));
}

//------------------------------------------------------------------------
void VramAllocator::Free(void* ptr)
{
	GFX_ASSERT(m_usedChains, "VramArea: trying to deallocate before initializing the area!");


	if (!ptr)
		return;     //nothing to delete

	u16 chainIndex = ComputeChainIndex(ptr);

	//add deallocated blocks to free block list
	for (u16 i = 0; i < m_usedChains[chainIndex].blockCount; ++i)
	{
		GFX_ASSERT(m_blocksUsed[m_usedChains[chainIndex].startIndex+i], "VramArea: inconsistent area state during deallocation!");
		m_blocksUsed[m_usedChains[chainIndex].startIndex+i] = false;
	}
	m_freeBlockCount+=m_usedChains[chainIndex].blockCount;

	//remove deallocated chain from used chain list
	m_usedChainCount--;
	for (u16 i = chainIndex; i < m_usedChainCount; ++i)
	{
		m_usedChains[i] = m_usedChains[i + 1];
	}
}

//------------------------------------------------------------------------
u32 VramAllocator::GetAllocatedSize(void* ptr)
{
	if (!ptr)
		return 0;

	u16 chainIndex = ComputeChainIndex(ptr);
	return m_blockSize*m_usedChains[chainIndex].blockCount;
}

//------------------------------------------------------------------------
u16 VramAllocator::ComputeChainIndex(void* ptr)
{
	GFX_ASSERT(reinterpret_cast<u32>(ptr) >= m_startAddress && reinterpret_cast<u32>(ptr) < (m_startAddress + (m_blockCount*m_blockSize)), 
		"VramArea ComputeChainIndex: pointer 0x%08X is not located in this area!", ptr);
	u16 deletedBlock = (reinterpret_cast<u32>(ptr) - m_startAddress)/ m_blockSize;

	GFX_ASSERT(reinterpret_cast<u32>(ptr) == (m_startAddress + (deletedBlock*m_blockSize)), "VramArea ComputeChainIndex: invalid address! (not on a block boundary)");
	GFX_ASSERT(m_blocksUsed[deletedBlock], "VramArea ComputeChainIndex: address was either not allocated by this area or corrupted!");

	//locate block in used chains
	u16 chainIndex = 0;
	for (; chainIndex < m_usedChainCount; ++chainIndex)
	{
		if (m_usedChains[chainIndex].startIndex == deletedBlock)
		{
			//found block we're trying to delete
			break;
		}
	}

	if (chainIndex == m_usedChainCount)
	{
		GFX_ASSERT(chainIndex != m_usedChainCount, "VramArea ComputeChainIndex: address was either not allocated by this area or corrupted!");
		return INVALID_CHAIN_INDEX;
	}

	return chainIndex;
}

//------------------------------------------------------------------------
bool VramAllocator::CanHandleAllocation(u32 size)
{
	GFX_ASSERT(m_usedChains, "VramArea: trying to allocate before initializing the area!");

	//compute number of needed blocks
	u16 neededBlockCount = size / m_blockSize;
	if (size % m_blockSize)
	{
		neededBlockCount++;
	}

	if (neededBlockCount > m_freeBlockCount)
	{
		return false;
	}

	//look for a free block chain that's long enough to host the allocation
	BlockChain currentChain;
	currentChain.blockCount = 0;
	currentChain.startIndex = 0;

	for (u16 block = 0; block < m_blockCount; ++block)
	{
		if (!m_blocksUsed[block])
		{
			if (block > 0 && m_blocksUsed[block-1])
			{
				//chain start
				currentChain.startIndex = block;
				currentChain.blockCount = 0;
			}
			currentChain.blockCount++;
		}

		if(currentChain.blockCount >= neededBlockCount)
		{
			return true;
		}
	}

	return false;
}

} // namespace ITF