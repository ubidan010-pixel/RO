
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONACTIVATESTONE_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionActivateStone.h"
#endif // _ITF_RAY_BTACTIONACTIVATESTONE_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionActivateStone);

Ray_BTActionActivateStone::Ray_BTActionActivateStone()
: m_animComponent(NULL)
, m_stone(ITF_INVALID_OBJREF)
, m_trigger(ITF_INVALID_OBJREF)
, m_finished(bfalse)
, m_finishMarkerReached(bfalse)
{
}

Ray_BTActionActivateStone::~Ray_BTActionActivateStone()
{
}

void Ray_BTActionActivateStone::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(Ray_EventStoneActivationFinished,1905511856), m_tree);
}

bbool Ray_BTActionActivateStone::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimatedComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnim().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getAnim().isValid(),"%s: there is no anim set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionActivateStone::decide( f32 _dt )
{
    return m_tree->getBlackBoard().factExists(Ray_AIFact_activateStone);
}

void Ray_BTActionActivateStone::onActivate()
{
    Super::onActivate();

    m_tree->getBlackBoard().getFact(Ray_AIFact_activateStone,m_stone);
    
    Actor* stoneActor = AIUtils::getActor(m_stone);

    if ( stoneActor )
    {
        Ray_EventActivateStone activateStone(m_tree->getActor()->getRef(),m_stone);
        stoneActor->onEvent(&activateStone);

		Vec2d lookDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());
		Vec2d stoneDir = stoneActor->get2DPos() - m_tree->getActor()->get2DPos();

		if ( lookDir.dot(stoneDir) < 0.f )
		{
			m_tree->getActor()->setFlipped(!m_tree->getActor()->isFlipped());
		}
    }

	m_tree->getBlackBoard().getFact(Ray_AIFact_activateStoneTrigger,m_trigger);

	Actor* triggerActor = AIUtils::getActor(m_trigger);

	if ( triggerActor )
	{
		Vec2d lookDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());
		Vec2d triggerDir = triggerActor->get2DPos() - m_tree->getActor()->get2DPos();

		if ( lookDir.dot(triggerDir) < 0.f )
		{
			m_tree->getActor()->setFlipped(!m_tree->getActor()->isFlipped());
		}
	}

    GroundAIControllerComponent* aiController = m_tree->getActor()->GetComponent<GroundAIControllerComponent>();

    if ( aiController )
    {
        aiController->setDisablePhysics(btrue);
    }

    PhantomComponent* phantom = m_tree->getActor()->GetComponent<PhantomComponent>();

    if ( phantom )
    {
        phantom->setDisabled(btrue);
    }

    if ( m_animComponent )
    {
        m_animComponent->setAnim(getTemplate()->getAnim());
    }

    m_finished = bfalse;
    m_finishMarkerReached = bfalse;
}

void Ray_BTActionActivateStone::onDeactivate()
{
    Super::onActivate();

    GroundAIControllerComponent* aiController = m_tree->getActor()->GetComponent<GroundAIControllerComponent>();

    if ( aiController )
    {
        aiController->setDisablePhysics(bfalse);
    }

    PhantomComponent* phantom = m_tree->getActor()->GetComponent<PhantomComponent>();

    if ( phantom )
    {
        phantom->setDisabled(bfalse);
    }
}

void Ray_BTActionActivateStone::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( Ray_EventStoneActivationFinished* activationFinished = _event->DynamicCast<Ray_EventStoneActivationFinished>(ITF_GET_STRINGID_CRC(Ray_EventStoneActivationFinished,1905511856)) )
    {
        m_finished = btrue;
    }
    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_End_Dance,3811896980))
        {
            if ( m_finished )
            {
                m_finishMarkerReached = btrue;
            }
        }
    }
}

bbool Ray_BTActionActivateStone::update( f32 _dt )
{
    Super::update(_dt);

    if ( m_finishMarkerReached )
    {
        m_finishMarkerReached = btrue;
        m_animComponent->setAnim(getTemplate()->getFinishAnim());
    }

    return btrue;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionActivateStone_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionActivateStone_Template)

    SERIALIZE_MEMBER("anim", m_anim);
    SERIALIZE_MEMBER("finishAnim", m_finishAnim);

END_SERIALIZATION()

Ray_BTActionActivateStone_Template::Ray_BTActionActivateStone_Template()
{
}

}