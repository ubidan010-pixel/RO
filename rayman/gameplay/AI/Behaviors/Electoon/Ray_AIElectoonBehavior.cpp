#include "precompiled_gameplay_rayman.h"

#ifndef __RAY_AIELECTOONBEHAVIOR_H__
#include "rayman/gameplay/AI/Behaviors/Electoon/Ray_AIElectoonBehavior.h"
#endif //__RAY_AIELECTOONBEHAVIOR_H__

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_TRAIL3DCOMPONENT_H_
#include "gameplay/components/misc/Trail3DComponent.h"
#endif //_ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_ELECTOONTARGETCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonTargetComponent.h"
#endif //_ITF_RAY_ELECTOONTARGETCOMPONENT_H_


#include <float.h>

//Eject, stand, walk, and N seconds after stand start : yeah

#define ELECTOON_MIN_SCALE                  1.2f
#define ELECTOON_MAX_SCALE                  1.4f

#define ELECTOON_LOVE_WHEN_NEAR_PROBABILITY 0.8f
#define ELECTOON_WATCHDOG_TIMER             30.f
#define ELECTOON_DISAP_MIN_SINUS_PHASE      0.3f //Disap is for disappear
#define ELECTOON_DISAP_MAX_SINUS_PHASE      0.6f
#define ELECTOON_DISAP_MIN_FASTSINUS_PHASE  1.f
#define ELECTOON_DISAP_MAX_FASTSINUS_PHASE  1.5f
#define ELECTOON_DISAP_MIN_FASTSINUS_MULT   0.5f
#define ELECTOON_DISAP_MAX_FASTSINUS_MULT   0.7f

#define ELECTOON_MIN_EJECTION_ANGLE			(60*MTH_DEGTORAD)
#define ELECTOON_MAX_EJECTION_ANGLE			((180-60)*MTH_DEGTORAD)

#define ELECTOON_WALK_EFFICIENCY        10.f

#define ELECTOON_MIN_DISAPPEAR_DELAY_DELTA  1.f //delta delay in seconds

//#define ELECTOON_MIN_TIME_BEFORE_YEAH		7.f
//#define ELECTOON_MAX_TIME_BEFORE_YEAH		10.f

#define ELECTOON_NORMALSTAND1_ANIM			ITF_GET_STRINGID_CRC(Stand_01,1160809003)
#define ELECTOON_NORMALSTAND2_ANIM			ITF_GET_STRINGID_CRC(Stand_02,3697076128)
#define ELECTOON_NORMALSTAND3_ANIM			ITF_GET_STRINGID_CRC(Stand_03,582173666)
#define ELECTOON_LOVE1_ANIM					ITF_GET_STRINGID_CRC(Stand_04,1262205851)
#define ELECTOON_LOVE2_ANIM					ITF_GET_STRINGID_CRC(Stand_05,656011123)
#define ELECTOON_LOVE3_ANIM					ITF_GET_STRINGID_CRC(Stand_06,1855247032)
#define ELECTOON_EJECTION1_ANIM				ITF_GET_STRINGID_CRC(Jump_Cartwheel,4198343224)
#define ELECTOON_EJECTION2_ANIM				ITF_GET_STRINGID_CRC(Jump_Stand,282709978)
#define ELECTOON_WALK_ANIM					ITF_GET_STRINGID_CRC(Run_01,759362506)
#define ELECTOON_YEAH1_ANIM					ITF_GET_STRINGID_CRC(Yeah_01,4184894003)
#define ELECTOON_YEAH2_ANIM					ITF_GET_STRINGID_CRC(Yeah_02,2148569728)
#define ELECTOON_PREDISAPPEARS_ANIM         ITF_GET_STRINGID_CRC(Jump_Antic,3942202937)
#define ELECTOON_DISAPPEARS_ANIM			ITF_GET_STRINGID_CRC(Jump_Stand,282709978)
#define ELECTOON_WALK_DECIDE_DIRECTION		2.f
#define ELECTOON_WALK_FORCE					10.f
#define ELECTOON_DISAPPEAR_SPEED			10.f

