#include "precompiled_gameplay_rayman.h"

#include "engine/actors/managers/ContextIconsManager.h"

#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_ControlsRemappingMenuHelper.h"
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
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

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif

#ifndef _ITF_UILISTOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIListOptionComponent.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif
#include <algorithm>

namespace ITF
{
#define CONTROLSREMAPPING_ACCEPT_BUTTON  ITF_GET_STRINGID_CRC(accept_button,25226343)
#define CONTROLSREMAPPING_CANCEL_BUTTON  ITF_GET_STRINGID_CRC(cancel_button,4260770984)

#if defined(ITF_WINDOWS)
#define CONTROLLER_OPTIONS_ID ITF_GET_STRINGID_CRC(controller_options,3547548201)
#endif

#define ICON_PLAYER1_UP     ITF_GET_STRINGID_CRC(player1_up_icon,1753979515)
#define ICON_PLAYER1_DOWN   ITF_GET_STRINGID_CRC(player1_down_icon,845618532)
#define ICON_PLAYER1_LEFT   ITF_GET_STRINGID_CRC(player1_left_icon,3896653396)
#define ICON_PLAYER1_RIGHT  ITF_GET_STRINGID_CRC(player1_right_icon,3424052495)
#define ICON_PLAYER1_RUN    ITF_GET_STRINGID_CRC(player1_run_icon,3457228901)
#define ICON_PLAYER1_JUMP   ITF_GET_STRINGID_CRC(player1_jump_icon,3525025255)
#define ICON_PLAYER1_HIT    ITF_GET_STRINGID_CRC(player1_hit_icon,4282892399)

#define ICON_PLAYER2_UP     ITF_GET_STRINGID_CRC(player2_up_icon,4288940261)
#define ICON_PLAYER2_DOWN   ITF_GET_STRINGID_CRC(player2_down_icon,441889963)
#define ICON_PLAYER2_LEFT   ITF_GET_STRINGID_CRC(player2_left_icon,2679901733)
#define ICON_PLAYER2_RIGHT  ITF_GET_STRINGID_CRC(player2_right_icon,456853284)
#define ICON_PLAYER2_RUN    ITF_GET_STRINGID_CRC(player2_run_icon,2628100497)
#define ICON_PLAYER2_JUMP   ITF_GET_STRINGID_CRC(player2_jump_icon,2380529337)
#define ICON_PLAYER2_HIT    ITF_GET_STRINGID_CRC(player2_hit_icon,425931337)

#define ICON_PLAYER3_UP     ITF_GET_STRINGID_CRC(player3_up_icon,1876599777)
#define ICON_PLAYER3_DOWN   ITF_GET_STRINGID_CRC(player3_down_icon,548047626)
#define ICON_PLAYER3_LEFT   ITF_GET_STRINGID_CRC(player3_left_icon,1679854463)
#define ICON_PLAYER3_RIGHT  ITF_GET_STRINGID_CRC(player3_right_icon,206088801)
#define ICON_PLAYER3_RUN    ITF_GET_STRINGID_CRC(player3_run_icon,1971818420)
#define ICON_PLAYER3_JUMP   ITF_GET_STRINGID_CRC(player3_jump_icon,3551164230)
#define ICON_PLAYER3_HIT    ITF_GET_STRINGID_CRC(player3_hit_icon,1301281176)

#define ICON_PLAYER4_UP     ITF_GET_STRINGID_CRC(player4_up_icon,4257930662)
#define ICON_PLAYER4_DOWN   ITF_GET_STRINGID_CRC(player4_down_icon,1841519437)
#define ICON_PLAYER4_LEFT   ITF_GET_STRINGID_CRC(player4_left_icon,2981630500)
#define ICON_PLAYER4_RIGHT  ITF_GET_STRINGID_CRC(player4_right_icon,2134331520)
#define ICON_PLAYER4_RUN    ITF_GET_STRINGID_CRC(player4_run_icon,3273055250)
#define ICON_PLAYER4_JUMP   ITF_GET_STRINGID_CRC(player4_jump_icon,3258270794)
#define ICON_PLAYER4_HIT    ITF_GET_STRINGID_CRC(player4_hit_icon,3161019062)

    struct IconMapping
    {
        StringID iconId;
        u32 playerIndex;
        ZInputManager::EGameAction action;
    };

    static const IconMapping s_iconMappings[] =
    {
        {ICON_PLAYER1_UP, 0, ZInputManager::Action_Up},
        {ICON_PLAYER1_DOWN, 0, ZInputManager::Action_Down},
        {ICON_PLAYER1_LEFT, 0, ZInputManager::Action_Left},
        {ICON_PLAYER1_RIGHT, 0, ZInputManager::Action_Right},
        {ICON_PLAYER1_RUN, 0, ZInputManager::Action_Run},
        {ICON_PLAYER1_JUMP, 0, ZInputManager::Action_Jump},
        {ICON_PLAYER1_HIT, 0, ZInputManager::Action_Hit},
        {ICON_PLAYER2_UP, 1, ZInputManager::Action_Up},
        {ICON_PLAYER2_DOWN, 1, ZInputManager::Action_Down},
        {ICON_PLAYER2_LEFT, 1, ZInputManager::Action_Left},
        {ICON_PLAYER2_RIGHT, 1, ZInputManager::Action_Right},
        {ICON_PLAYER2_RUN, 1, ZInputManager::Action_Run},
        {ICON_PLAYER2_JUMP, 1, ZInputManager::Action_Jump},
        {ICON_PLAYER2_HIT, 1, ZInputManager::Action_Hit},
        {ICON_PLAYER3_UP, 2, ZInputManager::Action_Up},
        {ICON_PLAYER3_DOWN, 2, ZInputManager::Action_Down},
        {ICON_PLAYER3_LEFT, 2, ZInputManager::Action_Left},
        {ICON_PLAYER3_RIGHT, 2, ZInputManager::Action_Right},
        {ICON_PLAYER3_RUN, 2, ZInputManager::Action_Run},
        {ICON_PLAYER3_JUMP, 2, ZInputManager::Action_Jump},
        {ICON_PLAYER3_HIT, 2, ZInputManager::Action_Hit},
        {ICON_PLAYER4_UP, 3, ZInputManager::Action_Up},
        {ICON_PLAYER4_DOWN, 3, ZInputManager::Action_Down},
        {ICON_PLAYER4_LEFT, 3, ZInputManager::Action_Left},
        {ICON_PLAYER4_RIGHT, 3, ZInputManager::Action_Right},
        {ICON_PLAYER4_RUN, 3, ZInputManager::Action_Run},
        {ICON_PLAYER4_JUMP, 3, ZInputManager::Action_Jump},
        {ICON_PLAYER4_HIT, 3, ZInputManager::Action_Hit},
    };

    static const u32 s_iconMappingsCount = sizeof(s_iconMappings) / sizeof(s_iconMappings[0]);

