#include "precompiled_GFXAdapter_DX12.h"

#include "DX12/Helpers.h"

#include "Core/math/BitTweak.h"

#include "ChunkContainer.h"
#include "VRAMAllocatorBinSizes.h"
#include "VRAMHandleLinks.h"
#include "VRAMMemoryTypes.h"

#include "core/math/MathTools.h"

#include <atomic>
#include <limits>

// MEMORY tracking 
#define MEMMNG_TRACER_ALLOC_VRAM(x_, size_) do {} while (0)
#define MEMMNG_TRACER_FREE_VRAM(x_, size_) do {} while (0)
#define MEMMNG_TRACER_ASKEDSIZE_VRAM(size_) do {} while (0)

#if MEMMNG_MEM_COUNT >= 1
#  define VRAM_COUNT_ALLOC_RESERVED(x, size)	do { DECL_MEM_USAGE(size); COUNT_ALLOC(VramMngDirectReserved);} while(0)
#  define VRAM_COUNT_ALLOC_UNRESERVED(x, size)	do { DECL_MEM_USAGE(size); COUNT_FREE(VramMngDirectReserved); } while(0)
#  define VRAM_COUNT_ALLOC(x, size)	do { DECL_MEM_USAGE(size); COUNT_ALLOC(VramMngDirect); } while(0)
#  define VRAM_COUNT_FREE(x, size)	do { DECL_MEM_USAGE(size); COUNT_FREE(VramMngDirect); } while(0)
#else
#  define VRAM_COUNT_ALLOC_RESERVED(x, size)	do{}while(0)
#  define VRAM_COUNT_ALLOC_UNRESERVED(x, size)	do{}while(0)
#  define VRAM_COUNT_ALLOC(x, size)	do{}while(0)
#  define VRAM_COUNT_FREE(x, size)	do{}while(0)
#endif

// size counts
#ifdef VRAM_ALLOC_COUNT
    #define VRAM_ALLOC_DEC_MEM_COUNT(size) m_totalFreeSize -= ChunkSize(size)
    #define VRAM_ALLOC_INC_MEM_COUNT(size) m_totalFreeSize += ChunkSize(size)
#else
    #define VRAM_ALLOC_DEC_MEM_COUNT(size) do{}while(0)
    #define VRAM_ALLOC_INC_MEM_COUNT(size) do{}while(0)
#endif

// link checks
#ifdef ASSERT_ENABLED
    #define CHUNK_CHECK_VALID_LINKS CheckBlockLinks
#else
    #define CHUNK_CHECK_VALID_LINKS(...)
#endif

namespace ITF::DX12::VRAMAllocator
{
    ChunkContainer::ChunkContainer()
        : m_chunkBin(0)
        , m_firstNotUsedEntry(0)
        , m_initialSize(0)
        , m_startAddr(0)
        , m_verboseOutOfMem(false)
        , m_forceOOMBreak(true)
        , m_staticBuffer(false)
    {
        #ifdef VRAM_ALLOC_COUNT
        m_totalFreeSize = 0;
        #endif
    }

    ChunkContainer::~ChunkContainer()
    {
        VRAM_COUNT_ALLOC_UNRESERVED(m_startAddr, m_initialSize);
        delete [] m_chunkBin;
    }

    static VRAMAddress computeFakeStartVRAMAddress(ID3D12Heap* _heap)
    {
        uPtr nbBitForAlignment = BitTweak::powerOfTwo(MinimalAlignment);

        uPtr heapAddress = reinterpret_cast<uPtr>(_heap);
        static std::atomic<uPtr> heapUniqueCount = 0;

        uPtr nbBitForIndex = 16 - nbBitForAlignment;
        uPtr nbBitForHeap = sizeof(uPtr) * 8 - nbBitForIndex - nbBitForAlignment; // should be 48 bits on x64
         // 64K until it wraps, but anyway we should not have that many heaps and we have the heap address to distinguish them
        uPtr offsetOfHeap = nbBitForAlignment;
        uPtr offsetOfIndex = sizeof(uPtr) * 8 - nbBitForIndex;

        uPtr clampedHeapAddress = heapAddress & ((uPtr(1) << nbBitForHeap) - uPtr(1));
        uPtr clampedHeapCount = heapUniqueCount & ((uPtr(1) << nbBitForIndex) - uPtr(1));
        uPtr fakeAddress = (clampedHeapCount << offsetOfIndex) | (clampedHeapAddress << offsetOfHeap);

        heapUniqueCount++;

        return reinterpret_cast<VRAMAddress>(fakeAddress);
    }

