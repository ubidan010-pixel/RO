#ifndef SERIALIZERFILE_H
#define SERIALIZERFILE_H

#include "core/serializer/ZSerializer.h"

namespace ITF
{

class CSerializerFile : public ISerializer
{
public:

    static constexpr uSize MAX_BUF_SIZE = 1024;

    CSerializerFile() { m_charBuf[0] = '\0'; }
    ~CSerializerFile() {}

    bbool        OpenRead( const char* file );
    bbool        OpenWrite( const char* file );
    void        Close();
    
    //bool        IsEnd() const;
    //void        Begin();


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
    virtual void ReadBuffer( char** buffer, uSize & size ) override;
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

    class File*         m_file;
    bbool               m_read;
    char                m_charBuf[MAX_BUF_SIZE];
};

} // namespace ITF


#endif // SERIALIZERFILE_H