    Ray_ControlsRemappingMenuHelper::Ray_ControlsRemappingMenuHelper()
        : Ray_BaseMenuHelper()
          , m_hasCommittedChanges(bfalse)
          , m_hasSnapshot(bfalse)
#if defined(ITF_WINDOWS)
          , m_hasSnapshotPCControlMode(bfalse)
          , m_snapshotPCControlMode(0)
          , m_hasSnapshotPCControlModeEdit(bfalse)
          , m_snapshotPCControlModeEdit(0)
          , m_isEditingControllerType(bfalse)
          , m_editingControllerTypeComponent(nullptr)
          , m_controllerTypeChangeCooldown(0.0f)
#endif
    {
        m_menuBaseName = "controlremapping";

        for (u32 i = 0; i < 4; ++i)
        {
            m_isRemappingModeByPlayer[i] = bfalse;
            m_isWaitingForReleaseByPlayer[i] = bfalse;
            m_remappingActionByPlayer[i] = ZInputManager::Action_Up;
            m_remappingComponentByPlayer[i] = nullptr;
            m_remappingCooldownByPlayer[i] = 0.0f;
            m_postRemapCooldownByPlayer[i] = 0.0f;
            m_remappingSourceByPlayer[i] = InputSource_Gamepad;
            m_previousIconLineIdByPlayer[i] = U32_INVALID;
            m_selectedComponentRefByPlayer[i] = ObjectRef::InvalidRef;
            m_hasSelectedComponentByPlayer[i] = bfalse;

            m_exitDecisionByPlayer[i] = ExitDecision_None;
            m_readyStatusActorByPlayer[i] = nullptr;

            m_hasSnapshotGamepad[i] = bfalse;
            m_hasSnapshotKeyboard[i] = bfalse;

            m_hasLastActionByPlayer[i] = bfalse;
            m_lastActionByPlayer[i] = ZInputManager::Action_Up;
        }
    }

    Ray_ControlsRemappingMenuHelper::~Ray_ControlsRemappingMenuHelper()
    {
        if (s_activeHelper == this)
        {
            s_activeHelper = nullptr;
        }
    }

    namespace ControlsRemappingConstants
    {
        static const f32 REMAPPING_COOLDOWN = 0.25f;
        static const f32 POST_REMAP_COOLDOWN = 0.15f;
    }

    static const char* getReadyStatusActorNameForPlayer(u32 playerIndex)
    {
        switch (playerIndex)
        {
        case 0: return "p1_ready_status";
        case 1: return "p2_ready_status";
        case 2: return "p3_ready_status";
        case 3: return "p4_ready_status";
        default: return "";
        }
    }

    static const LocalisationId kControlsRemappingNewInputLineId = []()
    {
        LocalisationId id;
        id = 7135u;
        return id;
    }();

    Ray_ControlsRemappingMenuHelper* Ray_ControlsRemappingMenuHelper::s_activeHelper = nullptr;

    Ray_ControlsRemappingMenuHelper* Ray_ControlsRemappingMenuHelper::getActiveHelper()
    {
        return s_activeHelper;
    }

    void Ray_ControlsRemappingMenuHelper::activateForControlsRemappingMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        UIMenu* menu = UI_MENUMANAGER->getMenu(m_menuBaseName);
        if (!menu)
            return;
        m_isActive = btrue;
        for (u32 i = 0; i < 4; ++i)
        {
            m_isRemappingModeByPlayer[i] = bfalse;
            m_isWaitingForReleaseByPlayer[i] = bfalse;
            m_remappingComponentByPlayer[i] = nullptr;
            m_remappingCooldownByPlayer[i] = 0.0f;
            m_postRemapCooldownByPlayer[i] = 0.0f;
            m_remappingSourceByPlayer[i] = InputSource_Gamepad;
            m_previousIconLineIdByPlayer[i] = U32_INVALID;
            m_selectedComponentRefByPlayer[i] = ObjectRef::InvalidRef;
            m_hasSelectedComponentByPlayer[i] = bfalse;

            m_exitDecisionByPlayer[i] = ExitDecision_None;
            m_readyStatusActorByPlayer[i] = nullptr;
        }
#if defined(ITF_WINDOWS)
        m_isEditingControllerType = bfalse;
        m_editingControllerTypeComponent = nullptr;
        m_controllerTypeChangeCooldown = 0.0f;
        m_previousSelectionStates.clear();
        m_hasSnapshotPCControlModeEdit = bfalse;
#endif
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(m_menuBaseName, this);
        m_menu = menu;

        s_activeHelper = this;

        m_hasCommittedChanges = bfalse;

        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            const char* name = getReadyStatusActorNameForPlayer(playerIndex);
            Actor* statusActor = (name && name[0] && m_menu)
                                     ? m_menu->findActorInMenuWorldByUserFriendly(name)
                                     : nullptr;
            m_readyStatusActorByPlayer[playerIndex] = statusActor;
            m_exitDecisionByPlayer[playerIndex] = ExitDecision_None;
            if (statusActor)
            {
                statusActor->disable();
            }
        }

        for (u32 i = 0; i < 4; ++i)
        {
            m_hasLastActionByPlayer[i] = bfalse;
            m_lastActionByPlayer[i] = ZInputManager::Action_Up;
        }
        captureRemappingSnapshot();
        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            const bbool hasPad = (INPUT_ADAPTER && INPUT_ADAPTER->isPadConnected(playerIndex)) ? btrue : bfalse;
            if (playerIndex != 0 && !hasPad)
                continue;

            if (UIComponent* upComp = findIconComponent(playerIndex, ZInputManager::Action_Up))
            {
                m_selectedComponentRefByPlayer[playerIndex] = upComp->getUIref();
                m_hasSelectedComponentByPlayer[playerIndex] = btrue;
                upComp->setIsSelected(btrue);
                upComp->onRollover();
                UIMenuManager::notifySiblingUIComponents(upComp, btrue);
            }
        }

