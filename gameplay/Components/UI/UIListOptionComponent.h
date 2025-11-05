#ifndef _ITF_UILISTOPTIONCOMPONENT_H_
#define _ITF_UILISTOPTIONCOMPONENT_H_

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

namespace ITF
{
    class UIListOptionComponent : public UIGameOptionComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIListOptionComponent, UIGameOptionComponent, 3621365669);

    public:
        DECLARE_SERIALIZE()

        UIListOptionComponent();
        virtual ~UIListOptionComponent();

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

        ITF_INLINE Actor*       getValueActor() const { return m_valueActor; }
        void                    setEditingMode(bbool editing);

    protected:
        ITF_INLINE const class UIListOptionComponent_Template* getTemplate() const;
        virtual void            handleSelectionChanged(bbool isSelected) override;

    private:
        void                    clear();
        void                    resolveValueActor();
        void                    applyValueColors();
        void                    applyValueColor(bbool isSelected);
        void                    resolveLeftArrow();
        void                    resolveRightArrow();
        void                    hideAllArrows();
        void                    showArrows();

        String8                 m_valuePath;
        Actor*                  m_valueActor;
        bbool                   m_valueColorsApplied;

        String8                 m_leftArrowPath;
        Actor*                  m_leftArrowActor;
        String8                 m_leftArrowHighlightPath;
        Actor*                  m_leftArrowHighlightActor;
        String8                 m_rightArrowPath;
        Actor*                  m_rightArrowActor;
        String8                 m_rightArrowHighlightPath;
        Actor*                  m_rightArrowHighlightActor;
        bbool                   m_isEditing;
    };

    class UIListOptionComponent_Template : public UIGameOptionComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIListOptionComponent_Template, UIGameOptionComponent_Template, 1371529000);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIListOptionComponent);

    public:
        UIListOptionComponent_Template();
        ~UIListOptionComponent_Template();

    private:

    };

    ITF_INLINE const UIListOptionComponent_Template* UIListOptionComponent::getTemplate() const
    {
        return static_cast<const UIListOptionComponent_Template*>(m_template);
    }

}
#endif // _ITF_UILISTOPTIONCOMPONENT_H_

