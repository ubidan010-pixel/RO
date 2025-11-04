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
        virtual ~UIFloatOptionComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void        Update              ( f32 _deltaTime );

        virtual     void        onActorClearComponents();
        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

        virtual     void        onPressed           ();
        virtual     void        onReleased          ();
        virtual     void        onRollover          ();
        virtual     void        onRollout           ();
        virtual     void        onAction            (const StringID & action);

        f32         getValue() const { return m_value; }
        void        setValue(f32 val, bbool updatePosition = btrue);

        ITF_INLINE Actor*       getSliderBackgroundStartActor() const { return m_sliderBackgroundStartActor; }
        ITF_INLINE Actor*       getSliderBackgroundEndActor() const { return m_sliderBackgroundEndActor; }
        ITF_INLINE Actor*       getSliderCursorActor() const { return m_sliderCursorActor; }

    protected:
        ITF_INLINE const class UIFloatOptionComponent_Template* getTemplate() const;

    private:
        void                    clear();
        void                    resolveSliderActors();
        void                    updateSliderVisuals();
        void                    updateSliderFromMouse();
        void                    switchToNormalActors();
        void                    switchToSelectedActors();
        void                    updateSelectionState();

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
        bbool                   m_wasSelected;
    };

    class UIFloatOptionComponent_Template : public UIGameOptionComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIFloatOptionComponent_Template, UIGameOptionComponent_Template, 443628752);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIFloatOptionComponent);

    public:
        UIFloatOptionComponent_Template();
        ~UIFloatOptionComponent_Template();

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

}
#endif // _ITF_UIFLOATOPTIONCOMPONENT_H_

