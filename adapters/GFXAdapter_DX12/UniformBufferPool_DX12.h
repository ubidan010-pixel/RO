#pragma once

#include "core/types.h"
#include "core/container/extendedVector.h"
#include "core/container/extendedMap.h"

#include "VRAMAllocator/VRAMRelocHdl.h"

#include "core/memory/UniquePtr.h"

namespace ITF::DX12
{

    class UniformBufferPool
    {
    public:
        explicit UniformBufferPool(ComPtr<ID3D12Device> _device);
        UniformBufferPool(const UniformBufferPool&) = delete;
        ~UniformBufferPool() = default;

        UniformBufferPool& operator = (const UniformBufferPool&) = delete;

        ID3D12Resource* allocUniformBuffer(u32 _size);

        void advanceToNext(ID3D12CommandQueue* _queue);

    private:
        using CollectionOfBuffers = Vector<UniquePtr<DX12::UploadBufferVRAMRelocHandle>>;
        using BinCollection = Vector<CollectionOfBuffers>;

        static UniquePtr<UploadBufferVRAMRelocHandle> getFirstFitBuffer(CollectionOfBuffers& _freeBuffers, u32 _size);
        BinCollection m_availableUniformBuffers; // fixed size vector (according to each bin)
        CollectionOfBuffers m_currentUniformBuffers;

        // The number of "frame" until we allow to reuse the submitted uniform buffers.
        // Should be synced with the number of frames allowed to be submitted.
        // The uniform buffers currently used by the GPU. Sent back to available after the GPU has processed them.
        struct BufferCollectionAndSync
        {
            CollectionOfBuffers buffers{};
            ComPtr<ID3D12Fence> fence{};
            u64 m_fenceValueRequested = 0;
        };
        u64 m_currentFenceValue = 0;
        Vector<BufferCollectionAndSync> m_submittedUniformBuffers;

        void moveBufferCollectionToAvailables(CollectionOfBuffers& _buffers);
        void recycleGPUProcessedBuffers();

        BufferCollectionAndSync getUnusedBufferCollectionAndSync(); // get it in m_unused or create a new one if empty
        Vector<BufferCollectionAndSync> m_unused; // just a collection of empty buffer vector and its sync, to avoid reallocations each frame.

        ComPtr<ID3D12Device> m_device{};
        enum : u32 { UNIFORM_BUFFER_ALIGNMENT = 256 }; // alignment of the uniform buffer (in bytes), DX12 requirement.
    };
}
