#include "precompiled_engine.h"

#if !defined(ITF_FINAL)

#ifndef _ITF_COMMANDHANDLER_H_
#include "engine/debug/CommandHandler.h"
#endif //_ITF_COMMANDHANDLER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_REPORTISSUEUTILS_H_
#include "engine/debug/reportIssueUtils.h"
#endif // _ITF_REPORTISSUEUTILS_H_

#ifndef _ITF_VIDEOCAPTURECOMMAND_H_
#include "engine/misc/videocaptureCommand.h"
#endif //_ITF_VIDEOCAPTURECOMMAND_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/bundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifdef ITF_X360
#ifndef _ITF_XGD3CHECK_H_
#include "core/system/x360/xgd3Check_x360.h"
#endif //_ITF_XGD3CHECK_H_
#endif //ITF_X360



namespace ITF
{

    CommandHandler::CommandHandler()
        : m_restoreSavedActorsPosFrame(0)
        , m_hide2d(0)
        , m_editor(NULL)
        , safeFrameDefaultMode(SF_NOSAFEFRAME)
        , safeFrameUser(safeFrameDefaultMode)
    {
    }

    CommandHandler::~CommandHandler()
    {

    }


    bbool CommandHandler::onMouseButton (InputAdapter::MouseButton _but, InputAdapter::PressStatus _status) { return btrue; }

    bbool CommandHandler::onMousePos (i32 _x, i32 _y) { return btrue; }

    bbool CommandHandler::onMouseWheel (i32 _wheelValue, i32 _wheelDelta) { return btrue; }

    bbool CommandHandler::onKeyProcess  (i32 _key, InputAdapter::PressStatus _keyStatus)
    {
        if (_keyStatus == InputAdapter::Pressed)
        {
            switch (_key)
            {

            case KEY_F5:
                LOG_GUILL_THE_WHITE("KEY_F5");
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    LOG_GUILL_THE_WHITE("reloadCurrentMap");
                    reloadCurrentMap();
                }
                else
                {
                    LOG_GUILL_THE_WHITE("reinitCurrentLevel");
                    loadLastCheckpoint(INPUT_ADAPTER->isKeyPressed(KEY_LALT));                    
                }
                break;


            case KEY_F6:
                {
                    LOG_GUILL_THE_WHITE("KEY_F6");
                    m_hide2d++;
                    m_hide2d &= HIDE2D_MASK;
                    GFX_ADAPTER->setHideFlags(0);
                    if (m_hide2d & HIDE2D_DBG)
                    {
                        LOG_GUILL_THE_WHITE("addHideFlags(GFXAdapter::HideFlag_DBGPrimitive | GFXAdapter::HideFlag_2DBox)");
                        GFX_ADAPTER->addHideFlags(GFXAdapter::HideFlag_DBGPrimitive | GFXAdapter::HideFlag_2DBox);
                    }
                    if (m_hide2d & HIDE2D_2DACTORS)
                    {
                        LOG_GUILL_THE_WHITE("addHideFlags(GFXAdapter::HideFlag_Font)");
                        GFX_ADAPTER->addHideFlags(GFXAdapter::HideFlag_Font);
                    }
                } 
                break;


            case 'P':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    LOG_GUILL_THE_WHITE("P + LCTRL");
                    // In editor    /*requestUHDScreenshot();*/
                    GFX_ADAPTER->requestScreenshotNormalMode();

                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LALT))
                {
                    LOG_GUILL_THE_WHITE("P + LALT");
                    // In editor    /*changeAutoPauseMode();*/
                }
                else
                {
                    LOG_GUILL_THE_WHITE("P");
                    CHEATMANAGER->togglePause();
#ifdef ITF_SUPPORT_DEBUGFEATURE
                    if(CHEATMANAGER->getPause())
                        DEBUGINFO->addDebugInfoDisplay("[CommandHandler] Pause", 1.0f);
                    else
                        DEBUGINFO->addDebugInfoDisplay("[CommandHandler] Unpause", 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE
                }
                break;

            case 'O':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    GFX_ADAPTER->switchOverDrawMode();
                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    // In editor    /*openScene(btrue);*/
                }
                else
                {
                    if ( CHEATMANAGER->getPause() )
                    {
                        CHEATMANAGER->setPauseStep();
                    }
                }
                break;

            case 'C':
                {
                    if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                    {
                        // In editor    /*copySelectionData();*/
                    }
                    else if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                    {
                        CHEATMANAGER->ResetTimeStepMult();
                    }
                }
                break;

/*
            case 'Q':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    CHEATMANAGER->DecrTimeStepMult();
                }
                return bfalse;*/

