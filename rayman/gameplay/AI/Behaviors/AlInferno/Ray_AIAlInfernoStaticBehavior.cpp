#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/AlInferno/Ray_AIAlInfernoStaticBehavior.h"
#endif //_ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_

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

    IMPLEMENT_OBJECT_RTTI(Ray_AIAlInfernoStaticBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIAlInfernoStaticBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("uturn", m_uturn, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("cycleUturn", m_cycleUturn, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("idleTime", m_idleTime);
    SERIALIZE_OBJECT("detectionRange", m_detectionRange);
    SERIALIZE_MEMBER("attackOnDetection", m_attackOnDetection);
    SERIALIZE_MEMBER("stickOnWalls",m_stickOnWalls);
    END_SERIALIZATION()

    Ray_AIAlInfernoStaticBehavior_Template::Ray_AIAlInfernoStaticBehavior_Template()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_attack(NULL)
    , m_cycleUturn(NULL)
    , m_idleTime(2.0f)
    , m_attackOnDetection(bfalse)
    , m_stickOnWalls(bfalse)
    {
        
    }

    Ray_AIAlInfernoStaticBehavior_Template::~Ray_AIAlInfernoStaticBehavior_Template()
    {
        SF_DEL(m_idle);        
        SF_DEL(m_uturn);
        SF_DEL(m_attack);
        SF_DEL(m_cycleUturn);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIAlInfernoStaticBehavior)

    Ray_AIAlInfernoStaticBehavior::Ray_AIAlInfernoStaticBehavior()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_attack(NULL)
    , m_cycleUturn(NULL)
    , m_idleTimer(0.0f)
    , m_flameHandle(U32_INVALID)
    , m_active(btrue)
    {
    }

    Ray_AIAlInfernoStaticBehavior::~Ray_AIAlInfernoStaticBehavior()
    {
    }

    void Ray_AIAlInfernoStaticBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_idle = createAiAction(getTemplate()->getIdle());
        m_uturn = createAiAction(getTemplate()->getUturn());
        m_attack = createAiAction(getTemplate()->getAttack());
        m_cycleUturn= createAiAction(getTemplate()->getCycleUturn());

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),m_aiComponent);

        m_active = btrue;
    }

    void Ray_AIAlInfernoStaticBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        if (m_currentAction == m_idle)
        {
            updateIdle(_delta);
        }
        else if (m_currentAction == m_uturn)
        {

        }
        else if (m_currentAction == m_attack)
        {

        }
    }

    void Ray_AIAlInfernoStaticBehavior::onEvent(Event * _event)
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
        }
        if (EventActivate * onActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            m_active = onActivate->getActivated();
        }
    }

    void Ray_AIAlInfernoStaticBehavior::onActivate()
    {
        if (getTemplate()->getStickOnWalls())
            m_physComponent->setStickOnWalls(btrue);
        if (m_cycleUturn)
        {
            startCycleUturn();
        }
        else
        {
            m_idleTimer = getTemplate()->getIdleTime();
            startIdle();
        }
    }

    void Ray_AIAlInfernoStaticBehavior::onDeactivate()
    {
        Super::onDeactivate();
        if (getTemplate()->getStickOnWalls())
            m_physComponent->setStickOnWalls(bfalse);

        if (m_fxController)
            m_fxController->stopFX(m_flameHandle);
        m_flameHandle = U32_INVALID;

    }

    void Ray_AIAlInfernoStaticBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_uturn)
            onFinishedUturn();
        else if (m_currentAction == m_attack)
            onFinishedAttack();
        else if (m_currentAction == m_cycleUturn)
            onFinishedCycleUturn();
    }

    void Ray_AIAlInfernoStaticBehavior::startIdle()
    {
        setAction(m_idle);
    }


    void Ray_AIAlInfernoStaticBehavior::startUturn()
    {
        if (m_uturn)
        {
            m_animatedComponent->resetCurTime();
            setAction(m_uturn,btrue);
        }
        else 
            onFinishedUturn();
    }

    void Ray_AIAlInfernoStaticBehavior::onFinishedUturn()
    {
        m_idleTimer = getTemplate()->getIdleTime();
        if (m_uturn)
            m_actor->setFlipped(!m_actor->isFlipped());
        startIdle();
    }

    void Ray_AIAlInfernoStaticBehavior::startAttack()
    {
        if (m_attack)
        {
            m_animatedComponent->resetTree();
            m_animComponent->resetCurTime();
            setAction(m_attack,btrue);
        }
        else
            onFinishedAttack();
    }

    void Ray_AIAlInfernoStaticBehavior::onFinishedAttack()
    {
        if (m_cycleUturn)
            startCycleUturn();
        else
            startUturn();
    }

    void Ray_AIAlInfernoStaticBehavior::updateIdle(f32 _dt)
    {
        if (getTemplate()->getAttackOnDetection() )
        {
            if (Actor * enemy = AIUtils::getClosestEnemy(m_actor,m_actor->isFlipped(),getTemplate()->getDetectionRange()))
            {
                if (!m_animatedComponent->isInTransition())
                {
                    const Vec2d lookdir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
                    const Vec2d dir = enemy->get2DPos() - m_actor->get2DPos();
                    const f32 dot = lookdir.dot(dir);
                    if (dot < 0.0f )
                    {
                        if (m_uturn)
                        {
                            startUturn();
                            return;
                        }
                    }
                    else
                    {
                        startAttack();
                        return;
                    }
                }

            }
            else
            {
                if (m_idleTimer == 0.0f)
                {
                    if (m_cycleUturn)
                        startCycleUturn();
                    else
                        startUturn();
                    return;
                }

                m_idleTimer = Max(m_idleTimer-_dt,0.0f);
                return;
            }
        }
        else
        {
            if (m_idleTimer == 0.0f && m_active)
            {
                startAttack();
                return;
            }

            m_idleTimer = Max(m_idleTimer-_dt,0.0f);
        }
    }

    void Ray_AIAlInfernoStaticBehavior::startCycleUturn()
    {
        setAction(m_cycleUturn);
    }

    void Ray_AIAlInfernoStaticBehavior::onFinishedCycleUturn()
    {
        m_idleTimer = getTemplate()->getIdleTime();
        m_actor->setFlipped(!m_actor->isFlipped());
        startIdle();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIAlInfernoStaticBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        AABB aabb = getTemplate()->getDetectionRange();
        if (m_actor->isFlipped())
            aabb.FlipHorizontaly(bfalse);
        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::white(),0.0f,1.0f,"DetectionRange");
    }
#endif // ITF_SUPPORT_EDITOR
}
