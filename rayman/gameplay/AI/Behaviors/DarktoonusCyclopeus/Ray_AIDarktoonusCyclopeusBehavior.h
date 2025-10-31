#ifndef _ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_
#define _ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIPlayAnimAction;
    class Ray_AIPerformHitPolylinePunchAction;

    class Ray_AIDarktoonusCyclopeusBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonusCyclopeusBehavior, AIBehavior,3317343475);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIDarktoonusCyclopeusBehavior();
        virtual ~Ray_AIDarktoonusCyclopeusBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            onEvent( Event * _event );
        virtual void            update(f32 _dt);
        virtual void            onActionFinished();
        virtual void            updateAnimInputs();
#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

                bbool           isHitable() const; 
                void            UpdatePupil( f32 _dt);

    private: 
                ActorRef        getClosestEnemies(f32 _range, bbool _mustStickOnPoly = bfalse);
                bbool           enemiesIsDetectForAttack();
                bbool           queryIsStickOnPolyline(Actor* _player); 
                bbool           queryIsDead(Actor* _player);

    private:

        ITF_INLINE const class Ray_AIDarktoonusCyclopeusBehavior_Template * getTemplate() const;

        AIPlayAnimAction*                       m_idleAction;
        AIPlayAnimAction*                       m_prepareFocusAction;
        AIPlayAnimAction*                       m_focusAction;
        Ray_AIPerformHitPolylinePunchAction*    m_attackAction;
        AIPlayAnimAction*                       m_waitHitAction;


        u32     m_stateAttack;
        u32     m_stateImpact;
        u32     m_stateWait;

        f32     m_countDownAttack;
        f32     m_countDownWait;
        f32     m_countDownFocus;

        f32     m_angleAttack;
        f32     m_distAttack;
        f32     m_distProgress;
        f32     m_distAttackInput;

        bbool   m_hasContact;
        bbool   m_transitionIsFinish;

        f32     m_prevFactorH;
        f32     m_prevFactorV;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class AIPlayAnimAction_Template;
    class Ray_AIPerformHitPolylinePunchAction_Template;

    class Ray_AIDarktoonusCyclopeusBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonusCyclopeusBehavior_Template, TemplateAIBehavior,617789785);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIDarktoonusCyclopeusBehavior);

    public:

        Ray_AIDarktoonusCyclopeusBehavior_Template();
        ~Ray_AIDarktoonusCyclopeusBehavior_Template();

        ITF_INLINE const AIPlayAnimAction_Template* getIdleAction() const { return m_idleAction; }
        ITF_INLINE const AIPlayAnimAction_Template* getPrepareFocusAction() const { return m_prepareFocusAction; }
        ITF_INLINE const AIPlayAnimAction_Template* getFocusAction() const { return m_focusAction; }
        ITF_INLINE const Ray_AIPerformHitPolylinePunchAction_Template*  getAttackAction() const { return m_attackAction; }
        ITF_INLINE const AIPlayAnimAction_Template* getWaitHitAction() const { return m_waitHitAction; }


        ITF_INLINE f32 getDistanceDetect() const                { return m_distanceDetect; }
        ITF_INLINE const Angle& getAngleMaxDetect() const       { return m_angleMaxDetect; }
        ITF_INLINE f32 getCountDownAttack() const               { return m_countDownAttack; }
        ITF_INLINE f32 getSpeed() const                         { return m_speed; }
        ITF_INLINE f32 getSpeedBack() const                     { return m_speedBack; }
        ITF_INLINE f32 getTimeWaiting() const                   { return m_timeWaiting; }
        ITF_INLINE f32 getTimeFocus() const                     { return m_timeFocus; }

        ITF_INLINE bbool getMustBeStickToAttack() const         { return m_mustBeStickToAttack; }

    private:

        AIPlayAnimAction_Template*                          m_idleAction;
        AIPlayAnimAction_Template*                          m_prepareFocusAction;        
        AIPlayAnimAction_Template*                          m_focusAction;        
        Ray_AIPerformHitPolylinePunchAction_Template*       m_attackAction;
        AIPlayAnimAction_Template*                          m_waitHitAction;

        f32    m_distanceDetect;
        Angle  m_angleMaxDetect;
        f32    m_countDownAttack;
        f32    m_speed;
        f32    m_speedBack;
        f32    m_timeWaiting;
        f32    m_timeFocus;

        bbool  m_mustBeStickToAttack;


    };

    ITF_INLINE const class Ray_AIDarktoonusCyclopeusBehavior_Template * Ray_AIDarktoonusCyclopeusBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIDarktoonusCyclopeusBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_
