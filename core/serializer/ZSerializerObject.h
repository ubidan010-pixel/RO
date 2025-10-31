
#ifndef SERIALIZEROBJECT_H
#define SERIALIZEROBJECT_H

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#include "core/memory/memoryId.h"

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_VOLUME_
#include "core/sound/volume.h"
#endif //ITF_VOLUME_

#ifndef _ITF_ANGLE_H_
#include "core/math/Angle.h"
#endif //_ITF_ANGLE_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

namespace ITF
{

enum ESerializeFlags
{
    // Use this flags as a parameter for your serializer to define what you want to load or save
    ESerialize_PropertyEdit_Load        =   0x001, // ESerialize_PropertyEdit is used to serialize data for exchanging with the editors.
    ESerialize_PropertyEdit_Save        =   0x002,
    ESerialize_Checkpoint_Load          =   0x004, // ESerialize_Checkpoint is used to serialize data for checkpoint handling
    ESerialize_Checkpoint_Save          =   0x008,
    ESerialize_Editor_Load              =   0x010, // ESerialize_Editor is used to serialize parameters used by the editor (for copy pasting etc)
    ESerialize_Editor_Save              =   0x020,
    ESerialize_Data_Load                =   0x040, // ESerialize_Data is used to define that we load the data itself
    ESerialize_Data_Save                =   0x080,
    ESerialize_Deprecate                =   0x100, // A property that is not used anymore, but is read so that we can keep it compatible
                                                   // it is loaded but never saved
    ESerialize_DataRaw                  =   0x200, // Save|load data but not in binary format
    ESerialize_DataBin                  =   0x400, // Save|load data only in binary format

    // This defines the serializing properties of each parameter to serialize.
    // Do not use this flags directly, instead use the groups.
    ESerialize_PropertyEdit             =   ESerialize_PropertyEdit_Save|ESerialize_PropertyEdit_Load,
    ESerialize_Checkpoint               =   ESerialize_Checkpoint_Save|ESerialize_Checkpoint_Load,
    ESerialize_Editor                   =   ESerialize_Editor_Load | ESerialize_Editor_Save,
    ESerialize_Data                     =   ESerialize_Data_Load | ESerialize_Data_Save,

    // The groups are used in the condition blocks to decide what to serialize
    ESerializeGroup_Data                =   ESerialize_Data,
    ESerializeGroup_DataEditable        =   ESerialize_Data|ESerialize_PropertyEdit,
    ESerializeGroup_Checkpoint          =   ESerialize_Checkpoint,
    ESerializeGroup_PropertyEdit        =   ESerialize_PropertyEdit,
    ESerializeGroup_Deprecate           =   ESerialize_Deprecate,

    ENUM_FORCE_SIZE_32(B)
};

enum ESerializerProperties
{
    ESerializerProperty_Binary      =   0x001,
    ESerializerProperty_Tool        =   0x002,
    ESerializerProperty_MemoryDump  =   0x004,
    ENUM_FORCE_SIZE_32(C)
};

class ObjectFactory;


using serializeObjectAllowedCB = bbool (*)(const ObjectRef& _ref);

struct ObjectMemInfo
{
    u32                 m_size;
    uPtr                m_objMem;
    char*               m_defValues;
};


#define UNINIT_F32 F32_INFINITY
#define UNINIT_VEC2D Vec2d(F32_INFINITY,F32_INFINITY)
#define UNINIT_VEC3D Vec3d(F32_INFINITY,F32_INFINITY,F32_INFINITY)
#define UNINIT_BBOOL -1


class CSerializerObject
{
public:

    CSerializerObject() : m_read(bfalse), m_factory(NULL), 
#ifdef ITF_SUPPORT_EDITOR
        m_saveOnlyDiff(bfalse), 
#endif
        m_properties(0) {}
    CSerializerObject(bbool _read) : m_read(_read), m_factory(NULL), 
#ifdef ITF_SUPPORT_EDITOR
        m_saveOnlyDiff(bfalse),
#endif
        m_properties(0) {}

    virtual ~CSerializerObject() {}


    bbool           IsLoading() const { return m_read; }

    template <class T>
    bbool           OpenObjectGroup( const char* tag, T *_object, uSize index = 0);
    template <class T>
    void            CloseObjectGroup();


    virtual bbool   OpenGroup( const char* tag, uSize index = 0) = 0;
    virtual void    CloseGroup() = 0;

    template <class T>
    bbool           SkipDefaultValue( T& _value);

    void            setFactory( ObjectFactory* _factory ) { m_factory = _factory; }
    ObjectFactory*  getFactory() const { return m_factory; }

    u32             getProperties() const { return m_properties; }

