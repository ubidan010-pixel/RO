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
    namespace ControlsRemappingConstants
    {
        static const f32 REMAPPING_COOLDOWN = 0.25f;
        static const f32 POST_REMAP_COOLDOWN = 0.15f;
    }

    Ray_ControlsRemappingMenuHelper* Ray_ControlsRemappingMenuHelper::s_activeHelper = nullptr;

#define CONTROLSREMAPPING_ACCEPT_BUTTON  ITF_GET_STRINGID_CRC(accept_button,25226343)
#define CONTROLSREMAPPING_CANCEL_BUTTON  ITF_GET_STRINGID_CRC(cancel_button,4260770984)

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

#define RESET_PLAYER1_BUTTON ITF_GET_STRINGID_CRC(player1_reset_to_default,3165421879)
#define RESET_PLAYER2_BUTTON ITF_GET_STRINGID_CRC(player2_reset_to_default,2129089661)
#define RESET_PLAYER3_BUTTON ITF_GET_STRINGID_CRC(player3_reset_to_default,829246893)
#define RESET_PLAYER4_BUTTON ITF_GET_STRINGID_CRC(player4_reset_to_default,1984975433)

    struct IconMapping
    {
        StringID iconId;
        u32 playerIndex;
        ZInputManager::EGameAction action;
    };

    static const IconMapping s_iconMappings[] =
    {
        { ICON_PLAYER1_UP,    0, ZInputManager::Action_Up    },
        { ICON_PLAYER1_DOWN,  0, ZInputManager::Action_Down  },
        { ICON_PLAYER1_LEFT,  0, ZInputManager::Action_Left  },
        { ICON_PLAYER1_RIGHT, 0, ZInputManager::Action_Right },
        { ICON_PLAYER1_RUN,   0, ZInputManager::Action_Run   },
        { ICON_PLAYER1_JUMP,  0, ZInputManager::Action_Jump  },
        { ICON_PLAYER1_HIT,   0, ZInputManager::Action_Hit   },
        { ICON_PLAYER2_UP,    1, ZInputManager::Action_Up    },
        { ICON_PLAYER2_DOWN,  1, ZInputManager::Action_Down  },
        { ICON_PLAYER2_LEFT,  1, ZInputManager::Action_Left  },
        { ICON_PLAYER2_RIGHT, 1, ZInputManager::Action_Right },
        { ICON_PLAYER2_RUN,   1, ZInputManager::Action_Run   },
        { ICON_PLAYER2_JUMP,  1, ZInputManager::Action_Jump  },
        { ICON_PLAYER2_HIT,   1, ZInputManager::Action_Hit   },
        { ICON_PLAYER3_UP,    2, ZInputManager::Action_Up    },
        { ICON_PLAYER3_DOWN,  2, ZInputManager::Action_Down  },
        { ICON_PLAYER3_LEFT,  2, ZInputManager::Action_Left  },
        { ICON_PLAYER3_RIGHT, 2, ZInputManager::Action_Right },
        { ICON_PLAYER3_RUN,   2, ZInputManager::Action_Run   },
        { ICON_PLAYER3_JUMP,  2, ZInputManager::Action_Jump  },
        { ICON_PLAYER3_HIT,   2, ZInputManager::Action_Hit   },
        { ICON_PLAYER4_UP,    3, ZInputManager::Action_Up    },
        { ICON_PLAYER4_DOWN,  3, ZInputManager::Action_Down  },
        { ICON_PLAYER4_LEFT,  3, ZInputManager::Action_Left  },
        { ICON_PLAYER4_RIGHT, 3, ZInputManager::Action_Right },
        { ICON_PLAYER4_RUN,   3, ZInputManager::Action_Run   },
        { ICON_PLAYER4_JUMP,  3, ZInputManager::Action_Jump  },
        { ICON_PLAYER4_HIT,   3, ZInputManager::Action_Hit   },
    };

    static const u32 s_iconMappingsCount = sizeof(s_iconMappings) / sizeof(s_iconMappings[0]);

    Ray_ControlsRemappingMenuHelper::Ray_ControlsRemappingMenuHelper()
        : Ray_BaseMenuHelper()
          , m_isRemappingMode(bfalse)
          , m_isWaitingForRelease(bfalse)
          , m_remappingPlayerIndex(0)
          , m_remappingAction(ZInputManager::Action_Up)
          , m_remappingComponent(nullptr)
          , m_remappingCooldown(0.0f)
          , m_postRemapCooldown(0.0f)
          , m_remappingSource(InputSource_Gamepad)
          , m_hasCommittedChanges(bfalse)
          , m_hasRestoredOnCancel(bfalse)
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
            m_hasSnapshotGamepad[i] = bfalse;
            m_hasSnapshotKeyboard[i] = bfalse;
        }
    }

    Ray_ControlsRemappingMenuHelper::~Ray_ControlsRemappingMenuHelper()
    {
        if (s_activeHelper == this)
        {
            s_activeHelper = nullptr;
        }
    }

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
        m_isRemappingMode = bfalse;
        m_isWaitingForRelease = bfalse;
        m_remappingComponent = nullptr;
        m_remappingCooldown = 0.0f;
        m_postRemapCooldown = 0.0f;
        m_remappingSource = InputSource_Gamepad;
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
        m_hasRestoredOnCancel = bfalse;
        captureRemappingSnapshot();

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
        cancelRemappingMode(btrue);
        m_isWaitingForRelease = bfalse;
        m_postRemapCooldown = 0.0f;

        ZInputManager* input = GAMEMANAGER->getInputManager();
        input->SetSuppressReceive(bfalse);

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
        if (m_isRemappingMode)
        {
            CONTEXTICONSMANAGER->hide();
            return;
        }

