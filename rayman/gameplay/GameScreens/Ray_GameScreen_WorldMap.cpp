#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerComponent.h"
#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_WM_LEADABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_LeadAbility.h"
#endif //_ITF_RAY_WM_LEADABILITY_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_RAY_ELECTOONTEETHCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ElectoonTeethComponent.h"
#endif //_ITF_RAY_ELECTOONTEETHCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#define WORLDMAPMENU_FRIENDLY "menuWorldMap"
#define HOMEMAPMENU_FRIENDLY "menuHomeMap"

#define PADCONFIG_FRIENDLY "padConfig_PS3"


namespace ITF {

IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_WorldMap)

//------------------------------------------------------------------------------
Ray_GameScreen_WorldMap::Ray_GameScreen_WorldMap()
    : Super()
    , m_firstFrame(btrue)
    , m_joinLeaveGameHandler()
    , m_preLoadedMap(ObjectRef::InvalidRef)
    , m_musicThemeSet(bfalse)
    , m_showingControls(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_GameScreen_WorldMap::~Ray_GameScreen_WorldMap()
{
    GAMEMANAGER->setInputModeForGamePlay(bfalse);
}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::init()
{
    RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());
    InitializationParameters *params = (InitializationParameters*)getInitParams();

    if ( params && params->m_worldRef.isValid() )
    {
        m_world = static_cast<World*>(params->m_worldRef.getObject());
        //RAY_GAMEMANAGER->loadMap(params->m_worldRef, RAY_GAMEMANAGER->getWorldMap(), btrue, bfalse, btrue);
        m_prefetchStarted = btrue;
    }
    else
    {
        ObjectRef rWorld = RAY_GAMEMANAGER->createMap();
        ITF_ASSERT(rWorld.isValid());

        m_world = (World*)GETOBJECT(rWorld);
         ITF_ASSERT(m_world!=NULL);
 
         Vector<ObjectPath> prefetchTargets;
 
         ObjectPath prefetchTarget = RAY_GAMEMANAGER->getWMCurrentLevelObjectPath();
         if (!prefetchTarget.isValid())
         {
             prefetchTarget = RAY_GAMEMANAGER->getWMCurrentWorldObjectPath();
         }

         if (!prefetchTarget.isValid())
         {
             if (const Ray_GameManagerConfig_Template* configTemplate = RAY_GAMEMANAGER->getTemplate())
             {
                 prefetchTarget = configTemplate->getWmStartNode();
             }
         }

         if (prefetchTarget.isValid())
         {
             prefetchTargets.push_back(prefetchTarget);
         }
 
         m_world->setPrefetchTargets(prefetchTargets);
 
         RAY_GAMEMANAGER->loadMap(rWorld, RAY_GAMEMANAGER->getWorldMap(), btrue, bfalse, btrue, GameManager::LEVEL_NAME_INITIALISATION_TYPE_SET);
         m_prefetchStarted = bfalse;
        m_musicThemeSet = bfalse;
    }


    SAVEGAME_ADAPTER->setEnableStatutCallback(saveSystemEnableStatutCallback);
    GAMEMANAGER->setInputModeForGamePlay(btrue);
    RAY_GAMEMANAGER->resetPlayerGlobalPowers();

    m_firstFrame = btrue;

}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::saveSystemEnableStatutCallback(bbool _enable)
{
    // force savegame item flag
    UI_MENUMANAGER->changeMenuItemActiveFlag(WORLDMAPMENU_FRIENDLY, "savegame", _enable);
}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::onWorldLoaded()
{
    RAY_GAMEMANAGER->spawnPlayerActors(GetClassCRCStatic());
    RAY_GAMEMANAGER->enableBaseScene(btrue);

    //update counts

    for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); ++i )
    {
        Player* player = GAMEMANAGER->getPlayer(i);
        if (player)
        {
            GAMEMANAGER->changePlayerGameMode(player, RAY_GAMEMODE_WORLDMAP);
        }
    }

    GAMEMANAGER->updateRichPresenceForActivePlayers(PRESENCE_GAMESCREEN_INWORLDMAP);
    RAY_GAMEMANAGER->stopFade(bfalse);
    if (RAY_GAMEMANAGER->isInPause())
    {
        RAY_GAMEMANAGER->leavePause(bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::update()
{
    if (m_prefetchStarted)
    {
        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
        RAY_GAMEMANAGER->switchToPreloadedMap(m_world->getRef());
        m_prefetchStarted = bfalse;
    }
    }
    else
    {
        if(!m_isWorldLoaded && m_world && !m_world->isSceneAsyncLoadRunning())
        {
            if ( !m_musicThemeSet )
            {
                    RAY_GAMEMANAGER->setMusicTheme(ITF_GET_STRINGID_CRC(WorldMap,1734041925));
                m_musicThemeSet = btrue;
            }
            else if ( m_world->isPhysicalReady() )
            {
                //m_world->stopPrefetch();
                m_isWorldLoaded = btrue;
            }
            else
                m_isWorldLoaded = bfalse;
        }

        if (!m_isWorldLoaded)
        {
            return;
        }

        checkSignInOutChanges();

        if (m_currentlyExiting)
            return;

        m_joinLeaveGameHandler.update();

        if (!m_firstFrame)
        {
            // in home map?
            Player* mainPlayer = RAY_GAMEMANAGER->getPlayer(RAY_GAMEMANAGER->getMainIndexPlayer());
            bbool inHomeMap = mainPlayer && (RAY_GAMEMODE_PLATFORMER == mainPlayer->getCurrentMode());

            // in level selection?
            bbool onLevel = RAY_GAMEMANAGER->getWMCurrentLevelObjectPath().isValid();

            // land of the dead is on word selection, don't display back button (fix for RO-17110)
            if (RAY_GAMEMANAGER->getWMCurrentLevelTag() == s_DE_A1)
            {
                onLevel = bfalse;
            }

            // main world map player moving?
            bbool isMoving = bfalse;
            Ray_WM_SpotComponent* standSpot = NULL;
            if (mainPlayer && (RAY_GAMEMODE_WORLDMAP == mainPlayer->getCurrentMode()))
            {
                if (Actor* mainPlayerActor = mainPlayer->getCurrentActor().getActor())
                {
                    if (Ray_WM_ControllerComponent* controller = mainPlayerActor->GetComponent<Ray_WM_ControllerComponent>())
                    {
                        isMoving = controller->isMoving();
                        standSpot = controller->getLeadAbility().getStandSpot();
                    }
                }
            }

            EContextIcon contextIconControls = ContextIcon_Controls;
#ifndef ITF_PS3
            contextIconControls = ContextIcon_Invalid;
#endif //ITF_PS3

            if (!UIMenuManager::IsBaseMenuHelper())
            {
                // show the proper context icons
                if (inHomeMap)
                {
                    // home map: no icon
                    CONTEXTICONSMANAGER->hide();
                }
                else if ( m_showingControls )
                {
                    // standing
                    CONTEXTICONSMANAGER->show(ContextIcon_Back, ContextIcon_Invalid);
                }
                else if (RAY_GAMEMANAGER->isPlayingWorldMapUnlockSequenceAndDisplayingUnlocks())
                {
                    // unlock pop-up: continue only
                    CONTEXTICONSMANAGER->show(ContextIcon_Invalid, ContextIcon_Continue);
                }
                else if (onLevel)
                {
                    if (isMoving)
                    {
                        // moving: no entering
                        CONTEXTICONSMANAGER->show(ContextIcon_RayHome, ContextIcon_Invalid, ContextIcon_Back);
                    }
                    else if (standSpot && (standSpot->getState()!=SPOT_STATE_CANNOT_ENTER) && !standSpot->isMid())
                    {
                        // open stand spot: enter displayed
                        CONTEXTICONSMANAGER->show(ContextIcon_RayHome, ContextIcon_Enter, ContextIcon_Back, contextIconControls);
                    }
                    else
                    {
                        // inaccessible stand spot: no enter (RO-16086)
                        CONTEXTICONSMANAGER->show(ContextIcon_RayHome, ContextIcon_Invalid, ContextIcon_Back, contextIconControls);
                    }
                }
                else
                {
                    if (isMoving)
                    {
                        // moving: no entering
                        CONTEXTICONSMANAGER->show(ContextIcon_RayHome, ContextIcon_Invalid);
                    }
                    else
                    {
                        // open spot: enter displayed
                        CONTEXTICONSMANAGER->show(ContextIcon_RayHome, ContextIcon_Enter, ContextIcon_Invalid, contextIconControls);
                    }
                }
            }


#ifdef ITF_PS3
            if( !GAMEMANAGER->isInPause() &&
                CONTEXTICONSMANAGER->isVisible() &&
                !isMoving &&
                !inHomeMap &&
                !RAY_GAMEMANAGER->isPlayingWorldMapUnlockSequenceAndDisplayingUnlocks() &&
                canPause() /* to handle the fade */)
            {
                u32 backButton = m_joyButton_B;
                if(SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
                {
                    backButton = m_joyButton_A;
                }

                for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    if (!SYSTEM_ADAPTER->isOSUIActive() && RAY_GAMEMANAGER->getPlayer(i)->getActiveAndPersistent())
                    {
                        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
                        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua, i, buttons, JOY_MAX_BUT);

                        if (!m_showingControls
                            && (buttons[m_joyButton_Y] == InputAdapter::JustReleased
                            || buttons[m_joyButton_Triangle] == InputAdapter::JustReleased) )
                        {
                            m_showingControls = btrue;
                            showConfig(btrue);
                        }
                        else if (m_showingControls
                            && buttons[backButton] == InputAdapter::JustReleased )
                        {
                            m_showingControls = bfalse;
                            showConfig(bfalse);
                        }
                    }
                }
            }
#endif //ITF_PS3

        }

        m_firstFrame = bfalse;
    }
}
//------------------------------------------------------------------------------
bbool Ray_GameScreen_WorldMap::canPause()
{
    // To avoid to see the worldmap in a strange state (ie some worlds / levels may appear on the scene
    // but they should not as they havn't been discovered yet), don't allow to pause game during fade in / fade out process.
    // It will let enough time for all spots to be initialized correctly and updated at least once.
    // Fixes : http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-17343
    return m_isWorldLoaded
        && RAY_GAMEMANAGER->canPauseWorldMap()
        && RAY_GAMEMANAGER->isFadeOutFinished()
        && RAY_GAMEMANAGER->isFadeInFinished();
}
//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::close()
{
    showPauseMenu(bfalse);
    RAY_GAMEMANAGER->enableBaseScene(bfalse);

    for (u32 i=0; i<RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
    {
        Player* player = RAY_GAMEMANAGER->getPlayer(i);
        if (player)
        {
            RAY_GAMEMANAGER->changePlayerGameMode(player, GAMEMODE_UNKNOWN);
        }
    }

    if (CONTEXTICONSMANAGER->isVisible())
    {
        CONTEXTICONSMANAGER->hide();
    }

    if ( m_showingControls )
    {
        showConfig( bfalse );
        m_showingControls = bfalse;
    }
}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::showPauseMenu(bbool _isVisible)
{
    if (_isVisible)
    {
        Player* mainPlayer = RAY_GAMEMANAGER->getPlayer(RAY_GAMEMANAGER->getMainIndexPlayer());
        bbool inHomeMap = mainPlayer && (RAY_GAMEMODE_PLATFORMER == mainPlayer->getCurrentMode());

        // in world map
        if(!inHomeMap)
        {
            UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), WORLDMAPMENU_FRIENDLY, btrue, this);
        }
        else //in home map
        {
            UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), HOMEMAPMENU_FRIENDLY, btrue, this);
        }

        // Menu options active flag
        {
            UI_MENUMANAGER->changeMenuItemActiveFlag(UI_MENUMANAGER->getCurrentMenuID()
                , "savegame", SAVEGAME_ADAPTER->IsSaveSystemEnable());

            UI_MENUMANAGER->changeMenuItemActiveFlag(UI_MENUMANAGER->getCurrentMenuID()
                , "achievements", (REWARD_ADAPTER && REWARD_ADAPTER->isEnable()));
        }
    }
    else
    {
        if (UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getPauseMenuPriority()))
            UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getPauseMenuPriority());
    }
}

