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
#include <cstring>

namespace ITF
{
    namespace
    {
        struct OptionNavigationEntry
        {
            const char* friendly;
            const char* up;
            const char* down;
            const char* left;
            const char* right;
        };

        static const OptionNavigationEntry s_optionNavigationEntries[] =
        {
            { "resolution_option",        nullptr,                  "window_option",          nullptr,                    "start_with_heart_option" },
            { "window_option",            "resolution_option",       "language_option",        nullptr,                    "run_button_option" },
            { "language_option",          "window_option",           "master_volume_option",   nullptr,                    "vibration_option" },
            { "master_volume_option",     "language_option",         "music_volume_option",    nullptr,                    "reset_to_default_button" },
            { "music_volume_option",      "master_volume_option",    "sfx_volume_option",      nullptr,                    "reset_to_default_button" },
            { "sfx_volume_option",        "music_volume_option",     "accept_button",          nullptr,                    "ubisoftconnect_button" },
            { "accept_button",            "sfx_volume_option",       nullptr,                  nullptr,                    "cancel_button" },
            { "cancel_button",            "ubisoftconnect_button",   nullptr,                  "accept_button",            nullptr },
            { "reset_to_default_button",  "intensity_option",        "ubisoftconnect_button",  "music_volume_option",      "cancel_button" },
            { "ubisoftconnect_button",    "reset_to_default_button", "cancel_button",          "sfx_volume_option",        "cancel_button" },
            { "start_with_heart_option",  nullptr,                  "run_button_option",      "resolution_option",        nullptr },
            { "run_button_option",        "start_with_heart_option", "vibration_option",       "window_option",            nullptr },
            { "vibration_option",         "run_button_option",       "intensity_option",       "language_option",          nullptr },
            { "intensity_option",         "vibration_option",        "reset_to_default_button","language_option",          nullptr },
        };

        static const size_t s_optionNavigationEntryCount = sizeof(s_optionNavigationEntries) / sizeof(s_optionNavigationEntries[0]);
    }

    Ray_OptionMenuHelper* Ray_OptionMenuHelper::s_activeHelper = nullptr;
    Ray_OptionMenuHelper::Ray_OptionMenuHelper()
        : m_mainListener(nullptr)
          , m_menu(nullptr)
          , m_isActive(bfalse)
          , m_menuState(MenuState_Navigate)
          , m_currentEditingOption(StringID::Invalid)
          , m_currentEditingComponent(nullptr)
          , m_timer(0.0f)
          , m_timeout(Ray_OptionMenuHelperConstants::INPUT_REPEAT_TIMEOUT)
          , m_timeoutJustPressed(Ray_OptionMenuHelperConstants::INPUT_INITIAL_DELAY)
          , m_firstPressed(btrue)
          , m_hasSnapshot(bfalse)
          , m_snapshotResolutionIndex(-1)
          , m_snapshotWindowed(bfalse)
          , m_snapshotLanguageIndex(-1)
          , m_snapshotStartWithHeartIndex(-1)
          , m_snapshotRunButtonMode(-1)
          , m_snapshotMurfyAssist(bfalse)
          , m_snapshotVibrationMode(-1)
          , m_snapshotMasterVolume(0.0f)
          , m_snapshotMusicVolume(0.0f)
          , m_snapshotSFXVolume(0.0f)
          , m_snapshotIntensity(0.0f)
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

        const StringID componentId = component->getID();
        if (componentId.isValid())
        {
            if (componentId == OPTIONMENU_RESET_TO_DEFAULT_BUTTON ||
                componentId == OPTIONMENU_ACCEPT_BUTTON ||
                componentId == OPTIONMENU_CANCEL_BUTTON)
            {
                if (isEditing())
                {
                    exitEditMode();
                }
            }

            if (handleResetToDefault(componentId) || handleAccept(componentId) || handleCancel(componentId))
            {
                return;
            }
        }

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
        if (id != OPTIONMENU_RESET_TO_DEFAULT_BUTTON)
            return bfalse;

        if (!RAY_GAMEMANAGER)
            return btrue;

        Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();

        optionManager.resetOption(OPTION_RESOLUTION);
        optionManager.resetOption(OPTION_WINDOWED);
        optionManager.resetOption(OPTION_LANGUAGE);
        optionManager.resetOption(OPTION_START_WITH_HEART);
        optionManager.resetOption(OPTION_RUN_BUTTON);
        optionManager.resetOption(OPTION_MURFY_ASSIST);
        optionManager.resetOption(OPTION_VIBRATIONS);
        optionManager.resetOption(OPTION_MASTER_VOLUME);
        optionManager.resetOption(OPTION_MUSIC_VOLUME);
        optionManager.resetOption(OPTION_SFX_VOLUME);
        optionManager.resetOption(OPTION_INTENSITY);

