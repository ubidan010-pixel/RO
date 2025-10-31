#ifdef ITF_PS3

#ifndef _ITF_ADAPTER_SAVEGAME_PS3_H__
#define _ITF_ADAPTER_SAVEGAME_PS3_H__

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#include <sys/fs_external.h>
#include <sysutil/sysutil_savedata.h>

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// WIN32 version of savegame manager. 
    /// WARNING! THIS CODE IS ONLY TEMPORARY!
    class Adapter_Savegame_PS3 : public Adapter_Savegame
    {
    public:
        Adapter_Savegame_PS3();
        virtual ~Adapter_Savegame_PS3() {}

        virtual void init();
        virtual void term();
        bbool initForBoot();
        void deleteSaveFile();
        void retryReadingSaveFile();

        virtual bbool hasPendingOperation() {return m_mainState!=State_Idle;}

        ///NB : LOOK AT UNSIGNED PROFILES IN GAMES : HOW SAVE IS HANDLED?

        ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone);
        ErrorCode startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwrite);
        ErrorCode startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName);
        ErrorCode startSavegameEnumerationUpdate(u32 _player);
        ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex) {return startSavegameEnumerationUpdate(_playerIndex);}
        virtual void update();

        virtual u32 getEnumeratedContentCount(u32 /*_playerIndex*/) {ITF_ASSERT(!hasPendingOperation());  return m_filteredDeviceContent.size();}
        virtual bbool getEnumeratedContent(u32 _playerIndex, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave);

        virtual void setCurrentOperationCompleteCallback(OperationCompleteCallback_t _callback);

        bbool isAskingWhatToDoBecauseFileExist() {return bfalse;}
        bbool isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave() {return bfalse;}
        bbool isAskingWhatToDoBecauseOfSaveError(){return bfalse;}
        bbool isAskingWhatToDoBecauseOfChoseNoDevice() {return bfalse;}
        bbool isAskingWhatToDoBecauseDeviceNoLongerValidForLoad() {return bfalse;}
        bbool isAskingWhatToDoBecauseOfLoadError() {return bfalse;}
        bbool isAskingWhatToDoBecauseLoadCantFind() {return bfalse;}
        bbool isAskingWhatToDoBecauseFileExistForNewGame() {return bfalse;}

        void signalOkForOverwrite(ChoiceWhenFileExists /*_choice*/) {}
        void signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame /*_choice*/) {}
        void signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave /*_choice*/){}
        void signalAnswerSaveError(ChoiceWhenSaveError /*_choice*/) {}
        void signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone /*_choice*/) {}
        void signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad /*_choice*/) {}
        void signalAnswerLoadError(ChoiceWhenLoadError /*_choice*/) {}
        void signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind /*_choice*/) {}

        bbool IsSaveProcessEnable() const;
        void setRootGamePath(String8 _rootPath) { m_rootPath = _rootPath;  initSystemPath(); }
        void preLoad(u32 _player, u32 _maxPlayersToUse = 1, bbool _force = bfalse);
        
        u32 getMaxSlot() const;
    private:
        /* The step of saving operation for progress counter in file operation callback */
        enum SaveOperation {
            SAVE_OPERATION_IDLE= 0,
            SAVE_OPERATION_STEP_ICON0,
            SAVE_OPERATION_STEP_ICON1,
            SAVE_OPERATION_STEP_PIC1,
            SAVE_OPERATION_STEP_GAMEFILE,
            SAVE_OPERATION_STEP_END,
            SAVE_OPERATION_STEP_FAILED
        };
        const u32 m_FilesToSave;
        SaveOperation m_currentSaveOperation;

        /* The step of loading operation for progress counter in file operation callback */
        enum LoadOperation {
            LOAD_OPERATION_IDLE = 0,
            LOAD_OPERATION_STEP_GAMEFILE,
            LOAD_OPERATION_STEP_END,
            LOAD_OPERATION_STEP_FAILED
        };
        const u32 m_FilesToLoad;
        LoadOperation m_currentLoadOperation;


        enum FilesSystem
        {
            FILE_GAMEFILE = 0,  // Must be First
            FILE_ICON0,
            //FILE_PIC1,
            FILE_COUNT          // Must be Last
        };
        String  PathFilesSystem [FILE_COUNT];
        u32     SizeFilesSystem [FILE_COUNT];

        struct DataFile
        {
            u8*     pData;
            u32     dataSize;
        };
        DataFile     DataFilesSystem [FILE_COUNT];

        enum MainState 
        {
            State_Idle,
            RetryReadingFile,
            ChooseDevice_PostedShowSelector, ChooseDevice_ShowingSelector,
            EnumeratingContent,
            Save_WritingFile,
            DeletingFile,
            Load_WaitingBeforeIdle,
            HeaderLoad_WaitingBeforeIdle,
            ENUM_FORCE_SIZE_32(0)
        };

        struct Content
        {
            char szFileName[128];
            char szDisplayName[CELL_SAVEDATA_SYSP_DETAIL_SIZE];
            bbool emptySave;
        };

        struct  Header
        {
            u16  displayedName [ 128 ];
            bbool emptySave;
            u32 fileSize;
            u32 slot;
            u32 dataCRC;
            u32 codeCRC;
        };
        static u32 getHeaderSize(){return sizeof(Header);}

        void handle_MustChooseDevice();
        void handle_JustClosedDeviceSelectorGUI();
        void handle_AsynchronousContentEnumeration();
        void handle_Save_CreateContent();
        void handle_WritingSaveFile();
        void handle_Load_WaitingBeforeIdle();
        void handle_HeaderLoad_WaitingBeforeIdle();
        void handle_DeletingFile();
        void handle_RetryReadingFile();

        bbool isSaveCorrupted();
        void initSystemPath();

        static ErrorCode sdkErrorToSystemError(int err);

