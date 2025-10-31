#include "precompiled_GFXAdapter_GCM.h"
#include "VertexShaderCache_GCM.h"

namespace ITF
{
    namespace GCM
    {
        VertexShaderCache::VertexShaderCache()
            : m_olderCacheEntry(0)
        {
            flush();
        }

        // empty the cache
        void VertexShaderCache::flush()
        {
            for(CacheEntries::const_iterator it = m_cacheEntries.begin(), itEnd = m_cacheEntries.end();
                it!=itEnd;
                ++it)
            {
                if (it->program)
                {
                    it->program->instructionSlot = InvalidInstructionSlot;
                }
            }
            m_cacheEntries.resize(0);
            CacheEntry startupEntry;
            startupEntry.program = NULL;
            startupEntry.firstSlot = 0u;
            startupEntry.nbSlot = NbInstructionSlotsInCache;
            m_cacheEntries.push_back(startupEntry);
            m_olderCacheEntry = 0;
        }

        u32 VertexShaderCache::allocProgramSlot(const VertexProgram & _program) // return slot location
        {
            checkValidity();
            CacheEntry newEntry;
            newEntry.program = &_program;
            newEntry.nbSlot = _program.instructionCount;
            // enough place to the end ?
            if (m_cacheEntries[m_olderCacheEntry].firstSlot + newEntry.nbSlot > NbInstructionSlotsInCache)
            {
                m_olderCacheEntry = 0; // restart, as not enough place to the end
            }
            // invalidate shaders slots until enough place
            u32 firstEntryToFree = m_olderCacheEntry;
            newEntry.firstSlot = m_cacheEntries[firstEntryToFree].firstSlot;
            // we merge all following entries in firstEntryToFree
            u32 lastEntryToFree = firstEntryToFree;
            u32 nbSlotAllocated = 0;
            for(u32 currentEntryToFree = firstEntryToFree;
                nbSlotAllocated<newEntry.nbSlot;
                currentEntryToFree++)
            {
                lastEntryToFree = currentEntryToFree;
                CacheEntry & curEntry = m_cacheEntries[currentEntryToFree];
                nbSlotAllocated += curEntry.nbSlot;
                if (curEntry.program)
                {
                    curEntry.program->instructionSlot = InvalidInstructionSlot;
                    curEntry.program = NULL;
                }
            }
            ITF_ASSERT(lastEntryToFree >= firstEntryToFree);
            // build the free block following the newEntry
            CacheEntry freeEntry;
            freeEntry.program = NULL;
            freeEntry.nbSlot = nbSlotAllocated - newEntry.nbSlot;
            freeEntry.firstSlot = newEntry.firstSlot + newEntry.nbSlot;
            // All the block between firstEntryToFree to currentEntryToFree has to be merged into 
            // 1 or 2 slots -> newEntry and freeEntry
            // 4 case -> 
            // - just reuse the entry (same size)
            // - add 1 entry
            // - do not add or remove entries
            // - remove entries
            m_cacheEntries[firstEntryToFree] = newEntry;
            if (freeEntry.nbSlot !=0)
            {
                if (lastEntryToFree-firstEntryToFree == 0)
                {
                    // add one for the newly freed block
                    m_cacheEntries.insert(m_cacheEntries.begin()+firstEntryToFree+1u, freeEntry);
                }
                else
                {
                    m_cacheEntries[firstEntryToFree+1u] = freeEntry;
                    // remove all entries left
                    m_cacheEntries.erase(m_cacheEntries.begin()+firstEntryToFree+1u,
                                         m_cacheEntries.begin()+lastEntryToFree+1u); // +1 as the last is excluded
                }
            }
            m_olderCacheEntry = firstEntryToFree+1u;
            // assign new slot in shader and return the slot
            _program.instructionSlot = newEntry.firstSlot;
            checkValidity();
            return newEntry.firstSlot;
        }

        void VertexShaderCache::checkValidity()
        {
            #ifdef ASSERT_ENABLED
            ITF_ASSERT(m_olderCacheEntry<m_cacheEntries.size());
            u32 currentSlot = 0;
            for(CacheEntries::const_iterator it = m_cacheEntries.begin(), itEnd = m_cacheEntries.end();
                it!=itEnd;
                ++it)
            {
                ITF_ASSERT(currentSlot == it->firstSlot);
                ITF_ASSERT(it->nbSlot != 0);
                if (it->program)
                {
                    ITF_ASSERT(it->program->instructionSlot != InvalidInstructionSlot);
                    ITF_ASSERT(it->program->instructionSlot == it->firstSlot);
                    ITF_ASSERT(it->program->instructionCount == it->nbSlot);
                }
                else
                {
                    CacheEntries::const_iterator itNext = it + 1u;
                    if (itNext!=itEnd)
                    {
                        ITF_ASSERT(itNext->program!=NULL); // after a free block, a not free one
                    }
                }
                currentSlot += it->nbSlot;
                ITF_ASSERT(currentSlot<=NbInstructionSlotsInCache);
            }
            #endif
        }

        void VertexShaderCache::loadProgram(cell::Gcm::Inline::CellGcmContext * _gcmCtxt, const VertexProgram & _program, u32 _loadSlot)
        {
            _gcmCtxt->SetVertexProgramLoadSlot(_loadSlot, _program.instructionCount, _program.ucode);
        }
    }
}

