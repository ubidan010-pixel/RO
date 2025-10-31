#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_
#define _ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_


#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylinePunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    class Ray_AIPerformHitPolylineJumpPunchAction : public Ray_AIPerformHitPolylinePunchAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPolylineJumpPunchAction, Ray_AIPerformHitPolylinePunchAction,2246706783);
        DECLARE_SERIALIZE()

    public:

        Ray_AIPerformHitPolylineJumpPunchAction();
        virtual ~Ray_AIPerformHitPolylineJumpPunchAction();

        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        update(f32 _dt);
        virtual void        onEvent(Event* _event);
    protected:

        ITF_INLINE const class Ray_AIPerformHitPolylineJumpPunchAction_Template * getTemplate() const;
        
        StickToPolylinePhysComponent * m_physComponent;
        f32     m_previousGravityMultiplier;
        f32     m_timer;
        bbool   m_speedReset;
        bbool   m_jump;
        bbool   m_antiGravStart;
        bbool   m_antiGravStop;
        bbool   m_justJumped;

    };

    class Ray_AIPerformHitPolylineJumpPunchAction_Template : public Ray_AIPerformHitPolylinePunchAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPolylineJumpPunchAction_Template,Ray_AIPerformHitPolylinePunchAction_Template,1280052077);
        DECLARE_AIACTION_TEMPLATE(Ray_AIPerformHitPolylineJumpPunchAction);
        DECLARE_SERIALIZE();

    public:

        Ray_AIPerformHitPolylineJumpPunchAction_Template();
        ~Ray_AIPerformHitPolylineJumpPunchAction_Template(){}

        f32     getJumpYForce() const { return m_jumpYForce; }
        f32     getJumpXForce() const { return m_jumpXForce; }
        f32     getAntiGravTime() const { return m_antiGravTime; }

    private:

        f32     m_jumpYForce;
        f32     m_jumpXForce;
        f32     m_antiGravTime;
    };

    ITF_INLINE const class Ray_AIPerformHitPolylineJumpPunchAction_Template * Ray_AIPerformHitPolylineJumpPunchAction::getTemplate() const
    {
        return static_cast<const class Ray_AIPerformHitPolylineJumpPunchAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_

