

#ifndef SERIALIZER_H
#define SERIALIZER_H
namespace ITF
{

class ISerializer
{
public:

    virtual void ReadByte( char& b ) = 0;
    virtual void ReadShort( short& s ) = 0;
    virtual void ReadInt( int& i ) = 0;
    virtual void ReadUInt64( u64& i ) = 0;
    virtual void ReadUInt32( u32& i ) = 0;
    virtual void ReadFloat( float& f ) = 0;
    virtual void ReadDouble( double& f ) = 0;
    virtual void ReadBool( bool& b ) = 0;
    //virtual void ReadString( CStringID& str ) = 0;
    virtual void ReadString( String & str ) = 0;
    virtual void ReadBuffer( char** buffer, uSize& size ) = 0;
    virtual void ReadBufferNoAlloc( char* buffer, uSize size ) = 0;

    virtual void WriteByte( const char& b ) = 0;
    virtual void WriteShort( const short& s ) = 0;
    virtual void WriteInt( const int& i ) = 0;
    virtual void WriteUInt64( const u64& i ) = 0;
    virtual void WriteUInt32( const u32& i ) = 0;
    virtual void WriteFloat( const float& f ) = 0;
    virtual void WriteDouble( const double& f ) = 0;
    virtual void WriteBool( const bool& b ) = 0;
    //virtual void WriteString( const CStringID& str ) = 0;
    virtual void WriteString( const String& str ) = 0;
    virtual void WriteBuffer( char* buffer, uSize size ) = 0;
    virtual void WriteBufferNoAlloc( const char* buffer, uSize size ) = 0;
};
} // namespace ITF

#endif // SERIALIZER_H

