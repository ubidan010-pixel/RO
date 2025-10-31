#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


// We have quite a few parameters to control our walking:
// 
// -	WalkForce
// -	Efficiency
// -	MoveTargetMultiplier
// 
// The calculation of the total force is done in 3 steps:
// 
// 1)	Get the base walk force defined in the action
// 2)	Multiplicate this force by the efficiency. The efficiency is based on our current speed, meaning that at min speed our efficiency is 100%,
//      and at top speed our efficiency is 0%, this way we make sure that after a certain speed we don’t add any more force.
//      The parameters to calculate the efficiency are:
//      a.	efficiencyMinSpeed = the speed at which the minimum efficiency will be used
//      b.	efficiencyMaxSpeed = the speed at which the maximum efficiency will be used
//      c.	efficiencyMinSpeedValue = the efficiency at a speed == efficiencyMinSpeed
//      d.	efficiencyMaxSpeedValue = the efficiency at a speed == efficiencyMaxSpeed
//      So if we set these values: a) 0 b) 5 c) 0 d) 1 and we go at 2.5 m/s our efficiency will be 0.5
// 3)	Multiplicate this force by the move target multiplier. This value is used to give some acceleration to the character. It’s just a number
//      to multiply the force, and this number increases over time since the action starts. The parameters are:
//      a.	moveTargetMultiplierMin: the minimum value of this number (when the action starts)
//      b.	moveTargetMultiplierMax: the maximum value of this number (when the action has been going on for moveTargetBlendTime)
//      c.	moveTargetBlendTime: the time it takes to reach the maximum moveTargetMultiplier.


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIWalkInDirAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIWalkInDirAction_Template)
        SERIALIZE_MEMBER("walkForce",m_walkForce);
        SERIALIZE_MEMBER("walkForceRandomFactor",m_walkForceRandomFactor);
        SERIALIZE_MEMBER("efficiencyMaxSpeed",m_efficiencyMaxSpeed);
        SERIALIZE_MEMBER("efficiencyMaxSpeedRandomFactor",m_efficiencyMaxSpeedRandomFactor);
        SERIALIZE_MEMBER("efficiencyMinGroundAngle",m_minGroundAngleEfficiencyAngle);
        SERIALIZE_MEMBER("efficiencyMaxGroundAngle",m_maxGroundAngleEfficiencyAngle);
        SERIALIZE_MEMBER("efficiencyMinAngleMultiplier",m_minGroundAngleEfficiencyMult);
        SERIALIZE_MEMBER("efficiencyMaxAngleMultiplier",m_maxGroundAngleEfficiencyMult);
        SERIALIZE_MEMBER("moveTargetMultiplierMin",m_moveTargetMultiplierMin);
        SERIALIZE_MEMBER("moveTargetMultiplierMax",m_moveTargetMultiplierMax);
        SERIALIZE_MEMBER("moveTargetBlendTime",m_moveTargetMultiplierBlendTime);
        SERIALIZE_MEMBER("walkAnimRate",m_walkAnimRate);
        SERIALIZE_MEMBER("minTime", m_minWalkTime);
        SERIALIZE_MEMBER("maxTime", m_maxWalkTime);
        SERIALIZE_MEMBER("walkLeftFlipsAnim", m_walkLeftFlipsAnim);
        SERIALIZE_MEMBER("useGroundAngle", m_useGroundAngle);
    END_SERIALIZATION()

        AIWalkInDirAction_Template::AIWalkInDirAction_Template()
        : m_walkForce(500.f)
        , m_walkForceRandomFactor(0.1f)
        , m_efficiencyMaxSpeed(5.43f)
        , m_efficiencyMaxSpeedRandomFactor(0.1f)
        , m_minGroundAngleEfficiencyAngle(btrue,45.f)
        , m_maxGroundAngleEfficiencyAngle(btrue,67.f)
        , m_minGroundAngleEfficiencyMult(1.f)
        , m_maxGroundAngleEfficiencyMult(0.f)
        , m_moveTargetMultiplierBlendTime(0.5f)
        , m_moveTargetMultiplierMin(0.f)
        , m_moveTargetMultiplierMax(1.f)
        , m_walkAnimRate(0.f)
        , m_minWalkTime(-1)     // default: walk forever
        , m_maxWalkTime(-1)
        , m_walkLeftFlipsAnim(bfalse)
        , m_useGroundAngle(btrue)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(WALK,2941578500);
    }

    IMPLEMENT_OBJECT_RTTI(AIWalkInDirAction)
    AIWalkInDirAction::AIWalkInDirAction()
        : m_physComponent(NULL)
        , m_moveTargetBlendTimer(0.f)
        , m_currentMoveTargetMultiplier(1.f)
        , m_walkLeft(bfalse)
        , m_walkTime(-1)
        , m_timer(0)
        , m_walkForce(1.f)
        , m_efficiencyMaxSpeed(1.f)
    {
    }

    AIWalkInDirAction::~AIWalkInDirAction()
    {
    }

    void AIWalkInDirAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        ITF_ASSERT(m_physComponent);

        m_walkForce = getTemplate()->getWalkForce() * (1.0f + Seeder::getSharedSeeder().GetFloat(-getTemplate()->getWalkForceRandomFactor(), getTemplate()->getWalkForceRandomFactor()));
        m_efficiencyMaxSpeed = getTemplate()->getEfficiencyMaxSpeed() * ( 1.0f + Seeder::getSharedSeeder().GetFloat(-getTemplate()->getEfficiencyMaxSpeedRandomFactor(), getTemplate()->getEfficiencyMaxSpeedRandomFactor()) );
    }

    void AIWalkInDirAction::onActivate()
    {
        Super::onActivate();

        m_moveTargetBlendTimer = 0.f;

        m_walkTime = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinWalkTime(), getTemplate()->getMaxWalkTime());
        m_timer = 0;
    }

    void AIWalkInDirAction::onDeactivate()
    {
        if (m_animComponent)
            m_animComponent->setPlayRate(1.f);
    }

    f32 AIWalkInDirAction::calculateEfficiency() const
    {
        f32 efficiency = AIUtils::walkCalculateEfficiency(m_physComponent->getSpeed().norm(),
                                                          m_efficiencyMaxSpeed,
                                                          m_physComponent->getFrictionCoef(),
                                                          m_aiComponent->getCurrentGravity().norm(),
                                                          m_aiComponent->getCurrentGravityDir(),
                                                          GetActor()->getAngle(),
                                                          getTemplate()->getMinGroundAngleEfficiencyAngle().ToRadians(),
                                                          getTemplate()->getMaxGroundAngleEfficiencyAngle().ToRadians(),
                                                          getTemplate()->getMinGroundAngleEfficiencyMult(),
                                                          getTemplate()->getMaxGroundAngleEfficiencyMult(),
                                                          getTemplate()->getUseGroundAngle());

        return efficiency;
    }

    void AIWalkInDirAction::update( f32 _delta )
    {
        if (getTemplate()->getMinWalkTime() != -1 && getTemplate()->getMaxWalkTime() != -1)
        {
            m_timer += _delta;
            if (m_timer > m_walkTime)
            {
                m_aiBehavior->onActionFinished();
                return;
            }
        }

        const PolyLineEdge* edge = m_physComponent->getStickedEdge();

        if ( !edge )
        {
            m_moveTargetBlendTimer = 0.f;
            return;
        }
        else
        {
            updateMoveTargetMultiplier(_delta);
        }

        Vec2d walkForce = m_physComponent->getCurrentGroundDir() * m_walkForce;

        if ( m_walkLeft )
        {
            walkForce *= -1.f;
        }

        f32 efficiency = calculateEfficiency();

        Vec2d force = walkForce;
        // multiplied by engine force
        force *= m_currentMoveTargetMultiplier;
        // multiplied by efficiency
        force *= efficiency;

        m_physComponent->addForce(force);

        Vec2d gravXaxis = m_aiComponent->getCurrentGravityDir().getPerpendicular();

        if (m_animComponent)
        {
            f32 speed    = f32_Abs(gravXaxis.dot(m_physComponent->getSpeed()));
            f32 playRate = getTemplate()->getWalkAnimRate() ? ( f32_Abs(speed) / getTemplate()->getWalkAnimRate() ) : 1.f;
            m_animComponent->setPlayRate(playRate);
            if (getTemplate()->getWalkLeftFlipsAnim())
            {
                m_actor->setFlipped(m_walkLeft);
            }
        }
    }

    void AIWalkInDirAction::updateMoveTargetMultiplier( f32 _deltaTime )
    {
        m_moveTargetBlendTimer = Min(m_moveTargetBlendTimer+_deltaTime,getTemplate()->getMoveTargetMultiplierBlendTime());

        if ( getTemplate()->getMoveTargetMultiplierBlendTime() )
        {
            m_currentMoveTargetMultiplier = Interpolate(getTemplate()->getMoveTargetMultiplierMin(),getTemplate()->getMoveTargetMultiplierMax(),m_moveTargetBlendTimer/getTemplate()->getMoveTargetMultiplierBlendTime());
        }
        else
        {
            m_currentMoveTargetMultiplier = getTemplate()->getMoveTargetMultiplierMax();
        }
    }

}