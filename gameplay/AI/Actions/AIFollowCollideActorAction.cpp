#include "precompiled_gameplay.h"

#ifndef _ITF_AIFOLLOWCOLLIDEACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowCollideActorAction.h"
#endif //_ITF_AIFOLLOWCOLLIDEACTORACTION_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(AIFollowCollideActorAction_Template)
BEGIN_SERIALIZATION_CHILD(AIFollowCollideActorAction_Template)
END_SERIALIZATION()
 
IMPLEMENT_OBJECT_RTTI(AIFollowCollideActorAction)

AIFollowCollideActorAction::AIFollowCollideActorAction()
: m_stickPhysComponent(NULL)
, m_contactOutPower(300.f)
, m_addedForceTime(0.f)
, m_addedForceTotalTime(0.5f)
, m_way(1.f)
{
}

AIFollowCollideActorAction::~AIFollowCollideActorAction()
{

}

void AIFollowCollideActorAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{   
    Super::onActorLoaded(_hotReload);

    m_stickPhysComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    ITF_ASSERT(m_stickPhysComponent);

}

void AIFollowCollideActorAction::onActivate()
{
    Super::onActivate();

    m_addedForceTime = 0.f;
}

void AIFollowCollideActorAction::update( f32 _delta )
{
    if (!m_followAnimLight || !m_physComponent || _delta <= 0.f)
        return;
    
    Super::update(_delta);

    const PolyLineEdge * polyEdge = m_stickPhysComponent->getStickedEdge();
    if (polyEdge)
    {
        // add force with specific direction
        m_addedForce      = polyEdge->m_normalizedVector * m_way;
        Vec2d   perp      = polyEdge->m_normalizedVector.getPerpendicular();
        if (perp.dot(m_stickPhysComponent->getSpeed()) > 0.f)
            perp *= -1.f;
        m_addedForce     += perp;
        m_addedForce     *= m_contactOutPower;
        m_addedForceTime  = m_addedForceTotalTime;
        m_way            *= -1.f;
    }

    if (m_addedForceTime > 0.f)
    {
        m_addedForceTime -= _delta;
        m_stickPhysComponent->addForce(m_addedForce * m_addedForceTime / m_addedForceTotalTime );
    }
}


}