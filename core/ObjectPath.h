#ifndef _ITF_OBJECTPATH_H_
#define _ITF_OBJECTPATH_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

namespace ITF
{

class ObjectPath
{
public:

    struct Level 
    {
        DECLARE_SERIALIZE();

        Level() : m_parent(bfalse) {}

        bbool operator == ( const Level& _level ) const { return m_name == _level.m_name && m_parent == _level.m_parent; }
        bbool operator != ( const Level& _level ) const { return m_name != _level.m_name || m_parent != _level.m_parent; }

        String8             m_name;             // The name of the scene if we travel down
        bbool               m_parent;           // Set to true if the path is traveling up
    };

    ObjectPath();
    ObjectPath(const ITF_STDSTRING& _path);
    ~ObjectPath();

    bbool               operator==( const ObjectPath& _path ) const;

    void                clear() { m_levels.clear(); invalidate(); }
    void                addLevel( const Level& _level ) { m_levels.push_back(_level); }

    void                setStrId( const char* _strId ) { m_strId = _strId; m_objId.invalidate(); }
    const char*         getStrId() const { return m_strId.cStr(); }

    void                setObjId( const ObjectId& _objId ) const { m_objId = _objId; }
    const ObjectId&     getObjId() const { return m_objId; }

    void                setIsAbsolute( bbool _val ) { m_absolute = _val; }
    bbool               getIsAbsolute() const { return m_absolute; }

    u32                 getNumLevels() const { return m_levels.size(); }
    const Level&        getLevel( u32 _index ) const { return m_levels[_index]; }

    bbool               isValid() const { return !m_strId.isEmpty(); }
    void                invalidate() { m_strId.clear(); m_objId.invalidate(); }

    void                toString( ITF_STDSTRING& _ret ) const;
    void                fromString( const ITF_STDSTRING& _path );

private:

    typedef ITF_VECTOR <Level> LevelArray;
    LevelArray          m_levels;
    String8             m_strId;
    mutable ObjectId    m_objId;
    bbool               m_absolute;
};

} // namespace ITF

#endif // _ITF_OBJECTPATH_H_

