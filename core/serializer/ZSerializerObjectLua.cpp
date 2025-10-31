#include "precompiled_core.h"

#ifdef ITF_SUPPORT_LUA

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

extern "C" 
{
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}

namespace ITF
{

CSerializerObjectLua::CSerializerObjectLua()
: m_indent(0)
, m_openGroup(bfalse)
, m_luaHandler(NULL)
, m_luaState(NULL)
{
    m_read = bfalse;
}

CSerializerObjectLua::~CSerializerObjectLua()
{
    Close();
}

bbool CSerializerObjectLua::OpenToRead( const String& _fileName )
{
    File* fileLoading = FILEMANAGER->openFile(_fileName,ITF_FILE_ATTR_READ);

    if (fileLoading)
    {
        m_read = btrue;

        u32 fileSize = (u32)fileLoading->getLength();
        char * readBuffer = new char[fileSize+1];
        u32 sizeRead = 0;
        fileLoading->read(readBuffer,fileSize,&sizeRead);
        ITF_ASSERT(fileSize == sizeRead);
        readBuffer[fileSize] = '\0';

        FILEMANAGER->closeFile(fileLoading);

        m_luaHandler = new LUAHandler;
        m_luaState = m_luaHandler->getLuaState();

        m_luaHandler->executeString(readBuffer);
        SF_DEL_ARRAY(readBuffer);
        LUAHandler::openGlobalTable(m_luaState,"params");
        return btrue;
    }
    else
    {
        return bfalse;
    }
}


bbool CSerializerObjectLua::OpenBufferToWrite(char * name, bbool _saveOnlyDiff, const char* templateName)
{
    m_indent        = 1;
    m_read          = bfalse;
#ifdef ITF_SUPPORT_EDITOR
    m_saveOnlyDiff  = _saveOnlyDiff;
#endif
    m_buffer        = "";
    m_buffer       += name;
    m_buffer       += " =\n{\n";
    m_openGroup     = btrue;
    
    if(templateName)
    {
        addIndentation();
        m_buffer   += "Name=\"";
        m_buffer   += templateName;
        m_buffer   += "\",\n";
    }
    return btrue;
}

ITF_STDSTRING CSerializerObjectLua::CloseBuffer()
{
    if ( m_openGroup && !m_buffer.empty() )
    {
        m_buffer += "}\n";
    }
    ITF_STDSTRING ret = m_buffer;

    m_openGroup = bfalse;
    m_buffer.clear();
    m_currentContainer.m_index = 0;
    m_currentContainer.m_total = 0;

    while ( !m_containers.empty() )
    {
        m_containers.pop();
    }

    return ret;
}


bbool CSerializerObjectLua::OpenToWrite( const String& _fileName, bbool _saveOnlyDiff )
{
    Close();

    m_path          = _fileName;
    return OpenBufferToWrite("params", _saveOnlyDiff);
}

void CSerializerObjectLua::Close()
{
    if ( m_read )
    {
        if ( m_luaHandler )
        {
            SF_DEL(m_luaHandler);
            m_luaState = NULL;
        }
    }
    else
    {
        ITF_STDSTRING bufferStr = CloseBuffer();
        if (!bufferStr.empty())
        {
            PathString_t fileName;

            m_path.getString(fileName);

            File* file = FILEMANAGER->openFile(fileName,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
            if ( file )
            {
                file->write(bufferStr.c_str(), u32(sizeof(u8)*bufferStr.length()));
                FILEMANAGER->closeFile(file);
            }
        }
    }
}

void CSerializerObjectLua::addIndentation()
{
    u32 spaces = m_indent*s_indentation;

    for ( u32 i = 0; i < spaces; i++ )
    {
        m_buffer += " ";
    }
}


bbool CSerializerObjectLua::OpenGroup( const char* tag, uSize index )
{
    if ( m_read )
    {
        if ( m_currentContainer.m_total )
        {
            if ( m_currentContainer.m_index == 0 )
            {
                LUAHandler::openTable(m_luaState,tag);
            }

            LUAHandler::openTable(m_luaState, index+1);
        }
        else
        {
            LUAHandler::openTable(m_luaState,tag);
        }
    }
    else
    {
        if ( m_currentContainer.m_total )
        {
            if ( index == 0 )
            {
                addIndentation();
                m_buffer += tag;
                m_buffer += " =\n";
                addIndentation();
                m_buffer += "{\n";
                m_indent++;
            }

            addIndentation();
            m_buffer += "{\n";
            m_indent++;
        }
        else
        {
            addIndentation();
            m_buffer += tag;
            m_buffer += " =\n";
            addIndentation();
            m_buffer += "{\n";
            m_indent++;
        }
    }

    m_currentContainer.m_index = index;
    m_containers.push(m_currentContainer);
    m_currentContainer.m_index = 0;
    m_currentContainer.m_total = 0;

    return btrue;
}

void CSerializerObjectLua::CloseGroup()
{
    m_currentContainer = m_containers.top();
    m_containers.pop();

    if ( m_read )
    {
        if ( m_currentContainer.m_total )
        {
            LUAHandler::closeTable(m_luaState);

            if ( m_currentContainer.m_index == m_currentContainer.m_total )
            {
                LUAHandler::closeTable(m_luaState);
                m_currentContainer.m_index = 0;
                m_currentContainer.m_total = 0;
            }
        }
        else
        {
            LUAHandler::closeTable(m_luaState);
        }
    }
    else
    {
        if ( m_currentContainer.m_total )
        {
            m_indent--;
            addIndentation();
            m_buffer += "},\n";

            m_currentContainer.m_index++;

            if ( m_currentContainer.m_index == m_currentContainer.m_total )
            {
                m_indent--;
                addIndentation();
                m_buffer += "},\n";
                m_currentContainer.m_index = 0;
                m_currentContainer.m_total = 0;
            }
        }
        else
        {
            m_indent--;
            addIndentation();
            m_buffer += "},\n";
        }
    }
}

void CSerializerObjectLua::Serialize( const char* tag, char& b )
{
    if ( m_read )
    {
        const char* str = LUAHandler::getCharFromTable(m_luaState,tag);

        if ( str )
        {
            b = *str;
        }
    }
    else if (!SkipDefaultValue(b))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = \"%c\",\n",tag,b);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, unsigned char& b )
{
    if ( m_read )
    {
        const char* str = LUAHandler::getCharFromTable(m_luaState,tag);

        if ( str )
        {
            b = (unsigned char)*str;
        }
    }
    else if (!SkipDefaultValue(b))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = \"%c\",\n",tag,b);
        m_buffer += buf;
    }
}


