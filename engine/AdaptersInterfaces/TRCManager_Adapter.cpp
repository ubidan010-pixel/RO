#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_
#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_


namespace ITF
{
//#define USE_DEBUG_TRCTASK
#define LOG_TRC(v)  LOG("[TRC]%s", v)
//#define LOG_TRC(v)

    // ----------------------------------------------------------
    // TRCMessage_Callback_WaitforPadState ----------------------
    // ----------------------------------------------------------
    bbool TRCMessage_Callback_WaitforPadState::check()
    {
        u32 padIndexStart, padIndexCount;
        bbool returnedValue = bfalse;
        if(_pad == U32_INVALID)
        {
            padIndexStart = 0;
            padIndexCount = GAMEMANAGER->getMaxPlayerCount();
        } else
        {
            padIndexStart = _pad;
            padIndexCount = _pad+1;
        }

        for(u32 i = padIndexStart; i < padIndexCount; i++)
        {
            bbool isPadConnected =  INPUT_ADAPTER->isPadConnected(i);
            if ( _wantedState == connected && isPadConnected)
                returnedValue = btrue;
            else if ( _wantedState == disconnected && !isPadConnected)
                returnedValue = btrue;
        }
        
        return returnedValue;
    }

    // ----------------------------------------------------------
    // TRCMessage_Callback_WaitSaveManager ----------------------
    // ----------------------------------------------------------
    bbool TRCMessage_Callback_WaitSaveManager::check()
    {
        if(!m_timer)
            m_timer = SYSTEM_ADAPTER->getTime();

        if (m_timer + m_minTimeToWait > SYSTEM_ADAPTER->getTime())
        {
            return bfalse;
        }

        return !SAVEGAME_ADAPTER->hasPendingOperation();
    }
    
	// ----------------------------------------------------------
	// TRCMessage_Base --------------------------------------
	// ----------------------------------------------------------
	void TRCMessage_Base::forceTexts(const String& title, const String& message) 
	{ 
		_message	= message; 
		_title		= title;
		_useLocBase = bfalse;
	}
	void TRCMessage_Base::kill()
	{
		_isAlive = bfalse;
        if(_pTaskCaller)
        {
            _pTaskCaller->wakeup();
            _pTaskCaller = NULL;
        }

        if(TRC_ADAPTER->getTRCHelper())
        {
            TRC_ADAPTER->getTRCHelper()->buildPlayerField(_baseName, _activePlayer, btrue);
        }
	}
    bbool TRCMessage_Base::checkStartCallback()
    {
        if ( _preStartCallback )
        {
            if (!_preStartCallback (this, _pParamsPreStart))
            {
                kill();
                callOnCloseCallback();
                return bfalse;
            }
        }
        return btrue;
    }
	bbool TRCMessage_Base::start() 
	{ 
        if ( !checkStartCallback() )
            return bfalse;

		_frameDisplayed = CURRENTFRAME;
		_isWaiting = bfalse;

        if(TRC_ADAPTER->getTRCHelper())
        {
            TRC_ADAPTER->getTRCHelper()->buildMessageField(_baseName, _message, bfalse);
            TRC_ADAPTER->getTRCHelper()->buildTitleField(_baseName, _title, bfalse);
            TRC_ADAPTER->getTRCHelper()->buildPlayerField(_baseName, _activePlayer, bfalse);
        }
        return btrue;
	}

    StringID TRCMessage_Base::onMenuPageAction(UIMenu * /*_menu*/, const StringID & /*_key*/, const StringID &_defaultAction)
    {
        return UI_MENUMANAGER->getMenuPageAction_Nothing();
    }

    void TRCMessage_Base::callOnCloseCallback(const StringID answer)
    {
        if(_onCloseCallback)
            _onCloseCallback(answer, this, _pParamsOnClose);

        u32 ListenersCount = TRC_ADAPTER->getListenersCount();
        for (u32 i = 0 ; i < ListenersCount; i++)
        {
            TRCManagerAdapter::TRCListener*  listener = TRC_ADAPTER->getListener(i);
            if (listener->_function)
                listener->_function ( answer, this, listener->_params );
        }
    }

