#include "precompiled_engine.h"
#ifndef _ITF_HATPICMANAGER_H_
#include "gameplay/Managers/PadHapticsManager.h"
#endif
#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

/********* MANAGERS ***********/

#ifndef _ITF_FACTIONMANAGER_H_
#include "gameplay/managers/FactionManager.h"
#endif //_ITF_FACTIONMANAGER_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_TWEENINSTRUCTIONS_H_
#include "gameplay/Components/Common/TweenInstructions.h"
#endif //_ITF_TWEENINSTRUCTIONS_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

namespace ITF
{
    bool PrefetchGmt( const Path& path, PrefetchFactory::Params& params )
    {
        return GAMEINTERFACE->allocateGameMaterial( path, true ) != NULL;
    }

    GameInterface::GameInterface() :
        m_isShowingSavingNotification(bfalse)
        , m_timeStartedNotifyingSave(0.f)
        , m_savePromptCount(0)
        , m_persistentGameDataFactory(NULL)
        , m_canInterruptCutSceneCallback(NULL)
        , m_cutSceneAlreadySeenCallback(NULL)
        , m_cutSceneDefaultUnskippableDurationFirstTime(2.f)
    {
        m_gameScreenFactory = newAlloc(mId_System, ObjectFactory);
        m_persistentGameDataFactory = newAlloc(mId_System, ObjectFactory);
        FactionManager::create();
        LinkManager::create();
        CinematicManager::create();
        TweenInstructionFactory_Template::create();
        TweenInstructionFactory::create();
        EventDelayHandler::create();
#ifdef USE_PAD_HAPTICS
        PadHapticsManager::create();
#else
        PadRumbleManager::create();
#endif
    }

    GameInterface::~GameInterface()
    {
        m_templateClientHandler.freeUsedTemplates();
        CinematicManager::destroy();
        FactionManager::destroy();
        LinkManager::destroy();
        EventDelayHandler::destroy();
#ifdef USE_PAD_HAPTICS
        PadHapticsManager::destroy();
#else
        PadRumbleManager::destroy();
#endif
        SF_DEL(m_gameScreenFactory);
        SF_DEL(m_persistentGameDataFactory);
    }

    void GameInterface::unloadGameResources()
    {
        if(GAMEMANAGER)
            GAMEMANAGER->unloadGameResources();
    }

    void GameInterface::init()
    {
        FACTION_MANAGER->init();
        LINKMANAGER->init();
#ifdef USE_PAD_HAPTICS
        HAPTICS_MANAGER->init();
#else
        PADRUMBLEMANAGER->init();
#endif

        registerSavegameErrorMenus();
    }

    void GameInterface::registerSavegameErrorMenus()
    {
        if(SAVEGAME_ADAPTER)
        {
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_FileAlreadyExists, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_DeviceWhenDeviceNoLongerValidForSave, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_SaveError, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_DeviceChosenAsNone, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_DeviceWhenDeviceNoLongerValidForLoad, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_LoadError, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_LoadCantFind, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_FileAlreadyExistsForNewGame, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_UserNotSignedIn, loadSavePromptShowHideCallback);
            SAVEGAME_ADAPTER->setShownHiddenPromptCallback(Adapter_Savegame::PromptType_DeletingCorruptedFile, loadSavePromptShowHideCallback);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID GameInterface::onMenuPageAction(UIMenu * _menu, const StringID & _action, const StringID &_defaultAction)
    {
        return UI_MENUMANAGER->onMenuPageAction_Default1ButtonMenu(_menu, _action, _defaultAction);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameInterface::loadSavePromptShowHideCallback(Adapter_Savegame::PromptType _prompt, bbool _show)
    {
        if(!_show)
            return ;

        // Save error => TRC error
        TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte;

        switch (_prompt)
        {
        case Adapter_Savegame::PromptType_DeletingCorruptedFile:
            errorContext = TRCManagerAdapter::Sav_AskForDelete;
            break;

        case Adapter_Savegame::PromptType_FileAlreadyExists:
            errorContext = TRCManagerAdapter::Sav_SaveAskForOverwrite;
            if(TRC_ADAPTER)
            {
                String8 BaseName;
                String  DisplayedName("");
                bbool   EmptyContent;
                SAVEGAME_ADAPTER->getEnumeratedContent(0, 0, BaseName, DisplayedName, EmptyContent);
                TRC_ADAPTER->setCustomParam_String(DisplayedName);

                // Device Name ?
                String SelectedDeviceName("");
                SAVEGAME_ADAPTER->GetSelectedDeviceName(SelectedDeviceName);
                TRC_ADAPTER->setCustomParam_String2(SelectedDeviceName);
            }
            break;

        case Adapter_Savegame::PromptType_DeviceWhenDeviceNoLongerValidForSave:
            errorContext = TRCManagerAdapter::Sav_DeviceNoMoreAvailableForSave; break;

        case Adapter_Savegame::PromptType_SaveError:
            errorContext = TRCManagerAdapter::Sav_ErrorDuringSave; break;

        case Adapter_Savegame::PromptType_DeviceChosenAsNone:
            errorContext = TRCManagerAdapter::Sav_NoAvailableStorage; break;

        case Adapter_Savegame::PromptType_DeviceWhenDeviceNoLongerValidForLoad:
            errorContext = TRCManagerAdapter::Sav_DeviceNoMoreAvailableForLoad; break;

        case Adapter_Savegame::PromptType_LoadCantFind:
            errorContext = TRCManagerAdapter::Sav_FileNotAvailable; break;

        case Adapter_Savegame::PromptType_LoadError:
            errorContext = TRCManagerAdapter::Sav_CorruptedFile; break;

        case Adapter_Savegame::PromptType_UserNotSignedIn:
            errorContext = TRCManagerAdapter::Sav_UserNotSignedIn; break;
        default:
            ITF_ASSERT(0);
            break;
        }

        if(TRC_ADAPTER && errorContext != TRCManagerAdapter::GenericContexte)
        {
            TRC_ADAPTER->addMessage(errorContext);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameInterface::updateViewsBehavior(f32 dt)
    {
        const SafeArray<View*>& views = View::getViews();
        for (u32 i = 0; i < views.size(); i++)
        {
            if (views[i]->isActive() && views[i]->getCamera())
            {
                Camera* pCam = views[i]->getCamera();
                pCam->update(dt);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameInterface::postWorldUpdate(f32 _dt)
    {
        updateSavingNotification();

        if(!GAMEMANAGER->isInPause())
            EVENTDELAY_HANDLER->update(_dt);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameInterface::updateSavingNotification()
    {
        if(SAVEGAME_ADAPTER->isSaving())
        {
            GAMEMANAGER->displaySaveNotification(btrue);
        }
#if defined(ITF_SUPPORT_CHEAT) && defined(ITF_AUTO_UNLOCK)
        else
        {
            GAMEMANAGER->setSuppressSaveNotification(bfalse);
        }
#endif
    }

	const GameMaterial_Template* GameInterface::allocateGameMaterial( const Path& _fileName, bool prefetch /* = false */ )
	{
        m_templateClientHandler.addUsedTemplate(_fileName.getStringID());
		return TEMPLATEDATABASE->getTemplate<GameMaterial_Template>(&m_templateClientHandler, _fileName, prefetch);
	}

    void GameInterface::reloadGameMaterial( const Path& _fileName ) const
    {
#ifdef ITF_SUPPORT_HOTRELOAD
        TEMPLATEDATABASE->reloadTemplate<GameMaterial_Template>(_fileName);
#endif //ITF_SUPPORT_HOTRELOAD
    }


} // ITF


