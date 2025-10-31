#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRunAwayBehavior.h"
#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AISleepBehavior.h"
#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

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

#ifndef _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterFloatingBehavior.h"
#endif //_ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIBubbleDeathBehavior.h"
#endif //_ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_AIHITWALLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIHitWallBehavior.h"
#endif //_ITF_RAY_AIHITWALLBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GroundAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_GroundAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_GroundAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_physComponent, "Ray_GroundAIComponent requires a StickToPolylinePhysComponent");
    END_VALIDATE_COMPONENT()


        Ray_GroundAIComponent::Ray_GroundAIComponent() 
        : Super()
        , m_roamBehavior(NULL)
        , m_runAwayBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_darktoonedBehavior(NULL)
        , m_undarktoonifyBehavior(NULL)
        , m_sleepBehavior(NULL)
        , m_spawnBehavior(NULL)
        , m_floatingBehavior(NULL)
        , m_closeRangeAttackBehavior(NULL)
        , m_hitWallBehavior(NULL)
        , m_physComponent(NULL)

        , m_timeCounter(0.f)
        , m_attackDelay(0.0f)
        , m_attackTimer(0.f)
        , m_hadEnemyInSight(bfalse)

        , m_darktoonified(bfalse)
        , m_crushed(bfalse)
        , m_swimmingWaterResistance(5.0f)
        , m_swimmingIsOnSurface(bfalse)
        , m_swimmingIsInside(bfalse)
        , m_swimmingSurfaceEdge(U32_INVALID)
        , m_swimmingClosestEdge(U32_INVALID)
        , m_swimingFloatForce(Vec2d::Zero)
        , m_swimingSurfaceT(0.f)
        , m_swimingClosestEdgeT(0.f)
        , m_swimingPushThreshold(0.90f)
        , m_swimingMoveTargetMultiplierBlendTime(0.5f)
        , m_moveTargetBlendTimer(0.f)
        , m_swimingWaterFrictionSet(bfalse)
        , m_swimingPrevAirFrictionMultiplier(0.f)
        , m_swimingWaterResistance(5.f)
        , m_swimingWaterResistMaxMult(10.f)
        , m_swimingInertia(Vec2d::Zero)
        , m_swimingPrevUnstickMinFrictionMultiplier(1.f)
        , m_swimingEnterDown(bfalse)
        , m_swimingTargetRotation(0.f)
        , m_swimingStandPose(0.f)
        , m_swimingTurnDistMult(50.f)
        , m_swimingCurrentAngularSpeed(0.f)
        , m_swimingTurnSpeedMult(10.f)
        , m_stance(Stance_Normal)
        , m_EnemyIsInFront(btrue)
        , m_dominoHitShape(NULL)
        , m_reHit(bfalse)
        , m_crushSize(bfalse)
    {
    }

    Ray_GroundAIComponent::~Ray_GroundAIComponent()
    {
        SF_DEL(m_dominoHitShape);
    }



    void Ray_GroundAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {


        m_roamBehavior = createAiBehavior(getTemplate()->getRoamBehavior());
        m_attackBehavior = static_cast<Ray_AIGroundAttackBehavior *>(createAiBehavior(getTemplate()->getAttackBehavior()));
        m_receiveHitBehavior = static_cast<Ray_AIReceiveHitBehavior *>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        m_deathBehavior =createAiBehavior(getTemplate()->getDeathBehavior());
        m_runAwayBehavior = static_cast<Ray_AIGroundRunAwayBehavior *>(createAiBehavior(getTemplate()->getRunAwayBehavior()));
        m_crushedBehavior = createAiBehavior(getTemplate()->getCrushedBehavior());
        m_darktoonedBehavior = createAiBehavior(getTemplate()->getDarktoonedBehavior());
        m_undarktoonifyBehavior = createAiBehavior(getTemplate()->getUndarktoonifyBehavior());
        m_sleepBehavior = static_cast<Ray_AISleepBehavior *>(createAiBehavior(getTemplate()->getSleepBehavior()));
        m_spawnBehavior = createAiBehavior(getTemplate()->getSpawnBehavior());
        m_floatingBehavior = createAiBehavior(getTemplate()->getFloatingBehavior());
        m_closeRangeAttackBehavior = createAiBehavior(getTemplate()->getCloseRangeAttackBehavior());
        m_hitWallBehavior = createAiBehaviorAndCast<Ray_AIHitWallBehavior>(getTemplate()->getHitWallBehavior());


        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryDarktoonificationPosition,785050175), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSpawn,3403067276), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693),this);

        if (m_crushedBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        }


        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated


        if (!getTemplate()->getDarktoonPath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_darktoonSpawner, getTemplate()->getDarktoonPath());
        }

        if (getTemplate()->getDominoHitShape())
        {
            if ( !m_dominoHitShape ||
                m_dominoHitShape->GetObjectClassCRC() != getTemplate()->getDominoHitShape()->GetObjectClassCRC() )
            {
                SF_DEL(m_dominoHitShape);
                m_dominoHitShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getDominoHitShape()->GetObjectClassCRC());
            }

            // make sure the existing scaled shape is not marked as flipped before copying the points
            if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                polygon->setFlipped(bfalse);
            }

            PhysShape::CopyShapeScaled(getTemplate()->getDominoHitShape(), m_dominoHitShape, m_actor->getScale());
        }
        onActivate();
    }

    void Ray_GroundAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
    }

    void Ray_GroundAIComponent::onActivate()
    {
        if ( m_phantomComponent )
        {
            m_phantomComponent->setDisabled(bfalse);
        }

        m_attackDelay = Seeder::getSharedSeeder().GetFloat(0, getTemplate()->getMaxAttackDelay());
        m_timeCounter = 0.f;
        m_attackTimer = 0.f;
        m_hadEnemyInSight = bfalse;

        setDarktoonified(getTemplate()->getDarktoonified_default(), ITF_INVALID_OBJREF);

        startSpawn();
        m_reHit = bfalse;

    }


    void Ray_GroundAIComponent::Update( f32 _dt )
    {
        //Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
        //GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + lookDir.to3d()); 

        f32 prevAttackTimer = m_attackTimer;

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

        if ( poly && edge )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

            if (  mat && ((mat->getDangerousLevel() > 0 && !getTemplate()->getInvincibleToDangerousMaterial()) || (mat->getIsWave()&&mat->getDangerousLevel() > 0)))
            {
                startDeath();
                return;
            }
        }

        updateBehaviors(_dt);

        if ( m_crushed )
        {
            processCrushed(&m_crushEvent);
            m_crushed = bfalse;
        }

        Super::Update(_dt);

        if ( prevAttackTimer == m_attackTimer )
        {
            m_attackTimer = 0.f;
        }
    }

    void Ray_GroundAIComponent::updateBehaviors( f32 _dt )
    {
        if ( !isCurrentBehavior(m_hitWallBehavior) && getHealth() <= 0 && m_currentBehavior != m_deathBehavior && m_currentBehavior != m_receiveHitBehavior)
        {
            startDeath();
            return;
        }

        if (getStance() != Stance_Swim && m_currentBehavior != m_deathBehavior)
        {
            trySwim();
        }

        PolyLine* deathPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Death,m_actor->get2DPos());
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());

        if (deathPoly && m_currentBehavior != m_deathBehavior)
        {
            startDeath();
            return;
        }

        if ( waterPoly )
        {
            startFloating(waterPoly->getRef());
            return;
        }

        if (m_currentBehavior != m_deathBehavior && isSquashAuthorized())
        {
            if (Ray_AIUtils::getSquashingPolyline(m_actor,m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdge(),m_physComponent->getRadius(),getTemplate()->getSquashPenetrationRadius() * m_physComponent->getRadius()))
            {
                startDeath();
                return;
            }
        }

        if (m_physComponent->getStickedPolyline() && ((m_currentBehavior == m_roamBehavior) || (m_currentBehavior == m_attackBehavior)))
        {
            u32 enemyCount = 0;
            Actor* closestEnemy = getClosestEnemy(getTemplate()->getCloseRangeAttackDetectionRange(), &enemyCount);
            if (closestEnemy)
            {
                m_lastAttacker = closestEnemy->getRef();
                Vec2d vecToVictim = closestEnemy->get2DPos() - m_actor->get2DPos();
                bbool targetIsLeft = vecToVictim.dot(Vec2d::Right) < 0;
                m_EnemyIsInFront = targetIsLeft == m_actor->isFlipped();
                startCloseRangeAttack(closestEnemy);
            }
        }
        if ( m_currentBehavior == m_roamBehavior )
        {
            updateRoam(_dt);
        }
        else if ( m_runAwayBehavior && m_currentBehavior == m_runAwayBehavior )
        {
            updateRunAway(_dt);
        }
        else if ( m_currentBehavior == m_attackBehavior )
        {
            updateAttack(_dt);
        }
        else if ( m_currentBehavior == m_receiveHitBehavior )
        {
            updateReceiveHit();
        }
        else if ( m_crushedBehavior && m_currentBehavior == m_crushedBehavior )
        {
            // nothing to do ATM...
        }
        else if ( m_darktoonedBehavior && m_currentBehavior == m_darktoonedBehavior )
        {
            // nothing to do ATM...
        }
        else if ( m_undarktoonifyBehavior && m_currentBehavior == m_undarktoonifyBehavior )
        {
            // nothing to do ATM...
        }
        else if ( m_sleepBehavior && m_currentBehavior == m_sleepBehavior )
        {
            updateSleep(_dt);
        }
        else if (m_floatingBehavior && m_currentBehavior == m_floatingBehavior)
        {
            updateFloating(_dt);
        }
        else if (m_closeRangeAttackBehavior && m_currentBehavior == m_closeRangeAttackBehavior)
        {

        }
    }

    void Ray_GroundAIComponent::updateRoam( f32 _dt )
    {
        if ( tryAttackOrRunAway(_dt, getTemplate()->getEnemyDetectionRange()) )
        {
            return;
        }

        if (getTemplate()->getRoamTime() > 0.0f && m_timeCounter > getTemplate()->getRoamTime())
        {
            startSleep();
        }
        m_timeCounter += _dt;
    }

    void Ray_GroundAIComponent::updateSleep( f32 _dt )
    {
        if (getTemplate()->getWakeUpOnEnemyDetection())
        {
            tryAttackOrRunAway(_dt, getTemplate()->getEnemyDetectionRange());
        }
        else if (getTemplate()->getWakeUpOnCloseEnemyDetection())
        {
            tryAttackOrRunAway(_dt, getTemplate()->getCloseRangeAttackDetectionRange());
        }

        if (getTemplate()->getSleepTime() > 0.0f && m_timeCounter > getTemplate()->getSleepTime())
        {
            startRoam();
        }
        m_timeCounter += _dt;
    }

    bbool Ray_GroundAIComponent::tryAttackOrRunAway( f32 _dt, const AABB& _range )
    {
        if (m_attackBehavior)
        {
            u32 enemyCount;
            Actor* closestEnemy = getClosestEnemy(_range, &enemyCount);

            if ( closestEnemy && queryAttack(_dt) )
            {
                if (m_darktoonified && m_darktoonedBehavior)
                {
                    setStance(Stance_Normal);
                    setBehavior(m_darktoonedBehavior);
                    return btrue;
                }

                if (m_runAwayBehavior && enemyCount > getTemplate()->getRunAwayEnemyLimit())
                {
                    startRunAway(closestEnemy->getRef());
                    return btrue;
                }
                else
                {
                    startAttack(closestEnemy->getRef());
                    return btrue;
                }
            }

        }
        return bfalse;
    }

    void Ray_GroundAIComponent::updateRunAway( f32 _dt )
    {
        // keep running if the enemy is still there
        if ( hasEnemyInRange(getTemplate()->getKeepRunningAwayRange()) )
        {
            m_timeCounter = 0.f;
        }

        m_timeCounter += _dt;

        if ( m_timeCounter >= getTemplate()->getRunAwayTime() )
        {
            // turn around and go back to idle
            m_actor->setFlipped(m_actor->isFlipped());
            startRoam();
        }
    }

    void Ray_GroundAIComponent::updateAttack( f32 _dt )
    {
        if ( m_attackBehavior->canBeInterrupted() )
        {
            Actor* victim = AIUtils::getActor(m_attackBehavior->getTarget());
            if ( victim )
            {
                bbool checkForChange = btrue;

                if (m_runAwayBehavior)
                {
                    // check if we're outnumbered and need to run away

                    u32 enemyCount;
                    Actor* closestEnemy = getClosestEnemy(getTemplate()->getEnemyDetectionRange(), &enemyCount);

                    if ( closestEnemy && enemyCount >= getTemplate()->getRunAwayEnemyLimit() )
                    {
                        startRunAway(closestEnemy->getRef());
                        checkForChange = bfalse;
                    }
                }

                //GFX_ADAPTER->drawDBGAABB(AIUtils::getAbsoluteAABB(m_enemyDetectionRange, m_actor->get2DPos(), m_actor->isFlipped()));
                if (checkForChange)
                {
                    // check if another enemy is a better candidate

                    Actor* closestEnemy = getClosestEnemy(getTemplate()->getEnemyChangeRange());
                    //GFX_ADAPTER->drawDBGAABB(AIUtils::getAbsoluteAABB(m_enemyChangeRange, m_actor->get2DPos(), m_actor->isFlipped()));

                    if ( closestEnemy ) //&& closestEnemy->getRef() != m_attackBehavior->getVictim() )
                    {
                        m_attackBehavior->setTarget(closestEnemy->getRef());
                    }
                }
            }
            else
            {
                // the victim actor disappeared: this shouldn't happen! (the behavior also checks this anyway)
                ITF_ASSERT_MSG(0, "invalid victim");
            }
        }
    }

    void Ray_GroundAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if ( m_currentBehavior == m_attackBehavior )
        {
            onFinishedAttack();
        }
        else if ( m_crushedBehavior  && m_currentBehavior == m_crushedBehavior )
        {
            onFinishedCrush();
        }
        else if ( m_currentBehavior == m_undarktoonifyBehavior )
        {
            onFinishedUndarktoonify();
        }
        else if ( m_currentBehavior == m_receiveHitBehavior )
        {
            onFinishedReceiveHit();
        }
        else if ( m_currentBehavior == m_deathBehavior )
        {
            onFinishedDeath();
        }
        else if ( m_currentBehavior == m_sleepBehavior )
        {
            onFinishedSleep();
        }
        else if (m_currentBehavior == m_spawnBehavior)
        {
            onFinishedSpawn();
        }
        else if (m_currentBehavior == m_floatingBehavior)
        {
            onFinishedFloat();
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
        }
    }

    void Ray_GroundAIComponent::onFinishedAttack()
    {
        startRoam();
        m_hadEnemyInSight = bfalse;
    }

    void Ray_GroundAIComponent::onFinishedReceiveHit()
    {
        if (getHealth() <= 0)
        {
            startDeath();
            return;
        }

        if (!m_darktoonified)
        {
            ObjectRef attackerRef = getLastAttacker();
            Actor* attackerActor = AIUtils::getActor(attackerRef);

            if ( attackerActor )
            {
                if ( m_attackBehavior && AIUtils::isTarget(getFaction(), AIUtils::getFaction(attackerActor)) )
                {
                    //disable spotting
                    m_hadEnemyInSight = btrue;
                    // revenge!
                    startAttack(attackerRef);
                }
                else
                {
                    // friendly fire
                    startRoam();
                }
            }
            else
            {
                // wtf?
                startRoam();

            }
        }
        else
        {
            // resume panicking like a zombie moron
            startRoam();
        }
    }

    void Ray_GroundAIComponent::updateDeath( f32 _dt )
    {
        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            return;
        }

        if (m_currentBehavior != m_deathBehavior)
        {
            startDeath();
        }
    }

    void Ray_GroundAIComponent::startRoam()
    {
        setStance(Stance_Normal);
        if (m_darktoonified && m_darktoonedBehavior)
        {
            setBehavior(m_darktoonedBehavior);
        }
        else
        {
            if (m_sleepBehavior && getTemplate()->getRoamTime() > 0.0f)
            {
                m_timeCounter = 0.0f;
            }

            if (m_roamBehavior)
            {
                setBehavior(m_roamBehavior);
            }
            else if (m_sleepBehavior)
            {
                startSleep();
            }
            else
            {
                // avoid recursion
                ITF_ASSERT_MSG(0, "startRoam but I don't have a roam or a sleep... what do I do??");
            }
        }
    }

    void Ray_GroundAIComponent::startRunAway( ObjectRef _attacker )
    {
        if (m_runAwayBehavior)
        {
            m_timeCounter = 0.f;
            m_runAwayBehavior->setData(_attacker);
            setBehavior(m_runAwayBehavior);
            m_hadEnemyInSight = bfalse;
            setStance(Stance_Scared);
        }
    }

    void Ray_GroundAIComponent::startAttack( ObjectRef _victim )
    {
        if (m_attackBehavior)
        {
            setStance(Stance_Fight);
            m_attackBehavior->setTarget(_victim);
            setBehavior(m_attackBehavior);
            m_hadEnemyInSight = btrue;
        }
        else
            startRoam();
    }

    void Ray_GroundAIComponent::startReceiveHit( HitStim* _hit )
    {
        setStance(Stance_Fight);
        if (_hit->getLevel() > 0 && getTemplate()->getCanRehit())
            m_reHit = btrue;
        else
            m_reHit = bfalse;

        if (m_currentBehavior == m_receiveHitBehavior)
            m_receiveHitBehavior->setData(_hit);
        else
        {
            setBehavior(m_receiveHitBehavior);
            m_receiveHitBehavior->setData(_hit);
            m_phantomComponent->setShape(ITF_GET_STRINGID_CRC(dominoPhantom, 2108039163));
        }
    }

    void Ray_GroundAIComponent::startDeath()
    {
        setBehavior(m_deathBehavior);
    }

    void Ray_GroundAIComponent::startCrushed()
    {
        setBehavior(m_crushedBehavior,btrue);
    }

    void Ray_GroundAIComponent::startSleep()
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

    void Ray_GroundAIComponent::startUndarktoonify()
    {
        if (m_undarktoonifyBehavior)
        {
            setBehavior(m_undarktoonifyBehavior);
        }
        else
        {
            onFinishedUndarktoonify();
        }
    }


    void Ray_GroundAIComponent::darktoonify( ObjectRef _darktoon )
    {
        setDarktoonified(btrue, _darktoon);
    }

    Actor* Ray_GroundAIComponent::undarktoonify( const Vec2d& _darktoonForce )
    {
        Actor* darktoon(NULL);
        if (m_darktoon.isValid())
        {
            darktoon = m_darktoon.getActor();
        }
        else
        {
            ITF_ASSERT_MSG(getTemplate()->getDarktoonified_default(), "we shouldn't have to spawn a darktoon unless starting darktoonified");

            darktoon = m_darktoonSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos());
            if (darktoon)
            {
                Ray_GroundAIComponent* ai = darktoon->GetComponent<Ray_GroundAIComponent>();
                ai->m_attackBehavior->setTarget(m_actor->getRef());
                ai->setBehavior(ai->m_attackBehavior);

                if ( _darktoonForce != Vec2d::Zero )
                {
                    EventAddForce force;
                    force.setForce(_darktoonForce);
                    darktoon->onEvent(&force);
                }

                ai->m_attackBehavior->HACK_setDarktoonifyAction();
            }
        }

        ITF_ASSERT_MSG(darktoon, "kicking off my darktoon but can't find the fucker!");
        if (darktoon)
        {
            Ray_EventUndarktoonify eventUnDT;
            eventUnDT.setSender(m_actor->getRef());
            darktoon->onEvent(&eventUnDT);
        }


        setDarktoonified(bfalse, ITF_INVALID_OBJREF);

        startUndarktoonify();

        return darktoon;
    }

    void Ray_GroundAIComponent::setDarktoonified( bbool _darktoonified, ObjectRef _darktoon )
    {
        if (_darktoonified)
        {
            m_darktoon.setValue(_darktoon.getValue());
            m_darktoonified = btrue;
            setFaction(RAY_FACTION_DARKTOON);
            setHealth(getTemplate()->getDarktoonifiedHealth());
        }
        else
        {
            m_darktoon.invalidate();
            m_darktoonified = bfalse;
            setFaction(getTemplate()->getFaction_default());
            setHealth(getTemplate()->getHealth_default());
        }
    }


    bbool Ray_GroundAIComponent::isBubbleDeathInProgress() const
    {
        return m_deathBehavior &&
            (m_currentBehavior == m_deathBehavior) &&
             m_deathBehavior->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_AIBubbleDeathBehavior,3031963609));
    }


    void Ray_GroundAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHit(hit);
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
            m_crushEvent = *crushed;
            m_crushed = btrue;
        }
        else if ( EventQueryBlocksHits* queryBlocksHit = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)) )
        {
            processQueryBlocksHits(queryBlocksHit);
        }
        else if ( Ray_EventQueryDarktoonificationPosition* queryDarktoonifyPosition = _event->DynamicCast<Ray_EventQueryDarktoonificationPosition>(ITF_GET_STRINGID_CRC(Ray_EventQueryDarktoonificationPosition,785050175)) )
        {
            processQueryDarktoonificationPosition(queryDarktoonifyPosition);
        }
        else if ( Ray_EventQueryCanDarktoonify* queryCanDarktoonify = _event->DynamicCast<Ray_EventQueryCanDarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444)) )
        {   
            processQueryCanDarktoonify(queryCanDarktoonify);
        }
        else if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventSpawn,3403067276)))
        {
            startSpawn();
        }
        else if (AnimGameplayEvent * onGameplayEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (getHealth() <= 0 && onGameplayEvent->getName() == MRK_Death )
            {
                if (m_currentBehavior == m_crushedBehavior && m_crushSize == btrue)
                    return;
                else   
                    startDeath();
            }
        }
        else if (EventHitSuccessful * onHitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)))
        {
            processHitSuccessful(onHitSuccessful);
        }
        else if (EventBlockedByPolyline * _onBlocked = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)))
        {
            processBlockedByPolyline(_onBlocked);
        }
        else if (Ray_EventAddSoftCollForce * onAddForce = _event->DynamicCast<Ray_EventAddSoftCollForce>(ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693)))
        {
            processSoftCollision(onAddForce);
        }

    }

    void Ray_GroundAIComponent::processHit( PunchStim* _hit )
    {
        if ( !m_receiveHitBehavior )
        {
            // this AI isn't hittable...
            return;
        }
        if (isCurrentBehavior(m_deathBehavior) || isCurrentBehavior(m_hitWallBehavior))
        {
            return;
        }


        if (m_currentBehavior == m_receiveHitBehavior)
        {
            if ( !m_receiveHitBehavior->canReceiveHit(_hit) )
            {
                return;
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



        // we accept the hit
        EReceivedHitType type = _hit->getReceivedHitType();

        // darktoonification hits can only affect one actor
        if ( type == RECEIVEDHITTYPE_DARKTOONIFY )
        {
            _hit->setStopProcessing(btrue);

            //cant darktoonify the same actor twice.
            if (m_darktoonified)
                return;
        }

        // notify the sender that he touched us
        Actor* sender = AIUtils::getActor(_hit->getSender());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }




        startReceiveHit(_hit);

        if ( !m_darktoonified && type == RECEIVEDHITTYPE_DARKTOONIFY )
        {
            darktoonify(_hit->getSender());
        }
        else if ( m_darktoonified )
        {
            undarktoonify(Vec2d::Zero);
        }

    }

    void Ray_GroundAIComponent::processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query )
    {
        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            query->setCanBeAttacked(bfalse);
            return;
        }

        query->setCanBeAttacked(m_currentBehavior != m_receiveHitBehavior);
    }

    void Ray_GroundAIComponent::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {
        if ( getHealth() <= 0 && !isBubbleDeathInProgress())
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            return;
        }

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if ( interactionQuery->getInteraction() == CharacterInteractionType_None && m_crushedBehavior && m_currentBehavior != m_closeRangeAttackBehavior)
            {
                interactionQuery->setInteraction(CharacterInteractionType_Crush);
                m_currentBehavior->onEvent(interactionQuery);
            }
        }
    }

    void Ray_GroundAIComponent::processCrushed( EventCrushed* crushed )
    {
        //ITF_ASSERT_MSG(m_crushedBehavior, "got crushed but we don't have a crushed behavior!");
        if ( m_darktoonified )
        {
            Vec2d forceDir;

            if ( m_physComponent->getStickedEdgeIndex() != U32_INVALID )
            {
                forceDir = m_physComponent->getCurrentGroundDir();
            }
            else
            {
                forceDir = Vec2d::Right;
            }

            if ( m_actor->isFlipped() )
            {
                forceDir *= -1.f;
            }

            Vec2d force = forceDir*getTemplate()->getDarktoonificationEjectionForceX();

            force += Vec2d::Up*getTemplate()->getDarktoonificationEjectionForceY();

            Actor* darktoon = undarktoonify(force);

            if ( darktoon )
            {
                darktoon->onEvent(crushed);
            }
        }
        else if (m_crushedBehavior)
        {
            m_lastAttacker = crushed->getSender();
            m_crushSize = 0;

            ActorRef ref = crushed->getSender();
            Actor * actor = ref.getActor();
            if (actor && GAMEMANAGER->isPlayerActor(ref))
            {
                Ray_EventQueryAIData query;
                actor->onEvent(&query);
                m_crushSize = query.getReducedSize();
            }

            if (!m_crushSize)
                receiveCrush(crushed);

            playLastHitFx();
            if (!isBubbleDeathInProgress())
            {
                startCrushed();
            }
        }
    }

    void Ray_GroundAIComponent::processQueryCanDarktoonify( Ray_EventQueryCanDarktoonify* _queryCanDarktoonify )
    {
        _queryCanDarktoonify->setCanDarktoonify(!m_darktoonified);
    }

    void Ray_GroundAIComponent::processQueryDarktoonificationPosition( Ray_EventQueryDarktoonificationPosition* _queryDarktoonifyPosition )
    {
        if ( getTemplate()->getDarktoonificationBone().isValid() )
        {
            u32 boneIndex = m_animComponent->getBoneIndex(getTemplate()->getDarktoonificationBone());
            if (boneIndex != U32_INVALID)
            {
                Vec3d bonePos;
                if ( m_animComponent->getBonePos(boneIndex, bonePos) )
                {
                    _queryDarktoonifyPosition->setPos(bonePos);
                    return;
                }
                else
                {
                    ITF_ASSERT_MSG(0, "can't get darktoonification bone pos, is bone '%s' valid?", getTemplate()->getDarktoonificationBone().getDebugString());
                }
            }
        }

        // no bone, fallback
        Vec3d pos = m_actor->getPos();
        _queryDarktoonifyPosition->setPos(pos);
    }

    void Ray_GroundAIComponent::processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit )
    {
        _queryBlocksHit->setCanBlockHits( bfalse );
    }




    bbool Ray_GroundAIComponent::queryAttack( f32 _dt )
    {
        bbool canAttack = bfalse;

        if ( m_attackTimer == 0 )
        {
            m_attackTimer = m_attackDelay;
        }
        else
        {
            m_attackTimer = Max(m_attackTimer - _dt, 0.f);

            if ( m_attackTimer == 0 )
            {
                canAttack = btrue;
            }
        }

        return canAttack;
    }


    Actor* Ray_GroundAIComponent::getClosestEnemy( const AABB& _range, u32* _enemyCount /*= NULL*/ ) const
    {
        return AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), _range, _enemyCount);
    }

    Actor* Ray_GroundAIComponent::getClosestEnemy( f32 _range, bbool _detectBehind /*= btrue*/, u32* _enemyCount /*= NULL*/ ) const
    {
        return AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), _range, btrue, _enemyCount);
    }

    bbool Ray_GroundAIComponent::hasEnemyInRange( f32 _range, bbool _detectBehind /*= btrue*/ ) const
    {
        return AIUtils::hasEnemyInRange(m_actor, m_actor->isFlipped(), _range);
    }

    void Ray_GroundAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        const Vec2d speed = m_physComponent->getSpeed();
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed,4218325830), speed.norm());

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY,461177981), speed.dot(-getCurrentGravityDir()));

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Stance,2021520747), (u32)(m_stance));

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Darktoonified,2716098321), static_cast<u32>(m_darktoonified));

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EnemyIsInFront,2121294079), static_cast<u32>(m_EnemyIsInFront));

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(CrushSize,2245983489), static_cast<u32>(m_crushSize));


        PhysActorForceArray windForces;
        Vec2d windForce = Vec2d::Zero;

        PHYSWORLD->getWindForces(m_actor->get2DPos(), Vec2d::Zero, m_actor->getDepth(), windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            windForce += windForces[i].m_force;
        }

        f32 force = windForce.dot(Vec2d::Right.Rotate(m_actor->isFlipped()?MTH_PI-m_actor->getAngle():m_actor->getAngle()));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WindForce,2257032332),force);

    }

    void Ray_GroundAIComponent::onFinishedCrush()
    {
        if ( m_darktoonified )
        {
            startRoam();
            return;
        }
        Actor* attackerActor = AIUtils::getActor(m_lastAttacker);

        if ( attackerActor )
        {
            if ( m_attackBehavior && AIUtils::isTarget(getFaction(), AIUtils::getFaction(attackerActor)) )
            {
                // revenge!
                startAttack(m_lastAttacker);
            }
            else
            {
                // friendly fire
                startRoam();
            }
        }
    }

    void Ray_GroundAIComponent::onFinishedUndarktoonify()
    {
        startRoam();
    }

    void Ray_GroundAIComponent::onFinishedDeath()
    {
        // nothing to do...
    }

    void Ray_GroundAIComponent::onFinishedSleep()
    {
        startRoam();
    }


    void Ray_GroundAIComponent::startSpawn()
    {
        if (m_spawnBehavior /*&& !m_actor->isEnabled()*/)
        {
            m_actor->enable();
            setBehavior(m_spawnBehavior);
        }
        else
        {
            startSleep();
        }
    }

    void Ray_GroundAIComponent::onFinishedSpawn()
    {
        if (m_sleepBehavior)
        {
            startSleep();
        }
        else
        {
            startRoam();
        }
    }

    void Ray_GroundAIComponent::onFinishedFloat()
    {
        startRoam();
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::updateFloating
    // @protected 
    // @param	f32 _dt
    // @return	void
    //************************************
    void Ray_GroundAIComponent::updateFloating( f32 _dt )
    {

    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::startFloating
    // @protected 
    // @param	ObjectRef _ref
    // @return	void
    //************************************
    void Ray_GroundAIComponent::startFloating( ObjectRef _ref )
    {
        if ( m_floatingBehavior )
        {
            //  m_floatingBehavior->setSwimingPolyline(_ref);
            setBehavior(m_floatingBehavior);
        }
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::setStance
    // @public 
    // @param	Stance _val
    // @return	void
    //************************************
    void Ray_GroundAIComponent::setStance(Stance _val)
    { 

        if (m_stance != _val)
        {

            if (m_stance == Stance_Swim)
            {
                if ( m_swimingWaterFrictionSet  )
                {
                    m_physComponent->setAirFrictionMultiplier(m_swimingPrevAirFrictionMultiplier);
                    m_swimingWaterFrictionSet = bfalse;
                }

                m_physComponent->setUpdateRotation(btrue);
                m_physComponent->setUnstickMinFrictionMultiplier(m_swimingPrevUnstickMinFrictionMultiplier);
                m_physComponent->setApplyWindSpeedLimiter(btrue);
                m_physComponent->setGravityMultiplier(1.0f);
                m_stance = _val; 
                setSwimingPolyline(NULL);
                //swimResetDash();
                //startDiveOutFx();
            }
            else if (_val == Stance_Swim)
            {
                //m_swimingTime = 0.f;
                m_physComponent->setUpdateRotation(bfalse);
                m_swimingPrevUnstickMinFrictionMultiplier = m_physComponent->getUnstickMinFrictionMultiplier();
                m_physComponent->setUnstickMinFrictionMultiplier(1000.f);
                m_physComponent->setApplyWindSpeedLimiter(bfalse);

                f32 angle = m_swimDirection.getAngle();
                NormalizeAngle2PI(angle);
                EMoveDir dir = getMoveDirectionFromAngle(angle);
                m_swimingEnterDown = (dir == MOVEDIR_DOWN);
                m_physComponent->setGravityMultiplier(0.0f);
                m_swimingInertia = m_physComponent->getSpeed();

                if ( m_swimingEnterDown )
                {
                    m_swimingInertia *= getTemplate()->getSwimingInertiaEnterDownMultiplier();
                }
                else
                {
                    m_swimingInertia *= getTemplate()->getSwimingInertiaEnterNormalMultiplier();
                }

                if ( m_swimingInertia.sqrnorm() > (getTemplate()->getSwimingInertiaEnterMaxSpeed()*getTemplate()->getSwimingInertiaEnterMaxSpeed()) )
                {
                    m_swimingInertia.normalize();
                    m_swimingInertia *= getTemplate()->getSwimingInertiaEnterMaxSpeed();
                }
                //startDiveFx();
            }

        }


        m_stance = _val; 
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::trySwim
    // @protected 
    // @return	void
    //************************************
    void Ray_GroundAIComponent::trySwim()
    {
        if ( m_currentGravityDir.dot(m_physComponent->getSpeed()) < 0.f )
        {
            // Avoid getting in water right after a jump
            return;
        }
        AIUtils::PolylineRegionList currentRegions;
        AIUtils::getPolylineRegions(m_actor->getDepth(),m_actor->get2DPos(),currentRegions);

        u32 numRegions = currentRegions.size();

        for ( u32 i = 0; i < numRegions; i++ )
        {
            AIUtils::PolylineRegionData& data = currentRegions[i];

            if ( data.m_regionId == s_RegionName_Swim && data.m_polyline )
            {
                setSwimingPolyline(data.m_polyline);
                break;
            }
        }
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::setSwimingPolyline
    // @protected 
    // @param	const PolyLine * _polyLine
    // @return	void
    //************************************
    void Ray_GroundAIComponent::setSwimingPolyline( const PolyLine* _polyLine )
    {
        // Process swiming event
        PolyLine* prevPolyLine = AIUtils::getPolyLine(m_swimmingPolyline);

        if (prevPolyLine && prevPolyLine->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), bfalse, m_physComponent->getSpeed() + m_physComponent->getForce()*LOGICDT, 0.f, m_physComponent->getWaterMultiplier(), m_physComponent->getWeight(),bfalse);
            prevPolyLine->m_ownerFrise->onEvent(&swiming);
        }

        if ( _polyLine )
        {
            m_swimmingPolyline = _polyLine->getRef();

            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_polyLine->getEdgeAt(0).getGameMaterial()));

            if ( mat )
            {
                m_swimmingWaterResistance = mat->getWaterResistance();
            }

            if ( getStance() != Stance_Swim  && m_floatingBehavior)
            {
                setBehavior(m_floatingBehavior,btrue);
            }

            if (_polyLine->m_ownerFrise)
            {
                EventSwiming swiming(m_actor->getRef(), btrue, m_physComponent->getSpeed() + m_physComponent->getForce()*LOGICDT, m_actor->getRadius(), m_physComponent->getWaterMultiplier(), m_physComponent->getWeight(),bfalse);
                _polyLine->m_ownerFrise->onEvent(&swiming);
            }
        }
        else
        {
            m_swimmingPolyline.invalidate();
            m_swimmingClosestEdge = U32_INVALID;
            m_swimmingSurfaceEdge = U32_INVALID;
            m_swimmingIsInside = bfalse;
            m_swimmingIsOnSurface = bfalse;
            //            m_swimmingIsOnJumpRange = bfalse;
            setStance(Stance_Normal);
        }
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::UpdatePhysicSwim
    // @public 
    // @param	f32 _dt
    // @param	bbool _static
    // @param	Vec2d _direction
    // @return	ITF::bbool
    //************************************
    bbool Ray_GroundAIComponent::UpdatePhysicSwim( f32 _dt, bbool _static, Vec2d _direction)
    {
        m_currentTargetForce = Vec2d::Zero;

        BaseObject* obj = GETOBJECT(m_swimmingPolyline);

        if ( !obj )
        {
            //m_parent->changeState(&m_parent->m_stateFalling);
            return bfalse;
        }

        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( !poly )
        {
            //m_parent->changeState(&m_parent->m_stateFalling);
            return bfalse;
        }
        m_moveTargetBlendTimer = Min(m_moveTargetBlendTimer+_dt,m_swimingMoveTargetMultiplierBlendTime);

        swimingUpdatePolylines(poly,_dt);

        poly = NULL;
        const PolyLineEdge* surfaceEdge = NULL;
        const PolyLineEdge* closestEdge = NULL;

        AIUtils::getPolyLine(m_swimmingPolyline,m_swimmingSurfaceEdge,poly,surfaceEdge);
        AIUtils::getPolyLine(m_swimmingPolyline,m_swimmingClosestEdge,poly,closestEdge);

        if ( !poly || !surfaceEdge || !closestEdge )
        {
            return bfalse;
        }

        m_swimDirection = _direction;

        m_swimingFloatForce = Vec2d::Zero;
        //m_swimmingDashForce = Vec2d::Zero;

        //if ( isPerformingSwimDash() )
        //{
        //    m_swimingDashHold &= ( _buttons[m_joyButton_A]==InputAdapter::JustPressed || _buttons[m_joyButton_A]==InputAdapter::Pressed );
        //}

        //setPathBlocked(bfalse);

        Vec2d startPos = m_actor->get2DPos();

        // stay inside the water region
        if ( !m_swimmingIsInside )
        {
            Vec2d closestPoint = closestEdge->getPos() + (closestEdge->m_vector * m_swimingClosestEdgeT);
            m_actor->updateComponentPosAngle(closestPoint.to3d(m_actor->getPos().m_z),m_actor->getAngle(),this);
        }

        //bbool mustFloat = !m_powerUpManager.hasPowerUp(Ray_PowerUp_Dive);

        //if ( mustFloat )
        //{
        //    swimUpdateFloat(_dt,*surfaceEdge,_static);
        //}
        //else
        //{
        swimUpdateDive(_dt,*surfaceEdge,_static);
        //}

        //if ( isPerformingSwimDash() )
        //{
        //    if ( !m_swimingDashForward )
        //    {
        //        resetSwimTargetRotation();
        //    }

        //    swimSetDashForce();
        //}

        //swimApplyForces(_dt,mustFloat);
        swimApplyForces(_dt,bfalse);

        UpdatePhysicSwimRotation(_dt);

        //m_swimingTime += _dt;

        if ( m_swimDirection != Vec2d::Zero )
        {
            m_swimingStandPose = 0.f;
        }
        else
        {
            if ( m_swimingStandPose <= getTemplate()->getSwimingTurnUpsideDownWait())
            {
                m_swimingStandPose += _dt;
            }
        }

        //m_swimingDashCounter = Max(m_swimingDashCounter-_dt,0.f);
        //m_swimingDashCoolCounter = Max(m_swimingDashCoolCounter-_dt,0.f);

        return btrue;
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimingUpdatePolylines
    // @protected 
    // @param	const PolyLine * _polyLine
    // @param	f32 _dt
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimingUpdatePolylines( const PolyLine* _polyLine, f32 _dt )
    {
        swimingUpdateAirFriction();

        AIUtils::getSwimingPolylineData(_polyLine,m_actor->getPos(),m_currentGravityDir,getTemplate()->getSwimingSurfaceRange(),
            m_swimmingSurfaceEdge,m_swimingSurfaceT,m_swimmingIsOnSurface,
            m_swimmingClosestEdge,m_swimingClosestEdgeT,m_swimmingIsInside);
    }
    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimUpdateDive
    // @protected 
    // @param	f32 _dt
    // @param	const PolyLineEdge & _surfaceEdge
    // @param	bbool _static
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimUpdateDive( f32 _dt, const PolyLineEdge& _surfaceEdge, bbool _static )
    {
        // Swim in direction
        if ( _static )
        {
            return;
        }


        //if ( m_swimmingIsOnSurface )
        //{
        //    swimSetFloatForce(_surfaceEdge);

        //    EMoveDir moveDirection = getMoveDataAnalog().getMoveDirection();

        //    if ( moveDirection == MOVEDIR_LEFT || moveDirection == MOVEDIR_RIGHT )
        //    {
        //        // While in the surface we move in the direction of the polyline to follow its shape
        //        moveDir = _surfaceEdge.m_normalizedVector;
        //        moveAxis = moveDir;
        //        moveAxis *= getMoveDataAnalog().getMoveReal().norm();

        //        if ( moveDirection == MOVEDIR_LEFT )
        //        {
        //            moveDir *= -1.f;
        //        }
        //    }
        //    else if ( moveDirection == MOVEDIR_DOWN )
        //    {
        //        // If not on the surface or going down it's free diving
        //        moveAxis = getMoveDataAnalog().getMoveReal();
        //        moveDir = getMoveDataAnalog().getMoveRealNormalized();
        //    }
        //}
        //else
        //{
        //    // If not on the surface or going down it's free diving
        //    moveAxis = getMoveDataAnalog().getMoveReal();
        //    moveDir = getMoveDataAnalog().getMoveRealNormalized();
        //}

        if ( !m_swimmingIsOnSurface && m_swimDirection != Vec2d::Zero)
        {
            swimProcessPush(m_swimDirection);
        }

        swimSetSwimForce(_dt,m_swimDirection,bfalse);

        if ( m_swimDirection != Vec2d::Zero )
        {
            f32 targetRotation = getVec2Angle(m_swimDirection);
            setSwimTargetRotation(targetRotation);
        }
        else
        {
            //if ( trySwimSprintJump() )
            //{
            //    // we are about to perform a sprint jump, so let it be
            //    moveAxis = getMoveDataAnalog().getMoveReal();
            //    moveDir = getMoveDataAnalog().getMoveRealNormalized();
            //}
            //else
            {
                resetSwimTargetRotation();
                //m_moveInput = Vec2d::Zero;
            }
        }
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimProcessPush
    // @protected 
    // @param	Vec2d & _moveDir
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimProcessPush( Vec2d& _moveDir )
    {
        // First of all lets see if we are trying to move towards a contact. If so then correct the movement
        const PhysContactsContainer& contacts = m_physComponent->getContacts();

        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            const SCollidableContact& c = contacts[i];

            f32 dotProd = _moveDir.dot(c.m_normal);

            if ( dotProd >= m_swimingPushThreshold )
            {
                // Pushing
                //setPathBlocked(btrue);
                _moveDir = Vec2d::Zero;
                break;
            }

            if ( dotProd <= 0.f )
            {
                continue;
            }

            f32 crossProd = _moveDir.cross(c.m_normal);

            if ( crossProd < 0.f )
            {
                _moveDir = c.m_normal.getPerpendicular();
            }
            else
            {
                _moveDir = c.m_normal.getPerpendicular()*-1.f;
            }

            break;
        }
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimSetSwimForce
    // @protected 
    // @param	f32 _dt
    // @param	const Vec2d & _moveDir
    // @param	bbool _float
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimSetSwimForce( f32 _dt, const Vec2d& _moveDir, bbool _float )
    {
        Vec2d targetMove;
        f32 topSpeed = getTemplate()->getSwimingTopSpeed();

        //if ( isSprinting() )
        //{
        //    topSpeed *= m_swimingSprintMaxSpeedMultiplier;
        //}

        if ( _moveDir != Vec2d::Zero )
        {
            // Set the swiming force
            targetMove = _moveDir*topSpeed;

            //if ( m_swimingTime < m_swimingRegainControl )
            //{
            //    f32 controlT = m_swimingTime / m_swimingRegainControl;

            //    targetMove *= controlT;
            //}

            f32 blendTime = m_swimingMoveTargetMultiplierBlendTime;

            if ( m_moveTargetBlendTimer < blendTime )
            {
                targetMove *= m_moveTargetBlendTimer / blendTime;
            }
        }
        else
        {
            targetMove = Vec2d::Zero;
        }

        Vec2d dif = targetMove - m_swimingInertia;
        f32 difSq = dif.sqrnorm();
        f32 inertiaSpeed =  getTemplate()->getSwimingInertiaSpeed() * _dt;

        if ( difSq > (inertiaSpeed*inertiaSpeed) )
        {
            dif.normalize();
            dif *= inertiaSpeed;
        }

        m_swimingInertia += dif;

        dif = m_swimingInertia - m_physComponent->getSpeed();
        difSq = dif.sqrnorm();

        if ( difSq > (topSpeed*topSpeed) )
        {
            dif.normalize();
            dif *= topSpeed;
        }

        m_currentTargetForce += dif / LOGICDT;
    }
    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimingUpdateAirFriction
    // @protected 
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimingUpdateAirFriction()
    {
        if ( !m_swimingWaterFrictionSet )
        {
            m_swimingPrevAirFrictionMultiplier = m_physComponent->getAirFrictionMultiplier();
        }

        f32 waterResist = m_swimingWaterResistance;
        f32 speedSq = m_physComponent->getSpeed().sqrnorm();

        if ( speedSq > (getTemplate()->getswimingWaterResistMinSpeed()*getTemplate()->getswimingWaterResistMinSpeed()) )
        {
            f32 speed = f32_Sqrt(speedSq);

            speed = Clamp(speed,getTemplate()->getswimingWaterResistMinSpeed(),getTemplate()->getSwimingWaterResistMaxSpeed());

            f32 t = ( speed - getTemplate()->getswimingWaterResistMinSpeed() ) / ( getTemplate()->getSwimingWaterResistMaxSpeed() - getTemplate()->getswimingWaterResistMinSpeed() );
            f32 multiplier = Interpolate(1.f,m_swimingWaterResistMaxMult,t);

            waterResist *= multiplier;
        }

        m_physComponent->setAirFrictionMultiplier(waterResist);

        m_swimingWaterFrictionSet = btrue;
    }
    //************************************
    // @name	ITF::Ray_GroundAIComponent::getMoveDirectionFromAngle
    // @protected 
    // @param	f32 _angle
    // @return	Ray_GroundAIComponent::EMoveDir
    //************************************
    Ray_GroundAIComponent::EMoveDir Ray_GroundAIComponent::getMoveDirectionFromAngle( f32 _angle ) const
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
    //************************************
    // @name	ITF::Ray_GroundAIComponent::swimApplyForces
    // @protected 
    // @param	f32 _dt
    // @param	bbool _mustFloat
    // @return	void
    //************************************
    void Ray_GroundAIComponent::swimApplyForces( f32 _dt, bbool _mustFloat )
    {
        bbool floating = m_swimingFloatForce != Vec2d::Zero;
        //bbool dashing = m_swimingDashForce != Vec2d::Zero || m_swimingDashCounter;
        Vec2d swimForce = m_currentTargetForce;

        if ( floating && !m_swimmingIsOnSurface )
        {
            Vec2d rightDir = m_currentGravityDir.getPerpendicular();

            swimForce = rightDir * rightDir.dot(swimForce);
        }
        //else if ( dashing )
        //{
        //    f32 moveComponent;
        //    f32 controlTime = m_swimingDashTotalLength * m_swimingDashGainControl;

        //    if ( m_swimingDashCounter >= controlTime )
        //    {
        //        moveComponent = 0.f;
        //    }
        //    else
        //    {
        //        moveComponent = 1.f - ( m_swimingDashCounter / controlTime );
        //    }

        //    Vec2d xDir = m_swimingDashDirection.getPerpendicular();

        //    moveComponent *= m_swimingDashDirection.dot(swimForce);

        //    swimForce = xDir * xDir.dot(swimForce);
        //    swimForce += ( m_swimingDashDirection * moveComponent );
        //}

        m_physComponent->addForce(swimForce);

        if ( floating )
        {
            m_physComponent->addForce(m_swimingFloatForce);
        }

        //if ( dashing )
        //{
        //    m_characterPhys->addForce(m_swimingDashForce);
        //}
    }
    //************************************
    // @name	ITF::Ray_GroundAIComponent::setSwimTargetRotation
    // @protected 
    // @param	f32 _target
    // @return	void
    //************************************
    void Ray_GroundAIComponent::setSwimTargetRotation( f32 _target )
    {
        m_swimingTargetRotation = _target;
        NormalizeAnglePI(m_swimingTargetRotation);
    }
    //************************************
    // @name	ITF::Ray_GroundAIComponent::resetSwimTargetRotation
    // @protected 
    // @return	void
    //************************************
    void Ray_GroundAIComponent::resetSwimTargetRotation()
    {
        f32 targetRotation = getVec2Angle(m_currentGravityDir.getPerpendicular());
        f32 anglePi = m_actor->getAngle();
        bbool lookingRight = !m_actor->isFlipped();//getIsLookingRight();

        //if ( isPerformingUTurn() && !m_uTurnFlipped )
        //{
        //    // If we are doing a uturn we assume always we are already at the target rotation
        //    lookingRight = !lookingRight;
        //}

        if ( !lookingRight )
        {
            anglePi += MTH_PI;
        }

        NormalizeAnglePI(anglePi);

        anglePi = f32_Abs(anglePi);

        bbool upsideDown = ( anglePi <= (MTH_PIBY2*0.5f) && !lookingRight ) ||
            ( anglePi >= (MTH_PI*0.75f) && lookingRight );

        if ( upsideDown )
        {
            if ( m_swimingStandPose >= getTemplate()->getSwimingTurnUpsideDownWait() )
            {
                if ( !lookingRight )
                {
                    targetRotation += MTH_PI;
                }
            }
            else
            {
                if ( lookingRight )
                {
                    targetRotation += MTH_PI;
                }
            }
        }
        else
        {
            if ( !lookingRight )
            {
                targetRotation += MTH_PI;
            }
        }

        m_swimingTargetRotation = targetRotation;
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::UpdatePhysicSwimRotation
    // @protected 
    // @param	f32 _dt
    // @return	void
    //************************************
    void Ray_GroundAIComponent::UpdatePhysicSwimRotation( f32 _dt )
    {
        bbool lookRight = !m_actor->isFlipped();//getIsLookingRight();
        //        bbool performingUTurn = isPerformingUTurn();

        // First we transform the angle to "world" space, removing the flipping
        f32 currentAngle = m_actor->getAngle();
        bbool piAdded = bfalse;

        //if ( isPerformingUTurn() )
        //{
        //    // While doing the uturn we assume the character is already in his final rotation
        //    if ( !m_uTurnFlipped )
        //    {
        //        lookRight = !lookRight;
        //    }
        //}

        if ( !lookRight )
        {
            currentAngle += MTH_PI;
            piAdded = btrue;
        }

        // Then we calculate the current angular speed
        f32 angleDif = getShortestAngleDelta(currentAngle,m_swimingTargetRotation);
        f32 curveDif = angleDif;

        // Check if we need to do a Turn or a Roll
        //if ( !performingUTurn )
        //{
        //    if ( f32_Abs(angleDif) >= (MTH_PI*0.75f) )
        //    {
        //        startUTurn( m_swimingStandPose>0.25f ? UTURNTYPE_SWIM_STAND : UTURNTYPE_SWIM_MOVE);

        //        curveDif = 0.f;
        //        m_swimingCurveSpeed = 0.f;
        //        m_swimingCurveCursor = 0.5f;
        //        m_swimingCurveTarget = 0.5f;
        //    }
        //    else
        //    {
        //        if ( m_moveInput == Vec2d::Zero )
        //        {
        //            curveDif = 0.f;
        //        }
        //    }
        //}
        //else
        //{
        //    curveDif = 0.f;
        //}

        UpdatePhysicSwimingCurve(_dt,curveDif);

        f32 force = (angleDif*m_swimingTurnDistMult) - (m_swimingCurrentAngularSpeed*m_swimingTurnSpeedMult);

        m_swimingCurrentAngularSpeed += force*_dt;

        currentAngle += m_swimingCurrentAngularSpeed * _dt;

        NormalizeAnglePI(currentAngle);

        if ( piAdded )
        {
            currentAngle -= MTH_PI;
        }

        m_actor->updateComponentPosAngle(m_actor->getPos(),currentAngle,this);
    }

    //************************************
    // @name	ITF::Ray_GroundAIComponent::UpdatePhysicSwimingCurve
    // @protected 
    // @param	f32 _dt
    // @param	f32 targetDif
    // @return	void
    //************************************
    void Ray_GroundAIComponent::UpdatePhysicSwimingCurve( f32 _dt, f32 targetDif )
    {
        //m_swimingCurveTarget = targetDif / m_swimingCurveMaxDif;
        //m_swimingCurveTarget = Clamp(m_swimingCurveTarget,-m_swimingCurveMaxOffset,m_swimingCurveMaxOffset);
        //m_swimingCurveTarget = 0.5f + m_swimingCurveTarget;

        //bbool invert = ( !getIsLookingRight() && !isPerformingUTurn() ) || ( getIsLookingRight() && isPerformingUTurn() );

        //if ( invert )
        //{
        //    m_swimingCurveTarget = 1.f - m_swimingCurveTarget;
        //}

        //f32 force = ( ( m_swimingCurveTarget - m_swimingCurveCursor ) * m_swimingCurveForceS ) -
        //    ( m_swimingCurveSpeed * m_swimingCurveForceD );

        //m_swimingCurveSpeed += force * _dt;
        //m_swimingCurveCursor += m_swimingCurveSpeed * _dt;
        //m_swimingCurveCursor = Clamp(m_swimingCurveCursor,0.f,1.f);
    }

    void Ray_GroundAIComponent::startCloseRangeAttack( Actor * _target)
    {
        if (m_closeRangeAttackBehavior && _target)
        {
            setBehavior(m_closeRangeAttackBehavior);
            EventSetDirection direction;
            EventQueryPhysicsData query;
            _target->onEvent(&query);
            Vec2d dir = query.getSpeed();
            dir.normalize();
            const f32 dot = dir.dot(Vec2d::Right);
            direction.setDirection(dot > 0.0f ? -Vec2d::Right:Vec2d::Right);
            m_closeRangeAttackBehavior->onEvent(&direction);
        }
    }

    void Ray_GroundAIComponent::onFinishedCloseRangeAttack()
    {
        //disable spotting
        m_hadEnemyInSight = btrue;
        startAttack(m_lastAttacker);
    }

    PhysShape * Ray_GroundAIComponent::getDominoHitShape() const
    {
        if (m_dominoHitShape )
            return m_dominoHitShape;
        else
            return m_phantomComponent->getShape();
    }

    void Ray_GroundAIComponent::updateReceiveHit()
    {
        if (m_dominoHitShape)
        {
            if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                polygon->setFlipped(m_actor->isFlipped());
            }
        }
        if (m_reHit)
        {
            updateRehit();
        }
    }

    void Ray_GroundAIComponent::updateRehit()
    {
        PhysShape* shape = m_dominoHitShape;
        if (!shape)
        {
            shape = m_phantomComponent->getShape();
        }
        else
        {
            // flip shape if necessary
            if ( PhysShapePolygon* polygon = m_dominoHitShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                polygon->setFlipped(m_actor->isFlipped());
            }
        }
        if (shape)
        {
            RehitStim* stim = STIMSMANAGER->requestStim<RehitStim>(shape);

#if defined(ITF_FINAL)
            if(stim)
#endif // ITF_FINAL
            {
                stim->setRewardReceiver(getRewardReceiver());
                stim->setDepth(m_actor->getDepth());
                stim->setSender(m_actor->getRef());
                stim->setLevel(getLastHitLevel()/* == 0 ? 0: getLastHitLevel() -1*/);
                stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
                stim->setAngle(m_actor->getAngle());
                stim->setFaction(getFaction());

                const Vec2d& shapePos = m_actor->get2DPos();
                stim->setPos(shapePos);
                stim->setPrevPos(shapePos);
                Vec2d speed = m_physComponent->getSpeed();
                speed.normalize();
                stim->setDirection(speed);
                stim->setFxPos(m_actor->getPos());

                STIMSMANAGER->sendStim(stim);
            }
        }
    }

    void Ray_GroundAIComponent::processHitSuccessful(EventHitSuccessful* _onHitSuccessful )
    {
        ObjectRef other = _onHitSuccessful->getSender();

        if ( other != m_actor->getRef() )
        {
            m_physComponent->setSpeed(m_physComponent->getSpeed() * getTemplate()->getDominoHitSpeedMultiplier());
            m_reHit = bfalse;
        }
    }


    void Ray_GroundAIComponent::processBlockedByPolyline( EventBlockedByPolyline * _onBlocked )
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

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            if (m_hitWallBehavior )
            {
                if (m_hitWallBehavior->canHitWall(_onBlocked))
                {
                    m_hitWallBehavior->setData(_onBlocked);
                    setBehavior(m_hitWallBehavior);
                }
            }
            else
            {
                startDeath();
            }
        }
    }

    void Ray_GroundAIComponent::processSoftCollision( Ray_EventAddSoftCollForce * onAddForce )
    {
        if (m_currentBehavior == m_roamBehavior
            || m_currentBehavior == m_attackBehavior
            || m_currentBehavior == m_runAwayBehavior)
            m_physComponent->addForce(onAddForce->getForce());
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_GroundAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        bbool flip = m_animComponent && m_actor->isFlipped(); 
        AABB aabb(getTemplate()->getEnemyDetectionRange());
        if (flip)
            aabb.FlipHorizontaly(bfalse);
        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::white(),0.f,1.f,"EnemyDetectionRange");
        aabb = getTemplate()->getEnemyChangeRange();
        if (flip)
            aabb.FlipHorizontaly(bfalse);
        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::pink(),0.f,1.f,"EnemyChangeRange");
        aabb = getTemplate()->getCloseRangeAttackDetectionRange();
        if (flip)
            aabb.FlipHorizontaly(bfalse);
        DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::yellow(),0.f,1.f,"CloseRangeDetectionRange");

        const PolyLine * poly = m_physComponent->getStickedPolyline();
        if (poly)
            AIUtils::debugDrawPolylineEdgeTypes(*poly);
    }