	// ----------------------------------------------------------
	// TRCMessage_Critical --------------------------------------
	// ----------------------------------------------------------
	bbool TRCMessage_Critical::start()
	{
		if(!TRCMessage_Base::start()) return bfalse;
        
		InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        ITF_MemSet(buttons, 0, sizeof(buttons));

		while(isAlive())
		{
            // Not used for now
			kill();
		}
        return btrue;
	}
	void TRCMessage_Critical::update()
	{

	
	}

    // ----------------------------------------------------------
    // TRCMessage_NoButtonCallback ------------------------------
    // ----------------------------------------------------------
    void  TRCMessage_NoButtonCallback::update()
    {
        if(_pCallback)
        {
            if (_pCallback->check())
            {
                kill();
                callOnCloseCallback();
            }
        }
    }
    TRCMessage_NoButtonCallback::~TRCMessage_NoButtonCallback()
    {        
        SF_DEL(_pCallback)
    }

	// ----------------------------------------------------------
	// TRCMessage_NoButtonTimer ---------------------------------
	// ----------------------------------------------------------
	bbool TRCMessage_NoButtonTimer::start()
	{
		if (TRCMessage_Base::start())
        {
            _openingTime = SYSTEM_ADAPTER->getTime();
            return btrue;
        }
        return bfalse;
	}

	void  TRCMessage_NoButtonTimer::update()
	{
		if(_openingTime + _timer < SYSTEM_ADAPTER->getTime())
		{
			kill();
            callOnCloseCallback();
		}
	}
	// ----------------------------------------------------------
	// TRCMessage_OneButton -------------------------------------
	// ----------------------------------------------------------
	bbool TRCMessage_OneButton::start()
	{
		if(TRCMessage_Base::start())
        {
            const static StringID s_TemplateButton("template_button");
            UI_MENUMANAGER->changeMenuItemFriendlyByID(_baseName, s_TemplateButton, _button);
            return btrue;
        }
        return bfalse;
	}
	void TRCMessage_OneButton::update()
	{

	}

    StringID TRCMessage_OneButton::onMenuPageAction(UIMenu * _menu, const StringID &_action, const StringID &_defaultAction) 
    {
        if( _action != input_actionID_Valid )
        {
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        if(!_button.isEmpty() && _frameDisplayed != CURRENTFRAME) // Don't kill it at same frame 
        {
            kill();
            callOnCloseCallback();
        }
        return UI_MENUMANAGER->getMenuPageAction_Nothing();
    }

	void TRCMessage_OneButton::onMenuItemAction(UIComponent* _UIComponent)
	{
        LOG_TRC("onMenuItemAction");
		if (_UIComponent->getIsSelected())
		{
			kill();
            callOnCloseCallback();
		}
	}
    // ----------------------------------------------------------
    // TRCMessage_TwoButton -------------------------------------
    // ----------------------------------------------------------
    bbool TRCMessage_TwoButton::start()
    {
        if (TRCMessage_Base::start())
        {
            const static StringID s_TemplateButtonLeft("template_button_left");
            const static StringID s_TemplateButtonRight("template_button_right");

            UI_MENUMANAGER->changeMenuItemFriendlyByID(_baseName, s_TemplateButtonLeft, _buttonL);            
            UI_MENUMANAGER->changeMenuItemFriendlyByID(_baseName, s_TemplateButtonRight, _buttonR);

            UIMenu* menu = UI_MENUMANAGER->getMenu(_baseName);
            if(menu)
                menu->reinitMenuSelection();
            
            return btrue;
        }
        return bfalse;        
    }

    StringID TRCMessage_TwoButton::onMenuPageAction(UIMenu * _menu, const StringID &_action, const StringID &_defaultAction) 
    {
        // Wait a little before taking input
        if( _action != input_actionID_Valid  || (_frameDisplayed+30 >= CURRENTFRAME))
        {
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        }

        StringID currentButtonID ; 
        if(_menu)
        {
            UIComponent* pUIComponent = _menu->getUIComponentSelected();
            if(pUIComponent) currentButtonID = pUIComponent->getID();
        }

        if(currentButtonID == ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_RIGHT,692652888))
        {
            onRightButtonAction();
            _answer = TRCManagerAdapter::RightButton;          
        }
            
        else if(currentButtonID == ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_LEFT,1255480269))
        {
            onLeftButtonAction();
            _answer = TRCManagerAdapter::LeftButton;        
        }	        

