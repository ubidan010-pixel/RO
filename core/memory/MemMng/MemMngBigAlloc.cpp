#include "precompiled_core.h"

#include <unordered_map>

#include "core/math/BitTweak.h"
#include "core/memory/STDAllocatorOnMalloc.h"
#include "MemMngPrivate.h"
#include "MemMngPage.h"
#include "MemMngBigAlloc.h"

#if defined(CHECK_BIGALLOC_POLICY) && CHECK_BIGALLOC_POLICY>0
#define MEMMNG_BIGCHUNKALLOC_STATS
#endif

// Chunk allocator for big and temporary allocations (based on the VRAMAllocator from Orbis).
// beware of reentrance -> unordered_map elements are allocated, so they should not be allocated there

#define MEMMNG_BIGCHUNKALLOC_ASSERT		MEMMNG_ASSERT
#define MEMMNG_BIGCHUNKALLOC_TRACE(...)	((void)0)

#ifdef MEMMNG_BIGCHUNKALLOC_STATS
    #define MEMMNG_BIGCHUNKALLOC_STATS_CREATE()	(++m_stat_totalChunks)
    #define MEMMNG_BIGCHUNKALLOC_STATS_REMOVE()	(--m_stat_totalChunks)
    #define MEMMNG_BIGCHUNKALLOC_STATS_ALLOC(size)	(++m_stat_numAllocatedChunks, m_stat_totalPeakSize = Max(m_stat_totalUsedSize, m_stat_totalPeakSize), m_stat_totalUsedSize += (u32)(size))
    #define MEMMNG_BIGCHUNKALLOC_STATS_FREE(size)	(--m_stat_numAllocatedChunks, m_stat_totalUsedSize -= u32(size))
#else
    #define MEMMNG_BIGCHUNKALLOC_STATS_CREATE()	((void)0)
    #define MEMMNG_BIGCHUNKALLOC_STATS_REMOVE()	((void)0)
    #if !defined(ITF_FINAL)
        #define MEMMNG_BIGCHUNKALLOC_STATS_ALLOC(size)	(m_stat_totalUsedSize += (u32)(size), m_stat_totalPeakSize = Max(m_stat_totalUsedSize, m_stat_totalPeakSize))
        #define MEMMNG_BIGCHUNKALLOC_STATS_FREE(size)	(m_stat_totalUsedSize -= (u32)(size))
    #else
        #define MEMMNG_BIGCHUNKALLOC_STATS_ALLOC(size)	((void)0)
        #define MEMMNG_BIGCHUNKALLOC_STATS_FREE(size)	((void)0)
    #endif
#endif

// link checks
#if defined(CHECK_BIGALLOC_POLICY) && CHECK_BIGALLOC_POLICY>0
#define CHECK_VALID_LINKS CheckBlockLinks
#else
#define CHECK_VALID_LINKS(...)
#endif


namespace ITF::MemMngBigAlloc
{
    namespace ChunkAllocator
    {
        typedef u8* MemAddress;
        typedef u32 ChunkIdx;
        typedef u64 ChunkSize;

        enum MemoryType
        {
            MemType_Big_Unknown = 0,
            MemType_Big_Last
        };

        enum
        {
            DefinedNumberOfBinSizes = 78  // must match NumberOfBinSizes in the .cpp
        };

        // minimal block size
        const ChunkSize MinimalChunkSize = 64*1024u;
        const ChunkSize MinimalAlignment = 64*1024u; // has to be a multiple of MinimalChunkSize

        struct ChunkEntry
        {
        public:
            MemAddress addr;
            ChunkSize size;
            u32 type; // only 8 types available
            u32 isFree;
            // linear linked list
            ChunkIdx   prev;
            ChunkIdx   next;
            // free list (used when in bin array)
            struct
            {
                ChunkIdx   prev;
                ChunkIdx   next;
            } freeLink;                 // free chunk -> double list 
        };

        //////////////////////////////////////////////////////////////////////////
        // chunk container
        //////////////////////////////////////////////////////////////////////////

