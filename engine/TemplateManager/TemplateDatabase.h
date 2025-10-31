
#ifndef ITF_TEMPLATEDATABASE_H
#define ITF_TEMPLATEDATABASE_H

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef SERIALIZEROBJECTBINARYFILE_H
#include "core/serializer/ZSerializerObjectBinaryFile.h"
#endif //SERIALIZEROBJECTBINARYFILE_H

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h" // For UpdateType
#endif // _ITF_PICKABLE_H_

namespace ITF
{

class TemplateObjBase : public IRTTIObject
{
    DECLARE_SERIALIZE_INTERFACE()
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TemplateObjBase,IRTTIObject,3395210690)

};

class TemplateObj : public TemplateObjBase
{
    friend class TemplateDatabase;

    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TemplateObj,TemplateObjBase,3594560684)

public:

    TemplateObj(): m_prefetched( true ) {}
    TemplateObj( const Path& _file ): m_prefetched( true ), m_file(_file) {}
    virtual ~TemplateObj() {}
    virtual bbool onTemplateLoaded() { return btrue; }

    const Path&                 getFile() const { return m_file; }
    void                        setFile(const Path & _file) { m_file = _file; }

    void                        saveInFile();

private:
    bool                        m_prefetched;
    Path                        m_file;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class TemplatePickable : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(TemplatePickable,TemplateObj,2105927065)
    DECLARE_SERIALIZE_VIRTUAL()

public:

    TemplatePickable();
    TemplatePickable( const Path& _path);
    ~TemplatePickable();

    f32                 getAngle() const { return m_angle.ToRadians(); }
    const Vec2d&        getScale() const { return m_scale; }
    u32                    getObjectFamily() const { return m_objectFamily; }

private:

    Angle               m_angle;
    Vec2d               m_scale;
    u32                 m_objectFamily;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class TemplateActorComponent : public TemplateObjBase
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(TemplateActorComponent,TemplateObjBase,1538592105)
    DECLARE_SERIALIZE_INTERFACE()

public:

    TemplateActorComponent()
        : m_actorTemplate(NULL)
    {}
    ~TemplateActorComponent() {}

    virtual class ActorComponent* createComponent() const = 0;
    virtual StringID::StringIdValueType getComponentClassCRC() const = 0;
    virtual void onTemplateLoaded() {}

    void setActorTemplate( class Actor_Template * _template ) { m_actorTemplate = _template; }
    const class Actor_Template* getActorTemplate() const { return m_actorTemplate; }

private:

    class Actor_Template    * m_actorTemplate;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////

class Actor_Template : public TemplatePickable
{
    DECLARE_OBJECT_CHILD_RTTI(Actor_Template,TemplatePickable,461732814)
    DECLARE_SERIALIZE_VIRTUAL()

public:

    typedef SafeArray<TemplateActorComponent*> ComponentList;

    Actor_Template();
    Actor_Template( const Path& _path);
    ~Actor_Template();

    virtual bbool           onTemplateLoaded();
    
    template <class T>
    T*                      GetComponent() const;

    void                    addTemplateComponent(class TemplateActorComponent* _templateActorComponent) { m_components.push_back(_templateActorComponent); }

    void                    delTemplateComponent(const String & _component, bbool _clearMem);
    void                    insertTemplateComponent( TemplateActorComponent* _actorComponent, u32 _index);
    TemplateActorComponent* getTemplateComponentFromString(const String & _component) const;
    TemplateActorComponent* getTemplateComponentFromString(const char* _component) const;
    void                    moveTemplateComponent( const String & _component, u32 _newPos);
    i32                     getIndexTemplateComponentFromString(const String & _component) const;
    i32                     getIndexTemplateComponentFromString(const char* _component) const;

    const Vec2d&            getScaleForced() const { return m_scaleForced; }
    void                    setScaleForced(Vec2d& _scaleForced) { m_scaleForced = _scaleForced; }
    const Vec2d&            getScaleMin() const { return m_scaleMin; }
    const Vec2d&            getScaleMax() const { return m_scaleMax; }
    bbool                   getIsProcedural() const { return m_isProcedural; }
    bbool                   getStartPaused() const { return m_startPaused; }
    void                    setStartPaused(bbool _startPaused) { m_startPaused = _startPaused; }
    const ComponentList&    getComponents() const { return m_components; }
    f32                     getForcedZ() const { return m_forcedZ; }
    bbool                   isZForced()const        { return m_useZForced; }
    const StringID&         getArchetype() const    { return m_archetype; }
    const StringID&         getType() const            { return m_type; }
    Pickable::UpdateType    getUpdateType() const { return m_updateType; }

