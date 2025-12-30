#include "precompiled_gameplay_rayman.h"

#include "engine/localisation/LocalisationManager.h"

#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif

#ifndef _ITF_UPLAYSERVICE_
#include "engine/networkservices/UPlayService.h"
#endif

#ifndef _ITF_ONLINEADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineAdapter.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineError.h"
#endif

#ifndef ITF_ONLINE_ADAPTER_UBISERVICES_H
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/SessionService_Ubiservices.h"
#endif //ITF_ONLINE_ADAPTER_UBISERVICES_H

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
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

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionManager.h"
#endif

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstring>
#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif
#include "GameScreens/Ray_GameScreen_Gameplay.h"
#include "GameScreens/Ray_GameScreen_MainMenu.h"

namespace ITF
{
#if !defined(ITF_WINDOWS) || defined(ITF_FORCE_CONSOLE_OPTION_MENU)
    #define ITF_OPTIONMENU_CONSOLE_LAYOUT 1
#endif

    namespace
    {
        static void applyRelativePosOffset(UIComponent* component, const Vec2d& offset)
        {
            if (!component)
                return;

            component->setRelativePos(Vec2d(
                component->getRelativePosX() + offset.m_x,
                component->getRelativePosY() + offset.m_y));
        }

        static void applyRelativePosOffsetToActor(Actor* actor, const Vec2d& offset)
        {
            if (!actor)
                return;

            if (UIComponent* ui = actor->GetComponent<UIComponent>())
                applyRelativePosOffset(ui, offset);
        }

        static void repositionListOptionInteractiveElements(UIListOptionComponent* listOption, const Vec2d& offset)
        {
            if (!listOption)
                return;
            if (UIGameOptionComponent* gameOption = listOption->DynamicCast<UIGameOptionComponent>(
                    ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641)))
            {
                applyRelativePosOffsetToActor(gameOption->getLabelActor(), offset);
                applyRelativePosOffsetToActor(gameOption->getSelectedBackgroundActor(), offset);
            }

            applyRelativePosOffsetToActor(listOption->getValueActor(), offset);
            applyRelativePosOffsetToActor(listOption->getLeftArrowActor(), offset);
            applyRelativePosOffsetToActor(listOption->getLeftArrowHighlightActor(), offset);
            applyRelativePosOffsetToActor(listOption->getRightArrowActor(), offset);
            applyRelativePosOffsetToActor(listOption->getRightArrowHighlightActor(), offset);
        }

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
            { "resolution_option",        "sfx_volume_option",       "window_option",          "start_with_heart_option",  "start_with_heart_option" },
            { "window_option",            "resolution_option",       "language_option",        "run_button_option",        "run_button_option" },
            { "language_option",          "window_option",           "master_volume_option",   "vibration_option",         "vibration_option" },
            { "master_volume_option",     "language_option",         "music_volume_option",    "intensity_option",         "intensity_option" },
            { "music_volume_option",      "master_volume_option",    "sfx_volume_option",      "ubisoftconnect_button",    "ubisoftconnect_button" },
            { "sfx_volume_option",        "music_volume_option",     "resolution_option",      "ubisoftconnect_button",    "ubisoftconnect_button" },
            { "ubisoftconnect_button",    "intensity_option",        "start_with_heart_option","sfx_volume_option",        "sfx_volume_option" },
            { "start_with_heart_option",  "ubisoftconnect_button",   "run_button_option",      "resolution_option",        "resolution_option" },
            { "run_button_option",        "start_with_heart_option", "vibration_option",       "window_option",            "window_option" },
            { "vibration_option",         "run_button_option",       "intensity_option",       "language_option",          "language_option" },
            { "intensity_option",         "vibration_option",        "ubisoftconnect_button",  "master_volume_option",     "master_volume_option" },
        };

        static const size_t s_optionNavigationEntryCount = sizeof(s_optionNavigationEntries) / sizeof(s_optionNavigationEntries[0]);
    }

    Ray_OptionMenuHelper* Ray_OptionMenuHelper::s_activeHelper = nullptr;

    void Ray_OptionMenuHelper::requestExitEditMode(UIComponent* component)
    {
        if (!s_activeHelper || !component)
            return;

        if (s_activeHelper->isEditing() && s_activeHelper->m_currentEditingComponent == component)
        {
            s_activeHelper->exitEditMode();
        }
    }

    Ray_OptionMenuHelper::Ray_OptionMenuHelper()
        : Ray_BaseMenuHelper()
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
          , m_showLanguageWarning(bfalse)
          , m_eventListenerRegistered(bfalse)
          , m_hasEditSnapshot(bfalse)
          , m_editSnapshotType(EditSnapshot_None)
          , m_editSnapshotOption(StringID::Invalid)
          , m_editSnapshotListIndex(-1)
          , m_editSnapshotFloatValue(0.0f)
          , m_editSnapshotBoolValue(bfalse)
          , m_hasDisplayOptionsBackgroundOverride(bfalse)
          , m_displayOptionsBackgroundOriginalQuadSize(Vec2d::Zero)
          , m_displayOptionsBackgroundOriginalRelativePos(Vec2d::Zero)
