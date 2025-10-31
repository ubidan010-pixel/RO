#ifndef _ITF_PLAYANIMONWEIGHTCHANGECOMPONENT_H_
#define _ITF_PLAYANIMONWEIGHTCHANGECOMPONENT_H_

namespace ITF
{
    class SoundComponent;
    class PolylineComponent;
    class LinkComponent;
    class Event;

    /* Play an animation/action upon entering a trigger,
     * or when weight changes (enter = increase, exit = decrease).
     * Nothing happens if weight is below an (optional) weight threshold.
     *
     * Requires:
     *  - an AnimLightComponent/Animated for animations/actions,
     *  - an (external) SequencePlayerComponent for sequences (referenced by its friendlyName).
     */
    class PlayAnimOnWeightChangeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnWeightChangeComponent,ActorComponent,2409137217);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PlayAnimOnWeightChangeComponent();
        virtual ~PlayAnimOnWeightChangeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		virtual void		onBecomeInactive();
        virtual void        onEvent( Event* _event);
        virtual void        Update( f32 _dt );

    private:

        ITF_INLINE const class PlayAnimOnWeightChangeComponent_Template*  getTemplate() const;

        AnimLightComponent* m_animComponent;
        AnimatedComponent*  m_animatedComponent;
        SoundComponent*     m_soundComponent;
        LinkComponent*      m_linkComponent;

        bbool               m_disabled;
        f32                 m_currentWeight;
        f32                 m_newWeight;
        bbool               m_isEntering;
        bbool               m_isExiting;

        void                onTrigger( bbool _enter );
        void                sendEventToChilds(bbool _enter);
        bbool               playAnim( const StringID& _animation );
        bbool               isAnimFinished() const;

        Event*              m_onEnterEvent;
        Event*              m_onExitEvent;
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class PlayAnimOnWeightChangeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnWeightChangeComponent_Template,TemplateActorComponent,1203810110);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayAnimOnWeightChangeComponent);

    public:

        PlayAnimOnWeightChangeComponent_Template();
        ~PlayAnimOnWeightChangeComponent_Template();

        const StringID&     getEnterAnim() const            { return m_enterAnim; }
        const StringID&     getEnterAnimIdle() const        { return m_enterAnimIdle; }
        const StringID&     getExitAnim() const             { return m_exitAnim; }
        const StringID&     getExitAnimIdle() const         { return m_exitAnimIdle; }
        bbool               getListenToTrigger() const      { return m_listenToTrigger; }
        bbool               getListenToWeight() const       { return m_listenToWeight; }
        Event*              getListenToEvent() const        { return m_listenToEvent; }
        bbool               getStayOnEnter() const          { return m_stayOnEnter; }
        f32                 getWeightThreshold() const      { return m_weightThreshold; }

        const Event*        getOnEnterEvent() const         { return m_onEnterEvent; }
        const Event*        getOnExitEvent() const          { return m_onExitEvent; }

    private:

        StringID            m_enterAnim;            // animation to play on entering trigger or polyline
        StringID            m_enterAnimIdle;        // animation to play after entering trigger or polyline
        StringID            m_exitAnim;             // animation to play on exiting trigger or polyline
        StringID            m_exitAnimIdle;         // animation to play after exiting trigger or polyline
        bbool               m_listenToTrigger;
        bbool               m_listenToWeight;
        Event*              m_listenToEvent;
        bbool               m_stayOnEnter;
        f32                 m_weightThreshold;

        Event*              m_onEnterEvent;
        Event*              m_onExitEvent;
    };



    ITF_INLINE const PlayAnimOnWeightChangeComponent_Template*  PlayAnimOnWeightChangeComponent::getTemplate() const {return static_cast<const PlayAnimOnWeightChangeComponent_Template*>(m_template);}


}

#endif // _ITF_PLAYANIMONWEIGHTCHANGECOMPONENT_H_