#ifndef ITF_FINAL
        static void DumpSaveError( int ret );
#endif //!ITF_FINAL

        u32 getTotalNeededSizeKB();

        static DWORD WINAPI saveThreadRoutine(void *_param);
        static DWORD WINAPI loadThreadRoutine(void *_param);
        static DWORD WINAPI loadHeaderThreadRoutine(void *_param);
        static DWORD WINAPI deleteSaveThreadRoutine(void *_param);
        static DWORD WINAPI retryThreadRoutine(void *_param);

        static void saveDataStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set);
        static void saveDataFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set);

        static void loadDataFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set);
        static void loadDataStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set);

        static void loadHeaderFileCallback(CellSaveDataCBResult *_cbResult, CellSaveDataFileGet *_get, CellSaveDataFileSet *_set);
        static void loadHeaderStatCallback(CellSaveDataCBResult *_cbResult, CellSaveDataStatGet *_get, CellSaveDataStatSet *_set);

        static void deleteSaveFixedCallback(CellSaveDataCBResult *_cbResult, CellSaveDataListGet *_get, CellSaveDataFixedSet *_set);
        static void deleteSaveDoneCallback(CellSaveDataCBResult *_cbResult, CellSaveDataDoneGet *_get);


        bbool startContentEnumeration(u32 _player);
        void setMainState(MainState _newState);
        static String8 getSavegamePath(const String8 &_filename);

        void readHeader();

        void tests();

        HANDLE m_IOOperationThread;
        Thread*             m_thread;

        DWORD m_choseDevice_PlayerIndex;
        DWORD m_contentEnumeration_PlayerIndex;

        SafeArray<Header> m_filteredDeviceContent;
        volatile MainState m_mainState;
        int m_frameSinceLastStateChange;
        String8 m_loadSaveBaseName;
        String  m_saveName;
        String  m_displayedName;
        bbool m_wasEnumerated;

        bbool m_threadRunning;
        bbool m_firstLoading;
        void*   fileBuffer;
        String8 m_rootPath;
        u32 m_currentSlot;
        bbool m_readerUpdated;
        bbool m_isNotOwnerOfTheSave;
        bbool m_isCreatingBootFiles;
        bbool m_fileIsCorrupted;

        volatile bbool m_hasToExit;

        CellSaveDataSystemFileParam m_dataSystemFileParam;
    };

#define SAVEGAME_ADAPTER_PS3             (static_cast< Adapter_Savegame_PS3* > (SAVEGAME_ADAPTER))

}

#endif // _ITF_ADAPTER_SAVEGAME_PS3_H__

#endif
