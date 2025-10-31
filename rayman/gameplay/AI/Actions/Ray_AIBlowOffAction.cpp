#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBLOWOFFACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIBlowOffAction.h"
#endif //_ITF_RAY_AIBLOWOFFACTION_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBlowOffAction)
BEGIN_VALIDATE_ACTION(Ray_AIBlowOffAction)
    VALIDATE_ACTION_PARAM("", m_physComponent, "PhysComponent mandatory");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
Ray_AIBlowOffAction::Ray_AIBlowOffAction()
    : Super()
    , m_timer(0.0f)
    , m_noMovementTimer(0.0f)
    , m_initialDir(Vec2d::Zero)
    , m_previousPos(Vec2d::Zero)
    , m_physComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIBlowOffAction::~Ray_AIBlowOffAction()
{
}

//------------------------------------------------------------------------------
void Ray_AIBlowOffAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{   
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_physComponent = m_actor->GetComponent<PhysComponent>();

    // not binding-compatible
    ITF_WARNING(
        m_actor, !m_actor->getParentBind(),
        "This actor shouldn't be bound because its Ray_AIBlowOffAction is "
        "moving it in world position");
}

//------------------------------------------------------------------------------
void Ray_AIBlowOffAction::onActivate()
{
    Super::onActivate();

    // remember initial direction, to keep for a while
    m_initialDir = m_physComponent->getSpeed();
    m_initialDir.normalize();
}

//------------------------------------------------------------------------------
void Ray_AIBlowOffAction::update(f32 _dt)
{
    // update timer
    m_timer += _dt;
    
    // determine target direction: keep initial direction for a while
    Vec2d targetDir = Vec2d::Up;
    if (m_timer < getTemplate()->getKeepDirDuration())
    {
        targetDir = m_initialDir;
    }
    else if (m_timer < (getTemplate()->getKeepDirDuration()
        + getTemplate()->getKeepDirTransitionDuration()))
    {
        f32 interp = (m_timer - getTemplate()->getKeepDirDuration())
            / getTemplate()->getKeepDirTransitionDuration();
        targetDir = Interpolate(m_initialDir, Vec2d::Up, interp);
    }
    else if (m_timer > (getTemplate()->getKeepDirDuration()
        + getTemplate()->getKeepDirTransitionDuration())
        + getTemplate()->getPostKeepDirLifetime())
    {
        m_aiBehavior->onActionFinished();
    }
    
    // a bit of lateral movement
    const f32 flySinus = sinf(m_timer * getTemplate()->getLateralFrequency() * MTH_2PI);
    const f32 flyAngle = getTemplate()->getLateralAmplitude().Radians() * flySinus;
    targetDir.Rotate(flyAngle);
    
    // apply fly force
    Vec2d targetSpeed = targetDir * getTemplate()->getMaxSpeed();
    Vec2d flyForce = targetSpeed - m_physComponent->getSpeed() * getTemplate()->getForceMultiplier();
    m_physComponent->addForce(flyForce - m_aiComponent->getCurrentGravity());

    // pitch (assumes angle is zero before action starts)
    const f32 pitchSinus = sinf(m_timer * getTemplate()->getPitchFrequency() * MTH_2PI);
    const f32 pitchAngle = getTemplate()->getPitchAmplitude().Radians() * pitchSinus;
    m_actor->setAngle(pitchAngle);
    
    // detect if didn't move for a while
    if (m_actor->get2DPos().IsEqual(m_previousPos, getTemplate()->getNoMovementEpsilon()))
    {
        if ((m_noMovementTimer += _dt) > getTemplate()->getNoMovementLifeTime())
        {
            m_aiBehavior->onActionFinished();
        }
    }
    else
    {
        m_noMovementTimer = 0.0f;
    }
    m_previousPos = m_actor->get2DPos();
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBlowOffAction_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIBlowOffAction_Template)
    SERIALIZE_MEMBER("aiFlyMaxSpeed",m_maxSpeed);
    SERIALIZE_MEMBER("aiFlyForceMultiplier" ,m_forceMultiplier); 
    SERIALIZE_MEMBER("aiFlyLateralFrequency", m_lateralFrequency); 
    SERIALIZE_MEMBER("aiFlyLateralAmplitude", m_lateralAmplitude); 
    SERIALIZE_MEMBER("aiFlyPitchFrequency" ,m_pitchFrequency); 
    SERIALIZE_MEMBER("aiFlyPitchAmplitude", m_pitchAmplitude); 
    SERIALIZE_MEMBER("aiFlyKeepDirDuration", m_keepDirDuration); 
    SERIALIZE_MEMBER("aiFlyKeepDirTransitionDuration", m_keepDirTransitionDuration); 
    SERIALIZE_MEMBER("aiFlyPostKeepDirLifetime", m_postKeepDirLifetime);
    SERIALIZE_MEMBER("aiFlyNoMovementLifeTime", m_noMovementLifeTime); 
    SERIALIZE_MEMBER("aiFlyNoMovementEpsilon", m_noMovementEpsilon); 
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIBlowOffAction_Template::Ray_AIBlowOffAction_Template()
    : Super()
    , m_maxSpeed(5.0f)
    , m_forceMultiplier(10.0f)
    , m_lateralFrequency(1.0f)
    , m_lateralAmplitude(bfalse, 0.0f)
    , m_pitchFrequency(1.0f)
    , m_pitchAmplitude(bfalse, 0.0f)
    , m_keepDirDuration(1.0f)
    , m_keepDirTransitionDuration(1.0f)
    , m_postKeepDirLifetime(1.0f)
    , m_noMovementLifeTime(1.0f)
    , m_noMovementEpsilon(MTH_EPSILON)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(MOVEFLY,644606417);
}

} // namespace ITF
