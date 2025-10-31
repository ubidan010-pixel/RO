#include "precompiled_trcadapter_WII.h"

#ifndef _ITF_TRCADAPTER_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_WII.h"
#endif //_ITF_TRCADAPTER_WII_H_

#ifndef _ITF_FILE_WII_H_
#include "core/file/wii/File_WII.h"
#endif //_ITF_FILE_WII_H_

#ifndef __MTH_INT_WII_H__
#include "core/math/WII/MTH_int_WII.h"
#endif //__MTH_INT_WII_H__

#ifndef _ITF_ADAPTER_SAVEGAME_WII_H__
#include "adapters/Adapter_Savegame_WII/Adapter_Savegame_WII.h"
#endif

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_TRCMESSAGES_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_Messages_WII.h"
#endif //_ITF_TRCMESSAGES_WII_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_VIDEOHANDLE_H_
#include "engine/video/videoHandle.h"
#endif //_ITF_VIDEOHANDLE_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"  
#endif // _ITF_RAKIINCLUDES_

#ifndef RAKI_AUDIOCALLBACKMANAGER_WII_H
#include "RakiEngine/Specific/_Wii/AudioCallbackManager_Wii.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_WII_H

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_


#include <revolution/os/OSThread.h>

namespace ITF
{
	TRCManagerAdapter::ExitValue TRCManagerAdapter_WII::m_exitValue = TRCManagerAdapter::Exit_None;

    String buildText(u32 lineID, EContextIconType iconType = ContextIconType_Invalid, u32 padIndex = 0)
    {
        static String workingStr;
        LocalisationId id;

        id.value = lineID;
        String buff = LOCALISATIONMANAGER->getText(id);
        workingStr  = buff ;

        if (iconType != ContextIconType_Invalid)
        {
            workingStr += CONTEXTICONSMANAGER->getIconStr(INPUT_ADAPTER->getPadType(padIndex), iconType);
        }

        return workingStr;
    }


    bbool TRCManagerAdapter_WII::_buildAndAddMessage(ErrorContext	errorContext)
    {
        String     message;
        String     str;
        TRCMessage_Base* pTRCMessage = NULL;

      
        switch(errorContext)
        {
        case Pad_DisconnectingDuringGameplay:
            ITF_ASSERT(_u32_customParam != U32_INVALID);
            message = buildText(4046);
            pTRCMessage = new TRCMessage_PadDeconnection(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, _u32_customParam));                

            pTRCMessage->SetForceOverPauseMenu();
            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);
            if(_u32_customParam != GAMEMANAGER->getMainIndexPlayer())
            {                
                ((TRCMessage_PadDeconnection*)pTRCMessage)->setButton(buildText(4164), input_actionID_Valid);      
            }
            pTRCMessage->setActivePlayer(_u32_customParam);            
            break;                     
        case Pad_ExtensionNunchukDetected:
            if(GAMEMANAGER->getMainIndexPlayer_Internal() == U32_INVALID || 
                (!GAMEMANAGER->isPlayableScreen() && GAMEMANAGER->getMainIndexPlayer_Internal() != _u32_customParam && !LOADINGSCREEN->isVisible()))
                break;

            ITF_ASSERT(_u32_customParam != U32_INVALID);            
            message = buildText(4048);
            pTRCMessage = new TRCMessage_ExtensionChange(errorContext);
            pTRCMessage->forceName("trcmessage_nunchuk_wii");
            pTRCMessage->setActivePlayer(_u32_customParam);
            pTRCMessage->setAllowedPlayer(_u32_customParam);
            pTRCMessage->SetForceOverPauseMenu();
            pTRCMessage->setCanBeDupplicated(btrue);
            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);

            str = buildText(4092);
            str += CONTEXTICONSMANAGER->getIconStr((u32)InputAdapter::Pad_WiiNunchuk, ContextIconType_Select);
            ((TRCMessage_ExtensionChange*)pTRCMessage)->setButton(str, input_actionID_Valid);       
            break;
        case Pad_ExtensionClassicDetected:
            if(GAMEMANAGER->getMainIndexPlayer_Internal() == U32_INVALID || 
                (!GAMEMANAGER->isPlayableScreen() && GAMEMANAGER->getMainIndexPlayer_Internal() != _u32_customParam && !LOADINGSCREEN->isVisible()))
                break;

        	ITF_ASSERT(_u32_customParam != U32_INVALID);
            message = buildText(4049);
            pTRCMessage = new TRCMessage_ExtensionChange(errorContext);
            pTRCMessage->forceName("trcmessage_classic_WII");
            pTRCMessage->setActivePlayer(_u32_customParam);
            pTRCMessage->setAllowedPlayer(_u32_customParam);
            pTRCMessage->SetForceOverPauseMenu();
            pTRCMessage->setCanBeDupplicated(btrue);
            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);

            str = buildText(4092);
            str += CONTEXTICONSMANAGER->getIconStr((u32)InputAdapter::Pad_WiiClassic, ContextIconType_Select);
            ((TRCMessage_ExtensionChange*)pTRCMessage)->setButton(str, input_actionID_Valid);    
            break;
        case Pad_NoExtensionDetected:
            if(GAMEMANAGER->getMainIndexPlayer_Internal() == U32_INVALID || 
                (!GAMEMANAGER->isPlayableScreen() && GAMEMANAGER->getMainIndexPlayer_Internal() != _u32_customParam && !LOADINGSCREEN->isVisible()))
                break;

        	ITF_ASSERT(_u32_customParam != U32_INVALID);
            message = buildText(4050);
            pTRCMessage = new TRCMessage_ExtensionChange(errorContext);
            pTRCMessage->forceName("trcmessage_sideway_wii");
            pTRCMessage->setActivePlayer(_u32_customParam);
            pTRCMessage->setAllowedPlayer(_u32_customParam);
            pTRCMessage->SetForceOverPauseMenu();
            pTRCMessage->setCanBeDupplicated(btrue);
            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);

            str = buildText(4092);
            str += CONTEXTICONSMANAGER->getIconStr((u32)InputAdapter::Pad_WiiSideWay, ContextIconType_Select);
            ((TRCMessage_ExtensionChange*)pTRCMessage)->setButton(str, input_actionID_Valid);  
            break;

        case Sav_NoEnoughInode:
        case Sav_NoEnoughSpace:
            {
                
        	if(errorContext == Sav_NoEnoughSpace)
        	{
            	ITF_ASSERT(_u32_customParam != U32_INVALID);                
             
                String8 userBlocks;
                userBlocks.setTextFormat("%d", _u32_customParam);

                String s (buildText(4159)); 
                s.replace("<X>", userBlocks.cStr());
                if(_u32_customParam == 1)
                {
                    s.replace("(s)", "");
                    s.replace("(ken)", "");
                }                
                                
                char* sReplaced = s.getCharCopy();
            	message = sReplaced;
                SF_DEL_ARRAY(sReplaced);
            } else //Sav_NoEnoughInode
            {                
                
            	message = buildText(4162);
            }
            
            pTRCMessage = new TRCMessage_NoEnoughSpaceOrInode_WII(errorContext);
            ((TRCMessage_NoEnoughSpaceOrInode_WII*)pTRCMessage)->setLeftButton(buildText(4062, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Valid);
            ((TRCMessage_NoEnoughSpaceOrInode_WII*)pTRCMessage)->setRightButton(buildText(4063, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Back);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer());
            }break;

        case Sav_AskForDelete:
            message = buildText(4087);

            pTRCMessage = new TRCMessage_TwoButton(errorContext);
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton(buildText(4064, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Valid);
            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton(buildText(4142, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Back);  //cancel
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer());
            break;

        case Sav_InitCreatingFiles:
        case Sav_InitCreatingBanner:
        case Sav_ReadFiles:
        	message = buildText(4161);       	
        	pTRCMessage = new TRCMessage_CorruptedSave_WII(errorContext);
        	                
            ((TRCMessage_CorruptedSave_WII*)pTRCMessage)->setLeftButton(buildText(4065, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Back);          // Delete file
            ((TRCMessage_CorruptedSave_WII*)pTRCMessage)->setRightButton(buildText((errorContext == Sav_ReadFiles) ? 4160 : 4062, ContextIconType_Invalid, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Valid);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer());
        	break;

        case Sav_ErrorDuringLoad:
            message = buildText(4161);       	
            pTRCMessage = new TRCMessage_OneButton(errorContext);
            ((TRCMessage_OneButton*)pTRCMessage)->setButton(buildText(4064, ContextIconType_Select, GAMEMANAGER->getMainIndexPlayer()), input_actionID_Valid);
            break;            
        }

        if(pTRCMessage)
        {
            pTRCMessage->forceTexts("", message);
            if(_pTaskCaller)
                pTRCMessage->SetTaskCaller(_pTaskCaller);
            _messages.push_back(pTRCMessage);
            return btrue;
        }
		return bfalse;
    }

    bbool TRCManagerAdapter_WII::checkSpaceDisk(ErrorContext ErrorContext, u32 fsblock, u32 inode)
    {
        if(!_isInitialized) return btrue;

        u32 answer;
        if(hwFile::checkAvailableSpace(fsblock, inode, &answer))
            return btrue;

        bbool isInodeFailed = ( (answer & NAND_CHECK_SYS_INSINODE) || (answer & NAND_CHECK_HOME_INSINODE) );
        bbool isFsblockFailed = ( (answer & NAND_CHECK_SYS_INSSPACE) || (answer & NAND_CHECK_HOME_INSSPACE) );
        ITF_ASSERT ( isInodeFailed || isFsblockFailed );

        switch (ErrorContext)
        {
        case Sav_CheckSpace:
            if( isFsblockFailed )
            {
            	u32 userblock = ceil (fsblock / 8.0f);
                setCustomParam_u32(userblock);
                addMessage(Sav_NoEnoughSpace);
            } 
            else // isInodeFailed
                addMessage(Sav_NoEnoughInode);
            break;
        default:
            ITF_ASSERT_MSG(0, "checkSpaceDisk_WII => context not implemented");
        }
        return bfalse; 
    }

    bbool TRCManagerAdapter_WII::checkIOError(ErrorContext errorContext, i32& _errorCode)
    {
        if(!_isInitialized) return btrue;

        i32 WorkingErrorCode = _errorCode; 
    	switch(errorContext)
    	{
    	case Sav_ReadFiles:
    		if (_errorCode == NAND_RESULT_ECC_CRIT
    				|| _errorCode  == NAND_RESULT_AUTHENTICATION)
    		{
    			addMessage(errorContext); 
    			return btrue;
    		}
    		break;
        case IO_OpeningFile:
            if(_ioError != IOErr_None)
            {
                _errorCode = -1; // Entry value or -1
                
            } else
                WorkingErrorCode = 0;
            break;
        case IO_ReadingFile:
            if(_ioError != IOErr_None)
                _errorCode = DVD_RESULT_FATAL_ERROR;
            break;
        case IO_DescriptingFile:
            if(_ioError != IOErr_None)
                _errorCode = 0; // bool
            break;
    	default:break;
    	//case NAND_RESULT_UNKNOWN: addMessage(IO_UnexpectedError); break;
    	//case NAND_RESULT_UNKNOWN: addMessage(IO_UnexpectedError); break;
    	}

        if(TRCManagerAdapter_WII::IOResIsValid(WorkingErrorCode))
            return btrue;

        LOG("IO Error detected during an IO call -- BEGIN");
        while(!TRCManagerAdapter_WII::IOResIsValid(WorkingErrorCode)
			&& !TRCManagerAdapter_WII::IsShutdownRequested())
        {
            WorkingErrorCode = DVDGetDriveStatus();   
            sleep(10);
        }
        while(isAFatalErrorRunning() && !TRCManagerAdapter_WII::IsShutdownRequested())
        {
            sleep(10);
        }
        LOG("IO Error detected during an IO call -- END");

    	return bfalse;
    }

	// This function clears some SDK callbacks that may be called after the reset process has begun. 
	// Do not clear any other callbacks, since these callbacks may access to subsystems already shut down, 
	// and lead to hangups and prevent the reset.
	// 
	// In particular, do not call :
	// AIRegisterDMACallback(0); // it is used by AX lib, may cause a infinite sleep.
	// GXSetBreakPtCallback(0), GXSetDrawDoneCallback(0), GXSetDrawSyncCallback(0), GXSetTexRegionCallback(0), GXSetTlutRegionCallback(0), GXSetVerifyCallback(0);
	// VISetPostRetraceCallback(0), VISetPreRetraceCallback(0)
	// OSSetErrorHandler(i,0)
	// OSSetPowerCallback(0), OSSetResetCallback(0), OSSetSwitchThreadCallback(0)
	// PADSetSamplingCallback(0)
	// WPADSetClearDeviceCallback(0), KPADSetConnectCallback (i,0), WPADSetExtensionCallback(i, 0), KPADSetSamplingCallback (i, 0), 
	// WPADSetSimpleSyncCallback(0), WPADSetSyncDeviceCallback(0)

	void TRCAdapterWII_resetCallback()
	{
		AXRegisterAuxACallback(0, 0);
		AXRegisterAuxBCallback(0, 0);
		AXRegisterAuxCCallback(0, 0);
		AXRegisterCallback(0);
	}

    DWORD TRCManagerAdapter_WII::ResetThread(void *_param)
    {
        OSThreadQueue threadQueue; 

        OSInitThreadQueue( &threadQueue );

		// If reset or power have been called before the thread queue has been created, proceed with reset.
		int level = OSDisableInterrupts(); 
		if (TRCManagerAdapter_WII::m_exitValue == TRCManagerAdapter::Exit_None)
		{
			m_TRCThreadQueue = &threadQueue;
			OSSetPowerCallback(TRCManagerAdapter_WII::PowerCallback);
			OSSetResetCallback(TRCManagerAdapter_WII::ResetCallback);
	        OSSleepThread( &threadQueue );
		}
		OSRestoreInterrupts(level); 

		// Exit requested        
		LOG("Exit requested (reset, power or return to menu)\n");

		// Enter critical section (to be sure that the engine is not reading or writing to the NAND).
		disableRESETandPOWER();

        //Darken the screen (before stopping the GX !)
        VISetBlack(GX_TRUE);
        VIFlush();
        VIWaitForRetrace();

		LOG("Exit started\n");

		// STOP CALLS TO WII API

		// Stop rumble and forbid to start any other.
		if (INPUT_ADAPTER)
			INPUT_ADAPTER->pauseAllRumble(btrue);

		// Stop sound engine 
		raki::AudioCallbackManager::singleton().CONSOLERESET_disableAudioCallback();

		// TRC_ADAPTER->pauseSounds();
		//if (SOUND_ADAPTER && SOUND_ADAPTER->isRunning()) 
		//	SOUND_ADAPTER->shutdown();

		// Don't do this, because it makes a deadlock with the DVD Error thread which keeps the GFX lock. 
		//GFXAdapter_WII* GFX = GFX_ADAPTER_WII;
		//if (GFX)
		//	GFX->LockRendering(); 

		// STOP WII API

		// Stop GX activity (DO NOT CALL GxAbortFrame !!!)
		GXFlush();
		GXDrawDone();

		// Stop sound
		AXQuit();

		// Reset callbacks
		TRCAdapterWII_resetCallback();

		// Apply action wanted
		switch(TRCManagerAdapter_WII::m_exitValue)
		{
		case TRCManagerAdapter::Exit_Shutdown:
			OSShutdownSystem();
			break;
		case TRCManagerAdapter::Exit_Restart:
			OSRestart(0);
			break;
		case TRCManagerAdapter::Exit_BackToMenu:
			OSReturnToMenu();
			break;
		case TRCManagerAdapter::Exit_BackToDataManagement:
			OSReturnToDataManager();
			break;
		}
        return 0;
    }

    DWORD TRCManagerAdapter_WII::DVDErrorThread(void *_param)
    {
        i32 WorkingErrorCode; 
        String errorMessage ("");        

        /* support for animating the messages */
        const i32  AnimateDelay = 300; /* ms */
        static char *aszDots[] = {"  ...", " . ..", " .. .", " . .."};
        static OSTick switchTick = OSDiffTick(OSGetTick(), OSMillisecondsToTicks(AnimateDelay + 1));
        static i32 i32AnimateState = sizeof(aszDots)/sizeof(char *) - 1;

        for ( ;; )
        {
            WorkingErrorCode = DVDGetDriveStatus();
            switch(WorkingErrorCode)
            {
            case DVD_STATE_WRONG_DISK:
            case DVD_STATE_NO_DISK:
            case DVD_STATE_FATAL_ERROR:
            case DVD_STATE_RETRY:
            case DVD_STATE_MOTOR_STOPPED:
            case DVD_STATE_NO_INPUT:
#ifdef FILE_BIG_WAIT
            case DVD_STATE_PAUSING:
#endif //FILE_BIG_WAIT
                // Error detected
                break;
            case DVD_STATE_WAITING:
#ifdef FILE_BIG_WAIT
			case DVD_STATE_BUSY:
				if (hwFile::m_BigWait) // Display "reading disk" if we have been waiting too long for a read query to finish. 
					break;
			case DVD_STATE_END:
#endif //FILE_BIG_WAIT
            default:
                // exit here, we don't need to go further
                sleep( 50 );
                continue;
            }

            disableRESETandPOWER();
            
            errorMessage.clear();

            LOG("IO Error detected by the DVDError Thread -- BEGIN");
            setFatalErrorStatus(btrue);

            enableRESETandPOWER();

            // LOG("[IO ERROR] Code: %i, %s", _errorCode, errorMessage.cStr());
            // Don't let other thread render until the message is skip.

            // Reactivate Dimming in case it was disabled
            bbool saveScreenProtectionState = GFX_ADAPTER->setScreenProtection(btrue);

            GFX_ADAPTER_WII->LockRendering();
            GFX_ADAPTER_WII->resetGX();
            setCanDrawContent(bfalse);

            // Stop Rumble
            INPUT_ADAPTER->pauseAllRumble(btrue);

            // Pause SplashScreen
            SYSTEM_ADAPTER_WII->pauseSplashScreen();

            // Disable HomeMenu
            bbool bHBMNeedUnpause = SYSTEM_ADAPTER_WII->disableHomeButton();

            // Pause game
            pauseGame();
            raki::AudioCallbackManager::singleton().DVDERROR_pauseAllSounds();
            
            bbool binkIsPaused = bfalse;
            // Pause bink lib
#ifdef USE_ITF_IO
            if(videoHandle::getVideoHandle())
            {
                videoHandle::getVideoHandle()->onIOError();
                binkIsPaused = (videoHandle::getVideoHandle()->isPaused());    
            }
#endif //USE_ITF_IO
            // Remove a possible black screen (that could be set by the home menu for example).
            if (!IsShutdownRequested())
            {
                VISetBlack(FALSE);
                VIFlush();
            }

            // Wait for a valid state
            bbool canExit = bfalse;
            while ( !canExit )
            {
                // TODO: do not compute errorMessage each time .. ? only for BUSY state
                // Get the localized Title NAME 
            
                switch(WorkingErrorCode)
                {
                case DVD_STATE_WRONG_DISK:
                    errorMessage.setTextFormat(TRCMessages_WII::ms_szDvdWrongDisc[TRCMessages_WII::getLanguageIndex()], "Rayman Origins");
                    break;
                case DVD_STATE_NO_DISK:
                    errorMessage.setTextFormat(TRCMessages_WII::ms_szDvdNoDisc[TRCMessages_WII::getLanguageIndex()], "Rayman Origins");
                    break;
                case DVD_STATE_FATAL_ERROR:
                    errorMessage = TRCMessages_WII::ms_szDvdFatal[TRCMessages_WII::getLanguageIndex()];
                    break;
                case DVD_STATE_RETRY:
                    errorMessage = TRCMessages_WII::ms_szDvdRetry[TRCMessages_WII::getLanguageIndex()];
                    break;
                case DVD_STATE_END:// for bink wait
                case DVD_STATE_BUSY:
                    {
                        OSTick tick = OSGetTick();
                        if(OSTicksToMilliseconds(OSDiffTick(tick, switchTick)) > AnimateDelay)
                        {
                            switchTick = tick;
                            if((++i32AnimateState) >= sizeof(aszDots)/sizeof(char *))
                                i32AnimateState = 0;
                        }
                    }

                    errorMessage.setTextFormat(TRCMessages_WII::ms_szDvdReadingDisc[TRCMessages_WII::getLanguageIndex()], aszDots[i32AnimateState]);
                    break;
                case DVD_STATE_MOTOR_STOPPED:
                case DVD_STATE_NO_INPUT:
                    errorMessage = TRCMessages_WII::ms_szDvdMotorOff[TRCMessages_WII::getLanguageIndex()];
                    break;
                }

                // Wait one pass to let the main thread finishing his current rendering
                GFX_ADAPTER_WII->DisplayMessageBox(errorMessage);
                WorkingErrorCode = DVDGetDriveStatus();    

                sleep(10);

                if(TRCManagerAdapter_WII::IsShutdownRequested())
                {
                    GFX_ADAPTER_WII->UnlockRendering();
                    return 0;
                }

                canExit = (WorkingErrorCode==DVD_STATE_END);
                OSYieldThread();
            }

#ifdef FILE_BIG_WAIT
			hwFile::m_BigWait = bfalse;
#endif //FILE_BIG_WAIT

            // Back to the previous state
            SYSTEM_ADAPTER_WII->unpauseSplashScreen();
            if (bHBMNeedUnpause)
                SYSTEM_ADAPTER_WII->enableHomeButton();
            resumeGame();
            raki::AudioCallbackManager::singleton().DVDERROR_resumeAllSounds();
			INPUT_ADAPTER->pauseAllRumble(bfalse);

            setFatalErrorStatus(bfalse);

            GFX_ADAPTER_WII->resetGX();
            GFX_ADAPTER_WII->UnlockRendering();
            setCanDrawContent(btrue);

#ifdef USE_ITF_IO
            if(binkIsPaused && videoHandle::getVideoHandle())            
                videoHandle::getVideoHandle()->resume();
#endif //USE_ITF_IO

            GFX_ADAPTER->setScreenProtection(saveScreenProtectionState);

            LOG("IO Error detected by the DVDError Thread -- END");
            sleep(20);
        }
        return 0;
    }
    
    void TRCManagerAdapter_WII::init()
    {
        TRCManagerAdapter::init();

    	// Handle NAND_RESULT_UNKNOWN / NAND_RESULT_CORRUPT errors
    	NANDSetAutoErrorMessaging(TRUE);
    	
    	// Handle FATAL Error by the System
    	DVDSetAutoFatalMessaging(TRUE);

		ITF::Synchronize::createCriticalSection(&m_NANDAccess);

        // Thread to handle DvD error
        THREADMANAGER->createThread(DVDError_Thread, 
            NULL, 
            ThreadSettings::m_settings[eThreadId_TRCManager]);  

        THREADMANAGER->createThread(Reset_Thread, 
            NULL, 
            ThreadSettings::m_settings[eThreadId_TRCManager]);  

    }    

	void TRCManagerAdapter_WII::PowerCallback()
	{
		RequestExit(Exit_Shutdown);
		//LOG("Shutdown requested by callback");
	}

	void TRCManagerAdapter_WII::ResetCallback()
	{
		RequestExit(Exit_Restart);
		//LOG("Reset requested by callback");
	}

	void TRCManagerAdapter_WII::RequestExit( ExitValue value )
	{
		if (m_exitValue == Exit_None) 
		{
			m_exitValue = value;
			if (TRC_ADAPTER)
			{
				TRC_ADAPTER->setCurrentExitValue(Exit_Restart);  // we don't care the type of exit, just flag it

				// Wake up thread if possible
				if(TRC_ADAPTER_WII->m_TRCThreadQueue)
				{
					OSWakeupThread(TRC_ADAPTER_WII->m_TRCThreadQueue);
				}
			}
		}
	}


    void TRCManagerAdapter_WII::update()
    {
        TRCManagerAdapter::update();

        static bbool previousShouldEnableHomeMenu = btrue;
        bbool shouldEnableHomeMenu = GAMEMANAGER->canDisplayTRCMessages();

        if (shouldEnableHomeMenu != previousShouldEnableHomeMenu)
        {
            previousShouldEnableHomeMenu = shouldEnableHomeMenu;
            if(!shouldEnableHomeMenu) 
                SYSTEM_ADAPTER_WII->disableHomeButton();
            else
                SYSTEM_ADAPTER_WII->enableHomeButton();
        }
    }

    bbool TRCManagerAdapter_WII::IOResIsValid( i32 _error )
    {
        return ( _error == DVD_STATE_END || _error == DVD_STATE_BUSY );
    }

    
    // ----------------------------------------------------------
    // TRCMessage_CorruptedSave_WII -----------------------------
    // ----------------------------------------------------------
    void TRCMessage_CorruptedSave_WII::onRightButtonAction()
    {
        // Continue without loading
        TRCMessage_TwoButton::onLeftButtonAction();
    }
    
    void TRCMessage_CorruptedSave_WII::onLeftButtonAction()
    {
        // Delete file
        SAVEGAME_ADAPTER_WII->recreate_files();
        TRCMessage_TwoButton::onRightButtonAction();
    }
    
    // ----------------------------------------------------------
    // TRCMessage_NoEnoughSpaceOrInode_WII ----------------------
    // ----------------------------------------------------------
    void TRCMessage_NoEnoughSpaceOrInode_WII::onRightButtonAction()
    {
		TRCManagerAdapter_WII::RequestExit(TRCManagerAdapter::Exit_BackToDataManagement);
        TRCMessage_TwoButton::onLeftButtonAction();
    }
    
    void TRCMessage_NoEnoughSpaceOrInode_WII::onLeftButtonAction()
    {
    	// Back to the Data Management screen
        SAVEGAME_ADAPTER->DoNotUseSaveSystem();
        TRCMessage_TwoButton::onRightButtonAction();
    }

    void TRCMessage_NoEnoughSpaceOrInode_WII::update()
    {
        TRCMessage_TwoButton::update();
        
        UIMenu* menu = UI_MENUMANAGER->getMenu(_baseName);
        if(menu)
        {
            UIComponent* pUIComponent = menu->getUIComponentSelected();
            if(pUIComponent)
            {
                if(!_lastComponentSelected) _lastComponentSelected = pUIComponent;

                const static StringID s_TemplateButtonLeft("template_button_left");
                if(pUIComponent->getID() == s_TemplateButtonLeft)
                {
                    _workingMessage = _buttonL + CONTEXTICONSMANAGER->getIconStr((u32)InputAdapter::Pad_WiiSideWay, ContextIconType_Select);
                    if(_lastComponentSelected!=pUIComponent)
                        _lastComponentSelected->forceContent(_buttonR);
                }
                else
                {
                    _workingMessage = _buttonR + CONTEXTICONSMANAGER->getIconStr((u32)InputAdapter::Pad_WiiSideWay, ContextIconType_Select);
                    if(_lastComponentSelected!=pUIComponent)
                        _lastComponentSelected->forceContent(_buttonL);
                }

                pUIComponent->forceContent( _workingMessage );
                _lastComponentSelected = pUIComponent;
            }
        }
    }

    // ----------------------------------------------------------
    // TRCMessage_ExtensionChange ----------------------
    // ----------------------------------------------------------
    void TRCMessage_ExtensionChange::update()
    {
        // Always check validity
        checkStartCallback();        
    }

    // ----------------------------------------------------------
    // TRCMessage_PadDeconnection ----------------------
    // ----------------------------------------------------------
    void TRCMessage_PadDeconnection::update()
    {
        TRCMessage_OneButtonWithCB::update();
        if(_button.isEmpty())
            return ;

        UIMenu* menu = UI_MENUMANAGER->getMenu(_baseName);
        if(menu)
        {
            UIComponent* pUIComponent = menu->getUIComponentSelected();
            if(pUIComponent)
            {
                
                if(pUIComponent->getID() == ITF_GET_STRINGID_CRC(template_button,4167922611))
                {
                    u32 padType = INPUT_ADAPTER->getPadType(GAMEMANAGER->getMainIndexPlayer());
                    
                    static String newButtonMsg;
                    newButtonMsg = _button + CONTEXTICONSMANAGER->getIconStr(padType, ContextIconType_Select);                    
                    pUIComponent->forceContent(newButtonMsg);
                }
            }
        }
    }

    // ----------------------------------------------------------
    // TRCTask_DebugExtension -----------------------------------
    // ----------------------------------------------------------
#ifndef ITF_FINAL	
	void TRCTask_DebugExtension::run()
	{
		InputAdapter::PressStatus	buttons[JOY_MAX_BUT];	
		for (u32 _chan = 0; _chan < 4; _chan++)
		{
			INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, _chan, buttons, JOY_MAX_BUT);
				
			if(buttons[m_joyButton_A] == InputAdapter::Pressed
				&& buttons[m_joyButton_B] == InputAdapter::Pressed)
			{
					 if(buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
				{	
					changeExtension(_chan, 0, WPAD_DEV_UNKNOWN);
				}
				else if(buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
				{	
					//changeExtension(_chan, 1, WPAD_DEV_NOT_FOUND);
				}
				else if(buttons[m_joyButton_DPadU] == InputAdapter::JustPressed)
				{	
					changeExtension(_chan, 2, WPAD_DEV_NOT_SUPPORTED);
				}
				else if(buttons[m_joyButton_DPadD] == InputAdapter::JustPressed)
				{	
					changeExtension(_chan, 3, WPAD_DEV_FUTURE);
				}
			}
		}		
    }
    
    void TRCTask_DebugExtension::changeExtension(u32 chan, u32 extension, u32 wpad_dev)
    {
    	if(extensionStatuts[chan][extension])
			WPADDetachDummyExtension(chan);
		else
			WPADAttachDummyExtension (chan, wpad_dev);
			
		extensionStatuts[chan][extension] = !extensionStatuts[chan][extension];
    }
    
    void TRCTask_DebugExtension::init()
    {
    	ITF_MemSet(extensionStatuts, 0, sizeof(extensionStatuts));
    }
#endif	//ITF_FINAL


} // namespace ITF