    ResourceID              addResource( Resource::ResourceType _type, const Path& _path );
    void                    removeResource(const ResourceID &_resource);

    ResourceGroup*            getTemplateResourceGroup() const;

private:

    ResourceGroup*            getResourceGroup();

    ComponentList       m_components;
    Vec2d               m_scaleForced;
    f32                 m_forcedZ;
    bbool               m_xFlipped;
    bbool               m_isProcedural;
    bbool               m_startPaused;
    bbool               m_useZForced;
    Pickable::UpdateType m_updateType;
    StringID            m_archetype;
    StringID            m_type;
    Vec2d               m_scaleMin;
    Vec2d               m_scaleMax;

    ResourceID          m_resourceGroup;
    static u32          s_groupCount;
};

template <class T>
T* Actor_Template::GetComponent() const
{
    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        TemplateActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor template could not have a NULL component template");

        if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
        {
            return static_cast<T*>(pComponent);
        }
    }

    return NULL;
}

#define DECLARE_ACTORCOMPONENT_TEMPLATE(actorComponentClass) \
public: \
    virtual class ActorComponent* createComponent() const { return newAlloc(mId_GameplayTemplate, actorComponentClass()); } \
    virtual StringID::StringIdValueType getComponentClassCRC() const { return getComponentClassCRCStatic(); } \
    static StringID::StringIdValueType getComponentClassCRCStatic() { return actorComponentClass::GetClassCRCStatic(); }


