#include "precompiled_core.h"

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{

#define LEVEL_SEPARATOR "|"
#define LEVEL_UP "..|"

ObjectPath::ObjectPath()
: m_absolute(bfalse)
{

}

ObjectPath::ObjectPath(const ITF_STDSTRING& _path)
: m_absolute(bfalse)
{
    fromString(_path);
}

ObjectPath::~ObjectPath()
{

}

bbool ObjectPath::operator==( const ObjectPath& _path ) const
{
    if ( m_absolute != _path.m_absolute )
    {
        return bfalse;
    }

    if ( m_objId.isValid() && _path.m_objId.isValid() )
    {
        if ( m_objId != _path.m_objId )
        {
            return bfalse;
        }
    }
    else
    {
        if ( m_strId != _path.m_strId )
        {
            return bfalse;
        }
    }

    LevelArray::const_reverse_iterator itMine = m_levels.rbegin();
    LevelArray::const_reverse_iterator itHis = _path.m_levels.rbegin();

    for ( ; itMine != m_levels.rend() && itHis != _path.m_levels.rend(); ++itMine, ++itHis )
    {
        const Level& myLevel = *itMine;
        const Level& hisLevel = *itHis;

        if ( myLevel != hisLevel )
        {
            return bfalse;
        }
    }

    return ( itMine == m_levels.rend() ) == ( itHis == _path.m_levels.rend() );
}

void ObjectPath::toString( ITF_STDSTRING& _ret ) const
{
    for ( LevelArray::const_iterator it = m_levels.begin(); it != m_levels.end(); ++it )
    {
        const Level& currentLevel = *it;

        if ( currentLevel.m_parent )
        {
            _ret += LEVEL_UP;
        }
        else
        {
            _ret += currentLevel.m_name.cStr();
            _ret += LEVEL_SEPARATOR;
        }
    }

    _ret += m_strId.cStr();
}

void ObjectPath::fromString( const ITF_STDSTRING& _path )
{
    clear();

    uSize length = uSize(_path.length());
    const char* current = _path.c_str();
    const char* pSeparator = strstr(current, LEVEL_SEPARATOR);

    if ( pSeparator )
    {
        const char* extension = strstr(current, ".isc");
        bbool isAbsolute = extension && extension < pSeparator;
        setIsAbsolute(isAbsolute);
    }

    Level level;

    while ( pSeparator )
    {
        if ( !_strnicmp(current,"..",pSeparator-current) )
        {
            level.m_parent = btrue;
            level.m_name.clear();
        }
        else
        {
            level.m_parent = bfalse;
            level.m_name = current;
            level.m_name.truncate(uSize(pSeparator-current));
        }

        m_levels.push_back(level);

        current = pSeparator+1;
        pSeparator = strstr(current,LEVEL_SEPARATOR);
    }

    m_strId = current;
    m_strId.truncate(length-uSize(current-_path.c_str()));
}

}