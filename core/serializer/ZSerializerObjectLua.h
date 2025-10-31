#ifndef SERIALIZEROBJECTLUA_H
#define SERIALIZEROBJECTLUA_H

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

struct lua_State;

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

class CSerializerObjectLua : public CSerializerObject
{
public:

    CSerializerObjectLua();
    virtual ~CSerializerObjectLua();

    bbool           OpenToRead( const String& _fileName );
    bbool           OpenToWrite( const String& _fileName, bbool _saveOnlyDiff = bfalse);
    void            Close();
    
    bbool           OpenBufferToWrite(char *name, bbool _saveOnlyDiff = bfalse, const char* templateName = NULL);
    ITF_STDSTRING   CloseBuffer();
    bbool           OpenGroup( const char* tag, uSize index = 0 ) override;
    void            CloseGroup() override;

    virtual void    Serialize( const char* tag, char& b ) override;
    virtual void    Serialize( const char* tag, unsigned char& b ) override;
    virtual void    Serialize( const char* tag, short& s ) override;
    virtual void    Serialize( const char* tag, int& i ) override;
    virtual void    Serialize( const char* tag, u32& i ) override;
    virtual void    Serialize( const char* tag, float& i ) override;
    virtual void    Serialize( const char* tag, double& i ) override;
    virtual void    Serialize( const char* tag, bool& b ) override;
    virtual void    Serialize( const char* tag, String & str ) override;
    virtual void    Serialize( const char* tag, ITF_STDSTRING & _str ) override;
    virtual void    Serialize( const char* tag, class Vec2d& vec ) override;
    virtual void    Serialize( const char* tag, class Vec3d& vec ) override;
    virtual void    Serialize( const char* tag, ObjectPath& _path ) override;
    virtual void    Serialize( const char* tag, class Color& _color ) override;
    virtual void    Serialize( const char* tag, class Angle& _angle ) override;
    virtual void    Serialize( const char* tag, class Volume& _volume) override;
    virtual void    Serialize( const char* tag, class Path& _path ) override;
    virtual void    Serialize( const char * tag, ObjectId & _id) override;
    virtual void    Serialize( const char* tag, StringID & _id) override;
    virtual void    Serialize( const char * tag, String8 & _id) override;
    virtual void    Serialize( const char* tag, struct LocalisationId& _id ) override;

    virtual void    SerializeEnumBegin( const char* tag, int& selection ) override;
    virtual void    SerializeEnumVar( int var_num, const char* var ) override;
    virtual void    SerializeEnumEnd() override;
    void            SetPath(const Path & _path) { m_path = _path; }

protected:

    virtual bbool   ReadArrayCount( const char* tag, uSize& count ) override;
    virtual bbool   WriteArrayCount( const char* tag, uSize count ) override;

    void            addIndentation();

    struct ContainerData
    {
        ContainerData() : m_index(0), m_total(0) {}
        ContainerData( u32 _total ) : m_index(0), m_total(_total) {}

        u32             m_index;
        u32             m_total;
    };

    ITF_STDSTRING               m_buffer;
    Path                        m_path;
    u32                         m_indent;
    ContainerData               m_currentContainer;
    ITF_STACK<ContainerData>    m_containers;
    bbool                       m_openGroup;
    
    LUAHandler*                 m_luaHandler;
    lua_State*                  m_luaState;

    static const u32 s_indentation = 4;
};


} // namespace ITF

#endif // SERIALIZEROBJECTLUA_H