        class ChunkContainer
        {
        public:

            ChunkContainer();
            ~ChunkContainer();

            void Init(MemAddress startOfBlock, ChunkSize blockSize, bool verboseOutOfMem = false);

            MemAddress GetStartAddress() const { return m_startAddr; }
            ChunkSize GetInitialSize() const { return m_initialSize; }

            MemAddress GetAddress(ChunkIdx idx) const { return GetChunk(idx).addr; }

            ChunkSize GetChunkSize(ChunkIdx idx) const { return GetChunk(idx).size; }

            bool IsAddressInRange(MemAddress addr) const { return (addr - m_startAddr) < m_initialSize; }
            ChunkIdx FindChunkFromAddress(MemAddress addr);

            ChunkIdx Alloc(ChunkSize size, MemoryType type);
            ChunkIdx AllocAligned(ChunkSize size, ChunkSize align, MemoryType type);

            void Free(ChunkIdx idx);

            bool IsEmpty() const;

            void PrintInfo();

            ChunkSize GetTotalSize()  { return m_stat_totalUsedSize; }
            ChunkSize GetTotalPeakSize()  { return m_stat_totalPeakSize; }

            const static ChunkIdx InvalidIndex = (ChunkIdx)-1;

        private:

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

            void TripleSplitChunk(ChunkIdx hChunkToSplit, ChunkIdx & hFirst, ChunkIdx & hSecond, ChunkIdx & hThird, ChunkSize size, ChunkSize align, MemoryType type);
            ChunkIdx SplitChunk(ChunkIdx bestFit, ChunkSize size, MemoryType type); // return the free chunk or invalid if not exist
            ChunkIdx MergeFree(ChunkIdx idx);

            // We use STDAllocatorOnMalloc to avoid reentrancy. This has to be further analyzed.
            using HashAllocator = STDAllocatorOnMalloc<std::pair<const MemAddress, ChunkIdx>>;
            using HashMap = std::unordered_map<MemAddress, ChunkIdx, std::hash<MemAddress>, std::equal_to<MemAddress>, HashAllocator>;
            HashMap m_addr2AllocatedChunk;  // map of addresses to the chunk

            ITF_VECTOR<ChunkEntry> m_chunks; // list of all chunks (not in memory order)
            ChunkIdx m_chunkBin[DefinedNumberOfBinSizes]; // list of all free chunks sorted by size
            ChunkIdx m_firstNotUsedEntry; // list of all index in m_chunks not used

            ChunkSize m_initialSize;
            MemAddress m_startAddr;

            ChunkSize m_stat_totalUsedSize;
            ChunkSize m_stat_totalPeakSize;
            ChunkSize m_stat_numAllocatedChunks;
            ChunkSize m_stat_totalChunks;
        };
        
        ChunkSize BinSizes[] =
        {
            0, // keep 0 here
            64*1024,
            128*1024,
            192*1024,
            256*1024,
            320*1024,
            384*1024,
            448*1024,
            512*1024,
            576*1024,
            640*1024,
            704*1024,
            768*1024,
            832*1024,
            896*1024,
            960*1024,
            1024*1024,
            1088*1024,
            1152*1024,
            1216*1024,
            1280*1024,
            1344*1024,
            1408*1024,
            1472*1024,
            1536*1024,
            1600*1024,
            1664*1024,
            1728*1024,
            1792*1024,
            1856*1024,
            1920*1024,
            1984*1024,
            2048*1024,
            2112*1024,
            2176*1024,
            2240*1024,
            2304*1024,
            2368*1024,
            2432*1024,
            2496*1024,
            2560*1024,
            2624*1024,
            2688*1024,
            2752*1024,
            2816*1024,
            2880*1024,
            2944*1024,
            3008*1024,
            3072*1024,
            3136*1024,
            3200*1024,
            3264*1024,
            3328*1024,
            3392*1024,
            3456*1024,
            3520*1024,
            3584*1024,
            3648*1024,
            3712*1024,
            3776*1024,
            3840*1024,
            3904*1024,
            3968*1024,
            4032*1024,
            4096*1024,
            4160*1024,
            4224*1024,
            4288*1024,
            4352*1024,
            4416*1024,
            4480*1024,
            4544*1024,
            4608*1024,
            4672*1024,
            4736*1024,
            4800*1024,
            4864*1024,
            uPtr(-1) // keep uPtr(-1) here (to avoid a special "last" case)
        };

