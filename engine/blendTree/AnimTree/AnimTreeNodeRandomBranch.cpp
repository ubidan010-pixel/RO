
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODERANDOMBRANCH_H_
#include "engine/blendTree/animTree/AnimTreeNodeRandomBranch.h"
#endif //_ITF_ANIMTREENODERANDOMBRANCH_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeRandomBranch)

AnimTreeNodeRandomBranch::AnimTreeNodeRandomBranch()
{
}

void AnimTreeNodeRandomBranch::updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    if ( m_activeLeaf < ( this->m_leafs.size() ) )
    {
        AnimTreeResult* result = static_cast<AnimTreeResult*>(_result);

        if ( result->m_animFinished && getTemplate()->getLoop())
        {
            u32 index = Seeder::GetUnseededU32(0,this->m_leafs.size());
            setActiveLeaf(_inputs,index,btrue);
            result->m_animFinished = bfalse;
        }
    }
}

void AnimTreeNodeRandomBranch::onBecomeActive( const InputContainer& _inputs )
{
    u32 index = Seeder::GetUnseededU32(0,this->m_leafs.size());
    setActiveLeaf(_inputs,index ,btrue);
    Super::onBecomeActive(_inputs);
}

void AnimTreeNodeRandomBranch::getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes )
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

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeRandomBranch_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodeRandomBranch_Template)
SERIALIZE_MEMBER("loop",m_loop);
END_SERIALIZATION()

}