    virtual void Serialize( const char* tag, char& b ) = 0;
    virtual void Serialize( const char* tag, unsigned char& b ) = 0;
    virtual void Serialize( const char* tag, short& s ) = 0;
    virtual void Serialize( const char* tag, int& i ) = 0;
    virtual void Serialize( const char* tag, u32& ui ) = 0;
    virtual void Serialize( const char* tag, float& i ) = 0;
    virtual void Serialize( const char* tag, double& i ) = 0;
    virtual void Serialize( const char* tag, bool& b ) = 0;
    virtual void Serialize( const char* tag, String& str ) = 0;

    virtual void Serialize( const char* tag, class Vec2d& vec ) = 0;
    virtual void Serialize( const char* tag, class Vec3d& vec ) = 0;
    virtual void Serialize( const char* tag, class ObjectPath& _objPath ) = 0;
    virtual void Serialize( const char* tag, class Color& _color) = 0;
    virtual void Serialize( const char* tag, ObjectId & _id) = 0;
    virtual void Serialize( const char* tag, String8 & _id) = 0;
    virtual void Serialize( const char* tag, ITF_STDSTRING & _name) = 0;
    virtual void Serialize( const char* tag, class Angle& _angle ) = 0;
    virtual void Serialize( const char* tag, class AngleAmount& _angle ) { Serialize(tag, static_cast<Angle&>(_angle)); }
    virtual void Serialize( const char* tag, class Path& _path ) = 0;
    virtual void Serialize( const char* tag, StringID & _id) = 0;
    virtual void Serialize( const char* tag, class Volume& _volume) = 0;
    virtual void Serialize( const char* tag, struct LocalisationId& id ) = 0;

    virtual void SerializeEnumBegin( const char* tag, int& selection ) = 0;
    virtual void SerializeEnumVar( int var_num, const char* var ) = 0;
    virtual void SerializeEnumEnd() = 0;

    template <class T>
    void Serialize( const char* tag, ITF_VECTOR <T*>& list, ESerializeFlags flags );
    template <class T>
    void Serialize( const char* tag, ITF_VECTOR <T>& list, ESerializeFlags flags );
    template <class key,class T>
    void Serialize( const char* tag, ITF_MAP <key,T*>& map, ESerializeFlags flags );
    template <class key,class T>
    void Serialize( const char* tag, ITF_MAP <key,T>& map, ESerializeFlags flags );
    template <class T>
    void Serialize( const char* tag, ITF_VECTOR <T>& list );
    template <class T, u32 _defaultSize>
    void Serialize( const char* tag, SafeArray <T,_defaultSize>& list );
    template <class T, u32 _arraySize>
    void Serialize( const char* tag, FixedArray <T, _arraySize>& list, ESerializeFlags flags );
    template <class T, u32 _defaultSize>
    void Serialize( const char* tag, SafeArray <T,_defaultSize>& list, ESerializeFlags flags );
    template <class T, u32 _defaultSize>
    void Serialize( const char* tag, SafeArray <T*,_defaultSize>& list, ESerializeFlags flags );
    template <class T>
    void Serialize( const char* tag, ITF_VECTOR <T*>& list, ObjectFactory* factory, ESerializeFlags flags );
    template <class key,class T>
    void Serialize( const char* tag, ITF_MAP<key, T*>& list, ObjectFactory* factory, ESerializeFlags flags );
    template <class T, u32 _defaultSize >
    void Serialize( const char* tag, SafeArray <T*,_defaultSize>& list, ObjectFactory* factory, ESerializeFlags flags );
    template <class T>
    void SerializeObject( const char* tag, T*& object, ObjectFactory* factory, ESerializeFlags flags );
    template <class T>
    void SerializeObject( const char* tag, T& object, ESerializeFlags flags );
    template <class T>
    void SerializeObject( const char* tag, T*& object, ESerializeFlags flags );

    void Serialize( const char* tag, ObjectRef& _ref ) { Serialize(tag,_ref.m_directValue); }

    ITF_INLINE bbool meetsConditionFlags( u32 _serializeFlags, u32 _memberFlags ) const;

protected:

    void SerializeFactoryDesc( const char* tag, ObjectFactory* factory, bbool _list );
    void SerializeContainerDesc( const char* tag );

    void SerializeFactoryClassNameStringID( StringID& prop ) { Serialize("NAME",prop); }
    void SerializeFactoryClassNameString( String8& prop ) { Serialize("NAME",prop); }

    virtual bbool ReadArrayCount( const char* tag, uSize& count ) = 0;
    virtual bbool WriteArrayCount( const char* tag, uSize count ) = 0;