#endif // ITF_SUPPORT_EDITOR

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_GroundAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_GroundAIComponent_Template)

        SERIALIZE_OBJECT_WITH_FACTORY("roamBehavior", m_roamBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("runAwayBehavior", m_runAwayBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attackBehavior", m_attackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("crushedBehavior", m_crushedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("darktoonedBehavior", m_darktoonedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("undarktoonifyBehavior", m_undarktoonifyBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("sleepBehavior", m_sleepBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("spawnBehavior", m_spawnBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("floatBehavior", m_floatingBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("closeRangeAttackBehavior", m_closeRangeAttackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("hitWallBehavior", m_hitWallBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT("enemyDetectionRange", m_enemyDetectionRange);
    SERIALIZE_OBJECT("enemyChangeRange", m_enemyChangeRange);
    SERIALIZE_OBJECT("closeRangeAttackDetectionRange", m_closeRangeAttackDetectionRange);
    SERIALIZE_MEMBER("keepRunningAwayRange", m_keepRunningAwayRange);
    SERIALIZE_MEMBER("runAwayTime", m_runAwayTime);
    SERIALIZE_MEMBER("runAwayEnemyLimit", m_runAwayEnemyLimit);
    SERIALIZE_MEMBER("wakeUpOnEnemyDetection", m_wakeUpOnEnemyDetection);
    SERIALIZE_MEMBER("wakeUpOnCloseEnemyDetection", m_wakeUpOnCloseEnemyDetection);
    SERIALIZE_MEMBER("roamTime", m_roamTime);
    SERIALIZE_MEMBER("sleepTime", m_sleepTime);
    SERIALIZE_MEMBER("canDetectEnemiesWhileSleeping", m_canDetectEnemiesWhileSleeping);

    SERIALIZE_MEMBER("darktoon", m_darktoonPath);
    SERIALIZE_MEMBER("darktoonified", m_darktoonified_default);
    SERIALIZE_MEMBER("darktoonificationBone", m_darktoonificationBone);
    SERIALIZE_MEMBER("darktoonifiedHealth", m_darktoonifiedHealth);
    SERIALIZE_MEMBER("darktoonificationEjectionForceX", m_darktoonificationEjectionForceX);
    SERIALIZE_MEMBER("darktoonificationEjectionForceY", m_darktoonificationEjectionForceY);

    SERIALIZE_MEMBER("maxAttackDelay", m_maxAttackDelay);


    SERIALIZE_MEMBER("swimWaterResistMinSpeed",m_swimingWaterResistMinSpeed);
    SERIALIZE_MEMBER("swimWaterResistMaxSpeed",m_swimingWaterResistMaxSpeed);
    SERIALIZE_MEMBER("swimTopSpeed",m_swimingTopSpeed);
    SERIALIZE_MEMBER("swimSurfaceRange",m_swimingSurfaceRange);
    SERIALIZE_MEMBER("swimTurnUpsideDownWait",m_swimingTurnUpsideDownWait);
    SERIALIZE_MEMBER("swimInertiaEnterDownMultiplier",m_swimingInertiaEnterDownMultiplier);
    SERIALIZE_MEMBER("swimInertiaEnterNormalMultiplier",m_swimingInertiaEnterNormalMultiplier);
    SERIALIZE_MEMBER("swimInertiaEnterMaxSpeed",m_swimingInertiaEnterMaxSpeed);
    SERIALIZE_MEMBER("swimInertiaSpeed",m_swimingInertiaSpeed);

    SERIALIZE_OBJECT_WITH_FACTORY("dominoHitShape", m_dominoHitShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("canRehit", m_canRehit);
    SERIALIZE_MEMBER("dominoHitSpeedMultiplier", m_dominoHitSpeedMultiplier);
    SERIALIZE_MEMBER("squashPenetrationRadius", m_squashPenetrationRadius);
    END_SERIALIZATION()

        Ray_GroundAIComponent_Template::Ray_GroundAIComponent_Template()
        : m_runAwayTime(10.f)
        , m_maxAttackDelay(0.1f)
        , m_roamTime(0.0f)
        , m_sleepTime(0.0f)
        , m_canDetectEnemiesWhileSleeping(btrue)

        , m_enemyDetectionRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_enemyChangeRange(Vec2d(-1, -1), Vec2d(3, 1))        // 3 meters front, 1 behind, 1 above & below
        , m_closeRangeAttackDetectionRange(Vec2d(-1, -1), Vec2d(1, 1))        // 3 meters front, 1 behind, 1 above & below
        , m_keepRunningAwayRange(2.f)
        , m_runAwayEnemyLimit(3)
        , m_wakeUpOnEnemyDetection(btrue)
        , m_wakeUpOnCloseEnemyDetection(btrue)

        , m_darktoonified_default(bfalse)
        , m_darktoonifiedHealth(50)
        , m_darktoonificationEjectionForceX(200.0f)
        , m_darktoonificationEjectionForceY(200.0f)
        , m_swimingSurfaceRange(1.0f)
        , m_swimingTopSpeed(4.f)
        , m_swimingWaterResistMinSpeed(4.f)
        , m_swimingWaterResistMaxSpeed(10.f)
        , m_swimingInertiaEnterDownMultiplier(0.4f)
        , m_swimingInertiaEnterNormalMultiplier(0.2f)
        , m_swimingInertiaEnterMaxSpeed(10.f)
        , m_swimingInertiaSpeed(8.f)
        , m_swimingTurnUpsideDownWait(0.35f)
        , m_roamBehavior(NULL)
        , m_runAwayBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_darktoonedBehavior(NULL)
        , m_undarktoonifyBehavior(NULL)
        , m_sleepBehavior(NULL)
        , m_spawnBehavior(NULL)
        , m_floatingBehavior(NULL)
        , m_closeRangeAttackBehavior(NULL)
        , m_hitWallBehavior(NULL)

        , m_dominoHitShape(NULL)
        , m_canRehit(btrue)
        , m_dominoHitSpeedMultiplier(0.2f)
        , m_squashPenetrationRadius(0.6f)
    {
    }

    Ray_GroundAIComponent_Template::~Ray_GroundAIComponent_Template()
    {
        SF_DEL(m_roamBehavior);
        SF_DEL(m_runAwayBehavior);
        SF_DEL(m_attackBehavior);
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_deathBehavior);
        SF_DEL(m_crushedBehavior);
        SF_DEL(m_darktoonedBehavior);
        SF_DEL(m_undarktoonifyBehavior);
        SF_DEL(m_sleepBehavior);
        SF_DEL(m_spawnBehavior);
        SF_DEL(m_floatingBehavior);
        SF_DEL(m_closeRangeAttackBehavior);
        SF_DEL(m_hitWallBehavior);
        SF_DEL(m_dominoHitShape);
    }
} // namespace ITF