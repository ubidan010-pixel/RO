#ifndef _ITF_ANIMTRIGGEREDCOMPONENT_H_
#define _ITF_ANIMTRIGGEREDCOMPONENT_H_

namespace ITF
{
    class AnimTriggeredComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimTriggeredComponent,ActorComponent,240414993);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AnimTriggeredComponent();
        ~AnimTriggeredComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );

    private:


        ITF_INLINE const class AnimTriggeredComponent_Template*  getTemplate() const;

        class AnimatedComponent*    m_animComponent;
        class DetectorComponent*    m_detectorComponent;
        bbool                       m_triggered;
    };

    //---------------------------------------------------------------------------------------------------

    class AnimTriggeredComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimTriggeredComponent_Template,TemplateActorComponent,1411916197);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimTriggeredComponent);

    public:

        AnimTriggeredComponent_Template() {}
        ~AnimTriggeredComponent_Template() {}

        const StringID&             getTriggerActionId() const { return m_triggerActionId; }
        const StringID&             getUntriggerActionId() const { return m_untriggerActionId; }

    private:

        StringID                    m_triggerActionId;
        StringID                    m_untriggerActionId;
    };



    ITF_INLINE const AnimTriggeredComponent_Template*  AnimTriggeredComponent::getTemplate() const {return static_cast<const AnimTriggeredComponent_Template*>(m_template);}

}

#endif // _ITF_ANIMTRIGGEREDCOMPONENT_H_

