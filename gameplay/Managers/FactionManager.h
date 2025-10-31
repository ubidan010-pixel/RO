#ifndef _ITF_FACTIONMANAGER_H_
#define _ITF_FACTIONMANAGER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

namespace ITF
{
    #define FACTION_MANAGER FactionManager::get()

    enum Interaction
    {
        INTERACTION_NONE                = 0x00,
        INTERACTION_TARGET              = 0x01,
        INTERACTION_RECEIVEHIT          = 0x02,
        INTERACTION_RECEIVEDAMAGE       = 0x04,
        INTERACTION_ALLY                = 0x08,
        INTERACTION_CRUSHENEMY          = 0x10,
    };

    typedef u32 InteractionMask;


    class FactionRelationContainer
    {
        DECLARE_SERIALIZE()

    public:
        ITF_VECTOR<InteractionMask> m_factions;
    };

    class FactionManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(FactionManager_Template, TemplateObj,2119702796);
        DECLARE_SERIALIZE()

    public:
        FactionManager_Template()
            : Super()
            , m_factionCount(0)
        {}
        FactionManager_Template(const Path& _path)
            : Super(_path)
            , m_factionCount(0)
        {}

        virtual bbool onTemplateLoaded();

        ITF_INLINE const ITF_VECTOR<FactionRelationContainer>& getFactions() const { return m_factions; }

    private:
        ITF_VECTOR<FactionRelationContainer> m_factions;
        u32 m_factionCount;
    };


    class FactionManager : public IEventListener
    {
        static FactionManager*      s_instance;

    public:

        static void                 create()    { s_instance = newAlloc(mId_Singleton, FactionManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static FactionManager*      get()       { return s_instance; } 

        FactionManager();
        virtual ~FactionManager();

        void            init();
        virtual void    onEvent( Event* _event );

        InteractionMask getInteractions(Faction _faction1, Faction _faction2) const;
        bbool           hasInteraction(Faction _faction1, Faction _faction2, Interaction _interaction) const;

    private:

        ITF_VECTOR<FactionRelationContainer> m_factions;
        u32 m_factionCount;
    };

};

#endif //_ITF_FACTIONMANAGER_H_
