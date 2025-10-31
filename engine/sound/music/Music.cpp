#include "precompiled_engine.h"


#ifndef _ITF_MUSIC_H_
#include "engine/sound/Music/Music.h"
#endif // _ITF_MUSIC_H_


#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#ifndef _ITF_MUSICPARTSET_H_
#include "engine/sound/Music/MusicPartSet.h"
#endif // _ITF_MUSICPARTSET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_


namespace ITF
{
///////////////////////////////////////////////////////////////////////
// MUSIC
///////////////////////////////////////////////////////////////////////
Music::Music()
    : m_musicTreeTemplate( NULL )
    , m_musicInstance( NULL )
    , m_inputList(NULL)
    , m_metronomeType(U32_INVALID)
{
}

Music::~Music()
{
    RESOURCE_MANAGER->removeUserToResourcePhysicalData( m_resourceGroup );
    RESOURCE_MANAGER->removeUserToResourceLogicalData( m_resourceGroup );
}

void Music::init( const MusicPartSet_Template * _setTemplate, const MusicTree_Template * _treeTemplate, const SafeArray <InputDesc>& _inputList )
{
    ITF_ASSERT( !m_musicPartResourceIDList.size() );
    ITF_ASSERT( !m_musicTreeTemplate );
    ITF_ASSERT( _setTemplate );
    ITF_ASSERT( _treeTemplate );

    m_musicTreeTemplate = _treeTemplate;
    m_metronomeType = _treeTemplate->getMetronomeType();

    ResourceGroup* resourceGroup = getResourceGroup();

    for ( u32 u = 0 ; u < _setTemplate->getMusicPartList().size(); ++u )
    {
        MusicPart_Template partTemplate = _setTemplate->getMusicPartList()[ u ];

        ResourceID musicPartResource = resourceGroup->addResource( Resource::ResourceType_MusicPart, partTemplate.getPath() );

        MusicPart * part = (MusicPart*)( musicPartResource.getResource() );
        part->setTemplate( partTemplate );

        //RAKI_OUTPUT( "0x%x MusicInit adding part 0x%x", this, part );

        m_musicPartResourceIDList.push_back( musicPartResource );
    }

    RESOURCE_MANAGER->addUserToResourcePhysicalData( m_resourceGroup );

    // JP : TEMP : until classes Music / MusicInstance / MusicManager are definitely implemented
    m_inputList = &_inputList;
}


ResourceGroup* Music::getResourceGroup()
{ 
    ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());

    if (grp)
        return grp;

    m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

    return (ResourceGroup*)(m_resourceGroup.getResource());
} 


const MusicPart* Music::getMusicPart( const StringID _name ) const
{
    for ( u32 u = 0 ; u < m_musicPartResourceIDList.size() ; ++u )
    {
        const MusicPart * part = ( const MusicPart* )( m_musicPartResourceIDList[ u ].getResource() );

//        LOG("Music::getMusicPart comparing <%s> with <%s>", part->getName().getDebugString(), _name.getDebugString() );

        if ( part && part->getName() == _name )
            return part;
    }

    return NULL;
}


const MusicPart* Music::getFirstMusicPart() const
{
    if ( m_musicPartResourceIDList.size() )
        return ( const MusicPart* )( m_musicPartResourceIDList[0].getResource() );

    return NULL;
}


bbool Music::isPlaying() const
{
    return m_musicInstance != NULL ? m_musicInstance->isPlaying() : bfalse;
}


bbool Music::isPhysicallyLoaded() const
{
    for ( u32 u = 0 ; u < m_musicPartResourceIDList.size() ; ++u )
    {
        MusicPart * part = ( MusicPart* )( m_musicPartResourceIDList[ u ].getResource() );

        if ( part && !part->isPhysicallyLoaded() )
            return bfalse;
    }

    return btrue;
}


///////////////////////////////////////////////////////////////////////
// MUSIC SIMPLE
///////////////////////////////////////////////////////////////////////

MusicSimple::MusicSimple()
{}

void MusicSimple::init( const StringID& _name, const Path& _resourcePath, const StringID& _eventId)
{
    ITF_ASSERT( !m_musicPartResourceIDList.size() );

    ResourceGroup* resourceGroup = getResourceGroup();

    ResourceID musicPartResource = resourceGroup->addResource( Resource::ResourceType_MusicPart, _resourcePath );

    MusicPart * part = (MusicPart*)( musicPartResource.getResource() );
    part->setName( _name );
    part->setWwiseEventGuid(_eventId);
#if defined(ITF_SUPPORT_WWISE)
    f32 waveduration = AUDIO_ADAPTER->getDurationFromGUID(_eventId);
    part->setWaveDuration(waveduration);
    part->setSampleSize( (u32)(waveduration * DEFAULT_SAMPLING_RATE));
#endif


    m_musicPartResourceIDList.push_back( musicPartResource );

    RESOURCE_MANAGER->addUserToResourcePhysicalData( m_resourceGroup );
}


} // namespace ITF