        const i32 resolutionIndex = optionManager.getListOptionIndex(OPTION_RESOLUTION, RAY_GAMEMANAGER->getResolutionIndex());
        const bbool windowed = optionManager.getBoolOption(OPTION_WINDOWED, RAY_GAMEMANAGER->isWindowed());
        const i32 languageIndex = optionManager.getListOptionIndex(OPTION_LANGUAGE, RAY_GAMEMANAGER->getLanguageIndex());
        const i32 startWithHeartIndex = optionManager.getListOptionIndex(OPTION_START_WITH_HEART, RAY_GAMEMANAGER->getStartWithHeartIndex());
        const i32 runButtonMode = optionManager.getListOptionIndex(OPTION_RUN_BUTTON, RAY_GAMEMANAGER->getRunButtonMode());
        const bbool murfyAssist = optionManager.getBoolOption(OPTION_MURFY_ASSIST, RAY_GAMEMANAGER->isMurfyAssistEnabled());
        const i32 vibrationMode = optionManager.getListOptionIndex(OPTION_VIBRATIONS, RAY_GAMEMANAGER->getVibrationMode());
        const f32 masterVolume = optionManager.getFloatOption(OPTION_MASTER_VOLUME, RAY_GAMEMANAGER->getMasterVolume());
        const f32 musicVolume = optionManager.getFloatOption(OPTION_MUSIC_VOLUME, RAY_GAMEMANAGER->getMusicVolume());
        const f32 sfxVolume = optionManager.getFloatOption(OPTION_SFX_VOLUME, RAY_GAMEMANAGER->getSFXVolume());
        const f32 intensity = optionManager.getFloatOption(OPTION_INTENSITY, RAY_GAMEMANAGER->getIntensity());

        RAY_GAMEMANAGER->setResolutionIndex(resolutionIndex);
        RAY_GAMEMANAGER->setWindowed(windowed);
        RAY_GAMEMANAGER->setLanguageIndex(languageIndex);
        RAY_GAMEMANAGER->setStartWithHeartIndex(startWithHeartIndex);
        RAY_GAMEMANAGER->setRunButtonMode(runButtonMode);
        RAY_GAMEMANAGER->setMurfyAssist(murfyAssist);
        RAY_GAMEMANAGER->setVibrationMode(vibrationMode);
        RAY_GAMEMANAGER->setMasterVolume(masterVolume);
        RAY_GAMEMANAGER->setMusicVolume(musicVolume);
        RAY_GAMEMANAGER->setSFXVolume(sfxVolume);
        RAY_GAMEMANAGER->setIntensity(intensity);

        RAY_GAMEMANAGER->applyDisplayOptions();
        refreshAllOptionVisuals();

