#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveInAirHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_

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
    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveInAirHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGround_ReceiveInAirHitAction_Template)
        SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
        SERIALIZE_MEMBER("hitForce", m_hitForce);
        SERIALIZE_MEMBER("fullAntigravTime", m_fullAntigravTime);
        SERIALIZE_MEMBER("antigravRampDownTime", m_antigravRampDownTime);
        SERIALIZE_MEMBER("canBlockHits", m_canBlockHits);

    END_SERIALIZATION()

    Ray_AIGround_ReceiveInAirHitAction_Template::Ray_AIGround_ReceiveInAirHitAction_Template()
    : m_gravityMultiplier(1.0f)
    , m_hitForce(0.0f)
    , m_fullAntigravTime(0.0f)
    , m_antigravRampDownTime(0.0f)
    , m_canBlockHits(bfalse)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveInAirHitAction)

    BEGIN_VALIDATE_ACTION(Ray_AIGround_ReceiveInAirHitAction)
    END_VALIDATE_ACTION()

    Ray_AIGround_ReceiveInAirHitAction::Ray_AIGround_ReceiveInAirHitAction()
        : Super()
        , m_justEntered(bfalse)
        , m_prevFrictionMultiplier(1.f)
        , m_prevGravityMultiplier(1.f)
        , m_fullAntigravTimer(0.f)
        , m_antigravRampDownTimer(0.f)
        , m_ejectionState(Cycle)
        , m_ejectionCycleCursor(0.0f)
        , m_physComponent(NULL)
        , m_timeInCurrentState(0)
    {
    }

    Ray_AIGround_ReceiveInAirHitAction::~Ray_AIGround_ReceiveInAirHitAction()
    {
    }

    void Ray_AIGround_ReceiveInAirHitAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStimNotify,1337973749),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),m_aiComponent);
    }


    void Ray_AIGround_ReceiveInAirHitAction::onActivate()
    {
        Super::onActivate();


        // reset

        m_prevFrictionMultiplier = m_physComponent->getFrictionMultiplier();
        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();
        m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());

        m_justEntered = btrue;

        // setup phys
        m_physComponent->setSpeed(Vec2d::Zero);
        m_physComponent->resetForces();

        // setup hit force
        setupHitForce();


        // setup antigrav
        if (getTemplate()->getFullAntigravTime() > 0.0f || getTemplate()->getAntigravRampDownTime() > 0.0f)
        {
            m_fullAntigravTimer = getTemplate()->getFullAntigravTime();
            m_antigravRampDownTimer = getTemplate()->getAntigravRampDownTime();
            m_physComponent->setGravityMultiplier(0.0f);
            setEjectionState(Cycle);
        }
        else
        {
            setEjectionState(Fall);
        }
        m_ejectionCycleCursor = 0.0f;


#ifdef DEBUG_HIT_TRAJECTORY
        m_debugLastPos = m_actor->get2DPos();
#endif
    }

    void Ray_AIGround_ReceiveInAirHitAction::onDeactivate()
    {
        Super::onDeactivate();
        m_fullAntigravTimer = 0.0f;
        m_antigravRampDownTimer = 0.0f;
        m_physComponent->setFrictionMultiplier(m_prevFrictionMultiplier);
        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
    }


    void Ray_AIGround_ReceiveInAirHitAction::setupHitForce()
    {
        Vec2d force = getTemplate()->getHitForce() * m_hitDir;
        m_physComponent->addForce(force);

    }


    void Ray_AIGround_ReceiveInAirHitAction::update( f32 _dt )
    {
#ifdef DEBUG_HIT_TRAJECTORY
        const Vec2d& pos = m_actor->get2DPos();
        GFX_ADAPTER->drawDBGLine(m_debugLastPos, pos, Color::white(), 1.f);
        m_debugLastPos = pos;
#endif

        Super::update(_dt);

        updateHit(_dt);

        updateAntigrav(_dt);

        m_justEntered = bfalse;
    }

    void Ray_AIGround_ReceiveInAirHitAction::updateHit( f32 _dt )
    {
        m_timeInCurrentState += _dt;

        bbool isActionFinished = bfalse;
        if ( (m_justLanded && m_ejectionState != Land) || (m_ejectionState==Fall && m_timeInCurrentState>3/* TODO */) )
        {
            m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
            setEjectionState(Land);

        }
        // here so that we ignore landing on first frame
        Super::updateHit(_dt);

        if ( m_ejectionState == Land )
        {
            bbool mustFinish = isAnimFinished() && !m_justEntered && m_physComponent->getStickedEdge();
            mustFinish = mustFinish || m_timeInCurrentState > 4;
            if ( mustFinish )  // wait to touch the ground again when ejected
            {
                isActionFinished = btrue;                
            }
        }

        if (isActionFinished)
        {
            m_aiBehavior->onActionFinished();
        }

        if (m_ejectionState == Cycle)
            m_ejectionCycleCursor += _dt;
    }

    void Ray_AIGround_ReceiveInAirHitAction::setEjectionState(EjectionState _newState)
    {
        m_timeInCurrentState = 0;
        m_ejectionState = _newState;
    }

    void Ray_AIGround_ReceiveInAirHitAction::updateAntigrav( f32 _dt )
    {
        m_fullAntigravTimer = Max(0.0f,m_fullAntigravTimer - _dt);
        if (m_fullAntigravTimer == 0.0f)
        {
            if (m_ejectionState == Cycle)
                setEjectionState(Fall);
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

    void Ray_AIGround_ReceiveInAirHitAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if (EventQueryBlocksHits* onQueryBlockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
        {
            onQueryBlockHits->setCanBlockHits(getTemplate()->getCanBlockHits());
        }
    }


    void Ray_AIGround_ReceiveInAirHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EjectionState,2291183735), static_cast<u32>(m_ejectionState));
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EjectionCycleCursor,2957783563), m_ejectionCycleCursor);
        }
    }

}