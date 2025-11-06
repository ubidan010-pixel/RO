#pragma once

#include <array>
#include "core/utility.h"
#include "core/templateSingleton.h"
#include "VRAMMemoryTypes.h"

#include "DX12/Helpers.h"

namespace ITF
{
    class MemoryPageManager;

    namespace DX12
    {
        struct VRAMHandleLinks;
        class VRAMRelocHandle;

        namespace VRAMAllocator
        {
            class PoolContainer : public TemplateSingleton<PoolContainer>
            {
                
            public:

                PoolContainer(ComPtr<ID3D12Device> _device, bbool _autoAllocPool = btrue, bbool _forceOOM = bfalse);
                PoolContainer(const PoolContainer&) = delete;
                ~PoolContainer();

                PoolContainer & operator = (const PoolContainer&) = delete;

                ComPtr<ID3D12Device> getDevice() const { return m_device; }

                // create a pool in a pre allocated area
                class ChunkContainer * createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType);

                // Allocate memory and create a pool to handle at least the given size
                // _staticBuffer : freeing the last block is not freeing the pool.
                // The pool allocation is done using the page manager given at construction.
                class ChunkContainer * allocAndCreatePool(ChunkSize _blockSize, VRAMResourceType _resType, bbool _staticBuffer = bfalse);

                // call update to give a chance to defragmentation
                // _bForceErase : flush the freed memory blocked being kept in the death row
                void update(u64 _cpuCurrentFrame, u64 _gpuLastFrame, bbool _bForceErase);

                // release unused dynamic pools.
                void releaseUnusedDynamicPools();
        
                // print stats
                void printInfo();

                // get stats
                void getStats(VRAMStats & stats);

                // Force a break during OOM
                void setForceOOMBreak(bbool _setOn);

                // Define the size of the pool to create when all previous pools are full. Default is 100MiB
                void setMemoryPoolGranularity(ChunkSize _granularity);
                uPtr getMemoryPoolGranularity() const { return m_memoryPoolGranularity; }

                // this method can be used to build a loading policy according to the amount of memory available
                // It return true if the allocation can be performed without allocating an additional pool
                template <VRAMResourceType t = VRAMResourceType::RESOURCE>
                bbool canHandleAllocation(ChunkSize _blockSize, ChunkSize _alignment);
                template <VRAMResourceType t = VRAMResourceType::RESOURCE>
                bbool canHandleAllocation(ChunkSize _blockSize);

            private:
                friend class ::ITF::DX12::VRAMRelocHandle;
                //////////////////////////////////////////////////////////////////////////
                // RelocHdl access
                //////////////////////////////////////////////////////////////////////////
        
                // find the ChunkContainer where the VRAMHandleLinks is pointing
                class ChunkContainer * findPoolFromLink(const VRAMHandleLinks & link);
                void eraseContainer(class ChunkContainer * chunkContainer); // delete an empty container

                // find the best ChunkContainer for a size, return NULL if no container found
                template <VRAMResourceType t = VRAMResourceType::RESOURCE>
                void alloc(ChunkSize size, VramMemoryCategory type, class ChunkContainer * & container, ChunkIdx & idx);
                template <VRAMResourceType t = VRAMResourceType::RESOURCE>
                void allocAligned(ChunkSize size, ChunkSize align, VramMemoryCategory type, class ChunkContainer * & container, ChunkIdx & idx);

                //////////////////////////////////////////////////////////////////////////

                template <typename Fct>
                void forEachContainer(Fct _fct);

                using PoolVector = Vector<class ChunkContainer *>;

                ComPtr<ID3D12Device> m_device = nullptr; // DX12 device, needed to create memory pools

                Mutex m_mutex; // accessed in the VRAMRelocHandle system
                std::array<PoolVector, toUnderlying(VRAMResourceType::Count)> m_pools;
        
                bbool m_forceOOMBreak = { bfalse };

                bbool m_autoAllocPool = btrue;

                ChunkSize m_memoryPoolGranularity = ChunkSize(100)*1024*1024;
            };

        } // namespace VRAMAllocator
    } // namespace DX12
} // ITF
