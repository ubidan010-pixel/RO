#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneWindBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneWindBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AILivingStoneWindBehavior_Template)
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneWindBehavior)

Ray_AILivingStoneWindBehavior::Ray_AILivingStoneWindBehavior()
: m_hysteresis(0.2f)
, m_timer(0.0f)
, m_continue(btrue)
, m_justEntered(btrue)
{
}

Ray_AILivingStoneWindBehavior::~Ray_AILivingStoneWindBehavior()
{
}

void Ray_AILivingStoneWindBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(WindStim,2026477007),m_aiComponent);
}

void Ray_AILivingStoneWindBehavior::update( f32 _delta  )
{
    Super::update(_delta);
    if (!m_physComponent->getStickedPolyline())
    {
        m_aiComponent->onBehaviorFinished();
        return;
    }

    //if (!m_justEntered)
    //{
    //    Vec2d speed = m_physComponent->getSpeed();
    //    PhysActorForceArray windForces;
    //    PHYSWORLD->getWindForces(m_actor->get2DPos(), speed, m_actor->getLayerID(), windForces);

    //    Vec2d force = Vec2d::Zero;
    //    for ( u32 i = 0; i < windForces.size(); i++ )
    //    {
    //        force += windForces[i].m_force;
    //    }
    //    if ((force == Vec2d::Zero && m_timer == 0.0f) || force.dot(AIUtils::getLookDir(m_actor,m_actor->isFlipped())) > 0.0f)
    //        m_aiComponent->onBehaviorFinished();
    //}
    //else
    //{
    //    m_justEntered = bfalse;
    //}
    if (!m_continue && m_timer == 0.0f)
        m_aiComponent->onBehaviorFinished();

    m_continue = bfalse;

    m_timer = Max(0.0f, m_timer - _delta);
}

void Ray_AILivingStoneWindBehavior::onEvent(Event * _event)
{
    Super::onEvent(_event);
    if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(WindStim,2026477007)))
    {
        m_continue = btrue;
    }
}

void Ray_AILivingStoneWindBehavior::onActivate()
{
    Super::onActivate();
    ITF_ASSERT(m_windDir != Vec2d::Zero);
    m_justEntered = btrue;
    m_timer = m_hysteresis;

    Vec2d myDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());

    bbool flip = myDir.dot(m_windDir) > 0.f;

    if ( flip )
    {
        performUTurnImmediate();
    }
    else
    {
        m_move->setWalkLeft(m_actor->isFlipped());
        setAction(m_move);
    }
}

void Ray_AILivingStoneWindBehavior::onDeactivate()
{
    Super::onDeactivate();

}

void Ray_AILivingStoneWindBehavior::setData(WindStim*_stim)
{
    m_source = _stim->getSender();
    m_windDir = _stim->getDir();
}

void Ray_AILivingStoneWindBehavior::onActionFinished()
{
    Super::onActionFinished();
    if ( m_currentAction == m_uturn )
    {
        m_move->setWalkLeft(m_actor->isFlipped());
        setAction(m_move);
    }
    else if ( m_currentAction == m_move )
    {
        m_aiComponent->onBehaviorFinished();
    }
}

}