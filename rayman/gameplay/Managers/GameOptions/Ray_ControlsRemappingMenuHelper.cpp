#include "precompiled_gameplay_rayman.h"

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

namespace ITF
{
    Ray_ControlsRemappingMenuHelper* Ray_ControlsRemappingMenuHelper::s_activeHelper = nullptr;

    // Define button IDs for controls remapping menu
#define CONTROLSREMAPPING_ACCEPT_BUTTON  ITF_GET_STRINGID_CRC(accept_button,25226343)
#define CONTROLSREMAPPING_CANCEL_BUTTON  ITF_GET_STRINGID_CRC(cancel_button,4260770984)

    // Player 1 Icons
#define ICON_PLAYER1_UP     ITF_GET_STRINGID_CRC(player1_up_icon,1753979515)
#define ICON_PLAYER1_DOWN   ITF_GET_STRINGID_CRC(player1_down_icon,845618532)
#define ICON_PLAYER1_LEFT   ITF_GET_STRINGID_CRC(player1_left_icon,3896653396)
#define ICON_PLAYER1_RIGHT  ITF_GET_STRINGID_CRC(player1_right_icon,3424052495)
#define ICON_PLAYER1_RUN    ITF_GET_STRINGID_CRC(player1_run_icon,3457228901)
#define ICON_PLAYER1_JUMP   ITF_GET_STRINGID_CRC(player1_jump_icon,3525025255)
#define ICON_PLAYER1_HIT    ITF_GET_STRINGID_CRC(player1_hit_icon,4282892399)
#define ICON_PLAYER1_BACK   ITF_GET_STRINGID_CRC(player1_back_icon,1021152668)

    // Player 2 Icons
#define ICON_PLAYER2_UP     ITF_GET_STRINGID_CRC(player2_up_icon,4288940261)
#define ICON_PLAYER2_DOWN   ITF_GET_STRINGID_CRC(player2_down_icon,441889963)
#define ICON_PLAYER2_LEFT   ITF_GET_STRINGID_CRC(player2_left_icon,2679901733)
#define ICON_PLAYER2_RIGHT  ITF_GET_STRINGID_CRC(player2_right_icon,456853284)
#define ICON_PLAYER2_RUN    ITF_GET_STRINGID_CRC(player2_run_icon,2628100497)
#define ICON_PLAYER2_JUMP   ITF_GET_STRINGID_CRC(player2_jump_icon,2380529337)
#define ICON_PLAYER2_HIT    ITF_GET_STRINGID_CRC(player2_hit_icon,425931337)
#define ICON_PLAYER2_BACK   ITF_GET_STRINGID_CRC(player2_back_icon,3528166033)

    // Player 3 Icons
#define ICON_PLAYER3_UP     ITF_GET_STRINGID_CRC(player3_up_icon,1876599777)
#define ICON_PLAYER3_DOWN   ITF_GET_STRINGID_CRC(player3_down_icon,548047626)
#define ICON_PLAYER3_LEFT   ITF_GET_STRINGID_CRC(player3_left_icon,1679854463)
#define ICON_PLAYER3_RIGHT  ITF_GET_STRINGID_CRC(player3_right_icon,206088801)
#define ICON_PLAYER3_RUN    ITF_GET_STRINGID_CRC(player3_run_icon,1971818420)
#define ICON_PLAYER3_JUMP   ITF_GET_STRINGID_CRC(player3_jump_icon,3551164230)
#define ICON_PLAYER3_HIT    ITF_GET_STRINGID_CRC(player3_hit_icon,1301281176)
#define ICON_PLAYER3_BACK   ITF_GET_STRINGID_CRC(player3_back_icon,3624079622)

    // Player 4 Icons
#define ICON_PLAYER4_UP     ITF_GET_STRINGID_CRC(player4_up_icon,4257930662)
#define ICON_PLAYER4_DOWN   ITF_GET_STRINGID_CRC(player4_down_icon,1841519437)
#define ICON_PLAYER4_LEFT   ITF_GET_STRINGID_CRC(player4_left_icon,2981630500)
#define ICON_PLAYER4_RIGHT  ITF_GET_STRINGID_CRC(player4_right_icon,2134331520)
#define ICON_PLAYER4_RUN    ITF_GET_STRINGID_CRC(player4_run_icon,3273055250)
#define ICON_PLAYER4_JUMP   ITF_GET_STRINGID_CRC(player4_jump_icon,3258270794)
#define ICON_PLAYER4_HIT    ITF_GET_STRINGID_CRC(player4_hit_icon,3161019062)
#define ICON_PLAYER4_BACK   ITF_GET_STRINGID_CRC(player4_back_icon,1461371878)

