#include "precompiled_engine.h"

#include "engine/boot/LoadingSequence.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

#define ITF_NO_PREFETCH_TRACKING

namespace ITF
{

Thread * volatile                   LoadingSequence::m_asyncThread = NULL;
LoadingSequence::SequenceLoadInfo   LoadingSequence::m_info;
TemplateClientHandler               LoadingSequence::m_clientHandler;
LoadingSequence::ResRef             LoadingSequence::m_resources;
bool                                LoadingSequence::m_delayRelease = true;

void LoadingSequence::load( const String& filename, bool setStatic, TemplateClientHandler* hdl /* = NULL */ )
{
    if ( FILEMANAGER->isRemote() )
        return;

    String  platform = FILESERVER->getPlatform();
    String  cookFile = FILESERVER->getCookedNamePlatform( filename, platform );
    File*   sequence = FILEMANAGER->openFile( cookFile, ITF_FILE_ATTR_READ );

    if ( sequence == NULL )
        return;

    u32 length = static_cast< u32 >( sequence->getLength() );
    u8* buffer = new u8[length];

    if ( !sequence->read( buffer, length ) )
    {
        SF_DEL( sequence );
        return;
    }

    ArchiveMemory           am( buffer, length );
    String                  path;
    u32                     size;
    PrefetchFactory::Params params;

    RESOURCE_MANAGER->pause();
    am.serialize( size );
    params.hdl = hdl;
    if ( params.hdl == NULL )
        params.hdl = &m_clientHandler;
    while ( size-- )
    {
        path.serialize( am );
        params.res = NULL;
        params.refType = 0;
#ifndef ITF_NO_PREFETCH_TRACKING
        LOG( "[PREFETCH] %s", StringToUTF8(path).get() );
#endif
        if ( !PrefetchFactory::prefetch( path, params ) )
            continue;
#ifndef ITF_NO_PREFETCH_TRACKING
        LOG( "[PREFETCH]  %s  Done", StringToUTF8(path).get());
#endif
        if ( params.res != NULL )
        {
            if ( params.refType != 0 )
            {
                PrefetchResource prefetch;

                prefetch.refType = params.refType;
                prefetch.resource = path;
                prefetch.type = params.type;
                ITF_ASSERT( prefetch.type != -1 );
                if ( prefetch.type == -1 )
                    prefetch.type = Resource::ResourceType_All;
                m_resources.push_back( prefetch );
            }
            params.res->setStatic( setStatic );
        }
    }
    RESOURCE_MANAGER->unpause();
    SF_DEL_ARRAY( buffer );
    SF_DEL( sequence );
}

// This method is for scene loading
void LoadingSequence::load( const ITF_VECTOR< Path >& files, ITF_VECTOR< PrefetchResource >& prefetched, TemplateClientHandler* hdl /* = NULL */ )
{
#ifndef ITF_WINDOWS
    ITF_ASSERT_MSG( hdl != NULL, "Provide a template client handler or templates wont be released" );

    ITF_VECTOR< Path >::const_iterator      end = files.end();
    PrefetchFactory::Params                 params;

    RESOURCE_MANAGER->pause();
    params.hdl = hdl;
    if ( params.hdl == NULL )
        params.hdl = &m_clientHandler;
    for ( ITF_VECTOR< Path >::const_iterator it = files.begin(); it != end; ++it )
    {
        params.res = NULL;
        params.refType = 0;
//#ifndef ITF_NO_PREFETCH_TRACKING
//        LOG("[PREFETCH] %ls", path.cStr() );
//#endif
        if ( !PrefetchFactory::prefetch( *it, params ) )
            continue;
//#ifndef ITF_NO_PREFETCH_TRACKING
//        LOG( "[PREFETCH]    Done", path.cStr() );
//#endif
        if ( params.res != NULL && params.refType != 0 )
        {
            PrefetchResource prefetch;

            prefetch.refType = params.refType;
            prefetch.resource = it->getStringID();
            prefetch.type = params.type;
            ITF_ASSERT( prefetch.type != -1 );
            if ( prefetch.type == -1 )
                prefetch.type = Resource::ResourceType_All;
            prefetched.push_back( prefetch );
        }
    }
    RESOURCE_MANAGER->unpause();
#endif // !ITF_WINDOWS
}

void LoadingSequence::loadAsync( const String& filename, bool setStatic, TemplateClientHandler* hdl /* = NULL */ )
{
    ITF_ASSERT_CRASH( m_asyncThread == NULL, "Only one loading sequence at a time allowed" );
    m_info.filename = filename;
    m_info.setStatic = setStatic;
    m_info.handler = hdl == NULL ? &m_clientHandler : hdl;
    m_asyncThread = THREADMANAGER->createThread( &LoadingSequence::loadProc, &m_info, ThreadSettings::m_settings[eThreadId_loadingSequence] );
}

void LoadingSequence::releaseResources()
{
#ifndef ITF_WINDOWS
    if ( m_delayRelease )
        return;

    ResourceID id;

    ITF_ASSERT_CRASH( m_asyncThread == NULL, "Loading still running" );
    for ( ResRef::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
    {
#ifndef ITF_NO_PREFETCH_TRACKING
        LOG("[PREFETCH] release %s", it->resource->getPath().getString8().cStr() );
#endif
        if ( RESOURCE_MANAGER->getResourceIdFromFile( id, it->resource, it->type ) )
        {
            if ( ( it->refType & PrefetchFactory::Params::Physical ) != 0 )
                RESOURCE_MANAGER->removeUserToResourcePhysicalData( id );
            if ( ( it->refType & PrefetchFactory::Params::Logical ) != 0 )
                RESOURCE_MANAGER->removeUserToResourceLogicalData( id );
        }
    }
    m_resources.clear();
#endif // !ITF_WINDOWS
}

void LoadingSequence::releaseResources( ITF_VECTOR< PrefetchResource >& prefetched )
{
    for ( ITF_VECTOR< PrefetchResource >::iterator it = prefetched.begin(); it != prefetched.end(); ++it )
    {
#ifndef ITF_NO_PREFETCH_TRACKING
        LOG("[PREFETCH] release %s", it->resource->getPath().getString8().cStr() );
#endif

        ResourceID id;

        if ( RESOURCE_MANAGER->getResourceIdFromFile( id, it->resource, it->type ) )
        {
            if ( ( it->refType & PrefetchFactory::Params::Physical ) != 0 )
                RESOURCE_MANAGER->removeUserToResourcePhysicalData( id );
            if ( ( it->refType & PrefetchFactory::Params::Logical ) != 0 )
                RESOURCE_MANAGER->removeUserToResourceLogicalData( id );
        }
    }

    ITF_VECTOR< PrefetchResource >().swap( prefetched );
}

bool LoadingSequence::isLoaded()
{
    return m_asyncThread == NULL && !RESOURCE_MANAGER->hasResourcesToLoad() && !RESOURCE_MANAGER->hasQueryOps();
}

void LoadingSequence::disableDelayRelease()
{
    m_delayRelease = false;
}

u32 WINAPI LoadingSequence::loadProc( void* )
{
    FILEMANAGER->loadBundles();
    load( m_info.filename, m_info.setStatic );
    FILEMANAGER->unloadBundles();
    m_asyncThread = NULL;
    return 0;
}

}
