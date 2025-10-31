
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODEBRANCHTRANSITION_H_
#include "engine/blendTree/animTree/AnimTreeNodeBranchTransition.h"
#endif //_ITF_ANIMTREENODEBRANCHTRANSITION_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeBranchTransition)

AnimTreeNodeBranchTransition::AnimTreeNodeBranchTransition()
{
}

void AnimTreeNodeBranchTransition::updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    if ( m_activeLeaf < ( this->m_leafs.size() - 1 ) )
    {
        AnimTreeResult* result = static_cast<AnimTreeResult*>(_result);

        if ( result->m_animFinished )
        {
            setActiveLeaf(_inputs,m_activeLeaf+1,bfalse);
            result->m_animFinished = bfalse;
        }
    }
}

void AnimTreeNodeBranchTransition::onBecomeActive( const InputContainer& _inputs )
{
    setActiveLeaf(_inputs,0,btrue);
    Super::onBecomeActive(_inputs);
}

void AnimTreeNodeBranchTransition::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
{
    if ( m_activeLeaf != U32_INVALID )
    {
        BlendTreeNode <AnimTreeResult>* node = this->m_leafs[m_activeLeaf];

        node->getPlayingNodes(_inputs,_playingNodes);
    }

    _playingNodes.push_back(this);
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeBranchTransition_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodeBranchTransition_Template)
END_SERIALIZATION()

}