#if !defined(ITF_WINDOWS)
        {
            const ObjectRefList& componentsList = m_menu->getUIComponentsList();
            bbool disabledControllerOptions = bfalse;
            for (u32 i = 0; i < componentsList.size(); ++i)
            {
                UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
                if (!comp)
                    continue;

                Actor* actor = comp->GetActor();
                if (!actor)
                    continue;

                if (actor->getUserFriendly() == "controller_options")
                {
                    comp->setCanBeSelected(bfalse);
                    disabledControllerOptions = btrue;
                    break;
                }
            }

            if (disabledControllerOptions)
            {
                const auto isSelectableComponent = [](UIComponent* component) -> bbool
                {
                    if (!component || !component->getActive() || !component->getCanBeSelected())
                        return bfalse;

                    Actor* actor = component->GetActor();
                    return actor && actor->isEnabled();
                };

                UIComponent* selected = m_menu->getUIComponentSelected();
                if (!isSelectableComponent(selected))
                {
                    UIComponent* candidate = nullptr;

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
            }
        }
#endif

#if defined(ITF_WINDOWS)
        if (RAY_GAMEMANAGER)
        {
            UIListOptionComponent* ctrlTypeComp = findControllerTypeComponent();
            if (ctrlTypeComp)
            {
                i32 currentType = RAY_GAMEMANAGER->getPCControlMode();
                updateControllerTypeDisplay(ctrlTypeComp, currentType);
            }
        }
#endif

        showContextIcons();
    }

    bbool Ray_ControlsRemappingMenuHelper::isPlayerParticipatingForExit(u32 playerIndex) const
    {
        if (playerIndex >= 4)
            return bfalse;

        if (playerIndex == 0)
            return btrue;

        return (INPUT_ADAPTER && INPUT_ADAPTER->isPadConnected(playerIndex)) ? btrue : bfalse;
    }

    void Ray_ControlsRemappingMenuHelper::resetExitDecisionsAndStatusUI()
    {
        for (u32 i = 0; i < 4; ++i)
        {
            m_exitDecisionByPlayer[i] = ExitDecision_None;
            if (m_readyStatusActorByPlayer[i])
            {
                m_readyStatusActorByPlayer[i]->disable();
            }
        }
    }

    void Ray_ControlsRemappingMenuHelper::setExitDecisionForPlayer(u32 playerIndex, ExitDecision decision)
    {
        if (playerIndex >= 4)
            return;
        if (!isPlayerParticipatingForExit(playerIndex))
            return;

        m_exitDecisionByPlayer[playerIndex] = decision;

        Actor* actor = m_readyStatusActorByPlayer[playerIndex];
        if (!actor)
            return;

        if (decision == ExitDecision_None)
        {
            actor->disable();
            return;
        }

        actor->enable();
        if (UIComponent* ui = actor->GetComponent<UIComponent>())
        {
            ui->forceContent(String("Ready"));
            ui->onResourceLoaded();
        }
    }

    bbool Ray_ControlsRemappingMenuHelper::areAllParticipatingPlayersReadyToExit() const
    {
        for (u32 i = 0; i < 4; ++i)
        {
            if (!isPlayerParticipatingForExit(i))
                continue;
            if (m_exitDecisionByPlayer[i] == ExitDecision_None)
                return bfalse;
        }
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::restoreRemappingSnapshotForPlayer(u32 playerIndex)
    {
        if (!m_hasSnapshot)
            return;
        if (playerIndex >= 4)
            return;
        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return;

        ZInputManager* input = GAMEMANAGER->getInputManager();

        if (m_hasSnapshotGamepad[playerIndex])
        {
            input->ApplyRemapping(playerIndex, InputSource_Gamepad, m_snapshotGamepad[playerIndex]);
        }

#if defined(ITF_WINDOWS)
        if (m_hasSnapshotKeyboard[playerIndex])
        {
            input->ApplyRemapping(playerIndex, InputSource_Keyboard, m_snapshotKeyboard[playerIndex]);
        }
#endif
    }

    void Ray_ControlsRemappingMenuHelper::applyExitDecisionsAndClose()
    {
        if (!areAllParticipatingPlayersReadyToExit())
            return;

        m_hasCommittedChanges = btrue;
        if (RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->saveGameOptions();
        }

        closeAndReturn();
    }

    void Ray_ControlsRemappingMenuHelper::captureRemappingSnapshot()
    {
        m_hasSnapshot = bfalse;

#if defined(ITF_WINDOWS)
        m_hasSnapshotPCControlMode = bfalse;
        if (RAY_GAMEMANAGER)
        {
            m_snapshotPCControlMode = RAY_GAMEMANAGER->getPCControlMode();
            m_hasSnapshotPCControlMode = btrue;
            m_hasSnapshot = btrue;
        }
#endif

        for (u32 i = 0; i < 4; ++i)
        {
            m_hasSnapshotGamepad[i] = bfalse;
            m_hasSnapshotKeyboard[i] = bfalse;
            m_snapshotGamepad[i].clear();
            m_snapshotKeyboard[i].clear();
        }

        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return;

        ZInputManager* input = GAMEMANAGER->getInputManager();
        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            ITF_VECTOR<u32> mapping;
            if (input->GetRemapping(playerIndex, InputSource_Gamepad, mapping))
            {
                m_snapshotGamepad[playerIndex] = mapping;
                m_hasSnapshotGamepad[playerIndex] = btrue;
                m_hasSnapshot = btrue;
            }

#if defined(ITF_WINDOWS)
            mapping.clear();
            if (input->GetRemapping(playerIndex, InputSource_Keyboard, mapping))
            {
                m_snapshotKeyboard[playerIndex] = mapping;
                m_hasSnapshotKeyboard[playerIndex] = btrue;
                m_hasSnapshot = btrue;
            }
#endif
        }
    }

    void Ray_ControlsRemappingMenuHelper::restoreRemappingSnapshot()
    {
        if (!m_hasSnapshot)
            return;

#if defined(ITF_WINDOWS)
        if (m_hasSnapshotPCControlMode && RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->setPCControlMode(m_snapshotPCControlMode);

            UIListOptionComponent* ctrlTypeComp = findControllerTypeComponent();
            if (ctrlTypeComp)
            {
                updateControllerTypeDisplay(ctrlTypeComp, m_snapshotPCControlMode);
            }
        }
#endif

        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return;
        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            cancelRemappingMode(playerIndex, btrue);
            m_isWaitingForReleaseByPlayer[playerIndex] = bfalse;
            m_postRemapCooldownByPlayer[playerIndex] = 0.0f;
        }

        ZInputManager* input = GAMEMANAGER->getInputManager();

        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            if (m_hasSnapshotGamepad[playerIndex])
            {
                input->ApplyRemapping(playerIndex, InputSource_Gamepad, m_snapshotGamepad[playerIndex]);
            }

#if defined(ITF_WINDOWS)
            if (m_hasSnapshotKeyboard[playerIndex])
            {
                input->ApplyRemapping(playerIndex, InputSource_Keyboard, m_snapshotKeyboard[playerIndex]);
            }
#endif
        }
    }

    void Ray_ControlsRemappingMenuHelper::showContextIcons()
    {
        if (!m_isActive || !CONTEXTICONSMANAGER)
            return;
        {
            u32 forcedPlayerIndex = U32_INVALID;
            if (UI_MENUMANAGER)
            {
                const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
                if (inputPlayer != U32_INVALID && inputPlayer < 4)
                {
                    forcedPlayerIndex = inputPlayer;
                }
            }
            if (forcedPlayerIndex == U32_INVALID)
            {
                u32 selectedPlayerIndex = U32_INVALID;
                if (tryGetSelectedPlayerIndex(selectedPlayerIndex))
                {
                    forcedPlayerIndex = selectedPlayerIndex;
                }
            }
            if (forcedPlayerIndex != U32_INVALID)
            {
                CONTEXTICONSMANAGER->setForcedPlayerIndex(forcedPlayerIndex);
            }
            else
            {
                CONTEXTICONSMANAGER->clearForcedPlayerIndex();
            }
        }

        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            if (m_isRemappingModeByPlayer[playerIndex])
            {
                CONTEXTICONSMANAGER->hide();
                return;
            }
        }

