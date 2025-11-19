#include "precompiled_gameplay_rayman.h"

#ifdef ITF_SUPPORT_ONLINETRACKING

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_



#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_


namespace ITF {

Ray_OnlineTrackingManager::Ray_OnlineTrackingManager():
  m_OnlineTrackingTimer(0.0f)
, m_diamondCollected(0)
, m_jumpCount(0)
, m_attackCount(0)
, m_LdiamondCollected(0)
, m_LjumpCount(0)
, m_LattackCount(0)
, m_timeElapsedSinceLastCheckPoint(0)
, m_checkPointOrderSinceLevelBegin(1)
, m_lastPaf(0)
, m_lastOffscreen(0)
, m_fLevelTime(0.0f)
, m_PlayerIntervalTime(0)
, m_Paf2Players(0)
, m_Paf3Players(0)
, m_Paf4Players(0)
, m_playerIntervalCount(0)
, m_awardUnlockCount(0)
, m_levelStopCount(0)
, m_currentLevelTime(0.0f)
, m_absolutePlayTime(0.0f)
, m_runCount(0)
, m_challengeTimeAttackCount(0)
, m_currentLevelGameOverCount(0)
, m_timeAttackTime(0)
, m_persistantTimer(0.0f)
, m_finalBrokenCageIndex(-1)
, m_hiddenBrokenCageIndex1(-1)
, m_hiddenBrokenCageIndex2(-1)
, m_TimeAttackAvailableForThisMap(bfalse)
, m_lumStage1Count(0)
, m_lumStage2Count(0)
{
	ResetDeathCountTimers(bfalse);
	ResetPlayTimes();
	m_gamePlayId = Ray_GameScreen_Gameplay::GetClassCRCStatic();
}

void Ray_OnlineTrackingManager::ResetPlayTimes()
{
	for (u32 i=0;i<4;i++)
	{
			m_PlayTimeForPlayerIndex[i]	= 0.0f;
	}
}

void Ray_OnlineTrackingManager::ResetDeathCountTimers(bool bResetCheckPointDataOnly)
{
		for (u32 i=0;i<4;i++)
		{
				m_deathCountSinceLastCheckPoint[i]  = 0;

				if (!bResetCheckPointDataOnly)
				{
					m_LdeathCountSinceLastCheckPoint[i] = 0;
				}
		}
}

void Ray_OnlineTrackingManager::onAttack()
{
	m_attackCount++;
}

void Ray_OnlineTrackingManager::onHitExit()
{
	m_lastPaf		= 0;
	m_lastOffscreen	= 0;
}

void Ray_OnlineTrackingManager::OnlineTrackingTrackDeath(ITF::u32 playerIndex)
{

	u32 num_players = GAMEMANAGER->getNumActivePlayers();

	bbool bAllPlayerAreDead = btrue;
	
	for (u32 i=0;i<num_players;i++)
	{
		Player *player = GAMEMANAGER->getPlayer(i);
		
		if (player)
		{
			if (!player->isDead())
			{
				bAllPlayerAreDead = bfalse;
				break;
			}
			
		}

	}

	if (bAllPlayerAreDead==btrue)
	{
		m_currentLevelGameOverCount++;
	}

	if (CONFIG->m_enableonlineTracking==btrue)
	{
		
		if (ONLINETRACKING_ADAPTER)
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_deathCountSinceLastCheckPoint[ playerIndex ]++;

			u32 currentEngineVersion = Versioning::getEngineVersion();

			String str,checkpointstr, levelCopy;

			if (CURRENTWORLD!=NULL)
			{
					if (CURRENTWORLD->getRootScene()!=NULL)
					{
						CURRENTWORLD->getRootScene()->getPath().getString(levelCopy);
					}
			}

			Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

			ITF_STDSTRING checkPointStr = "";

			if ( checkpoint )
			{
				ObjectPath path;
				SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);
				path.toString(checkPointStr);
			}


			Player *player = GAMEMANAGER->getPlayer(playerIndex);

			Actor *playerActor = player->getActor();

			if (playerActor)
			{
				ITF::String8 last_paf,last_offscreen;

				if (m_lastPaf == 0)
				{
					last_paf = "NO";
				}
				else
				{
					last_paf = "YES";
				}

				if (m_lastOffscreen == 0)
				{
					last_offscreen = "NO";
				}
				else
				{
					last_offscreen = "YES";
				}
				
				str.setTextFormat("VERSION=%d&NUMPLAYERS=%d&PLAYERID=%d&LEVELNAME=%s&CHECKPOINT=%s&DEATH=YES&PAF=%s&OFFSCREEN=%s&X=%.1f&Y=%.1f&Z=%.1f",
						currentEngineVersion,
						num_players,
						playerIndex+1,
                        StringToUTF8(levelCopy).get(),
						checkPointStr.c_str(),
						last_paf.cStr(),
						last_offscreen.cStr(),
						playerActor->getPos().getX(),
						playerActor->getPos().getY(),
						playerActor->getPos().getZ() );

				StringConverter c(str);

				ONLINETRACKING_ADAPTER->sendTag("PLAYER_HIT",c.getChar());

				onHitExit();

			}
		}
	}

}

