#include "precompiled_core.h"

#ifndef SERIALIZEROBJECTSIGNATURE_H
#include "core/serializer/ZSerializerObjectSignature.h"
#endif // SERIALIZEROBJECTSIGNATURE_H

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

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifdef ITF_IPAD
#include <string>
#endif


//#include "ZColor.h"
//#include "ZVectorMath.h"
//#include "ZAngle.h"
namespace ITF
{

    // Types used to check binary serialization validity
    struct TypesSignature
    {
        enum Value
        {
            _unsigned_long=1,
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
            _LocalisationId,
            ENUM_FORCE_SIZE_32(0)
        };
    };

CSerializerObjectSignature::CSerializerObjectSignature(ArchiveMemory* _Archive) : m_serializer(_Archive) 
{
    if (m_serializer->isReading())
        m_read = btrue;
    else
        m_read = bfalse;

    m_properties |= ESerializerProperty_Binary;
}

void CSerializerObjectSignature::rewindForReading()
{
    m_serializer->rewindForReading();
    m_read = btrue;
}


u32 CSerializerObjectSignature::computeCRC()
{
     Versioning::serializeAll(*m_serializer);
     const unsigned char* pData = static_cast<unsigned char*>(m_serializer->getData());
     u32 crcArchive = CRC::compute32( pData, m_serializer->getSize());
     return crcArchive;
}


#define SERIALIZE_TYPE(TYPE)                                            \
{                                                                       \
    TypesSignature::Value em = TypesSignature::_##TYPE;                 \
    m_serializer->serializeByte((u8)(em));                              \
}                                                                       \


void CSerializerObjectSignature::Serialize( const char* tag, char& b )
{
    SERIALIZE_TYPE(char);
}

void CSerializerObjectSignature::Serialize( const char* tag, unsigned char& b )
{
    SERIALIZE_TYPE(unsigned_char);
}

void CSerializerObjectSignature::Serialize( const char* tag, short& s )
{
    SERIALIZE_TYPE(short);
}

void CSerializerObjectSignature::Serialize( const char* tag, int& i )
{
    SERIALIZE_TYPE(int);
}

void CSerializerObjectSignature::Serialize( const char* tag, u32& i )
{
    SERIALIZE_TYPE(u32);
}

void CSerializerObjectSignature::Serialize( const char* tag, float& f )
{
    SERIALIZE_TYPE(float);
}

void CSerializerObjectSignature::Serialize( const char* tag, double& d )
{
    SERIALIZE_TYPE(double);
}

void CSerializerObjectSignature::Serialize( const char* tag, bool& b )
{
    SERIALIZE_TYPE(bool);
}

void CSerializerObjectSignature::Serialize( const char* tag, String & str )
{
    SERIALIZE_TYPE(String);
}

void CSerializerObjectSignature::Serialize( const char * tag, ObjectId & _id)
{
   SERIALIZE_TYPE(ObjectId);
}

void CSerializerObjectSignature::Serialize( const char * tag, StringID & _id)
{
    SERIALIZE_TYPE(StringID);
}

void CSerializerObjectSignature::Serialize( const char* tag, String8& str )
{
    SERIALIZE_TYPE(String8);
}

void CSerializerObjectSignature::Serialize( const char* tag, ITF_STDSTRING & _str)
{
   SERIALIZE_TYPE(ITF_STDSTRING);
}

void CSerializerObjectSignature::Serialize( const char* tag, Vec2d& vec )
{
   SERIALIZE_TYPE(Vec2d);
}

void CSerializerObjectSignature::Serialize( const char* tag, Vec3d& vec )
{
   SERIALIZE_TYPE(Vec3d);
}

void CSerializerObjectSignature::Serialize( const char* tag, ObjectPath& _path )
{
   SERIALIZE_TYPE(ObjectPath);
}

void CSerializerObjectSignature::Serialize( const char* tag, Color& _color )
{
    SERIALIZE_TYPE(Color);
}

void CSerializerObjectSignature::Serialize( const char* tag, Angle& _angle )
{
    SERIALIZE_TYPE(Angle);
}

void CSerializerObjectSignature::Serialize( const char* tag, Path& _path )
{
    SERIALIZE_TYPE(Path);
}


void CSerializerObjectSignature::Serialize( const char* tag, Volume& _volume)
{
    SERIALIZE_TYPE(Volume);
}

void CSerializerObjectSignature::Serialize( const char* tag, LocalisationId& _id )
{
    SERIALIZE_TYPE(LocalisationId);
}

void CSerializerObjectSignature::SerializeEnumBegin( const char* tag, int& selection )
{
}

bbool CSerializerObjectSignature::ReadArrayCount( const char* tag, uSize& count )
{
   return btrue;
}

bbool CSerializerObjectSignature::WriteArrayCount( const char* tag, uSize count )
{
    static_assert(sizeof(uSize) == sizeof(u32), "For binary compatibility, assume uSize is 32 bit");
    SERIALIZE_TYPE(u32);
    return btrue;
}

} // namespace ITF