//------------------------------------------------------------------------------
bbool Ray_GameScreen_WorldMap::findSpot(
    TrajectoryNodeComponent* _from,
    TrajectoryNodeComponent::NodeList& _path,
    TrajectoryNodeComponent::NodeList& _visited,
    u32 _recursionCount)
{
    // NB[LaurentCou]: this is a recursive search in a node components network.
    // Getting the first path to a world map spot i.e. a node with a
    // Ray_WM_SpotComponent; not guarenteed to be the shortest one. This enables
    // changing the links between levels on the fly, but could be changed to a
    // static array easily if bringing performance issues.

    // guard against too many recursions
    static const u32 s_maxRecursion = 16;
    if (_recursionCount >= s_maxRecursion)
    {
        ITF_WARNING(_from->GetActor(), bfalse, "Can't find path to spot, too many recursions");
        return bfalse;
    }

    TrajectoryNodeComponent::NodeList candidateNodes;
    _from->getChildNodes(candidateNodes);
    _from->getParentNodes(candidateNodes);
    u32 count = candidateNodes.size();
    for (u32 i=0; i<count; ++i)
    {
        TrajectoryNodeComponent* node = candidateNodes[i];

        // skip if already visited
        u32 index = _visited.find(node);
        if (index != U32_INVALID)
        {
            continue;
        }

        // add to visited nodes
        _visited.push_back(node);

        if (node->GetActor()->GetComponent<Ray_WM_SpotComponent>())
        {
            _path.insert(node, 0);
            return btrue;
        }

        if (findSpot(node, _path, _visited, ++_recursionCount))
        {
            _path.insert(node, 0);
            return btrue;
        }
    }

    ITF_WARNING(_from->GetActor(), bfalse, "Can't find path to spot, no spot found");
    return bfalse;
}

