#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIHunterLaunchBulletAction.h"
#endif //_ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIHunterLaunchBulletAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIHunterLaunchBulletAction_Template)
        SERIALIZE_MEMBER("endMarker", m_performHitEndMarker);
        SERIALIZE_MEMBER("bulletExitBone", m_bulletExitBoneName);
        SERIALIZE_MEMBER("bullet", m_bulletPath);
        SERIALIZE_MEMBER("offset", m_offset);
        SERIALIZE_MEMBER("useBoneOrientation", m_useBoneOrientation);
        SERIALIZE_MEMBER("launchSpeed", m_launchSpeed);
    END_SERIALIZATION()

    Ray_AIHunterLaunchBulletAction_Template::Ray_AIHunterLaunchBulletAction_Template()
    : m_performHitEndMarker("MRK_Hit_Performed")
    , m_offset(0)
    , m_launchSpeed(1.0f)
    , m_useBoneOrientation(bfalse)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIHunterLaunchBulletAction)

    Ray_AIHunterLaunchBulletAction::Ray_AIHunterLaunchBulletAction()
        : Super()
        , m_performHitEnded(bfalse)
        , m_bulletExitBoneIndex(U32_INVALID)
        , m_victim(ITF_INVALID_OBJREF)
    {
    }

    Ray_AIHunterLaunchBulletAction::~Ray_AIHunterLaunchBulletAction()
    {
    }

    void Ray_AIHunterLaunchBulletAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        SPAWNER->declareNeedsSpawnee(m_actor, &m_bulletSpawner, getTemplate()->getBulletPath());
    }

    void Ray_AIHunterLaunchBulletAction::onActivate()
    {
        Super::onActivate();

        m_performHitEnded = bfalse;
    }

    void Ray_AIHunterLaunchBulletAction::update( f32 _dt )
    {
        Super::update(_dt);

        if ( m_performHitEnded )
        {
            m_aiBehavior->onActionFinished();
        }
    }

    void Ray_AIHunterLaunchBulletAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == getTemplate()->getPerformHitEndMarker() )
            {
                m_performHitEnded = btrue;
            }
        }
    }

    void Ray_AIHunterLaunchBulletAction::performHit()
    {
        Vec2d bulletPos;
        Vec2d bulletDirection = m_direction;

        if (m_animComponent->isLoaded())
        {
            ITF_ASSERT_MSG(getTemplate()->getBulletExitBoneName().isValid(), "you have to set the bulletExitBone!");
            if (m_bulletExitBoneIndex == U32_INVALID)
            {
                m_bulletExitBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getBulletExitBoneName());
                ITF_ASSERT_MSG(m_bulletExitBoneIndex != U32_INVALID, "bulletExitBone not found!");
            }

            m_animComponent->getBonePos(m_bulletExitBoneIndex, bulletPos);
            if( getTemplate()->useBoneOrientation() )
            {
                m_animComponent->getBoneOrientation(m_bulletExitBoneIndex, bulletDirection);
            }
        }
        else
        {
            bulletPos = m_actor->get2DPos();
        }

        f32 bulletAngle = m_actor->isFlipped()?MTH_PI-bulletDirection.getOrientedAngle(Vec2d::Right):bulletDirection.getOrientedAngle(Vec2d::Right);

        //apply speed to bullet direction
        bulletDirection *= getTemplate()->getLaunchSpeed();

        // spawn the bullet in the root scene so they don't move with subscenes
        Actor* bullet = m_bulletSpawner.getSpawnee( CURRENTWORLD->getRootScene(), bulletPos.to3d(m_actor->getDepth()), bulletAngle);
        ITF_ASSERT(bullet);
        if (bullet) // crashed on 2010 Nov 10th while grouping objects
        {
            bullet->enable();

            EventSetDirection direction(bulletDirection);
            direction.setSender(m_actor->getRef());
            bullet->onEvent(&direction);

            EventSetTarget target(m_victim, Vec2d::Up * getTemplate()->getOffset());
            target.setSender(m_actor->getRef());
            bullet->onEvent(&target);

            Ray_EventHunterBulletLaunched bulletLaunched;
            bulletLaunched.setBuller(bullet->getRef());
            m_actor->onEvent(&bulletLaunched);
        }
    }

}