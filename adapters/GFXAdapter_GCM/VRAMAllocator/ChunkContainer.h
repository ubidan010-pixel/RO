#ifndef VRAM_CHUNKCONTAINER_H_
#define VRAM_CHUNKCONTAINER_H_

#include "VRAMHandleLinks.h"
#include "VRAMMemoryTypes.h"

namespace ITF
{
namespace GCM
{
namespace VRAMAllocator
{
    // minimal block size
    const ChunkSize MinimalChunkSize = 128;
    const ChunkSize MinimalAlignment = 128; // has to be a multiple of MinimalChunkSize

    u8 * const InvalidAddress = 0;
    const u32  InvalidOffset = 0x00000000; // note that 0 is also a valid offset

    ITF_COMPILE_TIME_ASSERT(not_enough_bits_for_number_of_vram_type, VRAM_Last <= (1 << 3)); // 3 bits for the type

    struct ChunkEntry
    {
    public:
        VRAMAddress addr;
        u32 size : 28;
        u32 type : 3; // only 8 types available
        u32 isFree : 1;
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
        ~ChunkContainer();

        void Init(VRAMAddress startOfBlock, ChunkSize blockSize, VRAMOffset offsetStart, VRAMLocation location, bool forceOOMBreak, bool verboseOutOfMem = false, bool staticBuffer = false);
        
        VRAMAddress GetStartAddress() const { return m_startAddr; }
        VRAMOffset GetStartOffset() const { return m_baseOffset; }

        VRAMAddress GetAddress(ChunkIdx idx) const { return GetChunk(idx).addr; }
        VRAMOffset GetOffset(ChunkIdx idx) const { return (GetAddress(idx) - m_startAddr) + m_baseOffset; }
        VRAMLocation GetLocation() const { return m_location; }
        
        ChunkIdx GetIndexFromLink(const VRAMHandleLinks & link) const;

        ChunkIdx Alloc(ChunkSize size, VramMemoryType type);
        ChunkIdx AllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type);

        bool IsOwner(const VRAMHandleLinks & hdlLink) const;

        bool IsEmpty() const;

        bool IsStatic() const { return m_staticBuffer; }
        
        void    SetForceOOMBreak(bool flag) { m_forceOOMBreak = flag; }

        void FreeRelocHandle(const VRAMHandleLinks & hdlLink);
        void LinkRelocHandle(VRAMHandleLinks & relocHandle, ChunkIdx idx);
        static ChunkSize MemSize(const VRAMHandleLinks & relocHandle);

        void PrintInfo();

        void GetStats(u32 & freeSizeLeft, u32 & biggestFreeBlock, u32 & initialSize);

        void Update(u32 cpuCurrentFrame, u32 gpuLastFrameCount, bool bForceErase);

		static const ChunkIdx InvalidIndex = (ChunkIdx)-1;

    private:

        static const ChunkEntry & GetChunkEntryFromLink(const VRAMHandleLinks & link);

        // As the RelocHdl links are by ptr, there is a problem with the fact
        // that the ChunkEntry are in a growing Vector.
        // So we are updating the links every time the vector is growing.
        void PatchVRAMHandleLinks(u32 nbEltMax);

        void Free(ChunkIdx idx);
        void SetInDeathRow(ChunkIdx idx);
        void CheckBlockLinks(ChunkIdx idx) const;

        ChunkIdx CreateChunk(); // create uninit chunk
        void RemoveChunk(ChunkIdx idx);
        inline const ChunkEntry & GetChunk(ChunkIdx chunkIdx) const { return m_chunks[chunkIdx]; }
        inline ChunkEntry & GetChunk(ChunkIdx chunkIdx) { return m_chunks[chunkIdx]; }
        bool IsValid(ChunkIdx idx) const { return idx!=InvalidIndex; }
        bool IsFreeChunk(ChunkIdx idx) const { return GetChunk(idx).isFree; }

        void UnlinkFree(ChunkIdx idx); // remove from free list
        void UnlinkFreeFromList(ChunkIdx idx, int binIndex); // same as above, but the bin index is known (faster)
        void LinkFreeAfter(ChunkIdx idx, ChunkIdx newEntry);
        void LinkFreeInHead(ChunkIdx idx, ChunkIdx oldHead);

        ChunkIdx FindBestChunkInFreeList(ChunkIdx start, ChunkSize size);
        ChunkIdx FindBestChunkInFreeList(ChunkIdx start, ChunkSize size, ChunkSize align);

        int GetBinChunkIndex(ChunkSize size); // return the good size block
        void InsertChunkInFreeList(ChunkIdx chunkIdx); // need to be already merged with adjacent blocks

        void TripleSplitChunk(ChunkIdx hChunkToSplit, ChunkIdx & hFirst, ChunkIdx & hSecond, ChunkIdx & hThird, ChunkSize size, ChunkSize align, VramMemoryType type);
        ChunkIdx SplitChunk(ChunkIdx bestFit, ChunkSize size, VramMemoryType type); // return the free chunk or invalid if not exist
        ChunkIdx MergeFree(ChunkIdx idx);

        ChunkIdx GetFirstChunk() const; // quite slow, use it only for debug purpose

        Vector<ChunkEntry> m_chunks; // list of all chunks (not in memory order)
        ChunkIdx * m_chunkBin; // list of all free chunks sorted by size
        ChunkIdx m_firstNotUsedEntry; // list of all index in m_chunks not used

        ChunkSize m_initialSize;
        VRAMAddress m_startAddr;

        VRAMLocation m_location;

        VRAMOffset m_baseOffset;

#ifdef VRAM_ALLOC_COUNT
        ChunkSize m_totalFreeSize;
#endif
        bool m_verboseOutOfMem;
        bool m_forceOOMBreak;
        bool m_staticBuffer;

        struct ChunkDeathRowEntry
        {
            ChunkDeathRowEntry() {}
            ChunkDeathRowEntry(ChunkIdx chunk, u32 frameCount) : m_chunk(chunk), m_frameCount(frameCount) {}
            ChunkIdx m_chunk;
            u32 m_frameCount;
        };

        typedef Vector<ChunkDeathRowEntry> ChunkDeathRow;
        ChunkDeathRow m_chunksInDeathRow;
    };
} // namespace VRAMAllocator
} // namespace GCM
} // namespace ITF

#endif // VRAM_CHUNKCONTAINER_H_
