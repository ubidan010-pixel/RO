
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODEMARCHBLEND_H_
#include "engine/blendTree/animTree/AnimTreeNodeMarchBlend.h"
#endif //_ITF_ANIMTREENODEMARCHBLEND_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeMarchBlend)

AnimTreeNodeMarchBlend::AnimTreeNodeMarchBlend()
: m_activeAction(U32_INVALID)
, m_activeTransition(U32_INVALID)
{

}

void AnimTreeNodeMarchBlend::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template )
{
    Super::init(_initData,_template);

    ITF_ASSERT(this->m_leafs.size() == _template->getCapacity());
}

void AnimTreeNodeMarchBlend::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    u32 prevBranch = m_activeAction;

    updateActiveChild(_inputs,m_activeAction==U32_INVALID);

    AnimTreeResult localResult;

    if ( prevBranch != U32_INVALID && prevBranch != m_activeAction )
    {
        if ( m_activeTransition != U32_INVALID )
        {
            this->m_leafs[m_activeTransition]->updateResult(0.f,_inputs,&localResult);

            if ( localResult.m_animPos < getTemplate()->getAnimPosToBlend() )
            {
                // If we are at the start of the animation we can blend directly to the next pose
                setActiveLeaf(_inputs,m_activeAction,bfalse);
                m_activeTransition = U32_INVALID;
            }
            else
            {
                // If it's too late then we blend to the transition of going to the new state
                m_activeTransition = prevBranch + 2;
                setActiveLeaf(_inputs,m_activeTransition,bfalse);
            }
        }
        else
        {
            // There is no transition playing, so we start playing one to go to the new action
            m_activeTransition = prevBranch + 2;
            setActiveLeaf(_inputs,m_activeTransition,btrue);
        }
    }
    else
    {
        if ( m_activeTransition != U32_INVALID )
        {
            // Check if our transition is finished
            this->m_leafs[m_activeTransition]->updateResult(0.f,_inputs,&localResult);

            if ( localResult.m_animFinished )
            {
                setActiveLeaf(_inputs,m_activeAction,btrue);
                m_activeTransition = U32_INVALID;
            }
        }
    }

    this->processBlend(_deltaTime,_inputs,_result);

    AnimTreeResult* result = static_cast<AnimTreeResult*>(_result);

    result->m_playingNodes.push_back(this);
}

void AnimTreeNodeMarchBlend::updateActiveChild( const InputContainer& _inputs, bbool _force )
{
    u32 leafIndex = 0;

    m_activeAction = U32_INVALID;

    for ( ChildDataContainer::iterator it = m_childData.begin(); it != m_childData.end() && m_activeAction == U32_INVALID; ++it, leafIndex++ )
    {
        BlendLeaf& leafCriterias = *it;

        m_activeAction = leafIndex;

        for (ITF_VECTOR<Criteria*>::const_iterator it = leafCriterias.m_criteria.begin(); it != leafCriterias.m_criteria.end(); ++it)
        {
            if (!(*it)->isEqual())
            {
                m_activeAction = U32_INVALID;
                break;
            }
        }
    }

    ITF_ASSERT(m_activeAction != U32_INVALID);

    if ( m_activeAction != U32_INVALID && _force )
    {
        setActiveLeaf(_inputs,m_activeAction,_force);
    }
}

void AnimTreeNodeMarchBlend::onTransitionPlay()
{
    m_activeAction = U32_INVALID;
    m_activeTransition = U32_INVALID;
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodeMarchBlend_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodeMarchBlend_Template)

    SERIALIZE_MEMBER("animPosToBlend",m_animPosToBlend);

END_SERIALIZATION()

AnimTreeNodeMarchBlend_Template::AnimTreeNodeMarchBlend_Template()
: m_animPosToBlend(0.5f)
{

}

}



