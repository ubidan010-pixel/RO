#include "precompiled_gameplay.h"

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{
    CinematicManager* CinematicManager::s_instance = NULL;

    CinematicManager::CinematicManager()
    : m_lockPlayersCount(0)
    , m_playersInfoCount(0)
    , m_needRecomputeChannels(btrue)
    , m_cameraSubjectsCount(0)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventLockPlayers,359572052),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),this);

        resetPlayersChannels();
    }

    CinematicManager::~CinematicManager()
    {
        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventLockPlayers,359572052), this);
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), this);
        }
    }

    void CinematicManager::clear()
    {
        m_lockPlayersCount = 0;
    }

    void CinematicManager::onEvent( Event * _event )
    {
        if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
        {
            clear();
        } else if (EventLockPlayers * eventLockPlayers = _event->DynamicCast<EventLockPlayers>(ITF_GET_STRINGID_CRC(EventLockPlayers,359572052)))
        {
            if (eventLockPlayers->getLock())
            {
                if (!m_lockPlayersCount && eventLockPlayers->getStopPlayers())
                    sendEventSequenceActorActivate(bfalse);

                if (!m_lockPlayersCount && eventLockPlayers->getStopInputs())
                    sendEventSequenceLosckInputs(bfalse, eventLockPlayers->getForceRevive());

                m_lockPlayersCount++;
            } else
            {
                ITF_ASSERT(m_lockPlayersCount > 0);
                if (m_lockPlayersCount > 0)
                    m_lockPlayersCount--;

                if (!m_lockPlayersCount && eventLockPlayers->getStopPlayers())
                    sendEventSequenceActorActivate(btrue);

                if (!m_lockPlayersCount && eventLockPlayers->getStopInputs())
                    sendEventSequenceLosckInputs(btrue, eventLockPlayers->getForceRevive());
            }
        }
    }

    void CinematicManager::sendEventSequenceActorActivate( bbool _activate )
    {
        EventSequenceActorActivate seqActivate(!_activate);
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
        {
            Actor * playerAct = GAMEMANAGER->getActivePlayer(i);
            if (!playerAct)
                continue;

            playerAct->onEvent(&seqActivate);
        }
    }

    void CinematicManager::sendEventSequenceLosckInputs( bbool _activate, bbool _revive )
    {
        EventSequenceActorPrepare seqPrepare(!_activate);
        if (!_activate)
            seqPrepare.setFriction(100.f); // add BIG friction
        seqPrepare.setRevive(_revive);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
        {
            Actor * playerAct = GAMEMANAGER->getActivePlayer(i);
            if (!playerAct)
                continue;

            playerAct->onEvent(&seqPrepare);
        }
    }

    void CinematicManager::resetPlayersChannels()
    {
        m_playerToChannelIndex.resize(MAX_SEQUENCE_CHANNELS);
        for (u32 i=0; i<MAX_SEQUENCE_CHANNELS; i++)
            m_playerToChannelIndex[i] = i;

        m_needRecomputeChannels = btrue;
    }



    void CinematicManager::controlCamera()
    {
        m_cameraSubjectsCount++;
        if(m_cameraSubjectsCount == 1)
        {
            m_cameraSubjects.clear();
        }
        // also stop playing sequences ...
        CAMERACONTROLLERMANAGER->stopCameraSequence(1.f);

        u32 count = CAMERACONTROLLERMANAGER->getSubjectListCount();
        for (u32 i = 0; i < count; i++)
        {        
            CameraControllerSubject subject;
            CAMERACONTROLLERMANAGER->getSubjectByIndex(0, subject);
            m_cameraSubjects.push_back(subject.m_ref);
            CAMERACONTROLLERMANAGER->unregisterSubject(subject.m_ref);
        }    
        CAMERA->setRotateCam(btrue);
        // force rotation to 0 when controlling camera
        CAMERA->setRotateZAngle(0.f);
    }

    bbool CinematicManager::releaseCamera(bbool _teleport)
    {
        m_cameraSubjectsCount--;
        if(m_cameraSubjectsCount <= 0)
        {
            bbool needTeleport = btrue;
            m_cameraSubjectsCount = 0;

            u32 count = m_cameraSubjects.size();
            for (u32 i = 0; i < count; i++)
            {        
                Actor* act = m_cameraSubjects[i].getActor();
                if(act)
                {
                    CAMERACONTROLLERMANAGER->registerSubject(m_cameraSubjects[i], 1.f, btrue, bfalse);

                    if (_teleport && needTeleport)
                    {
                        ICameraController *icam = CAMERACONTROLLERMANAGER->getMainCameraController();
                        if (icam)
                            icam->teleport(act->getPos());
                        needTeleport = bfalse;
                    }

                    CAMERACONTROLLERMANAGER->m_controllerJustTeleported = btrue;
                }
            }  
            m_cameraSubjects.clear();
            CAMERA->setRotateZAngle(0.f);
            CAMERA->setRotateCam(bfalse);
            return btrue;
        }
        return bfalse;
    }

    void CinematicManager::disactiveChannel(const StringID &_channel)
    {
        if(!_channel.isValid())
            return;

        ITF_VECTOR<StringID>::const_iterator it = m_inactiveChannels.begin();
        ITF_VECTOR<StringID>::const_iterator end = m_inactiveChannels.end();
        for (; it != end; ++it)
        {
            const StringID & data = *it;
            if(data == _channel)
                return;
        }

        m_inactiveChannels.push_back(_channel);
    }

    bbool CinematicManager::isActiveChannel(const StringID & _channel)
    {
        if(!_channel.isValid())
            return btrue;

        if (m_needRecomputeChannels || m_inactiveChannels.size() == 0)
            computeInactiveChannels();

        ITF_VECTOR<StringID>::const_iterator it = m_inactiveChannels.begin();
        ITF_VECTOR<StringID>::const_iterator end = m_inactiveChannels.end();
        for (; it != end; ++it)
        {
            const StringID & data = *it;
            if(data == _channel)
                return bfalse;
        }
        return btrue;
    }

    void CinematicManager::computeInactiveChannels()
    {
        m_inactiveChannels.clear();
        const ITF_VECTOR<SequencePlayers> & playersInfo = getPlayersInfoList();

        String8 channelName;

        u32 maxPlayers = GAMEMANAGER->getMaxPlayerCount();
        u32 nbPlayers  = 0;
        for (u32 i = 0; i < maxPlayers; ++i)
        {
            Player* player  = GAMEMANAGER->getPlayer(i);
            const SequencePlayers *seqPlayer  = NULL;
            if (i < playersInfo.size())
                seqPlayer  = &playersInfo[i];

            if (!player)
                continue;

            if (player->getActive() || (seqPlayer && seqPlayer->m_isActive))
                nbPlayers++;
        }
        for (u32 i = 1; i <= maxPlayers; ++i)
        {
            if (i != nbPlayers)
            {
                channelName.setTextFormat("NbPlayers_%i", i);
                disactiveChannel(channelName.cStr());
            }
        }

        // skinning system
        const ITF_VECTOR<String8> &familyList = GAMEMANAGER->getFamilyList();
        for (u32 i = 0; i < maxPlayers; ++i)
        {
            Player* player  = GAMEMANAGER->getPlayer(i);
            const SequencePlayers *seqPlayer  = NULL;
            if (i < playersInfo.size())
                seqPlayer  = &playersInfo[i];

            if (!player)
                continue;

            const PlayerIDInfo* playerIdInfo = GAMEMANAGER->getPlayerIDInfoFromPlayerIndex( i );

            ITF_VECTOR<String8>::const_iterator familyIter = familyList.begin();
            for (;familyIter != familyList.end(); ++familyIter)
            {

                if (!(player->getActive() || (seqPlayer && seqPlayer->m_isActive)) || !playerIdInfo || !playerIdInfo->getFamily().equals(*familyIter, btrue))
                {
                    channelName.setTextFormat("%i_%s", getPlayerChannel(i), familyIter->cStr());
                    disactiveChannel(channelName.cStr());
                }
            }
        }


        m_playerChannels.resize(maxPlayers);
        for (u32 i = 0; i < maxPlayers; ++i)
        {
            Player* player = GAMEMANAGER->getPlayer(i);
            if (!player)
            {
                m_playerChannels[i].invalidate();
            } else
            {
                // new way 0_globox
                const PlayerIDInfo* playerIdInfo = GAMEMANAGER->getPlayerIDInfoFromPlayerIndex( i );
                channelName.setTextFormat("%i_%s", getPlayerChannel(i), playerIdInfo->getFamily().cStr());
                m_playerChannels[i] = StringID(channelName);
            }
        }

        m_needRecomputeChannels = bfalse;
    }

    const ITF_VECTOR<StringID> & CinematicManager::getInactiveChannels()
    {
        if (m_needRecomputeChannels || m_inactiveChannels.size() == 0)
            computeInactiveChannels();

        return m_inactiveChannels;
    }



    void CinematicManager::disableSequencePlayers()
    {
        if (m_playersInfoCount++)
            return;


        // add control camera because there is no player to control it :)
        controlCamera();

        // Temporary, until the cutscene handles it properly
        GAMEMANAGER->onDisableSequencePlayers();

        u32 maxPlayers = GAMEMANAGER->getMaxPlayerCount();
        m_playersInfo.resize(maxPlayers);

        for (u32 i=0; i<maxPlayers; i++)
        {
            SequencePlayers &seqPlayer  = getPlayersInfoAtIndex(i);
            Player * player             = GAMEMANAGER->getPlayer(i);

            seqPlayer.m_isActive    = player->getActive();
            if (!seqPlayer.m_isActive)
                continue;

            Actor * playerAct       = player->getActor();

            if (playerAct)
            {
                seqPlayer.m_pos         = playerAct->getPos();
                seqPlayer.m_angle       = playerAct->getAngle();
                seqPlayer.m_isDead      = player->isDead();

                EventSequenceActorActivate sequenceEventActivate(btrue);
                playerAct->onEvent(&sequenceEventActivate);
            }

            GAMEMANAGER->deactivatePlayer(i);
        }

        EventLockPlayers evtLockPlayers(btrue);
        EVENTMANAGER->broadcastEvent(&evtLockPlayers);
    }

    void CinematicManager::enableSequencePlayers(bbool _reactivateAlive, bbool _reactivateActors)
    {
        if (m_playersInfoCount == 0)
            return;

        if (--m_playersInfoCount != 0)
            return;

        u32 maxPlayers = getPlayersInfoCount();
        if (maxPlayers == 0)
            return;

		for (u32 i=0; i<maxPlayers; i++)
		{
			SequencePlayers &seqPlayer  = getPlayersInfoAtIndex(i);
			Player * player             = GAMEMANAGER->getPlayer(i);

			if (!seqPlayer.m_isActive || !player || !player->getRegisteredActors().size())
				continue;

			Actor * prevPlayerAct       = player->getActor();

			if (_reactivateActors)
			{
				GAMEMANAGER->activatePlayer(i);
			}

			Actor * playerAct       = player->getActor();
			if (playerAct && prevPlayerAct)
			{
				playerAct->setPos(seqPlayer.m_pos);
				playerAct->setAngle(seqPlayer.m_angle);

				EventSequenceActorActivate sequenceEventActivate(bfalse);
				prevPlayerAct->onEvent(&sequenceEventActivate);
			}

			bbool isDead = player->isDead();
			if (_reactivateAlive)
            {
                if (isDead)
                    player->toggleDeadMode();
            } else
			{
				if (seqPlayer.m_isDead != isDead)
                    player->toggleDeadMode();
			}
		}
		m_playersInfo.clear();

		CAMERACONTROLLERMANAGER->m_controllerJustTeleported = btrue;

		EventLockPlayers evtLockPlayers(bfalse);
		EVENTMANAGER->broadcastEvent(&evtLockPlayers);
	

        // remove the controled camera
        releaseCamera(btrue);
    }

}
