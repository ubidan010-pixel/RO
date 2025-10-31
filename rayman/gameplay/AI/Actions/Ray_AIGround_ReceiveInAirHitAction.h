#ifndef _ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_
#define _ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_

#ifndef _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGroundReceiveHitAction.h"
#endif //_ITF_RAY_AIGROUNDRECEIVEHITACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;
    class PhantomComponent;

    class Ray_AIGround_ReceiveInAirHitAction : public Ray_AIGroundReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveInAirHitAction, Ray_AIGroundReceiveHitAction,2069215661);
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AIGround_ReceiveInAirHitAction();
        virtual ~Ray_AIGround_ReceiveInAirHitAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update( f32 _dt );
        virtual void                        onEvent( Event* _event);
        virtual void                        updateAnimInputs();

    protected:

        virtual void                        setupHitForce();
        virtual void                        updateHit( f32 _dt );

        StickToPolylinePhysComponent*       m_physComponent;

    private:
        enum EjectionState
        {
            TakeOff,
            Cycle,
            Fall,
            Land,
            ENUM_FORCE_SIZE_32(0)
        };

        ITF_INLINE const class Ray_AIGround_ReceiveInAirHitAction_Template* getTemplate() const;

        void                                updateAntigrav( f32 _dt );
        void                                setEjectionState(EjectionState _newState);

        // runtime values
        bbool                               m_justEntered;
        f32                                 m_prevFrictionMultiplier;
        f32                                 m_prevGravityMultiplier;
        f32                                 m_fullAntigravTimer;
        f32                                 m_antigravRampDownTimer;
        EjectionState                       m_ejectionState;
        f32                                 m_ejectionCycleCursor;
        f32                                 m_timeInCurrentState;

//#define DEBUG_HIT_TRAJECTORY
#ifdef DEBUG_HIT_TRAJECTORY
        Vec2d                               m_debugLastPos;
#endif
    };

    class Ray_AIGround_ReceiveInAirHitAction_Template : public Ray_AIGroundReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveInAirHitAction_Template,Ray_AIGroundReceiveHitAction_Template,2127135186);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGround_ReceiveInAirHitAction);

    public:

        Ray_AIGround_ReceiveInAirHitAction_Template();
        ~Ray_AIGround_ReceiveInAirHitAction_Template() {}

        f32                                 getGravityMultiplier() const { return m_gravityMultiplier; }
        f32                                 getHitForce() const { return m_hitForce; }
        f32                                 getFullAntigravTime() const { return m_fullAntigravTime; }
        f32                                 getAntigravRampDownTime() const { return m_antigravRampDownTime; }
        bbool                               getCanBlockHits() const { return m_canBlockHits; }

    private:

        f32                                 m_gravityMultiplier;
        f32                                 m_hitForce;
        f32                                 m_fullAntigravTime;
        f32                                 m_antigravRampDownTime;
        bbool                               m_canBlockHits;
    };

    ITF_INLINE const class Ray_AIGround_ReceiveInAirHitAction_Template* Ray_AIGround_ReceiveInAirHitAction::getTemplate() const { return static_cast<const Ray_AIGround_ReceiveInAirHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_