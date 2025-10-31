
#include "precompiled_core.h"
#include "ZSerializerMem.h"
#include <stdio.h>
namespace ITF
{

void CSerializerMem::OpenRead( char* mem )
{
    m_mem = static_cast<char*>(mem);
    m_index = 0;
    m_read = btrue;
}

void CSerializerMem::OpenWrite( char* mem )
{
    m_mem = static_cast<char*>(mem);
    m_index = 0;
    m_read = bfalse;
}

void CSerializerMem::ReadByte( char& b )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&b,(m_mem+m_index),sizeof(char));
    m_index += sizeof(char);
}

void CSerializerMem::ReadShort( short& s )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&s,&(m_mem[m_index]),sizeof(short));
    m_index += sizeof(short);
}

void CSerializerMem::ReadInt( int& i )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&i,(m_mem+m_index),sizeof(int));
    m_index += sizeof(int);
}

void CSerializerMem::ReadUInt64( u64& i )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&i,(m_mem+m_index),sizeof(u64));
    m_index += sizeof(u64);
}

void CSerializerMem::ReadUInt32( u32& i )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&i,(m_mem+m_index),sizeof(u32));
    m_index += sizeof(u32);
}

void CSerializerMem::ReadFloat( float& f )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&f,(m_mem+m_index),sizeof(float));
    m_index += sizeof(float);
}

void CSerializerMem::ReadDouble( double& f )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&f,(m_mem+m_index),sizeof(double));
    m_index += sizeof(double);
}

void CSerializerMem::ReadBool( bool& b )
{
    ITF_ASSERT(m_read);
    ITF_Memcpy(&b,(m_mem+m_index),sizeof(bool));
    m_index += sizeof(bool);
}

//void CSerializerMem::ReadString( CStringID& str )
//{
//    ITF_ASSERT(m_read);
//    CStringID::IDType id;
//    ITF_Memcpy(&id,(m_mem+m_index),sizeof(id));
//    str.SetUniqueID(id);
//    m_index += sizeof(id);
//}

void CSerializerMem::ReadString( String& str )
{
    ITF_ASSERT(m_read);

    uSize length = 0;
    ITF_Memcpy(&length, (m_mem+m_index), sizeof(uSize));
    m_index += sizeof(uSize);

    ITF_Memcpy(&m_charBufM, (m_mem+m_index), sizeof(char) * std::min(length, MAX_BUF_SIZE));
    m_index += sizeof(char)*static_cast<int>(length);

    if ( length < MAX_BUF_SIZE )
    {
        m_charBufM[length] = '\0';
    }
    else
    {
        m_charBufM[MAX_BUF_SIZE-1] = '\0';
    }

    str = m_charBufM;
}

void CSerializerMem::ReadBuffer( char** buffer, uSize& size )
{
    ITF_ASSERT(m_read);
    
    ITF_Memcpy(&size,(m_mem+m_index),sizeof(uSize));
    m_index += sizeof(uSize);

    *buffer = new char[size];

    ITF_Memcpy(*buffer,(m_mem+m_index),sizeof(char)*size);
    m_index += sizeof(char)*size;
}

void CSerializerMem::ReadBufferNoAlloc( char* buffer, uSize size )
{
    ITF_ASSERT(m_read);
    
    ITF_Memcpy(buffer,(m_mem+m_index),sizeof(char)*size);
    m_index += sizeof(char)*size;
}

void CSerializerMem::WriteByte( const char& b )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&b,sizeof(char));
    m_index += sizeof(char);
}

void CSerializerMem::WriteShort( const short& s )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&s,sizeof(short));
    m_index += sizeof(short);
}

void CSerializerMem::WriteInt( const int& i )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&i,sizeof(int));
    m_index += sizeof(int);
}

void CSerializerMem::WriteUInt64( const u64& i )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&i,sizeof(u64));
    m_index += sizeof(u64);
}

void CSerializerMem::WriteUInt32( const u32& i )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&i,sizeof(u32));
    m_index += sizeof(u32);
}

void CSerializerMem::WriteFloat( const float& f )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&f,sizeof(float));
    m_index += sizeof(float);
}

void CSerializerMem::WriteDouble( const double& f )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&f,sizeof(double));
    m_index += sizeof(double);
}

void CSerializerMem::WriteBool( const bool& b )
{
    ITF_ASSERT(!m_read);
    ITF_Memcpy((m_mem+m_index),&b,sizeof(bool));
    m_index += sizeof(bool);
}

//void CSerializerMem::WriteString( const CStringID& str )
//{
//    ITF_ASSERT(!m_read);
//    CStringID::IDType id = str.GetUniqueID();
//    memcpy((m_mem+m_index),&id,sizeof(id));
//    m_index += sizeof(id);
//}

void CSerializerMem::WriteString( const String& str )
{
    ITF_ASSERT(!m_read);

    uSize length = str.getLen();
    
    ITF_Memcpy((m_mem+m_index), &length, sizeof(uSize));
    m_index += sizeof(uSize);

    ITF_Memcpy((m_mem+m_index),StringConverter(str).getChar(),sizeof(char)*length);
    m_index += sizeof(char)*static_cast<int>(length);
}

void CSerializerMem::WriteBuffer( char* buffer, uSize size )
{
    ITF_ASSERT(!m_read);
    
    ITF_Memcpy((m_mem+m_index),&size, sizeof(uSize));
    m_index += sizeof(uSize);

    ITF_Memcpy((m_mem+m_index),buffer,sizeof(char)*size);
    m_index += static_cast<int>(sizeof(char)*size);
}

void CSerializerMem::WriteBufferNoAlloc( const char* buffer, uSize size )
{
    ITF_ASSERT(!m_read);
    
    ITF_Memcpy((m_mem+m_index),buffer,sizeof(char)*size);
    m_index += static_cast<int>(sizeof(char)*size);
}


} // namespace ITF