    void ChunkContainer::Init(ComPtr<ID3D12Heap> _heap, bool forceOOMBreak, bool verboseOutOfMem, bool staticBuffer)
    {
        VRAMAddress startOfBlock = computeFakeStartVRAMAddress(_heap.Get());

        ITF_ASSERT(BitTweak::IsAligned(startOfBlock, MinimalAlignment));

        D3D12_HEAP_DESC heapDesc = _heap->GetDesc();
        ITF_ASSERT(heapDesc.SizeInBytes < static_cast<UINT64>(std::numeric_limits<ChunkSize>::max())); // max 4GiB per pool
        ChunkSize blockSize = ChunkSize(heapDesc.SizeInBytes);

        ITF_ASSERT(m_memoryHeap == nullptr);
        m_memoryHeap = std::move(_heap);

        VRAM_COUNT_ALLOC_RESERVED(startOfBlock, blockSize);

        m_staticBuffer = staticBuffer;

        // create and clear bins
        m_chunkBin = newAlloc(mId_VRAMPool, ChunkIdx[NumberOfBinSizes]);
        for(ux i = 0; i<NumberOfBinSizes; ++i)
        {
            m_chunkBin[i] = ChunkContainer::InvalidIndex;
        }

        m_firstNotUsedEntry = InvalidIndex;
        m_initialSize = blockSize;
        m_startAddr = startOfBlock;
        m_verboseOutOfMem = verboseOutOfMem;
        m_forceOOMBreak = forceOOMBreak;
        #ifdef VRAM_ALLOC_COUNT
        m_totalFreeSize = blockSize;
        #endif

        ChunkIdx chunkIdx = CreateChunk();
        ChunkEntry & chunk = GetChunk(chunkIdx);
        chunk.addr = startOfBlock;
        chunk.size = blockSize;
        chunk.isFree = true;
        chunk.next = ChunkContainer::InvalidIndex;
        chunk.prev = ChunkContainer::InvalidIndex;
        chunk.freeLink.next = ChunkContainer::InvalidIndex;
        chunk.freeLink.prev = ChunkContainer::InvalidIndex;
        InsertChunkInFreeList(chunkIdx);
    }

    bool ChunkContainer::IsOwner(const VRAMHandleLinks & hdlLink) const
    {
        if (!m_chunks.empty())
        {
            const ChunkEntry * start = &m_chunks[0];
            const ChunkEntry * end = start+m_chunks.size();
            return ((const u8 *)&hdlLink)>=((const u8 *)(start)) && ((const u8 *)&hdlLink)<((const u8 *)(end));
        }
        return false;
    }

    bool ChunkContainer::IsEmpty() const
    {
        if (m_chunks.empty())
        {
            return true;
        }

        // search for the first free block in bins...

        ChunkIdx bestFit;

        ux binIndex = 0;
        do
        {
            bestFit = FindBestChunkInFreeList(m_chunkBin[binIndex], MinimalChunkSize);
            binIndex++;
        } while(!IsValid(bestFit) && binIndex<NumberOfBinSizes);

        return IsValid(bestFit) && GetChunk(bestFit).size == m_initialSize; // we get a free chunk with the same size as the full memory
    }

    void ChunkContainer::LinkRelocHandle(VRAMHandleLinks & relocHandle, ChunkIdx idx)
    {
        ChunkEntry & chunk = GetChunk(idx);
        relocHandle.next = chunk.allocLinks.next;
        relocHandle.prev = &chunk.allocLinks;
        if (relocHandle.next)
        {
            relocHandle.next->prev = &relocHandle;
        }
        chunk.allocLinks.next = &relocHandle;
        VRAM_ALLOC_ASSERT(chunk.allocLinks.prev == 0);
    }

    void ChunkContainer::FreeRelocHandle(const VRAMHandleLinks & hdlLink)
    {
        VRAM_ALLOC_ASSERT(!hdlLink.isValid());
        if (!hdlLink.isValid())
        {
            SetInDeathRow(GetIndexFromLink(hdlLink));
        }
    }

    ChunkSize ChunkContainer::MemSize(const VRAMHandleLinks & hdlLink)
    {
        VRAM_ALLOC_ASSERT(hdlLink.prev == 0);
        return GetChunkEntryFromLink(hdlLink).size;
    }

    void ChunkContainer::PatchVRAMHandleLinks(ux nbEltMax)
    {
        for(ux i = 0; i!=nbEltMax; ++i)
        {
            ChunkEntry & chunk = m_chunks[i];
            if (!chunk.isFree)
            {
                VRAM_ALLOC_ASSERT(chunk.allocLinks.isValid() || IsInDeathRow(i));
                VRAM_ALLOC_ASSERT(chunk.allocLinks.prev==0);
                if (chunk.allocLinks.next) // handle the case of handle links is the death row
                {
                    chunk.allocLinks.next->prev = &(chunk.allocLinks); // patch the first
                }
            }
        }
    }

    const ChunkEntry & ChunkContainer::GetChunkEntryFromLink(const VRAMHandleLinks & link)
    {
        // Here it's a bit hacky as we need to know the index of the block
        const u8 * chunkAddress = ((const u8*)&link) - offsetof(ChunkEntry, freeLink);
        return *((const ChunkEntry *)chunkAddress);
    }

    ChunkIdx ChunkContainer::GetIndexFromLink(const VRAMHandleLinks & link) const
    {
        return ChunkIdx(&(GetChunkEntryFromLink(link)) - &m_chunks[0]);
    }

