
#include "precompiled_core.h"

#include "ZSerializerFile.h"

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{

bbool CSerializerFile::OpenRead( const char* file )
{
    m_file = FILEMANAGER->openFile(file,ITF_FILE_ATTR_READ);
    if (m_file)
    {
        m_read = btrue;
        return btrue;
    }

    return bfalse;
}

bbool CSerializerFile::OpenWrite( const char* file )
{
    m_file = FILEMANAGER->openFile(file,ITF_FILE_ATTR_WRITE);
    if (m_file)
    {
        m_read = bfalse;
        return btrue;
    }
    return bfalse;
}

void CSerializerFile::Close()
{
    m_file = FILEMANAGER->closeFile(m_file);
}

//bool CSerializerFile::IsEnd() const
//{
//    return m_file.feof(m_file) != 0;
//}
//
//void CSerializerFile::Begin()
//{
//    fseek(m_file,0,SEEK_SET);
//}

void CSerializerFile::ReadByte( char& b )
{
    ITF_ASSERT(m_read);
    m_file->read(&b,sizeof(char));
}

void CSerializerFile::ReadShort( short& s )
{
    ITF_ASSERT(m_read);
    m_file->read(&s,sizeof(short));
}

void CSerializerFile::ReadInt( int& i )
{
    ITF_ASSERT(m_read);
    m_file->read(&i,sizeof(int));
}

void CSerializerFile::ReadUInt64( u64& i )
{
    ITF_ASSERT(m_read);
    m_file->read(&i,sizeof(u64));
}

void CSerializerFile::ReadUInt32( u32& i )
{
    ITF_ASSERT(m_read);
    m_file->read(&i,sizeof(u32));
}

void CSerializerFile::ReadFloat( float& f )
{
    ITF_ASSERT(m_read);
    m_file->read(&f,sizeof(float));
}

void CSerializerFile::ReadDouble( double& f )
{
    ITF_ASSERT(m_read);
    m_file->read(&f,sizeof(double));
}

void CSerializerFile::ReadBool( bool& b )
{
    ITF_ASSERT(m_read);
    m_file->read(&b,sizeof(bool));
}

//void CSerializerFile::ReadString( CStringID& str )
//{
//    ITF_ASSERT(m_read);
//    CStringID::IDType id;
//    m_file.read(&id,sizeof(id));
//    str.SetUniqueID(id);
//}

void CSerializerFile::ReadString( String & str )
{
    ITF_ASSERT(m_read);

    uSize length = 0;
    m_file->read(&length, sizeof(uSize));
    m_file->read(m_charBuf, u32(sizeof(char)*length));

    if ( length < MAX_BUF_SIZE )
    {
        m_charBuf[length] = '\0';
    }
    else
    {
        m_charBuf[MAX_BUF_SIZE-1] = '\0';
    }

    str = m_charBuf;
}

void CSerializerFile::ReadBuffer( char** buffer, uSize & size )
{
    ITF_ASSERT(m_read);
    m_file->read(&size, sizeof(uSize));
    *buffer = new char[size];
    m_file->read(*buffer, u32(sizeof(char)*size));
}

void CSerializerFile::ReadBufferNoAlloc( char* buffer, uSize size )
{
    ITF_ASSERT(m_read);
    m_file->read(buffer, u32(sizeof(char)*size));
}

void CSerializerFile::WriteByte( const char& b )
{
    ITF_ASSERT(!m_read);
    m_file->write(&b,sizeof(char));
}

void CSerializerFile::WriteShort( const short& s )
{
    ITF_ASSERT(!m_read);
    m_file->write(&s,sizeof(short));
}

void CSerializerFile::WriteInt( const int& i )
{
    ITF_ASSERT(!m_read);
    m_file->write(&i,sizeof(int));
}

void CSerializerFile::WriteUInt64( const u64& i )
{
    ITF_ASSERT(!m_read);
    m_file->write(&i,sizeof(u64));
}

void CSerializerFile::WriteUInt32( const u32& i )
{
    ITF_ASSERT(!m_read);
    m_file->write(&i,sizeof(u32));
}

void CSerializerFile::WriteFloat( const float& f )
{
    ITF_ASSERT(!m_read);
    m_file->write(&f,sizeof(float));
}

void CSerializerFile::WriteDouble( const double& f )
{
    ITF_ASSERT(!m_read);
    m_file->write(&f,sizeof(double));
}

void CSerializerFile::WriteBool( const bool& b )
{
    ITF_ASSERT(!m_read);
    m_file->write(&b,sizeof(bool));
}

//void CSerializerFile::WriteString( const CStringID& str )
//{
//    ITF_ASSERT(!m_read);
//
//    CStringID::IDType id = str.GetUniqueID();
//    m_file.write(&id,sizeof(id));
//}

void CSerializerFile::WriteString( const String & str )
{
    ITF_ASSERT(!m_read);

    uSize length = str.getLen();
    m_file->write(&length,sizeof(uSize));
    m_file->write(StringConverter(str).getChar(), u32(sizeof(char)*length));
}

void CSerializerFile::WriteBuffer( char* buffer, uSize size )
{
    ITF_ASSERT(!m_read);
    m_file->write(&size, sizeof(uSize));
    m_file->write(buffer, u32(sizeof(char)*size));
}

void CSerializerFile::WriteBufferNoAlloc( const char* buffer, uSize size )
{
    ITF_ASSERT(!m_read);
    m_file->write(buffer, u32(sizeof(char)*size));
}


} // namespace ITF

