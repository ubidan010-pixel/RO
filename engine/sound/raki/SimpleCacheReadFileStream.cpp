#include "precompiled_engine.h"
#ifdef ITF_SUPPORT_RAKI


#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_



namespace ITF
{

void * SimpleCacheReadFileStream::ms_cache = NULL;
u64 SimpleCacheReadFileStream::ms_cacheSize = 0;
bool SimpleCacheReadFileStream::ms_cacheIsUsed = false;



void SimpleCacheReadFileStream::allocateStaticCache( const u64 _cacheSize )
{
    RAKI_ASSERT( !ms_cache );

    ms_cache = raki::Memory::malloc( _cacheSize, raki::Memory::engine );
    ms_cacheSize = _cacheSize;

    RAKI_ASSERT( ms_cache );
}


void SimpleCacheReadFileStream::freeStaticCache()
{
    RAKI_ASSERT( !ms_cacheIsUsed );

    if ( ms_cache )
        raki::Memory::free( ms_cache );

    ms_cache = NULL;
    ms_cacheSize = 0;
}



SimpleCacheReadFileStream::SimpleCacheReadFileStream()
    : m_file( NULL )
    , m_size( 0 )
    , m_offsetInFile( 0 )
{
    RAKI_ASSERT( !ms_cacheIsUsed );
    RAKI_ASSERT( ms_cache );
    RAKI_ASSERT( ms_cacheSize );

    ms_cacheIsUsed = true;

    setIsReading();
}


SimpleCacheReadFileStream::~SimpleCacheReadFileStream()
{
    close();

    ms_cacheIsUsed = false;
}


bool SimpleCacheReadFileStream::open( const String &  _path )
{
    if ( m_file )
        close();

    m_offsetInFile = 0;

    FILEMANAGER->reloadBundles();
    m_file = FILEMANAGER->openFile( _path, ITF_FILE_ATTR_READ );
    m_size = FILEMANAGER->getFileSize( _path );

    if ( !m_file )
    {
        m_size = 0;
        return false;
    }

    ITF_ASSERT( m_size );

    u32 sizeToRead = ( m_size < ms_cacheSize ) ? (u32)m_size : (u32)ms_cacheSize; 
    u32 sizeRead = 0;

    m_file->read( ms_cache, sizeToRead, &sizeRead );

    if ( m_size < ms_cacheSize )
    {
        if ( sizeRead < m_size )
        {
            m_size = 0;
            return false;
        }
    }
    else
    {
        if ( sizeRead < ms_cacheSize )
        {
            m_size = 0;
            return false;
        }
    }

    return true;
}


void SimpleCacheReadFileStream::close()
{
    if ( m_file )
        FILEMANAGER->closeFile( m_file );

    m_file = NULL;
}


const u64 SimpleCacheReadFileStream::streamFromCache( void * _data, const u64 _size )
{
    RAKI_ASSERT( m_offsetInFile  < ms_cacheSize );

    u64 sizeCanRead = _size;

    if ( m_offsetInFile + sizeCanRead > ms_cacheSize )
        sizeCanRead = ms_cacheSize - m_offsetInFile;

    if ( m_offsetInFile + sizeCanRead > m_size )
        sizeCanRead = m_size - m_offsetInFile;

    if ( sizeCanRead )
    {
        void * src = m_offsetInFile + (u8*) ms_cache;

        raki::Memory::memcpy( _data, src, sizeCanRead );
    }

    m_offsetInFile += sizeCanRead;

    return sizeCanRead;
}


const u64 SimpleCacheReadFileStream::streamFromFile( void * _data, const u64 _size )
{
    RAKI_ASSERT( m_offsetInFile  >= ms_cacheSize );

    u32 sizeRead = 0;

    if ( m_file )
        m_file->read( (void*)_data, (u32)_size, &sizeRead );

    m_offsetInFile += sizeRead;

    return (u64)sizeRead;
}


const u64 SimpleCacheReadFileStream::stream( void * _data, const u64 _size )
{
    ITF_ASSERT( _data );
    ITF_ASSERT( _size );

    u64 sizeRead = 0;

/*#ifdef ITF_WII
    DCFlushRange( (void*)_data, (u32)_size );
#endif // ITF_WII*/

    if ( m_offsetInFile < ms_cacheSize )
    {
        sizeRead = streamFromCache( _data, _size );

        if ( sizeRead && ( sizeRead < _size ) && ( m_offsetInFile < m_size ) )
        {
            void * dest = sizeRead + (u8*) _data;

            sizeRead += streamFromFile( dest, ( _size - sizeRead ) );
        }
    }
    else
    {
        sizeRead = streamFromFile( _data, _size );
    }

/*#ifdef ITF_WII
//    DCStoreRange( (void*)_data, (u32)_size );
     if ( sizeRead )
         DCFlushRange( (void*)_data, (u32)sizeRead );
#endif // ITF_WII*/

    return sizeRead;
}


const u64 SimpleCacheReadFileStream::seek( const u64 _size, const SeekType _seekType )
{
    if ( m_file )
    {
        if ( _seekType == fromBeginning )
        {
            if ( _size <= ms_cacheSize )
            {
                if ( m_offsetInFile > ms_cacheSize )
                    m_file->seek( ms_cacheSize, begin );

                m_offsetInFile = _size;
            }
            else
            {
                m_offsetInFile = _size;
                m_file->seek( _size, begin );
            }
        }
        else if ( _seekType == fromCurrentPos )
        {
            m_offsetInFile += _size;

            if ( m_offsetInFile > ms_cacheSize )
                m_file->seek( m_offsetInFile, begin );
        }
        else
        {
            ITF_ASSERT(0);
        }
    }

    return m_offsetInFile;
}

const u64 SimpleCacheReadFileStream::getCurrentPos() const
{
    return m_offsetInFile;
}


const u64 SimpleCacheReadFileStream::getSize() const
{
    if ( m_file )
        return m_size;

    return 0;
}




} // namespace ITF 

#endif
