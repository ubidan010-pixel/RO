#ifndef _ITF_RAY_PLATFORMAICOMPONENT_H_
#define _ITF_RAY_PLATFORMAICOMPONENT_H_

namespace ITF
{
    class PolylineComponent;
    class Ray_TriggerBounceComponent;
    class Ray_PlatformTreeComponent_Template;

    class Ray_PlatformAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformAIComponent,ActorComponent,1208901062)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PlatformAIComponent();
        virtual ~Ray_PlatformAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event);
        virtual void        Update( f32 _dt );

                bbool       canMove( const bbool _activated ) const;
                void        setState( const bbool _activated, const f32 _cursor, const bbool _forceSet, const Ray_PlatformTreeComponent_Template* _treeParams );
                void        setTargetCoordinates( const Vec2d& _pos, const f32 _angle );
                bbool       isFree() const;

    private:
        ITF_INLINE const class Ray_PlatformAIComponent_Template * getTemplate() const;

        enum State
        {
            State_Hidden,
            State_Closed,
            State_Open,
            State_OpenFull,   // PlatformMan has 2 open states: normal (attracting), and full open
        };

        AnimLightComponent* m_animComponent;
        AnimatedComponent*  m_animatedComponent;
        PolylineComponent*  m_polylineComponent;
        Ray_TriggerBounceComponent* m_triggerBounceComponent;

        u32                 m_activateCount;
        u32                 m_openCount;
        u32                 m_openFullCount;
        f32                 m_hitTimer;
        f32                 m_currentWeight;

        State               m_targetState;
        State               m_state;
        StringID            m_currentTransition;
        f32                 m_transitionQueryTimer;
        bbool               m_hasBeenVacuumed;

        void                onActivate();
        void                onDeactivate();
        void                onOpen();
        void                onClose();
        void                onOpenFull();
        void                onCloseFull();
        void                onReceiveHit( HitStim* _hit );

        State               evaluateState();
        bbool               playTransitionToNewState( f32 _dt );
        void                playNewStateIdleAnim();
        void                onTransitionFinished();

        void                playTransition( const StringID& _transition );
        bbool               playTransition( const StringID& _transition, const StringID& _reverseTransition, bbool _synchronize );


        bbool               m_dirtyCoordinates;
        Vec2d               m_targetPos;
        f32                 m_targetAngle;

        void                updateCoordinates( f32 _dt );
    };

    //-------------------------------------------------------------------------------------
    class Ray_PlatformAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlatformAIComponent_Template,TemplateActorComponent,3933484897);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlatformAIComponent);

    public:

        Ray_PlatformAIComponent_Template();
        ~Ray_PlatformAIComponent_Template();

        bbool               getStartActivated() const { return m_startActivated; }
        bbool               getStartOpened() const { return m_startOpened; }
        const Event*        getActivateEvent() const { return m_activateEvent; }
        const StringID&     getActivateTransition() const { return m_activateTransition; }
        const StringID&     getActivateIdle() const { return m_activateIdle; }
        const Event*        getDeactivateEvent() const { return m_deactivateEvent; }
        const StringID&     getDeactivateTransition() const { return m_deactivateTransition; }
        const StringID&     getDeactivateIdle() const { return m_deactivateIdle; }
        f32                 getDeactivateTimeHysteresis() const { return m_deactivateTimeHysteresis; }
        bbool               getSynchronizeActivationTransitions() const { return m_synchronizeActivationTransitions; }
        const Event*        getOpenEvent() const { return m_openEvent; }
        const StringID&     getOpenTransition() const { return m_openTransition; }
        const StringID&     getOpenIdle() const { return m_openIdle; }
        const StringID&     getOpenCarryingIdle() const { return m_openCarryingIdle; }
        const Event*        getCloseEvent() const { return m_closeEvent; }
        const StringID&     getCloseTransition() const { return m_closeTransition; }
        f32                 getCloseTimeHysteresis() const { return m_closeTimeHysteresis; }
        bbool               getSynchronizeOpenCloseTransitions() const { return m_synchronizeOpenCloseTransitions; }
        const Event*        getOpenFullEvent() const { return m_openFullEvent; }
        const StringID&     getOpenFullTransition() const { return m_openFullTransition; }
        const StringID&     getOpenFullIdle() const { return m_openFullIdle; }
        const Event*        getCloseFullEvent() const { return m_closeFullEvent; }
        const StringID&     getCloseFullTransition() const { return m_closeFullTransition; }
        bbool               getSynchronizeOpenCloseFullTransition() const { return m_synchronizeOpenCloseFullTransitions; }
        const StringID&     getWeightIncreaseTransition() const { return m_weightIncreaseTransition; }
        const StringID&     getWeightDecreaseTransition() const { return m_weightDecreaseTransition; }
        f32                 getWeightThreshold() const { return m_weightThreshold; }
        ITF_INLINE bbool    getCloseOnHit() const { return m_closeOnHit; }
        ITF_INLINE StringID getCloseOnHitTransition() const { return m_closeOnHitTransition; }
        ITF_INLINE f32      getCloseOnHitDuration() const { return m_closeOnHitDuration; }
        ITF_INLINE bbool    getManageVacuum() const { return m_manageVacuum; }

    private:

        bbool               m_startActivated;
        bbool               m_startOpened;
        Event*              m_activateEvent;
        StringID            m_activateTransition;
        StringID            m_activateIdle;
        Event*              m_deactivateEvent;
        StringID            m_deactivateTransition;
        StringID            m_deactivateIdle;
        f32                 m_deactivateTimeHysteresis;
        bbool               m_synchronizeActivationTransitions;
        Event*              m_openEvent;
        StringID            m_openTransition;
        StringID            m_openIdle;
        StringID            m_openCarryingIdle;
        Event*              m_closeEvent;
        StringID            m_closeTransition;
        f32                 m_closeTimeHysteresis;
        bbool               m_synchronizeOpenCloseTransitions;
        bbool               m_closeOnHit;
        StringID            m_closeOnHitTransition;
        f32                 m_closeOnHitDuration;
        Event*              m_openFullEvent;
        StringID            m_openFullTransition;
        StringID            m_openFullIdle;
        Event*              m_closeFullEvent;
        StringID            m_closeFullTransition;
        bbool               m_synchronizeOpenCloseFullTransitions;
        StringID            m_weightIncreaseTransition;
        StringID            m_weightDecreaseTransition;
        f32                 m_weightThreshold;
        bbool               m_manageVacuum;
    };
    ITF_INLINE const class Ray_PlatformAIComponent_Template * Ray_PlatformAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_PlatformAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_PLATFORMAICOMPONENT_H_
