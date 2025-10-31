#ifdef ITF_WII

#ifndef _ITF_ADAPTER_SAVEGAME_WII_H__
#define _ITF_ADAPTER_SAVEGAME_WII_H__

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef TPL_H
#include <revolution/tpl.h>
#endif //TPL_H

namespace ITF
{
	//Banner info
	#define WII_SAVEGAME_TITLE			L"Rayman Origins"
	#define WII_SAVEGAME_COMMENT		L"Ubisoft Entertainment"
	#define WII_SAVEGAME_BANNERFLAG 	NAND_BANNER_FLAG_ANIM_LOOP/*|NAND_BANNER_FLAG_NOTCOPY*/
	
	//DVDROOT path
	#define WII_SAVEGAME_BANNERPATH		"nand/nand.tpl"
	#define WII_SAVEGAME_SLOTPATH		"%s/slot_%d.sav"	// %s is the HomeDir, %d is the player index
	
	// NAND_FSBLOCK_SIZE
	#define WII_SAVEGAME_BANNER_FSBLOCK		2
	#define WII_SAVEGAME_BANNER_INODE		1
	
	#define WII_SAVEGAME_FILE_INODE			1	// one dir used
	
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// WIN32 version of savegame manager. 
    /// WARNING! THIS CODE IS ONLY TEMPORARY!
    class Adapter_Savegame_WII : public Adapter_Savegame
    {
    public:
        Adapter_Savegame_WII();
        virtual ~Adapter_Savegame_WII() {}

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

        virtual void setCurrentOperationCompleteCallback(OperationCompleteCallback_t _callback) {m_currentOperationCompleteCallback = _callback;}

        virtual u32 getEnumeratedContentCount(u32 /*_playerIndex*/) { return MAX_SLOTS; }
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


        virtual void preLoad(u32 _player, u32 _maxPlayersToUse = 1, bbool _force = bfalse); 

		// Only use this function if there is a good reason (corrupted file ?)
		// It will reset the save file !
		void recreate_files();
        bbool read_headers();

		i32 m_lastError;
		
    private:
        enum MainState 
        {
            State_Idle,
            ChooseDevice_PostedShowSelector, ChooseDevice_ShowingSelector,
            EnumeratingContent,
            Save_CreatingContent, Save_WritingFile,
            Load_WaitingBeforeIdle
        };
		enum OperationAsyncState
		{
			Op_Idle=0, 
			Op_Running,
			Op_Ended,
			Op_EndedWithError
		};

        struct Content
        {
            char szFileName[128];
            wchar_t szDisplayName[128];
        };

        void handle_MustChooseDevice();
        void handle_JustClosedDeviceSelectorGUI();
        void handle_AsynchronousContentEnumeration();
        void handle_Save_CreateContent();
        void handle_WritingSaveFile();
        void handle_Load_WaitingBeforeIdle();

        static void saveThreadRoutine(s32 _result, NANDCommandBlock* _commandBlock);
		static void loadThreadRoutine(s32 _result, NANDCommandBlock* _commandBlock);

        bbool startContentEnumeration(u32 _player);
        void setMainState(MainState _newState);
        static String8 getSavegamePath();

        void tests();

		String8 getPlayerSlotPath(u32 _slotIndex) const;
		String8 getPlayerTmpSlotPath(u32 _slotIndex) const;

		// Tools for first boot
		static String8 getBannerPath();
		static String8 getTmpBannerPath();
		bbool check_banner();
		bbool isSaveFileExists();
		bbool init_createBanner(TPLPalettePtr _pal);
		bbool init_createFiles();
        bbool init_createAllFiles();
        bbool init_checkSize(TPLPalettePtr _pal);

        void wait_forDVDDiscError();

        u32 getCurrentBannerSize() const;
        u32 getWantedBannerSize() const;

        u32 getCurrentFilesSize() const;
        u32 getWantedFilesSize() const;

        struct  Header
        {
            u16  displayedName [ 128 ];
            bbool emptySave;
            u32 fileSize;            
            u32 CodeCRC;
            u32 DataCRC;
        };
        static u32 getHeaderSize(); 

        bbool readHeader(u32 slot, Header* pHeader);
		
		bbool move2home(const String8& src);
		
		NANDCommandBlock m_commandBlock;
        HANDLE m_IOOperationThread;
		NANDFileInfo m_fileInfo;

        DWORD m_choseDevice_PlayerIndex;
        DWORD m_contentEnumeration_PlayerIndex;

        SafeArray<Header> m_filteredDeviceContent;
        volatile MainState m_mainState;
		volatile OperationAsyncState m_opState;
        int m_frameSinceLastStateChange;
        String8 m_loadSaveBaseName;
        bbool m_wasEnumerated;

		
		u8*			 m_workingbuff;
        Header       m_workingheader;
		static const u32 MAX_SLOTS = 3;
		
		// Helpers	
		void preparebuff ();
		static u32  buffSize();
        static u32  totalSize(); // buffer + header

		void buff2slot(u32 dataSize);
		void data2buff(const void* data, u32 dataSize);

        bbool m_internalDisable;
    };
    

#define SAVEGAME_ADAPTER_WII 	(static_cast< Adapter_Savegame_WII* > (SAVEGAME_ADAPTER))

}

#endif // _ITF_ADAPTER_SAVEGAME_WII_H__

#endif