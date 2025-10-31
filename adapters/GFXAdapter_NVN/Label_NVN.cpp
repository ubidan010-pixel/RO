#include "precompiled_GFXAdapter_NVN.h"

#include "Label_NVN.h"

#include <numeric>

namespace ITF::NVN
{
    LabelManager::LabelManager(ux nbMaxLabel, nvn::Device& device)
        : m_labelCount(nbMaxLabel)
        , m_nvnDevice(device)
    {
        u32 size = m_labelCount * sizeof(PlatformLabel);
        m_mem.alloc(size);

        m_freeIndices.resize(m_labelCount);
        std::iota(std::make_reverse_iterator(m_freeIndices.end()), std::make_reverse_iterator(m_freeIndices.begin()), 0);

        nvn::BufferBuilder builder;
        builder.SetDefaults()
            .SetDevice(&device)
            .SetStorage(m_mem.getStorage().memoryPool, m_mem.getStorage().offset, size);

        ITF_VERIFY(m_labelBuffer.Initialize(&builder));

        m_labelArea = reinterpret_cast<PlatformLabel*>(m_labelBuffer.Map());
    }

    LabelManager::PlatformLabel* LabelManager::alloc()
    {
        if (m_freeIndices.empty())
        {
            ITF_ASSERT(0);
            return nullptr;
        }
        ux newIdx = m_freeIndices.back();
        m_freeIndices.pop_back();
        return m_labelArea + newIdx;
    }

    void LabelManager::free(LabelManager::PlatformLabel* _pLabel)
    {
        uPtr index = _pLabel - m_labelArea;
        ITF_ASSERT(index < uPtr(m_labelCount));
        // check it's not yet released
        ITF_ASSERT(m_freeIndices.end() == find(m_freeIndices.begin(), m_freeIndices.end(), index));
        m_freeIndices.push_back(index);
    }

    void Label::initInternal(u32 _initialValue)
    {
        ITF_ASSERT(m_labelAddr == nullptr);
        m_labelAddr = LabelManager::get().alloc();
        m_bufferAddress = LabelManager::get().getBufferAddress(m_labelAddr);
        if (m_labelAddr!=nullptr)
        {
            m_labelAddr->counter = _initialValue;
        }
    }

    void Label::release()
    {
        if (m_labelAddr != nullptr)
        {
            m_labelAddr->counter = 0;
            LabelManager::get().free(m_labelAddr);
            m_labelAddr = nullptr;
            m_bufferAddress = nvn::BufferAddress(0);
        }
    }
} // ITF::NVN


