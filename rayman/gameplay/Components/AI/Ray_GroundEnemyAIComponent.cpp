#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AIHITWALLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIHitWallBehavior.h"
#endif //_ITF_RAY_AIHITWALLBEHAVIOR_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AISleepBehavior.h"
#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GroundEnemyAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_GroundEnemyAIComponent)
    SERIALIZE_MEMBER("useRaycast",m_useRayCast);
    END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_GroundEnemyAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "Ray_GroundEnemyAIComponent requires a StickToPolylinePhysComponent");
    END_VALIDATE_COMPONENT()


    Ray_GroundEnemyAIComponent::Ray_GroundEnemyAIComponent() 
        : Super()
        , m_roamBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_spawnBehavior(NULL)
        , m_closeRangeAttackBehavior(NULL)
        , m_hitWallBehavior(NULL)
        , m_sleepBehavior(NULL)
        , m_physComponent(NULL)
        , m_timeCounter(0.f)
        , m_stance(Stance_Normal)
        , m_EnemyIsInFront(btrue)
        , m_closeRangeDetectionShape(NULL)
        , m_crushSize(bfalse)
        , m_posSet(bfalse)
        , m_rayCastInitPos(btrue)
        , m_useRayCast(btrue)
        , m_bindSkippedFrame(bfalse)
        , m_previousHitSender()
    {
    }

    Ray_GroundEnemyAIComponent::~Ray_GroundEnemyAIComponent()
    {
        SF_DEL(m_closeRangeDetectionShape);
    }

    void Ray_GroundEnemyAIComponent::updateReceiveHit()
    {
    }


    void Ray_GroundEnemyAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {


        m_roamBehavior = createAiBehavior(getTemplate()->getRoamBehavior());
        m_receiveHitBehavior = createAiBehaviorAndCast<Ray_AIReceiveHitBehavior>(getTemplate()->getReceiveHitBehavior());
        m_deathBehavior =createAiBehavior(getTemplate()->getDeathBehavior());
        m_crushedBehavior = createAiBehavior(getTemplate()->getCrushedBehavior());
        m_spawnBehavior = createAiBehavior(getTemplate()->getSpawnBehavior());
        m_closeRangeAttackBehavior = createAiBehavior(getTemplate()->getCloseRangeAttackBehavior());
        m_hitWallBehavior = createAiBehaviorAndCast<Ray_AIHitWallBehavior>(getTemplate()->getHitWallBehavior());
        m_sleepBehavior = createAiBehaviorAndCast<Ray_AISleepBehavior>(getTemplate()->getSleepBehavior());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSpawn,3403067276), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527),this);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated



        if (getTemplate()->getCloseRangeDetectionShape())
        {
            if ( !m_closeRangeDetectionShape ||
                m_closeRangeDetectionShape->GetObjectClassCRC() != getTemplate()->getCloseRangeDetectionShape()->GetObjectClassCRC() )
            {
                SF_DEL(m_closeRangeDetectionShape);
                m_closeRangeDetectionShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getCloseRangeDetectionShape()->GetObjectClassCRC());
            }

            // make sure the existing scaled shape is not marked as flipped before copying the points
            if ( PhysShapePolygon* polygon = m_closeRangeDetectionShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                polygon->setFlipped(bfalse);
            }

            PhysShape::CopyShapeScaled(getTemplate()->getCloseRangeDetectionShape(), m_closeRangeDetectionShape, m_actor->getScale());
        }
        onActivate();
    }

    void Ray_GroundEnemyAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        //reactivate all children binds
        setChildrenBindRuntimeDisabled(bfalse);
    }

    void Ray_GroundEnemyAIComponent::onActivate()
    {
        if ( m_phantomComponent )
        {
            m_phantomComponent->setDisabled(bfalse);
        }

        m_timeCounter = 0.f;
        startSpawn();
        m_posSet = bfalse;
    }


    void Ray_GroundEnemyAIComponent::Update( f32 _dt )
    {

        if (m_closeRangeDetectionShape)
        {
            if ( PhysShapePolygon* polygon = m_closeRangeDetectionShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                polygon->setFlipped(m_actor->isFlipped());
            }
        }


        if (ActorBind * bind = m_actor->getParentBind())
        {
            if (!m_bindSkippedFrame && !bind->isRuntimeDisabled())
            {
                m_bindSkippedFrame = btrue;
                return;
            }
        }
        else
            m_bindSkippedFrame = btrue;

        if (m_bindSkippedFrame && !m_posSet )
        {
            m_posSet  = btrue;
            if (m_rayCastInitPos && m_useRayCast)
            {
                {
                    if (ActorBind * bind = m_actor->getParentBind())
                    {
                        bind->setRuntimeDisabled(btrue);
                    }


                    Vec2d groundTestVec = -Vec2d::Up.Rotate(m_actor->getAngle()) *getTemplate()->getRayCastDist();// Vec2d( 0.0f, -getTemplate()->getRayCastDist() );
                    PhysRayCastContactsContainer groundContact;
                    f32 nearestVal = 99999.9f; 
                    i32 nearestIdx = -1;

                    Vec2d foundPoint;

                    if ( !PHYSWORLD->rayCastEnvironment(	m_actor->get2DPos(), 
                        m_actor->get2DPos() + groundTestVec, 
                        ECOLLISIONFILTER_ENVIRONMENT,  
                        m_actor->getDepth(), groundContact ) )
                    {
                        return;
                    }

                    Vec2d normal;

                    for ( u32 i = 0; i < groundContact.size(); ++i )
                    {
                        if ( groundContact[i].m_t0 < nearestVal )
                        {
                            PolyLine* poly = AIUtils::getPolyLine(groundContact[i].m_collidableUserData);

                            if ( poly )
                            {

                                normal = poly->getEdgeAt(groundContact[i].m_edgeIndex0).m_normalizedVector.getPerpendicular();

                                if ( normal.dot( groundTestVec ) < 0.0f )
                                {
                                    nearestVal = groundContact[i].m_t0;
                                    nearestIdx = i;
                                }
                            }
                        }
                    }
                    // Has a valid point use it
                    if ( nearestIdx >= 0 )
                    {
                        foundPoint = ( groundTestVec * groundContact[nearestIdx].m_t0 );
                        EventForceStickOnEdge event;
                        event.setEdgeIndex(groundContact[nearestIdx].m_edgeIndex0);
                        event.setPolyline(groundContact[nearestIdx].m_collidableUserData);
                        event.setPos(foundPoint + m_actor->get2DPos());
                        m_actor->onEvent(&event);
                    }
                }
            }

        }
        if (m_fxController)
        {
            StringID matId;

            if (m_physComponent->getStickedEdgeIndex() != U32_INVALID)
            {
                BaseObject* obj = GETOBJECT(m_physComponent->getStickedPolylineRef());
                if ( obj )  
                {
                    PolyLine * poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
                    if (poly)
                    {
                        matId = poly->getEdgeAt(m_physComponent->getStickedEdgeIndex()).getGameMaterial();
                    }
                }
            }
            m_fxController->setMaterial(matId);
        }
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdgeIndex(),poly,edge);

        if ( poly && edge && !m_hasBeenVaccumed )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

            if (  mat && ((mat->getDangerousLevel() > 0 && !getTemplate()->getInvincibleToDangerousMaterial()) || (mat->getIsWave()&&mat->getDangerousLevel() > 0)))
            {
                startDeath();
                return;
            }
        }
        updateBehaviors(_dt);


        AABB aabb;
        PhysShape * shape = getCloseRangeDetectionShape();
        if (shape)
        {
            shape->calculateAABB(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),aabb);
            m_actor->growRelativeAABBFromAbsolute(aabb);
        }

        Super::Update(_dt);
    }

    void Ray_GroundEnemyAIComponent::updateBehaviors( f32 _dt )
    {
        if( checkHealthDeath() )
        {
            startDeath();
            return;
        }
        
        if ( m_currentBehavior->isExtern() )
            return;

        PolyLine* deathPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Death,m_actor->get2DPos());
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

        if (deathPoly && m_currentBehavior != m_deathBehavior)
        {
            startDeath();
            return;
        }

        if ( waterPoly && getTemplate()->getDieInWater())
        {
            startDeath();
            return;
        }

        if (getTemplate()->getCheckSquash() && m_currentBehavior != m_deathBehavior && m_currentBehavior != m_hitWallBehavior && !m_hasBeenVaccumed )
        {
            if (Ray_AIUtils::getSquashingPolyline(m_actor,m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdge(),m_physComponent->getRadius(),getTemplate()->getSquashPenetrationRadius() * m_physComponent->getRadius()))
            {
                startDeath();
                return;
            }
        }
        

        if ( m_currentBehavior == m_roamBehavior )
        {
            updateRoam(_dt);
        }
        else if ( isCurrentBehavior(m_crushedBehavior))
        {
            // nothing to do ATM...
        }
        //else if ( isCurrentBehavior(m_closeRangeAttackBehavior))
        //{
        //    checkReactionAttack();
        //}
        else if ( m_currentBehavior == m_receiveHitBehavior)
        {
            updateReceiveHit();
        }
        else if (isCurrentBehavior(m_sleepBehavior))
        {
            updateSleep(_dt);
        }
    }

    bbool Ray_GroundEnemyAIComponent::checkHealthDeath()
    {
        if( getHealth() > 0
            || isCurrentBehavior(m_hitWallBehavior)
            || isCurrentBehavior(m_deathBehavior)
            || isCurrentBehavior(m_receiveHitBehavior)
            || isCurrentBehavior(m_crushedBehavior) )
        {
            return bfalse;
        }
        else
        {
            return btrue;
        }
    }

    void Ray_GroundEnemyAIComponent::updateSleep( f32 _dt )
    {
        checkReactionAttack();
    }
    void Ray_GroundEnemyAIComponent::updateRoam( f32 _dt )
    {
        if (getTemplate()->getCheckStickForCloseRangeAttack())
        {
            if (m_physComponent->getStickedPolyline())
                checkReactionAttack();
        }
        else
        {
            checkReactionAttack();
        }
    }



    void Ray_GroundEnemyAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if ( isCurrentBehavior(m_crushedBehavior ))
        {
            onFinishedCrush();
        }
        else if ( m_currentBehavior == m_receiveHitBehavior )
        {
            onFinishedReceiveHit();
        }
        else if ( m_currentBehavior == m_deathBehavior )
        {
            onFinishedDeath();
        }
        else if (m_currentBehavior == m_spawnBehavior)
        {
            onFinishedSpawn();
        }
        else if (m_currentBehavior == m_closeRangeAttackBehavior)
        {
            onFinishedCloseRangeAttack();
        }
        else if (isCurrentBehavior(m_hitWallBehavior))
        {
            if(getHealth() <= 0)
            {
                startDeath();
            }
            else
            {
                startRoam();
            }
        }
        else if ( m_currentBehavior == m_sleepBehavior )
        {
            onFinishedSleep();
        }
    }

    void Ray_GroundEnemyAIComponent::onFinishedReceiveHit()
    {
        m_previousHitSender.invalidate();

        m_phantomComponent->setDefaultShape();
        if (getHealth() <= 0)
        {
            startDeath();
            return;
        }

        startRoam();
    }

    void Ray_GroundEnemyAIComponent::updateDeath( f32 _dt )
    {
        if (m_currentBehavior != m_deathBehavior)
        {
            startDeath();
        }
    }

    void Ray_GroundEnemyAIComponent::startRoam(const bbool _restart/* = bfalse*/)
    {
        setStance(Stance_Normal);
     
        if (m_roamBehavior)
        {
            setBehavior(m_roamBehavior, _restart);
        }
        else
        {
            // avoid recursion
            ITF_ASSERT_MSG(0, "startRoam but I don't have a roam ... what do I do??");
        }
    }

    void Ray_GroundEnemyAIComponent::startReceiveHit( HitStim* _hit )
    {
        m_previousHitSender = _hit->getSender();

        setStance(Stance_Fight);
        if (m_currentBehavior == m_receiveHitBehavior)
            m_receiveHitBehavior->setData(_hit);
        else
        {
            setBehavior(m_receiveHitBehavior);
            if (m_currentBehavior == m_receiveHitBehavior)
            {
                m_receiveHitBehavior->setData(_hit);
            }
        }

        if( getTemplate()->unbindChildrenOnHit() )
        {
            setChildrenBindRuntimeDisabled(btrue);
        }
    }

    void Ray_GroundEnemyAIComponent::startDeath()
    {
        setHealth(0);
        setBehavior(m_deathBehavior);
        setChildrenBindRuntimeDisabled(btrue);
    }

    void Ray_GroundEnemyAIComponent::startCrushed()
    {
        m_physComponent->setSpeed(Vec2d::Zero);
        if (m_crushedBehavior)
            setBehavior(m_crushedBehavior,btrue);
        else
            onFinishedCrush();
    }
    void Ray_GroundEnemyAIComponent::startSleep()
    {
        if (m_sleepBehavior)
        {
            setStance(Stance_Normal);
            m_timeCounter = 0.0f;
            setBehavior(m_sleepBehavior);
        }
        else
        {
            onFinishedSleep();
        }
    }

    void Ray_GroundEnemyAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHit(hit);
        }
        else if (Ray_EventBounceToLayer * bounceToLayer = _event->DynamicCast<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527)))
        {
            processBounceToLayer(bounceToLayer);
        }
        else if ( Ray_EventQueryCanBeAttacked* query = _event->DynamicCast<Ray_EventQueryCanBeAttacked>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644)) )
        {
            processQueryCanBeAttacked(query);
        }
        else if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
        {
            processInteractionQuery(interactionQuery);
        }
        else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            processCrushed(crushed);
        }
        else if ( EventQueryBlocksHits* queryBlocksHit = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)) )
        {
            processQueryBlocksHits(queryBlocksHit);
        }
        else if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventSpawn,3403067276)))
        {
            startSpawn();
        }
        else if (EventBlockedByPolyline * _onBlocked = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)))
        {
            processBlockedByPolyline(_onBlocked);
        }
        else if (AnimGameplayEvent * onMarker = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (onMarker->getName() == MRK_Death && isCurrentBehavior(m_crushedBehavior) )
            {
                if (getHealth() <= 0 || m_crushSize == bfalse)
                    startDeath();
            }
        }
        else if (EventHitSuccessful* onHitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)))
        {
            processHitSuccessful(onHitSuccessful);
        }
        else if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (isCurrentBehavior(m_spawnBehavior) && !m_actor->isEnabled())
            {
                m_actor->enable();
                startSpawn();
            }
        }
        else if (Ray_EventAddSoftCollForce * onAddForce = _event->DynamicCast<Ray_EventAddSoftCollForce>(ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693)))
        {
            processSoftCollision(onAddForce);
        }

    }

    void Ray_GroundEnemyAIComponent::processHit( PunchStim* _hit )
    {
        if ( !m_receiveHitBehavior )
        {
            // this AI isn't hittable...
            return;
        }

        if (!m_currentBehavior)
            return;

        if (isCurrentBehavior(m_deathBehavior) || isCurrentBehavior(m_hitWallBehavior))
        {
            return;
        }

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            // no re-hit if requested by stim (fix for RO-15442)
            if (_hit->getHitEnemiesOnce() && _hit->getSender() == m_previousHitSender)
            {
                return;
            }

            bbool isDominoHit = _hit->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800));
            if ( !m_receiveHitBehavior->canReceiveHit(_hit))
            {
                if (isDominoHit)
                    return;
                else
                {
                    startDeath();
                    return;
                }
            }

        }
        else
        {
            EventQueryCanReceiveHit query;
            query.setStim(_hit);
            m_currentBehavior->onEvent(&query);

            if (!query.getCanReceiveHit())
                return;

            //check receivehitbehavior as well
            if ( !m_receiveHitBehavior->canReceiveHit(_hit) )
            {
                return;
            }

        }



        // notify the sender that he touched us
        Actor* sender = AIUtils::getActor(_hit->getSender());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        
        // detect own seeking bullets for rewards' Action_KillHunterWithOwnBullet
        if (getTemplate()->getDetectHunterOwnBullet())
        {
            if (_hit->getOriginalSender() == m_actor->getRef())
            {
                REWARD_MANAGER->Action_New(Ray_RewardManager::Action_KillHunterWithOwnBullet, GAMEMANAGER->getMainIndexPlayer());
            }
        }


        startReceiveHit(_hit);

    }

    void Ray_GroundEnemyAIComponent::processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query )
    {
        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            query->setCanBeAttacked(bfalse);
            return;
        }

        query->setCanBeAttacked(m_currentBehavior != m_receiveHitBehavior);
    }

    void Ray_GroundEnemyAIComponent::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if ( interactionQuery->getInteraction() == CharacterInteractionType_None && m_crushedBehavior && m_currentBehavior != m_closeRangeAttackBehavior)
            {
                ActorRef ref = interactionQuery->getSender();
                Actor * actor = ref.getActor();
                if (actor)
                {
                    if (actor->getBase().m_y > m_actor->getPos().m_y)
                        interactionQuery->setInteraction(CharacterInteractionType_Crush);

                }
                m_currentBehavior->onEvent(interactionQuery);
            }
        }
    }

    void Ray_GroundEnemyAIComponent::processCrushed( EventCrushed* crushed )
    {
        //ITF_ASSERT_MSG(m_crushedBehavior, "got crushed but we don't have a crushed behavior!");
        if (m_crushedBehavior)
        {
            m_crushSize = 0;

            ActorRef ref = crushed->getSender();
            Actor * actor = ref.getActor();
            if (actor && GAMEMANAGER->isPlayerActor(ref))
            {
                Ray_EventQueryAIData query;
                actor->onEvent(&query);
                m_crushSize = query.getReducedSize();
            }

            receiveCrush(crushed);
            playLastHitFx();

            if (m_currentBehavior != m_deathBehavior)
            {
                startCrushed();
            }
        }
    }


    void Ray_GroundEnemyAIComponent::processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit )
    {
        _queryBlocksHit->setCanBlockHits( bfalse );
    }



    void Ray_GroundEnemyAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        const Vec2d speed = m_physComponent->getSpeed();
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed, 4218325830), speed.norm());
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY, 461177981), speed.dot(-getCurrentGravityDir()));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Stance, 2021520747), (u32)(m_stance));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EnemyIsInFront, 2121294079), static_cast<u32>(m_EnemyIsInFront));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(CrushSize, 2245983489), static_cast<u32>(m_crushSize));


        PhysActorForceArray windForces;
        Vec2d windForce = Vec2d::Zero;
        PHYSWORLD->getWindForces(m_actor->get2DPos(), Vec2d::Zero, m_actor->getDepth(), windForces);
        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            windForce += windForces[i].m_force;
        }
        f32 force = windForce.dot(Vec2d::Right.Rotate(m_actor->isFlipped()?MTH_PI-m_actor->getAngle():m_actor->getAngle()));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(windForce, 2257032332), force);
    }

    void Ray_GroundEnemyAIComponent::onFinishedCrush()
    {
        if(getHealth() <= 0)
            startDeath();
        else
            startRoam();
    }

    void Ray_GroundEnemyAIComponent::onFinishedDeath()
    {
        // nothing to do...
    }

    void Ray_GroundEnemyAIComponent::startSpawn()
    {
        if (m_spawnBehavior /*&& !m_actor->isEnabled()*/)
        {
            //m_actor->enable();
            setBehavior(m_spawnBehavior);
        }
        else
        {
            startSleep();
        }
    }

    void Ray_GroundEnemyAIComponent::onFinishedSpawn()
    {
        startRoam();
    }

    void Ray_GroundEnemyAIComponent::onFinishedSleep()
    {
        startRoam(btrue);
    }

    //************************************
    // @name	ITF::Ray_GroundEnemyAIComponent::setStance
    // @public 
    // @param	Stance _val
    // @return	void
    //************************************
    void Ray_GroundEnemyAIComponent::setStance(Stance _val)
    { 
        m_stance = _val; 
    }


    //************************************
    // @name	ITF::Ray_GroundEnemyAIComponent::getMoveDirectionFromAngle
    // @protected 
    // @param	f32 _angle
    // @return	Ray_GroundEnemyAIComponent::EMoveDir
    //************************************
    Ray_GroundEnemyAIComponent::EMoveDir Ray_GroundEnemyAIComponent::getMoveDirectionFromAngle( f32 _angle ) const
    {
        EMoveDir dir;

        if ( _angle <= (45.f*MTH_DEGTORAD) || _angle >= (315*MTH_DEGTORAD) )
        {
            dir = MOVEDIR_RIGHT;
        }
        else if ( _angle >= (45.f*MTH_DEGTORAD) && _angle <= (135.f*MTH_DEGTORAD) )
        {
            dir = MOVEDIR_UP;
        }
        else if ( _angle >= (135.f*MTH_DEGTORAD) && _angle <= (225.f*MTH_DEGTORAD) )
        {
            dir = MOVEDIR_LEFT;
        }
        else
        {
            dir = MOVEDIR_DOWN;
        }

        return dir;
    }

    void Ray_GroundEnemyAIComponent::startCloseRangeAttack(const bbool _flip)
    {
        if (m_closeRangeAttackBehavior)
        {
            setBehavior(m_closeRangeAttackBehavior);
        }
    }

    void Ray_GroundEnemyAIComponent::onFinishedCloseRangeAttack()
    {
        m_closeRangeAttackVictims.clear();
        startRoam();
    }


    PhysShape * Ray_GroundEnemyAIComponent::getCloseRangeDetectionShape() const
    {
        if (m_closeRangeDetectionShape )
            return m_closeRangeDetectionShape;
        else
            return m_phantomComponent->getShape();
    }

    void Ray_GroundEnemyAIComponent::checkReactionAttack()
    {
        if (!m_closeRangeAttackBehavior)
            return;


        AABB aabb;
        PhysShape * shape = getCloseRangeDetectionShape();
        if (shape)
            shape->calculateAABB(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),aabb);

        Actor* buffer [256];
        Actor* buffer2[256];
        ActorList players         (sizeof(buffer)/sizeof(Actor*) ,MemoryId::mId_Temporary,buffer);
        ActorList collidingObjects(sizeof(buffer2)/sizeof(Actor*),MemoryId::mId_Temporary,buffer2);

        AIUtils::getLivePlayers(m_actor->getDepth(),players);

        EventCollide collide;
        collide.setPos(m_actor->get2DPos());
        collide.setAngle(m_actor->getAngle());
        collide.setShape(getCloseRangeDetectionShape());

        for (u32 i = 0; i < players.size(); ++i)
        {
            if (players[i]->getAABB().checkOverlap(aabb))
            {
                players[i]->onEvent(&collide);
                if (collide.isCollide())
                {
                    collide.setCollide(bfalse);
                    collidingObjects.push_back(players[i]);
                }
            }
        }

        bbool hitOne = bfalse;

        ActorList list;
        bbool flip = bfalse;
        f32 sqdist= F32_INFINITY;

        //remove actors no longer in collision
        for (u32 i = 0; i < m_closeRangeAttackVictims.size(); ++i)
        {
            bbool found = bfalse;
            for (u32 j = 0; j < collidingObjects.size(); ++j)
            {
                if (collidingObjects[j]->getRef() == m_closeRangeAttackVictims[i])
                {
                    found = btrue;
                    break;
                }
            }
            if (!found)
            {
                m_closeRangeAttackVictims.eraseKeepOrder(i);
                i--;
            }
        }

        m_EnemyIsInFront = btrue;
        for ( u32 i = 0; i < collidingObjects.size(); i++ )
        {
            Actor* actor = collidingObjects[i];

            if (AIUtils::isTarget(getFaction(), AIUtils::getFaction(actor)) && list.find(actor) == -1)
            {
				if (m_closeRangeAttackVictims.find(actor->getRef()) == -1)
				{
					// New contact
					list.push_back(actor);
					m_closeRangeAttackVictims.push_back(actor->getRef());

				}
				else
				{
					// Already in list, we can hit him

					hitOne = btrue;
					PunchStim bounce;

					Vec2d direction = actor->get2DPos() - m_actor->get2DPos();
					f32 curSqDist = direction.sqrnorm();
					if (curSqDist < sqdist && AIUtils::getLookDir(m_actor,m_actor->isFlipped()).dot(direction) < 0.0f)
					{
						//flip towards closest actor if necessary
						sqdist = curSqDist;
						//remove flip for now
						flip = btrue;
						m_EnemyIsInFront = bfalse;
					}

					direction.normalize();

					bounce.setLevel(1);
					bounce.setDirection(direction);
					bounce.setDepth(m_actor->getDepth());
					bounce.setSender(m_actor->getRef());
					bounce.setAngle(m_actor->getAngle());
					bounce.setPos(m_actor->get2DPos());
					bounce.setPrevPos(m_actor->get2DPos());
					bounce.setFxPos(actor->getPos());
					bounce.setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
					bounce.setFaction(getFaction());
					actor->onEvent(&bounce);

					m_physComponent->addForce(-direction * getTemplate()->getCloseRangeAttackPushBackForce());

				}

            }
        }



        if (hitOne)
        {
            //removed for now
            //if (flip)
            //    m_animatedComponent->setFlipped(!m_actor->isFlipped());
            startCloseRangeAttack(flip);
        }
    }


    void Ray_GroundEnemyAIComponent::processHitSuccessful( EventHitSuccessful* _onHitSuccessful )
    {
    }

    void Ray_GroundEnemyAIComponent::processBlockedByPolyline( EventBlockedByPolyline * _onBlocked )
    {
        if (m_currentBehavior == m_receiveHitBehavior)
        {
            if (m_hitWallBehavior )
            {
                 PolyLine* poly;
                const PolyLineEdge* edge;

                AIUtils::getPolyLine(_onBlocked->getPolyline(),_onBlocked->getEdge(),poly,edge);

                if ( poly && edge )
                {
                    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                    if ( !getTemplate()->getInvincibleToDangerousMaterial() && mat && mat->getDangerousLevel() > 0 )
                    {
                        startDeath();
                        return;
                    }
                }

                if (m_hitWallBehavior->canHitWall(_onBlocked))
                {
                    m_hitWallBehavior->setData(_onBlocked);
                    setBehavior(m_hitWallBehavior);
                }
            }
        }
    }

    void Ray_GroundEnemyAIComponent::processSoftCollision( Ray_EventAddSoftCollForce * onAddForce )
    {
        if (m_currentBehavior == m_roamBehavior)
            m_physComponent->addForce(onAddForce->getForce());
    }

    void Ray_GroundEnemyAIComponent::setChildrenBindRuntimeDisabled(const bbool _disabled)
    {
        ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
        const ObjectRefList & bindChildren = bindHandler.getChildren();
        for(u32 index = 0; index < bindChildren.size(); ++index)
        {
            const BaseObject * const object = bindChildren[index].getObject();
            if( object )
            {
                const Actor * const actor = object->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                if( actor )
                {
                    ActorBind * const bind = actor->getParentBind();
                    if( bind )
                    {
                        bind->setRuntimeDisabled(_disabled);
                    }
                }
            }
        }
    }

    void Ray_GroundEnemyAIComponent::processBounceToLayer( Ray_EventBounceToLayer* _bounce )
    {
        if (_bounce->getHurt() > 0)
        {
            PunchStim stim;
            stim.setSender(_bounce->getSender());
            stim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            stim.setIsRadial(btrue);
            stim.setLevel(_bounce->getHurt());
            stim.setDirection(Vec2d::Up);
            stim.setFaction(RAY_FACTION_MEGAENEMY);
            stim.setFxPos(m_actor->getPos());
            processHit(&stim);
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_GroundEnemyAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_closeRangeDetectionShape)
        {
            DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_closeRangeDetectionShape,Color::pink(),0.0f,"CloseRangeDetectionShape");
        }

		/*
        if (m_dominoHitShape)
        {
            DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_dominoHitShape,Color::pink(),0.0f,"DominoHitShape");
        }*/
    }
