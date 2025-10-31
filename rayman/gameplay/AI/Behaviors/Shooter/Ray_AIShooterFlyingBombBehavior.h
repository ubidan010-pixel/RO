#ifndef _ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_
#define _ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIShooterFlyingBombBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterFlyingBombBehavior, AIBehavior ,3238743339)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIShooterFlyingBombBehavior();
        virtual ~Ray_AIShooterFlyingBombBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        virtual void            onActionFinished();
		virtual void            onEvent( Event * _event);
        virtual void            updateAnimInputs();
        virtual void            onResourceReady();
        virtual void            onActorBecomeInactive();

	protected:

        void                    startMoveAction();
        void                    startOpeningAction();
        void                    startAlarmAction();
        void                    startExplodeAction();

        void                    processHit( HitStim * _stim );
        //bbool                   processHitSender( ObjectRef _sender );
        
      
		class AIBezierAction *	m_moveAction;
        class AIAction *	    m_openingAction;
        class AIAction *	    m_alarmAction;
        class AIAction *	    m_explodeAction;
        class DetectorComponent * m_detectorComponent;
        
        ActorRef                m_destinationActor;
        Vec2d                   m_destinationScale;

        f32                     m_alarmTimer;
        f32                     m_alarmBlinkTimer;
        f32                     m_alarmBlinkAnimDuration;
        f32                     m_warnAnimDuration;
        f32                     m_hitTimer;
        f32                     m_multiHitTimer;        
        
        /*ObjectRefList           m_lastHitActorsList;
        SafeArray<u32>          m_lastHitActorsFrameList;*/

    private:
        ITF_INLINE const class Ray_AIShooterFlyingBombBehavior_Template * getTemplate() const;

    };

    class Ray_AIShooterFlyingBombBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterFlyingBombBehavior_Template,TemplateAIBehavior,2272987957);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterFlyingBombBehavior);

    public:

        Ray_AIShooterFlyingBombBehavior_Template();
        ~Ray_AIShooterFlyingBombBehavior_Template();

        ITF_INLINE const class AIBezierAction_Template *    getMoveActionTemplate() const               { return m_moveAction; }
        ITF_INLINE const class AIAction_Template *          getOpeningActionTemplate() const            { return m_openingAction; }
        ITF_INLINE const class AIAction_Template *          getWaitBeforeExplodeActionTemplate() const  { return m_alarmAction; }
        ITF_INLINE const class AIAction_Template *          getExplodeActionTemplate() const            { return m_explodeAction; }
        ITF_INLINE const StringID &                         getAlarmAnimName() const                    { return m_alarmAnimName; }
        ITF_INLINE const StringID &                         getWarnAnimName() const                     { return m_warnAnimName; }
        ITF_INLINE const f32                                getAlarmDuration() const                    { return m_alarmDuration; }
        ITF_INLINE const bbool                              isTriggered() const                         { return m_isTriggered; }
        ITF_INLINE const f32                                getMoveActionInitScale() const              { return m_moveActionInitScale; }
        ITF_INLINE const u32                                getAnimationBankState() const               { return m_animationBankState; }
        ITF_INLINE const bbool                              getExplodeOnMoveActionsEnd() const          { return m_explodeOnMoveActionsEnd; }
        ITF_INLINE const Vec2d &                            getMoveActionVecOffsetMin() const           { return m_moveActionVecOffsetMin; }
        ITF_INLINE const Vec2d &                            getMoveActionVecOffsetMax() const           { return m_moveActionVecOffsetMax; }
        

    private:
	
        class AIBezierAction_Template * m_moveAction;
        class AIAction_Template *       m_openingAction;
        class AIAction_Template *       m_alarmAction;
        class AIAction_Template *	    m_explodeAction;
        
        StringID 	                    m_alarmAnimName;
        StringID 	                    m_warnAnimName;
        f32                             m_alarmDuration;
        bbool                           m_isTriggered;
        f32                             m_moveActionInitScale;
        u32                             m_animationBankState;
        bbool                           m_explodeOnMoveActionsEnd;
        Vec2d                           m_moveActionVecOffsetMin;
        Vec2d                           m_moveActionVecOffsetMax;

    };

    ITF_INLINE const class Ray_AIShooterFlyingBombBehavior_Template * Ray_AIShooterFlyingBombBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterFlyingBombBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_
