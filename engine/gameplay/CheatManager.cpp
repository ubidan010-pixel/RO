#include "precompiled_engine.h"

#include "zinput/ZInputManager.h"
#include "zinput/ZPad_Base.h"

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_


#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif // _ITF_INPUTADAPTER_H_

#ifndef _ITF_GFXADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFXADAPTER_H_

namespace ITF
{

CheatManager::CheatManager()
: m_timeStepMult(1.f)
, m_pause(bfalse)
, m_pauseStep(bfalse)
, m_debugCharacters(bfalse)
, m_isActive(bfalse)
, m_currentCheckpointIndex(0)
, m_enableInvincibleEnemies(bfalse)
, m_resetCharacterDebugTrajectories(0)
, m_playerInvincible(bfalse)
, m_canListenActions(bfalse)
, m_frameWaited(bfalse)
, m_listenerRegistered(bfalse)
{
}

void CheatManager::init( const CommandArgs& _args )
{
    setActive(CONFIG->m_startWithCheatsOn);
    //teleportPlayersToCurrentCheckpoint();
	INPUT_ADAPTER->addListener(this, 0);
}
float CheatManager::getPauseStepDt()
{
    return LOGICDT;
}

void CheatManager::IncrTimeStepMult()
{
    m_timeStepMult = Min(m_timeStepMult+0.1f,3.f);
}

void CheatManager::DecrTimeStepMult()
{
    m_timeStepMult = Max(m_timeStepMult-0.1f,0.1f);
}

void CheatManager::setPause( bbool _val )
{
    m_pause = _val;
    m_pauseStep = bfalse;
    GFX_ADAPTER->setPause(_val);

    GAMEMANAGER->pauseSounds(_val);
}

void CheatManager::update(f32 _dt)
{
#ifdef ITF_SUPPORT_CHEAT
    if (!m_isActive)
    {
        return;
    }

    if ( m_resetCharacterDebugTrajectories )
    {
        m_resetCharacterDebugTrajectories -= 1;
    }

    if(!m_listenerRegistered)
    {
        GAMEMANAGER->addCheatInputListener(this);
        m_listenerRegistered=btrue;
    }

    if(m_frameWaited)
    {
        m_canListenActions = bfalse;
        m_frameWaited = bfalse;
    }
    else if(m_canListenActions)
    {
        m_frameWaited = btrue;
    }

#endif //ITF_SUPPORT_CHEAT
}

void CheatManager::setActive(bbool _active)
{
    m_isActive = _active;
}

bbool CheatManager::onKey( i32 _key, InputAdapter::PressStatus _keyStatus )
{
    if (_keyStatus == InputAdapter::Pressed)
    {
        switch (_key)
        {
        case 'D':
            {
                if( INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) &&
                    INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) )
                {
                    m_resetCharacterDebugTrajectories = 2;
                }
            }
            break;
            case 'L':
                {
                    GAMEMANAGER->getInputManager()->SetActionRemap(0, ZInputManager::Action_Hit, ZPad_Base::BUTTON_FACE_SOUTH);
                }
            break;
        }
    }

    return btrue;
}

bbool CheatManager::onMouseButton( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
{
#ifdef ITF_SUPPORT_CHEAT
    if (_but != InputAdapter::MB_Middle) { return btrue; }
    if (_status != InputAdapter::Pressed) { return btrue; }

    if (!m_isActive) { return btrue; }
    if (UI_MENUMANAGER->getIsInDBGMenu()) { return btrue; }
    const bbool shift = INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) || INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT);
    if (!shift) { return btrue; }
    i32 mx = 0, my = 0;
    INPUT_ADAPTER->getMousePos(mx, my);
    const u32 playerIndex = GAMEMANAGER->getMainIndexPlayer();
    Actor* actor = GAMEMANAGER->getActivePlayer(playerIndex);
    if (!actor) { return btrue; }
    const f32 sceneZ = actor->getPos().m_z;
    Vec3d worldPos;
    GFX_ADAPTER->compute2DTo3D(Vec2d((f32)mx, (f32)my), sceneZ, worldPos);
    GAMEMANAGER->teleportPlayer(playerIndex, worldPos.truncateTo2D(), worldPos.m_z, btrue);
