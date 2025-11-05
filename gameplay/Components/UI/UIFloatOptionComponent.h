#ifndef _ITF_UIFLOATOPTIONCOMPONENT_H_
#define _ITF_UIFLOATOPTIONCOMPONENT_H_

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

namespace ITF
{
    class Actor;

    class UIFloatOptionComponent : public UIGameOptionComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIFloatOptionComponent, UIGameOptionComponent, 226609316);

    public:
        DECLARE_SERIALIZE()

        UIFloatOptionComponent();
        ~UIFloatOptionComponent() override;

        bbool       needsUpdate         (       ) const override { return btrue; }
        bbool       needsDraw           (       ) const override { return bfalse; }
        bbool       needsDraw2D         (       ) const override { return bfalse; }
        void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/) override;
        void        Update              ( f32 _deltaTime ) override;

        void        onActorClearComponents() override;
        void        onBecomeActive() override;
        void        onBecomeInactive() override;

        void        onPressed           () override;
        void        onReleased          () override;
        void        onRollover          () override;
        void        onRollout           () override;
        void        onAction            (const StringID & action) override;

        f32         getValue() const { return m_value; }
        void        setValue(f32 val, bbool updatePosition = btrue);
        f32         getCursorSpeed() const;

        ITF_INLINE Actor*       getSliderBackgroundStartActor() const { return m_sliderBackgroundStartActor; }
        ITF_INLINE Actor*       getSliderBackgroundEndActor() const { return m_sliderBackgroundEndActor; }
        ITF_INLINE Actor*       getSliderCursorActor() const { return m_sliderCursorActor; }

    protected:
        ITF_INLINE const class UIFloatOptionComponent_Template* getTemplate() const;
        virtual void            handleSelectionChanged(bbool isSelected) override;

    private:
        void                    clear() override;
        void                    resolveSliderActors();
        void                    updateSliderVisuals();
        void                    updateSliderFromMouse();
        void                    switchToNormalActors();
        void                    switchToSelectedActors();

        String8                 m_sliderBackgroundStartPath;
        Actor*                  m_sliderBackgroundStartActor;
        String8                 m_sliderBackgroundEndPath;
        Actor*                  m_sliderBackgroundEndActor;
        String8                 m_sliderCursorPath;
        Actor*                  m_sliderCursorActor;
        String8                 m_sliderBackgroundSelectedPath;
        Actor*                  m_sliderBackgroundSelectedActor;
        String8                 m_sliderCursorSelectedPath;
        Actor*                  m_sliderCursorSelectedActor;

        class TextureGraphicComponent2D* m_cursorGraphComponent;
        bbool                   m_isSliding;
        Vec2d                   m_originalCursorScale;
        Vec2d                   m_originalCursorSelectedScale;
        f32                     m_value;
    };

    class UIFloatOptionComponent_Template : public UIGameOptionComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIFloatOptionComponent_Template, UIGameOptionComponent_Template, 443628752)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIFloatOptionComponent)

    public:
        UIFloatOptionComponent_Template();
        ~UIFloatOptionComponent_Template() override;

        ITF_INLINE f32      getSpeedCursor() const { return m_speedCursor; }
        ITF_INLINE f32      getScaleOnSelected() const { return m_scaleOnSelected; }
        ITF_INLINE Color    getColorOnSelected() const { return m_colorOnSelected; }

    private:
        f32         m_speedCursor;
        f32         m_scaleOnSelected;
        Color       m_colorOnSelected;
    };

    ITF_INLINE const UIFloatOptionComponent_Template* UIFloatOptionComponent::getTemplate() const
    {
        return static_cast<const UIFloatOptionComponent_Template*>(m_template);
    }

    ITF_INLINE f32 UIFloatOptionComponent::getCursorSpeed() const
    {
        const UIFloatOptionComponent_Template* tpl = getTemplate();
        return tpl ? tpl->getSpeedCursor() : 0.0f;
    }

}
#endif // _ITF_UIFLOATOPTIONCOMPONENT_H_
