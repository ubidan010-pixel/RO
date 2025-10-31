#ifndef _ITF_SYNCHRONIZEDANIMCOMPONENT_H_
#define _ITF_SYNCHRONIZEDANIMCOMPONENT_H_

namespace ITF
{
    /*
        By default, this component plays the activeAnim on start.

        Link it to an ActivateTrigger, it will play the inactiveAnim.
        Activate the trigger: it will play the activeAnim.

        If the anims are synchronized on the metronome,
        they will get offsetted once activated (done in AnimLightComponent directly).
     */
    class SynchronizedAnimComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SynchronizedAnimComponent, ActorComponent,4229263636);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        SynchronizedAnimComponent();
        virtual ~SynchronizedAnimComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event );

    private:
        ITF_INLINE const class SynchronizedAnimComponent_Template* getTemplate() const;

        AnimLightComponent* m_animComponent;
    };


    //---------------------------------------------------------------------------------------------------

    class SynchronizedAnimComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SynchronizedAnimComponent_Template, TemplateActorComponent,1810829771);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SynchronizedAnimComponent);

    public:

        SynchronizedAnimComponent_Template()
        {
        }
        virtual ~SynchronizedAnimComponent_Template() {}

        const StringID& getInactiveAnim() const { return m_inactiveAnim; }
        const StringID& getActiveAnim() const { return m_activeAnim; }

    private:

        StringID m_inactiveAnim;
        StringID m_activeAnim;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const SynchronizedAnimComponent_Template* SynchronizedAnimComponent::getTemplate() const
    {
        return static_cast<const SynchronizedAnimComponent_Template*>(m_template);
    }
};

#endif //_ITF_SYNCHRONIZEDANIMCOMPONENT_H_