#if defined(ITF_WINDOWS)
        if (m_isEditingControllerType)
        {
            CONTEXTICONSMANAGER->show(ContextIcon_EditConfirm, ContextIcon_EditBack);
            return;
        }
#endif

        EContextIcon topRightIcon = ContextIcon_Invalid;
        u32 selectedPlayerIndex = U32_INVALID;
        if (UI_MENUMANAGER)
        {
            const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer != U32_INVALID && inputPlayer < 4)
            {
                selectedPlayerIndex = inputPlayer;
            }
        }
        if (selectedPlayerIndex == U32_INVALID)
        {
            (void)tryGetSelectedPlayerIndex(selectedPlayerIndex);
        }
        if (selectedPlayerIndex != U32_INVALID && GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            ZInputManager* input = GAMEMANAGER->getInputManager();
            const EInputSourceType source = getActiveSourceForReset(selectedPlayerIndex);
            if (!input->IsRemappingDefault(selectedPlayerIndex, source))
            {
                topRightIcon = ContextIcon_ResetControlToDefault;
            }
        }
        CONTEXTICONSMANAGER->show(ContextIcon_Invalid, ContextIcon_Cancel, ContextIcon_Select, topRightIcon);
    }

    void Ray_ControlsRemappingMenuHelper::UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected)
    {
        if (isSelected && uiComponent)
        {
            if (UI_MENUMANAGER)
            {
                const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
                if (inputPlayer < 4 && m_exitDecisionByPlayer[inputPlayer] != ExitDecision_None)
                {
                    setExitDecisionForPlayer(inputPlayer, ExitDecision_None);
                }
            }

            u32 playerIndex = U32_INVALID;
            ZInputManager::EGameAction action = ZInputManager::Action_Up;
            if (tryGetIconInfoFromComponent(uiComponent, playerIndex, action) && playerIndex < 4)
            {
                m_hasLastActionByPlayer[playerIndex] = btrue;
                m_lastActionByPlayer[playerIndex] = action;

                m_selectedComponentRefByPlayer[playerIndex] = uiComponent->getUIref();
                m_hasSelectedComponentByPlayer[playerIndex] = btrue;
            }
            else if (UI_MENUMANAGER)
            {
                const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
                if (inputPlayer < 4)
                {
                    m_selectedComponentRefByPlayer[inputPlayer] = uiComponent->getUIref();
                    m_hasSelectedComponentByPlayer[inputPlayer] = btrue;
                }
            }
        }
        showContextIcons();
    }

    UIComponent* Ray_ControlsRemappingMenuHelper::getSelectedComponentForPlayer(u32 inputPlayer) const
    {
        if (!m_menu || inputPlayer >= 4)
            return nullptr;

        if (m_hasSelectedComponentByPlayer[inputPlayer] && m_selectedComponentRefByPlayer[inputPlayer].isValid())
        {
            if (UIComponent* comp = UIMenuManager::getUIComponent(m_selectedComponentRefByPlayer[inputPlayer]))
            {
                return comp;
            }
        }

        // Fallback
        return const_cast<Ray_ControlsRemappingMenuHelper*>(this)->findIconComponent(
            inputPlayer, ZInputManager::Action_Up);
    }

    bbool Ray_ControlsRemappingMenuHelper::isNavigationLockedForPlayer(u32 inputPlayer) const
    {
        if (inputPlayer >= 4)
            return bfalse;

        if (m_isRemappingModeByPlayer[inputPlayer] || m_isWaitingForReleaseByPlayer[inputPlayer])
            return btrue;

#if defined(ITF_WINDOWS)
        if (inputPlayer == 0 && m_isEditingControllerType)
            return btrue;
#endif

        return bfalse;
    }

    bbool Ray_ControlsRemappingMenuHelper::tryGetIconInfoFromComponent(
        UIComponent* component, u32& outPlayerIndex, ZInputManager::EGameAction& outAction) const
    {
        outPlayerIndex = U32_INVALID;
        outAction = ZInputManager::Action_Up;

        if (!component)
            return bfalse;

        Actor* actor = component->GetActor();
        if (!actor)
            return bfalse;

        const StringID actorId(actor->getUserFriendly().cStr());
        if (!actorId.isValid())
            return bfalse;

        return const_cast<Ray_ControlsRemappingMenuHelper*>(this)->parseIconId(actorId, outPlayerIndex, outAction);
    }

    UIComponent* Ray_ControlsRemappingMenuHelper::findIconComponent(u32 playerIndex,
                                                                    ZInputManager::EGameAction action) const
    {
        if (!m_menu || playerIndex >= 4)
            return nullptr;

        StringID iconId;
        for (u32 i = 0; i < s_iconMappingsCount; ++i)
        {
            if (s_iconMappings[i].playerIndex == playerIndex && s_iconMappings[i].action == action)
            {
                iconId = s_iconMappings[i].iconId;
                break;
            }
        }
        if (!iconId.isValid())
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

            const StringID actorId(actor->getUserFriendly().cStr());
            if (actorId == iconId)
            {
                return comp;
            }
        }

        return nullptr;
    }

    ObjectRef Ray_ControlsRemappingMenuHelper::getFocusOverrideTargetForInputPlayer(
        UIComponent* current, u32 inputPlayer) const
    {
        if (!m_isActive || !m_menu || !current)
            return ObjectRef::InvalidRef;

        if (inputPlayer >= 4)
            return ObjectRef::InvalidRef;
        if (isNavigationLockedForPlayer(inputPlayer))
            return ObjectRef::InvalidRef;

#if defined(ITF_WINDOWS)
        if (Actor* actor = current->GetActor())
        {
            const StringID actorId(actor->getUserFriendly().cStr());
            if (actorId == CONTROLLER_OPTIONS_ID)
            {
                if (inputPlayer == 0)
                    return ObjectRef::InvalidRef;
                ZInputManager::EGameAction focusAction = ZInputManager::Action_Up;
                if (m_hasLastActionByPlayer[inputPlayer])
                {
                    focusAction = m_lastActionByPlayer[inputPlayer];
                }
                if (UIComponent* target = findIconComponent(inputPlayer, focusAction))
                {
                    return target->getUIref();
                }
                return ObjectRef::InvalidRef;
            }
        }
#endif
        u32 selectedPlayer = U32_INVALID;
        ZInputManager::EGameAction selectedAction = ZInputManager::Action_Up;
        const bbool onIcon = tryGetIconInfoFromComponent(current, selectedPlayer, selectedAction);
        if (onIcon && selectedPlayer == inputPlayer)
            return ObjectRef::InvalidRef;
        ZInputManager::EGameAction focusAction = ZInputManager::Action_Up;
        if (onIcon)
        {
            focusAction = selectedAction;
        }
        else if (m_hasLastActionByPlayer[inputPlayer])
        {
            focusAction = m_lastActionByPlayer[inputPlayer];
        }

        if (UIComponent* target = findIconComponent(inputPlayer, focusAction))
        {
            return target->getUIref();
        }

        return ObjectRef::InvalidRef;
    }

    u32 Ray_ControlsRemappingMenuHelper::getActionIndex(ZInputManager::EGameAction action)
    {
        switch (action)
        {
        case ZInputManager::Action_Up: return 0;
        case ZInputManager::Action_Down: return 1;
        case ZInputManager::Action_Left: return 2;
        case ZInputManager::Action_Right: return 3;
        case ZInputManager::Action_Run: return 4;
        case ZInputManager::Action_Jump: return 5;
        case ZInputManager::Action_Hit: return 6;
        default: return U32_INVALID;
        }
    }

    ZInputManager::EGameAction Ray_ControlsRemappingMenuHelper::getActionByIndex(u32 index)
    {
        switch (index)
        {
        case 0: return ZInputManager::Action_Up;
        case 1: return ZInputManager::Action_Down;
        case 2: return ZInputManager::Action_Left;
        case 3: return ZInputManager::Action_Right;
        case 4: return ZInputManager::Action_Run;
        case 5: return ZInputManager::Action_Jump;
        case 6: return ZInputManager::Action_Hit;
        default: return ZInputManager::Action_Up;
        }
    }

    bbool Ray_ControlsRemappingMenuHelper::tryGetSelectedPlayerIndex(u32& outPlayerIndex)
    {
        outPlayerIndex = U32_INVALID;

        if (!m_menu)
            return bfalse;

        UIComponent* selected = m_menu->getUIComponentSelected();
        if (!selected)
            return bfalse;

        Actor* actor = selected->GetActor();
        if (!actor)
            return bfalse;

        const StringID actorId(actor->getUserFriendly().cStr());
        if (!actorId.isValid())
            return bfalse;

        ZInputManager::EGameAction unusedAction = ZInputManager::Action_Up;
        return parseIconId(actorId, outPlayerIndex, unusedAction);
    }

    StringID Ray_ControlsRemappingMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action,
                                                               const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER)
            return defaultAction;

        const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
        const bbool validInputPlayer = (inputPlayer != U32_INVALID && inputPlayer < 4);

        if (validInputPlayer && m_exitDecisionByPlayer[inputPlayer] != ExitDecision_None)
        {
            if (action != input_actionID_Back)
            {
                setExitDecisionForPlayer(inputPlayer, ExitDecision_None);
            }
        }

        bbool anyRemappingOrWaiting = bfalse;
        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            if (m_isRemappingModeByPlayer[playerIndex] || m_isWaitingForReleaseByPlayer[playerIndex])
            {
                anyRemappingOrWaiting = btrue;
                break;
            }
        }

        if (anyRemappingOrWaiting)
        {
            if (action == input_actionID_Back)
            {
                if (validInputPlayer && m_isRemappingModeByPlayer[inputPlayer])
                {
                    cancelRemappingMode(inputPlayer, btrue);
                }
                if (validInputPlayer)
                {
                    m_isWaitingForReleaseByPlayer[inputPlayer] = bfalse;
                    m_postRemapCooldownByPlayer[inputPlayer] = 0.0f;
                }
                showContextIcons();
                return UI_MENUMANAGER->getMenuPageAction_Nothing();
            }

            if (action == input_actionID_DeleteSave || action == input_actionID_Other)
            {
                return UI_MENUMANAGER->getMenuPageAction_Nothing();
            }
        }