    Ray_ControlsRemappingMenuHelper::Ray_ControlsRemappingMenuHelper()
        : Ray_BaseMenuHelper()
          , m_isRemappingMode(bfalse)
          , m_isWaitingForRelease(bfalse)
          , m_remappingPlayerIndex(0)
          , m_remappingAction(ZInputManager::Action_Up)
          , m_remappingComponent(NULL)
          , m_remappingCooldown(0.0f)
          , m_postRemapCooldown(0.0f)
#if defined(ITF_WINDOWS)
          , m_isEditingControllerType(bfalse)
          , m_editingControllerTypeComponent(NULL)
          , m_controllerTypeInputTimer(0.0f)
          , m_controllerTypeFirstPressTimer(0.0f)
          , m_controllerTypeFirstPressed(btrue)
#endif
    {
        m_menuBaseName = "controlremapping";
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

        hideContextIcons();

        m_isActive = btrue;
        m_isRemappingMode = bfalse;
        m_isWaitingForRelease = bfalse;
        m_postRemapCooldown = 0.0f;
#if defined(ITF_WINDOWS)
        m_isEditingControllerType = bfalse;
        m_editingControllerTypeComponent = NULL;
        m_controllerTypeInputTimer = 0.0f;
        m_controllerTypeFirstPressTimer = 0.0f;
        m_controllerTypeFirstPressed = btrue;
        m_previousSelectionStates.clear();
#endif
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(m_menuBaseName, this);
        m_menu = UI_MENUMANAGER->getMenu(m_menuBaseName);

        if (!m_menu)
            return;

        s_activeHelper = this;

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

        // Show the controls remapping menu
        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), m_menuBaseName, btrue, this);
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
        if (handleAccept(componentId) || handleCancel(componentId))
            return;
#if defined(ITF_WINDOWS)
        if (actorId.isValid() && handleControllerTypeAction(actorId, component))
            return;