    ix ChunkContainer::GetBinChunkIndex(ChunkSize size) // return the good bin entry for the block size
    {
        ChunkSize * sizeAddress = std::upper_bound(BinSizes, BinSizes + NumberOfBinSizes, size);
        VRAM_ALLOC_ASSERT(sizeAddress!=BinSizes + NumberOfBinSizes); // we are supposed to always find one
        VRAM_ALLOC_ASSERT(BinSizes[0]==0 && sizeAddress!=BinSizes); // we are never supposed to find the first one as it's 0 (and we are using upper_bound)
        VRAM_ALLOC_ASSERT(ix(sizeAddress - BinSizes) - 1>=0);   // second check
        return ix(sizeAddress - BinSizes) - 1; // minus 1 as we get the ptr with a value succeeding size
    }

    void ChunkContainer::InsertChunkInFreeList(ChunkIdx chunkIdx) // need to be already merged with adjacent blocks
    {
        VRAM_ALLOC_ASSERT(GetChunk(chunkIdx).isFree);
        ChunkSize size = GetChunk(chunkIdx).size;
        ix binIndex = GetBinChunkIndex(size);
            
        // linear search
        ChunkIdx prev = InvalidIndex;
        ChunkIdx current = m_chunkBin[binIndex];
        while(IsValid(current) && GetChunk(current).size < size)
        {
            prev = current;
            current = GetChunk(current).freeLink.next;
        }
        if (!IsValid(prev))
        {
            VRAM_ALLOC_ASSERT(!IsValid(m_chunkBin[binIndex]) || GetChunk(m_chunkBin[binIndex]).size>=size);
            LinkFreeInHead(chunkIdx, m_chunkBin[binIndex]);
            m_chunkBin[binIndex] = chunkIdx;
        }
        else
        {
            LinkFreeAfter(prev, chunkIdx);
        }
    }

    ChunkIdx ChunkContainer::Alloc(ChunkSize size, VramMemoryCategory type)
    {
        VRAM_ALLOC_ASSERT(size!=0);
        MEMMNG_TRACER_ASKEDSIZE_VRAM(size);

        size = Max(size, MinimalChunkSize); // force size to be at least the minimal block size bytes
        size = BitTweak::GetAboveAlign(size, MinimalAlignment);

        ix binIndex = GetBinChunkIndex(size);

        ChunkIdx bestFit;

        // linear search
        do
        {
            bestFit = FindBestChunkInFreeList(m_chunkBin[binIndex], size);
            binIndex++;
        } while(!IsValid(bestFit) && binIndex<NumberOfBinSizes);

        if (!IsValid(bestFit))
        {
            // OUT OF MEM !
            if (m_verboseOutOfMem)
            {
                m_verboseOutOfMem = false; // Just report once
                VRAM_ALLOC_TRACE("**************************************************\n");
                VRAM_ALLOC_TRACE("Out of VRAM : requested size %d bytes\n",size);
                VRAM_ALLOC_TRACE("**************************************************\n");

                PrintInfo();
            }
                
            // Force a break
            if (m_forceOOMBreak)
            {
                // Call mem dump callback ?
                ITF_DEBUG_BREAK();
            }
                    
            return InvalidIndex;
        }

        UnlinkFreeFromList(bestFit, binIndex-1);
        ChunkIdx freeChunk = SplitChunk(bestFit, size, type);
        if (IsValid(freeChunk))
        {
            InsertChunkInFreeList(freeChunk);
            CHUNK_CHECK_VALID_LINKS(freeChunk);
        }
        CHUNK_CHECK_VALID_LINKS(bestFit);
        VRAM_ALLOC_DEC_MEM_COUNT(GetChunk(bestFit).size);
        MEMMNG_TRACER_ALLOC_VRAM(GetAddress(bestFit), GetChunk(bestFit).size);
        VRAM_COUNT_ALLOC(GetAddress(bestFit), GetChunk(bestFit).size);
        return bestFit;
    }

    ChunkIdx ChunkContainer::AllocAligned(ChunkSize size, ChunkSize align, VramMemoryCategory type)
    {
        VRAM_ALLOC_ASSERT(size!=0);
        VRAM_ALLOC_ASSERT(BitTweak::IsPow2(align));
        MEMMNG_TRACER_ASKEDSIZE_VRAM(size);

        // maximized to minimal alignment
        align = Max(align, MinimalAlignment);
        // simpler alloc if minimal alignment
        if (align==MinimalAlignment)
        {
            return Alloc(size, type);
        }
            
        size = Max(size, MinimalChunkSize); // force size to be at least the minimal block size bytes
        size = BitTweak::GetAboveAlign(size, MinimalAlignment);

        ix binIndex = GetBinChunkIndex(size);

        ChunkIdx bestFit;

        // linear search
        do
        {
            bestFit = FindBestChunkInFreeList(m_chunkBin[binIndex], size, align);
            binIndex++;
        } while(!IsValid(bestFit) && binIndex<NumberOfBinSizes);

        if (!IsValid(bestFit))
        {
            // OUT OF MEM !
            if (m_verboseOutOfMem)
            {
                m_verboseOutOfMem = false; // Just report once
                VRAM_ALLOC_TRACE("**************************************************\n");
                VRAM_ALLOC_TRACE("Out of VRAM : requested size %d bytes\n",size);
                VRAM_ALLOC_TRACE("**************************************************\n");
                PrintInfo();
            }
                
            // Force a break
            if (m_forceOOMBreak)
            {
                // Call mem dump callback ?
                ITF_DEBUG_BREAK();
            }
                    
            return InvalidIndex;
        }

        UnlinkFreeFromList(bestFit, binIndex-1);
        // split in 3 parts
        ChunkIdx first, second, third;
        TripleSplitChunk(bestFit, first, second, third, size, align, type);
        if (IsValid(first))
        {
            InsertChunkInFreeList(first);
            CHUNK_CHECK_VALID_LINKS(first);
        }
        if (IsValid(third))
        {
            InsertChunkInFreeList(third);
            CHUNK_CHECK_VALID_LINKS(third);
        }
        CHUNK_CHECK_VALID_LINKS(second);
        VRAM_ALLOC_DEC_MEM_COUNT(GetChunk(second).size);
        MEMMNG_TRACER_ALLOC_VRAM(GetAddress(second), GetChunk(second).size);
        VRAM_COUNT_ALLOC(GetAddress(second), GetChunk(second).size);

        return second;
    }

