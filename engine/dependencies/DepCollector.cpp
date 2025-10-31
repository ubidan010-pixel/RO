#include "precompiled_engine.h"

#include <algorithm>

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_AUTOLOCK_H_
#include "core/system/AutoLock.h"
#endif //_ITF_AUTOLOCK_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

static const char* s_validExt[] =
{
    "act",
    "imt",
    "gmt",
    "isg",
    "fxk",
    "atl",
    "wav",
    "ptc",
    "pbk",
    "anm",
    "skl",
    "fcg",
    "isc",
    "isd",
    "tga",
    "png",
    "tsc",
    "dep",
    "fnt",
    "fx",
    "frt",
    "alias",
    "xpr",
    "bse",
    "loa",
    NULL
};

static bool DepFormatPath( String& filename )
{
    if ( filename.isEmpty() )
        return false;
    ITF_ASSERT_MSG( filename.getLen() < _MAX_PATH, "File path is too long" );
    if ( filename.getLen() >= _MAX_PATH )
        return false;
    filename.toLower();
    filename.replace( "\\", "/" );
    return true;
}

DepStats::DepStats()
: total( 0 )
, count( 0 )
, worst( 0 )
, best( static_cast< u32 >( -1 ) )
{
}

u32 DepStats::getAverage() const
{
    return total / count;
}

DepCollection::RawPath::RawPath()
{
    ITF_MemSet( path, 0, _MAX_PATH );
}

DepCollection::RawPath::RawPath( const String& path )
{
    ITF_ASSERT_CRASH( path.getLen() < _MAX_PATH, "Invalid path length" );
    ITF_MemSet( this->path, 0, _MAX_PATH );
    ITF_Memcpy( this->path, path.cStr(), path.getLen() * sizeof( u16 ) );
}

bool DepCollection::RawPath::operator <( const RawPath& rhs ) const
{
#ifdef ITF_WCHAR_IS_16BIT
    return wcscmp( reinterpret_cast< const wchar_t* >( path ), reinterpret_cast< const wchar_t* >( rhs.path ) ) < 0;
#else
    String path(this->path);
    String otherPath(rhs.path);
    return wcscmp(StringToWChar(path).get(), StringToWChar(otherPath).get()) < 0;
#endif
}

DepCollection::NETIterator::NETIterator( const Collection& container )
: _container( container )
, _it( container.begin() )
{
}

bool DepCollection::NETIterator::haveNext() const
{
    return _it != _container.end();
}

void DepCollection::NETIterator::get( String& filename ) const
{
    ITF_ASSERT_CRASH( _it != _container.end(), "Invalid iterator" );
    filename.setText( _it->path );
}

void DepCollection::NETIterator::next() 
{
    ++_it;
}

DepCollection::NETIterator& DepCollection::NETIterator::operator =( const NETIterator& /*rhs*/ )
{
    return *this;
}

bool DepCollection::load( const String& cacheKey, const String& filename, const String& platform )
{
    ITF_ASSERT_MSG( !cacheKey.isEmpty(), "Warning invalid cache key" );
    if ( cacheKey.isEmpty() )
        return false;

    String  depFilename = getDependencyFilenameFrom( filename, platform );
    File*   file = FILEMANAGER->openFile( depFilename, ITF_FILE_ATTR_READ );
    bool    success = false;

    if ( file != NULL )
    {
        u32 len = static_cast< u32 >( file->getLength() );
        u8* pData = new u8[len];

        file->read( pData, len );

        ArchiveMemory   am( pData, len );
        String          internalCacheKey;
        u32             size = 0;
        u16             path[_MAX_PATH];

        internalCacheKey.serialize( am );
        if ( internalCacheKey == cacheKey )
        {
            am.serialize( size );
            while ( size )
            {
                am.serializeBlock16( path, _MAX_PATH );
                m_col.insert( Collection::value_type( path ) );
                --size;
            }
            success = true;
        }
        SF_DEL_ARRAY( pData );
        SF_DEL( file );
    }
    return success;
}

void DepCollection::save( const String& cacheKey, const String& filename, const String& platform )
{
    ITF_ASSERT_MSG( !cacheKey.isEmpty(), "Warning invalid cache key" );
    if ( cacheKey.isEmpty() )
        return;

    ArchiveMemory   am;
    String          depFilename = getDependencyFilenameFrom( filename, platform );
    File*           file = FILEMANAGER->openFile( depFilename, ITF_FILE_ATTR_CREATE_NEW | ITF_FILE_ATTR_WRITE );

    if ( file != NULL )
    {
        u32 size = u32(m_col.size());

        cacheKey.serialize( am );
        am.serialize( size );

        DepCollection::iterator end = m_col.end();

        for ( DepCollection::iterator it = m_col.begin(); it != end; ++it )
            am.serializeBlock16( const_cast< u16* >( reinterpret_cast< const u16* >( it->path ) ), _MAX_PATH );
        file->write( am.getData(), am.getSize() );
        SF_DEL( file );
    }
}

bool DepCollection::add( const String& filename )
{
    m_swap = filename;
    if ( !DepFormatPath( m_swap ) )
        return false;
    return m_col.insert( Collection::value_type( m_swap ) ).second;
}

