#pragma once

#ifdef ITF_DUMMY_SAVEGAME

// Sample file for new platform

#include "engine/AdaptersInterfaces/Adapter_Savegame.h"

#include "core/container/SafeArray.h"

// #include <dummy_platform_save_api.h>

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_SAVE_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "Save not implemented")
#else
    #define ITF_SAVE_NOT_IMPLEMENTED() do {} while (0)
#endif

namespace ITF
{

    class Adapter_Savegame_Dummy : public Adapter_Savegame
    {
    public:
        Adapter_Savegame_Dummy();
        virtual ~Adapter_Savegame_Dummy() override;

        virtual void init() override;
        virtual void term() override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// choose a device. May display a dialog box. Asynchronous. Savegames are automatically enumerated.
        /// Check result with getIsDeviceChosenFlags() and getLastLoadSaveError()
        /// @param _playerIndex The signed in player
        /// @param _minFreeSize The minimum free size that must be available on device. Also max savegame size
        /// @param _isSilentWhenChosenNone If false, shows a ChoiceWhenDeviceChosenAsNone prompt
        /// @return error code (Error_Ok if no error). A typical error is Error_LoadOrSaveForbiddenForPlayer.
        virtual ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize = 0, bbool _isSilentWhenChosenNone = btrue) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegame
        virtual ErrorCode startSavegameEnumerationUpdate(u32 _playerIndex) override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegames on every device
        virtual ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex) override;

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
        virtual bbool hasPendingOperation() override;

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        // PROMPT FUNCTIONS

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwrite
        virtual bbool isAskingWhatToDoBecauseFileExist() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwrite(ChoiceWhenFileExists _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwriteForNewGame
        virtual bbool isAskingWhatToDoBecauseFileExistForNewGame() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerDeviceNoLongerValidForSave
        virtual bbool isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device is no longer valid, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerSaveError
        virtual bbool isAskingWhatToDoBecauseOfSaveError() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When there was a file error while saving, answer with this function
        virtual void signalAnswerSaveError(ChoiceWhenSaveError _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerChoseNoDevice
        virtual bbool isAskingWhatToDoBecauseOfChoseNoDevice() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device selection was dismissed, answer with this fn
        virtual void signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because device was changed
        virtual bbool isAskingWhatToDoBecauseDeviceNoLongerValidForLoad() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///when Adapter_Savegame asks what to do because device was changed, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because of a load error
        virtual bbool isAskingWhatToDoBecauseOfLoadError() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because of a load error, answer with this fn
        virtual void signalAnswerLoadError(ChoiceWhenLoadError _choice) override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because it cant find a given savegame
        virtual bbool isAskingWhatToDoBecauseLoadCantFind() override;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because it cant find a given savegame, answer with this fn
        virtual void signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice) override;
    };

#define SAVEGAME_ADAPTER_DUMMY             (static_cast<Adapter_Savegame_Dummy*> (SAVEGAME_ADAPTER))

}

#endif // ITF_DUMMY_SAVEGAME