#endif // ITF_SUPPORT_EDITOR

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_GroundEnemyAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_GroundEnemyAIComponent_Template)

        SERIALIZE_OBJECT_WITH_FACTORY("roamBehavior", m_roamBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("crushedBehavior", m_crushedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("spawnBehavior", m_spawnBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeRangeAttackBehavior", m_closeRangeAttackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("hitWallBehavior", m_hitWallBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("sleepBehavior", m_sleepBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeRangeDetectionShape", m_closeRangeDetectionShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_MEMBER("closeRangeAttackPushBackForce", m_closeRangeAttackPushBackForce);
        SERIALIZE_MEMBER("squashPenetrationRadius", m_squashPenetrationRadius);
        SERIALIZE_MEMBER("dieInWater", m_dieInWater);
        SERIALIZE_MEMBER("rayCastDist", m_rayCastDist);
        SERIALIZE_MEMBER("checkSquash", m_checkSquash);
        SERIALIZE_MEMBER("unbindChildrenOnHit", m_unbindChildrenOnHit);
        SERIALIZE_MEMBER("checkStickForCloseRangeAttack", m_checkStickForCloseRangeAttack);
        SERIALIZE_MEMBER("detectHunterOwnBullet", m_detectHunterOwnBullet);

    END_SERIALIZATION()

    Ray_GroundEnemyAIComponent_Template::Ray_GroundEnemyAIComponent_Template()

    : m_roamBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_deathBehavior(NULL)
    , m_crushedBehavior(NULL)
    , m_spawnBehavior(NULL)
    , m_closeRangeAttackBehavior(NULL)
    , m_hitWallBehavior(NULL)
    , m_sleepBehavior(NULL)
    , m_closeRangeDetectionShape(NULL)
    , m_closeRangeAttackPushBackForce(600.0f)
    , m_squashPenetrationRadius(0.6f)
    , m_dieInWater(btrue)
    , m_rayCastDist(5.0f)
    , m_checkSquash(btrue)
    , m_unbindChildrenOnHit(bfalse)
    , m_checkStickForCloseRangeAttack(bfalse)
    , m_detectHunterOwnBullet(bfalse)
    {
    }

    Ray_GroundEnemyAIComponent_Template::~Ray_GroundEnemyAIComponent_Template()
    {
        SF_DEL(m_roamBehavior);
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_deathBehavior);
        SF_DEL(m_crushedBehavior);
        SF_DEL(m_spawnBehavior);
        SF_DEL(m_closeRangeAttackBehavior);
        SF_DEL(m_hitWallBehavior);
        SF_DEL(m_sleepBehavior);
        SF_DEL(m_closeRangeDetectionShape);
    }
} // namespace ITF
