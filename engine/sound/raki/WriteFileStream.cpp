#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI

#ifndef _ITF_WRITEFILESTREAM_H_
#include "engine/sound/raki/WriteFileStream.h"
#endif //_ITF_WRITEFILESTREAM_H_



namespace ITF
{

WriteFileStream::WriteFileStream()
    : m_file( NULL )
    , m_size( 0 )
{
    setIsWriting();
}


WriteFileStream::~WriteFileStream()
{
    close();
}

bool WriteFileStream::open( const String &  _path )
{
    if ( m_file )
        close();

    m_size = 0;

    m_file = FILEMANAGER->openFile( _path, ITF_FILE_ATTR_WRITE | ITF_FILE_ATTR_CREATE_NEW );

    if ( !m_file )
        return false;

    return true;
}


void WriteFileStream::close()
{
    if ( m_file )
        FILEMANAGER->closeFile( m_file );

    m_file = NULL;
}


const u64 WriteFileStream::stream( void * _data, const u64 _size )
{
    ITF_ASSERT( _data );
    ITF_ASSERT( _size );

    u32 sizeRead = 0;

    if ( m_file )
        m_file->write( (void*)_data, (u32)_size, &sizeRead );

    return sizeRead;
}


const u64 WriteFileStream::seek( const u64 _size, const SeekType _seekType )
{
    u64 ret = 0;

    if ( m_file )
    {
        if ( _seekType == SeekableStream::fromBeginning )
        {
            ret = m_file->seek( _size, begin );
        }
        else if ( _seekType == SeekableStream::fromCurrentPos )
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

const u64 WriteFileStream::getCurrentPos() const
{
    if ( m_file )
        return m_file->seek( 0, current );

    return 0;
}


const u64 WriteFileStream::getSize() const
{
    if ( m_file )
        return m_size;

    return 0;
}




} // namespace ITF 

#endif