#define ELECTOON_MAX_DELTA_Y_TO_FOLLOW      1.5f


namespace ITF
{
    static const f32 GROUND_NORMAL_Y_THRESHOLD = sinf(60*MTH_DEGTORAD);

    IMPLEMENT_OBJECT_RTTI(Ray_AIElectoonBehavior_Template)

        BEGIN_SERIALIZATION_CHILD(Ray_AIElectoonBehavior_Template)
            SERIALIZE_OBJECT("walk", m_walkAction);
            SERIALIZE_MEMBER("minTimeBeforeWalking", m_minTimeBeforeWalking);
            SERIALIZE_MEMBER("maxTimeBeforeWalking", m_maxTimeBeforeWalking);
            SERIALIZE_MEMBER("minTimeWalking", m_minTimeWalking);
            SERIALIZE_MEMBER("maxTimeWalking", m_maxTimeWalking);
            SERIALIZE_MEMBER("walkSpeed", m_typicalSpeed);
            SERIALIZE_MEMBER("minEjectionSpeed", m_minEjectionSpeed);
            SERIALIZE_MEMBER("maxEjectionSpeed", m_maxEjectionSpeed);
            SERIALIZE_MEMBER("lovePlayerDistance", m_lovePlayerDist);
            SERIALIZE_MEMBER("timeBeforeCanStopWalkingAndStandAgain", m_timeBeforeCanStopWalkingAndStandAgain);
            SERIALIZE_MEMBER("mustStay", m_mustStay);
            SERIALIZE_MEMBER("maxDeltaYToFollow", m_maxDeltaYToFollow);
    END_SERIALIZATION()

        Ray_AIElectoonBehavior_Template::Ray_AIElectoonBehavior_Template()
        : m_walkAction(NULL)
        , m_minTimeBeforeWalking(1.f)
        , m_maxTimeBeforeWalking(2.f)
        , m_timeBeforeCanStopWalkingAndStandAgain(2.f)
        , m_minTimeWalking(5.f)
        , m_maxTimeWalking(7.f)
        , m_typicalSpeed(2.f)
        , m_minEjectionSpeed(3.f)
        , m_maxEjectionSpeed(6.f)
        , m_lovePlayerDist(2.f)
        , m_mustStay(bfalse)
        , m_maxDeltaYToFollow(ELECTOON_MAX_DELTA_Y_TO_FOLLOW)
    {

    }

    Ray_AIElectoonBehavior_Template::~Ray_AIElectoonBehavior_Template()
    {
        SF_DEL(m_walkAction);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIElectoonBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIElectoonBehavior)
        VALIDATE_BEHAVIOR_PARAM("walk", getTemplate()->getWalkInDirAction()!=NULL, "Ray_AIElectoonBehavior_Template needs to set its walk Action with a AIWalkInDirAction_Template");
    END_VALIDATE_BEHAVIOR()

    ActorList Ray_AIElectoonBehavior::m_tmpActorList;
    bbool     Ray_AIElectoonBehavior::m_anElectoonStartedSignal=bfalse;

    Ray_AIElectoonBehavior::Ray_AIElectoonBehavior()
        : Super()
        , m_physComponent(NULL)
        , m_trailComponent(NULL)
        , m_walkAction(NULL)
        , m_state(State_NA)
        , m_timeInCurrentState(0)        
        , m_idealSpeed(0)
        , m_speedFactor(1.f)
        , m_fx1Handle(U32_INVALID)
        , m_fx2Handle(U32_INVALID)
        , m_posWhenstartDisappearing(0,0,0)
        , m_watchdogTimer(0)
        , m_totalTimeWalkingSinceAppeared(0)
        , m_justBlockedByWall(bfalse)
        , m_walking_blockedCounter(0)
        , m_walking_framesSinceChangedDirection(0)
        , m_roamingDir(Roam_Left)
    {
        m_holeSearchDist = 0.2f;
    }

    Ray_AIElectoonBehavior::~Ray_AIElectoonBehavior()
    {
    }

