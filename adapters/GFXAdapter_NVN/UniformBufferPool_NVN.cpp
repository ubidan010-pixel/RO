#include "precompiled_GFXAdapter_NVN.h"

#include "UniformBufferPool_NVN.h"
#include "core/math/BitTweak.h"

namespace ITF
{

    static constexpr u32 UniformBufferPool_NVN_BinSizes[] =
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

    static constexpr u32 getUniformBufferPoolBinSizeIdxFromSize(u32 _size)
    {
        auto it = std::lower_bound(std::begin(UniformBufferPool_NVN_BinSizes), std::end(UniformBufferPool_NVN_BinSizes), _size);
        if (it == std::end(UniformBufferPool_NVN_BinSizes))
            return ITF_ARRAY_SIZE(UniformBufferPool_NVN_BinSizes) - 1;
        return it - std::begin(UniformBufferPool_NVN_BinSizes);
    }

    NVNUniformBufferPool::NVNUniformBufferPool(nvn::Device& _device)
        : m_device(_device)
    {
        m_availableUniformBuffers.resize(ITF_ARRAY_SIZE(UniformBufferPool_NVN_BinSizes));
        m_uniformBufferAlignment = u32(NVN::getDeviceProperty(_device, nvn::DeviceInfo::UNIFORM_BUFFER_ALIGNMENT));
    }

    NVN::UniquePtr<NVN::BufferVRAMRelocHandle> NVNUniformBufferPool::getFirstFitBuffer(CollectionOfBuffers& _freeBuffers, u32 _size)
    {
        auto itBuffer = std::find_if(std::make_reverse_iterator(_freeBuffers.end()), std::make_reverse_iterator(_freeBuffers.begin()),
            [&](NVN::UniquePtr<NVN::BufferVRAMRelocHandle>& _bufferAllocHdl)
            {
                return _bufferAllocHdl->getBuffer()->GetSize() >= _size;
            });
        if (itBuffer == std::make_reverse_iterator(_freeBuffers.begin()))
            return {};
        NVN::UniquePtr<NVN::BufferVRAMRelocHandle> result = std::move(*itBuffer);
        _freeBuffers.erase(itBuffer.base() - 1);
        return result;
    }

    nvn::Buffer & NVNUniformBufferPool::allocUniformBuffer(u32 _size)
    {
        _size = BitTweak::GetAboveAlign(_size, m_uniformBufferAlignment);
        u32 binIdx = getUniformBufferPoolBinSizeIdxFromSize(u32(_size));
        ITF_ASSERT(binIdx < m_availableUniformBuffers.size());
        CollectionOfBuffers& freeBuffers = m_availableUniformBuffers[binIdx];

        NVN::UniquePtr<NVN::BufferVRAMRelocHandle> bufferPtr = getFirstFitBuffer(freeBuffers, _size);
        if (bufferPtr == nullptr)
        {
            bufferPtr = NVN::makeUnique<NVN::BufferVRAMRelocHandle>();
            bufferPtr->allocAligned(_size, m_uniformBufferAlignment);
        }

        m_currentUniformBuffers.push_back(std::move(bufferPtr));
        ITF_ASSERT(m_currentUniformBuffers.back() != nullptr && m_currentUniformBuffers.back()->getBuffer() != nullptr);
        return *m_currentUniformBuffers.back()->getBuffer();
    }

    NVNUniformBufferPool::BufferCollectionAndSync NVNUniformBufferPool::getUnusedBufferCollectionAndSync()
    {
        if (m_unused.empty())
        {
            m_unused.emplace_back();
            m_unused.back().sync = NVN::makeInitialized<nvn::Sync>(&m_device);
        }
        BufferCollectionAndSync collection = std::move(m_unused.back());
        m_unused.pop_back();
        ITF_ASSERT(collection.buffers.empty());
        return collection;
    }

    void NVNUniformBufferPool::moveBufferCollectionToAvailables(CollectionOfBuffers & _buffers)
    {
        for(auto & buffer : _buffers)
        {
            u32 binIndex = getUniformBufferPoolBinSizeIdxFromSize(buffer->getBuffer()->GetSize());
            m_availableUniformBuffers[binIndex].emplace_back(std::move(buffer));
        }

        _buffers.clear();
    }

    void NVNUniformBufferPool::recycleGPUProcessedBuffers()
    {
        for (auto it = m_submittedUniformBuffers.begin(); it != m_submittedUniformBuffers.end(); /* increment in the loop */)
        {
            BufferCollectionAndSync& submittedBuffer = *it;
            nvn::SyncWaitResult waitRes = submittedBuffer.sync->Wait(0);
            ITF_ASSERT(waitRes != nvn::SyncWaitResult::FAILED);
            if (waitRes != nvn::SyncWaitResult::TIMEOUT_EXPIRED)
            {
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

    void NVNUniformBufferPool::advanceToNext(nvn::CommandBuffer& _rdrCtxt)
    {
        recycleGPUProcessedBuffers();

        BufferCollectionAndSync buffersAndSync = getUnusedBufferCollectionAndSync();
        buffersAndSync.buffers.assign(std::make_move_iterator(m_currentUniformBuffers.begin()), std::make_move_iterator(m_currentUniformBuffers.end()));
        _rdrCtxt.FenceSync(buffersAndSync.sync.get(), nvn::SyncCondition::ALL_GPU_COMMANDS_COMPLETE, nvn::SyncFlagBits::FLUSH_FOR_CPU);

        m_currentUniformBuffers.clear();

        m_submittedUniformBuffers.emplace_back(std::move(buffersAndSync));
    }

}
