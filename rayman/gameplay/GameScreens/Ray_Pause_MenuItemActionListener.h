#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#define _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF
{
    class TRCMessage_Base;
    class Ray_Pause_MenuItemActionListener : public UIMenuManager::MenuItemActionListener
    {
    public:
        virtual void onMenuItemAction (UIComponent* _UIComponent);
        virtual void onMenuHideAction ( );
        virtual StringID onMenuPageAction(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction);

    protected :

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show the world map screen
        virtual void pause_showWorldMap() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add a second player in the game
        virtual void pause_addPlayer() {}
    
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Resume the game
        virtual void pause_resumeGame();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show the main menu screen
        virtual void pause_showMainMenu();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Exit the game
        virtual void pause_exitGame();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save and exit to main menu
        void pause_saveAndExit();

        ///////////////////////////////////////////////////////////////////////////////////////////
        StringID    m_previousID;
    };
}

#endif