#if defined(ITF_WINDOWS)
        if (m_isEditingControllerType && action == input_actionID_Back)
        {
            if (m_hasSnapshotPCControlModeEdit && RAY_GAMEMANAGER)
            {
                RAY_GAMEMANAGER->setPCControlMode(m_snapshotPCControlModeEdit);

                UIListOptionComponent* ctrlTypeComp = findControllerTypeComponent();
                if (ctrlTypeComp)
                {
                    updateControllerTypeDisplay(ctrlTypeComp, m_snapshotPCControlModeEdit);
                }
            }
            exitControllerTypeEditMode();
            showContextIcons();
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }
#endif

        if (action == input_actionID_DeleteSave)
        {
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        if (action == input_actionID_Other)
        {
            u32 selectedPlayerIndex = U32_INVALID;
            if (UI_MENUMANAGER)
            {
                const u32 currentInputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
                if (currentInputPlayer != U32_INVALID && currentInputPlayer < 4)
                {
                    selectedPlayerIndex = currentInputPlayer;
                }
            }
            if (selectedPlayerIndex == U32_INVALID)
            {
                (void)tryGetSelectedPlayerIndex(selectedPlayerIndex);
            }
            if (selectedPlayerIndex != U32_INVALID && GAMEMANAGER && GAMEMANAGER->getInputManager())
            {
                ZInputManager* input = GAMEMANAGER->getInputManager();
                const EInputSourceType source = getActiveSourceForReset(selectedPlayerIndex);
                if (!input->IsRemappingDefault(selectedPlayerIndex, source))
                {
                    input->ResetRemapping(selectedPlayerIndex, source);
                    showContextIcons();
                }
            }
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        if (action == input_actionID_Back)
        {
            if (validInputPlayer)
            {
                setExitDecisionForPlayer(inputPlayer, ExitDecision_Save);
                if (areAllParticipatingPlayersReadyToExit())
                {
                    applyExitDecisionsAndClose();
                }
            }
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        return Ray_BaseMenuHelper::onMenuPageAction(menu, action, defaultAction);
    }

    void Ray_ControlsRemappingMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component)
            return;

        if (UI_MENUMANAGER)
        {
            const u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer < 4)
            {
                if (m_exitDecisionByPlayer[inputPlayer] != ExitDecision_None)
                {
                    setExitDecisionForPlayer(inputPlayer, ExitDecision_None);
                }

                if (m_isRemappingModeByPlayer[inputPlayer] || m_isWaitingForReleaseByPlayer[inputPlayer])
                    return;
            }
        }

        StringID actorId;
        if (Actor* actor = component->GetActor())
        {
            actorId = StringID(actor->getUserFriendly().cStr());
        }

        if (!actorId.isValid())
            return;

#if defined(ITF_WINDOWS)
        if (handleControllerTypeAction(actorId, component))
            return;