void Ray_OnlineTrackingManager::SendLevelStop(CheckPointTag _CP_Tag)
{

		Ray_PersistentGameData_LevelTracking *pLevelData = RAY_GAMEMANAGER->getCurrentLevelData()->getTrackingData();

		PathString8_t level_copy;

		startNewTag();

		u32 slotId = RAY_GAMEMANAGER->getCurrentSlotIndex();

		addUint32("SLOT",slotId);

		if (CURRENTWORLD!=NULL)
		{
			if (CURRENTWORLD->getRootScene()!=NULL)
			{
				CURRENTWORLD->getRootScene()->getPath().getString(level_copy);
			}
		}

		u32 num_players = GAMEMANAGER->getNumActivePlayers();

		addUint32("NUMPLAYERS",num_players);
		addUint32("TIME",(int)floorf(m_currentLevelTime));
		addString("LEVELNAME",level_copy);
		addUint32("RUNCOUNT", m_runCount);
	
		addString("TAAVAILABLE", (m_TimeAttackAvailableForThisMap==btrue)?"YES":"NO");
	
		addString("MODE", (RAY_GAMEMANAGER->isTimeAttack()) ? "TA" : "WT");

		ESpot_State e = RAY_GAMEMANAGER->getWMSpotState(s_JU_B);

		if (e == SPOT_STATE_CLOSED)
		{
			addUint32("P", 1);
		}
		else
		{

			e = RAY_GAMEMANAGER->getWMSpotState(s_DE_A1);

			if (e==SPOT_STATE_CLOSED)
			{
				addUint32("P", 2);
			}
			else
			{
				addUint32("P", 3);
			}
		}

		if ((m_TimeAttackAvailableForThisMap==bfalse && pLevelData->getChallengeTimeAttackCount()==0) || (pLevelData->getChallengeTimeAttackCount()==0 && !RAY_GAMEMANAGER->isTimeAttack()) )
		{
			// TA not unlocked or unlocked but not completed /////
			addString("CHTA","NA");	

		}
		else
		{
			
				if (pLevelData->getChallengeTimeAttackCount()==0 && RAY_GAMEMANAGER->isTimeAttack() )
				{
					// time attack success for the first time///
					pLevelData->updateChallengeTimeAttackCount(m_runCount);
					m_challengeTimeAttackCount = m_runCount;
				}
				else
				{
					m_challengeTimeAttackCount = pLevelData->getChallengeTimeAttackCount();
				}

				addUint32("CHTA",m_challengeTimeAttackCount);

		}

		u32 total_lums = 0;

		for (u32 i=0;i<num_players;i++)
		{
				total_lums += RAY_GAMEMANAGER->getCurrentScore().getLums(i);
		}

		addUint32("LUMS", total_lums);

		addUint32("GAMEOVER",m_currentLevelGameOverCount);

		if(_CP_Tag == CP_EndOfLevel || _CP_Tag == CP_EndOfTimeAttack)
                addString("REASON", "COMPLETED");
        else if (_CP_Tag == CP_ExitLevel)
                addString("REASON", "QUIT");
		else if (_CP_Tag == CP_RestartLevel)
				addString("REASON","RESTART");

		/*if ( m_firstCompMap.count( level_copy ) == 0 ) 
		{
			addString("FIRSTCOMP","YES");
		}
		else
		{
			if (m_firstCompMap[level_copy] == LEVEL_ALREADY_COMPLETED )
			{
				addString("FIRSTCOMP","NO");
			}
			else
			{
				addString("FIRSTCOMP","YES");
			}
			
		}*/

		/*if (_CP_Tag==CP_EndOfLevel || _CP_Tag==CP_EndOfTimeAttack)
		{
			m_firstCompMap[level_copy] = LEVEL_ALREADY_COMPLETED;
		}*/

		
		f32 first_time_stored = pLevelData->getFirstTimeLevelCompleted();

		if (first_time_stored>0.0f)
		{
					addUint32("FIRSTCOMPTIME", (int)floorf(first_time_stored) );
					addString("FIRSTCOMP","NO");
		}
		// first_time_stored>0
		else if (_CP_Tag == CP_EndOfLevel || _CP_Tag == CP_EndOfTimeAttack)
		{
					pLevelData->setFirstTimeLevelCompleted(m_currentLevelTime);
					first_time_stored = m_currentLevelTime;
					addUint32("FIRSTCOMPTIME", (int)floorf(first_time_stored) );
					addString("FIRSTCOMP","YES");
		}			
		else  
		{
					addString("FIRSTCOMPTIME","NA");	
					addString("FIRSTCOMP","NA");
		}
				
		

		if (pLevelData->getChallengeLumsStage1()>0)
		{
			addUint32("CHLUMSTG1",pLevelData->getChallengeLumsStage1() );
		}
		else
		{
			addString("CHLUMSTG1","NA");
		}

		if (pLevelData->getChallengeLumsStage2()>0)
		{
			addUint32("CHLUMSTG2",pLevelData->getChallengeLumsStage2() );
		}
		else
		{
			addString("CHLUMSTG2","NA");
		}


		if ( m_finalBrokenCageIndex==-1)
		{
			addString("CAGE","NA");
		}
		else
		{
			addUint32("CAGE",m_finalBrokenCageIndex);

			if (pLevelData)
			{
				if (pLevelData->getChallengeCage()==0)
				{
					pLevelData->updateChallengeCage(m_runCount);
				}
			}

		}
		if ( m_hiddenBrokenCageIndex1==-1)
		{
			addString("CAGEHIDDEN1","NA");
		}
		else
		{
			addUint32("CAGEHIDDEN1",m_hiddenBrokenCageIndex1);

			if (pLevelData)
			{
				if (pLevelData->getChallengeHidden1()==0)
				{
					pLevelData->updateChallengeHidden1(m_runCount);
				}
			}
		}
		if ( m_hiddenBrokenCageIndex2 == -1)
		{
			addString("CAGEHIDDEN2","NA");
		}
		else
		{
			addUint32("CAGEHIDDEN2",m_hiddenBrokenCageIndex2);

			if (pLevelData)
			{
				if (pLevelData->getChallengeHidden2() == 0)
				{
					pLevelData->updateChallengeHidden2(m_runCount );
				}
			}
		}

		u32 ccage,ch1cage,ch2cage;

		ccage	= pLevelData->getChallengeCage();
		ch1cage = pLevelData->getChallengeHidden1();
		ch2cage = pLevelData->getChallengeHidden2();

		if (ccage>0)
		{
			addUint32("CHCAGE",ccage);
		}
		else
		{
			addString("CHCAGE","NA");
		}

		if (ch1cage>0)
		{
			addUint32("CHCAGEHIDDEN1",ch1cage);
		}
		else
		{
			addString("CHCAGEHIDDEN1","NA");
		}
		if (ch2cage>0)
		{
			addUint32("CHCAGEHIDDEN2",ch2cage);
		}
		else
		{
			addString("CHCAGEHIDDEN2","NA");
		}

		m_finalBrokenCageIndex = m_hiddenBrokenCageIndex1 = m_hiddenBrokenCageIndex2 = -1;


		if (_CP_Tag==CP_ExitLevel || _CP_Tag == CP_RestartLevel)
		{

			Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

			ITF_STDSTRING checkPointStr = "";

			if ( checkpoint )
			{
				ObjectPath path;
				SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);	
				path.toString(checkPointStr);
			}

			ITF::String8 checkp = ITF::String8(checkPointStr.c_str());

			i32 pos = 0;
			
			checkp.strstr('|',bfalse,&pos);

			if (pos!=0)
			{
				ITF::String8 checkpshort = checkp.substr( pos+1, checkp.getLen()-(pos+1));
				addString("CHK",checkpshort);
			}
			else
			{
				addString("CHK",checkp);
			}
		}
		else
		{
			addString("CHK","NA");
		}		

		sendTag("LEVEL_STOP",bfalse);
        sendSignal("LevelStop");

		m_levelStopCount++;

		//LOG("end sendLevelStop");


}


