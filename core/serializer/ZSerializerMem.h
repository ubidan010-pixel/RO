

#ifndef SERIALIZERMEM_H
#define SERIALIZERMEM_H

#include "core/serializer/ZSerializer.h"

namespace ITF
{

class CSerializerMem : public ISerializer
{
public:

    static constexpr uSize MAX_BUF_SIZE = 1024;

    CSerializerMem() = default;
    ~CSerializerMem() = default;

    void        OpenRead( char* mem );
    void        OpenWrite( char* mem );
    
    virtual void ReadByte( char& b ) override;
    virtual void ReadShort( short& s ) override;
    virtual void ReadInt( int& i ) override;
    virtual void ReadUInt64( u64& i ) override;
    virtual void ReadUInt32( u32& i ) override;
    virtual void ReadFloat( float& f ) override;
    virtual void ReadDouble( double& f ) override;
    virtual void ReadBool( bool& b ) override;
    //virtual void ReadString( CStringID& str ) override;
    virtual void ReadString( String & str ) override;
    virtual void ReadBuffer( char** buffer, uSize& size ) override;
    virtual void ReadBufferNoAlloc( char* buffer, uSize size ) override;

    virtual void WriteByte( const char& b ) override;
    virtual void WriteShort( const short& s ) override;
    virtual void WriteInt( const int& i ) override;
    virtual void WriteUInt64( const u64& i ) override;
    virtual void WriteUInt32( const u32& i ) override;
    virtual void WriteFloat( const float& f ) override;
    virtual void WriteDouble( const double& f ) override;
    virtual void WriteBool( const bool& b ) override;
    //virtual void WriteString( const CStringID& str ) override;
    virtual void WriteString( const String & str ) override;
    virtual void WriteBuffer( char* buffer, uSize size ) override;
    virtual void WriteBufferNoAlloc( const char* buffer, uSize size ) override;

private:

    char*               m_mem = nullptr;
    bbool               m_read = btrue;
    size_t              m_index = 0;
    char                m_charBufM[MAX_BUF_SIZE] = "";
};


} // namespace ITF

#endif // SERIALIZERMEM_H