    bbool                m_read;
    class ObjectFactory* m_factory;

#ifdef ITF_SUPPORT_EDITOR
    // data for non saving default values
    bbool                       m_saveOnlyDiff;
    SafeArray<ObjectMemInfo>    m_objectInfoStack;
#endif // ITF_SUPPORT_EDITOR

    u32                         m_properties;

#if defined(SUPPORT_STATS_MANAGER) && defined(ITF_WII)
    struct ObjectMemStats
    {
        u32 m_memusage;
        const char *m_name;
    };
    SafeArray<ObjectMemStats>    m_objectMemStats;
#endif // SUPPORT_STATS_MANAGER
};


template <class T>
inline bbool CSerializerObject::OpenObjectGroup( const char* tag, T *_object, uSize index)
{
#if defined(SUPPORT_STATS_MANAGER) && defined(ITF_WII)
    if (m_read)
    {
        ObjectMemStats m;
        m.m_memusage = MEM_M_AllocSize();
        m.m_name = tag;

        m_objectMemStats.push_back(m);
    }
#endif

#ifdef ITF_SUPPORT_EDITOR
    if (!m_read && m_saveOnlyDiff)
    {
        ObjectMemInfo curObjectInfo;
        curObjectInfo.m_objMem    = reinterpret_cast<uPtr>(_object);
        curObjectInfo.m_size      = sizeof(T);
        curObjectInfo.m_defValues = reinterpret_cast<char*>(new T());

        m_objectInfoStack.push_back(curObjectInfo);
    }
#endif // #ifdef ITF_SUPPORT_EDITOR

    return OpenGroup( tag, index);
}

template <class T>
void CSerializerObject::CloseObjectGroup()
{
#ifdef ITF_SUPPORT_EDITOR
    if (m_objectInfoStack.size() > 0)
    {
        ObjectMemInfo & objInfo = m_objectInfoStack[m_objectInfoStack.size()-1];

        T * ptr = (T *)objInfo.m_defValues;
        SF_DEL(ptr);
        objInfo.m_objMem = NULL;

        m_objectInfoStack.pop_back();
    }
#endif // #ifdef ITF_SUPPORT_EDITOR

    CloseGroup();

#if defined(SUPPORT_STATS_MANAGER) && defined(ITF_WII)
    if (m_objectMemStats.size())
    {
        ObjectMemStats & m = m_objectMemStats[m_objectMemStats.size()-1];

        u32 memusage = MEM_M_AllocSize();
        u32 diff = (memusage > m.m_memusage)? memusage - m.m_memusage : 0;
        StatsManager::getptr()->memObjectInc(m.m_name, diff, sizeof(T));
        m_objectMemStats.pop_back();
    }
#endif
}


template <class T>
inline bbool CSerializerObject::SkipDefaultValue( T& _value)
{
#ifdef ITF_SUPPORT_EDITOR
    if (m_objectInfoStack.size() == 0)
        return bfalse;

    ObjectMemInfo & objInfo = m_objectInfoStack[m_objectInfoStack.size()-1];
    uPtr         memValue = reinterpret_cast<uPtr>(&_value);

    if (memValue < objInfo.m_objMem || memValue >= objInfo.m_objMem + objInfo.m_size)
    {
        return bfalse;
    }
    uPtr            memDir      = memValue - objInfo.m_objMem;
    const char*     valueAddr   = objInfo.m_defValues + memDir;

    const T* defValue = reinterpret_cast<const T*>(valueAddr);
    return *defValue == _value;
#else
    return bfalse;
#endif
}


template <class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_VECTOR <T*>& list, ESerializeFlags flags )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;
        uSize prevSize = uSize(list.size());

        if (ReadArrayCount(tag, num))
        {
            for (uSize listIndex = num; listIndex < uSize(list.size()); listIndex++ )
            {
                SF_DEL(list[listIndex]);
            }

            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag,i) )
                {
                    T* object;
                    
                    if ( i >= prevSize )
                    {
                        object = newAlloc(mId_Serialization,T);
                        list[i] = object;
                    }
                    else
                    {
                        object = list[i];
                    }

                    object->Serialize(this,flags);
                    
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++ )
        {
            if ( OpenGroup(tag,i) )
            {
                list[i]->Serialize(this, flags);
                CloseGroup();
            }
        }
    }
}

template <class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_VECTOR <T>& list, ESerializeFlags flags )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if (OpenGroup(tag,i))
                {
                    T& object = list[i];
                    object.Serialize(this,flags);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if (OpenGroup(tag, i))
            {
                list[i].Serialize(this, flags);
                CloseGroup();
            }
        }
    }
}