        const ux NumberOfBinSizes = sizeof(BinSizes)/sizeof(BinSizes[0]);

        ChunkContainer::ChunkContainer()
            : m_firstNotUsedEntry(0)
            , m_initialSize(0)
            , m_startAddr(0)
            , m_stat_totalUsedSize(0)
            , m_stat_totalPeakSize(0)
            , m_stat_totalChunks(0)
            , m_stat_numAllocatedChunks(0)
        {
            ITF_COMPILE_TIME_ASSERT(DefinedNumberOfBinSizes_Not_Coherent, NumberOfBinSizes == DefinedNumberOfBinSizes);
        }

        ChunkContainer::~ChunkContainer()
        {
        }

        void ChunkContainer::Init(MemAddress startOfBlock, ChunkSize blockSize, bool verboseOutOfMem)
        {
            for(ux i = 0; i<NumberOfBinSizes; ++i)
            {
                m_chunkBin[i] = ChunkContainer::InvalidIndex;
            }

            m_firstNotUsedEntry = InvalidIndex;
            m_initialSize = blockSize;
            m_startAddr = startOfBlock;

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

        bool ChunkContainer::IsEmpty() const
        {
            if (m_chunks.empty())
            {
                return true;
            }
            if (m_chunks.size() == 1)
            {
                return m_chunks.front().isFree;
            }
            return false;
        }

        int ChunkContainer::GetBinChunkIndex(ChunkSize size) // return the good bin entry for the block size
        {
            ChunkSize * sizeAddress = std::upper_bound(BinSizes, BinSizes + NumberOfBinSizes, size);
            MEMMNG_BIGCHUNKALLOC_ASSERT(sizeAddress!=BinSizes + NumberOfBinSizes); // we are supposed to always find one
            MEMMNG_BIGCHUNKALLOC_ASSERT(BinSizes[0]==0 && sizeAddress!=BinSizes); // we are never supposed to find the first one as it's 0 (and we are using upper_bound)
            MEMMNG_BIGCHUNKALLOC_ASSERT(int(sizeAddress - BinSizes) - 1>=0);   // second check
            return int(sizeAddress - BinSizes) - 1; // minus 1 as we get the ptr with a value succeeding size
        }

        void ChunkContainer::InsertChunkInFreeList(ChunkIdx chunkIdx) // need to be already merged with adjacent blocks
        {
            MEMMNG_BIGCHUNKALLOC_ASSERT(GetChunk(chunkIdx).isFree);
            ChunkSize size = GetChunk(chunkIdx).size;
            int binIndex = GetBinChunkIndex(size);

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
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(m_chunkBin[binIndex]) || GetChunk(m_chunkBin[binIndex]).size>=size);
                LinkFreeInHead(chunkIdx, m_chunkBin[binIndex]);
                m_chunkBin[binIndex] = chunkIdx;
            }
            else
            {
                LinkFreeAfter(prev, chunkIdx);
            }
        }

        ChunkIdx ChunkContainer::FindChunkFromAddress(MemAddress addr)
        {
            auto it = m_addr2AllocatedChunk.find(addr);
            if (it == m_addr2AllocatedChunk.end())
            {
                return InvalidIndex;
            }
            else
            {
                return it->second;
            }
        }