void Ray_OnlineTrackingManager::onUnLockLumStage1()
{

	m_lumStage1Count = m_runCount;

	Ray_PersistentGameData_LevelTracking *pLevelData = RAY_GAMEMANAGER->getCurrentLevelData()->getTrackingData();

	pLevelData->updateChallengeLumsStage1( m_lumStage1Count - pLevelData->getChallengeLumsStage1());

}

void Ray_OnlineTrackingManager::onUnLockLumStage2()
{
	m_lumStage2Count = m_runCount;

	Ray_PersistentGameData_LevelTracking *pLevelData = RAY_GAMEMANAGER->getCurrentLevelData()->getTrackingData();



	pLevelData->updateChallengeLumsStage2( m_lumStage2Count - pLevelData->getChallengeLumsStage2());


}

void Ray_OnlineTrackingManager::onReachedCheckpoint(CheckPointTag _CP_Tag)
{

	// post launch///

	if (_CP_Tag==CP_EndOfLevel || _CP_Tag==CP_ExitLevel || _CP_Tag == CP_RestartLevel)
	{
		SendLevelStop(_CP_Tag);
	}


	// pre launch////

	if (CONFIG->m_enableonlineTracking==btrue)
	{
		String str,str2,checkpointstr;
        PathString8_t level_name;

		if (CURRENTWORLD!=NULL)
		{
			if (CURRENTWORLD->getRootScene()!=NULL)
			{
				CURRENTWORLD->getRootScene()->getPath().getString(level_name);
			}
		}

		Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

		ITF_STDSTRING checkPointStr = "";

		if ( checkpoint )
		{
			ObjectPath path;
			SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);	
			path.toString(checkPointStr);
		}

		u32 num_players = GAMEMANAGER->getNumActivePlayers();

		if (ONLINETRACKING_ADAPTER)
		{

		    u32 total_lums = 0;

			for (u32 i=0;i<num_players;i++)
			{
				total_lums += RAY_GAMEMANAGER->getCurrentScore().getLums(i);
			}

			startNewTag();

            String8 checkPointType;
            switch (_CP_Tag)
            {
            case CP_Normal:
                checkPointType = "CP_Normal";
                break;
            case CP_EndOfLevel:
                checkPointType = "CP_EndOfLevel";
                break;
            case CP_ExitLevel:
                checkPointType = "CP_ExitLevel";
                break;
            case CP_RestartLevel:
                checkPointType = "CP_RestartLevel";
                break;
            case CP_EndOfTimeAttack:
                checkPointType = "CP_EndOfTimeAttack";
                break;
            default: "CP_Normal";
            };

            addString("checkPointType", checkPointType);

			addUint32("numPlayers", num_players);
			addString("levelName", level_name);
			addString("checkPoint", ITF::String8(checkPointStr.c_str()));
			addUint32("checkPointOrder", m_checkPointOrderSinceLevelBegin );
			
			addUint32("lums", total_lums);//LUMS
			addUint32("coins", RAY_GAMEMANAGER->getTotalTakenToothCount());
			addUint32("brokenCages", RAY_GAMEMANAGER->getBrokenCageCount());

			if (_CP_Tag == CP_Normal)
			{
				
				addUint32("deathCountPlayer1", m_deathCountSinceLastCheckPoint[0]);
				addUint32("deathCountPlayer2", m_deathCountSinceLastCheckPoint[1]);
				addUint32("deathCountPlayer3", m_deathCountSinceLastCheckPoint[2]);
				addUint32("deathCountPlayer4", m_deathCountSinceLastCheckPoint[3]);
				addUint32("jumpCount", m_jumpCount);
				addUint32("attackCount", m_attackCount);
				addf32("time", m_timeElapsedSinceLastCheckPoint);

			}
			else
			{
			
				addUint32("deathCountPlayer1", m_LdeathCountSinceLastCheckPoint[0]);
				addUint32("deathCountPlayer2", m_LdeathCountSinceLastCheckPoint[1]);
				addUint32("deathCountPlayer3", m_LdeathCountSinceLastCheckPoint[2]);
				addUint32("deathCountPlayer4", m_LdeathCountSinceLastCheckPoint[3]);
				addUint32("jumpCount" , m_LjumpCount);
				addUint32("attackCount", m_LattackCount);
				addf32("time", m_fLevelTime);

			}

            if(_CP_Tag == CP_EndOfLevel)
                addString("completed", "completed");
            else if (_CP_Tag == CP_ExitLevel)
                addString("completed", "quit");

            sendTag(_CP_Tag == CP_Normal ? "CHECK_POINT" : "LEVEL_STOP2",btrue);

			if (_CP_Tag == CP_EndOfLevel)
			{
				startNewTag();
				addUint32("numPlayers",num_players);
				addString("levelName",level_name);
				addString("checkPoint", ITF::String8(checkPointStr.c_str()));
				addUint32("checkPointOrder", m_checkPointOrderSinceLevelBegin );
				
				addUint32("lums",total_lums);//LUMS
				addUint32("coins",RAY_GAMEMANAGER->getTotalTakenToothCount());
				addUint32("brokenCages",RAY_GAMEMANAGER->getBrokenCageCount());
				addUint32("deathCountPlayer1",m_deathCountSinceLastCheckPoint[0]);
				addUint32("deathCountPlayer2",m_deathCountSinceLastCheckPoint[1]);
				addUint32("deathCountPlayer3",m_deathCountSinceLastCheckPoint[2]);
				addUint32("deathCountPlayer4",m_deathCountSinceLastCheckPoint[3]);
				addUint32("jumpCount",m_jumpCount);
				addUint32("attackCount",m_attackCount);
				addf32("time",m_timeElapsedSinceLastCheckPoint);
				sendTag("CheckPoint",btrue);
			}
            else if(_CP_Tag == CP_Normal)
            {
                m_checkPointOrderSinceLevelBegin++; 

				for (int i=0;i<4;i++)
				{
					m_LdeathCountSinceLastCheckPoint[i] += m_deathCountSinceLastCheckPoint[i];
				}

				m_LdiamondCollected += m_diamondCollected;
				m_LjumpCount		+= m_jumpCount;
				m_LattackCount		+= m_attackCount;
				m_fLevelTime		+= m_timeElapsedSinceLastCheckPoint;

				resetStats(btrue);
				ResetDeathCountTimers(btrue);
            }

            sendSignal("CheckPoint");
		}
	}

	SavePersistant();

	
}