    void ChunkContainer::Free(ChunkIdx idx)
    {
        CHUNK_CHECK_VALID_LINKS(idx);

        ChunkEntry & chunkToFree = GetChunk(idx);

        MEMMNG_TRACER_FREE_VRAM(GetAddress(idx), chunkToFree.size);

        VRAM_ALLOC_INC_MEM_COUNT(chunkToFree.size);

        VRAM_COUNT_FREE(GetAddress(idx), chunkToFree.size);


        // transform to a free block
        chunkToFree.freeLink.next = InvalidIndex;
        chunkToFree.freeLink.prev = InvalidIndex;
        chunkToFree.isFree = true;
        ChunkIdx hNewFree = MergeFree(idx);
        InsertChunkInFreeList(hNewFree);
    }

    bool ChunkContainer::CanHandleAllocation(ChunkSize _blockSize, ux _alignment)
    {
        // simply alloc and free in row, disabling oom report
        bool verboseOOM = m_verboseOutOfMem;
        bool forcedOOMBreak = m_forceOOMBreak;
            
        m_verboseOutOfMem = bfalse;
        m_forceOOMBreak = bfalse;

        ChunkIdx chunkIdx = AllocAligned(_blockSize, _alignment, VRAM_Unknown);
        bool canHandle = (chunkIdx != InvalidIndex);
        if (canHandle)
        {
            Free(chunkIdx);
        }

        m_verboseOutOfMem = verboseOOM;
        m_forceOOMBreak = forcedOOMBreak;
        return canHandle;
    }

    void ChunkContainer::SetInDeathRow(ChunkIdx idx)
    {
        m_chunksInDeathRow.push_back(ChunkDeathRowEntry(idx, u64(-1)));
    }

    bool ChunkContainer::IsInDeathRow(ChunkIdx idx) const
    {
        for(ChunkDeathRow::const_iterator it = m_chunksInDeathRow.begin(), itEnd = m_chunksInDeathRow.end(); it!=itEnd; ++it)
        {
            if (it->m_chunk == idx)
                return true;
        }
        return false;
    }

    ChunkIdx ChunkContainer::CreateChunk()
    {
        if (!IsValid(m_firstNotUsedEntry))
        {
            ChunkIdx newIdx = ChunkIdx(m_chunks.size());
            ChunkEntry * first = m_chunks.empty() ? nullptr : m_chunks.data();
            m_chunks.resize(newIdx+1);
            if (m_chunks.data() != first) // relocated vector?
            {
                PatchVRAMHandleLinks(newIdx);
            }
            return newIdx;
        }
        // this is free entries linking (ie. elements of the vector that can be reused)
        ChunkIdx newIdx = m_firstNotUsedEntry;
        m_firstNotUsedEntry = GetChunk(m_firstNotUsedEntry).next;
        if (IsValid(m_firstNotUsedEntry))
            GetChunk(m_firstNotUsedEntry).prev = InvalidIndex;
        return newIdx;
    }

    void ChunkContainer::RemoveChunk(ChunkIdx idx)
    {
        VRAM_ALLOC_ASSERT(IsValid(idx));
        ChunkEntry & newUnused = GetChunk(idx);
        newUnused.next = m_firstNotUsedEntry;
        newUnused.prev = InvalidIndex;
        newUnused.isFree = true;
        m_firstNotUsedEntry = idx;
    }

    void ChunkContainer::UnlinkFree(ChunkIdx idx)
    {
        ChunkEntry & tHis = GetChunk(idx);
        if (IsValid(tHis.freeLink.prev))
        {
            GetChunk(tHis.freeLink.prev).freeLink.next = tHis.freeLink.next;
        }
        else
        {
            // we need to change the bin entry as it's the first
            ix binIndex = GetBinChunkIndex(tHis.size);
            m_chunkBin[binIndex] = tHis.freeLink.next;
        }
        if (IsValid(tHis.freeLink.next))
        {
            GetChunk(tHis.freeLink.next).freeLink.prev = tHis.freeLink.prev;
        }
    }

