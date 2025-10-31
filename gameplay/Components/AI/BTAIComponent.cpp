#include "precompiled_gameplay.h"

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTAIComponent)
BEGIN_SERIALIZATION_CHILD(BTAIComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(BTAIComponent)

    VALIDATE_COMPONENT_PARAM("BehaviorTree", m_behaviorTree.validate(), "Behavior Tree has errors");

END_VALIDATE_COMPONENT()

BTAIComponent::BTAIComponent()
: m_physComponent(NULL)
, m_animatedComponent(NULL)
, m_randomAnim(0.f)
, m_disabled(bfalse)
{
}

BTAIComponent::~BTAIComponent()
{
}

void BTAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_physComponent = m_actor->GetComponent<PhysComponent>();
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    m_behaviorTree.init(&getTemplate()->getBehaviorTree(),GetActor());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDetach,889341216),this);
}

void BTAIComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventDetach* detachEvent = _event->DynamicCast<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216)) )
    {
        if ( m_physComponent )
        {
            StickToPolylinePhysComponent* stickComponent = m_physComponent->DynamicCast<StickToPolylinePhysComponent>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent,3089159513));

            if ( stickComponent )
            {
                if ( detachEvent->getPolyLine() == ITF_INVALID_OBJREF )
                {
                    stickComponent->unstick();
                }
                else if ( stickComponent->getStickedPolylineRef() == detachEvent->getPolyLine() )
                {
                    if ( detachEvent->getEdge() == U32_INVALID ||
                         detachEvent->getEdge() == stickComponent->getStickedEdgeIndex() )
                    {
                        stickComponent->unstick();
                    }
                }
            }
        }
    }
}

void BTAIComponent::Update( f32 _dt )
{
    m_randomAnim = Seeder::getSharedSeeder().GetFloat();

    if ( !m_disabled )
    {
        m_behaviorTree.update(_dt);
    }
}

void BTAIComponent::updateAnimInput()
{
    if ( m_physComponent )
    {
        Vec2d gravityNormal = m_physComponent->getCurrentGravityDir().getPerpendicular();
        const Vec2d& speed = m_physComponent->getSpeed();
        f32 speedTotal = speed.norm();
        f32 speedX = gravityNormal.dot(m_physComponent->getSpeed());
        f32 speedY = (-m_physComponent->getCurrentGravityDir()).dot(m_physComponent->getSpeed());
        Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
        f32 lookX = gravityNormal.dot(lookDir);

        if ( getSign(speedX) == getSign(lookX) )
        {
            speedX = f32_Abs(speedX);
        }
        else
        {
            speedX = f32_Abs(speedX)*-1.f;
        }

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed,4218325830),speedTotal);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedX,3996234170),speedX);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY,461177981),speedY);
    }

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(RandomN,218439229), m_randomAnim);

    m_behaviorTree.updateAnimInput();
}

#ifdef ITF_SUPPORT_EDITOR
void BTAIComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    m_behaviorTree.drawEdit(drawInterface,_flags);
}
#endif // ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(BTAIComponent_Template)
BEGIN_SERIALIZATION(BTAIComponent_Template)

    SERIALIZE_OBJECT("behaviorTree",m_behaviorTree);

END_SERIALIZATION()

BTAIComponent_Template::BTAIComponent_Template()
{
}

BTAIComponent_Template::~BTAIComponent_Template()
{
}
}
