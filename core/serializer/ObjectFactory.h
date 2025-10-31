#ifndef _ITF_OBJECTFACTORY_H_
#define _ITF_OBJECTFACTORY_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#include "core/memory/memoryId.h" // for usage of mId_Gameplay

#if !defined(ITF_FINAL) && !defined(ITF_CONSOLE)
#define ITF_ENABLE_RTTI_CRC_CHECK
#endif

namespace ITF
{
    class IRTTIObject
    {
    public:
        virtual ~IRTTIObject() {}

        static StringID::StringIdValueType GetClassCRCStatic() { return 0; }
        static bool IsClassCRCStatic( StringID::StringIdValueType _crc ) { return false; }
        virtual bool IsClassCRC( StringID::StringIdValueType _crc ) const { return false; }
        virtual StringID::StringIdValueType GetObjectClassCRC() const = 0;

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
        static const char* GetClassNameStatic() {return NULL;}
        virtual const char* GetObjectClassName()const = 0;
#endif

        virtual IRTTIObject* CreateNewObject() const = 0;
        
        template <class T>
        const T* DynamicCast( StringID::StringIdValueType _classCRC ) const
        {
#ifdef ITF_ENABLE_RTTI_CRC_CHECK
            StringID className(T::GetClassNameStatic());
            ITF_ASSERT(className.GetValue()==_classCRC);
#endif
            if ( IsClassCRC(_classCRC) )
            {
                return static_cast<const T*>(this);
            }

            return NULL;
        }

        template <class T>
        T* DynamicCast( StringID::StringIdValueType _classCRC )
        {
#ifdef ITF_ENABLE_RTTI_CRC_CHECK
            StringID className(T::GetClassNameStatic());
            ITF_ASSERT(className.GetValue()==_classCRC);
#endif
            if ( IsClassCRC(_classCRC) )
            {
                return static_cast<T*>(this);
            }

            return NULL;
        }

        template <class T>
        static T* SafeDynamicCast( IRTTIObject *_object, StringID::StringIdValueType _classCRC )
        {
#ifdef ITF_ENABLE_RTTI_CRC_CHECK
            StringID className(T::GetClassNameStatic());
            ITF_ASSERT(className.GetValue()==_classCRC);
#endif
            if (_object)
            {
                if ( _object->IsClassCRC(_classCRC) )
                {
                    return static_cast<T*>(_object);
                }
            }
            return NULL;
        }

        template <class T>
        static const T* SafeDynamicCast(const IRTTIObject *_object, StringID::StringIdValueType _classCRC )
        {
            return (const T*)SafeDynamicCast<T>((IRTTIObject*) _object,_classCRC);
        }

    };

    class ObjectFactory
    {
    public:

        ObjectFactory() {}
        ~ObjectFactory() {}

        typedef IRTTIObject* (*ptrCreateObject)(void);
        typedef bool (*ptrIsClass)( StringID::StringIdValueType _crc );
        struct ClassInfo
        {
            ptrCreateObject                 CreateObject;
            ptrIsClass                      IsClass;
            StringID::StringIdValueType     m_crc;
        };

        typedef ITF_MAP<StringID, ClassInfo> ObjectContainer;
        typedef ITF_VECTOR<StringID> ObjectsNamesContainer;

        template <class C>
        void RegisterObject( StringID::StringIdValueType _classCRC )
        {
#ifdef ITF_ENABLE_RTTI_CRC_CHECK
            StringID idObj = C::GetClassNameStatic();
#ifdef ITF_DEBUG_STRINGID
            ITF_ASSERT_MSG(std::find(m_names.begin(), m_names.end(), idObj) == m_names.end(), "Already registered");
#endif
            ITF_ASSERT(idObj.GetValue()==_classCRC);
#endif

            ClassInfo info;
            info.CreateObject = C::CreateNewObjectStatic;
            info.m_crc = _classCRC;
            info.IsClass = C::IsClassCRCStatic;
            m_objectsRegistered[_classCRC] = info;

#ifdef ITF_DEBUG_STRINGID
            m_names.push_back(StringID(_classCRC));
#endif
        }

        template <class C>
        C* CreateObject( const char* _className ) const
        {
            StringID objectName = _className;
            return CreateObject<C>(objectName.GetValue());
        }

        template <class C>
        C* CreateObject( StringID::StringIdValueType _classCRC ) const
        {
            ObjectContainer::const_iterator it = m_objectsRegistered.find(_classCRC);

            if ( it != m_objectsRegistered.end() )
            {
                const ClassInfo& classInfo = it->second;

                ITF_ASSERT_CRASH(classInfo.IsClass(_classCRC) && classInfo.IsClass(C::GetClassCRCStatic()), "the type provided does not match the destination type");

                C* retObj = static_cast<C*>( classInfo.CreateObject() );
                ITF_ASSERT(retObj->GetObjectClassCRC()==_classCRC);
                return retObj;
            }

            ITF_ASSERT_MSG(0, StringConverter(String("The factory doesn't contain type : ") + StringID(_classCRC).getDebugString()).getChar());
            return NULL;
        }

        const ClassInfo* GetClassInfo( StringID::StringIdValueType _classCrc ) const
        {
            ObjectContainer::const_iterator it = m_objectsRegistered.find(_classCrc);

            if ( it != m_objectsRegistered.end() )
            {
                return &it->second;
            }

            return NULL;
        }

#ifdef ITF_DEBUG_STRINGID
        const ObjectsNamesContainer& GetObjectsNames() const { return m_names;}
#endif

        const ObjectContainer& GetObjectContainer() const {return m_objectsRegistered;}

    protected:

