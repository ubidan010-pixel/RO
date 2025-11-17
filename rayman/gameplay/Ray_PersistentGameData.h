#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#define _ITF_RAY_PERSISTENTGAMEDATA_H_

#ifndef _ITF_PERSISTENTGAMEDATA_H_
#include "gameplay/managers/PersistentGameData.h"
#endif

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

//CASA::VITA::TARIK::FRESCO GAMEPLAY
#define MAX_GAME_RELIC			91
#define MAX_GAME_RELIC_FRESCO1	43
#define MAX_GAME_RELIC_FRESCO2	48
#define MAX_FRESCO_RELIC		30
#define NB_FRESCO				2
#define NB_SAVESLOT				1

#define MAX_DUPLICATE_RELIC		1//5

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    class AbsoluteObjectPath
    {
        DECLARE_SERIALIZE()

    public:
        AbsoluteObjectPath();
        AbsoluteObjectPath(const Actor * const _actor, const StringID & _levelFriendly);
        AbsoluteObjectPath(const PackedObjectPath & _packedObjectPath, const StringID & _levelFriendly);
        ITF_INLINE bool operator == (const AbsoluteObjectPath & _other) const
        {
            return (m_packedObjectPath == _other.m_packedObjectPath) && (m_levelCRC == _other.m_levelCRC);
        }
        ITF_INLINE bool operator != (const AbsoluteObjectPath & _other) const
        {
            return !(*this == _other);
        }
        bbool isValid() const 
        {
            return m_packedObjectPath.isValid() && m_levelCRC != U32_INVALID;
        }

    private:
        PackedObjectPath m_packedObjectPath;
        u32 m_levelCRC;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_ISD : public PersistentGameData_ISD
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PersistentGameData_ISD,PersistentGameData_ISD,3883514709);
        DECLARE_SERIALIZE()
    public:
        Ray_PersistentGameData_ISD() { m_foundCageMask = 0;}
        ~Ray_PersistentGameData_ISD() {}

        virtual void clear() {m_pickedUpLums.clear(); m_foundCageMask = 0; m_takenTooth.clear(); m_alreadySeenCutScenes.clear();}

        void setTakenLum(const PackedObjectPath &_id, bbool _taken) {setIsObjectTaken(m_pickedUpLums, _id, _taken);}
        void setTakenTooth(const PackedObjectPath &_id, bbool _taken) {setIsObjectTaken(m_takenTooth, _id, _taken);}
        void setAlreadySeenCutScene(const PackedObjectPath &_id, bbool _alreadySeen) {setIsObjectTaken(m_alreadySeenCutScenes, _id, _alreadySeen);}
        void setBrokenCageFlag(u32 _cageIndex, bbool _taken) {if (_taken) m_foundCageMask |= (1<<_cageIndex); else m_foundCageMask &= ~(1<<_cageIndex);}

        bbool isLumTaken(const PackedObjectPath &_id) const {return m_pickedUpLums.find(_id)>=0;}
        bbool isToothTaken(const PackedObjectPath &_id) const {return m_takenTooth.find(_id)>=0;}
        bbool isCageBroken(u32 _cageIndex) const {return m_foundCageMask & (1<<_cageIndex);}
        bbool isCutSceneAlreadySeen(const PackedObjectPath &_id) const {return m_alreadySeenCutScenes.find(_id)>=0;}
        u32 getBrokenCageMask() const {return m_foundCageMask;}
        u32 getTakenToothCount() const {return m_takenTooth.size();}
        u32 getTakenLums() const {return m_pickedUpLums.size(); }

        const SafeArray<PackedObjectPath> &getAlreadySeenCutScenes() const {return m_alreadySeenCutScenes;}

        static void setIsObjectTaken(SafeArray<PackedObjectPath>& _objectList, const PackedObjectPath &_id, bbool _taken);

    private:

        virtual void copyFrom_Base(const PersistentGameData_ISD *_other)
        {
            Ray_PersistentGameData_ISD *other = (Ray_PersistentGameData_ISD*)_other;
            m_pickedUpLums = other->m_pickedUpLums; 
            m_takenTooth = other->m_takenTooth;
            m_alreadySeenCutScenes = other->m_alreadySeenCutScenes;
            m_foundCageMask = other->m_foundCageMask;
        }

        SafeArray<PackedObjectPath> m_pickedUpLums;
        SafeArray<PackedObjectPath> m_takenTooth;
        SafeArray<PackedObjectPath> m_alreadySeenCutScenes;
        u32 m_foundCageMask;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_LevelTracking
    {
        DECLARE_SERIALIZE()

    public:
        Ray_PersistentGameData_LevelTracking( const Ray_PersistentGameData_LevelTracking & obj ) 
        {
            m_runCount = obj.m_runCount;
            m_challengeTimeAttackCount = obj.m_challengeTimeAttackCount;
            m_challengeHidden1          = obj.m_challengeHidden1;
            m_challengeHidden2          = obj.m_challengeHidden2;
            m_challengeCage             = obj.m_challengeCage;
            m_challengeLumsStage1       = obj.m_challengeLumsStage1;
            m_challengeLumsStage2       = obj.m_challengeLumsStage2;
            m_firstTimeLevelCompleted   = obj.m_firstTimeLevelCompleted;
        }

        Ray_PersistentGameData_LevelTracking()
            : m_runCount (0)
            , m_challengeTimeAttackCount(0) 
            , m_firstTimeLevelCompleted(0.0f)
            , m_challengeLumsStage1(0)
            , m_challengeLumsStage2(0)
            , m_challengeCage(0)
            , m_challengeHidden1(0)
            , m_challengeHidden2(0)
        {

        }

        ~Ray_PersistentGameData_LevelTracking() {}

        ITF_INLINE void updateChallengeLumsStage2(u32 countToAdd = 1) { m_challengeLumsStage2 += countToAdd; }
        ITF_INLINE u32  getChallengeLumsStage2() const { return m_challengeLumsStage2; }

        ITF_INLINE void updateChallengeLumsStage1(u32 countToAdd = 1) { m_challengeLumsStage1 += countToAdd; }
        ITF_INLINE u32  getChallengeLumsStage1() const { return m_challengeLumsStage1; }

        ITF_INLINE void setFirstTimeLevelCompleted(f32 time) { m_firstTimeLevelCompleted = time; }
        ITF_INLINE f32  getFirstTimeLevelCompleted() const { return m_firstTimeLevelCompleted; }

        ITF_INLINE void updateChallengeHidden1(u32 countToAdd = 1) { m_challengeHidden1 += countToAdd; }
        ITF_INLINE u32  getChallengeHidden1() const { return m_challengeHidden1; }

        ITF_INLINE void updateChallengeHidden2(u32 countToAdd = 1) { m_challengeHidden2 += countToAdd; }
        ITF_INLINE u32  getChallengeHidden2() const { return m_challengeHidden2; }

        ITF_INLINE void updateChallengeCage(u32 countToAdd = 1) { m_challengeCage += countToAdd; }
        ITF_INLINE u32  getChallengeCage() const { return m_challengeCage; }

        ITF_INLINE void updateRunCount(u32 countToAdd = 1) { m_runCount += countToAdd; }
        ITF_INLINE u32  getRunCount() const { return m_runCount; }

        ITF_INLINE void updateChallengeTimeAttackCount(u32 countToAdd = 1) { m_challengeTimeAttackCount += countToAdd; }
        ITF_INLINE u32  getChallengeTimeAttackCount() const { return m_challengeTimeAttackCount; }

    private:
        u32 m_runCount;
        u32 m_challengeTimeAttackCount;
        u32 m_challengeHidden1, m_challengeHidden2;
        u32 m_challengeCage;
        u32 m_challengeLumsStage1, m_challengeLumsStage2;
        f32 m_firstTimeLevelCompleted;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_UniverseTracking
    {
        DECLARE_SERIALIZE()

    public:

        ~Ray_PersistentGameData_UniverseTracking(){}

        void clear()
        {
            m_timers.clear();
            m_pafCounter.clear();
        }
        Ray_PersistentGameData_UniverseTracking()
        {
            m_timers.resize(s_maxPlayers);
            m_pafCounter.resize(s_maxPlayers);
            for (u32 i = 0; i < s_maxPlayers; i++)
            {
                m_timers[i] = 0.0f;
                m_pafCounter[i] = 0;
            }
        }

        void setFrom( const Ray_PersistentGameData_UniverseTracking& _other )
        {            
            for (u32 i = 0; i < s_maxPlayers; i++)
            {
                setPafCounter(_other.getPafCounter(i), i);
                setTimer(_other.getTimer(i), i);
            }
        }

        ITF_INLINE void updateTimer(f32 timeToAddInSec, u32 playerIndex) { m_timers[playerIndex] += timeToAddInSec; }
        ITF_INLINE void setTimer(f32 newTimer, u32 playerIndex) { m_timers[playerIndex] = newTimer; }
        ITF_INLINE f32  getTimer(u32 playerIndex) const { ITF_ASSERT(playerIndex<s_maxPlayers); return m_timers[playerIndex]; }

        ITF_INLINE void updatePafCounter(u32 pafToAdd, u32 playerIndex) { m_pafCounter[playerIndex] += pafToAdd; }
        ITF_INLINE void setPafCounter(u32 newPafCount, u32 playerIndex) { m_pafCounter[playerIndex] = newPafCount; }
        ITF_INLINE u32  getPafCounter(u32 playerIndex) const { ITF_ASSERT(playerIndex<s_maxPlayers); return m_pafCounter[playerIndex]; }

    private:
        SafeArray<f32, s_maxPlayers> m_timers;
        SafeArray<u32, s_maxPlayers> m_pafCounter;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_Level : public PersistentGameData_Level
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PersistentGameData_Level,PersistentGameData_Level,2267624090);
        DECLARE_SERIALIZE()
    public:
        Ray_PersistentGameData_Level() : m_wonChallenges(0), m_levelState(SPOT_STATE_CLOSED), m_bestTimeAttack(U32_INVALID), m_bestLumAttack(0), m_foundRelicMask(0), m_hasWarning(bfalse), m_isSkipped(bfalse) {}
        ~Ray_PersistentGameData_Level() {}

        virtual PersistentGameData_ISD *createNewISD() {return newAlloc(mId_Gameplay, Ray_PersistentGameData_ISD);}

        Ray_PersistentGameData_ISD *getRayISD(const Path &_path) {return (Ray_PersistentGameData_ISD*)getISD(_path);}
        Ray_PersistentGameData_ISD *getRayISDConst(const Path &_path) const {return (Ray_PersistentGameData_ISD*)getISDConst(_path);}

        void setTakenLum(const Path &_path, const PackedObjectPath &_id, bbool _taken) {getRayISD(_path)->setTakenLum(_id, _taken);}
        void setTakenTooth(const Path &_path, const PackedObjectPath &_id, bbool _taken) {getRayISD(_path)->setTakenTooth(_id, _taken);}
        void setAlreadySeenCutScene(const Path &_path, const PackedObjectPath &_id, bbool _alreadySeen) {getRayISD(_path)->setAlreadySeenCutScene(_id, _alreadySeen);}
        void setBrokenCageFlag(const Path &_path, u32 _index, bbool _taken) {getRayISD(_path)->setBrokenCageFlag(_index, _taken);}
        void setCageMapPassedDoor(const PackedObjectPath &_objectPath, bbool _taken) {Ray_PersistentGameData_ISD::setIsObjectTaken(m_cageMapPassedDoors, _objectPath, _taken);}
        void setLevelState(const ESpot_State _state) { m_levelState = _state; }
        void setChallengeWon(EChallengeType _challenge) {m_wonChallenges |= _challenge;}
        void setBestTimeAttack( u32 _time ) { if ( m_bestTimeAttack == U32_INVALID || _time < m_bestTimeAttack ) m_bestTimeAttack = _time; }
        void setBestLumAttack(u32 _lum) {m_bestLumAttack = _lum;}
        void setLevelHasWarning(bbool _hasWarning) { m_hasWarning = _hasWarning; }
        void setLevelIsSkipped(bbool _isSkipped) { m_isSkipped = _isSkipped; }

        bbool isLumTaken(const Path &_path, const PackedObjectPath &_id) const {Ray_PersistentGameData_ISD *isd = getRayISDConst(_path); if (isd) return isd->isLumTaken(_id); else return bfalse;}
        bbool isToothTaken(const Path &_path, const PackedObjectPath &_id) const {Ray_PersistentGameData_ISD *isd = getRayISDConst(_path); if (isd) return isd->isToothTaken(_id); else return bfalse;}
        bbool isCageBroken(u32 _index) const {return getBrokenCageMask() & (1<<_index);}
        bbool isCutSceneAlreadySeen(const Path &_path, const PackedObjectPath &_id) const {Ray_PersistentGameData_ISD *isd = getRayISDConst(_path); if (isd) return isd->isCutSceneAlreadySeen(_id); else return bfalse;}
        bbool didPassDoorToCageMap(const PackedObjectPath &_objectPath) {return m_cageMapPassedDoors.find(_objectPath)>=0;}
        ESpot_State   getLevelState() { return m_levelState; }
        bbool isChallengeWon(EChallengeType _challenge) const {return (m_wonChallenges & _challenge)!=0;}
        u32 getBestTimeAttack() const { return m_bestTimeAttack; }
		u32 getBestLumAttack() const {return m_bestLumAttack;}

		void	setRelicTaken(u32 _relicIndex, bbool _taken)
        {
            if (_taken)
                m_foundRelicMask |= 1<<_relicIndex;
            else
                m_foundRelicMask &= ~(1<<_relicIndex);
        }
		bbool	isRelicTaken(u32 _relicIndex) const
        {
            return m_foundRelicMask&(1<<_relicIndex);
        }
		u32		getTakenRelicMask() const {return m_foundRelicMask;}
		u32		getTakenRelicCount() const {return countBits(m_foundRelicMask);}

        bbool getLevelHasWarning() const { return m_hasWarning; }
        bbool getLevelIsSkipped() const { return m_isSkipped; }

        void setFrom(const Ray_PersistentGameData_Level *_other, bbool _mergeCinematicList=bfalse, bbool _copyFoundRelicMask = btrue);
		void setFoundRelicMaskFrom(const Ray_PersistentGameData_Level *_other);
		       
        u32 getBrokenCageMask() const;

        u32 getBrokenCageCount() const {return countBits(getBrokenCageMask());}

        u32 getTakenToothCount() const;

        u32 getTakenLumCount(const Path &_path) { return getRayISD(_path)->getTakenLums(); }

        u32 computeElectoonCount() const;


        Ray_PersistentGameData_LevelTracking* getTrackingData() { return &m_trackingData; }

    private:
        static u32 countBits(u32 _mask);

        SafeArray<PackedObjectPath> m_cageMapPassedDoors; // we may have unloaded the ISD when passing the door, so it's easier to store this list here
        u32 m_wonChallenges;
        u32 m_bestTimeAttack;
        u32 m_bestLumAttack;
		u32 m_foundRelicMask;

        Ray_PersistentGameData_LevelTracking m_trackingData;

        virtual void copyFrom_Base(const PersistentGameData_Level *_other, bbool _copyFoundRelicMask = btrue) 
        {
            PersistentGameData_Level::copyFrom_Base(_other);
            Ray_PersistentGameData_Level *rayLevel = (Ray_PersistentGameData_Level*)_other;
            m_cageMapPassedDoors = rayLevel->m_cageMapPassedDoors;
            m_wonChallenges = rayLevel->m_wonChallenges;
            m_levelState = rayLevel->m_levelState;
            m_bestTimeAttack = rayLevel->m_bestTimeAttack;
            m_bestLumAttack = rayLevel->m_bestLumAttack;
            m_hasWarning = rayLevel->m_hasWarning;
            m_isSkipped = rayLevel->m_isSkipped;
            m_trackingData = rayLevel->m_trackingData;
			if(_copyFoundRelicMask)
			{
				m_foundRelicMask = rayLevel->m_foundRelicMask;
			}
        }

        ESpot_State m_levelState;
        bbool       m_hasWarning;
        bbool       m_isSkipped;

    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_Fresco //CASA::VITA::TARIK::FRESCO GAMEPLAY
    {
        DECLARE_SERIALIZE()

    public:

		///////////////////////////////////////////////////////////////////////////////////////////
		struct RelicInfo
		{
			DECLARE_SERIALIZE()
		public:
			RelicInfo()
				: m_type(RELIC_TYPE_NONE),m_status(RELIC_STATUS_LOCKED)
			{}

			RelicType m_type;
			RelicStatus m_status;
		};

		///////////////////////////////////////////////////////////////////////////////////////////
		struct FrescoInfo
		{
			DECLARE_SERIALIZE()

		public:

			FrescoInfo()
			{
				m_relicInfo.resize(MAX_FRESCO_RELIC);
				ReinitRelicsInfo();
			}

			void ReinitRelicsInfo()
			{
				m_inGameRelicCount = 0;
				for (u32 i = 0; i < MAX_FRESCO_RELIC; i++)
				{
					m_relicInfo[i].m_status = RELIC_STATUS_LOCKED;
					m_relicInfo[i].m_type = RELIC_TYPE_NONE;
				}
			}

			void setFrom( const FrescoInfo& _other )
			{
				ITF_ASSERT_CRASH(m_relicInfo.size() == _other.m_relicInfo.size(), "wrong Relic size");
				SetInGameRelicCount(_other.GetInGameRelicCount());
				for (u32 i = 0; i < MAX_FRESCO_RELIC; i++)
				{
					SetRelicStatus(i, _other.GetRelicStatus(i));
					SetRelicType(i, _other.GetRelicType(i));
				}
			}

			ITF_INLINE const RelicStatus GetRelicStatus(u32 _index) const
			{
				return m_relicInfo[_index].m_status;
			}			
			
			ITF_INLINE void SetRelicStatus(u32 _index, RelicStatus _status)
			{
				m_relicInfo[_index].m_status = _status;
			}

			ITF_INLINE const RelicType GetRelicType(u32 _index) const
			{
				return m_relicInfo[_index].m_type;
			}

			ITF_INLINE void SetRelicType(u32 _index, RelicType _type)
			{
				m_relicInfo[_index].m_type = _type;
			}

			ITF_INLINE const u32 GetInGameRelicCount() const
			{
				return m_inGameRelicCount;
			}

			ITF_INLINE void SetInGameRelicCount(u32 _count)
			{
				m_inGameRelicCount = _count;
			}

			ITF_INLINE void UpdateInGameRelicCount(u32 _count)
			{
				m_inGameRelicCount += _count;
			}

			u32 GetUnlockedRelicCount(RelicType _type)
			{
				u32 unlockedRelicCount = 0;
				for(u32 i = 0; i < MAX_FRESCO_RELIC; i++) 
				{
					if(m_relicInfo[i].m_status==RELIC_STATUS_UNLOCKED)
					{
						if(m_relicInfo[i].m_type == _type)
						{	
							unlockedRelicCount++;
						}	
					}
				}
				return unlockedRelicCount;
			}

			u32 GetUnlockedRelicCount()
			{
				u32 unlockedRelicCount = 0;
				for(u32 i = 0; i < MAX_FRESCO_RELIC; i++) 
				{
					if(m_relicInfo[i].m_status==RELIC_STATUS_UNLOCKED)
					{
						unlockedRelicCount++;
					}
				}
				return unlockedRelicCount;
			}

			u32 GetDuplicateCount()
			{
				u32 duplicateCount = (GetInGameRelicCount() - GetUnlockedRelicCount());
				return duplicateCount;
			}

			void	DebugFrescoData()
			{
				LOG("\n -----------inGameRelicCount %d ----------- ",m_inGameRelicCount);
				for (u32 i = 0; i < MAX_FRESCO_RELIC; i++)
				{
					if(m_relicInfo[i].m_status == RELIC_STATUS_UNLOCKED)
					{
						if(m_relicInfo[i].m_type == RELIC_TYPE_INGAME)
						{
							LOG("\n -----------Relic ID %d = UNLOCKED ----- From ------ GAME",i+1);
						}
						else if(m_relicInfo[i].m_type == RELIC_TYPE_NONE)
						{
							LOG("\n -----------Relic ID %d = UNLOCKED ----- From ------ NONE",i+1);
						}
					}
				}
			}

		private:
			u32 m_inGameRelicCount;
			SafeArray<RelicInfo, MAX_FRESCO_RELIC> m_relicInfo;
		};

 		///////////////////////////////////////////////////////////////////////////////////////////
       Ray_PersistentGameData_Fresco()
        {
            m_frescoInfo.resize(NB_FRESCO);
			ReinitAllInfo();			
        }

        void setFrom( const Ray_PersistentGameData_Fresco& _other )
        {
            ITF_ASSERT_CRASH(m_frescoInfo.size() == _other.m_frescoInfo.size(), "wrong frescoInfo size");
            for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
            {
				m_frescoInfo[frescoId].setFrom(_other.m_frescoInfo[frescoId]);
            }  
		}

		RelicStatus GetRelicStatus(u32 _frescoId,u32 _relicIndex)
		{
			return m_frescoInfo[_frescoId].GetRelicStatus(_relicIndex);
		}

		void SetRelicStatus(u32 _frescoId,u32 _relicIndex,RelicStatus _relicStatus)
		{
			m_frescoInfo[_frescoId].SetRelicStatus(_relicIndex,_relicStatus);
		}

		u32 GetInGameRelicCount()
		{
			u32 inGameRelicCount = 0;
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				inGameRelicCount += m_frescoInfo[frescoId].GetInGameRelicCount();
			} 
			return inGameRelicCount;
		}

		u32 GetUnlockedRelicCount()
		{
			u32 unlockedRelicCount = 0;
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				unlockedRelicCount += m_frescoInfo[frescoId].GetUnlockedRelicCount();
			}
			return unlockedRelicCount;
		}

		u32 GetUnlockedRelicCount(RelicType _type)
		{
			u32 unlockedRelicCount = 0;
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				unlockedRelicCount += m_frescoInfo[frescoId].GetUnlockedRelicCount(_type);
			} 
			return unlockedRelicCount;
		}

		u32 GetDuplicateCount()
		{
			u32 duplicateCount = 0;
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				duplicateCount += m_frescoInfo[frescoId].GetDuplicateCount();
			}
			return duplicateCount;
		}

		void ReinitAllInfo()
		{
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				m_frescoInfo[frescoId].ReinitRelicsInfo();
			}
		}

		void UnlockRelic(u32 _frescoId,u32 _relicId,RelicType _relicType)
		{	
			m_frescoInfo[_frescoId].SetRelicStatus(_relicId,RELIC_STATUS_UNLOCKED);
			m_frescoInfo[_frescoId].SetRelicType(_relicId,_relicType);
			//if(_relicType == RELIC_TYPE_NEAR)
			//	m_frescoInfo[_frescoId].UpdateNearRelicCount(1);
			//else
				m_frescoInfo[_frescoId].UpdateInGameRelicCount(1);
		}

		void UnlockAllRelics(RelicType _relicType)
		{
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				for(u32 relicId = 0; relicId < MAX_FRESCO_RELIC; relicId++) 
				{
					UnlockRelic(frescoId,relicId,_relicType);
				}
			}
		}


		bbool IsFrescoUnlocked(u32 _frescoId)
		{
			if(m_frescoInfo[_frescoId].GetUnlockedRelicCount()==MAX_FRESCO_RELIC)
				return btrue;

			return bfalse;
		}

		u32 GetMaxInGameRelics(u32 _frescoId)
		{
			if(_frescoId==0)
				return MAX_GAME_RELIC_FRESCO1;
			else
				return MAX_GAME_RELIC_FRESCO2;
		}

		u32			GetDebugFrescoId();
		RelicState	UpdateInGameRelicsInfo(u32 _frescoId);
		
