#ifndef _ITF_TRCManagerAdapter_H_
#define _ITF_TRCManagerAdapter_H_

#pragma once

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_


namespace ITF
{
    // ---------------------------------------------------
    // TRC ADAPTER
    // ---------------------------------------------------

    // This adapter aims all generic TRC
    // How it works?
    //
    // If you want tot generate a message, just call TRCAdapter::addMessage(context, callback...)
    // The adapter will search for the best message to display
    // There is helpers too, like "checkForSpaceDisk", a call which is able to add a message


#define TRCMESSAGE_ERROR_NO			0
#define TRCMESSAGE_ERROR_INVALID	U32_INVALID
#define TRC_MAXPAD                  4
#define TRC_PAD_TIMEOUT_PS3         2.70

    // ----------------------------------------------------------------------------------
    // There
    class TRCMessage_Callback_Template
    {
    public:
        virtual ~TRCMessage_Callback_Template() = default;
        virtual bbool check()=0;
    };
    class TRCMessage_Callback_WaitforInput: public TRCMessage_Callback_Template
    {
    public:
        TRCMessage_Callback_WaitforInput(u32 button, u32 pad):
          _button(button), _pad(pad) {}
        virtual ~TRCMessage_Callback_WaitforInput(){}

        enum Button
        {
            But_Any
        };
        bbool check();
    private:
        u32 _button, _pad;
    };
    class TRCMessage_Callback_WaitforPadState: public TRCMessage_Callback_Template
    {
    public:
        enum PadState
        {
            connected,
            disconnected
        };

        TRCMessage_Callback_WaitforPadState(PadState state, u32 pad): _wantedState(state), _pad(pad) {}
        virtual ~TRCMessage_Callback_WaitforPadState(){}
        bbool check();
    private:
        u32 _pad;
        PadState _wantedState;
    };

    class TRCMessage_Callback_WaitSaveManager: public TRCMessage_Callback_Template
    {
    public:
        TRCMessage_Callback_WaitSaveManager(f64 _MinTimeToWait):
          m_minTimeToWait(_MinTimeToWait), m_timer(0.0) {}
        virtual ~TRCMessage_Callback_WaitSaveManager(){}
        bbool check();

    private:
        f64 m_minTimeToWait;
        f64 m_timer;
    };

	// ----------------------------------------------------------------------------------
    class TRCTask
    {
    public:
        TRCTask(bbool Async = bfalse)
            : _isAsync(Async), _isAlive(btrue), _isSleeping(bfalse), _name(Unknown), _isDisable(bfalse) {}

        virtual ~TRCTask(){}

        virtual void 	init(){}
        virtual void    run()=0;
        virtual void    onWakeUp(){}
        void   wakeup() { _isSleeping = bfalse; _isDisable = bfalse; onWakeUp(); }

        bbool    isAlive() const { return _isAlive; }
        bbool    isAsync() const { return _isAsync; }
        bbool    isSleeping() const { return _isSleeping; }
        bbool    isDisable() const { return _isDisable; }

        enum TaskName
        {
            Unknown=0,
            Pad,
        };

        TaskName getName() const { return _name; }
    protected:
        void    startSleeping() { _isSleeping = btrue; }

        bbool _isAsync;
        bbool _isAlive;
        bbool _isSleeping;
        bbool _isDisable;
        TaskName    _name;
    };

    class TRCPadTask : public TRCTask
    {
    public:
        TRCPadTask() : _frameToWait(0), _currentPlayerDisconnected(U32_INVALID) {
            ITF_MemSet(_lastDisconnection, 0, sizeof(_lastDisconnection)) ;
            _isSleeping = btrue;
            _isDisable = btrue;
            _name = Pad;
        }
        virtual ~TRCPadTask(){}
        void run();
        virtual void    onWakeUp();

    private:
        f64  _lastDisconnection[TRC_MAXPAD];
        u32  _currentPlayerDisconnected;
        u32  _frameToWait;
    };
	// ----------------------------------------------------------------------------------
    #define TRCMANAGER_MAXTASK      5
    // ----------------------------------------------------------------------------------
    class TRCHelper
    {
    public:
        virtual ~TRCHelper(){}

