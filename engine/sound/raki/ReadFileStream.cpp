#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_



namespace ITF
{



ReadFileStream::ReadFileStream()
    : m_file( NULL )
    , m_size( 0 )
    , m_seekForopenFileAtFirstStream( 0 )
{
    setIsReading();
}


ReadFileStream::~ReadFileStream()
{
    close();
}


void ReadFileStream::openInStream( const String & _path )
{
    m_pathForOpenFileAtFirstStream = _path;
}


bool ReadFileStream::open( const String &  _path )
{
    m_pathForOpenFileAtFirstStream.clear();

    if ( m_file )
        close();

    FILEMANAGER->reloadBundles();
    m_file = FILEMANAGER->openFile( _path, ITF_FILE_ATTR_READ );
    m_size = FILEMANAGER->getFileSize( _path );

    if ( !m_file )
    {
        m_size = 0;
        return false;
    }

    ITF_ASSERT( m_size );

    return true;
}


void ReadFileStream::close()
{
    if ( m_file )
        FILEMANAGER->closeFile( m_file );

    m_file = NULL;
}


const u64 ReadFileStream::stream( void * _data, const u64 _size )
{
    ITF_ASSERT( _data );
    ITF_ASSERT( _size );

    u32 sizeRead = 0;

    if ( m_pathForOpenFileAtFirstStream.getLen() )
    {
        const String pathForOpenFileAtFirstStream = m_pathForOpenFileAtFirstStream;

        bool opened = open( pathForOpenFileAtFirstStream );

#ifndef ITF_DISABLE_LOG
        if ( !opened )
        {
            char * pathCharCopy = pathForOpenFileAtFirstStream.getCharCopy();
            LOG( "ReadFileStream::stream unable to open pathForOpenFileAtFirstStream %s", pathCharCopy );
            delete [] pathCharCopy;
        }
#endif // ITF_DISABLE_LOG

        if ( opened && m_seekForopenFileAtFirstStream )
            seek( m_seekForopenFileAtFirstStream, fromBeginning );

        m_seekForopenFileAtFirstStream = 0;
    }

/*#ifdef ITF_WII
    DCFlushRange( (void*)_data, (u32)_size );
#endif // ITF_WII*/

    if ( m_file )
        m_file->read( (void*)_data, (u32)_size, &sizeRead );

/*#ifdef ITF_WII
//    DCStoreRange( (void*)_data, (u32)_size );
     if ( sizeRead )
         DCFlushRange( (void*)_data, (u32)sizeRead );
#endif // ITF_WII*/

    return sizeRead;
}


const u64 ReadFileStream::seek( const u64 _size, const SeekType _seekType )
{
    u64 ret = 0;

    if ( m_pathForOpenFileAtFirstStream.getLen() )
    {
        ITF_ASSERT( _seekType == fromBeginning );

        m_seekForopenFileAtFirstStream = _size;

        ret = m_seekForopenFileAtFirstStream;
    }
    else if ( m_file )
    {
        if ( _seekType == fromBeginning )
        {
            ret = m_file->seek( _size, begin );
        }
        else if ( _seekType == fromCurrentPos )
        {
            ret = m_file->seek( _size, current );
        }
        else
        {
            ITF_ASSERT(0);
        }
    }

    return ret;
}

const u64 ReadFileStream::getCurrentPos() const
{
    if ( m_pathForOpenFileAtFirstStream.getLen() )
        return m_seekForopenFileAtFirstStream;

    if ( m_file )
        return m_file->seek( 0, current );

    return 0;
}


const u64 ReadFileStream::getSize() const
{
    if ( m_file )
        return m_size;

    return 0;
}




} // namespace ITF 

#endif