#ifdef ITF_SUPPORT_CHEAT
		void	DebugRelicsInfo();
		void	DebugFrescoData()
		{
			for (u32 frescoId = 0; frescoId < NB_FRESCO; frescoId++)
			{
				LOG("\n\n Fresco[%d] --------------------------------------------------",frescoId);
				LOG("\n -----------UnlockedRelicCount = %d -----------DuplicateCount = %d",m_frescoInfo[frescoId].GetUnlockedRelicCount(),m_frescoInfo[frescoId].GetDuplicateCount());
				m_frescoInfo[frescoId].DebugFrescoData();
			}
		}
#endif
    private:
		u32			GetAutoTakenRelicId(u32 _frescoId,RelicState _nextRelicState,u32 _unlockedRelicCount);
		RelicState	GetNextRelicState(u32 _frescoId,ITF_VECTOR<Vec2d> _newTargetPercentageList);
		u32			GetAllDuplicateCount();

    private:
		SafeArray<FrescoInfo, NB_FRESCO> m_frescoInfo;
    };

	///////////////////////////////////////////////////////////////////////////////////////////
	class Ray_PersistentFrescoData
	{
		DECLARE_SERIALIZE()

	public:
		Ray_PersistentFrescoData()
		{
			m_frescoSlot.resize(NB_SAVESLOT);
			m_relicDuplicateCount.resize(MAX_FRESCO_RELIC*NB_FRESCO);
			clear();
		}

		void clear()
		{
			for (u32 i = 0; i < NB_SAVESLOT; i++)
			{
				m_frescoSlot[i] = Ray_PersistentGameData_Fresco();
			}

			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				m_relicDuplicateCount[relicId] = 0;
			}
		}

		void setFrom( const Ray_PersistentFrescoData& _other )
		{
			ITF_ASSERT_CRASH(m_frescoSlot.size() == _other.m_frescoSlot.size(), "wrong fresco Slot count");
			for (u32 i = 0; i < NB_SAVESLOT; i++)
			{
				setFresco(i,_other.getFresco(i));
			}			
			
			ITF_ASSERT_CRASH(m_relicDuplicateCount.size() == _other.m_relicDuplicateCount.size(), "wrong Relic Duplicate count");
			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				setRelicDuplicateCount(relicId, _other.getRelicDuplicateCount(relicId));
			}


		}

		ITF_INLINE const Ray_PersistentGameData_Fresco& getFresco(u32 _slotIndex) const { return m_frescoSlot[_slotIndex]; }
		ITF_INLINE void setFresco(u32 _slotIndex, const Ray_PersistentGameData_Fresco& _value ) { m_frescoSlot[_slotIndex] = _value; }

		ITF_INLINE u32		getRelicDuplicateCount(u32 _relicId) const	{ return m_relicDuplicateCount[_relicId];}

		void setRelicDuplicateCount(u32 _relicId, u32 _count)
		{
			if(_count < MAX_DUPLICATE_RELIC)
			{
				m_relicDuplicateCount[_relicId] = _count;
			}
			else
			{
				m_relicDuplicateCount[_relicId] = MAX_DUPLICATE_RELIC;
			}
		}

		void UpdateRelicDuplicateCount(u32 _frescoId,u32  _relicId, i32 _count,RelicType _relicType=RELIC_TYPE_NONE)
		{
			u32  relicId = _relicId + (MAX_FRESCO_RELIC*_frescoId);
			if(_count > 0)
			{
				m_relicDuplicateCount[relicId] += _count;
				if(m_relicDuplicateCount[relicId] > MAX_DUPLICATE_RELIC)
				{
					m_relicDuplicateCount[relicId] = MAX_DUPLICATE_RELIC;
				}
			}
			else if((i32)(m_relicDuplicateCount[relicId]+_count) >= 0)
			{
				m_relicDuplicateCount[relicId] += _count;
			}
			else
			{
				m_relicDuplicateCount[relicId] = 0;
			}
		}

		bbool HasAtLeastOneRelicDuplicate()
		{
			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				if( m_relicDuplicateCount[relicId])
				{
					return true;
				}
			}

			return false;
		}

		void GetDuplicateList(ITF_VECTOR< std::pair<u32,u32> >  & duplicateList)
		{
			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				if( m_relicDuplicateCount[relicId])
				{
					duplicateList.push_back(std::pair<u32,u32>(relicId,m_relicDuplicateCount[relicId]));
				}
			}
		}


		void setAllRelicsDuplicateCount(u32 _count)
		{
			u32 count = _count;

			if(count > MAX_DUPLICATE_RELIC)
			{
				count = MAX_DUPLICATE_RELIC;
			}

			//for (u32 i = 0; i < NB_SAVESLOT; i++)
			//{
			//	m_frescoSlot[i].UnlockAllRelics(RELIC_TYPE_NEAR);
			//}

			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				m_relicDuplicateCount[relicId] = count;
			}
		}

		void UnlockAllRelics(RelicType _relicType)
		{
			for (u32 i = 0; i < NB_SAVESLOT; i++)
			{
				m_frescoSlot[i].UnlockAllRelics(_relicType);
			}
		}

		u32 GetUnlockedRelicCount(RelicType _type)
		{
			u32 unlockedRelicCount = 0;
			for (u32 i = 0; i < NB_SAVESLOT; i++)
			{
				unlockedRelicCount+=m_frescoSlot[i].GetUnlockedRelicCount(_type);
			}

			return unlockedRelicCount;
		}

		u32 GetUnlockedRelicCount()
		{
			u32 unlockedRelicCount = 0;
			for (u32 i = 0; i < NB_SAVESLOT; i++)
			{
				unlockedRelicCount+=m_frescoSlot[i].GetUnlockedRelicCount();
			}

			return unlockedRelicCount;
		}

		u32			GetDebugFrescoId(u32 _slotIndex = 0){return m_frescoSlot[_slotIndex].GetDebugFrescoId();}
		RelicState	UpdateInGameRelicsInfo(u32 _frescoId,u32 _slotIndex = 0){return m_frescoSlot[_slotIndex].UpdateInGameRelicsInfo(_frescoId);}
		void		ReinitAllInfo(u32 _slotIndex = 0){m_frescoSlot[_slotIndex].ReinitAllInfo();}
		bbool		IsFrescoUnlocked(u32 _frescoId,u32 _slotIndex = 0){return m_frescoSlot[_slotIndex].IsFrescoUnlocked(_frescoId);}
		RelicStatus GetRelicStatus(u32 _frescoId,u32 _relicIndex,u32 _slotIndex = 0){return m_frescoSlot[_slotIndex].GetRelicStatus(_frescoId,_relicIndex);}

