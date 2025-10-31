#include "precompiled_Adapter_Savegame_x360.h"

#ifndef _ITF_ADAPTER_SAVEGAME_X360_H__
#include "adapters/Adapter_Savegame_x360/Adapter_Savegame_x360.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_NETWORKSERVICES_X360_H_
#include "engine/NetworkServices/X360/NetworkServices_x360.h"
#endif //_ITF_NETWORKSERVICES_X360_H_

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#define MAX_SLOTS           3
#define SAVEGAME_DRIVE_LETTER "save"
#define SAVEGAME_BASENAME   "Savegame"
#define LOCALISATION_ID_SAVE_NAME   4127
#define SAVEGAME_FILESYSTEM_EXTRAROOM (128*1024)

#define ZEROVARIABLE(x) ZeroMemory(&x, sizeof(x))
#define SF_CLOSEHANDLE(x) {if ((x)!=INVALID_HANDLE_VALUE) {CloseHandle(x); x = INVALID_HANDLE_VALUE;} }

namespace ITF
{
    //#pragma optimize("",off)
    XCONTENT_DATA Adapter_Savegame_x360::m_tmpDeviceContent[xcontent_buffer_item_count];
    static DWORD g_acceptedNotifications[]=
    {
        XN_SYS_STORAGEDEVICESCHANGED
    };

    Adapter_Savegame_x360::Adapter_Savegame_x360() :
        m_contentEnumerationHandle(INVALID_HANDLE_VALUE),
            m_mainState(State_Idle),
            m_choseDevice_PlayerIndex(0),
            m_saving_PlayerIndex(0),
            m_mainPlayerIndex(0),
            m_contentEnumeration_PlayerIndex(0),
            m_wasEnumerated(bfalse),
            m_lastEnumerationFailed(bfalse),
            m_startCheckDeviceOkASAP(bfalse),
            m_statedWhenDeferredEnumerationFailed(State_Invalid),
            m_statedWhenDeferredEnumerationOk(State_Invalid),
            m_chooseDevice_newDeferredStateWhenDeviceSelected(State_Invalid),
            m_chooseDevice_newDeferredStateWhenDeviceHasError(State_Invalid),
            m_chooseDevice_newDeferredStateWhenConfirmedNoDevice(State_Invalid),
            m_chooseDevice_ForceUI(bfalse),
            m_mustSwitchToSaveErrorState(bfalse),
            m_mustSwitchToLoadErrorState(bfalse),
            m_chooseDevice_SilentWhenChosenNone(bfalse),
            m_reinforceOverwriteChecksForNewGame(bfalse),
            m_MaxPlayersForSaving(1),
            m_FirstDeviceChange(btrue),
            m_selectedDeviceIndex(U32_INVALID),
            m_currentSlotIndex(0),
            m_systemMessageListener(g_acceptedNotifications, ARRAYSIZE(g_acceptedNotifications)),
            m_threadIsRunning(bfalse),
            m_deviceChangedDuringSave(bfalse),
            m_thread(NULL),
            Adapter_Savegame()
        {
            ZEROVARIABLE(m_overlappedDevSelector);
            ZEROVARIABLE(m_overlappedContentEnum);
            ZEROVARIABLE(m_storageDevice);
            ZEROVARIABLE(m_overlappedSaving);
        }

        void Adapter_Savegame_x360::init()
        {
            static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER)->registerSystemMessageListener(&this->m_systemMessageListener);

