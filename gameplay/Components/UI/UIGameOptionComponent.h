#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#define _ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_


namespace ITF
{
    class Actor;

    class UIGameOptionComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIGameOptionComponent, UIComponent, 3059104641);

    public:
        DECLARE_SERIALIZE()

        UIGameOptionComponent();
        virtual ~UIGameOptionComponent();

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

        ITF_INLINE Actor*       getLabelActor() const { return m_labelActor; }
        ITF_INLINE Actor*       getBackgroundActor() const { return m_backgroundActor; }
        ITF_INLINE Actor*       getSelectedBackgroundActor() const { return m_selectedBackgroundActor; }

        virtual void            setEditingMode(bbool editing);

    protected:
        ITF_INLINE const class UIGameOptionComponent_Template* getTemplate() const;
        void                    setGameOptionEditScale(UIComponent* component, bbool editing) const;
        void                    applyLabelColors();
        void                    applyLabelColor(bbool isSelected);
        virtual void            handleSelectionChanged(bbool isSelected);
        Actor*                  m_labelActor;
        Actor*                  m_backgroundActor;
        Vec2d                   m_backgroundOriginalScale;
        bbool                   m_backgroundScaleInitialized;
        Actor*                  m_selectedBackgroundActor;
        bbool                   m_selectedBackgroundVisible;

    private:
        virtual void                    clear();
        Actor*                  resolveActorFromPath(const String8& path) const;
        void                    resolveLabelActor();
        void                    resolveBackgroundActor();
        void                    resolveSelectedBackgroundActor();
        void                    updateBackgroundSelection(bbool isSelected);
        void                    updateSelectedBackgroundVisibility(bbool isSelected);

        String8                 m_labelPath;
        String8                 m_backgroundPath;
        String8                 m_selectedBackgroundPath;
        bbool                   m_labelColorsApplied;
        bbool                   m_selectionInitialized;
        bbool                   m_wasSelected;
        bbool                   m_isInEditingMode;
        bbool                   m_backgroundHighlighted;
    };

    class UIGameOptionComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIGameOptionComponent_Template, UIComponent_Template, 1914604504);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIGameOptionComponent);

    public:
        UIGameOptionComponent_Template();
        ~UIGameOptionComponent_Template();

        ITF_INLINE f32 getIdleSelectedScale() const { return m_idleSelectedScale; }
        ITF_INLINE f32 getIdleSelectedPulseFrequency() const { return m_idleSelectedPulseFrequency; }

    private:
        f32 m_idleSelectedScale;
        f32 m_idleSelectedPulseFrequency;

    };

    ITF_INLINE const UIGameOptionComponent_Template* UIGameOptionComponent::getTemplate() const
    {
        return static_cast<const UIGameOptionComponent_Template*>(m_template);
    }

}
#endif // _ITF_UIGAMEOPTIONCOMPONENT_H_
