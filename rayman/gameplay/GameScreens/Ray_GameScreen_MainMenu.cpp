#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

//#include "../make/Techno/x360/spa.h"

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifndef ITF_ONLINE_ADAPTER_UBISERVICES_H
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/SessionService_Ubiservices.h"
#endif //ITF_ONLINE_ADAPTER_UBISERVICES_H

#ifndef _ITF_UPLAYSERVICE_
#include "engine/networkservices/UPlayService.h"
#endif //_ITF_UPLAYSERVICE_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif //_ITF_RAY_GAMEOPTIONNAMES_H_

#define TITLEMENU_FRIENDLY          "menutitlescreen"
#define MAINMENU_FRIENDLY           "menuPlaywii" //menuPlay
#ifdef ITF_WINDOWS
#define NEW_MENU_FRIENDLY             "menuStartPC"
#else
#define NEW_MENU_FRIENDLY             "menuStartConsole"
#endif

#define MAX_SLOTS                   3
#define LOADERROR_FRIENDLY          "menuMainCantLoad"
#define FRAME_TO_WAIT_TRCMSG        4
#define DAYS_TO_SHOW_WARNING_BOOT   10
#ifdef ITF_WII
#define TIMETOWAIT_TITLESCREEN      3
#define STARTMENU_FRIENDLY          "menuStart_WII"
#else
#define TIMETOWAIT_TITLESCREEN      0
#define STARTMENU_FRIENDLY          "menuStart"
#endif

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_
namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_MainMenu)


    bbool Ray_GameScreen_MainMenu::m_firstLoading = btrue;
    Ray_GameScreen_MainMenu *Ray_GameScreen_MainMenu::s_this = NULL;
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_MainMenu::Ray_GameScreen_MainMenu() :
    m_state(State_Initializing),
        m_playerIndex(0),
        m_storageDeviceChanged(bfalse),
        m_firstEntryInPlayMenu(btrue),
        m_wasDisplayingError(bfalse),
        m_currentSlotSelected(0),
        m_rollingDemoIsPlaying(bfalse),
        m_savingProcessEnabled(bfalse),
        m_startedMusic(bfalse),
        m_timeToWaitBeforeStartScreen(0.0),
        m_timeStartingToWait(0.0),
        m_wasPreloaded (bfalse),
        m_waitingFrameForTRCMsg(0),
        m_pendingShowPCMenu(bfalse),
        m_shouldShowWarningBoot(bfalse)

#ifdef ITF_SUPPORT_NETWORKSERVICES
        ,m_validUser(NULL)