            case 'W':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    CHEATMANAGER->IncrTimeStepMult();
                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    GFX_ADAPTER->switchWireFrameMode();
                }
                else
                {
                    WarningManager::clear();
                    WarningManager::filterDisplayForObjects(NULL, 0);
                }
                break;

            case 'R':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                    {
                        RASTER_CYCLERASTERGROUP();
                        return bfalse;
                    }
                    else
                    {
                        GFX_ADAPTER->toggleRaster();
                        return bfalse;
                    }
                }
                else
                {
                    // Used to rotate selection
                }
                break;

            case 'J':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    RASTER_DUMP(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT));
                }
                break;

            case 'K':
                {
                    if(INPUT_ADAPTER->isKeyPressed(KEY_LALT))
                    {
                        CAMERACONTROLLERMANAGER->switchCheatDezoomMax();
                    }
                    else
                    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
                        CAMERACONTROLLERMANAGER->switchDebugDrawModifiers();
                        if ( CAMERACONTROLLERMANAGER->isDebugDrawInfo() != CAMERACONTROLLERMANAGER->isDebugDrawModifiers() )
                        {
                            CAMERACONTROLLERMANAGER->switchDebugDrawInfo();
                        }
#endif //ITF_SUPPORT_DEBUGFEATURE
                    }
                }
                break;

            case 'L':
                if (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                {
                    reportIssueUtils::m_investigateMapStuck = btrue;
                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) || INPUT_ADAPTER->isKeyPressed(KEY_RCTRL))
                {
                    View::switchLockAll();
                }
                else
                {
#ifdef ITF_SUPPORT_DEBUGFEATURE
                    LINKMANAGER->cycleDrawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE
                }
                break;
                
#if defined(ITF_SUPPORT_VIDEOCAPTURE)

            case 'I':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    VideoCaptureCommand::stop();   
                }
                break;

            case 'B':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    VideoCaptureCommand::startStepByStep();   
                }
                break;
#endif // ITF_SUPPORT_VIDEOCAPTURE

#ifdef ITF_SUPPORT_CHEAT
            case 'Y':
                {
                    if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                    {
                        File::incSimulateSeekTime(-50);
                    }
                       
                }
                break;
            case 'V':
                {
                    if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                    {
                        File::incSimulateSeekTime(50);
                    }

                }
                break;

#ifdef ITF_X360
            case 'D':
                {
                    if (xgd3Check::exists())
                        xgd3Check::get().injectError(Seeder::GetUnseededU32(0,10));
                }
                break;
#endif //ITF_X360
#endif //ITF_SUPPORT_CHEAT

            case '9':
                {
                    GFX_ADAPTER->switchSafeFrameMode();
                    safeFrameUser = GFX_ADAPTER->getSafeFrameMode();
                    return bfalse;
                }
                break;

            }
       }

       return btrue;
    }

    bbool CommandHandler::onKey (i32 _key, InputAdapter::PressStatus _keyStatus)
    {
        bbool forwardEvent = btrue;
        if(m_editor)
        {
            forwardEvent = m_editor->onKeyProcess(_key, _keyStatus);
        }

        if(forwardEvent)
            onKeyProcess(_key, _keyStatus);

        return btrue;
    }

    /************************************************************************/
    /* Update                                                               */
    /************************************************************************/
    void CommandHandler::update(double _elapsed)
    {
        if (m_restoreSavedActorsPosFrame)
        {
            m_restoreSavedActorsPosFrame--;
            if (!m_restoreSavedActorsPosFrame)
            {
                for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    Actor * actor = GAMEMANAGER->getActivePlayer(i);
                    if (actor)
                        actor->setPos(m_savedActorsPos[i]);
                }
            }
        }
    }


    void CommandHandler::loadLastCheckpoint(bbool _keepPlayersPos)
    {
        if (_keepPlayersPos)
        {
            ITF_ASSERT_CRASH(CDHR_MAX_SUPPORTED_PLAYERS >= GAMEMANAGER->getMaxPlayerCount(), "please increase CDHR_MAX_SUPPORTED_PLAYERS" );
            for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
                if (Actor * actor = GAMEMANAGER->getActivePlayer(i))
                    m_savedActorsPos[i] = actor->getPos();

            m_restoreSavedActorsPosFrame = 2;
        }

        GAMEMANAGER->resetToLastCheckpointAndSave();
        if ( CHEATMANAGER && CHEATMANAGER->getPause() )
        {
            CHEATMANAGER->setPauseStep();
        }
    }

    void CommandHandler::reloadCurrentMap()
    {
        if(GAMEMANAGER->isPlayableScreen())
        {
            const World* pCurrentWorld = CURRENTWORLD;
            if(pCurrentWorld)
            {
                // Stop all music
                MUSICMANAGER->stopAll(0.0f);

                ITF_VERIFY(GAMEMANAGER->loadGameplayMap(pCurrentWorld->getRootScene()->getPath(), btrue, btrue ));
            }
        }
    }

};
#endif
