#include "precompiled_core.h"

#ifdef ITF_SUPPORT_XML

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

void SerializerToolXML::Serialize( const char* tag, char& b )
{
    ITF_STDSTRING typeTag = "ch";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),b);
}

void SerializerToolXML::Serialize( const char* tag, short& s )
{
    ITF_STDSTRING typeTag = "sh";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),s);
}

void SerializerToolXML::Serialize( const char* tag, i32& i )
{
    ITF_STDSTRING typeTag = "i";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),i);
}

void SerializerToolXML::Serialize( const char* tag, u32& i )
{
    ITF_STDSTRING typeTag = "ui";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),i);
}

void SerializerToolXML::Serialize( const char* tag, float& f )
{
    ITF_STDSTRING typeTag = "f";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),f);
}

void SerializerToolXML::Serialize( const char* tag, double& f )
{
    ITF_STDSTRING typeTag = "d";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),f);
}

void SerializerToolXML::Serialize( const char* tag, bool& b )
{
    ITF_STDSTRING typeTag = "b";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),b);
}

void SerializerToolXML::Serialize( const char* tag, String& str )
{
    ITF_STDSTRING typeTag = "str";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),str);
}

void SerializerToolXML::Serialize( const char* tag, String8& str )
{
    ITF_STDSTRING typeTag = "str";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),str);
}

void SerializerToolXML::Serialize( const char* tag, ITF_STDSTRING& str )
{
    ITF_STDSTRING typeTag = "str";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),str);
}

void SerializerToolXML::Serialize( const char* tag, Vec2d& vec )
{
    ITF_STDSTRING typeTag = "vec2";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),vec);
}

void SerializerToolXML::Serialize( const char* tag, Vec3d& vec )
{
    ITF_STDSTRING typeTag = "vec3";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),vec);
}

void SerializerToolXML::Serialize( const char* tag, ObjectPath& _path )
{
    ITF_STDSTRING typeTag = "str";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_path);
}

void SerializerToolXML::Serialize( const char* tag, Color& _color )
{
    ITF_STDSTRING typeTag = "col";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_color);
}

void SerializerToolXML::Serialize( const char* tag, Angle& _angle )
{
    ITF_STDSTRING typeTag = "ang";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_angle);
}

void SerializerToolXML::Serialize( const char* tag, AngleAmount& _angle )
{
    ITF_STDSTRING typeTag = "f";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_angle);
}

void SerializerToolXML::Serialize( const char* tag, Path& _path )
{
    ITF_STDSTRING typeTag = "path";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_path);
}

void SerializerToolXML::Serialize( const char* tag, StringID& _id )
{
    ITF_STDSTRING typeTag = "str";
    typeTag += tag;

    Super::Serialize(typeTag.c_str(),_id);
}

void SerializerToolXML::Serialize( const char* tag, LocalisationId& _id )
{
    Serialize( tag, _id.value );
}

}

#endif //ITF_SUPPORT_XML