        virtual void buildPlayerField(const String8& _menuName, u32 _playerIndex, bbool _asEmpty)=0;
        virtual void buildMessageField(const String8& _menuName, const String& _message, bbool _asEmpty)=0;
        virtual void buildTitleField(const String8& _menuName, const String& _title, bbool _asEmpty)=0;
    };
    // ----------------------------------------------------------------------------------

    class TRCMessage_Base;
    class TRCMessage_Critical;
    typedef bbool (*TRCMessage_PreStartCallback_t)(TRCMessage_Base* pMessage, void* pParams);
    typedef void (*TRCMessage_OnClose_t)(const StringID & answer, TRCMessage_Base* pMessage, void* pParams);

    class TRCManagerAdapter: public TemplateSingleton<TRCManagerAdapter>
	{
    public:
        enum Answer
        {
            NoButton = 0,
            LeftButton, // If there is only one button, Left will be selected
            RightButton,
            MidButton
        };
        enum ErrorContext
        {
            GenericContexte = 0,
			RangeContexte	= 100,
			Gen_NoEnoughSpaceToPlay,

            // UOR
            UPC_ConnectionLost = 400,
            UOR_WelcomeMessage,
            UOR_WelcomeBackMessage,
            UOR_FirstPartyOffline,
            UOR_CreateSessionError, // linked account & account not locked
            UOR_LockedAccount,
            UOR_News,
            UOR_PleaseWait,

            Pad_enum		= 500,
            Pad_DisconnectingDuringTitleScreen,
            Pad_DisconnectingDuringGameplay,
            Pad_DisconnectingDuringMenu,
            Pad_ExtensionNunchukDetected,
            Pad_NoExtensionDetected,
            Pad_ExtensionClassicDetected,
            Pad_LowBattery,

            Sav_enum		= 600,
            Sav_CheckSpace,
            Sav_CheckSpaceBoot,
            Sav_NoEnoughSpaceBoot,
            Sav_NoEnoughSpace,
            Sav_NoEnoughInode,
            Sav_OperationRunning,
            Sav_ReadFiles,
            Sav_InitCreatingFiles,
            Sav_InitCreatingBanner,
            Sav_NoAvailableStorage,
            Sav_NewGameAskForOverwrite,
            Sav_SaveAskForOverwrite,
            Sav_AskForDelete,
            Sav_UserNotSignedIn,
            Sav_SaveAndExit,
            Sav_DeviceNoMoreAvailableForSave,
            Sav_DeviceNoMoreAvailableForLoad,
            Sav_FileNotAvailable,
            Sav_ErrorDuringSave,
            Sav_ErrorDuringLoad,
            Sav_CorruptedFile,
            Sav_NotOwnerOfTheSave,
            Sav_WarningBoot,

            IO_enum			= 700,
            IO_UnexpectedError,
            IO_Corrupted,
            IO_OpeningFile,
            IO_ReadingFile,
            IO_SeekingFile,
            IO_WritingFile,
            IO_DescriptingFile,
            IO_DeletingFile,
            IO_RenamingFile,

            // Rewards
            Rwd_enum        = 800,
            Rwd_CheckSpaceBoot,
            Rwd_NoEnoughSpace,

            //Language
            Language_enum   = 900,
            Language_Warn,


            // Online
        };

        enum IOError
        {
            IOErr_None=0,
            IOErr_Internal,
            IOErr_DiscNotFound,
            IOErr_FileNotFound
        };

        enum ExitValue
        {
            Exit_None=0,
			Exit_Shutdown, // Quit on PS3
            Exit_Restart,
			Exit_BackToMenu,
			Exit_BackToDataManagement,
        };

        struct TRCListener
        {
            TRCMessage_OnClose_t    _function;
            void*                   _params;
        };

	protected:
	    void _newMessageProcess();
        void _newTaskProcess();
        bbool _canDisplayMessage(bbool forcePauseMenu = bfalse) const;

		void _showCurrentMessage();
        virtual bbool _openUISystemMessage(TRCMessage_Critical* pMessage){return bfalse;}

		void _hideAndKillCurrentMessage();
        void _hideCurrentMessage();
        virtual bbool _buildAndAddMessage(ErrorContext errorContext){ITF_ASSERT(0);return false;}

        String buildText(u32 lineID, EContextIconType iconType = ContextIconType_Invalid, u32 padIndex = U32_INVALID);

