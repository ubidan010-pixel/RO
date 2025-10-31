#include "precompiled_GFXAdapter_GCM.h"

#include "Label_GCM.h"

namespace ITF
{
namespace GCM
{
    LabelManager::LabelManager()
    {
        for(u32 i = LastLabelIndex+1; i != FirstLabelIndex; --i)
        {
            m_freeIndices.push_back(i-1);
        }
    }

    u32 LabelManager::allocIndex()
    {
        if (m_freeIndices.empty())
        {
            ITF_ASSERT(0);
            return InvalidLabelIndex;
        }
        u32 newIdx = m_freeIndices.back();
        m_freeIndices.pop_back();
        return newIdx;
    }

    void LabelManager::freeIndex(u32 _index)
    {
        ITF_ASSERT(_index>=FirstLabelIndex);
        ITF_ASSERT(_index<=LastLabelIndex);
        // check it's not yet released
        ITF_ASSERT(m_freeIndices.end() == std::find(m_freeIndices.begin(), m_freeIndices.end(), _index));
        m_freeIndices.push_back(_index);
    }

    void Label::init(u32 _initialValue)
    {
        ITF_ASSERT(labelIdx == LabelManager::InvalidLabelIndex);
        ITF_ASSERT(labelAddr == NULL);
        labelIdx = LabelManager::get().allocIndex();
        if (labelIdx!=LabelManager::InvalidLabelIndex)
        {
            labelAddr = cellGcmGetLabelAddress(labelIdx);
            *labelAddr = _initialValue;
        }
    }

    void Label::release()
    {
        if (labelIdx != LabelManager::InvalidLabelIndex)
        {
            LabelManager::get().freeIndex(labelIdx);
            labelIdx = LabelManager::InvalidLabelIndex;
            *labelAddr = 0;
        }
    }

} // GCM
} // ITF
