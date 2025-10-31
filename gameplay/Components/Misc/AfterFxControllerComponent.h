#ifndef _ITF_AFTERFXCONTROLLERCOMPONENT_H_
#define _ITF_AFTERFXCONTROLLERCOMPONENT_H_

namespace ITF
{
    class AfterFxComponent;

    class AfterFxControllerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AfterFxControllerComponent, ActorComponent,3876698127);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AfterFxControllerComponent();
        virtual ~AfterFxControllerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        const class AfterFxControllerComponent_Template* getTemplate() const;

        void manageActivationFromRangeRatio(const f32 _rangeRatio);

        AfterFxComponent*   m_afterFxComponent;
        bbool m_outOfRange;
    };

    class AfterFxControllerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AfterFxControllerComponent_Template,TemplateActorComponent,1843163453);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AfterFxControllerComponent);

    public:

        AfterFxControllerComponent_Template();
        ~AfterFxControllerComponent_Template();
        ITF_INLINE ITF::f32 getMinRange() const { return m_minRange; }
        ITF_INLINE ITF::f32 getMaxRange() const { return m_maxRange; }
        ITF_INLINE Color getMinColor() const { return m_minColor; }
        ITF_INLINE Color getMaxColor() const { return m_maxColor; }
        ITF_INLINE const AABB & getBoxShape() const { return m_boxShape; }
        ITF_INLINE bbool useCircle() const { return m_useCircle; }
        ITF_INLINE bbool useBox() const { return m_useBox; }
        ITF_INLINE bbool inactiveWhenOutOfRange() const { return m_inactiveWhenOutOfRange; }

    private:
        f32     m_minRange;
        f32     m_maxRange;
        Color   m_minColor;
        Color   m_maxColor;
        AABB    m_boxShape;
        bbool   m_useCircle;
        bbool   m_useBox;
        bbool   m_inactiveWhenOutOfRange;
    };

    ITF_INLINE const AfterFxControllerComponent_Template* AfterFxControllerComponent::getTemplate() const
    {
        return static_cast<const AfterFxControllerComponent_Template*>(m_template);
    }
};

#endif //_ITF_AFTERFXCONTROLLERCOMPONENT_H_