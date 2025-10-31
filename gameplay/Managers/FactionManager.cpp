#include "precompiled_gameplay.h"

#ifndef _ITF_FACTIONMANAGER_H_
#include "gameplay/managers/FactionManager.h"
#endif //_ITF_FACTIONMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FactionManager_Template)

    BEGIN_SERIALIZATION(FactionManager_Template)
        SERIALIZE_CONTAINER_OBJECT("factions", m_factions);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(FactionRelationContainer)
        SERIALIZE_CONTAINER("factions", m_factions);
    END_SERIALIZATION()

    #define FACTIONFILE_PATH GETPATH_ALIAS("factionconfig")

    bbool FactionManager_Template::onTemplateLoaded()
    {
#ifdef ITF_WINDOWS
        u32 factionCount = m_factions.size();
        // check that the array is square
        for (u32 i = 0; i < factionCount; i++)
        {
            ITF_ASSERT(m_factions[i].m_factions.size() == factionCount);
        }
#endif

        return Super::onTemplateLoaded();
    }


    FactionManager* FactionManager::s_instance = NULL;

    FactionManager::FactionManager()
        : m_factionCount(0)
    {
    }

    FactionManager::~FactionManager()
    {
        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252), this);
        }
        if (TEMPLATEDATABASE)
        {
            const Path factionFile(FACTIONFILE_PATH);
            TEMPLATEDATABASE->removeTemplateClient(factionFile.getStringID(), this);
        }
    }

    void FactionManager::init()
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252),this);

        const Path factionFile(FACTIONFILE_PATH);
        TEMPLATEDATABASE->addTemplateClient(factionFile.getStringID(), this);
        const FactionManager_Template* config = TEMPLATEDATABASE->getTemplate<FactionManager_Template>(this, factionFile);

        if (!config)
        {
            ITF_FATAL_ERROR("Couldn't load faction config: %s", factionFile.getString().cStr());
            return;
        }

        m_factions = config->getFactions(); // copy
        m_factionCount = m_factions.size();
#if 0
        for (u32 i = 0; i < m_factions.size();++i)
        {
            FactionRelationContainer & a = m_factions[i];
            String8 toto;
            for (u32 j = 0; j < a.m_factions.size(); ++j)
            {
                String8 tata;
                tata.setTextFormat("%d,",a.m_factions[j]);
                toto += tata;
            }
            LOG("%s",toto.cStr());

        }
#endif
    }

    void FactionManager::onEvent( Event* _event )
    {
#ifdef ITF_SUPPORT_HOTRELOAD
        if ( EventSingletonConfigChanged* eventConfigChanged = _event->DynamicCast<EventSingletonConfigChanged>(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252)) )
        {
            const Path originalFile(FACTIONFILE_PATH);
            String changedFile = eventConfigChanged->getAbsoluteFilename();
            PathString_t originalFileName;

            FilePath::transformToRelative(changedFile);
            originalFile.getString(originalFileName);

            if (changedFile == originalFileName)
            {
                const FactionManager_Template* config = TEMPLATEDATABASE->reloadTemplate<FactionManager_Template>(originalFile);
                if (!config)
                {
                    ITF_FATAL_ERROR("Couldn't reload faction config: %s", originalFile.getString().cStr());
                }

                m_factions = config->getFactions(); // copy
                m_factionCount = m_factions.size();
            }
        }

#endif //ITF_SUPPORT_HOTRELOAD
    }

    InteractionMask FactionManager::getInteractions(Faction _faction1, Faction _faction2) const
    {
        ITF_ASSERT(_faction1 < m_factionCount && _faction2 < m_factionCount);
        if (_faction1 >= m_factionCount || _faction2 >= m_factionCount)
        {
            return 0;   // no interaction
        }

        return m_factions[_faction1].m_factions[_faction2];
    }

    bbool FactionManager::hasInteraction( Faction _faction1, Faction _faction2, Interaction _interaction ) const
    {
        ITF_ASSERT(_faction1 < m_factionCount && _faction2 < m_factionCount);
        if (_faction1 >= m_factionCount || _faction2 >= m_factionCount)
        {
            return bfalse;
        }

        u32 relation = m_factions[_faction1].m_factions[_faction2];
        return (relation & _interaction) != 0;
    }

}