        u32 _u32_customParam;
        String _string_customParam, _string_customParam2;
        TRCTask* _pTaskCaller;
        u32 _ResetCounter, _DisplayContentCounter;
        ErrorContext _lastErrorContext;
        bbool _forcePause, _hasPausedTheGame;
        bbool _warningBootMessageDisplayed;
        IOError _ioError;
        ExitValue _debugExitValue, _currentExitValue;
        bbool _fatalError;

        void _update_tasks();
        void _update_messages();
        void _clearCustomParams();

        typedef ITF_VECTOR<TRCListener> TRCListener_List;
        TRCListener_List _listenersOnClose;
        u32 _freeSpaceDiskKB, _neededSpaceDiskKB;

        // This manager handles a list of messages, and will display them in arriving order
        SafeArray<TRCMessage_Base*>	_messages;
        SafeArray<TRCTask*> _tasks;

        // Current message displayed
        TRCMessage_Base*			_currentMessage;
        static i32 OnFileOperationResultCallback(FileOperation op, i32 res);

        bbool _isInitialized, _enable;
        bbool _isDisplayingCorruptedSaveMsg;
	public:

		TRCManagerAdapter();
		virtual ~TRCManagerAdapter();  // always declare virtual destructor for adapters

        // Please call this when you want to start the boot check
        virtual void onFirstTRCFrame() {}

        // Check if a message is already in the queue
        // This function is called in addMessage()
        bbool    existsMessage(ErrorContext errorContext) const;

        // Add a new message to display, the TRCManagerAdapter will decides if we have to display the message
        // in-game or during the boot (with the UI System message)
        // @param: The context of your TRC check, ex: Sav_*, Rwd_*
        // @param: A pre-start callback called just before displaying the message, can be usefull to test the vaidity of your context
        // @param: parameter of the pre-start callback
        // @param: A onClose callback caleed just after closed the message
        // @param: parameter of the onclose callback
        bbool    addMessage(ErrorContext errorContext,
            TRCMessage_PreStartCallback_t preStartCallback = NULL, void* pParams = NULL,
            TRCMessage_OnClose_t onCloseCallback = NULL, void* pParamsOnClose = NULL);

        // Kill the current message displayed
        // Warning: use this method only if you have to...
        void    killCurrentMessage();

        // Add/remove a general listener for the "onclose" callback
        // Warning: if you add the same function on onclose callback with "addMessage", and with "addListener",
        // it will be called twice.
        void    addListener(TRCMessage_OnClose_t onCloseCallback, void* pParamsOnClose = NULL);
        bbool   removeListener(TRCMessage_OnClose_t onCloseCallback, void* pParamsOnClose = NULL);

        // You have the possibility to add some custom parameter with context who needs it.
        // See context doc for the list
        void    setCustomParam_u32(u32 param)       { _u32_customParam = param; }
        void    setCustomParam_String(String param) { _string_customParam = param; }
        void    setCustomParam_String2(String param){ _string_customParam2 = param; }

        // Add a new task (can be a platform specific one), each task is permanent until it is killed
        // A task is NOT a thread, its a "frame check", called by the TRCAdapater update (see bellow)
        bbool    addTask(TRCTask* pTask);

        // Allow the user to add a listener of the message kill, this listener is like the "onclose" callback,
        // but with a "TRCTask" and its "wakeup" callback.
        void    setTaskCaller(TRCTask* pTaskCaller) { _pTaskCaller = pTaskCaller; }

        // To Enable PAD check
        void    enablePadTask();
        bbool   isPadTaskEnable() const;
        bbool buildGenericMessage(ErrorContext errorContext);

        // Main process
        // All these functions are redefined in the children.
        // Call update each frame to in order to update the Tasks and the messages
		virtual void	update();
		virtual void	init();
        virtual void    term()=0;

        // Disable/Enable for messages
        ITF_INLINE  void disableMessagesListener() { _enable = bfalse; }
        ITF_INLINE  void enableMessagesListener() { _enable = btrue; }

        // Reset and Power control
        // Several functions can call the enable/disable function, a counter will be incremented
        // Be aware that a "disable" needs an "enable".
        virtual void enableRESETandPOWER()  { ITF_ASSERT(_ResetCounter > 0); _ResetCounter --; }
        virtual void disableRESETandPOWER() { _ResetCounter ++; }