void Ray_OnlineTrackingManager::resetStats(bool bResetCheckPointDataOnly )
{

    m_timeElapsedSinceLastCheckPoint = 0.0f;
    m_jumpCount						 = 0;
    m_attackCount					 = 0;
    m_diamondCollected				 = 0;   

	if (!bResetCheckPointDataOnly)
	{
		 m_LjumpCount				= 0;
		 m_LattackCount				= 0;
		 m_LdiamondCollected		= 0; 
		 m_fLevelTime				= 0;
	}
}


void Ray_OnlineTrackingManager::startNewTag()
{
	m_trackingData.clear();
}

void Ray_OnlineTrackingManager::sendTag(const char* tag_key,bbool sendVersionTag)
{
	ITF::String8 tag_value;

	if (sendVersionTag==btrue)
	{
		u32 currentEngineVersion = Versioning::getEngineVersion();
		tag_value.setTextFormat("VERSION=%d", currentEngineVersion);
	}

	std::map <ITF::String8,ITF::String8>::const_iterator it;

    ITF::String8 current_tag;

	for (it = m_trackingData.begin() ; it != m_trackingData.end() ; ++it)
	{
		current_tag.setTextFormat("&%s=%s" , (*it).first.cStr(),(*it).second.cStr());
		tag_value += current_tag;
	}

	if (ONLINETRACKING_ADAPTER)
	{
		ONLINETRACKING_ADAPTER->sendTag( tag_key , tag_value.cStr() );
	}
}

