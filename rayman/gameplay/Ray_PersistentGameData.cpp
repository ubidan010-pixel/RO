#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif //_ITF_RAY_PERSISTENTGAMEDATA_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_ //CASA::VITA::TARIK::FRESCO GAMEPLAY
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h" //CASA::VITA::TARIK::FRESCO GAMEPLAY
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PersistentGameData_ISD)
    IMPLEMENT_OBJECT_RTTI(Ray_PersistentGameData_Level)
    IMPLEMENT_OBJECT_RTTI(Ray_PersistentGameData_Universe)

    BEGIN_SERIALIZATION(AbsoluteObjectPath)
        SERIALIZE_OBJECT("packedObjectPath", m_packedObjectPath);
        SERIALIZE_MEMBER("levelCRC", m_levelCRC);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_CHILD(Ray_PersistentGameData_ISD)
        SERIALIZE_CONTAINER_OBJECT("pickedUpLums", m_pickedUpLums);
        SERIALIZE_CONTAINER_OBJECT("takenTooth", m_takenTooth);
        SERIALIZE_CONTAINER_OBJECT("alreadySeenCutScenes", m_alreadySeenCutScenes);
        SERIALIZE_MEMBER("foundCageMask", m_foundCageMask);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_CHILD(Ray_PersistentGameData_Level)
        SERIALIZE_CONTAINER_OBJECT("cageMapPassedDoors", m_cageMapPassedDoors);
        SERIALIZE_MEMBER("wonChallenges", m_wonChallenges);
        SERIALIZE_LEVEL_STATE("levelState",m_levelState);
        SERIALIZE_MEMBER("bestTimeAttack", m_bestTimeAttack);
        SERIALIZE_MEMBER("bestLumAttack", m_bestLumAttack);
        SERIALIZE_MEMBER("hasWarning",m_hasWarning);
        SERIALIZE_MEMBER("isSkipped",m_isSkipped);
        SERIALIZE_OBJECT("trackingdata", m_trackingData);
		SERIALIZE_MEMBER("foundRelicMask", m_foundRelicMask);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Ray_PersistentGameData_LevelTracking)
        SERIALIZE_MEMBER("runCount", m_runCount);
        SERIALIZE_MEMBER("challengeTimeAttackCount", m_challengeTimeAttackCount);
        SERIALIZE_MEMBER("challengeHidden1", m_challengeHidden1);
        SERIALIZE_MEMBER("challengeHidden2", m_challengeHidden2);
        SERIALIZE_MEMBER("challengeCage", m_challengeCage);
        SERIALIZE_MEMBER("firstTimeLevelCompleted", m_firstTimeLevelCompleted);
        SERIALIZE_MEMBER("challengeLumsStage1", m_challengeLumsStage1);
        SERIALIZE_MEMBER("challengeLumsStage2", m_challengeLumsStage2);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Ray_PersistentGameData_Score)
        SERIALIZE_CONTAINER("lumCount", m_lumCount);
    END_SERIALIZATION()

	BEGIN_SERIALIZATION(Ray_PersistentFrescoData)//CASA::VITA::TARIK::FRESCO GAMEPLAY
		SERIALIZE_CONTAINER_OBJECT("frescoSlot", m_frescoSlot);
		SERIALIZE_CONTAINER("relicDuplicateCount", m_relicDuplicateCount);
	END_SERIALIZATION()

	BEGIN_SERIALIZATION_SUBCLASS(Ray_PersistentGameData_Fresco,RelicInfo)//CASA::VITA::TARIK::FRESCO GAMEPLAY
		SERIALIZE_RELIC_TYPE("type", m_type);
		SERIALIZE_RELIC_STATUS("status", m_status);
	END_SERIALIZATION()

	BEGIN_SERIALIZATION_SUBCLASS(Ray_PersistentGameData_Fresco,FrescoInfo)//CASA::VITA::TARIK::FRESCO GAMEPLAY
		SERIALIZE_MEMBER("inGameRelicCount", m_inGameRelicCount);
		SERIALIZE_CONTAINER_OBJECT("relicInfo", m_relicInfo);
	END_SERIALIZATION()

	BEGIN_SERIALIZATION(Ray_PersistentGameData_Fresco) //CASA::VITA::TARIK::FRESCO GAMEPLAY
			SERIALIZE_CONTAINER_OBJECT("frescoInfo", m_frescoInfo);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_PersistentGameData_WorldMap,WorldInfo)
        SERIALIZE_LEVEL_STATE("state", m_state);
        SERIALIZE_MEMBER("hasWarning",m_hasWarning);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Ray_PersistentGameData_WorldMap)
        SERIALIZE_CONTAINER_OBJECT("worldsInfo", m_worldsInfo);
        SERIALIZE_MEMBER("currentWorld",m_currentWorld);
        SERIALIZE_MEMBER("currentWorldTag",m_currentWorldTag);
        SERIALIZE_MEMBER("currentLevel",m_currentLevel);
        SERIALIZE_MEMBER("currentLevelTag",m_currentLevelTag);
		SERIALIZE_MEMBER("isUnlockRelicMessageDisplayed",m_isUnlockRelicMessageDisplayed);
        //SERIALIZE_MEMBER("lastElectoonCount", m_lastElectoonCount); //CAUTION WE DO NOT SAVE THEM IN ORDER TO HAVE U32_INVALID WHEN LOADED
        //SERIALIZE_MEMBER("lastToothCount", m_lastToothCount);//CAUTION WE DO NOT SAVE THEM IN ORDER TO HAVE U32_INVALID WHEN LOADED
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Ray_PersistentGameData_UniverseTracking)
        SERIALIZE_CONTAINER("timers", m_timers);
        SERIALIZE_CONTAINER("pafCounter", m_pafCounter);
    END_SERIALIZATION()
        
    BEGIN_SERIALIZATION_CHILD(Ray_PersistentGameData_Universe)
        SERIALIZE_OBJECT("score", m_score);
        SERIALIZE_OBJECT("worldMapData",m_worldMapData);
        SERIALIZE_OBJECT("trackingData",m_trackingData);
		SERIALIZE_OBJECT("frescoData",m_frescoData);
        SERIALIZE_CONTAINER_OBJECT("discoveredCageMapList", m_discoveredCageMapList);
        SERIALIZE_MEMBER("teethReturned",m_teethReturned);
        SERIALIZE_MEMBER("usedPlayerIDInfo",m_usedPlayerIDInfo);
        SERIALIZE_MEMBER("sprintTutorialDisabled",m_sprintTutorialDisabled);
        SERIALIZE_MEMBER("costumeLastPrice",m_costumeLastPrice);
        SERIALIZE_CONTAINER("costumesUsed",m_costumesUsed);
        SERIALIZE_CONTAINER("conceptsUnlocked",m_conceptsUnlocked);
        SERIALIZE_CONTAINER("conceptsRevealed",m_conceptsRevealed);
    END_SERIALIZATION()


    ///////////////////////////////////////////////////////////////////////////////////////////
    AbsoluteObjectPath::AbsoluteObjectPath()
    : m_packedObjectPath()
    , m_levelCRC(U32_INVALID)
    {
    }

    AbsoluteObjectPath::AbsoluteObjectPath(const Actor * const _actor, const StringID & _levelFriendly)
        : m_packedObjectPath(_actor)
    {
        m_levelCRC = _levelFriendly.GetValue();
    }

    AbsoluteObjectPath::AbsoluteObjectPath(const PackedObjectPath & _packedObjectPath, const StringID & _levelFriendly)
        : m_packedObjectPath(_packedObjectPath)
    {
        m_levelCRC = _levelFriendly.GetValue();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_PersistentGameData_ISD::setIsObjectTaken(SafeArray<PackedObjectPath>& _objectList, const PackedObjectPath &_id, bbool _taken)
    {
        if (_taken)
        {
            if (_objectList.find(_id)<0) 
            {
                _objectList.push_back(_id); 
            }
        }
        else
        {
            i32 existingPlace = _objectList.find(_id);
            if (existingPlace>=0)
            {
                _objectList.eraseNoOrder(existingPlace);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_PersistentGameData_Level::getBrokenCageMask() const
    {
        ISDContainer::const_iterator iter; 
        u32 r = 0;
        for (iter=m_ISDs.begin(); iter!=m_ISDs.end(); iter++)
        {
            Ray_PersistentGameData_ISD *isd = (Ray_PersistentGameData_ISD*)iter->second;
            u32 newMask = isd->getBrokenCageMask();
            ITF_WARNING_CATEGORY(LD, NULL, (newMask&r)==0, "2 cages have same index in same level");
            r = r | newMask;
        }
        return r;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_PersistentGameData_Level::countBits(u32 _mask)
    {
        u32 c=0;
        for (;_mask!=0; _mask = _mask>>1)
        {
            if (_mask&1)
                c++;
        }
        return c;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_PersistentGameData_Level::getTakenToothCount() const 
    {
        ISDContainer::const_iterator iter; 
        u32 r=0;
        for (iter=m_ISDs.begin(); iter!=m_ISDs.end(); iter++)
        {
            Ray_PersistentGameData_ISD *isd = (Ray_PersistentGameData_ISD*)iter->second;
            r = r + isd->getTakenToothCount();
        }
        return r;
    }
    //////////////////////////////////////////////////////////////////////////
    u32 Ray_PersistentGameData_Level::computeElectoonCount() const
    {
        u32 challengeMask = CHALLENGE_TIMEATTACK|CHALLENGE_LUMATTACK1|CHALLENGE_LUMATTACK2|CHALLENGE_BEATENBOSS;
        return getBrokenCageCount() + countBits(m_wonChallenges&challengeMask);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_PersistentGameData_Level::setFrom(const Ray_PersistentGameData_Level *_other, bbool _mergeCinematicList,bbool _copyFoundRelicMask)
    {
        ITF_MAP<StringID, SafeArray<PackedObjectPath> > previousCineState;
        if (_mergeCinematicList)
        {
            ISDContainer::const_iterator iter; 
            for (iter=m_ISDs.begin(); iter!=m_ISDs.end(); iter++)
            {
                Ray_PersistentGameData_ISD *ISD = (Ray_PersistentGameData_ISD*)iter->second;
                previousCineState.insert(std::pair<StringID, SafeArray<PackedObjectPath> >(iter->first, ISD->getAlreadySeenCutScenes()) );
            }
        }
        copyFrom_Base(_other, _copyFoundRelicMask); 
        //if merging cut scene flags, make union of previous and new states
        ITF_MAP<StringID, SafeArray<PackedObjectPath> >::const_iterator iter; 
        for (iter = previousCineState.begin(); iter!=previousCineState.end(); iter++)
        {
            ISDContainer::const_iterator ISDInNewState = m_ISDs.find(iter->first);
            if (ISDInNewState==m_ISDs.end())
            {
                PersistentGameData_ISD *newISD = createNewISD();
                ISDInNewState = m_ISDs.insert(ISDContainer::value_type(iter->first, newISD)).first;
            }

            const SafeArray<PackedObjectPath> &previousState = iter->second;
            Ray_PersistentGameData_ISD *ISDDataInNewState = (Ray_PersistentGameData_ISD*)ISDInNewState->second;
            for (u32 i=0; i<previousState.size(); i++)
            {
                const PackedObjectPath &seenCutScene = previousState[i];
                ISDDataInNewState->setAlreadySeenCutScene(seenCutScene, btrue);
            }
        }
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	void Ray_PersistentGameData_Level::setFoundRelicMaskFrom(const Ray_PersistentGameData_Level *_other)
	{
		Ray_PersistentGameData_Level *rayLevel = (Ray_PersistentGameData_Level*)_other;
		m_foundRelicMask = rayLevel->m_foundRelicMask;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_PersistentGameData_Universe::clear()
    {
        m_score = Ray_PersistentGameData_Score();        
        m_trackingData = Ray_PersistentGameData_UniverseTracking();
        m_worldMapData.clear();
		m_frescoData.clear();
        m_sprintTutorialDisabled = bfalse; // RO-12304
        m_teethReturned = 0;
        m_usedPlayerIDInfo = StringID::Invalid;
        m_costumeLastPrice = 0;
        m_conceptsUnlocked.clear();
        m_conceptsRevealed.clear();

        Super::clear();
    }

    const u32 Ray_PersistentGameData_Universe::getBrokenCageCount()
    {
        u32 counter = 0;
        LevelContainer::iterator it = m_levels.begin();
        LevelContainer::iterator it_end = m_levels.end();
        while (it != it_end)
        {
            counter += static_cast<Ray_PersistentGameData_Level*>(it->second)->getBrokenCageCount();

            ++it;
        }

        return counter;
    }

    const u32 Ray_PersistentGameData_Universe::getTotalTakenToothCount(const Ray_PersistentGameData_Level* currentLevel/*used instead of level if not null*/, const StringID &_levelName) const
    {
        u32 counter = 0;
        LevelContainer::const_iterator it = m_levels.begin();
        LevelContainer::const_iterator it_end = m_levels.end();
        while (it != it_end)
        {
            if (it->first == _levelName && currentLevel!=NULL && _levelName.isValid())
            {
                counter += currentLevel->getTakenToothCount();
            }
            else
            {
                counter += static_cast<Ray_PersistentGameData_Level*>(it->second)->getTakenToothCount();
            }

            ++it;
        }

        return counter;
    }

    u32 Ray_PersistentGameData_Universe::computeElectoonCount(const Ray_PersistentGameData_Level* currentLevel/*used instead of level if not null*/, const StringID &_levelName) const
    {
        u32 counter = 0;
        LevelContainer::const_iterator it = m_levels.begin();
        LevelContainer::const_iterator it_end = m_levels.end();
        while (it != it_end)
        {
            u32 electoonCount;
            if (it->first == _levelName && currentLevel!=NULL && _levelName.isValid())
            {
                electoonCount = currentLevel->computeElectoonCount();
            }
            else
            {
                electoonCount = static_cast<Ray_PersistentGameData_Level*>(it->second)->computeElectoonCount();
            }

            counter += electoonCount;

            ++it;
        }

        return counter;
    }

    void Ray_PersistentGameData_Universe::returnTeeth()
    {
        m_teethReturned = 0;
        LevelContainer::iterator it = m_levels.begin();
        LevelContainer::iterator it_end = m_levels.end();

        while (it != it_end)
        {
            m_teethReturned += static_cast<Ray_PersistentGameData_Level*>(it->second)->getTakenToothCount();
            ++it;
        }
    }

    f32 Ray_PersistentGameData_Universe::getTotalGameProgress() const
    {
        LevelContainer::const_iterator it = m_levels.begin();
        LevelContainer::const_iterator it_end = m_levels.end();

        u32 completed   = 0;
        u32 total       = 0;
        while (it != it_end)
        {
            Ray_PersistentGameData_Level* pLevel = static_cast<Ray_PersistentGameData_Level*>(it->second);
            if(pLevel)
            {
                if(pLevel->getLevelState() == SPOT_STATE_COMPLETED)
                    completed ++;
                total ++;
            }
            ++it;
        }

        return (completed / (f32)total);
    }

    void Ray_PersistentGameData_Universe::addCostumeUsed( const StringID& _id )
    {
        i32 index = m_costumesUsed.find(_id);

        if ( index == -1 )
        {
            m_costumesUsed.push_back(_id);
        }
    }

    bbool Ray_PersistentGameData_Universe::isCostumeUsed( const StringID& _id ) const
    {
        return m_costumesUsed.find(_id) != -1;
    }

    void Ray_PersistentGameData_Universe::addConceptUnlocked( const StringID& _id )
    {
        i32 index = m_conceptsUnlocked.find(_id);

        if ( index == -1 )
        {
            m_conceptsUnlocked.push_back(_id);
        }
    }

    bbool Ray_PersistentGameData_Universe::isConceptUnlocked( const StringID& _id ) const
    {
        return m_conceptsUnlocked.find(_id) != -1;
    }

    void Ray_PersistentGameData_Universe::addConceptRevealed( const StringID& _id )
    {
        i32 index = m_conceptsRevealed.find(_id);

        if ( index == -1 )
        {
            m_conceptsRevealed.push_back(_id);
        }
    }

    bbool Ray_PersistentGameData_Universe::isConceptRevealed( const StringID& _id ) const
    {
        return m_conceptsRevealed.find(_id) != -1;
    }

#ifdef ITF_SUPPORT_CHEAT
	void Ray_PersistentGameData_Fresco::DebugRelicsInfo() //CASA::VITA::TARIK::FRESCO GAMEPLAY
	{
		u32 fresco1InGameRelicCount				= m_frescoInfo[0].GetInGameRelicCount();
		u32 fresco1NewInGameRelicCount			= m_frescoInfo[0].GetUnlockedRelicCount(RELIC_TYPE_INGAME);
		u32 fresco1DuplicateInGameRelicCount	= fresco1InGameRelicCount-fresco1NewInGameRelicCount;

		u32 fresco2InGameRelicCount				= m_frescoInfo[1].GetInGameRelicCount();
		u32 fresco2NewInGameRelicCount			= m_frescoInfo[1].GetUnlockedRelicCount(RELIC_TYPE_INGAME);
		u32 fresco2DuplicateInGameRelicCount	= fresco2InGameRelicCount-fresco2NewInGameRelicCount;

		f32 PosX = 0.02f * GFX_ADAPTER->getScreenWidth();
		f32 fontHeight = 30.0f; 
		i32 depthRank = -6;
		f32 espace = 0.08f; 
		f32 espace2 = 0.05f; 
		String txt;

		f32 PosY = 0.1f * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco : Game ( %d/%d ))",GetInGameRelicCount(),MAX_GAME_RELIC);
		const char* text1Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_RED, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK , text1Content, 0, 0, fontHeight);

		PosY += espace * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco[1] : New --------> Game ( %d/%d ) ------> All( %d/%d )",fresco1NewInGameRelicCount,MAX_FRESCO_RELIC,fresco1NewInGameRelicCount,MAX_FRESCO_RELIC);
		const char* text2Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text2Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco[1] : Duplicate ---> Game ( %d/%d ) ------> All( %d/+ )",fresco1DuplicateInGameRelicCount,MAX_GAME_RELIC_FRESCO1,fresco1DuplicateInGameRelicCount);
		const char* text3Content = txt.getCharCopy();		
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text3Content, 0, 0, fontHeight);

		PosY += espace * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco[2] : New --------> Game ( %d/%d ) ------> All( %d/%d )",fresco2NewInGameRelicCount,MAX_FRESCO_RELIC,fresco2NewInGameRelicCount,MAX_FRESCO_RELIC);
		const char* text4Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text4Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco[2] : Duplicate ---> Game ( %d/%d ) ------> All( %d/+ )",fresco2DuplicateInGameRelicCount,MAX_GAME_RELIC_FRESCO2,fresco2DuplicateInGameRelicCount);
		const char* text5Content = txt.getCharCopy();		
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text5Content, 0, 0, fontHeight);
		
		PosY += espace * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco All : New --------> %d/%d",GetUnlockedRelicCount(),MAX_FRESCO_RELIC*NB_FRESCO);
		const char* text7Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text7Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Fresco All : Duplicate ---> %d/+",GetDuplicateCount());
		const char* text8Content = txt.getCharCopy();		
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text8Content, 0, 0, fontHeight);

		PosY += espace * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" Cheat ------->");
		const char* text9Content = txt.getCharCopy();		
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_RED, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK, text9Content, 0, 0, fontHeight);
		
		PosY += espace * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" --------------> GamePlay Simulation : R+L+DPadU");
		const char* text10Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK , text10Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" --------------> Near Simulation : R+L+DPadD");
		const char* text11Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK , text11Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" --------------> Reinit : R+L+DPadR");
		const char* text12Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK , text12Content, 0, 0, fontHeight);

		PosY += espace2 * GFX_ADAPTER->getScreenHeight();
		txt.setTextFormat(" --------------> Save : R+L+Y+DPadL");
		const char* text13Content = txt.getCharCopy();
		GFX_ADAPTER->drawText(GFX_ADAPTER->getFontByIndex(0), depthRank, COLOR_WHITE, PosX,PosY, 0.0f, Vec2d(0.f, 0.f), COLOR_BLACK , text13Content, 0, 0, fontHeight);
	}