        if(_answer != TRCManagerAdapter::NoButton)
        {
            callOnCloseCallback(_action);            
            kill();
        }

        return UI_MENUMANAGER->getMenuPageAction_Nothing();
    }

    void TRCMessage_TwoButton::onMenuItemAction(UIComponent* _UIComponent)
    {
        if (_UIComponent->getIsSelected())
        {
        //    kill();
        }
    }

    // ----------------------------------------------------------
    // TRCMessage_ThreeButton -----------------------------------
    // ----------------------------------------------------------
    StringID TRCMessage_ThreeButton::onMenuPageAction(UIMenu * _menu, const StringID &_action, const StringID &_defaultAction) 
    {
        if(_action == _buttonKeyM)
        {
            onMidButtonAction();
            callOnCloseCallback(_action);
            kill();
        }

        return TRCMessage_TwoButton::onMenuPageAction(_menu, _action, _defaultAction);
    }    

    bbool TRCMessage_ThreeButton::start()
    {
        if (TRCMessage_TwoButton::start())
        {
            const static StringID s_TemplateButtonMid("template_button_mid");
            UI_MENUMANAGER->changeMenuItemFriendlyByID(_baseName, s_TemplateButtonMid, _buttonM);
            return btrue;
        }
        return bfalse;        
    }
    void TRCMessage_ThreeButton::update()
    {
        TRCMessage_TwoButton::update();
    }
    // ----------------------------------------------------------
    // TRCPadTask -----------------------------------------------
    // ----------------------------------------------------------
    void TRCPadTask::run()
    {
        if (LOADINGSCREEN->isVisible())
            return ; 
   
        const u32 mainPlayer = GAMEMANAGER->getMainIndexPlayer_Internal();
        if(_frameToWait)
        {
            _frameToWait--;
            return;
        }

        u32 firstIndexPlayer, countPlayers;
        TRCManagerAdapter::ErrorContext err;
        bbool atLeastOneConnected = bfalse;
        // InGameplay ? we have to test all the controllers
        if ( GAMEMANAGER->isPlayableScreen() )
        {
            firstIndexPlayer = 0 ;
            countPlayers = GAMEMANAGER->getNumActiveAndPersistent();
            err = TRCManagerAdapter::Pad_DisconnectingDuringGameplay;
        } else if(mainPlayer == U32_INVALID )
        {
            firstIndexPlayer = 0;
            countPlayers = GAMEMANAGER->getMaxPlayerCount();
            atLeastOneConnected = btrue;
            err = TRCManagerAdapter::Pad_DisconnectingDuringTitleScreen;
#if defined (ITF_WII) || defined (ITF_X360)
            countPlayers = 0;   // We dont have to handle the deco in the menus here
#endif
        }
        else
        {
            firstIndexPlayer = mainPlayer;
            countPlayers = firstIndexPlayer+1;
            err = TRCManagerAdapter::Pad_DisconnectingDuringMenu;
#if defined (ITF_WII) || defined (ITF_X360)
            countPlayers = 0;   // We dont have to handle the deco in the menus here
#endif
        }
        
        u32 connectedPadCount=0;
        for(u32 player = firstIndexPlayer; player < countPlayers; player++)
        {            
            Player* pPlayer = GAMEMANAGER->isPlayableScreen() ? GAMEMANAGER->getPlayerFromActiveIndex(player): GAMEMANAGER->getPlayer(player);
            if(!pPlayer)
                continue;
            if(pPlayer->getIsDisconnected())
                continue;

            u32 padIndex = pPlayer->getIndex();
            bbool isPadConnected = INPUT_ADAPTER->isPadConnected(padIndex);
            if(isPadConnected)
            {
                connectedPadCount++;
            }
            else 
            {
                if(atLeastOneConnected 
                    && 
                   (player != (countPlayers-1) || connectedPadCount > 0 ) )
                isPadConnected = btrue; // force to ignore status
            }
            
            
            if (!isPadConnected)
            {
                // Check the time the pad is disconnected
                if(!_lastDisconnection[padIndex])
                    _lastDisconnection[padIndex] = SYSTEM_ADAPTER->getTime();
    #ifdef ITF_PS3
                if ( SYSTEM_ADAPTER->getTime() - _lastDisconnection[padIndex] > TRC_PAD_TIMEOUT_PS3)
    #endif //ITF_PS3
                {
                    if ( GAMEMANAGER->isPlayableScreen() && mainPlayer == U32_INVALID )
                        continue; 

                    _lastDisconnection[padIndex] = 0;
                    _currentPlayerDisconnected = atLeastOneConnected ? U32_INVALID: padIndex;
                    TRC_ADAPTER->setCustomParam_u32(padIndex);   // pad
                    TRC_ADAPTER->setTaskCaller(this);
                    TRC_ADAPTER->addMessage(err);
                    startSleeping();
                    break;
                }           
            } else if ( _lastDisconnection[padIndex] != 0 )
                _lastDisconnection[padIndex] = 0;
        }
    }

    void TRCPadTask::onWakeUp()
    {
        if(_currentPlayerDisconnected!=U32_INVALID)
        {
            if (!INPUT_ADAPTER->isPadConnected(_currentPlayerDisconnected))
                GAMEMANAGER->onPlayerDisconnected(_currentPlayerDisconnected);
            else 
                GAMEMANAGER->onPlayerReconnected(_currentPlayerDisconnected);

            _frameToWait = 2;
        }
    }

    // ----------------------------------------------------------
	// TRCManagerAdapterAdapter ----------------------------------------
	// ----------------------------------------------------------
	TRCManagerAdapter::TRCManagerAdapter() :
		    _currentMessage(NULL)
            , _ResetCounter(0)
            , _pTaskCaller(NULL)
            , _lastErrorContext(GenericContexte)
            , _hasPausedTheGame(bfalse)
            , _freeSpaceDiskKB(U32_INVALID)
            , _neededSpaceDiskKB(0)
            , _DisplayContentCounter(0)
            , _ioError(IOErr_None)
            , _debugExitValue(Exit_None)
            , _currentExitValue(Exit_None)
            , _fatalError(bfalse)
            , _isInitialized(bfalse)
            , _enable(bfalse)
            , _trcHelper(NULL)
            , m_PausedCounter(0)
            , _warningBootMessageDisplayed(bfalse)
            , _isDisplayingCorruptedSaveMsg(bfalse)
	{
        _tasks.reserve(TRCMANAGER_MAXTASK);
	}

	TRCManagerAdapter::~TRCManagerAdapter()
	{
        u32 messagesCount = _messages.size();
        for (u32 i = 0; i < messagesCount; i++)
        {
            if(_messages[i]) 
                SF_DEL(_messages[i]);
        }
        _messages.clear();

        SF_DEL(_currentMessage);

        u32 taskCount = _tasks.size();
        for (u32 i = 0; i < taskCount; i++)
        {
            if(_tasks[i]) 
                delete _tasks[i];
        }
        _tasks.clear();

        File::setOnFileOperationResultCallback(NULL);
	}
	
    bbool TRCManagerAdapter::existsMessage(ErrorContext errorContext) const
      {
        if(_currentMessage && 
            !_currentMessage->getCanBeDupplicated() &&
            _currentMessage->getContexte() == errorContext)
            return btrue;

        u32 messagesCount = _messages.size();
        for (u32 i = 0; i < messagesCount; i++)
        {
            if(_messages[i]->getContexte() == errorContext && !_messages[i]->getCanBeDupplicated() )
                return btrue;

            if ( _messages[i]->getCanBeDupplicated() && _u32_customParam == _messages[i]->getActivePlayer() )
                return btrue;
        }
        return bfalse;
    }

    void TRCManagerAdapter::_clearCustomParams()
    {
        _u32_customParam = U32_INVALID;
        _pTaskCaller = NULL;
    }

	bbool TRCManagerAdapter::addMessage(ErrorContext errorContext, 
        TRCMessage_PreStartCallback_t preStartCallback, void* pParams,
        TRCMessage_OnClose_t onCloseCallback, void* pParamsOnClose)
	{
        if (!_isInitialized || !_enable) return bfalse; 

        if(existsMessage(errorContext))
        {
            _clearCustomParams();
            return bfalse;
        }

		if ( _buildAndAddMessage(errorContext) )
        {
            if ( preStartCallback )
                _messages.back()->setPreStartCallback(preStartCallback, pParams);
            if ( onCloseCallback )
                _messages.back()->setOnCloseCallback(onCloseCallback, pParamsOnClose);
            _newMessageProcess();
        }
        _clearCustomParams();		
        return btrue;
	}

    void TRCManagerAdapter::addListener(TRCMessage_OnClose_t onCloseCallback, void* pParamsOnClose /* = NULL */)
    {
        if(!_isInitialized) return;

        TRCListener listener = { onCloseCallback, pParamsOnClose } ;
        _listenersOnClose.push_back(listener);
    }

    bbool TRCManagerAdapter::removeListener(TRCMessage_OnClose_t onCloseCallback, void* pParamsOnClose /* = NULL */)
    {
        TRCListener_List::iterator it = _listenersOnClose.begin(); 
        for(; it != _listenersOnClose.end();  ++it)
        {
            if ((*it)._function == onCloseCallback && 
                (*it)._params == pParamsOnClose)
            {
                _listenersOnClose.erase (it);
                return btrue;
            }
        }
        ITF_ASSERT_MSG(0, "Listener not found");
        return bfalse;
    }

    
    bbool TRCManagerAdapter::addTask(TRCTask* pTask)
    {
        if (!_isInitialized) return bfalse;

        _tasks.push_back(pTask);
        pTask->init();
        return bfalse;
    }

	void TRCManagerAdapter::_newMessageProcess()
	{
        if(_currentMessage && _currentMessage->getDisplayPriority() <  _messages.back()->getDisplayPriority() )
        {
            // Switch messages
            TRCMessage_Base* tmpMessage = _messages.back();
            _messages.pop_back();            

            if(!_currentMessage->isWaiting())
            {
                _hideCurrentMessage();
                _currentMessage->forceWaiting();
            }            
            _messages.push_back(_currentMessage);

            _currentMessage = tmpMessage;
        }        
		else if(_messages.size()>0 && _currentMessage==NULL)
		{		
			_currentMessage = _messages.back();
			_messages.pop_back();
		}
	}

    void TRCManagerAdapter::_hideCurrentMessage()
    {
        UI_MENUMANAGER->showMenuPage(
            _currentMessage->getDisplayPriority() == TRCMessage_Base::High ? 
            GAMEINTERFACE->getCriticalExitMenuPriority() : GAMEINTERFACE->getTRCMenuPriority(), _currentMessage->name(), bfalse);
    }
	
	void TRCManagerAdapter::_hideAndKillCurrentMessage()
	{
        // resume the game if needed
		if (_hasPausedTheGame && GAMEMANAGER->isInPause())
        {
			GAMEMANAGER->leavePause(bfalse);
            _hasPausedTheGame = bfalse;
        }
        // do not hide a message never shown
        if (!_currentMessage->isWaiting())
		    _hideCurrentMessage();

        _lastErrorContext = _currentMessage->getContexte();

        SF_DEL(_currentMessage);	    
	    _currentMessage = NULL;

        _isDisplayingCorruptedSaveMsg = bfalse;
	}
	
	void TRCManagerAdapter::_showCurrentMessage()
	{	        
		if (_currentMessage->start())
        {		
		    if (GAMEMANAGER->canPause() && !GAMEMANAGER->isInPause())
            {
                GAMEMANAGER->enterPause(bfalse);
                _hasPausedTheGame = btrue;
            } else
                _hasPausedTheGame = bfalse;

            if(_currentMessage->isUsingUIMessageSystem())
            {
                _openUISystemMessage(static_cast<TRCMessage_Critical*>(_currentMessage));
            } else
            {
                UI_MENUMANAGER->showMenuPage(_currentMessage->getDisplayPriority() == TRCMessage_Base::High ? 
                    GAMEINTERFACE->getCriticalExitMenuPriority() : GAMEINTERFACE->getTRCMenuPriority(), _currentMessage->name(), btrue, _currentMessage);
                
                // Set menu options 
                if(_currentMessage->getAllowedPlayer()!=U32_INVALID)
                {
                    UI_MENUMANAGER->setAllowedPadIndexOnly(_currentMessage->name(), _currentMessage->getAllowedPlayer());
                }

                if(_currentMessage->getContexte() == Sav_NoEnoughInode
                    || _currentMessage->getContexte() == Sav_NoEnoughSpace)
                {
                    UI_MENUMANAGER->setAllowedPadTypeOnly(_currentMessage->name(), InputAdapter::Pad_WiiSideWay);
                }
            }
        }
	}
	
	bbool TRCManagerAdapter::_canDisplayMessage(bbool forcePauseMenu) const
	{
        bbool canDisplay = bfalse;
        if (_currentMessage && _currentMessage->getSystemPriority() == TRCMessage_Base::HighPriority) 
            canDisplay = btrue;
        else
		    canDisplay = ( !LOADINGSCREEN->isVisible() && GAMEMANAGER->canDisplayTRCMessages() && !GAMEMANAGER->isNextOperationValid() && 
            ( forcePauseMenu || !GAMEMANAGER->isInPause() ));

        return canDisplay;
	}
	
    void TRCManagerAdapter::_update_tasks()
    {
        u32 tasksCount = _tasks.size();
        for (u32 i = 0; i < tasksCount ; i++)
        {
            TRCTask* pTask = _tasks[i];
            if (pTask && pTask->isAlive() && !pTask->isAsync() && !pTask->isSleeping())
            {
                pTask->run();
            }
        }
    }
    void TRCManagerAdapter::_update_messages()
    {
        if(_currentMessage)
        {
        	if(_currentMessage->isWaiting())
        	{
        		if (_canDisplayMessage(_currentMessage->CanForcePauseMenu()))
        		{
        			_showCurrentMessage();

                    LOG_TRC("Show new message");
        		}
        	} else if(_currentMessage->isAlive())
        	{
	            _currentMessage->update();	
                
                if (! UI_MENUMANAGER->isDisplayingPriority ( _currentMessage->getDisplayPriority() == TRCMessage_Base::High ? 
                    GAMEINTERFACE->getCriticalExitMenuPriority() : GAMEINTERFACE->getTRCMenuPriority() ) )
                {                  
                    LOG_TRC("Kill message ( not being displayed) ");
                    _currentMessage->kill();
                }

                if(GAMEMANAGER->canPause() && !GAMEMANAGER->isInPause())
                {
                    GAMEMANAGER->enterPause(bfalse);
                    _hasPausedTheGame = btrue;
                }
            }

            // Can be killed before starting
            if(!_currentMessage->isAlive())
            {
                LOG_TRC("Kill message ( not alive )");

                _hideAndKillCurrentMessage();
                _newMessageProcess();
            }
        }
    }

	void TRCManagerAdapter::update()
	{
        if (!_isInitialized) return; 

        _update_messages();
        _update_tasks();
	}

    bbool TRCManagerAdapter::isDisplayingError() const
    {
        return (_isInitialized && _currentMessage && !_currentMessage->isWaiting());
    }

    i32 TRCManagerAdapter::getcurrentErrorCode() const
    {
        if(_currentMessage==NULL)
            return TRCMESSAGE_ERROR_INVALID;
        return _currentMessage->error();
    }

    i32 TRCManagerAdapter::OnFileOperationResultCallback( FileOperation op, i32 res )
    {
        // FileOperation -> ErrorContext
        ErrorContext fileContext = GenericContexte;
        switch (op)
        {
        case FileOp_Delete:  fileContext = IO_DeletingFile;      break;
        case FileOp_GetInfo: fileContext = IO_DescriptingFile;   break;
        case FileOp_Open:    fileContext = IO_OpeningFile;       break;
        case FileOp_Read:    fileContext = IO_ReadingFile;       break;
        case FileOp_Rename:  fileContext = IO_RenamingFile;      break;
        case FileOp_Write:   fileContext = IO_WritingFile;       break;
        default:
            return res;
        }

        TRC_ADAPTER->checkIOError(fileContext, res);
        return res;
    }

    void TRCManagerAdapter::killCurrentMessage()
    {
        if(_currentMessage && !_currentMessage->isWaiting())
        {
            _currentMessage->kill();
            _update_messages ();
        }
    }

    void TRCManagerAdapter::init()
    {
        _isInitialized = btrue;
        _enable = btrue;

        File::setOnFileOperationResultCallback(OnFileOperationResultCallback);
    }

    void TRCManagerAdapter::pauseGame()
    {
        m_PausedCounter++;

        if(GAMEMANAGER_VALID && m_PausedCounter == 1)
        {
            if( !GAMEMANAGER->isInPause() )
            {
                GAMEMANAGER->pauseMap(btrue);
                GAMEMANAGER->stopDefaultBusMix();
                GAMEMANAGER->pauseSounds(btrue);
            }
            else
            {
                GAMEMANAGER->stopPauseBusMix();
            }

            SOUND_MANAGER()->playBusMix(SOUND_BUS_MIX_NOSOUND, btrue);
        }
    }

    void TRCManagerAdapter::resumeGame()
    {

        ITF_ASSERT(m_PausedCounter > 0);
        if(m_PausedCounter > 0)
        {
            m_PausedCounter--;

            if(GAMEMANAGER_VALID && m_PausedCounter == 0)
            {
                SOUND_MANAGER()->stopBusMix(SOUND_BUS_MIX_NOSOUND, btrue);

                if( !GAMEMANAGER->isInPause() )
                {
                    GAMEMANAGER->setDefaultBusMix();
                    GAMEMANAGER->pauseSounds(bfalse);
                    GAMEMANAGER->pauseMap(bfalse);
                }
                else
                {
                    GAMEMANAGER->setPauseBusMix();
                }
            }
        }

    }

    void TRCManagerAdapter::enablePadTask()
    {
        u32 tasksCount = _tasks.size();
        for (u32 i = 0; i < tasksCount ; i++)
        {
            TRCTask* pTask = _tasks[i];
            if (pTask && pTask->isSleeping() && pTask->getName() == TRCTask::Pad)
            {
                pTask->wakeup();
                break;
            }
        }
    }

    bbool TRCManagerAdapter::isPadTaskEnable() const
    {
        // In order to handle the "notrc" issue .. 
        if (!_isInitialized)
            return btrue;

        u32 tasksCount = _tasks.size();
        for (u32 i = 0; i < tasksCount ; i++)
        {
            TRCTask* pTask = _tasks[i];
            if (pTask && pTask->getName() == TRCTask::Pad)
            {
                return (!pTask->isDisable());
            }
        }
        ITF_ASSERT_MSG(0, "pad task not found");
        return bfalse;
    }

