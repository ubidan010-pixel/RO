#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFLYIDLEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFlyIdleAction.h"
#endif //_ITF_RAY_AIFLYIDLEACTION_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIFlyIdleAction)

    const f32 Ray_AIFlyIdleAction::m_calculateOffsetTime = 2.f;

    Ray_AIFlyIdleAction::Ray_AIFlyIdleAction()
        : Super()
        , m_physComponent(NULL)
        , m_startPos(Vec2d::Zero)
        , m_offset(Vec2d::Zero)
        , m_offsetTimer(0.f)
    {
    }

    Ray_AIFlyIdleAction::~Ray_AIFlyIdleAction()
    {
    }

    void Ray_AIFlyIdleAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {   
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<PhysComponent>();
        ITF_ASSERT(m_physComponent);
    }

    void Ray_AIFlyIdleAction::onActivate()
    {
        Super::onActivate();

        m_startPos = m_actor->get2DPos();
        m_offsetTimer = m_calculateOffsetTime;
    }

    void Ray_AIFlyIdleAction::onForceMove()
    {
        Super::onForceMove();

        if (m_actor)
            m_startPos = m_actor->get2DPos();
    }

    void Ray_AIFlyIdleAction::update( f32 _delta )
    {
        m_offsetTimer -= m_calculateOffsetTime;

        if ( m_offsetTimer < 0.f )
        {
            m_offsetTimer = m_calculateOffsetTime;

            m_offset.m_x = ( Seeder::getSharedSeeder().GetFloat() * 2.f ) - 1.f;
            m_offset.m_y = ( Seeder::getSharedSeeder().GetFloat() * 2.f ) - 1.f;
        }

        f32 stiff = getTemplate()->getStiff();
        f32 damp = getTemplate()->getDamp();

        const Vec2d& gravity = m_aiComponent->getCurrentGravity();
        Vec2d target = m_startPos + m_offset;
        Vec2d vecToTarget = target - m_actor->getPos().truncateTo2D();

        Vec2d flyForce = (vecToTarget * stiff) - ( m_physComponent->getSpeed() * damp);

        m_physComponent->addForce(flyForce - gravity);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIFlyIdleAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIFlyIdleAction_Template)
        SERIALIZE_MEMBER("stiff", m_stiff);
        SERIALIZE_MEMBER("damp", m_damp);
    END_SERIALIZATION()

    Ray_AIFlyIdleAction_Template::Ray_AIFlyIdleAction_Template()
        : Super()
        , m_stiff(2.0f)
        , m_damp(0.3f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(IDLEFLY,1408418246);
    }
}