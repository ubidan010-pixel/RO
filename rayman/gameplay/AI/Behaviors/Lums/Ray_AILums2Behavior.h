#ifndef _ITF_RAY_AILUMS2BEHAVIOR_H_
#define _ITF_RAY_AILUMS2BEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_PickedLumTrajectoryFollower.h"
#endif //__ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__

namespace ITF
{
    class TriggerComponent;
    class Ray_AIScoreLumBehavior;
    class Trail3DComponent;
    class PlayerDetectorComponent;

    class Ray_AILums2Behavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILums2Behavior, AIBehavior,3580183628)
        DECLARE_SERIALIZE();

    public:
        DECLARE_VALIDATE_BEHAVIOR()

        static const u32 MAX_LUM_TRAILS = 16;
        static const u32 MAX_FULLY_INTACT_LUM_TRAILS = 8;

		enum State_t {State_NA,
            State_Stands, 
            State_BeingTaken, 
            State_FlyingToScore, //flying to score may already have started in being taken. This state is for the remaining part
            State_PreTaken,
            State_Taken, 
            State_BeingTaken_Ejection,
            State_BeingTaken_CircularMovement,
			ENUM_FORCE_SIZE_32(0)
		};


        Ray_AILums2Behavior();
        ~Ray_AILums2Behavior();

        virtual void        update( f32 _deltaTime );

        virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}

    private:
        struct LumTrailDesc
        {
            ActorRef m_actor;
            f32 m_alpha;

            LumTrailDesc() {m_alpha = 1;}
        };

        enum WaitMovement_t {WaitMovement_Stands, WaitMovement_Circular, WaitMovement_Ejection};

        ITF_INLINE const class Ray_AILums2Behavior_Template * getTemplate() const;

        static const u32 BankState_Normal = 0; //default
        static const u32 BankState_King = 1;

        void setState(State_t _state);
        void clear();
        void resetState();
        void processPicking_IfPossible(ObjectRef _activator, bbool _isAutoPicked, bbool _isAccrobatic, bbool _usePlayerSpeed, WaitMovement_t _waitMovement, u32 _lumCount);
        bbool getIsPlayerInAccrobaticMode(ObjectRef _actorRef) const;
        Ray_AIScoreLumBehavior *fetchScoreLumBehavior();
        void fetchColorFromScore();
        void freeCompanion();
        void setCompanionLumFromMainLum();
        void createCompanion();
        f32 computePosOnTrajectory(Vec3d &_projPos, Vec3d &_projSpeed, bbool &_isWaitingBeforeStart, f32 _delay);
        f32 moveOnTrajectory();
        void sendAddLumToScoreLum(const ActorRef &_picker);
        void initializeBeingTaken();
        void updateSubAnimPhase();

