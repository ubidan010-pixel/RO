#ifndef _ITF_RAY_BREAKABLECAGECOMPONENT_H_
#define _ITF_RAY_BREAKABLECAGECOMPONENT_H_

namespace ITF
{
    class Ray_BreakableCageComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableCageComponent,ActorComponent,2851407257)
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_BreakableCageComponent();
        virtual ~Ray_BreakableCageComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    Update( f32 _deltaTime );

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_BreakableCageComponent_Template * getTemplate() const;
        class AnimLightComponent * m_animComponent;

        enum STATES
        {
            STAGE1,
            STAGE1to2,
            STAGE2,
            STAGE2to3,
            STAGE3,
        };
        STATES m_state;
        void setState(STATES _state);

        ActorRefList m_justLanded;
        ActorRefList m_currentlyOn;
        ActorRefList m_justLeft;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BreakableCageComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableCageComponent_Template,TemplateActorComponent,2375806396);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BreakableCageComponent);

    public:

        Ray_BreakableCageComponent_Template();
        ~Ray_BreakableCageComponent_Template();

        ITF_INLINE ITF::StringID getStage1Anim() const { return m_stage1Anim; }
        ITF_INLINE ITF::StringID getStage2Anim() const { return m_stage2Anim; }
        ITF_INLINE ITF::StringID getStage3Anim() const { return m_stage3Anim; }
        ITF_INLINE ITF::StringID getStage1toStage2Anim() const { return m_stage1toStage2Anim; }
        ITF_INLINE ITF::StringID getStage2toStage3Anim() const { return m_stage2toStage3Anim; }
 
    private:
        StringID m_stage1Anim;
        StringID m_stage2Anim;
        StringID m_stage3Anim;
        StringID m_stage1toStage2Anim;
        StringID m_stage2toStage3Anim;
    };

    ITF_INLINE const class Ray_BreakableCageComponent_Template * Ray_BreakableCageComponent::getTemplate() const
    {
        return static_cast<const class Ray_BreakableCageComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAY_BREAKABLECAGECOMPONENT_H_
