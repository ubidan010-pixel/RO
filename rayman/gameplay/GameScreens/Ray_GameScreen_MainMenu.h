#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#define _ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif //_ITF_RAY_OPTIONMENUHELPER_H_

namespace ITF
{
    class TRCMessage_Base;
    //////////////////////////////////////////////////////////////////////////
    ///The main menu
    class Ray_GameScreen_MainMenu : public GameScreenBase, public UIMenuManager::MenuItemActionListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_MainMenu,GameScreenBase,1338236129)
    public:
        //Implementation of base class members

        Ray_GameScreen_MainMenu();
        ~Ray_GameScreen_MainMenu();

        virtual void init();

        virtual void close();

        virtual void update();

        virtual bbool hasGameplay() {return bfalse;}

        virtual void onWorldLoaded();

        virtual void onMenuItemAction (UIComponent* _UIComponent);

        virtual StringID onMenuPageAction(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction);

        virtual bbool onMenuItemOtherAction(UIComponent* _UIComponent, const StringID & _action);

        bbool onUserSignedInOut(u32 _playerIndex, bbool _signedIn, bbool _isOnlineStatusChange);
        bbool  updateSelectedSlot(const StringID& _id);

        virtual void onEndOfPlayingVideo();
        virtual void onStartOfPlayingVideo();
        virtual void onPauseOfPlayingVideo();
        virtual void onEndOfFadeIn();

        virtual bbool canDisplayTRCMessages();

        virtual bbool canOpenMovie();
        virtual void onMapLoaded(ObjectRef _world);

        virtual bbool checkSignInOutChanges();

        u32 getPlayerIndex() {return m_playerIndex;}

    private:

        enum State
        {
            State_Initializing=0,
            State_WaitMapLoaded,
            State_ShowingTitleScreen,
            State_ShowingPressStart,
            State_PressingStart,
            State_ShowingMainMenu_Root,

            State_ShowingMainMenu_SaveLoad_Root,
            State_ShowingMainMenu_SaveLoad_WaitingForAction,
            State_ShowingMainMenu_SaveLoad_WaitingForDevice,
            State_ShowingMainMenu_SaveLoad_WaitingForDeleteAnswer,
            State_ShowingMainMenu_SaveLoad_DeleteSave,
            State_ShowingMainMenu_SaveLoad_DeletingSave,
            State_ShowingMainMenu_SaveLoad_DeleteRefreshing,

            State_ShowingMainMenu_Load_WaitLoaded,
            State_ShowingMainMenu_Load_Error,

            State_ShowingMainMenu_Load_Action,
            State_ShowingMainMenu_Load_StartLoading,

            State_ShowingMainMenu_NewGame_Action,
            State_ShowingMainMenu_NewGame_WaitTRCAnswer,
            State_ShowingMainMenu_NewGame_StartSaving,
            State_ShowingMainMenu_NewGame_WaitSavingProcess,
            State_ShowingMainMenu_NewGame_WaitPrologueLoad,
            State_ShowingMainMenu_NewGame_Ready,
            State_ShowingMainMenu_NewGame_PlayingVideo,

            State_ShowingMainMenu_NewGame_ChooseDevice,
            State_ShowingMainMenu_NewGame_WaitDeviceChosen,
            State_ShowingMainMenu_NewGame_WaitSaved,
            State_ShowingMainMenu_NewGame_WaitSavedNotificationCleared,
            State_SigninChange_WaitPendingStorageOperation,
            State_Exited,
        };

        void setState(State _state);


        void updatePlayerIndex();

        //Enter/update/leave for each state
        void enter_WaitMapLoaded();
        void update_WaitMapLoaded();
        void leave_WaitMapLoaded();
        void enter_ShowingTitleScreen();
        void update_ShowingPressStart();
        void leave_ShowingPressStart();
        void update_ShowingMainMenu_Root();
        void update_ShowingMainMenu_Load_StartLoading();
        void update_ShowingMainMenu_Load_WaitLoaded();
        void update_ShowingMainMenu_Load_ChooseDevice();
        void enter_ShowingMainMenu_Load_WaitDeviceChosen();
        void update_ShowingMainMenu_Load_WaitDeviceChosen();
        void leave_ShowingMainMenu_Load_WaitDeviceChosen();
        void update_ShowingTitleScreen();
        void update_ShowingMainMenu_Load_Error();
        void update_ShowingMainMenu_NewGame_ChooseDevice();
        void update_ShowingMainMenu_LoadSave_Root();
        void update_ShowingMainMenu_NewGame_WaitDeviceChosen();
        void update_ShowingMainMenu_NewGame_WaitSaved();
        void update_SigninChange_WaitPendingStorageOperation();

        void update_PressingStart();
        void update_ShowingMainMenu_Load_Action();
        void update_ShowingMainMenu_NewGame_Action();
        void update_ShowingMainMenu_NewGame_StartSaving();
        void update_ShowingMainMenu_NewGame_WaitSavingProcess();
        void update_ShowingMainMenu_NewGame_WaitPrologueLoad();
        void update_ShowingMainMenu_NewGame_Ready();
        void update_ShowingMainMenu_SaveLoad_WaitingForAction();

        void update_ShowingMainMenu_SaveLoad_DeleteSave();
        void update_ShowingMainMenu_SaveLoad_DeletingSave();
        void update_ShowingMainMenu_SaveLoad_DeleteRefreshing();

        void onPressStartAction();

        void enter_Exited();
        void leave_Exited() {}

        void activeBlackScreen(bbool _active);

        void startPlayingIntroVideo();
        void stopPlayingIntroVideo();

        void updatePadStatus();
        void fillSaveGameSlots(bbool forceEmpty = bfalse);
        void fillOneSlot(u32 _slotIndex, const String& _text);

        //storage enumeration
        void startSavegameCheckOnAnyDevice();
        void setCanShowLoadGame(bbool _show, u32 _slot = 0);

        static void completedEnumOnEveryDeviceCallback();
        static void storageDeviceChangedCallback();

        static void onCloseTRCMessage(const StringID & answer, TRCMessage_Base * pMessage, void* params);

        bbool shouldShowWarningBootPopup();
        void updateLastPlayTime();

        static bbool m_firstLoading;
        //
        State m_state;
        static Ray_GameScreen_MainMenu *s_this;
        u32 m_playerIndex;

        bbool m_storageDeviceChanged;
        SafeArray<bbool> m_showLoadMenuEntry;
        u32 m_currentSlotSelected;
        bbool m_firstEntryInPlayMenu;
        bbool m_wasDisplayingError;
        bbool m_savingProcessEnabled;
        bbool m_rollingDemoIsPlaying;
        bbool m_startedMusic;
        bbool m_wasPreloaded;
        f64 m_timeToWaitBeforeStartScreen;
        f64 m_timeStartingToWait;
        u32 m_waitingFrameForTRCMsg;
        bbool m_pendingShowPCMenu;
#ifdef ITF_SUPPORT_NETWORKSERVICES
        NetworkServices::User* m_validUser;
#endif //ITF_SUPPORT_NETWORKSERVICES
        Ray_OptionMenuHelper m_optionMenuHelper;
        //
    };
}


#endif
