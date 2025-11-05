#include "precompiled_TRCAdapter_PS5.h"

#include "TRCAdapter_PS5.h"
#include "engine/singleton/Singletons.h"
#include "gameplay/GameplayTypes.h"
#include "engine/localisation/LocalisationManager.h"
#include "engine/actors/managers/ContextIconsManager.h"
#include "gameplay/Managers/GameManager.h"
#include "gameplay/Components/UI/UIMenu.h"

namespace ITF
{
    static String buildText(u32 lineID, EContextIconType iconType = ContextIconType_Invalid)
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

    bbool TRCManagerAdapter_PS5::_buildAndAddMessage(ErrorContext errorContext)
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

        case Sav_WarningBoot:
            // Warning not needed, the save system is in memory and secured, we just skip it
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
            ITF_ASSERT_CRASH(0, "The save system is using the memory one so should never fail");
            pTRCMessage = new TRCMessage_OneButton(errorContext);
            message = buildText(4017);
            _isDisplayingCorruptedSaveMsg = btrue;
            ((TRCMessage_OneButton*)pTRCMessage)->setButton(buildText(4061, ContextIconType_Invalid), input_actionID_Valid);
            break;
        default:
            ITF_ASSERT_MSG(0, "Unhandled errorContext %d", errorContext);
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

} // namespace ITF