#endif
        if (actorId.isValid() && handleIconAction(actorId, component))
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

    bbool Ray_ControlsRemappingMenuHelper::parseIconId(const StringID& id, u32& outPlayerIndex,
                                                       ZInputManager::EGameAction& outAction)
    {
        // Player 1 icons
        if (id == ICON_PLAYER1_UP)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Up;
            return btrue;
        }
        if (id == ICON_PLAYER1_DOWN)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Down;
            return btrue;
        }
        if (id == ICON_PLAYER1_LEFT)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Left;
            return btrue;
        }
        if (id == ICON_PLAYER1_RIGHT)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Right;
            return btrue;
        }
        if (id == ICON_PLAYER1_RUN)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Run;
            return btrue;
        }
        if (id == ICON_PLAYER1_JUMP)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Jump;
            return btrue;
        }
        if (id == ICON_PLAYER1_HIT)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Hit;
            return btrue;
        }
        if (id == ICON_PLAYER1_BACK)
        {
            outPlayerIndex = 0;
            outAction = ZInputManager::Action_Back;
            return btrue;
        }

        // Player 2 icons
        if (id == ICON_PLAYER2_UP)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Up;
            return btrue;
        }
        if (id == ICON_PLAYER2_DOWN)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Down;
            return btrue;
        }
        if (id == ICON_PLAYER2_LEFT)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Left;
            return btrue;
        }
        if (id == ICON_PLAYER2_RIGHT)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Right;
            return btrue;
        }
        if (id == ICON_PLAYER2_RUN)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Run;
            return btrue;
        }
        if (id == ICON_PLAYER2_JUMP)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Jump;
            return btrue;
        }
        if (id == ICON_PLAYER2_HIT)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Hit;
            return btrue;
        }
        if (id == ICON_PLAYER2_BACK)
        {
            outPlayerIndex = 1;
            outAction = ZInputManager::Action_Back;
            return btrue;
        }

        // Player 3 icons
        if (id == ICON_PLAYER3_UP)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Up;
            return btrue;
        }
        if (id == ICON_PLAYER3_DOWN)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Down;
            return btrue;
        }
        if (id == ICON_PLAYER3_LEFT)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Left;
            return btrue;
        }
        if (id == ICON_PLAYER3_RIGHT)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Right;
            return btrue;
        }
        if (id == ICON_PLAYER3_RUN)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Run;
            return btrue;
        }
        if (id == ICON_PLAYER3_JUMP)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Jump;
            return btrue;
        }
        if (id == ICON_PLAYER3_HIT)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Hit;
            return btrue;
        }
        if (id == ICON_PLAYER3_BACK)
        {
            outPlayerIndex = 2;
            outAction = ZInputManager::Action_Back;
            return btrue;
        }

        // Player 4 icons
        if (id == ICON_PLAYER4_UP)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Up;
            return btrue;
        }
        if (id == ICON_PLAYER4_DOWN)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Down;
            return btrue;
        }
        if (id == ICON_PLAYER4_LEFT)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Left;
            return btrue;
        }
        if (id == ICON_PLAYER4_RIGHT)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Right;
            return btrue;
        }
        if (id == ICON_PLAYER4_RUN)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Run;
            return btrue;
        }
        if (id == ICON_PLAYER4_JUMP)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Jump;
            return btrue;
        }
        if (id == ICON_PLAYER4_HIT)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Hit;
            return btrue;
        }
        if (id == ICON_PLAYER4_BACK)
        {
            outPlayerIndex = 3;
            outAction = ZInputManager::Action_Back;
            return btrue;
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
        cancelRemappingMode(btrue);
        m_isRemappingMode = btrue;
        m_remappingPlayerIndex = playerIndex;
        m_remappingAction = action;
        m_remappingComponent = component;
        m_remappingCooldown = 0.25f;
        clearIconDisplay(component);
        
        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->SetSuppressReceive(btrue);
        }

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
        m_isWaitingForRelease = btrue;
        m_postRemapCooldown = 0.15f; 
        LOG("[ControlsRemapping] Entering wait-for-release state\n");
    }

    bbool Ray_ControlsRemappingMenuHelper::detectPhysicalControl(u32& outPhysicalControl)
    {
        outPhysicalControl = U32_INVALID;

        if (!GAMEMANAGER || !GAMEMANAGER->getInputManager())
            return bfalse;

        outPhysicalControl = GAMEMANAGER->getInputManager()->GetFirstActiveControl(m_remappingPlayerIndex);
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

        inputManager->SetActionRemap(m_remappingPlayerIndex, m_remappingAction, physicalControl);
        LOG("[ControlsRemapping] Remap complete Player %d, Action %d -> Physical %d\n", m_remappingPlayerIndex + 1, m_remappingAction, physicalControl);
        cancelRemappingMode(btrue);
    }

    void Ray_ControlsRemappingMenuHelper::updateRemappingMode(f32 deltaTime)
    {
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
        if (!detectPhysicalControl(physicalControl))
            return;

        if (physicalControl != U32_INVALID)
        {
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
        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->SetSuppressReceive(bfalse);
        }

        if (s_activeHelper == this)
        {
            s_activeHelper = nullptr;
        }
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

        if (m_isEditingControllerType)
        {
            exitControllerTypeEditMode();
        }
        else
        {
            enterControllerTypeEditMode(listComponent);
        }
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::enterControllerTypeEditMode(UIListOptionComponent* component)
    {
        if (!component || m_isEditingControllerType)
            return;

        m_isEditingControllerType = btrue;
        m_editingControllerTypeComponent = component;
        m_controllerTypeInputTimer = 0.0f;
        m_controllerTypeFirstPressTimer = 0.0f;
        m_controllerTypeFirstPressed = btrue;
        m_previousSelectionStates.clear();

        // Disable all other components to block navigation
        if (m_menu)
        {
            const ObjectRefList& componentsList = m_menu->getUIComponentsList();
            for (u32 i = 0; i < componentsList.size(); ++i)
            {
                UIComponent* comp = UIMenuManager::getUIComponent(componentsList[i]);
                if (!comp)
                    continue;

                bbool originalSelectable = comp->getCanBeSelected();

                // Keep current component selectable
                if (comp == static_cast<UIComponent*>(component))
                {
                    if (!originalSelectable)
                    {
                        m_previousSelectionStates.push_back(std::make_pair(comp, originalSelectable));
                        comp->setCanBeSelected(btrue);
                    }
                    continue;
                }

                // Disable all other selectable components
                if (originalSelectable)
                {
                    m_previousSelectionStates.push_back(std::make_pair(comp, originalSelectable));
                    comp->setCanBeSelected(bfalse);
                }
            }
        }

        component->setEditingMode(btrue);
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

        // Restore all component selectable states
        for (std::vector<std::pair<UIComponent*, bbool>>::iterator it = m_previousSelectionStates.begin();
             it != m_previousSelectionStates.end(); ++it)
        {
            if (it->first)
            {
                it->first->setCanBeSelected(it->second);
            }
        }
        m_previousSelectionStates.clear();

        m_isEditingControllerType = bfalse;
        m_editingControllerTypeComponent = NULL;
        m_controllerTypeInputTimer = 0.0f;
        m_controllerTypeFirstPressTimer = 0.0f;
        m_controllerTypeFirstPressed = btrue;
        LOG("[ControlsRemapping] Exited controller type edit mode\n");
    }

    void Ray_ControlsRemappingMenuHelper::adjustControllerType(UIListOptionComponent* listComponent, i32 direction)
    {
        if (!listComponent || direction == 0 || !RAY_GAMEMANAGER)
            return;

        i32 currentIndex = RAY_GAMEMANAGER->getPCControlMode();
        i32 newIndex = currentIndex + direction;

        // Wrap around
        if (newIndex < 0)
            newIndex = PC_CONTROL_MODE_CHOICES - 1;
        else if (newIndex >= static_cast<i32>(PC_CONTROL_MODE_CHOICES))
            newIndex = 0;

        if (newIndex == currentIndex)
            return;

        RAY_GAMEMANAGER->setPCControlMode(newIndex);
        updateControllerTypeDisplay(listComponent, newIndex);
        LOG("[ControlsRemapping] Controller type changed to: %s\n", RAY_GAMEMANAGER->getPCControlModeDisplayName(newIndex));
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
            return NULL;

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
        return NULL;
    }

    bbool Ray_ControlsRemappingMenuHelper::onMenuItemOtherAction(UIComponent* component, const StringID& action)
    {
        if (!m_isEditingControllerType)
            return bfalse;

        // Block ALL navigation/actions while in edit mode to ensure we handle them
        if (!component)
            return btrue;

        UIListOptionComponent* listComponent = component->DynamicCast<UIListOptionComponent>(ITF_GET_STRINGID_CRC(UIListOptionComponent, 3621365669));
        if (!listComponent || listComponent != m_editingControllerTypeComponent)
        {
            // Block actions on other components while editing
            return btrue;
        }

        // Handle back button to exit edit mode
        if (action == input_actionID_Back)
        {
            exitControllerTypeEditMode();
            return btrue;
        }

        // Handle validate (A/Enter) to also exit edit mode
        if (action == input_actionID_Valid)
        {
            exitControllerTypeEditMode();
            return btrue;
        }

        // Block up/down navigation while editing
        if (action == input_actionID_Up || action == input_actionID_UpHold ||
            action == input_actionID_Down || action == input_actionID_DownHold)
        {
            return btrue;
        }

        static const f32 FIRST_PRESS_DELAY = 0.35f;
        static const f32 REPEAT_RATE = 0.12f;

        // Handle left press - immediate first action
        if (action == input_actionID_Left)
        {
            m_controllerTypeFirstPressed = btrue;
            m_controllerTypeFirstPressTimer = 0.0f;
            adjustControllerType(listComponent, -1);
            return btrue;
        }

        // Handle right press - immediate first action
        if (action == input_actionID_Right)
        {
            m_controllerTypeFirstPressed = btrue;
            m_controllerTypeFirstPressTimer = 0.0f;
            adjustControllerType(listComponent, 1);
            return btrue;
        }

        // Handle hold for repeat with initial delay
        if (action == input_actionID_LeftHold || action == input_actionID_RightHold)
        {
            m_controllerTypeFirstPressTimer += LOGICDT;
            
            if (m_controllerTypeFirstPressed)
            {
                // Wait for first press delay before starting repeat
                if (m_controllerTypeFirstPressTimer > FIRST_PRESS_DELAY)
                {
                    m_controllerTypeFirstPressed = bfalse;
                    m_controllerTypeInputTimer = 0.0f;
                    i32 dir = (action == input_actionID_LeftHold) ? -1 : 1;
                    adjustControllerType(listComponent, dir);
                }
            }
            else
            {
                // Regular repeat rate
                m_controllerTypeInputTimer += LOGICDT;
                if (m_controllerTypeInputTimer > REPEAT_RATE)
                {
                    m_controllerTypeInputTimer = 0.0f;
                    i32 dir = (action == input_actionID_LeftHold) ? -1 : 1;
                    adjustControllerType(listComponent, dir);
                }
            }
            return btrue;
        }

        return btrue; // Block all other actions while editing
    }
#else
    bbool Ray_ControlsRemappingMenuHelper::onMenuItemOtherAction(UIComponent* /*component*/, const StringID& /*action*/)
    {
        return bfalse;
    }
#endif
}