#endif

        (void)handleIconAction(actorId, component);
    }

    bbool Ray_ControlsRemappingMenuHelper::parseIconId(const StringID& id, u32& outPlayerIndex,
                                                       ZInputManager::EGameAction& outAction)
    {
        for (u32 i = 0; i < s_iconMappingsCount; ++i)
        {
            if (id == s_iconMappings[i].iconId)
            {
                outPlayerIndex = s_iconMappings[i].playerIndex;
                outAction = s_iconMappings[i].action;
                return btrue;
            }
        }
        return bfalse;
    }

    bbool Ray_ControlsRemappingMenuHelper::handleIconAction(const StringID& id, UIComponent* component)
    {
        u32 playerIndex = 0;
        ZInputManager::EGameAction action = ZInputManager::Action_Up;

        if (!parseIconId(id, playerIndex, action))
            return bfalse;
        if (UI_MENUMANAGER)
        {
            u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer != U32_INVALID && inputPlayer != playerIndex)
            {
                LOG("[ControlsRemapping] Rejected: Controller %d cannot remap Player %d slot\n",
                    inputPlayer + 1, playerIndex + 1);
                return btrue;
            }
        }

        startRemappingMode(playerIndex, action, component);
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action,
                                                             UIComponent* component)
    {
        if (action == ZInputManager::Action_Back)
        {
            LOG("[ControlsRemapping] Action_Back remapping is disabled\n");
            return;
        }

        cancelRemappingMode(playerIndex, btrue);
        m_isRemappingModeByPlayer[playerIndex] = btrue;
        m_remappingActionByPlayer[playerIndex] = action;
        m_remappingComponentByPlayer[playerIndex] = component;
        m_remappingCooldownByPlayer[playerIndex] = ControlsRemappingConstants::REMAPPING_COOLDOWN;
        m_remappingSourceByPlayer[playerIndex] = InputSource_Count;
        clearIconDisplay(playerIndex, component);

        showContextIcons();

        LOG("[ControlsRemapping] Starting remap mode for Player %d, Action %d\n", playerIndex + 1, action);
    }

    void Ray_ControlsRemappingMenuHelper::clearIconDisplay(u32 playerIndex, UIComponent* component)
    {
        if (!component)
            return;

        if (playerIndex < 4)
        {
            m_previousIconLineIdByPlayer[playerIndex] = component->getLineId();
        }

        component->setTextModeYOverride(1u);
        component->setInitialFontHeightOverride(30.0f);
        component->setLineId(kControlsRemappingNewInputLineId);
        component->forceContent("");
    }

    void Ray_ControlsRemappingMenuHelper::restoreIconDisplay(u32 playerIndex, UIComponent* component)
    {
        if (!component)
            return;

        component->clearTextModeYOverride();
        component->clearInitialFontHeightOverride();

        if (playerIndex < 4)
        {
            component->setLineId(m_previousIconLineIdByPlayer[playerIndex]);
        }
        else
        {
            component->setLineId(LocalisationId::Invalid);
        }

        String content(component->getFriendly().cStr());
        component->forceContent(content);
    }

    void Ray_ControlsRemappingMenuHelper::cancelRemappingMode(u32 playerIndex, bbool restoreDisplay)
    {
        if (playerIndex >= 4)
            return;

        if (!m_isRemappingModeByPlayer[playerIndex])
            return;

        if (restoreDisplay)
        {
            restoreIconDisplay(playerIndex, m_remappingComponentByPlayer[playerIndex]);
        }

        m_isRemappingModeByPlayer[playerIndex] = bfalse;
        m_remappingComponentByPlayer[playerIndex] = nullptr;
        m_remappingCooldownByPlayer[playerIndex] = 0.0f;
        m_remappingSourceByPlayer[playerIndex] = InputSource_Gamepad;
        m_isWaitingForReleaseByPlayer[playerIndex] = btrue;
        m_postRemapCooldownByPlayer[playerIndex] = ControlsRemappingConstants::POST_REMAP_COOLDOWN;
        LOG("[ControlsRemapping] Entering wait-for-release state\n");
    }

    bbool Ray_ControlsRemappingMenuHelper::detectPhysicalControl(u32 playerIndex, u32& outPhysicalControl,
                                                                 EInputSourceType& outSource)
    {
        outPhysicalControl = U32_INVALID;
        outSource = InputSource_Gamepad;

        if (playerIndex >= 4)
            return bfalse;

        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return bfalse;
        ZInputManager* inputManager = GAMEMANAGER->getInputManager();

#if defined(ITF_WINDOWS)
        // First check for raw keyboard input (allows remapping to any key)
        i32 rawKey = inputManager->GetFirstKeyboardKey(playerIndex);
        if (rawKey >= 0)
        {
            outPhysicalControl = static_cast<u32>(rawKey);
            outSource = InputSource_Keyboard;
            return btrue;
        }
#endif

        // Then check gamepad/standard controls
        outPhysicalControl = inputManager->GetFirstActiveControl(playerIndex, &outSource);
        return (outPhysicalControl != U32_INVALID);
    }

    void Ray_ControlsRemappingMenuHelper::finalizeRemapping(u32 playerIndex, u32 physicalControl)
    {
        if (playerIndex >= 4)
            return;

        if (!GAMEMANAGER)
        {
            LOG("[ControlsRemapping] Cannot remap: GameManager unavailable\n");
            cancelRemappingMode(playerIndex, btrue);
            return;
        }

        ZInputManager* inputManager = GAMEMANAGER->getInputManager();
        if (!inputManager)
        {
            LOG("[ControlsRemapping] Cannot remap: InputManager unavailable\n");
            cancelRemappingMode(playerIndex, btrue);
            return;
        }

        const ZInputManager::EGameAction remapAction = m_remappingActionByPlayer[playerIndex];
        const EInputSourceType remapSource = m_remappingSourceByPlayer[playerIndex];

        // For keyboard remapping, we allow any key - validation only applies to gamepad controls
        bbool shouldValidate = (remapSource != InputSource_Keyboard);
        if (shouldValidate && !inputManager->IsActionRemapAllowed(remapAction, physicalControl))
        {
            LOG("[ControlsRemapping] Remap rejected Player %d, Action %d -> Physical %d\n", playerIndex + 1,
                remapAction, physicalControl);
            return;
        }

        EInputSourceType appliedSource = (remapSource < InputSource_Count) ? remapSource : InputSource_Gamepad;
        inputManager->SetActionRemap(playerIndex, remapAction, physicalControl, appliedSource);
        LOG("[ControlsRemapping] Remap complete Player %d, Action %d -> Physical %d (Source %d)\n",
            playerIndex + 1, remapAction, physicalControl, appliedSource);


        cancelRemappingMode(playerIndex, btrue);
        showContextIcons();
    }

    void Ray_ControlsRemappingMenuHelper::updateRemappingMode(f32 deltaTime)
    {
#if defined(ITF_WINDOWS)
        updateControllerTypeEditing(deltaTime);
#endif

        for (u32 playerIndex = 0; playerIndex < 4; ++playerIndex)
        {
            if (m_isWaitingForReleaseByPlayer[playerIndex])
            {
                m_postRemapCooldownByPlayer[playerIndex] -= deltaTime;
                if (m_postRemapCooldownByPlayer[playerIndex] <= 0.0f && GAMEMANAGER && GAMEMANAGER->getInputManager())
                {
                    u32 activeControl = GAMEMANAGER->getInputManager()->GetFirstActiveControl(playerIndex);
                    if (activeControl == U32_INVALID)
                    {
                        m_isWaitingForReleaseByPlayer[playerIndex] = bfalse;
                    }
                }
                continue;
            }

            if (!m_isRemappingModeByPlayer[playerIndex])
                continue;

            if (m_remappingCooldownByPlayer[playerIndex] > 0.0f)
            {
                m_remappingCooldownByPlayer[playerIndex] -= deltaTime;
                if (m_remappingCooldownByPlayer[playerIndex] > 0.0f)
                    continue;
            }

            u32 physicalControl = U32_INVALID;
            EInputSourceType source = InputSource_Gamepad;
            if (!detectPhysicalControl(playerIndex, physicalControl, source))
                continue;

            if (physicalControl != U32_INVALID)
            {
                m_remappingSourceByPlayer[playerIndex] = source;
                finalizeRemapping(playerIndex, physicalControl);
            }
        }
    }

    void Ray_ControlsRemappingMenuHelper::onClose()
    {
        if (CONTEXTICONSMANAGER)
        {
            CONTEXTICONSMANAGER->clearForcedPlayerIndex();
            CONTEXTICONSMANAGER->hide();
        }

        for (u32 i = 0; i < 4; ++i)
        {
            cancelRemappingMode(i, btrue);
            m_isWaitingForReleaseByPlayer[i] = bfalse;
        }
#if defined(ITF_WINDOWS)
        exitControllerTypeEditMode();
        m_previousSelectionStates.clear();
#endif

        resetExitDecisionsAndStatusUI();


        if (s_activeHelper == this)
        {
            s_activeHelper = nullptr;
        }
    }

    EInputSourceType Ray_ControlsRemappingMenuHelper::getActiveSourceForReset(u32 playerIndex) const
    {
#if defined(ITF_WINDOWS)
        if (INPUT_ADAPTER)
        {
            InputDeviceType lastDevice = INPUT_ADAPTER->getLastUsedInputDevice(playerIndex);
            if (lastDevice == InputDevice_Keyboard && INPUT_ADAPTER->IsKeyboardMouseEnabled())
            {
                return InputSource_Keyboard;
            }
        }
#endif
        return InputSource_Gamepad;
    }

    ObjectRef Ray_ControlsRemappingMenuHelper::getNavigationOverrideTarget(UIComponent* current, f32 joyX, f32 joyY)
    {
        if (!m_isActive || !m_menu || !current)
            return ObjectRef::InvalidRef;

        const ObjectRef currentRef = current->getUIref();

        const f32 absJoyX = f32_Abs(joyX);
        const f32 absJoyY = f32_Abs(joyY);
        if (absJoyX < MTH_EPSILON && absJoyY < MTH_EPSILON)
            return ObjectRef::InvalidRef;

#if defined(ITF_WINDOWS)
        if (m_isEditingControllerType)
        {
            const bbool preferHorizontal = absJoyX >= absJoyY;
            if (!preferHorizontal)
            {
                exitControllerTypeEditMode();
            }
            return ObjectRef::InvalidRef;
        }
#endif
        const bbool preferHorizontal = absJoyX >= absJoyY;
        if (preferHorizontal)
        {
            return currentRef;
        }
        u32 inputPlayer = U32_INVALID;
        if (UI_MENUMANAGER)
        {
            inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
        }
        if (inputPlayer == U32_INVALID || inputPlayer >= 4)
        {
            inputPlayer = 0;
        }

        const i32 dir = (joyY > 0.0f) ? 1 : -1;
        static const i32 actionCount = 7;

#if defined(ITF_WINDOWS)
        if (Actor* actor = current->GetActor())
        {
            const StringID actorId(actor->getUserFriendly().cStr());
            if (actorId == CONTROLLER_OPTIONS_ID)
            {
                if (dir < 0)
                {
                    if (UIComponent* target = findIconComponent(inputPlayer, ZInputManager::Action_Hit))
                    {
                        return target->getUIref();
                    }
                }
                else
                {
                    if (UIComponent* target = findIconComponent(inputPlayer, ZInputManager::Action_Up))
                    {
                        return target->getUIref();
                    }
                }
                return currentRef;
            }
        }
#endif

        u32 selectedPlayer = U32_INVALID;
        ZInputManager::EGameAction selectedAction = ZInputManager::Action_Up;
        const bbool isOnIcon = tryGetIconInfoFromComponent(current, selectedPlayer, selectedAction);

        if (!isOnIcon)
        {
            if (UIComponent* target = findIconComponent(inputPlayer, ZInputManager::Action_Up))
            {
                return target->getUIref();
            }
            return currentRef;
        }

        if (selectedPlayer != inputPlayer)
        {
            if (UIComponent* target = findIconComponent(inputPlayer, selectedAction))
            {
                return target->getUIref();
            }
            return currentRef;
        }
        const u32 currentIdx = getActionIndex(selectedAction);
        if (currentIdx == U32_INVALID)
        {
            return currentRef;
        }

        i32 nextIdx = static_cast<i32>(currentIdx) + dir;
        if (nextIdx < 0)
        {
#if defined(ITF_WINDOWS)
            if (inputPlayer == 0)
            {
                UIListOptionComponent* ctrlTypeComp = findControllerTypeComponent();
                if (ctrlTypeComp && ctrlTypeComp->getActive() && ctrlTypeComp->getCanBeSelected())
                {
                    Actor* actor = ctrlTypeComp->GetActor();
                    if (actor && actor->isEnabled())
                    {
                        return ctrlTypeComp->getUIref();
                    }
                }
            }
#endif
            nextIdx = actionCount - 1;
        }
        else if (nextIdx >= actionCount)
        {
#if defined(ITF_WINDOWS)
            if (inputPlayer == 0)
            {
                UIListOptionComponent* ctrlTypeComp = findControllerTypeComponent();
                if (ctrlTypeComp && ctrlTypeComp->getActive() && ctrlTypeComp->getCanBeSelected())
                {
                    Actor* actor = ctrlTypeComp->GetActor();
                    if (actor && actor->isEnabled())
                    {
                        return ctrlTypeComp->getUIref();
                    }
                }
            }
#endif
            nextIdx = 0;
        }

        const ZInputManager::EGameAction nextAction = getActionByIndex(static_cast<u32>(nextIdx));
        if (UIComponent* target = findIconComponent(inputPlayer, nextAction))
        {
            return target->getUIref();
        }

        return currentRef;
    }