void CSerializerObjectLua::Serialize( const char* tag, short& s )
{
    if ( m_read )
    {
        s = static_cast<short>(LUAHandler::getF32FromTable(m_luaState,tag,static_cast<f32>(s)));
    }
    else if (!SkipDefaultValue(s))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %i,\n",tag,s);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, int& i )
{
    if ( m_read )
    {
        i = static_cast<int>(LUAHandler::getF32FromTable(m_luaState,tag,static_cast<f32>(i)));
    }
    else if (!SkipDefaultValue(i))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %i,\n",tag,i);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, u32& ui )
{
    if ( m_read )
    {
        ui = LUAHandler::getU32FromTable(m_luaState,tag,ui);
    }
    else if (!SkipDefaultValue(ui))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %u,\n",tag,ui);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, StringID & _id )
{
    if ( m_read )
    {
        const char *res = LUAHandler::getCharFromTable(m_luaState,tag,NULL);

        if (res)
        {
            _id = res;
        }
    }
    else if (!SkipDefaultValue(_id))
    {
#ifdef ITF_DEBUG_STRINGID
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = \"%s\",\n",tag,_id.getDebugString());
        m_buffer += buf;
#else
        ITF_ASSERT_CRASH(bfalse, "MUST NOT BE CALLED IN THIS CONDITION - Strings in StringID are not available");
#endif
    }
}

void CSerializerObjectLua::Serialize( const char* tag, float& i )
{
    if ( m_read )
    {
        i = LUAHandler::getF32FromTable(m_luaState,tag,i);
    }
    else if (!SkipDefaultValue(i))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %f,\n",tag,i);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, double& i )
{
    if ( m_read )
    {
        i = LUAHandler::getF32FromTable(m_luaState,tag,static_cast<f32>(i));
    }
    else if (!SkipDefaultValue(i))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %f,\n",tag,i);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, bool& b )
{
    if ( m_read )
    {
        b = (LUAHandler::getU32FromTable(m_luaState,tag,b?1:0)>0)?true:false;
    }
    else if (!SkipDefaultValue(b))
    {
        char buf[256];

        addIndentation();
        SPRINTF_S(buf,256,"%s = %d,\n",tag,b?1:0);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, String& str )
{
    if ( m_read )
    {
        LUAHandler::getStringFromTable(m_luaState,tag,str);
    }
    else if (!SkipDefaultValue(str))
    {
        if ( !str.isEmpty() )
        {
            char buf[256];
            addIndentation();
            SPRINTF_S(buf,256,"%s = \"%s\",\n",tag,StringConverter(str).getChar());
            m_buffer += buf;
        }
    }
}

void CSerializerObjectLua::Serialize( const char* tag, String8& str )
{
    if ( m_read )
    {
        LUAHandler::getString8FromTable(m_luaState,tag,str);
    }
    else if (!SkipDefaultValue(str))
    {
        if ( !str.isEmpty() )
        {
            char buf[256];
            addIndentation();
            SPRINTF_S(buf,256,"%s = \"%s\",\n",tag,str.cStr());
            m_buffer += buf;
        }
    }
}

void CSerializerObjectLua::Serialize( const char* tag, ITF_STDSTRING& _str )
{
    String8 str;

    if ( !m_read )
    {
        str = _str.c_str();
    }

    Serialize(tag,str);

    if ( m_read )
    {
        _str = str.cStr();
    }
}

void CSerializerObjectLua::Serialize( const char* tag, ObjectId & _id)
{
    if ( m_read )
    {
        const char* str = LUAHandler::getCharFromTable(m_luaState,tag,NULL);

        if ( str )
        {
#ifdef ITF_IPAD
            SSCANF_S(str,"%016I64x",&_id.m_id);
#else
			SSCANF_S(str,"%016llx",&_id.m_id);
#endif
        }
    }
    else if (!SkipDefaultValue(_id))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = \"%016llx\",\n",tag,_id.m_id);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, Vec2d& vec )
{
    if ( m_read )
    {
        vec = LUAHandler::getVec2dFromTable(m_luaState,tag,vec);
    }
    else if (!SkipDefaultValue(vec))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = vector2dNew(%f,%f),\n",tag,vec.m_x,vec.m_y);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, Vec3d& vec )
{
    if ( m_read )
    {
        vec = LUAHandler::getVec3dFromTable(m_luaState,tag,vec);
    }
    else if (!SkipDefaultValue(vec))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = vectorNew(%f,%f,%f),\n",tag,vec.m_x,vec.m_y,vec.m_z);
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, ObjectPath& _path )
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

void CSerializerObjectLua::Serialize( const char* tag, Color& _color )
{
    if ( m_read )
    {
        const char* str = LUAHandler::getCharFromTable(m_luaState,tag,NULL);

        if ( str )
        {
            u32 colorN;
            SSCANF_S(str,"0x%08x",&colorN);
            _color = Color(ITFCOLOR_FROM_ARGB(colorN));
        }
    }
    else if (!SkipDefaultValue(_color))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = \"0x%08x\",\n",tag,ITFCOLOR_TO_ARGB(_color.getAsU32()));
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, Angle& _angle )
{
    if ( m_read )
    {
        _angle.SetDegrees(LUAHandler::getF32FromTable(m_luaState,tag,_angle.ToDegrees()));
    }
    else if (!SkipDefaultValue(_angle))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = %f,\n",tag,_angle.ToDegrees());
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, Volume& _volume)
{
    if ( m_read )
    {
        _volume.setdB(LUAHandler::getF32FromTable(m_luaState,tag,0.0f));
    }
    else if (!SkipDefaultValue(_volume))
    {
        char buf[256];
        addIndentation();
        SPRINTF_S(buf,256,"%s = %f,\n",tag,_volume.dB());
        m_buffer += buf;
    }
}

void CSerializerObjectLua::Serialize( const char* tag, Path& _path )
{
    if ( m_read )
    {
        const char* str = LUAHandler::getCharFromTable(m_luaState,tag,NULL);

        if ( str )
        {
            _path = str;
        }
    }
    else if (!SkipDefaultValue(_path))
    {
        if ( !_path.isEmpty() )
        {
            PathString8_t fileName;

            _path.getString(fileName);

            char buf[256];
            addIndentation();
            SPRINTF_S(buf,256,"%s = \"%s\",\n",tag,fileName.cStr());
            m_buffer += buf;
        }
    }
}

void CSerializerObjectLua::Serialize( const char* tag, LocalisationId& _id )
{
    Serialize( tag, _id.value );
}

void CSerializerObjectLua::SerializeEnumBegin( const char* tag, int& selection )
{
    Serialize(tag,selection);
}

void CSerializerObjectLua::SerializeEnumVar( int var_num, const char* var )
{
}

void CSerializerObjectLua::SerializeEnumEnd()
{
}

bbool CSerializerObjectLua::ReadArrayCount( const char* tag, uSize& count )
{
    i32 size = 0;
    bbool res = bfalse;

    if ( LUAHandler::openTable(m_luaState,tag,size) )
    {
        LUAHandler::closeTable(m_luaState);
        res = btrue;
    }

    count = static_cast<uSize>(size);
    m_currentContainer.m_total = size;
    m_currentContainer.m_index = 0;
    return res;
}

bbool CSerializerObjectLua::WriteArrayCount( const char* tag, uSize count )
{
    if ( count )
    {
        m_currentContainer.m_total = u32(count);
        m_currentContainer.m_index = 0;
    }
    return btrue;
}

} // namespace ITF
#endif //ITF_SUPPORT_LUA
