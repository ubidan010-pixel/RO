

#include "precompiled_gameplay.h"

#ifndef _ITF_WAYPOINTCOMPONENT_H_
#include "gameplay/Components/AI/WayPointComponent.h"
#endif //_ITF_WAYPOINTCOMPONENT_H_

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_
namespace ITF
{

IMPLEMENT_OBJECT_RTTI(WaypointComponent)

BEGIN_SERIALIZATION_CHILD(WaypointComponent)
END_SERIALIZATION()

WaypointComponent::WaypointComponent()
: m_registered(bfalse)
{
}

WaypointComponent::~WaypointComponent()
{
    clear();
}

void WaypointComponent::clear()
{
    if ( m_actor && m_actor->isActive() && m_registered )
    {
        WAYPOINTS_MANAGER->removeWaypoint(m_actor->getRef());
        m_registered = bfalse;
    }
}

void WaypointComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( !m_registered )
    {
        WAYPOINTS_MANAGER->addWaypoint(m_actor->getRef(),getTemplate()->getID(),m_actor->getDepth());
        m_registered = btrue;
    }
}

void WaypointComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_registered )
    {
        WAYPOINTS_MANAGER->removeWaypoint(m_actor->getRef());
        m_registered = bfalse;
    }
}

#ifdef ITF_SUPPORT_EDITOR
void WaypointComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec2d endPos = m_actor->get2DPos() + (Vec2d::Right.Rotate(m_actor->getAngle()));
    GFX_ADAPTER->drawArrow(m_actor->getPos(), endPos.to3d(m_actor->getPos().m_z), 1.f, 0.f, 0.f, 1.0f);
}

void WaypointComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    if(m_registered)
    {
        WAYPOINTS_MANAGER->removeWaypoint(m_actor->getRef());
        WAYPOINTS_MANAGER->addWaypoint(m_actor->getRef(),getTemplate()->getID(),m_actor->getDepth());
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(WaypointComponent_Template)
BEGIN_SERIALIZATION(WaypointComponent_Template)

    SERIALIZE_MEMBER("wayPointId", m_id);

END_SERIALIZATION()

WaypointComponent_Template::WaypointComponent_Template()
: m_id(StringID::Invalid)
{
}

}