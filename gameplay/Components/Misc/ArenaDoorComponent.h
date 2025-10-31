#ifndef _ITF_ARENADOORCOMPONENT_H_
#define _ITF_ARENADOORCOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    class ArenaDoorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ArenaDoorComponent, ActorComponent,1615905805);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ArenaDoorComponent();
        virtual ~ArenaDoorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

    protected:

        virtual void        openDoor();

        LinkComponent*      m_linkComponent;
        AnimLightComponent* m_animComponent;

        bbool               m_justStarted;
        bbool               m_isOpen;

    private:
        ITF_INLINE const class ArenaDoorComponent_Template*  getTemplate() const;

        bbool               queryIsDead( Actor* _child );

    };

    //-------------------------------------------------------------------------------------
    class ArenaDoorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ArenaDoorComponent_Template,TemplateActorComponent,3452134421);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ArenaDoorComponent);

    public:

        ArenaDoorComponent_Template();
        ~ArenaDoorComponent_Template() {}

        const StringID&     getOpenAnim() const { return m_openAnim; }

    protected:

        StringID            m_openAnim;
    };

    const ArenaDoorComponent_Template*  ArenaDoorComponent::getTemplate() const {return static_cast<const ArenaDoorComponent_Template*>(m_template);}
};

#endif //_ITF_ARENADOORCOMPONENT_H_