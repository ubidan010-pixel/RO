#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILUMS2BEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AILums2Behavior.h"
#endif //_ITF_RAY_AILUMS2BEHAVIOR_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LumMusicManagerAIComponent.h"
#endif //_ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_TRAIL3DCOMPONENT_H_
#include "gameplay/components/misc/Trail3DComponent.h"
#endif //_ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_RAY_AILUMS2COMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AILums2Component.h"
#endif //_ITF_RAY_AILUMS2COMPONENT_H_

#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#include "rayman/gameplay/Components/Lums/Ray_ScoreLumAIComponent.h"
#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

//#define _ACCROBATIC_PICKING
//#define _COMPANION_RED_LUM

#define _OPTIMIZE_ANIMATIONS_BY_VISIBILITY

#if defined(ITF_WII)
    #define _USE_ANIMLIGHT_COMPONENT
#endif


#ifdef _USE_ANIMLIGHT_COMPONENT
    #define ANIMATIONCOMPONENT_TYPE AnimLightComponent
    #define ANIMATIONCOMPONENT m_animComponent
#else
    #define ANIMATIONCOMPONENT_TYPE AnimatedComponent
    #define ANIMATIONCOMPONENT m_animatedComponent
#endif

///////////////////////////////////////////////////////////////////////////////////////////
#define LUMS2_STAND_ANIM		        "Stand"
#define LUMS2_FLYING_NORMAL_ANIM        "Fly_Normal"
#define LUMS2_FLYING_ACCROB_ANIM        "Fly_Accrobatic"
#define LUMS2_BEINGTAKEN_NORMAL_ANIM    "Picked_Normal"
#define LUMS2_BEINGTAKEN_ACCROB_ANIM    "Picked_Accrobatic"

#ifdef _COMPANION_RED_LUM
    #define LUMS2_COMPANION_ANIM            "Picked_Doubled"    
#else
#define LUMS2_COMPANION_ANIM            "Fly_Accrobatic"
#endif

#define LUMS2_COMPANION_PATH            GETPATH_ALIAS("basiclumscompanion")


#define DECLARE_COLORED_ANIM_ID(animName, stringWithoutColorPrefix) \
    static StringID animName##_Yellow = "Yellow_" stringWithoutColorPrefix;\
    static StringID animName##_Red = "Red_" stringWithoutColorPrefix;

#define GET_ANIM_ID(animName) selectFromColor( m_color,(animName##_Yellow),(animName##_Red) )

#define CHANGE_ANIM(animName) \
    {\
        const StringID &newAnim = GET_ANIM_ID(animName);\
        if (m_currentAnim != newAnim.GetValue())\
        {\
            m_animComponent->setAnim(newAnim);\
            m_currentAnim = newAnim.GetValue();\
        }\
    }

namespace ITF
{
    #define lumPickedSoundID_YellowNormal ITF_GET_STRINGID_CRC(MRK_Picked_lvl1,2887801407)
    #define lumPickedSoundID_YellowAccrobatic ITF_GET_STRINGID_CRC(MRK_Picked_lvl2,1874050816)
    #define lumPickedSoundID_RedNormal ITF_GET_STRINGID_CRC(MRK_Picked_red_lvl1,1349344919)
    #define lumPickedSoundID_RedAccrobatic ITF_GET_STRINGID_CRC(MRK_Picked_red_lvl2,1013504367)

    #define beingTakenStopMarker = ITF_GET_STRINGID_CRC(MRK_Picked_GameplayStop,2458198257)

    SafeArray<Ray_AILums2Behavior::LumTrailDesc,Ray_AILums2Behavior::MAX_LUM_TRAILS+1> Ray_AILums2Behavior::m_lumTrailList;


    DECLARE_COLORED_ANIM_ID(lums2CompanionAnim, LUMS2_COMPANION_ANIM)
    DECLARE_COLORED_ANIM_ID(lums2StandAnim, LUMS2_STAND_ANIM)
    DECLARE_COLORED_ANIM_ID(lums2FlyingNormalAnim, LUMS2_FLYING_NORMAL_ANIM)
    DECLARE_COLORED_ANIM_ID(lums2FlyingAccrobAnim, LUMS2_FLYING_ACCROB_ANIM)
    DECLARE_COLORED_ANIM_ID(lums2BeingTakenNormalAnim, LUMS2_BEINGTAKEN_NORMAL_ANIM)
    //DECLARE_COLORED_ANIM_ID(lums2BeingTakenAccrobAnim, LUMS2_BEINGTAKEN_ACCROB_ANIM)


