#include "precompiled_trcadapter_X360.h"

#ifndef _ITF_TRCADAPTER_X360_H_
#include "adapters/TRCAdapter_x360/TRCAdapter_x360.h"
#endif //_ITF_TRCADAPTER_X360_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif //_ITF_ADAPTER_SAVEGAME_H__

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

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_


namespace ITF
{

    String buildText(u32 lineID, EContextIconType iconType = ContextIconType_Invalid)
    {
        static String workingStr;
        LocalisationId id;

        id.value = lineID;
        String buff = LOCALISATIONMANAGER->getText(id);
        workingStr  = buff ;

        if (iconType != ContextIconType_Invalid)
        {
            workingStr = workingStr + " " + CONTEXTICONSMANAGER->getIconStr(InputAdapter::Pad_X360, iconType);
        }

        return workingStr;
    }

    bbool TRCManagerAdapter_x360::_buildAndAddMessage(ErrorContext	errorContext)
    {
        String     message;
        TRCMessage_Base* pTRCMessage = NULL;

        switch(errorContext)
        {    
        case Pad_DisconnectingDuringGameplay:
            ITF_ASSERT(_u32_customParam != U32_INVALID);
            message = buildText(4023);                        
            pTRCMessage = new TRCMessage_DeconnectionDuringGamePlay_X360(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, _u32_customParam));                

            pTRCMessage->SetForceOverPauseMenu();
            if(_u32_customParam != GAMEMANAGER->getMainIndexPlayer())
            {
                ((TRCMessage_DeconnectionDuringGamePlay_X360*)pTRCMessage)->setButton(buildText(4164, ContextIconType_Select), input_actionID_Back);      
            }
            pTRCMessage->setActivePlayer(_u32_customParam);
            break;
        case Sav_NoAvailableStorage:
            pTRCMessage = newAlloc(mId_System, TRCMessage_NoAvailableDevice_X360(errorContext));
            message = buildText(4011);            
            ((TRCMessage_NoAvailableDevice_X360*)pTRCMessage)->setRightButton(buildText(4057, ContextIconType_Invalid), input_actionID_Back); // 4057
            ((TRCMessage_NoAvailableDevice_X360*)pTRCMessage)->setLeftButton(buildText(4058, ContextIconType_Invalid), input_actionID_Valid);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());
            pTRCMessage->SetForceOverPauseMenu();
            break;
        case Sav_NewGameAskForOverwrite:
            // REMOVED
            pTRCMessage = newAlloc(mId_System, TRCMessage_TwoButton(errorContext));
            message = "You will loose all your current progression if you start a new game. Do you want to continue?";
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton("Continue (A)", input_actionID_Valid);
            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton("Cancel (B)", input_actionID_Back);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());
            break;
        case Sav_UserNotSignedIn:
            pTRCMessage = newAlloc(mId_System, TRCMessage_UserNotSignedIn_X360(GAMEMANAGER->getMainIndexPlayer(), errorContext));
            message = buildText(4022);
            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton(buildText(4059, ContextIconType_Invalid), input_actionID_Valid);
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton(buildText(4060, ContextIconType_Invalid), input_actionID_Back);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());
            break;
        case Sav_ErrorDuringSave:
        case Sav_DeviceNoMoreAvailableForLoad:
        case Sav_DeviceNoMoreAvailableForSave:
            pTRCMessage = newAlloc(mId_System, TRCMessage_SavDeviceNoMoreAvailableForSaving_X360(errorContext));
            pTRCMessage->SetForceOverPauseMenu();
            message = buildText(4012);

            ((TRCMessage_SavDeviceNoMoreAvailableForSaving_X360*)pTRCMessage)->setRightButton(buildText(4057, ContextIconType_Invalid), input_actionID_Back); // 4057
            ((TRCMessage_SavDeviceNoMoreAvailableForSaving_X360*)pTRCMessage)->setLeftButton(buildText(4058, ContextIconType_Invalid), input_actionID_Valid);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());
            break;
        case Sav_AskForDelete:
            pTRCMessage = new TRCMessage_TwoButton(errorContext);
            pTRCMessage->SetForceOverPauseMenu(); 
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());

            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton(buildText(4142, ContextIconType_Invalid), input_actionID_Back);  //cancel
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton(buildText(4064, ContextIconType_Invalid), input_actionID_Valid); // continue
            message = buildText(4087);

            break;
        case Sav_WarningBoot:
            pTRCMessage = newAlloc(mId_System, TRCMessage_NoButtonTimer(5.0, errorContext));
            //4009 = saving warning
            //4137 = achievement warning
            if(SAVEGAME_ADAPTER->IsSaveSystemEnable())
            {
            message = buildText(4009) + "\n\n" + buildText(4137);
            } else
            {
                message = buildText(4137);
            }
            break;
        case Sav_SaveAskForOverwrite:
            pTRCMessage = newAlloc(mId_System, TRCMessage_SavOverWriteSaving_X360(errorContext));
            pTRCMessage->SetForceOverPauseMenu();            
            message = buildText(4013);
     
            ((TRCMessage_SavOverWriteSaving_X360*)pTRCMessage)->setRightButton(buildText(4059, ContextIconType_Invalid), input_actionID_Valid);
            ((TRCMessage_SavOverWriteSaving_X360*)pTRCMessage)->setLeftButton(buildText(4060, ContextIconType_Invalid), input_actionID_Back);

            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer_Internal());
            break;

        case Sav_ErrorDuringLoad:
            pTRCMessage = new TRCMessage_OneButton(errorContext);
            message = buildText(4017);

            ((TRCMessage_OneButton*)pTRCMessage)->setButton(buildText(4061, ContextIconType_Invalid), input_actionID_Valid);    
            break;

        case Sav_CorruptedFile:
            pTRCMessage = new TRCMessage_SavErrorDuringLoad_X360(errorContext);
            pTRCMessage->SetForceOverPauseMenu(); 
            message = buildText(4017);

            ((TRCMessage_SavErrorDuringLoad_X360*)pTRCMessage)->setLeftButton(buildText(4065, ContextIconType_Invalid), input_actionID_Valid);
            ((TRCMessage_SavErrorDuringLoad_X360*)pTRCMessage)->setRightButton(buildText(4058, ContextIconType_Invalid), input_actionID_Back);
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

    void TRCManagerAdapter_x360::init()
    {
        TRCManagerAdapter::init();
    }

    //------------------------------------------------------------
    // TRCMessage_NoAvailableDevice_X360
    //------------------------------------------------------------
    void TRCMessage_NoAvailableDevice_X360::onLeftButtonAction()
    {
        SAVEGAME_ADAPTER->DoNotUseSaveSystem();
        SAVEGAME_ADAPTER->signalAnswerChoseNoDevice(Adapter_Savegame::DeviceChosenAsNone_NoLongerSave);
    }
    
    void TRCMessage_NoAvailableDevice_X360::onRightButtonAction()
    {
        SAVEGAME_ADAPTER->signalAnswerChoseNoDevice(Adapter_Savegame::DeviceChosenAsNone_ChangeDevice);
    }
    //------------------------------------------------------------
    // TRCMessage_SavDeviceNoMoreAvailableForSaving_X360
    //------------------------------------------------------------
    void TRCMessage_SavDeviceNoMoreAvailableForSaving_X360::onLeftButtonAction()
    {
        // Do not save
        SAVEGAME_ADAPTER->signalAnswerDeviceNoLongerValidForSave(Adapter_Savegame::DeviceNoLongerValidForSave_NoLongerSave);
    }

    void TRCMessage_SavDeviceNoMoreAvailableForSaving_X360::onRightButtonAction()
    {
        // select a storage
        SAVEGAME_ADAPTER->signalAnswerDeviceNoLongerValidForSave(Adapter_Savegame::DeviceNoLongerValidForSave_ChangeDevice);
        
    }
    //------------------------------------------------------------
    // TRCMessage_SavOverWriteSaving_X360
    //------------------------------------------------------------
    void TRCMessage_SavOverWriteSaving_X360::onRightButtonAction()
    {
        // Continue
        SAVEGAME_ADAPTER->signalOkForOverwrite(Adapter_Savegame::FileExists_Overwrite);        
    }
    void TRCMessage_SavOverWriteSaving_X360::onLeftButtonAction()
    {
        // Do not save
        SAVEGAME_ADAPTER->signalOkForOverwrite(Adapter_Savegame::FileExists_ChangeDevice);
    }
    //void TRCMessage_SavOverWriteSaving_X360::onMidButtonAction()
    //{
    //    // Another device
    //    SAVEGAME_ADAPTER->signalOkForOverwrite(Adapter_Savegame::FileExists_ChangeDevice);
    //}

    void TRCMessage_UserNotSignedIn_X360::update()
    {
        TRCMessage_TwoButton::update();

        if(NETWORKSERVICES->isUserSignedIn(_currentPlayerIndex))
        {
            kill();
            callOnCloseCallback();
        }
    }


    void TRCMessage_SavErrorDuringLoad_X360::onLeftButtonAction()
    {
        SAVEGAME_ADAPTER->signalAnswerLoadError(Adapter_Savegame::LoadError_DeleteFile);
    }

    void TRCMessage_SavErrorDuringLoad_X360::onRightButtonAction()
    {
        SAVEGAME_ADAPTER->signalAnswerLoadError(Adapter_Savegame::LoadError_CancelLoad);
    }

    bbool TRCMessage_DeconnectionDuringGamePlay_X360::start()
    {
        if ( TRCMessage_OneButtonWithCB::start() )
        {
            if ( !GAMEMANAGER->isPlayableScreen() )
            {
                return bfalse;
            }
            return btrue;
        }
        return bfalse;
    }

} // namespace ITF

