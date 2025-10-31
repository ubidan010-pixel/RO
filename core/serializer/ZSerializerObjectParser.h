#ifndef SERIALIZEROBJECTPARSER_H
#define SERIALIZEROBJECTPARSER_H

#ifdef ITF_SUPPORT_XML

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

namespace ITF
{

class CSerializerObjectParser : public CSerializerObject
{
public:

    CSerializerObjectParser(): m_xml(NULL), m_buffer(NULL), m_size(NULL), m_insideEnum(bfalse), m_current(NULL) {}
    CSerializerObjectParser(pugi::xml_node _current): CSerializerObject(false),m_xml(NULL), m_buffer(NULL), m_size(NULL), m_insideEnum(bfalse), m_current(_current) {}
    CSerializerObjectParser(PugiXMLWrap * _current);
    virtual ~CSerializerObjectParser();

    void         Init( PugiXMLWrap * _current );
    bbool        Open( const char* file, bbool read );
    bbool        OpenToRead( const char* buffer );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// OpenToWrite stores a destination buffer that will be allocated in the function "Close()"
    /// @param buffer the address of the pointer of the buffer
    /// @param size the address of the size of the buffer
    void         OpenToWrite( char** buffer, uSize* size );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Close the parser and allocate the given buffer and set the written size
    /// WARNING : YOU have to desallocate the buffer
    void         Close();

    virtual bbool OpenGroup( const char* tag, uSize index = 0 ) override;
    virtual void CloseGroup() override;

    virtual void Serialize( const char* tag, char& b ) override;
    virtual void Serialize( const char* tag, unsigned char& b ) override;
    virtual void Serialize( const char* tag, short& s ) override;
    virtual void Serialize( const char* tag, int& i ) override;
    virtual void Serialize( const char* tag, u32& i ) override;
    virtual void Serialize( const char* tag, float& i ) override;
    virtual void Serialize( const char* tag, double& i ) override;
    virtual void Serialize( const char* tag, bool& b ) override;
    virtual void Serialize( const char* tag, String & str ) override;
    virtual void Serialize( const char* tag, ITF_STDSTRING & str ) override;
    virtual void Serialize( const char* tag, class Vec2d& vec ) override;
    virtual void Serialize( const char* tag, class Vec3d& vec ) override;
    virtual void Serialize( const char* tag, ObjectPath& _path ) override;
    virtual void Serialize( const char* tag, class Color& _color ) override;
    virtual void Serialize( const char* tag, class Angle& _angle ) override;
    virtual void Serialize( const char* tag, class AngleAmount& _angle ) override;
    virtual void Serialize( const char* tag, class Volume& _volume) override;
    virtual void Serialize( const char* tag, class Path& _path ) override;
    virtual void Serialize( const char * tag, ObjectId & _id) override;
    virtual void Serialize( const char* tag, StringID & _id) override;
    virtual void Serialize( const char * tag, String8 & _id) override;
    virtual void Serialize( const char* tag, struct LocalisationId& _id ) override;

    virtual void SerializeEnumBegin( const char* tag, int& selection ) override;
    virtual void SerializeEnumVar( int var_num, const char* var ) override;
    virtual void SerializeEnumEnd() override;

protected:

    virtual bbool ReadArrayCount( const char* tag, uSize& count ) override;
    virtual bbool WriteArrayCount( const char* tag, uSize count ) override;

    PugiXMLWrap * m_xml;
    pugi::xml_document m_doc;
    pugi::xml_node m_current;
    char** m_buffer;
    uSize* m_size;
    String8 m_path;
    bbool   m_insideEnum;
};


} // namespace ITF

#endif //ITF_SUPPORT_XML
#endif // SERIALIZEROBJECT_H