#if defined(ITF_WINDOWS)
        if (m_isEditingControllerType)
        {
            CONTEXTICONSMANAGER->show(ContextIcon_EditConfirm, ContextIcon_EditBack);
            return;
        }
#endif
        CONTEXTICONSMANAGER->show(ContextIcon_Confirm, ContextIcon_Cancel, ContextIcon_EditConfirm);
    }

    StringID Ray_ControlsRemappingMenuHelper::onMenuPageAction(UIMenu* menu, const StringID& action, const StringID& defaultAction)
    {
        if (!UI_MENUMANAGER)
            return defaultAction;
        if (m_isRemappingMode || m_isWaitingForRelease)
        {
            if (action == input_actionID_Back)
            {
                cancelRemappingMode(btrue);
                m_isWaitingForRelease = bfalse;
                m_postRemapCooldown = 0.0f;
                if (GAMEMANAGER && GAMEMANAGER->getInputManager())
                {
                    GAMEMANAGER->getInputManager()->SetSuppressReceive(bfalse);
                }
                showContextIcons();
                return UI_MENUMANAGER->getMenuPageAction_Nothing();
            }

            if (action == input_actionID_DeleteSave)
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

    void Ray_ControlsRemappingMenuHelper::applyAndClose()
    {
        m_hasCommittedChanges = btrue;
        if (RAY_GAMEMANAGER)
        {
            RAY_GAMEMANAGER->saveGameOptions();
        }
        closeAndReturn();
    }

    void Ray_ControlsRemappingMenuHelper::cancelAndClose()
    {
        restoreRemappingSnapshot();
        m_hasRestoredOnCancel = btrue;
        closeAndReturn();
    }

    void Ray_ControlsRemappingMenuHelper::onMenuItemAction(UIComponent* component)
    {
        if (!component) return;

        StringID actorId;
        if (Actor* actor = component->GetActor())
        {
            actorId = StringID(actor->getUserFriendly().cStr());
        }
        const StringID componentId = component->getID();
        if (actorId.isValid() && handleResetToDefault(actorId))
            return;
#if defined(ITF_WINDOWS)
        if (actorId.isValid() && handleControllerTypeAction(actorId, component))
            return;
#endif
        if (actorId.isValid() && handleIconAction(actorId, component))
            return;
    }

    bbool Ray_ControlsRemappingMenuHelper::parseResetButtonId(const StringID& id, u32& outPlayerIndex)
    {
        if (id == RESET_PLAYER1_BUTTON)
        {
            outPlayerIndex = 0;
            return btrue;
        }
        if (id == RESET_PLAYER2_BUTTON)
        {
            outPlayerIndex = 1;
            return btrue;
        }
        if (id == RESET_PLAYER3_BUTTON)
        {
            outPlayerIndex = 2;
            return btrue;
        }
        if (id == RESET_PLAYER4_BUTTON)
        {
            outPlayerIndex = 3;
            return btrue;
        }
        return bfalse;
    }

    bbool Ray_ControlsRemappingMenuHelper::handleResetToDefault(const StringID& id)
    {
        u32 playerIndex = 0;
        if (!parseResetButtonId(id, playerIndex))
            return bfalse;
        if (UI_MENUMANAGER)
        {
            u32 inputPlayer = UI_MENUMANAGER->getCurrentInputPlayer();
            if (inputPlayer != U32_INVALID && inputPlayer != playerIndex)
            {
                LOG("[ControlsRemapping] Rejected: Controller %d cannot reset Player %d controls\n",
                    inputPlayer + 1, playerIndex + 1);
                return btrue;
            }
        }

        EInputSourceType activeSource = getActiveSourceForReset(playerIndex);
        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->ResetRemapping(playerIndex, activeSource);
        }

        return btrue;
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

    void Ray_ControlsRemappingMenuHelper::startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action, UIComponent* component)
    {
        if (action == ZInputManager::Action_Back)
        {
            LOG("[ControlsRemapping] Action_Back remapping is disabled\n");
            return;
        }

        cancelRemappingMode(btrue);
        m_isRemappingMode = btrue;
        m_remappingPlayerIndex = playerIndex;
        m_remappingAction = action;
        m_remappingComponent = component;
        m_remappingCooldown = ControlsRemappingConstants::REMAPPING_COOLDOWN;
        m_remappingSource = InputSource_Count;
        clearIconDisplay(component);

        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->SetSuppressReceive(btrue);
        }

        showContextIcons();

        LOG("[ControlsRemapping] Starting remap mode for Player %d, Action %d\n", playerIndex + 1, action);
    }

    void Ray_ControlsRemappingMenuHelper::clearIconDisplay(UIComponent* component)
    {
        if (!component)
            return;
        component->forceContent("");
    }

    void Ray_ControlsRemappingMenuHelper::restoreIconDisplay(UIComponent* component)
    {
        if (!component)
            return;
        String content(component->getFriendly().cStr());
        component->forceContent(content);
    }

    void Ray_ControlsRemappingMenuHelper::cancelRemappingMode(bbool restoreDisplay)
    {
        if (!m_isRemappingMode)
            return;

        if (restoreDisplay)
        {
            restoreIconDisplay(m_remappingComponent);
        }

        m_isRemappingMode = bfalse;
        m_remappingComponent = nullptr;
        m_remappingCooldown = 0.0f;
        m_remappingSource = InputSource_Gamepad;
        m_isWaitingForRelease = btrue;
        m_postRemapCooldown = ControlsRemappingConstants::POST_REMAP_COOLDOWN;
        LOG("[ControlsRemapping] Entering wait-for-release state\n");
    }

    bbool Ray_ControlsRemappingMenuHelper::detectPhysicalControl(u32& outPhysicalControl, EInputSourceType& outSource)
    {
        outPhysicalControl = U32_INVALID;
        outSource = InputSource_Gamepad;

        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return bfalse;
        ZInputManager* inputManager = GAMEMANAGER->getInputManager();

#if defined(ITF_WINDOWS)
        // First check for raw keyboard input (allows remapping to any key)
        i32 rawKey = inputManager->GetFirstKeyboardKey(m_remappingPlayerIndex);
        if (rawKey >= 0)
        {
            outPhysicalControl = static_cast<u32>(rawKey);
            outSource = InputSource_Keyboard;
            return btrue;
        }
#endif

        // Then check gamepad/standard controls
        outPhysicalControl = inputManager->GetFirstActiveControl(m_remappingPlayerIndex, &outSource);
        return (outPhysicalControl != U32_INVALID);
    }

    void Ray_ControlsRemappingMenuHelper::finalizeRemapping(u32 physicalControl)
    {
        if (!GAMEMANAGER)
        {
            LOG("[ControlsRemapping] Cannot remap: GameManager unavailable\n");
            cancelRemappingMode(btrue);
            return;
        }

        ZInputManager* inputManager = GAMEMANAGER->getInputManager();
        if (!inputManager)
        {
            LOG("[ControlsRemapping] Cannot remap: InputManager unavailable\n");
            cancelRemappingMode(btrue);
            return;
        }

        // For keyboard remapping, we allow any key - validation only applies to gamepad controls
        bbool shouldValidate = (m_remappingSource != InputSource_Keyboard);
        if (shouldValidate && !inputManager->IsActionRemapAllowed(m_remappingAction, physicalControl))
        {
            LOG("[ControlsRemapping] Remap rejected Player %d, Action %d -> Physical %d\n", m_remappingPlayerIndex + 1, m_remappingAction, physicalControl);
            return;
        }

        EInputSourceType appliedSource = (m_remappingSource < InputSource_Count) ? m_remappingSource : InputSource_Gamepad;
        inputManager->SetActionRemap(m_remappingPlayerIndex, m_remappingAction, physicalControl, appliedSource);
        LOG("[ControlsRemapping] Remap complete Player %d, Action %d -> Physical %d (Source %d)\n",
            m_remappingPlayerIndex + 1, m_remappingAction, physicalControl, appliedSource);

        cancelRemappingMode(btrue);
		showContextIcons();
    }

    void Ray_ControlsRemappingMenuHelper::updateRemappingMode(f32 deltaTime)
    {
#if defined(ITF_WINDOWS)
        updateControllerTypeEditing(deltaTime);
#endif

        if (m_isWaitingForRelease)
        {
            m_postRemapCooldown -= deltaTime;
            bbool allReleased = bfalse;

            if (m_postRemapCooldown <= 0.0f && GAMEMANAGER && GAMEMANAGER->getInputManager())
            {
                u32 activeControl = GAMEMANAGER->getInputManager()->GetFirstActiveControl(m_remappingPlayerIndex);
                if (activeControl == U32_INVALID)
                {
                    allReleased = btrue;
                }
            }

            if (allReleased)
            {
                m_isWaitingForRelease = bfalse;
                if (GAMEMANAGER && GAMEMANAGER->getInputManager())
                {
                    GAMEMANAGER->getInputManager()->SetSuppressReceive(bfalse);
                }
                LOG("[ControlsRemapping] Input released, Receive re-enabled\n");
            }
            return;
        }

        if (!m_isRemappingMode)
            return;

        if (m_remappingCooldown > 0.0f)
        {
            m_remappingCooldown -= deltaTime;
            if (m_remappingCooldown > 0.0f)
                return;
        }

        u32 physicalControl = U32_INVALID;
        EInputSourceType source = InputSource_Gamepad;
        if (!detectPhysicalControl(physicalControl, source))
            return;

        if (physicalControl != U32_INVALID)
        {
            m_remappingSource = source;
            finalizeRemapping(physicalControl);
        }
    }

    void Ray_ControlsRemappingMenuHelper::onClose()
    {
        cancelRemappingMode(btrue);
        m_isWaitingForRelease = bfalse;
#if defined(ITF_WINDOWS)
        exitControllerTypeEditMode();
        m_previousSelectionStates.clear();
#endif

        if (!m_hasCommittedChanges && !m_hasRestoredOnCancel)
        {
            restoreRemappingSnapshot();
        }

        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->SetSuppressReceive(bfalse);
        }

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

