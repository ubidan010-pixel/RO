#pragma once

#include "VRAMHandleLinks.h"
#include "VRAMMemoryTypes.h"
#include "core/container/extendedVector.h"
#include "NVN/Helpers.h"

namespace ITF::NVN::VRAMAllocator
{
    // minimal block size and alignment
    constexpr ChunkSize MinimalChunkSize = 256;
    constexpr ChunkSize MinimalAlignment = 256;
    static_assert(MinimalAlignment% MinimalChunkSize == 0, "MinimalAlignment must be a multiple of MinimalChunkSize");

    struct ChunkEntry
    {
    public:
        VRAMAddress addr;
        ChunkSize size; // max 4GiB per block
        u16 type;
        bool isFree;
        // linear linked list
        ChunkIdx   prev;
        ChunkIdx   next;
        // free list (used when in bin array)
        union
        {
            struct
            {
                ChunkIdx   prev;
                ChunkIdx   next;
            } freeLink;                 // free chunk -> double list 

            VRAMHandleLinks allocLinks; // allocated chunk -> double linked list of RelocHdl
        };
    };


    //////////////////////////////////////////////////////////////////////////
    // chunk container
    //////////////////////////////////////////////////////////////////////////

    class ChunkContainer
    {
    public:

        ChunkContainer();
        ChunkContainer(const ChunkContainer&) = delete;
        ~ChunkContainer();

        ChunkContainer & operator = (const ChunkContainer&) = delete;

        void Init(nvn::MemoryPoolBuilder poolBuilder, bool forceOOMBreak, bool verboseOutOfMem = false, bool staticBuffer = false);
        
        VRAMAddress GetStartAddress() const { return m_startAddr; }

        ChunkSize GetContainerSize() const { return m_initialSize; }

        VRAMAddress GetAddress(ChunkIdx idx) const { return GetChunk(idx).addr; }
        iPtr GetOffset(ChunkIdx idx) const { return GetChunk(idx).addr - m_startAddr; }

        GPUStorage GetStorage(ChunkIdx idx) const
        {
            const ChunkEntry& chunk = GetChunk(idx);
            VRAMChunkSize offset = VRAMChunkSize(chunk.addr - m_startAddr);
            return GPUStorage{ m_memoryPool.get(), offset, chunk.size}; }

        ChunkIdx GetIndexFromLink(const VRAMHandleLinks & link) const;

        ChunkIdx Alloc(ChunkSize size, VramMemoryCategory type);
        ChunkIdx AllocAligned(ChunkSize size, ChunkSize align, VramMemoryCategory type);

        bool CanHandleAllocation(ChunkSize _blockSize, ux _alignment);

        bool IsOwner(const VRAMHandleLinks & hdlLink) const;

        bool IsEmpty() const;

        bool IsStatic() const { return m_staticBuffer; }
        
        void SetForceOOMBreak(bool flag) { m_forceOOMBreak = flag; }
        bool GetForceOOMBreak() const { return m_forceOOMBreak; }

        void SetVerboseOOM(bool flag) { m_verboseOutOfMem = flag; }
        bool GetVerboseOOM() const { return m_verboseOutOfMem; }

        void FreeRelocHandle(const VRAMHandleLinks & hdlLink);
        void LinkRelocHandle(VRAMHandleLinks & relocHandle, ChunkIdx idx);
        static ChunkSize MemSize(const VRAMHandleLinks & relocHandle);

        void PrintInfo();

        void GetStats(ChunkSize & freeSizeLeft, ChunkSize & biggestFreeBlock, ChunkSize & initialSize);

        void Update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bool bForceErase);

        const nvn::MemoryPool* getMemoryPool() const { return m_memoryPool.get(); }
        nvn::MemoryPool* getMemoryPool() { return m_memoryPool.get(); }

        static const ChunkIdx InvalidIndex = ChunkIdx(-1);

    private:

        static const ChunkEntry & GetChunkEntryFromLink(const VRAMHandleLinks & link);

        // As the RelocHdl links are by ptr, there is a problem with the fact
        // that the ChunkEntry are in a growing Vector.
        // So we are updating the links every time the vector is growing.
        void PatchVRAMHandleLinks(ux nbEltMax);

        void Free(ChunkIdx idx);
        void SetInDeathRow(ChunkIdx idx);
        bool IsInDeathRow(ChunkIdx idx) const;
        void CheckBlockLinks(ChunkIdx idx) const;

        ChunkIdx CreateChunk(); // create uninit chunk
        void RemoveChunk(ChunkIdx idx);
        inline const ChunkEntry & GetChunk(ChunkIdx chunkIdx) const { return m_chunks[chunkIdx]; }
        inline ChunkEntry & GetChunk(ChunkIdx chunkIdx) { return m_chunks[chunkIdx]; }
        bool IsValid(ChunkIdx idx) const { return idx!=InvalidIndex; }
        bool IsFreeChunk(ChunkIdx idx) const { return GetChunk(idx).isFree; }

        void UnlinkFree(ChunkIdx idx); // remove from free list
        void UnlinkFreeFromList(ChunkIdx idx, ix binIndex); // same as above, but the bin index is known (faster)
        void LinkFreeAfter(ChunkIdx idx, ChunkIdx newEntry);
        void LinkFreeInHead(ChunkIdx idx, ChunkIdx oldHead);

        ChunkIdx FindBestChunkInFreeList(ChunkIdx start, ChunkSize size) const;
        ChunkIdx FindBestChunkInFreeList(ChunkIdx start, ChunkSize size, ChunkSize align) const;

        ix GetBinChunkIndex(ChunkSize size); // return the good size block
        void InsertChunkInFreeList(ChunkIdx chunkIdx); // need to be already merged with adjacent blocks

        void TripleSplitChunk(ChunkIdx hChunkToSplit, ChunkIdx & hFirst, ChunkIdx & hSecond, ChunkIdx & hThird, ChunkSize size, ChunkSize align, VramMemoryCategory type);
        ChunkIdx SplitChunk(ChunkIdx bestFit, ChunkSize size, VramMemoryCategory type); // return the free chunk or invalid if not exist
        ChunkIdx MergeFree(ChunkIdx idx);

        ChunkIdx GetFirstChunk() const; // quite slow, use it only for debug purpose

        UniquePtr<nvn::MemoryPool> m_memoryPool = nullptr;

        Vector<ChunkEntry> m_chunks; // list of all chunks (not in memory order)
        ChunkIdx * m_chunkBin; // list of all free chunks sorted by size
        ChunkIdx m_firstNotUsedEntry; // list of all index in m_chunks not used

        ChunkSize m_initialSize;
        VRAMAddress m_startAddr;

#ifdef VRAM_ALLOC_COUNT
        ChunkSize m_totalFreeSize;
#endif
        bool m_verboseOutOfMem;
        bool m_forceOOMBreak;
        bool m_staticBuffer;

        struct ChunkDeathRowEntry
        {
            ChunkDeathRowEntry(ChunkIdx chunk, u64 frameCount) : m_chunk(chunk), m_frameCount(frameCount) {}
            ChunkIdx m_chunk;
            u64 m_frameCount;
        };

        using ChunkDeathRow = Vector<ChunkDeathRowEntry>;
        ChunkDeathRow m_chunksInDeathRow;
    };
} // namespace ITF::NVN::VRAMAllocator