        ChunkIdx ChunkContainer::Alloc(ChunkSize size, MemoryType type)
        {
            MEMMNG_BIGCHUNKALLOC_ASSERT(size!=0);
            size = Max(size, MinimalChunkSize); // force size to be at least the minimal block size bytes
            size = BitTweak::GetAboveAlign(size, MinimalAlignment);

            int binIndex = GetBinChunkIndex(size);

            ChunkIdx bestFit;

            // linear search
            do
            {
                bestFit = FindBestChunkInFreeList(m_chunkBin[binIndex], size);
                binIndex++;
            } while(!IsValid(bestFit) && binIndex<NumberOfBinSizes);

            if (!IsValid(bestFit))
            {
                // No available chunks!

                return InvalidIndex;
            }

            UnlinkFreeFromList(bestFit, binIndex-1);
            ChunkIdx freeChunk = SplitChunk(bestFit, size, type);
            if (IsValid(freeChunk))
            {
                InsertChunkInFreeList(freeChunk);
                CHECK_VALID_LINKS(freeChunk);
            }
            CHECK_VALID_LINKS(bestFit);

            m_addr2AllocatedChunk.insert(std::pair<MemAddress, ChunkIdx>(GetChunk(bestFit).addr, bestFit));
            MEMMNG_BIGCHUNKALLOC_STATS_ALLOC(GetChunk(bestFit).size);
            return bestFit;
        }

        ChunkIdx ChunkContainer::AllocAligned(ChunkSize size, ChunkSize align, MemoryType type)
        {
            MEMMNG_BIGCHUNKALLOC_ASSERT(size!=0);
            MEMMNG_BIGCHUNKALLOC_ASSERT(BitTweak::IsPow2(align));
            // maximized to minimal alignment
            align = Max(align, MinimalAlignment);
            // simpler alloc if minimal alignment
            if (align==MinimalAlignment)
            {
                return Alloc(size, type);
            }

            size = Max(size, MinimalChunkSize); // force size to be at least the minimal block size bytes
            size = BitTweak::GetAboveAlign(size, MinimalAlignment);

            int binIndex = GetBinChunkIndex(size);

            ChunkIdx bestFit;

            // linear search
            do
            {
                bestFit = FindBestChunkInFreeList(m_chunkBin[binIndex], size, align);
                binIndex++;
            } while(!IsValid(bestFit) && binIndex<NumberOfBinSizes);

            if (!IsValid(bestFit))
            {
                // No available chunks!

                return InvalidIndex;
            }

            UnlinkFreeFromList(bestFit, binIndex-1);
            // split in 3 parts
            ChunkIdx first, second, third;
            TripleSplitChunk(bestFit, first, second, third, size, align, type);
            if (IsValid(first))
            {
                InsertChunkInFreeList(first);
                CHECK_VALID_LINKS(first);
            }
            if (IsValid(third))
            {
                InsertChunkInFreeList(third);
                CHECK_VALID_LINKS(third);
            }
            CHECK_VALID_LINKS(second);
            m_addr2AllocatedChunk.insert(std::pair<MemAddress, ChunkIdx>(GetChunk(bestFit).addr, bestFit));
            MEMMNG_BIGCHUNKALLOC_STATS_ALLOC(GetChunk(second).size);
            return second;
        }

        void ChunkContainer::Free(ChunkIdx idx)
        {
            CHECK_VALID_LINKS(idx);

            //MEMMNG_TRACER_FREE_PHYSCHUNK(GetAddress(idx));

            ChunkEntry & chunkToFree = GetChunk(idx);

            MEMMNG_BIGCHUNKALLOC_STATS_FREE(chunkToFree.size);

            m_addr2AllocatedChunk.erase(GetAddress(idx));

            // transform to a free block
            chunkToFree.freeLink.next = InvalidIndex;
            chunkToFree.freeLink.prev = InvalidIndex;
            chunkToFree.isFree = true;
            ChunkIdx hNewFree = MergeFree(idx);
            InsertChunkInFreeList(hNewFree);
        }

        ChunkIdx ChunkContainer::CreateChunk()
        {
            MEMMNG_BIGCHUNKALLOC_STATS_CREATE();

            if (!IsValid(m_firstNotUsedEntry))
            {
                ChunkIdx newIdx = ChunkIdx(m_chunks.size());
                m_chunks.resize(m_chunks.size()+1);
                return newIdx;
            }
            // this is free entries linking (forward only)
            ChunkIdx newIdx = m_firstNotUsedEntry;
            m_firstNotUsedEntry = GetChunk(m_firstNotUsedEntry).next;
            return newIdx;
        }

