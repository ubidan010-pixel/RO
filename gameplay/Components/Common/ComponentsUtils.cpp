#include "precompiled_gameplay.h"

#ifndef _ITF_COMPONENTSUTILS_H_
#include "gameplay/Components/Common/ComponentsUtils.h"
#endif //_ITF_COMPONENTSUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


namespace ITF
{
    const ActorSpawnBank_Template::ActorSpawnData * ActorSpawnBank_Template::getActorSpawnDataFromId( const StringID & _id )const
    {
        for( u32 i = 0; i < m_actorSpawnDataList.size() ; ++i )
        {
            const ActorSpawnData & data = m_actorSpawnDataList[i];
            if ( data.m_id == _id )
            {
                return &data;
            }
        }
        return NULL;
    }

    const Path & ActorSpawnBank_Template::getActorPathFromId( const StringID & _id ) const
    {
        const ActorSpawnData * data = getActorSpawnDataFromId( _id );
        if ( data != NULL )
        {
            return data->m_actorPath;
        }
        return Path::EmptyPath;
    }
   
    BEGIN_SERIALIZATION( ActorSpawnBank_Template )
        SERIALIZE_CONTAINER_OBJECT( "list",     m_actorSpawnDataList );
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS( ActorSpawnBank_Template, ActorSpawnData )
        SERIALIZE_MEMBER( "id",                         m_id );
        SERIALIZE_MEMBER( "path",                       m_actorPath );
        SERIALIZE_MEMBER( "recycle",                    m_recycle );
        SERIALIZE_MEMBER( "scale",                      m_useScale );
        SERIALIZE_MEMBER( "flip",                       m_flip );
        SERIALIZE_MEMBER( "userData",                   m_userData );
        SERIALIZE_OBJECT_WITH_FACTORY("onSpawnEvent",   m_onSpawnEvent,     ACTORSMANAGER->getEventFactory() );
        
    END_SERIALIZATION()

};
