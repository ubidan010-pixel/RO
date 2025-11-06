#include "precompiled_GFXAdapter_DX12.h"

#include "UniformBufferPool_DX12.h"
#include "core/math/BitTweak.h"

namespace ITF::DX12
{

    static constexpr u32 UniformBufferPool_DX12_BinSizes[] =
    {
        256, // 0..256
        512, // 257..512
        768, // 513..768
        1024, // etc.
        1280,
        1536,
        1792,
        2 * 1024,
        3 * 1024,
        3 * 1024 + 256, // Shaders meta data report that Most of buffers are 3296, rounded to 3*1024+256
        4 * 1024,
        8 * 1024,
        16 * 1024,
        32 * 1024,
        UINT32_MAX // 64K is probably the maximum size of a UBO, go to this case.
    };

    static u32 getUniformBufferPoolBinSizeIdxFromSize(u32 _size)
    {
        auto it = std::lower_bound(std::begin(UniformBufferPool_DX12_BinSizes), std::end(UniformBufferPool_DX12_BinSizes), _size);
        if (it == std::end(UniformBufferPool_DX12_BinSizes))
            return ITF_ARRAY_SIZE(UniformBufferPool_DX12_BinSizes) - 1;
        return u32(it - std::begin(UniformBufferPool_DX12_BinSizes));
    }

    UniformBufferPool::UniformBufferPool(ComPtr<ID3D12Device> _device)
        : m_device(std::move(_device))
    {
        m_availableUniformBuffers.resize(ITF_ARRAY_SIZE(UniformBufferPool_DX12_BinSizes));
    }

    UniquePtr<UploadBufferVRAMRelocHandle> UniformBufferPool::getFirstFitBuffer(CollectionOfBuffers& _freeBuffers, u32 _size)
    {
        auto itBuffer = std::find_if(std::make_reverse_iterator(_freeBuffers.end()), std::make_reverse_iterator(_freeBuffers.begin()),
            [&](UniquePtr<DX12::UploadBufferVRAMRelocHandle>& _bufferAllocHdl)
            {
                return _bufferAllocHdl->getStorage().size >= _size;
            });
        if (itBuffer == std::make_reverse_iterator(_freeBuffers.begin()))
            return {};
        UniquePtr<DX12::UploadBufferVRAMRelocHandle> result = std::move(*itBuffer);
        _freeBuffers.erase(itBuffer.base() - 1);
        return result;
    }

    ID3D12Resource * UniformBufferPool::allocUniformBuffer(u32 _size)
    {
        _size = BitTweak::GetAboveAlign(_size, UNIFORM_BUFFER_ALIGNMENT);
        u32 binIdx = getUniformBufferPoolBinSizeIdxFromSize(_size);
        ITF_ASSERT(binIdx < m_availableUniformBuffers.size());
        CollectionOfBuffers& freeBuffers = m_availableUniformBuffers[binIdx];

        UniquePtr<DX12::UploadBufferVRAMRelocHandle> bufferPtr = getFirstFitBuffer(freeBuffers, _size);
        if (bufferPtr == nullptr)
        {
            bufferPtr = makeUnique<MemoryId::mId_GfxAdapter, DX12::UploadBufferVRAMRelocHandle>();
            bufferPtr->allocAligned(_size, UNIFORM_BUFFER_ALIGNMENT);
        }

        m_currentUniformBuffers.push_back(std::move(bufferPtr));
        ITF_ASSERT(m_currentUniformBuffers.back() != nullptr);
        ITF_ASSERT(m_currentUniformBuffers.back()->getD3DResource() != nullptr);
        ITF_ASSERT(m_currentUniformBuffers.back()->getStorage().size != 0);
        ITF_ASSERT(m_currentUniformBuffers.back()->getStorage().memoryPool != nullptr);

        return m_currentUniformBuffers.back()->getD3DResource();
    }

    UniformBufferPool::BufferCollectionAndSync UniformBufferPool::getUnusedBufferCollectionAndSync()
    {
        if (m_unused.empty())
        {
            m_unused.emplace_back();
            m_device->CreateFence(m_currentFenceValue, D3D12_FENCE_FLAG_NONE, DX12_IID_COMPTR_ARGS(m_unused.back().fence));
        }
        BufferCollectionAndSync collection = std::move(m_unused.back());
        m_unused.pop_back();
        ITF_ASSERT(collection.buffers.empty());
        return collection;
    }

    void UniformBufferPool::moveBufferCollectionToAvailables(CollectionOfBuffers & _buffers)
    {
        for(auto & buffer : _buffers)
        {
            u32 binIndex = getUniformBufferPoolBinSizeIdxFromSize(buffer->getStorage().size);
            m_availableUniformBuffers[binIndex].emplace_back(std::move(buffer));
        }

        _buffers.clear();
    }

    void UniformBufferPool::recycleGPUProcessedBuffers()
    {
        for (auto it = m_submittedUniformBuffers.begin(); it != m_submittedUniformBuffers.end(); /* increment in the loop */)
        {
            BufferCollectionAndSync& submittedBuffer = *it;

            u64 completedValue = submittedBuffer.fence->GetCompletedValue();
            if (completedValue >= submittedBuffer.m_fenceValueRequested)
            {
                ITF_ASSERT(completedValue == submittedBuffer.m_fenceValueRequested); // logic issue! 
                moveBufferCollectionToAvailables(it->buffers);
                m_unused.emplace_back(std::move(submittedBuffer));
                it = m_submittedUniformBuffers.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void UniformBufferPool::advanceToNext(ID3D12CommandQueue* _queue)
    {
        recycleGPUProcessedBuffers();

        m_currentFenceValue++;

        BufferCollectionAndSync buffersAndSync = getUnusedBufferCollectionAndSync();
        buffersAndSync.buffers.assign(std::make_move_iterator(m_currentUniformBuffers.begin()), std::make_move_iterator(m_currentUniformBuffers.end()));
        buffersAndSync.m_fenceValueRequested = m_currentFenceValue;
        _queue->Signal(buffersAndSync.fence.Get(), buffersAndSync.m_fenceValueRequested);

        m_currentUniformBuffers.clear();
        m_submittedUniformBuffers.emplace_back(std::move(buffersAndSync));

    }
}
