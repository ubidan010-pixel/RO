
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREE_H_
#include "engine/blendTree/animTree/AnimTree.h"
#endif //_ITF_ANIMTREE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{

void AnimTree::updateTransitions( f32 _dt, const InputContainer& _inputs )
{
    Super::updateTransitions(_dt,_inputs);

    if ( m_currentTransition )
    {
        if ( m_currentBlendResult.m_animFinished )
        {
            m_currentTransition = NULL;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(AnimTree_Template)
BEGIN_SERIALIZATION_CHILD(AnimTree_Template)
END_SERIALIZATION()

AnimTree_Template::AnimTree_Template()
{
    m_factory = ACTORSMANAGER->getAnimTreeNodeFactory();
}

}
