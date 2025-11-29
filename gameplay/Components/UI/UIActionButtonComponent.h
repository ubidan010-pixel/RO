#ifndef _ITF_UIACTIONBUTTONCOMPONENT_H_
#define _ITF_UIACTIONBUTTONCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_


namespace ITF
{
    class Actor;
    class UIActionButtonComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIActionButtonComponent, UIComponent, 2847391056);

    public:
        DECLARE_SERIALIZE()

        UIActionButtonComponent();
        virtual ~UIActionButtonComponent();

        virtual     bbool       needsUpdate         (       ) const             { return bfalse; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual     void        onActorClearComponents();
        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

        virtual     void        onPressed           ();
        virtual     void        onReleased          ();
        virtual     void        onRollover          ();
        virtual     void        onRollout           ();
        virtual     void        onAction            (const StringID & action);

        ITF_INLINE const String8& getHighlightBgPath() const { return m_highlightBgPath; }

    protected:
        ITF_INLINE const class UIActionButtonComponent_Template* getTemplate() const;

    private:
        virtual void            clear();
        void                    setHighlightVisible(bbool visible);

        String8                 m_highlightBgPath;
        Actor*                  m_highlightBgActor;
    };

    class UIActionButtonComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIActionButtonComponent_Template, UIComponent_Template, 3156892741);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIActionButtonComponent);

    public:
        UIActionButtonComponent_Template();
        ~UIActionButtonComponent_Template();

    private:

    };

    ITF_INLINE const UIActionButtonComponent_Template* UIActionButtonComponent::getTemplate() const
    {
        return static_cast<const UIActionButtonComponent_Template*>(m_template);
    }

}
#endif // _ITF_UIACTIONBUTTONCOMPONENT_H_
