#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIBubbleDeathBehavior.h"
#endif //_ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIBubbleDeathBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIBubbleDeathBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("reward", m_reward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("rewardAtStart", m_rewardAtStart, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("fx",m_fxPath);
        SERIALIZE_MEMBER("spawnOnMarker",m_spawnOnMarker);
        SERIALIZE_OBJECT_WITH_FACTORY("swellAction", m_swellAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("floatAction", m_floatAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("explodeAction", m_explodeAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("floatTime",m_floatTime);
        SERIALIZE_MEMBER("floatForce",m_floatForce);
        SERIALIZE_MEMBER("sinkForce",m_sinkForce);
        SERIALIZE_MEMBER("floatAngleFrequency",m_floatAngleFrequency);
        SERIALIZE_MEMBER("floatAngleOffset",m_floatAngleOffset);
        SERIALIZE_MEMBER("floatAirFrictionMultiplier",m_floatAirFrictionMultiplier);
        SERIALIZE_MEMBER("hitForce",m_hitForce);
        SERIALIZE_MEMBER("avoidanceForce",m_avoidanceForce);
        SERIALIZE_MEMBER("avoidanceRadius",m_avoidanceRadius);
        SERIALIZE_MEMBER("minFloatSpeed",m_minFloatSpeed);
        SERIALIZE_MEMBER("maxFloatSpeed",m_maxFloatSpeed);
        SERIALIZE_MEMBER("minFloatExtraSpeed",m_minFloatExtraSpeed);
        SERIALIZE_MEMBER("maxFloatExtraSpeed",m_maxFloatExtraSpeed);
        SERIALIZE_MEMBER("minSinkSpeed",m_minSinkSpeed);
        SERIALIZE_MEMBER("maxSinkSpeed",m_maxSinkSpeed);
        SERIALIZE_MEMBER("minXSpeed",m_minXSpeed);
        SERIALIZE_MEMBER("maxXSpeed",m_maxXSpeed);
        SERIALIZE_MEMBER("floatForceTime",m_floatForceTime);
        SERIALIZE_MEMBER("softCollisionRadiusMultiplier",m_softCollisionRadiusMultiplier);
        SERIALIZE_MEMBER("squashPenetrationRadius",m_squashPenetrationRadius);
        SERIALIZE_MEMBER("usePhysRadiusAsSoftCollRadius",m_usePhysRadiusAsSoftCollRadius);
        SERIALIZE_MEMBER("explodeOnPlayer",m_explodeOnPlayer);
        SERIALIZE_MEMBER("floatForceX",m_floatForceX);
        SERIALIZE_MEMBER("explosionFeedBackTime",m_explosionFeedBackTime);
        SERIALIZE_MEMBER("explosionFeedBackFreq",m_explosionFeedBackFreq);
        SERIALIZE_MEMBER("explosionFeedBackAmplitude",m_explosionFeedBackAmplitude);
        SERIALIZE_MEMBER("floatForceExtraTime",m_floatForceExtraTime);
        SERIALIZE_MEMBER("pedestalMaxUserCount",m_pedestalMaxUserCount);
        SERIALIZE_MEMBER("pedestalOffset",m_pedestalOffset);
        SERIALIZE_MEMBER("checkWater",m_checkWater);
        SERIALIZE_MEMBER("supportedActorInvincibilityTimer",m_supportedActorInvincibilityTimer);
        SERIALIZE_MEMBER("waitDurationBeforeStimPossibility",m_waitDurationBeforeStimPossibility );
    END_SERIALIZATION()

    Ray_AIBubbleDeathBehavior_Template::Ray_AIBubbleDeathBehavior_Template()
        : m_reward(NULL)
        , m_rewardAtStart(NULL)
        , m_spawnOnMarker(btrue)
        , m_swellAction(NULL)
        , m_floatAction(NULL)
        , m_explodeAction(NULL)
        , m_floatTime(5.0f)
        , m_floatForce(25.0f)
        , m_sinkForce(4.0f)
        , m_floatAngleFrequency(0.2f)
        , m_floatAngleOffset(btrue,5.0f)
        , m_floatAirFrictionMultiplier(20.0f)
        , m_hitForce(800.0f)
        , m_avoidanceForce(50.0f)
        , m_avoidanceRadius(0.7f)
        , m_minFloatSpeed(1.0f)
        , m_maxFloatSpeed(3.0f)
        , m_floatForceExtraTime(1.0f)
        , m_minFloatExtraSpeed(2.0f)
        , m_maxFloatExtraSpeed(6.0f)
        , m_minSinkSpeed(1.0f)
        , m_maxSinkSpeed(3.0f)
        , m_minXSpeed(1.0f)
        , m_maxXSpeed(3.0f)
        , m_floatForceTime(0.5f)
        , m_softCollisionRadiusMultiplier(1.0f)
        , m_squashPenetrationRadius(0.9f)
        , m_usePhysRadiusAsSoftCollRadius(bfalse)
        , m_explodeOnPlayer(bfalse)
        , m_floatForceX(0.5f)
        , m_explosionFeedBackTime(1.0f)
        , m_explosionFeedBackFreq(10.0f)
        , m_explosionFeedBackAmplitude(0.5f)
        , m_pedestalMaxUserCount(4)
        , m_pedestalOffset(1.0f)
        , m_checkWater(btrue)
        , m_supportedActorInvincibilityTimer(0.2f)
        , m_waitDurationBeforeStimPossibility( 0.0f )
    {
    }

    Ray_AIBubbleDeathBehavior_Template::~Ray_AIBubbleDeathBehavior_Template()
    {
        SF_DEL(m_reward);
        SF_DEL(m_rewardAtStart);
        SF_DEL(m_swellAction);
        SF_DEL(m_floatAction);
        SF_DEL(m_explodeAction);
    }


    IMPLEMENT_OBJECT_RTTI(Ray_AIBubbleDeathBehavior)
    Ray_AIBubbleDeathBehavior::Ray_AIBubbleDeathBehavior()
        : Super()
        , m_reward(NULL)
        , m_rewardAtStart(NULL)
        , m_physComponent(NULL)
        , m_phantomComponent(NULL)
        , m_swellAction(NULL)
        , m_floatAction(NULL)
        , m_explodeAction(NULL)
        , m_floatTime(5.f)
        , m_timer(0.0f)
        , m_accumulatedTime(0.0f)
        , m_wasHit(bfalse)
        , m_initialAngle(0.0f)
        , m_floatForceTimer(0.0f)
        , m_firstUpdate(btrue)
        , m_floatForceExtraTimer(0.0f)
        , m_floatForceX(0.5f)
        , m_stimPossibilityTimer( 0.0f )
    {
    }

    Ray_AIBubbleDeathBehavior::~Ray_AIBubbleDeathBehavior()
    {
        SF_DEL(m_reward);
        SF_DEL(m_rewardAtStart);
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), m_aiComponent);
#endif
    }


    void Ray_AIBubbleDeathBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_swellAction = createAiAction(getTemplate()->getSwellAction());
        m_floatAction = createAiAction(getTemplate()->getFloatAction());
        m_explodeAction = createAiAction(getTemplate()->getExplodeAction());

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(VacuumStim,593568508),m_aiComponent);
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),m_aiComponent);
#endif


        initRewardAndFx();

        m_supportedActors.resize(getTemplate()->getPedestalMaxUserCount());

        m_floatTime = getTemplate()->getFloatTime();
        m_floatForceX = getTemplate()->getFloatForceX();
    }

    void Ray_AIBubbleDeathBehavior::onActivate()
    {
        //Super::onActivate();

        if (m_physComponent)
        {
            m_physComponent->setDisabled( bfalse );
            m_physComponent->unstick();
            m_physComponent->setStickingDisabled(btrue);
            m_physComponent->setGravityMultiplier(0.0f);
            m_physComponent->setUpdateRotation(bfalse);
            m_physComponent->setAirFrictionMultiplier(getTemplate()->getFloatAirFrictionMultiplier());
            m_physComponent->setSpeed(Vec2d::Zero);
        }

        if ( !getTemplate()->getSpawnOnMarker())
        {
            spawnDeathFx();
        }

        m_timer = 0.0f;

        if (m_swellAction)
            setAction(m_swellAction);
        else 
            startFloat();

        m_floatForceTimer = getTemplate()->getFloatForceTime();
        m_stimPossibilityTimer = getTemplate()->getWaitDurationBeforeStimPossibility();
        m_accumulatedTime = 0.0f;
        m_wasHit = bfalse;

        // needs a round phantom matching bubble's radius
        if (m_phantomComponent)
        {
            m_phantomComponent->setShape(ITF_GET_STRINGID_CRC(deathPhantom,1143035681));
            m_phantomComponent->setDisabled(bfalse);
        }
        static_cast<Ray_AIComponent*>(m_aiComponent)->setSoftCollisionRadiusMulitiplier(getTemplate()->getSoftCollisionRadiusMultiplier());
        m_firstUpdate =  btrue;

        // manage reward at death start
        ObjectRef rewardReceiver = static_cast<Ray_AIComponent*>(m_aiComponent)->getRewardReceiver();
        if ( ( m_rewardAtStart != NULL ) && rewardReceiver.isValid() )
        {
            m_rewardAtStart->clear();
            m_rewardAtStart->setup(m_actor);
            // reward number is serialized in the event
            m_rewardAtStart->setRewardReceiver( rewardReceiver );
            m_rewardAtStart->setEjectionDir( Vec2d::Up );
            GAMEMANAGER->onEvent(m_rewardAtStart);
        }
     
        if (GAMEMANAGER->isPlayerActor(rewardReceiver))
        {
            Player* player = GAMEMANAGER->getPlayerFromActor(rewardReceiver);
            if(player)
            {
                REWARD_MANAGER->Action_New(Ray_RewardManager::Action_KillEnnemy, player->getActiveIndex());
            }
        }        
    }

    void Ray_AIBubbleDeathBehavior::onEvent( Event * _event )
    {
        Super::onEvent( _event );
        if (AnimGameplayEvent * event = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (m_wasHit && getTemplate()->getSpawnOnMarker() && event->getName() == ITF_GET_STRINGID_CRC(MRK_DeathFX,1031732103))
            {
                spawnDeathFx();
            }
        }
        else if (PunchStim * stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            processStim(stim);

        }
        else if (EventCrushed * onCrush = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
        {
            processCrush(onCrush);

        }
        else if (EventBlockedByPolyline * _onBlocked = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)))
        {
            processBlockedByPolyline(_onBlocked);
        }
        else if (EventInteractionQuery * onQueryInteraction = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            processInteractionQuery(onQueryInteraction);

        }
        else if (Ray_EventQueryPedestalInfo * onQueryPedestal = _event->DynamicCast<Ray_EventQueryPedestalInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028)))
        {
            processQueryPedestalInfo(onQueryPedestal);

        }
        else if (Ray_EventReleasePedestal * onReleasePedestal = _event->DynamicCast<Ray_EventReleasePedestal>(ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324)))
        {
            processReleasePedestal(onReleasePedestal);
        }
        else if (VacuumStim * vacuum = _event->DynamicCast<VacuumStim>(ITF_GET_STRINGID_CRC(VacuumStim,593568508)))
        {
            if ( !m_wasHit )
            {
                m_wasHit = btrue;
                m_hitDir = Vec2d::Up;
                startExplode();
                spawnReward( m_hitDir, vacuum->getSender(), bfalse );
            }
        }
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
            if (m_actor->getWorld() != SPAWNER->getSpawnerWorld())
            {
                const Ray_EventSpawnRewardLum *reward = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>(getTemplate()->getReward(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
                const Ray_EventSpawnRewardLum *rewardAtStart = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>(getTemplate()->getRewardAtStart(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
                if (reward)
                    queryLumCount->addNormalLums(reward->getNumRewards());
                if ( rewardAtStart )
                    queryLumCount->addNormalLums(rewardAtStart->getNumRewards());
            }
        }

    }
    void Ray_AIBubbleDeathBehavior::processBlockedByPolyline( EventBlockedByPolyline * _onBlocked )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(_onBlocked->getPolyline(),_onBlocked->getEdge(),poly,edge);

        if ( poly && edge )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

            if ( mat && mat->getDangerousLevel() > 0 && m_currentAction != m_explodeAction)
            {
                startExplode();
            }
        }
    }

    void Ray_AIBubbleDeathBehavior::spawnDeathFx()
    {
        if (m_fxSpawner.isInitialized())
        {
            m_fxSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos(), m_actor->getAngle());
        }
    }

    void Ray_AIBubbleDeathBehavior::initRewardAndFx()
    {
        if (!getTemplate()->getFxPath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_fxSpawner, getTemplate()->getFxPath());
        }

        if ( getTemplate()->getReward() )
        {
            m_reward = getTemplate()->getReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getReward()),m_reward);
        }

        if ( getTemplate()->getRewardAtStart() )
        {
            m_rewardAtStart = getTemplate()->getRewardAtStart()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getRewardAtStart()),m_rewardAtStart);
        }        
    }

    void Ray_AIBubbleDeathBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_swellAction)
        {
            startFloat();
        }
        else if (m_currentAction == m_floatAction)
        {
            startExplode();
        }
        else if (m_currentAction == m_explodeAction)
        {
            if (m_actor->isSerializable())
                m_actor->disable();
            else
                m_actor->requestDestruction();
            m_aiComponent->onBehaviorFinished();
        }
    }

    void Ray_AIBubbleDeathBehavior::update( f32 _dt )
    {
        if (m_firstUpdate)
        {
            m_firstUpdate = bfalse;
            return;
        }

        Super::update( _dt );

        if (m_currentAction == m_swellAction)
        {
            updateSwell();

        }
        if (m_currentAction == m_floatAction)
        {
            updateFloat(_dt);
        }


        m_timer = Max(0.0f,m_timer - _dt);
        m_floatForceTimer = Max(0.0f,m_floatForceTimer - _dt);
        m_floatForceExtraTimer = Max(0.0f,m_floatForceExtraTimer - _dt);
        m_stimPossibilityTimer = Max(0.0f,m_stimPossibilityTimer - _dt);
        for (ITF_VECTOR<std::pair<ActorRef, f32> >::iterator it = m_recentlySupportedActors.begin(); it != m_recentlySupportedActors.end();)
        {
            if ((*it).second - _dt <= 0.0f)
                it = m_recentlySupportedActors.erase(it);
            else
            {
                (*it).second = (*it).second - _dt;
                ++it;
            }
        }
    }

    void Ray_AIBubbleDeathBehavior::startExplode()
    {
        if (m_wasHit)
        {

        }
        setAction(m_explodeAction);
    }

    void Ray_AIBubbleDeathBehavior::startFloat()
    {
        setAction(m_floatAction);
        m_initialAngle = m_actor->getAngle();
        m_timer = m_floatTime;
        m_accumulatedTime = 0.0f;
    }

    void Ray_AIBubbleDeathBehavior::updateSoftColl()
    {
        const f32 radius = getSoftCollRadius();
        if (radius < MTH_EPSILON)
        {
            return;
        }

        const f32 sqrDetectionRadius = sqr(2 * radius);
        const f32 timeMultiplier = 1.0f - (m_floatForceTimer / getTemplate()->getFloatForceTime());

#ifndef ITF_FINAL
        //DebugDraw::circle(m_actor->get2DPos(),m_actor->getDepth(),radius,Color::red(),0.0f,"SoftCol");
#endif // ITF_FINAL

        Vec2d totalForce = Vec2d::Zero;

        AIManager::LayerIterator iterator(m_actor->getDepth());
        while ( Actor* otherActor = iterator.getNext() )
        {
            if (otherActor == m_actor)
                continue;
            if (m_supportedActors.find( otherActor->getRef() ) != -1)
                continue;

            if ((otherActor->get2DPos() - m_actor->get2DPos()).sqrnorm() > sqrDetectionRadius )
            {
                continue;
            }

            const Player* const player = GAMEMANAGER->getPlayerFromActor(otherActor->getRef(), bfalse);
            if ( player )
            {
                if (player->isDead())
                {
                    continue;
                }

                if (getTemplate()->getExplodeOnPlayer())
                {
                    EventCollide collide;
                    collide.setSender(m_actor->getRef());
                    collide.setPos(m_actor->get2DPos());
                    collide.setAngle(m_actor->getAngle());

                    PhysShapeCircle circle;
                    if ( m_phantomComponent != NULL )
                    {
                        collide.setShape( m_phantomComponent->getShape() );
                    }
                    else
                    {
                        circle.setRadius( m_physComponent->getRadius() );
                        collide.setShape(&circle);
                    }

                    otherActor->onEvent(&collide);

                    if (collide.isCollide())
                    {
                        m_wasHit = btrue;
                        spawnReward( Vec2d::Up, otherActor->getRef() );
                        startExplode();
                        return;
                    }
                }
            }

            const Vec2d force = -Ray_AIUtils::getSoftCollForce(otherActor,m_actor->get2DPos(),radius,getTemplate()->getMaxFloatSpeed(),getTemplate()->getAvoidanceForce());
            if (player)
            {
                if (force.dot(Vec2d::Up) > 0.0f)
                {
                    totalForce += force;
                }
            }
            else
            {
                totalForce += force;
            }
        }

#ifndef ITF_FINAL
        //DebugDraw::arrow2D(m_actor->get2DPos(),m_actor->get2DPos() + totalForce,m_actor->getDepth(),Color::white(),1.5f,1.0f,1.0f);
#endif // ITF_FINAL
        totalForce.m_x *= timeMultiplier;
        //totalForce.m_y *= (1.0f - timeMultiplier);
        m_physComponent->addForce(timeMultiplier * totalForce);
    }

    void Ray_AIBubbleDeathBehavior::updateFloat( f32 _dt )
    {
        if (m_timer < getTemplate()->getExplosionFeedBackTime())
        {
            f32 scale = 1.0f  + getTemplate()->getExplosionFeedBackAmplitude() + getTemplate()->getExplosionFeedBackAmplitude()* f32_Sin(m_timer * getTemplate()->getExplosionFeedBackFreq());
            scale = Max(0.0f,scale);
            m_animComponent->setScaleMultiplier(scale);
        }

        if (m_timer == 0.0f)
        {
            startExplode();
        }
        else
        {
            if (checkSquash())
            {
                return;
            }

            if (getTemplate()->getCheckWater() && checkWater())
            {
                return;
            }

            updatePedestal();
            updateSoftColl();
            updateAngle(_dt);
            updateFloatForce();
        }
    }

    void Ray_AIBubbleDeathBehavior::updateAngle( f32 _dt )
    {
        m_accumulatedTime += _dt;
        const f32 sinus = sinf(m_accumulatedTime * getTemplate()->getFloatAngleFrequency() * MTH_2PI);
        const f32 angleOffset = getTemplate()->getFloatAngleOffset().Radians() * sinus;
        m_actor->setAngle(m_initialAngle + angleOffset);
    }

    f32 getForceMultiplier( const f32 speed, const f32 minSpeed, const f32 maxSpeed )
    {
        const f32 speedRange = maxSpeed - minSpeed;
        const f32 clampedSpeed = Clamp(speed - minSpeed, 0.0f, speedRange);
        const f32 forceMultiplier = 1.0f - (clampedSpeed / speedRange);
        return forceMultiplier;
    }

    void Ray_AIBubbleDeathBehavior::updateFloatForce()
    {
        Vec2d force;


        // handle Y speed
        if (getPedestalUserCount() > 0)
        {
            // heavy
            const f32 forceMultiplier = getForceMultiplier( -m_physComponent->getSpeed().m_y, getTemplate()->getMinSinkSpeed(), getTemplate()->getMaxSinkSpeed() );
            force.m_y = forceMultiplier * -getTemplate()->getSinkForce();
        }
        else
        {
            // empty
            if (m_floatForceExtraTimer > 0.0f)
            {
                const f32 forceMultiplier = getForceMultiplier( m_physComponent->getSpeed().m_y, getTemplate()->getMinFloatExtraSpeed(), getTemplate()->getMaxFloatExtraSpeed() );
                force.m_y = forceMultiplier * getTemplate()->getFloatForce();
            }
            else
            {
                const f32 forceMultiplier = getForceMultiplier( m_physComponent->getSpeed().m_y, getTemplate()->getMinFloatSpeed(), getTemplate()->getMaxFloatSpeed() );
                force.m_y = forceMultiplier * getTemplate()->getFloatForce();
            }
        }


        // handle X speed
        const f32 forceMultiplier = getForceMultiplier( m_physComponent->getSpeed().m_x * getSign(m_floatForceX), getTemplate()->getMinXSpeed(), getTemplate()->getMaxXSpeed() );
        force.m_x = forceMultiplier * m_floatForceX;


        m_physComponent->addForce(force);
#ifndef ITF_FINAL
        //DebugDraw::arrow2D(m_actor->get2DPos(),m_actor->get2DPos() + force,m_actor->getDepth(),Color::green(),1.5f,1.0f,1.0f);
#endif // ITF_FINAL
    }

    void Ray_AIBubbleDeathBehavior::updateSwell()
    {
        if (checkSquash())
        {
            return;
        }

        updatePedestal();
        updateSoftColl();
        updateFloatForce();
    }

    bbool Ray_AIBubbleDeathBehavior::checkSquash()
    {
        if (getTemplate()->getSquashPenetrationRadius() == 0.0f)
            return bfalse;

        const f32 radius = m_physComponent->getRadius();

        if ( Ray_AIUtils::getSquashingPolyline(m_actor,
                m_physComponent->getStickedPolylineRef(), m_physComponent->getStickedEdge(),
                radius, radius * getTemplate()->getSquashPenetrationRadius()) )
        {
            startExplode();
            return btrue;
        }

        return bfalse;
    }

    Vec2d Ray_AIBubbleDeathBehavior::getPedestalPos( u32 _slotIndex )
    {
        static const f32 tplAngleIncrement = 15 * MTH_DEGTORAD;
        f32 angle = 0.f;
        switch (_slotIndex)
        {
        case 0:
            angle = -tplAngleIncrement;
            break;
        case 1:
            angle = tplAngleIncrement;
            break;
        case 2:
            angle = -tplAngleIncrement * 2;
            break;
        case 3:
            angle = tplAngleIncrement * 2;
            break;
        }

        const f32 radius = m_physComponent->getRadius() + getTemplate()->getPedestalOffset();
        return m_actor->get2DPos() + Vec2d(0, radius).Rotate(angle +m_actor->getAngle());
    }

    i32 Ray_AIBubbleDeathBehavior::getPedestalFreeSlotIndex()
    {
        for (u32 i = 0; i < m_supportedActors.size(); i++)
        {
            if (!m_supportedActors[i].isValid())
            {
                return i;
            }
        }

        return -1;
    }

    u32 Ray_AIBubbleDeathBehavior::getPedestalUserCount()
    {
        u32 count = 0;

        for (u32 i = 0; i < m_supportedActors.size(); i++)
        {
            if (m_supportedActors[i].isValid())
            {
                count++;
            }
        }

        return count;
    }

    void Ray_AIBubbleDeathBehavior::updatePedestal()
    {
        i32 slotIndex = getPedestalFreeSlotIndex();
        if ( slotIndex == -1 )
        {
            return;
        }


        const f32 radius = (m_physComponent->getRadius() + getTemplate()->getPedestalOffset()) * 1.25f;
        const PhysShapeCircle shape(radius);
        const Vec2d checkPos = m_actor->get2DPos();

        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(checkPos,checkPos,m_actor->getAngle(),&shape,m_actor->getDepth(),(ECollisionFilter)(ECOLLISIONFILTER_CHARACTERS),contacts);

#ifndef ITF_FINAL
        //DebugDraw::shape(checkPos, m_actor->getDepth(), 0.f, &shape);
        //DebugDraw::circle(getPedestalPos(slotIndex), m_actor->getDepth(), 0.1f);
#endif // ITF_FINAL

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& contact = contacts[index];

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            Actor* otherActor = AIUtils::getActor(contact.m_collidableUserData);
            if ( !otherActor )
            {
                continue;
            }

            if ( !GAMEMANAGER->isPlayerActor(contact.m_collidableUserData) )
            {
                continue;
            }


            Ray_EventQueryAIData query;
            otherActor->onEvent(&query);

            if (query.getReducedSize())
            {
                Ray_EventCheckPedestalSupport pedestalCheck( m_actor->getRef(), getPedestalPos(slotIndex) );
                otherActor->onEvent(&pedestalCheck);

                if ( pedestalCheck.getTriggerPedestal() )
                {
                    m_supportedActors[slotIndex] = otherActor->getRef();


                    for (ITF_VECTOR<std::pair<ActorRef, f32> >::iterator it = m_recentlySupportedActors.begin(); it != m_recentlySupportedActors.end();)
                    {
                        if ((*it).first == otherActor->getRef())
                            it = m_recentlySupportedActors.erase(it);
                        else
                        {
                            ++it;
                        }
                    }

                    slotIndex = getPedestalFreeSlotIndex();
                    if ( slotIndex == -1 )
                    {
                        return;
                    }
                }
            }
        }
    }

    bbool Ray_AIBubbleDeathBehavior::canSupport()
    {
        return getPedestalUserCount() < getTemplate()->getPedestalMaxUserCount();
    }

    f32 Ray_AIBubbleDeathBehavior::getSoftCollRadius() const
    {
        if (getTemplate()->getUsePhysRadiusAsSoftCollRadius())
        {
            return m_physComponent->getRadius() * getTemplate()->getSoftCollisionRadiusMultiplier();
        }
        else
        {
            return m_actor->getScale().norm() * getTemplate()->getAvoidanceRadius();
        }
    }

    void Ray_AIBubbleDeathBehavior::processStim( PunchStim* stim )
    {
        if ( m_currentAction == m_explodeAction )
        {
            return;
        }

        // wait before stim possibility
        if ( m_stimPossibilityTimer > 0.0f )
        {
            return;
        }

        if ( !AIUtils::isEnemyHit(stim, m_aiComponent->getFaction()) )
        {
            return;
        }

        if (m_supportedActors.find(stim->getSender()) != -1)
        {
            return;
        }

        for (u32 i = 0; i < m_recentlySupportedActors.size(); ++i)
        {
            std::pair<ActorRef,f32> & info = m_recentlySupportedActors[i];
            if (info.first == stim->getSender())
                return;
        }


        m_wasHit = btrue;

        m_hitDir = stim->getDirection();
        startExplode();

        if ( m_reward != NULL )
        {
            ActorRef receiver = static_cast<Ray_AIComponent*>(m_aiComponent)->getRewardReceiver();
            if (GAMEMANAGER->isPlayerActor(stim->getSender()))
            {
                receiver = stim->getSender();
            }
            else
            {
                // if no receiver, get current hit owner
                if ( !receiver.isValid() )
                {
                    receiver = stim->getOriginalSender();
                }

            }
            if ( receiver.isValid() )
            {
                spawnReward( m_hitDir, receiver );
            }
        }

        if ( Actor* sender = ActorRef(stim->getSender()).getActor() )
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        if (m_fxController)
        {
            u32 fxHandle = m_fxController->playFeedback(stim->getSender(),ITF_GET_STRINGID_CRC(hit_crush,3169038330),m_actor->getRef());
            m_fxController->setFXPos(fxHandle,stim->getFxPos());
        }
    }

    void Ray_AIBubbleDeathBehavior::processCrush( EventCrushed * onCrush )
    {
        if (m_currentAction != m_explodeAction)
        {
            m_wasHit = btrue;
            m_hitDir = Vec2d::Up;
            startExplode();
            spawnReward( m_hitDir, onCrush->getSender() );
        }
    }

    void Ray_AIBubbleDeathBehavior::processInteractionQuery( EventInteractionQuery* onQueryInteraction )
    {
        ActorRef otherRef = onQueryInteraction->getSender();
        if ( Actor* otherActor = otherRef.getActor() )
        {
            if (GAMEMANAGER->isPlayerActor(otherRef) )
            {
                Ray_EventQueryAIData query;
                otherActor->onEvent(&query);
                if (query.getReducedSize())
                {
                    // if can't support any more mini players, we may explode
                    if (!canSupport() && (otherActor->getBase().m_y > m_actor->getPos().m_y))
                        onQueryInteraction->setInteraction(CharacterInteractionType_Crush);
                    else
                        onQueryInteraction->setInteraction(CharacterInteractionType_None);
                }
                else
                {
                    // never support big players -> explode
                    if (otherActor->getBase().m_y > m_actor->getPos().m_y)
                        onQueryInteraction->setInteraction(CharacterInteractionType_Crush);
                    else
                        onQueryInteraction->setInteraction(CharacterInteractionType_None);
                }
            }
            else
            {
                onQueryInteraction->setInteraction(CharacterInteractionType_Crush);
            }
        }
    }

    void Ray_AIBubbleDeathBehavior::processQueryPedestalInfo( Ray_EventQueryPedestalInfo* onQueryPedestal )
    {
        if (m_currentAction == m_explodeAction)
        {
            onQueryPedestal->setValid(bfalse);
        }
        else
        {
            const i32 iUser = m_supportedActors.find( onQueryPedestal->getSender() );
            if ( iUser != -1 )
            {
                onQueryPedestal->setValid(btrue);
                onQueryPedestal->setAllowFallFromMoveDown(bfalse);
                onQueryPedestal->setPedestalPos(getPedestalPos(iUser));
                onQueryPedestal->addObjectOnPedestal(onQueryPedestal->getSender());
            }
        }
    }

    void Ray_AIBubbleDeathBehavior::processReleasePedestal( Ray_EventReleasePedestal * onReleasePedestal )
    {
        const i32 iUser = m_supportedActors.find( onReleasePedestal->getSender() );
        if ( iUser != -1 )
        {
            m_supportedActors[iUser] = ITF_INVALID_OBJREF;
            m_recentlySupportedActors.push_back(std::pair<ActorRef,f32>(onReleasePedestal->getSender(),getTemplate()->getSupportedActorInvincibilityTimer()));
            if (getPedestalUserCount() == 0)
            {
                m_floatForceExtraTimer = getTemplate()->getFloatForceExtraTime();
            }
        }
    }

    void Ray_AIBubbleDeathBehavior::spawnReward( const Vec2d & _dir, ObjectRef _receiver, bbool _achievement )
    {

        if (Player * player =GAMEMANAGER->getPlayerFromActor(_receiver,bfalse))
        {
            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_PoppedEnemyBubble, player->getIndex());
        }

        if ( m_reward != NULL)
        {
            m_reward->clear();
            m_reward->setup(m_actor);
            // reward number is serialized in the event
            m_reward->setRewardReceiver( _receiver );
            m_reward->setEjectionDir( _dir );
            GAMEMANAGER->onEvent(m_reward);
        }
    }

    bbool Ray_AIBubbleDeathBehavior::checkWater()
    {
        const f32 radius = m_physComponent->getRadius();
        const Vec2d checkPos = m_actor->get2DPos() - Vec2d(0, radius);
#ifndef ITF_FINAL
        //DebugDraw::circle(checkPos, m_actor->getDepth(), 0.1f);
#endif // ITF_FINAL

        if ( AIUtils::getPolylineRegion(m_actor->getDepth(), s_RegionName_Swim, m_actor->get2DPos()) )
        {
            startExplode();
            return btrue;
        }

        return bfalse;
    }

}
