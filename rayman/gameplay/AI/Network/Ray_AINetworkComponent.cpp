#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINETWORKCOMPONENT_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkComponent.h"
#endif //_ITF_RAY_AINETWORKCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_AINetworkComponent)
BEGIN_SERIALIZATION_CHILD(Ray_AINetworkComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_AINetworkComponent)

    VALIDATE_COMPONENT_PARAM("", getTemplate()->getShape(), "Ray_AINetworkComponent requires a shape");

END_VALIDATE_COMPONENT()

Ray_AINetworkComponent::Ray_AINetworkComponent()
: m_scaledShape(NULL)
{
}

Ray_AINetworkComponent::~Ray_AINetworkComponent()
{
}

void Ray_AINetworkComponent::onActorClearComponents()
{
    SF_DEL(m_scaledShape);

    AIUtils::setAlwaysActive(m_actor,bfalse);
}

void Ray_AINetworkComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if (getTemplate()->getShape())
    {
        scaleShape();
    }

    AIUtils::setAlwaysActive(m_actor,btrue);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374), this);
}

void Ray_AINetworkComponent::scaleShape()
{
    if ( !m_scaledShape ||
          m_scaledShape->GetObjectClassCRC() != getTemplate()->getShape()->GetObjectClassCRC() )
    {
        SF_DEL(m_scaledShape);
        m_scaledShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getShape()->GetObjectClassCRC());
    }

    PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_scaledShape, m_actor->getScale());
}

void Ray_AINetworkComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    FixedArray <ActorRef,32> actorsInside;
    const Ray_AINetworkManager::RegisteredActorsList& actorsList = RAY_AINETWORKMANAGER->getRegisteredActors();
    u32 numActors = actorsList.size();
    EventQueryPhysShape queryShape;
    AABB myAABB;
    const Vec2d& myPos = m_actor->get2DPos();
    f32 myAngle = m_actor->getAngle();
    Ray_EventAINetworkWaypoint waypoint;

    waypoint.setSender(m_actor->getRef());

    m_scaledShape->calculateAABB(myPos,myPos,myAngle, myAABB);

    for ( u32 i = 0; i < numActors; i++ )
    {
        const Ray_AINetworkManager::NetworkActorInfo& actorInfo = actorsList[i];
        ActorRef actorRef = actorInfo.m_actor;
        Actor* actor = actorRef.getActor();

        if ( !actor )
        {
            continue;
        }

        queryShape.setPhysShape(NULL);
        actor->onEvent(&queryShape);

        if ( !queryShape.getPhysShape() )
        {
            continue;
        }

        const Vec2d& actorPos = actor->get2DPos();
        const f32 actorAngle = actor->getAngle();
        const PhysShape* playerShape = queryShape.getPhysShape();
        AABB playerAABB;
        playerShape->calculateAABB(actorPos, actorPos, actorAngle, playerAABB);

        if ( !myAABB.checkOverlap(playerAABB) )
        {
            continue;
        }

        PhysContactsContainer contacts;
        PhysSweepInfo sweepA, sweepB;

        PhysCollisionSolver::calculateSweepInfo(actorPos, actorPos, actorAngle, playerShape, sweepA);
        PhysCollisionSolver::calculateSweepInfo(myPos, myPos, myAngle, m_scaledShape, sweepB);

        PhysCollisionSolver::collide(sweepA, sweepB, contacts);

        if ( contacts.size() == 0 )
        {
            continue;
        }

        actorsInside.push_back(actorRef);

        if ( m_actorsInsideLastFrame.find(actorRef) == -1 )
        {
            actor->onEvent(&waypoint);

            RAY_AINETWORKMANAGER->updateWaypoint(actorRef,m_actor->getRef());
        }
    }

    u32 numInside = actorsInside.size();

    m_actorsInsideLastFrame.resize(numInside);

    for ( u32 i = 0; i < numInside; i++ )
    {
        m_actorsInsideLastFrame[i] = actorsInside[i];
    }
}

void Ray_AINetworkComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventScaleChanged* crushedEvent = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)) )
    {
        scaleShape();
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_AINetworkComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
    {
        return;
    }

    if ( m_scaledShape )
    {
        DebugDraw::shape(m_actor->getPos(), m_actor->getAngle(), m_scaledShape,
            Color::white(), 0.f, "ainetwork node", 85 * MTH_DEGTORAD);
    }
}
#endif // ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_AINetworkComponent_Template)
BEGIN_SERIALIZATION(Ray_AINetworkComponent_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());

END_SERIALIZATION()

Ray_AINetworkComponent_Template::Ray_AINetworkComponent_Template()
: m_shape(NULL)
{
}

Ray_AINetworkComponent_Template::~Ray_AINetworkComponent_Template()
{
    SF_DEL(m_shape);
}

}