#if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
          , m_consoleLayoutOptionMenuInstance(nullptr)
          , m_consoleLayoutLanguageRepositionApplied(bfalse)
#endif
    {
        m_menuBaseName = OPTION_MENU_NAME;
    }

    Ray_OptionMenuHelper::~Ray_OptionMenuHelper()
    {
        unregisterEventListeners();
        if (s_activeHelper == this)
            s_activeHelper = nullptr;
    }

    void Ray_OptionMenuHelper::activateForOptionMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        UIMenu* menu = UI_MENUMANAGER->getMenu(OPTION_MENU_NAME);
        if (!menu)
            return;

        m_isActive = btrue;
        s_activeHelper = this;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(OPTION_MENU_NAME, this);
        m_menu = menu;

        registerEventListeners();
        if (RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->registerResolutionOption();
        }
        onActivate();
        initializeMenuState();

#if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
        if (m_consoleLayoutOptionMenuInstance != m_menu)
        {
            m_consoleLayoutOptionMenuInstance = m_menu;
            m_consoleLayoutLanguageRepositionApplied = bfalse;
        }

        setOptionComponentHidden(OPTION_RESOLUTION, btrue);
        setOptionComponentHidden(OPTION_WINDOWED, btrue);
        if (!m_consoleLayoutLanguageRepositionApplied)
        {
            if (UIListOptionComponent* languageList = findListOptionComponent(OPTION_LANGUAGE))
            {
                if (UIComponent* resolutionComponent = findOptionComponent(OPTION_RESOLUTION))
                {
                    const Vec2d offset(
                        resolutionComponent->getRelativePosX() - languageList->getRelativePosX(),
                        resolutionComponent->getRelativePosY() - languageList->getRelativePosY());

                    repositionListOptionInteractiveElements(languageList, offset);
                    m_consoleLayoutLanguageRepositionApplied = btrue;
                }
            }
        }

        if (UI_MENUMANAGER)
        {
            if (UIComponent* languageComponent = findOptionComponent(OPTION_LANGUAGE))
            {
                if (languageComponent->getActive() && languageComponent->getCanBeSelected())
                {
                    UIComponent* selected = m_menu ? m_menu->getUIComponentSelected() : nullptr;
                    UI_MENUMANAGER->applySelectionChange(m_menu, selected, languageComponent);
                }
            }
        }

        if (!m_hasDisplayOptionsBackgroundOverride)
        {
            Actor* backgroundActor = m_menu->findActorInMenuWorldByUserFriendly("option_background");
            if (backgroundActor)
            {
                UIMenuItemText* backgroundUI = backgroundActor->GetComponent<UIMenuItemText>();
                if (TextureGraphicComponent2D* backgroundTex = backgroundActor->GetComponent<TextureGraphicComponent2D>())
                {
                    const Vec2d originalQuadSize = backgroundTex->getQuadSize();
                    if (originalQuadSize.m_y > 0.0f)
                    {
                        m_hasDisplayOptionsBackgroundOverride = btrue;
                        m_displayOptionsBackgroundOriginalQuadSize = originalQuadSize;
                        if (backgroundUI)
                            m_displayOptionsBackgroundOriginalRelativePos = Vec2d(backgroundUI->getRelativePosX(), backgroundUI->getRelativePosY());

                        Vec2d newQuadSize = originalQuadSize;
                        newQuadSize.m_y *= 0.75f;
                        backgroundTex->setQuadSize(newQuadSize);
                        Actor* accessibilityBackgroundActor = m_menu->findActorInMenuWorldByUserFriendly("accessibility_background");
                        UIComponent* accessibilityUI = accessibilityBackgroundActor ? accessibilityBackgroundActor->GetComponent<UIComponent>() : nullptr;
                        if (backgroundUI && accessibilityUI)
                        {
                            const f32 targetRelY = accessibilityUI->getRelativePosY();
                            const f32 deltaRelY = targetRelY - backgroundUI->getRelativePosY();
                            backgroundUI->setRelativePos(Vec2d(backgroundUI->getRelativePosX(), backgroundUI->getRelativePosY() + deltaRelY));
                            backgroundUI->onResourceLoaded();
                        }
                    }
                }
            }
        }