//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::showConfig(bbool _isVisible)
{
    if (_isVisible)
    {
        RAY_GAMEMANAGER->setInputModeForGamePlay(bfalse);
        RAY_GAMEMANAGER->setInputModeForMenu(btrue);

        //We want to pause huds, including any updates.
        Ray_ElectoonTeethComponent* etInstance =Ray_ElectoonTeethComponent::getInstance();
        if (etInstance)
        {
            etInstance->pause();
        }
        Ray_PlayerHudScoreComponent::setVisibility(bfalse);

        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getTutorialMenuPriority(), PADCONFIG_FRIENDLY, btrue, this);
    }
    else
    {
        if (UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getTutorialMenuPriority()))
        {
            Ray_ElectoonTeethComponent* etInstance =Ray_ElectoonTeethComponent::getInstance();
            if (etInstance)
            {
                etInstance->unpause();
            }
            Ray_PlayerHudScoreComponent::setVisibility(btrue);
            RAY_GAMEMANAGER->setInputModeForGamePlay(btrue);
            RAY_GAMEMANAGER->setInputModeForMenu(bfalse);
            UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getTutorialMenuPriority());
        }
    }
}
//------------------------------------------------------------------------------
void Ray_GameScreen_WorldMap::onMenuItemAction(UIComponent* _UIComponent)
{
    if(!_UIComponent) return;
    Ray_Pause_MenuItemActionListener::onMenuItemAction(_UIComponent);
    const StringID id = _UIComponent->getID();
    if (id == ITF_GET_STRINGID_CRC(Options, 3527952213))
    {
        m_optionMenuHelper.activateForOptionMenu(this);
    }
    else if (id == ITF_GET_STRINGID_CRC(controls, 1174371653))
    {
        m_controlsRemappingMenuHelper.activateForControlsRemappingMenu(this);
    }
}

//------------------------------------------------------------------------------
ITF::bbool Ray_GameScreen_WorldMap::checkSignInOutChanges()
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

} // namespace ITF
