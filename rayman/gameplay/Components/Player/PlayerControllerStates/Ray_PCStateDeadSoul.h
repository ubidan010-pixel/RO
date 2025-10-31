#ifndef _ITF_RAY_PCSTATEDEADSOUL_H_
#define _ITF_RAY_PCSTATEDEADSOUL_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_


    class StateDeadSoul_Template;
    class StateDeadSoul : public Ray_State
    {
        DECLARE_OBJECT_CHILD_RTTI(StateDeadSoul, Ray_State,2386069883)

    public:

        StateDeadSoul();
        virtual ~StateDeadSoul();

        virtual StringID getAnimAction() const
        {
            return ITF_GET_STRINGID_CRC(DEADSOUL,3042818582);
        }

        virtual void    onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void    onEnter();
        virtual void    onExit();
        virtual bbool   checkStateChange( f32 _dt );
        virtual void    update( f32 _dt );

        void            processPunch( const PunchStim* _punch );
        void            processCrush( EventCrushed* _crushed );

        void            triggerDeath();
        void            reviveOnFirstLivePlayer();

    private:
        ITF_INLINE const StateDeadSoul_Template* getTemplate() const;

        void            move( f32 _dt );
        void            clamp( f32 _dt );

        void            moveToPoint();

        void            startRevive( ActorRef _otherPlayer, const Vec2d& _dir );
        bbool           isActorGoodForRevive( Actor* _actor ) const;

        Vec2d           m_moveToPointTargetCurrent;
        bbool           m_doingMoveToPoint;

        Vec2d           m_moveInput;
        f32             m_currentAngle;
        f32             m_targetAngle;

        ActorRef        m_revivePlayer;
        Vec2d           m_reviveDir;
        Vec2d           m_revivePos;
        f32             m_inactiveTime;

        Actor*          getClosestRangePlayer();
    };

    class StateDeadSoul_Template
    {
        DECLARE_SERIALIZE();

    public:

        StateDeadSoul_Template();
        virtual ~StateDeadSoul_Template();

        const PhysShape* getPhantomShape() const { return m_phantomShape; }
        ITF_INLINE f32 getSoftCollisionRadiusMultiplier() const { return m_softCollisionRadiusMultiplier; }
        ITF_INLINE bbool getIgnoreCollisions() const { return m_ignoreCollisions; }

        ITF_INLINE f32 getMinLocalSpeed() const { return m_minLocalSpeed; }
        ITF_INLINE f32 getMaxLocalSpeed() const { return m_maxLocalSpeed; }
        ITF_INLINE f32 getMaxLocalSpeedWithBoost() const { return m_maxLocalSpeedWithBoost; }
        ITF_INLINE f32 getInputInertia() const { return m_inputInertia; }
        ITF_INLINE f32 getCamSpeedInfluence() const { return m_camSpeedInfluence; }

        ITF_INLINE f32 getEjectFromBorderForce() const { return m_ejectFromBorderForce; }
        ITF_INLINE const Vec2d& getEjectMargin() const { return m_ejectMargin; }

        //ITF_INLINE f32 getMinSpeedForRotation() const { return m_minSpeedForRotation; }
        ITF_INLINE f32 getMinInputForRotation() const { return m_minInputForRotation; }
        ITF_INLINE f32 getMaxAngle() const { return m_maxAngle.ToRadians(); }
        ITF_INLINE f32 getAngularSmoothMinSpeed() const { return m_angularSmoothMinSpeed; }
        ITF_INLINE f32 getAngularSmoothMaxSpeed() const { return m_angularSmoothMaxSpeed; }
        ITF_INLINE f32 getAngularSmoothMinValue() const { return m_angularSmoothMinValue; }
        ITF_INLINE f32 getAngularSmoothMaxValue() const { return m_angularSmoothMaxValue; }

        ITF_INLINE bbool getUseTrail() const { return m_useTrail; }

        void onTemplateLoaded();

    private:

        PhysShape*      m_phantomShape;
        f32             m_softCollisionRadiusMultiplier;
        bbool           m_ignoreCollisions;

        // the force applied with the joystick aims to reach that speed
        f32             m_minLocalSpeed;
        f32             m_maxLocalSpeed;
        f32             m_maxLocalSpeedWithBoost;           // when boosting (aka sprinting) with right trigger
        f32             m_inputInertia;
        f32             m_camSpeedInfluence;                // [0, 1]

        // screen clamping
        f32             m_ejectFromBorderForce;             // move away from screen borders
        Vec2d           m_ejectMargin;                      // push the player away if he's too close to the screen edges

        //f32             m_minSpeedForRotation;
        f32             m_minInputForRotation;
        Angle           m_maxAngle;
        f32             m_angularSmoothMinSpeed;
        f32             m_angularSmoothMaxSpeed;
        f32             m_angularSmoothMinValue;
        f32             m_angularSmoothMaxValue;

        bbool           m_useTrail;
    };



#endif //_ITF_RAY_PCSTATEDEADSOUL_H_