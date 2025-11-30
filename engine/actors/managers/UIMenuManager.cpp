#include "precompiled_engine.h"
#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif //_ITF_RAY_OPTIONMENUHELPER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_UIMENUSOUNDCOMPONENT_H_
#include "engine/actors/components/UIMenuSoundComponent.h"
#endif //_ITF_UIMENUSOUNDCOMPONENT_H_

#include "gameplay/Components/UI/UISliderComponent.h"

namespace ITF
{
    namespace
    {
#ifdef ITF_WINDOWS
        static const StringID kMenuStartID("menustartpc");
#else
        static const StringID kMenuStartID("menustartconsole");
#endif
        static const StringID kUbiConnectID("ubiconnect_button");
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    class DebugMenuSwapPlayer : public DebugMenuEntry
    {
    public:
        DebugMenuSwapPlayer(const String & _name, u32 _index)
            : DebugMenuEntry(_name)
            , m_index(_index)
        {}
        virtual void select(u32 _playerIndex) const
        {
            GAMEMANAGER->swapPlayer(_playerIndex, m_index);
        }
    private:
        u32 m_index;
    };

    class DebugMenuChangeFPS : public DebugMenuEntry
    {
    public:

        enum ChangeFPSType
        {
            TYPE_APP,
            TYPE_LOGIC,
        };

        DebugMenuChangeFPS(const String & _name, ChangeFPSType _type, u32 _fps)
            : DebugMenuEntry(_name)
            , m_type(_type)
            , m_fps(_fps)
        {}
        virtual void select(u32 _index) const
        {
            switch(m_type)
            {
                case TYPE_APP:
                    Singletons::get().setAppLoopFPS(static_cast<f32>(m_fps));
                    break;
                case TYPE_LOGIC:
                    Singletons::get().setLogicFPS(static_cast<f32>(m_fps));
                    break;
            }
        }
    private:

        ChangeFPSType   m_type;
        u32             m_fps;
    };

#endif // ITF_SUPPORT_CHEAT

#define STRINGID_ACTION_MENU_OPEN ITF_GET_STRINGID_CRC(MENU_OPEN, 1045216930)
#define STRINGID_ACTION_MENU_OPEN_FIRST ITF_GET_STRINGID_CRC(MENU_OPEN_FIRST, 2402030665)
#define STRINGID_ACTION_MENU_CLOSE ITF_GET_STRINGID_CRC(MENU_CLOSE, 4048853777)
#define STRINGID_ACTION_MENU_SELECTION_CHANGE ITF_GET_STRINGID_CRC(MENU_SELECTION_CHANGE, 2559508881)

#define PROFILE_SELECTION_MENU_ID	4141360183
#define MAIN_MENU_MENU_ID			1796752723

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuManager::UIMenuManager ()
        :
        m_inMenu(bfalse)
#ifdef ITF_SUPPORT_CHEAT
        , m_currentPageDBGMenu(0)
        , m_inDBGMenu(bfalse)
        , m_timerDBGMenu(0.0f)
        , m_timeoutDBGMenu(0.05f)
#endif // ITF_SUPPORT_CHEAT
#ifndef ITF_FINAL
		, m_mouseLeftInput(true)
#endif // ITF_FINAL
        , m_player(U32_INVALID)
        , m_timeout(0.2f)
        , m_timeoutJustPressed(0.3f)
        , m_timer(0.0f)
        , m_menuAction_ShowPreviousMenu("ShowPrevMenu")
        , m_menuAction_HideMenu("HideMenu")
        , m_menuAction_ApplyMenuItemAction("ApplyItem")
        , m_menuAction_Nothing("Nothing")
        , m_firstPressed(btrue)
        , m_joyReleased (btrue)
        , m_isMenuReady(bfalse)
        , m_canExecuteMenuItemAction(bfalse)
        , m_inGammaSettingsMenu(bfalse)
        , m_listenerRegistered(bfalse)
        , m_menuStartLastMainSelectionID(StringID::Invalid)
        , m_soundPlayerComponent(NULL)
#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)
		, m_mouseLocked(false)
#endif //ITF_WINDOWS
		{
#ifdef ITF_SUPPORT_CHEAT
        m_debugMenuMaps = newAlloc(mId_UI,DebugMenu);
        m_debugMenuCheats = newAlloc(mId_UI,DebugMenu);
        m_debugMenuSwapPlayers = newAlloc(mId_UI,DebugMenu);
        m_debugMenuSwapPlayers->addEntry(newAlloc(mId_UI, DebugMenuSwapPlayer("player 1", 0)));
        m_debugMenuSwapPlayers->addEntry(newAlloc(mId_UI, DebugMenuSwapPlayer("player 2", 1)));
        m_debugMenuSwapPlayers->addEntry(newAlloc(mId_UI, DebugMenuSwapPlayer("player 3", 2)));
        m_debugMenuSwapPlayers->addEntry(newAlloc(mId_UI, DebugMenuSwapPlayer("player 4", 3)));


        m_debugChangeFramerate = newAlloc(mId_UI,DebugMenu);
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("App loop 60fps", DebugMenuChangeFPS::TYPE_APP, 60)));
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("App loop 50fps", DebugMenuChangeFPS::TYPE_APP, 50)));
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("App loop 30fps", DebugMenuChangeFPS::TYPE_APP, 30)));
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("Game logic 60fps", DebugMenuChangeFPS::TYPE_LOGIC, 60)));
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("Game logic 50fps", DebugMenuChangeFPS::TYPE_LOGIC, 50)));
        m_debugChangeFramerate->addEntry(newAlloc(mId_UI, DebugMenuChangeFPS("Game logic 30fps", DebugMenuChangeFPS::TYPE_LOGIC, 30)));

        m_DBGmenus.push_back(m_debugMenuMaps);
        m_DBGmenus.push_back(m_debugMenuCheats);
        m_DBGmenus.push_back(m_debugMenuSwapPlayers);
        m_DBGmenus.push_back(m_debugChangeFramerate);
#endif // ITF_SUPPORT_CHEAT
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuManager::~UIMenuManager ()
    {
        for (ITF_VECTOR<UIMenu*>::iterator iter = m_menus.begin(); iter!=m_menus.end(); iter++)
            SF_DEL(*iter);

#ifdef ITF_SUPPORT_CHEAT
        for (ITF_VECTOR<DebugMenu*>::iterator iter = m_DBGmenus.begin(); iter!=m_DBGmenus.end(); iter++)
            SF_DEL(*iter);
#endif // ITF_SUPPORT_CHEAT
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::update ()
    {
        if(!m_listenerRegistered)
        {
			GAMEMANAGER->addMenuInputListener(this);
#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)
			if (INPUT_ADAPTER)
			{
				INPUT_ADAPTER->addListener(this, 2);
			}
#endif
			m_listenerRegistered = btrue;
        }

#ifdef ITF_SUPPORT_CHEAT
        // debug menu
        updateDBGMenuStatus();
        if (m_inDBGMenu)
        {
            updateSwapMenu();
            displayDebugMenu();
        }
#endif // ITF_SUPPORT_CHEAT

        m_timer += LOGICDT;
#ifdef ITF_SUPPORT_CHEAT
        if(GFX_ADAPTER->getfPs())
            m_timerDBGMenu += 1.f / GFX_ADAPTER->getfPs();
#endif // ITF_SUPPORT_CHEAT

        Ray_OptionMenuHelper* optionHelper = Ray_OptionMenuHelper::getActiveHelper();
        if (optionHelper && optionHelper->isActive())
        {
            optionHelper->updateTimer();
        }

        if (m_inMenu)
        {
            if (!m_isMenuReady)
            {
                m_isMenuReady = btrue;
            }
            else
            {
                m_canExecuteMenuItemAction = btrue;
            }
        }
        else
        {
            m_isMenuReady = bfalse;
            m_canExecuteMenuItemAction = bfalse;
        }
        // Check for pads
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            // Buttons
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

            // Joysticks
            float pos[JOY_MAX_AXES];
            INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentLua,i, pos, JOY_MAX_AXES);

#ifdef ITF_SUPPORT_CHEAT
            if (m_inDBGMenu)
                applyGamePadButtonDBGMenu (i,buts,pos);
#endif // ITF_SUPPORT_CHEAT
        }

    }

