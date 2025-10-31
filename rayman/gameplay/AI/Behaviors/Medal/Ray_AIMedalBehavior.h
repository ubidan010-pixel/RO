#ifndef _ITF_RAY_AIMEDALBEHAVIOR_H_
#define _ITF_RAY_AIMEDALBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif
#ifndef _ITF_STANDEECOMPONENT_H_
#include "engine/actors/components/StandeeComponent.h"
#endif
#define MEDAL_ELECTOON_COUNT            6

namespace ITF
{
	class TrailComponent;
    class PhantomComponent;

    class Ray_AIMedalBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIMedalBehavior, AIBehavior,1076789266);

    public:
        DECLARE_VALIDATE_BEHAVIOR()

		enum State_t {State_NA,State_Preappears,State_Appears, State_AddingElectoon, State_Finish, State_ReAdd, State_JustDisappeared,
			ENUM_FORCE_SIZE_32(0)
		};

        Ray_AIMedalBehavior();
        ~Ray_AIMedalBehavior();

		virtual void        update( f32 _deltaTime );

		virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}

        static bbool        isPlayingVictorySequence() {return m_isSequencePlaying;}

    private:
        ITF_INLINE const class Ray_AIMedalBehavior_Template * getTemplate() const;

        u32 computeElectoonMask(Actor *_ignoreCage, u32 _ignoreChallenges /*see EChallengeType*/, bbool* _isMedalComplete=NULL);
		void setState(State_t _state);
        void pauseWorld(bbool _pause);
        void setElectoonPresence(u32 _index, bbool _present);
        void setElectoonAppears(u32 _index, bbool _appears);
        void setElectoonBounces(u32 _index, bbool _bounces);
        void playVictorySequence();
        void prepareVictorySequence();
        void updateScaleAndPosition();
        void freeUnusedCineActor(bbool _bforce);
        void restoreDeadPlayers();
        void updateChallengeElectoons();
        void updateBeatenBossELectoon();
        void processShowMedalEvent(Ray_EventShowElectoonMedal *_showUI);
        u32 computeLowestBit(u32 mask);
        void setupOldElectoonsAsAlreadyPresent();
        void processEndLevel();
        void FindStandeeActor(World* world);
        void ChangeStandeePicture(StandeeComponent* _actor);

        static bbool m_isSequencePlaying;

        bbool               m_justBeatenBoss;
        bbool               m_justBrokenCage, m_playEndSequence, m_forTimeAndLumAttacks, m_playVictorySequence, m_autoEndLevel;
		bbool				m_processEndLevel;
		bbool				m_processSwitchEndLevel;

        bbool               m_dontSetTimeAttack, m_dontSetLumAttack1, m_dontSetLumAttack2;
		State_t				m_state;
		f32					m_timeInCurrentState;
        ObjectId            m_lastCallingCageId;
        ActorRef            m_cineActorRef, m_lastCallingActor;
        ActorRef            m_customCineActorRef;
        u32                 m_newElectoonMask, m_oldElectoonMask;
        u32                 m_electoonsToMakeAppear, m_alreadyPresentElectoonsToEmphasize;
        bbool               m_currentElectoonAlreadyExisted;

		ObjectRef			m_preloadedEndLevelMapRef;
    };

    class Ray_AIMedalBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIMedalBehavior_Template,TemplateAIBehavior,3301873927);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIMedalBehavior);

    public:

        Ray_AIMedalBehavior_Template();
        ~Ray_AIMedalBehavior_Template() {}


    private:

    };

    ITF_INLINE const class Ray_AIMedalBehavior_Template * Ray_AIMedalBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIMedalBehavior_Template *>(m_template);
    }
}

#endif // _ITF_RAY_AIMEDALBEHAVIOR_H_