        ObjectContainer         m_objectsRegistered;

#ifdef ITF_DEBUG_STRINGID
        ObjectsNamesContainer   m_names;
#endif
    };


} // namespace ITF

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define IMPLEMENT_OBJECT_RTTI(classname) \
    const char* classname::GetClassNameStatic() \
    { \
    	static const char STR[] = #classname; \
        return STR; \
    }

#define IMPLEMENT_OBJECT_RTTI_SUBCLASS(classname,subclassname) \
    const char* classname::subclassname::GetClassNameStatic() \
    { \
        static const char STR[] = #subclassname; \
        return STR; \
    }

#define IMPLEMENT_OBJECT_RTTI_TEMPLATE(classname) \
    const char* classname##_GetClassNameStatic() \
    { \
    	static const char STR[] = #classname; \
        return STR; \
    }
#else // ITF_ENABLE_RTTI_CRC_CHECK
    #define IMPLEMENT_OBJECT_RTTI(classname)
    #define IMPLEMENT_OBJECT_RTTI_TEMPLATE(classname)
    #define IMPLEMENT_OBJECT_RTTI_SUBCLASS(classname,subclassname)
#endif

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CLASSNAME() \
    virtual const char* GetObjectClassName() const { return GetClassNameStatic(); }
#else // ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CLASSNAME()
#endif // ITF_ENABLE_RTTI_CRC_CHECK

#define DECLARE_RTTI_COMMON(parent,classCRC) \
    public: \
    typedef parent Super; \
    DECLARE_RTTI_COMMON_CLASSNAME() \
    static bool IsClassCRCStatic( StringID::StringIdValueType _crc ) \
    { \
        if ( classCRC == _crc ) \
        { \
            return true; \
        } \
        else \
        { \
            return parent::IsClassCRCStatic(_crc); \
        } \
    } \
    virtual StringID::StringIdValueType GetObjectClassCRC() const { return classCRC; } \
    virtual bool IsClassCRC( StringID::StringIdValueType _crc ) const \
    { \
        if ( classCRC == _crc ) \
        { \
            return true; \
        } \
        else \
        { \
            return Super::IsClassCRC(_crc); \
        } \
    }

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_CLASSNAME() static const char* GetClassNameStatic();
#else // ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_CLASSNAME()
#endif // ITF_ENABLE_RTTI_CRC_CHECK

#define DECLARE_OBJECT_CHILD_RTTI(className,parent,classCrc) \
    public: \
    DECLARE_RTTI_COMMON(parent,classCrc) \
    DECLARE_RTTI_COMMON_CHILD_CLASSNAME() \
    static StringID::StringIdValueType GetClassCRCStatic() { return classCrc; } \
    static IRTTIObject*  CreateNewObjectStatic() { return newAlloc(mId_Serialization,className); } \
    virtual IRTTIObject* CreateNewObject() const { return CreateNewObjectStatic(); }

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_TEMPLATE_CLASSNAME(className) static const char* GetClassNameStatic() { return className##_GetClassNameStatic(); }
#else // ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_TEMPLATE_CLASSNAME(className)
#endif // ITF_ENABLE_RTTI_CRC_CHECK

#define DECLARE_OBJECT_CHILD_TEMPLATE_RTTI(className,parent,classCrc) \
    public: \
    DECLARE_RTTI_COMMON(parent,classCrc) \
    DECLARE_RTTI_COMMON_CHILD_TEMPLATE_CLASSNAME(className) \
    static StringID::StringIdValueType GetClassCRCStatic() { return classCrc; } \
    static IRTTIObject*  CreateNewObjectStatic() { return newAlloc(mId_Serialization,className); } \
    virtual IRTTIObject* CreateNewObject() const { return CreateNewObjectStatic(); }

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_ABSTRACT_CLASSNAME() static const char* GetClassNameStatic();
#else // ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_ABSTRACT_CLASSNAME()
#endif // ITF_ENABLE_RTTI_CRC_CHECK

// Same as DECLARE_OBJECT_CHILD_RTTI, but no factory is allowed
#define DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(className,parent,classCrc) \
    public: \
    DECLARE_RTTI_COMMON(parent,classCrc) \
    DECLARE_RTTI_COMMON_CHILD_ABSTRACT_CLASSNAME() \
    static StringID::StringIdValueType GetClassCRCStatic() { return classCrc; }

#ifdef ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_TEMPLATE_ABSTRACT_CLASSNAME(className) static const char* GetClassNameStatic() { return className##_GetClassNameStatic(); }
#else // ITF_ENABLE_RTTI_CRC_CHECK
#define DECLARE_RTTI_COMMON_CHILD_TEMPLATE_ABSTRACT_CLASSNAME(className)
#endif // ITF_ENABLE_RTTI_CRC_CHECK

// Same as DECLARE_OBJECT_CHILD_RTTI, but no factory is allowed
#define DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(className,parent,classCrc) \
    public: \
    DECLARE_RTTI_COMMON(parent,classCrc) \
    DECLARE_RTTI_COMMON_CHILD_TEMPLATE_ABSTRACT_CLASSNAME(className) \
    static StringID::StringIdValueType GetClassCRCStatic() { return classCrc; }

#define DECLARE_VALIDATE_OBJECT() virtual void validate(bbool &_isComponentValidated);
#define BEGIN_VALIDATE_OBJECT(_classname_) void _classname_::validate(bbool &_isComponentValidated) {
#define BEGIN_VALIDATE_OBJECT_CHILD(_classname_) void _classname_::validate(bbool &_isComponentValidated) { Super::validate(_isComponentValidated);
#define END_VALIDATE_OBJECT() }

#endif // _ITF_OBJECTFACTORY_H_

