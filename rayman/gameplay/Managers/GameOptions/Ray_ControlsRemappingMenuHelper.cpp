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
        : Ray_BaseMenuHelper()
    {
        m_menuBaseName = "controlremapping";
    }

    Ray_ControlsRemappingMenuHelper::~Ray_ControlsRemappingMenuHelper()
    = default;

    void Ray_ControlsRemappingMenuHelper::activateForControlsRemappingMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        hideContextIcons();

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
}
