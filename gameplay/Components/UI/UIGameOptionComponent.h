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

    protected:
        ITF_INLINE const class UIGameOptionComponent_Template* getTemplate() const;
        void                    applyLabelColors();
        void                    applyLabelColor(bbool isSelected);
        virtual void            handleSelectionChanged(bbool isSelected);
        Actor*                  m_labelActor;
        Actor*                  m_backgroundActor;
        Vec2d                   m_backgroundOriginalScale;
        f32                     m_backgroundTimer;
        bbool                   m_backgroundScaleInitialized;

    private:
        virtual void                    clear();
        Actor*                  resolveActorFromPath(const String8& path) const;
        void                    resolveLabelActor();
        void                    resolveBackgroundActor();
        void                    updateBackgroundSelection(bbool isSelected);

        String8                 m_labelPath;
        String8                 m_backgroundPath;
        bbool                   m_labelColorsApplied;
        bbool                   m_selectionInitialized;
        bbool                   m_wasSelected;
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
