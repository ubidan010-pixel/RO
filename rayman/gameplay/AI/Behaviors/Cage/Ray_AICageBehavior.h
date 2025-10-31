#ifndef _ITF_RAY_AICAGEBEHAVIOR_H_
#define _ITF_RAY_AICAGEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif


namespace ITF
{
	class TrailComponent;
    class PhantomComponent;
    class Ray_CageAIComponent;

    class Ray_AICageBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICageBehavior, AIBehavior,1257310741);

    public:
        DECLARE_VALIDATE_BEHAVIOR()

		enum State_t {State_NA,State_Preappears,State_Stands, State_Breaks, State_Broken ,
			ENUM_FORCE_SIZE_32(0)
		};

        Ray_AICageBehavior();
        ~Ray_AICageBehavior();

		virtual void        update( f32 _deltaTime );

		virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}
        static bbool        getGlobalLockOptOut() {return s_globalLocksOptOut;}

    private:

        ITF_INLINE const class Ray_AICageBehavior_Template * getTemplate() const;

        void sendCloseCageDoor();
        void setState(State_t _state);
        void setStandInputs();
        void updateLock(u32 _lockIndex,u32 _step);
        bbool checkPunchStimOnPhantomPolyline(const StringID &_lockID, PunchStim *_hit);
        bbool incrementLockStep(u32 _lockIndex, u32 &_step);
        void processSmashOpenEvent();
        void setLockRumbleAnim(u32 _lockIndex, bbool _on);
        void setCageRumbleAnim(bbool _on);
        bbool areAllEnemiesDead();
        void getElectoonTargets(SafeArray<ObjectRef> &_electoonPlacements, u32 _electoonCount) const;
        void resetState(); //reset state from current leveldata       
        void sendActivationToAfterFX(bbool _activation);
        void sendActivationToChangePage(bbool _activation);
        void sendSpawnElectoonsIfNotAlreadyDone();
        bbool isLinkedToForceField();
#ifdef ITF_SUPPORT_EDITOR
        void drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR
        void sendActivationMessageToMostObjects();
        void sendEventsWhenEnteringBrokenCageState();
        void spawnBrokenCageRewards();
        bbool isLastCage() const;
        Actor *spawnBrokenCageRewardFromCode(u32 _code, const Vec3d &_pos);
        void setLockOptOut(bbool _forbid);

		State_t				m_state;
		f32					m_timeInCurrentState;
        u32                 m_lLockStep, m_rLockStep;
        i32                 m_lastLLockAnim, m_lastRLockAnim;
        bbool               m_isLLockAnimPlaying, m_isRLockAnimPlaying; //those bools are only valid in stand
        bbool               m_alreadyTeleported;
        Vec2d               m_exitPoint;
        i32                 m_smashOpenTimer;
        f32                 m_fadeTimer;
        bbool               m_sendActivationMessageToObjectsASAP;
        SpawneeGenerator    m_electoonGenerator;
        SpawneeGenerator    m_stayElectoonGenerator;
        SpawneeGenerator    m_stillHeartGenerator;
        SpawneeGenerator    m_cineGenerator;
        u32                 m_lastFrameLockAltered;
        bbool               m_spawnedRewardsThisTime;
        bbool               m_spawnedEntranceElectoons;
        ObjectRef           m_breakerActor;
        bbool               m_wasSmashOpen;
        bbool               m_locksOptOut;
        static bbool        s_globalLocksOptOut;

        Ray_CageAIComponent *m_cageAIComponent;
    };

    class Ray_AICageBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICageBehavior_Template,TemplateAIBehavior,3603496838);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AICageBehavior);

    public:

        Ray_AICageBehavior_Template();
        ~Ray_AICageBehavior_Template(){}

        ITF_INLINE u32 getElectoonCount() const { return m_electoonCount; }
        ITF_INLINE f32 getElectoonAppearRadius() const { return m_electoonAppearRadius; }
        ITF_INLINE const Path & getElectoonPath() const { return m_electoonPath; }
        ITF_INLINE const Path & getStayElectoonPath() const { return m_stayElectoonPath; }
        ITF_INLINE const Path & getStillHeartPath() const {return m_stillHeartPath;}
        ITF_INLINE f32 getCageSpawnerYOffset() const { return m_cageSpawnerYOffset; }
        ITF_INLINE f32 getDurationBeforeFadeOut() const {return m_durationBeforeFadeOut;}
        ITF_INLINE f32 getBroken_generatedRewardRadius() const {return m_broken_generatedRewardRadius;}
        ITF_INLINE const SafeArray<u32> getBroken_generatedRewards() const {return m_broken_generatedRewards;}
        ITF_INLINE const Vec3d &getHeartOffset() const {return m_heartOffset;}
        ITF_INLINE i32 getLastCageIndex() const {return m_lastCageIndex;}
        ITF_INLINE f32 getAlreadyOpenAlpha() const {return m_alreadyOpenAlpha;}
        ITF_INLINE f32 getStayElectoonXOffset() const {return m_stayElectoonXOffset;}
        const Path&    getCinePath() const { return m_cinePath; }
        const Path&    getEndCinePath() const { return m_endCinePath; }

    private:
        f32                 m_cageSpawnerYOffset;
        u32                 m_electoonCount;
        f32                 m_electoonAppearRadius;
        Path                m_electoonPath;
        Path                m_stayElectoonPath;
        Path                m_stillHeartPath;
        SafeArray<u32>      m_broken_generatedRewards;
        f32                 m_broken_generatedRewardRadius;
        f32                 m_durationBeforeFadeOut;
        Vec3d               m_heartOffset;
        i32                 m_lastCageIndex;
        f32                 m_alreadyOpenAlpha;
        f32                 m_stayElectoonXOffset;
        Path                m_cinePath;
        Path                m_endCinePath;

    };

    ITF_INLINE const class Ray_AICageBehavior_Template * Ray_AICageBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AICageBehavior_Template *>(m_template);
    }
}

#endif // _ITF_Ray_AICageBehavior_H_

