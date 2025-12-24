#ifndef _ITF_RAY_BASEMENUHELPER_H_
#define _ITF_RAY_BASEMENUHELPER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF
{
    class UIComponent;
    class UIMenu;

    class Ray_BaseMenuHelper : public UIMenuManager::MenuItemActionListener
    {
    public:
        Ray_BaseMenuHelper();
        virtual ~Ray_BaseMenuHelper();

        bbool isActive() const { return m_isActive; }
        bbool isBaseMenuHelper() const override { return btrue; }
        void onMenuItemAction(UIComponent* _UIComponent) override;
        StringID onMenuPageAction(UIMenu* _menu, const StringID& _action, const StringID& _defaultAction) override;
        void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) override;
        virtual ObjectRef getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY);

    protected:
        virtual void showContextIcons();
        void closeAndReturn();

        virtual void onActivate() {}
        virtual void onClose() {}

        MenuItemActionListener* m_mainListener;
        UIMenu* m_menu;
        const char* m_menuBaseName;
        bbool m_isActive;
    };
}

#endif //_ITF_RAY_BASEMENUHELPER_H_

