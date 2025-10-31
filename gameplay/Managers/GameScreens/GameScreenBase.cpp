#include "precompiled_gameplay.h"

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(GameScreenBase)

    ///////////////////////////////////////////////////////////////////////////////////////////
    GameScreenBase::GameScreenBase() : 
        m_world(NULL), 
        m_isWorldLoaded(bfalse),
        m_currentlyExiting(bfalse),
        m_playerSignedInOut(bfalse),
        m_endOfLevelReached(bfalse),
        m_initializationParams(NULL),
        m_prefetchStarted(bfalse),
        m_canPause(btrue),
        m_resourceLoadingPaused(bfalse)

    {
        WorldManager::addListener(this);
    }

        GameScreenBase::~GameScreenBase() 
        {
            WorldManager::removeListener(this);
            SF_DEL(m_initializationParams);
        }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameScreenBase::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
    {
        if (_bSwitchDone && m_world==_pWorld && _pWorld)
        {
            m_isWorldLoaded = btrue;
            onWorldLoaded();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameScreenBase::onDeleteWorld(World* _pWorld)
    {
        ITF_ASSERT(_pWorld);
        if (m_world==_pWorld)
        {
            if ( m_resourceLoadingPaused )
                RESOURCE_MANAGER->unpause();
            m_isWorldLoaded = bfalse;
        }
    }

    void GameScreenBase::BeginLoading()
    {
        RESOURCE_MANAGER->pause();
        m_resourceLoadingPaused = btrue;
    }

    void GameScreenBase::EndLoading()
    {
        if ( m_resourceLoadingPaused && !RESOURCE_MANAGER->hasAddQueryOps() )
        {
            m_resourceLoadingPaused = bfalse;
            RESOURCE_MANAGER->unpause();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool GameScreenBase::onUserSignedInOut(u32 _playerIndex, bbool _signedIn, bbool _isOnlineStatusChange)
    {
        if (!_isOnlineStatusChange)
        {
            //if gameplay mode and changes occurs on a player, set m_playerSignedInOut
            if (hasGameplay())
            {
                if ( _playerIndex == GAMEMANAGER->getMainIndexPlayer() )                
                {
                    m_playerSignedInOut = btrue;
                    GAMEMANAGER->setMainIndexPlayer(U32_INVALID);
                }
            }
        }
        return m_playerSignedInOut;
    }


}