#endif

        updateVibrationOptionAvailability();
        showContextIcons();
    }

    void Ray_OptionMenuHelper::registerEventListeners()
    {
        if (m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->registerEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344), this);
            m_eventListenerRegistered = btrue;
        }
    }


    void Ray_OptionMenuHelper::setOptionComponentHidden(const StringID& optionId, bbool hidden)
    {
        if (!m_menu || !optionId.isValid())
            return;

        UIComponent* component = findOptionComponent(optionId);
        if (!component)
            return;
        setOptionComponentDeactivated(optionId, hidden);

        auto disableActor = [](Actor* actor)
        {
            if (actor)
                actor->disable();
        };

        auto enableActor = [](Actor* actor)
        {
            if (actor)
                actor->enable();
        };

        if (hidden)
        {
            disableActor(component->GetActor());

            if (UIGameOptionComponent* optionComponent = component->DynamicCast<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641)))
            {
                disableActor(optionComponent->getLabelActor());
                disableActor(optionComponent->getSelectedBackgroundActor());

                if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
                {
                    disableActor(listOption->getValueActor());
                    disableActor(listOption->getLeftArrowActor());
                    disableActor(listOption->getLeftArrowHighlightActor());
                    disableActor(listOption->getRightArrowActor());
                    disableActor(listOption->getRightArrowHighlightActor());
                }

                if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
                {
                    disableActor(floatOption->getSliderBackgroundStartActor());
                    disableActor(floatOption->getSliderBackgroundEndActor());
                    disableActor(floatOption->getSliderCursorActor());
                    disableActor(floatOption->getSliderBackgroundSelectedActor());
                    disableActor(floatOption->getSliderCursorSelectedActor());
                }

                if (UIToggleOptionComponent* toggleOption = component->DynamicCast<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent, 3689192266)))
                {
                    disableActor(toggleOption->getCheckboxOnActor());
                    disableActor(toggleOption->getCheckboxOffActor());
                }
            }

            return;
        }
        enableActor(component->GetActor());
        if (UIGameOptionComponent* optionComponent = component->DynamicCast<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641)))
        {
            enableActor(optionComponent->getLabelActor());
            if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
            {
                enableActor(listOption->getValueActor());
            }

            if (UIFloatOptionComponent* floatOption = component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
            {
                enableActor(floatOption->getSliderBackgroundStartActor());
                enableActor(floatOption->getSliderBackgroundEndActor());
                enableActor(floatOption->getSliderCursorActor());
            }
        }
    }

    void Ray_OptionMenuHelper::unregisterEventListeners()
    {
        if (!m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344), this);
        }

        m_eventListenerRegistered = bfalse;
    }

    void Ray_OptionMenuHelper::updateVibrationOptionAvailability()
    {
        if (!m_isActive || !m_menu || !INPUT_ADAPTER)
            return;

        const bbool hasConnectedGamepad = INPUT_ADAPTER->getGamePadCount() > 0;
        setOptionComponentDeactivated(OPTION_VIBRATIONS, !hasConnectedGamepad);
    }

    void Ray_OptionMenuHelper::onEvent(Event* _event)
    {
        if (!_event)
            return;

        if (_event->DynamicCast<EventControllerStateChanged>(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344)))
        {
            updateVibrationOptionAvailability();
        }
    }

    void Ray_OptionMenuHelper::setOptionComponentDeactivated(const StringID& optionId, bbool deactivated)
    {
        if (!m_menu || !optionId.isValid())
            return;

        UIComponent* component = findOptionComponent(optionId);
        if (!component)
            return;

        if (deactivated && isEditing() && (component == m_currentEditingComponent || optionId == m_currentEditingOption))
        {
            exitEditMode();
        }

        const bbool shouldBeActive = !deactivated;
        component->setActive(shouldBeActive);
        if (deactivated || (isEditing() && component != m_currentEditingComponent))
            component->setCanBeSelected(bfalse);
        else
            component->setCanBeSelected(btrue);

        if (UIGameOptionComponent* optionComponent = component->DynamicCast<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641)))
        {
            Actor* labelActor = optionComponent->getLabelActor();
            if (labelActor)
            {
                if (UIComponent* labelComponent = labelActor->GetComponent<UIComponent>())
                {
                    labelComponent->setActive(shouldBeActive);
                }
            }

            if (UIListOptionComponent* listOption = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
            {
                Actor* valueActor = listOption->getValueActor();
                if (valueActor)
                {
                    if (UIComponent* valueComponent = valueActor->GetComponent<UIComponent>())
                    {
                        valueComponent->setActive(shouldBeActive);
                    }
                }

                if (deactivated)
                {
                    listOption->setEditingMode(bfalse);
                }
            }
        }

        if (deactivated)
        {
            ensureValidSelection();
        }
    }

    void Ray_OptionMenuHelper::onActivate()
    {
        // Called after menu is set up, override point for derived classes
    }

    void Ray_OptionMenuHelper::showContextIcons()
    {
        updateContextIconsForState();
    }

    void Ray_OptionMenuHelper::updateContextIconsForState()
    {
        if (!m_isActive || !CONTEXTICONSMANAGER)
            return;

        if (isEditing())
        {
            CONTEXTICONSMANAGER->show(ContextIcon_EditConfirm, ContextIcon_EditBack);
        }
        else
        {
            const EContextIcon topRightIcon = hasNonDefaultOptions() ? ContextIcon_ResetControlToDefault : ContextIcon_Invalid;
            CONTEXTICONSMANAGER->show(ContextIcon_Confirm, ContextIcon_Cancel, ContextIcon_Select, topRightIcon);
        }
    }

    void Ray_OptionMenuHelper::captureEditSnapshot(UIComponent* component, const StringID& optionId)
    {
        m_hasEditSnapshot = bfalse;
        m_editSnapshotType = EditSnapshot_None;
        m_editSnapshotOption = StringID::Invalid;
        m_editSnapshotListIndex = -1;
        m_editSnapshotFloatValue = 0.0f;
        m_editSnapshotBoolValue = bfalse;

        if (!component || !optionId.isValid() || !RAY_GAMEMANAGER)
            return;

        m_editSnapshotOption = optionId;

        if (component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669)))
        {
            m_editSnapshotType = EditSnapshot_ListIndex;
            if (optionId == OPTION_RESOLUTION)
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getResolutionIndex();
            else if (optionId == OPTION_LANGUAGE)
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getLanguageIndex();
            else if (optionId == OPTION_START_WITH_HEART)
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getStartWithHeartIndex();
            else if (optionId == OPTION_RUN_BUTTON)
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getRunButtonMode();
            else if (optionId == OPTION_VIBRATIONS)
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getVibrationMode();
            else
                m_editSnapshotListIndex = RAY_GAMEMANAGER->getGameOptionManager().getListOptionIndex(optionId, -1);

            m_hasEditSnapshot = (m_editSnapshotListIndex >= 0);
            return;
        }

        if (component->DynamicCast<UIFloatOptionComponent>(ITF_GET_STRINGID_CRC(UIFloatOptionComponent, 226609316)))
        {
            m_editSnapshotType = EditSnapshot_Float;
            if (optionId == OPTION_MASTER_VOLUME)
                m_editSnapshotFloatValue = RAY_GAMEMANAGER->getMasterVolume();
            else if (optionId == OPTION_MUSIC_VOLUME)
                m_editSnapshotFloatValue = RAY_GAMEMANAGER->getMusicVolume();
            else if (optionId == OPTION_SFX_VOLUME)
                m_editSnapshotFloatValue = RAY_GAMEMANAGER->getSFXVolume();
            else if (optionId == OPTION_INTENSITY)
                m_editSnapshotFloatValue = RAY_GAMEMANAGER->getIntensity();
            else
                m_editSnapshotFloatValue = RAY_GAMEMANAGER->getGameOptionManager().getFloatOption(optionId, -1.0f);

            m_hasEditSnapshot = btrue;
            return;
        }

        if (component->DynamicCast<UIToggleOptionComponent>(ITF_GET_STRINGID_CRC(UIToggleOptionComponent, 3689192266)))
        {
            m_editSnapshotType = EditSnapshot_Bool;
            if (optionId == OPTION_WINDOWED)
                m_editSnapshotBoolValue = RAY_GAMEMANAGER->isWindowed();
            else if (optionId == OPTION_MURFY_ASSIST)
                m_editSnapshotBoolValue = RAY_GAMEMANAGER->isMurfyAssistEnabled();
            else
                m_editSnapshotBoolValue = RAY_GAMEMANAGER->getGameOptionManager().getBoolOption(optionId, bfalse);

            m_hasEditSnapshot = btrue;
            return;
        }
    }

    void Ray_OptionMenuHelper::restoreEditSnapshot()
    {
        if (!m_hasEditSnapshot || !m_editSnapshotOption.isValid() || !RAY_GAMEMANAGER)
            return;

        const StringID optionId = m_editSnapshotOption;

        if (m_editSnapshotType == EditSnapshot_ListIndex)
        {
            const i32 idx = m_editSnapshotListIndex;
            if (idx < 0)
                return;

            if (optionId == OPTION_RESOLUTION)
                RAY_GAMEMANAGER->setResolutionIndex(idx);
            else if (optionId == OPTION_LANGUAGE)
            {
                if (RAY_GAMEMANAGER->getCurrentGameScreen() != Ray_GameScreen_MainMenu::GetClassCRCStatic())
                {
                    RAY_GAMEMANAGER->setPendingLanguageIndex(idx);
                    m_showLanguageWarning = bfalse;
                }
                else
                {
                    RAY_GAMEMANAGER->setLanguageIndex(idx);
                }
            }
            else if (optionId == OPTION_START_WITH_HEART)
            {
                RAY_GAMEMANAGER->setStartWithHeartIndex(idx);
                if (RAY_GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_WORLDMAP))
                {
                    RAY_GAMEMANAGER->applyHealthModifierForAllPlayers();
                }
            }
            else if (optionId == OPTION_RUN_BUTTON)
                RAY_GAMEMANAGER->setRunButtonMode(idx);
            else if (optionId == OPTION_VIBRATIONS)
                RAY_GAMEMANAGER->setVibrationMode(idx);
            else
                RAY_GAMEMANAGER->getGameOptionManager().setListOptionIndex(optionId, idx);

            if (UIListOptionComponent* listComp = findListOptionComponent(optionId))
                updateListOptionDisplay(listComp, optionId, idx);
            return;
        }

        if (m_editSnapshotType == EditSnapshot_Float)
        {
            const f32 val = m_editSnapshotFloatValue;
            if (optionId == OPTION_MASTER_VOLUME)
                RAY_GAMEMANAGER->setMasterVolume(val);
            else if (optionId == OPTION_MUSIC_VOLUME)
                RAY_GAMEMANAGER->setMusicVolume(val);
            else if (optionId == OPTION_SFX_VOLUME)
                RAY_GAMEMANAGER->setSFXVolume(val);
            else if (optionId == OPTION_INTENSITY)
                RAY_GAMEMANAGER->setIntensity(val);
            else
                RAY_GAMEMANAGER->getGameOptionManager().setFloatOption(optionId, val);

            if (UIFloatOptionComponent* floatComp = findFloatOptionComponent(optionId))
                floatComp->setValue(val, btrue);
            return;
        }

        if (m_editSnapshotType == EditSnapshot_Bool)
        {
            const bbool val = m_editSnapshotBoolValue;
            if (optionId == OPTION_WINDOWED)
                RAY_GAMEMANAGER->setWindowed(val);
            else if (optionId == OPTION_MURFY_ASSIST)
                RAY_GAMEMANAGER->setMurfyAssist(val);
            else
                RAY_GAMEMANAGER->getGameOptionManager().setBoolOption(optionId, val);

            if (UIToggleOptionComponent* toggleComp = findToggleOptionComponent(optionId))
                toggleComp->setValue(val);
            return;
        }
    }

    void Ray_OptionMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component)
            return;
        const StringID componentId = component->getID();
        if (componentId.isValid())
        {
            if (componentId == OPTIONMENU_UBISOFTCONNECT_BUTTON)
            {
                if (isEditing())
                {
                    exitEditMode();
                }

            }

            if (handleConnect(componentId))
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

    ITF::StringID ITF::Ray_OptionMenuHelper::onMenuPageAction(ITF::UIMenu* menu, const ITF::StringID& action, const ITF::StringID& defaultAction)
    {
        if (!UI_MENUMANAGER)
            return defaultAction;
        if (isEditing())
        {
            if (action == input_actionID_Back || action == input_actionID_DeleteSave)
                return UI_MENUMANAGER->getMenuPageAction_Nothing();

            return Ray_BaseMenuHelper::onMenuPageAction(menu, action, defaultAction);
        }

        if (action == input_actionID_Other)
        {
            if (hasNonDefaultOptions())
            {
                resetAllOptionsToDefault();
                showContextIcons();
            }
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        if (action == input_actionID_DeleteSave)
        {
            applyAndClose();
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        if (action == input_actionID_Back)
        {
            cancelAndClose();
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        return Ray_BaseMenuHelper::onMenuPageAction(menu, action, defaultAction);
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

    bbool Ray_OptionMenuHelper::hasNonDefaultOptions() const
    {
        if (!RAY_GAMEMANAGER)
            return bfalse;

        const Ray_GameOptionManager& optionManager = RAY_GAMEMANAGER->getGameOptionManager();

        const StringID optionIds[] = {
            OPTION_RESOLUTION,
            OPTION_WINDOWED,
            OPTION_LANGUAGE,
            OPTION_START_WITH_HEART,
            OPTION_RUN_BUTTON,
            OPTION_MURFY_ASSIST,
            OPTION_VIBRATIONS,
            OPTION_MASTER_VOLUME,
            OPTION_MUSIC_VOLUME,
            OPTION_SFX_VOLUME,
            OPTION_INTENSITY,
        };

        for (u32 i = 0; i < (sizeof(optionIds) / sizeof(optionIds[0])); ++i)
        {
            const Ray_GameOption* opt = optionManager.getOption(optionIds[i]);
            if (!opt)
                continue;

            switch (opt->getType())
            {
            case Ray_GameOption::OptionType_Bool:
                {
                    const Ray_GameOptionBool* boolOpt = static_cast<const Ray_GameOptionBool*>(opt);
                    if (boolOpt->getValue() != boolOpt->getDefaultValue())
                        return btrue;
                }
                break;
            case Ray_GameOption::OptionType_Int:
                {
                    const Ray_GameOptionInt* intOpt = static_cast<const Ray_GameOptionInt*>(opt);
                    if (intOpt->getValue() != intOpt->getDefaultValue())
                        return btrue;
                }
                break;
            case Ray_GameOption::OptionType_Float:
                {
                    const Ray_GameOptionFloat* floatOpt = static_cast<const Ray_GameOptionFloat*>(opt);
                    if (f32_Abs(floatOpt->getValue() - floatOpt->getDefaultValue()) > Ray_OptionMenuHelperConstants::FLOAT_EPSILON)
                        return btrue;
                }
                break;
            case Ray_GameOption::OptionType_IntList:
                {
                    const Ray_GameOptionIntList* listOpt = static_cast<const Ray_GameOptionIntList*>(opt);
                    if (listOpt->getSelectedIndex() != listOpt->getDefaultIndex())
                        return btrue;
                }
                break;
            case Ray_GameOption::OptionType_FloatList:
                {
                    const Ray_GameOptionFloatList* listOpt = static_cast<const Ray_GameOptionFloatList*>(opt);
                    if (listOpt->getSelectedIndex() != listOpt->getDefaultIndex())
                        return btrue;
                }
                break;
            case Ray_GameOption::OptionType_StringList:
                {
                    const Ray_GameOptionStringList* listOpt = static_cast<const Ray_GameOptionStringList*>(opt);
                    if (listOpt->getSelectedIndex() != listOpt->getDefaultIndex())
                        return btrue;
                }
                break;
            default:
                break;
            }
        }

        return bfalse;
    }

    void Ray_OptionMenuHelper::resetAllOptionsToDefault()
    {
        if (!RAY_GAMEMANAGER)
            return;

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

        refreshAllOptionVisuals();
    }

    bbool Ray_OptionMenuHelper::handleConnect(const StringID& id)
    {
        if (id != OPTIONMENU_UBISOFTCONNECT_BUTTON)
            return bfalse;

#if defined(ITF_WINDOWS) && defined(ITF_SUPPORT_UPLAY)
        if (!UPLAYSERVICE || UPLAYSERVICE->isOverlayActive())
            return bfalse;

        i32 ret = UPLAYSERVICE->showOverlay();
        if (ret)
        {
            TRC_ADAPTER->addMessage(TRCManagerAdapter::ErrorContext::UOR_FirstPartyOffline);
        }
#elif defined(ITF_SUPPORT_UBISERVICES)
        if (!ONLINE_ADAPTER)
            return bfalse;

        OnlineError ret = ONLINE_ADAPTER->getSessionService()->launchConnect();
        LOG("[UBICONNECT] - Ray_OptionMenuHelper: overlay returned: %d", ret.getCode());
        if (ret.getType() != OnlineError::Success)
        {
            TRC_ADAPTER->addMessage(TRCManagerAdapter::ErrorContext::UOR_FirstPartyOffline);
        }
#endif
        return btrue;
    }

    void ITF::Ray_OptionMenuHelper::applyAndClose()
    {
        if (RAY_GAMEMANAGER)
        {
            if (m_showLanguageWarning)
            {
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Language_Warn);
                m_showLanguageWarning = false;
            }

            RAY_GAMEMANAGER->saveGameOptions();
            RAY_GAMEMANAGER->applyGameSetting(RAY_GAMEMANAGER->getResolutionIndex() != m_snapshotResolutionIndex);

            if (m_hasSnapshot &&
                RAY_GAMEMANAGER->getCurrentGameScreen() == Ray_GameScreen_Gameplay::GetClassCRCStatic() &&
                RAY_GAMEMANAGER->getStartWithHeartIndex() != m_snapshotStartWithHeartIndex)
            {
                TRC_ADAPTER->addMessage(TRCManagerAdapter::OptionMenu_ChangesApplyAfterRespawn);
            }
        }

        closeAndReturn();
    }

    void ITF::Ray_OptionMenuHelper::cancelAndClose()
    {
        m_showLanguageWarning = false;

        if (m_hasSnapshot)
        {
            restoreSnapshot();
        }

        refreshAllOptionVisuals();
        closeAndReturn();
    }

    void Ray_OptionMenuHelper::onClose()
    {
        unregisterEventListeners();
        exitEditMode();

        if (CONTEXTICONSMANAGER)
        {
            CONTEXTICONSMANAGER->hide();
        }

#if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
        if (m_hasDisplayOptionsBackgroundOverride && m_menu)
        {
            Actor* backgroundActor = m_menu->findActorInMenuWorldByUserFriendly("option_background");
            if (backgroundActor)
            {
                UIMenuItemText* backgroundUI = backgroundActor->GetComponent<UIMenuItemText>();
                if (TextureGraphicComponent2D* backgroundTex = backgroundActor->GetComponent<TextureGraphicComponent2D>())
                {
                    backgroundTex->setQuadSize(m_displayOptionsBackgroundOriginalQuadSize);
                    if (backgroundUI)
                    {
                        backgroundUI->setRelativePos(m_displayOptionsBackgroundOriginalRelativePos);
                        backgroundUI->onResourceLoaded();
                    }
                }
            }

            m_hasDisplayOptionsBackgroundOverride = bfalse;
            m_displayOptionsBackgroundOriginalQuadSize = Vec2d::Zero;
            m_displayOptionsBackgroundOriginalRelativePos = Vec2d::Zero;
        }
#endif

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentEditingComponent = nullptr;
        m_previousSelectionStates.clear();
        m_timer = 0.0f;
        m_firstPressed = btrue;
        m_hasSnapshot = bfalse;
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

        captureEditSnapshot(component, optionId);
        updateContextIconsForState();

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

        if (UIGameOptionComponent* optionComponent = component->DynamicCast<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641)))
        {
            optionComponent->setEditingMode(btrue);
        }
    }

    void Ray_OptionMenuHelper::exitEditMode()
    {
        if (m_menuState != MenuState_EditOption)
            return;

        if (m_currentEditingComponent)
        {
            UIGameOptionComponent* optionComponent = m_currentEditingComponent->DynamicCast<UIGameOptionComponent>(ITF_GET_STRINGID_CRC(UIGameOptionComponent, 3059104641));
            if (optionComponent)
            {
                optionComponent->setEditingMode(bfalse);
            }
        }

        for (auto& entry : m_previousSelectionStates)
        {
            if (!entry.first)
                continue;
            entry.first->setCanBeSelected(entry.second && entry.first->getActive());
        }
        m_previousSelectionStates.clear();

        m_menuState = MenuState_Navigate;
        m_currentEditingOption = StringID::Invalid;
        m_currentEditingComponent = nullptr;
        m_hasEditSnapshot = bfalse;
        m_editSnapshotType = EditSnapshot_None;
        m_editSnapshotOption = StringID::Invalid;
        updateContextIconsForState();
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

    void Ray_OptionMenuHelper::ensureValidSelection() const
    {
        if (!m_menu || !UI_MENUMANAGER)
            return;

        UIComponent* selected = m_menu->getUIComponentSelected();
        const auto isSelectableComponent = [](UIComponent* component) -> bbool
        {
            if (!component || !component->getActive() || !component->getCanBeSelected())
                return bfalse;

            Actor* actor = component->GetActor();
            return actor && actor->isEnabled();
        };

        if (isSelectableComponent(selected))
            return;

        UIComponent* candidate = nullptr;

        const ObjectRefList& componentsList = m_menu->getUIComponentsList();
        for (u32 i = 0; i < componentsList.size(); ++i)
        {
            UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
            if (!isSelectableComponent(comp))
                continue;

            if (comp->getIsDefaultSelected())
            {
                candidate = comp;
                break;
            }
        }

        if (!candidate)
        {
            for (u32 i = 0; i < componentsList.size(); ++i)
            {
                UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
                if (isSelectableComponent(comp))
                {
                    candidate = comp;
                    break;
                }
            }
        }

        if (candidate)
        {
            UI_MENUMANAGER->applySelectionChange(m_menu, selected, candidate);
        }
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
                            u32 lineId = 0;
                            if (optionId == OPTION_RESOLUTION)
                                displayName = RAY_GAMEMANAGER->getResolutionDisplayName(currentIndex);
                            else if (optionId == OPTION_LANGUAGE)
                                displayName = RAY_GAMEMANAGER->getLanguageDisplayName(currentIndex);
                            else if (optionId == OPTION_START_WITH_HEART)
                                lineId = RAY_GAMEMANAGER->getStartWithHeartLineId(currentIndex);
                            else if (optionId == OPTION_RUN_BUTTON)
                                lineId = RAY_GAMEMANAGER->getRunButtonLineId(currentIndex);
                            else if (optionId == OPTION_VIBRATIONS)
                                lineId = RAY_GAMEMANAGER->getVibrationLineId(currentIndex);

                            if (displayName && displayName[0] != '\0')
                            {
                                valueComp->forceContent(displayName);
                            }
                            if (lineId != 0) {
                                LocalisationId locId;
                                locId = lineId;
                                valueComp->setLineId(locId);
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
            if (RAY_GAMEMANAGER->getCurrentGameScreen() != Ray_GameScreen_MainMenu::GetClassCRCStatic())
            {
                RAY_GAMEMANAGER->setPendingLanguageIndex(newIndex);
                m_showLanguageWarning = btrue;
            }
            else
            {
                RAY_GAMEMANAGER->setLanguageIndex(newIndex);
            }

        }
        else if (optionId == OPTION_START_WITH_HEART)
        {
            RAY_GAMEMANAGER->setStartWithHeartIndex(newIndex);
            if (RAY_GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_WORLDMAP))
            {
                RAY_GAMEMANAGER->applyHealthModifierForAllPlayers();
            }
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
        u32 lineId = 0u;

        if (optionId == OPTION_RESOLUTION)
        {
            displayName = RAY_GAMEMANAGER->getResolutionDisplayName(index);
        }
        else if (optionId == OPTION_LANGUAGE)
        {
            displayName = RAY_GAMEMANAGER->getLanguageDisplayName(index);
        }
        else if (optionId == OPTION_START_WITH_HEART)
        {
            lineId = RAY_GAMEMANAGER->getStartWithHeartLineId(index);
        }
        else if (optionId == OPTION_RUN_BUTTON)
        {
            lineId = RAY_GAMEMANAGER->getRunButtonLineId(index);
        }
        else if (optionId == OPTION_VIBRATIONS)
        {
            lineId = RAY_GAMEMANAGER->getVibrationLineId(index);
        }

        if (lineId != 0u)
        {
            LocalisationId locId;
            locId = lineId;
            valueComp->setLineId(locId);
            return;
        }

        if (displayName && displayName[0] != '\0')
        {
            valueComp->forceContent(displayName);
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

    #if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
        const bbool languageAsResolution = friendly.equals("language_option", bfalse);
    #endif

        for (size_t i = 0; i < s_optionNavigationEntryCount; ++i)
        {
            const OptionNavigationEntry& entry = s_optionNavigationEntries[i];

#if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
            if (languageAsResolution)
            {
                if (!entry.friendly || std::strcmp(entry.friendly, "resolution_option") != 0)
                    continue;
            }
            else
#endif
            {
                if (!entry.friendly || !friendly.equals(entry.friendly, bfalse))
                    continue;
            }

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

#if defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
            if (languageAsResolution && direction == Navigation_Down)
            {
                targetFriendly = "master_volume_option";
            }
            if (std::strcmp(targetFriendly, "resolution_option") == 0 || std::strcmp(targetFriendly, "window_option") == 0)
            {
                targetFriendly = "language_option";
            }
#endif

            return findComponentByFriendlyName(targetFriendly);
        }

        return nullptr;
    }

    ObjectRef Ray_OptionMenuHelper::getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY)
    {
        if (!m_isActive || !m_menu || !current || TRC_ADAPTER->existsMessage(TRCManagerAdapter::Language_Warn))
            return ObjectRef::InvalidRef;

        const bbool editing = isEditing();
        if (!editing && !isNavigating())
            return ObjectRef::InvalidRef;

        const f32 absJoyX = f32_Abs(joyX);
        const f32 absJoyY = f32_Abs(joyY);
        if (absJoyX < MTH_EPSILON && absJoyY < MTH_EPSILON)
            return ObjectRef::InvalidRef;

        ENavigationDirection direction;
        const bbool preferHorizontal = absJoyX >= absJoyY;
        if (preferHorizontal)
        {
            if (absJoyX < MTH_EPSILON)
                return ObjectRef::InvalidRef;

            if (editing)
                return ObjectRef::InvalidRef;

            direction = (joyX > 0.0f) ? Navigation_Right : Navigation_Left;
        }
        else
        {
            if (absJoyY < MTH_EPSILON)
                return ObjectRef::InvalidRef;
            direction = (joyY > 0.0f) ? Navigation_Down : Navigation_Up;
        }

        UIComponent* navigationStart = current;
        if (editing)
        {
            if (m_currentEditingComponent)
            {
                navigationStart = m_currentEditingComponent;
            }

            exitEditMode();
        }

        if (!navigationStart)
            navigationStart = current;

        UIComponent* target = nullptr;
        UIComponent* navigationCursor = navigationStart;
        for (size_t attempt = 0; attempt < s_optionNavigationEntryCount; ++attempt)
        {
            UIComponent* candidate = getNavigationTarget(navigationCursor, direction);
            if (!candidate || candidate == navigationStart)
                break;

            Actor* candidateActor = candidate->GetActor();
            if (candidate->getActive() && candidate->getCanBeSelected() && candidateActor && candidateActor->isEnabled())
            {
                target = candidate;
                break;
            }

            navigationCursor = candidate;
        }

        return target ? target->getUIref() : ObjectRef::InvalidRef;
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

#if !defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
        UpdateResolutionText();
#endif
        UpdateLanguageText();
        UpdateStartWithHeartText();
        UpdateRunButtonText();
        UpdateVibrationText();
        UpdateMurfyAssistToggle();
#if !defined(ITF_OPTIONMENU_CONSOLE_LAYOUT)
        UpdateWindowCheckboxVisual();
#endif
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
            restoreEditSnapshot();
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