void Ray_OnlineTrackingManager::sendSignal(const char* signalName)
{
    if (ONLINETRACKING_ADAPTER)
    {
        ONLINETRACKING_ADAPTER->sendSignal(signalName);
    }
}

void Ray_OnlineTrackingManager::update(f32 _dt)
{

	u32 numplayers = GAMEMANAGER->getNumActivePlayers();

	if (GAMEMANAGER->isInPause()==bfalse && RAY_GAMEMANAGER->getCurrentGameScreen() == m_gamePlayId && numplayers>0)
	{

		// post launch tracking /////////////////////////////////////////

		
	
		m_PlayerIntervalTime					+= _dt;
		m_PlayTimeForPlayerIndex[numplayers-1]	+= _dt;
		m_currentLevelTime						+= _dt;
		m_persistantTimer						+= _dt;
        m_absolutePlayTime                      += _dt;

		if (m_persistantTimer>5.0f)
		{

			Ray_PersistentGameData_UniverseTracking *pUniverseData = RAY_GAMEMANAGER->getPersistentGameData()->getTrackingData();

			if (pUniverseData)
			{

				for (u32 i=0;i<4;i++)
				{
					pUniverseData->setTimer(	m_PlayTimeForPlayerIndex[i],i);
					
				}

				Ray_PersistentGameData_UniverseTracking *pUniverseData = RAY_GAMEMANAGER->getPersistentGameData()->getTrackingData();


				if (pUniverseData)
				{
					pUniverseData->setPafCounter(m_Paf2Players,0);
					pUniverseData->setPafCounter(m_Paf3Players,1);
					pUniverseData->setPafCounter(m_Paf4Players,2);
				}


                if (ONLINETRACKING_ADAPTER)
                {
                    u32 playTime = (u32)floorf(m_absolutePlayTime);
                    ONLINETRACKING_ADAPTER->updatePlayTime(playTime);
                }

				//LOG("save persistant timers %.1f",m_PlayTimeForPlayerIndex[0] );
			}

			m_persistantTimer = 0.0f;

		}

		if (m_PlayerIntervalTime > PLAYER_INTERVAL_TIME)
		{
            SendPlayerIntervalTag();
		}


		// $GS: player.position since is a dangerous high freq event; skip it.
        // https://confluence.ubisoft.com/display/UbisoftDataOffice/player.position
        // "Can only be used in Prod if set in a very low sampling Population (under 1%)."
#if defined(TRACKING_PLAYER_POS)
		if (CONFIG->m_enableonlineTracking==btrue)
			{

			m_OnlineTrackingTimer				+= _dt;
			m_timeElapsedSinceLastCheckPoint	+= _dt;

			if (m_OnlineTrackingTimer> 0.2f)
			{

					m_OnlineTrackingTimer = 0.f;

					u32 currentEngineVersion = Versioning::getEngineVersion();

					String level_name,str;
					Vec3d pos_player[4];
					u32 bubble_player[4];

					if (CURRENTWORLD!=NULL)
					{
						if (CURRENTWORLD->getRootScene()!=NULL)
						{
							CURRENTWORLD->getRootScene()->getPath().getString(level_name);
						}
					}

					Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

					ITF_STDSTRING checkPointStr = "";

					if ( checkpoint )
					{
							ObjectPath path;
							SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);
							path.toString(checkPointStr);
					}

					u32 num_player = GAMEMANAGER->getNumActivePlayers();

					for (int i=0;i<4;i++)
					{

						Player *player = GAMEMANAGER->getPlayer(i);

						if (player->getActive()==btrue &&  player->getActor())//the player could not have an actor associated
						{

							pos_player[i]    = player->getActor()->getPos();
							bubble_player[i] = (player->isDead())?1:0;
						}
						else
						{
							bubble_player[i] = 0;
						}
					}

					str.setTextFormat( "VERSION=%d&LEVELNAME=%s&CHECKPOINT=%s&NUMPLAYERS=%d&P1BBLE=%d&P2BBLE=%d&P3BBLE=%d&P4BBLE=%d&P1X=%.1f&P1Y=%.1f&P1Z=%.1f&P2X=%.1f&P2Y=%.1f&P2Z=%.1f&P3X=%.1f&P3Y=%.1f&P3Z=%.1f&P4X=%.1f&P4Y=%.1f&P4Z=%.1f"	,
						currentEngineVersion,
                        StringToUTF8(level_name).get(),
						checkPointStr.c_str(),
						num_player,
						bubble_player[0]     , bubble_player[1]     , bubble_player[2]     , bubble_player[3],
						pos_player[0].getX() , pos_player[0].getY() , pos_player[0].getZ() ,
						pos_player[1].getX() , pos_player[1].getY() , pos_player[1].getZ() ,
						pos_player[2].getX() , pos_player[2].getY() , pos_player[2].getZ() ,
						pos_player[3].getX() , pos_player[3].getY() , pos_player[3].getZ() );
                
					StringConverter c(str);
                    
					if (ONLINETRACKING_ADAPTER)
					{
						ONLINETRACKING_ADAPTER->sendTag("PLAYER_POS", c.getChar());
					}

			}
			}
#endif
	}

}

