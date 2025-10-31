#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DOORTELEPORTCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DoorTeleportComponent.h"
#endif //_ITF_RAY_DOORTELEPORTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_DoorTeleportComponent)

BEGIN_SERIALIZATION_CHILD(Ray_DoorTeleportComponent)
END_SERIALIZATION()

Ray_DoorTeleportComponent::Ray_DoorTeleportComponent()
: m_exitPointPos(Vec2d::Zero)
, m_animComponent(NULL)
{
}

Ray_DoorTeleportComponent::~Ray_DoorTeleportComponent()
{
}

void Ray_DoorTeleportComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
}

void Ray_DoorTeleportComponent::onResourceReady()
{
    if ( getTemplate()->getExitPoint().isValid() )
    {
        setExitPointPos();
    }
}

void Ray_DoorTeleportComponent::setExitPointPos()
{
    AnimLightComponent* animLight = m_actor->GetComponent<AnimLightComponent>();
    AnimPolyline* pPolyline = NULL;

    if ( animLight )
    {
        const Vec2d* pointsBuffer = animLight->getCurrentFirstPolylinePoint(getTemplate()->getExitPoint(), &pPolyline);
        if (pointsBuffer)
        {
            m_exitPointPos = ( *pointsBuffer - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
        }
    }
}

Vec2d Ray_DoorTeleportComponent::getExitPointPos() const
{
    return m_exitPointPos.Rotate(m_actor->getAngle()) + m_actor->get2DPos();
}

void Ray_DoorTeleportComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( EventTrigger* triggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        triggered(triggerEvent);
    }
}

void Ray_DoorTeleportComponent::triggered( EventTrigger* _trigger )
{
    Actor* user = AIUtils::getActor(_trigger->getActivator());
    Actor* sender = AIUtils::getActor(_trigger->getSender());

    if ( !user || !sender )
    {
        return;
    }
    
    f32 entryAngle = sender->getAngle();

    if ( sender->isFlipped() )
    {
        entryAngle += MTH_PI;
    }

    NormalizeAnglePI(entryAngle);

    f32 exitAngle = m_actor->getAngle() + MTH_PI;

    if ( m_actor->isFlipped() )
    {
        exitAngle += MTH_PI;
    }

    NormalizeAnglePI(exitAngle);

    Vec2d destPoint = getExitPointPos();
    const Vec2d& refPoint = _trigger->getRefPos();
    Vec2d upDir = Vec2d::Right.Rotate(m_actor->getAngle()+MTH_PIBY2);

    destPoint += upDir * refPoint.m_y;

    Ray_DoorTeleport teleport(destPoint.to3d(m_actor->getPos().m_z),m_actor->getAngle(),entryAngle,exitAngle);

    if ( m_animComponent )
    {
        teleport.setFlip(!m_actor->isFlipped());
    }

    user->onEvent(&teleport);
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_DoorTeleportComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec2d destPoint = getExitPointPos();

    GFX_ADAPTER->drawDBGCircle(destPoint.m_x, destPoint.m_y, 0.2f, 0.f, 1.f, 0.f, 0.0f, GetActor()->getDepth());
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_DoorTeleportComponent_Template)
BEGIN_SERIALIZATION(Ray_DoorTeleportComponent_Template)

    SERIALIZE_MEMBER("exitPointId",m_exitPointId);

END_SERIALIZATION()

}

