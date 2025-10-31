#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI

#ifndef ITF_BINFILEREADER_H
#include "engine/sound/raki/BinFileReader.h"
#endif ITF_BINFILEREADER_H



#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif // _ITF_READFILESTREAM_H_




namespace ITF
{




    BinFileReader::BinFileReader()
        : m_size( 0 )
        , m_data( NULL )
    {
    }


    BinFileReader::~BinFileReader()
    {
        if ( m_data )
            raki::Memory::free( m_data );
    }


    bool BinFileReader::readFrom( const String & _path )
    {
        if ( m_data )
            return false;

        ReadFileStream file;

        if ( !file.open( _path ) )
            return false;

        m_size = file.getSize();

        m_data = raki::Memory::malloc( m_size, raki::Memory::data ); 

        if ( !m_data )
            return false;

        return m_size == file.stream( m_data, m_size );
    }


    const u64 BinFileReader::getSize()
    {
        return m_size;
    }


    void * BinFileReader::getData()
    {
        return m_data;
    }





} // namespace ITF 

#endif
