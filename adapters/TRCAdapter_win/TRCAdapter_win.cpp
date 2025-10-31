#include "precompiled_trcadapter_win.h"

#include "adapters/TRCAdapter_win/TRCAdapter_win.h"

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

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

    bbool TRCManagerAdapter_win::_buildAndAddMessage(ErrorContext	errorContext)
    {
        String     message;
        TRCMessage_Base* pTRCMessage = NULL;

        switch(errorContext)
        {
        case Pad_DisconnectingDuringTitleScreen:            
            message = "A pad is required to play Rayman Origins.";            
            pTRCMessage = new TRCMessage_OneButtonWithCB(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, 0));
            pTRCMessage->SetForceOverPauseMenu();
            break;
        case Pad_DisconnectingDuringMenu:
        case Pad_DisconnectingDuringGameplay:
            ITF_ASSERT(_u32_customParam != U32_INVALID);
            message = "Communications with the Pad have been interrupted.";            
            pTRCMessage = new TRCMessage_OneButtonWithCB(
                new TRCMessage_Callback_WaitforPadState(TRCMessage_Callback_WaitforPadState::connected, _u32_customParam));                
             
            pTRCMessage->SetForceOverPauseMenu();
            pTRCMessage->changeDisplayPriority(TRCMessage_Base::High);
            if(errorContext==Pad_DisconnectingDuringGameplay && _u32_customParam != GAMEMANAGER->getMainIndexPlayer())
            {
                ((TRCMessage_OneButtonWithCB*)pTRCMessage)->setButton(buildText(4164, ContextIconType_Select), input_actionID_Back);      
            }
            pTRCMessage->setActivePlayer(_u32_customParam);
            break;
        case Pad_LowBattery:
            message = "The battery charge is running low.";//temp
            pTRCMessage = new TRCMessage_NoButtonTimer(2.2, errorContext);
            break;
        case Sav_OperationRunning:
            pTRCMessage = newAlloc(mId_System, TRCMessage_NoButtonCallback(
                new TRCMessage_Callback_WaitSaveManager(2.0), errorContext
                ));
            message = "Operation is running. Please Wait.";
            pTRCMessage->SetForceOverPauseMenu();
            break;
        case Sav_NewGameAskForOverwrite:
            pTRCMessage = newAlloc(mId_System, TRCMessage_TwoButton(errorContext));
            message = "You will loose all your current progression if you start a new game. Do you want to continue?";
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton("Continue (A)", input_actionID_Valid);
            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton("Cancel (B)", input_actionID_Back);            
            break;
        case Sav_SaveAndExit:
            pTRCMessage = newAlloc(mId_System, TRCMessage_TwoButton(errorContext));
            pTRCMessage->SetForceOverPauseMenu();
            message = "If you save your game, you will return to the main menu.";
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton("Continue and save (A)", input_actionID_Valid);
            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton("Back to the game (B)", input_actionID_Back);
            break;
        case Sav_AskForDelete:
            pTRCMessage = new TRCMessage_TwoButton(errorContext);
            pTRCMessage->setAllowedPlayer(GAMEMANAGER->getMainIndexPlayer());

            ((TRCMessage_TwoButton*)pTRCMessage)->setRightButton(buildText(4142, ContextIconType_Invalid), input_actionID_Back);  //cancel
            ((TRCMessage_TwoButton*)pTRCMessage)->setLeftButton(buildText(4064, ContextIconType_Invalid), input_actionID_Valid); // continue
            message = buildText(4087);
            break;
        //case Sav_WarningBoot:
        //    pTRCMessage = newAlloc(mId_System, TRCMessage_NoButtonTimer(4.0, errorContext));
        //    message.setTextFormat("%s\n\n%s", buildText(4009).cStr(), buildText(4137).cStr());
        //    //4137  = achievement warning

        //    break;
        case Sav_ErrorDuringLoad:
            pTRCMessage = new TRCMessage_OneButton(errorContext);
            message = buildText(4017);

            ((TRCMessage_OneButton*)pTRCMessage)->setButton(buildText(4061, ContextIconType_Invalid), input_actionID_Valid);    
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

    bbool TRCManagerAdapter_win::checkSpaceDisk(ErrorContext ErrorContext, u32 bytes)
    {
        return btrue;
    }

    void TRCManagerAdapter_win::init()
    {
        TRCManagerAdapter::init();
    }

} // namespace ITF