#endif

	RelicState Ray_PersistentGameData_Fresco::GetNextRelicState(u32 _frescoId,ITF_VECTOR<Vec2d> _newTargetPercentageList) //CASA::VITA::TARIK::FRESCO GAMEPLAY
	{
		u32 unlockedRelicCount = m_frescoInfo[_frescoId].GetUnlockedRelicCount();

		f32 frescoProgressionRange = 30.f;
		f32 newTargetPercentage = 0.f;

		for(u32 i = 0; i < _newTargetPercentageList.size(); i++) 
		{
			if(unlockedRelicCount < _newTargetPercentageList[i].m_x)
			{
				frescoProgressionRange = _newTargetPercentageList[i].m_x;
				newTargetPercentage = _newTargetPercentageList[i].m_y;
				break;
			}
		}

		u32 duplicateCount = m_frescoInfo[_frescoId].GetDuplicateCount();
		f32 currentDuplicatePer = (duplicateCount*100)/(frescoProgressionRange+duplicateCount);
		f32 targetDuplicatePer = 100.f - newTargetPercentage;
		if(currentDuplicatePer > targetDuplicatePer)
		{
			return RELIC_STATE_NEW;
		}

		u32 randomValue = Seeder::getSharedSeeder().GetU32(100);
		if( randomValue > newTargetPercentage)
		{
			return RELIC_STATE_DUPLICATE;
		}

		if(unlockedRelicCount < frescoProgressionRange)
		{
			if(((unlockedRelicCount + 1) >= frescoProgressionRange) && (currentDuplicatePer < targetDuplicatePer))
			{
				return RELIC_STATE_DUPLICATE;
			}

			return RELIC_STATE_NEW;
		}

		return RELIC_STATE_DUPLICATE;
	}

	u32 Ray_PersistentGameData_Fresco::GetAutoTakenRelicId(u32 _frescoId,RelicState _nextRelicState,u32 _unlockedRelicCount) //CASA::VITA::TARIK::FRESCO GAMEPLAY
	{
		u32 autoTakenRelicId = 0;
		if(_nextRelicState == RELIC_STATE_NEW)
		{
			SafeArray<u32>          frescoLockedRelic;
			u32 frescoLockedRelicCount = MAX_FRESCO_RELIC - _unlockedRelicCount;

			for(u32 i = 0; i < MAX_FRESCO_RELIC; i++) 
			{
				if(m_frescoInfo[_frescoId].GetRelicStatus(i)==RELIC_STATUS_LOCKED)
					frescoLockedRelic.push_back(i);
			}


			u32 relicIndex = Seeder::getSharedSeeder().GetU32(frescoLockedRelicCount-1);
			autoTakenRelicId = frescoLockedRelic[relicIndex];
		}
		else
		{
			SafeArray<u32>          frescoUnlockedRelic;
			for(u32 i = 0; i < MAX_FRESCO_RELIC; i++) 
			{
				if(m_frescoInfo[_frescoId].GetRelicStatus(i)==RELIC_STATUS_UNLOCKED)
					frescoUnlockedRelic.push_back(i);
			}

			u32 relicIndex = Seeder::getSharedSeeder().GetU32(_unlockedRelicCount-1);
			autoTakenRelicId = frescoUnlockedRelic[relicIndex];
		}
		return autoTakenRelicId;
	}

	u32 Ray_PersistentGameData_Fresco::GetDebugFrescoId() //CASA::VITA::TARIK::FRESCO GAMEPLAY
	{
		u32 debugFrescoId = 0;

		u32 fresco1NewInGameRelicCount	= m_frescoInfo[0].GetUnlockedRelicCount();
		u32 fresco2NewInGameRelicCount	= m_frescoInfo[1].GetUnlockedRelicCount();

		u32 fresco1InGameRelicCount	= m_frescoInfo[0].GetInGameRelicCount();
		u32 fresco2InGameRelicCount	= m_frescoInfo[1].GetInGameRelicCount();

		bbool maxFresco1GameRelicsTaken = bfalse;
		bbool maxFresco2GameRelicsTaken = bfalse;

		if((fresco1NewInGameRelicCount >=MAX_FRESCO_RELIC) && (fresco1InGameRelicCount >= MAX_GAME_RELIC_FRESCO1))
		{
			maxFresco1GameRelicsTaken = btrue;
		}

		if((fresco2NewInGameRelicCount >=MAX_FRESCO_RELIC) && (fresco2InGameRelicCount >= MAX_GAME_RELIC_FRESCO2))
		{
			maxFresco2GameRelicsTaken = btrue;
		}

		if(maxFresco1GameRelicsTaken && maxFresco2GameRelicsTaken)
		{
			debugFrescoId = 1000;
		}
		else if(!maxFresco1GameRelicsTaken)
		{
			debugFrescoId = 0;
		}
		else 
		{
			debugFrescoId = 1;
		}

		return debugFrescoId;
	}

	RelicState Ray_PersistentGameData_Fresco::UpdateInGameRelicsInfo(u32 _frescoId) //CASA::VITA::TARIK::FRESCO GAMEPLAY
	{
		if(_frescoId >= NB_FRESCO)
		{
			return RELIC_STATE_NONE;
		}	

		if(GetInGameRelicCount()==MAX_GAME_RELIC)
		{
			return RELIC_STATE_NONE;
		}		

		u32 frescoId = _frescoId;
		u32 currentfrescoInGameRelicCount		= m_frescoInfo[frescoId].GetInGameRelicCount();
		u32 currentfrescoNewInGameRelicCount	= m_frescoInfo[frescoId].GetUnlockedRelicCount();
		u32 currentfrescoMaxInGameRelics		= GetMaxInGameRelics(frescoId);

		RelicState nextRelicState = RELIC_STATE_NONE;

		if((MAX_FRESCO_RELIC  - currentfrescoNewInGameRelicCount) >= (currentfrescoMaxInGameRelics - currentfrescoInGameRelicCount))
		{
			nextRelicState = RELIC_STATE_NEW;
		}

		u32 unlockedRelicCount	= m_frescoInfo[frescoId].GetUnlockedRelicCount();

		u32 relicId = 0;		

		if(unlockedRelicCount==0)
		{
			relicId = Seeder::getSharedSeeder().GetU32(MAX_FRESCO_RELIC-1);
			UnlockRelic(frescoId,relicId,RELIC_TYPE_INGAME);
			return RELIC_STATE_NEW;
		}

		if(unlockedRelicCount == MAX_FRESCO_RELIC)
		{
			nextRelicState = RELIC_STATE_DUPLICATE;
		}

		if(nextRelicState == RELIC_STATE_NONE)
		{
			ITF_VECTOR<Vec2d> newTargetPercentageList = RAY_GAMEMANAGER->getFrescoGamePlayProgressionInfo();
			nextRelicState = GetNextRelicState(frescoId,newTargetPercentageList);
		}

		relicId = GetAutoTakenRelicId(frescoId,nextRelicState,unlockedRelicCount);

		if(nextRelicState == RELIC_STATE_NEW)
		{
			UnlockRelic(frescoId,relicId,RELIC_TYPE_INGAME);
		}
		else if(nextRelicState == RELIC_STATE_DUPLICATE)
		{
			RAY_GAMEMANAGER->UpdateFrescoRelicDuplicateCount(frescoId,relicId,1,RELIC_TYPE_INGAME);
			m_frescoInfo[frescoId].UpdateInGameRelicCount(1);
		}


		return nextRelicState;
	}
}

