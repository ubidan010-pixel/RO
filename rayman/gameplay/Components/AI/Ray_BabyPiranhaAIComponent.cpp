#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BABYPIRANHAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BabyPiranhaAIComponent.h"
#endif //_ITF_RAY_BABYPIRANHAAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_BABYPIRANHACOMPONENT_H_
#include "rayman/gameplay/Components/Piranha/Ray_BabyPiranhaComponent.h"
#endif //_ITF_RAY_BABYPIRANHACOMPONENT_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BabyPiranhaAIComponent_Template)

        BEGIN_SERIALIZATION_CHILD(Ray_BabyPiranhaAIComponent_Template)
        SERIALIZE_MEMBER("speed",m_speed);
        SERIALIZE_MEMBER("detectionRange",m_detectionRange);
        SERIALIZE_MEMBER("attackRange",m_attackRange);
        SERIALIZE_MEMBER("attackTime",m_attackTime);
        SERIALIZE_MEMBER("attackCooldown",m_attackCooldown);
        SERIALIZE_MEMBER("idlePercent",m_idlePercent);
        SERIALIZE_MEMBER("attackJumpPercent",m_attackJumpPercent);
        SERIALIZE_MEMBER("attackDivePercent",m_attackDivePercent);
        SERIALIZE_MEMBER("idlePerturbation", m_idlePerturbation);
        SERIALIZE_MEMBER("attackPerturbation", m_attackPerturbation);
        SERIALIZE_MEMBER("attackStartTime", m_attackStartTime);
        SERIALIZE_MEMBER("escapeDst", m_escapeDst);
        SERIALIZE_MEMBER("escapeTime", m_escapeTime);
        SERIALIZE_MEMBER("hitLevel", m_hitLevel);
        SERIALIZE_MEMBER("bounceMultiplier", m_bounceMultiplier);
        SERIALIZE_MEMBER("onlyAttackTargetInWater", m_onlyAttackTargetInWater);
    END_SERIALIZATION()

    Ray_BabyPiranhaAIComponent_Template::Ray_BabyPiranhaAIComponent_Template()
    : m_speed(0.5f)
    , m_detectionRange(5.f)
    , m_idlePercent(0.05f)
    , m_attackRange(1.f)
    , m_attackTime(0.5f)
    , m_attackCooldown(2.f)
    , m_attackJumpPercent(0.3f)
    , m_attackDivePercent(0.3f)
    , m_idlePerturbation(0.001f)
    , m_attackPerturbation(0.005f)
    , m_attackStartTime(0.1f)
    , m_escapeTime(2.f)
    , m_hitLevel(1)
    , m_bounceMultiplier(1.f)
    , m_escapeDst(3.f)
    , m_onlyAttackTargetInWater(bfalse)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_BabyPiranhaAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BabyPiranhaAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_OBJECT(Ray_BabyPiranhaAIComponent)
        VALIDATE_PARAM("babyPiranhaComponent",m_babyPiranhaComp != NULL,"No baby piranha component on actor!");
    END_VALIDATE_OBJECT()

    Ray_BabyPiranhaAIComponent::Ray_BabyPiranhaAIComponent()
        : m_state(BABYPIRANHA_IDLE)
        , m_babyPiranhaComp(NULL)
        , m_escapeDst(3.f)
        , m_hitAngle(bfalse,MTH_PI*0.5f)
    {

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_babyPiranhaComp   = m_actor->GetComponent<Ray_BabyPiranhaComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        setState(BABYPIRANHA_IDLE);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::setState(State _state)
    {
        m_state = _state;

        switch(_state)
        {
            case BABYPIRANHA_IDLE:
                m_targetActor.invalidate(); 
                initiateCurrentState_Normal();
                break;
            case BABYPIRANHA_FOLLOW:
                initiateCurrentState_Normal();
                break;
            case BABYPIRANHA_HIT:
                InitiateCurrentState_Escape();
                break;
            case BABYPIRANHA_ATTACK_UP:
                initiateAttack(btrue);
                break;
            case BABYPIRANHA_ATTACK_DOWN:
                initiateAttack(bfalse);
                break;
            case BABYPIRANHA_COOLDOWN:
                initiateCurrentState_Normal();
                break;

        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::sendHit(ObjectRef _victim, PhysShapeCircle *_collidingPiranha, const Vec2d &_collidingPos)
    {
        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(_collidingPiranha);

        stim->setSender(m_actor->getRef());
        stim->setDepth(m_actor->getDepth());
        stim->setPos(_collidingPos);
        stim->setPrevPos(_collidingPos);
        stim->setDirection(m_hitAngle.ToVector());
        stim->setLevel(getTemplate()->getHitLevel());
        stim->setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        stim->setBounceMultiplier(getTemplate()->getBounceMultiplier());
        stim->setAngle(m_hitAngle.ToRadians());
        stim->setFaction(getFaction());
        //AIUtils::drawDBGPhysShape(stim->getPos(),z,0,stim->getShape(),Color::red(), 0.5f);
        STIMSMANAGER->sendStim(stim);

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::onEvent( class Event * _event)
    {
        Super::onEvent(_event);

        if ( Ray_EventAttackGranted* attackGranted = _event->DynamicCast<Ray_EventAttackGranted>(ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789)) )
        {
            //We previously requested to attack a character. We receive this event because the fight system agrees

            if ( isAttacking() ) // We actually attack when in ATTACK state
            {
                bbool tryAttack = btrue;

                if (getTemplate()->getOnlyAttackTargetInWater())
                {
                    tryAttack = bfalse;
                    if ( Actor* target = m_targetActor.getActor() )
                    {
                        Ray_EventQueryAIData query;
                        query.setSender(m_actor->getRef());
                        target->onEvent(&query);

                        ObjectRef targetPolyline = query.getSwimmingPolyline();
                        if (!targetPolyline.isValid())
                        {
                            if ( PolyLine* poly = AIUtils::getPolylineRegion(target->getDepth(), s_RegionName_Swim, target->get2DPos()) )
                            {
                                targetPolyline = poly->getRef();
                            }
                        }
                        tryAttack = targetPolyline == m_babyPiranhaComp->getSwimPolyline();
                    }
                }

                if (tryAttack)
                {
                    PhysShapeCircle* collidingShape = NULL;
                    Vec2d pos = m_actor->get2DPos();
                    if ( m_babyPiranhaComp->isCollidingWithPiranha((Actor*)_event->getSender().getObject(), collidingShape, pos) )
                    {
                        ITF_ASSERT(collidingShape);
                        sendHit(attackGranted->getSender(), collidingShape, pos);
                        attackGranted->setAttackUsed(btrue);
                    }
                }
            }
        }
        else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            mayStartHit(hit);
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::Update( f32 _deltaTime )
    {
        Super::Update( _deltaTime );

        switch(m_state)
        {
        case BABYPIRANHA_IDLE:
            processStateIdle(_deltaTime);
            break;
        case BABYPIRANHA_FOLLOW:
            processStateFollow(_deltaTime);
            break;
        case BABYPIRANHA_ATTACK_UP:
        case BABYPIRANHA_ATTACK_DOWN:
            processStateAttack(_deltaTime);
            break;
        case BABYPIRANHA_COOLDOWN:
            processStateCooldown(_deltaTime);
            break;
        case BABYPIRANHA_HIT:
            processStateHit(_deltaTime);
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Start the state that was specified in m_state (normal behavior)
    void Ray_BabyPiranhaAIComponent::initiateCurrentState_Normal()
    {
        m_timer   = 0;
        m_babyPiranhaComp->stopAllAttacks();
        if (m_babyPiranhaComp->canAttackUp())
        {
            m_babyPiranhaComp->startAttackUp(getTemplate()->getIdlePercent());
        }
        else
        {
            m_babyPiranhaComp->startAttackUp(0.f);
        }
        m_babyPiranhaComp->setPertubation(getTemplate()->getIdlePerturbation());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///same as initiateCurrentState_Normal, but for when the piranha escapes
    void Ray_BabyPiranhaAIComponent::InitiateCurrentState_Escape()
    {
        m_timer   = 0;
        m_babyPiranhaComp->stopAllAttacks();
        m_babyPiranhaComp->startEscape();
        m_babyPiranhaComp->setPertubation(getTemplate()->getIdlePerturbation());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///initiate attack state
    void Ray_BabyPiranhaAIComponent::initiateAttack(bbool _attackUp)
    {
        m_state         = _attackUp?BABYPIRANHA_ATTACK_UP:BABYPIRANHA_ATTACK_DOWN;
        m_timer         = 0;
        m_babyPiranhaComp->stopAllAttacks();
        if (_attackUp)
            m_babyPiranhaComp->startAttackUp(getTemplate()->getAttackJumpPercent());
        else
            m_babyPiranhaComp->startAttackDown(getTemplate()->getAttackDivePercent());
        m_babyPiranhaComp->setPertubation(getTemplate()->getAttackPerturbation());
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    u32   Ray_BabyPiranhaAIComponent::getLastFrameRequestedAttackersCount(ActorRef _victim)
    {
        u32 count = RAY_FIGHTMANAGER->getLastFrameRequestedAttackersCount(_victim, m_babyPiranhaComp->getSwimPolyline());
        if (!GAMEMANAGER->isPlayerActor(_victim))
        {
            //We use a penalty to have lower priority. 2 because 1 should not trigger a reevaluation
            count += 2;
        }
        return count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::requestAttack()
    {
        RAY_FIGHTMANAGER->requestAttack(m_actor->getRef(), m_targetActor, m_babyPiranhaComp->getSwimPolyline());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_BabyPiranhaAIComponent::followTarget( f32 _dt )
    {
        u32 currentTargetEnemy = getLastFrameRequestedAttackersCount(m_targetActor);
        if (currentTargetEnemy > 1) // target has multiple enemy, try to attack someone else
        {
            ActorRef    newTarget;
            u32         newTargetEnemyCount;
            if (searchTarget(&newTarget, &newTargetEnemyCount))
            {
                if (newTargetEnemyCount < currentTargetEnemy)
                {
                    RAY_FIGHTMANAGER->removeFromLastFrameRequestedAttackers(m_actor->getRef(), m_targetActor);
                    m_targetActor       = newTarget;
                    return bfalse;
                }
            }
        }

        Actor* target =  m_targetActor.getActor();
        if (!target || !AIUtils::isTargetStillValid(m_actor, target))
        {
            //If we follow someone who's no longer here (or is dead), cancel
            setState(BABYPIRANHA_IDLE);
            return bfalse;
        }


        bbool stayOnSurface = btrue;

        if ( m_babyPiranhaComp->canGoInsideWater() )
        {
            PolyLine* targetPolyline = AIUtils::getPolylineRegion(target->getDepth(), s_RegionName_Swim, target->get2DPos());
            if (targetPolyline && targetPolyline->getRef() == m_babyPiranhaComp->getSwimPolyline())
            {
                // the target is swimming in the same water as us

                u32 surfaceEdge = U32_INVALID;
                f32 surfaceEdgeT = 0.f;
                bbool onSurface = bfalse;
                u32 closestEdge = U32_INVALID;
                f32 closestEdgeT = 0.f;
                bbool isInside = bfalse;

                AIUtils::getSwimingPolylineData(targetPolyline, target->getPos(), -Vec2d::Up, 0.1f,
                    surfaceEdge, surfaceEdgeT, onSurface,
                    closestEdge, closestEdgeT, isInside);

                stayOnSurface = onSurface;
            }
        }


        const Vec3d& targetPos = target->getPos();
        const f32 maxMovement = getTemplate()->getSpeed() * _dt;

        if (stayOnSurface)
        {
            // MOVE ALONG SURFACE

            // move on X only
            f32 xDest = targetPos.m_x - m_actor->getPos().m_x;
            f32 xDestClamp;
            if (xDest >= 0)
                xDestClamp = Min(xDest, maxMovement);
            else
                xDestClamp = Max(xDest, -maxMovement);

            // move Following the polyline
            m_babyPiranhaComp->moveOnPolyline(xDestClamp);
        }
        else
        {
            // DIVE INSIDE WATER

            Vec2d vecToTarget = target->get2DPos() - m_actor->get2DPos();
            if (vecToTarget != Vec2d::Zero)
            {
                f32 length = vecToTarget.norm();
                vecToTarget /= length;
                m_actor->set2DPos( m_actor->get2DPos() + vecToTarget * Min(length, maxMovement) );
            }
        }

        //if we are not in FOLLOW state and if we are beyond the attack range, switch to FOLLOW
        //else if we are beyond detection distance, switch to IDLE
        if (m_state != BABYPIRANHA_FOLLOW)
        {
            if ( (targetPos - m_actor->getPos()).sqrnorm() > sqr(getTemplate()->getAttackRange()))
            {
                setState(BABYPIRANHA_FOLLOW);
                return bfalse;
            }
        }
        else if ( (targetPos - m_actor->getPos()).sqrnorm() > sqr(getTemplate()->getDetectionRange()) )
        {
            setState(BABYPIRANHA_IDLE);
            return bfalse;
        }

        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_BabyPiranhaAIComponent::searchTarget(ActorRef *_pTarget, u32 *_pTargetEnemyCount)
    {
        if (searchTargetByType(_pTarget, _pTargetEnemyCount, btrue))
        {
            return btrue;
        }

        return searchTargetByType(_pTarget, _pTargetEnemyCount, bfalse);
    }    
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Search for actor who has fewest enemies, with _isForPlayer=btrue for player, _isForPlayer=bfalse for others
    bbool Ray_BabyPiranhaAIComponent::searchTargetByType(ActorRef *_pTarget, u32 *_pTargetEnemyCount, bbool _isForPlayer)
    {
        ActorList actors;
        if (_isForPlayer)
        {
            AIUtils::getLivePlayers(m_actor->getDepth(), actors);
        }
        else
        {
            AIUtils::getEnemies(getFaction(), m_actor->getDepth(), actors);
        }

        f32 range = getTemplate()->getDetectionRange()*getTemplate()->getDetectionRange();
        f32         matchDistance = range + 1.f;
        u32         matchNbEnnemy = U32_INVALID;
        ActorRef    matchActor;
        Vec3d       actorPos = m_actor->getPos();
        for (u32 i = 0; i < actors.size(); i++)
        {
            Actor *actor = actors[i];
            f32 curSqrNorm = (actor->getPos() - actorPos).sqrnorm();
            if (curSqrNorm < range)
            {
                u32 nbAttackers;
                nbAttackers = getLastFrameRequestedAttackersCount(actor->getRef());
                if (nbAttackers < matchNbEnnemy || (nbAttackers == matchNbEnnemy && curSqrNorm < matchDistance) )
                {
                    matchActor      = actor->getRef();
                    matchDistance   = curSqrNorm;
                    matchNbEnnemy   = nbAttackers;
                }
            }
        }
        if (!matchActor.isValid())
            return bfalse;

        *_pTarget           = matchActor;
        *_pTargetEnemyCount = matchNbEnnemy;

        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::processStateIdle(f32 _deltaTime)
    {
        u32 targetEnemyCount;
        //It we are idle, and if we have a target, set current target, prepare update of attack list, and follow
        if (searchTarget(&m_targetActor, &targetEnemyCount))
        {
            requestAttack(); //because we need to update attacker count each frame, except when really idle
            setState(BABYPIRANHA_FOLLOW);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::processStateFollow(f32 _deltaTime)
    {
        if (!followTarget(_deltaTime))
            return;

        requestAttack();

        if ( (m_targetActor.getActor()->getPos() - m_actor->getPos()).sqrnorm() < sqr(getTemplate()->getAttackRange()) )
        {
            mayAttackTarget();            
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_BabyPiranhaAIComponent::mayAttackTarget()
    {
        bbool attackUp = m_targetActor.getActor()->getPos().m_y > m_actor->getPos().m_y;
        if (attackUp)
        {
            if (m_babyPiranhaComp->canAttackUp())
            {
                setState(BABYPIRANHA_ATTACK_UP);
                return btrue;
            }
        }
        else
        {
            setState(BABYPIRANHA_ATTACK_DOWN);
            return btrue;
        }

        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::processStateAttack(f32 _deltaTime)
    {
        if (!followTarget(_deltaTime))
            return;

        requestAttack();

        m_timer += _deltaTime;
        if (m_timer > getTemplate()->getAttackTime())
        {
            setState(BABYPIRANHA_COOLDOWN);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::processStateCooldown(f32 _deltaTime)
    {
        if (!followTarget(_deltaTime))
            return;

        requestAttack();

        m_timer += _deltaTime;
        if (m_timer > getTemplate()->getAttackCooldown())
        {
            setState(BABYPIRANHA_IDLE);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::processStateHit(f32 _deltaTime)
    {
        m_timer += _deltaTime;
        if (m_timer > getTemplate()->getEscapeTime())
        {
            setState(BABYPIRANHA_COOLDOWN);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaAIComponent::mayStartHit( PunchStim *hit )
    {
        i32                 hitLevel        = hit->getLevel();
        if (hitLevel >= 2)
        {
            if (AIUtils::isEnemyHit(hit, getFaction()))
            {
                setState(BABYPIRANHA_HIT);
            }
        }
    }

}
