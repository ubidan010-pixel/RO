#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#define _ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_RAY_SOFTCOLLISION_H_
#include "rayman/gameplay/AI/Utils/Ray_SoftCollision.h"
#endif //_ITF_RAY_SOFTCOLLISION_H_

namespace ITF
{
    class LinkComponent;
    class LinkCurveComponent;
    class AnimLightComponent;
    class AnimatedComponent;
    class PlayerDetectorComponent;

    /*
        Attach this to an actor that will act as the trunk of a tree.
        Have platforms linked as children/leafs, the trunk will open/close them
        (move them from its pos to their initial pos, along a bezier).
        A LinkCurveComponent will draw the trunk-to-leaf branches.

        If the leafs have a Ray_PlatformAIComponent, it will set their state
        (and they will play their own anims to appear, open etc).
    */
    class Ray_PlatformTreeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformTreeComponent, ActorComponent,4107374076)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlatformTreeComponent();
        virtual ~Ray_PlatformTreeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        onBecomeActive();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
                void        editor_setActivated( bbool _value );
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_PlatformTreeComponent_Template* getTemplate() const;

        class ChildData
        {
        public:
            ChildData()
                : m_startDelay(0.f)
                , m_duration(0.f)
                , m_time(0.f)
            {}

            f32     m_startDelay;

            f32     m_duration;
            f32     m_time;     // [0, 1] - lerped
        };
        typedef ITF_MAP<ActorRef, ChildData> ChildDataMap;

        ChildData&  getChildData( Actor* child );


        LinkComponent*      m_linkComponent;
        LinkCurveComponent* m_linkCurveComponent;
        AnimLightComponent* m_animComponent;
        AnimatedComponent*  m_animatedComponent;
        PlayerDetectorComponent* m_playerDetector;


        void        tryOpen();
        void        tryClose();
        void        applyStateForced( const bbool _open );
        void        forceOpen();
        void        forceClose();

        void        updateChildren( const f32 _dt );
        void        updateChild( const f32 _dt, ChildData& _childData, Actor* _child, const Vec2d& p0, const Vec2d& n0 );

        bbool       m_startActivated;
        f32         m_startDelayTimer;

        ChildDataMap m_childDataMap;


        enum State
        {
            State_Invalid,
            State_Open,
            State_Open_Wiggling,
            State_Closed,
            State_Closed_Wiggling,
            State_Opening_In,
            State_Opening_Loop1,    // wait for branches to open
            State_Opening_Loop2,    // extra timer
            State_Opening_Out,
            State_Closing_In,
            State_Closing_Loop1,    // wait for branches to close
            State_Closing_Loop2,    // extra timer
            State_Closing_Out,
            ENUM_FORCE_SIZE_32(State)
        };

        bbool       isActivated() const;
        void        forceChangeState( const State _newState ) { m_state = State_Invalid; changeState(_newState); }
        void        changeState( const State _newState );
        void        updateState( const f32 _dt );

        bbool       allChildrenOpen() const;
        bbool       allChildrenClosed() const;
        bbool       playAnim( const StringID _anim );
        void        setCollisionDisabled( const bbool _disable );

        State       m_state;
        f32         m_timer;

        void        detectWiggle();
        u32         m_playersInsideCount;

        Ray_SoftCollision m_softCollision;
    };

    //-------------------------------------------------------------------------------------
    class Ray_PlatformTreeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformTreeComponent_Template,TemplateActorComponent,485283312);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlatformTreeComponent);

    public:

        Ray_PlatformTreeComponent_Template();
        ~Ray_PlatformTreeComponent_Template() {}

        ITF_INLINE bbool getIgnoreEventTrigger() const { return m_ignoreEventTrigger; }
        f32             getMaxStartDelay() const { return m_maxStartDelay; }
        f32             getChildMoveSpeed() const { return m_childMoveSpeed; }
        f32             getChildMoveSpeedRandom() const { return m_childMoveSpeedRandom; }
        bbool           getFastMode() const { return m_fastMode; }
        f32             getChildAngleCorrectionDist() const { return m_childAngleCorrectionDist; }
        f32             getChildOneOverLinkFadeDist() const { return m_childOneOverLinkFadeDist; }
        f32             getChildAppearDist() const { return m_childAppearDist; }
        f32             getChildDisappearDist() const { return m_childDisappearDist; }
        f32             getChildOpenDist() const { return m_childOpenDist; }
        f32             getChildCloseDist() const { return m_childCloseDist; }
        const StringID& getAnimOpened() const { return m_animOpened; }
        const StringID& getAnimClosed() const { return m_animClosed; }
        const StringID& getAnimAppear() const { return m_animAppear; }
        const StringID& getAnimDisappear() const { return m_animDisappear; }
        const StringID& getAnimHidden() const { return m_animHidden; }
        ITF_INLINE f32  getOpeningDelay() const { return m_openingDelay; }
        ITF_INLINE f32  getClosingDelay() const { return m_closingDelay; }

        const Ray_SoftCollision_Template& getSoftCollision() const { return m_softCollision; }
        Ray_SoftCollision_Template& getSoftCollision() { return m_softCollision; }
        ITF_INLINE bbool getDisableCollisionDuringTransition() const { return m_disableCollisionDuringTransition; }
        ITF_INLINE bbool getDisableActorOnOpen() const { return m_disableActorOnOpen; }

        bbool getCanWiggle() const { return m_canWiggle; }
        const StringID& getPadRumbleWiggle() const { return m_padRumbleWiggle; }
        const StringID& getAnimWiggleOpened() const { return m_animWiggleOpened; }
        const StringID& getAnimWiggleClosed() const { return m_animWiggleClosed; }

        virtual void    onTemplateLoaded();

    private:

        bbool       m_ignoreEventTrigger;
        f32         m_maxStartDelay;
        f32         m_childMoveSpeed;           // average speed; used to determine the child duration
        f32         m_childMoveSpeedRandom;     // anemone only: used to randomize the speed
        bbool       m_fastMode;                 // false (default): smooth accelerate and smooth decelerate, true: fast accelerate and smooth decelerate

        f32         m_childAngleCorrectionDist; // % of length - when the child reaches the end of its curve, its tangent is not its final angle: compensate
        f32         m_childLinkFadeDist;        // % of length - fade the link below this distance
        f32         m_childOneOverLinkFadeDist; // onTemplateLoaded
        f32         m_childAppearDist;          // % of length
        f32         m_childDisappearDist;       // % of length
        f32         m_childOpenDist;            // % of length - only used by child PlatformAI
        f32         m_childCloseDist;           // % of length - only used by child PlatformAI

        StringID    m_animOpened;   // IdleOpen
        StringID    m_animClosed;   // IdleClosed
        StringID    m_animAppear;   // Appear
        StringID    m_animDisappear;// Disappear
        StringID    m_animHidden;   // Hidden
        f32         m_openingDelay; // how long we stay hidden after opening
        f32         m_closingDelay; // how long we stay hidden after closing

        bbool       m_canWiggle;                // wiggle when player passing by?
        StringID    m_padRumbleWiggle;          // pad rumble to start when wiggling
        StringID    m_animWiggleOpened;         // wiggle anim when opened
        StringID    m_animWiggleClosed;         // wiggle anim when closed

        Ray_SoftCollision_Template m_softCollision;
        bbool       m_disableCollisionDuringTransition;
        bbool       m_disableActorOnOpen;       // for PlatformMan
    };

    ITF_INLINE const class Ray_PlatformTreeComponent_Template * Ray_PlatformTreeComponent::getTemplate() const
    {
        return static_cast<const class Ray_PlatformTreeComponent_Template *>(m_template);
    }

};

#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_