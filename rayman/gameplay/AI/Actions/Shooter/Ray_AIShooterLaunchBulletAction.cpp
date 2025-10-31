#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIShooterLaunchBulletAction.h"
#endif //_ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterLaunchBulletAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIShooterLaunchBulletAction_Template)
        //SERIALIZE_MEMBER("endMarker", m_performHitEndMarker);
        //SERIALIZE_MEMBER("minBurstCount", m_minBurstCount);
        //SERIALIZE_MEMBER("maxBurstCount", m_maxBurstCount);
        SERIALIZE_MEMBER("bulletExitBone", m_bulletExitBoneName);
        SERIALIZE_MEMBER("useBonesEnd", m_useBonesEnd );
        SERIALIZE_MEMBER("bullet", m_bulletPath);
        SERIALIZE_MEMBER("offset", m_offset);
    END_SERIALIZATION()

    Ray_AIShooterLaunchBulletAction_Template::Ray_AIShooterLaunchBulletAction_Template()
    //: m_performHitEndMarker("MRK_Hit_Performed")
    //, m_minBurstCount(1)
    //, m_maxBurstCount(1)
    : m_offset(0)
    , m_useBonesEnd( bfalse )
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterLaunchBulletAction)

    Ray_AIShooterLaunchBulletAction::Ray_AIShooterLaunchBulletAction()
        : Super()
        , m_performHitEnded(bfalse)
        /*, m_burstCount(U32_INVALID)
        , m_burstIndex(U32_INVALID)*/
        , m_bulletExitBoneIndex(U32_INVALID)
        , m_victim(ITF_INVALID_OBJREF)
    {
    }

    Ray_AIShooterLaunchBulletAction::~Ray_AIShooterLaunchBulletAction()
    {
    }

    void Ray_AIShooterLaunchBulletAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        SPAWNER->declareNeedsSpawnee(m_actor, &m_bulletSpawner, getTemplate()->getBulletPath());
    }

    void Ray_AIShooterLaunchBulletAction::onActivate()
    {
        m_animComponent->resetCurTime( );
        Super::onActivate();
        m_performHitEnded = bfalse;
        m_performHit = bfalse;

        /*m_burstCount = Seeder::getSharedSeeder().GetU32(getTemplate()->getMinBurstCount(),getTemplate()->getMaxBurstCount());
        ITF_ASSERT(m_burstCount > 0);
        m_burstIndex = 0;*/
    }

    void Ray_AIShooterLaunchBulletAction::update( f32 _dt )
    {
        //Super::update(_dt);

        if ( m_performHitEnded )
        {
            m_aiBehavior->onActionFinished();
        }
        
        if ( m_performHit )
        {
            performHit();
            m_performHit = bfalse;
        }

        /*
        if ( m_performHitEnded )
        {
            m_performHitEnded = bfalse;

            m_burstIndex++;
            if (m_burstIndex < m_burstCount)
            {
                // play again
                m_animComponent->resetCurTime();
            }
            else
            {
                m_aiBehavior->onActionFinished();
            }
        }*/
    }

    void Ray_AIShooterLaunchBulletAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);
    }

    void Ray_AIShooterLaunchBulletAction::onEndMarkerReached()
    {
        m_performHitEnded = btrue;
    }

    void Ray_AIShooterLaunchBulletAction::performHit( )
    {
        Vec3d bulletPos;

        if (m_animComponent->isLoaded())
        {
            ITF_ASSERT_MSG(getTemplate()->getBulletExitBoneName().isValid(), "you have to set the bulletExitBone!");
            if (m_bulletExitBoneIndex == U32_INVALID)
            {
                m_bulletExitBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getBulletExitBoneName());
                ITF_ASSERT_MSG(m_bulletExitBoneIndex != U32_INVALID, "bulletExitBone not found!");
            }

            m_animComponent->getBonePos(m_bulletExitBoneIndex, bulletPos, getTemplate()->useBonesEnd());
        }
        else
        {
            bulletPos = m_actor->getPos();
        }

        //GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() + m_direction, Color::red(), 0.1f);

        // recompute the dir from spawn bone pos
        //
        Vec2d shootDir = m_direction;
        /*if ( BaseObject * target = m_victim.getObject() )
        {
            if ( Actor * targetAct = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                shootDir = ( targetAct->getPos() - bulletPos).truncateTo2D();
                shootDir.normalize();
            }
        }*/

        f32 bulletAngle = shootDir.getOrientedAngle(Vec2d::Right);

        Actor* bullet = m_bulletSpawner.getSpawnee(m_actor->getScene(), bulletPos, bulletAngle);
        
        ITF_ASSERT(bullet);
        if (bullet) // crashed on 2010 Nov 10th while grouping objects
        {
            // to avoid spawned object never destroyed => onLoaded but no onbecomeactive/onbecomeinactive called
            // check the visibility of the bullet 
            AABB actorAABB = m_actor->getAABB();
            actorAABB.ScaleFromCenter( Vec2d( 2.0f, 2.0f ) );

            if ( CAMERA->isRectVisible( actorAABB, m_actor->getDepth() ) )
            {
                bullet->enable();

                EventSetDirection direction(shootDir);
                direction.setSender(m_actor->getRef());
                bullet->onEvent(&direction);

                EventSetTarget target(m_victim, Vec2d::Up * getTemplate()->getOffset());
                target.setSender(m_actor->getRef());
                bullet->onEvent(&target);
            }
            else
            {
                bullet->requestDestruction();
            }
        }
    }

}