#endif // ITF_SUPPORT_CHEAT
    return btrue;
}

void CheatManager::teleportToCheckpoint( GameManager::teleportTarget _target )
{
    switch(_target)
    {
    case GameManager::NextCheckPoint:
        GAMEMANAGER->teleportToNextCheckpoint(btrue);
        break;

    case GameManager::PrevCheckPoint:
        GAMEMANAGER->teleportToNextCheckpoint(bfalse);
        break;

    case GameManager::FirstCheckPoint:
        GAMEMANAGER->teleportToFirstCheckpoint(bfalse);
        break;

    case GameManager::LastCheckPoint:
        GAMEMANAGER->resetToLastCheckpoint();
        break;

    default:
        ITF_ASSERT(0);
        break;
    }
}

void CheatManager::Receive( u32 deviceID /* player */, f32 axis, const StringID& action )
{
#ifdef ITF_SUPPORT_CHEAT
    #define s_CheatPause ITF_GET_STRINGID_CRC(CHEAT_PAUSE,313203915)
    #define s_ClearWarnings ITF_GET_STRINGID_CRC(CHEAT_CLEARWARNING,2279822752)
    #define s_MaxDeZoom ITF_GET_STRINGID_CRC(CHEAT_MAXDEZOOM,2365869674)
    #define s_Zoom ITF_GET_STRINGID_CRC(CHEAT_ZOOM,2616823591)

    #define s_CheatTeleporteNext ITF_GET_STRINGID_CRC(CHEAT_TELEPORTE_NEXT,1622343128)
    #define s_CheatTeleportePrev ITF_GET_STRINGID_CRC(CHEAT_TELEPORTE_PREV,3554097641)
    #define s_CheatTeleporteFirst ITF_GET_STRINGID_CRC(CHEAT_TELEPORTE_FIRST,2800949481)
    #define s_CheatTeleporteLast ITF_GET_STRINGID_CRC(CHEAT_TELEPORTE_LAST,194895442)

    #define s_CheatActive ITF_GET_STRINGID_CRC(CHEAT_ACTIVE,3130787906)

    if(action==s_CheatActive)
    {
        m_canListenActions = btrue;
    }

    if ( ! getCanListenActions()  )
        return ;

    if(action==s_CheatPause)
    {
        togglePause();
    }
    else if(action==s_ClearWarnings)
    {
#ifndef ITF_FINAL
        WarningManager::clear();
#endif
    }
    else if(action==s_MaxDeZoom)
    {
        CAMERACONTROLLERMANAGER->switchCheatDezoomMaxWithPad();
    }
    else if(action==s_Zoom)
    {
        f32 zoom = axis;

        f32 zoomIncrement = 0.0f;
        if (zoom < -0.5f)
            zoomIncrement = 0.1f;
        else if (zoom > 0.5f)
            zoomIncrement = -0.1f;

        CAMERACONTROLLERMANAGER->setCheatZoom(zoomIncrement);
    }

    // CHECKPOINTS
    if (!UI_MENUMANAGER->getIsInDBGMenu() )
    {
        if(action==s_CheatTeleporteNext)
            teleportToCheckpoint(GameManager::NextCheckPoint);
        else if(action==s_CheatTeleportePrev)
            teleportToCheckpoint(GameManager::PrevCheckPoint);
        else if(action==s_CheatTeleporteFirst)
            teleportToCheckpoint(GameManager::FirstCheckPoint);
        else if(action==s_CheatTeleporteLast)
            teleportToCheckpoint(GameManager::LastCheckPoint);
    }

    onReceive(deviceID, axis, action);
#endif //ITF_SUPPORT_CHEAT
}

CheatManager::~CheatManager()
{
    if (INPUT_ADAPTER)
    {
        INPUT_ADAPTER->removeListener(this);
    }
    if(m_listenerRegistered)
    {
        if (GAMEMANAGER_VALID)
        {
            GAMEMANAGER->removeInputListener(this);
        }
        m_listenerRegistered=bfalse;
    }
}

}