template <class T, u32 _arraySize >
inline void CSerializerObject::Serialize(const char* tag, FixedArray <T, _arraySize>& list, ESerializeFlags flags)
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            list.resize(num);

            for ( uSize i = 0; i < num; i++ )
            {
                if ( OpenGroup(tag,i) )
                {
                    T& object = list[i];
                    object.Serialize(this, flags);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for ( uSize i = 0; i < num; i++ )
        {
            if ( OpenGroup(tag,i) )
            {
                list[i].Serialize(this, flags);
                CloseGroup();
            }
        }
    }
}

template <class T, u32 _defaultSize >
inline void CSerializerObject::Serialize( const char* tag, SafeArray <T,_defaultSize>& list, ESerializeFlags flags )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag,i) )
                {
                    T& object = list[i];
                    object.Serialize(this,flags);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if ( OpenGroup(tag,i) )
            {
                list[i].Serialize(this,flags);
                CloseGroup();
            }
        }
    }
}
template <class T, u32 _defaultSize>
inline void CSerializerObject::Serialize( const char* tag, SafeArray <T*,_defaultSize>& list, ESerializeFlags flags )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;
        uSize prevSize = list.size();

        if (ReadArrayCount(tag, num))
        {
            for (uSize listIndex = num; listIndex < list.size(); listIndex++ )
            {
                SF_DEL(list[listIndex]);
            }

            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag,i) )
                {
                    T* object;

                    if ( i >= prevSize )
                    {
                        object = newAlloc(mId_Serialization,T);
                        list[i] = object;
                    }
                    else
                    {
                        object = list[i];
                    }

                    object->Serialize(this,flags);

                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if ( OpenGroup(tag, i) )
            {
                list[i]->Serialize(this, flags);
                CloseGroup();
            }
        }
    }
}

template <class key,class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_MAP <key,T*>& map, ESerializeFlags flags )
{
    using base_container_map = ITF_MAP<key,T*>;
    using const_iterator = typename base_container_map::const_iterator ;

    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;
        
        if (ReadArrayCount(tag, num))
        {
            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag, i) )
                {
                    key k;
                    Serialize("key", k);
                    T* new_object = newAlloc(mId_Serialization, T);
                    new_object->Serialize(this, flags);
                    map[k] = new_object;
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = map.size();
        uSize i = 0;

        WriteArrayCount(tag, num);

        for (const_iterator it = map.begin(); it != map.end(); ++it, i++ )
        {
            if ( OpenGroup(tag, i) )
            {
                key k = it->first;
                Serialize("key", k);
                it->second->Serialize(this,flags);
                CloseGroup();
            }
        }
    }
}

template <class key,class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_MAP <key,T>& map, ESerializeFlags flags )
{
  /*  if (flags & ESerialize_Clone_Load)
    {
        ITF_ASSERT(flags & ESerialize_Template_Load);
    }
    if (flags & ESerialize_Clone_Save)
    {
        ITF_ASSERT(flags & ESerialize_Template_Save);
    }*/
    using base_container_map = ITF_MAP<key,T>;
    using iterator = typename base_container_map::iterator;

    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;
        
        if (ReadArrayCount(tag, num))
        {
            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag,i) )
                {
                    key k;
                    Serialize("key",k);
                    T& object = map[k];
                    object.Serialize(this,flags);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = map.size();
        int i = 0;

        WriteArrayCount(tag,num);

        for (iterator it = map.begin(); it != map.end(); ++it, i++ )
        {
            if ( OpenGroup(tag,i) )
            {
                key k = it->first;
                Serialize("key", k);
                it->second.Serialize(this,flags);
                CloseGroup();
            }
        }
    }
}

template <class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_VECTOR <T>& list )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;
        
        if (ReadArrayCount(tag, num))
        {
            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag, i) )
                {
                    Serialize("VAL", list[i]);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if ( OpenGroup(tag, i) )
            {
                Serialize("VAL", list[i]);
                CloseGroup();
            }
        }
    }
}

template <class T, u32 _defaultSize>
inline void CSerializerObject::Serialize( const char* tag, SafeArray <T,_defaultSize>& list )
{
    SerializeContainerDesc(tag);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            list.resize(num);

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag, i) )
                {
                    Serialize("VAL", list[i]);
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if ( OpenGroup(tag, i) )
            {
                Serialize("VAL", list[i]);
                CloseGroup();
            }
        }
    }
}

