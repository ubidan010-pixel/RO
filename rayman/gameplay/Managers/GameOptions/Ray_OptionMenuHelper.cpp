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

#ifndef _ITF_UILISTOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIListOptionComponent.h"
#endif

#ifndef _ITF_UIFLOATOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIFloatOptionComponent.h"
#endif

#ifndef _ITF_UITOGGLEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIToggleOptionComponent.h"
#endif

#ifndef _ITF_UIMENUITEMTEXT_H_
#include "gameplay/components/UI/UIMenuItemText.h"
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

        loadOptionsFromSaveFile();
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

    StringID Ray_OptionMenuHelper::getOptionIdFromFriendlyName(const String8& friendlyName) const
    {
        if (friendlyName == "resolution_option")
            return OPTION_RESOLUTION;
        else if (friendlyName == "window_option")
            return OPTION_WINDOWED;
        else if (friendlyName == "language_option")
            return OPTION_LANGUAGE;
        else if (friendlyName == "start_with_heart_option")
            return OPTION_START_WITH_HEART;
        else if (friendlyName == "run_button_option")
            return OPTION_RUN_BUTTON;
        else if (friendlyName == "murfy_assist_option")
            return OPTION_MURFY_ASSIST;
        else if (friendlyName == "vibration_option")
            return OPTION_VIBRATIONS;
        else if (friendlyName == "master_volume_option")
            return OPTION_MASTER_VOLUME;
        else if (friendlyName == "music_volume_option")
            return OPTION_MUSIC_VOLUME;
        else if (friendlyName == "sfx_volume_option")
            return OPTION_SFX_VOLUME;
        else if (friendlyName == "intensity_option")
            return OPTION_INTENSITY;

        return StringID::Invalid;
    }

    void Ray_OptionMenuHelper::loadOptionsFromSaveFile()
    {
        if (!m_menu || !RAY_GAMEMANAGER)
            return;

        Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();

        const ObjectRefList& componentsList = m_menu->getUIComponentsList();
        for (u32 i = 0; i < componentsList.size(); ++i)
        {
            UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
            if (!comp)
                continue;

            Actor* actor = comp->GetActor();
            if (!actor)
                continue;

            String8 friendlyName = actor->getUserFriendly();
            if (friendlyName.isEmpty())
                continue;

            StringID optionId = getOptionIdFromFriendlyName(friendlyName);
            if (!optionId.isValid())
                continue;

            UIListOptionComponent* listOption = comp->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            if (listOption)
            {
                i32 currentIndex = optionManager.getListOptionIndex(optionId, -1);
                if (currentIndex >= 0)
                {
                    Actor* valueActor = listOption->getValueActor();
                    if (valueActor)
                    {
                        UIComponent* valueComp = valueActor->GetComponent<UIComponent>();
                        if (valueComp)
                        {
                            const char* displayName = "";
                            if (optionId == OPTION_RESOLUTION)
                                displayName = RAY_GAMEMANAGER->getResolutionDisplayName(currentIndex);
                            else if (optionId == OPTION_LANGUAGE)
                                displayName = RAY_GAMEMANAGER->getLanguageDisplayName(currentIndex);
                            else if (optionId == OPTION_START_WITH_HEART)
                                displayName = RAY_GAMEMANAGER->getStartWithHeartDisplayName(currentIndex);
                            else if (optionId == OPTION_RUN_BUTTON)
                                displayName = RAY_GAMEMANAGER->getRunButtonDisplayName(currentIndex);
                            else if (optionId == OPTION_VIBRATIONS)
                                displayName = RAY_GAMEMANAGER->getVibrationDisplayName(currentIndex);

                            if (displayName && displayName[0] != '\0')
                            {
                                valueComp->forceContent(displayName);
                            }
                        }
                    }
                }
                continue;
            }

            UIFloatOptionComponent* floatOption = comp->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316));
            if (floatOption)
            {
                f32 currentValue = optionManager.getFloatOption(optionId, -1.0f);
                if (currentValue >= 0.0f)
                {
                    floatOption->setValue(currentValue, btrue);
                }
                continue;
            }

            UIToggleOptionComponent* toggleOption = comp->DynamicCast<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent, 3689192266));
            if (toggleOption)
            {
                bbool currentValue = optionManager.getBoolOption(optionId, bfalse);
                toggleOption->setValue(currentValue);
            }
        }
    }
}
