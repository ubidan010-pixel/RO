#include "precompiled_engine.h"

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif  //_ITF_REWARDADAPTER_H_

namespace ITF
{
    RewardAdapter::RewardAdapter() :  m_enable(btrue), m_mainPlayerIndex(U32_INVALID)
    {
    }

    void RewardAdapter::init(bbool enable)
    {
        m_enable = enable ;
    }
    void RewardAdapter::update()
    {

    }

    void RewardAdapter::addReward(const RewardID& _id, u32 _plateformID)
    {
        STD_PAIR<RewardID, u32> newReward;
        newReward.first = _id;
        newReward.second = _plateformID;

        m_rewardMap.insert(newReward);
    }

    u32  RewardAdapter::getRewardPlateform(const RewardID& _id)const
    {
        RewardsMap::const_iterator ite = m_rewardMap.find(_id);
        if (ite == m_rewardMap.end())
            return U32_INVALID;
        return (ite->second);
    }

    RewardManager::RewardManager() :  m_updated(false)
        , m_tagsCount(0), m_timersCount(0), m_enable(btrue), m_computed(bfalse)
    {
    }

    void RewardManager::computeRewardManager()

    {
        if(m_computed)  return;
        for(u32 p=0; p < PlayersMax; p++)
        {
            m_currentSession.m_actions[p].m_tags.resize(m_tagsCount);
            m_currentSession.m_actions[p].m_timers.resize(m_timersCount);

            m_previousSession.m_actions[p].m_tags.resize(m_tagsCount);
            m_previousSession.m_actions[p].m_timers.resize(m_timersCount);

            m_totalSessions.m_actions[p].m_tags.resize(m_tagsCount);
            m_totalSessions.m_actions[p].m_timers.resize(m_timersCount);
        }
        m_computed = btrue;

        Reset();
    }

    void RewardManager::Reset()
    {
        ITF_ASSERT(m_computed);
        if(!m_computed)  return;

        for(u32 p=0; p < PlayersMax; p++)
        {
            m_currentSession.m_actions[p].m_tags.fillZero();
            m_currentSession.m_actions[p].m_timers.fillZero();
            m_previousSession.m_actions[p].m_tags.fillZero();
            m_previousSession.m_actions[p].m_timers.fillZero();
            m_totalSessions.m_actions[p].m_tags.fillZero();
            m_totalSessions.m_actions[p].m_timers.fillZero();
        }
    }

    RewardManager::~RewardManager()
    {
    }

    void RewardManager::update()
    {
    }


    void  RewardManager::stopAllTimers()
    {
        for(u32 p=0; p<PlayersMax; p++)
            for(u32 t = 0; t < m_timersCount; t++)
                Action_TimerStop(t, p);
    }

    void RewardManager::Action_TimerResume(Timer _timer, u32 _player, bbool _forceStartIfNeeded)
    {
        _player = getSafePlayerIndex(_player);
        if(!m_currentSession.m_actions[_player].m_actif)
            return;

        sTimer *pTimer = getTimer(_timer, _player);

        if(_forceStartIfNeeded && !pTimer->m_actif)
            Action_TimerStart(_timer, _player);
        else if(pTimer && pTimer->m_actif && !pTimer->m_running)
        {
            pTimer->m_start = (f32)SYSTEM_ADAPTER->getTime();
            pTimer->m_running = btrue;
            IamUpdated();
        }
    }
    void RewardManager::Action_TimerPause(Timer _timer, u32 _player, bbool _forceUpdateToTotalSession)
    {
        _player = getSafePlayerIndex(_player);
        if(!m_currentSession.m_actions[_player].m_actif)
            return;

        sTimer *pTimer = getTimer(_timer, _player);
        if(pTimer && pTimer->m_actif && pTimer->m_running)
        {
            pTimer->m_total += ((f32)SYSTEM_ADAPTER->getTime()-pTimer->m_start);
            pTimer->m_running = bfalse;
            pTimer->m_updated = btrue;
            IamUpdated();

            if ( _forceUpdateToTotalSession )
            {
                sTimer *pTotalSessionTimer = getTimerFromTotalSession(_timer, _player);
                if(pTotalSessionTimer)
                {
                    pTotalSessionTimer->m_total += pTimer->m_total;
                    pTotalSessionTimer->m_updated = btrue;
                    pTimer->m_total = 0;
                }
            }
        }
    }