template <class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_VECTOR<T*>& list, ObjectFactory* factory, ESerializeFlags flags )
{
    SerializeFactoryDesc(tag,factory,btrue);

    if ( m_read )
    {
        uSize num;
        
        if (ReadArrayCount(tag, num))
        {
            if ( !factory )
            {
                factory = m_factory;
            }

            for (uSize listIndex = num; listIndex < list.size(); listIndex++ )
            {
                SF_DEL(list[listIndex]);
            }

            uSize prevSize = list.size();

            list.resize(num);

            uSize i = 0;
            uSize index = 0;

            while ( i < num )
            {
                if ( OpenGroup(tag,index++) )
                {
                    StringID objId;

#if !defined(ITF_CONSOLE)
                    if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                    {
                        String8 strObject;
                        SerializeFactoryClassNameString(strObject);
                        objId = strObject.cStr();
                    }
                    else
#endif
                    {
                        SerializeFactoryClassNameStringID(objId);
                    }
                    

                    T* object = i < prevSize ? list[i] : NULL;

                    if ( !object || object->GetObjectClassCRC() == objId )
                    {
                        SF_DEL(object);

                        if ( objId.isValid() )
                        {
                            object = factory->CreateObject<T>(objId);
                        }
                    }

                    if ( object )
                    {
                        object->Serialize(this,flags);
                        list[i++] = object;
                    }
                    else
                    {
                        num--;
                        list.resize(num);
                    }
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++ )
        {
            if ( OpenGroup(tag, i) )
            {
                T* object = list[i];

#if defined(ITF_ENABLE_RTTI_CRC_CHECK)
                if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                {
                    String8 strObject = object->GetObjectClassName();
                    SerializeFactoryClassNameString(strObject);
                }
                else
#endif
                {
#if !defined(ITF_ENABLE_RTTI_CRC_CHECK)
                    ITF_ASSERT_CRASH((getProperties()&ESerializerProperty_Binary) != 0,"If ITF_ENABLE_RTTI_CRC_CHECK is disabled we should not serialize on a parser");
#endif
                    StringID objId = object->GetObjectClassCRC();
                    SerializeFactoryClassNameStringID(objId);
                }

                object->Serialize(this, flags);
                CloseGroup();
            }
        }
    }
}

template <class key,class T>
inline void CSerializerObject::Serialize( const char* tag, ITF_MAP<key,T*>& map, ObjectFactory* factory, ESerializeFlags flags )
{
    using base_container_map = ITF_MAP<key,T*>;
    using const_iterator = typename base_container_map::const_iterator;

    SerializeFactoryDesc(tag,factory,btrue);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            if ( !factory )
            {
                factory = m_factory;
            }

            //TODO : delete existing items

            for (uSize i = 0; i < num; i++)
            {
                if ( OpenGroup(tag, i) )
                {
                    key k;
                    Serialize("key",k);
                    StringID objId;

#if !defined(ITF_CONSOLE)
                    if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                    {
                        String8 strObject;
                        SerializeFactoryClassNameString(strObject);
                        objId = strObject.cStr();
                    }
                    else
#endif
                    {
                        SerializeFactoryClassNameStringID(objId);
                    }

                    //TODO : handle existing objects in map

                    if (objId.isValid())
                    {
                        T* new_object = factory->CreateObject<T>(objId);

                        map[k] = new_object;

                        if ( new_object )
                        {
                            new_object->Serialize(this,flags);
                        }
                    }
                    else
                    {
                        map[k] = NULL;
                    }
                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = map.size();

        WriteArrayCount(tag, num);

        int i = 0;

        for (const_iterator it = map.begin(); it != map.end(); ++it, i++ )
        {
            if ( OpenGroup(tag,i) )
            {
                key k = it->first;
                Serialize("key", k);
                T *object = it->second;
                if (object)
                {
#if defined(ITF_ENABLE_RTTI_CRC_CHECK)
                    if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                    {
                        String8 strObject = object->GetObjectClassName();
                        SerializeFactoryClassNameString(strObject);
                    }
                    else
#endif
                    {
#if !defined(ITF_ENABLE_RTTI_CRC_CHECK)
                        ITF_ASSERT_CRASH((getProperties()&ESerializerProperty_Binary) != 0,"If ITF_ENABLE_RTTI_CRC_CHECK is disabled we should not serialize on a parser");
#endif
                        StringID objId = object->GetObjectClassCRC();
                        SerializeFactoryClassNameStringID(objId);
                    }

                    object->Serialize(this,flags);
                }
                else
                {
#if !defined(ITF_CONSOLE)
                    if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                    {
                        String8 emptyClassName;
                        SerializeFactoryClassNameString(emptyClassName);
                    }
                    else
#endif
                    {
                        StringID emptyObjId;
                        SerializeFactoryClassNameStringID(emptyObjId);
                    }
                }
                
                CloseGroup();
            }
        }

    }
}

template <class T, u32 _defaultSize>
inline void CSerializerObject::Serialize( const char* tag, SafeArray <T*,_defaultSize>& list, ObjectFactory* factory, ESerializeFlags flags )
{
    SerializeFactoryDesc(tag,factory,btrue);

    if ( m_read )
    {
        uSize num;

        if (ReadArrayCount(tag, num))
        {
            if ( !factory )
            {
                factory = m_factory;
            }

            for (uSize listIndex = num; listIndex < list.size(); listIndex++ )
            {
                SF_DEL(list[listIndex]);
            }

            uSize prevSize = list.size();

            list.resize(num);

            uSize i = 0;
            uSize index = 0;

            while ( i < num )
            {
                if ( OpenGroup(tag,index++) )
                {
                    StringID objId;

#if !defined(ITF_CONSOLE)
                    if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                    {
                        String8 strObject;
                        SerializeFactoryClassNameString(strObject);
                        objId = strObject.cStr();
                    }
                    else
#endif
                    {
                        SerializeFactoryClassNameStringID(objId);
                    }

                    T* object = i < prevSize ? list[i] : NULL;

                    if ( !object || object->GetObjectClassCRC() != objId )
                    {
                        SF_DEL(object);

                        if ( objId.isValid() )
                        {
                            object = factory->CreateObject<T>(objId);
                        }
                    }

                    if ( object )
                    {
                        object->Serialize(this,flags);
                        list[i++] = object;
                    }
                    else
                    {
                        num--;
                        list.resize(num);
                    }

                    CloseGroup();
                }
            }
        }
    }
    else
    {
        uSize num = list.size();

        WriteArrayCount(tag, num);

        for (uSize i = 0; i < num; i++)
        {
            if ( OpenGroup(tag, i) )
            {
                T* object = list[i];

#if defined(ITF_ENABLE_RTTI_CRC_CHECK)
                if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                {
                    String8 strObject = object->GetObjectClassName();
                    SerializeFactoryClassNameString(strObject);
                }
                else
#endif
                {
#if !defined(ITF_ENABLE_RTTI_CRC_CHECK)
                    ITF_ASSERT_CRASH((getProperties()&ESerializerProperty_Binary) != 0,"If ITF_ENABLE_RTTI_CRC_CHECK is disabled we should not serialize on a parser");
#endif
                    StringID objId = object->GetObjectClassCRC();
                    SerializeFactoryClassNameStringID(objId);
                }

                object->Serialize(this,flags);
                CloseGroup();
            }
        }
    }
}

template <class T>
inline void CSerializerObject::SerializeObject( const char* tag, T*& object, ObjectFactory* factory, ESerializeFlags flags )
{
    SerializeFactoryDesc(tag,factory,bfalse);

    if ( m_read )
    {
        if ( OpenGroup(tag) )
        {
            StringID objId;

#if !defined(ITF_CONSOLE)
            if ( (getProperties()&ESerializerProperty_Binary) == 0 )
            {
                String8 strObject;
                SerializeFactoryClassNameString(strObject);
                objId = strObject.cStr();
            }
            else
#endif
            {
                SerializeFactoryClassNameStringID(objId);
            }

            if ( objId.isValid() )
            {
                if ( !factory )
                {
                    factory = m_factory;
                }

                if ( !object || objId != object->GetObjectClassCRC() )
                {
                    SF_DEL(object);
                    object = factory->CreateObject<T>(objId);
                }

                if ( object )
                {
                    object->Serialize(this,flags);
                }
            }
            else
            {
                SF_DEL(object);
            }

            CloseGroup();
        }
    }
    else
    {
        if ( OpenGroup(tag) )
        {
            if ( object )
            {
#if defined(ITF_ENABLE_RTTI_CRC_CHECK)
                if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                {
                    String8 strObject = object->GetObjectClassName();
                    SerializeFactoryClassNameString(strObject);
                }
                else
#endif
                {
#if !defined(ITF_ENABLE_RTTI_CRC_CHECK)
                    ITF_ASSERT_CRASH((getProperties()&ESerializerProperty_Binary) != 0,"If ITF_ENABLE_RTTI_CRC_CHECK is disabled we should not serialize on a parser");
#endif
                    StringID objId = object->GetObjectClassCRC();
                    SerializeFactoryClassNameStringID(objId);
                }

                object->Serialize(this,flags);
            }
            else
            {
#if !defined(ITF_CONSOLE)
                if ( (getProperties()&ESerializerProperty_Binary) == 0 )
                {
                    String8 emptyObject;
                    SerializeFactoryClassNameString(emptyObject);
                }
                else
#endif
                {
                    StringID emptyId;
                    SerializeFactoryClassNameStringID(emptyId);
                }
            }

            CloseGroup();
        }
    }
}

template <class T>
inline void CSerializerObject::SerializeObject( const char* tag, T& object, ESerializeFlags flags )
{
    if ( OpenGroup(tag) )
    {
        object.Serialize(this,flags);
        CloseGroup();
    }
}

template <class T>
inline void CSerializerObject::SerializeObject( const char* tag, T*& object, ESerializeFlags flags )
{
    if ( m_read )
    {
        bbool needRead = bfalse;

        if ( (getProperties()&ESerializerProperty_Binary) != 0 )
        {
            Serialize("read",needRead);
        }
        else
        {
            needRead = OpenGroup(tag);
        }

        if ( needRead )
        {
            if ( !object )
            {
                object = newAlloc(mId_Serialization, T);
            }

            object->Serialize(this,flags);
            CloseGroup();
        }
        else
        {
            if ( object )
            {
                SF_DEL(object);
            }
        }
    }
    else
    {
        bbool needWrite = bfalse;

        if ( object )
        {
            needWrite = btrue;

            if ( (getProperties()&ESerializerProperty_Binary) != 0 )
            {
                Serialize("read",needWrite);
            }

            if ( OpenGroup(tag) )
            {
                object->Serialize(this,flags);
                CloseGroup();
            }
        }
        else
        {
            needWrite = bfalse;

            if ( (getProperties()&ESerializerProperty_Binary) != 0 )
            {
                Serialize("read",needWrite);
            }
        }
    }
}

bbool CSerializerObject::meetsConditionFlags( u32 _serializeFlags, u32 _memberFlags ) const
{
    if ( ESerialize_Deprecate == _memberFlags )
    {
        // If the value is deprecated we ignore:
        //      * in binary format
        //      * when not loading the data value
        if ( (getProperties()&ESerializerProperty_Binary) != 0 )
        {
            return bfalse;
        }
        else
        {
            return (_serializeFlags&ESerialize_Data_Load);
        }
    }
    else if(ESerialize_DataRaw == _memberFlags)
    {
        // We ignore binary format
        if ( (getProperties()&ESerializerProperty_Binary) != 0 )
        {
            return bfalse;
        }
        else
        {
            return (_serializeFlags&ESerialize_Data);
        }
    }
    else if(ESerialize_DataBin == _memberFlags)
    {
        // We ignore non-binary format
        if ( (getProperties()&ESerializerProperty_Binary) != 0 )
        {
            return (_serializeFlags&ESerialize_Data);
        }
        else
        {
            return bfalse;
        }
    }

    return (_serializeFlags&_memberFlags) != 0;
}

#define DECLARE_SERIALIZE_INTERFACE()   \
    public: \
    virtual void Serialize( CSerializerObject* serializer, ESerializeFlags flags ) = 0; \
    virtual void SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) = 0;

#define DECLARE_SERIALIZE() \
    public: \
    void Serialize( CSerializerObject* serializer, ESerializeFlags flags ); \
    void SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags );

#define DECLARE_SERIALIZE_VIRTUAL() \
    public: \
    virtual void Serialize( CSerializerObject* serializer, ESerializeFlags flags ); \
    virtual void SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags );

#ifdef ITF_CONSOLE
#define SERIALIZE_OBJECT_NAME(object)   NULL
#else
#define SERIALIZE_OBJECT_NAME(object)   #object
#endif

#define BEGIN_SERIALIZATION(object) \
    void object::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(object), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object>();  \
        } \
    }   \
    void object::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    { 

#define BEGIN_SERIALIZATION_TEMPLATE(object) \
    template <class T> \
    void object<T>::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(object), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object>();  \
        } \
    }   \
    template <class T> \
    void object<T>::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {

#define BEGIN_SERIALIZATION_TEMPLATE_ABSTRACT(object) \
    template <class T> \
    void object<T>::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        ITF_ASSERT(0); \
    } \
    template <class T> \
    void object<T>::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {

#define BEGIN_SERIALIZATION_ABSTRACT(object) \
    void object::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        ITF_ASSERT(0); \
    }   \
    void object::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {

#define BEGIN_SERIALIZATION_SUBCLASS(object,subclass) \
    void object::subclass::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(subclass), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object::subclass>();  \
        } \
    }   \
    void object::subclass::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {

#ifdef ITF_WII
#define BEGIN_SERIALIZATION_SUBCLASS_TEMPLATE(object,subclass) \
    template <class T> \
    void object<T>::subclass::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(subclass), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<typename object <class T>::subclass>();  \
        } \
    }   \
    template <class T> \
    void object<T>::subclass::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {

#else
#define BEGIN_SERIALIZATION_SUBCLASS_TEMPLATE(object,subclass) \
    template <class T> \
    void object<T>::subclass::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(subclass), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<typename object<T>::subclass>();  \
        } \
    }   \
    template <class T> \
    void object<T>::subclass::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {
#endif

#define BEGIN_SERIALIZATION_CHILD(object) \
    void object::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(object), this) ) \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object>();  \
        } \
    } \
    void object::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        Super::SerializeImpl(serializer,flags);

