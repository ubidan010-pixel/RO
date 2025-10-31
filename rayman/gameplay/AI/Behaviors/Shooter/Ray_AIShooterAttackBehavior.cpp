#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIShooterLaunchBulletAction.h"
#endif //_ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterAttackBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIShooterAttackBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("hit", m_hit, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("giveup", m_giveUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("attackDistance", m_attackDistance);
        SERIALIZE_MEMBER("gunPivotBone", m_gunPivotBoneName);
        SERIALIZE_MEMBER("rotateUsingAnimation", m_rotateUsingAnimation);
        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
        SERIALIZE_MEMBER("minMaxAngleFlip", m_minMaxAngleFlip);
        SERIALIZE_MEMBER("lookRight", m_lookRight);
        SERIALIZE_MEMBER("giveUpRange", m_giveUpRange);
        SERIALIZE_MEMBER("aimCursorRestingPos", m_aimCursorRestingPos);
        SERIALIZE_MEMBER("aimTimeToFade", m_aimTimeToFade);
        SERIALIZE_MEMBER("aimOffset", m_aimOffset);
        SERIALIZE_MEMBER("forceAngle", m_forceAngle);
        SERIALIZE_MEMBER("forcedAngle", m_forcedAngle);
        SERIALIZE_MEMBER("keepDirForBurst", m_keepDirForBurst);
        SERIALIZE_MEMBER("autoForceDir", m_autoForceDir );
        SERIALIZE_MEMBER("DEBUG_disableAttack", m_DEBUG_disableAttack);

        SERIALIZE_MEMBER("canonPivotBoneName", m_canonPivotBoneName);
        SERIALIZE_MEMBER("canonExtremityBoneName", m_canonExtremityBoneName);
        
    END_SERIALIZATION()

    Ray_AIShooterAttackBehavior_Template::Ray_AIShooterAttackBehavior_Template()
    : m_idle(NULL)
    , m_hit(NULL)
    , m_giveUp(NULL)
    , m_rotateUsingAnimation(bfalse)
    , m_attackDistance(1.f)
    , m_minAngle(btrue, 0)
    , m_maxAngle(btrue, 90)
    , m_minMaxAngleFlip(bfalse)
    , m_giveUpRange(0.0f)
    , m_lookRight(btrue)
    , m_aimCursorRestingPos(0.0)
    , m_aimTimeToFade(1.0f)
    , m_aimOffset(0.0f)
    , m_forcedAngle(bfalse,0.0f)
    , m_forceAngle(bfalse)
    , m_DEBUG_disableAttack(bfalse)
    , m_keepDirForBurst( bfalse )
    , m_autoForceDir( bfalse )
    {

    }

    Ray_AIShooterAttackBehavior_Template::~Ray_AIShooterAttackBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_hit);
        SF_DEL(m_giveUp);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("idle", m_idle, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("hit", m_hit, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()


    Ray_AIShooterAttackBehavior::Ray_AIShooterAttackBehavior()
        : Super()
        , m_victim(ITF_INVALID_OBJREF)
        , m_gunPivotBoneIndex(U32_INVALID)
        , m_aimCursor(0.f)
        , m_idle(NULL)
        , m_hit(NULL)
        , m_giveUp(NULL)
        , m_aimCursorSpeed(0.0f)
        , m_KeepAimDirActive( bfalse )
        , m_canonPivotBoneIndex( U32_INVALID )
        , m_canonExtremityBoneIndex( U32_INVALID )
        , m_timedSpawnerData( NULL )
    {
    }

    Ray_AIShooterAttackBehavior::~Ray_AIShooterAttackBehavior()
    {
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getIdle()));
        m_hit = static_cast<Ray_AIShooterLaunchBulletAction*>(createAiAction(getTemplate()->getHit()));
        m_giveUp = createAiAction(getTemplate()->getGiveUp());

        Super::onActorLoaded(_hotReload);

        m_timedSpawner.initialize( m_actor, m_timedSpawnerData, NULL );
        m_timedSpawner.warmSpawnOnly( btrue );
        //m_timedSpawner.declareSpawnee( Path::EmptyPath );

        m_fixedAngle = getTemplate()->getForcedAngle();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTimedSpawnerDataSet,436096372), m_aiComponent );
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onResourceReady()
    {
        if ( getTemplate()->getCanonPivotBoneName().isValid() )
        {
            m_canonPivotBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getCanonPivotBoneName());
            ITF_ASSERT_MSG(m_canonPivotBoneIndex != U32_INVALID, "canon Pivot Bone Index not found!");
        }
        if ( getTemplate()->getCanonExtremityBoneName().isValid() )
        {
            m_canonExtremityBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getCanonExtremityBoneName());
            ITF_ASSERT_MSG(m_canonExtremityBoneIndex != U32_INVALID, "canon Extremity Bone Index not found!");
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onActivate()
    {
        //ITF_ASSERT_MSG(m_victim != ITF_INVALID_OBJREF, "no victim!");

        if ( m_timedSpawnerData != NULL )
        {
            m_timedSpawner.setTimedSpawnData( m_timedSpawnerData );
            m_timedSpawner.setActivate( btrue );
        }

        setAction(m_idle, btrue );
        // init the anim cursor
        aim();
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onDeactivate()
    {
        m_victim = ITF_INVALID_OBJREF;
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onEvent( Event* _event)
    {
        Super::onEvent( _event );
        
        if ( EventTimedSpawnerDataSet * spawnData = _event->DynamicCast<EventTimedSpawnerDataSet>(ITF_GET_STRINGID_CRC(EventTimedSpawnerDataSet,436096372)) )
        {
            TimedSpawnerData::EventToData( *spawnData, m_timedSpawnerDataFromEvent );
            m_timedSpawner.setTimedSpawnData( &m_timedSpawnerDataFromEvent );

            if ( !m_timedSpawner.isActivated() )
            {
                m_timedSpawner.setActivate( btrue );
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::update( f32 _delta  )
    {
        /*
        if (getTemplate()->getGiveUpRange() > 0.0f)
        {
            if (m_giveUp)
            {
                if (m_currentAction != m_giveUp)
                {
                    Actor * enemy = AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getGiveUpRange());
                    if (!enemy || (enemy && enemy->getRef() == ITF_INVALID_OBJREF))
                    {
                        m_aimCursorSpeed = (1.0f - getTemplate()->getAimCursorRestingPos()) / getTemplate()->getAimTimeToFade();
                        m_aimCursorSpeed = m_aimCursor > getTemplate()->getAimCursorRestingPos() ? -m_aimCursorSpeed: m_aimCursorSpeed;
                        setAction(m_giveUp);
                    }
                    else
                        aim();
                }
                else if (m_currentAction == m_giveUp)
                {
                    //fade aim cursor
                    f32 newAimCursor = m_aimCursor + _delta*m_aimCursorSpeed;
                    if ((getTemplate()->getAimCursorRestingPos() >= m_aimCursor && getTemplate()->getAimCursorRestingPos() <= newAimCursor )||
                        (getTemplate()->getAimCursorRestingPos() >= newAimCursor && getTemplate()->getAimCursorRestingPos() <= m_aimCursor ))
                        m_aimCursor = getTemplate()->getAimCursorRestingPos();
                    else
                        m_aimCursor = newAimCursor;
                }
                else
                    aim();
            }
            else
            {
                Actor * enemy = AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getGiveUpRange());
                if (enemy->getRef() == ITF_INVALID_OBJREF)
                {
                    m_aiComponent->onBehaviorFinished();
                }
                else
                    aim();

            }
        }
        else */
        {

            aim();

            if ( !getTemplate()->getDisableAttack() )
            {
                m_timedSpawner.update( _delta );

                if ( m_timedSpawner.warmIsSpawnNeeded() )
                {
                    bbool keepDir = getTemplate()->getKeepDirForBurst() && !m_timedSpawner.isBurstFinished();

                    if ( keepDir && !m_KeepAimDirActive )
                    {
                        m_KeepAimDirActive = btrue;
                        getAimDir( m_keepAimDir );
                    }
                    
                    setAction( m_hit, btrue );
                }

                if ( m_timedSpawner.isSpawnFinished() )
                {
                    //m_aiComponent->onBehaviorFinished();
                }
            }
        }

        Super::update(_delta);
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::onActionFinished()
    {
        /*
        if ( m_currentAction == m_idle )
        {
            if (getTemplate()->getDisableAttack())
                setAction(m_idle);
            else
            {
                m_KeepAimDirActive = bfalse;
                if ( getTemplate()->getKeepDirForBurst() )
                {
                    m_KeepAimDirActive = btrue;
                    getAimDir( m_keepAimDir );
                }

                //m_hit->setTimedSpawnerData( m_timedSpawnerData );
                setAction(m_hit);
            }
        }
        else*/ if (m_giveUp && m_currentAction == m_giveUp)
        {
            m_aiComponent->onBehaviorFinished();
        }
        else // m_hit
        {
            m_KeepAimDirActive = getTemplate()->getKeepDirForBurst() && !m_timedSpawner.isBurstFinished();
            setAction(m_idle);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::getCanonDir( Vec2d & _res_dir )
    {
        //_res_dir = Vec2d::Zero;

        if ( ( m_canonPivotBoneIndex != U32_INVALID ) && ( m_canonExtremityBoneIndex != U32_INVALID ) )
        {
            Vec2d firstPos, secondPos;
            m_animComponent->getBonePos(m_canonPivotBoneIndex, firstPos );
            m_animComponent->getBonePos(m_canonExtremityBoneIndex, secondPos );
            _res_dir = secondPos - firstPos;
            _res_dir.normalize();
        }    
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::getAimDir( Vec2d & _res_dir )
    {
        _res_dir = Vec2d::Right;

        Actor* victim = AIUtils::getActor(m_victim);
        if (victim)
        {
            Vec2d pos = victim->get2DPos();
            Vec2d offset = Vec2d::Up.Rotate(victim->getAngle());
            offset *= getTemplate()->getAimOffset();
            pos += offset;

            if ( m_canonPivotBoneIndex != U32_INVALID )
            {
                Vec2d pivotPos;
                m_animComponent->getBonePos(m_canonPivotBoneIndex, pivotPos );
                _res_dir = ( pos - pivotPos ).normalize();
            }
            else
            {
                _res_dir = ( pos - m_actor->get2DPos() ).normalize();
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::aim()
    {
        Actor* victim = AIUtils::getActor(m_victim);
        if ( victim || getTemplate()->getIsForcedAngle() )
        {
            Vec2d direction;
            getAimDir( direction );
            if ( m_KeepAimDirActive )
            {
                direction = m_keepAimDir;
            }
            
            Vec2d myForward = Vec2d::Right.Rotate(m_actor->getAngle());
            bbool flip = direction.dot(myForward) < 0;
            if ( getTemplate()->getIsForcedAngle() )
            {
                flip = !getTemplate()->getLookRight();
                if ( ( f32_Abs( canonizeAngle( m_actor->getAngle() ) ) > MTH_PIBY2 ) )
                {
                    flip = !flip;
                }               

                f32 angle = /*f32_Abs*/( m_fixedAngle.ToRadians() );//getTemplate()->getForcedAngle().ToRadians() );
                if ( flip )
                {
                    angle = MTH_PI - /*f32_Abs*/( angle );
                }
                angle = canonizeAngle( m_actor->getAngle() + angle );
                direction = Vec2d::Right.Rotate( angle );
            }
            else if ( getTemplate()->getAutoForceDir() )
            {
                flip = !getTemplate()->getLookRight();
            }

            Vec2d canonDir;
            Vec2d shootDir = direction;
            getCanonDir( canonDir );
            if ( !canonDir.IsEqual( Vec2d::Zero ) && ( direction.dot( canonDir ) < 0.9f ) )
            {
                shootDir = canonDir;            
            }

            m_hit->setDirection( shootDir );
            m_hit->setVictim(m_victim);
            m_actor->setFlipped(flip);

            if (getTemplate()->getRotateUsingAnimation())
            {
                aimWithAnimation(direction, flip);
            }
            else if (getTemplate()->getGunPivotBoneName().isValid())
            {
                aimWithBone(direction, flip);
            }
            else
            {
                aimWithActor(direction, flip);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterAttackBehavior::aimWithActor( Vec2d& _direction, bbool _flip )
    {
        f32 angle = _direction.getOrientedAngle(Vec2d::Right);
        if (_flip)
            angle += MTH_PI;

        m_actor->setAngle(angle);
    }

    void Ray_AIShooterAttackBehavior::aimWithBone( Vec2d& _direction, bbool _flip )
    {
        if (!m_animComponent->isLoaded())
            return;

        if (m_gunPivotBoneIndex == U32_INVALID)
        {
            m_gunPivotBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getGunPivotBoneName());
            ITF_ASSERT_MSG(m_gunPivotBoneIndex != U32_INVALID, "gunPivotBone not found!");
        }


        f32 min, max, actorAngle;
        f32 angle = getAngleInActorSpace(_direction, _flip, min, max, actorAngle);

        // apply relatively to current bone angle
        f32 boneAngle = m_animComponent->getBone(m_gunPivotBoneIndex)->getAngle(); // absolute
        angle = getShortestAngleDelta(boneAngle, angle + actorAngle);
        m_animComponent->getCurSubAnim(0)->getSkeleton()->RotateBone(&m_animComponent->getAnimMeshScene()->m_AnimInfo.getCurrentBoneList(), m_gunPivotBoneIndex, angle);
    }

    void Ray_AIShooterAttackBehavior::aimWithAnimation( Vec2d& _direction, bbool _flip )
    {
        f32 min, max, actorAngle;
        f32 angle = getAngleInActorSpace(_direction, _flip, min, max, actorAngle);

        m_aimCursor = getShortestAngleDelta(min, angle) / getShortestAngleDelta(min, max);
        if (_flip)
            m_aimCursor = 1.0f - m_aimCursor;
        if ( getTemplate()->isMinMaxAngleFlip() )
        {
            m_aimCursor = 1.0f - m_aimCursor;
        }
    }

    f32 Ray_AIShooterAttackBehavior::getAngleInActorSpace( Vec2d& _direction, bbool _flip, f32& min, f32& max, f32& actorAngle )
    {
        // compute everything relatively to the actor

        min = canonizeAngle( getTemplate()->getMinAngle().ToRadians() );
        max = canonizeAngle( getTemplate()->getMaxAngle().ToRadians() );
        actorAngle = m_actor->getAngle();

        if (_flip)
        {
            // flipped: reverse the range
            f32 swap = min;
            min = -max;
            max = -swap;
            actorAngle += MTH_PI;
        }

        f32 angle = 0.0f;
       /* if (getTemplate()->getIsForcedAngle())
        {
            //angle = _flip?MTH_PI-getTemplate()->getForcedAngle().ToRadians():getTemplate()->getForcedAngle().ToRadians();
            angle = getTemplate()->getForcedAngle().ToRadians();
            if ( angle < 0.0f )
            {
                angle = MTH_PI - f32_Abs( angle );
            }
            //angle = canonizeAngle(actorAngle + angle );
        }
        else*/
            angle = _direction.getOrientedAngle(Vec2d::Right);  // wanted angle

        
        angle = getShortestAngleDelta(actorAngle, angle);       // relative to actor
        
        if (angle < min || angle > max)
        {
            // we're outside the range, choose which side to clamp
            f32 dMin = f32_Abs(getShortestAngleDelta(min, angle));
            f32 dMax = f32_Abs(getShortestAngleDelta(max, angle));
            angle = (dMin < dMax) ? min : max;
        }
        //angle = Clamp(angle, min, max);

        //GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() + Vec2d::Right.Rotate(angle), Color::blue());
        //GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() + Vec2d::Right.Rotate(angle + actorAngle), Color::red());

        return angle;
    }

    void Ray_AIShooterAttackBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AimCursor,2028595416), m_aimCursor);
    }

}
