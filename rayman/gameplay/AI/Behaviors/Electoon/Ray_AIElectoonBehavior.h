#ifndef __RAY_AIELECTOONBEHAVIOR_H__
#define __RAY_AIELECTOONBEHAVIOR_H__

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{
    class Trail3DComponent;

    class Ray_AIElectoonBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIElectoonBehavior, AIBehavior,1886121291);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

		enum State_t {State_NA,State_PreAppear,State_Appear, State_Stand, State_Walking, State_PreYeah_Signal, State_Yeah, State_PreDisappears, State_Disappearing,
			ENUM_FORCE_SIZE_32(0)
		};
        enum RoamDir_t {Roam_Left=0, Roam_Right,ENUM_FORCE_SIZE_32(RoamDir)};

        Ray_AIElectoonBehavior();
        ~Ray_AIElectoonBehavior();

        virtual void        update( f32 _deltaTime );

        virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}

    private:
        ITF_INLINE const class Ray_AIElectoonBehavior_Template * getTemplate() const;
        virtual void        onActorBecomeActive();
        virtual void        onActorBecomeInactive();

        void				invertRoamingDir() {if (m_roamingDir==Roam_Left) m_roamingDir = Roam_Right; else m_roamingDir = Roam_Left;}

        void                enableTrail(bbool _enable);

        void setState(State_t _state);
        void switchToStand(bbool _canChangeAnim);
        //bbool isOnGround();
        void updateWalking(f32 _deltaTime, bbool _signaledMustGoAway);
        Actor *getRewardSpawner(bbool &_mustZeroSpeed);
        Actor *getNearestPlayer();
        void safeStopFXController(u32 &_fxHandle, FXControllerComponent *_controller);
        bbool isInWater();
        void setIdealSpeed(f32 _idealSpeed);
        void updateWalking_blockedCounter(f32 _idealSpeed, f32 _deltaTime);
        bbool hasHoleInFront_Extended( Actor* _actor, bbool _walkLeft, PolyLine* _poly,
            u32 _startEdge, f32 _searchDist, f32 _radius, 
            const Vec2d& _gravityDir, f32& _holeDepth );



        StickToPolylinePhysComponent *m_physComponent;
        Trail3DComponent *m_trailComponent;
        AIWalkInDirAction *m_walkAction;

        Vec3d               m_posWhenstartDisappearing;
        u32                 m_fx1Handle, m_fx2Handle;
        State_t				m_state;
        static ActorList	m_tmpActorList;
        f32					m_timeInCurrentState;
        u32					m_randomForStandAnim;        
        f32					m_timeBeforeWalking;
        f32					m_deltaTimeBeforeYeah;
        f32                 m_timeWalking;
        f32					m_idealSpeed;
        f32					m_speedFactor;
        f32                 m_disappearSinFactor, m_disappearFastSinFactor, m_disappearFastSinMult;
        f32                 m_watchdogTimer;
        bbool               m_willStopWhenNear;
        f32                 m_totalTimeWalkingSinceAppeared;
        static bbool        m_anElectoonStartedSignal;
        ObjectRef           m_ejectionTargetObj;
        Vec3d               m_ejectionTargetPos;
        bbool               m_justBlockedByWall;
        f32                 m_walking_blockedCounter;
        u32					m_walking_framesSinceChangedDirection;
        RoamDir_t			m_roamingDir;
        f32					m_holeSearchDist;
    };

    class Ray_AIElectoonBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIElectoonBehavior_Template,TemplateAIBehavior,2056904193);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIElectoonBehavior);

    public:

        Ray_AIElectoonBehavior_Template();
        ~Ray_AIElectoonBehavior_Template();


        ITF_INLINE const class AIWalkInDirAction_Template* getWalkInDirAction() const { return m_walkAction; }
        ITF_INLINE f32 getMinTimeBeforeWalking() const { return m_minTimeBeforeWalking; }
        ITF_INLINE f32 getMaxTimeBeforeWalking() const { return m_maxTimeBeforeWalking; }
        ITF_INLINE f32 getMinTimeWalking() const { return m_minTimeWalking; }
        ITF_INLINE f32 getMaxTimeWalking() const { return m_maxTimeWalking; }
        ITF_INLINE f32 getTypicalSpeed() const { return m_typicalSpeed; }
        ITF_INLINE f32 getMinEjectionSpeed() const { return m_minEjectionSpeed; }
        ITF_INLINE f32 getMaxEjectionSpeed() const { return m_maxEjectionSpeed; }
        ITF_INLINE f32 getLovePlayerDist() const { return m_lovePlayerDist; }
        ITF_INLINE f32 getTimeBeforeCanStopWalkingAndStandAgain() const { return m_timeBeforeCanStopWalkingAndStandAgain; }
        ITF_INLINE bbool getMustStay() const { return m_mustStay; }
        ITF_INLINE f32 getMaxDeltaYToFollow() const {return m_maxDeltaYToFollow;}

    private:
        AIWalkInDirAction_Template* m_walkAction;
        f32                         m_minTimeBeforeWalking;
        f32                         m_maxTimeBeforeWalking;
        f32                         m_minTimeWalking;
        f32                         m_maxTimeWalking;
        f32                         m_typicalSpeed;
        f32                         m_minEjectionSpeed;
        f32                         m_maxEjectionSpeed;
        f32                         m_lovePlayerDist;
        f32                         m_timeBeforeCanStopWalkingAndStandAgain;
        f32                         m_maxDeltaYToFollow;
        bbool                       m_mustStay;
    };

    ITF_INLINE const class Ray_AIElectoonBehavior_Template * Ray_AIElectoonBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIElectoonBehavior_Template *>(m_template);
    }
}

#endif
