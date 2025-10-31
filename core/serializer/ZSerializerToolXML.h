#ifndef SERIALIZERTOOLXML_H
#define SERIALIZERTOOLXML_H

#ifdef ITF_SUPPORT_XML

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

namespace ITF
{

class SerializerToolXML : public CSerializerObjectParser
{
    typedef CSerializerObjectParser Super;
public:

    SerializerToolXML() { m_properties |= ESerializerProperty_Tool; }

    virtual void Serialize( const char* tag, char& b ) override;
    virtual void Serialize( const char* tag, short& s ) override;
    virtual void Serialize( const char* tag, i32& i ) override;
    virtual void Serialize( const char* tag, u32& i ) override;
    virtual void Serialize( const char* tag, float& i ) override;
    virtual void Serialize( const char* tag, double& i ) override;
    virtual void Serialize( const char* tag, bool& b ) override;
    virtual void Serialize( const char* tag, String& str ) override;
    virtual void Serialize( const char* tag, String8 & _id) override;
    virtual void Serialize( const char* tag, ITF_STDSTRING & _id) override;
    virtual void Serialize( const char* tag, class Vec2d& vec ) override;
    virtual void Serialize( const char* tag, class Vec3d& vec ) override;
    virtual void Serialize( const char* tag, ObjectPath& _path ) override;
    virtual void Serialize( const char* tag, class Color& _color ) override;
    virtual void Serialize( const char* tag, class Angle& _angle ) override;
    virtual void Serialize( const char* tag, class AngleAmount& _angle ) override;
    virtual void Serialize( const char* tag, class Path& _path ) override;
    virtual void Serialize( const char* tag, class StringID& _id) override;
    virtual void Serialize( const char* tag, struct LocalisationId& _id ) override;
};

}

#endif //ITF_SUPPORT_XML

#endif // SERIALIZEROBJECT_H


