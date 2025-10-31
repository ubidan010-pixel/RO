#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHECKPOINTSCORE_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_CheckpointScore.h"
#endif //_ITF_RAY_CHECKPOINTSCORE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#define SCOREBAORD_FRIENDLY "ScoreRecap"

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_CheckpointScore)
    //
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_CheckpointScore::Ray_GameScreen_CheckpointScore()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_CheckpointScore::~Ray_GameScreen_CheckpointScore()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_CheckpointScore::init()
    {
        RAY_GAMEMANAGER->enableBaseScene(btrue);
        showScores(btrue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_CheckpointScore::onWorldLoaded()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_CheckpointScore::update()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_CheckpointScore::close()
    {
        showScores (bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_CheckpointScore::canPause()
    {
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_CheckpointScore::showScores(bbool _isVisible)
    {
        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getGameMenuPriority(), SCOREBAORD_FRIENDLY, _isVisible, this);
    }
}
