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

namespace ITF
{
    namespace
    {
        struct ButtonControlMapping
        {
            u32 buttonIndex;
            u32 controlIndex;
        };

        struct AxisControlMapping
        {
            u32 axisIndex;
            f32 threshold;
            bbool positive;
            u32 controlIndex;
        };

        static const ButtonControlMapping kButtonMappings[] = {
            { m_joyButton_DPadU, ZPad_Base::DPAD_UP },
            { m_joyButton_DPadD, ZPad_Base::DPAD_DOWN },
            { m_joyButton_DPadL, ZPad_Base::DPAD_LEFT },
            { m_joyButton_DPadR, ZPad_Base::DPAD_RIGHT },
            { m_joyButton_A, ZPad_Base::BUTTON_FACE_SOUTH },
            { m_joyButton_B, ZPad_Base::BUTTON_FACE_EAST },
            { m_joyButton_X, ZPad_Base::BUTTON_FACE_WEST },
            { m_joyButton_Y, ZPad_Base::BUTTON_FACE_NORTH },
            { m_joyButton_LB, ZPad_Base::BUTTON_L_SHOULDER },
            { m_joyButton_RB, ZPad_Base::BUTTON_R_SHOULDER },
            { m_joyButton_ThumbLeft, ZPad_Base::BUTTON_L_THUMB },
            { m_joyButton_ThumbRight, ZPad_Base::BUTTON_R_THUMB },
            { m_joyButton_Back, ZPad_Base::BUTTON_SELECT },
            { m_joyButton_Start, ZPad_Base::BUTTON_START },
            { m_joyButton_TriggerLeft, ZPad_Base::TRIGGER_LEFT },
            { m_joyButton_TriggerRight, ZPad_Base::TRIGGER_RIGHT },
        };

        static const AxisControlMapping kAxisMappings[] = {
            { m_joyStickLeft_Y, 0.55f, btrue,  ZPad_Base::STICK_L_UP },
            { m_joyStickLeft_Y, 0.55f, bfalse, ZPad_Base::STICK_L_DOWN },
            { m_joyStickLeft_X, 0.55f, btrue,  ZPad_Base::STICK_L_RIGHT },
            { m_joyStickLeft_X, 0.55f, bfalse, ZPad_Base::STICK_L_LEFT },
            { m_joyStickRight_Y, 0.55f, btrue,  ZPad_Base::STICK_R_UP },
            { m_joyStickRight_Y, 0.55f, bfalse, ZPad_Base::STICK_R_DOWN },
            { m_joyStickRight_X, 0.55f, btrue,  ZPad_Base::STICK_R_RIGHT },
            { m_joyStickRight_X, 0.55f, bfalse, ZPad_Base::STICK_R_LEFT },
            { m_joyTrigger_Left, 0.5f, btrue,  ZPad_Base::TRIGGER_LEFT },
            { m_joyTrigger_Right, 0.5f, btrue,  ZPad_Base::TRIGGER_RIGHT },
        };

        static const f32 kRemapInputDelay = 0.25f;
    }

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
    {
        m_menuBaseName = "controlremapping";
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            m_lastButtonStates[i] = InputAdapter::Released;
        }
        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            m_lastAxisStates[i] = 0.0f;
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

        hideContextIcons();

        m_isActive = btrue;
        m_isRemappingMode = bfalse;
        m_isWaitingForRelease = bfalse;
        m_postRemapCooldown = 0.0f;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(m_menuBaseName, this);
        m_menu = UI_MENUMANAGER->getMenu(m_menuBaseName);

        if (!m_menu)
            return;

        s_activeHelper = this;
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
        m_remappingCooldown = kRemapInputDelay;
        clearIconDisplay(component);
        snapshotInputState();
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

