#include "precompiled_engine.h"

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#include "core/UnicodeTools.h"

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(TemplateObj)
IMPLEMENT_OBJECT_RTTI(TemplateObjBase)

bool PrefetchAct( const Path& path, PrefetchFactory::Params& params )
{
    return PrefetchTemplate< Actor_Template >( path, params );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void  TemplateDatabase::addToCache(TemplateclientID _client, const Path& _path,u32 _uncompressedSize,u32 _compressed,const u8* _buffer)
{
    ITF_ASSERT_MSG(isTemplateClient(_path.getStringID(), _client), "TemplateDatabase::getTemplate : please register client first");
    ITF_MAP<StringID,TemplateCache>::iterator iter = m_templateCache.find(_path.getStringID());
    if (iter==m_templateCache.end())
    {
        TemplateCache cache;
        cache.m_compressedSize      = _compressed;
        cache.m_uncompressedSize    = _uncompressedSize;
        cache.m_pBuffer             = new_array(u8,_compressed,mId_GameplayTemplate);
        ITF_Memcpy((void*)cache.m_pBuffer,_buffer,_compressed);
        m_templateCache[_path.getStringID()] = cache;
    }

}

void TemplateDatabase::removeFromCache(const StringID& _id)
{
    ITF_MAP<StringID,TemplateCache>::iterator it = m_templateCache.find( _id );

    if ( it != m_templateCache.end() )
    {
        TemplateCache& cache = (*it).second;

        SF_DEL_ARRAY(cache.m_pBuffer);
        m_templateCache.erase(it);
    }
}

void TemplateDatabase::removeFromCache(const Path& _path)
{
    removeFromCache( _path.getStringID() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArchiveMemory*   TemplateDatabase::getFromCache(const Path& _path)
{
    ITF_MAP<StringID,TemplateCache>::iterator iter = m_templateCache.find(_path.getStringID());
    if (iter!=m_templateCache.end())
    {
        TemplateCache& cache = (*iter).second;

        //uncompress
        u8* pAlloc = new_array(u8,cache.m_uncompressedSize,mId_Serialization);
        Compress::uncompressBuffer(pAlloc,cache.m_uncompressedSize,cache.m_pBuffer,cache.m_compressedSize);
        ArchiveMemory* archive = new ArchiveMemory(pAlloc,cache.m_uncompressedSize);
        SF_DEL_ARRAY(pAlloc);
        return archive;
    }

    return NULL;
}

bbool TemplateDatabase::hasCacheTemplate(const StringID& _id)
{
    return m_templateCache.find(_id) != m_templateCache.end();
}

bbool TemplateDatabase::hasCacheTemplate(const Path& _path)
{
    return hasCacheTemplate( _path.getStringID() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_SUPPORT_LUA

void TemplateObj::saveInFile()
{
    CSerializerObjectLua serializer;
    String path;
    
    getFile().getString(path);

    if ( serializer.OpenBufferToWrite("params", btrue, StringID(GetObjectClassCRC()).getDebugString()) )
    {
        serializer.SetPath(path);

        String fullPath;
        if (FILEMANAGER->TranslatePath(fullPath, path) && SOURCECONTROL_ADAPTER)
            SOURCECONTROL_ADAPTER->checkout(fullPath);

        // Allow const -> non const because we KNOW that we are in save Mode
        Serialize(&serializer,ESerialize_Data_Save);
        serializer.Close();
    }
}
#endif //ITF_SUPPORT_LUA
//////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(TemplatePickable)

BEGIN_SERIALIZATION(TemplatePickable)

    SERIALIZE_MEMBER("ANGLE",m_angle);
    SERIALIZE_MEMBER("SCALE",m_scale);
    SERIALIZE_MEMBER("ObjectFamily",m_objectFamily);

END_SERIALIZATION()

TemplatePickable::TemplatePickable()
: m_angle(bfalse, 0)
, m_scale(Vec2d::One)
, m_objectFamily(Pickable::ObjectFamily_LevelDesign)
{
}

TemplatePickable::TemplatePickable( const Path& _path)
: Super(_path)
, m_angle(bfalse, 0)
, m_scale(Vec2d::One)
, m_objectFamily(Pickable::ObjectFamily_LevelDesign)
{
}

TemplatePickable::~TemplatePickable()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

u32          Actor_Template::s_groupCount = 0;

IMPLEMENT_OBJECT_RTTI(Actor_Template)

BEGIN_SERIALIZATION_CHILD(Actor_Template)

    SERIALIZE_MEMBER("scaleForced",m_scaleForced);
    SERIALIZE_MEMBER("scaleMin",m_scaleMin);
    SERIALIZE_MEMBER("scaleMax",m_scaleMax);
    SERIALIZE_MEMBER("xFLIPPED",m_xFlipped);
    SERIALIZE_MEMBER("PROCEDURAL",m_isProcedural);
    SERIALIZE_MEMBER("STARTPAUSED",m_startPaused); 
    SERIALIZE_MEMBER("zForced",m_forcedZ);
    SERIALIZE_MEMBER("useZForced", m_useZForced);
	SERIALIZE_MEMBER("archetype",m_archetype);
	SERIALIZE_MEMBER("type",m_type);
    SERIALIZE_ENUM_BEGIN("updatetype",m_updateType);
        SERIALIZE_ENUM_VAR(Pickable::UpdateType_OnlyIfVisible);
        SERIALIZE_ENUM_VAR(Pickable::UpdateType_OffscreenAllowed);
    SERIALIZE_ENUM_END();

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_CONTAINER_WITH_FACTORY("COMPONENTS",m_components,TEMPLATEDATABASE->getComponentsFactory());
    END_CONDITION_BLOCK()

    

END_SERIALIZATION()

Actor_Template::Actor_Template()
: m_scaleForced(0.f, 0.f)
, m_forcedZ(0.0f)
, m_xFlipped(bfalse)
, m_isProcedural(bfalse)
, m_startPaused(bfalse)
, m_useZForced(bfalse)
, m_archetype(StringID::Invalid)
, m_type(StringID::Invalid)
, m_scaleMin(0.f, 0.f)
, m_scaleMax(0.f, 0.f)
, m_updateType(Pickable::UpdateType_OnlyIfVisible)
{
	m_resourceGroup.invalidateResourceId();
}

Actor_Template::Actor_Template( const Path& _path)
: Super(_path)
, m_scaleForced(0.f, 0.f)
, m_forcedZ(0.0f)
, m_xFlipped(bfalse)
, m_isProcedural(bfalse)
, m_startPaused(bfalse)
, m_useZForced(bfalse)
, m_archetype(StringID::Invalid)
, m_type(StringID::Invalid)
, m_scaleMin(0.f, 0.f)
, m_scaleMax(0.f, 0.f)
, m_updateType(Pickable::UpdateType_OnlyIfVisible)
{
	m_resourceGroup.invalidateResourceId();
}

Actor_Template::~Actor_Template()
{
    u32 numComponents = m_components.size();

    for ( u32 i = 0; i < numComponents; i++ )
    {
        TemplateActorComponent* comp = m_components[i];
        SF_DEL(comp);
    }

	RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);
	m_resourceGroup.invalidateResourceId();
}

bbool Actor_Template::onTemplateLoaded()
{
    if (m_resourceGroup.isValidResourceId())
	{
		RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);
		m_resourceGroup.invalidateResourceId();
	}
	m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

    const bbool xOk = m_scaleMin.m_x == 0.0f || m_scaleMax.m_x == 0.0f || m_scaleMin.m_x <= m_scaleMax.m_x;
    const bbool yOk = m_scaleMin.m_y == 0.0f || m_scaleMax.m_y == 0.0f || m_scaleMin.m_y <= m_scaleMax.m_y;
    ITF_UNUSED(xOk);
    ITF_UNUSED(yOk);
    ITF_WARNING_CATEGORY(LD, NULL, xOk, "scaleMin.x > scaleMax.x in %s", getFile().getString8().cStr());
    ITF_WARNING_CATEGORY(LD, NULL, yOk, "scaleMin.y > scaleMax.y in %s", getFile().getString8().cStr());


    bbool isOk = btrue;
    u32 numComponents = m_components.size();

    for ( u32 i = 0; i < numComponents; i++ )
    {
        TemplateActorComponent* comp = m_components[i];
		comp->setActorTemplate(this);
        comp->onTemplateLoaded();

        u32 j = i+1;

        while ( j < numComponents )
        {
            TemplateActorComponent* temp = m_components[j];
            
            if ( temp->GetObjectClassCRC() == comp->GetObjectClassCRC() )
            {
                isOk = bfalse;
                SF_DEL(temp);
                m_components.eraseKeepOrder(j);
                numComponents--;
            }
            else
            {
                j++;
            }
        }
    }

    return isOk;
}



ResourceGroup* Actor_Template::getResourceGroup()
{ 
	ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());

	if (grp)
		return grp;

    m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

	return (ResourceGroup*)(m_resourceGroup.getResource());
}

ResourceGroup* Actor_Template::getTemplateResourceGroup() const
{
	if ( m_resourceGroup.isValidResourceId() )
		return (ResourceGroup*)(m_resourceGroup.getResource());

	return NULL;
}

ResourceID Actor_Template::addResource( Resource::ResourceType _type, const Path& _path )
{
	return getResourceGroup()->addResource(_type,_path);
}

void Actor_Template::removeResource(const ResourceID &_resource)
{
	ResourceGroup* grp = getResourceGroup();
	ITF_ASSERT(grp);
	grp->removeResource(_resource);
}

void Actor_Template::delTemplateComponent(const String & _component, bbool _clearMem)
{
    TemplateActorComponent* component = getTemplateComponentFromString(_component);
    if (!component)
        return;
        
    int pos = m_components.find(component);
    if (pos  >= 0)
    {
        m_components.eraseKeepOrder(pos);
        return;
    }
    else
    {
        ITF_ASSERT_MSG(0, "Del template component error: component not found with name (%s)", UTF16ToUTF8(_component.cStr()).get());
    }
}

void Actor_Template::insertTemplateComponent( TemplateActorComponent* _actorComponent, u32 _index)
{
    if (_index >= m_components.size())
    {
        addTemplateComponent( _actorComponent);
        return;
    }
    m_components.insert(_actorComponent, _index);
}

TemplateActorComponent* Actor_Template::getTemplateComponentFromString(const String & _component) const
{
    StringConverter name(_component);
    return getTemplateComponentFromString(name.getChar());
}

TemplateActorComponent* Actor_Template::getTemplateComponentFromString(const char* _component) const
{
    StringID objId = _component;
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        TemplateActorComponent* pComponent = m_components[it];

        if(!pComponent)
            continue;

        if ( objId.GetValue() == pComponent->GetObjectClassCRC() )
            return pComponent;
    }

    return NULL;
}

i32 Actor_Template::getIndexTemplateComponentFromString(const String & _component) const
{
    StringConverter name(_component);
    return getIndexTemplateComponentFromString(name.getChar());
}

i32 Actor_Template::getIndexTemplateComponentFromString(const char* _component) const
{
    StringID objId = _component;
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        TemplateActorComponent* pComponent = m_components[it];

        if(!pComponent)
            continue;

        if ( objId.GetValue() == pComponent->GetObjectClassCRC() )
            return (i32)it;
    }

    return -1;
}