        return btrue;
    }

    bbool Ray_OptionMenuHelper::handleAccept(const StringID& id)
    {
        if (id != OPTIONMENU_ACCEPT_BUTTON)
            return bfalse;

        if (RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->applyDisplayOptions();
            RAY_GAMEMANAGER->saveGameOptions();
        }

        closeAndReturn();
        return btrue;
    }

    bbool Ray_OptionMenuHelper::handleCancel(const StringID& id)
    {
        if (id != OPTIONMENU_CANCEL_BUTTON)
            return bfalse;

        if (RAY_GAMEMANAGER && m_hasSnapshot)
        {
            restoreSnapshot();
            refreshAllOptionVisuals();
        }
        else
        {
            refreshAllOptionVisuals();
        }

        closeAndReturn();
        return btrue;
    }

    void Ray_OptionMenuHelper::closeAndReturn()
    {
        exitEditMode();
        m_isActive = bfalse;
        m_menu = nullptr;
        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentEditingComponent = nullptr;
        m_previousSelectionStates.clear();
        m_timer = 0.0f;
        m_firstPressed = btrue;
        m_hasSnapshot = bfalse;
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
        captureSnapshot();
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
            step = Ray_OptionMenuHelperConstants::SLIDER_DEFAULT_STEP;
        
        step *= Ray_OptionMenuHelperConstants::SLIDER_SPEED_MULTIPLIER;

        f32 currentValue = floatOption->getValue();
        f32 newValue = currentValue + ((direction > 0) ? step : -step);
        newValue = std::max(floatOption->getMinValue(), std::min(floatOption->getMaxValue(), newValue));

        if (std::fabs(newValue - currentValue) < Ray_OptionMenuHelperConstants::FLOAT_EPSILON)
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

    UIComponent* Ray_OptionMenuHelper::findOptionComponent(const StringID& optionId) const
    {
        if (!m_menu || !optionId.isValid())
            return nullptr;

        const ObjectRefList& componentsList = m_menu->getUIComponentsList();
        for (u32 i = 0; i < componentsList.size(); ++i)
        {
            UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
            if (!comp)
                continue;

            if (getOptionIdForComponent(comp) == optionId)
                return comp;
        }

        return nullptr;
    }

    UIListOptionComponent* Ray_OptionMenuHelper::findListOptionComponent(const StringID& optionId) const
    {
        UIComponent* component = findOptionComponent(optionId);
        if (!component)
            return nullptr;

        return component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
    }

    UIToggleOptionComponent* Ray_OptionMenuHelper::findToggleOptionComponent(const StringID& optionId) const
    {
        UIComponent* component = findOptionComponent(optionId);
        if (!component)
            return nullptr;

        return component->DynamicCast<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent, 3689192266));
    }

    UIFloatOptionComponent* Ray_OptionMenuHelper::findFloatOptionComponent(const StringID& optionId) const
    {
        UIComponent* component = findOptionComponent(optionId);
        if (!component)
            return nullptr;

        return component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316));
    }

    UIComponent* Ray_OptionMenuHelper::findComponentByFriendlyName(const char* friendlyName) const
    {
        if (!m_menu || !friendlyName || friendlyName[0] == '\0')
            return nullptr;

        const ObjectRefList& componentsList = m_menu->getUIComponentsList();
        for (u32 i = 0; i < componentsList.size(); ++i)
        {
            UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
            if (!comp)
                continue;

            Actor* actor = comp->GetActor();
            if (!actor)
                continue;

            const String8& compFriendly = actor->getUserFriendly();
            if (compFriendly.isEmpty())
                continue;

            if (compFriendly.equals(friendlyName, bfalse))
                return comp;
        }

        return nullptr;
    }

    UIComponent* Ray_OptionMenuHelper::getNavigationTarget(UIComponent* current, ENavigationDirection direction) const
    {
        if (!current)
            return nullptr;

        Actor* actor = current->GetActor();
        if (!actor)
            return nullptr;

        const String8& friendly = actor->getUserFriendly();
        if (friendly.isEmpty())
            return nullptr;

        for (size_t i = 0; i < s_optionNavigationEntryCount; ++i)
        {
            const OptionNavigationEntry& entry = s_optionNavigationEntries[i];
            if (!entry.friendly || !friendly.equals(entry.friendly, bfalse))
                continue;

            const char* targetFriendly = nullptr;
            switch (direction)
            {
            case Navigation_Up:    targetFriendly = entry.up; break;
            case Navigation_Down:  targetFriendly = entry.down; break;
            case Navigation_Left:  targetFriendly = entry.left; break;
            case Navigation_Right: targetFriendly = entry.right; break;
            default: break;
            }

            if (!targetFriendly)
                return nullptr;

            return findComponentByFriendlyName(targetFriendly);
        }

        return nullptr;
    }

    ObjectRef Ray_OptionMenuHelper::getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY) const
    {
        if (!m_isActive || !isNavigating() || !m_menu || !current)
            return ObjectRef::InvalidRef;

        const f32 absJoyX = f32_Abs(joyX);
        const f32 absJoyY = f32_Abs(joyY);
        if (absJoyX < MTH_EPSILON && absJoyY < MTH_EPSILON)
            return ObjectRef::InvalidRef;

        ENavigationDirection direction;
        if (absJoyX >= absJoyY)
        {
            if (absJoyX < MTH_EPSILON)
                return ObjectRef::InvalidRef;
            direction = (joyX > 0.0f) ? Navigation_Right : Navigation_Left;
        }
        else
        {
            if (absJoyY < MTH_EPSILON)
                return ObjectRef::InvalidRef;
            direction = (joyY > 0.0f) ? Navigation_Down : Navigation_Up;
        }

        UIComponent* target = getNavigationTarget(current, direction);
        if (!target || target == current || !target->getActive() || !target->getCanBeSelected())
            return ObjectRef::InvalidRef;

        return target->getUIref();
    }

    void Ray_OptionMenuHelper::UpdateResolutionText()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIListOptionComponent* listComponent = findListOptionComponent(OPTION_RESOLUTION))
        {
            const i32 index = RAY_GAMEMANAGER->getResolutionIndex();
            if (index >= 0)
            {
                updateListOptionDisplay(listComponent, OPTION_RESOLUTION, index);
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateLanguageText()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIListOptionComponent* listComponent = findListOptionComponent(OPTION_LANGUAGE))
        {
            const i32 index = RAY_GAMEMANAGER->getLanguageIndex();
            if (index >= 0)
            {
                updateListOptionDisplay(listComponent, OPTION_LANGUAGE, index);
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateStartWithHeartText()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIListOptionComponent* listComponent = findListOptionComponent(OPTION_START_WITH_HEART))
        {
            const i32 index = RAY_GAMEMANAGER->getStartWithHeartIndex();
            if (index >= 0)
            {
                updateListOptionDisplay(listComponent, OPTION_START_WITH_HEART, index);
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateRunButtonText()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIListOptionComponent* listComponent = findListOptionComponent(OPTION_RUN_BUTTON))
        {
            const i32 index = RAY_GAMEMANAGER->getRunButtonMode();
            if (index >= 0)
            {
                updateListOptionDisplay(listComponent, OPTION_RUN_BUTTON, index);
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateVibrationText()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIListOptionComponent* listComponent = findListOptionComponent(OPTION_VIBRATIONS))
        {
            const i32 index = RAY_GAMEMANAGER->getVibrationMode();
            if (index >= 0)
            {
                updateListOptionDisplay(listComponent, OPTION_VIBRATIONS, index);
            }
        }
    }

    void Ray_OptionMenuHelper::UpdateMurfyAssistToggle()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIToggleOptionComponent* toggleComponent = findToggleOptionComponent(OPTION_MURFY_ASSIST))
        {
            toggleComponent->setValue(RAY_GAMEMANAGER->isMurfyAssistEnabled());
        }
    }

    void Ray_OptionMenuHelper::UpdateWindowCheckboxVisual()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIToggleOptionComponent* toggleComponent = findToggleOptionComponent(OPTION_WINDOWED))
        {
            toggleComponent->setValue(RAY_GAMEMANAGER->isWindowed());
        }
    }

    void Ray_OptionMenuHelper::UpdateMasterVolumeSlider()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIFloatOptionComponent* floatComponent = findFloatOptionComponent(OPTION_MASTER_VOLUME))
        {
            floatComponent->setValue(RAY_GAMEMANAGER->getMasterVolume(), btrue);
        }
    }

    void Ray_OptionMenuHelper::UpdateMusicVolumeSlider()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIFloatOptionComponent* floatComponent = findFloatOptionComponent(OPTION_MUSIC_VOLUME))
        {
            floatComponent->setValue(RAY_GAMEMANAGER->getMusicVolume(), btrue);
        }
    }

    void Ray_OptionMenuHelper::UpdateSFXVolumeSlider()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIFloatOptionComponent* floatComponent = findFloatOptionComponent(OPTION_SFX_VOLUME))
        {
            floatComponent->setValue(RAY_GAMEMANAGER->getSFXVolume(), btrue);
        }
    }

    void Ray_OptionMenuHelper::UpdateIntensitySlider()
    {
        if (!RAY_GAMEMANAGER)
            return;

        if (UIFloatOptionComponent* floatComponent = findFloatOptionComponent(OPTION_INTENSITY))
        {
            floatComponent->setValue(RAY_GAMEMANAGER->getIntensity(), btrue);
        }
    }

    void Ray_OptionMenuHelper::refreshAllOptionVisuals()
    {
        if (!m_menu)
            return;

        UpdateResolutionText();
        UpdateLanguageText();
        UpdateStartWithHeartText();
        UpdateRunButtonText();
        UpdateVibrationText();
        UpdateMurfyAssistToggle();
        UpdateWindowCheckboxVisual();
        UpdateMasterVolumeSlider();
        UpdateMusicVolumeSlider();
        UpdateSFXVolumeSlider();
        UpdateIntensitySlider();
    }

    void Ray_OptionMenuHelper::captureSnapshot()
    {
        if (!RAY_GAMEMANAGER)
        {
            m_hasSnapshot = bfalse;
            return;
        }

        m_snapshotResolutionIndex = RAY_GAMEMANAGER->getResolutionIndex();
        m_snapshotWindowed = RAY_GAMEMANAGER->isWindowed();
        m_snapshotLanguageIndex = RAY_GAMEMANAGER->getLanguageIndex();
        m_snapshotStartWithHeartIndex = RAY_GAMEMANAGER->getStartWithHeartIndex();
        m_snapshotRunButtonMode = RAY_GAMEMANAGER->getRunButtonMode();
        m_snapshotMurfyAssist = RAY_GAMEMANAGER->isMurfyAssistEnabled();
        m_snapshotVibrationMode = RAY_GAMEMANAGER->getVibrationMode();
        m_snapshotMasterVolume = RAY_GAMEMANAGER->getMasterVolume();
        m_snapshotMusicVolume = RAY_GAMEMANAGER->getMusicVolume();
        m_snapshotSFXVolume = RAY_GAMEMANAGER->getSFXVolume();
        m_snapshotIntensity = RAY_GAMEMANAGER->getIntensity();
        m_hasSnapshot = btrue;
    }

    void Ray_OptionMenuHelper::restoreSnapshot()
    {
        if (!RAY_GAMEMANAGER || !m_hasSnapshot)
            return;

        RAY_GAMEMANAGER->setResolutionIndex(m_snapshotResolutionIndex);
        RAY_GAMEMANAGER->setWindowed(m_snapshotWindowed);
        RAY_GAMEMANAGER->setLanguageIndex(m_snapshotLanguageIndex);
        RAY_GAMEMANAGER->setStartWithHeartIndex(m_snapshotStartWithHeartIndex);
        RAY_GAMEMANAGER->setRunButtonMode(m_snapshotRunButtonMode);
        RAY_GAMEMANAGER->setMurfyAssist(m_snapshotMurfyAssist);
        RAY_GAMEMANAGER->setVibrationMode(m_snapshotVibrationMode);
        RAY_GAMEMANAGER->setMasterVolume(m_snapshotMasterVolume);
        RAY_GAMEMANAGER->setMusicVolume(m_snapshotMusicVolume);
        RAY_GAMEMANAGER->setSFXVolume(m_snapshotSFXVolume);
        RAY_GAMEMANAGER->setIntensity(m_snapshotIntensity);
        RAY_GAMEMANAGER->applyDisplayOptions();
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
        else if (action == input_actionID_LeftHold)
        {
            UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316));

            // For list options, apply initial delay
            if (listOption)
            {
                if (m_timer > m_timeout)
                {
                    if (m_firstPressed && m_timer > m_timeoutJustPressed)
                    {
                        m_timer = 0.0f;
                        m_firstPressed = bfalse;
                    }
                    if (!m_firstPressed)
                    {
                        m_timer = 0.0f;
                        adjustListOption(listOption, m_currentEditingOption, -1);
                        return btrue;
                    }
                }
            }
            // For slider (float option), no delay needed - execute immediately when held
            else if (floatOption)
            {
                // If first press, execute immediately and reset timer
                if (m_firstPressed)
                {
                    m_firstPressed = bfalse;
                    m_timer = 0.0f;
                    adjustFloatOption(floatOption, m_currentEditingOption, -1);
                    return btrue;
                }
                // For continuous movement, execute every frame for smooth slider movement
                // No timeout check needed - slider should update continuously while held
                adjustFloatOption(floatOption, m_currentEditingOption, -1);
                return btrue;
            }
        }
        else if (action == input_actionID_RightHold)
        {
            UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316));

            // For list options, apply initial delay
            if (listOption)
            {
                if (m_timer > m_timeout)
                {
                    if (m_firstPressed && m_timer > m_timeoutJustPressed)
                    {
                        m_timer = 0.0f;
                        m_firstPressed = bfalse;
                    }
                    if (!m_firstPressed)
                    {
                        m_timer = 0.0f;
                        adjustListOption(listOption, m_currentEditingOption, 1);
                        return btrue;
                    }
                }
            }
            // For slider (float option), no delay needed - execute immediately when held
            else if (floatOption)
            {
                // If first press, execute immediately and reset timer
                if (m_firstPressed)
                {
                    m_firstPressed = bfalse;
                    m_timer = 0.0f;
                    adjustFloatOption(floatOption, m_currentEditingOption, 1);
                    return btrue;
                }
                // For continuous movement, execute every frame for smooth slider movement
                // No timeout check needed - slider should update continuously while held
                adjustFloatOption(floatOption, m_currentEditingOption, 1);
                return btrue;
            }
        }

        return bfalse;
    }
}
