#ifndef _ITF_3DS_VRAMALLOCATOR_H_
#define _ITF_3DS_VRAMALLOCATOR_H_

#ifdef ITF_CTR

#include <bitset>

namespace ITF
{

class VramAllocator
{

public:
	enum E_AllocDirection
	{
		AllocDirection_FromBeginning,
		AllocDirection_FromEnd
	};

public:
	VramAllocator();
	~VramAllocator();

	void Initialize(uptr vramStartAddress, std::size_t vramSize);
	void Finalize();

	void* Alloc(u32 size, E_AllocDirection direction = AllocDirection_FromBeginning);             //alloc will always be aligned on block boundary
	void  Free(void* ptr);
	bool  CanHandleAllocation(u32 size);

	u32 GetAllocatedSize(void* ptr);

	inline u32    GetBlockSize();
	inline u32    GetTotalSize();         
	inline u32    GetAvailableSize();    
	inline u32    GetUsedSize();     
	inline u32    GetStartAddress();
	inline u32    GetEndAddress();

private:
	void OutputFreeBlocks();
	void OutputUsedBlocks();

	u16 ComputeChainIndex(void* ptr);
	enum { INVALID_CHAIN_INDEX = 65535 };

private:
	static const std::size_t BLOCK_SIZE = 4 * 1024; //4KB
	static const std::size_t STATIC_VRAM_SIZE = 4 * 1024 * 1024; // 3MB
	static const std::size_t STATIC_BITSET_SIZE = STATIC_VRAM_SIZE / (BLOCK_SIZE) ; // 4MB / 4KB

private:
	struct BlockChain
	{
		u16 startIndex;
		u16 blockCount;
	};

	u32 m_startAddress;
	u32 m_endAddress;
	u32 m_blockCount;

	std::bitset<STATIC_BITSET_SIZE>   m_blocksUsed;
	BlockChain*     m_usedChains;

	u16       m_usedChainCount;
	u16       m_freeBlockCount;

	u16       m_blockSize;
};

inline u32 VramAllocator::GetBlockSize()
{
	return m_blockSize;
}

inline u32 VramAllocator::GetTotalSize()
{
	return m_blockCount*m_blockSize;
}

inline u32 VramAllocator::GetAvailableSize()
{
	return m_freeBlockCount*m_blockSize;
}

inline u32 VramAllocator::GetUsedSize()
{
	return GetTotalSize()-GetAvailableSize();
}

inline u32 VramAllocator::GetStartAddress()
{
	return m_startAddress;
}

inline u32 VramAllocator::GetEndAddress()
{
	return m_endAddress;
}

} // namespace ITF

#endif // ITF_CTR
#endif // _ITF_3DS_VRAMALLOCATOR_H_