    void Ray_ControlsRemappingMenuHelper::snapshotInputState()
    {
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            m_lastButtonStates[i] = InputAdapter::Released;
        }

        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            m_lastAxisStates[i] = 0.0f;
        }

        if (!INPUT_ADAPTER)
            return;

        if (m_remappingPlayerIndex >= JOY_MAX_COUNT)
            return;

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, buttons, JOY_MAX_BUT);
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            m_lastButtonStates[i] = buttons[i];
        }

        f32 axes[JOY_MAX_AXES] = {0.0f};
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, axes, JOY_MAX_AXES);
        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            m_lastAxisStates[i] = axes[i];
        }
    }

    bbool Ray_ControlsRemappingMenuHelper::detectPhysicalControl(u32& outPhysicalControl)
    {
        outPhysicalControl = U32_INVALID;

        if (!INPUT_ADAPTER)
            return bfalse;

        if (m_remappingPlayerIndex >= JOY_MAX_COUNT)
            return bfalse;

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, buttons, JOY_MAX_BUT);

        f32 axes[JOY_MAX_AXES] = {0.0f};
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, axes, JOY_MAX_AXES);

        auto buttonTriggered = [&](u32 idx) -> bbool
        {
            if (idx >= JOY_MAX_BUT)
                return bfalse;

            InputAdapter::PressStatus current = buttons[idx];
            InputAdapter::PressStatus previous = m_lastButtonStates[idx];

            if (current == InputAdapter::JustPressed || current == InputAdapter::Double_Press)
                return btrue;

            if (current == InputAdapter::Pressed && (previous == InputAdapter::Released || previous == InputAdapter::JustReleased))
                return btrue;

            return bfalse;
        };

        for (u32 i = 0; i < sizeof(kButtonMappings) / sizeof(kButtonMappings[0]); ++i)
        {
            if (buttonTriggered(kButtonMappings[i].buttonIndex))
            {
                outPhysicalControl = kButtonMappings[i].controlIndex;
                for (u32 b = 0; b < JOY_MAX_BUT; ++b)
                {
                    m_lastButtonStates[b] = buttons[b];
                }
                for (u32 a = 0; a < JOY_MAX_AXES; ++a)
                {
                    m_lastAxisStates[a] = axes[a];
                }
                return btrue;
            }
        }

        auto axisActivated = [&](const AxisControlMapping& mapping) -> bbool
        {
            if (mapping.axisIndex >= JOY_MAX_AXES)
                return bfalse;

            f32 current = axes[mapping.axisIndex];
            f32 previous = m_lastAxisStates[mapping.axisIndex];

            if (mapping.positive)
            {
                return current > mapping.threshold && previous <= mapping.threshold;
            }

            return current < -mapping.threshold && previous >= -mapping.threshold;
        };

        for (u32 i = 0; i < sizeof(kAxisMappings) / sizeof(kAxisMappings[0]); ++i)
        {
            if (axisActivated(kAxisMappings[i]))
            {
                outPhysicalControl = kAxisMappings[i].controlIndex;
                for (u32 b = 0; b < JOY_MAX_BUT; ++b)
                {
                    m_lastButtonStates[b] = buttons[b];
                }
                for (u32 a = 0; a < JOY_MAX_AXES; ++a)
                {
                    m_lastAxisStates[a] = axes[a];
                }
                return btrue;
            }
        }

        for (u32 b = 0; b < JOY_MAX_BUT; ++b)
        {
            m_lastButtonStates[b] = buttons[b];
        }
        for (u32 a = 0; a < JOY_MAX_AXES; ++a)
        {
            m_lastAxisStates[a] = axes[a];
        }

        return bfalse;
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
            bbool allReleased = btrue;
            if (INPUT_ADAPTER && m_postRemapCooldown <= 0.0f)
            {
                InputAdapter::PressStatus buttons[JOY_MAX_BUT];
                INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, buttons, JOY_MAX_BUT);

                f32 axes[JOY_MAX_AXES] = {0.0f};
                INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_remappingPlayerIndex, axes, JOY_MAX_AXES);
                for (u32 i = 0; i < JOY_MAX_BUT; ++i)
                {
                    if (buttons[i] == InputAdapter::Pressed || buttons[i] == InputAdapter::JustPressed)
                    {
                        allReleased = bfalse;
                        break;
                    }
                }
                if (allReleased)
                {
                    const f32 threshold = 0.4f;
                    for (u32 i = 0; i < JOY_MAX_AXES; ++i)
                    {
                        if (axes[i] > threshold || axes[i] < -threshold)
                        {
                            allReleased = bfalse;
                            break;
                        }
                    }
                }
            }
            else
            {
                allReleased = bfalse;
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
        if (GAMEMANAGER && GAMEMANAGER->getInputManager())
        {
            GAMEMANAGER->getInputManager()->SetSuppressReceive(bfalse);
        }

        if (s_activeHelper == this)
        {
            s_activeHelper = nullptr;
        }
    }
}
