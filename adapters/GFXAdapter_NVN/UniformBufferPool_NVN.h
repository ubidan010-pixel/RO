#pragma once

#include "core/types.h"
#include "core/container/extendedVector.h"
#include "core/container/extendedMap.h"

#include "VRAMAllocator/VRAMRelocHdl.h"

namespace ITF
{

    class NVNUniformBufferPool
    {
    public:
        explicit NVNUniformBufferPool(nvn::Device& _device);
        NVNUniformBufferPool(const NVNUniformBufferPool&) = delete;
        ~NVNUniformBufferPool() = default;

        NVNUniformBufferPool& operator = (const NVNUniformBufferPool&) = delete;

        nvn::Buffer& allocUniformBuffer(u32 _size);

        void advanceToNext(nvn::CommandBuffer & _rdrCtxt);

    private:
        using CollectionOfBuffers = Vector<NVN::UniquePtr<NVN::BufferVRAMRelocHandle>>;
        using BinCollection = Vector<CollectionOfBuffers>;

        static NVN::UniquePtr<NVN::BufferVRAMRelocHandle> getFirstFitBuffer(NVNUniformBufferPool::CollectionOfBuffers& _freeBuffers, u32 _size);
        BinCollection m_availableUniformBuffers; // fixed size vector (according to each bin)
        CollectionOfBuffers m_currentUniformBuffers;

        // The number of "frame" until we allow to reuse the submitted uniform buffers.
        // Should be synced with the number of frames allowed to be submitted.
        // The uniform buffers currently used by the GPU. Sent back to available after the GPU has processed them.
        struct BufferCollectionAndSync
        {
            CollectionOfBuffers buffers;
            NVN::UniquePtr<nvn::Sync> sync;
        };
        Vector<BufferCollectionAndSync> m_submittedUniformBuffers;

        void moveBufferCollectionToAvailables(CollectionOfBuffers& _buffers);
        void recycleGPUProcessedBuffers();

        BufferCollectionAndSync getUnusedBufferCollectionAndSync(); // get it in m_unused or create a new one if empty
        Vector<BufferCollectionAndSync> m_unused; // just a collection of empty buffer vector and its sync, to avoid reallocations each frame.

        nvn::Device& m_device;
        u32 m_uniformBufferAlignment = 0; // alignment of the uniform buffer (in bytes), fetched from device properties
    };

}