#if defined(ITF_WINDOWS)
        if (!m_isEditingControllerType)
            return ObjectRef::InvalidRef;

        const f32 absJoyX = f32_Abs(joyX);
        const f32 absJoyY = f32_Abs(joyY);
        if (absJoyX < MTH_EPSILON && absJoyY < MTH_EPSILON)
            return ObjectRef::InvalidRef;

        const bbool preferHorizontal = absJoyX >= absJoyY;
        if (preferHorizontal)
        {
            return ObjectRef::InvalidRef;
        }

        exitControllerTypeEditMode();
        return ObjectRef::InvalidRef;
#else
        return ObjectRef::InvalidRef;
#endif
    }

#if defined(ITF_WINDOWS)
#define CONTROLLER_OPTIONS_ID ITF_GET_STRINGID_CRC(controller_options,3547548201)

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

        UIListOptionComponent* listComponent = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
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
                return comp->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
            }
        }
        return nullptr;
    }

    bbool Ray_ControlsRemappingMenuHelper::handleExternalEditingInput(UIComponent* component, const StringID& action)
    {
#if defined(ITF_WINDOWS)
        if (!s_activeHelper)
            return bfalse;

        return s_activeHelper->processEditingInput(component, action);
#else
        (void)component; (void)action;
        return bfalse;
#endif
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

        UIListOptionComponent* listComponent = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
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