void UIMenuManager::notifySiblingUIComponents(UIComponent* component, bbool isSelected)
{
    if (!component)
        return;

    Actor* actor = component->GetActor();
    if (!actor)
        return;

    SafeArray<UIComponent*> siblings = actor->GetComponents<UIComponent>();
    for (u32 i = 0; i < siblings.size(); ++i)
    {
        if (siblings[i] != component)
        {
            siblings[i]->setIsSelected(isSelected);
            if (isSelected)
                siblings[i]->onRollover();
            else
                siblings[i]->onRollout();
        }
    }
}

void UIMenuManager::applySelectionChange(UIMenu* menu, UIComponent* oldSel, UIComponent* newSel)
{
    MenuItemActionListener* listener = getCurrentMenuActionListener();
    
    if (oldSel)
    {
        oldSel->setIsSelected(bfalse);
        oldSel->onRollout();
        if (listener) 
            listener->UpdateMenuOnSelectionChange(oldSel, false);
        notifySiblingUIComponents(oldSel, bfalse);
    }

    if (newSel)
    {
        newSel->setIsSelected(btrue);
        newSel->onRollover();
        if (listener) 
            listener->UpdateMenuOnSelectionChange(newSel, true);
        notifySiblingUIComponents(newSel, btrue);
    }

    if (menu)
    {
        rememberMainSelectionForMenuStart(oldSel, newSel, menu);
    }
}

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIMenuManager::executeMenuPageAction(const StringID &_action, const StringID &_inputAction)
    {
        if (_action==StringID())
            return bfalse;
        else if (_action==getMenuPageAction_ShowPreviousMenu())
        {
            if (m_menuTree.size() == 0 && GAMEMANAGER->isInPause())
            {
                SYSTEM_ADAPTER->setSystemUnPauseFlag(btrue);
                return btrue;
            }
            else
            {
                showPreviousMenu();
                return btrue;
            }
        }
        else if (_action==getMenuPageAction_HideMenu())
        {
            hideMenu(getHighestShownPriority());
            return btrue;
        }
        else if (_action==getMenuPageAction_ApplyMenuItemAction())
        {
            applyMenuItemAction();
            return btrue;
        }
        else if (_action==getMenuPageAction_Nothing())
        {
            return applyMenuItemOtherAction(_inputAction);
        }
        else
        {
            ITF_ASSERT_MSG(bfalse, "Unknown menu action");
            return bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID UIMenuManager::fetchMenuPageAction(const StringID &_action, const StringID &_defaultAction)
    {
        MenuItemActionListener *listener = getCurrentMenuActionListener();
        if (listener)
            return listener->onMenuPageAction(getMenu(m_currentMenuID), _action, _defaultAction);

        return _defaultAction;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID UIMenuManager::onMenuPageAction_DefaultRootMenu(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction)
    {
        if (_action == input_actionID_Valid)
            return getMenuPageAction_ApplyMenuItemAction();
        else if (_action == input_actionID_Back)
            return getMenuPageAction_ShowPreviousMenu();
        else if (_action == input_actionID_Other || _action == input_actionID_DeleteSave )
            return getMenuPageAction_Nothing();

        StringID nullCmd;
        return nullCmd;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID UIMenuManager::onMenuPageAction_Default1ButtonMenu(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction)
    {
        if (_action == input_actionID_Valid)
            return getMenuPageAction_ApplyMenuItemAction();
        StringID nullCmd;
        return nullCmd;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIMenuManager::applyGamePadButton (const StringID & _action)
    {
        StringID menuCommand;

        UIMenu* pMenu = getMenu(m_currentMenuID);
        bbool   bStartActionIsValid = bfalse;
        if(pMenu)
        {
            if ( pMenu->getMenuID() == ITF_GET_STRINGID_CRC(menuStart,3997834800) )
            {
                bStartActionIsValid = btrue;
            }
        }

        // Use of the D-pad: the button is just pressed -> go up, down, left or right
        if ( _action == input_actionID_Left )
        {
            m_timer = 0.0f;
            selectMenuItemWithDPad (m_joyButton_DPadL);
            m_firstPressed = btrue;
        }
        else if ( _action == input_actionID_Down )
        {
            m_timer = 0.0f;
            selectMenuItemWithDPad (m_joyButton_DPadD);
            m_firstPressed = btrue;
        }
        else if ( _action == input_actionID_Right )
        {
            m_timer = 0.0f;
            selectMenuItemWithDPad (m_joyButton_DPadR);
            m_firstPressed = btrue;
        }
        else if ( _action == input_actionID_Up )
        {
            m_timer = 0.0f;
            selectMenuItemWithDPad (m_joyButton_DPadU);
            m_firstPressed = btrue;
        }

        // Use of the D-pad: the button is pressed -> scroll up, down, left or right
        else if (m_timer > m_timeout && _action == input_actionID_UpHold )
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;
                selectMenuItemWithDPad (m_joyButton_DPadU);
            }
        }
        else if (m_timer > m_timeout && _action == input_actionID_DownHold )
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;
                selectMenuItemWithDPad (m_joyButton_DPadD);
            }
        }
        else if (m_timer > m_timeout && _action == input_actionID_RightHold )
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;
                selectMenuItemWithDPad (m_joyButton_DPadR);
            }
        }
        else if (m_timer > m_timeout && _action == input_actionID_LeftHold )
        {
            if (m_firstPressed && m_timer > m_timeoutJustPressed)
            {
                m_timer = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timer = 0.0f;
                selectMenuItemWithDPad (m_joyButton_DPadL);
            }
        }

        //// Use of the joystick: the joy is just pressed -> go up or down
        //else if (m_joyReleased && (_pos[0] > 0.7f || _pos[1] > 0.7f || _pos[0] < -0.7f || _pos[1] < -0.7f))
        //{
        //    m_joyReleased = bfalse;
        //    m_timer = 0.0f;
        //    selectMenuItemWithJoystick(_pos);
        //    m_firstPressed = btrue;
        //}

        //// Use of the joystick: the joy is pressed -> scroll up or down
        //else if (m_timer > m_timeout && (_pos[0] > 0.7f || _pos[1] > 0.7f || _pos[0] < -0.7f || _pos[1] < -0.7f))
        //{
        //    if (m_firstPressed && m_timer > m_timeoutJustPressed)
        //    {
        //        m_timer = 0.0f;
        //        m_firstPressed = bfalse;
        //    }
        //    if (!m_firstPressed)
        //    {
        //        m_timer = 0.0f;
        //        selectMenuItemWithJoystick(_pos);;
        //    }
        //}

        //// The joystick is released
        //else if (_pos[0] != 0.0f && _pos[1] != 0.0f &&
        //    _pos[0] > -0.7f && _pos[0] < 0.7f &&
        //    _pos[1] > -0.7f && _pos[1] < 0.7f)
        //    m_joyReleased = btrue;

        // Press Item action
        else if ( _action == input_actionID_Valid || (bStartActionIsValid && _action == input_actionID_Start) )
        {
            m_firstPressed = btrue;
            m_timer = 0.0;
            applyMenuPressAction();
        }
        //else if (_buts[m_joyButton_A] == InputAdapter::Pressed )
        //{
        //    if (m_firstPressed && m_timer > m_timeoutJustPressed)
        //    {
        //        m_timer = 0.0f;
        //        m_firstPressed = bfalse;
        //    }
        //    if (!m_firstPressed)
        //    {
        //        m_timer = 0.0f;
        //        applyMenuPressAction();;
        //    }
        //}

        // Others buttons
        if ( _action == input_actionID_Valid || (bStartActionIsValid && _action == input_actionID_Start) )
            menuCommand = fetchMenuPageAction(input_actionID_Valid, getMenuPageAction_ApplyMenuItemAction());//applyMenuItemAction();
        else if ( _action == input_actionID_Back )
            menuCommand = fetchMenuPageAction(_action, getMenuPageAction_ShowPreviousMenu());
        else if ( _action == input_actionID_Other || _action == input_actionID_DeleteSave )
            menuCommand = fetchMenuPageAction(_action, getMenuPageAction_Nothing());


        if (pMenu)
        {
            UIComponent* pUIComponent = pMenu->getUIComponentSelected();
            if (pUIComponent)
            {
                if ( _action == input_actionID_onReleased )
                {
                    pUIComponent->onReleased();
                    pUIComponent->setIsPressed(bfalse);
                }
                else if ( _action == input_actionID_onPressed )
                {
                    pUIComponent->onPressed();
                    pUIComponent->setIsPressed(btrue);
                }

                pUIComponent->onAction(_action);
            }
        }

        return executeMenuPageAction(menuCommand, _action);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    void UIMenuManager::applyGamePadButtonDBGMenu (u32 _numPad, InputAdapter::PressStatus* _buts, float* _pos)
    {
        // Page change in debug menu
        if (_buts[m_joyButton_RB] == InputAdapter::JustPressed)
        {
            m_currentPageDBGMenu = (m_currentPageDBGMenu+1)%m_DBGmenus.size();
            m_player = _numPad; // update player index
        }
        else if (_buts[m_joyButton_LB] == InputAdapter::JustPressed)
        {
            m_currentPageDBGMenu = (m_currentPageDBGMenu-1)%m_DBGmenus.size();
            m_player = _numPad; // update player index
        }

        // Use of the D-pad: the button is just pressed -> go up or down
        else if (_buts[m_joyButton_DPadU] == InputAdapter::JustPressed )
        {
            m_timerDBGMenu = 0.0f;
            m_DBGmenus[m_currentPageDBGMenu]->up();
            m_firstPressed = btrue;
            m_player = _numPad; // update player index
        }
        else if (_buts[m_joyButton_DPadD] == InputAdapter::JustPressed )
        {
            m_timerDBGMenu = 0.0f;
            m_DBGmenus[m_currentPageDBGMenu]->down();
            m_firstPressed = btrue;
            m_player = _numPad; // update player index
        }

        // Use of the D-pad: the button is pressed -> scroll up or down
        else if (m_timerDBGMenu > m_timeoutDBGMenu && _buts[m_joyButton_DPadU] == InputAdapter::Pressed)
        {
            if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_DBGmenus[m_currentPageDBGMenu]->up();
            }
        }
        else if (m_timerDBGMenu > m_timeoutDBGMenu && _buts[m_joyButton_DPadD] == InputAdapter::Pressed)
        {
            if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_DBGmenus[m_currentPageDBGMenu]->down();
            }
        }

        // Use of the joystick: the joy is just pressed -> go up or down
        else if (m_joyReleased && _pos[1] > 0.7f)
        {
            m_joyReleased = bfalse;
            m_timerDBGMenu = 0.0f;
            m_DBGmenus[m_currentPageDBGMenu]->up();
            m_firstPressed = btrue;
            m_player = _numPad; // update player index
        }
        else if (m_joyReleased && _pos[1] < -0.7f)
        {
            m_joyReleased = bfalse;
            m_timerDBGMenu = 0.0f;
            m_DBGmenus[m_currentPageDBGMenu]->down();
            m_firstPressed = btrue;
            m_player = _numPad; // update player index
        }

        // Use of the joystick: the joy is pressed -> scroll up or down
        else if (m_timerDBGMenu > m_timeoutDBGMenu && _pos[1] > 0.7f)
        {
            if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_DBGmenus[m_currentPageDBGMenu]->up();
            }
        }
        else if (m_timerDBGMenu > m_timeoutDBGMenu && _pos[1] < -0.7f)
        {
            if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_firstPressed = bfalse;
            }
            if (!m_firstPressed)
            {
                m_timerDBGMenu = 0.0f;
                m_DBGmenus[m_currentPageDBGMenu]->down();
            }
        }

        // The joystick is released
        else if (_pos[1] != 0.0f && _pos[1] > -0.7f && _pos[1] < 0.7f)
            m_joyReleased = btrue;

        // Others buttons
        else if (_buts[m_joyButton_X] == InputAdapter::JustPressed)
            CHEATMANAGER->setActive( !CHEATMANAGER->getActive() );
        else if (_buts[m_joyButton_A] == InputAdapter::JustPressed)
        {
            m_DBGmenus[m_currentPageDBGMenu]->select(_numPad);
            m_player = _numPad; // update player index
        }
    }
