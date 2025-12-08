#ifndef ITF_ADAPTER_SAVEGAME_NINTENDO_H_
#define ITF_ADAPTER_SAVEGAME_NINTENDO_H_

#pragma once

#include "engine/AdaptersInterfaces/Adapter_Savegame.h"

#include <nn/account/account_Types.h>
#include <deque>

namespace ITF
{

    class Adapter_Savegame_Nintendo : public Adapter_Savegame
    {
    public:
                    Adapter_Savegame_Nintendo();
        virtual     ~Adapter_Savegame_Nintendo();

        void        init() override;
        void        term() override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// choose a device. May display a dialog box. Asynchronous. Savegames are automatically enumerated.
        /// Check result with getIsDeviceChosenFlags() and getLastLoadSaveError()
        /// @param _playerIndex The signed in player
        /// @param _minFreeSize The minimum free size that must be available on device. Also max savegame size
        /// @param _isSilentWhenChosenNone If false, shows a ChoiceWhenDeviceChosenAsNone prompt
        /// @return error code (Error_Ok if no error). A typical error is Error_LoadOrSaveForbiddenForPlayer.
        /// $GB: never called
        virtual ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize = 0, bbool _isSilentWhenChosenNone = btrue) override { return Error_Ok; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegame
        virtual ErrorCode   startSavegameEnumerationUpdate(u32 _playerIndex) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegames on every device
        virtual ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex) override
        {
            return startSavegameEnumerationUpdate(_playerIndex);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Asynchronous save.
        /// @param _playerIndex The player we save for. Controller port index (0-3 on XBOX)
        /// @param _baseName Basename (without extension) of the saved file/archive
        /// @param _displayName Display name of the file/archive
        /// @param _data Data to save
        /// @param _dataSize Size of this data
        /// @param _warnIfOverwrite btrue if reinforce overwrite checks (for ex when creating a new game)
        /// @return Error_Ok if ok so far, or Error_LoadOrSaveForbiddenForPlayer. Look at getLastLoadSaveErrorCode for asynchronous error
        virtual ErrorCode startSave(u32 _playerIndex, u32 _slotIndex, const String8& _baseName, const String& displayName, const void* _data, u32 _dataSize, bbool _warnIfOverwriteForNewGame) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Asynchronously load savegame into m_currentSavegame (see getLastLoadedOrSavedData.)
        /// @param _playerIndex The player we load for. Controller port index (0-3 on XBOX)
        /// @param baseName Basename (without extension) of the file/archive to load
        /// @return Error_Ok or Error_LoadOrSaveForbiddenForPlayer. Look at getLastLoadSaveErrorCode for asynchronous error
        virtual ErrorCode startLoad(u32 _playerIndex, u32 _slotIndex, const String8& baseName) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get number of files/content/(...) we enumerated last time
        /// @param _playerIndex The player we enumerated for. Controller port index (0-3 on XBOX)
        /// $GB: never called
        virtual u32 getEnumeratedContentCount(u32 _playerIndex) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get details about enumerated files
        /// @param _playerIndex The player we enumerated for. Controller port index (0-3 on XBOX)
        /// @param _index
        /// @param _baseName
        /// @param _displayName
        virtual bbool getEnumeratedContent(u32 _playerIndex, u32 _index, String8& _baseName, String& _displayName, bbool& _emptyContent) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this to update asynchronous operations
        virtual void update() override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// is there a pending async operation?
        /// Calling hasPendingOperation is required before calling getLastLoadedOrSavedData or getEnumeratedContent or getEnumeratedContentCount
        virtual bbool hasPendingOperation() override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        // PROMPT FUNCTIONS

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwrite
        virtual bbool isAskingWhatToDoBecauseFileExist() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwrite(ChoiceWhenFileExists _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwriteForNewGame
        virtual bbool isAskingWhatToDoBecauseFileExistForNewGame() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerDeviceNoLongerValidForSave
        virtual bbool isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device is no longer valid, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerSaveError
        virtual bbool isAskingWhatToDoBecauseOfSaveError() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When there was a file error while saving, answer with this function
        virtual void signalAnswerSaveError(ChoiceWhenSaveError _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerChoseNoDevice
        virtual bbool isAskingWhatToDoBecauseOfChoseNoDevice() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device selection was dismissed, answer with this fn
        virtual void signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because device was changed
        virtual bbool isAskingWhatToDoBecauseDeviceNoLongerValidForLoad() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///when Adapter_Savegame asks what to do because device was changed, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because of a load error
        virtual bbool isAskingWhatToDoBecauseOfLoadError() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because of a load error, answer with this fn
        virtual void signalAnswerLoadError(ChoiceWhenLoadError _choice) override {}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because it cant find a given savegame
        virtual bbool isAskingWhatToDoBecauseLoadCantFind() override { return bfalse; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because it cant find a given savegame, answer with this fn
        virtual void signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice) override {}

    private:

        struct SaveDataEntry
        {
            void loadFromBuffer(const Vector<u8>& _buffer);
            void saveToBuffer(Vector<u8>& _buffer) const;

            struct File
            {
                String8 name;
                String8 displayName;
                Vector<u8> data;
            };

            Vector<File> m_files;
        };

        struct WriteSample
        {
            f64 timeSeconds;
            u32 ops;
            u32 bytes;
        };

        struct PendingSaveOp
        {
            SaveDataEntry data;
            u32 payloadSize = 0;
            u32 slotIndex = 0;
            String8 fileToLoad;
        };

        std::deque<WriteSample> m_writeWindow;      // sliding window of writes (last 60s)
        std::deque<PendingSaveOp> m_pendingSaves;   // saves deferred for TRC0011 budget

        bool tryConsumeWriteBudget(u32 payloadSize);
        void pruneWriteWindow(f64 nowSeconds);
        void enqueuePendingSave(SaveDataEntry&& data, u32 payloadSize, u32 slotIndex, const String8& fileToLoad);
        bool scheduleNextPendingSave();

        // A thread executing save/load operations.
        // Result of the operations are copied in main thread in update()
        Thread m_operationThread{};
        u32 operationThreadMethod();

        void setupMemorySaveArea();

        struct Operation
        {
            enum Type
            {
                Save,
                Load,
            };

            Type opType = Save;
            SaveDataEntry saveData;
        };

        nn::account::Uid m_initialUserUID;

        CondVariable m_operationCondVar{ "SaveGame_Operation" };
        // begin variables to protect with m_operationCondVar
        Vector<Operation> m_operations; // enqueue operations. m_operationCondVar.signal() is called each time an operation is added
        bool m_saveMounted = false; // set to true when the save directory is mounted
        bool m_initDone = false; // set to true when the thread has finish to initialize
        bool m_stopOperationThread = false; // set to true to request to stop the thread
        bool m_operationThreadDone = true; // set to false when the thread has effectively been stopped

        Mutex m_doneLoadSaveMutex; // protect m_doneLoadOrSave
        // enqueue results of operations to copy on main thread in update()
        // For the moment, just the last one (more recent) is copied.
        Vector<SaveDataEntry> m_doneLoadOrSave;

        // This member is synced with the save content (as soon as a first content fetch request was done).
        // Do not modify directly, copy it, modify the copy and trigger a save operation.
        // After a save, it is updated. The member m_currentSavegame will contain the current selected file content.
        SaveDataEntry m_currentFullSaveData; // main thread

        // m_fileIdxToLoad is kept to choose what file content to copy in m_currentSavegame.
        u32 m_fileIdxToLoad = U32_INVALID; // main thread
        String8 m_fileToLoad{}; // just for assert

        void addSaveOperation(SaveDataEntry _data);
        void addLoadOperation();

        void processOperation(Operation operation); // in thread
        static void processLoadOperation(SaveDataEntry& dataLoaded); // in thread
        static void processSaveOperation(const SaveDataEntry& dataToSave); // in thread

    };

    #define SAVEGAME_ADAPTER_NINTENDO (static_cast<Adapter_Savegame_Nintendo*>(SAVEGAME_ADAPTER))
}

#endif // ITF_ADAPTER_SAVEGAME_NINTENDO_H_