    void ChunkContainer::UnlinkFreeFromList(ChunkIdx idx, ix binIndex) // same as above, but the bin index is known (faster)
    {
        ChunkEntry & tHis = GetChunk(idx);
        if (IsValid(tHis.freeLink.prev))
        {
            GetChunk(tHis.freeLink.prev).freeLink.next = tHis.freeLink.next;
        }
        else
        {
            // we need to change the bin entry as it's the first
            m_chunkBin[binIndex] = tHis.freeLink.next;
        }
        if (IsValid(tHis.freeLink.next))
        {
            GetChunk(tHis.freeLink.next).freeLink.prev = tHis.freeLink.prev;
        }
    }

    void ChunkContainer::LinkFreeAfter(ChunkIdx idx, ChunkIdx newEntryIdx)
    {
        ChunkEntry & tHis = GetChunk(idx);
        ChunkEntry & newEntry = GetChunk(newEntryIdx);
        newEntry.freeLink.prev = idx;
        newEntry.freeLink.next = tHis.freeLink.next;
        tHis.freeLink.next = newEntryIdx;
        if (IsValid(newEntry.freeLink.next))
        {
            GetChunk(newEntry.freeLink.next).freeLink.prev = newEntryIdx;
        }
    }

    void ChunkContainer::LinkFreeInHead(ChunkIdx idx, ChunkIdx oldHeadIdx) // "optimized for head" version of LinkBefore
    {
        ChunkEntry & tHis = GetChunk(idx);
        tHis.freeLink.prev = InvalidIndex;
        tHis.freeLink.next = oldHeadIdx;
        if (IsValid(oldHeadIdx))
        {
            ChunkEntry & oldHead = GetChunk(oldHeadIdx);
            VRAM_ALLOC_ASSERT(!IsValid(oldHead.freeLink.prev));
            oldHead.freeLink.prev = idx;
        }
    }

    // parse a chunk list to find the smallest of at least this size    
    ChunkIdx ChunkContainer::FindBestChunkInFreeList(ChunkIdx start, ChunkSize size, ChunkSize align) const
    {
        ChunkIdx current = start;
        while(IsValid(current))
        {
            const ChunkEntry & chunk = GetChunk(current);
            if (chunk.size >= size &&
                BitTweak::GetAboveAlign(chunk.addr, align) + size <=  chunk.addr + chunk.size) // we can align and stay in the bounds ?
            {
                break;
            }
            current = GetChunk(current).freeLink.next;
        }
        return current;
    }

    ChunkIdx ChunkContainer::FindBestChunkInFreeList(ChunkIdx start, ChunkSize size) const
    {
        ChunkIdx current = start;
        while(IsValid(current) && GetChunk(current).size < size)
        {
            current = GetChunk(current).freeLink.next;
        }
        return current;
    }

    // return the free chunk or invalid if not exist
    ChunkIdx ChunkContainer::SplitChunk(ChunkIdx hChunkToSplit, ChunkSize size, VramMemoryCategory type)
    {
        // split only if the remaining free block is big enough
        VRAM_ALLOC_ASSERT(GetChunk(hChunkToSplit).size>=size);

        ChunkIdx hNewFreeChunk = InvalidIndex;

        if (GetChunk(hChunkToSplit).size-size >= MinimalChunkSize)
        {
            hNewFreeChunk = CreateChunk();
            ChunkIdx hNewAllocChunk = hChunkToSplit;

            ChunkEntry & newFreeChunk = GetChunk(hNewFreeChunk);
            ChunkEntry & newAllocChunk = GetChunk(hNewAllocChunk);

            newFreeChunk.size = newAllocChunk.size - size;
            newFreeChunk.addr = VRAMAddress((char *)(newAllocChunk.addr)+size);
            newFreeChunk.next = newAllocChunk.next;
            newFreeChunk.prev = hNewAllocChunk;
            newFreeChunk.isFree = true;

            newAllocChunk.size = size;

            // link correctly (chunks links)
            if (IsValid(newAllocChunk.next))
            {
                GetChunk(newAllocChunk.next).prev = hNewFreeChunk;
            }
            newAllocChunk.next = hNewFreeChunk;
        }

        // set free links to "InvalidFree"
        ChunkEntry & newAllocChunk = GetChunk(hChunkToSplit);
        newAllocChunk.allocLinks.next = 0;
        newAllocChunk.allocLinks.prev = 0;
        newAllocChunk.isFree = false;
        newAllocChunk.type = type;
        return hNewFreeChunk;
    }