bool DepCollection::remove( const String& filename )
{
    m_swap = filename;
    if ( !DepFormatPath( m_swap ) )
        return false;
    return m_col.erase( m_swap ) != 0;
}

DepCollection::size_type DepCollection::size() const
{
    return m_col.size();
}

bool DepCollection::contains( const String& filename ) const
{
    m_swap = filename;
    return DepFormatPath( m_swap ) && m_col.find( m_swap ) != m_col.end();
}

void DepCollection::clear()
{
    m_col.clear();
    m_swap.clear();
}

DepCollection::NETIterator DepCollection::getNetIterator() const
{
    return NETIterator( m_col );
}

DepCollection::iterator DepCollection::begin()
{
    return m_col.begin();
}

DepCollection::const_iterator DepCollection::begin() const
{
    return m_col.begin();
}

DepCollection::iterator DepCollection::end()
{
    return m_col.end();
}

DepCollection::const_iterator DepCollection::end() const
{
    return m_col.end();
}

void DepCollection::initFilters()
{
    initFilters( s_validExt );
}

void DepCollection::initFilters( const char** filters )
{
    m_extFilters.clear();
    if ( filters == NULL )
        return;
    for ( u32 i = 0; filters[i] != NULL; ++i )
        m_extFilters.insert( filters[i] );
}

void DepCollection::destroyFilters()
{
    m_extFilters.clear();
}

bool DepCollection::filterFile( const String& filename ) const
{
    if ( m_extFilters.empty() )
        return false;
    return m_extFilters.find( FilePath::getExtension( filename ) ) == m_extFilters.end();
}

void DepCollection::filterExt()
{
    if ( m_extFilters.empty() )
        return;
    
    iterator it = m_col.begin();
    iterator end = m_col.end();

    while ( it != end )
    {
        if ( filterFile( it->path ) )
            it = m_col.erase( it );
        else
            ++it;
    }
}

String DepCollection::getDependencyFilenameFrom( const String& filename, const String& platform )
{
    ITF_ASSERT_CRASH( FILESERVER != NULL, "Need of a valid file server instance" );

    String res;
    String cachePath = FILESERVER->getCachedNamePlatform( filename, platform, bfalse );

    res += FilePath::getDirectory( cachePath );
    res += FilePath::getFilenameWithoutExtension( cachePath );
    res += ".dep2.cache";
    return res;
}

DepCollector::DepCollector()
: m_filters( NULL )
{
    Synchronize::createCriticalSection( &m_cs );
}

DepCollector::~DepCollector()
{
    Synchronize::destroyCriticalSection( &m_cs );
}

bool DepCollector::contains( const String& filename ) const
{
    csAutoLock lock( &m_cs );

    m_processed.m_swap = filename;
    if ( !DepFormatPath( m_processed.m_swap ) )
        return false;
    return m_processed.m_col.find( m_processed.m_swap ) != m_processed.end() || m_pendings.m_col.find( m_processed.m_swap ) != m_pendings.end();
}

void DepCollector::clear()
{
    csAutoLock lock( &m_cs );

    m_processed.clear();
    m_pendings.clear();
    m_stats.clear();
}

bool DepCollector::add( const String& filename )
{
    csAutoLock lock( &m_cs );

    return m_pendings.add( filename );
}

void DepCollector::add( const DepCollection& col )
{
    csAutoLock                                      lock( &m_cs );
    ITF_VECTOR< DepCollection::RawPath >::iterator  it;

    m_diff.clear();
    m_diff.resize( col.m_col.size() );
    it = set_difference( col.begin(), col.end(), m_processed.begin(), m_processed.end(), m_diff.begin() );
    m_pendings.m_col.insert( m_diff.begin(), it );
}

bool DepCollector::getNextProcessing( String& filename )
{
    csAutoLock lock( &m_cs );

    if ( m_pendings.m_col.empty() )
        return false;
    filename.setText( m_pendings.begin()->path );
    m_processed.m_col.insert( *m_pendings.begin() );
    m_pendings.m_col.erase( m_pendings.m_col.begin() );
    return true;
}

void DepCollector::addStat( const String& ext, u32 timer )
{
    csAutoLock  lock( &m_cs );
    DepStats&   stats = m_stats[ext];

    stats.total += timer;
    stats.count += 1;
    if ( timer > stats.worst )
        stats.worst = timer;
    if ( timer < stats.best )
        stats.best = timer;
}

DepCollector::StatMap& DepCollector::getStats()
{
    return m_stats;
}

const DepCollector::StatMap& DepCollector::getStats() const
{
    return m_stats;
}

const char** DepCollector::getFilters() const
{
    return m_filters;
}

void DepCollector::setDefaultFilters()
{
    setFilters( s_validExt );
}

void DepCollector::setFilters( const char** filters )
{
    m_filters = filters;
}

void DepCollector::resetFilters()
{
    m_filters = NULL;
}

DepCollection& DepCollector::getProcessed()
{
    return m_processed;
}

const DepCollection& DepCollector::getProcessed() const
{
    return m_processed;
}

DepCollection& DepCollector::getPendings()
{
    return m_pendings;
}

const DepCollection& DepCollector::getPendings() const
{
    return m_pendings;
}

}
