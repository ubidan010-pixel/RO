#ifndef _ITF_UIMENUMANAGER_H_
#define _ITF_UIMENUMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{
    class UIComponent;
    class UIMenu;
    class UIMenuSoundComponent;

    ///////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_CHEAT
    class DebugMenuEntry
    {
    public:
        DebugMenuEntry( const String & _name) : m_name(_name){}
        virtual ~DebugMenuEntry() = default;
        virtual void select(u32 _player) const {}
        virtual const String getName(u32 _player) const { return m_name; }
        void setName( const char* _name ) { m_name = _name; }
    protected:
        String m_name;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenu
    {
    public:
        DebugMenu() : m_selection(0){}
        virtual ~DebugMenu()
        {
            for (ITF_VECTOR<DebugMenuEntry* >::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
                if (*it)
                    delete *it;
            m_entries.clear();
        }
        virtual void display(f32 x, f32 y, u32 _player);
        void up();
        void down() ;
        void setEntryName( const char* _name, u32 _index );
        virtual void select(u32 _player = 0){ m_entries[m_selection]->select(_player);}
        virtual void addEntry(DebugMenuEntry * _entry);
    protected:
        u32 m_selection;
        ITF_VECTOR<DebugMenuEntry* > m_entries;

    };
#endif // ITF_SUPPORT_CHEAT

    ///////////////////////////////////////////////////////////////////////////////////////////
#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)
    class UIMenuManager : public IInputListener, public Interface_InputListener
#else
    class UIMenuManager final : public IInputListener
#endif
    {
        friend class UIMenu;

    public:
        ///////////////////////////////////////////////////////////////////////////////////////////////
        class MenuItemActionListener
        {
        public:
            ///////////////////////////////////////////////////////////////////////////////////////////
            /// this callback is run when the user activates a menu entry
            /// @param _UIComponent the menu entry the user activated
            virtual void onMenuItemAction (UIComponent* _UIComponent) = 0;

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// this callback is run when the users does an action which is related to a whole menu page
            /// @param _menu the current menu page
            /// @param _key the key which caused the action
            /// @param _defaultAction default action, among predefined actions (see getMenuPageAction_*() )
            virtual StringID onMenuPageAction(UIMenu * /*_menu*/, const StringID & /*_key*/, const StringID &_defaultAction) {return _defaultAction;}

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// this callback is run when the user hides a menu entry (like with a "back" action
            virtual void onMenuHideAction ( ) {}


            ///////////////////////////////////////////////////////////////////////////////////////////
            /// this callback is run each time another button than valid or back is pressed/released
            virtual bbool onMenuItemOtherAction(UIComponent* _UIComponent, const StringID & _action) { return bfalse; }

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// this callback is run when the user press a button
            /// @param _UIComponent the menu entry the user activated
            virtual void onMenuItemPressAction (UIComponent* _UIComponent) {}
            virtual void UpdateMenuOnSelectionChange(UIComponent* uiComponent, bbool isSelected) {}
            virtual bbool isBaseMenuHelper() const { return bfalse; }
        };

		virtual void Receive( u32 player, f32 axis, const StringID& action ) ;
#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)
                UIComponent* getUIComponentUnderMouse();
        bbool IsCurrentMenuVerticalScrolling() const;
        bbool IsMouseInputAllowed() const;
                virtual bbool onMouseButton( InputAdapter::MouseButton but, InputAdapter::PressStatus status );
                virtual bbool onMousePos ( i32 x, i32 y );
                virtual bbool onMouseWheel ( i32 wheelValue, i32 wheelDelta );
                virtual bbool onKey ( i32 key, InputAdapter::PressStatus status );
#endif // ITF_WINDOWS && ITF_FINAL

        //
        enum MenuPriority
        {
            MenuPriority_Critical=0,
            MenuPriority_Highest,
            MenuPriority_High,
            MenuPriority_Normal,
            MenuPriority_Low,
            MenuPriority_Lowest,
            MenuPriority_Notification,

            MenuPriority_Count,//RESERVED. Must be last of enum
            ENUM_FORCE_SIZE_32(0)
        };


        //////////////////////////////////////////////////////////////////////////////////////////////
        UIMenuManager();
        ~UIMenuManager();

        void                        addMenu             (String8 _menuBaseName);
        UIMenu*                     getMenu             (const StringID &_menuID);
        void                        removeUIComponentOfTheMenu (UIComponent* _uiComponent);
        void                        update              (       );


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add an UIComponent to its menu
        /// @param _ref reference of the UIComponent
        void                        addUIComponentToMenu(ObjectRef _ref);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Register menu sound player component to this manager
        /// @param _soundPlayerComponent ptr to the UIMenuSoundComponent
        void                        registerUIMenuSoundPlayer(UIMenuSoundComponent * const _soundPlayerComponent);
        void                        unregisterUIMenuSoundPlayer(UIMenuSoundComponent * const _soundPlayerComponent);
        void                        stopMenuSound();


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show/hide a menu
		/// @param _priority the priority of the menu
        /// @param _menuID id of the menu
        /// @param _isVisible true if the menu must be displayed
        /// @param _listener callback function
        void                       showMenuPage        (MenuPriority _priority, const StringID &_menuID, bbool _isVisible, MenuItemActionListener* _listener = NULL, bbool _previousMenu = bfalse);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show the father of the current menu
        void                       showPreviousMenu    (       );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Hide the menu and delete its saved hierarchy
        void                        hideMenu            (  MenuPriority _priority   );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check if a menu is displayed
        bbool                       getIsInMenu         (       ) const { return m_inMenu;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check if a menu is displayed for given priority
        bbool                       isDisplayingPriority( MenuPriority _priority ) const { return  m_MenuStack[(int)_priority].isValid();}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check if the debug menu is displayed
#ifdef ITF_SUPPORT_CHEAT
        bbool                       getIsInDBGMenu      (       ) const { return m_inDBGMenu; }
#endif // ITF_SUPPORT_CHEAT
        bbool                       getIsGammaSettingsMenu() const {  return m_inGammaSettingsMenu; }
        void                        setIsGammaSettingsMenu(bbool value) { m_inGammaSettingsMenu = value;}

#ifdef ITF_SUPPORT_CHEAT
        void                        setIsInDBGMenu      (bbool _isInDBGMenu, u32 _player) { m_inDBGMenu = _isInDBGMenu; m_currentPageDBGMenu = 0; m_player = m_inDBGMenu ? _player : U32_INVALID; }
        void                        displayDebugMenu    (       );
#endif // ITF_SUPPORT_CHEAT
        void                        setIDCurrentMenu    (const StringID &_menuID) { m_currentMenuID = _menuID;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Return the base name of the menu given by its path
        /// @param _menuPath path of the menu
        String8                     getMenuBaseNameFromPath (String8 _menuPath);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the stringID related to the ShowPreviousMenu action
        const StringID&             getMenuPageAction_ShowPreviousMenu() {return m_menuAction_ShowPreviousMenu;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the stringID related to the hideMenu action
        const StringID&             getMenuPageAction_HideMenu() {return m_menuAction_HideMenu;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the stringID related to the ApplyMenuItemAction action
        const StringID&             getMenuPageAction_ApplyMenuItemAction() {return m_menuAction_ApplyMenuItemAction;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the stringID related to the nothing action
        const StringID&             getMenuPageAction_Nothing() {return m_menuAction_Nothing;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// default implementation of menu page actions for root menus. This is a helper
        /// @param _menu the menu page
        /// @param _key the key which caused the action
        /// @param _defaultAction the default action (given by the caller)
        /// @return the action you want to run, or an empty StringID
        StringID                    onMenuPageAction_DefaultRootMenu(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// default implementation of menu page actions for 1 button-only menus (eg A on XBOX). This is a helper
        /// @param _menu the menu page
        /// @param _key the key which caused the action
        /// @param _defaultAction the default action (given by the caller)
        /// @return the action you want to run, or an empty StringID
        StringID                    onMenuPageAction_Default1ButtonMenu(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction);

#ifdef ITF_SUPPORT_CHEAT
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Adds a debug menu. Automatically deleted.
        void                        addDebugMenu( DebugMenu* _cheat );
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Adds a debug menu in Cheat page. Automatically deleted.
        void                        addDebugMenuCheat( DebugMenuEntry* _cheat );
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add an entry to the MAPS page of the debug menu. Automatically deleted.
        /// @param _entry The debug menu entry
        void                        addDebugMenuMap (DebugMenuEntry *_entry);
#endif // ITF_SUPPORT_CHEAT

        bbool                       canExecuteMenuItemAction    (       )   { return m_canExecuteMenuItemAction; }
		bool						changeMenuItemFriendly (const StringID& menuID, const String8& friendlyToFind, const String8& newFriendly) ;
        bool						changeMenuItemFriendlyByID (const StringID& menuID, const StringID& IDToFind, const String& newFriendly) ;
        bool						changeMenuItemActiveFlag (const StringID& menuID, const String8& friendlyToFind, bbool activeFlag) ;
        bbool                       setLineID(const StringID& menuID, const String8& friendlyToFind, LocalisationId val) ;

        StringID                    getCurrentMenuID  (        ) { return m_currentMenuID; }
        void                        setAllowedPadIndexOnly(const StringID& menuID, u32 padIndex);
        void                        setAllowedPadTypeOnly(const StringID& menuID, InputAdapter::PadType padType);

        void                        enableMenu(const StringID& menuID);
        void                        disableMenu(const StringID& menuID);
        bbool                       isEnable(const StringID& menuID);
        void                        setMenuListener(const StringID& menuID, MenuItemActionListener* listener);

        static UIComponent*         getUIComponent          (ObjectRef _ref);
        void                        applySelectionChange    (UIMenu* menu, UIComponent* oldSel, UIComponent* newSel);
        UIComponent*                getUIComponentFromFriendly(const StringID& menuID, const String8& friendlyToFind);
        UIComponent*                getUIComponentFromID(const StringID& menuID, const StringID& IDToFind);
        MenuItemActionListener*     getCurrentMenuActionListener();
        static void                 notifySiblingUIComponents(UIComponent* component, bbool isSelected);
    private:
        void                        rememberMainSelectionForMenuStart(UIComponent* oldSel, UIComponent* newSel, UIMenu* menu);
        bbool                       handleMenuStartHorizontalNavigation(UIMenu* menu, UIComponent* selected, f32 joyX, f32 joyY);
        void                        onMenuStartVisibilityChanged(UIMenu* pMenu, bbool isVisible);
    protected:
#ifdef ITF_SUPPORT_CHEAT
        void                        updateDBGMenuStatus     ();
#endif // ITF_SUPPORT_CHEAT
        UIMenu*                     getCurrentMenu          (       );
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Search which component must be selected
        /// @param _UIComponentsList list of the components
        /// @param _pUIComponentSelected current component selected in this list
        /// @param _angleJoy angle of the joy
        /// @param _findFarthest true if we must find the UIComponent the farthest of the UIComponent selected (for menu wrapping)
        ///                      by default we are looking for the nearest
        ObjectRef                   findUIComponentToSelect (ObjectRefList _UIComponentsList, UIComponent* _pUIComponentSelected, f32 _angleJoy, const bbool _findFarthest = bfalse);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check the buttons and joysticks of the pad
        /// @param _buts buttons of the pad
        /// @param _pos joysticks of the pad
        /// @return true if the game pad button had an effect
        bbool                       applyGamePadButton (const StringID & _action);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check the buttons and joysticks of the pad in the debug menu
        /// @param _numPad number of the pad
        /// @param _buts buttons of the pad
        /// @param _pos joysticks of the pad
#ifdef ITF_SUPPORT_CHEAT
        void                        applyGamePadButtonDBGMenu (u32 _numPad, InputAdapter::PressStatus* _buts, float* _pos);
#endif // ITF_SUPPORT_CHEAT

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Determine which UIComponent must be selected (according to the button/joy pressed and the current UIComponent selected)
        /// @param _joyX value on the X-axis
        /// @param _joyY value on the Y-axis
        void                        selectMenuItem (f32 _joyX, f32 _joyY);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Select a menu item with the joystick
        /// @param _pos position of the joystick
        void                        selectMenuItemWithJoystick (float* _pos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Select a menu item with the D-pad
        /// @param _button button pressed
        void                        selectMenuItemWithDPad (u32 _button);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Apply the action of the UIComponent selected
        void                        applyMenuItemAction();
        void                        applyMenuPressAction();
        bbool                       applyMenuItemOtherAction(const StringID & _action);

        //Menu page actions
        StringID                    fetchMenuPageAction(const StringID &_action, const StringID &_defaultAction); //given a key (eg m_joyButton_B), return the according action. May pass to the menu listener
        bbool                       executeMenuPageAction(const StringID &_action, const StringID &_inputAction); //Execute menu action
        bbool                       isAButtonPressed() const;
        bbool                       isAButtonReleased() const;

        MenuPriority                getHighestShownPriority(); //return priority of currently shown menu (highest priority), or MenuPriority_Count if none

        void                        sendActionToSoundPlayer(const StringID & _menu, const StringID & _action, const StringID & _selection) const;

        // Update the player names in the swap players menu
#ifdef ITF_SUPPORT_CHEAT
        void                        updateSwapMenu();
#endif // ITF_SUPPORT_CHEAT

        ITF_VECTOR<UIMenu*>         m_menus;
        ITF_VECTOR<StringID>        m_menuTree;
        StringID                    m_currentMenuID;
        bbool                       m_inMenu;
#ifdef ITF_SUPPORT_CHEAT
        bbool                       m_inDBGMenu;
#endif // ITF_SUPPORT_CHEAT
        u32                         m_player;

        StringID                    m_menuAction_ShowPreviousMenu;
        StringID                    m_menuAction_HideMenu;
        StringID                    m_menuAction_ApplyMenuItemAction;
        StringID                    m_menuAction_Nothing;

#ifdef ITF_SUPPORT_CHEAT
        // Debug Menu
        DebugMenu*                  m_debugMenuMaps;
        DebugMenu*                  m_debugMenuCheats;
        DebugMenu*                  m_debugMenuSwapPlayers;
        DebugMenu*                  m_debugChangeFramerate;
        u32                         m_currentPageDBGMenu;
        ITF_VECTOR<DebugMenu*>      m_DBGmenus;
#endif // ITF_SUPPORT_CHEAT
#ifndef ITF_FINAL
		bool						m_mouseLeftInput;
#endif // ITF_FINAL

        typedef ITF_MAP<StringID,MenuItemActionListener*> MenuItemActionMap;
        StringID                    m_MenuStack[MenuPriority_Count];
        MenuItemActionMap           m_menuItemActionMap;

        f32                         m_timeout;
#ifdef ITF_SUPPORT_CHEAT
        f32                         m_timeoutDBGMenu;
#endif // ITF_SUPPORT_CHEAT
        f32                         m_timer;
#ifdef ITF_SUPPORT_CHEAT
        f32                         m_timerDBGMenu;
#endif // ITF_SUPPORT_CHEAT
        f32                         m_timeoutJustPressed;
        bbool                       m_firstPressed;
        bbool                       m_joyReleased;

        bbool                       m_isMenuReady;
        bbool                       m_canExecuteMenuItemAction;

        bbool                       m_inGammaSettingsMenu;// temp E3
        bbool                       m_listenerRegistered;
        StringID                    m_menuStartLastMainSelectionID;

        UIMenuSoundComponent*       m_soundPlayerComponent;

#ifdef ITF_WINDOWS
		bool						m_mouseLocked;
#endif //ITF_WIN32
        u32                         m_currentInputPlayer;
    public:
        u32                         getCurrentInputPlayer() const { return m_currentInputPlayer; }
    };
}
#endif // _ITF_UIMENUMANAGER_H_