    void Ray_AIElectoonBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_randomForStandAnim = rand();
        m_timeBeforeWalking = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinTimeBeforeWalking(), getTemplate()->getMaxTimeBeforeWalking());
        m_timeWalking = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinTimeWalking(), getTemplate()->getMaxTimeWalking());
        m_deltaTimeBeforeYeah = Seeder::getSharedSeeder().GetFloat(0.f, 1.f);
        m_speedFactor = Seeder::getSharedSeeder().GetFloat(1.f, 1.1f);
        m_disappearSinFactor = Seeder::getSharedSeeder().GetFloat(ELECTOON_DISAP_MIN_SINUS_PHASE, ELECTOON_DISAP_MAX_SINUS_PHASE);
        m_disappearFastSinFactor = Seeder::getSharedSeeder().GetFloat(ELECTOON_DISAP_MIN_FASTSINUS_PHASE, ELECTOON_DISAP_MAX_FASTSINUS_PHASE);
        m_holeSearchDist = Seeder::getSharedSeeder().GetFloat(0.2f, 0.5f);
        if (Seeder::getSharedSeeder().GetFloat(0.f, 1.f)>0.5f)
            m_disappearSinFactor = -m_disappearSinFactor;
        m_disappearFastSinMult = Seeder::getSharedSeeder().GetFloat(ELECTOON_DISAP_MIN_FASTSINUS_MULT, ELECTOON_DISAP_MAX_FASTSINUS_MULT);
        if (Seeder::getSharedSeeder().GetFloat(0.f, 1.f)>0.5f)
            m_disappearFastSinMult = - m_disappearFastSinMult;

        m_willStopWhenNear = Seeder::getSharedSeeder().GetFloat(0.f, 1.f)<ELECTOON_LOVE_WHEN_NEAR_PROBABILITY;
        m_justBlockedByWall = bfalse;

        m_walkAction = static_cast<AIWalkInDirAction*>(createAiAction(getTemplate()->getWalkInDirAction()));

        Super::onActorLoaded(_hotReload);

        m_actor->growRelativeAABB(AABB(Vec2d(-0.01f,-0.01f),Vec2d(0.01f,0.01f)));

        m_deltaTimeBeforeYeah = Seeder::getSharedSeeder().GetFloat(0.f, 1.f);
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_trailComponent = m_actor->GetComponent<Trail3DComponent>();

        f32 scale = Seeder::getSharedSeeder().GetFloat(ELECTOON_MIN_SCALE, ELECTOON_MAX_SCALE);
        m_actor->setScale(Vec2d(scale,scale));

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetTarget,4067373371),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);

        setIdealSpeed(0);
        setState(State_PreAppear);        

    }

    void Ray_AIElectoonBehavior::onResourceReady()
    {
        Super::onResourceReady();						
    }

    Actor *Ray_AIElectoonBehavior::getNearestPlayer()
    {
        m_tmpActorList.clear();
        AIUtils::getPlayers(m_actor->getDepth(), m_tmpActorList);
        Actor *bestActor=NULL;
        f32 bestDist = FLT_MAX;
        for (u32 i=0; i<m_tmpActorList.size(); i++)
        {
            Actor *player=m_tmpActorList[i];
            Vec2d deltaToPlayer = player->get2DPos()-m_actor->get2DPos();
            f32 dist = deltaToPlayer.sqrnorm();
            if (dist<bestDist)
            {
                bestActor = player;
                bestDist = dist;
                break;
            }
        }
        return bestActor;
    }

    void Ray_AIElectoonBehavior::switchToStand(bbool _canChangeAnim)
    {
        Actor *player = getNearestPlayer();
        bbool isPlayerNearby = bfalse;
        Vec2d stand_DeltaToPlayer(0,0);

        if (player)
        {
            stand_DeltaToPlayer = player->get2DPos()-m_actor->get2DPos();
            f32 dist = (stand_DeltaToPlayer).sqrnorm();
            isPlayerNearby = (dist<(getTemplate()->getLovePlayerDist()*getTemplate()->getLovePlayerDist()));
        }
        

        //select right animation, depending on whether player is nearby, 
        if (isPlayerNearby)
        {
            const StringID electoon_LoveAnims[]=
            {
                ELECTOON_LOVE1_ANIM,
                ELECTOON_LOVE2_ANIM,
                ELECTOON_LOVE3_ANIM
            };
            u32 r = m_randomForStandAnim%ITF_ARRAY_SIZE(electoon_LoveAnims);
            m_actor->setFlipped(stand_DeltaToPlayer.m_x<0);
            if (_canChangeAnim)
                m_animatedComponent->setAnim(electoon_LoveAnims[r]);

        }
        else
        {
            const StringID electoon_NormalStandAnim[]=
            {
                ELECTOON_NORMALSTAND1_ANIM,
                ELECTOON_NORMALSTAND2_ANIM,
                ELECTOON_NORMALSTAND3_ANIM
            };
            u32 r = m_randomForStandAnim%ITF_ARRAY_SIZE(electoon_NormalStandAnim);
            if (_canChangeAnim)
                m_animatedComponent->setAnim(electoon_NormalStandAnim[r]);
        }
        setState(State_Stand);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor *Ray_AIElectoonBehavior::getRewardSpawner(bbool &_mustZeroSpeed)
    {
        _mustZeroSpeed = bfalse;
        if (m_ejectionTargetObj.isValid())
        {
            BaseObject *obj = m_ejectionTargetObj.getObject();
            Actor *res = NULL;
            if (obj)
            {
                res = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            }
            bbool isInvalid = bfalse;
            if (!res)
            {
                isInvalid = btrue;
            }
            else
            {
                TriggerComponent* trigger = res->GetComponent<TriggerComponent>();
                ITF_ASSERT(trigger);
                bbool isSpawnerLoaded = btrue;
                if (trigger)
                {
                    isSpawnerLoaded = !trigger->isTriggerOnceDone();
                }

                if (!isSpawnerLoaded)
                {
                    isInvalid = btrue;
                }
            }

            if (isInvalid)
            {
                _mustZeroSpeed = btrue;
                m_ejectionTargetObj.invalidate();
                return NULL;
            }
            else
            {
                return res;
            }
        }
        else
        {
            return NULL;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIElectoonBehavior::updateWalking_blockedCounter(f32 _idealSpeed, f32 _deltaTime)
    {
        f32 threshold=0.1f;
        if (f32_Abs(m_idealSpeed)>(2*threshold)
            && f32_Abs(m_physComponent->getSpeed().sqrnorm()<(threshold*threshold))!=0)
        {
            m_walking_blockedCounter+= _deltaTime;
        }
        else
        {
            m_walking_blockedCounter = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIElectoonBehavior::setIdealSpeed(f32 _idealSpeed)
    {
        if (_idealSpeed*m_idealSpeed<=0)
        {
            m_walking_blockedCounter = 0;
            m_walking_framesSinceChangedDirection = 0;
            m_justBlockedByWall = bfalse;
        }
        m_idealSpeed = _idealSpeed;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIElectoonBehavior::hasHoleInFront_Extended( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
        u32 _startEdge, f32 _searchDist, f32 _radius, 
        const Vec2d& _gravityDir, f32& _holeDepth )
    {
        if (AIUtils::hasHoleInFront(
            m_actor, 
            _walkLeft, _poly,
            _startEdge, 
            _searchDist, 
            _radius, 
            _gravityDir,
            _holeDepth ))
        {
            PhysRayCastContactsContainer contacts;
            //confirm
            Vec2d feet = _actor->get2DPos();
            f32 signedDist = _walkLeft?-_searchDist:_searchDist;
            Vec2d feetPlusDist = feet+Vec2d(signedDist,0);
            Vec2d lower = feetPlusDist + Vec2d(0,-0.3f); ///////////////Tweaked value for electoons
            bbool r = PHYSWORLD->rayCastEnvironment( feetPlusDist, lower,
                ECOLLISIONFILTER_ENVIRONMENT,
                m_actor->getDepth(),
                contacts );
            if (!r)
            {
                _holeDepth = 1000;
                return btrue;
            }
            return bfalse;
        }
        else
        {
            return bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIElectoonBehavior::updateWalking(f32 _deltaTime, bbool _signaledMustGoAway)
    {
        bbool mustStay = getTemplate()->getMustStay();

        m_totalTimeWalkingSinceAppeared += _deltaTime;

        if ( !mustStay && m_watchdogTimer>ELECTOON_WATCHDOG_TIMER)
        {
            setState(State_PreYeah_Signal);
        }
        else if (isInWater() && !mustStay)
        {
            setState(State_PreDisappears);
        }
        else if (m_aiComponent->getHealth()<=0)
        {
            setState(State_PreDisappears);
        }
        else if ( ( !mustStay && m_totalTimeWalkingSinceAppeared > m_timeWalking ) || _signaledMustGoAway)
        {
            setState(State_PreYeah_Signal);
        }
        else
        {
            Actor *player = getNearestPlayer();
            if (!player)
            {
                setState(State_PreYeah_Signal);
            }
            else
            {
                Vec2d deltaToPlayer = player->get2DPos()-m_actor->get2DPos();
                f32 randomizedSpeed = getTemplate()->getTypicalSpeed() * m_speedFactor;
                f32 sqDistToPlayer = deltaToPlayer.sqrnorm();

                bbool mustZeroSpeed;
                Actor *rewardSpawner = getRewardSpawner(mustZeroSpeed);

                if (mustZeroSpeed)
                {
                    setIdealSpeed(0);
                    switchToStand(btrue);
                    return;////////////////////EXIT POINT
                }

                if (rewardSpawner)
                {
                    Vec2d deltaToReward = rewardSpawner->get2DPos()-m_actor->get2DPos();
                    m_roamingDir = (deltaToReward.m_x<0)?Roam_Left:Roam_Right;
                }
                else
                {
                    if (m_willStopWhenNear 
                        && (m_timeInCurrentState>getTemplate()->getTimeBeforeCanStopWalkingAndStandAgain()) 
                        && (sqDistToPlayer<(getTemplate()->getLovePlayerDist()*getTemplate()->getLovePlayerDist())))
                    {
                        switchToStand(btrue);
                        return;////////////////////EXIT POINT
                    }
                }

                setIdealSpeed((m_roamingDir==Roam_Left)?-randomizedSpeed:randomizedSpeed);

                // TODO: If you want to randomize the speed, add random values on the template
                //m_walkAction->setSpeed(0.f,randomizedSpeed);
                if ( m_walkAction )
                {
                    m_walkAction->setWalkLeft(m_idealSpeed<0);
                }

                updateWalking_blockedCounter(m_idealSpeed,_deltaTime);

                bbool isBlocked = m_walking_blockedCounter>0.2f;

                PolyLine *polyLine = m_physComponent->getStickedPolyline();
                if (polyLine)
                {
                    f32 holeDepth = 0;
                    f32 searchDist = m_holeSearchDist;

                    bbool mustStand = isBlocked && m_physComponent->getSpeed().norm()<0.01f;
                    mustStand = mustStand || hasHoleInFront_Extended(
                        m_actor, 
                        m_idealSpeed<0, polyLine,
                        m_physComponent->getStickedEdgeIndex(), 
                        searchDist, 
                        m_physComponent->getRadius(), 
                        m_physComponent->getCurrentGravityDir(),
                        holeDepth );
                    if (mustStand)
                    {
                        //setState(State_Yeah); //temporary. Dont set preyeah_signal
                        if (m_timeInCurrentState<1.f)
                        {
                            if (m_walking_framesSinceChangedDirection>(0.2f/LOGICDT))
                            {
                                invertRoamingDir();
                            }
                            else
                            {
                                invertRoamingDir();
                                switchToStand(btrue);
                            }

                        }
                        else
                        {
                            invertRoamingDir();
                            switchToStand(btrue);
                        }
                    }
                    else
                    {
                        if (m_timeInCurrentState>1  && isBlocked)
                        {
                            if (m_walking_framesSinceChangedDirection>(0.2f/LOGICDT))
                            {
                                invertRoamingDir();
                            }
                        }
                    }
                }

            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIElectoonBehavior::isInWater()
    {
        PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
        return waterPoly!=NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIElectoonBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeInCurrentState += _deltaTime;
        m_watchdogTimer += _deltaTime;
        m_walking_framesSinceChangedDirection++;

        if (m_anElectoonStartedSignal)
        {
            m_deltaTimeBeforeYeah -= _deltaTime;
        }

        bbool signaledMustGoAway = (m_deltaTimeBeforeYeah<0);

        //anim inputs
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY,461177981), m_physComponent->getSpeed().m_y);

        //states
        switch (m_state)
        {
        default:
            ITF_ASSERT(0);
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_NA:
            m_watchdogTimer = 0;
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreAppear:
            {
                setState(State_Appear);
                m_watchdogTimer = 0;
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Appear:
            {
                m_roamingDir = (Seeder::getSharedSeeder().GetFloat(0,1)>0.5f)?Roam_Left:Roam_Right;
                if (m_watchdogTimer>ELECTOON_WATCHDOG_TIMER)
                {
                    setState(State_Yeah);
                }
                else if (isInWater() && !getTemplate()->getMustStay())
                {
                    setState(State_PreDisappears);
                }
                else if (m_physComponent->getStickedPolyline())
                {
                    switchToStand(btrue);
                }

                break;
            }
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Stand:
            {
                if ( !getTemplate()->getMustStay() &&
                   ( m_watchdogTimer>ELECTOON_WATCHDOG_TIMER ||  signaledMustGoAway ) )
                {
                    setState(State_Yeah);
                }
                else if (isInWater() && !getTemplate()->getMustStay())
                {
                    setState(State_PreDisappears);
                }
                else if (m_aiComponent->getHealth()<=0)
                {
                    setState(State_PreDisappears);
                }
                else if(m_timeInCurrentState>m_timeBeforeWalking)
                {
                    setState(State_Walking);
                }/*
                else if (m_animatedComponent->isSubAnimFinished())
                {
                    switchToStand(); //Reevaluate stand type
                }*/
                break;
            }
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreYeah_Signal:
            setState(State_Yeah);
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Yeah:

            if (m_animatedComponent->isMainNodeFinished())
            {
                setState(State_PreDisappears);
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreDisappears:
            {
                if (m_animatedComponent->isMainNodeFinished())
                {
                    setState(State_Disappearing);
                }
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Disappearing:
            {
                Vec3d delta(0, ELECTOON_DISAPPEAR_SPEED*m_timeInCurrentState,0);
                f32 t = m_timeInCurrentState*5;
                delta += m_disappearFastSinMult*f32_Sin(t*m_disappearFastSinFactor)+f32_Sin(t*m_disappearSinFactor);                
                m_actor->forceMove(m_posWhenstartDisappearing+delta);                    
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Walking:
            {
                updateWalking(_deltaTime, signaledMustGoAway);
            } break;
        }


        m_justBlockedByWall = bfalse;

    }

    void Ray_AIElectoonBehavior::safeStopFXController(u32 &_fxHandle, FXControllerComponent *_controller)
    {
        if (_fxHandle!=U32_INVALID)
        {
            _controller->stopFX(_fxHandle);
            _fxHandle = U32_INVALID;
        }
    }


    void Ray_AIElectoonBehavior::enableTrail(bbool _enable)
    {
        if (!m_actor)
            return;

        SafeArray<Trail3DComponent*> trails = m_actor->GetComponents<Trail3DComponent>();
        FXControllerComponent* controller = m_actor->GetComponent<FXControllerComponent>();

        for (u32 i=0; i<trails.size(); i++)
        {
            Trail3DComponent *trail = trails[i];
            if (_enable)
                trail->unpause();
            else
                trail->pause();
        }
        const StringID fx1("star");
        const StringID fx2("star_small");
        if (_enable)
        {
            if (m_fx1Handle==U32_INVALID)
            {
                m_fx1Handle = controller->playFX(fx1);
            }
            if (m_fx2Handle==U32_INVALID)
            {
                m_fx2Handle = controller->playFX(fx2);
            }
            m_animatedComponent->pause();
        }
        else
        {
            safeStopFXController(m_fx1Handle, controller);
            safeStopFXController(m_fx2Handle, controller);
            m_animatedComponent->unpause();
        }
    }

    void Ray_AIElectoonBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (EventSetTarget *setTarget = _event->DynamicCast<EventSetTarget>(ITF_GET_STRINGID_CRC(EventSetTarget,4067373371)))
        {
            m_ejectionTargetObj = setTarget->getTarget();
            Actor *obj = IRTTIObject::SafeDynamicCast<Actor>(m_ejectionTargetObj.getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
            if (obj)
            {
                m_ejectionTargetPos = obj->getPos();
                f32 minRange = 1.5f;
                f32 maxRange = 2.5f;
                Ray_ElectoonTargetComponent *targetComponent = obj->GetComponent<Ray_ElectoonTargetComponent>();
                if (targetComponent)
                {
                    minRange = targetComponent->getMinRange();
                    maxRange = targetComponent->getMaxRange();
                }

                f32 deltaEjectionX = Seeder::getSharedSeeder().GetFloat(minRange,maxRange);

                //y = y0-0.5.g.t�+v0y.t
                //x = x0+v0x.t
                //v0y = k.v0x

                f32 ejectionSpeedX = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinEjectionSpeed(),getTemplate()->getMaxEjectionSpeed());
                f32 dx = m_ejectionTargetPos.m_x - m_actor->getPos().m_x;

                dx = Ray_ElectoonTargetComponent::getDeltaXFromCageToTarget(dx, deltaEjectionX);

                if (dx<0)
                {
                    ejectionSpeedX = -ejectionSpeedX;
                }
                f32 dy = m_ejectionTargetPos.m_y - m_actor->getPos().m_y;
                f32 gravity = -(PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth()).m_y);
                f32 k = (dy+0.5f*gravity*dx*dx/(ejectionSpeedX*ejectionSpeedX))/dx;
                f32 ejectionSpeedY = ejectionSpeedX*k;
                setState(State_Appear);
                ////////////////////////PATCH
                m_physComponent->setGravityMultiplier(1.f);
                m_physComponent->setSpeed(Vec2d(ejectionSpeedX, ejectionSpeedY));

            }
            else
            {
                m_ejectionTargetObj.invalidate();
            }
        }
        else if (EventBlockedByPolyline *blockedByPolyLine = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)))
        {
            BaseObject* obj = GETOBJECT(blockedByPolyLine->getPolyline());
            if ( obj )
            {
                const PolyLine* poly = static_cast<PolyLine*>(obj);
                const PolyLineEdge* edge = &poly->getEdgeAt(blockedByPolyLine->getEdge());

                if (m_physComponent->getEdgeOrientationType(edge->m_normalizedVector)==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL)
                {
                    m_justBlockedByWall = btrue;
                }
            }
        }
        else if (Ray_EventSpawnRewardLum *spawnEvt = _event->DynamicCast<Ray_EventSpawnRewardLum>(ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722)))
        {
            Ray_EventSpawnRewardLum evt(*spawnEvt);
            evt.setEjectionDir(Vec2d(0,1));
            evt.setup(m_actor);
            RAY_GAMEMANAGER->onEvent(&evt);
        }
        else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            if (hit->getFaction()==RAY_FACTION_MEGAENEMY) //hardcoded reaction to avoid last minute bugs
            {
                m_aiComponent->setHealth(0);
            }
        }
    }

    void Ray_AIElectoonBehavior::setState(State_t _state)
    {
        m_walking_blockedCounter = 0;
        m_justBlockedByWall = bfalse;

        switch(m_state)
        {
        case State_Appear:
        case State_PreAppear:
        case State_NA:
            m_totalTimeWalkingSinceAppeared = 0;
            break;
        }
        if (_state!=m_state)
        {
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        bbool canDisplay = btrue;

        enableTrail(_state==State_Disappearing);

        m_physComponent->setGravityMultiplier(1.f);
        m_physComponent->setWeightMultiplier(1.f);

        switch(_state)
        {
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreAppear:
            {
                m_watchdogTimer = 0;
                canDisplay = bfalse;
                setNullAction();
                m_physComponent->setSpeed(Vec2d(0,0));
                if (m_randomForStandAnim&1)
                {
                    m_animatedComponent->setAnim(ELECTOON_EJECTION2_ANIM);			
                }
                else
                {
                    m_animatedComponent->setAnim(ELECTOON_EJECTION1_ANIM);			
                }
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Appear:
            {
                m_anElectoonStartedSignal = bfalse;
                m_watchdogTimer = 0;
                setNullAction();
                m_physComponent->setSpeed(Vec2d(0,0));
                m_deltaTimeBeforeYeah = Seeder::getSharedSeeder().GetFloat(0.f, 1.f);
                f32 ejectionAngle = Seeder::getSharedSeeder().GetFloat(ELECTOON_MIN_EJECTION_ANGLE,ELECTOON_MAX_EJECTION_ANGLE);
                f32 ejectionSpeed = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinEjectionSpeed(),getTemplate()->getMaxEjectionSpeed());

                EventTrail trail(btrue);
                m_trailComponent->onEvent(&trail);

                if (m_randomForStandAnim&1)
                {
                    m_animatedComponent->setAnim(ELECTOON_EJECTION2_ANIM);			
                }
                else
                {
                    m_animatedComponent->setAnim(ELECTOON_EJECTION1_ANIM);			
                }
                Vec2d speed;
                f32_CosSin(ejectionAngle, &speed.m_x, &speed.m_y);
                speed*=ejectionSpeed;
                m_actor->setFlipped(speed.m_x<0);
                if (getTemplate()->getMustStay())
                {
                    speed.m_y = 0;
                }
                m_physComponent->setSpeed(speed);
                m_physComponent->unstick();
                m_physComponent->setDisabled(bfalse);
                m_physComponent->setGravityMultiplier(0.2f);
                m_physComponent->setWeightMultiplier(0.1f);
                break;
            }
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Stand:
            setNullAction();
            m_physComponent->setSpeed(Vec2d(0,0));
            m_physComponent->resetTorque();
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Walking:
            setAction(m_walkAction);
            m_actor->setFlipped(m_roamingDir==Roam_Left);
            m_walkAction->setWalkLeft(m_roamingDir==Roam_Left);
            setIdealSpeed(0);
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreYeah_Signal:
            m_anElectoonStartedSignal = btrue;
            break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Yeah:
            {
                setNullAction();
                m_physComponent->setSpeed(Vec2d(0,0));

                const StringID electoon_Yeah[]=
                {
                    ELECTOON_YEAH1_ANIM,
                    ELECTOON_YEAH2_ANIM
                };
                u32 r = Seeder::getSharedSeeder().GetU32(0, ITF_ARRAY_SIZE(electoon_Yeah)-1);
                m_animatedComponent->setAnim(electoon_Yeah[r]);
            } break;

        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreDisappears:
            {
                setNullAction();
                m_physComponent->setSpeed(Vec2d(0,0));
                m_animatedComponent->setAnim(ELECTOON_PREDISAPPEARS_ANIM);
            } break;
        ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Disappearing:
            {
                m_posWhenstartDisappearing = m_actor->getPos();                

                setNullAction();
                m_physComponent->setSpeed(Vec2d(0,0));
                EventTrail trail(btrue);
                m_trailComponent->onEvent(&trail);

                m_physComponent->setDisabled(btrue);
                m_animatedComponent->setAnim(ELECTOON_DISAPPEARS_ANIM);
            } break;

        }

        m_animatedComponent->enableDraw(canDisplay);
    }

    void Ray_AIElectoonBehavior::onActorBecomeActive()
    {
        if (m_state==State_NA)
            setState(State_PreAppear);
    }

    void Ray_AIElectoonBehavior::onActorBecomeInactive()
    {
        setState(State_NA);

        if ( !getTemplate()->getMustStay() )
        {
            m_actor->requestDestruction();
        }
    }
}
