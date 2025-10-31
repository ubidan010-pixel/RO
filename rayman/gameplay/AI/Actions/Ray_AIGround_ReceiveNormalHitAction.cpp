#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveNormalHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveNormalHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGround_ReceiveNormalHitAction_Template)
        SERIALIZE_MEMBER("frictionMultiplier", m_frictionMultiplier);
        SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
        SERIALIZE_MEMBER("hitForce", m_hitForce);
        SERIALIZE_MEMBER("verticalHitForce", m_verticalHitForce);
        SERIALIZE_MEMBER("randomHitForce", m_randomHitForce);
        SERIALIZE_MEMBER("randomAngleMax", m_randomAngleMax);
        SERIALIZE_MEMBER("antigravRampUpTime", m_antigravRampUpTime);
        SERIALIZE_MEMBER("fullAntigravTime", m_fullAntigravTime);
        SERIALIZE_MEMBER("antigravRampDownTime", m_antigravRampDownTime);
        SERIALIZE_MEMBER("pushBackForce", m_pushBackForce);
        SERIALIZE_MEMBER("canBlockHits", m_canBlockHits);
        SERIALIZE_MEMBER("minStunTime", m_minStunTime);
        SERIALIZE_MEMBER("maxStunTime", m_maxStunTime);
        SERIALIZE_MEMBER("ejectAnimSpeed", m_ejectAnimSpeed);
        SERIALIZE_MEMBER("brakeForce", m_brakeForce);
        SERIALIZE_MEMBER("brakeHeight", m_brakeHeight);
        SERIALIZE_MEMBER("speedLimit", m_speedLimit);
        SERIALIZE_MEMBER("mustFinishWithHurtTimer", m_mustFinishWithHurtTimer);
        SERIALIZE_MEMBER("resetSpeedAtStart", m_resetSpeedAtStart);
        SERIALIZE_MEMBER("orientVerticalForceToGround", m_orientVerticalForceToGround);
    END_SERIALIZATION()

    Ray_AIGround_ReceiveNormalHitAction_Template::Ray_AIGround_ReceiveNormalHitAction_Template()
    : m_frictionMultiplier(1.0f)
    , m_gravityMultiplier(1.0f)
    , m_hitForce(0.0f)
    , m_verticalHitForce(0.0f)
    , m_randomHitForce(0.0f)
    , m_randomAngleMax(btrue, 70.f)
    , m_antigravRampUpTime(0.0f)
    , m_fullAntigravTime(0.0f)
    , m_antigravRampDownTime(0.0f)
    , m_pushBackForce(0.0f)
    , m_canBlockHits(bfalse)
    , m_minStunTime(0.f)
    , m_maxStunTime(0.f)
    , m_ejectAnimSpeed(0.0f)
    , m_brakeHeight(0.0f)
    , m_brakeForce(500.0f)
    , m_speedLimit(0.5f)
    , m_mustFinishWithHurtTimer(bfalse)
    , m_resetSpeedAtStart( bfalse )
    , m_orientVerticalForceToGround(bfalse)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveNormalHitAction)

    BEGIN_VALIDATE_ACTION(Ray_AIGround_ReceiveNormalHitAction)
    END_VALIDATE_ACTION()

    Ray_AIGround_ReceiveNormalHitAction::Ray_AIGround_ReceiveNormalHitAction()
        : Super()
        , m_updatePushBack(bfalse)
        , m_justEntered(bfalse)
        , m_prevFrictionMultiplier(1.f)
        , m_prevGravityMultiplier(1.f)
        , m_antigravRampUpTimer(0.f)
        , m_fullAntigravTimer(0.f)
        , m_antigravRampDownTimer(0.f)
        , m_stunTimer(0.f)
        , m_stunned(bfalse)
        , m_animCursor(0.f)
        , m_initialHeight(0.0f)
        , m_isInAir(bfalse)
        , m_speedReset(bfalse)
        , m_receiveHitBehavior(NULL)
    {
    }

    Ray_AIGround_ReceiveNormalHitAction::~Ray_AIGround_ReceiveNormalHitAction()
    {
    }

    void Ray_AIGround_ReceiveNormalHitAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated
        m_receiveHitBehavior = m_aiBehavior->DynamicCast<Ray_AIReceiveHitBehavior>(ITF_GET_STRINGID_CRC(Ray_AIReceiveHitBehavior,2277900683));
        ITF_ASSERT(m_receiveHitBehavior);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStimNotify,1337973749),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),m_aiComponent);
    }


    void Ray_AIGround_ReceiveNormalHitAction::onActivate()
    {
        Super::onActivate();


        // reset

        m_prevFrictionMultiplier = m_physComponent->getFrictionMultiplier();
        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();

        m_stunned = bfalse;
        m_stunTimer = 0.f;

        m_pushbackPosOnHit = Vec2d::Zero;
        m_updatePushBack = bfalse;

        m_justEntered = btrue;



        // setup phys
        m_physComponent->setFrictionMultiplier(getTemplate()->getFrictionMultiplier());

        if ( !m_physComponent->getStickedEdge() )
        {
            m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());
        }

        m_physComponent->setSpeed(Vec2d::Zero);


        // setup hit force
        setupHitForce();

        m_speedReset = getTemplate()->getResetSpeedAtStart();

        if (getTemplate()->getPushBackForce() > 0.0f && m_hitPushBackDistance != 0.0f)
        {
            m_pushbackPosOnHit = AIUtils::getActor(m_attacker)->get2DPos();
            Vec2d dist = m_actor->get2DPos() - m_pushbackPosOnHit;
            //f32 distance = dist.norm();
            //if (distance < m_hitPushBackDistance)
            //    m_updatePushBack = btrue;

            if (m_physComponent->getStickedEdge()!=NULL)
            {
                Vec2d dist = m_actor->get2DPos() - m_pushbackPosOnHit;
                f32 distance = dist.norm();
                if (distance >= m_hitPushBackDistance)
                {
                }
                else
                {
                    f32 factor = 1.0f - Clamp(distance / m_hitPushBackDistance, 0.0f, 1.0f);
                    m_physComponent->addForce(m_hitDir * factor * getTemplate()->getPushBackForce());
                }
            }


        }

        // setup antigrav
        if (getTemplate()->getFullAntiGravTime() > 0.0f || getTemplate()->getAntiGravRampDownTime() > 0.0f)
        {
            m_antigravRampUpTimer = getTemplate()->getAntiGravRampUpTime();
            m_fullAntigravTimer = getTemplate()->getFullAntiGravTime();
            m_antigravRampDownTimer = getTemplate()->getAntiGravRampDownTime();

            // to avoid 1 frame of gravity speed add
            if ( m_speedReset && ( m_fullAntigravTimer > 0.0f ) )
            {
                m_physComponent->setGravityMultiplier( 0.f );
            }
        }

        m_initialHeight = m_actor->getPos().m_y;