void Ray_OnlineTrackingManager::onGameRestart(bbool _ReStartedByPlayers)
{

	if (_ReStartedByPlayers==btrue)
	{
	    // RESTART BY GAME_OVER not send in post launch mode
		onReachedCheckpoint(CP_RestartLevel);
	}

	if(!ONLINETRACKING_ADAPTER)
        return;

	if (CONFIG->m_enableonlineTracking==btrue)
	{


    PathString_t levelName;

	if (CURRENTWORLD!=NULL)
	{
		if (CURRENTWORLD->getRootScene()!=NULL)
		{
			CURRENTWORLD->getRootScene()->getPath().getString(levelName);
		}
	}
    
    Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();
    ITF_STDSTRING checkPointStr = "";

    if ( checkpoint )
    {
        ObjectPath path;
        SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);
        path.toString(checkPointStr);
    }

    String str;
    str.setTextFormat("VERSION=%d&NUMPLAYERS=%d&LEVELNAME=%s&CHECKPOINT=%s&REASON=%s&DURATION=%.1f", 
        Versioning::getEngineVersion(),
        GAMEMANAGER->getNumActivePlayers(),
        StringToUTF8(levelName).get(),
        checkPointStr.c_str(),
        _ReStartedByPlayers ? "RESTART" : "GAMEOVER",
        m_timeElapsedSinceLastCheckPoint
        );

    char* tag = str.getCharCopy();
        
    ONLINETRACKING_ADAPTER->sendTag("LEVEL_RESTART", tag);

    SF_DEL_ARRAY(tag);

    resetStats(bfalse);

	}

}


