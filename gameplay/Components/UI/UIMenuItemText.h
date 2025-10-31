#ifndef _ITF_UIMENUITEMTEXT_H_
#define _ITF_UIMENUITEMTEXT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{
    class UIMenuItemText : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuItemText,UIComponent,3255902461);

    public:
        DECLARE_SERIALIZE()

        UIMenuItemText();
        ~UIMenuItemText();

        virtual     void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     bbool           needsUpdate         (       ) const             { return btrue; }
        virtual     bbool           needsDraw           (       ) const             { return bfalse; }
        virtual     bbool           needsDraw2D         (       ) const             { return btrue; }
        virtual     void            Update              ( f32 _deltaTime );
        virtual     void            Draw2D              (       );
        virtual     void            onRollover();

    private:
        ITF_INLINE const class UIMenuItemText_Template * getTemplate() const;
        void                        clear();

        f32                         m_timer;
    };

    class UIMenuItemText_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuItemText_Template,UIComponent_Template,3322807307);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuItemText);
        UIMenuItemText_Template() : m_idleSelectedScale(0.05f), m_idleSelectedPulseFrequency(0.7f) {}
    public:
        ITF_INLINE f32 getIdleSelectedScale() const { return m_idleSelectedScale; }
        ITF_INLINE f32 getIdleSelectedPulseFrequency() const { return m_idleSelectedPulseFrequency; }
    private:
        f32 m_idleSelectedScale;
        f32 m_idleSelectedPulseFrequency;
    };

    ITF_INLINE const UIMenuItemText_Template* UIMenuItemText::getTemplate() const
    {
        return static_cast<const UIMenuItemText_Template*>(m_template);
    }

}
#endif // _ITF_UIMENUITEMTEXT_H_