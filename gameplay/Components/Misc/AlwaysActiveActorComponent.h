#ifndef _ITF_ALWAYSACTIVEACTORCOMPONENT_H_
#define _ITF_ALWAYSACTIVEACTORCOMPONENT_H_

namespace ITF
{
    class AlwaysActiveActorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AlwaysActiveActorComponent, ActorComponent,3338470990);
        DECLARE_VALIDATE_COMPONENT()

    public:
        DECLARE_SERIALIZE()

		AlwaysActiveActorComponent();
		virtual ~AlwaysActiveActorComponent();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }
        //virtual void    onBecomeInactive();
        virtual void    onUnloadResources();
        virtual void    onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion );

		virtual void    Update( f32 _dt );
        virtual void    onEvent( Event * _event );

    private:
    
        void    AddToAlwaysActiveActors();
        void    removeFromAlwaysActiveActors( Scene* _pScene = NULL );


        f32     m_safeTimeOutTimer;
        bbool   m_started;
        bbool   m_enteredScreen;
        //bbool   m_needModifiedRequest;

        ITF_INLINE const class AlwaysActiveActorComponent_Template*  getTemplate() const;
    };

    //---------------------------------------------------------------------------------------------------

    class AlwaysActiveActorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AlwaysActiveActorComponent_Template, TemplateActorComponent,1432225983);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AlwaysActiveActorComponent);

    public:

        AlwaysActiveActorComponent_Template();
        virtual ~AlwaysActiveActorComponent_Template();

        ITF_INLINE const Event  *       getStartEvent() const               { return m_startEvent; }
        ITF_INLINE const Event  *       getStopEvent() const                { return m_stopEvent; }
        ITF_INLINE const f32            getSafeTimeOutDuration() const      { return m_safeTimeOutDuration; }
        ITF_INLINE const bbool          isStoppingOnScreenLeaving() const   { return m_stopOnScreenLeaving; }
    
    private:
        Event    *  m_startEvent;           // event which set the actor has always active
        Event    *  m_stopEvent;            // event which remove the actor has always active

        f32         m_safeTimeOutDuration;  // Duration before auto-removing from always active actor. -1 is not time out
        bbool       m_stopOnScreenLeaving;  // Remove from always active actor list if leaving the current screen. (must have entered screen before)
		
    };


    const AlwaysActiveActorComponent_Template*  AlwaysActiveActorComponent::getTemplate() const {return static_cast<const AlwaysActiveActorComponent_Template*>(m_template);}
}

#endif // _ITF_ALWAYSACTIVEACTORCOMPONENT_H_