#endif // ITF_SUPPORT_CHEAT

    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    void UIMenuManager::updateDBGMenuStatus     ()
    {
#ifdef ITF_SUPPORT_CHEAT
        if (!CONFIG->m_allowDebug)
        {
            return;
        }

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

            bbool bPressedLBRB = buts[m_joyButton_LB] == InputAdapter::Pressed &&  buts[m_joyButton_RB] == InputAdapter::Pressed;
#ifdef ITF_WII //LG :tmp code please fix it
            bPressedLBRB = btrue;
#endif //ITF_WII


            if ( bPressedLBRB)
            {
                // LB+RB+Back to show debug menu
                if (buts[m_joyButton_DPadU] == InputAdapter::JustPressed)
                {
                    bool newDebugMenuEnabled = !UI_MENUMANAGER->getIsInDBGMenu();
                    UI_MENUMANAGER->setIsInDBGMenu(newDebugMenuEnabled, i);

                    // Pause the game
                    if (newDebugMenuEnabled /*|| UI_MENUMANAGER->getIsInMenu()*/)
                        INPUT_ADAPTER->disableEnvironment(InputAdapter::EnvironmentEngine);
                    else
                        INPUT_ADAPTER->enableEnvironment(InputAdapter::EnvironmentEngine);

                    UI_MENUMANAGER->setIsInDBGMenu(newDebugMenuEnabled, i);
                }
                else if (buts[m_joyButton_DPadD] == InputAdapter::JustPressed)
                {
                    GAMEMANAGER->cheatUnlockAllLevel();
                }
            }

        }
#endif //ITF_SUPPORT_CHEAT
    }
#endif // ITF_SUPPORT_CHEAT

    static bbool IsFreeAccessMenu(const StringID& menuId)
    {
        // optionmenu
        static const StringID optionMenuID = ITF_GET_STRINGID_CRC(optionmenu, 1286202687);
        return menuId == optionMenuID;
    }

#ifdef ITF_SUPPORT_CHEAT
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::addDebugMenuMap (DebugMenuEntry *_entry)
    {
        m_debugMenuMaps->addEntry(_entry);
    }
