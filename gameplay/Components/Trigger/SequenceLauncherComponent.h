#ifndef _ITF_SEQUENCETRIGGERCOMPONENT_H_
#define _ITF_SEQUENCETRIGGERCOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    /*
        Upon listening to an EventTrigger:
            - prevents players from joining
            - waits for players to be ready
            - starts a sequence
    */
    class SequenceLauncherComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SequenceLauncherComponent, ActorComponent,759529471);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        SequenceLauncherComponent();
        virtual ~SequenceLauncherComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent(Event * _event);
        virtual void        Update( f32 _dt );

    private:
        ITF_INLINE const class SequenceLauncherComponent_Template* getTemplate() const;

        LinkComponent* m_linkComponent;

        void setLock( bbool _lock );
        bbool arePlayersReady();
        void playSequence();

        bbool m_locked;
    };


    //---------------------------------------------------------------------------------------------------

    class SequenceLauncherComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SequenceLauncherComponent_Template, TemplateActorComponent,1287162758);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SequenceLauncherComponent);

    public:

        SequenceLauncherComponent_Template()
        {
        }
        virtual ~SequenceLauncherComponent_Template() {}

        const EventSequenceControl& getEventPlaySequence() const { return m_eventPlaySequence; }

    private:

        EventSequenceControl m_eventPlaySequence;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const SequenceLauncherComponent_Template* SequenceLauncherComponent::getTemplate() const
    {
        return static_cast<const SequenceLauncherComponent_Template*>(m_template);
    }
};

#endif //_ITF_SEQUENCETRIGGERCOMPONENT_H_