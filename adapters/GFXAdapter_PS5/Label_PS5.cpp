#include "precompiled_GFXAdapter_PS5.h"

#include "Label_PS5.h"

namespace ITF::Gnmp
{
    LabelManager::LabelManager(u8 * _labelArea, ux _labelMemorySize)
        : m_labelArea(reinterpret_cast<PlatformLabel*>(_labelArea))
        , m_labelCount(_labelMemorySize / ux(sizeof(PlatformLabel)))
    {
        for(ux i = m_labelCount; i != 0; --i)
        {
            m_freeIndices.push_back(i-1);
        }
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

    void Label::initInternal(u64 _initialValue)
    {
        ITF_ASSERT(m_labelAddr == nullptr);
        m_labelAddr = LabelManager::get().alloc();
        if (m_labelAddr!=nullptr)
        {
            m_labelAddr->m_value = _initialValue;
        }
    }

    void Label::release()
    {
        if (m_labelAddr != nullptr)
        {
            m_labelAddr->m_value = 0;
            LabelManager::get().free(m_labelAddr);
            m_labelAddr = nullptr;
            m_resHandler.unregister();
        }
    }
} // ITF::Gnmp