#endif // ITF_SUPPORT_CHEAT

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::addMenu (String8 _menuBaseName)
    {
        StringID menuID = StringID(_menuBaseName);
        m_menus.push_back(newAlloc(mId_UI,UIMenu(menuID)));
        UIMenu* pUIMenu = getMenu(menuID);
        if(pUIMenu)
            pUIMenu->setMenuFriendly(_menuBaseName);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::addUIComponentToMenu (ObjectRef _ref)
    {
        UIComponent* pUIComponent = getUIComponent(_ref);
        if (pUIComponent)
        {
            for (ITF_VECTOR<UIMenu*>::iterator iter = m_menus.begin(); iter!=m_menus.end(); iter++)
            {
                UIMenu* pMenu = (*iter);
                if (pMenu && pMenu->getMenuID() == pUIComponent->getMenuID())
                {
                    pMenu->addUIComponent(_ref);
                    pMenu->reinitMenuSelection();
                    break;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::registerUIMenuSoundPlayer(UIMenuSoundComponent * const _soundPlayerComponent)
    {
        ITF_ASSERT(m_soundPlayerComponent == NULL);
        m_soundPlayerComponent = _soundPlayerComponent;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::unregisterUIMenuSoundPlayer(UIMenuSoundComponent * const _soundPlayerComponent)
    {
        ITF_ASSERT(m_soundPlayerComponent == _soundPlayerComponent);
        m_soundPlayerComponent = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::removeUIComponentOfTheMenu (UIComponent* _uiComponent)
    {
        if (_uiComponent)
        {
            for (ITF_VECTOR<UIMenu*>::iterator iter = m_menus.begin(); iter!=m_menus.end(); iter++)
            {
                UIMenu* pMenu = (*iter);
                if (pMenu && pMenu->getMenuID() == _uiComponent->getMenuID())
                {
                    pMenu->removeUIComponent(_uiComponent);
                    break;
                }
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent* UIMenuManager::getUIComponent (ObjectRef _ref)
    {
        const Actor* pActor = (const Actor*)GETOBJECT(_ref);
        if (pActor)
            return pActor->GetComponent<UIComponent>();
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    void UIMenuManager::addDebugMenu( DebugMenu* _menu )
    {
        m_DBGmenus.push_back(_menu);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::addDebugMenuCheat( DebugMenuEntry* _cheat )
    {
        m_debugMenuCheats->addEntry(_cheat);
    }
#endif // ITF_SUPPORT_CHEAT

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenu* UIMenuManager::getCurrentMenu ()
    {
        return getMenu(m_currentMenuID);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenu* UIMenuManager::getMenu (const StringID &_menuID)
    {
        for (ITF_VECTOR<UIMenu*>::iterator iter = m_menus.begin(); iter!=m_menus.end(); iter++)
        {
            UIMenu* pUIMenu = (*iter);
            if (_menuID == pUIMenu->getMenuID())
                return pUIMenu;
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::selectMenuItemWithDPad (u32 _button)
    {
        f32 joyX = 0.f;
        f32 joyY = 0.f;

        if (_button == m_joyButton_DPadL)
            joyX = -1.f;
        else if (_button == m_joyButton_DPadR)
            joyX = 1.f;
        else if (_button == m_joyButton_DPadU)
            joyY = -1.f;
        else if (_button == m_joyButton_DPadD)
            joyY = 1.f;

        selectMenuItem(joyX, joyY);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::selectMenuItemWithJoystick (float* _pos)
    {
        f32 joyX = _pos[0];
        f32 joyY = -_pos[1];

        selectMenuItem(joyX, joyY);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::selectMenuItem (f32 joyX, f32 joyY)
    {
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (pMenu)
        {
            ObjectRefList UIComponentsList = pMenu->getUIComponentsList();
            if (UIComponentsList.size() > 0 && (joyX !=0 || joyY !=0))
            {
                // Current component selected
                UIComponent* pUIComponentSelected = pMenu->getUIComponentSelected();
                if (pUIComponentSelected)
                {
                    if (handleMenuStartHorizontalNavigation(pMenu, pUIComponentSelected, joyX, joyY))
                        return;

                    Ray_OptionMenuHelper* optionHelper = Ray_OptionMenuHelper::getActiveHelper();
                    if (optionHelper)
                    {
                        ObjectRef overrideRef = optionHelper->getNavigationOverrideTarget(pUIComponentSelected, joyX, joyY);
                        if (overrideRef.isValid())
                        {
                            ObjectRef refComponentSelected = pUIComponentSelected->getUIref();
                            if (overrideRef != refComponentSelected)
                            {
                                UIComponent* newSel = getUIComponent(overrideRef);
                                if (newSel)
                                {
                                    applySelectionChange(pMenu, pUIComponentSelected, newSel);
                                    return;
                                }
                            }
                            else
                            {
                                return;
                            }
                        }
                    }

                    f32 angleJoy = atan2f(joyY, joyX);
                    ObjectRef refComponentSelected = pUIComponentSelected->getUIref();
                    ObjectRef refNextComponentSelected = findUIComponentToSelect(UIComponentsList,pUIComponentSelected,angleJoy);

                    // No UIComponent found so find the farthest (wrapping)
                    if (refNextComponentSelected == refComponentSelected)
                        refNextComponentSelected = findUIComponentToSelect(UIComponentsList,pUIComponentSelected,angleJoy,btrue);


                    if (refComponentSelected == refNextComponentSelected)
                        return;

                    UIComponent* oldSel = getUIComponent(refComponentSelected);
                    UIComponent* newSel = getUIComponent(refNextComponentSelected);
                    applySelectionChange(pMenu, oldSel, newSel);

                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ObjectRef UIMenuManager::findUIComponentToSelect (ObjectRefList _UIComponentsList, UIComponent* _pUIComponentSelected, f32 _angleJoy, const bbool _findFarthest)
    {
        ObjectRef refComponentSelected = _pUIComponentSelected->getUIref();
        ObjectRef refNextComponentSelected = refComponentSelected;
        Vec3d posComponentSelected = _pUIComponentSelected->GetActor()->getPos();

        f32 angleJoyComponent;
        if (_findFarthest)
            angleJoyComponent = 0.0f;
        else
            angleJoyComponent = F32_INFINITY;

        for (u32 i = 0; i < _UIComponentsList.size(); i++)
        {
            UIComponent* pUIComponent = getUIComponent(_UIComponentsList[i]);
            if (!pUIComponent || !pUIComponent->getActive() || !pUIComponent->getCanBeSelected())
                continue;

            ObjectRef refUIComponent = pUIComponent->getUIref();
            if (refUIComponent == refComponentSelected)
                continue;

            Vec3d posComponent = pUIComponent->GetActor()->getPos();
            Vec2d delta(posComponent.m_x - posComponentSelected.m_x,
                        posComponent.m_y - posComponentSelected.m_y);

            f32 angleComponents = atan2f(delta.m_y, delta.m_x);
            f32 sqDist = delta.sqrnorm();
            f32 angleJoyComponentTemp = angleComponents - _angleJoy;

            if (angleJoyComponentTemp < -MTH_PI)
                angleJoyComponentTemp += 2 * MTH_PI;
            if (angleJoyComponentTemp > MTH_PI)
                angleJoyComponentTemp -= 2 * MTH_PI;

            angleJoyComponentTemp = f32_Abs(angleJoyComponentTemp);
            f32 score = angleJoyComponentTemp * 60.0f + (sqDist / 5000.0f);

            if ((!_findFarthest && angleJoyComponentTemp < (MTH_PI / 2.5f) && score < angleJoyComponent) ||
                (_findFarthest && angleJoyComponentTemp > (MTH_PI / 1.5f) && score > angleJoyComponent))
            {
                angleJoyComponent = score;
                refNextComponentSelected = refUIComponent;
            }
        }

        return refNextComponentSelected;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    void UIMenuManager::updateSwapMenu()
    {
        u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();

        for ( u32 i = 0; i < numPlayers; i++ )
        {
            const PlayerIDInfo* player = GAMEMANAGER->getPlayerIDInfoFromPlayerIndex(i);
            String8 debugStr;

            debugStr.setTextFormat("Player %u: %s",i,player->getId().cStr());

            m_debugMenuSwapPlayers->setEntryName(debugStr.cStr(),i);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::displayDebugMenu()
    {
#ifdef ITF_WII
#define HEIGHT_START 15.0f
#define WIDTH_START 30.0f
#define HEIGHT_INCREASE 30.0f
#else
#define HEIGHT_START 100.0f
#define WIDTH_START 75.0f
#define HEIGHT_INCREASE 50.0f
#endif //ITF_WII

        f32 height = HEIGHT_START;
        f32 width   = WIDTH_START;

        f32 R = 1.f, G = .2f, B = .2f;
        f32 border = 5.0f;

        f32 widthByCol = 200.0f;
#if defined (ITF_PS3) || defined (ITF_X360)
        widthByCol = 400.0f;
#endif

        GFX_ADAPTER->draw2dBox(Vec2d(width-border, height-border),
            widthByCol + border *2,
            60.0f + border *2,
            0xBBcccccc, 0xBBcccccc,0xBBcccccc, 0xBBcccccc);

        String help;help.setTextFormat("X -> Cheat Mode %s", CHEATMANAGER->getActive() ? "ON" : "OFF");
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help = "Y -> Swap player";
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help = "RB / LB -> cycle menu pages";
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help.setTextFormat("Player index: %d", m_player);
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);

        height+=HEIGHT_INCREASE;

        m_DBGmenus[m_currentPageDBGMenu]->display(width,height,m_player);
    }
#endif // ITF_SUPPORT_CHEAT

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuManager::MenuPriority UIMenuManager::getHighestShownPriority()
    {
        for (int i=0; i<(int)MenuPriority_Count; i++)
            if (m_MenuStack[i].isValid())
                return (MenuPriority)i;
        return MenuPriority_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::showMenuPage (MenuPriority _priority, const StringID &_menuID, bbool _isVisible, MenuItemActionListener* _listener, bbool _previousMenu)
    {
        UIMenu* pMenu = getMenu(_menuID);
        ITF_ASSERT((int)_priority>=0 && _priority<MenuPriority_Count);

        // We dont need to auto select a field
        //if ( pMenu )
        //{
        //    pMenu->setAlwaysMenuSelected(btrue);
        //}

        if (_isVisible)
        {
            ITF_ASSERT(!m_MenuStack[_priority].isValid());
            m_MenuStack[(int)_priority] = _menuID;
        }
        else
        {
            ITF_ASSERT(m_MenuStack[_priority].isValid());
            ITF_ASSERT(m_MenuStack[_priority] == _menuID);
            m_MenuStack[_priority] = StringID::Invalid;
        }

        bbool wasFound=bfalse;
        ITF_UNUSED(wasFound);
        if (pMenu)
        {
            World * pWorld = (World*)GETOBJECT(pMenu->getWorldMenuRef());
            if (pWorld)
            {
                wasFound = btrue;
                if (_isVisible)
                {
                    m_menuItemActionMap[_menuID] = _listener;
                }

                pWorld->setAllowUpdate(_isVisible);
                pWorld->setAllowDraw(_isVisible);
                pWorld->setActive(_isVisible);
            }

            pMenu->enable();
        }

        MenuPriority newPriority =getHighestShownPriority();
        if (newPriority!=MenuPriority_Count)
        {
            m_currentMenuID = m_MenuStack[newPriority];
            m_inMenu = btrue;
        }
        else
        {
            m_currentMenuID = StringID::Invalid;
            m_inMenu = bfalse;
        }

        //propagate menu open or close to sound player
        if( _isVisible )
        {
            if( _previousMenu )
            {
                sendActionToSoundPlayer(_menuID, STRINGID_ACTION_MENU_OPEN, StringID::Invalid);
            }
            else
            {
                sendActionToSoundPlayer(_menuID, STRINGID_ACTION_MENU_OPEN_FIRST, StringID::Invalid);
            }
        }
        else
        {
            sendActionToSoundPlayer(_menuID, STRINGID_ACTION_MENU_CLOSE, StringID::Invalid);
        }

        {
            if (_menuID == kMenuStartID)
            {
                onMenuStartVisibilityChanged(pMenu, _isVisible);
            }
        }

        ITF_ASSERT_MSG(wasFound, "Menu was not found");
    }

    UIComponent* UIMenuManager::getUIComponentFromID(const StringID& menuID, const StringID& IDToFind)
    {
        UIMenu* pMenu = getMenu(menuID);
        if (!pMenu)
            return NULL;

        ObjectRefList UIComponentsList = pMenu->getUIComponentsList();
        u32 sizeList = UIComponentsList.size();
        for (u32 i = 0; i < sizeList; i++)
        {
            UIComponent* pComponent = getUIComponent ( UIComponentsList[i] );

            if (pComponent && pComponent->getID() == IDToFind)
            {
                return pComponent;
            }
        }

        return NULL;
    }

    UIComponent* UIMenuManager::getUIComponentFromFriendly(const StringID& menuID, const String8& friendlyToFind)
    {
        UIMenu* pMenu = getMenu(menuID);
        if (!pMenu)
            return NULL;

        ObjectRefList UIComponentsList = pMenu->getUIComponentsList();
        u32 sizeList = UIComponentsList.size();
        for (u32 i = 0; i < sizeList; i++)
        {
            UIComponent* pComponent = getUIComponent ( UIComponentsList[i] );

            if (pComponent && pComponent->getFriendly().equals(friendlyToFind, false))
            {
                return pComponent;
            }
        }

        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bool UIMenuManager::changeMenuItemActiveFlag (const StringID& menuID, const String8& friendlyToFind, bbool activeFlag )
    {
        UIComponent* pComponent = getUIComponentFromFriendly(menuID, friendlyToFind);
        if(pComponent)
        {
            if(!activeFlag && pComponent->getIsSelected())
            {
                selectMenuItem(0.0f, 1.0f);
            }
            pComponent->setActive(activeFlag);
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
	bool UIMenuManager::changeMenuItemFriendly (const StringID& menuID, const String8& friendlyToFind, const String8& newFriendly)
	{
        UIComponent* pComponent = getUIComponentFromFriendly(menuID, friendlyToFind);
        if(pComponent)
        {
            pComponent->forceContent(newFriendly);
            return true;
        }
        return false;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    bool UIMenuManager::changeMenuItemFriendlyByID (const StringID& menuID, const StringID& IDToFind, const String& newFriendly)
    {
        UIComponent* pComponent = getUIComponentFromID(menuID, IDToFind);
        if(pComponent)
        {
            pComponent->forceContent(newFriendly);
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    bbool UIMenuManager::setLineID (const StringID& menuID, const String8& friendlyToFind, LocalisationId val)
    {
        UIComponent* pComponent = getUIComponentFromFriendly(menuID, friendlyToFind);
        if(pComponent)
        {
            pComponent->setLineId(val);
            return btrue;
        }
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void UIMenuManager::applyMenuPressAction()
    {
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (pMenu)
        {
            // Get the current item selected in order to know the action to apply
            UIComponent* pUIComponent = pMenu->getUIComponentSelected();
            if (pUIComponent)
            {
                MenuItemActionListener *listener = getCurrentMenuActionListener();
                if (listener)
                    listener->onMenuItemPressAction(pUIComponent);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::applyMenuItemAction()
    {
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (pMenu)
        {
            // Get the current item selected in order to know the action to apply
            UIComponent* pUIComponent = pMenu->getUIComponentSelected();
            if (pUIComponent)
            {
                StringID menuSonID = pUIComponent->getMenuSonID();
                MenuItemActionListener *listener = getCurrentMenuActionListener();

                if (menuSonID != StringID::Invalid)
                {
                    m_menuTree.push_back(m_currentMenuID); // Push it to save the hierarchy
                    MenuPriority priority = getHighestShownPriority();
                    ITF_ASSERT(priority!=MenuPriority_Count);

                    showMenuPage(priority,m_currentMenuID, bfalse); // Hide the current menu
                    showMenuPage(priority,menuSonID, btrue, listener); // Show its son
                    UIMenu* pMenu = getMenu(menuSonID);
                    if(pMenu)
                    {
                        pMenu->reinitMenuSelection();
                    }
                }

                if (listener)
                    listener->onMenuItemAction(pUIComponent);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIMenuManager::applyMenuItemOtherAction(const StringID& _action)
    {
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (pMenu)
        {
            UIComponent* pUIComponent = pMenu->getUIComponentSelected();
            if (pUIComponent)
            {
                MenuItemActionListener *listener = getCurrentMenuActionListener();
                if (listener)
                {
                    return listener->onMenuItemOtherAction(pUIComponent, _action);
                }
            }
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    UIMenuManager::MenuItemActionListener* UIMenuManager::getCurrentMenuActionListener()
    {
        MenuItemActionMap::const_iterator callback = m_menuItemActionMap.find(m_currentMenuID);
        if (callback != m_menuItemActionMap.end())
            return callback->second;
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::showPreviousMenu()
    {
        if (m_menuTree.size() > 0)
        {
            MenuPriority priority = getHighestShownPriority();
            ITF_ASSERT(priority!=MenuPriority_Count);

            MenuItemActionListener *listener = getCurrentMenuActionListener();
            if(listener)
                listener->onMenuHideAction();

            showMenuPage(priority, m_currentMenuID, bfalse); // Hide the current menu
            UIMenu* pMenu = getMenu(m_currentMenuID);
            if(pMenu)
            {
                pMenu->reinitMenuSelection();
            }

            u32 treeSize = m_menuTree.size();
            showMenuPage(priority, m_menuTree[treeSize - 1], btrue, listener, btrue); // Show its father
            m_menuTree.pop_back();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::hideMenu (MenuPriority _priority)
    {
        ITF_ASSERT(_priority>=0 && _priority<MenuPriority_Count);
        if (_priority<0 || _priority>=MenuPriority_Count)
        {
            return;
        }

        StringID menuId = m_MenuStack[_priority];
        showMenuPage(_priority, menuId, bfalse);

        // reinitialize selection
        UIMenu* pMenu = getMenu(menuId);
        if(pMenu)
        {
            pMenu->reinitMenuSelection();
        }
        for(u32 i = 0; i < m_menuTree.size(); i++)
        {
            UIMenu* pMenuTree = getMenu(m_menuTree[i]);
            if(pMenuTree)
            {
                pMenuTree->reinitMenuSelection();
            }
        }
        m_menuTree.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    String8 UIMenuManager::getMenuBaseNameFromPath (String8 _menuPath)
    {
        i32 indexStart;
        _menuPath.strstr("/", btrue, &indexStart);
        indexStart++;
        // path.getLen() - 4: to remove .isc
        return _menuPath.substr(indexStart, (_menuPath.getLen() - 4) - indexStart);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    void DebugMenu::up()
    {
        m_selection = (m_entries.size() + m_selection - 1)%m_entries.size();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenu::down()
    {
        m_selection = (m_selection+1)%m_entries.size();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenu::setEntryName( const char* _name, u32 _index )
    {
        m_entries[_index]->setName(_name);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenu::addEntry(DebugMenuEntry* _entry)
    {
        m_entries.push_back(_entry);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenu::display(f32 x, f32 y, u32 _player)
    {
        f32 border = 5.0f;
        f32 height = (f32)y;
        f32 R,G,B;
        f32 xoffset = 0.0f;

        f32 heightByLine = 15.0f;
        f32 widthByCol = 200.0f;
#if defined (ITF_PS3) || defined (ITF_X360)
        widthByCol = 400.0f;
#endif

        u32 maxEntries = (u32)(( GFX_ADAPTER->getScreenHeight() - y ) / heightByLine) - 2;
#ifdef ITF_WII
        // To avoid invisible line behind FPS and MEM display
        if(maxEntries > 2) maxEntries -= 2;
#endif // ITF_WII

        GFX_ADAPTER->draw2dBox(Vec2d(x-border, y-border),
            ((m_entries.size() / maxEntries)+1) * widthByCol + border *2,
            f32_Min((f32)m_entries.size(), (f32)maxEntries) * heightByLine + border *2,
            0xBBcccccc, 0xBBcccccc,0xBBcccccc, 0xBBcccccc);

        for (u32 i = 0; i < m_entries.size(); ++i)
        {
            if (i == m_selection)
            {
                R = 0.9f; G = .4f; B = .4f;
            }
            else
            {
                R = 0.7f; G = .1f; B = .1f;
            }
            GFX_ADAPTER->drawDBGText(m_entries[i]->getName(_player),x+xoffset,height,R,G,B);
            height += heightByLine;

            if((i+1)%maxEntries == 0)
            {
                xoffset+= widthByCol;
                height = y;
            }
        }
    }
#endif // ITF_SUPPORT_CHEAT

    void UIMenuManager::Receive( u32 player, f32 axis, const StringID& action )
    {
        if (SYSTEM_ADAPTER->isOSUIActive())
            return;

        UIMenu* pMenu = getMenu(m_currentMenuID);
        if(!pMenu) return ;

        if(!pMenu->isEnable())
            return;

        const bbool skipValidation = IsFreeAccessMenu(m_currentMenuID);
        if (!skipValidation)
        {
            u32 mainIndex   = ( GAMEMANAGER->getMainIndexPlayer_Internal() );
            u32 pauseOwner  = ( GAMEMANAGER->getIndexPauseOwner() );

            // First case: we have to check the pad is allowed to access the menu
            if ( pMenu->isPadAllowed(player) == bfalse )
            {
                return ;
            }
            // Second case: Test if the current menu is only available for a specific player
            if ( pMenu->getAllowedIndexPadOnly() != U32_INVALID )
            {
                if ( pMenu->getAllowedIndexPadOnly() != player )
                    return ;
            }
            // Third case: we have to check the pause menu and the main menu
            // and they are only available for a specific player too (main player or pause owner)
            else
            {
                if( mainIndex != U32_INVALID && mainIndex != player)
                {
                    // MainMenu ?
                    if ( !GAMEMANAGER->isPlayableScreen() )
                        return ;
                }

                if ( pauseOwner != player )
                {
                    // Owner not set yet
                    if ( GAMEMANAGER->isInPause() && pauseOwner == U32_INVALID )
                    {
                        // Not an active player??
                        if ( GAMEMANAGER->getActiveAndPersistentPlayer(player) == NULL )
                        {
                            return;
                        }

                        GAMEMANAGER->setIndexPauseOwner(player);
                    }

                    // Pause menu ?
                    else if ( GAMEMANAGER->isPlayableScreen() && !TRC_ADAPTER->isDisplayingError())
                        return ;
                }

                // First menu validated is the press start button
                if ( (action == input_actionID_Valid || action == input_actionID_Start) && mainIndex == U32_INVALID )
                {
#ifdef ITF_WII
                    if(player!=0)    return ;
#endif //ITF_WII

                    GAMEMANAGER->setMainIndexPlayer(player);
                }
            }
        }

        //send action to sound players
        StringID selectedElement = StringID::Invalid;
        StringID previousMenu = m_currentMenuID;
        StringID menuAction = action;
        if( pMenu->getUIComponentSelected() )
        {
            selectedElement = pMenu->getUIComponentSelected()->getID();
        }

        bbool actionEffect = bfalse;
        if(m_inMenu
#ifdef ITF_SUPPORT_CHEAT
            && !m_inDBGMenu
#endif // ITF_SUPPORT_CHEAT
            )
        {
            actionEffect = applyGamePadButton(action);
        }

        if( menuAction == input_actionID_DeleteSave && !actionEffect )
        {
            menuAction = input_actionID_DeleteSaveNoEffect;
        }
        sendActionToSoundPlayer(previousMenu, menuAction, selectedElement);
        //check if menu selection has changed
        if(pMenu && previousMenu == m_currentMenuID )
        {
            if( pMenu->getUIComponentSelected() )
            {
                StringID newSelectedElement = pMenu->getUIComponentSelected()->getID();
                if( selectedElement != newSelectedElement )
                {
                    sendActionToSoundPlayer(m_currentMenuID, STRINGID_ACTION_MENU_SELECTION_CHANGE, selectedElement);
                }
            }
        }
    }

#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)

    UIComponent* UIMenuManager::getUIComponentUnderMouse()
    {
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (!pMenu)
        {
            return nullptr;
        }
        i32 x;
        i32 y;
        AABB bb;
        INPUT_ADAPTER->getMousePos(x, y);
        const Vec3d mousePos(static_cast<float>(x), static_cast<float>(y), 0.f);
        ObjectRefList components = pMenu->getUIComponentsList();
        i32 size = components.size();

        UIComponent* bestComponent = nullptr;
        f32 bestZ = -999999.0f;

        for (int i = size - 1; i >= 0; --i)
        {
            UIComponent* comp = getUIComponent(components[i]);
            if (comp && comp->getActive() && comp->getCanBeSelected())
            {
                Actor* actor = comp->GetActor();
                if (!actor)
                    continue;

                bbool isOptionComponent = comp->IsClassCRC(UIGameOptionComponent::GetClassCRCStatic());

                if (isOptionComponent)
                {
                    AABB actorAABB = actor->getRelativeAABB();
                    if (actorAABB.isValid())
                    {
                        Vec3d actorPos = actor->getPos();
                        Vec2d min(actorPos.m_x + actorAABB.getMin().m_x, actorPos.m_y + actorAABB.getMin().m_y);
                        Vec2d max(actorPos.m_x + actorAABB.getMax().m_x, actorPos.m_y + actorAABB.getMax().m_y);
                        bb = AABB(Vec3d(min.m_x, min.m_y, 0.f), Vec3d(max.m_x, max.m_y, 0.f));
                    }
                    else
                    {
                        comp->get2DBoundingBox(bb);
                    }

                    if (bb.isValid())
                    {
                        const Vec2d& min = bb.getMin();
                        const Vec2d& max = bb.getMax();
                        f32 width = max.m_x - min.m_x;
                        f32 height = max.m_y - min.m_y;

                        const f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
                        const f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
                        const f32 maxAllowedWidth = (screenWidth > 0.0f ? screenWidth * 1.25f : 99999.0f);
                        const f32 maxAllowedHeight = (screenHeight > 0.0f ? screenHeight * 1.25f : 99999.0f);

                        if (width > maxAllowedWidth || height > maxAllowedHeight)
                        {
                            continue;
                        }
                    }
                }
                else
                {
                    comp->get2DBoundingBox(bb);
                }

                if (bb.isValid() && bb.contains(Vec3d(mousePos)))
                {
                    Vec3d pos = actor->getBoundLocalPos();
                    if (pos.m_z > bestZ)
                    {
                        bestZ = pos.m_z;
                        bestComponent = comp;
                    }
                }
            }
        }

        return bestComponent;
    }

    bbool UIMenuManager::IsCurrentMenuVerticalScrolling() const
    {
        const u32 menuValue = m_currentMenuID.GetValue();
        // ROE-711: main menu has vertical + horizontal now
        return menuValue == PROFILE_SELECTION_MENU_ID /* || menuValue == MAIN_MENU_MENU_ID*/;
    }

    bbool UIMenuManager::onMouseButton( InputAdapter::MouseButton but, InputAdapter::PressStatus status )
	{
		switch (status)
		{
		case InputAdapter::Pressed:
#ifndef ITF_FINAL
			if (but == InputAdapter::MB_Middle)
			{
				m_mouseLeftInput = !m_mouseLeftInput;
			}
#endif //ITF_FINAL
			if (but != InputAdapter::MB_Left)
			{
				break;
			}
			if (m_mouseLocked)
			{
				break;
			}
			m_mouseLocked = true;
			if (but == InputAdapter::MB_Left)
			{
				UIMenu* pMenu = getMenu(m_currentMenuID);
				if (!pMenu)
				{
					break;
				}
				if (getUIComponentUnderMouse() == pMenu->getUIComponentSelected())
				{
					u32 pauseOwner = GAMEMANAGER->getIndexPauseOwner();
					if (pauseOwner == U32_INVALID)
					{
						pauseOwner = 0;
					}
					Receive(pauseOwner, 1.f, input_actionID_onPressed);
				}
			}
			break;

		case InputAdapter::Released:
#ifndef ITF_FINAL
			if (!m_mouseLeftInput)
			{
				break;
			}
#endif //ITF_FINAL
			if (!m_mouseLocked)
			{
				break;
			}
			m_mouseLocked = false;
			if (but != InputAdapter::MB_Left)
			{
				break;
			}
			UIComponent* componentUnderMouse = getUIComponentUnderMouse();
			UIMenu* pMenu = getMenu(m_currentMenuID);
			if (!pMenu)
			{
				break;
			}
			UIComponent* selectedComponent = pMenu->getUIComponentSelected();
			if (IsCurrentMenuVerticalScrolling())
			{
				// in this menu, scroll up/down to the selected menu before applying the action because the elements move when highlighted
				float scrollDirectionX = 0.f;
				float scrollDirectionY = -1.f;
				ObjectRefList components = pMenu->getUIComponentsList();
				i32 size = components.size();
				for (int i = 0; i < size; ++i)
				{
					if (pMenu->getUIComponentSelected() == getUIComponent(components[i]))
					{
						scrollDirectionX = 0.f;
						scrollDirectionY = 1.f;
					}
				}
				if (!componentUnderMouse)
				{
					break;
				}

                // ROE-711: there shouldn't be infinite looping in UI logic, in case we add an extra component
                for (int i = 0; i < 1000 && (pMenu->getUIComponentSelected() != componentUnderMouse); i++)
				{
					selectMenuItem(scrollDirectionX, scrollDirectionY);
				}
			}
			else if (!componentUnderMouse)
			{
				if (!selectedComponent)
				{
					break;
				}
				componentUnderMouse = selectedComponent;
			}
			else if (!selectedComponent || (selectedComponent != componentUnderMouse && selectedComponent->IsClassCRC(UISliderComponent::GetClassCRCStatic()) == false))
			{
				break;
			}
			u32 pauseOwner = GAMEMANAGER->getIndexPauseOwner();
			if (pauseOwner == U32_INVALID)
			{
				pauseOwner = 0;
			}
			Receive(pauseOwner, 1.f, input_actionID_Valid);
			Receive(pauseOwner, 1.f, input_actionID_Start);
			Receive(pauseOwner, 1.f, input_actionID_onReleased);
			break;

		}
		return btrue;
	}

	bbool UIMenuManager::onMousePos (i32 x, i32 y)
	{

        if (m_mouseLocked)
        {
            return true;
        }

        if (IsCurrentMenuVerticalScrolling())
        {
            return true;
        }

        UIComponent* componentUnderMouse = getUIComponentUnderMouse();
        UIMenu* pMenu = getMenu(m_currentMenuID);
        if (!pMenu)
        {
            return true;
        }

        UIComponent* selectedComponent = pMenu->getUIComponentSelected();
        if (selectedComponent == componentUnderMouse)
        {
            return btrue;
        }

        if (componentUnderMouse)
        {
            if (selectedComponent)
            {
                selectedComponent->setIsSelected(bfalse);
                selectedComponent->onRollout();
                if (MenuItemActionListener *listener = getCurrentMenuActionListener())
                    listener->UpdateMenuOnSelectionChange(selectedComponent, false);
                notifySiblingUIComponents(selectedComponent, bfalse);
            }

            componentUnderMouse->setIsSelected(btrue);
            componentUnderMouse->onRollover();
            if (MenuItemActionListener *listener = getCurrentMenuActionListener())
                listener->UpdateMenuOnSelectionChange(componentUnderMouse, true);
            notifySiblingUIComponents(componentUnderMouse, btrue);

            sendActionToSoundPlayer(m_currentMenuID, STRINGID_ACTION_MENU_SELECTION_CHANGE, selectedComponent->getID());
        }
        return true;
	}

	bbool UIMenuManager::onMouseWheel (i32 wheelValue, i32 wheelDelta)
	{
		if (IsCurrentMenuVerticalScrolling())
		{
			f32 joyX = 0.f;
			f32 joyY = static_cast<float>(-wheelDelta);
			selectMenuItem(joyX, joyY);
			UIMenu* pMenu = getMenu(m_currentMenuID);
			if (!pMenu)
			{
				return true;
			}
			UIComponent* selectedComponent = pMenu->getUIComponentSelected();
			sendActionToSoundPlayer(m_currentMenuID, STRINGID_ACTION_MENU_SELECTION_CHANGE, selectedComponent->getID());
		}
		return btrue;
	}

	bbool UIMenuManager::onKey ( i32 key, InputAdapter::PressStatus status )
	{
		return true;
	}
#endif // ITF_WINDOWS && ITF_FINAL

    void UIMenuManager::setAllowedPadIndexOnly( const StringID& menuID, u32 padIndex )
    {
        UIMenu* pMenu = getMenu(menuID);
        if(pMenu)
        {
            pMenu->setAllowedIndexPadOnly(padIndex);
        }
    }
    void UIMenuManager::setAllowedPadTypeOnly( const StringID& menuID, InputAdapter::PadType padType )
    {
        UIMenu* pMenu = getMenu(menuID);
        if(pMenu)
        {
            pMenu->setPadTypeAllowed((u32)padType);
        }
    }

    void UIMenuManager::enableMenu( const StringID& menuID )
    {
        UIMenu* pMenu = getMenu(menuID);
        if(pMenu)
        {
            pMenu->enable();
        }
    }

    void UIMenuManager::disableMenu( const StringID& menuID )
    {
        UIMenu* pMenu = getMenu(menuID);
        if(pMenu)
        {
            pMenu->disable();
        }
    }

    ITF::bbool UIMenuManager::isEnable( const StringID& menuID )
    {
        UIMenu* pMenu = getMenu(menuID);
        if(pMenu)
        {
            return pMenu->isEnable();
        }
        return bfalse;
    }

    void UIMenuManager::setMenuListener(const StringID& menuID, MenuItemActionListener* listener)
    {
        m_menuItemActionMap[menuID] = listener;
    }

    void UIMenuManager::sendActionToSoundPlayer(const StringID & _menu, const StringID & _action, const StringID & _selection) const
    {
        if( m_soundPlayerComponent )
        {
            m_soundPlayerComponent->onAction(_menu, _action, _selection);
        }
    }

    void UIMenuManager::stopMenuSound()
    {
        if( m_soundPlayerComponent )
        {
            m_soundPlayerComponent->stopAllSounds();
        }
    }

    void UIMenuManager::rememberMainSelectionForMenuStart(UIComponent* oldSel, UIComponent* newSel, UIMenu* menu)
    {
        if (!menu || menu->getMenuID() != kMenuStartID)
            return;

        if (newSel && newSel->getID() == kUbiConnectID)
        {
            if (oldSel && oldSel->getID() != kUbiConnectID)
            {
                m_menuStartLastMainSelectionID = oldSel->getID();
            }
        }
        else if (newSel && newSel->getID() != kUbiConnectID)
        {
            m_menuStartLastMainSelectionID = newSel->getID();
        }
    }

    bbool UIMenuManager::handleMenuStartHorizontalNavigation(UIMenu* menu, UIComponent* selected, f32 joyX, f32 joyY)
    {
        if (!menu || menu->getMenuID() != kMenuStartID || !selected)
            return bfalse;

        const f32 absJoyX = f32_Abs(joyX);
        const f32 absJoyY = f32_Abs(joyY);
        const StringID currentComponentId = selected->getID();

        if (joyX < 0.0f && absJoyX >= absJoyY && currentComponentId != kUbiConnectID)
        {
            UIComponent* ubisoftComponent = menu->getUIComponentByID(kUbiConnectID);
            if (ubisoftComponent && ubisoftComponent->getActive() && ubisoftComponent->getCanBeSelected())
            {
                applySelectionChange(menu, selected, ubisoftComponent);
                return btrue;
            }
        }

        else if (joyX > 0.0f && absJoyX >= absJoyY && currentComponentId == kUbiConnectID)
        {
            UIComponent* targetComponent = NULL;
            if (m_menuStartLastMainSelectionID.isValid())
            {
                targetComponent = menu->getUIComponentByID(m_menuStartLastMainSelectionID);
                if (!(targetComponent && targetComponent->getActive() && targetComponent->getCanBeSelected()))
                {
                    targetComponent = NULL;
                }
            }
            if (targetComponent)
            {
                applySelectionChange(menu, selected, targetComponent);
                return btrue;
            }
        }

        return bfalse;
    }

    void UIMenuManager::onMenuStartVisibilityChanged(UIMenu* pMenu, bbool isVisible)
    {
        if (!pMenu)
        {
            m_menuStartLastMainSelectionID = StringID::Invalid;
            return;
        }

        if (isVisible)
        {
            UIComponent* selectedComponent = pMenu->getUIComponentSelected();
            if (selectedComponent && selectedComponent->getID() != kUbiConnectID)
                m_menuStartLastMainSelectionID = selectedComponent->getID();
            else
                m_menuStartLastMainSelectionID = StringID::Invalid;
        }
        else
        {
            m_menuStartLastMainSelectionID = StringID::Invalid;
        }
    }
}
