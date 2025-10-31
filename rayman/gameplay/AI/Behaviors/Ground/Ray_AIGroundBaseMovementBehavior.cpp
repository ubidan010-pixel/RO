#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIBOUNCETOLAYERACTION_H_
#include "gameplay/ai/Actions/AIBounceToLayerAction.h"
#endif //_ITF_AIBOUNCETOLAYERACTION_H_

#ifndef _ITF_AIBUMPERACTION_H_
#include "gameplay/ai/Actions/AIBumperAction.h"
#endif //_ITF_AIBUMPERACTION_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseMovementBehavior_Template)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(Ray_AIGroundBaseMovementBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("move", m_move, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("uturn", m_uturn, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("bounceToLayer", m_bounceToLayer, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("bounce", m_bounce, ACTORSMANAGER->getAIActionsFactory());

        SERIALIZE_MEMBER("obstacleDetectionRange", m_obstacleDetectionRange);
        SERIALIZE_MEMBER("obstacleJumpHeight", m_obstacleJumpHeight);
        SERIALIZE_MEMBER("holeDetectionRange", m_holeDetectionRange);
        SERIALIZE_MEMBER("holeJumpDepth", m_holeJumpDepth);
        SERIALIZE_MEMBER("stuckDelay", m_stuckDelay);
        SERIALIZE_MEMBER("uturnDelay", m_uturnDelay);
    END_SERIALIZATION()
        Ray_AIGroundBaseMovementBehavior_Template::Ray_AIGroundBaseMovementBehavior_Template()
        : Super()
        , m_idle(NULL)
        , m_move(NULL)
        , m_uturn(NULL)
        , m_bounceToLayer(NULL)
        , m_bounce(NULL)
        , m_obstacleDetectionRange(1.5f)
        , m_obstacleJumpHeight(2.f)
        , m_holeDetectionRange(1.5f)
        , m_holeJumpDepth(3.f)
        , m_stuckDelay(0.5f)
        , m_uturnDelay(0.1f)
    {

    }

    Ray_AIGroundBaseMovementBehavior_Template::~Ray_AIGroundBaseMovementBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_move);
        SF_DEL(m_uturn);
        SF_DEL(m_bounceToLayer);
        SF_DEL(m_bounce);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseMovementBehavior)
    Ray_AIGroundBaseMovementBehavior::Ray_AIGroundBaseMovementBehavior()
    : Super()
    , m_stuckTimer(0.f)
    , m_uturnTimer(0.f)
    , m_idle(NULL)
    , m_move(NULL)
    , m_uturn(NULL)
    , m_bounceToLayer(NULL)
    , m_bounce(NULL)
    , m_bouncingToLayer(bfalse)
    , m_bouncing(bfalse)
    {
    }

    Ray_AIGroundBaseMovementBehavior::~Ray_AIGroundBaseMovementBehavior()
    {
    }

    void Ray_AIGroundBaseMovementBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_uturn && m_currentAction == m_uturn)
        {
            onActionFinishedUturn();
        }
        else if (m_bounceToLayer && m_currentAction == m_bounceToLayer)
        {
            onActionFinishedBounceToLayer();
        }
        else if (m_bounce && m_currentAction == m_bounce)
        {
            onActionFinishedBounce();
        }
        else if (m_move && m_currentAction == m_move)
        {
            onActionFinishedMove();
        }
    }


    void Ray_AIGroundBaseMovementBehavior::onActivate()
    {
        m_stuckTimer = 0.f;
        m_uturnTimer = 0.f;
    }

    void Ray_AIGroundBaseMovementBehavior::update( f32 _dt )
    {
        f32 prevUTurnTimer = m_uturnTimer;

        Super::update(_dt);

        if ( prevUTurnTimer == m_uturnTimer )
        {
            m_uturnTimer = 0.f;
        }

        if (m_bouncingToLayer)
        {
            m_bouncingToLayer = bfalse;
            startBounceToLayer();
            return;

        }

        if (m_bouncing)
        {
            m_bouncing = bfalse;
            startBounce();
        }

    }


    bbool Ray_AIGroundBaseMovementBehavior::queryUTurn( f32 _dt )
    {
        bbool canUTurn = bfalse;

        if ( m_uturnTimer == 0 )
        {
            m_uturnTimer = getTemplate()->getUturnDelay();
        }
        else
        {
            m_uturnTimer = Max(m_uturnTimer - _dt, 0.f);

            if ( m_uturnTimer == 0 )
            {
                canUTurn = btrue;
            }
        }

        return canUTurn;
    }

    void Ray_AIGroundBaseMovementBehavior::performUTurn( f32 _dt )
    {
        if ( queryUTurn(_dt) )
        {
            performUTurnImmediate();
        }
        else
        {
            setAction(m_idle);
        }
    }

    void Ray_AIGroundBaseMovementBehavior::performUTurnImmediate()
    {
        Ray_EventUturn uturn;
        m_aiComponent->onEvent(&uturn);
        if (m_uturn)
        {
            bbool reset = bfalse;
            if (m_currentAction == m_uturn)
            {
                reset = btrue;
                //m_animatedComponent->setFlipped(!m_actor->isFlipped());
            }
            setAction(m_uturn,reset);
            m_animatedComponent->resetCurTime();    // TODO: should this go in setAction?
        }
        else
        {
            onActionFinishedUturn();
        }
    }

    void Ray_AIGroundBaseMovementBehavior::onActionFinishedUturn()
    {
        m_actor->setFlipped( !m_actor->isFlipped() );
    }

    void Ray_AIGroundBaseMovementBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_idle = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getIdle()));
        m_move = static_cast<AIWalkInDirAction*>(createAiAction(getTemplate()->getMove()));
        m_uturn = static_cast<AIUTurnAction*>(createAiAction(getTemplate()->getUturn()));
        m_bounceToLayer = static_cast<AIBounceToLayerAction*>(createAiAction(getTemplate()->getBounceToLayer()));
        m_bounce = static_cast<AIBumperAction*>(createAiAction(getTemplate()->getBounce()));


        Super::onActorLoaded( _hotReload );

        if (m_bounceToLayer)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527),m_aiComponent);
        }

        if (m_bounce)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(BounceStim,819805069),m_aiComponent);
        }
    }

    void Ray_AIGroundBaseMovementBehavior::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (Ray_EventBounceToLayer * bounce = _event->DynamicCast<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527)))
        {
            if (m_bounceToLayer)
            {
                m_bouncingToLayer = btrue;
                m_bounceToLayer->setData(bounce->getSpeed(),bounce->getHeight(), bounce->getHeight2(),bounce->getTarget(),bounce->getOffset());
            }
        }
        else if (BounceStim * bounceStim = _event->DynamicCast<BounceStim>(ITF_GET_STRINGID_CRC(BounceStim,819805069)))
        {
            if (m_bounce)
            {
                m_bouncing = btrue;
                m_bounce->setData(bounceStim->getBounceDir());
            }
        }


    }

    void Ray_AIGroundBaseMovementBehavior::startBounceToLayer()
    {
        setAction(m_bounceToLayer);
        playBounceFx();
    }

    void Ray_AIGroundBaseMovementBehavior::startBounce()
    {
        setAction(m_bounce,btrue);
        playBounceFx();
    }

    void Ray_AIGroundBaseMovementBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_bouncingToLayer = bfalse;
        m_bouncing = bfalse;
    }

    void Ray_AIGroundBaseMovementBehavior::playBounceFx()
    {
        if (m_fxController)
        {
            PolyLine* poly;
            const PolyLineEdge* edge;
            AIUtils::getPolyLine(m_physComponent->getStickedPolylineRef(), m_physComponent->getStickedEdgeIndex(), poly, edge);
            if (poly)
            {
                if (poly && edge)
                {
                    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
                    u32 handle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(bounceFx,3317830063),mat);
                    m_fxController->setFXPos(handle,m_actor->getBase());
                }                
            }
        }
    }

}