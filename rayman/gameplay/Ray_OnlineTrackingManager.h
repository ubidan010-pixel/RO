#ifndef ITF_RAYONLINETRACKING_MANAGER_H
#define ITF_RAYONLINETRACKING_MANAGER_H

#define PLAYER_INTERVAL_TIME				600.0f

#ifdef ITF_SUPPORT_ONLINETRACKING

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H

namespace ITF
	{
	class Ray_OnlineTrackingManager 
	{

	public:

															Ray_OnlineTrackingManager();
		virtual												~Ray_OnlineTrackingManager() {};

		void												update(f32 _dt);

		void												ResetDeathCountTimers(bool bResetCheckPointDataOnly);
		void                                                resetStats(bool bResetCheckPointDataOnly);

        enum CheckPointTag
        {
            CP_Normal = 0,
            CP_EndOfLevel,
            CP_ExitLevel,
			CP_RestartLevel,
			CP_EndOfTimeAttack
        }; 

		enum LevelCompletionTag
		{
				LEVEL_NEVER_COMPLETED = 0,
				LEVEL_ALREADY_COMPLETED
		};

		void												onReachedCheckpoint(CheckPointTag _CP_Tag = CP_Normal);
		void												onAttack();
		void												onHitExit();

		void												OnlineTrackingTrackDeath(ITF::u32 playerIndex);

		
		void												startNewTag();


        void                                                onGameRestart(bbool _ReStartedByPlayers);

        ITF_INLINE void										addString(const ITF::String8& key, const ITF::String8& value)
		{
			m_trackingData[key] = value;
		};

		ITF_INLINE void										addUint32(const ITF::String8& key,ITF::u32	    value)
		{
			ITF::String8 data;
			data.setTextFormat("%d",value);
			m_trackingData[key] = data.cStr();
		};

		

		ITF_INLINE void										addf32(const ITF::String8& key,ITF::f32	    value)
		{
			ITF::String8 data;
			data.setTextFormat("%.3f",value);
			m_trackingData[key] = data.cStr();
		};

		
		void												sendTag(const char* tag_key,bbool sendVersionTag= btrue);


		/// post launch										//////////////////////////////////


		void												SetPlayTimeFromSaveSlot();
		void												SendPlayerIntervalTag();
		void												SendLevelStop(CheckPointTag _CP_Tag);
		void												ResetPlayTimes();
		void												SavePersistant();
		void												AwardUnlock(ITF::u32 awardIdu32);


		// call this when lumStage has just been unlocked			/////

		void												onUnLockLumStage1();
		void												onUnLockLumStage2();

 
	public:

		// checkpoint info//////////////////////////////////////////////////////////////////////
		u32													m_deathCountSinceLastCheckPoint[4];
		f32													m_timeElapsedSinceLastCheckPoint;
		u32													m_diamondCollected;
		u32													m_jumpCount;
		u32													m_attackCount;
		u32													m_checkPointOrderSinceLevelBegin;

		// level info//////////////////////////////////////////////////////////////////////
		u32													m_LdeathCountSinceLastCheckPoint[4];
		u32													m_LdiamondCollected;
		u32													m_LjumpCount;
		u32													m_LattackCount;
		f32													m_fLevelTime;
		


		u32													m_lastPaf;
		u32													m_lastOffscreen;


		// post launch										/////////////////////////


		

		f32													m_PlayTimeForPlayerIndex[4];

		f32													m_currentLevelTime;
		u32													m_currentLevelGameOverCount;

		u32													m_Paf2Players;
		u32													m_Paf3Players;
		u32													m_Paf4Players;

		u32													m_playerIntervalCount;
		u32													m_awardUnlockCount;
		u32													m_levelStopCount;

		u32													m_runCount;
		u32													m_challengeTimeAttackCount;


		u32													m_lumStage1Count;
		u32													m_lumStage2Count;


		f32													m_timeAttackTime;

		i32													m_finalBrokenCageIndex;
		i32													m_hiddenBrokenCageIndex1;
		i32													m_hiddenBrokenCageIndex2;


		StringID											m_gamePlayId;


		bbool												m_TimeAttackAvailableForThisMap;

		std::map<ITF::String8,u32>							m_firstCompMap;
		
	private:

		f32													m_OnlineTrackingTimer; //sendTag every 0.2 s

		std::map<ITF::String8,ITF::String8>					m_trackingData;
		

		f32													m_persistantTimer; // save date each 5.0 seconds


		f32													m_PlayerIntervalTime;
	};

};

#endif // ITF_SUPPORT_ONLINETRACKING

#endif 