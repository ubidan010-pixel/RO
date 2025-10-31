#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#include "rayman/gameplay/GameScreens/Ray_Pause_MenuItemActionListener.h"
#endif //_ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Pause_MenuItemActionListener::pause_resumeGame()
    {
        if (RAY_GAMEMANAGER->isInPause())
        {
            RAY_GAMEMANAGER->leavePause();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Pause_MenuItemActionListener::pause_showMainMenu()
    {
        RAY_GAMEMANAGER->postGameScreenChange<Ray_GameScreen_MainMenu>(bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Pause_MenuItemActionListener::pause_exitGame()
    {
        SYSTEM_ADAPTER->requestExit();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID Ray_Pause_MenuItemActionListener::onMenuPageAction(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction)
    {
        return UI_MENUMANAGER->onMenuPageAction_DefaultRootMenu(_menu, _action, _defaultAction);
    }

    void Ray_Pause_MenuItemActionListener::onMenuHideAction ( )
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Pause_MenuItemActionListener::onMenuItemAction (UIComponent* _UIComponent)
    {
        if (!UI_MENUMANAGER->canExecuteMenuItemAction())
            return ;

        const StringID id = _UIComponent->getID();  
        
        const StringID mainMenu = ITF_GET_STRINGID_CRC(MAINMENU, 244070065);
        const StringID resume = ITF_GET_STRINGID_CRC(RESUME, 2819602867);
        const StringID confirmYes = ITF_GET_STRINGID_CRC(CONFIRM_YES, 3118550926);
        const StringID exitHomeMap = ITF_GET_STRINGID_CRC(exitHomeMap, 1273950691);
        const StringID confirmNo = ITF_GET_STRINGID_CRC(CONFIRM_NO, 893466327);

        if(id == confirmYes)
        {
            if(m_previousID == mainMenu)
            {
                pause_saveAndExit();
            }
        }
        else if (id == confirmNo)
        {
            UI_MENUMANAGER->showPreviousMenu();
        }
        else if (id == resume)
        {
            pause_resumeGame();
        }
        else if(id == exitHomeMap)
        {
            if (RAY_GAMEMANAGER->isInPause())
            {
                SYSTEM_ADAPTER->setSystemUnPauseFlag(btrue);
            }

            // RO-15886 & RO-15268: player gets stuck on the world map as the
            // change page sequence can be playing at this point and would never
            // get stopped
            RAY_GAMEMANAGER->stopAllSequences(); 
            GAMEMANAGER->updateRichPresenceForActivePlayers(PRESENCE_GAMESCREEN_INWORLDMAP);
            RAY_GAMEMANAGER->triggerChangeRoomSequence(bfalse);

            pause_resumeGame();
        }

        m_previousID = id;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Pause_MenuItemActionListener::pause_saveAndExit()
    {
        RAY_GAMEMANAGER->saveGameState(bfalse, RAY_GAMEMANAGER->getCurrentSlotIndex());
        RAY_GAMEMANAGER->backToTheMainMenuASAP(btrue);
    }
}