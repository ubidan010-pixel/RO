#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneStaticBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_

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

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneStaticBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AILivingStoneStaticBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("uturn", m_uturn, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("aggro", m_aggro, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("detectionRadius", m_detectionRadius);
    SERIALIZE_MEMBER("detectionHysteresisTime", m_detectionHysteresisTime);
    SERIALIZE_OBJECT("attackRange", m_attackRange);
    END_SERIALIZATION()

    Ray_AILivingStoneStaticBehavior_Template::Ray_AILivingStoneStaticBehavior_Template()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_aggro(NULL)
    , m_attack(NULL)
    , m_detectionRadius(5.0f)
    , m_detectionHysteresisTime(0.5f)
    {
        
    }

    Ray_AILivingStoneStaticBehavior_Template::~Ray_AILivingStoneStaticBehavior_Template()
    {
        SF_DEL(m_idle);        
        SF_DEL(m_uturn);
        SF_DEL(m_aggro);
        SF_DEL(m_attack);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneStaticBehavior)

    Ray_AILivingStoneStaticBehavior::Ray_AILivingStoneStaticBehavior()
    : m_idle(NULL)
    , m_uturn(NULL)
    , m_aggro(NULL)
    , m_attack(NULL)
    , m_timer(0.0f)
    {
    }

    Ray_AILivingStoneStaticBehavior::~Ray_AILivingStoneStaticBehavior()
    {
    }

    void Ray_AILivingStoneStaticBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_idle = createAiAction(getTemplate()->getIdle());
        m_uturn = createAiAction(getTemplate()->getUturn());
        m_aggro = createAiAction(getTemplate()->getAggro());
        m_attack = createAiAction(getTemplate()->getAttack());
    }

    void Ray_AILivingStoneStaticBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        Actor * player = AIUtils::getClosestLivePlayer(m_actor);

        if (m_currentAction == m_aggro)
        {
            if (!player)
                return;
            if (checkAttack(player))
                return;
            if (checkIdle(player, _delta))
                return;
            if (checkUturn(player))
                return;
        }
        else if (m_currentAction != m_attack)
            checkAggro(player);
    }

    void Ray_AILivingStoneStaticBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
    }

    void Ray_AILivingStoneStaticBehavior::onActivate()
    {
        //Super::onActivate();
        startIdle();
    }

    void Ray_AILivingStoneStaticBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }

    void Ray_AILivingStoneStaticBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_uturn)
            onFinishedUturn();
        else if (m_currentAction == m_attack)
            onFinishedAttack();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AILivingStoneStaticBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        DebugDraw::circle(m_actor->getPos(),getTemplate()->getDetectionRadius(),Color::pink(),0.0f,"DetectionRadius");
        AABB aabb;
        getAttackRange(aabb);

        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::green(),0.0f,1.0f,"AttackRange");
    }
#endif // ITF_SUPPORT_EDITOR

    void Ray_AILivingStoneStaticBehavior::startIdle()
    {
        setAction(m_idle);
    }

    bbool Ray_AILivingStoneStaticBehavior::checkUturn(Actor * _player)
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

    void Ray_AILivingStoneStaticBehavior::startUturn()
    {
        if (m_uturn)
            setAction(m_uturn);
        else 
            onFinishedUturn();
    }

    void Ray_AILivingStoneStaticBehavior::onFinishedUturn()
    {
        m_actor->setFlipped(!m_actor->isFlipped());
    }

    void Ray_AILivingStoneStaticBehavior::startAggro()
    {
        m_timer = getTemplate()->getDetectionHysteresisTime();
        setAction(m_aggro);
    }

    void Ray_AILivingStoneStaticBehavior::checkAggro(Actor * _player)
    {
        if (_player)
        {
            if ((_player->get2DPos() - m_actor->get2DPos()).sqrnorm() < (getTemplate()->getDetectionRadius() * getTemplate()->getDetectionRadius()))
            {
                const Vec2d lookdir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
                const Vec2d dirToPlayer = _player->get2DPos() - m_actor->get2DPos();
                if (lookdir.dot(dirToPlayer) < 0.0f)
                {
                    m_actor->setFlipped(!m_actor->isFlipped());
                }
                startAggro();
            }
        }
    }

    bbool Ray_AILivingStoneStaticBehavior::checkIdle( Actor * player, f32 _dt )
    {
        if ((player->get2DPos() - m_actor->get2DPos()).sqrnorm() >= (getTemplate()->getDetectionRadius() * getTemplate()->getDetectionRadius()))
        {
            m_timer = Max(m_timer - _dt, 0.0f);
            if (m_timer == 0.0f)
            {
                startIdle();
                return btrue;
            }
        }
        else
        {
            m_timer = getTemplate()->getDetectionHysteresisTime();
        }
        return bfalse;
    }

    bbool Ray_AILivingStoneStaticBehavior::checkAttack( Actor * _player)
    {
        AABB aabb;
        getAttackRange(aabb);
        
        if (aabb.contains(_player->get2DPos() - m_actor->get2DPos()))
        {
            startAttack();
            return btrue;
        }
        return bfalse;
    }

    void Ray_AILivingStoneStaticBehavior::startAttack()
    {
        if (m_attack)
        {
            m_animComponent->resetCurTime();
            setAction(m_attack,btrue);
        }
        else
            onFinishedAttack();
    }

    void Ray_AILivingStoneStaticBehavior::onFinishedAttack()
    {
        startAggro();
    }

    void Ray_AILivingStoneStaticBehavior::getAttackRange( AABB & _range ) const
    {
        _range = getTemplate()->getAttackRange();
        _range.Scale(m_actor->getScale());
        if (m_actor->isFlipped())
            _range.FlipHorizontaly(bfalse);
    }
}

