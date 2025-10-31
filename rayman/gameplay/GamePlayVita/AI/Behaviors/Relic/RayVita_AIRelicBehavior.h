#ifndef _ITF_RayVita_AIRelicBehavior_H_
#define _ITF_RayVita_AIRelicBehavior_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_PickedLumTrajectoryFollower.h"
#endif //__ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__

namespace ITF
{
    class TriggerComponent;
	class FXControllerComponent;
	class LinkComponent;

    class RayVita_AIRelicBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(RayVita_AIRelicBehavior, AIBehavior,267847435);

    public:
        DECLARE_VALIDATE_BEHAVIOR()

		enum State_t {
			State_Stands, 
			State_Signe,
			State_BeingTaken,
			State_FlyingToInterface,
			State_PreTaken,
			State_EndTaken,
			State_Taken,
			ENUM_FORCE_SIZE_32(0)
		};

        RayVita_AIRelicBehavior();
        ~RayVita_AIRelicBehavior();

		virtual void        update( f32 _deltaTime );

		virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}
		virtual void		onActorBecomeInactive();

        void                resetState();      //reset state from current leveldata

        u32                 getFrescoId() const { return m_frescoId; }
		i32                 getRelicIndex() const { return m_relicIndex; }

    private:
        ITF_INLINE const class RayVita_AIRelicBehavior_Template * getTemplate() const;
		bbool               fetchTrajectoryParameters(Actor *_player);
		f32                 moveOnTrajectory();
		void                playSound(const StringID& _sound);


		void                setState(State_t _state);
        bbool		        isNearMainPlayer() const;

		State_t				m_state;
        u32                 m_frescoId;
		i32                 m_relicIndex;

        TriggerComponent*   m_triggerComponent;
		Ray_PickedLumTrajectoryFollower  m_pickingTrajectoryFollower;
		FXControllerComponent * m_fxControllerComponent;
		LinkComponent*      m_linkComponent;
		Vec2d               m_destinationScorePos;
		f32					m_timeInCurrentState;
		bbool               m_usePickingAnim;
		f32					m_waitingBeforSignTime;
		f32					m_idleSoundTime;
    };

    class RayVita_AIRelicBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(RayVita_AIRelicBehavior_Template,TemplateAIBehavior,357578962);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(RayVita_AIRelicBehavior);

    public:

        RayVita_AIRelicBehavior_Template();
        ~RayVita_AIRelicBehavior_Template(){}

        ITF_INLINE u32                 getFrescoId() const {return m_frescoId;}
		ITF_INLINE i32                 getRelicIndex() const {return m_relicIndex;}
		ITF_INLINE const StringID&	   getIdleSoundID() const { return m_idleSound; }

		ITF_INLINE f32                 getTakenTrajectoryFactorX() const { return m_takenTrajectoryFactorX; }
		ITF_INLINE f32                 getTakenTrajectoryFactorY() const { return m_takenTrajectoryFactorY; }
		ITF_INLINE f32                 getPercentTimeStartFading() const { return m_percentTimeStartFading; }
		ITF_INLINE f32                 getAlphaWhenReachedScore() const {return m_alphaWhenReachedScore;}
		ITF_INLINE f32                 getEndDuration() const {return m_endDuration;}
		ITF_INLINE f32                 getPickingAnimDuration() const {return m_pickingAnimDuration;}
		ITF_INLINE const AABB&         getParticleLumAABB() const {return m_particleLumAABB;}
		ITF_INLINE f32                 getScaleWhenReachedScore() const {return m_scaleWhenReachedScore;}
		ITF_INLINE f32                 getPercentTimeStartScaling() const {return m_percentTimeStartScaling;}
		ITF_INLINE f32                 getSigneDuration() const {return m_signeDuration;}
		ITF_INLINE f32                 getWaitingBeforeSigneDuration() const {return m_waitingBeforeSigneDuration;}
		ITF_INLINE f32                 getWaitingBeforePlayIdleSoundDuration() const {return m_waitingBeforePlayIdleSoundDuration;}
		ITF_INLINE f32                 getIconDuration() const {return m_iconDuration;}

    private:

		u32                 m_frescoId;
        i32                 m_relicIndex;

		StringID            m_idleSound;

		f32                 m_takenTrajectoryFactorX;
		f32                 m_takenTrajectoryFactorY;
		f32                 m_percentTimeStartFading;
		f32                 m_alphaWhenReachedScore;
		f32                 m_endDuration;
		f32                 m_iconDuration;
		f32                 m_pickingAnimDuration;
		AABB                m_particleLumAABB;
		f32                 m_scaleWhenReachedScore;
		f32                 m_percentTimeStartScaling;
		f32                 m_signeDuration;
		f32                 m_waitingBeforeSigneDuration;
		f32                 m_waitingBeforePlayIdleSoundDuration;
    };

    ITF_INLINE const class RayVita_AIRelicBehavior_Template * RayVita_AIRelicBehavior::getTemplate() const
    {
        return static_cast<const class RayVita_AIRelicBehavior_Template *>(m_template);
    }
}

#endif // _ITF_RayVita_AIRelicBehavior_H_

