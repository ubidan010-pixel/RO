#ifndef _ITF_SPEEDINPUTPROVIDERCOMPONENT_H_
#define _ITF_SPEEDINPUTPROVIDERCOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    class SpeedInputProviderComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SpeedInputProviderComponent, ActorComponent,3980326790);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        SpeedInputProviderComponent();
        virtual ~SpeedInputProviderComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onBecomeActive();
        virtual void Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class SpeedInputProviderComponent_Template* getTemplate() const;

        void sendInputEvent( const StringID& _input, const f32 _value );

        LinkComponent* m_linkComponent;

        Vec3d m_prevPos;
        f32 m_speed;    // made a member for debug
    };

    class SpeedInputProviderComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SpeedInputProviderComponent_Template, TemplateActorComponent,2751973800);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SpeedInputProviderComponent);

    public:

        SpeedInputProviderComponent_Template();
        ~SpeedInputProviderComponent_Template();

        ITF_INLINE StringID getSpeedInput() const { return m_speedInput; }
        ITF_INLINE StringID getSpeedXInput() const { return m_speedXInput; }
        ITF_INLINE StringID getSpeedYInput() const { return m_speedYInput; }
        ITF_INLINE StringID getSpeedZInput() const { return m_speedZInput; }

    private:

        StringID m_speedInput;
        StringID m_speedXInput;
        StringID m_speedYInput;
        StringID m_speedZInput;
    };

    ITF_INLINE const SpeedInputProviderComponent_Template* SpeedInputProviderComponent::getTemplate() const { return static_cast<const SpeedInputProviderComponent_Template*>(m_template); }
};

#endif //_ITF_SPEEDINPUTPROVIDERCOMPONENT_H_