#ifndef _ITF_RAY_BTACTIONRECEIVEHIT_H_
#define _ITF_RAY_BTACTIONRECEIVEHIT_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionReceiveHit : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionReceiveHit,BTAction,2540357493);

    public:

        Ray_BTActionReceiveHit();
        ~Ray_BTActionReceiveHit();

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual bbool                           update( f32 _dt );
        virtual void                            updateAnimInput();
        virtual void                            onEvent(Event * _event);

    private:

        const class Ray_BTActionReceiveHit_Template*  getTemplate() const;

        void                                    setupHit( bbool _restart );
        void                                    setupPunchFront( bbool _restart );
        void                                    setupPunchUp( bbool _restart );
        void                                    setupHurtBounce( bbool _restart );
        void                                    setupEarthquake( bbool _restart );

        void                                    updatePunchFront( f32 _dt );
        void                                    updatePunchUp( f32 _dt );
        void                                    updateHurtBounce( f32 _dt );
        void                                    updateEarthquake( f32 _dt );
        void                                    updateCrashed( f32 _dt );

        void                                    updateSwimStance( f32 _dt );

        bbool                                   checkFinishedPunchFront() const;
        bbool                                   checkFinishedPunchUp() const;
        bbool                                   checkFinishedHurtBounce() const;
        bbool                                   checkFinishedEarthquake() const;
        bbool                                   checkFinishedCrashed() const;

        void                                    setCrashAnim();
        bbool                                   checkFinished() const;

        AnimatedComponent*                      m_animComponent;
        class FXControllerComponent*            m_fxController;
        class GroundAIControllerComponent*      m_aiController;
        class StickToPolylinePhysComponent*     m_physComponent;

        ObjectRef                               m_instigator;
        u32                                     m_hitLevel;
        Vec3d                                   m_hitFxPos;
        u32                                     m_reactType;
        Vec2d                                   m_hitDir;
        StringID                                m_feedBackAction;

        f32                                     m_totalTime;
        f32                                     m_timeCounter;
        ObjectRef                               m_blockedPolyline;
        u32                                     m_blockedEdgeIndex;
        f32                                     m_crashedDist;
        bbool                                   m_crashedVertical;
        Vec2d                                   m_currentSpeed;
        bbool                                   m_wasBlocked;
        bbool                                   m_appexStart;
        bbool                                   m_appexStop;
        bbool                                   m_appex;
        bbool                                   m_crashed;
    };

    class Ray_BTActionReceiveHit_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionReceiveHit_Template,BTAction_Template,104441640);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionReceiveHit);

    public:

        Ray_BTActionReceiveHit_Template();
        ~Ray_BTActionReceiveHit_Template() {}

        StringID                    getAnim() const { return m_anim; }

        f32                         getReceivePunchFrontWeakForce() const { return m_receivePunchFrontWeakForce; }
        f32                         getReceivePunchFrontStrongForce() const { return m_receivePunchFrontStrongForce; }
        f32                         getReceivePunchFrontMegaForce() const { return m_receivePunchFrontMegaForce; }
        f32                         getReceivePunchFrontWeakFrictionMultiplier() const { return m_receivePunchFrontWeakFrictionMult; }
        f32                         getReceivePunchFrontStrongFrictionMultiplier() const { return m_receivePunchFrontStrongFrictionMult; }
        f32                         getReceivePunchFrontMegaFrictionMultiplier() const { return m_receivePunchFrontMegaFrictionMult; }
        f32                         getReceivePunchFrontWeakTime() const { return m_receivePunchFrontWeakTime; }
        f32                         getReceivePunchFrontStrongTime() const { return m_receivePunchFrontStrongTime; }
        f32                         getReceivePunchFrontMegaTime() const { return m_receivePunchFrontMegaTime; }
        f32                         getReceivePunchUpWeakForce() const { return m_receivePunchUpWeakForce; }
        f32                         getReceivePunchUpStrongForce() const { return m_receivePunchUpStrongForce; }
        f32                         getReceivePunchUpMegaForce() const { return m_receivePunchUpMegaForce; }
        f32                         getReceivePunchUpWeakGravityMultiplier() const { return m_receivePunchUpWeakGravityMult; }
        f32                         getReceivePunchUpStrongGravityMultiplier() const { return m_receivePunchUpStrongGravityMult; }
        f32                         getReceivePunchUpMegaGravityMultiplier() const { return m_receivePunchUpMegaGravityMult; }
        f32                         getReceiveEarthquakeWeakForce() const { return m_receiveEarthquakeWeakForce; }
        f32                         getReceiveEarthquakeStrongForce() const { return m_receiveEarthquakeStrongForce; }
        f32                         getReceiveEarthquakeMegaForce() const { return m_receiveEarthquakeMegaForce; }
        f32                         getReceiveBounceVWeakForce() const { return m_receiveBounceVWeakForce; }
        f32                         getReceiveBounceVStrongForce() const { return m_receiveBounceVStrongForce; }
        f32                         getReceiveBounceVMegaForce() const { return m_receiveBounceVMegaForce; }
        f32                         getReceiveBounceHWeakForce() const { return m_receiveBounceHWeakForce; }
        f32                         getReceiveBounceHStrongForce() const { return m_receiveBounceHStrongForce; }
        f32                         getReceiveBounceHMegaForce() const { return m_receiveBounceHMegaForce; }
        f32                         getReceiveBounceWeakTime() const { return m_receiveBounceWeakTime; }
        f32                         getReceiveBounceStrongTime() const { return m_receiveBounceStrongTime; }
        f32                         getReceiveBounceMegaTime() const { return m_receiveBounceMegaTime; }
        f32                         getReceiveBounceWeakFrictionMult() const { return m_receiveBounceWeakFrictionMult; }
        f32                         getReceiveBounceStrongFrictionMult() const { return m_receiveBounceStrongFrictionMult; }
        f32                         getReceiveBounceMegaFrictionMult() const { return m_receiveBounceMegaFrictionMult; }

    private:

        StringID                    m_anim;

        f32                         m_receivePunchFrontWeakForce;       // Hit force applied when receiving a front punch weak
        f32                         m_receivePunchFrontStrongForce;     // Hit force applied when receiving a front punch strong
        f32                         m_receivePunchFrontMegaForce;       // Hit force applied when receiving a front punch mega
        f32                         m_receivePunchFrontWeakFrictionMult;// Friction multiplier when receiving a punch front weak hit
        f32                         m_receivePunchFrontStrongFrictionMult;// Friction multiplier when receiving a punch front strong hit
        f32                         m_receivePunchFrontMegaFrictionMult;// Friction multiplier when receiving a punch front mega hit
        f32                         m_receivePunchFrontWeakTime;        // How long we stay on the punch front weak hit
        f32                         m_receivePunchFrontStrongTime;      // How long we stay on the punch front strong hit
        f32                         m_receivePunchFrontMegaTime;        // How long we stay on the punch front mega hit
        f32                         m_receivePunchUpWeakForce;          // Ejecting Y force on the character when receiving a punch up weak hit
        f32                         m_receivePunchUpStrongForce;        // Ejecting Y force on the character when receiving a punch up strong hit
        f32                         m_receivePunchUpMegaForce;          // Ejecting Y force on the character when receiving a punch up strong hit
        f32                         m_receivePunchUpWeakGravityMult;    // Gravity multiplier when receiving a punch up weak
        f32                         m_receivePunchUpStrongGravityMult;  // Gravity multiplier when receiving a punch up strong
        f32                         m_receivePunchUpMegaGravityMult;    // Gravity multiplier when receiving a punch up mega
        f32                         m_receiveEarthquakeWeakForce;       // Down force when performing the weak earthquake on someone in the air
        f32                         m_receiveEarthquakeStrongForce;     // Down force when performing the strong earthquake on someone in the air
        f32                         m_receiveEarthquakeMegaForce;       // Down force when performing the mega earthquake on someone in the air
        f32                         m_receiveBounceVWeakForce;          // Force of the weak hurt bounce
        f32                         m_receiveBounceVStrongForce;        // Force of the strong hurt bounce
        f32                         m_receiveBounceVMegaForce;          // Force of the mega hurt bounce
        f32                         m_receiveBounceHWeakForce;          // Force of the weak hurt bounce
        f32                         m_receiveBounceHStrongForce;        // Force of the strong hurt bounce
        f32                         m_receiveBounceHMegaForce;          // Force of the mega hurt bounce
        f32                         m_receiveBounceWeakTime;            // How long we stay in hurt bounce weak
        f32                         m_receiveBounceStrongTime;          // How long we stay in hurt bounce strong
        f32                         m_receiveBounceMegaTime;            // How long we stay in hurt bounce mega
        f32                         m_receiveBounceWeakFrictionMult;    // Friction multiplier when receiving a hurt bounce weak
        f32                         m_receiveBounceStrongFrictionMult;  // Friction multiplier when receiving a hurt bounce strong
        f32                         m_receiveBounceMegaFrictionMult;    // Friction multiplier when receiving a hurt bounce mega
    };

    ITF_INLINE const Ray_BTActionReceiveHit_Template*  Ray_BTActionReceiveHit::getTemplate() const {return static_cast<const Ray_BTActionReceiveHit_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONRECEIVEHIT_H_
