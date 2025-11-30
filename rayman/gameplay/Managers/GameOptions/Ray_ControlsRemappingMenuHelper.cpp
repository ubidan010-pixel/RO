#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CONTROLSREMAPPINGMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_ControlsRemappingMenuHelper.h"
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

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif

namespace ITF
{
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
          , m_remappingPlayerIndex(0)
          , m_remappingAction(ZInputManager::Action_Up)
    {
        m_menuBaseName = "controlremapping";
    }

    Ray_ControlsRemappingMenuHelper::~Ray_ControlsRemappingMenuHelper()
    = default;

    void Ray_ControlsRemappingMenuHelper::activateForControlsRemappingMenu(MenuItemActionListener* mainListener)
    {
        if (!UI_MENUMANAGER)
            return;

        hideContextIcons();

        m_isActive = btrue;
        m_isRemappingMode = bfalse;
        m_mainListener = mainListener;
        UI_MENUMANAGER->setMenuListener(m_menuBaseName, this);
        m_menu = UI_MENUMANAGER->getMenu(m_menuBaseName);

        if (!m_menu)
            return;

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
        if (actorId.isValid() && handleIconAction(actorId))
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

    bbool Ray_ControlsRemappingMenuHelper::handleIconAction(const StringID& id)
    {
        u32 playerIndex = 0;
        ZInputManager::EGameAction action = ZInputManager::Action_Up;

        if (!parseIconId(id, playerIndex, action))
            return bfalse;

        startRemappingMode(playerIndex, action);
        return btrue;
    }

    void Ray_ControlsRemappingMenuHelper::startRemappingMode(u32 playerIndex, ZInputManager::EGameAction action)
    {
        m_isRemappingMode = btrue;
        m_remappingPlayerIndex = playerIndex;
        m_remappingAction = action;

        LOG("[ControlsRemapping] Starting remap mode for Player %d, Action %d\n", playerIndex + 1, action);
    }
}
