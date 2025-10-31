#ifndef _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_
#define _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_

#ifndef _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGroundReceiveHitAction.h"
#endif //_ITF_RAY_AIGROUNDRECEIVEHITACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;
    class PhantomComponent;

    class Ray_AIGround_ReceiveNormalHitAction : public Ray_AIGroundReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveNormalHitAction, Ray_AIGroundReceiveHitAction,2587773906);
        DECLARE_VALIDATE_ACTION()

    public:

        Ray_AIGround_ReceiveNormalHitAction();
        virtual ~Ray_AIGround_ReceiveNormalHitAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update( f32 _dt );
        virtual void                        onEvent( Event* _event);
        virtual void                        updateAnimInputs();

    protected:

        virtual void                        setupHitForce();
        virtual void                        setupRandomHitForce();
        virtual void                        updateHit( f32 _dt );

        StickToPolylinePhysComponent*       m_physComponent;
        class Ray_AIReceiveHitBehavior*     m_receiveHitBehavior;

    private:

        ITF_INLINE const class Ray_AIGround_ReceiveNormalHitAction_Template* getTemplate() const;

        void                                updateAntigrav( f32 _dt );
        void                                updateEjectAnim( f32 _dt );
        void                                updatePushBack( f32 _dt );

        f32                                 m_initialHeight;
        bbool                               m_speedReset;

        Vec2d                               m_pushbackPosOnHit;
        bbool                               m_updatePushBack;


        // runtime values
        bbool                               m_justEntered;
        f32                                 m_prevFrictionMultiplier;
        f32                                 m_prevGravityMultiplier;
        f32                                 m_antigravRampUpTimer;
        f32                                 m_fullAntigravTimer;
        f32                                 m_antigravRampDownTimer;
        f32                                 m_stunTimer;
        bbool                               m_isInAir;

        // anim inputs
        bbool                               m_stunned;
        f32                                 m_animCursor;           // only for LowEjecting hits (for now)

//#define DEBUG_HIT_TRAJECTORY
#ifdef DEBUG_HIT_TRAJECTORY
        Vec2d                               m_debugLastPos;
#endif
    };

    class Ray_AIGround_ReceiveNormalHitAction_Template : public Ray_AIGroundReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGround_ReceiveNormalHitAction_Template,Ray_AIGroundReceiveHitAction_Template,2224777414);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIGround_ReceiveNormalHitAction);

    public:

        Ray_AIGround_ReceiveNormalHitAction_Template();
        ~Ray_AIGround_ReceiveNormalHitAction_Template() {}

        f32                                 getFrictionMultiplier() const { return m_frictionMultiplier; }
        f32                                 getGravityMultiplier() const { return m_gravityMultiplier; }
        f32                                 getHitForce() const { return m_hitForce; }
        f32                                 getVerticalHitForce() const { return m_verticalHitForce; }
        f32                                 getRandomHitForce() const { return m_randomHitForce; }
        const Angle&                        getRandomAngleMax() const { return m_randomAngleMax; }
        f32                                 getAntiGravRampUpTime() const { return m_antigravRampUpTime; }
        f32                                 getFullAntiGravTime() const { return m_fullAntigravTime; }
        f32                                 getAntiGravRampDownTime() const { return m_antigravRampDownTime; }
        f32                                 getPushBackForce() const { return m_pushBackForce; }
        bbool                               getCanBlockHits() const { return m_canBlockHits; }
        f32                                 getMinStunTime() const { return m_minStunTime; }
        f32                                 getMaxStunTime() const { return m_maxStunTime; }
        f32                                 getEjectAnimSpeed() const { return m_ejectAnimSpeed; }
        f32                                 getBrakeForce() const { return m_brakeForce; }
        f32                                 getBrakeHeight() const { return m_brakeHeight; }
        f32                                 getSpeedLimit() const { return m_speedLimit; }
        bbool                               getMustFinishWithHurtTimer() const {return m_mustFinishWithHurtTimer;}
        ITF_INLINE bbool                    getResetSpeedAtStart() const {return m_resetSpeedAtStart;}
        ITF_INLINE bbool                    getOrientVerticalForceToGround() const { return m_orientVerticalForceToGround; }

    private:

        f32                                 m_frictionMultiplier;
        f32                                 m_gravityMultiplier;
        f32                                 m_hitForce;
        f32                                 m_verticalHitForce;
        f32                                 m_randomHitForce;       // a force can be added
        Angle                               m_randomAngleMax;       // with a random angle
        f32                                 m_antigravRampUpTime;
        f32                                 m_fullAntigravTime;
        f32                                 m_antigravRampDownTime;
        f32                                 m_pushBackForce;
        bbool                               m_canBlockHits;
        f32                                 m_minStunTime;
        f32                                 m_maxStunTime;
        f32                                 m_ejectAnimSpeed;       // only for LowEjecting hits (for now)
        f32                                 m_brakeForce;
        f32                                 m_brakeHeight;
        f32                                 m_speedLimit;
        bbool                               m_mustFinishWithHurtTimer;
        bbool                               m_resetSpeedAtStart;
        bbool                               m_orientVerticalForceToGround;
    };

    ITF_INLINE const class Ray_AIGround_ReceiveNormalHitAction_Template* Ray_AIGround_ReceiveNormalHitAction::getTemplate() const { return static_cast<const Ray_AIGround_ReceiveNormalHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_