    static ITF_INLINE const StringID & selectFromColor(Ray_AIScoreLumBehavior::Color_t _color, const StringID &_yellow, const StringID &_red)
    {
        if (_color == Ray_AIScoreLumBehavior::Color_Yellow)
            return _yellow;
        else
            return _red;
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AILums2Behavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AILums2Behavior_Template)
        SERIALIZE_MEMBER("takenTrajectoryFactorX", m_takenTrajectoryFactorX);
        SERIALIZE_MEMBER("takenTrajectoryFactorY", m_takenTrajectoryFactorY);
        SERIALIZE_MEMBER("percentTimeStartFading", m_percentTimeStartFading);
        SERIALIZE_MEMBER("alphaWhenReachedScore", m_alphaWhenReachedScore);
        SERIALIZE_MEMBER("waitDurationPerRankWhenAutoPicked", m_waitDurationPerRankWhenAutoPicked);
        SERIALIZE_MEMBER("isKing", m_isKing);
        SERIALIZE_MEMBER("startKingComboFXLevel1", m_startKingComboFXLevel1);
        SERIALIZE_MEMBER("startKingComboFXLevel2", m_startKingComboFXLevel2);
        SERIALIZE_MEMBER("lumTrailACT", m_lumTrailACT);
        SERIALIZE_MEMBER("trailWidth", m_trailWidth);
        SERIALIZE_MEMBER("redCompanionLumDelay", m_redCompanionLumDelay);
        SERIALIZE_MEMBER("circularRotationSpeed", m_circularRotationSpeed);
        SERIALIZE_MEMBER("lumKingMusicVolume", m_lumKingMusicVolume);
        SERIALIZE_MEMBER("extraDuration", m_extraDuration);
        SERIALIZE_MEMBER("pickingAnimDuration", m_pickingAnimDuration);
        SERIALIZE_OBJECT("particleLumAABB", m_particleLumAABB);
        SERIALIZE_MEMBER("scaleWhenReachedScore", m_scaleWhenReachedScore);
        SERIALIZE_MEMBER("percentTimeStartScaling", m_percentTimeStartScaling);
        SERIALIZE_MEMBER("playerDetectorMultiplierInWater", m_playerDetectorMultiplierInWater);
    END_SERIALIZATION()

    Ray_AILums2Behavior_Template::Ray_AILums2Behavior_Template()
    : m_takenTrajectoryFactorX(0.5f)
    , m_takenTrajectoryFactorY(0.2f)
    , m_percentTimeStartFading(0.8f)
    , m_waitDurationPerRankWhenAutoPicked(1.f)
    , m_isKing(bfalse)
    , m_companionLumACT(LUMS2_COMPANION_PATH)
    , m_trailWidth(1.f)
    , m_alphaWhenReachedScore(0.2f)
    , m_redCompanionLumDelay(0.1f)
    , m_circularRotationSpeed(10.f)
    , m_lumKingMusicVolume(1.0f,bfalse)
    , m_extraDuration(0.3f)
    , m_pickingAnimDuration(0.25f)
    , m_particleLumAABB(Vec2d(-0.25f,0), Vec2d(0.25f,1-0.655f))
    , m_scaleWhenReachedScore(0.5f)
    , m_percentTimeStartScaling(0.5f)
    , m_playerDetectorMultiplierInWater(1.5f)
    {
    }

