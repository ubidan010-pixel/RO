#include "precompiled_gameplay.h"

#ifndef _ITF_AIBOUNCETOLAYERACTION_H_
#include "gameplay/ai/Actions/AIBounceToLayerAction.h"
#endif //_ITF_AIBOUNCETOLAYERACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIBounceToLayerAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIBounceToLayerAction_Template)
    END_SERIALIZATION()

        AIBounceToLayerAction_Template::AIBounceToLayerAction_Template()
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(BOUNCE,421809396);
    }

    IMPLEMENT_OBJECT_RTTI(AIBounceToLayerAction)

    BEGIN_VALIDATE_ACTION(AIBounceToLayerAction)
        VALIDATE_ACTION_PARAM("", m_physComponent, "AIJumpAction requires a StickToPolylinePhysComponent");
    END_VALIDATE_ACTION()


    AIBounceToLayerAction::AIBounceToLayerAction()
        : m_physComponent(NULL)
        , m_target(ITF_INVALID_OBJREF)
        , m_startPos(Vec3d::Zero)
        , m_totalTime(0.f)
        , m_speed(7.f)
        , m_timeCounter(0.f)
        , m_height(3.f)
        , m_height2(3.f)
        , m_targetOffset(Vec2d::Zero)
        , m_gravity(0.f)
        , m_currentSpeed(Vec2d::Zero)
        , m_prevPos(Vec2d::Zero)
        , m_targetPoint(Vec3d::Zero)
    {
    }

    AIBounceToLayerAction::~AIBounceToLayerAction()
    {
    }

    void AIBounceToLayerAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void AIBounceToLayerAction::onActivate()
    {
        Super::onActivate();

        m_startPos = m_actor->getPos();
        m_prevPos = m_actor->get2DPos();
        m_physComponent->setDisabled(btrue,btrue,btrue);
        m_gravity = m_physComponent->getCurrentGravity().norm();
        m_targetPoint = getTargetPosition();

        f32 dist = ( m_startPos - m_targetPoint ).norm();

        m_totalTime = dist / m_speed;
    }

    void AIBounceToLayerAction::onDeactivate()
    {
        Super::onDeactivate();

        m_physComponent->setDisabled(bfalse);
    }
    void AIBounceToLayerAction::update( f32 _delta )
    {
        Super::update(_delta);

        if ( m_timeCounter >= m_totalTime )
        {
            m_aiBehavior->onActionFinished();
        }

        Actor* target = AIUtils::getActor(m_target);

        if ( !target )
        {
            m_aiComponent->onBehaviorFinished();
        }

        m_timeCounter += _delta;

        m_targetPoint = getTargetPosition();

        updatePosition(_delta);
        updateSpeed(_delta);

        m_prevPos = m_actor->get2DPos();
    }


    void AIBounceToLayerAction::updatePosition( f32 _dt )
    {
        Vec3d bPoint0 = ( m_targetPoint + m_startPos ) * 0.5f;
        bPoint0 += Vec3d::Up * m_height;
        Vec3d bPoint1 = m_targetPoint + (Vec3d::Up*m_height2);

        f32 t = m_timeCounter / m_totalTime;
        Vec3d currentP = getBezierPosition(m_startPos,bPoint0,bPoint1,m_targetPoint,t);

        m_actor->setPos(currentP);
    }

    void AIBounceToLayerAction::updateSpeed( f32 _dt )
    {
        Vec2d dist = m_actor->get2DPos() - m_prevPos;
        Vec2d speed = dist / _dt;

        m_physComponent->setSpeed(speed);
    }

    Vec3d AIBounceToLayerAction::getTargetPosition() const
    {
        Vec2d targetPos = m_targetPoint.truncateTo2D();
        f32 targetZ = m_targetPoint.m_z;
        Actor* target = AIUtils::getActor(m_target);

        if ( target )
        {
            Scene* targetScene = target->getScene();

            targetPos = target->get2DPos() + m_targetOffset;
            targetZ = m_actor->getTemplate()->getForcedZ();

            if ( targetScene )
            {
                SubSceneActor* subSceneActor = targetScene->getSubSceneActorHolder();

                if ( subSceneActor )
                {
                    targetZ += subSceneActor->getPos().m_z;
                }
            }
        }

        return Vec3d(targetPos.m_x,targetPos.m_y,targetZ);
    }

    void AIBounceToLayerAction::setData( f32 _speed, f32 _height, f32 _height2, ObjectRef _target, const Vec2d& _offset )
    {
        m_speed = _speed;
        m_timeCounter = 0.f;
        m_height = _height;
        m_height2 = _height2;
        m_target = _target;
        m_targetOffset = _offset;
    }

}