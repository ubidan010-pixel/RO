#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

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

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_LOGICDATAUTILS_H_
#include "engine/logicdatabase/LogicDataUtils.h"
#endif //_ITF_LOGICDATAUTILS_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{
    #if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
        ITF_VECTOR<Path> CSerializerObjectLogicDB::m_alreadyHandledErrors;

        bbool CSerializerObjectLogicDB::isPathErrorHandled(const Path& _path)
        {
            ITF_VECTOR<Path>::iterator found = std::find(CSerializerObjectLogicDB::m_alreadyHandledErrors.begin(), CSerializerObjectLogicDB::m_alreadyHandledErrors.end(), _path);
            return found != CSerializerObjectLogicDB::m_alreadyHandledErrors.end();
        }

        void CSerializerObjectLogicDB::addHandledPathError(const Path& _path)
        {
            if (isPathErrorHandled(_path))
                return;
            m_alreadyHandledErrors.push_back(_path);
        }

    #endif // !defined (ITF_FINAL) && !defined (ITF_CONSOLE)

    void CSerializerObjectLogicDB::init(const Path & _path,const String& _platform)
    {
        m_bIsValid = btrue;
        PathString_t pathName;
        _path.getString(pathName);
        const LogicDataTable * tbl = LOGICDATABASE->getTableFromId(pathName,_platform);
        ITF_ASSERT_MSG(tbl, "Can't get table from Id : '%ls'", pathName.cStr());
        if ( tbl )
        {
            m_tables.push_back(tbl);
        }
        else
        {
            m_bIsValid = bfalse;
#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
            if (!isPathErrorHandled(_path))
            {
                ITF_ERROR("Problem loading file %s", _path.getString8().cStr());
                addHandledPathError(_path);
            }
#endif
        }
    }
    
    CSerializerObjectLogicDB::CSerializerObjectLogicDB(const Path & _path):CSerializerObject(btrue)//always in read mode
    {
        init(_path,FILESERVER->getPlatform());
    }

    CSerializerObjectLogicDB::CSerializerObjectLogicDB(const Path & _path,const String& _platform):CSerializerObject(btrue)//always in read mode
    {
       init(_path,_platform);
    }

    CSerializerObjectLogicDB::CSerializerObjectLogicDB(const String& _id, const String8 & _luaContent)
        : CSerializerObject(btrue)//always in read mode
    {
        m_bIsValid = btrue;
        const LogicDataLink* ldl    = LOGICDATABASE->createFileByBuffer(_id, _luaContent.cStr());
        const LogicDataTable* tbl   = ldl ? ldl->m_pLogicDataTable : NULL;
        ITF_ASSERT(tbl);
        if ( tbl )
        {
            m_tables.push_back(tbl);
        }
        else
        {
            m_bIsValid = bfalse;
            ITF_ERROR("Problem loading buffer %ls : %s", _id.cStr(), _luaContent.cStr());
        }
    }

    bbool CSerializerObjectLogicDB::isEmpty() const
    {
        if (!m_bIsValid)
            return btrue;
        return m_tables.back()->getIsEmpty();
    }

    bbool CSerializerObjectLogicDB::OpenGroup( const char* tag, uSize index )
    {
        if (!m_bIsValid)
            return bfalse;
        const LogicDataTable * tbl = m_tables.back()->getTable(tag);
        if (tbl)
        {
            if (tbl->getCount())
                tbl = (const LogicDataTable *)tbl->getIndexArrayRead(index);
            m_tables.push_back(tbl);
            return btrue;
        }
        return bfalse;
    }

    void CSerializerObjectLogicDB::CloseGroup()
    {
        if (!m_bIsValid)
            return;
        ITF_ASSERT(m_tables.size() > 0);
        m_tables.pop_back();
    }

    void CSerializerObjectLogicDB::Serialize( const char* tag, char& b )
    {
        b = m_tables.back()->getU8(tag,b);
    }

    void CSerializerObjectLogicDB::Serialize( const char* tag, unsigned char& b )
    {
        b = m_tables.back()->getU8(tag,b);
    }

    void CSerializerObjectLogicDB::Serialize( const char* tag, short& s )
    {
        ITF_ASSERT(0);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, u32& i )
    {
        i = m_tables.back()->getU32(tag,i);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, int& i )
    {
        i = m_tables.back()->getI32(tag,i);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, float& f )
    {
        f = m_tables.back()->getF32(tag,f);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, String & str )
    {
        m_tables.back()->getString(tag,str);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Vec2d& vec )
    {
        m_tables.back()->getVec2d(tag,vec);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Vec3d& vec )
    {
        m_tables.back()->getVec3d(tag,vec);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, ObjectPath& _path )
    {
        ITF_STDSTRING str;

        Serialize(tag,str);

        _path.fromString(str);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Color& _color )
    {
        m_tables.back()->getColor(tag,_color);
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Angle& _angle )
    {
        f32 degrees = m_tables.back()->getF32(tag,_angle.ToDegrees());
        _angle.SetDegrees(degrees);

    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Volume& _volume )
    {
        f32 db = m_tables.back()->getF32(tag,_volume.dB());
        _volume.setdB(db);

    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, StringID& _id )
    {
        String str;
        m_tables.back()->getString(tag,str);

        if ( !str.isEmpty() )
        {
            _id = StringConverter(str).getChar();
        }
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, class Path& _path )
    {
        String str;
        m_tables.back()->getString(tag,str);

        if ( !str.isEmpty() )
        {
            _path = str;
        }
    }
    void CSerializerObjectLogicDB::Serialize( const char * tag, String8 & _id)
    {
        String str;
        m_tables.back()->getString(tag,str);

        if ( !str.isEmpty() )
        {
            _id = StringConverter(str).getChar();
        }
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, ITF_STDSTRING& _str )
    {
        String str;
        m_tables.back()->getString(tag,str);

        if ( !str.isEmpty() )
        {
            _str = StringConverter(str).getChar();
        }
    }
    void CSerializerObjectLogicDB::Serialize( const char* tag, LocalisationId& _id )
    {
        Serialize( tag, _id.value );
    }
    bbool CSerializerObjectLogicDB::ReadArrayCount( const char* tag, uSize & count )
    {
        const   LogicDataTable* table = m_tables.back()->getTable(tag);
        count = 0;
        if (table)
            count = table->getCount();

        //LOG(">>>>>>%s %d",tag,count);
        return table != NULL;
    }
    bbool CSerializerObjectLogicDB::WriteArrayCount( const char* tag, uSize count )
    {
        ITF_ASSERT(0);
        return bfalse;
    }

} // namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE
