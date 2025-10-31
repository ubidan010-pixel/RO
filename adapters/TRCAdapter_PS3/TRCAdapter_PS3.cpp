#include "precompiled_TRCAdapter_PS3.h"

#include <np/drm.h>

#ifndef _ITF_TRCADAPTER_PS3_H_
#include "adapters/TRCAdapter_PS3/TRCAdapter_PS3.h"
#endif //_ITF_TRCADAPTER_PS3_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_ADAPTER_SAVEGAME_PS3_H__
#include "adapters/Adapter_Savegame_PS3/Adapter_Savegame_PS3.h"
#endif //_ITF_ADAPTER_SAVEGAME_PS3_H__

#ifndef _ITF_REWARDADAPTER_PS3_H__
#include "adapters/RewardAdapter_PS3/RewardAdapter_PS3.h"
#endif //_ITF_REWARDADAPTER_PS3_H__

#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifdef TRC_USE_SYSTEM_MSG
#include <sysutil/sysutil_msgdialog.h>
#endif //TRC_USE_SYSTEM_MSG

namespace ITF
{

    TRCManagerAdapter_PS3::OnIOErrorOnMainThreadCB TRCManagerAdapter_PS3::ms_ioErrorOnMainCB = NULL;
    u64 TRCManagerAdapter_PS3::ms_ioErrorOnMainCBUsrData = 0;


#ifndef ITF_FINAL
// Define one or both defines to activate the launch and quit repeated test
//#define ITF_PS3_QUIT_INCREMENT_CHECK
//#define ITF_PS3_DISC_FAILURE_INCREMENT_CHECK
#endif

#ifndef ITF_FINAL
system_time_t g_quitStart = 0;
ux g_quitTargetCount = 0;
ux g_quitCount = 0;
#endif

#if defined(ITF_PS3_QUIT_INCREMENT_CHECK) || defined(ITF_PS3_DISC_FAILURE_INCREMENT_CHECK) 
    static const char * QuitCountFileName = "/app_home/QuitCount.txt";

    static ux loadQuitCount(const char * _countFileName)
    {
        ux lastFrameValue = 0;

        FILE * f = fopen(_countFileName, "r");

        if (f)
        {
            fscanf(f, "%d", &lastFrameValue);
            fclose(f);
        }
        return lastFrameValue;
    }


    static void saveQuitCount(const char * _countFileName, ux _newQuitCount)
    {
        FILE * f = fopen(_countFileName, "w+");
        if (f)
        {
            fprintf(f, "%d", _newQuitCount);
            fclose(f);
        }
    }

#endif


    String buildText(u32 lineID, EContextIconType iconType = ContextIconType_Invalid)
    {        
        static String workingStr;
        LocalisationId id;

        id.value = lineID;
        String buff = LOCALISATIONMANAGER->getText(id);
        workingStr  = buff ;

        if (iconType != ContextIconType_Invalid)
        {

            if (SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
            {
                if (iconType == ContextIconType_Select)
                    iconType = ContextIconType_Back;
                else if (iconType == ContextIconType_Back)
                    iconType = ContextIconType_Select;
            }

            workingStr = workingStr + " " + CONTEXTICONSMANAGER->getIconStr(InputAdapter::Pad_PS3, iconType);
        }

        return workingStr;
    }

    bbool TRCManagerAdapter_PS3::_buildAndAddMessage(ErrorContext	errorContext)
    {
        String     message;
        TRCMessage_Base* pTRCMessage = NULL;

        switch(errorContext)
        {
        case Pad_DisconnectingDuringTitleScreen:
            // TEMP TEXTE:
            message = buildText(4482);
            pTRCMessage = new TRCMessage_OneButtonWithCB(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, U32_INVALID)); // INVALID = ALL pad
            pTRCMessage->SetForceOverPauseMenu();
            break;
        case Pad_DisconnectingDuringMenu:
        case Pad_DisconnectingDuringGameplay:
            ITF_ASSERT(_u32_customParam != U32_INVALID);

            {
                String portNumberStr ;
                portNumberStr.setTextFormat(" %d", _u32_customParam+1);
                message = message + buildText(4037) + portNumberStr;
            }
            
            pTRCMessage = new TRCMessage_OneButtonWithCB(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, _u32_customParam));                

            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);
            pTRCMessage->SetForceOverPauseMenu();
            if(errorContext==Pad_DisconnectingDuringGameplay && _u32_customParam != GAMEMANAGER->getMainIndexPlayer())
            {                
                ((TRCMessage_OneButtonWithCB*)pTRCMessage)->setButton(buildText(4164, ContextIconType_Select), input_actionID_Back);      
            }
            pTRCMessage->setActivePlayer(_u32_customParam);
        break;

