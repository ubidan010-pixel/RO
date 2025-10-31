
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONPLAYANIM_H_
#include "gameplay/ai/BTs/BTActionPlayAnim.h"
#endif // _ITF_BTACTIONPLAYANIM_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionPlayAnim);

BTActionPlayAnim::BTActionPlayAnim()
: m_animComponent(NULL)
, m_firstUpdate(bfalse)
{
}

void BTActionPlayAnim::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimLightComponent>();
}

bbool BTActionPlayAnim::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getAnim().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getAnim().isValid(),"%s doesn't specify an anim to play",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"animComponent",m_animComponent!=NULL,"%s: animation component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionPlayAnim::onActivate()
{
    m_animComponent->setAnim(getTemplate()->getAnim());
    m_firstUpdate = btrue;
}

bbool BTActionPlayAnim::update( f32 _dt )
{
    if ( !m_firstUpdate )
    {
        if ( getTemplate()->getRestartOnFact().isValid() )
        {
            if ( m_tree->getBlackBoard().factExists(getTemplate()->getRestartOnFact()) )
            {
                m_animComponent->resetCurTime();
            }
        }
    }

    m_firstUpdate = bfalse;

    if ( m_animComponent->isAnimLooped() )
    {
        return btrue;
    }
    else
    {
        if ( getTemplate()->getRetOnFinish() )
        {
            return !m_animComponent->isAnimToPlayFinished();
        }
        else
        {
            return btrue;
        }
    }
}

IMPLEMENT_OBJECT_RTTI(BTActionPlayAnim_Template);

BEGIN_SERIALIZATION_CHILD(BTActionPlayAnim_Template)

    SERIALIZE_MEMBER("anim",m_anim);
    SERIALIZE_MEMBER("restartOnFact",m_restartOnFact);
    SERIALIZE_MEMBER("retOnFinish",m_retOnFinish);

END_SERIALIZATION()

}