#ifdef ITF_SUPPORT_CHEAT
		void	DebugRelicsInfo(u32 _slotIndex = 0){m_frescoSlot[_slotIndex].DebugRelicsInfo();}

		void	DebugRelicsDuplicateCount()
		{
			LOG("\n\n relicDuplicateCount----------------------------");
			for (u32 relicId = 0; relicId < MAX_FRESCO_RELIC*NB_FRESCO; relicId++)
			{
				if(m_relicDuplicateCount[relicId])
					LOG("\n Relic ID %d --> DuplicateCount = %d",relicId+1,m_relicDuplicateCount[relicId]);
			}
		}

		void	DebugFrescoData(u32 _slotIndex = 0){m_frescoSlot[_slotIndex].DebugFrescoData();}
#endif


	private:
		SafeArray<Ray_PersistentGameData_Fresco, NB_SAVESLOT> m_frescoSlot;
		SafeArray<u32, MAX_FRESCO_RELIC*NB_FRESCO> m_relicDuplicateCount;
	};

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_Score
    {
        DECLARE_SERIALIZE()

    public:
        Ray_PersistentGameData_Score()
        {
            m_lumCount.resize(s_maxPlayers);
            for (u32 i = 0; i < s_maxPlayers; i++)
            {
                m_lumCount[i] = 0;
            }
        }

        void setFrom( const Ray_PersistentGameData_Score& _other )
        {
            ITF_ASSERT_CRASH(m_lumCount.size() == _other.m_lumCount.size(), "wrong player count");
            for (u32 i = 0; i < s_maxPlayers; i++)
            {
                setLums(i, _other.getLums(i));
            }
        }

        u32 getLums(u32 _playerIndex) const
        {
            return m_lumCount[_playerIndex];
        }

        void setLums(u32 _playerIndex, u32 _count)
        {
            m_lumCount[_playerIndex] = _count;
        }

    private:
        SafeArray<u32, s_maxPlayers> m_lumCount;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_WorldMap
    {
        DECLARE_SERIALIZE()

    public:
        
        struct WorldInfo
        {
            DECLARE_SERIALIZE()
            WorldInfo()
                : m_state(SPOT_STATE_CLOSED)
                , m_hasWarning(bfalse)
            {}

            ESpot_State m_state;
            bbool       m_hasWarning;
        };

        Ray_PersistentGameData_WorldMap()
            : m_currentWorld()
            , m_currentWorldTag(StringID::Invalid)
            , m_currentLevel()
            , m_currentLevelTag(StringID::Invalid)
            , m_lastElectoonCount(U32_INVALID), m_lastToothCount(U32_INVALID)
			, m_isUnlockRelicMessageDisplayed(bfalse)
        {
        }

        void clear()
        {
            m_worldsInfo.clear();

            m_currentWorld.clear();
            m_currentWorldTag.invalidate();
            m_currentLevel.invalidate();
            m_currentLevelTag.invalidate();
            m_lastElectoonCount = U32_INVALID;
            m_lastToothCount = U32_INVALID;
			m_isUnlockRelicMessageDisplayed = bfalse;
        }

        ITF_INLINE const ESpot_State    getWorldState( const StringID& _tag ) { return m_worldsInfo[_tag].m_state; }
        ITF_INLINE void                 setWorldState( const StringID& _tag, ESpot_State _state ) { m_worldsInfo[_tag].m_state = _state; }

        ITF_INLINE const bbool          getWorldHasWarning( const StringID& _tag ) { return m_worldsInfo[_tag].m_hasWarning; }
        ITF_INLINE void                 setWorldHasWarning( const StringID& _tag, bbool _hasWarning ) { m_worldsInfo[_tag].m_hasWarning = _hasWarning; }

        ITF_INLINE const ObjectPath&    getCurrentWorldObjectPath() const { return m_currentWorld; }
        ITF_INLINE const StringID&      getCurrentWorldTag() const { return m_currentWorldTag; }
        ITF_INLINE void                 setCurrentWorld( const ObjectPath& _path, const StringID& _tag ) { m_currentWorld = _path; m_currentWorldTag = _tag; }

        ITF_INLINE const ObjectPath&    getCurrentLevelObjectPath() const { return m_currentLevel; }
        ITF_INLINE const StringID&      getCurrentLevelTag() const { return m_currentLevelTag; }
        ITF_INLINE void                 setCurrentLevel( const ObjectPath& _path, const StringID& _tag ) { m_currentLevel = _path; m_currentLevelTag = _tag; }

        ITF_INLINE u32                  getLastToothCount() const {return m_lastToothCount;}
        ITF_INLINE void                 setLastToothCount(u32 _lastToothCount) {m_lastToothCount = _lastToothCount;}

        ITF_INLINE u32                  getLastElectoonCount() const {return m_lastElectoonCount;}
        ITF_INLINE void                 setLastElectoonCount(u32 _lastElectoonCount) {m_lastElectoonCount = _lastElectoonCount;}

		ITF_INLINE bbool                getIsUnlockRelicMessageDisplayed() const {return m_isUnlockRelicMessageDisplayed;}
		ITF_INLINE void                 setIsUnlockRelicMessageDisplayed(bbool _isUnlockRelicMessageDisplayed) {m_isUnlockRelicMessageDisplayed = _isUnlockRelicMessageDisplayed;}

    private:
        ITF_MAP<StringID, WorldInfo>    m_worldsInfo;
        ObjectPath                      m_currentWorld;
        StringID                        m_currentWorldTag;
        ObjectPath                      m_currentLevel;
        StringID                        m_currentLevelTag;
        u32                             m_lastElectoonCount, m_lastToothCount;
		bbool							m_isUnlockRelicMessageDisplayed;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_PersistentGameData_Universe : public PersistentGameData_Universe
    {
        //typedef PersistentGameData_Universe Super;
        DECLARE_OBJECT_CHILD_RTTI(Ray_PersistentGameData_Universe,PersistentGameData_Universe,991986087);
        DECLARE_SERIALIZE()
    public:
        Ray_PersistentGameData_Universe() :
            m_teethReturned(0),
            m_sprintTutorialDisabled(bfalse),
            m_costumeLastPrice(0)
            {}
        ~Ray_PersistentGameData_Universe() {}

        virtual PersistentGameData_Level *createNewLevel() {return newAlloc(mId_Gameplay,Ray_PersistentGameData_Level);}
        virtual void clear();

        ITF_INLINE const Ray_PersistentGameData_Score& getScore() const { return m_score; }
        ITF_INLINE void setScore( const Ray_PersistentGameData_Score& _value ) { m_score = _value; }

		ITF_INLINE const Ray_PersistentFrescoData&		getFrescoData() const { return m_frescoData; }
		ITF_INLINE Ray_PersistentFrescoData&			getFrescoData() { return m_frescoData; }
		ITF_INLINE void									UnlockAllRelics(RelicType _relicType){m_frescoData.UnlockAllRelics(_relicType);}
		ITF_INLINE void									setAllRelicsDuplicateCount(u32 _count){m_frescoData.setAllRelicsDuplicateCount(_count);}
		ITF_INLINE u32									GetUnlockedRelicCount(){return m_frescoData.GetUnlockedRelicCount();}
		ITF_INLINE u32									GetUnlockedRelicCount(RelicType _type){return m_frescoData.GetUnlockedRelicCount(_type);}
		ITF_INLINE RelicState							UpdateInGameRelicsInfo(u32 _frescoId){return m_frescoData.UpdateInGameRelicsInfo(_frescoId);}
		ITF_INLINE u32									GetDebugFrescoId(){return m_frescoData.GetDebugFrescoId();}
		ITF_INLINE u32 									getRelicDuplicateCount(u32  _relicId){return m_frescoData.getRelicDuplicateCount(_relicId);}
		ITF_INLINE void									ReinitAllInfo(){m_frescoData.ReinitAllInfo();}

#ifdef ITF_SUPPORT_CHEAT
		ITF_INLINE void									DebugRelicsInfo(){m_frescoData.DebugRelicsInfo();}
		ITF_INLINE void									DebugFrescoData(){m_frescoData.DebugFrescoData();}
		ITF_INLINE void									DebugRelicsDuplicateCount(){m_frescoData.DebugRelicsDuplicateCount();}
#endif

        ITF_INLINE const Ray_PersistentGameData_WorldMap&   getWorldMapData() const { return m_worldMapData; }
        ITF_INLINE Ray_PersistentGameData_WorldMap&         getWorldMapData() { return m_worldMapData; }
        ITF_INLINE ESpot_State       getWorldState( const StringID& _tag ) { return m_worldMapData.getWorldState(_tag); }
        ITF_INLINE void              setWorldState( const StringID& _tag, ESpot_State _state ) { m_worldMapData.setWorldState(_tag,_state); }

        ITF_INLINE bbool             getWorldHasWarning( const StringID& _tag ) { return m_worldMapData.getWorldHasWarning(_tag); }
        ITF_INLINE void              setWorldHasWarning( const StringID& _tag, bbool _hasWarning ) { m_worldMapData.setWorldHasWarning(_tag,_hasWarning); }

        ITF_INLINE void              setCurrentWorld( const ObjectPath& _path, const StringID& _tag ) { m_worldMapData.setCurrentWorld(_path, _tag); }
        ITF_INLINE const ObjectPath& getCurrentWorldObjectPath() const { return m_worldMapData.getCurrentWorldObjectPath(); }
        ITF_INLINE const StringID&   getCurrentWorldTag() const { return m_worldMapData.getCurrentWorldTag(); }

        ITF_INLINE void              setCurrentLevel( const ObjectPath& _path, const StringID& _tag ) { m_worldMapData.setCurrentLevel(_path, _tag); }
        ITF_INLINE const ObjectPath& getCurrentLevelObjectPath() const { return m_worldMapData.getCurrentLevelObjectPath(); }
        ITF_INLINE const StringID&   getCurrentLevelTag() const { return m_worldMapData.getCurrentLevelTag(); }

        u32     getLumsTotal() const { return 0; }

        f32             getTotalGameProgress() const ;
        const u32       getBrokenCageCount();
        const u32       getTotalTakenToothCount(const Ray_PersistentGameData_Level* currentLevel/*used instead of level if not null*/, const StringID &_levelName) const;
        const u32       getStoneCount() { return 0; } // TODO

        // Teeth and Dead Guy
        void            returnTeeth();
        u32             getTeethReturned() const { return m_teethReturned; }

        // PlayerIDInfo Used
        const StringID& getUsedPlayerIDInfo() const { return m_usedPlayerIDInfo; }
        void            setUsedPlayerIDInfo( const StringID& _id ) { m_usedPlayerIDInfo = _id; }

        // Detect new Costume available
        ITF_INLINE const u32        getCostumeLastPrice() const { return m_costumeLastPrice; }
        ITF_INLINE void             setCostumeLastPrice(const u32 _newValue) { m_costumeLastPrice = _newValue; }

        // 
        u32     computeElectoonCount(const Ray_PersistentGameData_Level* currentLevel/*used instead of level if not null*/, const StringID &_levelName) const;

        // was the sprint/chest tutorial displayed once?
        bbool   getSprintTutorialDisabled() const { return m_sprintTutorialDisabled; }
        void    setSprintTutorialDisabled(bbool _val) { m_sprintTutorialDisabled = _val; }

        ITF_INLINE void addDiscoveredCageMap(const PackedObjectPath & _object, const StringID & _level)
        {
            const AbsoluteObjectPath absoluteObjectPath(_object, _level);
            if(m_discoveredCageMapList.find(absoluteObjectPath) == U32_INVALID)
            {
                m_discoveredCageMapList.push_back(absoluteObjectPath);
            }
        }

        ITF_INLINE bbool isDiscoveredCageMap(const PackedObjectPath & _object, const StringID & _level)
        {
            const AbsoluteObjectPath absoluteObjectPath(_object, _level);
            return (m_discoveredCageMapList.find(absoluteObjectPath) != U32_INVALID);
        }

        ITF_INLINE Ray_PersistentGameData_UniverseTracking* getTrackingData() { return &m_trackingData; }

        void                                addCostumeUsed( const StringID& _id );
        bbool                               isCostumeUsed( const StringID& _id ) const;

    private:
        Ray_PersistentGameData_Score        m_score;
        Ray_PersistentGameData_WorldMap     m_worldMapData;
        Ray_PersistentGameData_UniverseTracking m_trackingData;

		Ray_PersistentFrescoData				m_frescoData;

        SafeArray<AbsoluteObjectPath>       m_discoveredCageMapList;

        u32                                 m_teethReturned;
        u32                                 m_costumeLastPrice;
        StringID                            m_usedPlayerIDInfo;

        SafeArray <StringID>                m_costumesUsed;

        // was the sprint/chest tutorial displayed once?
        bbool                               m_sprintTutorialDisabled;
    };
}


#endif