#define BEGIN_SERIALIZATION_CHILD_TEMPLATE(object) \
    template <class T> \
    void object<T>::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(object), this) ) \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object>();  \
        } \
    } \
    template <class T> \
    void object<T>::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        Super::SerializeImpl(serializer,flags);

#define BEGIN_SERIALIZATION_ABSTRACT_CHILD_TEMPLATE(object) \
    template <class T> \
    void object<T>::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        ITF_ASSERT(0); \
    } \
    template <class T> \
    void object<T>::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        Super::SerializeImpl(serializer,flags);

#define BEGIN_SERIALIZATION_ABSTRACT_CHILD(object) \
    void object::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        ITF_ASSERT(0); \
    } \
    void object::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        Super::SerializeImpl(serializer,flags);


#define BEGIN_SERIALIZATION_CHILD_SUBCLASS(object,subclass) \
    void object::subclass::Serialize( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        if ( serializer->OpenObjectGroup(SERIALIZE_OBJECT_NAME(subclass), this) )  \
        { \
            SerializeImpl(serializer,flags); \
            serializer->CloseObjectGroup<object::subclass>();  \
        } \
    } \
    void object::subclass::SerializeImpl( CSerializerObject* serializer, ESerializeFlags flags ) \
    {   \
        Super::SerializeImpl(serializer,flags);

#define END_SERIALIZATION() \
    }