        // Returns if we can display game content
        ITF_INLINE bbool canDrawContent() const { return _DisplayContentCounter == 0; }
        ITF_INLINE void  setCanDrawContent(bbool enableDraw) { if(enableDraw) _DisplayContentCounter --; else _DisplayContentCounter ++; }

        // Returns if there is a fatal error (disc error for ex on WII)
        ITF_INLINE bbool isAFatalErrorRunning() const { return _fatalError; }
        ITF_INLINE void setFatalErrorStatus(bbool fatalErrorStatus) { _fatalError =fatalErrorStatus; }

        // Returns the last error code set
        // The error code is not formated, see getlastErrorContext() to get the last context
        i32 getcurrentErrorCode() const;

        // TO avoid displaying the warning boot message several times
        ITF_INLINE bbool isWarningBootMessageDisplayed() const { return _warningBootMessageDisplayed; }
        ITF_INLINE void setWarningBootMessageIsDisplayed() { _warningBootMessageDisplayed = btrue; }

        // Returns the last error context set
        // If you want to reset the error (which is stored here until a new error is displayed)
        // please see "resetLastErrorContext"
        ITF_INLINE ErrorContext getlastErrorContext() const { return _lastErrorContext; }

        // Allow the user to reset the last context after used it
        ITF_INLINE void  resetLastErrorContext() { _lastErrorContext = GenericContexte; }

        // Check we are displaying an error (a message can be alone in the list, and not being displayed)
        bbool isDisplayingError() const;

        ITF_INLINE bbool isDisplayingCorruptedSave(){ return _isDisplayingCorruptedSaveMsg;}

        // Register a TRCManager to fill some fields depending on the gameplay
        void registerTRCHelper(TRCHelper* trcHelper) { _trcHelper = trcHelper; }
        void unRegisterTRCHelper() { registerTRCHelper(NULL); }
        TRCHelper* getTRCHelper() { return _trcHelper; }

        // ----------------------------------------------------------------------------
        // Configuration: set/get needed parameters
        // -> Warning: they add a message if they need it
        // ----------------------------------------------------------------------------
        ITF_INLINE void setFreeSpaceDiskKB(u32 kbytes) { _freeSpaceDiskKB = kbytes; }
        ITF_INLINE u32  getFreeSpaceDiskKB() const { return _freeSpaceDiskKB; }

        // Returns if if can display the start menu of the game
        // Its very important to check the status here of all required mudules
        virtual bbool isBootRequirementsFinished() { return btrue; }

        // ----------------------------------------------------------------------------
        // Few helpers now
        // -> Warning: they can add a message if they need it
        // ----------------------------------------------------------------------------

        // Check the space disk, and add a message following the errorContext received
        // Need a setFreeSpaceDiskKB() before using them
        virtual bbool checkSpaceDisk(ErrorContext errorContext, u32 fsblock, u32 inode){return bfalse;}  // WII
        virtual bbool checkSpaceDisk(ErrorContext errorContext, u32 bytes){return bfalse;}               // Other

        // Return if there is a handled error during a HDD operation
        virtual bbool checkIOError(ErrorContext errorContext, i32& _errorCode){return btrue;}

        // Modify the exit value following the debugExitValue set, and the current platform
        // And set the current value in currentExitValue (see getCurrentExitValue)
        virtual void checkExitCode(i32& _exitCode){}

        // Returns the current exit value of the system, which has been set by the checkExitCode()
        ITF_INLINE ExitValue getCurrentExitValue() const { return _currentExitValue; }
        ITF_INLINE bbool isExitRequested() const { return _currentExitValue != Exit_None; }
        ITF_INLINE void setCurrentExitValue(ExitValue _exitValue) { _currentExitValue = _exitValue; }

        // At next IO check, an error will be generated
        ITF_INLINE void debugSetIOError(IOError ioError) { _ioError = ioError; }
        ITF_INLINE void debugSetExitValue(ExitValue exitValue) { _debugExitValue = exitValue; }

        // Static calls
        ITF_INLINE bbool isASaveError(const ErrorContext e) const { return (e >=Sav_enum && e <= Sav_enum+RangeContexte); }
		ITF_INLINE bbool isAPadError(const ErrorContext e) const { return (e >=Pad_enum && e <= Pad_enum+RangeContexte); }
        ITF_INLINE bbool isAnIOError(const ErrorContext e) const { return (e >=IO_enum && e <= IO_enum+RangeContexte); }