    void ChunkContainer::TripleSplitChunk(ChunkIdx hChunkToSplit, ChunkIdx & hFirst, ChunkIdx & hSecond, ChunkIdx & hThird, ChunkSize size, ChunkSize align, VramMemoryCategory type)
    {
        // split only if the remaining free block is big enough
        VRAM_ALLOC_ASSERT(GetChunk(hChunkToSplit).size>=size);
        VRAM_ALLOC_ASSERT(BitTweak::GetAboveAlign(GetChunk(hChunkToSplit).addr, align) + size <=  GetChunk(hChunkToSplit).addr + GetChunk(hChunkToSplit).size); // we got place for aligned block ?

        hFirst = InvalidIndex;
        hSecond = hChunkToSplit;
        hThird = InvalidIndex;

        VRAMAddress addrSecond = BitTweak::GetAboveAlign(GetChunk(hSecond).addr, align);
        VRAMAddress addrThird = addrSecond + size;
        VRAMAddress addrNext = GetChunk(hChunkToSplit).addr + GetChunk(hChunkToSplit).size;
        VRAM_ALLOC_ASSERT(!IsValid(GetChunk(hChunkToSplit).next) || GetChunk(GetChunk(hChunkToSplit).next).addr==addrNext);

        if (addrSecond != GetChunk(hSecond).addr) // there is a first ?
        {
            hFirst = CreateChunk();

            ChunkEntry & firstChunk = GetChunk(hFirst);
            ChunkEntry & secondChunk = GetChunk(hSecond);

            firstChunk.size = ChunkSize(addrSecond - secondChunk.addr);
            firstChunk.addr = secondChunk.addr;
            firstChunk.next = hSecond;
            firstChunk.prev = secondChunk.prev;
            firstChunk.isFree = true;

            // link correctly (chunks links)
            if (IsValid(secondChunk.prev))
            {
                GetChunk(secondChunk.prev).next = hFirst;
            }
            secondChunk.prev = hFirst;
        }

        if (addrNext - addrThird >= MinimalChunkSize)
        {
            hThird = CreateChunk();

            ChunkEntry & thirdChunk = GetChunk(hThird);
            ChunkEntry & secondChunk = GetChunk(hSecond);

            thirdChunk.size = ChunkSize(addrNext - addrThird);
            thirdChunk.addr = addrThird;
            thirdChunk.next = secondChunk.next;
            thirdChunk.prev = hSecond;
            thirdChunk.isFree = true;

            // link correctly (chunks links)
            if (IsValid(secondChunk.next))
            {
                GetChunk(secondChunk.next).prev = hThird;
            }
            secondChunk.next = hThird;

            secondChunk.size = size;
        }
        else
        {
            GetChunk(hSecond).size = ChunkSize(addrNext - addrSecond);
            VRAM_ALLOC_ASSERT(size + (addrNext - addrThird) == GetChunk(hSecond).size);
        }

        ChunkEntry & newAllocChunk = GetChunk(hSecond);
        newAllocChunk.addr = addrSecond;
        newAllocChunk.allocLinks.next = 0;
        newAllocChunk.allocLinks.prev = 0;
        newAllocChunk.isFree = false;
        newAllocChunk.type = type;
    }

    ChunkIdx ChunkContainer::MergeFree(ChunkIdx hNewFreeChunk)
    {
        ChunkEntry & newFreeChunk = GetChunk(hNewFreeChunk);
            
        // 4 cases -> FFF FFA AFF and AFA
        if (IsValid(newFreeChunk.prev) && IsFreeChunk(newFreeChunk.prev))
        {
            // merge with previous
            ChunkIdx hPrevChunk = newFreeChunk.prev;
            UnlinkFree(hPrevChunk); // we are going to merge it, so definitely remove it from the free list
            ChunkEntry & prevChunk = GetChunk(hPrevChunk);
            newFreeChunk.prev = prevChunk.prev;
            if (IsValid(newFreeChunk.prev))
            {
                VRAM_ALLOC_ASSERT(GetChunk(newFreeChunk.prev).next == hPrevChunk);
                GetChunk(newFreeChunk.prev).next = hNewFreeChunk;
            }
            newFreeChunk.size += prevChunk.size;
            newFreeChunk.addr = prevChunk.addr;
            RemoveChunk(hPrevChunk);
        }
        if (IsValid(newFreeChunk.next) && IsFreeChunk(newFreeChunk.next))
        {
            // merge with previous
            ChunkIdx hNextChunk = newFreeChunk.next;
            UnlinkFree(hNextChunk);
            ChunkEntry & nextChunk = GetChunk(hNextChunk);
            newFreeChunk.next = nextChunk.next;
            if (IsValid(newFreeChunk.next))
            {
                VRAM_ALLOC_ASSERT(GetChunk(newFreeChunk.next).prev == hNextChunk);
                GetChunk(newFreeChunk.next).prev = hNewFreeChunk;
            }
            newFreeChunk.size += nextChunk.size;
            RemoveChunk(hNextChunk);
        }
        return hNewFreeChunk;
    }

