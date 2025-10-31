#ifndef _ITF_PERSISTENTGAMEDATA_H_
#define _ITF_PERSISTENTGAMEDATA_H_


#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif  //_ITF_REWARDADAPTER_H_


namespace ITF
{

	///////////////////////////////////////////////////////////////////////////////////////////
	class PackedObjectPath
	{
		DECLARE_SERIALIZE()

	public:
		PackedObjectPath() : m_pathCode(U32_INVALID) {}
		PackedObjectPath(const Actor *_actor);
		bool operator==(const PackedObjectPath& _other) const {return (m_id == _other.m_id) && (m_pathCode==_other.m_pathCode);}
		bool operator!=(const PackedObjectPath& _other) const {return ! (*this==_other); }
        bbool isValid() const {return m_pathCode!=U32_INVALID && m_id.isValid();}
	private:
		ObjectId m_id;
		u32 m_pathCode;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	class PersistentGameData_ISD  : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(PersistentGameData_ISD,IRTTIObject,1407954474);
        DECLARE_SERIALIZE_VIRTUAL()

    public:
        PersistentGameData_ISD() {}
        virtual ~PersistentGameData_ISD() {}
        virtual void clear() {}

		virtual void copyFrom_Base(const PersistentGameData_ISD *_other) {}
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class PersistentGameData_Level : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(PersistentGameData_Level,IRTTIObject,2881934861);
        DECLARE_SERIALIZE_VIRTUAL()

    public:
        PersistentGameData_Level() {}
        virtual ~PersistentGameData_Level() {destroyContent();}

        virtual void clear() {destroyContent();}

        PersistentGameData_ISD *getISD(const Path &_path);
		PersistentGameData_ISD *getISDConst(const Path &_path) const;

		virtual void copyFrom_Base(const PersistentGameData_Level *_other,bbool _copyFoundRelicMask = btrue);

        virtual void destroyContent();
		virtual PersistentGameData_ISD *createNewISD() {return NULL;}
        typedef ITF_MAP<StringID, PersistentGameData_ISD*>  ISDContainer;

        ISDContainer m_ISDs;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class PersistentGameData_Universe : public IRTTIObject
    {
		DECLARE_OBJECT_CHILD_RTTI(PersistentGameData_Universe,IRTTIObject,2288281166);
        DECLARE_SERIALIZE_VIRTUAL()
    public:
        PersistentGameData_Universe() {}
        virtual ~PersistentGameData_Universe() {destroyContent();}

        virtual void clear() {destroyContent();}
        PersistentGameData_Level *getLevel(const StringID &_levelID);
		PersistentGameData_Level *getLevelConst(const StringID &_levelID) const;
        RewardManager::SaveSession* getRewardSession() { return &m_rewardsSession; }

        virtual void destroyContent();
        typedef ITF_MAP<StringID, PersistentGameData_Level*> LevelContainer;

		virtual PersistentGameData_Level *createNewLevel() {return NULL;}

        LevelContainer m_levels;

        RewardManager::SaveSession  m_rewardsSession;
        static u32  Version;
    };

}

#endif