#ifdef ITF_SUPPORT_EDITOR
        void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        void onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        void startKingLumMusic(bbool _isAccrobatic);
        void playLumPickingMusic(bbool _isRedMode, bbool _isAccrobatic);
        LumTrailDesc *getNewTrail();
        void updateLumTrailList();
        void removeAndInvalidateCurrentLumTrail();
        f32 computeCircularMotionDuration(u32 _rank, u32 _maxRank, f32 &_radiusMultiplier);
        void processAutoPickupCircle(Ray_EventRewardPickedUp *_onPickedUp);
        void processLumEjectionEvent(Ray_EventLumEjection *_onEjection);
        bbool fetchTrajectoryParameters(Actor *_player, bbool _usePickingAnim);
        void switchToParticle_FlyingToScore();
        void prepareAutoPickupCircle(const Vec3d &_center, u32 _maxRank, f32 _radius, const ObjectRef &_sender);
        void updatePlayerDetectorRadius();
        bbool isTaken();
        void setIsTaken(bbool _isTaken);

        State_t				m_state;
        f32					m_timeInCurrentState;
        bbool               m_isAcrobatic;
        Ray_AIScoreLumBehavior::Color_t m_color;
        ActorRef            m_companion;
        ActorRef            m_actorWhoPicked;
        bbool               m_isAutoPicked;
        FixedArray<u32,16>  m_subAnimsToPatch;

        Vec3d               m_flightStart2D;
        Vec2d               m_flightDirection2D;
        Vec2d               m_flightGoal2D;

        Vec3d               m_initialTakenPos;
        Vec3d               m_playerNextPos;
        Vec2d				m_ejectionWhenSpawned_Quadratics[3];
        f32					m_ejectionDurationWhenSpawned;
        ObjectRef			m_actorWhoPicksAfterEjection;
        u32                 m_lastFrameTrailListWasUpdated;
        u32                 m_addedLumPlayerIndex;
        u32                 m_addedLumValue;
        bbool               m_usePlayerSpeed;
        Vec2d               m_destinationScorePos;
        u32                 m_lumCount;
        bbool               m_forceYellow;
        u32                 m_waterCheckOffset;

        Ray_PickedLumTrajectoryFollower::CircularMotionDesc  m_circularMotionDesc;
        WaitMovement_t      m_waitMovement;

        Ray_PickedLumTrajectoryFollower  m_pickingTrajectoryFollower;

        StringID::StringIdValueType m_currentAnim;

        PlayerDetectorComponent *m_playerDetectorComponent;
        TriggerComponent*   m_triggerComponent;
        SpawneeGenerator    m_companionLumGenerator;
        bbool               m_isTaken;
        SpawneeGenerator    m_lumTrailGenerator;
        bbool               m_usePickingAnim;
        static SafeArray<LumTrailDesc,MAX_LUM_TRAILS+1> m_lumTrailList;
        ActorRef            m_currentLumTrail;
    };

    class Ray_AILums2Behavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILums2Behavior_Template,TemplateAIBehavior,4099824222);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILums2Behavior);

    public:

        Ray_AILums2Behavior_Template();
        ~Ray_AILums2Behavior_Template();

        f32                 getTakenTrajectoryFactorX() const { return m_takenTrajectoryFactorX; }
        f32                 getTakenTrajectoryFactorY() const { return m_takenTrajectoryFactorY; }
        f32                 getPercentTimeStartFading() const { return m_percentTimeStartFading; }
        f32                 getWaitDurationPerRankWhenAutoPicked() const { return m_waitDurationPerRankWhenAutoPicked; }
        bbool               getIsKing() const { return m_isKing; }
        const StringID&     getStartKingComboFXLevel1() const {return m_startKingComboFXLevel1;}
        const StringID&     getStartKingComboFXLevel2() const {return m_startKingComboFXLevel2;}
        const Path&         getCompanionLumACT() const {return m_companionLumACT;}
        const Path&         getLumTrailACT() const {return m_lumTrailACT;}
        f32                 getTrailWidth() const {return m_trailWidth;}
        f32                 getAlphaWhenReachedScore() const {return m_alphaWhenReachedScore;}
        f32                 getCompanionLumDelay() const {return m_redCompanionLumDelay;}
        f32                 getCircularRotationSpeed() const {return m_circularRotationSpeed;}
        Volume              getLumKingMusicVolume() const {return m_lumKingMusicVolume;}
        f32                 getExtraDuration() const {return m_extraDuration;}
        f32                 getPickingAnimDuration() const {return m_pickingAnimDuration;}
        const AABB&         getParticleLumAABB() const {return m_particleLumAABB;}
        f32                 getScaleWhenReachedScore() const {return m_scaleWhenReachedScore;}
        f32                 getPercentTimeStartScaling() const {return m_percentTimeStartScaling;}
        f32                 getPlayerDetectorMultiplierInWater() const {return m_playerDetectorMultiplierInWater;}

    private:

        f32                 m_takenTrajectoryFactorX;
        f32                 m_takenTrajectoryFactorY;
        f32                 m_percentTimeStartFading;
        f32                 m_waitDurationPerRankWhenAutoPicked;
        bbool               m_isKing;
        StringID            m_startKingComboFXLevel1, m_startKingComboFXLevel2;
        Path                m_lumTrailACT;
        Path                m_companionLumACT;
        f32                 m_trailWidth;
        f32                 m_alphaWhenReachedScore;
        f32                 m_redCompanionLumDelay;
        f32                 m_circularRotationSpeed;
        Volume              m_lumKingMusicVolume;
        f32                 m_extraDuration;
        f32                 m_pickingAnimDuration;
        AABB                m_particleLumAABB;
        f32                 m_scaleWhenReachedScore;
        f32                 m_percentTimeStartScaling;
        f32                 m_playerDetectorMultiplierInWater;
    };

    ITF_INLINE const class Ray_AILums2Behavior_Template * Ray_AILums2Behavior::getTemplate() const
    {
        return static_cast<const class Ray_AILums2Behavior_Template *>(m_template);
    }
}

#endif // _ITF_RAY_AILUMS2BEHAVIOR_H_

