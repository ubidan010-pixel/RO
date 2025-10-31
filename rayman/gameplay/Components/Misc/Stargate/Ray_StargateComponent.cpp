#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STARGATECOMPONENT_H_
#include "rayman/gameplay/components/Misc/Stargate/Ray_StargateComponent.h"
#endif //_ITF_RAY_STARGATECOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_StargateComponent)

BEGIN_SERIALIZATION_CHILD(Ray_StargateComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_StargateComponent)
    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animComponent!=NULL, "Animated component is mandatory");
    VALIDATE_COMPONENT_PARAM("DetectorComponent", m_detectorComponent!=NULL, "Detector component is mandatory");
    VALIDATE_COMPONENT_PARAM("LinkComponent", m_linkComponent!=NULL, "LinkComponent is mandatory");
END_VALIDATE_COMPONENT()

Ray_StargateComponent::Ray_StargateComponent()
: m_animComponent(NULL)
, m_detectorComponent(NULL)
, m_linkComponent(NULL)
, m_activated(bfalse)
, m_used(bfalse)
{
}

Ray_StargateComponent::~Ray_StargateComponent()
{
}

void Ray_StargateComponent::onActorLoaded( Pickable::HotReloadType _reloadType )
{
    Super::onActorLoaded(_reloadType);

    m_animComponent = m_actor->GetComponent<AnimatedComponent>();
    m_detectorComponent = m_actor->GetComponent<DetectorComponent>();
    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerStargate,3125962121),this);
}

void Ray_StargateComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_used || !m_activated )
    {
        return;
    }

	updateTeleportArea();
    checkUse();
}

void Ray_StargateComponent::updateTeleportArea()
{
    m_teleportArea.clear();

	if ( m_animComponent )
	{
		u32 numPoints = 0;
		const Vec2d* points = NULL;

		points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getTeleportAreaName(), &numPoints);

		if ( points )
		{
            FixedArray <Vec2d,64> shapePoints;
            transform2d xf;

            xf.m_pos = m_actor->get2DPos();
            xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
            xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

			shapePoints.resize(numPoints);

			if ( !m_actor->isFlipped() )
			{
				for ( u32 i = 0; i < numPoints; i++ )
				{
					shapePoints[i] = mulTransform2dT( xf, points[i] );
				}
			}
			else
			{
				for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
				{
					shapePoints[numPoints-i-1] = mulTransform2dT( xf, points[i] );
				}
			}

            m_teleportArea.setPoints(shapePoints);
		}
	}
}

void Ray_StargateComponent::checkUse()
{
	if ( !m_teleportArea.isValid() )
	{
		return;
	}

	PhysContactsContainer contacts;
	PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(),
		&m_teleportArea, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
		contacts);

	const u32 contactCount = contacts.size();
	for ( u32 i = 0; i < contactCount; i++ )
	{
		ActorRef actorRef = contacts[i].m_collidableUserData;

		if (actorRef == m_actor->getRef())
		{
			continue;
		}

		Actor* otherActor = actorRef.getActor();
		if (!otherActor)
		{
			continue;
		}

        if ( GAMEMANAGER->isPlayerActor(actorRef) )
        {
            useDoor(actorRef);
            break;
        }
    }
}

void Ray_StargateComponent::useDoor( ObjectRef _triggerer )
{
    Actor* activator = AIUtils::getActor(_triggerer);

    if ( activator )
    {
        Vec3d startPos = m_actor->getPos();
        u32 boneIndex = m_animComponent->getBoneIndex(getTemplate()->getBoneStart());

        if ( boneIndex != U32_INVALID )
        {
            Vec2d bonePos;
            m_animComponent->getBonePos(boneIndex,bonePos);
            startPos = bonePos.to3d(m_actor->getPos().m_z);
        }

        Ray_EventTriggerStargate enterDoorEvent(m_actor->getRef(),startPos,getTemplate()->getSpeed());
        activator->onEvent(&enterDoorEvent);
    }
}

void Ray_StargateComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventTrigger* triggered = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        m_activated = btrue;
    }
    else if ( Ray_EventTriggerStargate* stargateTriggered = _event->DynamicCast<Ray_EventTriggerStargate>(ITF_GET_STRINGID_CRC(Ray_EventTriggerStargate,3125962121)) )
    {
        m_used = btrue;
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_StargateComponent_Template)
BEGIN_SERIALIZATION(Ray_StargateComponent_Template)

    SERIALIZE_MEMBER("speed",m_speed);
    SERIALIZE_MEMBER("boneStart",m_startBone);
	SERIALIZE_MEMBER("teleportArea",m_teleportArea);

END_SERIALIZATION()

Ray_StargateComponent_Template::Ray_StargateComponent_Template()
: m_speed(7.f)
{
}

}

