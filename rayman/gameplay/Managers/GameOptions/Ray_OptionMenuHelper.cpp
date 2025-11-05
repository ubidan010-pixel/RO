#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
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

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionManager.h"
#endif

#include <algorithm>
#include <cmath>

namespace ITF
{
    Ray_OptionMenuHelper* Ray_OptionMenuHelper::s_activeHelper = nullptr;
    Ray_OptionMenuHelper::Ray_OptionMenuHelper()
        : m_mainListener(nullptr)
          , m_menu(nullptr)
          , m_isActive(bfalse)
          , m_menuState(MenuState_Navigate)
          , m_currentEditingOption(StringID::Invalid)
          , m_currentEditingComponent(nullptr)
          , m_timer(0.0f)
          , m_timeout(0.2f)
          , m_timeoutJustPressed(0.3f)
          , m_firstPressed(btrue)
    {
    }

    Ray_OptionMenuHelper::~Ray_OptionMenuHelper()
    = default;

    void Ray_OptionMenuHelper::activateForOptionMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        m_isActive = btrue;
        s_activeHelper = this;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(OPTION_MENU_NAME, this);
        m_menu = UI_MENUMANAGER->getMenu(OPTION_MENU_NAME);
        if (!m_menu)
            return;

        initializeMenuState();
    }

    void Ray_OptionMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component)
            return;

        // If we were editing, pressing confirm on the edited component exits edit mode.
        if (isEditing())
        {
            if (component == m_currentEditingComponent)
            {
                exitEditMode();
                return;
            }

            // Switching to another component ends the current edit session first.
            exitEditMode();
        }

        StringID optionId = getOptionIdForComponent(component);
        if (!optionId.isValid())
            return;

        UIToggleOptionComponent* toggleComponent = component->DynamicCast<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent, 3689192266));
        if (toggleComponent)
        {
            toggleOption(toggleComponent, optionId);
            return;
        }

        if (!isOptionEditable(component))
            return;

        enterEditMode(component, optionId);
    }

    StringID Ray_OptionMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action,
                                                    const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER) return defaultAction;
        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(menu, action, defaultAction);
    }

    bbool Ray_OptionMenuHelper::onMenuItemOtherAction(UIComponent* component, const StringID& action)
    {
        if (!component)
            return bfalse;

        if (!isEditing())
            return bfalse;

        // Lock navigation while editing.
        if (action == input_actionID_Up || action == input_actionID_Down ||
            action == input_actionID_UpHold || action == input_actionID_DownHold)
        {
            return btrue;
        }

        return processEditingInput(component, action);
    }

    void Ray_OptionMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
        if (!isEditing())
            return;

        if (!uiComponent)
            return;

        if (!isSelected && uiComponent == m_currentEditingComponent)
        {
            exitEditMode();
            return;
        }

        if (isSelected && uiComponent != m_currentEditingComponent)
        {
            exitEditMode();
        }
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
        if (s_activeHelper == this)
            s_activeHelper = nullptr;
    }

    void Ray_OptionMenuHelper::initializeMenuState()
    {
        if (!m_menu) return;

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentEditingComponent = nullptr;
        m_previousSelectionStates.clear();

        const ObjectRefList& componentsList = m_menu->getUIComponentsList();
        for (u32 i = 0; i < componentsList.size(); ++i)
        {
            UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
            if (!comp)
                continue;

            UIListOptionComponent* listOption = comp->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            if (listOption)
            {
                listOption->setEditingMode(bfalse);
            }
        }

        loadOptionsFromSaveFile();
    }

    void Ray_OptionMenuHelper::enterEditMode(UIComponent* component, const StringID& optionId)
    {
        if (!component || !optionId.isValid())
            return;

        if (m_menuState == MenuState_EditOption && m_currentEditingComponent == component)
            return;

        exitEditMode();

        m_menuState = MenuState_EditOption;
        m_currentEditingOption = optionId;
        m_currentEditingComponent = component;
        m_previousSelectionStates.clear();

        if (m_menu)
        {
            const ObjectRefList& componentsList = m_menu->getUIComponentsList();
            for (u32 i = 0; i < componentsList.size(); ++i)
            {
                UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
                if (!comp)
                    continue;

                bbool originalSelectable = comp->getCanBeSelected();

                if (comp == component)
                {
                    if (!originalSelectable)
                    {
                        m_previousSelectionStates.emplace_back(comp, originalSelectable);
                        comp->setCanBeSelected(btrue);
                    }
                    continue;
                }

                if (originalSelectable)
                {
                    m_previousSelectionStates.emplace_back(comp, originalSelectable);
                    comp->setCanBeSelected(bfalse);
                }
            }
        }

        if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
        {
            listOption->setEditingMode(btrue);
        }
    }

    void Ray_OptionMenuHelper::exitEditMode()
    {
        if (m_menuState != MenuState_EditOption)
            return;

        if (m_currentEditingComponent)
        {
            UIListOptionComponent* listOption = m_currentEditingComponent->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            if (listOption)
            {
                listOption->setEditingMode(bfalse);
            }
        }

        for (auto& entry : m_previousSelectionStates)
        {
            if (!entry.first)
                continue;
            entry.first->setCanBeSelected(entry.second);
        }
        m_previousSelectionStates.clear();

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentEditingComponent = nullptr;
        m_timer = 0.0f;
        m_firstPressed = btrue;
    }

    bbool Ray_OptionMenuHelper::isOptionEditable(UIComponent* component) const
    {
        if (!component)
            return bfalse;

        if (component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
            return btrue;

        if (component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
            return btrue;

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

    StringID Ray_OptionMenuHelper::getOptionIdForComponent(UIComponent* component) const
    {
        if (!component)
            return StringID::Invalid;

        Actor* actor = component->GetActor();
        if (!actor)
            return StringID::Invalid;

        String8 friendlyName = actor->getUserFriendly();
        if (friendlyName.isEmpty())
            return StringID::Invalid;

        return getOptionIdFromFriendlyName(friendlyName);
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

    void Ray_OptionMenuHelper::toggleOption(UIToggleOptionComponent* toggleComponent, const StringID& optionId)
    {
        if (!toggleComponent || !optionId.isValid() || !RAY_GAMEMANAGER)
            return;

        Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();
        bbool currentValue = optionManager.getBoolOption(optionId, bfalse);
        bbool newValue = !currentValue;

        if (optionId == OPTION_WINDOWED)
        {
            RAY_GAMEMANAGER->setWindowed(newValue);
        }
        else if (optionId == OPTION_MURFY_ASSIST)
        {
            RAY_GAMEMANAGER->setMurfyAssist(newValue);
        }
        else
        {
            optionManager.setBoolOption(optionId, newValue);
        }

        toggleComponent->setValue(newValue);
        applyOptionChange(optionId);
    }

    void Ray_OptionMenuHelper::adjustListOption(UIListOptionComponent* listComponent, const StringID& optionId, i32 direction)
    {
        if (!listComponent || !optionId.isValid() || direction == 0 || !RAY_GAMEMANAGER)
            return;

        Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();
        Ray_GameOption* option = optionManager.getOption(optionId);
        if (!option)
            return;

        i32 currentIndex = optionManager.getListOptionIndex(optionId, -1);
        if (currentIndex < 0)
            return;

        i32 optionCount = 0;
        switch (option->getType())
        {
        case Ray_GameOption::OptionType_IntList:
            optionCount = static_cast<i32>(static_cast<Ray_GameOptionIntList*>(option)->getOptions().size());
            break;
        case Ray_GameOption::OptionType_FloatList:
            optionCount = static_cast<i32>(static_cast<Ray_GameOptionFloatList*>(option)->getOptions().size());
            break;
        case Ray_GameOption::OptionType_StringList:
            optionCount = static_cast<i32>(static_cast<Ray_GameOptionStringList*>(option)->getOptions().size());
            break;
        default:
            break;
        }

        if (optionCount <= 0)
            return;

        i32 newIndex = currentIndex + ((direction > 0) ? 1 : -1);
        if (newIndex < 0)
            newIndex = optionCount - 1;
        else if (newIndex >= optionCount)
            newIndex = 0;

        if (newIndex == currentIndex)
            return;

        if (optionId == OPTION_RESOLUTION)
        {
            RAY_GAMEMANAGER->setResolutionIndex(newIndex);
        }
        else if (optionId == OPTION_LANGUAGE)
        {
            RAY_GAMEMANAGER->setLanguageIndex(newIndex);
        }
        else if (optionId == OPTION_START_WITH_HEART)
        {
            RAY_GAMEMANAGER->setStartWithHeartIndex(newIndex);
        }
        else if (optionId == OPTION_RUN_BUTTON)
        {
            RAY_GAMEMANAGER->setRunButtonMode(newIndex);
        }
        else if (optionId == OPTION_VIBRATIONS)
        {
            RAY_GAMEMANAGER->setVibrationMode(newIndex);
        }
        else
        {
            optionManager.setListOptionIndex(optionId, newIndex);
        }

        updateListOptionDisplay(listComponent, optionId, newIndex);
        applyOptionChange(optionId);
    }

    void Ray_OptionMenuHelper::adjustFloatOption(UIFloatOptionComponent* floatComponent, const StringID& optionId, i32 direction)
    {
        if (!floatComponent || !optionId.isValid() || direction == 0 || !RAY_GAMEMANAGER)
            return;

        Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();
        Ray_GameOption* option = optionManager.getOption(optionId);
        if (!option || option->getType() != Ray_GameOption::OptionType_Float)
            return;

        Ray_GameOptionFloat* floatOption = static_cast<Ray_GameOptionFloat*>(option);

        f32 step = floatComponent->getCursorSpeed();
        if (step <= 0.0f)
            step = 0.05f;

        f32 currentValue = floatOption->getValue();
        f32 newValue = currentValue + ((direction > 0) ? step : -step);
        newValue = std::max(floatOption->getMinValue(), std::min(floatOption->getMaxValue(), newValue));

        if (std::fabs(newValue - currentValue) < 1e-4f)
            return;

        if (optionId == OPTION_MASTER_VOLUME)
        {
            RAY_GAMEMANAGER->setMasterVolume(newValue);
        }
        else if (optionId == OPTION_MUSIC_VOLUME)
        {
            RAY_GAMEMANAGER->setMusicVolume(newValue);
        }
        else if (optionId == OPTION_SFX_VOLUME)
        {
            RAY_GAMEMANAGER->setSFXVolume(newValue);
        }
        else if (optionId == OPTION_INTENSITY)
        {
            RAY_GAMEMANAGER->setIntensity(newValue);
        }
        else
        {
            optionManager.setFloatOption(optionId, newValue);
        }

        floatComponent->setValue(newValue, btrue);
        applyOptionChange(optionId);
    }

    void Ray_OptionMenuHelper::updateListOptionDisplay(UIListOptionComponent* listComponent, const StringID& optionId, i32 index) const
    {
        if (!listComponent || !RAY_GAMEMANAGER)
            return;

        Actor* valueActor = listComponent->getValueActor();
        if (!valueActor)
            return;

        UIComponent* valueComp = valueActor->GetComponent<UIComponent>();
        if (!valueComp)
            return;

        const char* displayName = "";

        if (optionId == OPTION_RESOLUTION)
            displayName = RAY_GAMEMANAGER->getResolutionDisplayName(index);
        else if (optionId == OPTION_LANGUAGE)
            displayName = RAY_GAMEMANAGER->getLanguageDisplayName(index);
        else if (optionId == OPTION_START_WITH_HEART)
            displayName = RAY_GAMEMANAGER->getStartWithHeartDisplayName(index);
        else if (optionId == OPTION_RUN_BUTTON)
            displayName = RAY_GAMEMANAGER->getRunButtonDisplayName(index);
        else if (optionId == OPTION_VIBRATIONS)
            displayName = RAY_GAMEMANAGER->getVibrationDisplayName(index);

        if (displayName && displayName[0] != '\0')
        {
            valueComp->forceContent(displayName);
        }
    }

    void Ray_OptionMenuHelper::applyOptionChange(const StringID& optionId) const
    {
        if (!RAY_GAMEMANAGER || !optionId.isValid())
            return;

        if (optionId == OPTION_RESOLUTION || optionId == OPTION_WINDOWED)
        {
            RAY_GAMEMANAGER->applyDisplayOptions();
        }
    }

    void Ray_OptionMenuHelper::updateTimer()
    {
        if (isEditing())
        {
            m_timer += LOGICDT;
        }
    }

    bbool Ray_OptionMenuHelper::processEditingInput(UIComponent* component, const StringID& action)
    {
        if (!component || component != m_currentEditingComponent || !isEditing())
            return bfalse;

        if (action == input_actionID_Back)
        {
            exitEditMode();
            return btrue;
        }

        // Use of the D-pad: the button is just pressed -> go left or right
        if (action == input_actionID_Left)
        {
            m_timer = 0.0f;
            m_firstPressed = btrue;

            if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
            {
                adjustListOption(listOption, m_currentEditingOption, -1);
                return btrue;
            }

            if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
            {
                adjustFloatOption(floatOption, m_currentEditingOption, -1);
                return btrue;
            }
        }
        else if (action == input_actionID_Right)
        {
            m_timer = 0.0f;
            m_firstPressed = btrue;

            if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
            {
                adjustListOption(listOption, m_currentEditingOption, 1);
                return btrue;
            }

            if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
            {
                adjustFloatOption(floatOption, m_currentEditingOption, 1);
                return btrue;
            }
        }

        // Use of the D-pad: the button is pressed -> scroll left or right
        else if (m_timer > m_timeout && action == input_actionID_LeftHold)
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;

                if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
                {
                    adjustListOption(listOption, m_currentEditingOption, -1);
                    return btrue;
                }

                if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
                {
                    adjustFloatOption(floatOption, m_currentEditingOption, -1);
                    return btrue;
                }
            }
        }
        else if (m_timer > m_timeout && action == input_actionID_RightHold)
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;

                if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
                {
                    adjustListOption(listOption, m_currentEditingOption, 1);
                    return btrue;
                }

                if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
                {
                    adjustFloatOption(floatOption, m_currentEditingOption, 1);
                    return btrue;
                }
            }
        }

        return bfalse;
    }
}
