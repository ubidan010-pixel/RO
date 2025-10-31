#include "precompiled_gameplay.h"

#ifndef _ITF_ANIMATIONATLAS_H_
#include "gameplay/AI/Utils/AnimationAtlas.h"
#endif //_ITF_ANIMATIONATLAS_H_

namespace ITF
{
    BEGIN_SERIALIZATION(AnimationAtlas)
        SERIALIZE_CONTAINER_OBJECT("sequence", m_sequence);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(AnimationAtlas, Key)
        SERIALIZE_MEMBER("atlas", m_atlas);
        SERIALIZE_MEMBER("count", m_count);
    END_SERIALIZATION()

    //*****************************************************************************

    u32 AnimationAtlas::getMaxFrame() const
    {
        u32 total = 0;

        for (u32 i = 0; i < m_sequence.size(); i++)
        {
            const Key& frame = m_sequence[i];
            total += frame.m_count;
        }

        return total;
    }

    //*****************************************************************************
    // Animation Player
    //*****************************************************************************

    void AnimationAtlasPlayer::update()
    {
        m_curFrame++;

        if (m_curFrame >= m_maxFrame)
        {
            m_curFrame = 0;
        }
    }

    //*****************************************************************************

    void AnimationAtlasPlayer::setFrame(u32 _frame)
    {
        if (m_maxFrame > 0)
        {
            m_curFrame = _frame%m_maxFrame;
        }
        else
        {
            m_curFrame = 0;
        }
    }

    //*****************************************************************************

    u32 AnimationAtlasPlayer::getCurAtlas()
    {
        u32 atlas = 0;
        u32 cumul = 0;

        const SafeArray<AnimationAtlas::Key>& sequence = m_anim->getKeys();

        for (u32 i = 0; i < sequence.size(); i++)
        {
            const AnimationAtlas::Key& frame = sequence[i];
            cumul += frame.m_count;

            if (m_curFrame < cumul)
            {
                atlas = frame.m_atlas;
                break;
            }
        }

       return atlas;
    }

    //*****************************************************************************

    void AnimationAtlasPlayer::init( const AnimationAtlas* _anim )
    {
        ITF_ASSERT(_anim);

        m_anim = _anim;
        m_curFrame = 0;
        m_maxFrame = m_anim->getMaxFrame();
    }

};
