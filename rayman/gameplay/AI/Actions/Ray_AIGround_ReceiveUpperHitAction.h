#ifndef _ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_
#define _ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_

#ifndef _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGroundReceiveHitAction.h"
#endif //_ITF_RAY_AIGROUNDRECEIVEHITACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;
    class PhantomComponent;

    class Ray_AIGround_ReceiveUpperHitAction : public Ray_AIGroundReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveUpperHitAction, Ray_AIGroundReceiveHitAction,3467614903);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AIGround_ReceiveUpperHitAction();
        virtual ~Ray_AIGround_ReceiveUpperHitAction();

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

        ITF_INLINE const class Ray_AIGround_ReceiveUpperHitAction_Template* getTemplate() const;

        void                                updateAntigrav( f32 _dt );


        f32                                 m_initialHeight;
        bbool                               m_speedReset;


        // runtime values
        bbool                               m_justEntered;
        f32                                 m_prevFrictionMultiplier;
        f32                                 m_prevGravityMultiplier;
        f32                                 m_fullAntigravTimer;
        f32                                 m_antigravRampDownTimer;
        enum EjectionState
        {
            TakeOff,
            Cycle,
            Fall,
            Land,
            ENUM_FORCE_SIZE_32(1)
        };
        EjectionState                       m_ejectionState;
        f32                                 m_ejectionCycleCursor;


//#define DEBUG_HIT_TRAJECTORY
#ifdef DEBUG_HIT_TRAJECTORY
        Vec2d                               m_debugLastPos;
#endif
    };

    class Ray_AIGround_ReceiveUpperHitAction_Template : public Ray_AIGroundReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveUpperHitAction_Template,Ray_AIGroundReceiveHitAction_Template,4226911414);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGround_ReceiveUpperHitAction);

    public:

        Ray_AIGround_ReceiveUpperHitAction_Template();
        ~Ray_AIGround_ReceiveUpperHitAction_Template() {}

        f32                                 getGravityMultiplier() const { return m_gravityMultiplier; }
        f32                                 getHitForce() const { return m_hitForce; }
        f32                                 getFullAntigravTime() const { return m_fullAntigravTime; }
        f32                                 getAntigravRampDownTime() const { return m_antigravRampDownTime; }
        bbool                               getCanBlockHits() const { return m_canBlockHits; }
        f32                                 getBrakeForce() const { return m_brakeForce; }
        f32                                 getBrakeHeight() const { return m_brakeHeight; }

    private:

        f32                                 m_gravityMultiplier;
        f32                                 m_hitForce;
        f32                                 m_fullAntigravTime;
        f32                                 m_antigravRampDownTime;
        bbool                               m_canBlockHits;
        f32                                 m_brakeForce;
        f32                                 m_brakeHeight;
    };

    ITF_INLINE const class Ray_AIGround_ReceiveUpperHitAction_Template* Ray_AIGround_ReceiveUpperHitAction::getTemplate() const { return static_cast<const Ray_AIGround_ReceiveUpperHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_