void Ray_OnlineTrackingManager::AwardUnlock(ITF::u32 awardIdu32)
{
	startNewTag();

    addUint32("awardId" , awardIdu32);
	addUint32("totalPlaytime"   , (int)floorf(m_PlayTimeForPlayerIndex[0]+m_PlayTimeForPlayerIndex[1]+m_PlayTimeForPlayerIndex[2]+m_PlayTimeForPlayerIndex[3]));
	sendTag("AWARD_UNLOCK",bfalse);

	m_awardUnlockCount++;

    // player.progression.achievementUnlock
    String8 progressValue;
    progressValue.setTextFormat("%s/%s", awardIdu32, REWARD_ADAPTER->getRewardsCount());
    addString("progressionType", "UplayActionUnlock");
    addString("progressionValue", progressValue);
    sendSignal("AchievementUnlock");

	SavePersistant();

}

void Ray_OnlineTrackingManager::SendPlayerIntervalTag()
{

	u32 num_players = GAMEMANAGER->getNumActivePlayers();

    if (GAMEMANAGER->isInPause() == bfalse && RAY_GAMEMANAGER->getCurrentGameScreen() == m_gamePlayId && num_players > 0)
    {
        // dont send in menus etc....

        //LOG("sending PlayerInterval m_PlayerIntervalTime : %.1f", m_PlayerIntervalTime);

        startNewTag();

        u32 slotId = RAY_GAMEMANAGER->getCurrentSlotIndex();

        // can overflow but it's non critical tracking
        u32 totalPlaytime = (u32)floorf(m_PlayTimeForPlayerIndex[0] + m_PlayTimeForPlayerIndex[1] + m_PlayTimeForPlayerIndex[2] + m_PlayTimeForPlayerIndex[3]);
        addUint32("numPlayers", num_players);
        addUint32("totalPlaytime", totalPlaytime);
        addUint32("player1Playtime", (i32)floorf(m_PlayTimeForPlayerIndex[0]));
        addUint32("player2Playtime", (i32)floorf(m_PlayTimeForPlayerIndex[1]));
        addUint32("player3Playtime", (i32)floorf(m_PlayTimeForPlayerIndex[2]));
        addUint32("player4Playtime", (i32)floorf(m_PlayTimeForPlayerIndex[3]));

        addUint32("paf2Players", m_Paf2Players);
        addUint32("paf3Players", m_Paf3Players);
        addUint32("paf4Players", m_Paf4Players);

        addUint32("slot", slotId);

        sendTag("PLAYER_INTERVAL", bfalse);

        sendSignal("PlayerInterval");

        m_playerIntervalCount++;
        m_PlayerIntervalTime = 0.0f;
    }

	//SavePersistant();
	
}


void Ray_OnlineTrackingManager::SavePersistant()
{

	Ray_PersistentGameData_UniverseTracking *pUniverseData = RAY_GAMEMANAGER->getPersistentGameData()->getTrackingData();

	if (pUniverseData)
	{
		for (u32 i=0;i<4;i++)
		{
			pUniverseData->setTimer(	m_PlayTimeForPlayerIndex[i],i);
			//LOG("ONLINE TRACKING : setting timer for player %d : %.1f", i, m_PlayTimeForPlayerIndex[i]);
		}

		pUniverseData->setPafCounter(m_Paf2Players,0);
		pUniverseData->setPafCounter(m_Paf3Players,1);
		pUniverseData->setPafCounter(m_Paf4Players,2);

	}

		
	Ray_PersistentGameData_LevelTracking *pLevelData = RAY_GAMEMANAGER->getCurrentLevelData()->getTrackingData();

	if (pLevelData)
	{			
		pLevelData->updateRunCount(m_runCount - pLevelData->getRunCount() );
		pLevelData->updateChallengeTimeAttackCount(m_challengeTimeAttackCount - pLevelData->getChallengeTimeAttackCount() );
	}


}

};

/// post launch

#endif // ITF_SUPPORT_ONLINETRACKING
