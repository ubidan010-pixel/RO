#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIHunterAttackBehavior.h"
#endif //_ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_

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

#ifndef _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIHunterLaunchBulletAction.h"
#endif //_ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIHunterAttackBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIHunterAttackBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("aim", m_aim, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("hit", m_hit, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("giveup", m_giveUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("attackDistance", m_attackDistance);
        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
        SERIALIZE_MEMBER("lookRight", m_lookRight);
        SERIALIZE_MEMBER("DEBUG_disableAttack", m_DEBUG_disableAttack);
        SERIALIZE_MEMBER("giveUpRange", m_giveUpRange);
        SERIALIZE_MEMBER("aimCursorRestingPos", m_aimCursorRestingPos);
        SERIALIZE_MEMBER("aimTimeToFade", m_aimTimeToFade);
        SERIALIZE_MEMBER("aimOffset", m_aimOffset);
        SERIALIZE_MEMBER("forceAngle", m_forceAngle);
        SERIALIZE_MEMBER("forcedAngle", m_forcedAngle);
        SERIALIZE_MEMBER("numBulletsPerCycle", m_numBulletsPerCycle);
        SERIALIZE_MEMBER("timeBetweenBullets", m_timeBetweenBullets);
        SERIALIZE_MEMBER("timeBetweenCycles", m_timeBetweenCycles);
        SERIALIZE_MEMBER("canFlip", m_canFlip);
        SERIALIZE_MEMBER("findEnemyRadius", m_findEnemyRadius);
        SERIALIZE_MEMBER("useRadius", m_useRadius);
        SERIALIZE_OBJECT("detectionRange", m_detectionRange);
        SERIALIZE_MEMBER("minArmAngle", m_minArmAngle);
        SERIALIZE_MEMBER("maxArmAngle", m_maxArmAngle);
        SERIALIZE_MEMBER("maxLivingMissiles", m_maxLivingMissiles);
    END_SERIALIZATION()

    Ray_AIHunterAttackBehavior_Template::Ray_AIHunterAttackBehavior_Template()
    : m_idle(NULL)
    , m_aim(NULL)
    , m_hit(NULL)
    , m_giveUp(NULL)
    , m_attackDistance(1.f)
    , m_minAngle(btrue, 0)
    , m_maxAngle(btrue, 90)
    , m_giveUpRange(0.0f)
    , m_lookRight(btrue)
    , m_aimCursorRestingPos(0.0)
    , m_aimTimeToFade(1.0f)
    , m_aimOffset(0.0f)
    , m_forcedAngle(bfalse,0.0f)
    , m_forceAngle(bfalse)
    , m_numBulletsPerCycle(3)
    , m_timeBetweenBullets(0.35f)
    , m_timeBetweenCycles(2.0f)
    , m_canFlip(btrue)
    , m_findEnemyRadius(4.f)
    , m_detectionRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
    , m_useRadius(btrue)
    , m_DEBUG_disableAttack(bfalse)
    , m_minArmAngle(btrue,-90.0f)
    , m_maxArmAngle(btrue,90.0f)
    , m_maxLivingMissiles(2)
    {

    }

    Ray_AIHunterAttackBehavior_Template::~Ray_AIHunterAttackBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_aim);
        SF_DEL(m_hit);
        SF_DEL(m_giveUp);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIHunterAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIHunterAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("aim", m_aim, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("hit", m_hit, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()


    Ray_AIHunterAttackBehavior::Ray_AIHunterAttackBehavior()
        : Super()
        , m_linkedToActivator(bfalse)
        , m_victim(ITF_INVALID_OBJREF)
        , m_aimCursor(0.f)
        , m_idle(NULL)
        , m_aim(NULL)
        , m_hit(NULL)
        , m_giveUp(NULL)
        , m_aimCursorSpeed(0.0f)
        , m_numBulletsPerCycle(3)
        , m_numTotalCycles(U32_INVALID)
        , m_currentCycle(0)
        , m_currentBullet(0)
        , m_bulletTimer(0.0f)
        , m_cycleTimer(0.0f)
        , m_enabled(btrue)
    {
    }

    Ray_AIHunterAttackBehavior::~Ray_AIHunterAttackBehavior()
    {
    }

    void Ray_AIHunterAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = createAiAction(getTemplate()->getIdle());
        m_aim = createAiAction(getTemplate()->getAim());
        m_hit = static_cast<Ray_AIHunterLaunchBulletAction*>(createAiAction(getTemplate()->getHit()));
        m_giveUp = createAiAction(getTemplate()->getGiveUp());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHunterBulletLaunched,1767723773), m_aiComponent);
        
        m_numBulletsPerCycle = getTemplate()->getNumBulletsPerCycle();
    }

    void Ray_AIHunterAttackBehavior::onActivate()
    {
        //Super::onActivate();
        m_aimCursor = getTemplate()->getAimCursorRestingPos();
        startIdle();
    }

    void Ray_AIHunterAttackBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }

    void Ray_AIHunterAttackBehavior::update( f32 _delta  )
    {
        
        for (i32 i = m_bulletsLaunched.size()-1; i >= 0; --i)
        {
            Actor * actor  = m_bulletsLaunched[i].getActor();
            if (!actor || ( !actor->isActive() && ( actor->getLastActiveFrame() == CURRENTFRAME - 1 ) ) )
            {
                m_bulletsLaunched.eraseNoOrder(i);
            }
        }


        if (m_currentAction == m_idle )
        {
            updateIdle(_delta);
        }
        else if (m_currentAction == m_hit)
        {
            updateHit(_delta);
        }
        else if (m_currentAction == m_giveUp)
        {
            updateGiveUp(_delta);
        }
        else if (m_currentAction == m_aim)
        {
            updateAim(_delta);
        }

        Super::update(_delta);

        AABB aabb = getTemplate()->getDetectionRange();
        if (m_actor->isFlipped())
            aabb.FlipHorizontaly(bfalse);

        m_actor->growRelativeAABB(aabb);
    }

    void Ray_AIHunterAttackBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (EventActivate * _onActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            if (_onActivate->getActivated())
            {
                m_enabled = btrue;
                m_linkedToActivator = btrue;
            }
            else
            {
                m_enabled = bfalse;
                m_linkedToActivator = btrue;
            }
        }   
        else if (Ray_EventHunterBulletLaunched * onBulletLaunched = _event->DynamicCast<Ray_EventHunterBulletLaunched>(ITF_GET_STRINGID_CRC(Ray_EventHunterBulletLaunched,1767723773)))
        {
            m_bulletsLaunched.push_back(onBulletLaunched->getBuller());
        }
    }

    void Ray_AIHunterAttackBehavior::onActionFinished()
    {
        if ( m_currentAction == m_idle )
        {
            onFinishedIdle();
        }
        else if (m_giveUp && m_currentAction == m_giveUp)
        {
            onFinishedGiveUp();
        }
        else if (m_currentAction == m_hit)
        {
            onFinishedHit();
        }
        else if (m_currentAction == m_aim)
        {
            onFinishedAim();
        }
    }

    void Ray_AIHunterAttackBehavior::aim()
    {
        Actor* victim = AIUtils::getActor(m_victim);
        if (victim)
        {
            Vec2d pos = victim->get2DPos();
            Vec2d offset = Vec2d::Up.Rotate(victim->getAngle());
            offset *= getTemplate()->getAimOffset();
            pos += offset;
            Vec2d direction = ( pos - m_actor->get2DPos() ).normalize();
            Vec2d myForward = Vec2d::Right.Rotate(m_actor->getAngle());

            bbool currentFlip = m_actor->isFlipped();

            bbool flip = currentFlip;
            if (getTemplate()->getCanFlip())
                flip = direction.dot(myForward) < 0;
            //flip = bfalse;

            f32 dirAngle = direction.getAngle();
            if (currentFlip)
                dirAngle = MTH_PI - dirAngle;
            dirAngle  = canonizeAngle(dirAngle);
            dirAngle = Clamp(dirAngle,getTemplate()->getMinArmAngle().Radians(),getTemplate()->getMaxArmAngle().Radians());

            direction = Vec2d::Right.Rotate(currentFlip?MTH_PI-dirAngle:dirAngle);

            m_hit->setDirection( direction);
            m_hit->setVictim(m_victim);
            m_actor->setFlipped(flip);

            aimWithAnimation(direction, flip);
        }
    }

    void Ray_AIHunterAttackBehavior::aimWithAnimation( const Vec2d& _direction, bbool _flip )
    {
        f32 min, max, actorAngle;
        f32 angle = getAngleInActorSpace(_direction, _flip, min, max, actorAngle);

        Vec2d direction = Vec2d::Right.Rotate(angle+actorAngle);
        m_hit->setDirection( direction);
        m_aimCursor = getShortestAngleDelta(min, angle) / getShortestAngleDelta(min, max);
        if (_flip)
            m_aimCursor = 1.0f - m_aimCursor;

    }

    f32 Ray_AIHunterAttackBehavior::getAngleInActorSpace( const Vec2d& _direction, bbool _flip, f32& min, f32& max, f32& actorAngle )
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
        if (getTemplate()->getIsForcedAngle())
            angle = _flip?MTH_PI-getTemplate()->getForcedAngle().ToRadians():getTemplate()->getForcedAngle().ToRadians();
        else
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

    void Ray_AIHunterAttackBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AimCursor,2028595416), m_aimCursor);
    }


    void Ray_AIHunterAttackBehavior::onFinishedIdle()
    {
        //if (m_DEBUG_disableAttack)
        //    setAction(m_idle);
        //else
        //    setAction(m_hit);
    }

    void Ray_AIHunterAttackBehavior::onFinishedGiveUp()
    {
        startIdle();
    }

    void Ray_AIHunterAttackBehavior::onFinishedHit()
    {
        ++m_currentBullet;
        if (m_currentBullet == m_numBulletsPerCycle)
            ++m_currentCycle;
        
        if ( !m_enabled )
        {
            startIdle();
        }
        else
        {
            setAction(m_aim);
            if (m_numTotalCycles != U32_INVALID)
            {
                if (m_currentCycle >= m_numTotalCycles)
                {
                    startIdle();
                }
            }
        }
        
    }

    void Ray_AIHunterAttackBehavior::updateIdle(f32 _delta)
    {
        Actor * enemy = getVictim();

        if (m_linkedToActivator)
        {
            if (m_enabled)
                startAim();
        }
        else
        {
            if (enemy)
            {
                setVictim(enemy->getRef());
                startAim();
            }
        }
    }

    void Ray_AIHunterAttackBehavior::updateHit( f32 _delta )
    {
        Actor * enemy = getVictim();
        if (enemy )
            setVictim(enemy->getRef());
        aim();
    }

    void Ray_AIHunterAttackBehavior::updateGiveUp( f32 _delta )
    {
        //fade aim cursor
        f32 newAimCursor = m_aimCursor + _delta*m_aimCursorSpeed;
        if ((getTemplate()->getAimCursorRestingPos() >= m_aimCursor && getTemplate()->getAimCursorRestingPos() <= newAimCursor )||
            (getTemplate()->getAimCursorRestingPos() >= newAimCursor && getTemplate()->getAimCursorRestingPos() <= m_aimCursor ))
            m_aimCursor = getTemplate()->getAimCursorRestingPos();
        else
            m_aimCursor = newAimCursor;
    }

    void Ray_AIHunterAttackBehavior::startGiveUp()
    {
        m_aimCursorSpeed = (1.0f - getTemplate()->getAimCursorRestingPos()) / getTemplate()->getAimTimeToFade();
        m_aimCursorSpeed = m_aimCursor > getTemplate()->getAimCursorRestingPos() ? -m_aimCursorSpeed: m_aimCursorSpeed;
        if (m_giveUp)
        {
            setAction(m_giveUp);
        }
        else
            startIdle();
    }

    void Ray_AIHunterAttackBehavior::setData(u32 _numBulletsPerCycle,u32 _numCycles)
    {
        m_numBulletsPerCycle = _numBulletsPerCycle;
        m_numTotalCycles = _numCycles;
    }

    void Ray_AIHunterAttackBehavior::updateAim( f32 _delta )
    {
        if (!m_linkedToActivator && !updateVictim() )
            return; 

        if ( !m_enabled )
        {
            startIdle();
        }

        aim();



            if (m_currentBullet == m_numBulletsPerCycle)
            {
                if (m_cycleTimer == 0.0f)
                {
                    if (getCurrentLivingMissiles() < getTemplate()->getMaxLivingMissiles())
                    {
                        m_cycleTimer = getTemplate()->getTimeBetweenCycles();
                        m_currentBullet = 0;
                        startHit();
                    }
                }
                if (!m_animatedComponent->isInTransition())
                    m_cycleTimer = Max(0.0f,m_cycleTimer-_delta);
            }
            else
            {
                if (m_bulletTimer == 0.0f)
                {
                    if (getCurrentLivingMissiles() < getTemplate()->getMaxLivingMissiles())
                        startHit();
                }
                if (!m_animatedComponent->isInTransition())
                    m_bulletTimer = Max(0.0f, m_bulletTimer - _delta);
            }
        //}
        //else
        //{
        //    if (m_enabled)
        //    {
        //        bbool flip = m_actor->isFlipped();
        //        const Vec2d lookDir = AIUtils::getLookDir(m_actor, flip);
        //        aimWithAnimation( lookDir, flip );
        //    }
        //}
    }

    void Ray_AIHunterAttackBehavior::onFinishedAim()
    {
        m_victim.invalidate();
    }

    void Ray_AIHunterAttackBehavior::startIdle()
    {
        setAction(m_idle);
    }

    void Ray_AIHunterAttackBehavior::startHit()
    {
        m_bulletTimer = getTemplate()->getTimeBetweenBullets();
        setAction(m_hit);
    }

    void Ray_AIHunterAttackBehavior::startAim()
    {
        m_bulletTimer = 0.0f;
        m_cycleTimer = getTemplate()->getTimeBetweenCycles();
        m_currentBullet = 0;
        m_currentCycle = 0;
        setAction(m_aim);
    }

    Actor * Ray_AIHunterAttackBehavior::getVictim()
    {
        Actor * enemy  = NULL;
        if (getTemplate()->getUseRadius())
            enemy =  AIUtils::getClosestEnemy(m_actor,m_actor->isFlipped(),getTemplate()->getFindEnemyRadius());
        else
            enemy =  AIUtils::getClosestEnemy(m_actor,m_actor->isFlipped(),getTemplate()->getDetectionRange());
        return enemy;
    }

    bbool Ray_AIHunterAttackBehavior::updateVictim()
    {
        Actor * enemy = getVictim();
        if (!enemy )
        {
            startGiveUp();
            return bfalse;
        }
        else
        {
            if (getTemplate()->getGiveUpRange() > 0.0f)
            {
                if ((m_actor->get2DPos() - enemy->get2DPos()).sqrnorm() > getTemplate()->getGiveUpRange() * getTemplate()->getGiveUpRange())
                {
                    startGiveUp();
                    return bfalse;
                }
            }
        }

        setVictim(enemy->getRef());

        return btrue;
    }

    u32 Ray_AIHunterAttackBehavior::getCurrentLivingMissiles() const
    {
        return m_bulletsLaunched.size();
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIHunterAttackBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (getTemplate()->getUseRadius())
        {
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos(),getTemplate()->getFindEnemyRadius(),Color::red());
        }
        else
        {
            AABB aabb = getTemplate()->getDetectionRange();
            if (m_actor->isFlipped())
                aabb.FlipHorizontaly(bfalse);
            DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::red(),0.f,1.f,"DetectionRange");
        }
    }
#endif // ITF_SUPPORT_EDITOR
}