        // Listeners helpers
        ITF_INLINE u32 getListenersCount() const { return _listenersOnClose.size(); }
        ITF_INLINE TRCListener* getListener(u32 index) { if(getListenersCount()>index) return & _listenersOnClose[index]; return NULL; }

        // Save notification status
        virtual bbool  isSaveNotificationEnable() const { return btrue; }

        // Helpers to pause engine sounds
        void pauseGame();
        void resumeGame();
    private:

        u32         m_PausedCounter;

        friend class TRCMessage_Base;
        TRCHelper* _trcHelper;
	};

    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------
    class TRCMessage_Base: public UIMenuManager::MenuItemActionListener
    {
    public:
    	enum SystemPriority
    	{
    		HighPriority = 0,		// Disable RESET and POWER
    		NormalPriority
    	};

        // If a hight priority is added, it will be displayed even if a message is active.
        enum DisplayPriority
        {
            Low = 0,
            Normal,
            High
        };

    protected:
        String	_title, _message;
        bbool	_useLocBase;
        bbool	_isAlive;
        bbool	_isWaiting;
        bbool   _forceOverPauseMenu;
        bbool   _canBeDupplicated;
        SystemPriority	_systemPriority;
        DisplayPriority _displayPriority;
        i32		_errorCode;
        String8	_baseName;
        TRCTask* _pTaskCaller;
        u32     _activePlayer, _allowedPlayer;

        bbool   _useUIMessageSystem;
        u32     _frameDisplayed;

        // callback to check if we still can display the message
        TRCMessage_PreStartCallback_t   _preStartCallback;
        TRCMessage_OnClose_t            _onCloseCallback;
        void*   _pParamsPreStart;
        void*   _pParamsOnClose;

        void        callOnCloseCallback(const StringID answer = StringID::Invalid);

    public:

        TRCMessage_Base(String8 baseName = "",
            TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : _ErrorContext(errorContext), _errorCode(errorCode), _baseName(baseName), _useLocBase(true), _pTaskCaller(NULL), _isWaiting(true),
            _preStartCallback(NULL), _pParamsPreStart(NULL),
            _onCloseCallback(NULL), _pParamsOnClose(NULL), _forceOverPauseMenu(bfalse),
            _isAlive(btrue), _systemPriority(NormalPriority), _activePlayer(U32_INVALID), _allowedPlayer(U32_INVALID),
            _useUIMessageSystem(bfalse), _frameDisplayed(0), _displayPriority(Normal), _canBeDupplicated(bfalse)
        {}
        virtual ~TRCMessage_Base(){}

        void        setOnCloseCallback(TRCMessage_OnClose_t callback, void* pParams) { _onCloseCallback = callback; _pParamsOnClose = pParams; }
        void        setPreStartCallback(TRCMessage_PreStartCallback_t callback, void* pParams) { _preStartCallback = callback; _pParamsPreStart = pParams; }
        void	    forceTexts(const String& title, const String& message);

		ITF_INLINE bbool isWaiting() const { return _isWaiting; }
        ITF_INLINE void forceWaiting() { _isWaiting = btrue; }
        ITF_INLINE bbool isAlive() const { return _isAlive; }
        void		kill();

        ITF_INLINE void changeDisplayPriority(DisplayPriority priority) { _displayPriority = priority; }
        ITF_INLINE DisplayPriority getDisplayPriority() const { return _displayPriority; }

        ITF_INLINE i32 error() const { return _errorCode; }
        ITF_INLINE const String8&	name() const { return _baseName; }
        ITF_INLINE const String&  message() const { return _message; }

        void        forceName(const String8& name) { _baseName = name; }
        ITF_INLINE u32 getActivePlayer() const { return _activePlayer; }
        ITF_INLINE void setActivePlayer(u32 _player) { _activePlayer = _player; }

        ITF_INLINE u32 getAllowedPlayer() const { return _allowedPlayer; }
        ITF_INLINE void setAllowedPlayer(u32 _player) { _allowedPlayer = _player; }

        ITF_INLINE void setCanBeDupplicated(bbool canBeDupplicated) { _canBeDupplicated = canBeDupplicated; }
        ITF_INLINE bbool getCanBeDupplicated() const { return _canBeDupplicated; }

        TRCManagerAdapter::ErrorContext   getContexte() const { return _ErrorContext; }
        virtual void onMenuItemAction (UIComponent* _UIComponent){}
        virtual StringID onMenuPageAction(UIMenu * /*_menu*/, const StringID & /*_key*/, const StringID &_defaultAction);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// this callback is run when the users does an action which is related to a whole menu page
        /// @param _menu the current menu page
        /// @param _key the key which caused the action
        /// @param _defaultAction default action, among predefined actions (see getMenuPageAction_*() )
        //virtual StringID onMenuPageAction(UIMenu * /*_menu*/, int /*_key*/, const StringID &_defaultAction) {return _defaultAction;}

        virtual void	update()=0;
        virtual bbool	start();

        ///////////////////////////////////////////////////////////////////////////////////////////
		bbool       isUsingUIMessageSystem() const { return _useUIMessageSystem; }
        SystemPriority getSystemPriority() const { return _systemPriority; }

        void SetTaskCaller(TRCTask* pTaskCaller) { _pTaskCaller = pTaskCaller; }
        void SetForceOverPauseMenu() { _forceOverPauseMenu = true; }
        bbool CanForcePauseMenu() const { return _forceOverPauseMenu; }
    protected:
        TRCManagerAdapter::ErrorContext	_ErrorContext;

        bbool       checkStartCallback();
        void        setUseUIMessageSystem(bbool useUIMessageSystem) { _useUIMessageSystem = useUIMessageSystem; }
    };