#ifdef DEBUG_HIT_TRAJECTORY
        m_debugLastPos = m_actor->get2DPos();
#endif
    }

    void Ray_AIGround_ReceiveNormalHitAction::onDeactivate()
    {
        Super::onDeactivate();
        m_antigravRampUpTimer = 0.0f;
        m_fullAntigravTimer = 0.0f;
        m_antigravRampDownTimer = 0.0f;
        m_physComponent->setFrictionMultiplier(m_prevFrictionMultiplier);
        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
    }


    void Ray_AIGround_ReceiveNormalHitAction::setupHitForce()
    {
        Vec2d force = getTemplate()->getHitForce() * m_hitDir;
        if (getTemplate()->getOrientVerticalForceToGround() && m_physComponent->getStickedEdge())
        {
            force += getTemplate()->getVerticalHitForce() * m_physComponent->getStickedEdge()->m_normalizedVector.getPerpendicular();
        }
        else
        {
            force += getTemplate()->getVerticalHitForce() * -m_aiComponent->getCurrentGravityDir();
        }
        m_physComponent->addForce(force);

        if (getTemplate()->getRandomHitForce())
        {
            setupRandomHitForce();
        }
    }

    void Ray_AIGround_ReceiveNormalHitAction::setupRandomHitForce()
    {
        // A random angle is chosen either side of a normal.
        // The normal is the edge normal if we're sticked,
        // otherwise the hit direction.

        Vec2d normal;

        if ( const PolyLineEdge* edge = m_physComponent->getStickedEdge() )
        {
            normal = edge->m_normalizedVector.getPerpendicular();
        }
        else
        {
            normal = m_hitDir;
        }

        f32 hitAngle = normal.getOrientedAngle(Vec2d::Right);
        hitAngle += Seeder::getSharedSeeder().GetFloat(-getTemplate()->getRandomAngleMax().ToRadians(), getTemplate()->getRandomAngleMax().ToRadians());
        Vec2d hitDir = Vec2d::Right.Rotate(hitAngle);

        m_physComponent->addForce(hitDir * getTemplate()->getRandomHitForce());

        faceHitDir(hitDir);

        //GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + hitDir.to3d(), Color::white(), 0.1f, 0.5f);
    }


    void Ray_AIGround_ReceiveNormalHitAction::update( f32 _dt )
    {
#ifdef DEBUG_HIT_TRAJECTORY
        const Vec2d& pos = m_actor->get2DPos();
        GFX_ADAPTER->drawDBGLine(m_debugLastPos, pos, Color::white(), 1.f);
        m_debugLastPos = pos;
#endif

        Super::update(_dt);

        updateHit(_dt);
        if (!m_justEntered)
        {
            bbool landedAfterFinishedHurt = m_receiveHitBehavior->getHurtTimer() == 0.0f &&  m_justLanded;            

            if (getTemplate()->getMustFinishWithHurtTimer())
            {
                if (m_receiveHitBehavior->getHurtTimer()==0)
                {
                    m_aiBehavior->onActionFinished();
                    return;
                }
            }
            else
            {
                if (isAnimFinished() || landedAfterFinishedHurt)
                {
                    m_aiBehavior->onActionFinished();
                    return;
                }
            }
        }

        updateAntigrav(_dt);

        updateEjectAnim(_dt);

        updatePushBack(_dt);

        m_justEntered = bfalse;
    }

    void Ray_AIGround_ReceiveNormalHitAction::updateHit( f32 _dt )
    {
        Super::updateHit(_dt);
        if (m_justLanded)
            m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);


    }

    void Ray_AIGround_ReceiveNormalHitAction::updateAntigrav( f32 _dt )
    {
        //dont do anything special until we start coming back down
        if (!m_speedReset)
        {
            //we started going back down
            if (m_physComponent->getSpeed().m_y < 0.0f && m_physComponent->getStickedEdge()==NULL)
            {
                m_speedReset = btrue;
                //if antigrav is set then set gravity to null
                if (getTemplate()->getFullAntiGravTime() > 0 || getTemplate()->getAntiGravRampDownTime() > 0)
                    m_physComponent->setGravityMultiplier(0.0f);
            }
            else
            {
                //increase gravity multiplier 
                f32 currentHeight = m_actor->getPos().m_y;
                f32 force = (getTemplate()->getBrakeHeight() > 0.0f) ? (1.0f-((currentHeight-m_initialHeight)/getTemplate()->getBrakeHeight())) * getTemplate()->getBrakeForce():0.0f;
                m_physComponent->setGravityMultiplier(force + m_prevGravityMultiplier);
            }
        }
        else
        {
            //going back down
            m_fullAntigravTimer = Max(0.0f,m_fullAntigravTimer - _dt);
            if (m_fullAntigravTimer == 0.0f)
            {
                m_antigravRampDownTimer = Max(0.0f,m_antigravRampDownTimer - _dt);
                if (m_antigravRampDownTimer > 0.0f)
                {
                    m_physComponent->setGravityMultiplier(m_prevGravityMultiplier-(m_antigravRampDownTimer / getTemplate()->getAntiGravRampDownTime()));
                }
                else
                {
                    m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
                }
            }
            else
            {
                m_physComponent->setGravityMultiplier(0.0f);
            }
        }
    }

    // only for LowEjecting hits (for now)
    void Ray_AIGround_ReceiveNormalHitAction::updateEjectAnim( f32 _dt )
    {
        if (getTemplate()->getEjectAnimSpeed())
        {
            f32 ySpeed = m_physComponent->getSpeed().dot(m_aiComponent->getCurrentGravityDir());
            f32 minSpeed = getTemplate()->getEjectAnimSpeed() * 0.5f;

            m_animCursor = ( ySpeed + minSpeed ) / getTemplate()->getEjectAnimSpeed();
            m_animCursor = Clamp(m_animCursor,0.f,1.f);
        }
    }

    void Ray_AIGround_ReceiveNormalHitAction::updatePushBack( f32 _dt )
    {
        if (m_updatePushBack && m_physComponent->getStickedEdge()!=NULL)
        {
            Vec2d dist = m_actor->get2DPos() - m_pushbackPosOnHit;
            f32 distance = dist.norm();
            if (distance >= m_hitPushBackDistance)
            {
                m_updatePushBack = bfalse;
            }
            else
            {
                f32 factor = 1.0f - Clamp(distance / m_hitPushBackDistance, 0.0f, 1.0f);
                m_physComponent->addForce(m_hitDir * factor * getTemplate()->getPushBackForce());
            }
        }
    }

    void Ray_AIGround_ReceiveNormalHitAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if (EventQueryBlocksHits* onQueryBlockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            onQueryBlockHits->setCanBlockHits(getTemplate()->getCanBlockHits());
        }
    }


    void Ray_AIGround_ReceiveNormalHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Stunned,1059090638), static_cast<u32>(m_stunned));

        if (getTemplate()->getEjectAnimSpeed())
        {
            m_animatedComponent->setProceduralCursor(m_animCursor);
        }
    }

}