    void RewardManager::pauseAllTimers()
    {
        for(u32 p=0; p<PlayersMax; p++)
            for(u32 t = 0; t < m_timersCount; t++)
                Action_TimerPause(t, p);
    }
    void RewardManager::resumeAllTimers()
    {
        for(u32 p=0; p<PlayersMax; p++)
            for(u32 t = 0; t < m_timersCount; t++)
                Action_TimerResume(t, p);
    }



    RewardManager::sTimer* RewardManager::getTimer(Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        ITF_ASSERT(_timer<m_timersCount);
        return &m_currentSession.m_actions[_player].m_timers[_timer];
    }


    RewardManager::sTimer* RewardManager::getTimerFromTotalSession(Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        ITF_ASSERT(_timer<m_timersCount);
        return &m_totalSessions.m_actions[_player].m_timers[_timer];
    }

    void RewardManager::Action_TimerStart(Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        if(!m_currentSession.m_actions[_player].m_actif)
            return;

        sTimer *pTimer = getTimer(_timer, _player);
        if(pTimer && !pTimer->m_actif)
        {
            pTimer->m_start = (f32)SYSTEM_ADAPTER->getTime();
            pTimer->m_running = btrue;
            pTimer->m_actif = btrue;
            IamUpdated();
        }
    }

    void RewardManager::Action_TimerStop(Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        if(!m_currentSession.m_actions[_player].m_actif)
            return;

        ITF_ASSERT(_timer<m_tagsCount);

        sTimer *pTimer = getTimer(_timer, _player);
        if(pTimer && pTimer->m_running && pTimer->m_actif)
        {
            pTimer->m_total += ((f32)SYSTEM_ADAPTER->getTime()-pTimer->m_start);
            pTimer->m_running = bfalse;
            pTimer->m_actif = bfalse;
            pTimer->m_updated = btrue;

            sTimer *pTotalSessionTimer = getTimerFromTotalSession(_timer, _player);
            if(pTotalSessionTimer)
            {
                pTotalSessionTimer->m_total += pTimer->m_total;
                pTotalSessionTimer->m_updated = btrue;
            }
            IamUpdated();
        }
    }

    void RewardManager::Action_TimerReset(Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);

        ITF_ASSERT(_timer<m_tagsCount);