    // TRC message displayed during boot
    class TRCMessage_Critical : public TRCMessage_Base
    {
    public:
        enum UISystemButtons
        {
            UI_NoButton = 0,
            UI_YesNoButton,
            UI_OkButton
        };


        TRCMessage_Critical(
            UISystemButtons systemButtons,

            TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_Base("", errorContext, errorCode), _systemButtons(systemButtons)
            , _backgroundEnable(btrue), _soundEnable(btrue), _backButtonEnable(btrue), _secondsTimeOut(0.0f)
        {
            setUseUIMessageSystem(btrue);
            _systemPriority = HighPriority;
        }
        virtual ~TRCMessage_Critical(){}

        void	update();
        bbool	start();

        ITF_INLINE void    setTimeOut(f32 _delayInSec) { _secondsTimeOut = _delayInSec; }
        ITF_INLINE void    disableSound ()      { _soundEnable = bfalse; }
        ITF_INLINE void    disableBackground () { _backgroundEnable = bfalse; }
        ITF_INLINE void    disableBackButton () { _backButtonEnable = bfalse; }
        ITF_INLINE UISystemButtons getUISystemButtons() const { return _systemButtons; }

        ITF_INLINE bbool    isSoundEnable() const { return _soundEnable; }
        ITF_INLINE bbool    isBackgroundEnable() const { return _backgroundEnable; }
        ITF_INLINE bbool    isBackButtonEnable() const { return _backButtonEnable; }
        ITF_INLINE f32      getTimeOut() const { return _secondsTimeOut; }
    protected:
        UISystemButtons _systemButtons;
        bbool       _backgroundEnable, _soundEnable, _backButtonEnable;
        f32         _secondsTimeOut;
    };

    class TRCMessage_NoButtonTimer : public TRCMessage_Base
    {
    public:
        TRCMessage_NoButtonTimer(f64 timer, TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_Base("TRCMessage_NoButtonTimer", errorContext, errorCode), _timer(timer), _openingTime(0.0)
        {
        }
        virtual ~TRCMessage_NoButtonTimer(){}

        void	update();
        bbool	start();

    protected:
        f64		_timer, _openingTime;
    };

    class TRCMessage_NoButtonCallback : public TRCMessage_Base
    {
    public:
        TRCMessage_NoButtonCallback(TRCMessage_Callback_Template* pCallback, TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_Base("TRCMessage_NoButtonTimer", errorContext, errorCode), _pCallback(pCallback)
        {
        }
        virtual ~TRCMessage_NoButtonCallback();

        virtual void	update();

    protected:
        TRCMessage_Callback_Template* _pCallback;
    };
    class TRCMessage_OneButton : public TRCMessage_Base
    {
    public:
        TRCMessage_OneButton(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_Base("TRCMessage_OneButton", errorContext, errorCode), _buttonKey(m_joyButton_A)
        {
        }
        virtual ~TRCMessage_OneButton(){}

        virtual void	update();
        virtual bbool	start();

        void    setButton(const String& button, u32 buttonKey ) { _button = button; _buttonKey = buttonKey; }
        virtual void onMenuItemAction (UIComponent* _UIComponent);
        virtual StringID onMenuPageAction(UIMenu * /*_menu*/, const StringID & /*_key*/, const StringID &_defaultAction);
    protected:
        String _button;
        u32 _buttonKey;
    };

