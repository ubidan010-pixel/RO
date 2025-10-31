#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveUpperHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_

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


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveUpperHitAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIGround_ReceiveUpperHitAction_Template)
        SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
        SERIALIZE_MEMBER("hitForce", m_hitForce);
        SERIALIZE_MEMBER("fullAntigravTime", m_fullAntigravTime);
        SERIALIZE_MEMBER("antigravRampDownTime", m_antigravRampDownTime);
        SERIALIZE_MEMBER("canBlockHits", m_canBlockHits);
        SERIALIZE_MEMBER("brakeForce", m_brakeForce);
        SERIALIZE_MEMBER("brakeHeight", m_brakeHeight);
    END_SERIALIZATION()

    Ray_AIGround_ReceiveUpperHitAction_Template::Ray_AIGround_ReceiveUpperHitAction_Template()
    : m_gravityMultiplier(1.0f)
    , m_hitForce(0.0f)
    , m_fullAntigravTime(0.0f)
    , m_antigravRampDownTime(0.0f)
    , m_canBlockHits(bfalse)
    , m_brakeHeight(0.0f)
    , m_brakeForce(500.0f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveUpperHitAction)

    BEGIN_VALIDATE_ACTION(Ray_AIGround_ReceiveUpperHitAction)
    END_VALIDATE_ACTION()

    Ray_AIGround_ReceiveUpperHitAction::Ray_AIGround_ReceiveUpperHitAction()
        : Super()

        , m_justEntered(bfalse)
        , m_prevFrictionMultiplier(1.f)
        , m_prevGravityMultiplier(1.f)
        , m_fullAntigravTimer(0.f)
        , m_antigravRampDownTimer(0.f)
        , m_ejectionState(TakeOff)
        , m_ejectionCycleCursor(0.0f)
        , m_speedReset(bfalse)
        , m_initialHeight(0.0f)
    {
    }

    Ray_AIGround_ReceiveUpperHitAction::~Ray_AIGround_ReceiveUpperHitAction()
    {
    }

    void Ray_AIGround_ReceiveUpperHitAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),m_aiComponent);
    }


    void Ray_AIGround_ReceiveUpperHitAction::onActivate()
    {
        Super::onActivate();


        // reset

        m_prevFrictionMultiplier = m_physComponent->getFrictionMultiplier();
        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();
        m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());


        m_justEntered = btrue;
        m_ejectionState = TakeOff;
        m_ejectionCycleCursor = 0.0f;


        // setup phys
        //if (m_physComponent->getStickedEdge() == NULL)
        {
            m_physComponent->setSpeed(Vec2d::Zero);
            m_physComponent->resetForces();
        }


        // setup hit force
        setupHitForce();

        m_speedReset = bfalse;


        // setup antigrav
        if (getTemplate()->getFullAntigravTime() > 0.0f || getTemplate()->getAntigravRampDownTime() > 0.0f)
        {
            m_fullAntigravTimer = getTemplate()->getFullAntigravTime();
            m_antigravRampDownTimer = getTemplate()->getAntigravRampDownTime();
        }

        m_initialHeight = m_actor->getPos().m_y;






#ifdef DEBUG_HIT_TRAJECTORY
        m_debugLastPos = m_actor->get2DPos();
#endif
    }

    void Ray_AIGround_ReceiveUpperHitAction::onDeactivate()
    {
        Super::onDeactivate();
        m_fullAntigravTimer = 0.0f;
        m_antigravRampDownTimer = 0.0f;
        m_physComponent->setFrictionMultiplier(m_prevFrictionMultiplier);
        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
    }


    void Ray_AIGround_ReceiveUpperHitAction::setupHitForce()
    {
        Vec2d force = getTemplate()->getHitForce() * Vec2d::Up;
        m_physComponent->addForce(force);

    }


    void Ray_AIGround_ReceiveUpperHitAction::update( f32 _dt )
    {
#ifdef DEBUG_HIT_TRAJECTORY
        const Vec2d& pos = m_actor->get2DPos();
        GFX_ADAPTER->drawDBGLine(m_debugLastPos, pos, Color::white(), 1.f);
        m_debugLastPos = pos;
#endif

        Super::update(_dt);

        updateHit(_dt);
        bbool justLandedState = bfalse;
        if (m_justLanded && m_ejectionState != Land && m_ejectionState != TakeOff)
        {
            m_ejectionState =Land;
            justLandedState = btrue;
            m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
        }

        if ( m_ejectionState == Land && !justLandedState && isAnimFinished() &&
            !m_justEntered && m_physComponent->getStickedEdge() )  // wait to touch the ground again when ejected
        {
            m_aiBehavior->onActionFinished();
            return;
        }

        updateAntigrav(_dt);

        m_justEntered = bfalse;
    }

    void Ray_AIGround_ReceiveUpperHitAction::updateHit( f32 _dt )
    {
        Super::updateHit(_dt);
    }

    void Ray_AIGround_ReceiveUpperHitAction::updateAntigrav( f32 _dt )
    {
        //dont do anything special until we start coming back down
        if (!m_speedReset)
        {
            //we started going back down
            if (m_physComponent->getSpeed().m_y < 0.0f && m_physComponent->getStickedEdge()==NULL)
            {
                m_speedReset = btrue;
                m_ejectionState = Cycle;
                m_physComponent->setSpeed(Vec2d::Zero);
                //if antigrav is set then set gravity to null
                if (getTemplate()->getFullAntigravTime() > 0 || getTemplate()->getAntigravRampDownTime() > 0)
                    m_physComponent->setGravityMultiplier(0.0f);
            }
            else
            {
                //increase gravity multiplier 
                f32 currentHeight = m_actor->getPos().m_y;
                f32 force = (getTemplate()->getBrakeHeight() > 0.0f && getTemplate()->getBrakeForce() > 0.0f) ? (1.0f-((currentHeight-m_initialHeight)/getTemplate()->getBrakeHeight())) * getTemplate()->getBrakeForce():0.0f;
                force = Max(force, 0.0f);
                m_physComponent->setGravityMultiplier(force + getTemplate()->getGravityMultiplier());
            }
        }
        else
        {
            //going back down
            m_fullAntigravTimer = Max(0.0f,m_fullAntigravTimer - _dt);
            if (m_fullAntigravTimer == 0.0f)
            {
                if (m_ejectionState == Cycle)
                    m_ejectionState = Fall;
                m_antigravRampDownTimer = Max(0.0f,m_antigravRampDownTimer - _dt);
                if (m_antigravRampDownTimer > 0.0f)
                {
                    m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier()-(m_antigravRampDownTimer / getTemplate()->getAntigravRampDownTime()));
                }
                else
                {
                    m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());
                }
            }
            else
            {
                m_physComponent->setGravityMultiplier(0.0f);
            }
        }
        if (m_ejectionState == Cycle)
            m_ejectionCycleCursor += _dt;
    }


    void Ray_AIGround_ReceiveUpperHitAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if (EventQueryBlocksHits* onQueryBlockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            onQueryBlockHits->setCanBlockHits(getTemplate()->getCanBlockHits());
        }
    }

    void Ray_AIGround_ReceiveUpperHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EjectionState,2291183735), static_cast<u32>(m_ejectionState));
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EjectionCycleCursor,2957783563), m_ejectionCycleCursor);
        }
    }

}