#endif //ITF_SUPPORT_NETWORKSERVICES
    {
        s_this = this;
        m_showLoadMenuEntry.push_back( btrue );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_MainMenu::~Ray_GameScreen_MainMenu()
    {
        SAVEGAME_ADAPTER->setDeviceChangedCallback(NULL);
        if(RAY_GAMEMANAGER->getDebugSaves())
            if(TRC_ADAPTER) TRC_ADAPTER->removeListener(onCloseTRCMessage, this);

        GAMEMANAGER->setInputModeForMenu(bfalse);

        if(!m_savingProcessEnabled)
        {
            SAVEGAME_ADAPTER->DoNotUseSaveSystem();
        }
#ifdef ITF_SUPPORT_NETWORKSERVICES
        SF_DEL(m_validUser);
#endif //ITF_SUPPORT_NETWORKSERVICES
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onWorldLoaded()
    {
        if(m_firstLoading)
        {
            if(TRC_ADAPTER)
            {
                //TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_WarningBoot);
                TRC_ADAPTER->enablePadTask();
            }

            m_timeStartingToWait = SYSTEM_ADAPTER->getTime();
            m_timeToWaitBeforeStartScreen = TIMETOWAIT_TITLESCREEN;
            m_firstLoading = bfalse;
        }
        setState(State_ShowingTitleScreen);

        calculateAndLogLastPlayTime();
        RAY_GAMEMANAGER->saveGameOptions();

        if (!m_wasPreloaded)
            RAY_GAMEMANAGER->setMusicTheme(ITF_GET_STRINGID_CRC(menu,105712373));

        if (RAY_GAMEMANAGER->isInPause())
            RAY_GAMEMANAGER->leavePause(bfalse);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::init()
    {
        m_state = State_Initializing;
        //invalidate checkpoint path to make sure next default prefetch will takes the first checkpoint
        RAY_GAMEMANAGER->setFirstCheckpointPath(ObjectPath());

        RAY_GAMEMANAGER->removePlayerActors();

        RAY_GAMEMANAGER->initPlayers();

        World* playerWorld = (World*)GAMEMANAGER->getPlayerWorld().getObject();
        playerWorld->setActive(bfalse);

        GAMEMANAGER->spawnPlayerActors(Ray_GameScreen_Gameplay::GetClassCRCStatic());

		RAY_GAMEMANAGER->stopFade(btrue);

        bool        prefetched;
        ObjectRef   rWorld = RAY_GAMEMANAGER->getMainMenuWorld();

        m_world = (World*)GETOBJECT(rWorld);
        prefetched = m_world != NULL;
        if ( m_world == NULL )
        {
            rWorld = RAY_GAMEMANAGER->createMap();
            m_world = (World*)GETOBJECT(rWorld);
            m_wasPreloaded = bfalse;
            ITF_ASSERT(m_world!=NULL);
            if ( m_world != NULL )
            {
                //load menu world
                RAY_GAMEMANAGER->loadMap(rWorld, RAY_GAMEMANAGER->getMainMenuBackMapForDebugSaves(),btrue,btrue,btrue,GameManager::LEVEL_NAME_INITIALISATION_TYPE_EMPTY);
                m_world->setAlwaysActive(btrue);
            }
            else
            {
                ITF_FATAL_ERROR("Could not load main menu map");
            }
        }
        else
            m_wasPreloaded = btrue;

        setState(State_WaitMapLoaded);

        Vec3d pos(0, 0, 1);
        CAMERA->forcePosition(pos);
        CAMERACONTROLLERMANAGER->teleport(pos);

        //
        SAVEGAME_ADAPTER->setDeviceChangedCallback(storageDeviceChangedCallback);
        updatePlayerIndex();
        setCanShowLoadGame(btrue);

        GAMEMANAGER->setInactiveRichPresenceIndex(PRESENCE_NOTPLAYING);
        GAMEMANAGER->updateRichPresenceForActivePlayers(PRESENCE_NOTPLAYING, btrue);

        // check ..
        if(RAY_GAMEMANAGER->getDebugSaves())
            if(TRC_ADAPTER) TRC_ADAPTER->addListener(onCloseTRCMessage, this);

        GAMEMANAGER->setInputModeForMenu(btrue);

        RAY_GAMEMANAGER->stopAllSequences();
        if(m_firstLoading)
        {
            SAVEGAME_ADAPTER->setCodeCRC ( RAY_GAMEMANAGER->computeSaveCodeCRC() );
        }

        // Invalid the main index in the REWARD_ADAPTER to force a reloading of the achievements
        if(REWARD_ADAPTER)
            REWARD_ADAPTER->setMainPlayerIndexIsInvalid();

        if ( prefetched )
            RAY_GAMEMANAGER->switchToPreloadedMap( rWorld );

        // Allows all the button in the title screen
        INPUT_ADAPTER->setButtonModeStatus(InputAdapter::MixedMode);

        CONFIG->m_blackClearColorNeeded = bfalse;
        m_waitingFrameForTRCMsg = FRAME_TO_WAIT_TRCMSG;

#if defined(ITF_SUPPORT_CHEAT) && defined(ITF_AUTO_UNLOCK)
        // TODO: need to be removed for final release
        RAY_GAMEMANAGER->cheatUnlockAllLevel();
        RAY_GAMEMANAGER->cheatMaxCurrency();
        RAY_GAMEMANAGER->saveGameState(false, RAY_GAMEMANAGER->getCurrentSlotIndex());
#endif
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::updatePlayerIndex()
    {
        m_playerIndex = GAMEMANAGER->getMainIndexPlayer_Internal();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::setCanShowLoadGame(bbool _show, u32 _slot)
    {
        if ( m_showLoadMenuEntry.size() <= _slot )
            m_showLoadMenuEntry.resize ( _slot + 1 );

        m_showLoadMenuEntry[_slot] = _show;
        if(_show)
            SAVEGAME_ADAPTER->UseSaveSystem();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::startSavegameCheckOnAnyDevice()
    {
        updatePlayerIndex();
        Adapter_Savegame::ErrorCode code = SAVEGAME_ADAPTER->startSavegameEnumerationUpdateOnEveryDevice(getPlayerIndex());
        if (code==Adapter_Savegame::Error_LoadOrSaveForbiddenForPlayer)
            setCanShowLoadGame(bfalse);
        else if (code!=Adapter_Savegame::Error_Ok)
            setCanShowLoadGame(btrue);
        else
            SAVEGAME_ADAPTER->setCurrentOperationCompleteCallback(completedEnumOnEveryDeviceCallback);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::completedEnumOnEveryDeviceCallback()
    {
        //if (SAVEGAME_ADAPTER->getLastAsyncErrorCode()!=Adapter_Savegame::Error_Ok)
        //{
        //    s_this->setCanShowLoadGame(btrue); //If IO error, let's be careful
        //    return;
        //}

        //u32 count = SAVEGAME_ADAPTER->getEnumeratedContentCount(s_this->getPlayerIndex());
        //bbool found = bfalse;
        //String8 baseName;
        //String displayName;
        //bbool emptySave = bfalse;
        ////
        //for (u32 index=0; index<count; index++)
        //{
        //    if (SAVEGAME_ADAPTER->getEnumeratedContent(s_this->getPlayerIndex(), index, baseName, displayName, emptySave))
        //    {
        //        if (baseName==SAVEGAME_BASENAME)
        //        {
        //            found = btrue;
        //            break;
        //        }
        //    }
        //}
        ////
        //s_this->setCanShowLoadGame(found);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::storageDeviceChangedCallback()
    {
        if(s_this->m_state >= State_ShowingMainMenu_Root)
        {
            //s_this->setCanShowLoadGame(btrue);
            s_this->m_storageDeviceChanged = btrue;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::setState(State _state)
    {
        if (m_state==_state)
            return;

        //Leaving state
        switch(m_state)
        {
        case State_ShowingPressStart: leave_ShowingPressStart(); break;
        case State_Exited: leave_Exited(); break;
        }

        m_state = _state;

        // Entering state
        switch(m_state)
        {
        case State_ShowingTitleScreen: enter_ShowingTitleScreen(); break;
        case State_Exited : enter_Exited(); break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_WaitMapLoaded()
    {
        if(m_world && !m_world->isSceneAsyncLoadRunning())
        {
            if(!m_prefetchStarted)
            {
                m_world->prefetchResource();
                m_prefetchStarted = btrue;
            }
            else if(m_world->isPhysicalReady())
            {
                m_world->stopPrefetch();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::enter_ShowingTitleScreen()
    {
        //show menu
        setCanShowLoadGame(btrue);
        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getTitleScreenMenuPriority(), TITLEMENU_FRIENDLY, btrue, this);

        // Active all pad

#ifdef ITF_SUPPORT_NETWORKSERVICES
        SF_DEL(m_validUser);
#endif //ITF_SUPPORT_NETWORKSERVICES
        GAMEMANAGER->setMainIndexPlayer(U32_INVALID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingPressStart()
    {
        // $GB PS5 hack for nice startup screen transition
        SYSTEM_ADAPTER->hideStartupSplashScreen();

        if ( !m_startedMusic )
        {
            // Start menu music
            const StringID menu = ITF_GET_STRINGID_CRC(Menu,105712373);
            MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY,menu,0.0f,1.0f);

            m_startedMusic = btrue;
        }

        // WII only
#ifdef ITF_WII
        if( (!TRC_ADAPTER || !TRC_ADAPTER->isDisplayingError())
            && !m_rollingDemoIsPlaying)
        {
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            for(u32 pad = 0; pad < /*JOY_MAX_COUNT*/1; pad++)
            {
                INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, pad, buts, JOY_MAX_BUT);
                for (u32 but = 0; but < JOY_MAX_BUT; but++)
                {
                    if (buts[but] == InputAdapter::JustReleased && but != m_joyButton_Home)
                    {
                        GAMEMANAGER->setMainIndexPlayer(pad);
                        onPressStartAction();
                        setState(State_PressingStart);
                        RAY_GAMEMANAGER->forceInputConfigReset(pad);
                    }
                }
            }
        }
#endif //ITF_WII
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_PressingStart()
    {
        // on wii, we have to wait the end of the input to continue
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, 0, buts, JOY_MAX_BUT);
        for (u32 but = 0; but < JOY_MAX_BUT; but++)
        {
            if (buts[but] != InputAdapter::Released && but != m_joyButton_Home)
            {
                return;
            }
        }
        // back to normal way
        setState(State_ShowingMainMenu_SaveLoad_Root);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::leave_ShowingPressStart()
    {
        startSavegameCheckOnAnyDevice();

        //hide menu
        UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getGameMenuPriority());
        UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getTitleScreenMenuPriority());
        // UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), MAINMENU_FRIENDLY, btrue, this);
        UI_MENUMANAGER->disableMenu(MAINMENU_FRIENDLY);
        m_pendingShowPCMenu = btrue;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_Load_StartLoading()
    {
        if (!SAVEGAME_ADAPTER->hasPendingOperation()) //Wait for previous IO completion
        {
            // Fix a bug when the device is changed in same time the input is received.
            // -> storage choice poped
            // -> and input was treated just after the window was closed
            if(m_storageDeviceChanged)
            {
                // come back to the slot screen
                setState(State_ShowingMainMenu_SaveLoad_Root);
                return;
            }

            //Load
            String8 baseName;
            RAY_GAMEMANAGER->getSaveBaseName(baseName, m_currentSlotSelected);
            RAY_GAMEMANAGER->clearWholeGameState();

            Adapter_Savegame::ErrorCode err=SAVEGAME_ADAPTER->startLoad(getPlayerIndex(), m_currentSlotSelected, baseName);
            if (err!=Adapter_Savegame::Error_Ok)
            {
                if (err==Adapter_Savegame::Error_SavegameNotFound || err==Adapter_Savegame::Error_LoadSave_NoDevice)
                    setState(State_ShowingMainMenu_SaveLoad_Root);
                else
                    setState(State_ShowingMainMenu_Load_Error);
            }
            else
            {
                setState(State_ShowingMainMenu_Load_WaitLoaded);
                UI_MENUMANAGER->disableMenu(MAINMENU_FRIENDLY);
                UI_MENUMANAGER->stopMenuSound();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_Load_WaitLoaded()
    {
        if (!SAVEGAME_ADAPTER->hasPendingOperation() && !TRC_ADAPTER->isDisplayingError())
        {
            Adapter_Savegame::ErrorCode err=SAVEGAME_ADAPTER->getLastAsyncErrorCode();

            if (err!=Adapter_Savegame::Error_Ok)
            {
                if (err==Adapter_Savegame::Error_SavegameNotFound || err==Adapter_Savegame::Error_LoadSave_NoDevice)
                    setState(State_ShowingMainMenu_SaveLoad_Root);
                else
                setState(State_ShowingMainMenu_Load_Error);
            }
            else if (!m_currentlyExiting)
            {
                const ITF_VECTOR<u8> &savegame = SAVEGAME_ADAPTER->getLastLoadedOrSavedData();

                RAY_GAMEMANAGER->setCurrentSlotIndex(m_currentSlotSelected);
                u32 dataCRC = RAY_GAMEMANAGER->computeSaveDataCRC(&savegame[0], savegame.size());

                if (SAVEGAME_ADAPTER->checkDataCRCForCurrentSave(dataCRC)
                    && RAY_GAMEMANAGER->loadGameState(&savegame[0], savegame.size()))
                {
                    // Free preloaded prologue if preloaded
                    ObjectRef prologueWorld = RAY_GAMEMANAGER->getPreloadedPrologueWorld();
                    if(prologueWorld.isValid())
                        GAMEMANAGER->deletePreloadedMap(prologueWorld);

                    World* playerWorld = (World*)GAMEMANAGER->getPlayerWorld().getObject();
                    playerWorld->setActive(btrue);

                    RAY_GAMEMANAGER->postGameScreenChange<Ray_GameScreen_WorldMap>(bfalse);
                    m_savingProcessEnabled = btrue;
                    setState(State_Exited);
                    m_currentlyExiting = btrue;
                } else
                {
                    setState(State_ShowingMainMenu_Load_Error);
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_Load_Action()
    {
        if(SAVEGAME_ADAPTER->IsSaveSystemEnable() && m_showLoadMenuEntry[m_currentSlotSelected])
        {
            setState(State_ShowingMainMenu_Load_StartLoading);
        } else
        {
            ITF_ASSERT(0);
            setState(State_ShowingPressStart);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_SaveLoad_WaitingForAction()
    {
        if(SAVEGAME_ADAPTER->hasPendingOperation() || m_storageDeviceChanged)
        {
            // If an operation starts during this step, go back
            setState(State_ShowingMainMenu_SaveLoad_Root);
            return;
        }

        // Check if something occurs
        // SAVEGAME_ADAPTER->checkForSaveStateValidity(getPlayerIndex());
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::fillOneSlot(u32 _slotIndex, const String& _text)
    {
        const StringID slotList[] =
        {
            ITF_GET_STRINGID_CRC(slot_1, 1889828865),
            ITF_GET_STRINGID_CRC(slot_2, 539726452),
            ITF_GET_STRINGID_CRC(slot_3, 2254922994)
        };
        UI_MENUMANAGER->changeMenuItemFriendlyByID(MAINMENU_FRIENDLY, slotList[_slotIndex], _text);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::fillSaveGameSlots(bbool forceEmpty)
    {
        String8 baseName;
        String  displayName;
        bbool   emptySave = bfalse;

        for (u32 count = 0; count<MAX_SLOTS; count++)
        {
            if(forceEmpty || !SAVEGAME_ADAPTER->IsSaveSystemEnable())
                emptySave = btrue;
            else
            {
                RAY_GAMEMANAGER->getSaveBaseName(baseName, count);
                SAVEGAME_ADAPTER->getEnumeratedContent(getPlayerIndex(), count, baseName, displayName, emptySave);
            }

            if(emptySave)
            {
                // Empty slot --> New game
                LocalisationId id;
                id = 3712;
                displayName = forceEmpty? "" : LOCALISATIONMANAGER->getText(id);
                emptySave = btrue;
            }

            fillOneSlot(count, displayName);
            setCanShowLoadGame(!emptySave , count);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::activeBlackScreen(bbool _active)
    {
        m_world->setAllowDraw(!_active);
        UIMenu* pUIMenu = UI_MENUMANAGER->getMenu(MAINMENU_FRIENDLY);
        if(pUIMenu)
        {
            ObjectRef worldR = pUIMenu->getWorldMenuRef();
            if(worldR.isValid())
            {
                World* pMenuWorld = (World*)worldR.getObject();
                if(pMenuWorld)
                {
                    pMenuWorld->setAllowDraw(!_active);
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_LoadSave_Root()
    {
        if(SAVEGAME_ADAPTER->hasPendingOperation() || GAMEMANAGER->isSaveNotificationDisplayed())
        {
            // Wait end of operation
            return;
        }

        if(m_storageDeviceChanged)
        {
            SAVEGAME_ADAPTER->checkForSaveStateValidity(getPlayerIndex());
            m_storageDeviceChanged = bfalse;
            return;
        }

        // Because more than one message can be displayed here, just wait a little
        if ( m_waitingFrameForTRCMsg != 0 )
        {
            m_waitingFrameForTRCMsg --;
            if(TRC_ADAPTER->isDisplayingCorruptedSave())
            {
                activeBlackScreen(btrue);
            }
        }

        if(!TRC_ADAPTER || !TRC_ADAPTER->isDisplayingError())
        {
            bbool warningBootShown = bfalse;
            if(TRC_ADAPTER && !TRC_ADAPTER->isWarningBootMessageDisplayed() && SAVEGAME_ADAPTER->IsSaveProcessEnable() && shouldShowWarningBootPopup())
            {
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_WarningBoot);
                TRC_ADAPTER->setWarningBootMessageIsDisplayed();
                warningBootShown = btrue;
            }

            activeBlackScreen(bfalse);
            if(m_pendingShowPCMenu && !warningBootShown)
            {
                UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), NEW_MENU_FRIENDLY, btrue, this);
                m_pendingShowPCMenu = bfalse;
                updateLastPlayTime();
            }

            // If the save system is disable, do not check save slot
            fillSaveGameSlots(bfalse);
            // --------------------------------------------
            setState(State_ShowingMainMenu_SaveLoad_WaitingForAction);
            UI_MENUMANAGER->enableMenu(MAINMENU_FRIENDLY);
        }
        else
        {
            m_waitingFrameForTRCMsg = FRAME_TO_WAIT_TRCMSG;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onCloseTRCMessage(const StringID & answer, TRCMessage_Base* pMessage, void* params)
    {
        Ray_GameScreen_MainMenu* pThis = (Ray_GameScreen_MainMenu*)params;

        const StringID buttonYes = ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_LEFT, 1255480269);
        const StringID buttonNo = ITF_GET_STRINGID_CRC(TEMPLATE_BUTTON_RIGHT, 692652888);

        StringID currentButtonID ;
        UIMenu* pUIMenu= UI_MENUMANAGER->getMenu(UI_MENUMANAGER->getCurrentMenuID());
        if(pUIMenu)
        {
            UIComponent* pUIComponent = pUIMenu->getUIComponentSelected();
            if(pUIComponent) currentButtonID = pUIComponent->getID();
        }

        if(pMessage->getContexte() == TRCManagerAdapter::Sav_WarningBoot)
        {
            if(pThis->m_pendingShowPCMenu)
            {
                UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), NEW_MENU_FRIENDLY, btrue, pThis);
                pThis->m_pendingShowPCMenu = bfalse;
                pThis->updateLastPlayTime();
            }
            return;
        }

        if(!currentButtonID.isValid())
            return ;

        switch (pThis->m_state)
        {
        case  State_ShowingMainMenu_NewGame_WaitTRCAnswer:
            if ( currentButtonID == buttonYes)
            {
                // Continue
                pThis->setState(State_ShowingMainMenu_NewGame_StartSaving);
            } else if ( currentButtonID == buttonNo )
            {
                // Cancel
                pThis->setState(State_ShowingMainMenu_SaveLoad_WaitingForAction);
            }
            break;
        case State_ShowingMainMenu_SaveLoad_WaitingForDeleteAnswer:
            if ( currentButtonID == buttonYes )
            {
                // Continue
                pThis->setState(State_ShowingMainMenu_SaveLoad_DeleteSave);
            } else  if ( currentButtonID == buttonNo )
            {
                // Cancel
                pThis->setState(State_ShowingMainMenu_SaveLoad_WaitingForAction);
            }
            break;
        case State_ShowingMainMenu_Load_Error:
            pThis->setState(State_ShowingMainMenu_SaveLoad_Root);
            break;
        default: break;
        }

        switch ( pMessage->getContexte() )
        {
        case TRCManagerAdapter::Sav_NoAvailableStorage:
            SAVEGAME_ADAPTER->signalAnswerChoseNoDevice(
                ( currentButtonID == buttonYes) ? Adapter_Savegame::DeviceChosenAsNone_NoLongerSave :
                Adapter_Savegame::DeviceChosenAsNone_ChangeDevice);
            break;
        case TRCManagerAdapter::Sav_UserNotSignedIn:
            SAVEGAME_ADAPTER->signalAnswerWhenUserNotSignedIn(
                ( currentButtonID == buttonYes ) ? Adapter_Savegame::NotSignedIn_Continue :
                Adapter_Savegame::NotSignedIn_Retry);
            break;
        case TRCManagerAdapter::Sav_AskForDelete:
            SAVEGAME_ADAPTER->signalForDeleteCorruptedFile(
                ( currentButtonID == buttonYes ) ? Adapter_Savegame::CorruptedFile_Delete :
                Adapter_Savegame::CorruptedFile_Cancel);
            break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_Ready()
    {
        const Path ProloguePath(RAY_GAMEMANAGER->getPrologueMap());
        ObjectRef rWorld = RAY_GAMEMANAGER->getPreloadedPrologueWorld();

        if(rWorld.isValid())
        {
            GAMEMANAGER->mayLoadBaseMap();

            RAY_GAMEMANAGER->mapLoadResetInfo();

            Ray_GameScreen_Gameplay::InitializationParameters initData;
            initData.m_mapName = ProloguePath;
            initData.m_forceUnloadBaseMap = bfalse;
            initData.m_worldRef = rWorld;

            RAY_GAMEMANAGER->postGameScreenChange<Ray_GameScreen_Gameplay>(bfalse, &initData );

            // start fade
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypePrologue());

            if(World* pWorld = (World*)rWorld.getObject())
                if(Scene* pScene = pWorld->getRootScene())
                {
                    RAY_GAMEMANAGER->setCurrentLevelName(pScene->getPath());
                    RAY_GAMEMANAGER->makeLevelStateFromPersistentUniverse();
                }

            const StringID& levelMusic = RAY_GAMEMANAGER->getMapConfigMusic(ITF_GET_STRINGID_CRC(Prologue,291121491));
            RAY_GAMEMANAGER->setMusicTheme(levelMusic);

            CONFIG->m_blackClearColorNeeded = btrue;
        }
        else
        {
            GAMEMANAGER->loadGameplayMap(ProloguePath, btrue, bfalse);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::stopPlayingIntroVideo()
    {
        // Cut the video
        EventPlayMovie ev;
        ev.setPlay(bfalse);
        EVENTMANAGER->broadcastEvent (&ev);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::startPlayingIntroVideo()
    {
        // Keep the change state first
        setState(State_ShowingMainMenu_NewGame_PlayingVideo);

        // Launch intro video here
        EventPlayMovie ev;
        EVENTMANAGER->broadcastEvent (&ev);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_WaitSavingProcess()
    {
        if(!SAVEGAME_ADAPTER->hasPendingOperation() && !GAMEMANAGER->isSaveNotificationDisplayed())
        {
            if (SAVEGAME_ADAPTER->getLastAsyncErrorCode()==Adapter_Savegame::Error_Ok)
            {
                setState(State_ShowingMainMenu_NewGame_WaitPrologueLoad);
            }
            else
            {
                setState(State_ShowingMainMenu_SaveLoad_Root);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_WaitPrologueLoad()
    {
        bbool nextStep = btrue;
        if(RAY_GAMEMANAGER->getPreloadedPrologueWorld().isValid())
        {
            nextStep = RAY_GAMEMANAGER->isPreloadedPrologueReady();
        }

        if(nextStep)
            startPlayingIntroVideo();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_StartSaving()
    {
        if(m_storageDeviceChanged)
        {
            // come back to the slot screen
            setState(State_ShowingMainMenu_SaveLoad_Root);
            return;
        }

        RAY_GAMEMANAGER->clearWholeGameState();

        MUSICMANAGER->stopAll();// Isap : to fix assert in musicmanager when new games

        if ( RAY_GAMEMANAGER->saveGameState(btrue, m_currentSlotSelected) )
        {
            m_savingProcessEnabled = btrue;
            setState(State_ShowingMainMenu_NewGame_WaitSavingProcess);

            // block menu actions
            UI_MENUMANAGER->disableMenu(MAINMENU_FRIENDLY);
        }
        else
        {
            setState(State_ShowingMainMenu_SaveLoad_Root);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_Action()
    {
        if(SAVEGAME_ADAPTER->IsSaveSystemEnable())
        {
            setState(State_ShowingMainMenu_NewGame_StartSaving);
        }
        else
        {
            // No Save during this mode
            RAY_GAMEMANAGER->clearWholeGameState();
            RAY_GAMEMANAGER->setWMCurrentWorld(RAY_GAMEMANAGER->getTemplate()->getWmStartNode(), s_HomeMap);
            setState(State_ShowingMainMenu_NewGame_WaitPrologueLoad);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_NewGame_WaitSaved()
    {
        if (!SAVEGAME_ADAPTER->hasPendingOperation())
        {
            if (SAVEGAME_ADAPTER->getLastAsyncErrorCode()==Adapter_Savegame::Error_Ok)
                setState(State_ShowingMainMenu_NewGame_WaitSavedNotificationCleared);
            else
                setState(State_ShowingMainMenu_SaveLoad_Root);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_SigninChange_WaitPendingStorageOperation()
    {
        if (!SAVEGAME_ADAPTER->hasPendingOperation())
        {
            UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getGameMenuPriority());
            setState(State_ShowingTitleScreen);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_Load_Error()
    {
        TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_ErrorDuringLoad);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::enter_Exited()
    {
        UIMenuManager::MenuPriority prio = GAMEINTERFACE->getGameMenuPriority();
        if (UI_MENUMANAGER->isDisplayingPriority(prio))
            UI_MENUMANAGER->hideMenu(prio);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_MainMenu::onUserSignedInOut(u32 _playerIndex, bbool _signedIn, bbool _isOnlineStatusChange)
    {
        // we don't care about this status
        if(_isOnlineStatusChange)
            return bfalse;

        bbool res = (m_state!=State_ShowingPressStart);
#ifdef ITF_SUPPORT_NETWORKSERVICES
        if (!NETWORKSERVICES || !CONFIG->m_enableNetwork)
        {
            LOG("[NetworkServices] Not init or disabled!");
            return res;
        }

        //if (!_isOnlineStatusChange && (m_state!=State_ShowingPressStart))
        {
            /*
            1 Connect 4 controllers
            2 Boot the title
            3 On the title screen, a user with a gamer profile A signed-in to Q1 (quadrant in the top left corner) presses Start and becomes the main profile
            4 The gamer profile A disconnects his controller
            Result: The 'Please reconnect a controller' Xbox system UI is triggered

            At this point we have 3 different cases with the following behaviors:

            A. The user with the gamer profile A reconnects his controller or connects a new controller
            Result: the gamer profile A is automatically signed in to Q1 on the newly connected controller

            B. The user with the gamer profile A takes the 2nd, 3rd or 4th controller and signs in the gamer profile A via the Xbox Guide
            Result: the message 'A sign-in change occurred. You are now being returned to the main menu' is triggered and the game reboots to the main menu.

            C. If a gamer profile B, C or D is already signed-in to the 2nd, 3rd or 4th controller (Q2, Q3, Q4), the user with the gamer profile A signs-out one of the gamer profiles via the Xbox Guide and signs-in the gamer profile A again
            Result: the message 'A sign-in change occurred. You are now being returned to the main menu' is triggered and the game reboots to the main menu.
            */

            NetworkServices::User newUser;
            NETWORKSERVICES->getUserFromControllerIndex(_playerIndex, &newUser) ;
            if (getPlayerIndex()==_playerIndex)
            {
                if ( !_signedIn )
                {
                    if(m_state == State_ShowingMainMenu_NewGame_PlayingVideo)
                    {
                        stopPlayingIntroVideo();
                        // remove black screen
                        m_world->setAllowDraw(btrue);

                        // Start menu music
                        const StringID menu = ITF_GET_STRINGID_CRC(Menu,105712373);
                        MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY,menu,0.0f,1.0f);
                    }

                    setState(State_SigninChange_WaitPendingStorageOperation);
                    m_firstEntryInPlayMenu = btrue;

                    if(REWARD_ADAPTER)
                        REWARD_ADAPTER->setMainPlayerIndexIsInvalid();

                    if(TRC_ADAPTER && TRC_ADAPTER->isDisplayingError())
                        TRC_ADAPTER->killCurrentMessage();

                    if(SAVEGAME_ADAPTER->hasPendingOperation())
                        SAVEGAME_ADAPTER->cancelAllOperations();
                }
                else if ( !SAVEGAME_ADAPTER->IsSaveSystemEnable() )
                {
                    // Allow him to save game
                    SAVEGAME_ADAPTER->UseSaveSystem();

                    if(m_state == State_ShowingMainMenu_NewGame_PlayingVideo)
                    {
                        stopPlayingIntroVideo();
                        // remove black screen
                        m_world->setAllowDraw(btrue);

                        // Start menu music
                        const StringID menu = ITF_GET_STRINGID_CRC(Menu,105712373);
                        MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY,menu,0.0f,1.0f);
                    }

                    setState(State_SigninChange_WaitPendingStorageOperation);
                }

                // -------------- Case A -----------------------
                else if ( _signedIn )
                {
                    if(m_validUser && !m_validUser->isValid)
                        NETWORKSERVICES->getUserFromControllerIndex(getPlayerIndex(), m_validUser);

                    res = bfalse;
                }
            } else
            {
                // Check profile
                if( NETWORKSERVICES->compareUsers( &newUser, m_validUser ) )
                {
                    // A Valid Profile is now connected  (same account, but different controller)
                    GAMEMANAGER->setMainIndexPlayer(_playerIndex);

                }
                else
                {
                    res = bfalse;
                }
                updatePlayerIndex();
                // ----------------------------------------------
            }
        }
#endif //ITF_SUPPORT_NETWORKSERVICES
        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update()
    {
#ifdef ITF_CTR
        if (!m_currentlyExiting)
        {
            m_currentlyExiting = btrue;
            Path path("Scene/Production/1_2_CloudGate/1_2_CloudGate_CTR.isc");
            GAMEMANAGER->loadGameplayMap(path, btrue, bfalse);
        }
#else

        switch (m_state)
        {
        case State_WaitMapLoaded:                               update_WaitMapLoaded(); break;
        case State_ShowingTitleScreen:                          update_ShowingTitleScreen(); break;
        case State_ShowingPressStart:                           update_ShowingPressStart(); break;
        case State_PressingStart:                               update_PressingStart(); break;
        case State_ShowingMainMenu_SaveLoad_Root:               update_ShowingMainMenu_LoadSave_Root(); break;
        case State_ShowingMainMenu_SaveLoad_WaitingForAction:   update_ShowingMainMenu_SaveLoad_WaitingForAction(); break;
        case State_ShowingMainMenu_Load_Action:                 update_ShowingMainMenu_Load_Action(); break;
        case State_ShowingMainMenu_Load_StartLoading:           update_ShowingMainMenu_Load_StartLoading(); break;
        case State_ShowingMainMenu_Load_WaitLoaded:             update_ShowingMainMenu_Load_WaitLoaded(); break;
        case State_ShowingMainMenu_NewGame_Action:              update_ShowingMainMenu_NewGame_Action(); break;
        case State_ShowingMainMenu_NewGame_StartSaving:         update_ShowingMainMenu_NewGame_StartSaving(); break;
        case State_ShowingMainMenu_NewGame_WaitSavingProcess:   update_ShowingMainMenu_NewGame_WaitSavingProcess(); break;
        case State_ShowingMainMenu_NewGame_WaitPrologueLoad:    update_ShowingMainMenu_NewGame_WaitPrologueLoad(); break;
        case State_ShowingMainMenu_NewGame_Ready:               update_ShowingMainMenu_NewGame_Ready(); break;
        case State_ShowingMainMenu_SaveLoad_DeleteSave:         update_ShowingMainMenu_SaveLoad_DeleteSave(); break;
        case State_ShowingMainMenu_SaveLoad_DeletingSave:       update_ShowingMainMenu_SaveLoad_DeletingSave(); break;
        case State_ShowingMainMenu_SaveLoad_DeleteRefreshing:   update_ShowingMainMenu_SaveLoad_DeleteRefreshing(); break;
        case State_SigninChange_WaitPendingStorageOperation:    update_SigninChange_WaitPendingStorageOperation(); break;
        case State_ShowingMainMenu_Load_Error:                  update_ShowingMainMenu_Load_Error(); break;
        }
#endif

        updatePadStatus();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::updatePadStatus()
    {
        if ( getPlayerIndex() == U32_INVALID)
            return ;

        if (m_state > State_ShowingPressStart
            && m_state != State_ShowingMainMenu_NewGame_PlayingVideo
            && m_state != State_ShowingMainMenu_NewGame_Ready
            && UI_MENUMANAGER->isEnable(MAINMENU_FRIENDLY)
            && UI_MENUMANAGER->getCurrentMenuID() == MAINMENU_FRIENDLY)
        {
            EContextIcon LeftIcon = ContextIcon_Invalid;
            UIMenu* pUIMenu = UI_MENUMANAGER->getMenu(MAINMENU_FRIENDLY);
            if(pUIMenu)
            {
                UIComponent* pUIComponent = pUIMenu->getUIComponentSelected();
                if(pUIComponent &&
                    updateSelectedSlot (pUIComponent->getID()))
                {
                    ITF_ASSERT(m_currentSlotSelected<m_showLoadMenuEntry.size());
                    if (m_currentSlotSelected<m_showLoadMenuEntry.size())
                    {
                        if( (TRC_ADAPTER && TRC_ADAPTER->isDisplayingError()) || !SAVEGAME_ADAPTER->IsSaveProcessEnable() )
                        {
                            LeftIcon = ContextIcon_Invalid;
                        }
                        else
                        {
                            if(m_wasDisplayingError)
                                m_wasDisplayingError = bfalse;
                            else
                                LeftIcon = m_showLoadMenuEntry[m_currentSlotSelected] ? ContextIcon_Delete : ContextIcon_Invalid;
                        }
                    }
                }
            }

            CONTEXTICONSMANAGER->show(LeftIcon, ContextIcon_Select);
        }
        else
        {
            CONTEXTICONSMANAGER->hide();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::close()
    {
        if(TRC_ADAPTER && TRC_ADAPTER->isDisplayingError())
            TRC_ADAPTER->killCurrentMessage();

        if (CONTEXTICONSMANAGER->isVisible())
        {
            CONTEXTICONSMANAGER->hide();
        }

        SAVEGAME_ADAPTER->setCurrentOperationCompleteCallback(NULL);

        if(m_state == State_ShowingMainMenu_NewGame_Ready || m_rollingDemoIsPlaying)
        {
            stopPlayingIntroVideo();// will stop rolling demo video too
        }

        setState(State_Exited);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onPressStartAction()
    {
        if(m_state!=State_ShowingPressStart)
            return;

        if(m_firstEntryInPlayMenu)
            m_firstEntryInPlayMenu = bfalse;


        updatePlayerIndex();
#ifdef ITF_SUPPORT_NETWORKSERVICES
        if (NETWORKSERVICES && CONFIG->m_enableNetwork)
        {
            if (!m_validUser)
            {
                m_validUser = newAlloc(mId_Gameplay, NetworkServices::User);
            }

            NETWORKSERVICES->getUserFromControllerIndex(getPlayerIndex(), m_validUser);
        }
#endif // ITF_SUPPORT_NETWORKSERVICES

        // Last play time already calculated and logged in onWorldLoaded

        SAVEGAME_ADAPTER->preLoad(getPlayerIndex(), 1, 1);
        if(TRC_ADAPTER)
            TRC_ADAPTER->resetLastErrorContext();
        setState(State_ShowingMainMenu_SaveLoad_Root);

        if(REWARD_ADAPTER)
            REWARD_ADAPTER->setMainPlayerIndex(getPlayerIndex());

        // Game mode (only the allowed extension)
        INPUT_ADAPTER->setButtonModeStatus(InputAdapter::ExtensionOnly);

        GAMEMANAGER->updateRichPresenceForPlayer(PRESENCE_INTHEMENUS, getPlayerIndex());
        fillSaveGameSlots(btrue);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onMenuItemAction (UIComponent* _UIComponent)
    {
        const StringID id = _UIComponent->getID();
        const StringID pressStart = ITF_GET_STRINGID_CRC(PRESSSTART, 3883472400);
        const StringID pressOptions = ITF_GET_STRINGID_CRC(Options, 3527952213);
        const StringID pressControlsRemapping = ITF_GET_STRINGID_CRC(controls, 1174371653);
        const StringID quitGame = ITF_GET_STRINGID_CRC(QUITGAME,4257843919);
        const StringID pressUbiconnect = ITF_GET_STRINGID_CRC(ubiconnect_button, 3233676773);
        const StringID pressNew1Button = ITF_GET_STRINGID_CRC(news1_button, 1178697612);
        const StringID pressNew2Button = ITF_GET_STRINGID_CRC(news2_button, 4067685073);
        if(m_state==State_Exited || m_state==State_ShowingMainMenu_Load_WaitLoaded || m_state==State_ShowingMainMenu_NewGame_PlayingVideo || m_state==State_PressingStart)
            return;

        if(TRC_ADAPTER && TRC_ADAPTER->isDisplayingError())
            return;

        // Action received ?
        if(m_rollingDemoIsPlaying)
        {
            EventPlayMovie ev;
            ev.setPlay(bfalse);
            EVENTMANAGER->broadcastEvent (&ev);
            return;
        }

        if (id == pressStart)
        {
            onPressStartAction();
        }
        else if (id == pressOptions)
        {
            m_optionMenuHelper.activateForOptionMenu(this);
        }
        else if (id == pressControlsRemapping)
        {
            m_controlsRemappingMenuHelper.activateForControlsRemappingMenu(this);
        }
        else if (id == pressUbiconnect)
        {
            LOG("[UBICONNECT] - Ray_GameScreen_MainMenu: Ubisoft Connect button pressed");
#if defined(ITF_WINDOWS) && defined(ITF_SUPPORT_UPLAY)
            if (UPLAYSERVICE && !UPLAYSERVICE->isOverlayActive())
            {
                UPLAYSERVICE->showOverlay();
            }
#elif defined(ITF_SUPPORT_UBISERVICES)
            if (ONLINE_ADAPTER)
            {
                bool ret = ONLINE_ADAPTER->getSessionService()->launchConnect();
                LOG("[UBICONNECT] - Ray_GameScreen_MainMenu: overlay returned: %d", ret);
            }
#endif
        }
        else if (id == pressNew1Button)
        {
            LOG("[NEWS] - Ray_GameScreen_MainMenu: News1 button pressed");
        }
        else if (id == pressNew2Button)
        {
            LOG("[NEWS] - Ray_GameScreen_MainMenu: News2 button pressed");
        }
        else if (id == quitGame)
        {
            SYSTEM_ADAPTER->requestExit();
        }
        // ----------------------
        if(updateSelectedSlot(id))
        {
            setState(m_showLoadMenuEntry[m_currentSlotSelected] ?
                State_ShowingMainMenu_Load_Action :
                State_ShowingMainMenu_NewGame_Action);
        }

        updatePlayerIndex();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    StringID Ray_GameScreen_MainMenu::onMenuPageAction(UIMenu *_menu, const StringID & _action, const StringID &_defaultAction)
    {
        return UI_MENUMANAGER->onMenuPageAction_DefaultRootMenu(_menu, _action, _defaultAction);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_MainMenu::onMenuItemOtherAction(UIComponent* _UIComponent, const StringID & _action )
    {
        if(!m_showLoadMenuEntry[m_currentSlotSelected])
            return bfalse;
        //
        if (!updateSelectedSlot(_UIComponent->getID()))
            return bfalse;

        if(_action == input_actionID_DeleteSave)
        {
            // Ask for removing the file
            if ( m_state == State_ShowingMainMenu_SaveLoad_WaitingForAction
                && SAVEGAME_ADAPTER->IsSaveProcessEnable() )
            {
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Sav_AskForDelete);
                setState(State_ShowingMainMenu_SaveLoad_WaitingForDeleteAnswer);
                return btrue;
            }
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_SaveLoad_DeleteSave()
    {
        RAY_GAMEMANAGER->clearWholeGameState();
        if (RAY_GAMEMANAGER->saveGameState(btrue, m_currentSlotSelected, btrue))
        {
            setState(State_ShowingMainMenu_SaveLoad_DeletingSave);
            UI_MENUMANAGER->disableMenu(MAINMENU_FRIENDLY); // re-enable by the slot refresh

            // 4499 -> deleting
            LocalisationId id;
            id = 4499;

            fillOneSlot(m_currentSlotSelected, LOCALISATIONMANAGER->getText(id));
        }
        else
        {
            setState(State_ShowingMainMenu_SaveLoad_Root);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_SaveLoad_DeletingSave()
    {
        if(!SAVEGAME_ADAPTER->hasPendingOperation())
        {
            // Deleted ?
            setState(State_ShowingMainMenu_SaveLoad_DeleteRefreshing);
            SAVEGAME_ADAPTER->preLoad(getPlayerIndex());
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingMainMenu_SaveLoad_DeleteRefreshing()
    {
        if(!SAVEGAME_ADAPTER->hasPendingOperation())
        {
            // Reset pending message operation
            if(TRC_ADAPTER) TRC_ADAPTER->resetLastErrorContext();
            setState(State_ShowingMainMenu_SaveLoad_Root);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_MainMenu::updateSelectedSlot( const StringID& _id )
    {
        u32 newSlot = U32_INVALID;
        if (_id == ITF_GET_STRINGID_CRC(SLOT_1, 1889828865))
        {
            newSlot = 0;
        }
        else if (_id == ITF_GET_STRINGID_CRC(SLOT_2, 539726452))
        {
            newSlot = 1;
        }
        else if (_id == ITF_GET_STRINGID_CRC(SLOT_3, 2254922994))
        {
            newSlot = 2;
        }

        if(newSlot != U32_INVALID)
        {
            m_currentSlotSelected = newSlot;
            return btrue;
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onEndOfPlayingVideo()
    {
        if(m_state==State_ShowingMainMenu_NewGame_PlayingVideo)
        {
            setState(State_ShowingMainMenu_NewGame_Ready);
        } else
        {
            // Start menu music
            const StringID menuStringID = ITF_GET_STRINGID_CRC(Menu, 105712373);
            MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY, menuStringID, 0.0f, 1.0f);

            // RollingDemo stopped
            m_rollingDemoIsPlaying = bfalse;
            GAMEMANAGER->setInputModeForMenu(btrue);

#ifdef ITF_WII
            m_world->setAllowDraw(btrue);
#endif // ITF_WII
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onStartOfPlayingVideo()
    {
        MUSICMANAGER->stop(METRONOME_TYPE_GAMEPLAY,0.5f);
        if(m_state!=State_ShowingMainMenu_NewGame_PlayingVideo)
        {
            // RollingDemo
            m_rollingDemoIsPlaying = btrue;
            GAMEMANAGER->setInputModeForMenu(bfalse);
        }

#ifdef ITF_WII
        m_world->setAllowDraw(bfalse);
#endif // ITF_WII
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onPauseOfPlayingVideo()
    {
        // Video is paused in order to clause it
        onEndOfPlayingVideo();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::onEndOfFadeIn()
    {
        if( m_state==State_ShowingMainMenu_NewGame_PlayingVideo )
        {
            m_world->setAllowDraw(bfalse);
            UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getGameMenuPriority());
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::update_ShowingTitleScreen()
    {
        if(SYSTEM_ADAPTER->getTime() - m_timeStartingToWait >= m_timeToWaitBeforeStartScreen)
        {
            UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), STARTMENU_FRIENDLY, btrue, this);
            setState(State_ShowingPressStart);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_MainMenu::canOpenMovie()
    {
#ifdef ITF_WII
        if(TRC_ADAPTER->isDisplayingError()) return bfalse;
#endif

        return ( m_state == State_ShowingPressStart );
    }

    void Ray_GameScreen_MainMenu::onMapLoaded( ObjectRef _world )
    {
        Super::onMapLoaded(_world);


        //we went through a classic loading screen. do a fade.
        if(!m_wasPreloaded )
        {
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());
            RAY_GAMEMANAGER->stopFade(bfalse);
            //World * world = (World*)_world.getObject();
            //u32 sceneCount = world->getSceneCount();
            //for (u32 i = 0 ; i < sceneCount ; ++i)
            //{
            //    Scene * pScene = world->getSceneAt(i);
            //    if ( pScene && pScene->getMusicTheme() != StringID::Invalid )
            //        RAY_GAMEMANAGER->setMusicTheme(pScene->getMusicTheme());
            //}
        }



    }

    ITF::bbool Ray_GameScreen_MainMenu::checkSignInOutChanges()
    {
        if (m_playerSignedInOut)
        {
            if (!m_currentlyExiting)
            {
                m_currentlyExiting = btrue;
                RAY_GAMEMANAGER->goToTheMainMenu();
            }
        }
        return m_currentlyExiting;

    }

    bbool Ray_GameScreen_MainMenu::canDisplayTRCMessages()
    {
        if(m_state==State_ShowingMainMenu_NewGame_PlayingVideo || m_state==State_ShowingMainMenu_NewGame_Ready || m_state==State_Exited)
            return bfalse;
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_MainMenu::calculateAndLogLastPlayTime()
    {
        if(!RAY_GAMEMANAGER)
        {
            m_shouldShowWarningBoot = btrue;
            return;
        }

        f32 lastPlayTimeFloat = RAY_GAMEMANAGER->getGameOptionManager().getFloatOption(LAST_PLAY_TIME, 0.0f);
        if(lastPlayTimeFloat == 0.0f)
        {
            LOG("[MainMenu] Init - First time playing (lastPlayTime = 0.0)");
            m_shouldShowWarningBoot = btrue;
        }
        else
        {
            const f64 currentTime = SYSTEM_ADAPTER->getEpochSeconds();
            const f64 lastPlayTime = (f64)lastPlayTimeFloat;
            f64 deltaSeconds = currentTime - lastPlayTime;
            if (deltaSeconds < 0.0) deltaSeconds = 0.0;

            const u32 secondsPerMinute = 60u;
            const u32 secondsPerHour = 60u * secondsPerMinute;
            const u32 secondsPerDay = 24u * secondsPerHour;

            const u32 days = (u32)(deltaSeconds / (f64)secondsPerDay);
            f64 remainder = deltaSeconds - (f64)days * (f64)secondsPerDay;
            const u32 hours = (u32)(remainder / (f64)secondsPerHour);
            remainder -= (f64)hours * (f64)secondsPerHour;
            const u32 minutes = (u32)(remainder / (f64)secondsPerMinute);
            remainder -= (f64)minutes * (f64)secondsPerMinute;
            const u32 seconds = (u32)(remainder + 0.5);

            LOG("[MainMenu] Init - Last played %u days, %u hours, %u minutes, %u seconds ago (lastPlayTime: %.2f, currentTime: %.2f)",
                days, hours, minutes, seconds, lastPlayTime, currentTime);
            m_shouldShowWarningBoot = (days >= DAYS_TO_SHOW_WARNING_BOOT);
        }
    }

    bbool Ray_GameScreen_MainMenu::shouldShowWarningBootPopup()
    {
        return m_shouldShowWarningBoot;
    }

    void Ray_GameScreen_MainMenu::updateLastPlayTime()
    {
        if(!RAY_GAMEMANAGER)
            return;
        f64 currentTime = SYSTEM_ADAPTER->getEpochSeconds();
        RAY_GAMEMANAGER->getGameOptionManager().setFloatOption(LAST_PLAY_TIME, (f32)currentTime);
    }

}