    class TRCMessage_OneButtonWithCB: public TRCMessage_OneButton
    {
    public:
        TRCMessage_OneButtonWithCB(
            TRCMessage_Callback_Template* pCallback,
            TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte,
            i32 errorCode = TRCMESSAGE_ERROR_NO) :
                    _pCallback(pCallback), TRCMessage_OneButton(errorContext, errorCode){}
        virtual ~TRCMessage_OneButtonWithCB(){ SF_DEL(_pCallback); }

        virtual void	update()
        {
            TRCMessage_OneButton::update();
            if(_pCallback)
            {
                if (_pCallback->check())
                {
                    kill();
                    callOnCloseCallback();
                }
            }

        }
        virtual bbool	start()
        {
            return TRCMessage_OneButton::start();
        }
    private:
        TRCMessage_Callback_Template* _pCallback;
    };

    #define s_buttonKeyLeft ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_LEFT,1255480269)
    #define s_buttonKeyRight ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_RIGHT,692652888)
    #define s_buttonKeyMid ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_MID,1570235137)

    class TRCMessage_TwoButton : public TRCMessage_Base
    {
    public:
        TRCMessage_TwoButton(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_Base("TRCMessage_TwoButton", errorContext, errorCode),
            _buttonKeyL(s_buttonKeyLeft), _buttonKeyR(s_buttonKeyRight), _answer(TRCManagerAdapter::NoButton)
        {}
        virtual ~TRCMessage_TwoButton(){}
        virtual void	update(){}
        bbool	start();

        void    setLeftButton(const String& _button, const StringID& _buttonKey ) { _buttonL = _button; _buttonKeyL = _buttonKey; }
        void    setRightButton(const String& _button, const StringID& _buttonKey ) { _buttonR = _button; _buttonKeyR = _buttonKey; }

        virtual void onMenuItemAction (UIComponent* _UIComponent);
        virtual StringID onMenuPageAction(UIMenu * /*_menu*/, const StringID &_action /*_key*/, const StringID &_defaultAction);

        virtual void onLeftButtonAction() {  }
        virtual void onRightButtonAction(){  }
    protected:
        String _buttonL, _buttonR;
        StringID _buttonKeyL, _buttonKeyR;
        TRCManagerAdapter::Answer   _answer;
    };

    class TRCMessage_ThreeButton : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_ThreeButton(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_TwoButton(errorContext, errorCode), _buttonKeyM(s_buttonKeyMid)
        {
            forceName("TRCMessage_ThreeButton");
        }
        virtual ~TRCMessage_ThreeButton(){}

        void	update();
        bbool	start();

        void    setMidButton(const String8& _button, const StringID& _buttonKey ) { _buttonM = _button; _buttonKeyM = _buttonKey; }

        virtual StringID onMenuPageAction(UIMenu * /*_menu*/, const StringID &_action /*_key*/, const StringID &_defaultAction);

        virtual void onMidButtonAction() {  }
    private:
        String8 _buttonM;
        StringID _buttonKeyM;
    };

    // ---------------------------------------------------
    // TRCTools,
    // Few utils tools used by TRC
    // ---------------------------------------------------
    class TRCTools
    {
    public:
        ITF_INLINE static u32 Bytes2KBytes(u32 bytes) { return ((bytes+1023)/1024); }
    };

    // ---------------------------------------------------


    // Debug Task
#ifndef ITF_FINAL
    class TRCTask_DebugIO : public TRCTask
    {
    public:
        void run();
    };

    class TRCTask_DebugExit : public TRCTask
    {
    public:
        void run();
    };
#endif //ITF_FINAL

    #define TRC_ADAPTER         TRCManagerAdapter::getptr()
} // namespace ITF

#endif //_ITF_TRCManagerAdapter_H_
