#include "precompiled_gameplay.h"

#ifndef _ITF_AIFALLNOPHYSACTION_H_
#include "gameplay/ai/Actions/AIFallNoPhysAction.h"
#endif //_ITF_AIFALLNOPHYSACTION_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIFallNoPhysAction)
BEGIN_VALIDATE_ACTION(AIFallNoPhysAction)
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIFallNoPhysAction::AIFallNoPhysAction()
    : Super()
    , m_timer(0.0f)
    , m_speed(0.0f)
{
}

//------------------------------------------------------------------------------
AIFallNoPhysAction::~AIFallNoPhysAction()
{
}

//------------------------------------------------------------------------------
void AIFallNoPhysAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
}


//------------------------------------------------------------------------------
void AIFallNoPhysAction::onActivate()
{
    Super::onActivate();

    // reset stuff
    m_timer = 0.0f;
    m_speed = 0.0f;
}

//------------------------------------------------------------------------------
void AIFallNoPhysAction::update(f32 _dt)
{
    // fake fall
    m_speed += getTemplate()->getFakeGravity() * _dt;
    m_speed = Min(m_speed, getTemplate()->getMaxSpeed());
    Vec3d pos = m_actor->getPos();
    pos.m_y -= m_speed * _dt;
    m_actor->setBoundWorldPos(pos);

    // stop after timeout (if duration set)
    if (F32_INFINITY != getTemplate()->getDuration() &&
        (m_timer += _dt) > getTemplate()->getDuration())
    {
        m_aiBehavior->onActionFinished();
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIFallNoPhysAction_Template)
BEGIN_SERIALIZATION_CHILD(AIFallNoPhysAction_Template)
    SERIALIZE_MEMBER("duration", m_duration);
    SERIALIZE_MEMBER("fakeGravity", m_fakeGravity);
    SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIFallNoPhysAction_Template::AIFallNoPhysAction_Template()
    : Super()
    , m_duration(F32_INFINITY)
    , m_fakeGravity(1.0f)
    , m_maxSpeed(1.0f)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(FALL,100613701);
}

//------------------------------------------------------------------------------
AIFallNoPhysAction_Template::~AIFallNoPhysAction_Template()
{
}

} // namespace ITF