#if defined(ITF_WINDOWS)
    bbool Ray_ControlsRemappingMenuHelper::handleControllerTypeAction(const StringID& id, UIComponent* component)
    {
        if (id != CONTROLLER_OPTIONS_ID)
            return bfalse;

        if (UI_MENUMANAGER)
        {
            u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer != U32_INVALID && inputPlayer != 0)
            {
                LOG("[ControlsRemapping] Rejected: Only Player 1 can change controller type\n");
                return btrue;
            }
        }

        UIListOptionComponent* listComponent = component->DynamicCast<UIListOptionComponent>(
            ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
        if (!listComponent)
            return bfalse;
        if (m_isEditingControllerType && m_editingControllerTypeComponent == listComponent)
        {
            exitControllerTypeEditMode();
            return btrue;
        }

        if (m_isEditingControllerType)
        {
            exitControllerTypeEditMode();
        }

        enterControllerTypeEditMode(listComponent);
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::enterControllerTypeEditMode(UIListOptionComponent* component)
    {
        if (!component || m_isEditingControllerType)
            return;

        m_hasSnapshotPCControlModeEdit = bfalse;
        if (RAY_GAMEMANAGER)
        {
            m_snapshotPCControlModeEdit = RAY_GAMEMANAGER->getPCControlMode();
            m_hasSnapshotPCControlModeEdit = btrue;
        }

        m_isEditingControllerType = btrue;
        m_editingControllerTypeComponent = component;
        m_controllerTypeChangeCooldown = 0.0f;
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

                if (comp == static_cast<UIComponent*>(component))
                {
                    if (!originalSelectable)
                    {
                        m_previousSelectionStates.push_back(std::make_pair(comp, originalSelectable));
                        comp->setCanBeSelected(btrue);
                    }
                    continue;
                }

                if (originalSelectable)
                {
                    m_previousSelectionStates.push_back(std::make_pair(comp, originalSelectable));
                    comp->setCanBeSelected(bfalse);
                }
            }
        }

        component->setEditingMode(btrue);
        showContextIcons();
        LOG("[ControlsRemapping] Entered controller type edit mode\n");
    }

    void Ray_ControlsRemappingMenuHelper::exitControllerTypeEditMode()
    {
        if (!m_isEditingControllerType)
            return;

        if (m_editingControllerTypeComponent)
        {
            m_editingControllerTypeComponent->setEditingMode(bfalse);
        }

        for (std::vector<std::pair<UIComponent*, bbool>>::iterator it = m_previousSelectionStates.begin();
             it != m_previousSelectionStates.end(); ++it)
        {
            if (it->first)
            {
                it->first->setCanBeSelected(it->second && it->first->getActive());
            }
        }
        m_previousSelectionStates.clear();

        m_isEditingControllerType = bfalse;
        m_editingControllerTypeComponent = nullptr;
        m_controllerTypeChangeCooldown = 0.0f;
        m_hasSnapshotPCControlModeEdit = bfalse;
        showContextIcons();
        LOG("[ControlsRemapping] Exited controller type edit mode\n");
    }

    void Ray_ControlsRemappingMenuHelper::adjustControllerType(UIListOptionComponent* listComponent, i32 direction)
    {
        if (!listComponent || direction == 0 || !RAY_GAMEMANAGER)
            return;

        const i32 currentIndex = RAY_GAMEMANAGER->getPCControlMode();
        const bbool isControllerModeAvailable = RAY_GAMEMANAGER->isPCControllerModeAvailable();

        i32 candidateIndex = currentIndex;
        for (u32 attempt = 0; attempt < PC_CONTROL_MODE_CHOICES; ++attempt)
        {
            candidateIndex += direction;
            if (candidateIndex < 0)
                candidateIndex = PC_CONTROL_MODE_CHOICES - 1;
            else if (candidateIndex >= static_cast<i32>(PC_CONTROL_MODE_CHOICES))
                candidateIndex = 0;

            if (candidateIndex == PCControlMode_Controller && !isControllerModeAvailable)
            {
                LOG("[ControlsRemapping] Skipping Controller mode: no unassigned controllers available\n");
                continue;
            }
            break;
        }

        if (candidateIndex == currentIndex)
            return;

        RAY_GAMEMANAGER->setPCControlMode(candidateIndex);
        updateControllerTypeDisplay(listComponent, candidateIndex);
        m_controllerTypeChangeCooldown = ControlsRemappingConstants::REMAPPING_COOLDOWN;
        LOG("[ControlsRemapping] Controller type changed to: %s\n",
            RAY_GAMEMANAGER->getPCControlModeDisplayName(candidateIndex));
    }

    void Ray_ControlsRemappingMenuHelper::updateControllerTypeDisplay(UIListOptionComponent* listComponent, i32 index)
    {
        if (!listComponent || !RAY_GAMEMANAGER)
            return;

        Actor* valueActor = listComponent->getValueActor();
        if (!valueActor)
            return;

        UIComponent* valueComp = valueActor->GetComponent<UIComponent>();
        if (!valueComp)
            return;

        const char* displayName = RAY_GAMEMANAGER->getPCControlModeDisplayName(index);
        if (displayName && displayName[0] != '\0')
        {
            valueComp->forceContent(displayName);
        }
    }

    UIListOptionComponent* Ray_ControlsRemappingMenuHelper::findControllerTypeComponent() const
    {
        if (!m_menu)
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

            const String8& friendly = actor->getUserFriendly();
            if (friendly == "controller_options")
            {
                return comp->DynamicCast<
                    UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            }
        }
        return nullptr;
    }

    void Ray_ControlsRemappingMenuHelper::updateControllerTypeEditing(f32 deltaTime)
    {
        if (!m_isEditingControllerType || !m_editingControllerTypeComponent)
            return;

        if (m_controllerTypeChangeCooldown > 0.0f)
        {
            m_controllerTypeChangeCooldown -= deltaTime;
            m_controllerTypeChangeCooldown = std::max(m_controllerTypeChangeCooldown, 0.0f);
        }
    }

    bbool Ray_ControlsRemappingMenuHelper::processEditingInput(UIComponent* component, const StringID& action)
    {
        if (!m_isEditingControllerType || !component)
            return bfalse;

        UIListOptionComponent* listComponent = component->DynamicCast<UIListOptionComponent>(
            ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
        if (!listComponent || listComponent != m_editingControllerTypeComponent)
            return bfalse;

        if (UI_MENUMANAGER)
        {
            u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer != U32_INVALID && inputPlayer != 0)
            {
                return btrue;
            }
        }

        if (action == input_actionID_Back)
        {
            if (m_hasSnapshotPCControlModeEdit && RAY_GAMEMANAGER)
            {
                RAY_GAMEMANAGER->setPCControlMode(m_snapshotPCControlModeEdit);
                updateControllerTypeDisplay(listComponent, m_snapshotPCControlModeEdit);
            }
            exitControllerTypeEditMode();
            return btrue;
        }

        if (m_controllerTypeChangeCooldown > 0.0f)
        {
            return btrue;
        }

        if (action == input_actionID_Left || action == input_actionID_LeftHold)
        {
            adjustControllerType(listComponent, -1);
            return btrue;
        }
        if (action == input_actionID_Right || action == input_actionID_RightHold)
        {
            adjustControllerType(listComponent, 1);
            return btrue;
        }
        return bfalse;
    }
#endif
}
