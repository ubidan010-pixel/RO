#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WALKINGBULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_WalkingBulletAIComponent.h"
#endif //_ITF_RAY_WALKINGBULLETAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WalkingBulletAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WalkingBulletAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_WalkingBulletAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "Ray_WalkingBulletAIComponent requires a StickToPolylinePhysComponent");
    END_VALIDATE_COMPONENT()

    Ray_WalkingBulletAIComponent::Ray_WalkingBulletAIComponent()
        : Super()
        , m_physComponent(NULL)
    {
    }

    Ray_WalkingBulletAIComponent::~Ray_WalkingBulletAIComponent()
    {
    }

    void Ray_WalkingBulletAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void Ray_WalkingBulletAIComponent::Update( f32 _dt )
    {
        AIWalkInDirAction* walk = m_behavior->getCurrentAction() ? m_behavior->getCurrentAction()->DynamicCast<AIWalkInDirAction>(ITF_GET_STRINGID_CRC(AIWalkInDirAction,2047489259)) : NULL;
        if (walk)
        {
            walk->setWalkLeft(m_actor->isFlipped());
            walk->setMaxSpeed(getTemplate()->getSpeed());
        }

        Super::Update(_dt);
    }

    void Ray_WalkingBulletAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        static StringID s_IsFlying = "IsFlying";
        m_animatedComponent->setInput(s_IsFlying, m_physComponent->getStickedPolyline() == NULL);
    }

    void Ray_WalkingBulletAIComponent::updateMovement( f32 _dt )
    {
        if (m_currentBehavior == m_launchBehavior)
        {
            return;
        }

        if ( m_physComponent->getStickedPolyline() )
        {
            // counter gravity
            m_physComponent->addForce( -getCurrentGravity() );
#ifndef ITF_FINAL
            //DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() - getCurrentGravity(), m_actor->getDepth(), Color::green());
#endif // ITF_FINAL

            // add a force normal to the edge
            Vec2d groundNormal = m_physComponent->getCurrentGroundDir().getPerpendicular();
            m_physComponent->addForce( -groundNormal * getTemplate()->getStickForce() );
#ifndef ITF_FINAL
            //DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() - groundNormal * getTemplate()->getStickForce(), m_actor->getDepth(), Color::blue());
#endif // ITF_FINAL
            
            // maintain target speed
            m_direction = AIUtils::getFlipped(m_actor) ? -Vec2d::Right : Vec2d::Right;
            m_direction = m_direction.Rotate(groundNormal.getOrientedAngle(Vec2d::Up));
            Vec2d targetSpeed = m_direction * getTemplate()->getSpeed();
            Vec2d moveForce = (targetSpeed - m_physComponent->getSpeed()) / _dt;
            m_physComponent->addForce(moveForce);
#ifndef ITF_FINAL
            //DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + moveForce, m_actor->getDepth(), Color::red());
#endif // ITF_FINAL
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WalkingBulletAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_WalkingBulletAIComponent_Template)
        SERIALIZE_MEMBER("stickForce", m_stickForce);
    END_SERIALIZATION()

    Ray_WalkingBulletAIComponent_Template::Ray_WalkingBulletAIComponent_Template()
    :  m_stickForce(10.f)
    {
    }

};