    void ChunkContainer::CheckBlockLinks(ChunkIdx idx) const
    {
#ifdef VRAM_ALLOC_ASSERT_ENABLED
        const ChunkEntry & chunk = GetChunk(idx);
        VRAM_ALLOC_ASSERT(chunk.size>=MinimalChunkSize);
        // check mem block linking
        if (IsValid(chunk.prev))
        {
            const ChunkEntry & chunkPrev = GetChunk(chunk.prev);
            VRAM_ALLOC_ASSERT(((char *)chunkPrev.addr) + chunkPrev.size == (char *)chunk.addr);
            VRAM_ALLOC_ASSERT(chunkPrev.next == idx);
        }
        if (IsValid(chunk.next))
        {
            const ChunkEntry & chunkNext = GetChunk(chunk.next);
            VRAM_ALLOC_ASSERT(((char *)chunk.addr) + chunk.size == (char *)chunkNext.addr);
            VRAM_ALLOC_ASSERT(chunkNext.prev == idx);
        }
        // check free links
        if (chunk.isFree)
        {
            // no contiguous free blocks
            VRAM_ALLOC_ASSERT(!IsValid(chunk.next) || !GetChunk(chunk.next).isFree);
            VRAM_ALLOC_ASSERT(!IsValid(chunk.prev) || !GetChunk(chunk.prev).isFree);
            // incremental size in free links
            VRAM_ALLOC_ASSERT(!IsValid(chunk.freeLink.next) || GetChunk(chunk.freeLink.next).size>=chunk.size);
            VRAM_ALLOC_ASSERT(!IsValid(chunk.freeLink.prev) || GetChunk(chunk.freeLink.prev).size<=chunk.size);
        }
#endif
    }

