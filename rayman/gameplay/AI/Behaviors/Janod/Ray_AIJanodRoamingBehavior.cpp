#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIJANODROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodRoamingBehavior.h"
#endif //_ITF_RAY_AIJANODROAMINGBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#include "rayman/gameplay/Components/AI/Ray_JanodAIComponent.h"
#endif //_ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM


#define JANOD_STAND_ANIM        ITF_GET_STRINGID_CRC(Idle,1633200266)
#define JANOD_PREJUMP_ANIM      ITF_GET_STRINGID_CRC(prebounce,2711646441)
#define JANOD_JUMPING_ANIM      ITF_GET_STRINGID_CRC(bounce,421809396)
#define JANOD_BRISTLE_ANIM      ITF_GET_STRINGID_CRC(bristle,1958611750)
#define JANOD_FALL_ANIM         ITF_GET_STRINGID_CRC(FALL,100613701)
#define JANOD_FALL2_ANIM        ITF_GET_STRINGID_CRC(FREEFALL,1432709601)
#define JANOD_BRISTLETOFALL_ANIM ITF_GET_STRINGID_CRC(BristleToFall,3029929902)
#define JANOD_APEX_ANIM         ITF_GET_STRINGID_CRC(apex,3117147296)
#define JANOD_SLEEPS_ANIM       ITF_GET_STRINGID_CRC(sleeps,76585770)
#define JANOD_TINYCRUSH_ANIM    ITF_GET_STRINGID_CRC(TINYCRUSH,1741324672)

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIJanodRoamingBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIJanodRoamingBehavior_Template)
        SERIALIZE_MEMBER("apexTypicalHeight",m_apexTypicalHeight);
        SERIALIZE_MEMBER("detectionDistance",m_detectionDistance);       
        SERIALIZE_MEMBER("delayBeforeHurts", m_delayBeforeHurts);
        SERIALIZE_MEMBER("startToFloatTime", m_startToFloatTime);
        SERIALIZE_MEMBER("floatTimeBeforeFalling", m_floatTimeBeforeFalling);
        SERIALIZE_MEMBER("speedThatTriggersFloating", m_speedThatTriggersFloating);
        SERIALIZE_MEMBER("bristingAirMultiplier", m_bristingAirMultiplier);
        SERIALIZE_MEMBER("floatConstantForceDuration", m_floatConstantForceDuration);
        SERIALIZE_MEMBER("bristlingConstantForceDuration", m_bristlingConstantForceDuration);
        SERIALIZE_MEMBER("floatMaxForce", m_floatMaxForce);
        SERIALIZE_MEMBER("fallToVictimInitialSpeed", m_fallToVictimInitialSpeed);
        SERIALIZE_MEMBER("mustTrackVictim", m_mustTrackVictim);
        SERIALIZE_MEMBER("stimFeedback", m_stimFeedback);
        SERIALIZE_MEMBER("stimFeedbackDist", m_stimFeedbackDist);
        SERIALIZE_MEMBER("interJanodForce", m_interJanodForce);
        SERIALIZE_MEMBER("sideForce", m_sideForce);
        SERIALIZE_MEMBER("maxBounceAngleFromVertical", m_maxBounceAngleFromVertical);
        SERIALIZE_MEMBER("smallAngleForBounceOnlyForce", m_smallAngleForBounceOnlyForce);
        SERIALIZE_MEMBER("isAsleep", m_isAsleep);
        SERIALIZE_MEMBER("hasConstantX", m_hasConstantX);
        SERIALIZE_MEMBER("hasContantCoordinate_Force", m_hasContantCoordinate_Force);
        SERIALIZE_MEMBER("isCycleBased", m_isCycleBased);
        SERIALIZE_MEMBER("totalCycleDuration", m_totalCycleDuration);
        SERIALIZE_MEMBER("anticipationAnimationDuration", m_anticipationAnimationDuration);
        SERIALIZE_MEMBER("durationForNormalFallAnimSpeed", m_durationForNormalFallAnimSpeed);
        SERIALIZE_MEMBER("minFallAnimSpeedRatio", m_minFallAnimSpeedRatio);
        SERIALIZE_MEMBER("maxFallAnimSpeedRatio", m_maxFallAnimSpeedRatio);
        SERIALIZE_MEMBER("freeFallMinSpeed", m_freeFallMinSpeed);
    END_SERIALIZATION()

    Ray_AIJanodRoamingBehavior_Template::Ray_AIJanodRoamingBehavior_Template()
    : m_sideForce(100.f)
        , m_detectionDistance(10.f)
        , m_delayBeforeHurts(0.0f)
        , m_floatTimeBeforeFalling(1.f)
        , m_speedThatTriggersFloating(0.5f)
        , m_bristingAirMultiplier(20.f)
        , m_startToFloatTime(0.5f)
        , m_floatConstantForceDuration(0.5f)
        , m_bristlingConstantForceDuration(0.2f)
        , m_floatMaxForce(500.f)
        , m_fallToVictimInitialSpeed(1.f)
        , m_mustTrackVictim(btrue)
        , m_stimFeedback(500.f)
        , m_stimFeedbackDist(1)
        , m_interJanodForce(0.f)
        , m_maxBounceAngleFromVertical(btrue, 180.f)
        , m_smallAngleForBounceOnlyForce(btrue, 0.f)
        , m_isAsleep(bfalse)
        , m_hasConstantX(bfalse)
        , m_hasContantCoordinate_Force(50.f)
        , m_apexTypicalHeight(1.5f)
        , m_isCycleBased(bfalse)
        , m_totalCycleDuration(3.f)
        , m_anticipationAnimationDuration(0.32f)
        , m_durationForNormalFallAnimSpeed(0.3f)
        , m_minFallAnimSpeedRatio(0.75f)
        , m_maxFallAnimSpeedRatio(2.f)
        , m_freeFallMinSpeed(0.5f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIJanodRoamingBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIJanodRoamingBehavior)
    END_VALIDATE_BEHAVIOR()

    Ray_AIJanodRoamingBehavior::Ray_AIJanodRoamingBehavior()
    : m_physComponent(NULL)
    , m_state(State_NA)
    , m_timeInCurrentState(0)
    , m_previousSpeed(0,0)
    , m_bouncingOnWall(bfalse)
    , m_bouncingOnWall_NextSpeed(0,0)
    , m_apexPoint(0,0)
    , m_lastBristleContact(0,0)
    , m_computedBounceForce(0)
    , m_normalGravityMultiplier(1.f)
    , m_delayedSetPosAtApex(bfalse)
    , m_jumpFallAnimPlayRate(1)
    , m_signaledTinyCrush(bfalse)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIJanodRoamingBehavior::~Ray_AIJanodRoamingBehavior()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),m_aiComponent);

        bounceUtility_reset();
        //m_aiComponent->setFaction(RAY_FACTION_ENEMY);
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_physComponent->setUpdateRotation(bfalse);
        bounceUtility_registerEvents(m_actor, m_aiComponent);
        m_animComponent->setBankState(getTemplate()->getMustTrackVictim()?1:0);

        if (getTemplate()->getIsCycleBased())
        {
            m_normalGravityMultiplier = computeGravityMultiplierForCycleBased();
        }
        else
        {
            m_normalGravityMultiplier = 1;
        }
        m_physComponent->setGravityMultiplier(m_normalGravityMultiplier);

        f32 h = getTemplate()->getApexTypicalHeight();
        f32 g = m_normalGravityMultiplier*f32_Abs(PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth()).m_y);
        f32 DT = LOGICDT;
        //formula : v0� = 2.g.Yapex                  and   v0=(bounceForce-g).dt
        m_computedBounceForce = f32_Sqrt(2.f*g*h)/DT+g;

        bounceUtility_setBounceForceAmplitude(m_computedBounceForce);
        bounceUtility_setBounceForceMaxAngleFromVertical(getTemplate()->getMaxBounceAngleFromVertical().ToRadians());
        bounceUtility_setSmallAngleForBounceOnlyForce(getTemplate()->getSmallAngleForBounceOnlyForce().ToRadians());
        ITF_WARNING(m_actor, !(!getTemplate()->getMustTrackVictim() && getTemplate()->getInterJanodForce()!=0), "mustTrackVictim is not set, but there is an interjanod force");
        resetState();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::resetState()
    {
        m_state = State_NA;

        m_delayedSetPosAtApex = bfalse;
        m_signaledTinyCrush = bfalse;
        m_previousSpeed = Vec2d::Zero;
        bounceUtility_reset();
        setCurrentAnim(JANOD_STAND_ANIM);
        if (getTemplate()->getIsAsleep())
        {
            setState(State_Sleeps);
        }
        else
        {
            Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
            ITF_ASSERT_CRASH(janod, "no janod component");

            if (janod->getIfStartsAtApex())
            {
                m_delayedSetPosAtApex = btrue;                
            }

            if (janod->getMustWaitForTrigger())
            {
                setState(State_WaitTrigger);
            }
            else
            {
                setState(State_WaitDelayBeforeStarting);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::setCurrentAnim(const StringID &_newAnim)
    {
        m_currentAnim = _newAnim;
        m_animComponent->setAnim(_newAnim);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::setOutOfFloatSpeed()
    {
        Actor *victim = getVictim();
        if (victim)
        {
            Vec2d direction = victim->get2DPos()-m_actor->get2DPos();
            direction.normalize();
            if (direction.m_y>0)
            {
                direction.m_y = 0; //CAUTION : FROM NOW, DIRECTION MAY NOT BE NORMALIZED
            }
            const f32 maxAngle = MTH_PI*0.4f;
            f32 maxAngleCos, maxAngleSin;
            f32_CosSin(maxAngle, &maxAngleCos, &maxAngleSin);
            if (direction.m_x<-maxAngleSin)
                direction = Vec2d(-maxAngleSin, -maxAngleCos);
            else if (direction.m_x>maxAngleSin)
                direction = Vec2d(maxAngleSin, -maxAngleCos);

            m_physComponent->setSpeed(direction*getTemplate()->getFallToVictimInitialSpeed());
        }
        else
        {
            m_physComponent->setSpeed(Vec2d(0,-0.001f));
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::mayReachForcedX()
    {
        if (getTemplate()->getHasConstantX())
        {
            f32 mass = m_physComponent->getWeight();
            Vec2d force = bounceUtility_getForceToReach(mass, 
                m_physComponent->getSpeed(), 
                m_actor->get2DPos(),
                Vec2d(m_actor->getWorldInitialPos().m_x, m_actor->get2DPos().m_y),
                0.5f,
                getTemplate()->getHasContantCoordinate_Force());
            force.m_y = 0;
            m_physComponent->addForce(force);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_AIJanodRoamingBehavior::computeGravityMultiplierForCycleBased()
    {
        ITF_ASSERT(getTemplate()->getIsCycleBased());
        f32 jumpDuration = computeJumpDuration();
        m_jumpFallAnimPlayRate = getTemplate()->getDurationForNormalFallAnimSpeed()/jumpDuration;
        m_jumpFallAnimPlayRate = std::min(m_jumpFallAnimPlayRate, getTemplate()->getMaxFallAnimSpeedRatio());
        m_jumpFallAnimPlayRate = std::max(m_jumpFallAnimPlayRate, getTemplate()->getMinFallAnimSpeedRatio());
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(jumpFallPlayRate,2928522516), m_jumpFallAnimPlayRate);
        f32 currentG = f32_Abs(PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth()).m_y);
        ITF_ASSERT(currentG!=0);
        f32 desiredG = 2*getTemplate()->getApexTypicalHeight()/(jumpDuration*jumpDuration);
        return desiredG/currentG;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::setState(State_t _state, const void *_params)
    {
        State_t oldState = m_state;
        Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
        ITF_ASSERT_CRASH(janod, "no janod component");

        m_timeInCurrentState = 0;
        m_state = _state;
        m_physComponent->setSpeedMultiplier(Vec2d::One);
        m_physComponent->setGravityMultiplier(m_normalGravityMultiplier);
        m_physComponent->setAirFrictionMultiplier(0);
        
        janod->setIsInBristleMode(bfalse);

        switch(_state)
        {

        case State_WaitTrigger:
            {
                m_physComponent->setSpeedMultiplier(Vec2d::Zero);
                setCurrentAnim(JANOD_STAND_ANIM);
            } break;
        case State_WaitDelayBeforeStarting:
            {
                m_physComponent->setSpeedMultiplier(Vec2d::Zero);
                setCurrentAnim(JANOD_STAND_ANIM);
            } break;

        case State_Sleeps:
            {
                setCurrentAnim(JANOD_SLEEPS_ANIM);
            } break;
        case State_PreJump:
            {
                //LOG("--this=%p time=%f", this, (f32)SYSTEM_ADAPTER->getTime());

                ITF_ASSERT_CRASH(_params!=NULL, "no parameter for prejump");
                const Vec2d &edgeDir = *(const Vec2d*)_params;
                m_edgeWhenEnteredPreJump = edgeDir;
                m_physComponent->setSpeedMultiplier(Vec2d::Zero);
                setCurrentAnim(JANOD_PREJUMP_ANIM);
            }break;
        case State_JumpOrFall:
            {
                if (oldState==State_FloatingBeforeFalling_Part1)
                {
                    setOutOfFloatSpeed();
                }

                bounceUtility_EnterJumpOrFall(JANOD_JUMPING_ANIM, m_physComponent, m_animComponent);
            }break;
        case State_ReactingToTinyCrush:
            {
                setCurrentAnim(JANOD_TINYCRUSH_ANIM);
                m_physComponent->setSpeed(Vec2d(0,0));
                m_physComponent->setGravityMultiplier(0.f);
            } break;
        case State_Bristling:
            setCurrentAnim(JANOD_BRISTLE_ANIM);
            m_physComponent->setSpeed(Vec2d(0,0));
            m_physComponent->setGravityMultiplier(0.f);
            break;
        case State_FloatingBeforeFalling_Part1:
            {
                setCurrentAnim(JANOD_APEX_ANIM);
                m_physComponent->setGravityMultiplier(0);
                m_apexPoint = m_actor->get2DPos()+getTemplate()->getStartToFloatTime()*m_physComponent->getSpeed();
            } break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::onActivate()
    {
        if (m_state==State_NA)
        {
            resetState();
        }
        else
        {
            if (m_state==State_WaitTrigger)
            {
                setState(State_WaitTrigger);
            }
            else if (m_state==State_WaitDelayBeforeStarting)
            {
                setState(State_WaitDelayBeforeStarting);
            }
            else  if (getTemplate()->getIsAsleep())
            {
                setState(State_Sleeps);
            }
            else
            {
                setState(State_JumpOrFall);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::onDeactivate()
    {
        setState(State_JumpOrFall);
        m_physComponent->setGravityMultiplier(1);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor *Ray_AIJanodRoamingBehavior::getVictim() const
    {
        if (!getTemplate()->getMustTrackVictim())
        {
            return NULL;
        }
        Actor* victim = AIUtils::getClosestEnemy(m_actor, m_physComponent->getSpeed().m_x < 0.f, getTemplate()->getDetectionDistance());
        return victim;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Vec2d Ray_AIJanodRoamingBehavior::bounceUtility_getBounceForce( Vec2d _edgeDir)
    {
        Vec2d force = bounceUtility_getBounceOnlyForce(_edgeDir, 1.f);
        
        Actor* victim = getVictim();
        if (victim)
        {

            f32 dx = victim->get2DPos().m_x - m_actor->get2DPos().m_x;
            f32 forcex = getTemplate()->getSideForce();
            if (dx<0)
            {
                forcex = -forcex;
            }
            force.m_x += forcex;
        }

        return force;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::reactToOtherJanods()
    {
        static ActorList actorList;
        AABB searchZone = m_actor->getAABB();
        const Vec2d margin(1,1);
        searchZone.setMin(searchZone.getMin()-margin);
        searchZone.setMax(searchZone.getMax()+margin);

        PhantomComponent *phantom = m_actor->GetComponent<PhantomComponent>();
        if (!phantom)
        {
            return;
        }
        PhysShape *shape1 = phantom->getShape();
        if (!shape1)
        {
            return;
        }
        PhysShapeCircle *circle1 = shape1->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
        if (!circle1)
        {
            return;
        }
        PhysPhantom *physPhantom1 = phantom->getPhantom();
        ITF_ASSERT_CRASH(physPhantom1, "no physphantom");
        
        Vec2d center1 = physPhantom1->getPos();
        f32 radius1_2 = circle1->getRadius();
        if (radius1_2==0)
        {
            return;
        }
        f32 invRadiusSum = 1.f/(2.f*radius1_2);

        actorList.clear();
        AI_MANAGER->getActorsFromLayerWithAIComponent<Ray_JanodAIComponent>(m_actor->getDepth(),searchZone, actorList);
        for (u32 i=0; i<actorList.size(); i++)
        {
            Actor *actor = actorList[i];
            if (actor==m_actor)
            {
                continue;
            }
            PhantomComponent *phantom = actor->GetComponent<PhantomComponent>();
            if (!phantom)
            {
                continue;
            }
            PhysShape *shape2 = phantom->getShape();
            if (!shape2)
            {
                continue;
            }
            PhysPhantom *physPhantom2 = phantom->getPhantom();
            if (!physPhantom2)
            {
                continue;
            }

            const Vec2d &center2 = physPhantom2->getPos();
            
            Vec2d deltaCenter(center1-center2);
            f32 dist = deltaCenter.norm();
            f32 normalizedDist = dist * invRadiusSum;
            if (normalizedDist>1) 
            {
                normalizedDist = 1;
            }
            Vec2d forceDirection = deltaCenter;
            if (dist>0)
            {
                forceDirection *= 1.f/dist;                
            }
            normalizedDist = std::max(normalizedDist, 0.9f);
            f32 force = (1-normalizedDist)*getTemplate()->getInterJanodForce();

            if (force>0)
            {
                Vec2d vforce = forceDirection*force;
                m_physComponent->addForce(vforce);
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIJanodRoamingBehavior::checkIfSwitchToBristle(bbool _affectBristlingPoint)
    {
        StickToPolylinePhysComponent *phys = m_actor->GetComponent<StickToPolylinePhysComponent>();
        static PhysContactsContainer contacts;

        if (wasCrushedRecently())
            return bfalse;

        ITF_ASSERT_CRASH(phys, "no StickToPolylinePhysComponent");

        if (phys)
        {
            PhysShapeCircle circle(phys->getRadius());
            
            {
                PhysSweepInfo janodSweepInfo;
                PhysCollisionSolver::calculateSweepInfo(m_actor->get2DPos(), 
                    m_actor->get2DPos(), 
                    m_actor->getAngle(),
                    &circle,
                    janodSweepInfo);

                for (u32 i=0; i<RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    Ray_Player *player = (Ray_Player*)RAY_GAMEMANAGER->getPlayer(i);
                    if (player && player->getActive() && !player->isDead())
                    {
                        PhysSweepInfo playerSweepInfo;
                        EventQueryPhysShape queryShape;
                        Actor *playerActor = player->getActor();
                        if (!playerActor)
                            continue;

                        playerActor->onEvent(&queryShape);
                        const PhysShape *playerShape = queryShape.getPhysShape();
                        if (!playerShape)
                        {
                            continue;
                        }
                        PhysCollisionSolver::calculateSweepInfo(queryShape.getPosition(),
                            queryShape.getPosition(),
                            queryShape.getAngle(),
                            queryShape.getPhysShape(),
                            playerSweepInfo);

                        if (PhysCollisionSolver::collide(playerSweepInfo, janodSweepInfo, contacts))
                        {
                            if (_affectBristlingPoint)
                            {
                                m_lastBristleContact = contacts[0].m_contactPoint;
                                for (u32 i=1; i<contacts.size(); i++)
                                {
                                    m_lastBristleContact += contacts[i].m_contactPoint;
                                }
                                m_lastBristleContact *= 1/(f32) contacts.size();
                                //DebugDraw::circle(m_lastBristleContact, m_actor->getDepth(), 0.2f, Color::white(), 1.f);
                            }
                            contacts.clear();
                            return btrue;
                        }                        
                    }
                }

            }
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::bounceUtility_UpdateJumpOrFall_onIsOnGround(const Vec2d &_normalizedEdge, bbool /*isFromActor*/)
    {
        setState(State_PreJump, &_normalizedEdge );
    }

    void Ray_AIJanodRoamingBehavior::bounceUtility_UpdateJumpOrFall_onNearTopOfJump()
    {
        setState(State_FloatingBeforeFalling_Part1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_AIJanodRoamingBehavior::computeJumpDuration()
    {
        ITF_ASSERT(getTemplate()->getIsCycleBased());

        f32 preJumpAnimDuration=getTemplate()->getAnticipationAnimationDuration();

        f32 result = 0.5f*(getTemplate()->getTotalCycleDuration()-preJumpAnimDuration-getTemplate()->getFloatTimeBeforeFalling());
        ITF_WARNING(m_actor,
            result>0, 
            "prejump animation (%.1f), floating duration(%.1f) do not sum up to be less than total cycle duration", 
            preJumpAnimDuration, 
            getTemplate()->getFloatTimeBeforeFalling());
        
        if (result<0)
            result = 0.1f;
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::updateJumpOrFall()
    {
        reactToOtherJanods();

        StringID fallAnim;

        if (m_currentAnim == JANOD_FALL_ANIM || m_currentAnim==JANOD_BRISTLETOFALL_ANIM)
        {
             if (m_animatedComponent->isMainNodeFinished() && m_physComponent->getSpeed().m_y<-getTemplate()->getFreeFallMinSpeed())
                 fallAnim = JANOD_FALL2_ANIM;
             else
                 fallAnim = m_currentAnim;
        }
        else if (m_currentAnim == JANOD_FALL2_ANIM)
        {
            fallAnim = JANOD_FALL2_ANIM;
        }
        else if (m_currentAnim == JANOD_BRISTLE_ANIM)
        {
            fallAnim = JANOD_BRISTLETOFALL_ANIM;
        }
        else
        {
            fallAnim = JANOD_FALL_ANIM;
        }

        bounceUtility_UpdateJumpOrFall(m_physComponent,
            JANOD_JUMPING_ANIM,
            fallAnim,
            getTemplate()->getSpeedThatTriggersFloating(),
            m_previousSpeed);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        m_timeInCurrentState += _delta;
        m_animComponent->setBankState(getTemplate()->getMustTrackVictim()?1:0);
        
        if (m_delayedSetPosAtApex)
        {
            m_actor->setPos(m_actor->getWorldInitialPos()+Vec3d(0,getTemplate()->getApexTypicalHeight(),0));
            m_delayedSetPosAtApex = bfalse;
        }

        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_WaitTrigger:
            {
                //we exit in onevent
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_ReactingToTinyCrush:
            {
                mayReachForcedX();
                if (m_animatedComponent->isMainNodeFinished())
                {
                    switchToStartState();
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_WaitDelayBeforeStarting:
                {
                    Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
                    ITF_ASSERT_CRASH(janod, "no janod component");
                    if (m_timeInCurrentState>janod->getDelayBeforeStarting())
                    {
                        if (janod->getIfStartsAtApex())
                        {
                            setState(State_FloatingBeforeFalling_Part1);
                            m_timeInCurrentState = getTemplate()->getFloatTimeBeforeFalling();
                        }
                        else
                        {
                            setState(State_JumpOrFall);
                        }
                    }
                } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Sleeps:
                {
                    Vec3d initialPos = m_actor->getWorldInitialPos();
                    f32 mass = m_physComponent->getWeight();
                    Vec2d force = bounceUtility_getForceToReach(mass, 
                        m_physComponent->getSpeed(), 
                        m_actor->get2DPos(),
                        (const Vec2d&)initialPos,
                        0.1f,
                        getTemplate()->getHasContantCoordinate_Force());
                    Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
                    if (janod && janod->canFallWhenSleeping())
                    {
                        force.m_y = 0;
                    }
                    m_physComponent->addForce(force);

                    if (checkIfSwitchToBristle(btrue))
                    {
                        setState(State_Bristling);
                    }
                    else if (m_signaledTinyCrush)
                    {
                        setState(State_ReactingToTinyCrush);
                    }
                } break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_PreJump:
            {
                reactToOtherJanods();
                if (m_animatedComponent->isMainNodeFinished())
                {
                    bounceUtility_bounceOnEdge_applyBouncePhysics(m_physComponent, m_edgeWhenEnteredPreJump);
                    setState(State_JumpOrFall);
                }
                else
                {
                    if (checkIfSwitchToBristle(btrue))
                    {
                        setState(State_Bristling);
                    }
                    else if (m_signaledTinyCrush)
                    {
                        setState(State_ReactingToTinyCrush);
                    }
                }

            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_FloatingBeforeFalling_Part1:
            {
                //bounceUtility_updateBounceOnWall(m_physComponent); //It uses input speed, which may cause problem in this state
                reactToOtherJanods();

                f32 mass = m_physComponent->getWeight();
                Vec2d speed = m_physComponent->getSpeed();
                m_physComponent->addForce(bounceUtility_getForceToReach(mass, speed, m_actor->get2DPos(), m_apexPoint, getTemplate()->getFloatConstantForceDuration(), getTemplate()->getFloatMaxForce()));
                
                if (m_timeInCurrentState>getTemplate()->getFloatTimeBeforeFalling())
                {
                    setState(State_JumpOrFall);
                }
                else if (checkIfSwitchToBristle(btrue))
                {
                    setState(State_Bristling);
                }
                else if (m_signaledTinyCrush)
                {
                    setState(State_ReactingToTinyCrush);
                }

            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_JumpOrFall:
                {
                    updateJumpOrFall();
                    mayReachForcedX();
                    //
                    if (checkIfSwitchToBristle(btrue))
                    {
                        setState(State_Bristling);
                    }
                    else if (m_signaledTinyCrush)
                    {
                        setState(State_ReactingToTinyCrush);
                    }

                }break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Bristling:
                {
                    f32 mass = m_physComponent->getWeight();
                    Vec2d speed = m_physComponent->getSpeed();

                    bbool isReallyBristling = m_timeInCurrentState>=LOGICDT;
                    Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
                    ITF_ASSERT_CRASH(janod, "no janod component");
                    janod->setIsInBristleMode(isReallyBristling);

                    if (getTemplate()->getMustTrackVictim())
                    {
                        m_physComponent->addForce(bounceUtility_getForceToReach(mass, speed, m_actor->get2DPos(), m_lastBristleContact, getTemplate()->getBristlingConstantForceDuration(), getTemplate()->getFloatMaxForce()));
                    }
                    mayReachForcedX();

                    if (m_signaledTinyCrush && !checkIfSwitchToBristle(bfalse))
                    {
                        setState(State_ReactingToTinyCrush);
                    }
                    else
                    {
                        if (isReallyBristling && checkIfSwitchToBristle(bfalse) )
                        {
                            attackEnnemies();
                        }

                        if (m_timeInCurrentState>=getTemplate()->getBristlingTime())
                        {
                            switchToStartState();
                        }
                    }

                } break;
        }

        m_previousSpeed = m_physComponent->getSpeed();
        bounceUtility_clearCurrentBounceCollision();
        m_signaledTinyCrush = bfalse;

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::switchToStartState()
    {
        if (getTemplate()->getIsAsleep())
        {
            setState(State_Sleeps);
        }
        else
        {
            setState(State_JumpOrFall);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::onActionFinished()
    {
        Super::onActionFinished();

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodRoamingBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        bounceUtility_processEvent(_event, m_physComponent);
        Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
        ITF_ASSERT_CRASH(janod, "no janod component");

        if ( EventCrushed *crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
        {
            if (crushed->getCharacterSize()==CHARACTERSIZE_SMALL)
                m_signaledTinyCrush = btrue;
        }
       /* else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            janod->processHit(hit);            
        }*/
        /*else if ( Ray_EventAttackGranted* attackGranted = _event->DynamicCast<Ray_EventAttackGranted>(ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789)) )
        {
            bbool attacks = m_state == State_Bristling;
            attackGranted->setAttackUsed(attacks);

            if (attacks && !wasCrushedRecently())
            {
                sendStim(attackGranted->getSender());
            }
        }*/
        else if ( EventTrigger *trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (m_state == State_WaitTrigger && trigger->getActivated())          
            {
                setState(State_WaitDelayBeforeStarting);
            }
        }
    }

}

