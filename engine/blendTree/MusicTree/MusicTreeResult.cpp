
#include "precompiled_engine.h"

#ifndef _ITF_MUSICTREERESULT_H_
#include "engine/blendTree/MusicTree/MusicTreeResult.h"
#endif //_ITF_MUSICTREERESULT_H_

namespace ITF
{
/*
void AnimTreeResultLeaf::updateBlend( bbool _activeNode, bbool _mostSignificant, f32 _currentWeight )
{
    m_weight *= _currentWeight;
    m_usePatches &= _mostSignificant;
    m_useEvents &= _activeNode;

    if ( !m_useEvents )
    {
        m_events.clear();
    }
}

void AnimTreeResult::updateBlend( BlendTreeResultBase* _blendResult, bbool _activeNode, bbool _mostSignificant, f32 _currentWeight )
{
    Super::updateBlend(_blendResult,_activeNode,_mostSignificant,_currentWeight);

    AnimTreeResult* otherResult = static_cast<AnimTreeResult*>(_blendResult);

    u32 numAnims = otherResult->m_anims.size();

    for ( u32 animIndex = 0; animIndex < numAnims; animIndex++ )
    {
        AnimTreeResultLeaf& localResult = otherResult->m_anims[animIndex];

        localResult.updateBlend(_activeNode,_mostSignificant,_currentWeight);

        m_anims.push_back(localResult);
    }

    if ( _activeNode )
    {
        m_animFinished = otherResult->m_animFinished;
        m_animLooped = otherResult->m_animLooped;
        m_animPos = otherResult->m_animPos;
    }
}
*/
}



