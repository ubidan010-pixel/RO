#include "precompiled_gameplay.h"

#ifndef _ITF_PERSISTENTGAMEDATA_H_
#include "gameplay/managers/PersistentGameData.h"
#endif

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

namespace ITF
{
	////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(PersistentGameData_ISD);
    IMPLEMENT_OBJECT_RTTI(PersistentGameData_Level);
	IMPLEMENT_OBJECT_RTTI(PersistentGameData_Universe)

    BEGIN_SERIALIZATION(PackedObjectPath)
        SERIALIZE_MEMBER("id",m_id);
        SERIALIZE_MEMBER("pathCode",m_pathCode);
    END_SERIALIZATION()

	BEGIN_SERIALIZATION(PersistentGameData_ISD)
	END_SERIALIZATION()

	BEGIN_SERIALIZATION(PersistentGameData_Level)
	    SERIALIZE_CONTAINER_WITH_FACTORY("ISDs", m_ISDs, GAMEINTERFACE->getPersistentGameDataFactory());
	END_SERIALIZATION()

	BEGIN_SERIALIZATION(PersistentGameData_Universe)
		SERIALIZE_CONTAINER_WITH_FACTORY("Levels", m_levels, GAMEINTERFACE->getPersistentGameDataFactory());
        SERIALIZE_OBJECT("Rewards", m_rewardsSession);
	END_SERIALIZATION()

    ////////////////////////////////////////////////////////////////////////////////////
    // Update this number when you want to force a "corrupted" file on next engine
    u32 PersistentGameData_Universe::Version = 1;

	////////////////////////////////////////////////////////////////////////////////////
	PackedObjectPath::PackedObjectPath(const Actor *_actor)
	{
		const int maxBufferSize=256;
		char buffer[maxBufferSize];
		char *bufferPos = buffer;
		char *bufferEnd = buffer+maxBufferSize;

		buffer[0] = 0;
		m_id = _actor->getObjectId();
		ITF_ASSERT_CRASH(!_actor->isSpawned(), "spawned object");


		Scene *ownerScene = _actor->getScene();
		while(ownerScene)
		{
			SubSceneActor *holder = ownerScene->getSubSceneActorHolder();
			if (!holder)
			{
				break;
			}
			const String8 &holderName = holder->getUserFriendly();
			u32 holderNameLen = holderName.getLen();
			//We add '|' + holderName+ <zero>
			char *nextPos = bufferPos + 1 + holderNameLen;
			if (nextPos>=bufferEnd)
			{
				ITF_ASSERT_MSG(nextPos<=bufferEnd, "buffer overrun");
				break;
			}
			bufferPos[0] = '|';
			strcpy(bufferPos+1, holderName.cStr());
			bufferPos = nextPos;

			ownerScene = holder->getScene();
		}
		m_pathCode = StringID::StrToCRC(buffer);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void PersistentGameData_Level::copyFrom_Base(const PersistentGameData_Level *_other,bbool _copyFoundRelicMask)
	{
		if (this==_other)
			return;
		destroyContent();
		ISDContainer::const_iterator iter;
		for (iter=_other->m_ISDs.begin(); iter!=_other->m_ISDs.end(); iter++)
		{
			PersistentGameData_ISD *res = createNewISD();
			ITF_ASSERT_CRASH(res!=NULL, "can't create ISD game data");
			m_ISDs.insert(ISDContainer::value_type(iter->first, res));
			res->copyFrom_Base(iter->second);
		}

	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    PersistentGameData_ISD *PersistentGameData_Level::getISDConst(const Path &_path) const
    {
		StringID id(StringID::StrToCRC(_path.getBasename()));

        ISDContainer::const_iterator iter = m_ISDs.find(id);
        if (iter==m_ISDs.end())
        {
			return NULL;
        }
        else
        {
            return iter->second;
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    PersistentGameData_ISD *PersistentGameData_Level::getISD(const Path &_path)
    {
		StringID id(StringID::StrToCRC(_path.getBasename()));

        ISDContainer::const_iterator iter = m_ISDs.find(id);
        PersistentGameData_ISD *res;
        if (iter==m_ISDs.end())
        {
            res = createNewISD();
            ITF_ASSERT_CRASH(res!=NULL, "can't create ISD game data");
            m_ISDs.insert(ISDContainer::value_type(id, res));
        }
        else
        {
            return iter->second;
        }

        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    PersistentGameData_Level *PersistentGameData_Universe::getLevelConst(const StringID &_levelID) const
    {
        LevelContainer::const_iterator iter = m_levels.find(_levelID);
        if (iter==m_levels.end())
        {
			return NULL;
        }
        else
        {
            return iter->second;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    PersistentGameData_Level *PersistentGameData_Universe::getLevel(const StringID &_levelID)
    {
        LevelContainer::const_iterator iter = m_levels.find(_levelID);
        PersistentGameData_Level *res;
        if (iter==m_levels.end())
        {
            res = createNewLevel();
            ITF_ASSERT_CRASH(res!=NULL, "can't create level");
            m_levels.insert(LevelContainer::value_type(_levelID, res));
        }
        else
        {
            return iter->second;
        }

        return res;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void PersistentGameData_Level::destroyContent()
    {
        ISDContainer::iterator iter;
        for (iter=m_ISDs.begin(); iter!=m_ISDs.end(); iter++)
        {
            SF_DEL(iter->second);
        }
        m_ISDs.clear();

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void PersistentGameData_Universe::destroyContent()
    {
        LevelContainer::iterator iter;
        for (iter=m_levels.begin(); iter!=m_levels.end(); iter++)
        {
            SF_DEL(iter->second);
        }
        m_levels.clear();
    }

}
