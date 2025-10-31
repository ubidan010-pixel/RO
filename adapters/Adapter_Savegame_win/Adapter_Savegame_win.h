#ifndef _ITF_ADAPTER_SAVEGAME_WIN_H__
#define _ITF_ADAPTER_SAVEGAME_WIN_H__

#pragma once

#ifdef ITF_WINDOWS

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_


namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// WINDOWS version of savegame manager. 
    /// WARNING! THIS CODE IS ONLY TEMPORARY! ... 15 years later, still temporary...
    class Adapter_Savegame_win : public Adapter_Savegame
    {
    public:
        Adapter_Savegame_win();
        virtual ~Adapter_Savegame_win() {}

        virtual void init();
        virtual void term();


        virtual bbool hasPendingOperation() {return m_mainState!=State_Idle;}

        ///NB : LOOK AT UNSIGNED PROFILES IN GAMES : HOW SAVE IS HANDLED?

        ErrorCode startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone);
        ErrorCode startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _warnIfOverwrite);
        ErrorCode startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName);
        ErrorCode startSavegameEnumerationUpdate(u32 _player);
        ErrorCode startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex) {return startSavegameEnumerationUpdate(_playerIndex);}
        virtual void update();
        i32 findBasenameAmongEnumerated(const String8 &_baseName);
        virtual u32 getEnumeratedContentCount(u32 /*_playerIndex*/) {ITF_ASSERT(!hasPendingOperation());  return m_filteredDeviceContent.size();}
        virtual bbool getEnumeratedContent(u32 _playerIndex, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave);

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

        bbool checkDataCRCForCurrentSave(u32 _dataCRC) ;
    private:
        enum MainState 
        {
            State_Idle,
            ChooseDevice_PostedShowSelector, ChooseDevice_ShowingSelector,
            EnumeratingContent,
            Save_CreatingContent, Save_WritingFile,
            Load_WaitingBeforeIdle,
            ENUM_FORCE_SIZE_32(0)
        };


        struct Header
        {
            wchar_t SaveName[MAX_PATH];   
            u32 CodeCRC;
            u32 DataCRC;
        };
        struct Content
        {
            char szFileName[MAX_PATH];
            Header sHeader;
        };

        void handle_MustChooseDevice();
        void handle_JustClosedDeviceSelectorGUI();
        void handle_AsynchronousContentEnumeration();
        void handle_Save_CreateContent();
        void handle_WritingSaveFile();
        void handle_Load_WaitingBeforeIdle();

        static DWORD WINAPI saveThreadRoutine(void *_param);

        bbool startContentEnumeration(u32 _player);
        void setMainState(MainState _newState);
        static String8 getSavegamePath(const String8 &_filename);

        void tests();

        HANDLE m_IOOperationThread;

        DWORD m_choseDevice_PlayerIndex;
        DWORD m_contentEnumeration_PlayerIndex;

        SafeArray<Content> m_filteredDeviceContent;
        volatile MainState m_mainState;
        int m_frameSinceLastStateChange;
        String8 m_loadSaveBaseName;
        String  m_displayedName; 
        bbool m_wasEnumerated;


        bbool readHeader(String8& filePath, Header* pHeader);        
        u32   getHeaderSize() const;
    };

}

#endif // ITF_WINDOWS

#endif // _ITF_ADAPTER_SAVEGAME_WIN_H__
