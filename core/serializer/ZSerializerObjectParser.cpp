#include "precompiled_core.h"

#ifdef ITF_SUPPORT_XML

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

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

#include <sstream>

namespace ITF
{

CSerializerObjectParser::CSerializerObjectParser(PugiXMLWrap * _current)
: m_xml(NULL)
, m_buffer(NULL)
, m_size(NULL)
, m_insideEnum(bfalse)
{
    Init(_current);
}

CSerializerObjectParser::~CSerializerObjectParser()
{
    //delete m_parser;
}

void CSerializerObjectParser::Init( PugiXMLWrap * _current )
{
    m_read = true;
    m_xml = _current;
    m_insideEnum = bfalse;
}

bbool CSerializerObjectParser::Open( const char* file, bbool read )
{
    Close();

    if ( read )
    {
        m_read = btrue;
        m_xml = new PugiXMLWrap;
        return m_xml->openFile(file,false);
    }
    else
    {
        m_read = false;
        
        m_doc.reset();

        pugi::xml_node decl = m_doc.prepend_child(pugi::node_declaration);
        decl.append_attribute("version").set_value("1.0");
        decl.append_attribute("encoding").set_value("ISO-8859-1");

        m_current = m_doc.append_child( "root" );
        m_path = file;
        return btrue;
    }
}

bbool CSerializerObjectParser::OpenToRead( const char* buffer )
{
    Close();

    m_read = btrue;
    m_xml = new PugiXMLWrap;

    return m_xml->openString(buffer);
}

void CSerializerObjectParser::OpenToWrite( char** buffer, uSize* size )
{
    Close();
    
    m_read = false;
    m_doc.reset();

    pugi::xml_node decl = m_doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version").set_value("1.0");
    decl.append_attribute("encoding").set_value("ISO-8859-1");
 
    m_current = m_doc.append_child( "root" );
    m_buffer = buffer;
    m_size = size;
    m_path.clear();
}

class PugiXmlWriterToString8 : public pugi::xml_writer
{
public:
    PugiXmlWriterToString8() = default;

    virtual void write(const void* data, size_t size) override
    {
        const char* str = static_cast<const char*>(data);
        String8 newText;
        newText.setText(str, u32(size));
        m_content += newText;
    }