            //        tests();
        }

        void Adapter_Savegame_x360::preLoad(u32 _player, u32 _maxPlayersToUse, bbool _force)
        {
            m_mainPlayerIndex = _player;
            m_MaxPlayersForSaving = _maxPlayersToUse;
            if (! NETWORKSERVICES->isUserSignedIn(_player))
            {
                NETWORKSERVICES->askForSignIn(_maxPlayersToUse);
            }

            // We start a new game, we want to re-enable the save system 
            // Because, we can came from a disabled system (ex: when you remove the MU before going back in the main menu)
            if(_force)
                enableSaveSystem();

            setMainState(PreLoad_Running);
            if(_force || m_startCheckDeviceOkASAP)
                setDeviceChosenFlags(bfalse,bfalse); 
        }

        void Adapter_Savegame_x360::checkForSaveStateValidity(u32 _playerIndex)
        {
            checkIfDeviceChanged();
            if (m_startCheckDeviceOkASAP)
            {            
                if(IsSaveSystemEnable())
                {
                    if ( m_storageDevice != XCONTENTDEVICE_ANY && isCurrentDeviceDisconnected() )
                    {
                        setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle);
                    }
                    else if ( m_storageDevice == XCONTENTDEVICE_ANY )
                    {
                        setDeviceChosenFlags(bfalse,bfalse); 
                        startChooseDevice(_playerIndex, m_chooseDevice_MinFreeSize, bfalse);
                    }
                }

                m_startCheckDeviceOkASAP = bfalse;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Adapter_Savegame_x360::canUserUseStorage(u32 _player, bbool _checkStorageDeviceChosen)
        {
            bbool r = NETWORKSERVICES->isUserSignedIn(_player) &&             
                !SYSTEM_ADAPTER->isTrialMode();
            if (_checkStorageDeviceChosen)
                r = r && (m_storageDeviceIsChosen && !m_storageDeviceIsChosenAsNone);
            return r;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::tests()
        {

#if 0
            while (! NETWORKSERVICES->isUserSignedIn(0))
            {
                NETWORKSERVICES->update();
                SYSTEM_ADAPTER->present();
            }

            startChooseDevice(0, 1024*1024*64, btrue);
            while (hasPendingOperation())
            {
                update();
                SYSTEM_ADAPTER->present();
            }
            SafeArray<u8> savetst;
            for (u32 i=0; i<512; i++)
                savetst.push_back((u8)i);

            startSave(0, "hello", String((u16*)L"20.05.2011 - 10:59, World 4 "), &savetst[0], savetst.size(), bfalse);
            while (hasPendingOperation())
            {
                update();
                SYSTEM_ADAPTER->present();
            }
            //
            startSavegameEnumerationUpdate(0);
            while (hasPendingOperation())
            {
                update();
                SYSTEM_ADAPTER->present();
            }

            LOG("---Found savegames:");
            u32 enumCount = getEnumeratedContentCount(0);
            for (u32 enumIndex=0; enumIndex<enumCount; enumIndex++)
            {
                String displayName;
                String8 baseName;
                bbool emptySave;
                bbool ok = getEnumeratedContent(0, enumIndex, baseName, displayName, emptySave);
                ITF_ASSERT(ok);
                LOG("%ls (%s)", displayName.cStr(), baseName.cStr());
            }

            String displayName;
            ErrorCode code = startLoad(0, "hello");        
            ITF_ASSERT(code==Error_Ok);
            const ITF_VECTOR<u8> &loaded = getLastLoadedOrSavedData();
            ITF_ASSERT(savetst.size()==loaded.size());
            for (u32 i=0; i<savetst.size(); i++)
                ITF_ASSERT(savetst[i]==loaded[i]);
#endif
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::setMainState(MainState _state)
        {
            if (_state == m_mainState)
                return;
            ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

            //mode switching : 1st leaving a special screen
            switch(m_mainState)
            {
            case Save_AskOverWrite: firePromptCallback(PromptType_FileAlreadyExists, bfalse); break;
            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle:
            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother: firePromptCallback(PromptType_DeviceWhenDeviceNoLongerValidForSave, bfalse); break;
            case Save_IOError_AskWhatToDo: firePromptCallback(PromptType_SaveError, bfalse); break;
            case ChooseDevice_ChosenNone_RetryOrContinue: firePromptCallback(PromptType_DeviceChosenAsNone, bfalse); break;
            case Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother: firePromptCallback(PromptType_DeviceWhenDeviceNoLongerValidForLoad, bfalse); break;
            case Load_IOError_AskWhatToDo: firePromptCallback(PromptType_LoadError, bfalse); break;
            case Load_CantFind_AskWhatToDo: firePromptCallback(PromptType_LoadCantFind, bfalse); break;
            case Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer: firePromptCallback(PromptType_FileAlreadyExistsForNewGame, bfalse); break;
            case PreLoadFailed: firePromptCallback(PromptType_UserNotSignedIn, bfalse); break;
            case Load_IOError_DeletingConfirmation: firePromptCallback(PromptType_DeletingCorruptedFile, bfalse); break;
            };

            // If an IO Error happens, we must check if this error is caused by a device removed. 
            // If true, just signal the user, and stay IDLE after. We don't want to continue the previous action
            if(_state == Save_IOError_AskWhatToDo || _state == Load_CantFind_AskWhatToDo || _state == Load_IOError_AskWhatToDo)
            {
                checkIfDeviceChanged();
                if ( m_startCheckDeviceOkASAP && isCurrentDeviceDisconnected() )
                {
                    _state = Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle; 
                    m_lastLoadSaveError = Error_SavegameNotFound;
                    m_startCheckDeviceOkASAP = bfalse;
                }
            }

            //then : entering a special screen
            switch(_state)
            {
            case Save_AskOverWrite: firePromptCallback(PromptType_FileAlreadyExists, btrue); break;
            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle:
            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother: firePromptCallback( PromptType_DeviceWhenDeviceNoLongerValidForSave, btrue); break;
            case Save_IOError_AskWhatToDo: firePromptCallback(PromptType_SaveError, btrue); break;
            case Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer: firePromptCallback(PromptType_FileAlreadyExistsForNewGame, btrue); break;
            case ChooseDevice_ChosenNone_RetryOrContinue: firePromptCallback(PromptType_DeviceChosenAsNone, btrue); break;
            case Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother: firePromptCallback(PromptType_DeviceWhenDeviceNoLongerValidForLoad, btrue); break;
            case Load_IOError_AskWhatToDo: firePromptCallback(PromptType_LoadError, btrue); break;
            case Load_CantFind_AskWhatToDo : firePromptCallback(PromptType_LoadCantFind, btrue); break;
            case PreLoadFailed: firePromptCallback(PromptType_UserNotSignedIn, btrue); break;
            case Load_IOError_DeletingConfirmation: firePromptCallback(PromptType_DeletingCorruptedFile, btrue); break;
            };

            m_mainState = _state;

            //
            if (_state==State_Idle && m_currentOperationCompleteCallback!=NULL)
            {
                m_currentOperationCompleteCallback(); 
                m_currentOperationCompleteCallback = NULL;
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::notifyStorageChanged()
        {
            if (m_deviceChangedCallback)
                m_deviceChangedCallback();

            //If changed and was already chosen, prepare device choice
            // if (m_FirstDeviceChange || (m_storageDeviceIsChosen && !m_storageDeviceIsChosenAsNone))
            {
                // TRC: 049
                // The user is prompted to select a storage device and can continue with saving
                m_startCheckDeviceOkASAP = btrue;
            }

            m_FirstDeviceChange = bfalse;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::checkIfDeviceChanged()
        {
            DWORD message;
            ULONG_PTR param;
            while(m_systemMessageListener.popMessage(message, param))
            {
                switch(message)
                {
                case XN_SYS_STORAGEDEVICESCHANGED:
                    notifyStorageChanged();
                    break;
                default:
                    ITF_ASSERT_MSG(bfalse, "Unhandled message");
                    break;

                }
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::term()
        {
            static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER)->unregisterSystemMessageListener(&this->m_systemMessageListener);
            ITF_ASSERT(m_mainState == State_Idle);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::cancelCurrentContentEnumeration()
        {
            if (m_contentEnumerationHandle!=INVALID_HANDLE_VALUE)
            {
                XCancelOverlapped(&m_overlappedContentEnum);
                SF_CLOSEHANDLE(m_contentEnumerationHandle);
                m_wasEnumerated = bfalse;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_preLoadRunning()
        {
            if(!NETWORKSERVICES->isUserSignInProcessRunning() && !NETWORKSERVICES->isUserSignedIn(m_mainPlayerIndex))
            {            
                setMainState(PreLoadFailed);
            }
            else if(NETWORKSERVICES->isUserSignedIn(m_mainPlayerIndex))
            {
                if(!m_storageDeviceIsChosen)
                {
                    startChooseDevice(m_mainPlayerIndex, m_chooseDevice_MinFreeSize, bfalse);
                } else
                {
                    setMainState(State_Idle);
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_MustChooseDevice()
        {
            DWORD errorCode;
            setDeviceChosenFlags(bfalse,bfalse);
            ULARGE_INTEGER dataSize;
            ITF_ASSERT(m_chooseDevice_MinFreeSize>0);

            ULONGLONG xdataSize = XContentCalculateSize(m_chooseDevice_MinFreeSize, 1);        
            dataSize.QuadPart = xdataSize+SAVEGAME_FILESYSTEM_EXTRAROOM;
            DWORD contentFlags = m_FirstDeviceChange ? XCONTENTFLAG_NONE : XCONTENTFLAG_FORCE_SHOW_UI;

            errorCode = XShowDeviceSelectorUI( m_choseDevice_PlayerIndex, XCONTENTTYPE_SAVEDGAME, contentFlags, dataSize, &m_storageDevice, &m_overlappedDevSelector );
            if (errorCode==ERROR_IO_PENDING)
            {
                setMainState(ChooseDevice_ShowingSelector);
                setDeviceChosenFlags(bfalse,bfalse);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Adapter_Savegame_x360::getEnumeratedContent(u32 /*_playerIndex*/, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave)
        {
            ITF_ASSERT(!hasPendingOperation());


            if(m_slotList.size()>_index && m_filteredDeviceContent.size()>0)
            {
                _baseName       = m_filteredDeviceContent[0].data.szFileName;
                _displayName    = (u16*)m_slotList[_index].displayedName;
                _emptySave      = (m_slotList[_index].fileSize == 0);
            } else
            {
                _emptySave = btrue;
            }
            return btrue;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startSavegameEnumerationUpdate(u32 _playerIndex)
        {
            bbool couldStartEnumeration = startContentEnumeration(_playerIndex);
            return couldStartEnumeration?Error_Ok:getLastAsyncErrorCode();

        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex)
        {
            if (!m_storageDeviceIsChosen && !m_storageDeviceIsChosenAsNone)
            {
                startChooseDevice(_playerIndex, 0, bfalse);
                return Error_Ok;
            } else
            {
                XCONTENTDEVICEID oldDeviceID = m_storageDevice;
                bbool oldDeviceIsChosen = m_storageDeviceIsChosen;
                bbool oldDeviceIsChosenAsNone = m_storageDeviceIsChosenAsNone;

                m_storageDeviceIsChosen = btrue;
                m_storageDeviceIsChosenAsNone = bfalse;
                m_storageDevice = XCONTENTDEVICE_ANY;

                ErrorCode errorResult = startSavegameEnumerationUpdate(_playerIndex);

                m_storageDeviceIsChosen = oldDeviceIsChosen;
                m_storageDeviceIsChosenAsNone = oldDeviceIsChosenAsNone;
                m_storageDevice = oldDeviceID;

                return errorResult;
            }        
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Adapter_Savegame_x360::startContentEnumeration(u32 _player, MainState _stateWhenDeferredOk, MainState _stateWhenDeferredFailure)
        {
            m_lastEnumerationFailed = btrue;
            if (!canUserUseStorage(_player, bfalse))
            {
                m_lastLoadSaveError = Error_LoadOrSaveForbiddenForPlayer;
                return bfalse;
            }
            if (!m_storageDeviceIsChosen || m_storageDeviceIsChosenAsNone)
            {
                m_lastLoadSaveError = Error_CouldNotEnumerate;
                return bfalse;
            }

            //Start enumeration
            DWORD errorCode;
            DWORD enumBuffSize;        

            m_contentEnumeration_PlayerIndex = _player;

            cancelCurrentContentEnumeration();
            m_filteredDeviceContent.clear();

            SF_CLOSEHANDLE(m_contentEnumerationHandle);
            m_statedWhenDeferredEnumerationFailed = _stateWhenDeferredFailure;
            m_statedWhenDeferredEnumerationOk = _stateWhenDeferredOk;

            errorCode = XContentCreateEnumerator(_player, m_storageDevice, 
                XCONTENTTYPE_SAVEDGAME, 
                XCONTENTFLAG_ENUM_EXCLUDECOMMON, //content flags
                xcontent_buffer_item_count, //Nb of elements to be retrieved, here we only want content count
                &enumBuffSize,
                &m_contentEnumerationHandle);
            ITF_ASSERT(enumBuffSize == sizeof(XCONTENT_DATA)*xcontent_buffer_item_count);

            if (errorCode != ERROR_SUCCESS || m_contentEnumerationHandle==INVALID_HANDLE_VALUE)
            {
                SF_CLOSEHANDLE(m_contentEnumerationHandle);
                //There was a problem with enumeration
                m_lastLoadSaveError = Error_CouldNotEnumerate;
                return bfalse;
            }
            else
            {            
                enumBuffSize = sizeof(XCONTENT_DATA)*xcontent_buffer_item_count;


                if (XEnumerate(m_contentEnumerationHandle, &m_tmpDeviceContent[0], enumBuffSize, NULL, &m_overlappedContentEnum) !=ERROR_IO_PENDING)
                {
                    SF_CLOSEHANDLE(m_contentEnumerationHandle);
                    //There was a problem with enumeration
                    m_lastLoadSaveError = Error_CouldNotEnumerate;
                    return bfalse;
                }
                else
                {
                    m_lastEnumerationFailed = bfalse;
                    setMainState(EnumeratingContent);
                    return btrue;
                }

            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_JustClosedDeviceSelectorGUI()
        {
            m_startCheckDeviceOkASAP = bfalse;

            // dev selector exit code :
            // if B was hit:  XGetOverlappedResult = 65B (ERROR_FUNCTION_FAILED) , XGetOverlappedExtendedError : 4C7 (Error_CANCELLED)
            // if GUIDE was hit : XGetOverlappedResult=XGetOverlappedExtendedError = 4C7 

            DWORD overlappedRes = XGetOverlappedResult(&m_overlappedDevSelector, NULL, FALSE);
            //DWORD extRes = XGetOverlappedExtendedError(&m_overlappedDevSelector);
            if (overlappedRes!=ERROR_SUCCESS)
            {
                //The user hit B. We want to show a "no device selected - retry or continue?" msg if allowed
                setDeviceChosenFlags(btrue, btrue);
                if (m_chooseDevice_SilentWhenChosenNone)
                    setMainState(m_chooseDevice_newDeferredStateWhenDeviceSelected);
                else
                {
                    setMainState(ChooseDevice_ChosenNone_RetryOrContinue);
                }
            }
            else
            {
                //1st, presume choice is ok
                setDeviceChosenFlags(btrue, bfalse);
                //then enumerate
                bbool couldStartEnumeration = startContentEnumeration(m_choseDevice_PlayerIndex,
                    m_chooseDevice_newDeferredStateWhenDeviceSelected,
                    m_chooseDevice_newDeferredStateWhenDeviceHasError);

                if (couldStartEnumeration)
                    setDeviceChosenFlags(btrue, bfalse);
                else
                    setDeviceChosenFlags(bfalse, bfalse);

                if (!couldStartEnumeration)
                    setMainState(m_chooseDevice_newDeferredStateWhenDeviceHasError);
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startChooseDevice_Internal(u32 _playerIndex, u32 _minFreeSize,
            MainState _newDeferredStateWhenDeviceSelected, 
            MainState _newDeferredStateWhenDeviceHasError,
            MainState _newDeferredStateWhenConfirmedNoDevice,
            bbool _forceUI,
            bbool _silentWhenChosenNone)
        {
            m_startCheckDeviceOkASAP = bfalse;
            m_chooseDevice_SilentWhenChosenNone = _silentWhenChosenNone;

            if (!canUserUseStorage(_playerIndex, bfalse))
                return Error_LoadOrSaveForbiddenForPlayer;
            m_choseDevice_PlayerIndex = _playerIndex;
            setMainState(ChooseDevice_PostedShowSelector);
            m_filteredDeviceContent.clear();
            m_chooseDevice_ForceUI = _forceUI;
            m_chooseDevice_MinFreeSize = _minFreeSize;
            m_chooseDevice_newDeferredStateWhenDeviceSelected = _newDeferredStateWhenDeviceSelected;
            m_chooseDevice_newDeferredStateWhenDeviceHasError = _newDeferredStateWhenDeviceHasError;
            m_chooseDevice_newDeferredStateWhenConfirmedNoDevice = _newDeferredStateWhenConfirmedNoDevice;
            setDeviceChosenFlags(bfalse,bfalse);

            return Error_Ok;

        }

        ///////////////////////////////////////////////////////////////////////////////////////////

        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
        {
            m_lastLoadSaveError = Error_Ok;
            if(!_minFreeSize) _minFreeSize = m_chooseDevice_MinFreeSize;
            return startChooseDevice_Internal(_playerIndex, _minFreeSize, State_Idle, State_Idle, State_Idle, bfalse, _isSilentWhenChosenNone);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::startReadingHeaders()
        {
            m_threadIsRunning = btrue;
            ITF_MemoryBarrier();
            m_thread = THREADMANAGER->createThread(headersThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);            
            if(m_thread==NULL)
            {
                m_threadIsRunning = bfalse;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_ReadHeaders()
        {
            if (m_threadIsRunning)
                return ; 
        
            XContentClose(SAVEGAME_DRIVE_LETTER, NULL);
            m_thread = NULL;

			// Special case to handle a corrupted save on a new DEVICE 
			// We have to prompt user if he wants to overwrite the save, not the corrupted message
            if ( m_statedWhenDeferredEnumerationOk == Save_DeviceWasChanged_StillValid && m_deviceChangedDuringSave )
            {                
                m_mustSwitchToLoadErrorState = bfalse;
            }

            if (m_mustSwitchToLoadErrorState)
            {
                m_mustSwitchToLoadErrorState = bfalse;
                setMainState(Load_IOError_AskWhatToDo);
            }
            else
                setMainState(m_statedWhenDeferredEnumerationOk);

            m_wasEnumerated = btrue;
            m_deviceChangedDuringSave = bfalse;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        u32 WINAPI Adapter_Savegame_x360::headersThreadRoutine(void *_param)
        {
            Adapter_Savegame_x360 *caller = (Adapter_Savegame_x360*)_param;
            //
            HANDLE file = INVALID_HANDLE_VALUE;
            ErrorCode returnedStatus=Error_Ok;

            for(u32 i = 0; i < caller->m_filteredDeviceContent.size() && returnedStatus == Error_Ok; i++)
            {
                // If the current save file is corrupted, do not read the headers
                if(caller->m_filteredDeviceContent[i].isCorrupted)
                {
                    returnedStatus = Error_LoadSave_IOError;
                    continue;
                }

                // Read content .. 
                DWORD errorCode = XContentCreate( caller->m_mainPlayerIndex, 
                    SAVEGAME_DRIVE_LETTER, 
                    &caller->m_filteredDeviceContent[i].data,
                    XCONTENTFLAG_OPENEXISTING, 
                    NULL, NULL, NULL);
                if( errorCode != ERROR_SUCCESS )
                {
                    returnedStatus = Error_LoadSave_IOError;
                    continue;
                }

                // Open Save file
                char filename[256];
                sprintf(filename, "%s:\\%s.sav", SAVEGAME_DRIVE_LETTER, caller->m_filteredDeviceContent[i].data.szFileName);

                file = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
                if (file==INVALID_HANDLE_VALUE)
                {
                    returnedStatus = Error_IncorrectSavegameFormat;
                }
                else
                {
                    DWORD totalFileSize = GetFileSize(file, NULL);
                    if (totalFileSize<caller->m_chooseDevice_MinFreeSize)
                    {
                        returnedStatus = Error_IncorrectSavegameFormat;
                    }
                    else
                    {              
                        // Start to fill the slot list (note: one slot if a Header, with the name of the save + CRC param to check the integrity of the data)
                        caller->m_slotList.clear();
                        for(u32 headerIndex = 0; headerIndex<MAX_SLOTS; headerIndex++)
                        {
                            DWORD totalReadSize=0;
                            BOOL couldRead = FALSE;
                            Header newHeader;

                            // Get size of one header, and move 
                            u32 fileSize = (u32)(caller->m_chooseDevice_MinFreeSize/ (f32)MAX_SLOTS);
                            SetFilePointer(file, fileSize*headerIndex, NULL, FILE_BEGIN);

                            couldRead = (ReadFile(file, &newHeader, getHeaderSize(), &totalReadSize, NULL));
                            if(!couldRead || totalReadSize != getHeaderSize())
                            {
                                returnedStatus = Error_LoadSave_IOError;
                                break;
                            }
                            else
                            {
                                caller->m_slotList.push_back(newHeader);   

                                // Check corrupted headers --> 1 header corrupted = the entire slot corrupted
                                if(caller->m_codeCRC != newHeader.CodeCRC && newHeader.CodeCRC != 0)
                                {
                                    LOG("SaveAdapter: Header corrupted; ");
                                    returnedStatus = Error_LoadSave_IOError;
                                    SF_CLOSEHANDLE(file);
                                    break;
                                }

                                // Continue to read the headers
                                returnedStatus = Error_Ok;
                            }
                        }

                    }

                    SF_CLOSEHANDLE(file);
                }
            }

            // Something wrong ? Just flag the file as a corrupted one
            if (returnedStatus!=Error_Ok)
            {
                caller->m_filteredDeviceContent[0].isCorrupted = btrue;
                caller->switchToLoadErrorState(returnedStatus);
            }

            ITF_MemoryBarrier();
            caller->m_threadIsRunning = bfalse;            
            return 0;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_AsynchronousContentEnumeration()
        {        
            ITF_ASSERT((m_mainState==EnumeratingContent) == (m_contentEnumerationHandle != INVALID_HANDLE_VALUE));
            //If there is a pending content enum, enumerate
            if (m_contentEnumerationHandle != INVALID_HANDLE_VALUE)
            {
                if (XHasOverlappedIoCompleted(&m_overlappedContentEnum))
                {
                    DWORD returnedCount=0;
                    DWORD dwStatus = XGetOverlappedResult( &m_overlappedContentEnum, &returnedCount, TRUE );

                    if ( ERROR_SUCCESS == dwStatus )
                    {
                        u32 deviceContentEntryCount = (u32)returnedCount;

                        //Enumerate every content, check player is the creator and add to m_filteredDeviceContent
                        for (i32 index=0; index<(i32)deviceContentEntryCount; index++)
                        {
                            BOOL isCreator=FALSE;
                            DWORD errorCode = XContentGetCreator(m_contentEnumeration_PlayerIndex, &m_tmpDeviceContent[index], &isCreator, NULL, NULL);

                            if ((errorCode==ERROR_SUCCESS) || (errorCode==ERROR_FILE_CORRUPT))
                            {
                                // Not a valid file name
                                if( strcmp (m_tmpDeviceContent[index].szFileName, SAVEGAME_BASENAME) != 0 )
                                    continue; 

                                Content cnt; 
                                cnt.data = m_tmpDeviceContent[index];
                                cnt.isCorrupted = (errorCode==ERROR_FILE_CORRUPT) || !isCreator;
                                m_filteredDeviceContent.push_back(cnt);
                                break;
                            }
                        }

                        //Continue enumeration
                        if (XEnumerate(m_contentEnumerationHandle, &m_tmpDeviceContent[0], sizeof(m_tmpDeviceContent), NULL, &m_overlappedContentEnum) != ERROR_IO_PENDING)
                        {
                            SF_CLOSEHANDLE(m_contentEnumerationHandle);
                            m_wasEnumerated = btrue;
                            m_lastEnumerationFailed = btrue;
                            setMainState(m_statedWhenDeferredEnumerationFailed);
                        } 
                        else
                        {
                            setMainState(ReadingHeader);
                            startReadingHeaders();
                            m_lastEnumerationFailed = bfalse;
                        }
                    }
                    else
                    {
                        bbool errorIsNoMoreFile = (HRESULT)XGetOverlappedExtendedError(&m_overlappedContentEnum)==HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);

                        SF_CLOSEHANDLE(m_contentEnumerationHandle);
                        if (errorIsNoMoreFile)
                        {
                            m_wasEnumerated = btrue;
                            setMainState(m_statedWhenDeferredEnumerationOk);
                            m_lastEnumerationFailed = bfalse;
                        }
                        else
                        {
                            m_wasEnumerated = btrue;
                            setMainState(m_statedWhenDeferredEnumerationFailed);
                            m_lastEnumerationFailed = btrue;
                        }
                    }
                }
            }

        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        i32 Adapter_Savegame_x360::findBasenameAmongEnumerated(const String8 &_baseName)
        {
            i32 foundIndex = -1;
            for (u32 i=0; i<m_filteredDeviceContent.size(); i++)
            {
                if (strcmp(m_filteredDeviceContent[i].data.szFileName, _baseName.cStr())==0)
                {
                    foundIndex=(i32)i;
                    break;
                }
            }
            return foundIndex;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Adapter_Savegame_x360::isCurrentDeviceDisconnected() const
        {
            return ( XContentGetDeviceState(m_storageDevice, NULL) ==  ERROR_DEVICE_NOT_CONNECTED );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startLoad(u32 _playerIndex, u32 _slotIndex, const String8 &baseName)
        {
            ITF_ASSERT(m_wasEnumerated);
            //
            ITF_ASSERT(!hasPendingOperation());
            if(hasPendingOperation())
                return Error_AlreadySaving; 

            if (!canUserUseStorage(_playerIndex, bfalse))
                return Error_LoadOrSaveForbiddenForPlayer;

            m_loadSaveBaseName  = SAVEGAME_BASENAME;
            m_loading_PlayerIndex = _playerIndex;
            m_isLoading = btrue;
            m_currentSlotIndex = _slotIndex;

            // If the device is not connected, signal the user, and stop the current action
            checkIfDeviceChanged();
            if (m_startCheckDeviceOkASAP && isCurrentDeviceDisconnected() )
                {
                    setMainState(Load_SignalCantLoad_WrongDevice);
                m_startCheckDeviceOkASAP = bfalse;
                    return Error_LoadSave_NoDevice;
                }

            setMainState(Load_Start);
            return Error_Ok;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Load_Start()
        {
            if (m_storageDeviceIsChosen && m_storageDeviceIsChosenAsNone)
            {
                m_lastLoadSaveError = Error_LoadSave_NoDevice;
                setMainState(State_Idle);
            }
            ITF_ASSERT(!m_lastEnumerationFailed); //should not happen because we called checkDeviceIsOk


            //Search for content among enumerated
            u32 foundIndex = (u32)findBasenameAmongEnumerated(m_loadSaveBaseName);
            //
            if (foundIndex==(u32)-1)
            {
                switchToLoadErrorState(Error_SavegameNotFound);
                return;
            }
            m_selectedDeviceIndex = foundIndex;
            if(m_filteredDeviceContent[m_selectedDeviceIndex].isCorrupted)
            {
                switchToLoadErrorState(Error_LoadSave_IOError);
                return;
            }

            ZEROVARIABLE(m_overlappedLoading);

            //open archive
            DWORD errorCode = XContentCreate( m_loading_PlayerIndex, 
                SAVEGAME_DRIVE_LETTER, 
                &m_filteredDeviceContent[m_selectedDeviceIndex].data,
                XCONTENTFLAG_OPENEXISTING, 
                NULL, NULL, 
                &m_overlappedLoading );
            if( errorCode != ERROR_IO_PENDING )
            {
                switchToLoadErrorState(Error_LoadSave_IOError);
                return;
            }

            //
            setMainState(Load_OpeningArchive);
        }


        void Adapter_Savegame_x360::handle_Load_OpeningArchive()
        {
            if (!XHasOverlappedIoCompleted(&m_overlappedLoading))
                return;

            //
            DWORD errorCode = XGetOverlappedResult( &m_overlappedLoading, NULL, FALSE );
            if (errorCode == ERROR_SUCCESS)
            {
                m_lastLoadSaveError = Error_Ok;

                setMainState(Load_LoadingFromThread);
                
                m_threadIsRunning = btrue;
                ITF_MemoryBarrier();
                m_thread = THREADMANAGER->createThread(loadThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
                if(m_thread!=NULL)
                {
                    m_threadIsRunning  = bfalse;
                }
            }
            else
                switchToLoadErrorState(Error_LoadSave_IOError);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        u32 WINAPI Adapter_Savegame_x360::loadThreadRoutine(void *_param)
        {
            Adapter_Savegame_x360 *caller = (Adapter_Savegame_x360*)_param;
            //
            HANDLE file = INVALID_HANDLE_VALUE;
            ErrorCode returnedStatus=Error_Ok;

            //Open savegame
            char filename[256];
            sprintf(filename, "%s:\\%s.sav", SAVEGAME_DRIVE_LETTER, caller->m_loadSaveBaseName.cStr());

            file = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            if (file==INVALID_HANDLE_VALUE)
            {
                returnedStatus = Error_IncorrectSavegameFormat;
            }
            else
            {
                DWORD totalFileSize = GetFileSize(file, NULL);
                if (totalFileSize>caller->m_chooseDevice_MinFreeSize)
                {
                    returnedStatus = Error_IncorrectSavegameFormat;
                }
                else
                {                
                    u32 fileSize = (u32)(caller->m_chooseDevice_MinFreeSize/ (f32)MAX_SLOTS);
                    SetFilePointer(file, fileSize*caller->m_currentSlotIndex, NULL, FILE_BEGIN);

                    DWORD totalReadSize=0;
                    BOOL couldRead = FALSE;
                    Header tempHeader;
                    couldRead = (ReadFile(file, &tempHeader, getHeaderSize(), &totalReadSize, NULL));
                    if(!couldRead || totalReadSize != getHeaderSize())
                    {
                        returnedStatus = Error_LoadSave_IOError;
                    }
                    else
                    {
                        // Check Header 
                        if(caller->m_codeCRC && tempHeader.CodeCRC != caller->m_codeCRC)
                        {
                            returnedStatus = Error_LoadSave_IOError;
                        }
                        else
                        {
                            fileSize -= getHeaderSize();
                            caller->m_currentSavegame.resize(tempHeader.fileSize);

                            totalReadSize=0;
                            couldRead = ReadFile(file, &caller->m_currentSavegame[0], tempHeader.fileSize, &totalReadSize, NULL);
                            if (!couldRead || (totalReadSize!=tempHeader.fileSize))
                                returnedStatus = Error_LoadSave_IOError;
                            else
                                returnedStatus = Error_Ok;
                        }
                    }
                }
            }

            if (file!=INVALID_HANDLE_VALUE)
                SF_CLOSEHANDLE(file);

            if (returnedStatus!=Error_Ok)
            {
                caller->switchToLoadErrorState(returnedStatus);
            }

            ITF_MemoryBarrier();
            caller->m_threadIsRunning = bfalse;            
            return 0;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Load_LoadingFromThread()
        {
            //if LOAD thread ended, 
            if(m_threadIsRunning)
                return ;

            //close all
            DWORD err = XContentClose(SAVEGAME_DRIVE_LETTER, NULL);
            m_thread = NULL;
            
            //update states
            m_isLoading = bfalse;
            if (err!=ERROR_SUCCESS)
            {
                m_lastLoadSaveError = Error_LoadSave_IOError;
                m_mustSwitchToLoadErrorState = btrue;
            }

            if (m_mustSwitchToLoadErrorState)
            {
                m_mustSwitchToLoadErrorState = bfalse;
                if(isCurrentDeviceDisconnected())                        
                {
                    setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle);
                    m_lastLoadSaveError = Error_LoadSave_NoDevice;
                }
                else
                {
                    setMainState(State_Idle);
                m_lastLoadSaveError = Error_LoadSave_IOError;
                }
            }
            else
                setMainState(State_Idle);
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        bbool Adapter_Savegame_x360::isCurrentSlotEmpty() const
        {
            return ( m_slotList.size() <= m_currentSlotIndex || m_slotList[m_currentSlotIndex].fileSize == 0 );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        Adapter_Savegame::ErrorCode Adapter_Savegame_x360::startSave(u32 _playerIndex, u32 _slotIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool _reinforceOverwriteChecks)
        {
            if(hasPendingOperation())
                return Error_AlreadySaving; 

            m_reinforceOverwriteChecksForNewGame = _reinforceOverwriteChecks;
            ITF_ASSERT_MSG(_dataSize<=m_chooseDevice_MinFreeSize, "Savegame size is greater than maximum size from device selector");

            if (!canUserUseStorage(_playerIndex, bfalse))
                return Error_LoadOrSaveForbiddenForPlayer;

            //Save for subsequent states
            m_saving_PlayerIndex = _playerIndex;
            m_loadSaveBaseName  = SAVEGAME_BASENAME;
            m_loadSaveDisplayName = displayName;
            m_currentSlotIndex = _slotIndex;
            m_deviceChangedDuringSave = bfalse;

            //Make name of the slot
            LocalisationId id;
            id = LOCALISATION_ID_SAVE_NAME; // Save Game [slotNumber]
            m_saveGameName = LOCALISATIONMANAGER->getText(id);
            m_saveGameName.replace("[slotNumber]", "");


            if(_dataSize)
            {
                m_currentSavegame.resize(_dataSize);
                XMemCpyStreaming_Cached(&m_currentSavegame[0], _data, _dataSize);
            } else
            {
                m_currentSavegame.clear();
            }
            m_lastLoadSaveError = Error_Ok;

            // Check if the device is still valid
            // If not, ask to the player if he wants to select another storage, 
            // but stay IDLE after that. Do not try to continue the saving action.
            checkIfDeviceChanged();
            if (m_startCheckDeviceOkASAP)
            {
                if ( isCurrentDeviceDisconnected() )
                {
                    setMainState(isCurrentSlotEmpty()? Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle: Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);
                    m_startCheckDeviceOkASAP = bfalse;
                    m_deviceChangedDuringSave = btrue;
                    return Error_LoadSave_NoDevice;
                }
            }

            if(_dataSize==0)
            {
                // Check if one save is not empty
                bbool allSaveAreEmpty = btrue;
                for(u32 i= 0; i < m_slotList.size(); i++)
                {
                    if(m_slotList[i].fileSize > 0 && m_slotList[i].slot != _slotIndex)
                    {
                        allSaveAreEmpty = bfalse;
                    }
                }
                // If all the headers are empty, delete the file
                if(allSaveAreEmpty)
                {
                    setMainState(Save_StartDeletingContent);
                    //checkDeviceIsOk(Save_StartDeletingContent, Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);
                    return Error_Ok;
                }            
            }

            // new game ?
            // Do not check for overwrite 
            if ( isCurrentSlotEmpty() )
            {
                setMainState(Save_StartCreatingContent_PreCheckNewGameOverwrite);
                return Error_Ok;
            }
            
            checkDeviceIsOk(Save_StartCreatingContent_PreCheckNewGameOverwrite, Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);        
            return Error_Ok;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Save_DeviceWasChanged_StillValid()
        {
            ITF_ASSERT(m_wasEnumerated && !m_lastEnumerationFailed);
            if (findBasenameAmongEnumerated(m_loadSaveBaseName)>=0)
            {
                setMainState( Save_AskOverWrite);
                return;
            }
            setMainState(Save_StartCreatingContent_PreCheckNewGameOverwrite);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Generic call for changing device in a load error
        void Adapter_Savegame_x360::loadProblem_ChangeDevice()
        {        
            if (startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, Load_Start, Load_SignalCantLoad_WrongDevice, Load_SignalCantLoad_WrongDevice, btrue, btrue)!=Error_Ok)
                setMainState(Load_SignalCantLoad_WrongDevice);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerLoadError(ChoiceWhenLoadError _choice)
        {
            if (m_mainState==Load_IOError_AskWhatToDo)
            {
                switch(_choice)
                {
                case LoadError_DeleteFile:
                    setMainState(Load_IOError_DeletingConfirmation);
                    break;
                case LoadError_CancelLoad: 
                    m_operationCancelled=btrue; 
                    setMainState(State_Idle);                 
                    disableSaveSystem();
                    break;
                default : ITF_ASSERT(bfalse); break;
                }
            }
            else
                ITF_ASSERT(bfalse);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice)
        {
            if (m_mainState==Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother)
            {
                switch(_choice)
                {
                case DeviceNoLongerValidForLoad_ChangeDevice: loadProblem_ChangeDevice(); break;
                case DeviceNoLongerValidForLoad_CancelLoad: setMainState(State_Idle); m_operationCancelled=btrue; break;
                default : ITF_ASSERT(bfalse); break;
                }
            }
            else
                ITF_ASSERT(bfalse);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice)
        {
            if (m_mainState==Load_CantFind_AskWhatToDo)
            {
                switch(_choice)
                {
                case LoadCantFind_ChangeDevice:loadProblem_ChangeDevice(); break;
                case LoadCantFind_CancelLoad:setMainState(State_Idle); m_operationCancelled=btrue; break;
                default : ITF_ASSERT(bfalse); break;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice)
        {
            if (m_mainState==Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother)
            {
                switch (_choice)
                {
                case DeviceNoLongerValidForSave_ChangeDevice:
                    if (startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, Save_DeviceWasChanged_StillValid, Save_SignalCantSave_WrongDevice, Save_SignalCantSave_WrongDevice, btrue, bfalse)!=Error_Ok)
                        setMainState(Save_SignalCantSave_WrongDevice);
                    break;
                case DeviceNoLongerValidForSave_ForceGiveUp:
                    setMainState(State_Idle);
                    break;
                case DeviceNoLongerValidForSave_NoLongerSave:
                    m_operationCancelled = btrue;
                    setDeviceChosenFlags(btrue, btrue);
                    setMainState(State_Idle);
                    disableSaveSystem();
                    m_storageDevice = XCONTENTDEVICE_ANY;
                    break;
                default:
                    ITF_ASSERT(bfalse);
                    break;
                }
            }
            else 
            {
                switch (_choice)
                {
                case DeviceNoLongerValidForSave_ChangeDevice:
                    if (startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, State_Idle, State_Idle, State_Idle, btrue, bfalse)!=Error_Ok)
                        setMainState(Save_SignalCantSave_WrongDevice);
                    break;
                case DeviceNoLongerValidForSave_NoLongerSave:                
                    setDeviceChosenFlags(btrue, btrue);                
                    disableSaveSystem();
                    m_storageDevice = XCONTENTDEVICE_ANY;
                    setMainState(State_Idle);
                    break;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice)
        {
            if (m_mainState==ChooseDevice_ChosenNone_RetryOrContinue)
            {
                switch(_choice)
                {
                case DeviceChosenAsNone_ChangeDevice:
                    setDeviceChosenFlags(bfalse,bfalse);
                    startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, 
                        m_chooseDevice_newDeferredStateWhenDeviceSelected, 
                        m_chooseDevice_newDeferredStateWhenDeviceHasError,
                        m_chooseDevice_newDeferredStateWhenConfirmedNoDevice,
                        btrue,
                        bfalse);
                    break;
                case DeviceChosenAsNone_NoLongerSave:
                    m_operationCancelled = btrue;
                    setDeviceChosenFlags(btrue, btrue);
                    setMainState(m_chooseDevice_newDeferredStateWhenConfirmedNoDevice);
                    break;
                default : ITF_ASSERT(bfalse); break;
                }
            }
            else
            {
                ITF_ASSERT_MSG(bfalse, "signalAnswerChoseNoDevice");
            }

        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalForDeleteCorruptedFile(ChoiceWhenDeleteingCorruptedFile _choice)
        {
            if( m_mainState==Load_IOError_DeletingConfirmation)
            {
                switch(_choice)
                {
                case CorruptedFile_Delete:
                    setMainState(Save_StartDeletingContent);
                    break;
                case CorruptedFile_Cancel:
                    /*setMainState(State_Idle);
                    disableSaveSystem();*/
                    setMainState(Load_IOError_AskWhatToDo);
                    break;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalOkForOverwrite(ChoiceWhenFileExists _choice)
        {
            if (m_mainState==Save_AskOverWrite)
            {
                switch(_choice)
                {
                case FileExists_Overwrite:
                    //Overwrite : simply continue with writing
                    setMainState(Save_StartCreatingContent);
                    break;
                case FileExists_ForceGiveup:
                    setMainState(State_Idle);
                    break;
                case FileExists_NoLongerSave:
                    setDeviceChosenFlags(btrue, btrue);
                    m_operationCancelled = btrue;
                    setMainState(State_Idle);
                    break;
                case FileExists_ChangeDevice:
                    //the user wants to change the device
                    //if he can't, signal an error in savegame (error will be handled by error prompt)
                    //if he can, signal Still Valid
                    //if (startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, Save_DeviceWasChanged_StillValid, Save_SignalCantSave_WrongDevice, Save_SignalCantSave_WrongDevice,btrue, bfalse)!=Error_Ok)
                    //setMainState(Save_SignalCantSave_WrongDevice);
                    if(m_currentSavegame.size()==0)
                    {
                        // cancel the delete action
                        setMainState(State_Idle);
                    } else
                    {
                    setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);
                    }
                    break;
                default: ITF_ASSERT(bfalse); break;

                }
            }
            else
            {
                ITF_ASSERT_MSG(bfalse, "signalOkForOverwrite was called in wrong state");
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice)
        {
            if (m_mainState==Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer)
            {
                switch(_choice)
                {
                case FileExistsForNewGame_Overwrite:
                    //Overwrite : simply continue with writing
                    setMainState(Save_StartCreatingContent);
                    break;
                case FileExistsForNewGame_ForceGiveup:
                    setMainState(State_Idle);
                    break;
                case FileExistsForNewGame_CancelSave:
                    m_operationCancelled=btrue;
                    m_lastLoadSaveError = Error_CancelledSaving;
                    setMainState(State_Idle);
                    break;
                default: ITF_ASSERT(bfalse); break;
                }
            }
            else
            {
                ITF_ASSERT_MSG(bfalse, "signalOkForOverwrite was called in wrong state");
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Save_DeletingContent()
        {
            if (XHasOverlappedIoCompleted(& m_overlappedSaving))
            {          
                DWORD res = XGetOverlappedResult( &m_overlappedSaving, NULL, TRUE ) ;
                if( res == ERROR_SUCCESS )
                {

                    i32 ContentIndex = findBasenameAmongEnumerated(m_loadSaveBaseName);
                    if(ContentIndex==-1)
                    {
                        switchToSaveErrorState(Error_LoadSave_IOError);
                        return;
                    }

                    m_filteredDeviceContent.eraseNoOrder( ContentIndex );
                    m_slotList.clear();
                    setMainState(State_Idle);
                }
                else
                    switchToSaveErrorState(Error_LoadSave_IOError);

                m_isSaving = bfalse;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Save_StartDeletingContent()
        {
            if (m_lastEnumerationFailed)
            {
                switchToSaveErrorState(Error_LoadSave_IOError);
                return;
            }

            if (!m_storageDeviceIsChosen || m_storageDeviceIsChosenAsNone)
            {
                m_lastLoadSaveError = Error_LoadSave_NoDevice;
                setMainState(State_Idle);
                return;
            }
            m_lastLoadSaveError = Error_Ok;
            ZEROVARIABLE(m_overlappedSaving);


            //Save for subsequent states
            m_saving_PlayerIndex = m_mainPlayerIndex;
            m_loadSaveBaseName  = SAVEGAME_BASENAME;

            //Make name of the slot
            LocalisationId id;
            id = LOCALISATION_ID_SAVE_NAME; // Save Game [slotNumber]
            m_saveGameName = LOCALISATIONMANAGER->getText(id);
            m_saveGameName.replace("[slotNumber]", "");

            m_lastLoadSaveError = Error_Ok;

            checkIfDeviceChanged();
            if (m_startCheckDeviceOkASAP && isCurrentDeviceDisconnected() )
            {
                setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);
                m_startCheckDeviceOkASAP = bfalse;
                return ;
            }

            i32 ContentIndex = findBasenameAmongEnumerated(m_loadSaveBaseName);
            if(ContentIndex==-1)
            {
                switchToSaveErrorState(Error_LoadSave_IOError);
                return;
            }

            // Mount the device associated with the display name for writing
            DWORD errorCode = XContentDelete( m_saving_PlayerIndex, &m_filteredDeviceContent[ContentIndex].data, &m_overlappedSaving );
            if( errorCode != ERROR_IO_PENDING )
            {
                switchToSaveErrorState(Error_LoadSave_IOError);
                return;
            }

            // Continue next state
            setMainState(Save_DeletingContent);
            m_wasEnumerated = bfalse;
            m_lastLoadSaveError = Error_Ok;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_StartCreatingContent()
        {
            if (m_lastEnumerationFailed)
            {
                switchToSaveErrorState(Error_LoadSave_IOError);
                return;
            }

            if (!m_storageDeviceIsChosen || m_storageDeviceIsChosenAsNone)
            {
                m_lastLoadSaveError = Error_LoadSave_NoDevice;
                setMainState(State_Idle);
                return;
            }

            checkIfDeviceChanged();
            if (m_startCheckDeviceOkASAP && isCurrentDeviceDisconnected() )
            {
                m_startCheckDeviceOkASAP = bfalse;
                m_deviceChangedDuringSave = btrue;
                // If the slot is Empty, Do just stay IDLE after the popup is displayed
                // Do not try to save again
                if(m_slotList.size() <= m_currentSlotIndex 
                    || m_slotList[m_currentSlotIndex].fileSize == 0)
                {
                    setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle);
                    return ;                    
                }

                setMainState(Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother);
                return ;
            }

            m_lastLoadSaveError = Error_Ok;
            ZEROVARIABLE(m_overlappedSaving);

            XCONTENT_DATA contentData = {0};
            strcpy_s( contentData.szFileName, m_loadSaveBaseName.cStr() );
            wcscpy_s( contentData.szDisplayName, (wchar_t*)m_saveGameName.cStr() );
            contentData.dwContentType = XCONTENTTYPE_SAVEDGAME;
            contentData.DeviceID = m_storageDevice;

            // Mount the device associated with the display name for writing
            DWORD errorCode = XContentCreate( m_saving_PlayerIndex, SAVEGAME_DRIVE_LETTER, &contentData, XCONTENTFLAG_OPENALWAYS, NULL, NULL, &m_overlappedSaving );
            if( errorCode != ERROR_IO_PENDING )
            {
                switchToSaveErrorState(Error_LoadSave_IOError);
                return;
            }

            // Continue next state
            if( m_currentSavegame.size() > 0 ) 
                m_isSaving = btrue;

            setMainState(Save_CreatingContent);
            m_wasEnumerated = bfalse;
            m_lastLoadSaveError = Error_Ok;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_Save_CreateContent()
        {
            if (XHasOverlappedIoCompleted(& m_overlappedSaving))
            {            
                if( XGetOverlappedResult( &m_overlappedSaving, NULL, TRUE ) == ERROR_SUCCESS )
                {
                    setMainState(Save_WritingFile);
                    //WARNING : STATE CHANGES ARE FORBIDDEN AFTER CREATETHREAD!

                    m_threadIsRunning = btrue;
                    ITF_MemoryBarrier();
                    m_thread = THREADMANAGER->createThread(saveThreadRoutine, this, ThreadSettings::m_settings[eThreadId_SaveManager]);
                    if ( m_thread == NULL)
                    {
                        m_threadIsRunning = bfalse;
                    }
               
                }
                else
                {
                    m_isSaving = bfalse;
                    switchToSaveErrorState(Error_LoadSave_IOError);
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerWhenUserNotSignedIn(ChoiceWhenUserNotSignedIn _choice) 
        {
            if (m_mainState==PreLoadFailed)
            {
                if(NETWORKSERVICES->isUserSignedIn(m_mainPlayerIndex))
                {
                    // User is signed by himself, just continue the process
                    setMainState(PreLoad_Running);
                    return;
                }

                switch(_choice)
                {
                case NotSignedIn_Retry:
                    NETWORKSERVICES->askForSignIn(m_MaxPlayersForSaving);
                    setMainState(PreLoad_Running);
                    break;
                case NotSignedIn_Continue:
                    disableSaveSystem();
                    setMainState(State_Idle);    
                    break;
                }
            }
            else
            {
                ITF_ASSERT_MSG(bfalse,"signalAnswerWhenUserNotSignedIn was called in unexpected state");
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::signalAnswerSaveError(ChoiceWhenSaveError _choice)
        {
            if (m_mainState==Save_IOError_AskWhatToDo)
            {
                switch(_choice)
                {
                case SaveError_ChangeDevice:
                    //the user wants to change the device
                    //if he can't, signal an error in savegame (error will be handled by error prompt)
                    //if he can, signal Save_DeviceWasChanged_StillValid (because device was just changed and valid, and we are resuming save problem)                
                    if (startChooseDevice_Internal(m_choseDevice_PlayerIndex, m_chooseDevice_MinFreeSize, Save_DeviceWasChanged_StillValid, Save_SignalCantSave_WrongDevice, Save_SignalCantSave_WrongDevice, btrue, bfalse)!=Error_Ok)
                        setMainState(Save_SignalCantSave_WrongDevice);                
                    break;
                case SaveError_NoLongerSave:
                    m_operationCancelled = btrue;
                    setDeviceChosenFlags(btrue, btrue);
                    setMainState(State_Idle);
                    break;
                }
            }
            else
            {
                ITF_ASSERT_MSG(bfalse,"signalAnswerSaveError was called in unexpected state");
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        u32 WINAPI Adapter_Savegame_x360::saveThreadRoutine(void *_param)
        {
            Adapter_Savegame_x360 *caller = (Adapter_Savegame_x360*)_param;

            char filename[256];
            sprintf(filename, "%s:\\%s.sav", SAVEGAME_DRIVE_LETTER, caller->m_loadSaveBaseName.cStr());

            HANDLE loadSaveFileHandle = CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if (loadSaveFileHandle==INVALID_HANDLE_VALUE)
            {
                caller->switchToSaveErrorState(Error_LoadSave_IOError);
                goto exit;
            }
            else
            {
                DWORD bytesWritten=0, headerBytesWritten=0;
                const u32 headerSize = sizeof(Header);
                // Write header
                Header tmpHeader;

                // Fill basic informations
                tmpHeader.CodeCRC = caller->m_codeCRC;
                tmpHeader.DataCRC = caller->m_dataCRC;
                tmpHeader.fileSize = caller->m_currentSavegame.size();
                tmpHeader.slot = caller->m_currentSlotIndex;

                // Get the size of the string to copy
                u32 displayNameSize = caller->m_loadSaveDisplayName.getLen()*sizeof(u16);
                if(displayNameSize> sizeof(tmpHeader.displayedName))
                    displayNameSize = sizeof(tmpHeader.displayedName);

                ITF_Memcpy(tmpHeader.displayedName, caller->m_loadSaveDisplayName.cStr(), displayNameSize);
                tmpHeader.displayedName[caller->m_loadSaveDisplayName.getLen()]=0;

                // Get the total size of the file to check integrity
                u32 totalFileSize = GetFileSize(loadSaveFileHandle, NULL);

                // Just created ? We have to reserve the space for the MAX_SLOT
                if(totalFileSize==0)
                {
                    SetFilePointer( loadSaveFileHandle, caller->m_chooseDevice_MinFreeSize, NULL, FILE_BEGIN );
                    SetEndOfFile( loadSaveFileHandle );

                    // fill the slot list with empty header
                    caller->m_slotList.clear();

                    Header emptyHeader;
                    emptyHeader.CodeCRC = 0;
                    emptyHeader.DataCRC = 0;
                    emptyHeader.fileSize = 0;
                    emptyHeader.displayedName[0] = 0;

                    for (u32 i = 0; i < MAX_SLOTS; i++)
                    {
                        emptyHeader.slot = i;
                        caller->m_slotList.push_back(emptyHeader);
                    }
                }

                // Move in the file to slot size
                u32 fileSize = (u32)(caller->m_chooseDevice_MinFreeSize/ (f32)MAX_SLOTS);
                SetFilePointer(loadSaveFileHandle, fileSize*caller->m_currentSlotIndex, NULL, FILE_BEGIN);

                bbool headerWrite = WriteFile(loadSaveFileHandle, &tmpHeader, headerSize, &headerBytesWritten, NULL);
                bbool fileWrite = headerWrite && ((tmpHeader.fileSize == 0 || WriteFile(loadSaveFileHandle, &caller->m_currentSavegame[0], caller->m_currentSavegame.size(), &bytesWritten, NULL)));
                if (headerWrite && fileWrite)
                {
                    if ( caller->m_currentSavegame.size()!=bytesWritten || headerSize != headerBytesWritten )
                    {                    
                        CloseHandle(loadSaveFileHandle);
                        caller->switchToSaveErrorState(Error_LoadSave_IOError);
                        goto exit;
                    }
                    else
                    {
                        // Update header information to avoid future reading (only if the slot are initialized by the HEARDER thread
                        if(caller->m_slotList.size()>caller->m_currentSlotIndex)
                            ITF_Memcpy(&caller->m_slotList[caller->m_currentSlotIndex], &tmpHeader, headerSize);

                        caller->m_lastLoadSaveError = Error_Ok;
                        CloseHandle(loadSaveFileHandle);
                        goto exit;
                    }
                }            
                else
                {
                    // Write operation fails
                    CloseHandle(loadSaveFileHandle);
                    caller->switchToSaveErrorState(Error_LoadSave_IOError);                
                    goto exit;
                }
            }

exit:
            ITF_MemoryBarrier();
            caller->m_threadIsRunning = bfalse;            
            return 0;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_StartCreatingContent_PreCheckNewGameOverwrite()
        {        
            if (m_reinforceOverwriteChecksForNewGame && findBasenameAmongEnumerated(m_loadSaveBaseName)>=0)
                setMainState(Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer);
            else
                setMainState(Save_StartCreatingContent);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::handle_WritingSaveFile()
        {
            if(m_threadIsRunning)
                return;

            m_thread = NULL;
            XContentClose( SAVEGAME_DRIVE_LETTER, NULL );
            
            m_isSaving = bfalse;
            if (m_mustSwitchToSaveErrorState)
            {
                m_mustSwitchToSaveErrorState = bfalse;
                setMainState(Save_IOError_AskWhatToDo);
            }
            else
            {
                setMainState(State_Idle);

                // Reset corrupted flag when a new save is completed
				// The only case we found it's when the data is corrupted by removing the storage during a saving
				// It's not a real corruption, so it's safe to remove the flag (because we can't save with a real one)
				// You only can save when you want to overwrite the data in game
				// A better fix should be to remove the file and recreate it, but I need time to do it...
                i32 foundIndex = findBasenameAmongEnumerated(m_loadSaveBaseName);
                if ( foundIndex >= 0 && m_filteredDeviceContent.size() > foundIndex )
                {
                    m_filteredDeviceContent[foundIndex].isCorrupted = bfalse;
                    m_mustSwitchToLoadErrorState = bfalse;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::checkDeviceIsOk(MainState _stateAfterOk, MainState _stateAfterFailure)
        {
            ITF_ASSERT(m_storageDeviceIsChosen);
            m_startCheckDeviceOkASAP = bfalse;
            if (!startContentEnumeration(m_choseDevice_PlayerIndex, _stateAfterOk, _stateAfterFailure))
                setMainState(_stateAfterFailure);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::switchToSaveErrorState(ErrorCode _code)
        {        
            m_lastLoadSaveError = _code;
            if (m_mainState==Save_WritingFile)
                m_mustSwitchToSaveErrorState = btrue; //if we are in save thread, delay actual change in main thread
            else
                setMainState(Save_IOError_AskWhatToDo);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::switchToLoadErrorState(ErrorCode _code)
        {
            m_lastLoadSaveError = _code;
            if (m_mainState==Load_LoadingFromThread || m_mainState==ReadingHeader)
                m_mustSwitchToLoadErrorState = btrue;
            else
            {
                if (_code==Error_SavegameNotFound)
                    setMainState(Load_CantFind_AskWhatToDo);
                else
                    setMainState(Load_IOError_AskWhatToDo);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::GetSelectedDeviceName(String & deviceName)
        {
            if(m_storageDeviceIsChosen && !m_storageDeviceIsChosenAsNone)
            {
                WCHAR wDeviceName[128];
                if(XContentGetDeviceName(m_storageDevice, wDeviceName, 128) == ERROR_SUCCESS)
                {
                    deviceName = (u16*)wDeviceName;
                }
            }        
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Adapter_Savegame_x360::update()
        {
            checkIfDeviceChanged();

            //If there's a posted Choose Device request, try to display the device selector
            switch(m_mainState)
            {
            case PreLoad_Running:
                handle_preLoadRunning();
                break;
            case ChooseDevice_PostedShowSelector:
                handle_MustChooseDevice();
                break;
            case ChooseDevice_ShowingSelector:
                if (XHasOverlappedIoCompleted(&m_overlappedDevSelector))
                {
                    handle_JustClosedDeviceSelectorGUI();
                }
                break;
            case EnumeratingContent:
                handle_AsynchronousContentEnumeration();
                break;

            case Save_StartCreatingContent_PreCheckNewGameOverwrite:
                handle_StartCreatingContent_PreCheckNewGameOverwrite();
                break;

            case Save_StartCreatingContent_PreCheckNewGameOverwrite_WaitAnswer:
                //We exit this state using signalAnswerFileExistsForNewGame
                break;

            case Save_StartCreatingContent:
                handle_StartCreatingContent();
                break;

            case Save_CreatingContent:
                handle_Save_CreateContent();
                break;

            case Save_AskOverWrite:
                //We exit this state using signalOkForOverwrite();
                break;

            case Save_DeviceWasChanged_StillValid:
                handle_Save_DeviceWasChanged_StillValid();
                return;

            case Load_DeviceWasChanged_NoLongerValid_AskIfChooseAnother:
                //We exit this state using signalAnswerDeviceNoLongerValidForLoad
                break;

            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnotherAndStayIdle:
            case Save_DeviceWasChanged_NoLongerValid_AskIfChooseAnother:
                //We exit this state using signalAnswerDeviceNoLongerValidForSave
                break;

            case Save_IOError_AskWhatToDo:
                //we exit this state with signalAnswerSaveError
                break;

            case ChooseDevice_ChosenNone_RetryOrContinue:
                //we exit this state with signalAnswerChoseNoDevice
                break;

            case Load_Start:
                handle_Load_Start();
                break;

            case Load_OpeningArchive:
                handle_Load_OpeningArchive();
                break;

            case Load_LoadingFromThread:
                handle_Load_LoadingFromThread();
                break;

            case Load_IOError_AskWhatToDo:
                //we exit this state with signalAnswerLoadError
                //setMainState(State_Idle);// waithing for messages ;. 
                break;

            case Load_CantFind_AskWhatToDo:
                //we exit this state with signalAnswerLoadCantFind
                break;

            case Save_WritingFile:
                handle_WritingSaveFile();
                break;

            case Load_SignalCantLoad_WrongDevice:
                if (m_storageDeviceIsChosenAsNone)
                    setMainState(State_Idle);
                else
                    switchToLoadErrorState(Error_CouldNotEnumerate);
                break;

            case Save_SignalCantSave_WrongDevice:
                if (m_storageDeviceIsChosenAsNone)
                    setMainState(State_Idle);
                else
                    switchToSaveErrorState(Error_CouldNotEnumerate);
                break;

            case PreLoadFailed:
                // Wait Signal
                break;
            case State_Idle:
                m_isSaving = bfalse;
                m_isLoading = bfalse;
                break;

            case Save_StartDeletingContent:
                handle_Save_StartDeletingContent();
                break;

            case Save_DeletingContent:
                handle_Save_DeletingContent();
                break;

            case ReadingHeader:
                handle_ReadHeaders();
                break;
            }

            // Check operation cancel status flag
            if(m_mainState != State_Idle && m_operationCancelled)
            {
                m_operationCancelled = bfalse;            
            }
        }

        bbool Adapter_Savegame_x360::SaveProfileSettings( u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize )
        {
            if(_dataSize>XPROFILE_SETTING_MAX_SIZE)
            {
                ITF_ASSERT(0);
                return bfalse;
            }

            // 1: Get Setting ID
            u32 settingID=0; 
            switch(_settings)
            {
            case ProfileSettings_Slot1: settingID = XPROFILE_TITLE_SPECIFIC1; break;
            case ProfileSettings_Slot2: settingID = XPROFILE_TITLE_SPECIFIC2; break;
            case ProfileSettings_Slot3: settingID = XPROFILE_TITLE_SPECIFIC3; break;
            }

            // 2: Set profile setting structure
            XUSER_PROFILE_SETTING settingsToWrite;
            settingsToWrite.user.dwUserIndex = _playerIndex;
            settingsToWrite.dwSettingId = settingID;

            // 3: Set data
            settingsToWrite.data.type = XUSER_DATA_TYPE_BINARY ;
            settingsToWrite.data.binary.pbData = _pData;        
            settingsToWrite.data.binary.cbData = _dataSize;

            // 4: Write in user profile
            DWORD res = XUserWriteProfileSettings(_playerIndex,1,&settingsToWrite,NULL);
            return (res==ERROR_SUCCESS);
        }

        bbool Adapter_Savegame_x360::LoadProfileSettings( u32 _playerIndex, ProfileSettings _settings, u8* _pData, u32 _dataSize )
        {
            DWORD settingID=0; 
            switch(_settings)
            {
            case ProfileSettings_Slot1: settingID = XPROFILE_TITLE_SPECIFIC1; break;
            case ProfileSettings_Slot2: settingID = XPROFILE_TITLE_SPECIFIC2; break;
            case ProfileSettings_Slot3: settingID = XPROFILE_TITLE_SPECIFIC3; break;
            }

            DWORD sizeToWrite =0;
            u32 res = XUserReadProfileSettings(0, _playerIndex,1,&settingID, &sizeToWrite, NULL, NULL);
            if(res!=ERROR_INSUFFICIENT_BUFFER)
            {
                return bfalse;
            }

            XUSER_READ_PROFILE_SETTING_RESULT* pResults = (XUSER_READ_PROFILE_SETTING_RESULT*)Memory::malloc(sizeToWrite);

            res = XUserReadProfileSettings(0, _playerIndex,1,&settingID, &sizeToWrite, pResults , NULL);
            if(res == ERROR_SUCCESS && pResults->pSettings->data.binary.cbData > 0)
            {
                ITF_Memcpy(_pData, pResults->pSettings->data.binary.pbData, _dataSize);
                return btrue;
            }
            return bfalse;
        }

        bbool Adapter_Savegame_x360::checkDataCRCForCurrentSave( u32 _dataCRC )
        {
            if(m_currentSlotIndex<m_slotList.size())
            {
                return (_dataCRC == m_slotList[m_currentSlotIndex].DataCRC);
            }
            return bfalse;
        }

        void Adapter_Savegame_x360::prepareInit( u32 _minFreeSize )
        {
            Adapter_Savegame::prepareInit(_minFreeSize*MAX_SLOTS);
        }

        void Adapter_Savegame_x360::cancelAllOperations()
        {
            bbool changeStateForIdle = bfalse;
            switch(m_mainState)
            {
            case EnumeratingContent:
                cancelCurrentContentEnumeration();
                changeStateForIdle = btrue;
                break;
            case Save_CreatingContent:
                XCancelOverlapped(&m_overlappedSaving);
                changeStateForIdle = btrue;
                break;
            case Load_OpeningArchive:
                XCancelOverlapped(&m_overlappedLoading);
                changeStateForIdle = btrue;
                break;

            case Load_LoadingFromThread:
            case Save_WritingFile:
            case Save_DeletingContent:
            case ReadingHeader:
                break;

            default:
                // check all thread here ... 
                changeStateForIdle = btrue;
            }

            if(changeStateForIdle)
                m_mainState = State_Idle;
        }

}