#ifndef ITF_FINAL
    void TRCTask_DebugIO::run()
    {
#ifdef USE_DEBUG_TRCTASK
        InputAdapter::PressStatus	buttons[JOY_MAX_BUT];	
        for (u32 _pad = 0; _pad < JOY_MAX_COUNT; _pad++)
        {
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, _pad, buttons, JOY_MAX_BUT);

            if(buttons[m_joyButton_ThumbRight] == InputAdapter::Pressed)
            {
                if(buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
                    TRC_ADAPTER->debugSetIOError(TRCManagerAdapter::IOErr_DiscNotFound);
                else if(buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
                    TRC_ADAPTER->debugSetIOError(TRCManagerAdapter::IOErr_FileNotFound);
                //else if(buttons[m_joyButton_DPadU] == InputAdapter::JustPressed)   
                //else if(buttons[m_joyButton_DPadD] == InputAdapter::JustPressed)
            }
        }
#endif //USE_DEBUG_TRCTASK
    }

    void TRCTask_DebugExit::run()
    {
#ifdef USE_DEBUG_TRCTASK
        InputAdapter::PressStatus	buttons[JOY_MAX_BUT];	
        for (u32 _pad = 0; _pad < JOY_MAX_COUNT; _pad++)
        {
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, _pad, buttons, JOY_MAX_BUT);

            if(buttons[m_joyButton_ThumbLeft] == InputAdapter::Pressed)
            {
                if(buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
                    TRC_ADAPTER->debugSetExitValue(TRCManagerAdapter::Exit_Shutdown); // quit on PS3, shutdown on Wii
                else if(buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
                    TRC_ADAPTER->debugSetExitValue(TRCManagerAdapter::Exit_Restart);
                else if(buttons[m_joyButton_DPadU] == InputAdapter::JustPressed)
                    TRC_ADAPTER->debugSetExitValue(TRCManagerAdapter::Exit_BackToMenu);
            }
        }
#endif //USE_DEBUG_TRCTASK
    }

#endif //ITF_FINAL
} // namespace ITF
