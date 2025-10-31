#ifdef ITF_X360

#ifndef _ITF_ADAPTER_SAVEGAME_X360_H__
#define _ITF_ADAPTER_SAVEGAME_X360_H__

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_SYSTEMMESSAGELISTENER_X360_H_
#include "adapters/SystemAdapter_x360/SystemMessageListener_x360.h"
#endif


namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// XBOX360 version of savegame manager.
    /// TODO :
    /// -multiple users
    /// -Error_LoadOrSaveForbiddenForPlayer (demo/not signed in)
    /// -plug'n'play
    /// -asynchronous load
    class Adapter_Savegame_x360 : public Adapter_Savegame
    {
    public:
        Adapter_Savegame_x360();
        virtual ~Adapter_Savegame_x360() {}

        virtual void init();
        virtual void term();


        virtual bbool hasPendingOperation() {return m_mainState!=State_Idle ;}

        ///NB : LOOK AT UNSIGNED PROFILES IN GAMES : HOW SAVE IS HANDLED?

        ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize = 0, bbool _isSilentWhenChosenNone = btrue);
        ErrorCode startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwrite);
        ErrorCode startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName);
        ErrorCode startSavegameEnumerationUpdate(u32 _player);
        ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex);

        void update();

        bbool isAskingWhatToDoBecauseFileExist() {return m_mainState==Save_AskOverWrite;}
        bbool isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave() {return m_mainState==Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother;}
        bbool isAskingWhatToDoBecauseOfSaveError() {return m_mainState==Save_IOError_AskWhatToDo;}
        bbool isAskingWhatToDoBecauseOfChoseNoDevice() {return m_mainState==ChooseDevice_ChosenNone_RetryOrContinue;}
        bbool isAskingWhatToDoBecauseDeviceNoLongerValidForLoad() {return m_mainState==Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother;}
        bbool isAskingWhatToDoBecauseOfLoadError() {return m_mainState==Load_IOError_AskWhatToDo;}
        bbool isAskingWhatToDoBecauseLoadCantFind() {return m_mainState==Load_CantFind_AskWhatToDo;}
        bbool isAskingWhatToDoBecauseFileExistForNewGame() {return m_mainState==Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer;}

        void signalForDeleteCorruptedFile(ChoiceWhenDeleteingCorruptedFile _choice);
        void signalOkForOverwrite(ChoiceWhenFileExists _choice);
        void signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice);
        void signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice);
        void signalAnswerSaveError(ChoiceWhenSaveError _choice);
        void signalAnswerWhenUserNotSignedIn(ChoiceWhenUserNotSignedIn _choice);
        void signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice);
        void signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice);
        void signalAnswerLoadError(ChoiceWhenLoadError _choice);
        void signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice);

        u32 getEnumeratedContentCount(u32 /*_playerIndex*/) {ITF_ASSERT(!hasPendingOperation());  return m_filteredDeviceContent.size();}
        bbool getEnumeratedContent(u32 _playerIndex, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave);

        bbool canUserUseStorage(u32 _player, bbool _checkStorageDeviceChosen);
        void preLoad(u32 _player, u32 _maxPlayersToUse = 1, bbool _force = bfalse);

        void checkForSaveStateValidity(u32 _playerIndex);
        void GetSelectedDeviceName(String & deviceName);

        virtual bbool SaveProfileSettings(u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize);
        virtual bbool LoadProfileSettings(u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize);

        void cancelAllOperations();
        void prepareInit(u32 _minFreeSize);
        bbool checkDataCRCForCurrentSave( u32 _dataCRC );
    private:
        enum MainState 
        {
            State_Idle,
            
            PreLoad_Running, PreLoadFailed,
            ChooseDevice_PostedShowSelector, ChooseDevice_ShowingSelector,
            ChooseDevice_ChosenNone_RetryOrContinue,

            ReadingHeader,
            EnumeratingContent,

            Save_AskOverWrite,
            Save_DeviceWasChanged_StillValid, //here, device was changed or just selected (with dialog box) with success after IO errors
            Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother,
            Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle,
            Save_SignalCantSave_WrongDevice, //Device has an error, or confirmed we dont want a device
            Save_StartCreatingContent_PreCheckNewGameOverwrite,
            Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer,
            Save_StartCreatingContent, Save_CreatingContent, Save_WritingFile,
            Save_StartDeletingContent, Save_DeletingContent,
            Save_IOError_AskWhatToDo,

            Load_IOError_DeletingConfirmation,
            Load_Start,
            Load_OpeningArchive,
            Load_LoadingFromThread,
            Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother,
            Load_SignalCantLoad_WrongDevice,
            Load_IOError_AskWhatToDo,
            Load_CantFind_AskWhatToDo,


            State_Invalid, // invalid state
            ENUM_FORCE_SIZE_32(0)
        };

        void setMainState(MainState _state);

        bbool isPreLoadingFailed() const { return m_mainState == PreLoadFailed; }

        void startReadingHeaders();
        void handle_ReadHeaders();
        void handle_preLoadRunning();
        void handle_MustChooseDevice();
        void handle_JustClosedDeviceSelectorGUI();
        void handle_AsynchronousContentEnumeration();
        void handle_StartCreatingContent_PreCheckNewGameOverwrite();
        void handle_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer();
        void handle_StartCreatingContent();
        void handle_Save_CreateContent();
        void handle_WritingSaveFile();
        void handle_Save_DeviceWasChanged_StillValid();
        void handle_Save_StartDeletingContent();
        void handle_Save_DeletingContent();
        void handle_Load_Start();
        void handle_Load_OpeningArchive();
        void handle_Load_LoadingFromThread();

        void notifyStorageChanged();
        void checkIfDeviceChanged();
        void checkDeviceIsOk(MainState _stateAfterOk, MainState _stateAfterFailure);
        bbool isCurrentDeviceDisconnected() const;

        void switchToSaveErrorState(ErrorCode _code);
        void switchToLoadErrorState(ErrorCode _code);

        static u32 WINAPI saveThreadRoutine(void *_param);
        static u32 WINAPI loadThreadRoutine(void *_param);
        static u32 WINAPI headersThreadRoutine(void *_param);

        bbool startContentEnumeration(u32 _player, MainState _stateWhenDeferredOk=State_Idle, MainState _stateWhenDeferredFailure=State_Idle); //if fails, lastAsyncError is filled
        void cancelCurrentContentEnumeration();

        ErrorCode startChooseDevice_Internal(u32 _playerIndex, u32 _minFreeSize, 
            MainState _newDeferredStateWhenDeviceSelected, 
            MainState _newDeferredStateWhenDeviceHasError, 
            MainState _newDeferredStateWhenConfirmedNoDevice,
            bbool _forceUI,
            bbool _chooseDevice_SilentWhenChoseNone);

        i32 findBasenameAmongEnumerated(const String8 &_baseName);
        
        bbool isCurrentSlotEmpty() const ; 

        void loadProblem_ChangeDevice();

        void tests();

        static const u32 xcontent_buffer_item_count = 16;
        XOVERLAPPED m_overlappedDevSelector;
        XOVERLAPPED m_overlappedContentEnum;
        XOVERLAPPED m_overlappedSaving;
        XOVERLAPPED m_overlappedLoading;
        

        volatile bbool m_threadIsRunning; 
        Thread* m_thread;


        //
        XCONTENTDEVICEID m_storageDevice;       


        struct Header
        {
            u16 displayedName [ 128 ];
            u32 CodeCRC;
            u32 DataCRC;
            u32 fileSize;
            u32 slot;
        };

        struct Content
        {
            bbool   isCorrupted;
            XCONTENT_DATA   data;
        };

        static u32 getHeaderSize() { return sizeof(Header); }
        //content enumeration
        HANDLE m_contentEnumerationHandle;
        
        SafeArray<Header> m_slotList;
        SafeArray<Content> m_filteredDeviceContent;
        static XCONTENT_DATA m_tmpDeviceContent[xcontent_buffer_item_count];
        
        u32 m_selectedDeviceIndex;
        //
        volatile MainState m_mainState;

        //Data for inter-state communication for several actions
        DWORD m_mainPlayerIndex;
        DWORD m_choseDevice_PlayerIndex;
        bbool m_chooseDevice_SilentWhenChosenNone;
        DWORD m_contentEnumeration_PlayerIndex;
        DWORD m_saving_PlayerIndex, m_loading_PlayerIndex;
        String8 m_loadSaveBaseName;
        String m_loadSaveDisplayName, m_saveGameName; // slot name
        u32 m_currentSlotIndex;
        bbool m_wasEnumerated, m_lastEnumerationFailed;
        MainState m_statedWhenDeferredEnumerationFailed, m_statedWhenDeferredEnumerationOk;
        MainState m_chooseDevice_newDeferredStateWhenDeviceSelected,
            m_chooseDevice_newDeferredStateWhenDeviceHasError,
            m_chooseDevice_newDeferredStateWhenConfirmedNoDevice;
        bbool m_chooseDevice_ForceUI;
        bbool m_reinforceOverwriteChecksForNewGame;

        //Plug'n'play
        SystemMessageListener_x360 m_systemMessageListener;
        bbool m_startCheckDeviceOkASAP, m_FirstDeviceChange;
        
        bbool m_deviceChangedDuringSave;
        //
        bbool m_mustSwitchToSaveErrorState, m_mustSwitchToLoadErrorState;
        u32 m_MaxPlayersForSaving;
    };

}

#endif // _ITF_ADAPTER_SAVEGAME_X360_H__

#endif
