#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_ControlsRemappingMenuHelper.h"
#endif

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif

namespace ITF
{
    // Define button IDs for controls remapping menu
    #define CONTROLSREMAPPING_ACCEPT_BUTTON  ITF_GET_STRINGID_CRC(accept_button,25226343)
    #define CONTROLSREMAPPING_CANCEL_BUTTON  ITF_GET_STRINGID_CRC(cancel_button,4260770984)

    Ray_ControlsRemappingMenuHelper::Ray_ControlsRemappingMenuHelper()
        : m_mainListener(nullptr)
        , m_menu(nullptr)
        , m_isActive(bfalse)
    {
    }

    Ray_ControlsRemappingMenuHelper::~Ray_ControlsRemappingMenuHelper()
    = default;

    void Ray_ControlsRemappingMenuHelper::activateForControlsRemappingMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        m_isActive = btrue;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(m_menuBaseName, this);
        m_menu = UI_MENUMANAGER->getMenu(m_menuBaseName);

        if (!m_menu)
            return;

        // Show the controls remapping menu
        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), m_menuBaseName, btrue, this);
    }

    void Ray_ControlsRemappingMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component) return;

        const StringID id = component->getID();
        if (handleAccept(id) || handleCancel(id))
            return;
    }

    bbool Ray_ControlsRemappingMenuHelper::handleAccept(const StringID& id)
    {
        if (id != CONTROLSREMAPPING_ACCEPT_BUTTON)
            return bfalse;

        closeAndReturn();
        return btrue;
    }

    bbool Ray_ControlsRemappingMenuHelper::handleCancel(const StringID& id)
    {
        if (id != CONTROLSREMAPPING_CANCEL_BUTTON)
            return bfalse;

        closeAndReturn();
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::closeAndReturn()
    {
        m_isActive = bfalse;
        m_menu = nullptr;

        if (UI_MENUMANAGER)
        {
            UI_MENUMANAGER->showPreviousMenu();
            UI_MENUMANAGER->setMenuListener(UI_MENUMANAGER->getCurrentMenuID(), m_mainListener);
        }
    }

    StringID Ray_ControlsRemappingMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action,
                                                               const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER) return defaultAction;
        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(menu, action, defaultAction);
    }

    bbool Ray_ControlsRemappingMenuHelper::onMenuItemOtherAction(UIComponent* component, const StringID& action)
    {
        return bfalse;
    }

    void Ray_ControlsRemappingMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
        // Handle selection changes if needed
        // For now, keep it simple
    }
}