struct TemplateCache
{
    u32             m_uncompressedSize;
    u32             m_compressedSize;
    const u8*       m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class TemplateDatabase
{
private:
    typedef ITF_MAP <StringID,TemplateObj*> TemplateMap;

public:
    TemplateDatabase();
    ~TemplateDatabase();

    typedef const void*     TemplateclientID;

    void                    clear();

    bbool                   hasTemplate( const Path& _path ) const;
    bbool                   hasTemplate( const StringID& _id ) const;
    template <class T>
    const T*                getTemplate( TemplateclientID _client, const Path& _path, bool prefetch = false );
    template <class T>
    const T*                getTemplate( TemplateclientID _client, const StringID& _id );
    void                    unloadTemplate( const StringID& _id );
    template <class T>
    const T*                reloadTemplate( const Path& _path );
    ObjectFactory*          getComponentsFactory() { return &m_componentsFactory; }
    ObjectFactory*          getBTNodesFactory() { return &m_btNodesFactory; }
    ObjectFactory*          getClassTemplateFactory() {return  &m_classTemplateFactory;}

    bool                    isTemplatePrefetched( const Path& _path ) const;
    bbool                   isTemplateMarkedForRemove( const Path& _path ) const;

#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
    bbool                   serializeClassTemplate(const String& _path,const String& _platform);
#endif //(!ITF_FINAL) && !defined (ITF_CONSOLE)

    void                    enableModeBinary()         {m_modeBinary = 1;}
    void                    disableModeBinary()        {m_modeBinary = 0;}
    const  u32              isModeBinary()             const    {return m_modeBinary;}       

    void                    addTemplateClient(const StringID& _template, TemplateclientID _client);
    void                    removeTemplateClient(const StringID& _template, TemplateclientID _client);
    bbool                   isTemplateClient(const StringID& _template, TemplateclientID _client);

    void                    addToCache(TemplateclientID _client, const Path& _path,u32 _uncompressedSize,u32 _compressed,const u8* buffer);
    void                    removeFromCache(const StringID& _id);
    void                    removeFromCache(const Path& _path);
    ArchiveMemory*          getFromCache(const Path& _path);
    bbool                   hasCacheTemplate(const StringID& _id);
    bbool                   hasCacheTemplate(const Path& _path);

private:
    struct TemplateClients
    {
        StringID    m_template;
        SafeArray<TemplateclientID> m_clients;
    };

    template< typename T >
    T* prefetchTemplate( const Path& _path );
    void loadTemplate( TemplateclientID _client, TemplateObj* newTemplate, const Path& _path );

    mutable ITF_THREAD_CRITICAL_SECTION m_templatesCs;
    mutable ITF_THREAD_CRITICAL_SECTION m_templatesClientsCs;
    u32                                 m_modeBinary;
    TemplateMap                         m_templates;
    ObjectFactory                       m_componentsFactory;
    ObjectFactory                       m_btNodesFactory;
    ObjectFactory                       m_classTemplateFactory;
    ITF_MAP<StringID, TemplateClients>  m_templateClients;
    ITF_MAP<StringID,TemplateCache>     m_templateCache;

#if defined(ITF_SUPPORT_LOGICDATABASE)
    static const u32 action_nothing = 0;
    static const u32 action_delete = 1;
    static const u32 action_search = 2;

    struct TemplateMissingFileInfo
    {
        Path    m_path;
        Path    m_newPath;
        u32     m_action;
    };
    typedef ITF_VECTOR<TemplateMissingFileInfo> MissingFilesList;
    bbool checkFile( Path& _path );
    bbool lookForMissingFile( TemplateMissingFileInfo& _info, const String& _actorFileName, Path& _luaPath ) const;
    MissingFilesList        m_missingFiles;
#else
    bbool                   checkFile( Path& _path ) const { return btrue; }
#endif // ITF_SUPPORT_LOGICDATABASE

    
};

template <class T>
const T* TemplateDatabase::getTemplate( TemplateclientID _client, const StringID& _id )
{
    ITF_ASSERT_MSG(isTemplateClient(_id, _client), "TemplateDatabase::getTemplate : please register client first");

    csAutoLock lock( &m_templatesCs );
    TemplateMap::const_iterator it = m_templates.find(_id);

    if ( it != m_templates.end() && !it->second->m_prefetched )
    {
        return it->second->DynamicCast<T>(T::GetClassCRCStatic());
    }

    return NULL;
}

template <class T>
const T* TemplateDatabase::getTemplate( TemplateclientID _client, const Path& _path, bool prefetch )
{
     TemplateMap::const_iterator it;
     {
         csAutoLock                  lock( &m_templatesCs );
         it = m_templates.find(_path.getStringID());
    }

    Path                        loadPath = _path;

    if ( it == m_templates.end() && !checkFile( loadPath ) )
        return NULL;
    if ( prefetch )
    {
        if ( it != m_templates.end() )
            return it->second->DynamicCast<T>(T::GetClassCRCStatic());
        return prefetchTemplate< T >( loadPath );
    }
    else if ( it != m_templates.end() )
    {
        ITF_ASSERT_CRASH( it->second != NULL, "NULL template registered" );
        if ( it->second->m_prefetched )
            loadTemplate( _client, it->second, loadPath );
        return it->second->DynamicCast<T>(T::GetClassCRCStatic());
    }
    
    T* newTemplate = prefetchTemplate< T >( _path );

    if ( newTemplate != NULL )
        loadTemplate( _client, newTemplate, _path );
    return newTemplate;
}

template< typename T >
T* TemplateDatabase::prefetchTemplate( const Path& _path )
{
    CSerializerObject* dbSerializer = NULL;

    if (m_modeBinary)
    {
        ArchiveMemory* archive = getFromCache(_path);
        
        CSerializerObjectBinary* dbSerializerBinary = archive ? newAlloc(mId_GameplayTemplate, CSerializerObjectBinary(archive,btrue)) : newAlloc(mId_GameplayTemplate, CSerializerObjectBinaryFile());

        if (dbSerializerBinary)
        {
            if (!archive)
            {
                ((CSerializerObjectBinaryFile*)dbSerializerBinary)->open(_path,btrue);
            }
            
            if ( !dbSerializerBinary->isValid())
            {
                PathString_t pathName;

                _path.getString(pathName);

                LOG( "[TEMPLATE_DATABASE] Template file not found %s", StringToUTF8(pathName).get() );
                SF_DEL(dbSerializerBinary);
                return NULL;
            }
        }
        dbSerializer = dbSerializerBinary;
    }

#ifdef ITF_SUPPORT_EDITOR
    else
    {
        PathString_t pathName;

        _path.getString(pathName);

        CSerializerObjectLogicDB* dbSerializerLogic = newAlloc(mId_GameplayTemplate, CSerializerObjectLogicDB(_path));
        if ( dbSerializerLogic && !dbSerializerLogic->isValid() || dbSerializerLogic->isEmpty() )
        {
            LOG( "[TEMPLATE_DATABASE] Template file not found %s", StringToUTF8(pathName).get() );
            SF_DEL(dbSerializerLogic);
            return NULL;
        }

        dbSerializer = dbSerializerLogic;

        // check if the template class name is there
        // NB[LaurentCou]: ideally, if the name is missing, the logic database
        // would fail parsing, but it doesn't due to the way it's used below
        // NB2[LaurentCou]: warning isn't enough, assert isn't enough
        // => don't load the template if the template class name is missing
        String8 className;
        dbSerializer->Serialize("name", className);
        if (className.isEmpty())
        {
            ITF_ASSERT_MSG(
                bfalse,
                "[TEMPLATE_DATABASE] Template name not found in %s",
                StringToUTF8(pathName).get());
            ITF_WARNING(
                NULL, bfalse,
                "[TEMPLATE_DATABASE] Template name not found in %s",
                StringToUTF8(pathName).get());
            SF_DEL(dbSerializerLogic);
            return NULL;
        }
    }

#endif //ITF_SUPPORT_LOGICDATABASE

    //T* newTemplate;
    //dbSerializer->SerializeObject("params",newTemplate,m_classTemplateFactory,ESerialize_Data_Load);
    //ITF_ASSERT(newTemplate->getObjectClass()==T::GetStaticClassName);

    T* newTemplate = newAlloc(mId_GameplayTemplate, T(_path));
    newTemplate->Serialize(dbSerializer,ESerialize_Data_Load);
    newTemplate->m_prefetched = true;
    SF_DEL(dbSerializer);
    {
        csAutoLock lock( &m_templatesCs );
        m_templates[_path.getStringID()] = newTemplate;
    }
    return newTemplate;
}


#ifdef ITF_SUPPORT_HOTRELOAD
template <class T>
const T* TemplateDatabase::reloadTemplate( const Path& _path )
{
    csAutoLock lock( &m_templatesCs );
    StringID id = _path.getStringID();
    TemplateMap::const_iterator it = m_templates.find(id);

    if ( it == m_templates.end() )
    {
        return NULL;
    }

    T* reloadTemplate = it->second->DynamicCast<T>(T::GetClassCRCStatic());

    ITF_ASSERT(reloadTemplate);

    if ( !reloadTemplate )
    {
        return NULL;
    }

    Path loadPath = _path;

    if ( !checkFile(loadPath) )
    {
        return NULL;
    }

    PathString_t pathName;

    loadPath.getString(pathName);

    if (LOGICDATABASE->needRebuildId(pathName.cStr()))
    {
        LogicDataLink* pLogicDataLink = LOGICDATABASE->createFile(pathName.cStr());

        if (!pLogicDataLink)
        {
            ITF_ASSERT(0);
            m_templates.erase(id);
            return NULL;
        }
    }

    CSerializerObjectLogicDB dbSerializer(_path);

    if ( !dbSerializer.isValid() || dbSerializer.isEmpty() )
    {
        ITF_ASSERT(0);
        return NULL;
    }

    // Delete it so that we get completely new values
    delete reloadTemplate;
    reloadTemplate = newAlloc(mId_GameplayTemplate, T(loadPath));
    reloadTemplate->Serialize(&dbSerializer,ESerialize_Data_Load);
    reloadTemplate->m_prefetched = false;
    m_templates[id] = reloadTemplate;

    bbool isOk = reloadTemplate->onTemplateLoaded();

    if ( !isOk )
    {
        String msg = "Duplicate components in :\n";
        msg += pathName;
        SYSTEM_ADAPTER->messageBox("ERROR", msg, ITF_MSG_OK, ITF_IDYES);
    }

    return reloadTemplate;
}
#endif //ITF_SUPPORT_HOTRELOAD

}

#endif // ITF_TEMPLATEDATABASE_H