        sTimer *pTimer = getTimer(_timer, _player);
        if ( pTimer )
        {
            pTimer->m_total = 0.0f;
            pTimer->m_running = bfalse;
            pTimer->m_actif = bfalse;
            pTimer->m_updated = btrue;

            sTimer *pTotalSessionTimer = getTimerFromTotalSession(_timer, _player);
            if(pTotalSessionTimer)
            {
                pTotalSessionTimer->m_total = 0.0f;
                pTotalSessionTimer->m_updated = btrue;
            }
            IamUpdated();
        }
    }

    void RewardManager::Action_Set(Tag _tag, u32 _player, f32 _value)
    {
        _player = getSafePlayerIndex(_player);
        ITF_ASSERT(_tag<m_tagsCount);

        if(m_currentSession.m_actions[_player].m_actif)
        {
            m_currentSession.m_actions[_player].m_tags[_tag].m_value    = _value;
            m_currentSession.m_actions[_player].m_tags[_tag].m_updated  = btrue;
        }
        m_totalSessions.m_actions[_player].m_tags[_tag].m_value   = _value;
        m_totalSessions.m_actions[_player].m_tags[_tag].m_updated = btrue;
        IamUpdated();
    }

    void RewardManager::Action_New(Tag _tag, u32 _player, f32 _count)
    {
        _player = getSafePlayerIndex(_player);
        ITF_ASSERT(_tag<m_tagsCount);

        if(m_currentSession.m_actions[_player].m_actif)
        {
            m_currentSession.m_actions[_player].m_tags[_tag].m_value      += _count;
            m_currentSession.m_actions[_player].m_tags[_tag].m_updated  = btrue;
        }
        m_totalSessions.m_actions[_player].m_tags[_tag].m_value    += _count;
        m_totalSessions.m_actions[_player].m_tags[_tag].m_updated = btrue;
        IamUpdated();
    }

    void RewardManager::Action_AddPlayer(u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        m_currentSession.m_actions[_player].m_actif = btrue;
        m_currentSession.m_actions[_player].m_enterSession = (f32)SYSTEM_ADAPTER->getTime();
    }
    void RewardManager::Action_RemovePlayer(u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        m_currentSession.m_actions[_player].m_actif = bfalse;
        m_currentSession.m_actions[_player].m_leaveSession = (f32)SYSTEM_ADAPTER->getTime();
    }

    void RewardManager::Session_New()
    {
        ITF_ASSERT(m_computed);

        // Reset data
        for (u32 p = 0; p < PlayersMax; p++)
        {
            m_currentSession.m_actions[p].m_tags.fillZero();
            m_currentSession.m_actions[p].m_timers.fillZero();
            m_currentSession.m_actions[p].m_actif = bfalse;
            m_currentSession.m_actions[p].m_enterSession = 0.0;
            m_currentSession.m_actions[p].m_leaveSession = 0.0;
        }

        m_currentSession.m_LengthInSec = 0;
        m_currentSession.m_Start = (f32)SYSTEM_ADAPTER->getTime();
        m_currentSession.m_Total = 0.0;
        m_currentSession.m_isRunning = btrue;
        m_currentSession.m_isPaused = bfalse;

        if(m_totalSessions.m_Start == 0.0)
            m_totalSessions.m_Start =  m_currentSession.m_Start;
    }

    void RewardManager::Session_Reset()
    {
        bbool actifState [PlayersMax];

        // Save active state
        for (u32 p = 0; p < PlayersMax; p++)
            actifState[p] = m_currentSession.m_actions[p].m_actif;

        Session_New();

        // Restore state
        for (u32 p = 0; p < PlayersMax; p++)
            m_currentSession.m_actions[p].m_actif = actifState[p];
    }

    void RewardManager::Session_Pause()
    {
        if(m_currentSession.m_isRunning && !m_currentSession.m_isPaused)
        {
            m_currentSession.m_Total += ((f32)SYSTEM_ADAPTER->getTime() - m_currentSession.m_Start);
            m_currentSession.m_isPaused = btrue;
            pauseAllTimers();
        }
    }

    void RewardManager::Session_Resume()
    {
        if(m_currentSession.m_isRunning && m_currentSession.m_isPaused)
        {
            m_currentSession.m_Start = (f32)SYSTEM_ADAPTER->getTime();
            m_currentSession.m_isPaused = bfalse;
            resumeAllTimers();
        }
    }

    void RewardManager::Session_Stop()
    {
        if ( !m_currentSession.m_isRunning )
        {
            ITF_ASSERT_MSG(0, "Session cannot be paused, its already done");
            return;
        }

        Session_Pause();
        stopAllTimers();
        m_currentSession.m_LengthInSec = (u32)roundFloat((f32)m_currentSession.m_Total);
        m_currentSession.m_isRunning = bfalse;

        // Save current session
        for (u32 p = 0; p < PlayersMax; p++)
        {
            m_previousSession.m_actions[p].m_tags = m_currentSession.m_actions[p].m_tags;
            m_previousSession.m_actions[p].m_timers = m_currentSession.m_actions[p].m_timers;

            m_previousSession.m_actions[p].m_actif =  m_currentSession.m_actions[p].m_actif;
            m_previousSession.m_actions[p].m_enterSession = m_currentSession.m_actions[p].m_enterSession;
            m_previousSession.m_actions[p].m_leaveSession = m_currentSession.m_actions[p].m_leaveSession;

            m_totalSessions.m_LengthInSec += m_currentSession.m_LengthInSec;
        }

        m_previousSession.m_LengthInSec = m_currentSession.m_LengthInSec;
        m_previousSession.m_Start = m_currentSession.m_Start;
        m_previousSession.m_isRunning = bfalse;
    }

    bbool RewardManager::IsTagValueUpdated(GameSession* _gameSession, Tag _tag, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        if(!_gameSession || _player >= PlayersMax)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, tag cannot be returned.");
            return bfalse;
        }

        return _gameSession->m_actions[_player].m_tags[_tag].m_updated;
    }

    f32 RewardManager::GetTagValue( GameSession* _gameSession, Tag _tag, u32 _player )
    {
        _player = getSafePlayerIndex(_player);
        if(!_gameSession || _player >= PlayersMax)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, tag cannot be returned.");
            return -1.f;
        }

        return _gameSession->m_actions[_player].m_tags[_tag].m_value;
    }

    bbool RewardManager::GetIsSessionRunning( GameSession* _gameSession )
    {
        if(!_gameSession)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, session cannot be evaluated.");
            return bfalse;
        }

        return _gameSession->m_isRunning;
    }

    bbool RewardManager::GetIsPlayerActif( GameSession* _gameSession, u32 _player )
    {
        _player = getSafePlayerIndex(_player);
        if(!_gameSession || _player >= PlayersMax)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, status of player cannot be returned.");
            return bfalse;
        }

        return _gameSession->m_actions[_player].m_actif;
    }

    bbool RewardManager::IsTimerUpdated(GameSession* _gameSession, Timer _timer, u32 _player)
    {
        _player = getSafePlayerIndex(_player);
        if(!_gameSession || _player >= PlayersMax)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, time elapsed cannot be returned.");
            return bfalse;
        }

        sTimer* _sTimer = & _gameSession->m_actions[_player].m_timers[_timer];
        if(_sTimer)
            return _sTimer->m_updated;
        return bfalse;
    }

    f64 RewardManager::GetTimeElapsedTimer( GameSession* _gameSession, Timer _timer, u32 _player )
    {
        _player = getSafePlayerIndex(_player);
        if(!_gameSession || _player >= PlayersMax)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, time elapsed cannot be returned.");
            return 0.0;
        }

        sTimer* _sTimer = & _gameSession->m_actions[_player].m_timers[_timer];
        if (!_sTimer->m_actif && !_sTimer->m_running)
            return _sTimer->m_total;

        // Timer is still running
        return 0.0;
    }

    u32 RewardManager::GetSessionLengthInSec( GameSession* _gameSession )
    {
        if(!_gameSession)
        {
            ITF_ASSERT_MSG(0, "Wrong value for reward manager, session length cannot be returned.");
            return bfalse;
        }

        return _gameSession->m_LengthInSec;
    }

    void RewardManager::Action_GlobalEvent( Tag _tag, StringID _id  )
    {
        sEvent ev;
        ev.m_param = _id;
        ev.m_tag = _tag;
        m_currentSession.m_events.push_back(ev);
        IamUpdated();
    }

    void RewardManager::clearEvents()
    {
        m_currentSession.m_events.clear();
    }

    bbool RewardManager::IsGlobalEventTriggered( Tag _tag )
    {
        for (u32 i = 0; i < m_currentSession.m_events.size(); i++)
        {
            if(m_currentSession.m_events[i].m_tag == _tag)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    StringID RewardManager::GetGlobalEventID( Tag _tag )
    {
        for (u32 i = 0; i < m_currentSession.m_events.size(); i++)
        {
            if(m_currentSession.m_events[i].m_tag == _tag)
            {
                return m_currentSession.m_events[i].m_param;
            }
        }
        return StringID::Invalid;
    }

    u32 RewardManager::getSafePlayerIndex( u32 _player )
    {
        if(_player<PlayersMax) return _player;
        ITF_ASSERT_MSG(0, "Warning, player for REWARD MANAGER not correct, 0 will be used.");
        return 0;
    }

    bbool RewardManager::backupSaveSession( SaveSession* _pSaveSession, u32 _playerToBackup )
    {
        ITF_ASSERT(_pSaveSession!=NULL);
        _pSaveSession->m_tags.clear();
        for(u32 i = 0; i < m_totalSessions.m_actions[_playerToBackup].m_tags.size(); i++)
        {
            _pSaveSession->m_tags.push_back(m_totalSessions.m_actions[_playerToBackup].m_tags[i].m_value);
        }

        _pSaveSession->m_timersTotal.clear();
        for(u32 i = 0; i < m_totalSessions.m_actions[_playerToBackup].m_timers.size(); i++)
        {
            _pSaveSession->m_timersTotal.push_back(m_totalSessions.m_actions[_playerToBackup].m_timers[i].m_total);
        }
        return btrue;
    }

    bbool RewardManager::setSaveSessionFromSave( SaveSession* _pSaveSession, u32 _playerToSet )
    {
        ITF_ASSERT(_pSaveSession!=NULL);
        // Debug assert only, the return false is normal
        //ITF_ASSERT(_pSaveSession->m_timersTotal.size() ==  m_totalSessions.m_actions[_playerToSet].m_timers.size());
        //ITF_ASSERT(_pSaveSession->m_tags.size() ==  m_totalSessions.m_actions[_playerToSet].m_tags.size());

        if(!_pSaveSession
            ||
            _pSaveSession->m_tags.size() !=  m_totalSessions.m_actions[_playerToSet].m_tags.size()
            ||
            _pSaveSession->m_timersTotal.size() !=  m_totalSessions.m_actions[_playerToSet].m_timers.size())
        {
            return bfalse;
        }

        Reset();

        for(u32 i = 0; i < m_totalSessions.m_actions[_playerToSet].m_tags.size(); i++)
        {
            m_totalSessions.m_actions[_playerToSet].m_tags[i].m_updated = bfalse;
            m_totalSessions.m_actions[_playerToSet].m_tags[i].m_value = _pSaveSession->m_tags[i];
        }

        for(u32 i = 0; i < m_totalSessions.m_actions[_playerToSet].m_timers.size(); i++)
        {
            m_totalSessions.m_actions[_playerToSet].m_timers[i].m_total = _pSaveSession->m_timersTotal[i];
            m_totalSessions.m_actions[_playerToSet].m_timers[i].m_updated = bfalse;
        }
        return btrue;
    }

    void RewardManager::postUpdate()
    {
        m_updated = bfalse;
        clearEvents();

        for(u32 player = 0; player < PlayersMax; player++)
        {
            for(u32 i = 0; i < m_totalSessions.m_actions[player].m_tags.size(); i++)
            {
                m_totalSessions.m_actions[player].m_tags[i].m_updated = bfalse;
            }

            for(u32 i = 0; i < m_totalSessions.m_actions[player].m_timers.size(); i++)
            {
                m_totalSessions.m_actions[player].m_timers[i].m_updated = bfalse;
            }
        }
    }

    BEGIN_SERIALIZATION(RewardManager::SaveSession)
        SERIALIZE_CONTAINER("tags", m_tags);
    SERIALIZE_CONTAINER("timers", m_timersTotal);
    END_SERIALIZATION()

}// namespace ITF
