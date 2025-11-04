#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif

namespace ITF
{
    Ray_OptionMenuHelper::Ray_OptionMenuHelper()
        : m_mainListener(nullptr)
          , m_menu(nullptr)
          , m_isActive(bfalse)
          , m_menuState(MenuState_Navigate)
          , m_currentEditingOption(StringID::Invalid)
    {
    }

    Ray_OptionMenuHelper::~Ray_OptionMenuHelper()
    = default;

    void Ray_OptionMenuHelper::activateForOptionMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        m_isActive = btrue;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(OPTION_MENU_NAME, this);
        m_menu = UI_MENUMANAGER->getMenu(OPTION_MENU_NAME);
        if (!m_menu)
            return;

        initializeMenuState();
    }

    void Ray_OptionMenuHelper::onMenuItemAction(UIComponent* component)
    {
    }

    StringID Ray_OptionMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action,
                                                    const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER) return defaultAction;
        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(menu, action, defaultAction);
    }

    bbool Ray_OptionMenuHelper::onMenuItemOtherAction(UIComponent* component, const StringID& action)
    {
        return bfalse;
    }

    void Ray_OptionMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
    }

    bbool Ray_OptionMenuHelper::handleResetToDefault(const StringID& id)
    {
        return bfalse;
    }

    bbool Ray_OptionMenuHelper::handleAccept(const StringID& id)
    {
        return bfalse;
    }

    bbool Ray_OptionMenuHelper::handleCancel(const StringID& id)
    {
        return bfalse;
    }

    void Ray_OptionMenuHelper::closeAndReturn()
    {
        m_isActive = bfalse;
        m_menu = nullptr;
        if (UI_MENUMANAGER)
        {
            UI_MENUMANAGER->showPreviousMenu();
            UI_MENUMANAGER->setMenuListener(UI_MENUMANAGER->getCurrentMenuID(), m_mainListener);
        }
    }

    void Ray_OptionMenuHelper::initializeMenuState()
    {
        if (!m_menu) return;

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
    }

    void Ray_OptionMenuHelper::enterEditMode(const StringID& optionId)
    {
    }

    void Ray_OptionMenuHelper::exitEditMode()
    {
    }

    bbool Ray_OptionMenuHelper::isOptionEditable(const StringID& optionId) const
    {
        return bfalse;
    }

    void Ray_OptionMenuHelper::hideAllArrows()
    {
    }

    void Ray_OptionMenuHelper::showArrowsForOption(const StringID& optionId)
    {
    }

    void Ray_OptionMenuHelper::hideArrowsForOption(const StringID& optionId)
    {
    }

    StringID Ray_OptionMenuHelper::getOptionIdFromComponent(const StringID& componentId) const
    {
        return StringID::Invalid;
    }

    void Ray_OptionMenuHelper::setUIVisibilitySelectable(const StringID& id, bbool visible, bbool selectable) const
    {
    }

    void Ray_OptionMenuHelper::setVisibilityFor(std::initializer_list<StringID> ids, bbool visible,
                                                bbool selectable) const
    {
    }

    void Ray_OptionMenuHelper::ensureValidSelection() const
    {
    }
}
