#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveHitLaunchBulletAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveHitLaunchBulletAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGround_ReceiveHitLaunchBulletAction_Template)
        SERIALIZE_MEMBER("bulletPath",m_bulletPath);
        SERIALIZE_MEMBER("launcherBoneName",m_launcherBoneName);
        SERIALIZE_MEMBER("launcherMarker",m_launcherMarker);
        SERIALIZE_MEMBER("bulletSpeed",m_bulletSpeed);
    END_SERIALIZATION()

    Ray_AIGround_ReceiveHitLaunchBulletAction_Template::Ray_AIGround_ReceiveHitLaunchBulletAction_Template()
    : Super()
    , m_bulletPath(Path::EmptyPath)
    , m_launcherBoneName(StringID::Invalid)
    , m_launcherMarker(StringID::Invalid)
    , m_bulletSpeed(0.f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGround_ReceiveHitLaunchBulletAction)

    BEGIN_VALIDATE_ACTION(Ray_AIGround_ReceiveHitLaunchBulletAction)
    END_VALIDATE_ACTION()

    Ray_AIGround_ReceiveHitLaunchBulletAction::Ray_AIGround_ReceiveHitLaunchBulletAction()
    : Super()
    , m_launchBullet(bfalse)
    , m_bulletExitBoneIndex(U32_INVALID)
    , m_spawner()
    {
    }

    Ray_AIGround_ReceiveHitLaunchBulletAction::~Ray_AIGround_ReceiveHitLaunchBulletAction()
    {
    }

    void Ray_AIGround_ReceiveHitLaunchBulletAction::onActorLoaded(Pickable::HotReloadType _reload)
    {
        Super::onActorLoaded(_reload);
        SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner, getTemplate()->getBulletPath());
    }

    void Ray_AIGround_ReceiveHitLaunchBulletAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == getTemplate()->getLauncherMarker() )
            {
                m_launchBullet = btrue;
            }
        }
    }

    void Ray_AIGround_ReceiveHitLaunchBulletAction::update(f32 _dt)
    {
        Super::update(_dt);

        if ( m_launchBullet )
        {
            launchBullet();
            m_launchBullet = bfalse;
        }
    }

    void Ray_AIGround_ReceiveHitLaunchBulletAction::launchBullet()
    {    
        Vec2d bulletPos = Vec2d::Zero;
        Vec2d bulletDirection = Vec2d::Zero;
        if (m_animComponent && m_animComponent->isLoaded())
        {
            ITF_ASSERT_MSG(getTemplate()->getLauncherBoneName().isValid(), "you have to set the bulletExitBone!");
            if (m_bulletExitBoneIndex == U32_INVALID)
            {
                m_bulletExitBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getLauncherBoneName());
                ITF_ASSERT_MSG(m_bulletExitBoneIndex != U32_INVALID, "bulletExitBone not found!");
            }

            m_animComponent->getBonePos(m_bulletExitBoneIndex, bulletPos);
            m_animComponent->getBoneOrientation(m_bulletExitBoneIndex, bulletDirection);
        }
        else
        {
            bulletPos = m_actor->get2DPos();
        }

        f32 bulletAngle = m_actor->isFlipped()?MTH_PI-bulletDirection.getOrientedAngle(Vec2d::Right):bulletDirection.getOrientedAngle(Vec2d::Right);

        //apply speed to bullet direction
        bulletDirection *= getTemplate()->getBulletSpeed();

        // spawn the bullet in the root scene so they don't move with subscenes
        Actor* bullet = m_spawner.getSpawnee( CURRENTWORLD->getRootScene(), bulletPos.to3d(m_actor->getDepth()), bulletAngle);
        ITF_ASSERT(bullet);
        if (bullet) // crashed on 2010 Nov 10th while grouping objects
        {
            bullet->enable();

            EventSetDirection direction(bulletDirection);
            direction.setSender(m_actor->getRef());
            bullet->onEvent(&direction);

            Ray_EventHunterBulletLaunched bulletLaunched;
            bulletLaunched.setBuller(bullet->getRef());
            m_actor->onEvent(&bulletLaunched);
        }
    }
}
