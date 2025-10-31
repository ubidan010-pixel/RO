#include "precompiled_engine.h"

#include <algorithm>

#include "core/UnicodeTools.h"

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_BUNDLEPROFILER_H_
#include "engine/file/bundleProfiler.h"
#endif //_ITF_BUNDLEPROFILER_H_

#ifndef ITF_FINAL
#ifndef _ITF_FILEPACK_H_
#include "engine/file/FilePack.h"
#endif //_ITF_FILEPACK_H_
#endif //ITF_FINAL

#define ITF_NO_BUNDLE_TRACKING
#define ITF_NO_REMOVE_PENDING_TRACKING

#define XGD3_LAYER1_START 2116880

namespace ITF
{
//----------------------------------------------------------------------------------------------------------------------------------------

//Value owned by each thread using a fileManager;
ThreadLocalStorage BundleManager::m_TLSBundle;

HeaderRegistry::HeaderRegistry()
{
    Synchronize::createCriticalSection( &m_cs );
}

HeaderRegistry::~HeaderRegistry()
{
    {
        csAutoLock lock( &m_cs );
        ITF_ASSERT( m_registry.empty() );
    }
    Synchronize::destroyCriticalSection( &m_cs );
}

SharableBundleHeader* HeaderRegistry::acquireHeader( const String8& bundleFilename )
{
    csAutoLock          lock( &m_cs );
    Registry::iterator  it = m_registry.find( bundleFilename );

    if ( it != m_registry.end() )
    {
        it->second->addRef();
        return it->second;
    }
    return NULL;
}

bbool HeaderRegistry::add( const String8& bundleFilename, SharableBundleHeader*& header )
{
    ITF_ASSERT_CRASH( header != NULL, "Invalid header pointer" );
    {
        csAutoLock          lock( &m_cs );
        Registry::iterator  it = m_registry.find( bundleFilename );

        if ( it == m_registry.end() )
        {
            header->addRef();
            m_registry[bundleFilename] = header;
            return btrue;
        }
        if ( it->second != NULL)
            LOG( "[BUNDLE] SharableBundleHeader duplicate" );
        if ( it->second == NULL )
        {
            header->addRef();
            it->second = header;
            return btrue;
        }
        SF_DEL( header );
        header = it->second;
        header->addRef();
    }
    return bfalse;
}

void HeaderRegistry::release( const String8& bundleFilename )
{
    csAutoLock          lock( &m_cs );
    Registry::iterator  it = m_registry.find( bundleFilename );

    ITF_ASSERT_MSG( it != m_registry.end() && it->second != NULL, "Header not registered oO" );

    if ( it != m_registry.end() && it->second != NULL )
    {
        SharableBundleHeader * header = it->second;
        ITF_ASSERT( header->getRefCount() != 0 );
        if ( header->getRefCount() != 0 ) // insure to delete it even with 0 refcount
        {
            header->removeRef();
        }
        if ( header->getRefCount() == 0 )
        {
            SF_DEL( it->second );
            m_registry.erase( bundleFilename );
        }
    }
}

BundleManager::BundleManager()
: m_opId( 0 )
, m_DataVersion( 0 )
, m_secureFat( NULL )
{
    Synchronize::createCriticalSection( &m_cs );
    Synchronize::createCriticalSection( &m_pendingsCs );
    Synchronize::createCriticalSection( &m_csLockAccess );
    m_Root = DATA_FOLDER;

    //singleton allocation
#ifndef ITF_FINAL
    newAlloc(mId_Debug, BundleProfileManager());
#endif //ITF_FINAL

}   

//----------------------------------------------------------------------------------------------------------------------------------------

BundleManager::~BundleManager()
{
#ifndef ITF_FINAL
    BundleProfileManager* pBundleProfile = BundleProfileManager::getptr();
    SF_DEL(pBundleProfile);
#endif //ITF_FINAL

    Synchronize::destroyCriticalSection( &m_cs );
    Synchronize::destroyCriticalSection( &m_pendingsCs );
    Synchronize::destroyCriticalSection( &m_csLockAccess );
    SF_DEL( m_secureFat );
}

//----------------------------------------------------------------------------------------------------------------------------------------
//enable lockaccess to avoid collision between file access,until a better solution is done
void BundleManager::lockAccess()
{
   Synchronize::enterCriticalSection(&m_csLockAccess);

}

void BundleManager::unlockAccess()
{
   Synchronize::leaveCriticalSection(&m_csLockAccess);
}


bbool BundleManager::loadSecureFat()
{
    ITF_ASSERT( m_secureFat == NULL );
    if ( m_secureFat == NULL )
    {
        m_secureFat = newAlloc( mId_System, GlobalFat );
        if ( !m_secureFat->load( "secure_fat.gf" ) )
        {
            SF_DEL( m_secureFat );
            return bfalse;
        }
    }
    return m_secureFat != NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------

static void FmtBundleFilename( String& _szFilename )
{
    const char * platform = "UNKNOWN";

#ifdef ITF_WINDOWS
    platform = "pc";
#elif defined(ITF_X360)
    platform = "x360";
#elif defined(ITF_PS3)
    platform = "PS3";
#elif defined(ITF_IPAD)
    platform = "iPad";
#elif defined(ITF_CTR)
    platform = "CTR";
#elif defined(ITF_WII)
    platform = "WII";
#elif defined(ITF_PS5)
    platform = "PS5";
#elif defined(ITF_NX)
    platform = "NX";
#elif defined(ITF_OUNCE)
    platform = "OUNCE";
#elif defined(ITF_XBOX_SERIES)
    platform = "XBOXSERIES";
#endif //ITF_WINDOWS

    _szFilename.setTextFormat( "%s_%s.ipk", UTF16ToUTF8(_szFilename.cStr()).get(), platform);
}

#ifdef ITF_X360
static u8 resolveLayout( u32 sortKey )
{
    if ( sortKey == U32_INVALID )
        return 0;
    return sortKey >= XGD3_LAYER1_START ? 1 : 0;
}

static bbool retrieveLayerBundle( GlobalFat* fat, const String& filename, String& fmtFilename, u32 lastAccessSortKeyLayer )
{
    String finalPath;

    ITF_ASSERT_CRASH( fat != NULL, "Invalid fat pointer" );
    fmtFilename = filename;
    FmtBundleFilename( fmtFilename );
    if ( !FILEMANAGER->TranslatePath( finalPath, fmtFilename ) || !hwFile::exists( finalPath ) )
        return bfalse;
    return lastAccessSortKeyLayer == resolveLayout( fat->getSortKey( fmtFilename ) );
}
#endif

bbool BundleManager::registerBundle( const String& _szFilename, bbool useCache /* = bfalse */, FileManager::BundlePriority prio /* = FileManager::Lowest */, const char* tag /* = NULL */ )
{
    csAutoLock  lock( &m_cs );
    String8     str = _szFilename.cStr();

    for ( u32 i = 0; i < m_bundles.size(); ++i )
        if ( m_bundles[i].keyWordName == str )
        {
            m_bundles[i].refCount += 1;
#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[BUNDLE][%s] Adding reference on bundle %s (%d)", ThreadSettings::getCurrentThreadName(), m_bundles[i].bundleFilename.cStr(), m_bundles[i].refCount );
#endif
            return btrue;
        }

    String fmtFilename = _szFilename;
    String layerFilename = _szFilename;
    String finalPath;

    FmtBundleFilename( fmtFilename );
    if ( !FILEMANAGER->TranslatePath( finalPath, fmtFilename ) || !hwFile::exists( finalPath ) )
    {
#ifdef ITF_X360
        if ( m_secureFat == NULL )
            return bfalse;

        u32 lastAccessSortKeyLayer = resolveLayout( BundleFile::m_lastAccessSortKey );

        layerFilename = _szFilename + "_0";
        if ( !retrieveLayerBundle( m_secureFat, layerFilename, fmtFilename, lastAccessSortKeyLayer ) )
        {
            layerFilename = _szFilename + "_1";
            if ( !retrieveLayerBundle( m_secureFat, layerFilename, fmtFilename, lastAccessSortKeyLayer ) )
                return bfalse;
        }
#else
        return bfalse;
#endif
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Registering bundle %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(layerFilename).get() );
    if ( m_secureFat != NULL && !m_secureFat->isBundleRegistered( layerFilename ) )
    {
        LOG( "[BUNDLE][%s] Using bundle %s not found in secure fat", ThreadSettings::getCurrentThreadName(), StringToUTF8(fmtFilename).get() );
    }
#endif

    BundleInfo info;

    info.bundleFilename = fmtFilename.cStr();
    info.keyWordName = _szFilename.cStr();
    info.tag = tag;
    info.useCache = useCache;
    info.prio = prio;
    info.refCount = 1;
    m_bundles.push_back( info );
    m_opId += 1;
    return btrue;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bbool BundleManager::unregisterBundleTag( const char* tag )
{
    csAutoLock lock( &m_cs );

    for ( u32 i = 0; i < m_bundles.size(); ++i )
        if ( m_bundles[i].tag == tag )
            unregisterBundle( i );
    return bfalse;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bbool BundleManager::unregisterBundle( const String& _szFilename )
{
    csAutoLock  lock( &m_cs );
    String8     str = _szFilename.cStr();
    
    for ( u32 i = 0; i < m_bundles.size(); ++i )
        if ( m_bundles[i].keyWordName == str )
            return unregisterBundle( i );
    return bfalse;
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool sortFilePosition(FilesPosition& _input1, FilesPosition& _input2)
{
    return _input1 < _input2;
}


void  BundleManager::orderLoad(ITF_VECTOR<FilesPosition>& _orderedList)
{
    PathString_t pathFilename;

    for (ITF_VECTOR<FilesPosition>::iterator iter = _orderedList.begin();iter!=_orderedList.end();iter++)
    {
       FilesPosition& filePos = (*iter);

       filePos.m_resourceID.getPath()->getString(pathFilename);
       String filename = FILESERVER->getCookedName(pathFilename);
       getPositionInBundle(filename, filePos);
    }

    std::sort(_orderedList.begin(), _orderedList.end(), sortFilePosition);
}

//----------------------------------------------------------------------------------------------------------------------------------------

bbool BundleManager::isRegistered( const String& _szFilename ) const
{
    csAutoLock  lock( &m_cs );
    String8     str = _szFilename.cStr();

    for ( u32 i = 0; i < m_bundles.size(); ++i )
        if ( m_bundles[i].keyWordName == str )
            return btrue;
    return bfalse;
}

//----------------------------------------------------------------------------------------------------------------------------------------

u32 BundleManager::loadBundles()
{
    BundleLists*                pBundleList = getBundleList();
    ITF_VECTOR< BundleInfo >    bundles;
    u32                         opId;

    {
        csAutoLock lock( &m_cs );

        bundles = m_bundles;
        opId = m_opId;
    }

    ITF_ASSERT_CRASH( pBundleList == NULL, "Should be unloaded before" );
    pBundleList = newAlloc( mId_System, BundleLists );
    m_TLSBundle.setValue( pBundleList );

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Loading bundles", ThreadSettings::getCurrentThreadName() );
#endif

    for ( u32 i = 0; i < bundles.size(); ++i )
    {
        BundleFile* pBundle = openBundle( bundles[i], pBundleList->m_registered.getSize() );

        if ( pBundle != NULL )
            pBundleList->m_registered.addFile( pBundle );
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Loading bundles done", ThreadSettings::getCurrentThreadName() );
#endif

    pBundleList->m_opId = opId;
    return pBundleList->m_registered.getSize();
}

//------------------------------------------------------------------------------------------------------------------------------------

void BundleManager::unloadBundles()
{
    BundleLists* pBundleList = getBundleList();

    ITF_ASSERT_CRASH( pBundleList != NULL, "Should be loaded before" );

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Unloading bundles", ThreadSettings::getCurrentThreadName() );
#endif

    unloadBundles( pBundleList->m_registered );
    unloadBundles( pBundleList->m_secure );

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Unloading bundles done", ThreadSettings::getCurrentThreadName() );
#endif

    SF_DEL( pBundleList );
    m_TLSBundle.setNULL();
}

//------------------------------------------------------------------------------------------------------------------------------------

bbool  BundleManager::getPositionInBundle(const String& _szFileName, FilesPosition& filePos)
{
    BundleFile* file = getBundle( _szFileName );

    if ( file != NULL )
    {
        filePos.m_file = file;
        file->fileGetPosition(_szFileName, filePos);
        return btrue;
    }

    return bfalse;
}

//------------------------------------------------------------------------------------------------------------------------------------

void BundleManager::synchronize()
{
    BundleLists*                pBundleList = getBundleList();
    ITF_VECTOR< BundleInfo >    bundles;
    u32                         opId;

    ITF_ASSERT_CRASH( pBundleList != NULL, "loadBundles must be called on this thread before" );

    {
        csAutoLock lock( &m_cs );

        opId = m_opId;
        if ( pBundleList->m_opId == opId )
            return;
        bundles = m_bundles;
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s] Synchronize", ThreadSettings::getCurrentThreadName() );
    LOG( "[BUNDLE][%s]  Listing", ThreadSettings::getCurrentThreadName() );
    for ( u32 i = 0; i < pBundleList->m_registered.getSize(); ++i )
    {
        BundleFile* pFile = pBundleList->m_registered.getBundleFile( i );

        LOG( "[BUNDLE][%s]      %s in regular list", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get() );
    }
    for ( u32 i = 0; i < pBundleList->m_secure.getSize(); ++i )
    {
        BundleFile* pFile = pBundleList->m_secure.getBundleFile( i );

        LOG( "[BUNDLE][%s]      %s in secure list", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get() );
    }
    LOG( "[BUNDLE][%s]  Listing done", ThreadSettings::getCurrentThreadName() );
#endif

    for ( u32 i = 0; i < bundles.size(); ++i )
    {
        BundleFile* pFile = NULL;

        if ( i < pBundleList->m_registered.getSize() )
        {
            pFile = pBundleList->m_registered.getBundleFile( i );
            if ( pFile->getFilename() == bundles[i].bundleFilename )
                continue;
        }

        u32 found = i + 1;

        while ( found < pBundleList->m_registered.getSize() )
        {
            pFile = pBundleList->m_registered.getBundleFile( found );
            if ( pFile->getFilename() == bundles[i].bundleFilename )
                break;
            found += 1;
        }

        if ( found < pBundleList->m_registered.getSize() )
        {
            pFile = pBundleList->m_registered.getBundleFile( found );

#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[BUNDLE][%s] Moving bundle %s from %d to %d index", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get(), found, i );
#endif

            pFile->setPriority( bundles[i].prio );
            pBundleList->m_registered.removeFile( found );
            pBundleList->m_registered.insertFile( pFile, i );
            continue;
        }

        found = i + 1;

        while ( found < pBundleList->m_secure.getSize() )
        {
            pFile = pBundleList->m_secure.getBundleFile( found );
            if ( pFile->getFilename() == bundles[i].bundleFilename )
                break;
            found += 1;
        }

        if ( found < pBundleList->m_secure.getSize() )
        {
            pFile = pBundleList->m_secure.getBundleFile( found );

#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[BUNDLE][%s] Synchronizing file %s from secure list", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get() );
#endif

            pBundleList->m_secure.removeFile( found );
            pBundleList->m_registered.insertFile( pFile, i );
            continue;
        }

        pFile = openBundle( bundles[i], pBundleList->m_registered.getSize() );
        if ( pFile != NULL )
            pBundleList->m_registered.insertFile( pFile, i );
    }

    while ( pBundleList->m_registered.getSize() > bundles.size() )
    {
        unloadBundle( pBundleList->m_registered, bundles.size() );
        pBundleList->m_registered.removeFile( bundles.size() );
    }

    u32 i = 0;

    while ( i < pBundleList->m_secure.getSize() )
    {
        BundleFile* pFile = pBundleList->m_secure.getBundleFile( i );

        if ( pFile->GetTrackedReferences().empty() )
        {
            unloadBundle( pBundleList->m_secure, i );
            pBundleList->m_secure.removeFile( i );
            continue;
        }
        ++i;
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s]  Listing", ThreadSettings::getCurrentThreadName() );
    for ( u32 i = 0; i < pBundleList->m_registered.getSize(); ++i )
    {
        BundleFile* pFile = pBundleList->m_registered.getBundleFile( i );

        LOG( "[BUNDLE][%s]      %s in regular list", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get() );
    }
    for ( u32 i = 0; i < pBundleList->m_secure.getSize(); ++i )
    {
        BundleFile* pFile = pBundleList->m_secure.getBundleFile( i );

        LOG( "[BUNDLE][%s]      %s in secure list", ThreadSettings::getCurrentThreadName(), StringToUTF8(pFile->getFilename()).get() );
    }
    LOG( "[BUNDLE][%s]  Listing done", ThreadSettings::getCurrentThreadName() );
    LOG( "[BUNDLE][%s] Synchronize done", ThreadSettings::getCurrentThreadName() );
#endif

    pBundleList->m_opId = opId;
}

//------------------------------------------------------------------------------------------------------------------------------------

Filepack*   BundleManager::fileOpen(const String& _szFileName)
{
    BundleFile* file = getBundle( _szFileName );

    if ( file != NULL )
        return file->fileOpen( _szFileName );
    return NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------

u64   BundleManager::fileGetLastTimeWriteAccess(const String& _szFileName)
{
    BundleFile* file = getBundle( _szFileName );

    if ( file != NULL )
        return file->fileGetLastTimeWriteAccess( _szFileName );
    return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------

bbool   BundleManager::fileExists(const String& _szFileName)
{
    return getBundle( _szFileName ) != NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------

u64     BundleManager::fileGetSize(const String& _szFileName)
{
    BundleFile* file = getBundle( _szFileName );

    if ( file != NULL )
        return file->fileGetSize( _szFileName );
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------

BundleFile* BundleManager::openBundle( const BundleInfo& info, u32 index )
{
    SharableBundleHeader*   header = m_headers.acquireHeader( info.bundleFilename );
    bbool                   getHeaders = header == NULL;
    u32                     dataVersion = 0;

    if ( header == NULL )
        header = newAlloc( mId_System, SharableBundleHeader( m_Root ) );

    BundleFile* pBundle = newAlloc( mId_System, BundleFile );

    if (!pBundle->openBundle( info.bundleFilename, header, index, getHeaders, dataVersion, info.useCache ))
    {
        delete pBundle;
        if (getHeaders)
            delete header;
        else
            m_headers.release(info.bundleFilename);
        return NULL;
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s]  Bundle %s opened %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(pBundle->getFilename()).get(), getHeaders ? "and headers retrieved" : "using already loaded headers" );
#endif

    pBundle->setPriority( info.prio );

#ifdef ITF_X360
    if ( m_secureFat != NULL )
    {
        pBundle->m_sortKey = m_secureFat->getSortKey( info.bundleFilename );
#ifndef ITF_NO_BUNDLE_TRACKING
        if ( pBundle->m_sortKey == U32_INVALID )
        {
            LOG( "[BUNDLE][%s]  Invalid sort key for bundle %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(pBundle->getFilename()).get() );
        }
#endif
    }
#endif

    if ( getHeaders && !m_headers.add( info.bundleFilename, header ) )
        pBundle->overrideHeader( header );
    BUNDLEMANAGER->setDataVersion( dataVersion );
    return pBundle;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void BundleManager::unloadBundles( BundleList& list )
{
    for ( u32 i = 0; i < list.getSize(); ++i )
        unloadBundle( list, i );
    list.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void BundleManager::unloadBundle( BundleList& list, u32 index )
{
    BundleFile* file = list.getBundleFile( index );

    const ITF_SET< Filepack* >& trackedRef = file->GetTrackedReferences();

    if ( !trackedRef.empty() )
    {
#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[BUNDLE][%s] Trying to unload bundle %s with following files still in use:", ThreadSettings::getCurrentThreadName(), StringToUTF8(file->getFilename()).get() );
        for ( ITF_SET< Filepack* >::const_iterator it = trackedRef.begin(); it != trackedRef.end(); ++it )
        {
            LOG( "[BUNDLE][%s]  %s still in use", ThreadSettings::getCurrentThreadName(), StringToUTF8(( *it )->getFilename()).get() );
        }
#endif

        csAutoLock      lock( &m_pendingsCs );
        BundlePending   pending;

        pending.threadId = ThreadSettings::getCurrentThreadId();
        pending.pFile = file;
        m_pendingDeletePool.push_back( pending );
        return;
    }

#ifndef ITF_NO_BUNDLE_TRACKING
    LOG( "[BUNDLE][%s]  Unloading bundle %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(file->getFilename()).get() );
#endif

    String filename = file->getFilename();

    delete file;
    m_headers.release( filename.cStr() );
}

//----------------------------------------------------------------------------------------------------------------------------------------

bbool BundleManager::unregisterBundle( u32 index )
{
    if ( m_bundles[index].refCount == 1 )
    {
#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[BUNDLE][%s] Unregistering bundle %s", ThreadSettings::getCurrentThreadName(), m_bundles[index].bundleFilename.cStr() );
#endif
        m_bundles.erase( m_bundles.begin() + index );
        m_opId += 1;
    }
    else
    {
        m_bundles[index].refCount -= 1;
#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[BUNDLE][%s] Removing reference on bundle %s (%d)", ThreadSettings::getCurrentThreadName(), m_bundles[index].bundleFilename.cStr(), m_bundles[index].refCount );
#endif
    }
    return btrue;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void BundleManager::removePendings()
{
    csAutoLock  lock( &m_pendingsCs );
    String      filename;

    u32 i = 0;

#if !defined( ITF_NO_BUNDLE_TRACKING ) && !defined( ITF_NO_REMOVE_PENDING_TRACKING )
    if ( !m_pendingDeletePool.empty() )
    {
        LOG( "[BUNDLE] Trying to remove pendings files" );
    }
#endif

    while ( i < m_pendingDeletePool.size() )
    {
        BundlePending& pending = m_pendingDeletePool[i];

        if ( pending.pFile->GetTrackedReferences().empty() )
        {

#if !defined( ITF_NO_BUNDLE_TRACKING ) && !defined( ITF_NO_REMOVE_PENDING_TRACKING )
            LOG( "[BUNDLE]  %s removed from thread %s", StringToUTF8(pending.pFile->getFilename()).get(), ThreadSettings::m_settings[pending.threadId].m_name );
#endif
            filename = pending.pFile->getFilename();
            delete pending.pFile;
            m_pendingDeletePool.erase( m_pendingDeletePool.begin() + i );
            m_headers.release( filename.cStr() );
            continue;
        }
        ++i;
    }

#if !defined( ITF_NO_BUNDLE_TRACKING ) && !defined( ITF_NO_REMOVE_PENDING_TRACKING )
    for ( u32 i = 0; i < m_pendingDeletePool.size(); ++i )
    {
        BundlePending&              pending = m_pendingDeletePool[i];
        const ITF_SET< Filepack* >& trackedRef = pending.pFile->GetTrackedReferences();

        LOG( "[BUNDLE]  %s still pending from thread %s:", StringToUTF8(pending.pFile->getFilename()).get(), ThreadSettings::m_settings[pending.threadId].m_name );
        for ( ITF_SET< Filepack* >::const_iterator it = trackedRef.begin(); it != trackedRef.end(); ++it )
        {
            LOG( "[BUNDLE]      %s still in use", StringToUTF8(( *it )->getFilename()).get() );
        }
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------

BundleFile* BundleManager::getBundle( const String& _szFileName )
{
    BundleLists*    pbundleList = getBundleList();
    i32             prio = FileManager::Highest;

    ITF_ASSERT_CRASH( pbundleList != NULL, "loadBundles must be called on this thread before" );
    while ( prio >= FileManager::Lowest )
    {
        for ( i32 i = pbundleList->m_registered.getSize() - 1; i >= 0 ; --i )
        {
            BundleFile* file = pbundleList->m_registered.getBundleFile( i );

            ITF_ASSERT( file->getPriority() >= FileManager::Lowest && file->getPriority() <= FileManager::Highest );
            if ( file->getPriority() == prio && file->fileExists( _szFileName ) )
                return file;
        }
        --prio;
    }

    for ( u32 i = 0; i < pbundleList->m_secure.getSize(); ++i )
    {
        BundleFile* file = pbundleList->m_secure.getBundleFile( i );

        if ( file->fileExists( _szFileName ) )
        {
#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[BUNDLE][%s] Using file %s contained in unregistered bundle %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(_szFileName).get(), StringToUTF8(file->getFilename()).get() );
#endif
            return file;
        }
    }

    if ( m_secureFat != NULL )
    {
        String bundleFilename = m_secureFat->findBundleContaining( _szFileName );

        if ( bundleFilename.isEmpty() )
        {
#ifndef ITF_NO_BUNDLE_TRACKING
            LOG( "[BUNDLE][%s] Using secure fat to retrieve file %s but no suitable bundle found", ThreadSettings::getCurrentThreadName(), StringToUTF8(_szFileName).get() );
#endif
            return NULL;
        }
#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[BUNDLE][%s] Using bundle %s from secure fat to retrieve file %s", ThreadSettings::getCurrentThreadName(), StringToUTF8(bundleFilename).get(), StringToUTF8(_szFileName).get() );
#endif
        
        BundleInfo info;

        FmtBundleFilename( bundleFilename );
        info.bundleFilename = bundleFilename.cStr();

        BundleFile* pBundle = openBundle( info, pbundleList->m_secure.getSize() );

        ITF_ASSERT_CRASH( pBundle->fileExists( _szFileName ), "Secure fat return bundle %s but does not contains file %s", StringToUTF8(info.bundleFilename).get(), StringToUTF8(_szFileName).get() );
        pbundleList->m_secure.addFile( pBundle );
        return pBundle;
    }
    return NULL;
}

}
