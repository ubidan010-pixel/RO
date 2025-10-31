#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILOOKATBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AILookatBehavior.h"
#endif //_ITF_RAY_AILOOKATBEHAVIOR_H_

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

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AILookatBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AILookatBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("uturn", m_uturn, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("aggro", m_aggro, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("detectionRadius", m_detectionRadius);
    SERIALIZE_MEMBER("detectionHysteresisTime", m_detectionHysteresisTime);
    SERIALIZE_MEMBER("attackRadius", m_attackRadius);
    SERIALIZE_MEMBER("attackDelay", m_attackDelay);
    SERIALIZE_MEMBER("attackLookAt", m_attackLookAt);
    SERIALIZE_MEMBER("lookAtSmoothFactor", m_lookAtSmoothFactor);
    SERIALIZE_MEMBER("stickOnWalls", m_stickOnWalls);
    END_SERIALIZATION()

    Ray_AILookatBehavior_Template::Ray_AILookatBehavior_Template()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_aggro(NULL)
    , m_attack(NULL)
    , m_detectionRadius(5.0f)
    , m_detectionHysteresisTime(0.5f)
    , m_attackRadius(1.8f)
    , m_attackDelay(2.0f)
    , m_attackLookAt(bfalse)
    , m_lookAtSmoothFactor(1.0f)
    , m_stickOnWalls(bfalse)
    {
        
    }

    Ray_AILookatBehavior_Template::~Ray_AILookatBehavior_Template()
    {
        SF_DEL(m_idle);        
        SF_DEL(m_uturn);
        SF_DEL(m_aggro);
        SF_DEL(m_attack);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AILookatBehavior)

    Ray_AILookatBehavior::Ray_AILookatBehavior()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_aggro(NULL)
    , m_attack(NULL)
    , m_detectionHysteresisTimer(0.0f)
    , m_attackDelayTimer(0.0f)
    , m_flameHandle(U32_INVALID)
    , m_updateLookAt(btrue)
    , m_physComponent(NULL)
    {
    }

    Ray_AILookatBehavior::~Ray_AILookatBehavior()
    {
    }

    void Ray_AILookatBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_idle = createAiAction(getTemplate()->getIdle());
        m_uturn = createAiAction(getTemplate()->getUturn());
        m_aggro = createAiAction(getTemplate()->getAggro());
        m_attack = createAiAction(getTemplate()->getAttack());

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
    }

    void Ray_AILookatBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        Actor * player = AIUtils::getClosestLivePlayer(m_actor);

        if (m_currentAction == m_aggro)
        {
            updateAggro(player, _delta);
        }
        else if (m_currentAction == m_idle)
        {
            updateIdle(player);
        }
        else if (m_currentAction == m_attack)
        {
            updateAttack(player, _delta);
        }
    }

    void Ray_AILookatBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        if (AnimGameplayEvent * onGamePlayEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (onGamePlayEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Flame_Start,491367522))
            {
                if (m_fxController)
                    m_flameHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(MRK_Flame,2715706329));
            }
            else if (onGamePlayEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Flame_Stop,959055747))
            {
                if (m_fxController)
                    m_fxController->stopFX(m_flameHandle);
                m_flameHandle = U32_INVALID;
            }
            else if (onGamePlayEvent->getName() == ITF_GET_STRINGID_CRC(MRK_StopUpdateLookat,3668161387))
            {
                m_updateLookAt = bfalse;
            }
        }
    }

    void Ray_AILookatBehavior::onActivate()
    {
        //Super::onActivate();
        if (getTemplate()->getStickOnWalls())
            m_physComponent->setStickOnWalls(btrue);
        m_attackDelayTimer = 0.0f;
        startIdle();
    }

    void Ray_AILookatBehavior::onDeactivate()
    {
        Super::onDeactivate();
        if (getTemplate()->getStickOnWalls())
            m_physComponent->setStickOnWalls(bfalse);
        if (m_fxController)
            m_fxController->stopFX(m_flameHandle);
        m_flameHandle = U32_INVALID;
    }

    void Ray_AILookatBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_uturn)
            onFinishedUturn();
        else if (m_currentAction == m_attack)
            onFinishedAttack();
    }

    void Ray_AILookatBehavior::startIdle()
    {
        setAction(m_idle);
    }

    bbool Ray_AILookatBehavior::checkUturn(Actor * _player)
    {
        if (_player)
        {
            const Vec2d lookdir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
            const Vec2d dirToPlayer = _player->get2DPos() - m_actor->get2DPos();
            if (lookdir.dot(dirToPlayer) < 0.0f)
            {
                startUturn();
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_AILookatBehavior::startUturn()
    {
        if (m_uturn)
            setAction(m_uturn);
        else 
            onFinishedUturn();
    }

    void Ray_AILookatBehavior::onFinishedUturn()
    {
        m_actor->setFlipped(!m_actor->isFlipped());
    }

    void Ray_AILookatBehavior::startAggro()
    {
        m_detectionHysteresisTimer = getTemplate()->getDetectionHysteresisTime();
        setAction(m_aggro);
    }

    bbool Ray_AILookatBehavior::checkAggro(Actor * _player)
    {
        if (_player)
        {
            const f32 sqrDist = (_player->get2DPos() - m_actor->get2DPos()).sqrnorm();
            if ( sqrDist < (getTemplate()->getDetectionRadius() * getTemplate()->getDetectionRadius())
                && sqrDist > (getTemplate()->getAttackRadius() * getTemplate()->getAttackRadius())
                )
            {
                const Vec2d lookdir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
                const Vec2d dirToPlayer = _player->get2DPos() - m_actor->get2DPos();
                if (lookdir.dot(dirToPlayer) < 0.0f)
                {
                    m_actor->setFlipped(!m_actor->isFlipped());
                }
                startAggro();
                return btrue;
            }
        }
        return bfalse;
    }

    bbool Ray_AILookatBehavior::checkIdle( Actor * _player, f32 _dt )
    {
        if (!_player ||
            (_player->get2DPos() - m_actor->get2DPos()).sqrnorm() >= (getTemplate()->getDetectionRadius() * getTemplate()->getDetectionRadius()))
        {
            m_detectionHysteresisTimer = Max(m_detectionHysteresisTimer - _dt, 0.0f);
            if (m_detectionHysteresisTimer == 0.0f)
            {
                startIdle();
                return btrue;
            }
        }
        else
        {
            m_detectionHysteresisTimer = getTemplate()->getDetectionHysteresisTime();
        }
        return bfalse;
    }

    bbool Ray_AILookatBehavior::checkAttack( Actor * _player)
    {
        if (_player &&
            m_attackDelayTimer == 0.0f &&
            (_player->get2DPos() - m_actor->get2DPos()).sqrnorm() < (getTemplate()->getAttackRadius() * getTemplate()->getAttackRadius()))
        {
            startAttack();
            return btrue;
        }
        return bfalse;
    }

    void Ray_AILookatBehavior::startAttack()
    {
        m_updateLookAt = getTemplate()->getAttackLookAt();
        if (m_attack)
        {
            m_animatedComponent->resetTree();
            m_animComponent->resetCurTime();
            setAction(m_attack,btrue);
        }
        else
            onFinishedAttack();
    }

    void Ray_AILookatBehavior::updateAttack(Actor * _player, f32 _delta)
    {
        if (m_updateLookAt)
        {
            updateLookAt(_delta, _player);
        }
    }

    void Ray_AILookatBehavior::onFinishedAttack()
    {
        m_attackDelayTimer = getTemplate()->getAttackDelay();
        startAggro();
    }

    void Ray_AILookatBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs(  );
        f32 angle = m_lookAtAngle.ToRadians();
        angle -= m_actor->getAngle();
        if (m_actor->isFlipped())
            angle = MTH_PI -angle;
        angle = canonizeAngle(angle);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(lookAtAngle,1473755958),angle * MTH_RADTODEG);

    }

    void Ray_AILookatBehavior::updateLookAt( f32 _dt, Actor * _player )
    {
        if (!_player)
        {
            return;
        }
    
        m_target = _player->getRef();
        Vec2d lookDir = _player->get2DPos() - m_actor->get2DPos();
        lookDir.normalize();

        const Vec2d myLookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
        if (lookDir.dot(myLookDir) < 0.0f)
            m_actor->setFlipped(!m_actor->isFlipped());

        f32 angle = lookDir.getAngle();
 /*       if (m_actor->isFlipped())
            angle = MTH_PI -angle;*/
        if (m_actor->isFlipped())
            angle = f32_Modulo(canonizeAngle(angle) + MTH_2PI,MTH_2PI);
        else
            angle = canonizeAngle(angle) ;
        
        f32 oldAngle = m_lookAtAngle.ToRadians();
        m_lookAtAngle.SetRadians(AIUtils::michelSmooth(
            oldAngle,
            angle,
            getTemplate()->getLookAtSmoothFactor()));
    }

    void Ray_AILookatBehavior::updateAggro( Actor * _player, f32 _delta )
    {
        m_attackDelayTimer = Max(m_attackDelayTimer-_delta,0.0f);
        if (checkAttack(_player))
            return;
        if (checkIdle(_player, _delta))
            return;

        updateLookAt(_delta, _player);
    }

    void Ray_AILookatBehavior::updateIdle( Actor * _player ) 
    {
        if (checkAggro(_player))
            return;
        if (checkAttack(_player))
            return;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AILookatBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        DebugDraw::circle(m_actor->getPos(),getTemplate()->getDetectionRadius(),Color::pink(),0.0f,"DetectionRadius");
        DebugDraw::circle(m_actor->getPos(),getTemplate()->getAttackRadius(),Color::green(),0.0f,"AttackRadius");

    }
#endif // ITF_SUPPORT_EDITOR
}