    const String8 & getContent() const
    {
        return m_content;
    }

private:
    String8 m_content;
};


void CSerializerObjectParser::Close()
{
    if (m_read)
    {
        SF_DEL(m_xml);
    }
    else if ( m_doc.first_child() )
    {
        if ( m_path.isEmpty() )
        {
            PugiXmlWriterToString8 writer;
            m_doc.save(writer);

           
            *m_size = uSize(writer.getContent().getLen() + 1u);
            *m_buffer = new char[*m_size];
            strcpy(*m_buffer, writer.getContent().cStr());
        }
        else
        {
            m_doc.save_file(m_path.cStr());
            m_path.clear();
        }

        m_doc.reset();
    }

    m_size = NULL;
    m_buffer = NULL;
}

bbool CSerializerObjectParser::OpenGroup( const char* tag, uSize index )
{
    if ( m_read )
    {
        return m_xml->enterElement(tag, index);
    }
    else
    {
        m_current = m_current.append_child(tag);
        return btrue;
    }
}

void CSerializerObjectParser::CloseGroup()
{
    if ( m_read )
    {
        m_xml->exitElement();
    }
    else
    {
        m_current = m_current.parent();
    }
}

void CSerializerObjectParser::Serialize( const char* tag, char& b )
{
    if ( m_read )
    {
        const char* attribute = m_xml->readAttribute(tag);

        if ( attribute )
        {
            b = attribute[0];
        }
    }
    else
    {
        String8 temp = "";
        temp += b;

        m_current.append_attribute(tag).set_value(temp.cStr());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, unsigned char& b )
{
    if ( m_read )
    {
        const char* attribute = m_xml->readAttribute(tag);

        if ( attribute )
        {
            b = (unsigned char)attribute[0];
        }
    }
    else
    {
        String8 temp = "";
        temp += b;

        m_current.append_attribute(tag).set_value(temp.cStr());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, short& s )
{
    if ( m_read )
    {
        s = static_cast<short>(m_xml->readIntAttribute(tag, s));
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%d",s);
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, int& i )
{
    if ( m_read )
    {
        i = m_xml->readIntAttribute(tag, i);
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%d",i);
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, u32& ui )
{
    if ( m_read )
    {
        const char* charValue = m_xml->readAttribute(tag);

        if ( charValue )
        {
            ui = static_cast<u32>(strtoul(charValue,NULL,10));
        }
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%u",ui);
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, StringID & _id )
{
    if ( m_read )
    {
        const char *result;
        result = m_xml->readAttribute(tag, NULL);
        if (result)
        {
            _id = result;
        }
    }
    else
    {
#ifdef ITF_DEBUG_STRINGID
        m_current.append_attribute(tag).set_value(_id.getDebugString());
#else
        ITF_ASSERT_CRASH(bfalse, "MUST NOT BE CALLED IN THIS CONDITION - Strings in StringID are not available");
#endif
    }
}

void CSerializerObjectParser::Serialize( const char* tag, float& i )
{
    if ( m_read )
    {
        i = m_xml->readFloatAttribute(tag, i);
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%f",i);
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, double& i )
{
    if ( m_read )
    {
        i = m_xml->readFloatAttribute(tag, (float)i);
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%f",i);

        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, bool& b )
{
    if ( m_read )
    {
        b = m_xml->readIntAttribute(tag, b) != 0;
    }
    else
    {
        char buf[64];
        SPRINTF_S(buf,64,"%d",b);
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, String& str )
{
    StringConverter s(str);
    if ( m_read )
    {
        str = m_xml->readAttribute(tag, s.getChar());
    }
    else
    {
        m_current.append_attribute(tag).set_value(s.getChar());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, String8& str )
{
    if ( m_read )
    {
        str = m_xml->readAttribute(tag, str.cStr());
    }
    else
    {
        m_current.append_attribute(tag).set_value(str.cStr());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, ITF_STDSTRING& _str )
{
    if ( m_read )
    {
        _str = m_xml->readAttribute(tag, _str.c_str());
    }
    else
    {
        m_current.append_attribute(tag).set_value(_str.c_str());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, ObjectId & _id)
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        static_assert(sizeof(unsigned long long) == sizeof(_id.m_id));
        if ( val.getLen() )
        {
            SSCANF_S(val.cStr(),"%016llx", reinterpret_cast<unsigned long long*>(&_id.m_id));
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%016llx", static_cast<unsigned long long>(_id.m_id));
        m_current.append_attribute(tag).set_value(buf);
    }
}

//void CSerializerObjectParser::Serialize( const char* tag, CStringID& ret )
//{
//    if ( m_read )
//    {
//        CStringID::IDType id = m_currentNode->GetUInt(tag,CStringID::InvalidID);
//        ret.SetUniqueID(id);
//    }
//    else
//    {
//        m_currentNodeWrite->SetUInt(tag,ret.GetUniqueID());
//    }
//}

void CSerializerObjectParser::Serialize( const char* tag, Vec2d& vec )
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            SSCANF_S(val.cStr(),"%f %f",&vec.m_x,&vec.m_y);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f %f",vec.m_x,vec.m_y);

        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, Vec3d& vec )
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            SSCANF_S(val.cStr(),"%f %f %f",&vec.m_x,&vec.m_y,&vec.m_z);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f %f %f",vec.m_x,vec.m_y,vec.m_z);

        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, ObjectPath& _path )
{
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

void CSerializerObjectParser::Serialize( const char* tag, Color& _color )
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            f32 rgba[4];
            ITF_MemSet(rgba,0,sizeof(rgba));
            SSCANF_S(val.cStr(),"%f %f %f %f",&rgba[0],&rgba[1],&rgba[2],&rgba[3]);
            _color = Color(rgba[3],rgba[0],rgba[1],rgba[2]);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f %f %f %f",_color.getRed(),_color.getGreen(),_color.getBlue(),_color.getAlpha());

        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, Angle& _angle )
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            f32 deg;
            SSCANF_S(val.cStr(),"%f",&deg);
            _angle.SetDegrees(deg);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f",_angle.ToDegrees360());
        
        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, AngleAmount& _angle )
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            f32 deg;
            SSCANF_S(val.cStr(),"%f",&deg);
            _angle.SetDegrees(deg);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f",_angle.ToDegrees());

        m_current.append_attribute(tag).set_value(buf);
    }
}

void CSerializerObjectParser::Serialize( const char* tag, Volume& _volume)
{
    if ( m_read )
    {
        String8 val;

        val = m_xml->readAttribute(tag);

        if ( val.getLen() )
        {
            f32 db;
            SSCANF_S(val.cStr(),"%f",&db);
            _volume.setdB(db);
        }
    }
    else
    {
        char buf[256];
        SPRINTF_S(buf,256,"%f",_volume.dB());

        m_current.append_attribute(tag).set_value(buf);
    }
}
void CSerializerObjectParser::Serialize( const char* tag, Path& _path )
{
    PathString8_t fileName;

    _path.getString(fileName);

    if ( m_read )
    {
        _path = m_xml->readAttribute(tag, fileName.cStr());
    }
    else
    {
        m_current.append_attribute(tag).set_value(fileName.cStr());
    }
}

void CSerializerObjectParser::Serialize( const char* tag, LocalisationId& _id )
{
    Serialize( tag, _id.value );
}

void CSerializerObjectParser::SerializeEnumBegin( const char* tag, int& selection )
{
    if ( m_read )
    {
        uSize count = 0;

        if (ReadArrayCount("ENUM", count))
        {        
            for ( uSize i = 0; i < count; i++ )
            {
                OpenGroup("ENUM",i);

                String8 name;

                Serialize("NAME",name);

                if ( !_strcmpi(name.cStr(),tag) )
                {
                    Serialize("SEL",selection);
                    m_insideEnum = btrue;
                    break;
                }
                else
                {
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        String enumTag = tag;
        OpenGroup("ENUM");
        Serialize("NAME",enumTag);
        Serialize("SEL",selection);
        m_insideEnum = btrue;
    }
}

void CSerializerObjectParser::SerializeEnumVar( int var_num, const char* var )
{
    if ( !m_read && m_insideEnum )
    {
        String str = var;

        OpenGroup("ENUMVAR");
        Serialize("NAME",str);
        Serialize("NUM",var_num);
        CloseGroup();
    }
}

void CSerializerObjectParser::SerializeEnumEnd()
{
    if ( m_insideEnum )
    {
        CloseGroup();
        m_insideEnum = bfalse;
    }
}

bbool CSerializerObjectParser::ReadArrayCount( const char* tag, uSize & count )
{
    //count = m_currentNode->GetNumChildren(tag);
    count = 0;
    if (m_xml->enterElement(tag))
    {
        do
        {
            count ++;
        } while (m_xml->nextSameElement());
    }
    return btrue;
}

bbool CSerializerObjectParser::WriteArrayCount( const char* tag, uSize count )
{
    return btrue;
}

} // namespace ITF
#endif //ITF_SUPPORT_XML