        case Sav_NotOwnerOfTheSave:
            pTRCMessage = newAlloc(mId_System, TRCMessage_OneButton(errorContext));
            message = buildText(4032);

            ((TRCMessage_OneButton*)pTRCMessage)->setButton(buildText(4092, ContextIconType_Invalid), input_actionID_Valid);
            break;

        case Sav_WarningBoot:        
            if(SAVEGAME_ADAPTER->IsSaveSystemEnable())
            {
                pTRCMessage = newAlloc(mId_System, TRCMessage_NoButtonTimer(5.0, errorContext));            
                message = buildText(4026);
            }
            break;

        case Sav_AskForDelete:
            pTRCMessage = new TRCMessage_TwoButton(errorContext);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer());

            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton(buildText(4142, ContextIconType_Invalid), input_actionID_Back); 
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton(buildText(4064, ContextIconType_Invalid), input_actionID_Valid); // continue
            message = buildText(4087);

            break;

        case Sav_CorruptedFile:
        case Sav_ErrorDuringLoad:
            pTRCMessage = new TRCMessage_SavCorruptedFile_PS3(errorContext);
            message = buildText(4031);
            _isDisplayingCorruptedSaveMsg = btrue;

            ((TRCMessage_SavCorruptedFile_PS3*)pTRCMessage)->setLeftButton(buildText(4065, ContextIconType_Invalid), input_actionID_Valid);
            ((TRCMessage_SavCorruptedFile_PS3*)pTRCMessage)->setRightButton(buildText(3714, ContextIconType_Invalid), input_actionID_Back);//retry
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
    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::updateFakeFreeSpace(i32 spaceToAddInKB)
    {
        _freeSpaceDiskKB += spaceToAddInKB;
    }
    //-------------------------------------------------------------------------------------
    bbool TRCManagerAdapter_PS3::checkSpaceDisk(ErrorContext ErrorContext, u32 kbytes)
    {
        if(!_isInitialized) return btrue; 

        bbool res = btrue; 
        if (_freeSpaceDiskKB < kbytes)
        {            
            if(ErrorContext == Sav_CheckSpaceBoot || ErrorContext == Rwd_CheckSpaceBoot)
            {
                _neededSpaceDiskKB += kbytes;
                // message will be displayed later
            } else
            {
                u32 neededSpaceKB = kbytes - _freeSpaceDiskKB; 

                setCustomParam_u32(neededSpaceKB);
                addMessage(Sav_NoEnoughSpace);
            }            
            res = bfalse;
        } else
        {
            // Fake Free space
            _freeSpaceDiskKB -= kbytes;
        }

        switch(ErrorContext)
        {
        case Sav_CheckSpaceBoot:    SetBootRequiredChecked(Boot_SaveDataSpaceDisk); break;
        case Rwd_CheckSpaceBoot:    SetBootRequiredChecked(Boot_RewardsSpaceDisk);  break;
        }

        return res;
    }
    //-------------------------------------------------------------------------------------
    void TRCMessage_UIMessageSystemCB(int buttonType, void *userData)
    {
#ifdef TRC_USE_SYSTEM_MSG
        switch(buttonType)
        {
        case CELL_MSGDIALOG_BUTTON_NONE:

            break;
        case CELL_MSGDIALOG_BUTTON_OK: // OR CELL_MSGDIALOG_BUTTON_YES

            break;
        case CELL_MSGDIALOG_BUTTON_NO:

            break;
        case CELL_MSGDIALOG_BUTTON_ESCAPE:

            break;
        case CELL_MSGDIALOG_BUTTON_INVALID:
        default:

            break;
        }
#endif //TRC_USE_SYSTEM_MSG
    }
    //-------------------------------------------------------------------------------------
    bbool TRCManagerAdapter_PS3::_openUISystemMessage(TRCMessage_Critical* pMessage)
    {    
#ifdef TRC_USE_SYSTEM_MSG
        int messageType = 0;
        switch(pMessage->getUISystemButtons())
        {
            case TRCMessage_Critical::UI_NoButton:      messageType |= CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE;    break;
            case TRCMessage_Critical::UI_OkButton:      messageType |= CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK;      break;
            case TRCMessage_Critical::UI_YesNoButton:   messageType |= CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO;   break;
        }
        messageType |= (pMessage->isSoundEnable()) ? CELL_MSGDIALOG_TYPE_SE_MUTE_OFF : CELL_MSGDIALOG_TYPE_SE_MUTE_ON;
        messageType |= (pMessage->isBackgroundEnable()) ? CELL_MSGDIALOG_TYPE_BG_VISIBLE : CELL_MSGDIALOG_TYPE_BG_INVISIBLE;
        messageType |= (pMessage->isBackButtonEnable()) ? CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF : CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON;

        int ret = cellMsgDialogOpen2(
            messageType,                    // Type of message
            (char*)pMessage->message().convertStringClasstoUtf8Str(),     // UTF-8 message
            TRCMessage_UIMessageSystemCB,   // Callback for user response
            pMessage,                       // user data
            NULL
        );

        if(pMessage->getTimeOut() != 0)
        {
            cellMsgDialogClose(pMessage->getTimeOut()*1000.0f);
        }
#endif //TRC_USE_SYSTEM_MSG
        return btrue;
    }
    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::onFirstTRCFrame()
    {
        ITF_ASSERT(_currentBootTask==BootTask_None);
        if(_currentBootTask==BootTask_None && _isInitialized)
        {
            _currentBootTask ++;
            _taskManager.pushTask(startBootTask, onBootTaskFinish, this);
        }        
    }
    //-------------------------------------------------------------------------------------
    static DWORD WINAPI gameContentNoSpaceDialogThread(void *_param)
    {
        u32 _neededSpaceDiskKB = (u32)_param;
        int ret = cellGameContentErrorDialog(CELL_GAME_ERRDIALOG_NOSPACE_EXIT, _neededSpaceDiskKB, NULL);
        if (ret != CELL_GAME_RET_OK)
        {
            ITF_ASSERT(0);
        }
        return 0;
    }

    static DWORD WINAPI gameContentDiskErrorDialogThread(void *_param)
    {
        int ret = cellGameContentErrorDialog(CELL_GAME_ERRDIALOG_BROKEN_EXIT_HDDGAME, 0, NULL);
        if (ret != CELL_GAME_RET_OK)
        {
            ITF_ASSERT(0);
        }
        return 0;
    }

    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::checkForBootSpaceDisk()
    {
        if(!_isInitialized) return;
        ITF_ASSERT(!_bootRequirementTested);

        if ( _currentBootTask  != BootTask_Last  )
            return ; 

        // Check if the boot requirement is available for testing
        for (u32 i = 0;  i < Boot_Requirements_Count; i++)
        {
            if(!_bootRequirementStatus[i]) return;
        }
        
        _bootRequirementTested = btrue;


        if (!_keyFileCheckSuccess)
        {
            _thread = THREADMANAGER->createThread(gameContentDiskErrorDialogThread, 
                (void*)0, 
                ThreadSettings::m_settings[eThreadId_SaveManager]);  
        }
        else if (_neededSpaceDiskKB > 0)
        {
            u32 _finalSpaceDiskKBNeeded = _neededSpaceDiskKB - _freeSpaceDiskKB;

            _thread = THREADMANAGER->createThread(gameContentNoSpaceDialogThread, 
                (void*)_finalSpaceDiskKBNeeded, 
                ThreadSettings::m_settings[eThreadId_SaveManager]);  
        }
    }
    //-------------------------------------------------------------------------------------
    TRCManagerAdapter_PS3::~TRCManagerAdapter_PS3()
    {
    }
    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::init()
    {
        TRCManagerAdapter::init();

        // Set the non used boot requirement
        // TODO: add a better way to use this feature
        SetBootRequiredChecked(Boot_GameDataSpaceDisk);
        SetBootRequiredChecked(Boot_GameSpecificDataSpaceDisk);

#if defined(ITF_PS3_QUIT_INCREMENT_CHECK) || defined(ITF_PS3_DISC_FAILURE_INCREMENT_CHECK)
        g_quitTargetCount = loadQuitCount(QuitCountFileName);
        printf("Exit at check event/io number %d\n", g_quitTargetCount);
#endif
    }

    void TRCManagerAdapter_PS3::term()
    {
        _taskManager.cancelAllTasks();
    }

    //-------------------------------------------------------------------------------------
    TRCManagerAdapter_PS3::TRCManagerAdapter_PS3() :
        _thread(NULL), _bootRequirementTested(bfalse)
        , _currentBootTask(BootTask_None)
        , _keyFileCheckSuccess(bfalse)
    {
        for(ux i = 0; i < Boot_Requirements_Count; ++i)
            _bootRequirementStatus[i] = bfalse;

        for(ux i = 0; i < IO_THREAD_NB_MAX; ++i)
            m_ioThreadBlocked[i] = bfalse;
    }
    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::update()
    {
        TRCManagerAdapter::update();

        if(!_isInitialized) return;
        if(!_bootRequirementTested)
        {
            checkForBootSpaceDisk();
        }

        _taskManager.update();
    }

    bbool TRCManagerAdapter_PS3::checkKeyFile()
    {
        // running on the task thread
        if (!SYSTEM_ADAPTER_PS3->isHDDMode()
            || SYSTEM_ADAPTER_PS3->isLaunchedFromAppHome())
        {
            _keyFileCheckSuccess = btrue; // if not HDD -> file check is OK
            return btrue;
        }

        const SceNpDrmKey k_licensee =
        {
            { 0x0C, 0x01, 0xC7, 0x39, 0x2A, 0xFC, 0x90, 0x02, 0xB2, 0x3D, 0xCD, 0x7D, 0x5B, 0x67, 0xD3, 0x13 }
        };

        const char NpDRMFileName[] = "key.edat";
        const char * bootPath = SYSTEM_ADAPTER_PS3->getBootPath();
        char * NpDRMFilePath = newAlloc(mId_System, char[strlen(bootPath)+ strlen(NpDRMFileName)+1]);
        strcpy(NpDRMFilePath, bootPath);
        strcat(NpDRMFilePath, NpDRMFileName);

        CellFsStat sb;
        // check existence
        if (CELL_FS_SUCCEEDED != cellFsStat(NpDRMFilePath, &sb))
        {
            delete NpDRMFilePath;
            _keyFileCheckSuccess = bfalse;
            return bfalse;            
        }
        // check DRM
        if (CELL_OK != sceNpDrmIsAvailable2(&k_licensee, NpDRMFilePath))
        {
            delete NpDRMFilePath;
            _keyFileCheckSuccess = bfalse;
            return bfalse;
        }
        // open as DRM EDATA (by using the "undocumented" additional cellFsOpenArg)
        SceNpDrmOpenArg arg;
        arg.flag = SCE_NP_DRM_OPEN_FLAG;
        i32 handle = -1;
        CellFsErrno err = cellFsOpen(NpDRMFilePath, CELL_FS_O_RDONLY, &handle, &arg, sizeof(arg));
        delete NpDRMFilePath;
        if (err != CELL_FS_SUCCEEDED)
        {
            _keyFileCheckSuccess = bfalse;
            return bfalse;
        }
        const char keyText[] = "{+\"5O1@*~#B~|k]@72-I)^[U{5Ky%[";
        char keyTextRead[sizeof(keyText)];
        u64 nRead = 0;
        if (CELL_FS_SUCCEEDED != cellFsRead(handle, keyTextRead, strlen(keyText), &nRead) 
            || nRead != strlen(keyText))
        {
            cellFsClose(handle);
            _keyFileCheckSuccess = bfalse;
            return bfalse;
        }
        // close string
        keyTextRead[strlen(keyText)] = 0;
        _keyFileCheckSuccess = !strcmp(keyTextRead, keyText);
        cellFsClose(handle);        
        return _keyFileCheckSuccess;
    }

    //-------------------------------------------------------------------------------------
    i32 TRCManagerAdapter_PS3::startBootTask( void* pParam )
    {
        TRCManagerAdapter_PS3* pTRCManager = (TRCManagerAdapter_PS3*)pParam;
        if(!pParam || TRC_ADAPTER->getCurrentExitValue() != Exit_None)
        {
#ifndef ITF_FINAL
            printf("TRCManagerAdapter_PS3::startBootTask is leaving. Boot task aborted.\n"); // LOG() can be destroyed here
#endif //ITF_FINAL
            return 0;
        }

        switch (pTRCManager->_currentBootTask)
        {
        case BootTask_KeyFileCheck:
            {
                pTRCManager->checkKeyFile();
                pTRCManager->SetBootRequiredChecked(Boot_KeyFileCheck);
            }
            break;
        case BootTask_Save:
            if(SAVEGAME_ADAPTER_PS3) 
            {
                if(!SAVEGAME_ADAPTER_PS3->initForBoot())
                {
                    pTRCManager->SetBootRequiredChecked(Boot_SaveDataSpaceDisk);
                }
            }
            break;
        case BootTask_Rewards: 
            if(REWARD_ADAPTER_PS3) 
            {
                if (!REWARD_ADAPTER_PS3->initForBoot())
                {
                    pTRCManager->SetBootRequiredChecked(Boot_RewardsSpaceDisk);
                }
            }
            break;
        }
        return 0 ;
    }

    //-------------------------------------------------------------------------------------
    void TRCManagerAdapter_PS3::onBootTaskFinish( void* pParam, i32 _result )
    {
        TRCManagerAdapter_PS3* pTRCManager = (TRCManagerAdapter_PS3*)pParam;
        pTRCManager->_currentBootTask ++;

        switch (pTRCManager->_currentBootTask)
        {
        case BootTask_Last:
            break;
        default:
            pTRCManager->_taskManager.pushTask(startBootTask, onBootTaskFinish, pParam);
        }
    }
    //-------------------------------------------------------------------------------------
    bbool TRCManagerAdapter_PS3::checkIOError( ErrorContext errorContext, i32 & _errorCode )
    {
        if(!_isInitialized) return btrue;

#ifdef ITF_PS3_DISC_FAILURE_INCREMENT_CHECK
        u32 prevVal = cellAtomicIncr32(&g_quitCount);
        if (g_quitTargetCount == prevVal)
        {
            saveQuitCount(QuitCountFileName, prevVal+1);
            printf("Auto io error increment check %d \n", prevVal);
            ITF_MemoryBarrier();
            g_quitStart = sys_time_get_system_time();
            _ioError = IOErr_Internal;
            _debugExitValue = Exit_Shutdown;
            ITF_MemoryBarrier();
        }
#endif

        // No error found? Check if we have to create a fake one
        if(_errorCode == CELL_FS_SUCCEEDED)
        {
            switch (_ioError)
            {
            case IOErr_DiscNotFound:    _errorCode = CELL_FS_EBADF; break;
            case IOErr_FileNotFound:    _errorCode = CELL_FS_ENOENT; break;
            case IOErr_Internal:        _errorCode = CELL_FS_EIO; break;
            }

            // Reset error 
            _ioError = IOErr_None;
        }

        // Do not allow an IO operation from the main thread
#ifndef ITF_FINAL
        if(errorContext == IO_ReadingFile || errorContext == IO_OpeningFile 
            || errorContext == IO_WritingFile || errorContext == IO_SeekingFile)
        {
            if (Synchronize::getCurrentThreadId() != ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
            {
                //printf("Loading from main thread\n");
            }
        }        
#endif //ITF_FINAL

        if(_errorCode == CELL_FS_EIO)
        {
#ifndef ITF_FINAL
            printf("[IO ERROR] IO error detected (0x%x), THREAD ID: %x\n", _errorCode, Synchronize::getCurrentThreadId());
#endif //ITF_FINAL

            if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
            {
                if (ms_ioErrorOnMainCB)
                {
                    (*ms_ioErrorOnMainCB)(ms_ioErrorOnMainCBUsrData);
                }
                ITF_ASSERT_CRASH(0, "First reading happened and failed too late."); // Should never reach this. I
                _Exit(0);
            }
            else
            {
                // Important internal thread
                ITF_THREAD_ID thrId = Synchronize::getCurrentThreadId();
                bbool hasToSleep = (thrId == ThreadSettings::m_settings[eThreadId_ResourceManager].m_threadID)
                    || (thrId == ThreadSettings::m_settings[eThreadId_SceneManager].m_threadID)
                    || (thrId == ThreadSettings::m_settings[eThreadId_loadingSequence].m_threadID);

                // The kill will be handled by "FastReset" from framework
                if(hasToSleep)
                {
#ifndef ITF_FINAL
                    printf("[IO ERROR] THREAD is now sleeping...\n");
#endif //ITF_FINAL
                    if (thrId == ThreadSettings::m_settings[eThreadId_ResourceManager].m_threadID)
                        m_ioThreadBlocked[IO_THREAD_RESOURCE_MGR] = btrue;
                    if (thrId == ThreadSettings::m_settings[eThreadId_SceneManager].m_threadID)
                        m_ioThreadBlocked[IO_THREAD_SCENE_MGR] = btrue;
                    if (thrId == ThreadSettings::m_settings[eThreadId_loadingSequence].m_threadID)
                        m_ioThreadBlocked[IO_THREAD_LOADING_SEQ] = btrue;

                    for(;;) { sleep(10000); }
                }
            }
        }

        if (_errorCode != CELL_FS_SUCCEEDED && _errorCode != CELL_FS_ERROR_ENOENT)
        {
#ifndef ITF_FINAL
            fprintf(stderr, "IO error 0x%08X reported\n", _errorCode);
#endif //ITF_FINAL
        }

        return (_errorCode == CELL_FS_SUCCEEDED);
    }

    void TRCManagerAdapter_PS3::checkExitCode( i32& _exitCode )
    {
        // on PS3, we can only quit the game (no restart/no back to main menu)
        // and the exit code is a boolean which means if the user wants to exit the game
        if(_debugExitValue == Exit_Shutdown)
        {
            _exitCode = 1;
            _debugExitValue = Exit_None;
        }

#ifdef ITF_PS3_QUIT_INCREMENT_CHECK
        if (_exitCode!=1)
        {
            u32 prevVal = cellAtomicIncr32(&g_quitCount);
            if (g_quitTargetCount == prevVal)
            {
                saveQuitCount(QuitCountFileName, prevVal+1);
                printf("Auto quit increment check %d\n", prevVal);
                ITF_MemoryBarrier();
                g_quitStart = sys_time_get_system_time();
                _exitCode = 1;
                ITF_MemoryBarrier();
            }
        }
#endif

        _currentExitValue = (_exitCode == 1) ? Exit_Shutdown : Exit_None;
    }

    bbool TRCManagerAdapter_PS3::isBootRequirementsFinished()
    {
        return (!_isInitialized) || ((_currentBootTask  == BootTask_Last ) && ( _bootRequirementTested));
    }

    bbool TRCManagerAdapter_PS3::isIOThreadBlocked(TRCManagerAdapter_PS3::IOThread _ioThread) const
    {
        return m_ioThreadBlocked[_ioThread];
    }

    void TRCManagerAdapter_PS3::ReCreateSaveFilesAfterCorruption()
    {
        SAVEGAME_ADAPTER_PS3->deleteSaveFile();
    }

    void TRCMessage_SavCorruptedFile_PS3::onLeftButtonAction()
    {
        TRC_ADAPTER_PS3->ReCreateSaveFilesAfterCorruption();
    }

    void TRCMessage_SavCorruptedFile_PS3::onRightButtonAction()
    {
        SAVEGAME_ADAPTER_PS3->retryReadingSaveFile();
    }

} // namespace ITF

