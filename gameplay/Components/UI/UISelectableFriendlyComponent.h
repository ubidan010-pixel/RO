#ifndef _ITF_UISELECTABLEFRIENDLYCOMPONENT_H_
#define _ITF_UISELECTABLEFRIENDLYCOMPONENT_H_

#ifndef _ITF_UIMENUITEMTEXT_H_
#include "gameplay/components/UI/UIMenuItemText.h"
#endif //_ITF_UIMENUITEMTEXT_H_

namespace ITF
{
    class UISelectableFriendlyComponent : public UIMenuItemText
    {
        DECLARE_OBJECT_CHILD_RTTI(UISelectableFriendlyComponent, UIMenuItemText, 393591125);

    public:
        DECLARE_SERIALIZE()

        UISelectableFriendlyComponent();
        ~UISelectableFriendlyComponent();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    Update(f32 _deltaTime);

    private:
        ITF_INLINE const class UISelectableFriendlyComponent_Template* getTemplate() const;
        void            applyFriendlyFromSelection();

        String8         m_defaultFriendly;
        String8         m_selectedFriendly;
        bbool           m_lastSelected;
    };

    class UISelectableFriendlyComponent_Template : public UIMenuItemText_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UISelectableFriendlyComponent_Template, UIMenuItemText_Template, 1124587126);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UISelectableFriendlyComponent);

    public:
        UISelectableFriendlyComponent_Template() {}
        ~UISelectableFriendlyComponent_Template() {}

        ITF_INLINE const String8& getDefaultFriendly() const { return m_defaultFriendly; }
        ITF_INLINE const String8& getSelectedFriendly() const { return m_selectedFriendly; }

    private:
        String8 m_defaultFriendly;
        String8 m_selectedFriendly;
    };

    ITF_INLINE const UISelectableFriendlyComponent_Template* UISelectableFriendlyComponent::getTemplate() const
    {
        return static_cast<const UISelectableFriendlyComponent_Template*>(m_template);
    }
}

#endif //_ITF_UISELECTABLEFRIENDLYCOMPONENT_H_
