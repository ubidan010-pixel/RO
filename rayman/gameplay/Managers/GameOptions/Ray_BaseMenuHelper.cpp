#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BASEMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_BaseMenuHelper.h"
#endif

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif

namespace ITF
{
    Ray_BaseMenuHelper::Ray_BaseMenuHelper()
        : m_mainListener(nullptr)
        , m_menu(nullptr)
        , m_menuBaseName(nullptr)
        , m_isActive(bfalse)
    {
    }

    Ray_BaseMenuHelper::~Ray_BaseMenuHelper()
    = default;

    void Ray_BaseMenuHelper::onMenuItemAction(UIComponent* _UIComponent)
    {
    }

    StringID Ray_BaseMenuHelper::onMenuPageAction(UIMenu* _menu, const StringID& _action, const StringID& _defaultAction)
    {
        if (!UI_MENUMANAGER) return _defaultAction;
        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(_menu, _action, _defaultAction);
    }

    bbool Ray_BaseMenuHelper::onMenuItemOtherAction(UIComponent* _UIComponent, const StringID& _action)
    {
        return bfalse;
    }

    void Ray_BaseMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
    }

    void Ray_BaseMenuHelper::showContextIcons()
    {
    }

    void Ray_BaseMenuHelper::hideContextIcons()
    {
        if (CONTEXTICONSMANAGER)
        {
            CONTEXTICONSMANAGER->hide();
        }
    }

    void Ray_BaseMenuHelper::closeAndReturn()
    {
        m_isActive = bfalse;
        m_menu = nullptr;

        if (UI_MENUMANAGER)
        {
            UI_MENUMANAGER->showPreviousMenu();
            UI_MENUMANAGER->setMenuListener(UI_MENUMANAGER->getCurrentMenuID(), m_mainListener);
        }

        onClose();
        showContextIcons();
    }
}