    void ChunkContainer::Update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bool bForceErase)
    {
        for(ChunkDeathRow::iterator it = m_chunksInDeathRow.begin();
            it!=m_chunksInDeathRow.end();
            )
        {
            if (it->m_frameCount <= gpuLastFrameCount || bForceErase)
            {
                Free(it->m_chunk);
                it = m_chunksInDeathRow.erase(it);
            }
            else
            {
                if (it->m_frameCount == u64(-1))
                {
                    it->m_frameCount = cpuCurrentFrame;
                }
                ++it;
            }
        }
    }

    void ChunkContainer::GetStats(ChunkSize & freeSizeLeft, ChunkSize & biggestFreeBlockSize, ChunkSize & initialSize)
    {
        initialSize = m_initialSize;

#if !defined(ITF_DISABLE_DEBUGINFO)
        // Get the biggest free block
        ChunkIdx biggestFreeBlock = InvalidIndex;
        for(ix i = NumberOfBinSizes-1; i>=0; --i)
        {
            if (m_chunkBin[i]!=InvalidIndex)
            {
                biggestFreeBlock = m_chunkBin[i];
                break;
            }
        }
        biggestFreeBlockSize = biggestFreeBlock!=InvalidIndex?GetChunk(biggestFreeBlock).size:0;
#else
        biggestFreeBlockSize = 0;
#endif

#ifdef VRAM_ALLOC_COUNT
        freeSizeLeft = m_totalFreeSize;
#else
        freeSizeLeft = m_initialSize; // report as empty
#endif
    }

    // This method is a bit complex and should be used only in debug
    // It return the first block of memory
    ChunkIdx ChunkContainer::GetFirstChunk() const
    {
        for(std::vector<ChunkEntry>::const_iterator it = m_chunks.begin(), itEnd = m_chunks.end();
            it!=itEnd;
            ++it)
        {
            if (it->addr == m_startAddr)
            {
                ChunkIdx foundChunk = ChunkIdx(std::distance(m_chunks.begin(), it));
                // verify it's not in the unused chunk list
                ChunkIdx unusedChunk = m_firstNotUsedEntry;
                VRAM_ALLOC_ASSERT(!IsValid(unusedChunk) || !IsValid(GetChunk(unusedChunk).prev));
                while (IsValid(unusedChunk) && unusedChunk!=foundChunk)
                {
                    unusedChunk = GetChunk(unusedChunk).next;
                };
                if (!IsValid(unusedChunk))
                {   // not unused -> return it
                    return foundChunk;
                }
            }
        }
        VRAM_ALLOC_ASSERT(0); // no first block found !?
        return InvalidIndex;
    }

    void ChunkContainer::PrintInfo()
    {
    #ifdef VRAM_ALLOC_TRACE_ENABLED
        VRAM_ALLOC_TRACE("********************************************************\n");
        VRAM_ALLOC_TRACE("VRAM Mapping Report\n");

        // Get the biggest free block
        ChunkIdx biggestFreeBlock = InvalidIndex;
        for(ix i = NumberOfBinSizes-1; i>=0; --i)
        {
            if (m_chunkBin[i]!=InvalidIndex)
            {
                biggestFreeBlock = m_chunkBin[i];
                break;
            }
        }
            
        // Get the smallest free block
        ChunkIdx smallestFreeBlock = InvalidIndex;
        for(ux i = 0; i<NumberOfBinSizes; ++i)
        {
            if (m_chunkBin[i]!=InvalidIndex)
            {
                smallestFreeBlock = m_chunkBin[i];
                break;
            }
        }

        ChunkSize smallestSize = smallestFreeBlock!=InvalidIndex?GetChunk(smallestFreeBlock).size:0;
        ChunkSize biggestSize = biggestFreeBlock!=InvalidIndex?GetChunk(biggestFreeBlock).size:0;

        #define VRAM_KILO_BYTE(size)    (float(size)/1024.f)
        #define VRAM_MEGA_BYTE(size)    (VRAM_KILO_BYTE(size)/1024.f)

        #ifdef VRAM_ALLOC_COUNT
        VRAM_ALLOC_TRACE("Total Free Size = %d B = %.2fKB = %.2f MB\n", m_totalFreeSize,  VRAM_KILO_BYTE(m_totalFreeSize), VRAM_MEGA_BYTE(m_totalFreeSize));
        #endif
        VRAM_ALLOC_TRACE("Biggest free block size = %d B = %.2f KB = %.2f MB (block %d)\n", biggestSize,  VRAM_KILO_BYTE(biggestSize), VRAM_MEGA_BYTE(biggestSize), biggestFreeBlock);
        VRAM_ALLOC_TRACE("Smallest free block size = %d B = %.2f KB = %.2f MB (block %d)\n", smallestSize,  VRAM_KILO_BYTE(smallestSize), VRAM_MEGA_BYTE(smallestSize), smallestFreeBlock);

        // draw the mapping
        ux nbRow = 80;
        ChunkSize granularity = 64*1024;

        VRAM_ALLOC_TRACE("R = RenderTarget, D = DepthBuffer, T = Texture, V = VertexBuffer, I = IndexBuffer\n");
        VRAM_ALLOC_TRACE("U = unknown, S = Shader, O = Volatile\n");
        struct BlockName
        {
            char acronym;
            const char * fullName;
        };
        static BlockName blockNames[] =
        {
            { 'U', "Unknown" },
            { 'R', "RenderTarget"},
            { 'D', "DepthBuffer"},
            { 'T', "Texture"},
            { 'V', "VertexBuffer"},
            { 'I', "IndexBuffer"},
            { 'S', "Shader"},
            { 'O', "Volatile"}
        };

        static ChunkSize blockSize[VRAM_Last];
        std::fill(blockSize, blockSize+VRAM_Last, 0);
            
        // Get the first chunk
        ChunkIdx currentChunk = GetFirstChunk();
        if (currentChunk == InvalidIndex)
        {
            VRAM_ALLOC_TRACE("Internal Error : no block starting at the start address\n");
        }

        ChunkSize allocatedSize = 0;
        ChunkSize freeSize = 0;
        ux currentRow = 0;
        ChunkSize currentAddressOffset = 0;
        VRAM_ALLOC_TRACE("0x00000000: ");
        // ok, loop to print the mapping
        while (currentChunk != InvalidIndex)
        {
            ChunkEntry & chunk = GetChunk(currentChunk);
            char blockName;
            if (chunk.isFree)
            {
                freeSize += chunk.size;
                blockName = '.';
            }
            else
            {
                allocatedSize += chunk.size;
                blockName = blockNames[chunk.type].acronym;
                blockSize[chunk.type] += chunk.size;
            }

            ChunkSize distToNext = ChunkSize(BitTweak::GetAboveAlign(currentAddressOffset, granularity) - currentAddressOffset);
            ChunkSize curSize = chunk.size;
            if (curSize>=distToNext)
            {
                if (currentRow==nbRow)
                {
                    VRAM_ALLOC_TRACE("\n");
                    VRAM_ALLOC_TRACE("0x%08X: ", BitTweak::GetAboveAlign(currentAddressOffset, granularity));
                    currentRow = 0;
                }

                VRAM_ALLOC_TRACE("%c", blockName);
                currentRow++;
                curSize -= distToNext;
            }
                
            for(ChunkSize i = 0; i<curSize/granularity; ++i)
            {
                if (currentRow==nbRow)
                {
                    VRAM_ALLOC_TRACE("\n");
                    VRAM_ALLOC_TRACE("0x%08X: ", currentAddressOffset + i*granularity);
                    currentRow = 0;
                }

                VRAM_ALLOC_TRACE("%c", blockName);
                currentRow++;
            }

            currentAddressOffset += chunk.size;

            // go to next chunk
            currentChunk = chunk.next;
        }
        VRAM_ALLOC_TRACE("\n");
        if (allocatedSize+freeSize != m_initialSize)
        {
            VRAM_ALLOC_TRACE("Internal Error : %d B found in allocated blocks, %dB found in free blocks, %d B available at startup (memory lost ?)\n",
                    allocatedSize, freeSize, m_initialSize);
        }
        #ifdef VRAM_ALLOC_COUNT
        if (freeSize != m_totalFreeSize)
        {
            VRAM_ALLOC_TRACE("Internal Error : %d B of free mem in alloc time counter, while a total of %d B available in free blocks\n",
                    m_totalFreeSize, freeSize);
        }
        #endif
        for(ux i=0; i<VRAM_Last; i++)
        {
            VRAM_ALLOC_TRACE("%s = %d B = %.2f KB = %.2f MB\n", blockNames[i].fullName, blockSize[i], VRAM_KILO_BYTE(blockSize[i]), VRAM_MEGA_BYTE(blockSize[i]));
        }
        VRAM_ALLOC_TRACE("End of VRAM Mapping Report\n");
        VRAM_ALLOC_TRACE("********************************************************\n");
    #endif // VRAM_ALLOC_TRACE_ENABLED
    }
} // ITF::DX12::VRAMAllocator