    Ray_AILums2Behavior_Template::~Ray_AILums2Behavior_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AILums2Behavior)
    BEGIN_SERIALIZATION(Ray_AILums2Behavior)
    END_SERIALIZATION()

    BEGIN_VALIDATE_BEHAVIOR(Ray_AILums2Behavior)
    VALIDATE_BEHAVIOR_PARAM("", m_triggerComponent!=NULL, "There must be a TriggerComponent for detecting picking");
    //ITF_WARNING(m_actor, !m_actor->getParentBind(), "This actor shouldn't be bound because it's moving by itself in the world");
    END_VALIDATE_BEHAVIOR()

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AILums2Behavior::isTaken()
    {
        Ray_AILums2Component *lumComponent = IRTTIObject::SafeDynamicCast<Ray_AILums2Component>(m_aiComponent,ITF_GET_STRINGID_CRC(Ray_AILums2Component,82903916));
        if (lumComponent)
        {
            return lumComponent->isTaken();
        }
        return m_isTaken;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::setIsTaken(bbool _isTaken)
    {
        Ray_AILums2Component *lumComponent = IRTTIObject::SafeDynamicCast<Ray_AILums2Component>(m_aiComponent,ITF_GET_STRINGID_CRC(Ray_AILums2Component,82903916));
        if (lumComponent)
        {
            return lumComponent->setIsTaken(_isTaken);
        }
        m_isTaken = _isTaken;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AILums2Behavior::LumTrailDesc *Ray_AILums2Behavior::getNewTrail()
    {
        if (m_lumTrailGenerator.isInitialized())
        {           
            Actor *act = m_lumTrailGenerator.getSpawnee(m_actor->getScene(),m_actor->getPos());
            if (act)
            {
                
                LumTrailDesc desc;
                desc.m_actor = act->getRef();
                desc.m_alpha = 1.f;
                m_lumTrailList.push_back(desc);
                return &m_lumTrailList.back();
            }
            return NULL;
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::removeAndInvalidateCurrentLumTrail()
    {
        /*
        if (m_currentLumTrail.isValid())
        {
            for (i32 i=0; i<(i32)m_lumTrailList.size(); i++)
            {
                LumTrailDesc &desc = m_lumTrailList[i];
                if (desc.m_actor==m_currentLumTrail)
                {
                    Actor *act = m_lumTrailList[i].m_actor.getActor();
                    if (act)
                    {
                        act->requestDestruction();
                    }
                    m_lumTrailList.eraseNoOrder(i);
                    break;
                }
            }
            m_currentLumTrail.invalidate();
        }
        */
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::updateLumTrailList()
    {
        if (m_lastFrameTrailListWasUpdated != CURRENTFRAME)
        {
            ITF_ASSERT(MAX_LUM_TRAILS>MAX_FULLY_INTACT_LUM_TRAILS);
            //1st, remove deleted lum trails from list
            for (i32 i=0; i<(i32)m_lumTrailList.size(); i++)
            {
                const ActorRef &r = m_lumTrailList[i].m_actor;
                i32 minLumTrailIndex = (i32)m_lumTrailList.size()-(i32)MAX_LUM_TRAILS;
                if (!r.getActor() || (i<minLumTrailIndex) )
                {
                    Actor *act = m_lumTrailList[i].m_actor.getActor();
                    if (act)
                    {
                        act->requestDestruction();
                    }

                    m_lumTrailList.eraseKeepOrder(i);
                    i--;
                }
            }
            //we decimate (by decreasing their alpha) old lums
            i32 minUndecimated = (i32)m_lumTrailList.size()-(i32)MAX_FULLY_INTACT_LUM_TRAILS;
            for (i32 i=0; i<minUndecimated; i++)
            {
                m_lumTrailList[i].m_alpha *= 0.99f;        
            }
            //lums that started being decimated continue being decimated
            for (i32 i=0; i<(i32)m_lumTrailList.size(); i++)
            {
                LumTrailDesc &desc = m_lumTrailList[i];
                if (desc.m_alpha<1)
                {
                    desc.m_alpha *= 0.9f;
                }
            }
            m_lastFrameTrailListWasUpdated = CURRENTFRAME;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AILums2Behavior::Ray_AILums2Behavior()
        : Super()
        , m_state(State_NA)
        , m_triggerComponent(NULL)
        , m_isAcrobatic(bfalse)
        , m_color(Ray_AIScoreLumBehavior::Color_Yellow)
        , m_isTaken(bfalse)
        , m_isAutoPicked(bfalse)
        , m_currentAnim(StringID::InvalidId)
        , m_ejectionDurationWhenSpawned(0)
        , m_lastFrameTrailListWasUpdated(U32_INVALID)
        , m_addedLumValue(0)
        , m_addedLumPlayerIndex(0)
        , m_timeInCurrentState(0)
        , m_usePlayerSpeed(btrue)
        , m_waitMovement(WaitMovement_Stands)
        , m_destinationScorePos(0,0)
        , m_usePickingAnim(bfalse)
        , m_lumCount(1)
        , m_forceYellow(bfalse)
        , m_waterCheckOffset(0)
        , m_playerDetectorComponent(NULL)
   {
       static u32 serialNumber = 0;
       m_waterCheckOffset = serialNumber++;
   }

    Ray_AILums2Behavior::~Ray_AILums2Behavior()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent );
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), m_aiComponent);
#endif

        removeAndInvalidateCurrentLumTrail();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::updateSubAnimPhase()
    {
        if (m_actor->isPhysicalReady())
        {
            SubAnimSet *animSet = ANIMATIONCOMPONENT->getSubAnimSet();
            ITF_ASSERT(animSet);
            static const f32 spacing = 0.25f;
            static const u32 noteCount = 8;
            static const f32 oneOverNoteCount = 1.f / 8.f;
            f32 pos = f32_Abs(100000.f+m_actor->get2DPos().m_x)+f32_Abs(100000.f+m_actor->get2DPos().m_y);
            u32 phase = ((u32)(pos/spacing)) % noteCount;

            for (u32 index=0; index<m_subAnimsToPatch.size(); index++)
            {                
                SubAnim *subanim = animSet->getSubAnim(m_subAnimsToPatch[index]);
                ITF_ASSERT(subanim);
                subanim->setSyncOffset(phase * oneOverNoteCount);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

#if defined(_ACCROBATIC_PICKING) || defined(_COMPANION_RED_LUM)
        SPAWNER->declareNeedsSpawnee(m_actor, &m_companionLumGenerator, getTemplate()->getCompanionLumACT());
#endif
        if (!getTemplate()->getLumTrailACT().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_lumTrailGenerator, getTemplate()->getLumTrailACT());
        }

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);

        m_triggerComponent = m_actor->GetComponent<TriggerComponent>();
        m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventLumEjection,368805757), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225), m_aiComponent);
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),m_aiComponent);
#endif
        m_aiComponent->setFaction(RAY_FACTION_NEUTRAL);

        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
        resetState();
    }

    void Ray_AILums2Behavior::onResourceReady()
    {
        Super::onResourceReady();
        if ( getTemplate()->getIsKing() )
        {
            ANIMATIONCOMPONENT->setBankState(BankState_King);
        }
        else
        {
            //Replacing generic autophase by phase based on position
            m_subAnimsToPatch.clear();
            SubAnimSet *animSet = ANIMATIONCOMPONENT->getSubAnimSet();
            ITF_ASSERT(animSet);
            u32 animCount = animSet->getSubAnimCount();
            for (u32 animIndex=0; animIndex<animCount; animIndex++)
            {
                SubAnim *subanim = animSet->getSubAnim(animIndex);
                ITF_ASSERT(subanim);
                if (subanim->getIsSync() && (subanim->getSyncEigthNote()==8)/*special value for auto phase*/)
                {
                    m_subAnimsToPatch.push_back(animIndex);
                }
            }
            updateSubAnimPhase();
        }

    }

    void Ray_AILums2Behavior::clear() 
    {
        m_triggerComponent = NULL;
        m_isAcrobatic = bfalse;
    }

    void Ray_AILums2Behavior::resetState()
    {
        m_actor->setPos( m_actor->getWorldInitialPos() + Vec3d( 0.0f, 0.0f, m_actor->getTemplate()->getForcedZ() ) );
        m_currentAnim = StringID::InvalidId;
        m_isAcrobatic = bfalse;
        m_actorWhoPicked.invalidate();
        m_forceYellow = bfalse;
        fetchColorFromScore();
        m_isAutoPicked = bfalse;
        m_actorWhoPicksAfterEjection.invalidate();
        m_usePlayerSpeed = bfalse;
        m_lumCount = 1;
        
        removeAndInvalidateCurrentLumTrail();            

        freeCompanion();
        if (isTaken())
            setState(State_Taken);
        else
        {
            TweenComponent *tween = m_actor->GetComponent<TweenComponent>();
            if (tween)
            {
                tween->unpause();
            }

            setState(State_Stands);        
        }
        updatePlayerDetectorRadius();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AILums2Behavior::fetchTrajectoryParameters(Actor *_player, bbool _usePickingAnim)
    {
        if (!_player)
            return bfalse;

        m_usePickingAnim = _usePickingAnim;
        m_destinationScorePos = Vec2d::Zero;
        Vec3d playerPos_WhenPicked = _player->getPos();
        sendAddLumToScoreLum(_player->getRef());
        if (RAY_GAMEMANAGER->isPlayerActor(_player->getRef()))
        {
            Player *playerInfo = GAMEMANAGER->getPlayerFromActor(_player->getRef());
            ITF_ASSERT(playerInfo);
            if (playerInfo)
            {
                m_destinationScorePos = Ray_PlayerHudScoreComponent::getScorePosFromPlayerIndex(playerInfo->getIndex());
            }
        }


        AABB scaledParticleLumAABB = getTemplate()->getParticleLumAABB();
        f32 scale = m_actor->getScale().m_x;
        scaledParticleLumAABB.setMin(scaledParticleLumAABB.getMin()*scale);
        scaledParticleLumAABB.setMax(scaledParticleLumAABB.getMax()*scale);
        Ray_PickedLumTrajectoryFollower::CircularMotionDesc *circularMotionDesc = NULL;
        if (m_waitMovement==WaitMovement_Circular)
            circularMotionDesc = &m_circularMotionDesc;
        m_pickingTrajectoryFollower.initializeParameters(m_destinationScorePos, 
            _player->getRef(), 
            getTemplate()->getTakenTrajectoryFactorX(), getTemplate()->getTakenTrajectoryFactorY(),
            m_usePlayerSpeed,
            playerPos_WhenPicked,
            getTemplate()->getPercentTimeStartFading(),
            scaledParticleLumAABB,
            m_addedLumValue, m_addedLumPlayerIndex,
            getTemplate()->getAlphaWhenReachedScore(),
            circularMotionDesc,
            m_actor->getPos(),
            getTemplate()->getScaleWhenReachedScore(),
            getTemplate()->getPercentTimeStartScaling());

        return btrue;
        
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::freeCompanion()
    {
        if (m_companion.isValid())
        {
            Actor *act = m_companion.getActor();
            if (act)
            {
                act->requestDestruction();
            }
            m_companion.invalidate();
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_AILums2Behavior::moveOnTrajectory()
    {
        Vec3d projPos, projSpeed;
        Vec3d newPos;
        bbool isWaitingBeforeStart=bfalse;
        m_pickingTrajectoryFollower.computeFlightTrajectory(bfalse, m_actor->getPos());
        f32 alpha;
        bbool justReachedAlphaThreshold;
        f32 dummyScale=1;
        f32 lambda = m_pickingTrajectoryFollower.computePosOnTrajectory(projPos, projSpeed,isWaitingBeforeStart,0, alpha, justReachedAlphaThreshold,dummyScale);
        f32 completion = lambda;

        m_actor->setFlipped(bfalse);//projSpeed.m_x>=0);
        GFX_ADAPTER->compute2DTo3D(projPos, newPos);
        m_actor->forceMove(newPos);

#ifdef _COMPANION_RED_LUM
        Actor *companion = m_companion.getActor();
        if (companion)
        {
            Vec3d companionProjPos, companionProjSpeed, newCompanionPos;
            bbool dummyBool;

            f32 companionLambda = computePosOnTrajectory(companionProjPos, companionProjSpeed,dummyBool,getTemplate()->getCompanionLumDelay());
            completion = std::min(completion, companionLambda);
            GFX_ADAPTER->compute2DTo3D(companionProjPos, newCompanionPos);
            companion->forceMove(newCompanionPos);
            companion->setScale(m_actor->getScale());
            ANIMATIONCOMPONENT_TYPE *anim = companion->GetComponent<ANIMATIONCOMPONENT_TYPE>();
            companion->setFlipped(m_actor->isFlipped());
            anim->setColor(ANIMATIONCOMPONENT->getColor());
            ITF_ASSERT(anim);
        }
#endif
        ANIMATIONCOMPONENT->setColor(Color(alpha,1,1,1));

        if (!isWaitingBeforeStart && !m_currentLumTrail.isValid())
        {
            /*
            LumTrailDesc *desc = getNewTrail();
            if (desc)
            {
                m_currentLumTrail = desc->m_actor;
            }
            else
            {
                m_currentLumTrail.invalidate();
            }
            */

        }            

        /*
        Actor *trailActor = m_currentLumTrail.getActor();
        if (trailActor)
        {
            //Trail3DComponent *trailComponent = trailActor->GetComponent<Trail3DComponent>();
            Trail3DComponent *trail3d = trailActor->GetComponent<Trail3DComponent>();
            ITF_ASSERT(trail3d);
            if (trail3d)
            {
                ITF_VECTOR<Trail> &trailList = trail3d->getTrailList();
                trail3d->setAlpha(ANIMATIONCOMPONENT->getColor().m_a);
                u32 trailCount = trailList.size();
                for (u32 i=0; i<trailCount; i++)
                {
                    Trail &trail = trailList[i];
                    Vec3d itemPos = m_actor->getPos();
                    Vec3d itemTop = itemPos+Vec3d(0,getTemplate()->getTrailWidth()*m_actor->getScale().m_y,0);
                    Vec3d itemPosProj, itemTopProj;
                    GFX_ADAPTER->compute3DTo2D(itemPos, itemPosProj);
                    GFX_ADAPTER->compute3DTo2D(itemTop, itemTopProj);
                    f32 length = ((Vec2d&)itemTopProj-(Vec2d&)itemPosProj).norm();
                    trail.setThicknessScale(length/(f32)GFX_ADAPTER->getScreenHeight());
                }
            }
            trailActor->setScale(Vec2d(100,100));
            trailActor->setPos(projPos);
        }
        */

        return completion;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::switchToParticle_FlyingToScore()
    {
        //switch to particle
        Ray_ScoreLumAIComponent *scoreLum = Ray_AIUtils::fetchScoreLumComponent();
        if (scoreLum)
        {
            ANIMATIONCOMPONENT->enableDraw(bfalse);

            for (u32 i=0; i<m_lumCount; i++)
            {
                Ray_PickedLumTrajectoryFollower &lum = scoreLum->pushLumTrajectoryFollower(m_pickingTrajectoryFollower);
                lum.setFlightDelay(getTemplate()->getCompanionLumDelay()*i);
            }
            setState(State_FlyingToScore);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::updatePlayerDetectorRadius()
    {
        if (m_playerDetectorComponent)
        {
            PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
            m_playerDetectorComponent->setScaleMultiplier(waterPoly?getTemplate()->getPlayerDetectorMultiplierInWater():1.f);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeInCurrentState += _deltaTime;
        m_pickingTrajectoryFollower.incrementTime(_deltaTime);

#ifdef _OPTIMIZE_ANIMATIONS_BY_VISIBILITY
        bbool enableAnimation = btrue;
        if (m_state==State_Stands)
        {
            AABB aabbWithMargin(m_actor->getAABB());
            if (aabbWithMargin.isValid())
            {
                f32 margin = std::max(f32_Abs(m_actor->getScale().m_x), 1.f);
                Vec2d margin2d(margin,margin);
                aabbWithMargin.setMin(aabbWithMargin.getMin()-margin2d);
                aabbWithMargin.setMax(aabbWithMargin.getMax()+margin2d);
                if (!CAMERA->isRectVisible(aabbWithMargin, m_actor->getDepth()))
                {
                    enableAnimation = bfalse;
                }
            }
        }
        if (enableAnimation)
            ANIMATIONCOMPONENT->unpause();        
        else
            ANIMATIONCOMPONENT->pause();
#endif

        if (((CURRENTFRAME+m_waterCheckOffset) & 31)==0)
        {
            updatePlayerDetectorRadius();
        }
        updateLumTrailList();
        

        //Update the states. 
        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Stands:
            {
                m_actor->enable();
                fetchColorFromScore();
                CHANGE_ANIM(lums2StandAnim)
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_BeingTaken:
            {
                moveOnTrajectory();
#ifdef _ACCROBATIC_PICKING
                setCompanionLumFromMainLum();
#endif

                if (RAY_GAMEMANAGER->isPlayerActor(m_actorWhoPicked))
                {
                    Player *player = RAY_GAMEMANAGER->getPlayerFromActor(m_actorWhoPicked);
                    if (player)
                    {
                        u32 index = player->getIndex();
                        Ray_PlayerHudScoreComponent::setForcedDisplay(index);
                    }

                }

                if (m_pickingTrajectoryFollower.getTimeSinceTaken()>=getTemplate()->getPickingAnimDuration() || !m_usePickingAnim)
                {
                    switchToParticle_FlyingToScore();
                }

                //Color is no longer updated
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_PreTaken:
            {
                if (m_timeInCurrentState>=getTemplate()->getExtraDuration())
                {
                    setState(State_Taken);
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Taken:
            {
                //Color is no longer updated
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_FlyingToScore:
            {
                f32 lambda = moveOnTrajectory();
#ifdef _ACCROBATIC_PICKING
                setCompanionLumFromMainLum();
#endif
                //Color is no longer updated
                if (lambda>=1)
                {
                    setState(State_PreTaken);
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_BeingTaken_CircularMovement:
            {
                Ray_ScoreLumAIComponent *scoreLum = Ray_AIUtils::fetchScoreLumComponent();
                if (scoreLum)
                {
                    ANIMATIONCOMPONENT->enableDraw(bfalse);
                    for (u32 i=0; i<m_lumCount; i++)
                    {
                        Ray_PickedLumTrajectoryFollower &lum = scoreLum->pushLumTrajectoryFollower(m_pickingTrajectoryFollower);
                        lum.setCircularMotionDescRank(i);
                        f32 radiusMultiplier;
                        lum.setCircularMotionDescDuration(computeCircularMotionDuration(i, m_lumCount, radiusMultiplier));
                        lum.setCircularMotionRadius(m_circularMotionDesc.m_radius*radiusMultiplier);

                    }
                    setState(State_FlyingToScore);
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_BeingTaken_Ejection:
            {				
                f32 ejectionTime =m_timeInCurrentState;
                bbool finished=ejectionTime>m_ejectionDurationWhenSpawned;
                if (finished)
                {
                    ejectionTime = m_ejectionDurationWhenSpawned;
                }
                Vec2d pos = m_ejectionWhenSpawned_Quadratics[0]+
                    m_ejectionWhenSpawned_Quadratics[1]*ejectionTime+
                    m_ejectionWhenSpawned_Quadratics[2]*(ejectionTime*ejectionTime);
                m_actor->set2DPos(pos);				
                if (finished)
                {
                    m_pickingTrajectoryFollower.computeFlightTrajectory(btrue, m_actor->getPos());
                    switchToParticle_FlyingToScore();
                }

            } break;

        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AILums2Behavior::getIsPlayerInAccrobaticMode(ObjectRef _actorRef) const
    {
        Actor* actor = AIUtils::getActor(_actorRef);
        if (actor && RAY_GAMEMANAGER->isPlayerActor(_actorRef))
        {
            Ray_EventQueryIsAccrobatic query;
            actor->onEvent(&query);

            if (query.getValue())
            {
                return btrue;
            }
        }
        return bfalse;
            }



    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::processPicking_IfPossible(ObjectRef _activator, bbool _isAutoPicked, bbool _isAccrobatic, bbool _usePlayerSpeed, WaitMovement_t _waitMovement, u32 _lumCount)
    {
        Actor* actor = AIUtils::getActor(_activator);

        //override mode when king lum
        bbool lumKingCircleMode = getTemplate()->getIsKing();

        if (lumKingCircleMode)
        {
            Ray_AIScoreLumBehavior *scoreLum = Ray_AIUtils::fetchScoreLumBehavior();
            ITF_ASSERT_CRASH(scoreLum, "no score lum");
            f32 height = std::max(m_actor->getAABB().getHeight(),0.f);
            m_actor->setScale(m_actor->getLocalInitialScale()*0.5f);
            prepareAutoPickupCircle(m_actor->getPos(),scoreLum->getTemplate()->getLumKingValue(),height*0.5f, _activator);
            _waitMovement = WaitMovement_Circular;
        }

        //
        m_isAcrobatic = bfalse;
        m_usePlayerSpeed = _usePlayerSpeed;
        m_waitMovement = _waitMovement;
        m_lumCount = _lumCount;

        bbool isOk = actor && RAY_GAMEMANAGER->isPlayerActor(_activator);
        if (isOk)
        {
            bbool usePickingAnim = _waitMovement==WaitMovement_Stands;
            if (_waitMovement !=WaitMovement_Stands)
                m_usePlayerSpeed = bfalse;
            isOk = fetchTrajectoryParameters(actor, usePickingAnim);
        }

        if (isOk)
        {
            m_actorWhoPicked = _activator;
            m_isAutoPicked = _isAutoPicked;
#ifdef _ACCROBATIC_PICKING
            m_isAcrobatic = _isAccrobatic;
#else
            m_isAcrobatic = bfalse;
#endif
            switch(m_waitMovement)
            {
            case WaitMovement_Circular:
                setState(State_BeingTaken_CircularMovement);
                break;
            case WaitMovement_Ejection:
                setState(State_BeingTaken_Ejection);
                break;
            default:
                setState(State_BeingTaken);
                break;
            }
        }
        else
        {
            m_actorWhoPicked.invalidate();
            m_isAutoPicked = bfalse;
            m_isAcrobatic = bfalse;
        }

        //In some cases, player is no longer OK. So, we simply do nothing
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_AILums2Behavior::computeCircularMotionDuration(u32 _rank, u32 _maxRank, f32 &_radiusMultiplier)
    {
        _maxRank = std::max(_maxRank,1U);

        _radiusMultiplier = 1;
        u32 timeRank = _rank%5;
        if (_maxRank>5)
        {
            u32 radiusRank = _rank/5;
            u32 maxRadiusRank = _maxRank/5;
            _radiusMultiplier = (radiusRank+1)/(f32)maxRadiusRank;
        }
        f32 timingScale = 1.f;
        return (timeRank+1)*getTemplate()->getWaitDurationPerRankWhenAutoPicked()*timingScale;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::processAutoPickupCircle(Ray_EventRewardPickedUp *_onPickedUp)
    {
        prepareAutoPickupCircle(_onPickedUp->getPos(),_onPickedUp->getMaxRank(),_onPickedUp->getRadius(),_onPickedUp->getActivator());
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::prepareAutoPickupCircle(const Vec3d &_center, u32 _maxRank, f32 _radius, const ObjectRef &_sender)
    {
        m_circularMotionDesc.m_rotationCenter = _center;
        u32 maxRank = std::max(_maxRank,1U);
        f32 _radiusMultiplier = 1;
        m_circularMotionDesc.m_circularMotionDuration = computeCircularMotionDuration(0, maxRank, _radiusMultiplier);
        m_circularMotionDesc.m_circularMotionSpeed = getTemplate()->getCircularRotationSpeed();
        m_circularMotionDesc.m_radius = _radius; //we don't take radius multiplier in account : we'll take it when transformed to particle
        m_circularMotionDesc.m_maxRank = _maxRank;
        m_circularMotionDesc.m_rank = 0;
        m_actorWhoPicksAfterEjection = _sender;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::processLumEjectionEvent(Ray_EventLumEjection *_onEjection)
    {
        if (m_state==State_Stands)
        {
            //compute standard ejection parabola
            Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
            gravity = gravity.Rotate(_onEjection->getEjectionGravityAngle().ToRadians());
            Vec2d v0 = (gravity+_onEjection->getEjectionForce())*LOGICDT;
            m_ejectionWhenSpawned_Quadratics[2] = 0.5f*gravity;
            m_ejectionWhenSpawned_Quadratics[1] = v0;
            m_ejectionWhenSpawned_Quadratics[0] = m_actor->get2DPos();
            m_ejectionDurationWhenSpawned = _onEjection->getEjectionDuration();
            f32 endT = m_ejectionDurationWhenSpawned;
            Vec2d endPos2d = m_ejectionWhenSpawned_Quadratics[0]+
                m_ejectionWhenSpawned_Quadratics[1]*endT+
                m_ejectionWhenSpawned_Quadratics[2]*(endT*endT);
            Vec3d endPos(endPos2d.m_x,endPos2d.m_y,m_actor->getDepth());

            m_actor->setWorldInitialPos(endPos);

            processPicking_IfPossible(_onEjection->getSender(),_onEjection->getIsAutoPickup(),bfalse,bfalse, WaitMovement_Ejection, 1);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        /*
        if (AnimGameplayEvent *animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if ((animEvent->getName()==beingTakenStopMarker) && m_state==State_BeingTaken)
            {
                setState(State_FlyingToScore);
            }           
        }
        else */
        if (Ray_EventRewardPickedUp * onPickedUp = _event->DynamicCast<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225)))
        {
            m_forceYellow = onPickedUp->getForceYellowLum();
            if (m_state==State_Stands)
            {
                if (onPickedUp->isLumsAutoPickupCircle())
                {
                    processAutoPickupCircle(onPickedUp);
                    processPicking_IfPossible(onPickedUp->getActivator(), btrue, bfalse, btrue, WaitMovement_Circular,std::max(onPickedUp->getMaxRank(),1U));
                }
                else
                {
                    processPicking_IfPossible(onPickedUp->getActivator(), btrue, bfalse, btrue, WaitMovement_Stands,1);
                }               
            }
        }
        else if (Ray_EventLumEjection *onEjection = _event->DynamicCast<Ray_EventLumEjection>(ITF_GET_STRINGID_CRC(Ray_EventLumEjection,368805757)))
        {
            processLumEjectionEvent(onEjection);
        }
        else if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
        {
            resetState();
        }
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
#ifdef ITF_SUPPORT_EDITOR
            //NB : We do not count spawned objects, because it would count rewards twice
            if ((m_actor->getWorld() != SPAWNER->getSpawnerWorld()) && !m_actor->isSpawned())
            {
                if (getTemplate()->getIsKing())
                {
                    queryLumCount->addLumKings(1);
                }
                else
                {
                    queryLumCount->addNormalLums(1);
                }
            }
#endif
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::setCompanionLumFromMainLum()
    {
        Actor *companion = m_companion.getActor();
        if (companion)
        {
            companion->forceMove(m_actor->getPos());
            companion->setScale(m_actor->getScale());
            ANIMATIONCOMPONENT_TYPE *anim = companion->GetComponent<ANIMATIONCOMPONENT_TYPE>();
            companion->setFlipped(bfalse);//m_actor->isFlipped());
            anim->setColor(ANIMATIONCOMPONENT->getColor());
            ITF_ASSERT(anim);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::createCompanion()
    {
        Path companionPath(LUMS2_COMPANION_PATH);
        Actor *companion = m_companionLumGenerator.getSpawnee(m_actor->getScene(), m_actor->getPos());
        ITF_WARNING(m_actor, companion!=NULL, "companion actor %s not declared as spawnable", companionPath.getString8().cStr());
        if (companion)
        {
            m_companion = companion->getRef();
            ANIMATIONCOMPONENT_TYPE *anim = companion->GetComponent<ANIMATIONCOMPONENT_TYPE>();
            if (anim)
            {                
                anim->setAnim(GET_ANIM_ID(lums2CompanionAnim));
            }

            setCompanionLumFromMainLum();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::sendAddLumToScoreLum(const ActorRef &_picker)
    {
        Actor *actor = (Actor*)Ray_AIScoreLumBehavior::getInstance().getObject();
        m_addedLumValue = 0;
        m_addedLumPlayerIndex = 0;
        if (actor)
        {
            Player *player = RAY_GAMEMANAGER->getPlayerFromActor(_picker, bfalse);
            if (player)
            {
                Ray_EventAddLum addLumEvent;
                for (u32 i=0; i<m_lumCount; i++)
                {
                    addLumEvent.m_isKingLum = getTemplate()->getIsKing();
                    addLumEvent.m_isAccrobatic = m_isAcrobatic && !getTemplate()->getIsKing();
                    addLumEvent.m_forceYellow = m_forceYellow;
                    addLumEvent.setSender(m_actor->getRef());
                    addLumEvent.m_activator = _picker;
                    actor->onEvent(&addLumEvent);
                }
                m_addedLumValue = 1;
                m_lumCount *= addLumEvent.m_outputScore;
                m_addedLumPlayerIndex = player->getIndex();
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::startKingLumMusic(bbool _isAccrobatic)
    {
        Actor *musicManager = Ray_LumMusicManagerAIComponent::findMusicManager(RAY_GAMEMANAGER->getBaseMapWorld());

        ITF_WARNING(NULL, musicManager, "There must be 1 Ray_LumMusicManagerAIComponent in this map");

        if (musicManager)
        {
            Ray_LumMusicManagerAIComponent * component = musicManager->GetComponent<Ray_LumMusicManagerAIComponent>();
            if (component)
            {
                component->startKingLumMusic();
            }
        }
        //// stop previous
        //MUSICMANAGER->stop(METRONOME_TYPE_LUMKING,0.0f);
        //MUSICMANAGER->play(METRONOME_TYPE_LUMKING,lumkingMusic,0.0f,getTemplate()->getLumKingMusicVolume().ratio());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::playLumPickingMusic(bbool _isRedMode, bbool _isAccrobatic)
    {
        AABB invalidAABB;
        invalidAABB.invalidate();

        Actor *musicManager = Ray_LumMusicManagerAIComponent::findMusicManager(RAY_GAMEMANAGER->getBaseMapWorld());
            
        ITF_WARNING(NULL, musicManager, "There must be 1 Ray_LumMusicManagerAIComponent in this map");

        if (musicManager)
        {
            SoundComponent *soundComponent = musicManager->GetComponent<SoundComponent>();
            ITF_WARNING(musicManager, soundComponent!=NULL, "LumMusicManagerAIComponent : no sound component");
            if (soundComponent)
            {
                StringID soundID;
                if (_isRedMode)
                {
                    if (_isAccrobatic)
                    {
                        soundID = lumPickedSoundID_RedAccrobatic;
                    }
                    else
                    {
                        soundID = lumPickedSoundID_RedNormal;
                    }
                }
                else
                {
                    if (_isAccrobatic)
                    {
                        soundID = lumPickedSoundID_YellowAccrobatic;
                    }
                    else
                    {
                        soundID = lumPickedSoundID_YellowNormal;
                    }
                }

                soundComponent->playSound(soundID);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::initializeBeingTaken()
    {
        if (m_actor->getParentBind())
            m_actor->getParentBind()->setRuntimeDisabled(btrue);
        m_pickingTrajectoryFollower.computeFlightTrajectory(btrue, m_actor->getPos());
        setIsTaken(btrue);

        fetchColorFromScore();

        TweenComponent *tween = m_actor->GetComponent<TweenComponent>();
        if (tween)
        {
            tween->pause();
        }

        if (getTemplate()->getIsKing())
        {
            startKingLumMusic(m_isAcrobatic);
            AIUtils::setAlwaysActive(m_actor,btrue);
        }
        bbool isInRedMode = m_color==Ray_AIScoreLumBehavior::Color_Red;
        playLumPickingMusic(isInRedMode, m_isAcrobatic);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void Ray_AILums2Behavior::setState(State_t _state)
    {
        bbool canHaveTrail = bfalse;

        if (_state!=m_state)
        {
            if (m_state==State_FlyingToScore) //we are leaving last state where companion could be present
            {
                freeCompanion();
            }
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        switch(_state)
        {
        case State_Stands:
            {
                setIsTaken(bfalse);
                m_actor->setFlipped(bfalse);
                CHANGE_ANIM( lums2StandAnim )
            } break;

        case State_BeingTaken_Ejection:
            {
                canHaveTrail = btrue;
                initializeBeingTaken();
                m_actor->setFlipped(bfalse);
                CHANGE_ANIM( lums2FlyingNormalAnim )
            } break;

        case State_BeingTaken_CircularMovement:
            {
                canHaveTrail = btrue;
                initializeBeingTaken();
                m_actor->setFlipped(bfalse);
                CHANGE_ANIM( lums2FlyingNormalAnim )
            } break;
        case State_BeingTaken:  
            {
                canHaveTrail = btrue;
                initializeBeingTaken();
                CHANGE_ANIM(lums2BeingTakenNormalAnim)
            } break;
        case State_FlyingToScore: 
            {
                //
                if (m_isAcrobatic)
                {
                    CHANGE_ANIM(lums2FlyingAccrobAnim)                    
                }
                else
                {
                    CHANGE_ANIM(lums2FlyingNormalAnim)                    
                }
                canHaveTrail = btrue;
            } break;
        case State_PreTaken:
            {
            } break;
        case State_Taken:
            {
                if (m_actor->isSpawned())
                {
                    m_actor->requestDestruction();
                }
                m_actor->disable();
            } break;
        }

        if (!canHaveTrail)
        {
            removeAndInvalidateCurrentLumTrail();
        }
    }

    void Ray_AILums2Behavior::fetchColorFromScore()
    {
        if (getTemplate()->getIsKing() || m_forceYellow)
        {
            m_color = Ray_AIScoreLumBehavior::Color_Yellow;
        }
        else
        {
            Ray_AIScoreLumBehavior * behavior = Ray_AIUtils::fetchScoreLumBehavior();
            if (behavior)
            {
                m_color = behavior->getGlobalLumColor();
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        Ray_EventQueryLumCount queryLumEvent;

        EVENTMANAGER->broadcastEvent(&queryLumEvent);

        u32 kingLumCount = queryLumEvent.getLumKingCount();
        u32 lumCount = queryLumEvent.getNormalLumCount();
        DebugDraw::text(m_actor->getPos(), Color::white(), "%d lums,%d kings (total:%d)", lumCount, kingLumCount, lumCount+kingLumCount);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AILums2Behavior::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        updateSubAnimPhase();
    }
#endif // ITF_SUPPORT_EDITOR
}