#ifdef ITF_CONSOLE

#define SERIALIZE_MEMBER(name,member) \
    serializer->Serialize(NULL,member)

#define SERIALIZE_CONTAINER_WITH_FACTORY(name,container,factory) \
    serializer->Serialize(NULL,container,factory,flags)

#define SERIALIZE_CONTAINER_OBJECT(name,container) \
    serializer->Serialize(NULL,container,flags)

#define SERIALIZE_CONTAINER(name,container) \
    serializer->Serialize(NULL,container)

#define SERIALIZE_OBJECT(name,object) \
    serializer->SerializeObject(NULL,object,flags)

#define SERIALIZE_OBJECT_WITH_FACTORY(name,object,factory) \
    serializer->SerializeObject(NULL,object,factory,flags)

#define SERIALIZE_ENUM_BEGIN(name,val) \
    serializer->SerializeEnumBegin(NULL,reinterpret_cast<int&>(val))

#else

#define SERIALIZE_MEMBER(name,member) \
    serializer->Serialize(name,member)

#define SERIALIZE_CONTAINER_WITH_FACTORY(name,container,factory) \
    serializer->Serialize(name,container,factory,flags)

#define SERIALIZE_CONTAINER_OBJECT(name,container) \
    serializer->Serialize(name,container,flags)

#define SERIALIZE_CONTAINER(name,container) \
    serializer->Serialize(name,container)

#define SERIALIZE_OBJECT(name,object) \
    serializer->SerializeObject(name,object,flags)

#define SERIALIZE_OBJECT_WITH_FACTORY(name,object,factory) \
    serializer->SerializeObject(name,object,factory,flags)

#define SERIALIZE_ENUM_BEGIN(name,val) \
    serializer->SerializeEnumBegin(name,reinterpret_cast<int&>(val))

#endif //ITF_CONSOLE
#define SERIALIZE_FUNCTION(function,member_flags) \
    if ( (flags&(member_flags)) != 0 && (serializer->getProperties()&ESerializerProperty_MemoryDump) == 0 ) \
        function()

#define SERIALIZE_FUNCTION_PARAMETER(function,paramater,member_flags) \
    if ( (flags&(member_flags)) != 0 && (serializer->getProperties()&ESerializerProperty_MemoryDump) == 0 ) \
        function(paramater)

#define SERIALIZE_ENUM_VAR( var ) \
    if ( (serializer->getProperties()&ESerializerProperty_Tool) ) \
        serializer->SerializeEnumVar( var, SERIALIZE_OBJECT_NAME(var) )

#define SERIALIZE_ENUM_END() \
    serializer->SerializeEnumEnd()

#define BEGIN_CONDITION_BLOCK(member_flags) \
    if ( serializer->meetsConditionFlags(flags,member_flags) ) \
    {

#define END_CONDITION_BLOCK() \
    }

} // namespace ITF

#endif // SERIALIZEROBJECT_H


