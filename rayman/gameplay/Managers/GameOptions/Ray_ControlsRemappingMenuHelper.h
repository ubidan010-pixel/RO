#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#define _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF
{
    class UIComponent;
    class UIMenu;

    class Ray_ControlsRemappingMenuHelper : public UIMenuManager::MenuItemActionListener
    {
    public:
        Ray_ControlsRemappingMenuHelper();
        virtual ~Ray_ControlsRemappingMenuHelper();

        void activateForControlsRemappingMenu(MenuItemActionListener* mainListener);
        bbool isActive() const { return m_isActive; }

        void onMenuItemAction(UIComponent* _UIComponent) override;
        StringID onMenuPageAction(UIMenu* _menu, const StringID& _action, const StringID& _defaultAction) override;
        bbool onMenuItemOtherAction(UIComponent* _UIComponent, const StringID& _action) override;
        void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) override;

    private:
        bbool handleBack(const StringID& id);
        void closeAndReturn();

        MenuItemActionListener* m_mainListener;
        UIMenu*    m_menu;
        const char* m_menuBaseName = "controlremapping";
        bbool      m_isActive;
    };
}

#endif //_ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
