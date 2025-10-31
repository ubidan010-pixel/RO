#ifndef _ITF_ADAPTER_SAVEGAME_H__
#define _ITF_ADAPTER_SAVEGAME_H__

#pragma once

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace ITF
{

    class Adapter_Savegame
    {
    public:
        enum ErrorCode
        {
            Error_Ok=0,
            Error_LoadOrSaveForbiddenForPlayer, //storage may be disabled for demo, or player didn't want to chose storage, or user may not be logged in
            Error_Internal, //system errors, like an internal OS object which could not be created
            Error_CouldNotEnumerate,//Wrong device
            Error_SavegameNotFound, //May happen if you did not update savegame enumeration
            Error_IncorrectSavegameFormat,

            //getLastLoadSaveErrorCode only returns among Error_Ok plus the following values :
            Error_LoadSave_NoDevice, Error_LoadSave_IOError, Error_CancelledSaving,
            Error_AlreadySaving,
            ENUM_FORCE_SIZE_32(2)
        };

        //prompt types
        enum PromptType
        {
            PromptType_FileAlreadyExists,
            PromptType_FileAlreadyExistsForNewGame,
            PromptType_DeviceWhenDeviceNoLongerValidForSave, 
            PromptType_SaveError,
            PromptType_DeviceChosenAsNone,
            PromptType_UserNotSignedIn,
            PromptType_DeviceWhenDeviceNoLongerValidForLoad, 
            PromptType_LoadError,
            PromptType_LoadCantFind,
            PromptType_DeletingCorruptedFile,
            PromptType_Count, //reserved,
            ENUM_FORCE_SIZE_32(1)
        };

        enum ChoiceWhenDeleteingCorruptedFile
        {
            CorruptedFile_Delete,
            CorruptedFile_Cancel,
            ENUM_FORCE_SIZE_32(11)
        };
        //accepted answers  when prompt is displayed
        enum ChoiceWhenFileExists
        {
            FileExists_Overwrite,
            FileExists_ForceGiveup,//In case of emergency(TRC) exit. For example when just changed sign-in state
            FileExists_ChangeDevice,
            FileExists_NoLongerSave,
            ENUM_FORCE_SIZE_32(3)
        };

        //accepted answers  when prompt is displayed
        enum ChoiceWhenFileExistsForNewGame
        {
            FileExistsForNewGame_Overwrite,
            FileExistsForNewGame_ForceGiveup,//In case of emergency(TRC) exit. For example when just changed sign-in state
            FileExistsForNewGame_CancelSave,
            ENUM_FORCE_SIZE_32(4)
        };

        //accepted answers when prompt is displayed
        enum ChoiceWhenDeviceNoLongerValidForSave
        {
            DeviceNoLongerValidForSave_ChangeDevice,
            DeviceNoLongerValidForSave_ForceGiveUp,//In case of emergency(TRC) exit. For example when just changed sign-in state
            DeviceNoLongerValidForSave_NoLongerSave,
            ENUM_FORCE_SIZE_32(0)
        };
        //accepted answers when prompt is displayed
        enum ChoiceWhenSaveError 
        {
            SaveError_ChangeDevice,
            SaveError_ForceGiveUp, //
            SaveError_NoLongerSave ,
            ENUM_FORCE_SIZE_32(5)
        };
        //accepted answers when device was just chosen as "no device" (dismissed dev selector)
        enum ChoiceWhenDeviceChosenAsNone 
        {
            DeviceChosenAsNone_ChangeDevice,
            DeviceChosenAsNone_ForceGiveUp, //
            DeviceChosenAsNone_NoLongerSave,
            ENUM_FORCE_SIZE_32(6)
        };
        //accepted answers when prompt is displayed
        enum ChoiceWhenDeviceNoLongerValidForLoad 
        {
            DeviceNoLongerValidForLoad_ChangeDevice,
            DeviceNoLongerValidForLoad_ForceGiveUp, //
            DeviceNoLongerValidForLoad_CancelLoad,
            ENUM_FORCE_SIZE_32(7)
        };
        //accepted answers when prompt is displayed
        enum ChoiceWhenLoadError 
        {
            LoadError_DeleteFile, //
            LoadError_CancelLoad,
            ENUM_FORCE_SIZE_32(8)
        };
        //accepted answers when prompt is displayed
        enum ChoiceWhenLoadCantFind 
        {
            LoadCantFind_ChangeDevice,
            LoadCantFind_ForceGiveUp, //
            LoadCantFind_CancelLoad ,
            ENUM_FORCE_SIZE_32(9)
        };

        enum ChoiceWhenUserNotSignedIn
        {
            NotSignedIn_Retry,
            NotSignedIn_Continue,
            ENUM_FORCE_SIZE_32(10)
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        typedef void (*ShownHiddenPromptCallback_t)(PromptType, bbool _showing);
        typedef void (*DeviceChangedCallback_t)();
        typedef void (*OperationCompleteCallback_t)();
        typedef void (*EnableStatutCallback_t)(bbool enable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Construction
        Adapter_Savegame() : m_isSaving(bfalse), m_isLoading(bfalse), m_lastLoadSaveError(Error_Ok), m_chooseDevice_MinFreeSize(0), 
            m_deviceChangedCallback(NULL), m_currentOperationCompleteCallback(NULL), m_canSaveLoad(btrue), m_enableStatutCallback(NULL),
            m_operationCancelled(bfalse), m_saveCanBeDuplicated(btrue), m_dataCRC(0), m_codeCRC(0), m_disableSystemDefinitively(bfalse)
        {
            setDeviceChosenFlags(bfalse,bfalse); 
            for (int i=0; i<PromptType_Count; i++) m_shownHiddenPromptCallback[i]=NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Destruction
        virtual ~Adapter_Savegame() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// initialization
        virtual void init() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// termination
        virtual void term() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// choose a device. May display a dialog box. Asynchronous. Savegames are automatically enumerated.
        /// Check result with getIsDeviceChosenFlags() and getLastLoadSaveError()
        /// @param _playerIndex The signed in player
        /// @param _minFreeSize The minimum free size that must be available on device. Also max savegame size
        /// @param _isSilentWhenChosenNone If false, shows a ChoiceWhenDeviceChosenAsNone prompt
        /// @return error code (Error_Ok if no error). A typical error is Error_LoadOrSaveForbiddenForPlayer.
        virtual ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize = 0, bbool _isSilentWhenChosenNone = btrue) = 0;    

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this after pending device choice completed. Will return status
        /// @param _isChosen The device was chosen, and has enough room
        /// @param _isChosenAsNone The user did not want to select a device
        void getIsDeviceChosenFlags(bbool &_isChosen, bbool &_isChosenAsNone) {_isChosen = m_storageDeviceIsChosen; _isChosenAsNone = m_storageDeviceIsChosenAsNone;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegame
        virtual ErrorCode startSavegameEnumerationUpdate(u32 _playerIndex) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start asynchronous enumeration of savegames on every device
        virtual ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Asynchronous save.
        /// @param _playerIndex The player we save for. Controller port index (0-3 on XBOX)
        /// @param _baseName Basename (without extension) of the saved file/archive
        /// @param _displayName Display name of the file/archive
        /// @param _data Data to save
        /// @param _dataSize Size of this data
        /// @param _warnIfOverwrite btrue if reinforce overwrite checks (for ex when creating a new game)
        /// @return Error_Ok if ok so far, or Error_LoadOrSaveForbiddenForPlayer. Look at getLastLoadSaveErrorCode for asynchronous error
        virtual ErrorCode startSave(u32 _playerIndex, u32 _slotIndex, const String8 &_baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwriteForNewGame) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Asynchronously load savegame into m_currentSavegame (see getLastLoadedOrSavedData.)
        /// @param _playerIndex The player we load for. Controller port index (0-3 on XBOX)
        /// @param baseName Basename (without extension) of the file/archive to load
        /// @return Error_Ok or Error_LoadOrSaveForbiddenForPlayer. Look at getLastLoadSaveErrorCode for asynchronous error
        virtual ErrorCode startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get number of files/content/(...) we enumerated last time
        /// @param _playerIndex The player we enumerated for. Controller port index (0-3 on XBOX)
        virtual u32 getEnumeratedContentCount(u32 _playerIndex)  = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get details about enumerated files
        /// @param _playerIndex The player we enumerated for. Controller port index (0-3 on XBOX)
        /// @param _index
        /// @param _baseName
        /// @param _displayName
        virtual bbool getEnumeratedContent(u32 _playerIndex, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptyContent) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Accessor to data we loaded or saved last time
        const ITF_VECTOR<u8> &getLastLoadedOrSavedData() {return m_currentSavegame;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the error code for asynchronous operations
        ErrorCode getLastAsyncErrorCode() const {return m_lastLoadSaveError;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this to update asynchronous operations
        virtual void update() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// is there a pending async operation?
        virtual bbool hasPendingOperation() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// are we saving now?
        bbool isSaving() const {return m_isSaving;}


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Start first tasks 
        virtual void preLoad(u32 _player, u32 _maxPlayersToUse = 1, bbool _force = bfalse) {}
        virtual bbool isPreLoadingFailed() const { return bfalse; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        // CALLBACK FUNCTIONS


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set callback which will be called when a device is removed/inserted
        void setDeviceChangedCallback(DeviceChangedCallback_t _callback) {m_deviceChangedCallback = _callback;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set callback fired when prompt should be shown or hidden
        /// @param _promptType the prompt type 
        /// @param _callback the callback (null if none)
        void setShownHiddenPromptCallback(PromptType _promptType, ShownHiddenPromptCallback_t _callback) {m_shownHiddenPromptCallback[(int)_promptType] = _callback;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set callback which will be called after next completion of async operation(not called if the operation can't be started)
        /// Must be called after operation is initiated and successful. Automatically cleared.
        virtual void setCurrentOperationCompleteCallback(OperationCompleteCallback_t _callback) {m_currentOperationCompleteCallback = _callback;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set callback fired when the statut system is updated
        void setEnableStatutCallback(EnableStatutCallback_t _callback) {m_enableStatutCallback=_callback;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        // PROMPT FUNCTIONS

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwrite
        virtual bbool isAskingWhatToDoBecauseFileExist() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwrite(ChoiceWhenFileExists _choice) = 0;
        virtual void signalForDeleteCorruptedFile(ChoiceWhenDeleteingCorruptedFile _choice){}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system want you to answer with signalOkForOverwriteForNewGame
        virtual bbool isAskingWhatToDoBecauseFileExistForNewGame() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because of file overwrite, answer with this fn
        virtual void signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerDeviceNoLongerValidForSave
        virtual bbool isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device is no longer valid, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerSaveError
        virtual bbool isAskingWhatToDoBecauseOfSaveError() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When there was a file error while saving, answer with this function
        virtual void signalAnswerSaveError(ChoiceWhenSaveError _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if the system wants you to answer with signalAnswerChoseNoDevice
        virtual bbool isAskingWhatToDoBecauseOfChoseNoDevice() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// When Adapter_Savegame asks what to do because device selection was dismissed, answer with this fn
        virtual void signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because device was changed
        virtual bbool isAskingWhatToDoBecauseDeviceNoLongerValidForLoad() = 0;    
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///when Adapter_Savegame asks what to do because device was changed, answer with this fn
        virtual void signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because of a load error
        virtual bbool isAskingWhatToDoBecauseOfLoadError()  = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because of a load error, answer with this fn
        virtual void signalAnswerLoadError(ChoiceWhenLoadError _choice) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue when Adapter_Savegame asks what to do because it cant find a given savegame
        virtual bbool isAskingWhatToDoBecauseLoadCantFind() = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// when Adapter_Savegame asks what to do because it cant find a given savegame, answer with this fn
        virtual void signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice) = 0;
        virtual void signalAnswerWhenUserNotSignedIn(ChoiceWhenUserNotSignedIn _choice) {};

        virtual void checkForSaveStateValidity(u32 _playerIndex) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// platform like WII need to know at first boot (ie:init) the max size of the save
        virtual void prepareInit(u32 _minFreeSize) { m_chooseDevice_MinFreeSize = _minFreeSize; }
        virtual bbool checkDataCRCForCurrentSave(u32 _dataCRC) { return btrue; }
        ITF_INLINE void setCodeCRC(u32 _codeCRC) { m_codeCRC = _codeCRC; }
        ITF_INLINE void setDataCRC(u32 _dataCRC) { m_dataCRC = _dataCRC; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// System status
        void UseSaveSystem() { if(!m_disableSystemDefinitively) enableSaveSystem(); }
        void DoNotUseSaveSystem(bbool _definitively = bfalse) { disableSaveSystem(); if(_definitively) m_disableSystemDefinitively = btrue; }
        bbool IsSaveSystemEnable() const { return canUseSaveSystem(); }
        virtual bbool IsSaveProcessEnable() const { return IsSaveSystemEnable(); }

        bbool IsSaveFileCanBeDuplicated() const { return m_saveCanBeDuplicated; }
        void  setSaveFileCanBeDuplicated(bbool canBeDuplicated) { m_saveCanBeDuplicated = canBeDuplicated; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Cancel flag
        virtual void cancelAllOperations() {}
        bbool IsJustCancelled() const { return m_operationCancelled; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Device info
        virtual void GetSelectedDeviceName(String & deviceName){}


        enum ProfileSettings
        {
            ProfileSettings_Slot1=0,
            ProfileSettings_Slot2,
            ProfileSettings_Slot3
        };
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Profile save settings
        virtual bbool SaveProfileSettings(u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize){return bfalse;}
        virtual bbool LoadProfileSettings(u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize){return bfalse;}

    protected:
        void firePromptCallback(PromptType _prompt, bbool _show)
        {if (m_shownHiddenPromptCallback[_prompt]) m_shownHiddenPromptCallback[_prompt](_prompt, _show); }

        void setDeviceChosenFlags(bbool _done, bbool _chosenAsNone) {m_storageDeviceIsChosen=_done;m_storageDeviceIsChosenAsNone=_chosenAsNone;}

        bbool m_storageDeviceIsChosen;
        bbool m_storageDeviceIsChosenAsNone;
        bbool m_isSaving;
        bbool m_isLoading;
        bbool m_saveCanBeDuplicated;
        ErrorCode m_lastLoadSaveError;
        u32 m_chooseDevice_MinFreeSize;
        ITF_VECTOR<u8> m_currentSavegame;
        ShownHiddenPromptCallback_t m_shownHiddenPromptCallback[PromptType_Count];
        DeviceChangedCallback_t m_deviceChangedCallback;    
        OperationCompleteCallback_t m_currentOperationCompleteCallback;
        EnableStatutCallback_t m_enableStatutCallback;
        bbool m_canSaveLoad, m_operationCancelled;

        u32 m_dataCRC, m_codeCRC;
        bbool m_disableSystemDefinitively;

        // Just to handle fatal error
        void	disableSaveSystem() { if(m_enableStatutCallback) m_enableStatutCallback(bfalse); m_canSaveLoad = bfalse; }
        void	enableSaveSystem() { if(m_enableStatutCallback) m_enableStatutCallback(btrue); m_canSaveLoad = btrue; }
        bbool	canUseSaveSystem() const { return m_canSaveLoad; }
    };

}

#endif // _ITF_ADAPTER_SAVEGAME_H__