void Actor_Template::moveTemplateComponent( const String & _component, u32 _newPos)
{
    if (_newPos >= m_components.size())
        return;

    TemplateActorComponent* actorComponent = getTemplateComponentFromString(_component);
    if (!actorComponent)
        return;

    delTemplateComponent( _component, bfalse );
    insertTemplateComponent( actorComponent, _newPos);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(TemplateActorComponent)

//////////////////////////////////////////////////////////////////////////////////////////////////////

TemplateDatabase::TemplateDatabase()
{
    m_modeBinary = bfalse;
    Synchronize::createCriticalSection( &m_templatesCs );
    Synchronize::createCriticalSection( &m_templatesClientsCs );
}

TemplateDatabase::~TemplateDatabase()
{
    clear();
    Synchronize::destroyCriticalSection( &m_templatesCs );
    Synchronize::destroyCriticalSection( &m_templatesClientsCs );
}

void TemplateDatabase::clear()
{
    m_templateClients.clear();
   // ITF_ASSERT_MSG(0 == m_templateClients.size(), "some template clients don't unregister");
    csAutoLock lock( &m_templatesCs );

    for ( TemplateMap::iterator it = m_templates.begin(); it != m_templates.end(); ++it )
    {
        TemplateObj* obj = it->second;

        delete obj;
    }

    m_templates.clear();
}

bbool TemplateDatabase::hasTemplate( const Path& _path ) const
{
    csAutoLock lock( &m_templatesCs );
    return m_templates.find(_path.getStringID()) != m_templates.end();
}

bbool TemplateDatabase::hasTemplate( const StringID& _id ) const
{
    csAutoLock lock( &m_templatesCs );
    return m_templates.find(_id) != m_templates.end();
}

bool TemplateDatabase::isTemplatePrefetched( const Path& _path ) const
{
    csAutoLock                  lock( &m_templatesCs );
    TemplateMap::const_iterator it = m_templates.find( _path.getStringID() );

    if ( it != m_templates.end() )
        return it->second->m_prefetched;
    return false;
}

bbool TemplateDatabase::isTemplateMarkedForRemove( const Path& _path ) const
{
#if defined(ITF_SUPPORT_LOGICDATABASE)
    ITF_VECTOR<TemplateMissingFileInfo>::const_iterator it = m_missingFiles.begin();
    ITF_VECTOR<TemplateMissingFileInfo>::const_iterator end = m_missingFiles.end();

    for (; it != end; ++it)
    {
        const TemplateMissingFileInfo& localInfo = *it;

        if ( localInfo.m_path == _path )
        {
            return localInfo.m_action == action_delete;
        }
    }
#endif
    return bfalse;
}

#if defined(ITF_SUPPORT_LOGICDATABASE)

bbool TemplateDatabase::lookForMissingFile( TemplateDatabase::TemplateMissingFileInfo& _info, const String& _actorFileName, Path& _luaPath ) const
{
    _info.m_action = action_nothing;
    for (u32 itPath = 0; itPath < CONFIG->m_actorLoadPaths.size(); itPath++)
    {
        char found[2048];
        found[0] = 0;
        const ActorLoadPath& loadPath = CONFIG->m_actorLoadPaths[itPath];
        String thisPath = DATA_FOLDER;
        thisPath += loadPath.m_path;
        if (Directory::searchFile(found, StringConverter(thisPath).getChar(), _actorFileName, loadPath.m_recursive))
        {
            String newpath(found);
            newpath += _actorFileName;
            switch(SYSTEM_ADAPTER->messageBox("Use file found in folder?", newpath, ITF_MSG_YESNO | ITF_MSG_ICONQUESTION,ITF_IDNO))
            {
            case ITF_IDYES:
                _info.m_action = action_search;
                _info.m_newPath = newpath;
                _luaPath = newpath;
                return btrue;
            default:
                break;
            }
        }
    }

    return bfalse;
}

bbool TemplateDatabase::checkFile( Path& _path )
{
    bbool hasError  = bfalse;

    if (!FILEMANAGER->fileExists(_path.getString()))
    {
        ITF_VECTOR<TemplateMissingFileInfo>::const_iterator it = m_missingFiles.begin();
        ITF_VECTOR<TemplateMissingFileInfo>::const_iterator end = m_missingFiles.end();

        for (; it != end; ++it)
        {
            const TemplateMissingFileInfo& localInfo = *it;

            if (localInfo.m_path == _path)
            {
                switch (localInfo.m_action)
                {
                case action_nothing:
                    return bfalse;

                case action_delete:
                    _path = "";
                    return bfalse;

                case action_search:
                    _path = (*it).m_newPath;
                    return btrue;

                default:
                    ITF_ASSERT_MSG(bfalse, "unhandled case, this is bad");
                    return bfalse;
                }
            }
        }

        TemplateMissingFileInfo info;
        info.m_path = _path;
        info.m_newPath = "";

        String msg = "Missing file:\n";
        msg += _path.getString();
        msg += "\nPress Yes to look for the file in the database";
        msg += "\nPress No to remove this object from the scene";
        msg += "\nPress Cancel to leave the scene as it is";
        switch (SYSTEM_ADAPTER->messageBox("ERROR", msg, ITF_MSG_YESNOCANCEL,ITF_IDCANCEL))
        {
        case ITF_IDYES:
            {
                bbool pathFound = bfalse;

                String currentName = FilePath::getFilename(_path.getString());
                currentName.toLower();

                for (u32 itAlias = 0; itAlias < CONFIG->m_actorAliases.size(); itAlias++)
                {
                    const ActorAlias& alias = CONFIG->m_actorAliases[itAlias];
                    if (alias.m_oldName == currentName)
                    {
                        pathFound = lookForMissingFile(info, alias.m_newName, _path);
                        if (pathFound)
                        {
                            break;
                        }
                    }
                }

                if (!pathFound)
                {
                    pathFound = lookForMissingFile(info, currentName, _path);
                }

                if (!pathFound)
                {
                    SYSTEM_ADAPTER->messageBox("FILE NOT FOUND", "Could not find file", ITF_MSG_OK | ITF_MSG_ICONERROR,ITF_IDYES);
                    hasError  = btrue;
                }
            }
            break;
        case ITF_IDNO:
            _path = "";
            info.m_action = action_delete;
            hasError  = btrue;
            break;
        case ITF_IDCANCEL:
            info.m_action = action_nothing;
            hasError  = btrue;
            break;
        }

        m_missingFiles.push_back(info);

        CSerializerObjectLogicDB::addHandledPathError(info.m_path);
        if (hasError)
        {
            return bfalse;
        }

        return btrue;
    }
    return btrue;
}

bbool   TemplateDatabase::serializeClassTemplate(const String& _path,const String& _platform)
{

    CSerializerObjectLogicDB dbSerializer(_path,_platform);

    if ( !dbSerializer.isValid() || dbSerializer.isEmpty() )
    {
        return bfalse;
    }

    TemplateObj* templateObj = NULL;
    String8 className;
    dbSerializer.Serialize("name",className);

    templateObj = m_classTemplateFactory.CreateObject<TemplateObj>(StringID(className).GetValue());
    ITF_ASSERT_MSG(templateObj,"Template not found %s in file %s", className.cStr(), StringToUTF8(_path.cStr()));

    if (!templateObj)
        return bfalse;

    templateObj->Serialize(&dbSerializer,ESerialize_Data_Load);

    ArchiveMemory arch;
    CSerializerObjectBinary  serializer(&arch);
    templateObj->Serialize(&serializer,ESerialize_Data_Save);

    String filename = FILESERVER->getCookedNamePlatform(_path,_platform);

    File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
    if (pFile)
    {
        pFile->write(arch.getData(),arch.getSize());
        FILEMANAGER->closeFile(pFile);
        pFile = NULL;
    }

   

    SF_DEL(templateObj);

    return btrue;
}

#endif // ITF_SUPPORT_LOGICDATABASE

void TemplateDatabase::loadTemplate( TemplateclientID _client, TemplateObj* newTemplate, const Path& _path )
{
    ITF_ASSERT_MSG(isTemplateClient(_path.getStringID(), _client), "TemplateDatabase::getTemplate : please register client first");
    ITF_ASSERT( newTemplate != NULL );
    if ( newTemplate == NULL )
        return;
    newTemplate->m_prefetched = false;
    if ( !newTemplate->onTemplateLoaded() )
    {
        String msg = "Duplicate components in :\n";
        PathString_t pathName;
        _path.getString(pathName);
        msg += pathName;
        SYSTEM_ADAPTER->messageBox("ERROR", msg, ITF_MSG_OK, ITF_IDYES);
    }

//    LOG("[TemplateDatabase] load %s", StringConverter(_path.getString()).getChar());

}

void TemplateDatabase::addTemplateClient(const StringID& _template, TemplateclientID _client)
{
    csAutoLock lock( &m_templatesClientsCs );

    ITF_MAP<StringID, TemplateClients>::iterator it = m_templateClients.find(_template);
    if (it == m_templateClients.end())
    {
        TemplateClients clients;
        clients.m_template = _template;
        clients.m_clients.push_back(_client);
        m_templateClients[_template] = clients;
    }
    else
    {
        TemplateClients& clients = it->second;
        if (clients.m_clients.find(_client) >= 0)
        {
            ITF_ASSERT_MSG(bfalse, "template client already registered");
            return;
        }
        clients.m_clients.push_back(_client);
    }
}

void TemplateDatabase::removeTemplateClient(const StringID& _template, TemplateclientID _client)
{
    csAutoLock lock( &m_templatesClientsCs );

    ITF_MAP<StringID, TemplateClients>::iterator it = m_templateClients.find(_template);
    if (it == m_templateClients.end())
    {
        ITF_ASSERT_MSG(0, "not a client");
        return;
    }
    TemplateClients& clients = it->second;
    i32 index = clients.m_clients.find(_client);
    if (index  < 0)
    {
        ITF_ASSERT_MSG(bfalse, "not a client");
        return;
    }
    clients.m_clients.eraseNoOrder(index);
    if ( hasCacheTemplate( _template ) )
    {
        switch ( clients.m_clients.size() )
        {
        case 0:
            removeFromCache( _template );
            m_templateClients.erase(it);
            break;
        case 1:
            if ( hasTemplate( _template ) )
                unloadTemplate( _template );
            break;
        }
    }
    else if ( clients.m_clients.size() == 0 )
    {
        unloadTemplate(_template);
        m_templateClients.erase(it);
    }
}

bbool TemplateDatabase::isTemplateClient(const StringID& _template, TemplateclientID _client)
{
    csAutoLock lock( &m_templatesClientsCs );

    ITF_MAP<StringID, TemplateClients>::iterator it = m_templateClients.find(_template);
    if (it == m_templateClients.end())
    {
        return bfalse;
    }
    else
    {
        TemplateClients& clients = it->second;
        if (clients.m_clients.find(_client) < 0)
        {
            return bfalse;
        }
        return btrue;
    }
}

void TemplateDatabase::unloadTemplate( const StringID& _id )
{    
    csAutoLock              lock( &m_templatesCs );
    TemplateMap::iterator   it = m_templates.find(_id);

    if ( it == m_templates.end() )
    {
        ITF_ASSERT_MSG(bfalse, "TemplateDatabase::unloadTemplate is trying to unload an unknown template");
        return;
    }

    TemplateObj* unloadTemplate = (TemplateObj*)it->second;

    delete unloadTemplate;
    m_templates.erase(it);
}

}
