#include "precompiled_core.h"

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef SERIALIZER_H
#include "core/serializer/ZSerializer.h"
#endif // SERIALIZER_H

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#ifdef ITF_IPAD
#include <string>
#endif


//#include "ZColor.h"
//#include "ZVectorMath.h"
//#include "ZAngle.h"
namespace ITF
{

    // Types used to check binary serialization validity
    struct Types
    {
        enum Value
        {
            _unsigned_long,
            _char,
            _unsigned_char,
            _short,
            _int,
            _u32,
            _Color,
            _Angle,
            _bool,
            _float,
            _double,
            _String,
            _String8,
            _StringID,
            _ObjectId,
            _ITF_STDSTRING,
            _Vec2d,
            _Vec3d,
            _Path,
            _Volume,
            _size_t,
            _ObjectPath,
            ENUM_FORCE_SIZE_32(0)
        };
    };

// Usefull to trace serialisation differences
//#define USE_CHECK_TYPE 

#if USE_CHECK_TYPE

    // If you have the assert, then someone may have break the symetry of binary serialization load/save
    // Try to find wich class member are serialized with only *****_Load or ******_Save flag
    #define CHECK_TYPE(TYPE)                                                                                                \
    {                                                                                                                       \
        Types::Value em = Types::_##TYPE;                                                                                   \
        m_serializer->serialize((u32&)em);                                                                                  \
        if(m_serializer->isReading())                                                                                       \
        {                                                                                                                   \
            ITF_ASSERT_CRASH(Types::_##TYPE == em, "Error, trying to read a specific type of data, but another was found"); \
        }                                                                                                                   \
    }                                                                                                                       \

#else

    #define CHECK_TYPE(TYPE)

#endif // !ITF_FINAL


#if 0
    #define DEBUG_LOG(_format, ...)     ITF::ERROR_HANDLER->Log(_format, __VA_ARGS__)
#else
    #define DEBUG_LOG(_format, ...)
#endif

CSerializerObjectBinary::CSerializerObjectBinary(ArchiveMemory* _Archive,bbool _bDestroySerializer)
{
    m_bDestroySerializer = _bDestroySerializer;
    Init( _Archive );
}


CSerializerObjectBinary::~CSerializerObjectBinary()
{
    if (m_bDestroySerializer)
    {
        SF_DEL(m_serializer);
    }
}


void CSerializerObjectBinary::Init( ArchiveMemory* _Archive )
{
    m_serializer = _Archive;
    if (m_serializer->isReading())
        m_read = btrue;
    else
        m_read = bfalse;

    m_properties |= ESerializerProperty_Binary;
}

void CSerializerObjectBinary::rewindForReading()
{
    m_serializer->rewindForReading();
    m_read = btrue;
}

void CSerializerObjectBinary::Serialize( const char* tag, char& b )
{
    DEBUG_LOG("%s :: %u", tag, b);
    CHECK_TYPE(char);
    m_serializer->serialize((char&)b);
}

void CSerializerObjectBinary::Serialize( const char* tag, unsigned char& b )
{
    DEBUG_LOG("%s :: %c", tag, b);
    CHECK_TYPE(unsigned_char);
    m_serializer->serialize((unsigned char&)b);
}

void CSerializerObjectBinary::Serialize( const char* tag, short& s )
{
    DEBUG_LOG("%s :: %u", tag, s);
    CHECK_TYPE(short);
    m_serializer->serialize((short&)s);
}

void CSerializerObjectBinary::Serialize( const char* tag, int& i )
{
    DEBUG_LOG("%s :: %i", tag, i);
    CHECK_TYPE(int);
    m_serializer->serialize((int&)i);
}

void CSerializerObjectBinary::Serialize( const char* tag, u32& i )
{
    DEBUG_LOG("%s :: %u", tag, i);
    CHECK_TYPE(u32);
    m_serializer->serialize((u32&)i);
}

void CSerializerObjectBinary::Serialize( const char* tag, float& f )
{
    DEBUG_LOG("%s :: %f", tag, f);
    CHECK_TYPE(float);
    m_serializer->serialize((float&)f);
}

void CSerializerObjectBinary::Serialize( const char* tag, double& d )
{
    DEBUG_LOG("%s :: %d", tag, d);
    CHECK_TYPE(double);
    m_serializer->serialize((double&)d);
}

void CSerializerObjectBinary::Serialize( const char* tag, bool& b )
{
    DEBUG_LOG("%s :: %u", tag, b);
    CHECK_TYPE(bool);
    m_serializer->serialize((bool&)b);
}

void CSerializerObjectBinary::Serialize( const char* tag, String & str )
{
    DEBUG_LOG("%s :: %s", tag, StringToUTF8(str).get());
    CHECK_TYPE(String);
    str.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char * tag, ObjectId & _id)
{
    DEBUG_LOG("%s :: oid", tag);
    CHECK_TYPE(ObjectId);
    m_serializer->serialize((u64&)_id.m_id);
}

void CSerializerObjectBinary::Serialize( const char * tag, StringID & _id)
{
    DEBUG_LOG("%s :: %s", tag, _id.c_str());
    CHECK_TYPE(StringID);
    m_serializer->serialize((u32&)_id.m_id);
}

void CSerializerObjectBinary::Serialize( const char* tag, String8& str )
{
    DEBUG_LOG("%s :: %s", tag, str.cStr());
    CHECK_TYPE(String8);
    str.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, ITF_STDSTRING & _str)
{
    String8 buf;

    if (!m_serializer->isReading())
    {
        buf = _str.c_str();
    }

    DEBUG_LOG("%s :: %s", tag, buf.cStr());
    CHECK_TYPE(ITF_STDSTRING);
    Serialize(tag,buf);

    if (m_serializer->isReading())
    {
        _str = buf.cStr();
    }
}

void CSerializerObjectBinary::Serialize( const char* tag, Vec2d& vec )
{
    DEBUG_LOG("%s :: vec2D", tag);
    CHECK_TYPE(Vec2d);
    vec.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, Vec3d& vec )
{
    DEBUG_LOG("%s :: vec3D", tag);
    CHECK_TYPE(Vec3d);
    vec.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, ObjectPath& _path )
{
    DEBUG_LOG("%s :: objPath", tag);
    CHECK_TYPE(ObjectPath);

    ITF_STDSTRING path;

    if ( m_read )
    {
        Serialize(tag,path);
        _path.fromString(path);
    }
    else
    {
        _path.toString(path);
        Serialize(tag,path);
    }
}

void CSerializerObjectBinary::Serialize( const char* tag, Color& _color )
{
    DEBUG_LOG("%s :: color", tag);
    CHECK_TYPE(Color);
    _color.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, Angle& _angle )
{
    DEBUG_LOG("%s :: angle", tag);
    CHECK_TYPE(Angle);
    _angle.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, Path& _path )
{
    DEBUG_LOG("%s :: path", tag);
    CHECK_TYPE(Path);
    _path.serialize(*m_serializer);
}


void CSerializerObjectBinary::Serialize( const char* tag, Volume& _volume)
{
    DEBUG_LOG("%s :: volume", tag);
    CHECK_TYPE(Volume);
    _volume.serialize(*m_serializer);
}

void CSerializerObjectBinary::Serialize( const char* tag, LocalisationId& _id )
{
    Serialize( tag, _id.value );
}

void CSerializerObjectBinary::SerializeEnumBegin( const char* tag, int& selection )
{
    DEBUG_LOG("%s :: %i", tag ? tag : "", selection);
    CHECK_TYPE(int);
    m_serializer->serialize((i32&)selection);
}

bbool CSerializerObjectBinary::ReadArrayCount( const char* tag, uSize& count )
{
    DEBUG_LOG("%s :: %u", tag, count);
    static_assert(sizeof(uSize) == sizeof(u32), "For binary compatibility, assume uSize is 32 bit");
    CHECK_TYPE(_u32);
    m_serializer->serialize(count);
    return btrue;
}

bbool CSerializerObjectBinary::WriteArrayCount( const char* tag, uSize count )
{
    DEBUG_LOG("%s :: %u", tag, count);
    static_assert(sizeof(uSize) == sizeof(u32), "For binary compatibility, assume uSize is 32 bit");
    CHECK_TYPE(_u32);
    m_serializer->serialize(count);
    return btrue;
}

} // namespace ITF