        void ChunkContainer::RemoveChunk(ChunkIdx idx)
        {
            MEMMNG_BIGCHUNKALLOC_ASSERT(IsValid(idx));
            MEMMNG_BIGCHUNKALLOC_STATS_REMOVE();
            GetChunk(idx).next = m_firstNotUsedEntry;
            GetChunk(idx).isFree = true;
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
                int binIndex = GetBinChunkIndex(tHis.size);
                m_chunkBin[binIndex] = tHis.freeLink.next;
            }
            if (IsValid(tHis.freeLink.next))
            {
                GetChunk(tHis.freeLink.next).freeLink.prev = tHis.freeLink.prev;
            }
        }

        void ChunkContainer::UnlinkFreeFromList(ChunkIdx idx, int binIndex) // same as above, but the bin index is known (faster)
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
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(oldHead.freeLink.prev));
                oldHead.freeLink.prev = idx;
            }
        }

        // parse a chunk list to find the smallest of at least this size    
        ChunkIdx ChunkContainer::FindBestChunkInFreeList(ChunkIdx start, ChunkSize size, ChunkSize align)
        {
            ChunkIdx current = start;
            while(IsValid(current))
            {
                ChunkEntry & chunk = GetChunk(current);
                if (chunk.size >= size &&
                    BitTweak::GetAboveAlign(((ChunkSize)chunk.addr), align) + size <=  ((ChunkSize)chunk.addr) + chunk.size) // we can align and stay in the bounds ?
                {
                    break;
                }
                current = GetChunk(current).freeLink.next;
            }
            return current;
        }

        ChunkIdx ChunkContainer::FindBestChunkInFreeList(ChunkIdx start, ChunkSize size)
        {
            ChunkIdx current = start;
            while(IsValid(current) && GetChunk(current).size < size)
            {
                current = GetChunk(current).freeLink.next;
            }
            return current;
        }

        // return the free chunk or invalid if not exist
        ChunkIdx ChunkContainer::SplitChunk(ChunkIdx hChunkToSplit, ChunkSize size, MemoryType type)
        {
            // split only if the remaining free block is big enough
            MEMMNG_BIGCHUNKALLOC_ASSERT(GetChunk(hChunkToSplit).size>=size);

            ChunkIdx hNewFreeChunk = InvalidIndex;

            if (GetChunk(hChunkToSplit).size-size >= MinimalChunkSize)
            {
                hNewFreeChunk = CreateChunk();
                ChunkIdx hNewAllocChunk = hChunkToSplit;

                ChunkEntry & newFreeChunk = GetChunk(hNewFreeChunk);
                ChunkEntry & newAllocChunk = GetChunk(hNewAllocChunk);

                newFreeChunk.size = newAllocChunk.size - size;
                newFreeChunk.addr = MemAddress((char *)(newAllocChunk.addr)+size);
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

            newAllocChunk.isFree = false;
            newAllocChunk.type = type;
            return hNewFreeChunk;
        }

        void ChunkContainer::TripleSplitChunk(ChunkIdx hChunkToSplit, ChunkIdx & hFirst, ChunkIdx & hSecond, ChunkIdx & hThird, ChunkSize size, ChunkSize align, MemoryType type)
        {
            // split only if the remaining free block is big enough
            MEMMNG_BIGCHUNKALLOC_ASSERT(GetChunk(hChunkToSplit).size>=size);
            MEMMNG_BIGCHUNKALLOC_ASSERT(BitTweak::GetAboveAlign((ChunkSize)GetChunk(hChunkToSplit).addr, align) + size <=  (ChunkSize)GetChunk(hChunkToSplit).addr + GetChunk(hChunkToSplit).size); // we got place for aligned block ?

            hFirst = InvalidIndex;
            hSecond = hChunkToSplit;
            hThird = InvalidIndex;

            MemAddress addrSecond = (MemAddress)BitTweak::GetAboveAlign((ChunkSize)GetChunk(hSecond).addr, align);
            MemAddress addrThird = (MemAddress)((ChunkSize)addrSecond + size);
            MemAddress addrNext = GetChunk(hChunkToSplit).addr + GetChunk(hChunkToSplit).size;
            MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(GetChunk(hChunkToSplit).next) || GetChunk(GetChunk(hChunkToSplit).next).addr==addrNext);

            if (addrSecond != GetChunk(hSecond).addr) // there is a first ?
            {
                hFirst = CreateChunk();

                ChunkEntry & firstChunk = GetChunk(hFirst);
                ChunkEntry & secondChunk = GetChunk(hSecond);

                firstChunk.size = (ChunkSize)addrSecond - (ChunkSize)secondChunk.addr;
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

                thirdChunk.size = addrNext - addrThird;
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
                GetChunk(hSecond).size = addrNext - addrSecond;
                MEMMNG_BIGCHUNKALLOC_ASSERT(size + (addrNext - addrThird) == GetChunk(hSecond).size);
            }

            ChunkEntry & newAllocChunk = GetChunk(hSecond);
            newAllocChunk.addr = addrSecond;

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
                UnlinkFree(hPrevChunk);
                ChunkEntry & prevChunk = GetChunk(hPrevChunk);
                newFreeChunk.prev = prevChunk.prev;
                if (IsValid(newFreeChunk.prev))
                {
                    MEMMNG_BIGCHUNKALLOC_ASSERT(GetChunk(newFreeChunk.prev).next == hPrevChunk);
                    GetChunk(newFreeChunk.prev).next = hNewFreeChunk;
                }
                newFreeChunk.size += prevChunk.size;
                newFreeChunk.addr = prevChunk.addr;
                RemoveChunk(hPrevChunk);
            }
            if (IsValid(newFreeChunk.next) && IsFreeChunk(newFreeChunk.next))
            {
                // merge with previous  (next?)
                ChunkIdx hNextChunk = newFreeChunk.next;
                UnlinkFree(hNextChunk);
                ChunkEntry & nextChunk = GetChunk(hNextChunk);
                newFreeChunk.next = nextChunk.next;
                if (IsValid(newFreeChunk.next))
                {
                    MEMMNG_BIGCHUNKALLOC_ASSERT(GetChunk(newFreeChunk.next).prev == hNextChunk);
                    GetChunk(newFreeChunk.next).prev = hNewFreeChunk;
                }
                newFreeChunk.size += nextChunk.size;
                RemoveChunk(hNextChunk);
            }
            return hNewFreeChunk;
        }

        void ChunkContainer::CheckBlockLinks(ChunkIdx idx) const
        {
            const ChunkEntry & chunk = GetChunk(idx);
            MEMMNG_BIGCHUNKALLOC_ASSERT(chunk.size>=MinimalChunkSize);
            // check mem block linking
            if (IsValid(chunk.prev))
            {
                const ChunkEntry & chunkPrev = GetChunk(chunk.prev);
                MEMMNG_BIGCHUNKALLOC_ASSERT(((char *)chunkPrev.addr) + chunkPrev.size == (char *)chunk.addr);
                MEMMNG_BIGCHUNKALLOC_ASSERT(chunkPrev.next == idx);
            }
            if (IsValid(chunk.next))
            {
                const ChunkEntry & chunkNext = GetChunk(chunk.next);
                MEMMNG_BIGCHUNKALLOC_ASSERT(((char *)chunk.addr) + chunk.size == (char *)chunkNext.addr);
                MEMMNG_BIGCHUNKALLOC_ASSERT(chunkNext.prev == idx);
            }
            // check free links
            if (chunk.isFree)
            {
                // no contiguous free blocks
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(chunk.next) || !GetChunk(chunk.next).isFree);
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(chunk.prev) || !GetChunk(chunk.prev).isFree);
                // incremental size in free links
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(chunk.freeLink.next) || GetChunk(chunk.freeLink.next).size>=chunk.size);
                MEMMNG_BIGCHUNKALLOC_ASSERT(!IsValid(chunk.freeLink.prev) || GetChunk(chunk.freeLink.prev).size<=chunk.size);
            }
        }

    }  // namespace ChunkAllocator

    static Mutex * g_bigAllocMutex = NULL;
    static MemoryPageManager * g_bigMemoryPageManager = NULL;
    static ChunkAllocator::ChunkContainer * g_chunkContainer = NULL;

    // outside access
    bool Init()
    {
        static MemoryPageManager bigMemoryPageManager;
        g_bigMemoryPageManager = &bigMemoryPageManager;
        bigMemoryPageManager.Init(MemoryPageManager::InitParams().setPageAlignment(ChunkAllocator::MinimalAlignment));
        static ChunkAllocator::ChunkContainer bigChunkContainer;
        g_chunkContainer = &bigChunkContainer;
        bigChunkContainer.Init(bigMemoryPageManager.GetAreaBegin(), bigMemoryPageManager.GetAreaEnd() - bigMemoryPageManager.GetAreaBegin());
        static Mutex bigAllocMutex;
        g_bigAllocMutex = &bigAllocMutex;
        return true;
    }

    void GetRange(void * & _baseAddress, void * & _limitAddress)
    {
        _baseAddress = g_chunkContainer->GetStartAddress();
        _limitAddress = (u8*)_baseAddress + g_chunkContainer->GetInitialSize() - 1u;
    }

    bool IsInRange(void * _ptr)
    {
        return g_chunkContainer->IsAddressInRange((u8 *)_ptr);
    }

    ux GetPageSize()
    {
        return g_bigMemoryPageManager->GetPageSize();
    }

    void* Alloc(u32 _size)
    {
        ScopeLockMutex locker(*g_bigAllocMutex);
        ChunkAllocator::ChunkIdx chkIdx = g_chunkContainer->Alloc(_size, ChunkAllocator::MemType_Big_Unknown);
        if (chkIdx == ChunkAllocator::ChunkContainer::InvalidIndex)
        {
            return NULL;
        }
        ChunkAllocator::ChunkSize effectiveBlockSize = g_chunkContainer->GetChunkSize(chkIdx);
        u8 * requestedAddress = g_chunkContainer->GetAddress(chkIdx);
        u8 * committed = g_bigMemoryPageManager->CommitPages(requestedAddress, effectiveBlockSize);
        if (committed == NULL) // alloc failed -> probably out of physical memory
        {
            g_chunkContainer->Free(chkIdx);
        }
        else
        {
            FILL_ALLOC(committed, effectiveBlockSize);
        }
        MEMMNG_ASSERT(committed == NULL || committed == requestedAddress);
        return committed;
    }

    void Free(void * _ptr)
    {
        ScopeLockMutex locker(*g_bigAllocMutex);
        ChunkAllocator::ChunkIdx chkIdx = g_chunkContainer->FindChunkFromAddress((u8 *)_ptr);
        MEMMNG_ASSERT(chkIdx!=ChunkAllocator::ChunkContainer::InvalidIndex);
        auto chunkSize = g_chunkContainer->GetChunkSize(chkIdx);
        FILL_FREE(_ptr, chunkSize);
        g_chunkContainer->Free(chkIdx);
        g_bigMemoryPageManager->DecommitPages((u8 *)_ptr, chunkSize);
    }

    ux MSize(void * _ptr)
    {
        ScopeLockMutex locker(*g_bigAllocMutex);
        ChunkAllocator::ChunkIdx chkIdx = g_chunkContainer->FindChunkFromAddress((u8 *)_ptr);
        MEMMNG_ASSERT(chkIdx!=ChunkAllocator::ChunkContainer::InvalidIndex);
        return g_chunkContainer->GetChunkSize(chkIdx);
    }

} // namespace ITF::MemMngBigAlloc
