#include "precompiled_engine.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif // ITF_IPAD

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#include "engine/profiler/profilerMacro.h"

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Actor)

static bbool s_prevFlipped = bfalse;

DECLARE_RASTER(actorUpdate, RasterGroup_ActorUpdate, Color::blue());
DECLARE_RASTER(componentUpdate, RasterGroup_ComponentUpdate, Color::green());

BEGIN_SERIALIZATION_CHILD(Actor)

    SERIALIZE_FUNCTION(clear,ESerialize_Data_Load);
    SERIALIZE_FUNCTION(preChangeProperties,ESerialize_PropertyEdit_Load);
    SERIALIZE_FUNCTION(preSaveData, ESerialize_Data_Save);

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("LUA",m_lua);
        SERIALIZE_MEMBER("xFLIPPED", m_startFlipped);
        SERIALIZE_OBJECT("parentBind", m_pParentBind);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable|ESerializeGroup_Checkpoint)
        SERIALIZE_CONTAINER_WITH_FACTORY("COMPONENTS",m_components,ACTORSMANAGER->getActorComponentsFactory());
    END_CONDITION_BLOCK()

    SERIALIZE_FUNCTION(postChangeProperties,ESerialize_PropertyEdit_Load);

END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////
static SafeArray<StringID::StringIdValueType> g_rasterColorRegistry;

u32 Actor::s_groupCount = 0;
///////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor()
: Pickable()
, m_templateActor(NULL)
, m_lastUpdateFrame(U32_INVALID)
, m_templateChanged(bfalse)
, m_is2DActor(bfalse)
, m_updatePaused(bfalse)
, m_drawDisabled(bfalse)
, m_startFlipped(bfalse)
, m_isFlipped(bfalse)
, m_isSpawned(bfalse)
, m_needsRegisteringSerializableData(btrue)
, m_pParentBind(NULL)
, m_componentDestructionValidation( bfalse )
, m_overrideTemplateActor(NULL)
, m_deleteOverrideTemplateWithActor(bfalse)
, m_destructionRequested(bfalse)
{
    m_templateClientAdded = 0;
    setObjectType(BaseObject::eActor);
    m_resourceGroup.invalidateResourceId();
    m_binding.setOwner(this);
    m_childrenHandler.setOwner(this);
    m_initialScale = UNINIT_VEC2D;
    m_initialRot = UNINIT_F32;
    #ifdef ASSERT_ENABLED
        m_hotReloadingStatus = HotReloadType_None;
    #endif  //ASSERT_ENABLED

}

Actor::~Actor()
{
#ifdef ITF_SUPPORT_LUA
    if (LUA_HANDLER)
    {
        PathString_t luaPathName;

        m_lua.getString(luaPathName);

        LUA_HANDLER->unregisterLUAFileUsage(this->getRef(),luaPathName.cStr());
    }
#endif //ITF_WINDOWS

    clear();
    clearBinds(btrue);

    RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);
    notifyParentsAndChildren(ChangeType_Deleted);

    if (m_overrideTemplateActor && m_deleteOverrideTemplateWithActor)
    {
        SF_DEL(m_overrideTemplateActor);
    }

    if (TEMPLATEDATABASE && m_templateClientAdded)
    {
        TEMPLATEDATABASE->removeTemplateClient(m_lua.getStringID(), this);
        m_templateClientAdded = 0;
    }
}

void Actor::preChangeProperties()
{
    s_prevFlipped = m_startFlipped;
}

void Actor::postChangeProperties()
{
    if ( s_prevFlipped != m_startFlipped )
    {
        setFlipped(m_startFlipped);
    }
}

void Actor::preSaveData()
{
    // if it's a temporary bind (runtime), delete the object before serialization
    // it's not re-created after the serialization => why ? because it needs actorbind copy, creation of a member...
    // a better management would be bind "a bind manager" maybe ??
    if ( ( m_pParentBind != NULL ) && m_pParentBind->m_temporaryBind )
    {
        unbindFromParent(btrue);
    }
}
    
void Actor::setStartFlipped( bbool _value )
{
    if ( _value != m_startFlipped )
    {
        m_startFlipped = _value;
    }
}    

void Actor::setFlipped( bbool _value )
{
    if(_value != m_isFlipped)
    {
        m_isFlipped = _value;
    }
}

void Actor::requestDestruction()
{
    if ( !m_destructionRequested )
    {
        m_childrenHandler.requestChildrenDestruction();

        if ( m_componentDestructionValidation )
        {
            bbool destructionValidated = btrue;

            for ( u32 it = 0; it < m_components.size(); ++it )
            {
                ActorComponent* component = m_components[it];
                if ( !component->isActorDestructionValidated() )
                {
                    destructionValidated = bfalse;
                    break;
                }
            }
            if ( !destructionValidated )
                return;
        }

        // if it's a temp actor clear bind now
        if ( isSpawned() )
        {
            clearBinds( btrue );
        }

        if (Scene* pScene = getScene())
        {
            pScene->unregisterPickable(this, btrue);
        }
        m_destructionRequested = btrue;
    }
}

void Actor::setLUAAsDataError()
{
    if (!hasDataError())
    {
        String Label = "Lua has some error(s) or lua file is missing : ";
        String fullpath;
        PathString_t luaPathName;

        m_lua.getString(luaPathName);

        FILEMANAGER->TranslatePath(fullpath, luaPathName);
        Label += fullpath;
        setDataError(Label);
    }
}

void Actor::onLoadProcessTemplate()
{
    if (!m_templateClientAdded)
    {
        TEMPLATEDATABASE->addTemplateClient(m_lua.getStringID(), this);
        m_templateClientAdded = 1;
    }

    m_templateActor = m_overrideTemplateActor ? m_overrideTemplateActor : TEMPLATEDATABASE->getTemplate<Actor_Template>(this, m_lua);

#ifdef ITF_SUPPORT_EDITOR
    if (!m_overrideTemplateActor && m_templateActor)
    {
        if (m_lua != m_templateActor->getFile())
        {
            setTemplateChanged(btrue);
            m_lua = m_templateActor->getFile();
        }
    }
#endif // ITF_SUPPORT_EDITOR

    u32 numComponents = m_components.size();

    for ( u32 i = 0; i < numComponents; i++ )
    {
        ActorComponent* actorComponent = m_components[i];

        if ( actorComponent )
        {
            actorComponent->SetActor(this);
        }
    }

    if ( !m_templateActor )
    {
        setLUAAsDataError();
        pauseAllComponents();
        return;
    }
    
#ifdef  ITF_SUPPORT_EDITOR
    setObjectFamily(m_templateActor->getObjectFamily());
#endif //ITF_SUPPORT_EDITOR

    // Fix any difference between the instance data and the template
    // First delete all the component duplicates
    const Actor_Template::ComponentList& components = m_templateActor->getComponents();
    u32 numTemplateComponents = components.size();

    for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
    {
        const TemplateActorComponent* compTemplate = components[templateIndex];
        u32 occurrences = 0;
        u32 componentIndex = 0;

        while ( componentIndex < m_components.size() )
        {
            ActorComponent* component = m_components[componentIndex];

            if ( component->GetObjectClassCRC() == compTemplate->getComponentClassCRC() )
            {
                occurrences++;

                if ( occurrences > 1 )
                {
                    // Delete duplicates
                    SF_DEL(component);
                    m_components.eraseKeepOrder(componentIndex);
                }
                else
                {
                    componentIndex++;
                }
            }
            else
            {
                componentIndex++;
            }
        }
    }

    // Then delete all the components that are not present on the template
    u32 componentIndex = 0;
    while ( componentIndex < m_components.size() )
    {
        ActorComponent* component = m_components[componentIndex];
        bbool found = bfalse;

        for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
        {
            const TemplateActorComponent* compTemplate = components[templateIndex];
            if ( component->GetObjectClassCRC() == compTemplate->getComponentClassCRC() )
            {
                found = btrue;
                break;
            }
        }

        if ( !found )
        {
            SF_DEL(component);
            m_components.eraseKeepOrder(componentIndex);
        }
        else
        {
            componentIndex++;
        }
    }

    // Finally arrange the components order to match the one in the template
    for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
    {
        const TemplateActorComponent* compTemplate = components[templateIndex];
        StringID::StringIdValueType componentClassCRC = compTemplate->getComponentClassCRC();
        bbool found = bfalse;

        for ( u32 componentIndex = templateIndex; componentIndex < m_components.size(); componentIndex++ )
        {
            ActorComponent* comp = m_components[componentIndex];

            if ( comp->GetObjectClassCRC() == componentClassCRC )
            {
                found = btrue;
                comp->setTemplate(compTemplate);

                if ( componentIndex != templateIndex )
                {
                    if ( templateIndex >= m_components.size() )
                    {
                        m_components.resize(templateIndex+1);
                    }

                    ActorComponent* prevComp = m_components[templateIndex];
                    m_components[templateIndex] = comp;
                    m_components[componentIndex] = prevComp;
                }

                break;
            }
        }

        if (!found)
        {
            ActorComponent* comp = compTemplate->createComponent();

            comp->SetActor(this);
            comp->setTemplate(compTemplate);

            if ( templateIndex >= m_components.size() )
            {
                m_components.push_back(comp);
            }
            else
            {
                m_components.insert(comp,templateIndex);
            }
        }
    }

    /*
    // TODO: faster version in final
    // This part can be skipped in final. In final we can assume the order is correct.
    // Still, we need to set the template.
    const TemplateActor::ComponentList& components = m_templateActor->getComponents();
    u32 numTemplateComponents = components.size();
    u32 numComponents = m_components.size();

    for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
    {
        const TemplateActorComponent* compTemplate = components[templateIndex];
        const char* componentClassName = compTemplate->getComponentClassName();
        bbool found = bfalse;

        for ( u32 componentIndex = 0; componentIndex < numComponents; componentIndex++ )
        {
            ActorComponent* comp = m_components[componentIndex];

            if ( comp->GetObjectClassName() == componentClassName )
            {
                found = btrue;
                comp->setTemplate(compTemplate);

                if ( componentIndex != templateIndex )
                {
                    if ( templateIndex >= numComponents )
                    {
                        m_components.resize(templateIndex+1);
                    }

                    ActorComponent* prevComp = m_components[templateIndex];
                    m_components[templateIndex] = comp;
                    m_components[componentIndex] = prevComp;
                }

                break;
            }
        }

        if ( !found )
        {
            ActorComponent* newComponent = compTemplate->createComponent();
            newComponent->setTemplate(compTemplate);
            m_components.push_back(newComponent);
            numComponents++;
        }
    }

    u32 componentsIndex = 0;

    while ( componentsIndex < numComponents )
    {
        ActorComponent* comp = m_components[componentsIndex];

        if ( !comp->getTemplate() )
        {
            SF_DEL(comp);
            m_components.eraseKeepOrder(componentsIndex);
            numComponents--;
        }
        else
        {
            componentsIndex++;
        }
    }
    */
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bbool  Actor::onLoadedFromScene(HotReloadType _hotReload )
{
    return Super::onLoadedFromScene(_hotReload);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Actor::onLoaded(HotReloadType _hotReload)
{
#ifdef ASSERT_ENABLED
    m_hotReloadingStatus = _hotReload;
#endif  //ASSERT_ENABLED

    // Compatibility with scene saved when ActorBind's factory was used
    if(m_pParentBind && !m_pParentBind->m_parent.isValid())
    {
        SF_DEL(m_pParentBind);
    }

    onLoadProcessTemplate();

    if ( m_templateActor )
    {
        if ( m_initialScale == UNINIT_VEC2D )
        {
            setLocalInitialScale(m_templateActor->getScale());
        }
        if ( m_initialRot == UNINIT_F32 )
        {
            setLocalInitialRot(m_templateActor->getAngle());
        }

        if(!isUpdateTypeFromTemplateOverridden())
            setUpdateType(m_templateActor->getUpdateType());
    }

    Super::onLoaded(_hotReload);

    if ( hasDataError() )
    {
        #ifdef ASSERT_ENABLED
            m_hotReloadingStatus = HotReloadType_None;
        #endif  //ASSERT_ENABLED
        return;
    }

    // add template resource group as a child of actor's resource group
    ResourceGroup * grp = getResourceGroup();
    if ( grp )
    {
        if (_hotReload == HotReloadType_Checkpoint && !grp->isEmpty())
            grp->clearAll(bfalse);

        ResourceGroup * templateGrp = getTemplateResourceGroup();
        if ( templateGrp )
            grp->addResource(templateGrp);
    }

    resetCurrentInfos();

    // If there is a parent bind, warn the parent at the loading time
    if(m_pParentBind)
    {
        if(HotReloadType_None == _hotReload)
        {
            Pickable* pParent = NULL;
            const ObjectPath& parentPath = m_pParentBind->m_parent;
            if(parentPath.getIsAbsolute())
                pParent = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
            else
                pParent = SceneObjectPathUtils::getObjectFromRelativePath(this, parentPath);

            if(pParent)
            {
                ITF_ASSERT_CRASH(pParent->getObjectType() == BaseObject::eActor, "bad type");
                Actor* pActorParent = (Actor*)pParent;
                    pActorParent->getChildrenBindHandler().addChild(this); // here the children make the parent to compute curr and init pos info
            }
            else
            {
                ITF_STDSTRING str;
                parentPath.toString(str);

                ITF_WARNING_PARAM_CATEGORY(LD,this, "m_pParentBind", 0, "Dead bind! Cannot find parent: '%s'", str.c_str());
            }
        }
        else if (HotReloadType_Checkpoint == _hotReload)
        {
            // reset runtime actor-bind disabling
            m_pParentBind->setRuntimeDisabled(bfalse);
        }
    }

    // The parent is positionning his children at load time
    // It's done from parent and children because of the call order of onLoaded(...)
    if(HotReloadType_None == _hotReload)
    {
        const ObjectRefList& bindedChildren = m_childrenHandler.getChildren();
        for(u32 i = 0; i < bindedChildren.size(); ++i)
        {
            Actor* pChild = static_cast<Actor*>(bindedChildren[i].getObject());

            m_childrenHandler.updateWorldCoordinates(pChild, pChild->getParentBind());

            pChild->setWorldInitialPos(pChild->getPos());
            pChild->setWorldInitialRot(pChild->getAngle());
        }
    }

#ifdef ITF_SUPPORT_LUA
    {
        PathString_t luaPathName;

        m_lua.getString(luaPathName);

        LUA_HANDLER->registerLUAFileUsage(this->getRef(),luaPathName.cStr());
    }
#endif //ITF_WINDOWS

    for ( u32 it = 0; it < m_components.size(); )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");
        if(pComponent)
        {
            pComponent->SetActor(this);
            pComponent->onActorLoaded(_hotReload);
            it++;
        }
        else
        {
            setDataError("Null component");
            m_components.eraseKeepOrder(it);
        }
    }

    if (!hasDataError())
    {  
        // Now that all the components are initialized, add them to the update lists
        // This needs to be done after loading ALL the components, in case one modifies another
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* actorComponent = m_components[it];
            if(!actorComponent)
                continue;

            if (actorComponent->needsDraw() )
            {
#ifndef ITF_FINAL
                ITF_ASSERT(!actorComponent->needsDraw2D());
                ITF_ASSERT(!is2DActor());
                if (m_drawComponents.size() >= m_components.size())
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .act file? m_drawComponents.size() >= m_components.size()");
                }
                if (m_drawComponents.find(actorComponent) >= 0)
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .act file? m_drawComponents.find(actorComponent) >= 0");
                }
#endif // ITF_FINAL
                if (!hasDataError())
                {
                    m_drawComponents.push_back(actorComponent);
                }
            }
            else if (actorComponent->needsDraw2D())
            {
#ifndef ITF_FINAL
                if (m_draw2DComponents.size() >= m_components.size())
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .act file? m_draw2DComponents.size() >= m_components.size()");
                }
                if (m_draw2DComponents.find(actorComponent) >= 0)
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .act file? m_draw2DComponents.find(actorComponent) >= 0");
                }
#endif // ifndef ITF_FINAL
                if (!hasDataError())
                {
                    m_draw2DComponents.push_back(actorComponent);
                }
                set2DActor(btrue);
            }

            if ( actorComponent->needsUpdate() && !hasDataError())
            {
                ITF_ASSERT_CRASH(m_updateComponents.size() < m_components.size(),"m_updateComponents.size() < m_components.size()");
                ITF_ASSERT_CRASH(m_updateComponents.find(actorComponent) < 0,"m_updateComponents.find(actorComponent)");
                m_updateComponents.push_back(actorComponent);
            }
        }

        if (HotReloadType_None == _hotReload && is2DActor())
        {
            getScene()->add2DActor(this);
        }
    }


    setPos(getWorldInitialPos());
    onForceMove();

    if (isStartPaused())
    {
        disable();
    }


    updatePhysicalReady();

    if ( startFlipped() )
    {
        setFlipped(btrue);
    }

    #ifdef ASSERT_ENABLED
        m_hotReloadingStatus = HotReloadType_None;
    #endif  //ASSERT_ENABLED
}

void Actor::forceMove(const Vec3d &_newPos)
{
    if (_newPos != getPos())
    {
        setPos(_newPos);
        onForceMove(); 
    }
}

void Actor::clear()
{
    // the test that verify if resource is already unloaded is done in unloadResources function
    unloadResources();

    if (isCleared())
    {
        ITF_ASSERT(!m_registeredEvents.size());
        ITF_ASSERT(!isActive()||hasDataError());
        return;
    }

    setActive(bfalse);    // mjanod: was after 'ClearComponents' but some component may need this info sooner
    m_registeredEvents.clear();

#ifdef ITF_WINDOWS
    ClearComponents(btrue,!ACTORSMANAGER->isActorCooking());
#else
    ClearComponents(btrue,btrue);
#endif //ITF_WINDOWS


    m_isFlipped = bfalse;
}

void Actor::AddComponent( ActorComponent* _actorComponent, bbool _udpateLists )
{
    ITF_ASSERT_MSG(_actorComponent, "Do not add a null component to an actor");
    if(_actorComponent == NULL)
        return;

    m_components.push_back(_actorComponent);
    _actorComponent->SetActor(this);

    if (!_udpateLists)
        return;

    if ( _actorComponent->needsDraw() )
    {
        ITF_ASSERT(m_drawComponents.size() < m_components.size());
        ITF_ASSERT(m_drawComponents.find(_actorComponent) < 0);
        m_drawComponents.push_back(_actorComponent);
    }
    else if (_actorComponent->needsDraw2D())
    {
        ITF_ASSERT(m_draw2DComponents.size() < m_components.size());
        ITF_ASSERT(m_draw2DComponents.find(_actorComponent) < 0);
        m_draw2DComponents.push_back(_actorComponent);
    }

    if ( _actorComponent->needsUpdate() )
    {
        ITF_ASSERT_CRASH(m_updateComponents.size() < m_components.size(),"m_updateComponents.size() < m_components.size()");
        ITF_ASSERT_CRASH(m_updateComponents.find(_actorComponent) < 0,"m_updateComponents.find(_actorComponent) < 0");
        m_updateComponents.push_back(_actorComponent);
    }
}

ActorComponent* Actor::getComponentFromStaticClassCRC(StringID::StringIdValueType _staticClassCRC) const
{
     for ( u32 it = 0; it < m_components.size(); ++it )
     {
         ActorComponent* pComponent = m_components[it];
         ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

         if ( pComponent && pComponent->IsClassCRC(_staticClassCRC) )
         {
             return static_cast<ActorComponent*>(pComponent);
         }
     }

     return NULL;
}

void Actor::ClearComponents(bbool _clearMem,bbool _clearcomponents)
{
    if (_clearcomponents)
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* component = m_components[it];
            component->onActorClearComponents();
        }
    }

    if (_clearMem)
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* component = m_components[it];

            for ( u32 i = 0; i < m_registeredEvents.size(); i++ )
            {
                RegisteredEvent& registeredEvent = m_registeredEvents[i];
                u32 numListeners = registeredEvent.m_listeners.size();

                for ( u32 j = 0; j < numListeners; j++ )
                {
                    if ( registeredEvent.m_listeners[j] == component )
                    {
                        registeredEvent.m_listeners.eraseNoOrder(j);
                        break;
                    }
                }
            }

            SF_DEL(m_components[it]);
        }
    }
    m_components.clear();
    m_drawComponents.clear();
    m_draw2DComponents.clear();
    m_updateComponents.clear();
}

void Actor::setUpdateType( UpdateType _t, bbool _propagate/* = bfalse*/ )
{
    Super::setUpdateType(_t, _propagate);

    if(_propagate)
    {
        for(u32 i = 0; i < m_childrenHandler.getChildren().size(); ++i)
        {
            if(Actor* pChild = static_cast<Actor*>(m_childrenHandler.getChildren()[i].getObject()))
            {
                pChild->setUpdateType(_t, _propagate);
            }
        }
    }
}

void    Actor::unloadCells()
{
    resetCellUpdateInfo();
}

void Actor::updateComponentPosAngle( const Vec3d& _pos, f32 _angle, ActorComponent* _skip )
{
    setPos(_pos);
    setAngle(_angle);

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];

        ITF_ASSERT(pComponent);

        if ( pComponent != _skip ) 
        {
            pComponent->onUpdatePos();
        }
    }
}

#if defined ITF_WINDOWS && !defined (ITF_FINAL)
#define CHECK_SCENE() {const Scene* pScene = getScene();ITF_ASSERT_CRASH(pScene,"scene associated to actor can't be null %s",m_userFriendly.cStr());}
#else
#define CHECK_SCENE() {}
#endif //defined ITF_WINDOWS && !defined (ITF_FINAL)

void Actor::update( f32 _deltaTime)
{
    PROFILER_UPDATE_PICKABLE(this,getRef());
#ifdef ITF_SUPPORT_DEBUGFEATURE
    DebugInfo::stopOnRef(this);//set ITF::DebugInfo::m_breakOnRef to 0 to disable it
#endif  //ITF_SUPPORT_DEBUGFEATURE

    if ( m_lastUpdateFrame == CURRENTFRAME) // same frame without pause
    {
        return;
    }

#ifndef ITF_FINAL
    m_drawCount = 0;
#endif // ITF_FINAL

    CHECK_SCENE()

    ITF_ASSERT_MSG(isActive(),"Something is very wrong, update is being called without calling onBecomeActive");

    STATS_ACTOR_UPDATEINC

    m_lastUpdateFrame = CURRENTFRAME;

    startComputingNextAABB();
    resetNextAABB();

    if (hasDataError())
    {
        setPhysicalReady(btrue);
        return;
    }

#ifdef RASTER_ENABLED
    if (CONFIG->m_showActorPerf)
    {
        const Color &color = RastersManager::getColorFromName(getUserFriendly().cStr(), g_rasterColorRegistry);
        RASTER_CHANGE_NAME(actorUpdate, getUserFriendly().cStr(), color);
        TIMERASTER_START(actorUpdate);
    }
#endif // RASTER_ENABLED

    for ( u32 it = 0; it < m_updateComponents.size(); ++it )
    {
        ActorComponent* pActorComponent = m_updateComponents[it];
        if(!pActorComponent->isPaused()) 
        {
            if  (!m_updatePaused || pActorComponent->needsDraw())
            {
#ifdef RASTER_ENABLED
                if (CONFIG->m_showComponentPerf)
                {
                    char namebuff[512];
                    const char *name = namebuff;
                    if(CONFIG->m_showActorComponentPerf)
#ifdef ITF_ENABLE_RTTI_CRC_CHECK
                        sprintf(namebuff, "%s,%s", getUserFriendly().cStr(), pActorComponent->GetObjectClassName());
#else
                        sprintf(namebuff, "%s,%u", getUserFriendly().cStr(), pActorComponent->GetObjectClassCRC());
#endif // ITF_ENABLE_RTTI_CRC_CHECK
                    else
                        name = StringID(pActorComponent->GetObjectClassCRC()).getDebugString();
                    const Color &color = RastersManager::getColorFromName(name, g_rasterColorRegistry);
                    RASTER_CHANGE_NAME(componentUpdate, name, color);
                    TIMERASTER_START(componentUpdate);
                }
#endif // RASTER_ENABLED

                pActorComponent->Update(_deltaTime);

#ifdef RASTER_ENABLED
                if (CONFIG->m_showComponentPerf)
                {
                    TIMERASTER_STOP(componentUpdate);
                    FLUSH_DYNAMIC_RASTER(componentUpdate);
                }
#endif // RASTER_ENABLED
            }
        }
    }

#ifdef RASTER_ENABLED
    if (CONFIG->m_showActorPerf)
    {
        TIMERASTER_STOP(actorUpdate);
        FLUSH_DYNAMIC_RASTER_REF(actorUpdate, getRef());
    }
#endif // RASTER_ENABLED

    swapNextAABB();

    m_childrenHandler.update(_deltaTime);
}

void Actor::swapNextAABB()
{
    ITF_ASSERT(m_isComputingNextAABB);
    if (m_isComputingNextAABB)
    {        
        m_isComputingNextAABB = bfalse;
        changeRelativeAABB(m_nextRelativeAABB);
        updateAbsoluteAABBFromRelative();
    }
}
void Actor::draw()
{
    PROFILER_DRAW_PICKABLE(this,getRef());

    if (hasDataError() || m_drawDisabled)
        return;

    STATS_ACTOR_DRAWINC

    setLastVisibleFrame(CURRENTFRAME);

#ifndef ITF_FINAL
    m_drawCount++;
#endif // ITF_FINAL

    for ( u32 it = 0; it < m_drawComponents.size(); ++it )
    {
        if(!m_drawComponents[it]->isPaused()) 
            m_drawComponents[it]->Draw();
    }
}

void Actor::draw2D()
{

    PROFILER_DRAW_PICKABLE(this,getRef());

    if (hasDataError() || m_drawDisabled)
        return;

    STATS_ACTOR_DRAW2DINC

    setLastVisibleFrame(CURRENTFRAME);

    for ( u32 it = 0; it < m_draw2DComponents.size(); ++it )
    {
        if(!m_draw2DComponents[it]->isPaused()) 
            m_draw2DComponents[it]->Draw2D();
    }
}


bbool Actor::isUpdateAllowed() const
{
    if (CHEATMANAGER->getPause())
        return bfalse;
    return isEnabled();
}

void Actor::onSceneActive()
{
    Super::onSceneActive();

    if(!hasDataError())
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onSceneActive();
        }
    }

    registerInWorldCells();
}

void Actor::onSceneInactive()
{
    Super::onSceneInactive();

    if(!hasDataError())
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onSceneInactive();
        }
    }
}

void Actor::onBecomeActive()
{    
    if(isActive())
        return;

    Super::onBecomeActive();

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];

        ITF_ASSERT(pComponent);
        if(pComponent->isPaused() )
            continue;

        pComponent->onBecomeActive();
    }

    if (isCellObjectType() && isAllowedForCell())
    {
#ifdef ITF_WINDOWS
        if (CHEATMANAGER->getPause())
        {
#ifdef ITF_SUPPORT_EDITOR
            onEditorMove();        // AABB not updated when paused
            if (EDITOR)
                EDITOR->registerInWorldCellsDuringPause(getRef());
#endif // ITF_SUPPORT_EDITOR
        }
#endif // ITF_WINDOWS
    }
}

void Actor::onBecomeInactive()
{
    if(!isActive())
        return;

    Super::onBecomeInactive();

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        
        ITF_ASSERT(pComponent);
        if(pComponent->isPaused())
            continue;

        pComponent->onBecomeInactive();
    }
}

void Actor::disable()
{
    if(isActive())
    {
        m_lastUpdateFrame = CURRENTFRAME; // Trick to prevent another update when disabled from the update of another object

        if (is2DActor())
            getWorld()->addObjectToInactivate(getRef());
    }

    if (isEnabled())
    {
        getBinding()->unbindFromParent();
    }

    Super::disable();
}

void Actor::onForceMove()
{
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->onForceMove();
    }
}


f32 Actor::getRadius() const
{
    PhysComponent* physComp = GetComponent<PhysComponent>();

    if ( physComp )
    {
        return physComp->getRadius();
    }

    return 0.f;
}

Actor * Actor::createFromDataArchive(ArchiveMemory &_archive, Scene *_newScene, bbool _needLoadTemplateFromACT, bbool _needsRegistering)
{
    Actor* newActor = newAlloc(mId_Actor, Actor());
    newActor->setNeedsRegisteringSerializableData(_needsRegistering);
    CSerializerObjectBinary serializeBin(&_archive);
    newActor->Serialize(&serializeBin,ESerialize_Data_Load);
    newActor->regenerateObjectId();

    if (_newScene)
    {
        _newScene->registerPickable(newActor);
        _newScene->processRegisterPending();
    }
    return newActor;
}

Pickable * Actor::clone( Scene* _pDstScene, bbool _callOnLoaded ) const
{
    ITF_ASSERT_MSG(_pDstScene, "cloning actor inside an empy scene");

    Actor* newActor = newAlloc(mId_Actor, Actor());
    BaseObjectClone(const_cast<Actor*>(this), newActor);

    ITF_ASSERT(getObjectId() != newActor->getObjectId());

    if (_pDstScene)
    {
        _pDstScene->registerPickable(newActor);
        _pDstScene->processRegisterPending();

        Vec3d worldPos = getWorldInitialPos();
        Vec2d worldScale = getWorldInitialScale();
        f32 worldRot = getWorldInitialRot();
        newActor->setWorldInitialPos(worldPos);
        newActor->setWorldInitialRot(worldRot);
        newActor->setWorldInitialScale(worldScale);
        newActor->setPos(worldPos);
        newActor->setScale(worldScale);
        newActor->setAngle(worldRot);
    }

    if(_callOnLoaded)
        newActor->onLoaded();

    return newActor;
}

bbool   Actor::updatePhysicalReady() 
{
    if (hasDataError())
    {
        setPhysicalReady(btrue);
        return btrue;
    }

    ResourceGroup* grp = getResourceGroup();
    if ( grp )
    {
        grp->updatePhysicallyLoaded();
        if (!grp->isPhysicallyLoaded() )
        {
            setPhysicalReady(bfalse);
            return bfalse;
        }
    }

    
    if (!isPhysicalReady())
    {
        setPhysicalReady(btrue);
        onResourceLoaded();
    }

    return isPhysicalReady();
}

ResourceGroup* Actor::getResourceGroup()
{ 
    ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());

    if (grp)
        return grp;

    m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
    return (ResourceGroup*)(m_resourceGroup.getResource());
} 

ResourceID Actor::addResource( Resource::ResourceType _type, const Path& _path )
{
    return getResourceGroup()->addResource(_type,_path);
}

void Actor::removeResource(const ResourceID &_resource)
{
    ResourceGroup* grp = getResourceGroup();
    ITF_ASSERT(grp);
    grp->removeResource(_resource);
}

ResourceID Actor::addResourceInTemplate( Resource::ResourceType _type, const Path& _path )
{
    return getTemplateResourceGroup()->addResource(_type,_path);
}

void Actor::removeResourceInTemplate(const ResourceID &_resource)
{
    ResourceGroup* grp = getTemplateResourceGroup();
    ITF_ASSERT(grp);
    grp->removeResource(_resource);
}



void Actor::unloadResources()
{
    if(isRequestedLoading())
    {
        Super::unloadResources();
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onUnloadResources();
        }
    }
} 

void Actor::onResourceLoaded()
{
     ITF_ASSERT(isPhysicalReady());

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "NULL component !!!");
        pComponent->onResourceLoaded();
    }

    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];

        ITF_ASSERT_MSG(pComponent, "NULL component !!!");
        pComponent->onResourceReady();
    }

#ifndef ITF_FINAL
    if (!validate() && !hasDataError())
        setDataError("Actor validation failed");

    computeNbBones();//profile purpose
#endif

    
}

void Actor::onResourceLoadedCallback(void * _context)
{
    ObjectRef context{ u32(uPtr(_context)) };
    Actor * actor = static_cast<Actor *>(context.getObject());
    if (actor)
    {
        if (actor->hasDataError())
            return;
        actor->onResourceLoaded();
    }
}


void Actor::pauseComponent(StringID::StringIdValueType _crc)
{
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        if(pComponent->IsClassCRC(_crc))
        {
            pComponent->pause();
        }
    }
}

void Actor::unpauseComponent(StringID::StringIdValueType _crc)
{
    if(hasDataError())
        return;

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        if(pComponent->IsClassCRC(_crc))
        {
            pComponent->unpause();
        }
    } 
}

void Actor::pauseAllComponents()
{
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->pause();
    }
}

void Actor::unpauseAllComponents()
{
    if(hasDataError())
        return;

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->unpause();
    } 
}


void Actor::growRelativeAABBFromAbsolute(const AABB& _absoluteAABB)
{
    if (m_isComputingNextAABB)
        m_nextRelativeAABB.grow(AABB(_absoluteAABB.getMin()-get2DPos(), _absoluteAABB.getMax()-get2DPos()));
    else
        m_relativeAABB.grow(AABB(_absoluteAABB.getMin()-get2DPos(), _absoluteAABB.getMax()-get2DPos()));
}

void Actor::growRelativeAABB(const AABB& _aabb )
{
    if (m_isComputingNextAABB)
        m_nextRelativeAABB.grow(_aabb );
    else
        m_relativeAABB.grow(_aabb );
    ITF_ASSERT(m_absoluteAABB.isValid());
}

Vec3d Actor::getBase() const
{
    Vec2d HorizAxis;
    f32_CosSin(getAngle(), &HorizAxis.m_x, &HorizAxis.m_y);
    Vec2d vertAxis = HorizAxis.getPerpendicular();
    Vec2d contact = Vec2d(getPos().m_x, getPos().m_y);

    contact -= vertAxis * getRadius();
    return Vec3d(contact.m_x, contact.m_y, getDepth());
}

void Actor::registerEvent( StringID::StringIdValueType _eventName, IEventListener* _listener )
{
    ObjectFactory* eventFactory = ACTORSMANAGER->getEventFactory();

    const ObjectFactory::ClassInfo* eventClass = eventFactory->GetClassInfo(_eventName);
    ITF_ASSERT_MSG(eventClass, "Events must be registered in the event factory!");
    if (!eventClass)
    {
        return;
    }

    RegisteredListenerArray* registeredListeners(NULL);

    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); )
    {
        RegisteredEvent& registeredEvent = *it;
        StringID::StringIdValueType registerEventName = registeredEvent.m_eventClass->m_crc;

        if ( _eventName == registerEventName )
        {
            // this event has already been registered
            registeredListeners = &registeredEvent.m_listeners;
        }
        else if ( eventClass->IsClass(registerEventName) )
        {
            // a parent event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            if ( components.find(_listener) != -1 )
            {
                // ... by this component: we're covered.
                return;
            }
        }
        else if ( registeredEvent.m_eventClass->IsClass(_eventName) )
        {
            // a child event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            i32 index = components.find(_listener);
            if ( index != -1 )
            {
                // ... by this component: remove it (it'll be replaced with this one)
                components.eraseNoOrder(index);

                if (components.size() == 0)
                {
                    it = m_registeredEvents.erase(it);
                    continue;   // erase returns the next element or end
                }
            }
        }

        it++;
    }


    if ( !registeredListeners )
    {
        // this event has never been registered, add an entry
#ifdef ITF_WII
        MEM_M_PushExt(MEM_C_MEM1_ALLOC);
#endif
        m_registeredEvents.push_back(RegisteredEvent());
#ifdef ITF_WII
        MEM_M_PopExt();
#endif
        RegisteredEvent& registeredEvent = m_registeredEvents.back();
        registeredEvent.m_eventClass = eventClass;
        registeredListeners = &registeredEvent.m_listeners;
    }

    // add our component if not already present
    if ( registeredListeners->find(_listener) == -1 )
    {
        registeredListeners->push_back(_listener);
    }
}

void Actor::unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener )
{
    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++)
    {
        RegisteredEvent& registeredEvent = *it;
        
        if ( registeredEvent.m_eventClass->IsClass(_crc) )
        {
            // a child event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            i32 index = components.find(_listener);
            if ( index != -1 )
            {
                // ... by this component: remove it
                components.eraseNoOrder(index);

                if (components.size() == 0)
                {
                    it = m_registeredEvents.erase(it);
                }
            }
            break;
        }
    }
}

bbool Actor::isEventRegistered( StringID::StringIdValueType _crc )
{
    ObjectFactory* eventFactory = ACTORSMANAGER->getEventFactory();

    const ObjectFactory::ClassInfo* eventClass = eventFactory->GetClassInfo(_crc);
    ITF_ASSERT_MSG(eventClass, "Events must be registered in the event factory!");
    if (!eventClass)
    {
        return bfalse;
    }

    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++ )
    {
        RegisteredEvent& registeredEvent = *it;

        if ( registeredEvent.m_eventClass->m_crc == _crc )
        {
            return btrue;
        }
    }
    return bfalse;
}

void Actor::onEvent(Event* _event)
{
    STATS_ONACTOREVENTINC
    //Super::onEvent(_event); pickable::onEvent is empty

    if ( EventTeleport* pTeleportEvent = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)) )
    {
        if (pTeleportEvent->getApplyPosAndAngle())
        {
            setPos(pTeleportEvent->getPos());
            setAngle(pTeleportEvent->getAngle());
            getBinding()->unbindFromParent();

            if ( pTeleportEvent->getReset() )
            {
                onForceMove();
            }
#if !defined(ITF_DISABLE_WARNING)
            // Check for prefetch issues
            if(GAMEMANAGER->isPlayerActor(getRef()))
            {
                if(Actor* pSender = static_cast<Actor*>(pTeleportEvent->getSender().getObject()))
                {
                    World* pWorld = pSender->getWorld();
                    static const Vec2d tolerance(5.0f, 5.0f);
                    const AABB aabb(pTeleportEvent->getPos().truncateTo2D() - tolerance, pTeleportEvent->getPos().truncateTo2D() + tolerance);

                    ITF_WARNING_CATEGORY(LD, pSender, pWorld->isPhysicalReady(aabb), "Popping may occurs at destination, you should use prefetch mecanisms: AABB prefetch");
                }
            }
#endif // ITF_DISABLE_WARNING
        }
    }
    else if ( EventTrigger* pTriggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        if(pTriggerEvent->getActivated())
            setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    }
    else if ( EventPause* onPause = _event->DynamicCast<EventPause>(ITF_GET_STRINGID_CRC(EventPause,829919267)) )
    {
        if (onPause->getPause())
            disable();
        else
            enable();
    }

    u32 registeredEventsSize = m_registeredEvents.size();

    if ( registeredEventsSize )
    {
        RegisteredEvent* pregisteredEvent = &m_registeredEvents[0];

        for ( u32 i = 0; i < registeredEventsSize; i++, pregisteredEvent++ )
        {
            if ( _event->IsClassCRC(pregisteredEvent->m_eventClass->m_crc) )
            {
                const u32 listenersSize = pregisteredEvent->m_listeners.size();
                if(listenersSize)
                {
                    ITF_ASSERT(pregisteredEvent);
                    IEventListener **ppListener = &pregisteredEvent->m_listeners[0];

                    for ( u32 j = 0; j < listenersSize; j++, ppListener++ )
                    {
                        (*ppListener)->onEvent(_event);
                    }

                    // don't break because components could have registered at different inheritance levels
                }
            }
        }
    }
}

void Actor::saveCheckpointData()
{
    if ( !hasDataError() )
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);
        if (dataContainer)
        {
            SF_DEL(dataContainer->m_checkpoint);
            dataContainer->m_checkpoint = newAlloc(mId_SavegameManager,ArchiveMemory);
            CSerializerObjectBinary serializeBin(dataContainer->m_checkpoint);
            Serialize(&serializeBin,ESerialize_Checkpoint_Save);
        }
    }
}


void Actor::postLoadCheckpointData()
{
    if ( !hasDataError() )
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);
        if (dataContainer && dataContainer->m_checkpoint)
        {
            dataContainer->m_checkpoint->rewindForReading();
            CSerializerObjectBinary serializeBin(dataContainer->m_checkpoint);
            Serialize(&serializeBin,ESerialize_Checkpoint_Load);
        }
        onLoadedCheckpoint(); // call onLoadedCheckpoint() outside of the if() statement for newly created objects
    }

    RESOURCE_MANAGER->flushPendingOps();
}

void Actor::onLoadedCheckpoint()
{
    if (!isStartPaused())
    {
        enable();
    }

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->onCheckpointLoaded();
    }
}

void Actor::fillHotReloadData(ActorHotReload &_reloadInfo, bbool _skipInstanceData)
{
    _reloadInfo.m_actorRef          = getRef();
    _reloadInfo.m_enabled           = isEnabled();
    _reloadInfo.m_resLoaded         = isRequestedLoading();
    _reloadInfo.m_skipInstanceData  = _skipInstanceData;
#ifndef ITF_FINAL
    _reloadInfo.m_actorWas2DBeforeHotReload = is2DActor();
#endif
}

void Actor::oneStepHotReload(bbool _skipInstanceData)
{
    ActorHotReload hotreloadInfo;
    fillHotReloadDataAndHotReload(hotreloadInfo, _skipInstanceData, bfalse); // bfalse : don't use 2 step hot reload
    notifyParentsAndChildren(ChangeType_Reloaded);
}

void Actor::fillHotReloadDataAndHotReload( ActorHotReload& _hotreloadInfo, bbool _skipInstanceData, bbool _delayPostreload)
{
    fillHotReloadData(_hotreloadInfo, _skipInstanceData);

    hotReload(_hotreloadInfo, _delayPostreload);
}

void Actor::hotReload( ActorHotReload& _reloadInfo, bbool _useTwoStepHotreload  )
{
    RESOURCE_MANAGER->setHotReloading(btrue);

    ITF_ASSERT(_reloadInfo.m_actorRef == getRef());

    clear();
#if !defined(ITF_FINAL)
    WarningManager::clearWarningsForObject(this);
#endif

    _reloadInfo.m_childrenBeforeClear = m_childrenHandler.getChildren();


#ifdef  ITF_SUPPORT_EDITOR
    resetResourceError();
#endif //ITF_SUPPORT_EDITOR

    resetDataError();

    if ( !isSpawned() && isSerializable() && !_reloadInfo.m_skipInstanceData )
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);

        if ( dataContainer )
        {
            // Load data
            ArchiveMemory* arc = dataContainer->m_instance;

            // Load the instance data
            ITF_ASSERT(arc);
            arc->rewindForReading();
            CSerializerObjectBinary instanceSerializer(arc);
            Serialize(&instanceSerializer,ESerialize_Data_Load);
        }
    }

    RESOURCE_MANAGER->flushPendingOps();
    setLastActiveFrame(0);

    if (!_useTwoStepHotreload)
        postHotReload( _reloadInfo, HotReloadType_Default );

    RESOURCE_MANAGER->setHotReloading(bfalse);
}

void Actor::postHotReload( const ActorHotReload& _reloadInfo, HotReloadType _hotReloadType )
{
    onLoaded(_hotReloadType);
    updatePhysicalReady();

    if ( _reloadInfo.m_resLoaded )  // ! Otherwise, AlwaysActive objects will have their resources unloaded after F5
    {
        loadResources();
    }

//    if (HotReloadType_Checkpoint != _hotReloadType)
    {
        if ( _reloadInfo.m_enabled )
        {
            enable();
        }
    }

#ifndef ITF_FINAL  
    if (HotReloadType_Default == _hotReloadType)
    {
        if (is2DActor())
        {
            if (!_reloadInfo.m_actorWas2DBeforeHotReload)
            {
                Vec3d pos2d; 
                GFX_ADAPTER->compute3DTo2D(getPos(), pos2d);
                pos2d.m_z = 0.f;
                setPos(pos2d);
            }
        }
        else
        {
            if (_reloadInfo.m_actorWas2DBeforeHotReload)
            {
                Vec3d curPos, pos3d;
                curPos = getPos();
                curPos.m_z = 0.f;
                GFX_ADAPTER->compute2DTo3D(curPos, pos3d);
                setPos(pos3d);
            }
        }
    }
#endif
}


void Actor::hotReloadFromMem( ArchiveMemory* _archive )
{
    ObjectRefList childrenBeforeClear = m_childrenHandler.getChildren();
    clearBinds(bfalse); // because objbinding is cleared we need to register again all binds after onLoaded (parent and children)

    Super::hotReloadFromMem(_archive);
    
    SafeArray<BaseObject*> resolvedRefs;
    ID_SERVER->getObjectListNoNull(childrenBeforeClear, resolvedRefs);
    for(u32 i = 0; i < resolvedRefs.size(); ++i)
        m_childrenHandler.addChild(static_cast<Actor*>(resolvedRefs[i]));
}

void Actor::onAddedToScene( Scene* _pScene )
{
    Super::onAddedToScene(_pScene);

    if ( needsRegisteringSerializableData() )
    {
        ACTORSMANAGER->registerSerializeDataActor(this);
    }

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->onActorAddedToScene(_pScene);
    }
}

void Actor::onRemovedFromScene( Scene* _pScene ,bbool _forDeletion)
{

    //notify components
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);

        pComponent->onActorRemovedFromScene(_pScene, _forDeletion);
    }

    //unregister the actor serializable data
    if ( ACTORSMANAGER && needsRegisteringSerializableData() )
    {
        ACTORSMANAGER->unregisterSerializeDataActor(this);
    }

    //unregister from scene
    Super::onRemovedFromScene(_pScene,_forDeletion);
}

void Actor::replaceReinitDataByCurrent()
{
    if ( !needsRegisteringSerializableData() )
    {
        return;
    }

    if(ACTORSMANAGER->isActorRegistered(this))
        ACTORSMANAGER->unregisterSerializeDataActor(this);
    else
    {
        ITF_ASSERT_MSG(!isSerializable(), "reiniting an unregistered but serializable actor!");
    }

    ACTORSMANAGER->registerSerializeDataActor(this);

    m_childrenHandler.replaceReinitDataByCurrent();
}

void Actor::regenerateObjectId()
{
    Super::regenerateObjectId();

    // Apply regenerate for component that include BadeObjects
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT(pComponent);
        pComponent->regenerateObjectId();
    }
}

void Actor::setDataError(const String& _str)
{
    Pickable::setDataError(_str);

    growRelativeAABB(AABB(-Vec2d::One, Vec2d::One));
    ObjBinding* pBinding = getBinding();
    if(pBinding)
    {
        pBinding->unbindAllChildren();
        pBinding->unbindFromParent();
    }
}

#ifndef ITF_FINAL
bbool Actor::validate()
{
    bbool wasInvalid = hasDataError();
    resetDataError();
    bbool isValid = btrue;
    String errorMsg;

#ifdef  ITF_SUPPORT_EDITOR
    if ( hasResourceError() )
    {
        errorMsg = getResourceErrorLabel();
        isValid = bfalse;
    }
#endif //ITF_SUPPORT_EDITOR

    if (!m_templateActor)
    {
        String8 error;
        error.setTextFormat("actor with missing template %s", getLua().getString8().cStr());
        ITF_WARNING_CATEGORY(GPP,this, bfalse, error.cStr());

        errorMsg += error;
        isValid = bfalse;
    }
    else
    {
        const u32 uSize = m_components.size();
        if (uSize==0)
        {
            String8 error;
            error.setTextFormat("Actor has no component : '%s'", getLua().getString8().cStr());
            ITF_WARNING_CATEGORY(GPP,this, bfalse, error.cStr());

            errorMsg += error;
            isValid = bfalse;
        }

        bbool subValidation = btrue;
        String errorLabel("invalid component(s):");
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];

            if(!pComponent)
            {
                ITF_WARNING_CATEGORY(GPP,this, pComponent!=NULL, "Actor has a NULL component : '%s'", getLua().getString8().cStr());
                continue;
            }
            bbool isComponentValid = btrue;

            pComponent->validate(isComponentValid);

            if (!isComponentValid && isValid)
            {
                errorLabel += "\n";
                errorLabel += StringID(pComponent->GetObjectClassCRC()).getDebugString();
                subValidation = bfalse;
            }
        }

        if(!subValidation)
        {
            errorMsg += errorLabel;
            isValid = bfalse;
        }
    }

    if( wasInvalid && isValid )
    {
        unpauseAllComponents();
    }
    else if ( !isValid )
    {
        setDataError(errorMsg);
        pauseAllComponents();
    }

    return isValid;
}
#endif //!ITF_FINAL

Actor* ActorRef::getActor() const
{
    BaseObject* pObj = getObject();
    if (pObj && pObj->getObjectType() == BaseObject::eActor)
        return (Actor*)pObj;
    else 
        return NULL;
}

void ActorRef::setActor( const Actor* _actor )
{
    setValue(_actor->getRef().getValue());
#ifdef GPP_DEBUG
    m_pointer = _actor;
#endif // GPP_DEBUG
}

ActorRef & ActorRef::operator =(const ObjectRef & _other)
{
    setValue(_other.getValue());
#ifdef GPP_DEBUG
    m_pointer = getActor();
#endif // GPP_DEBUG
    return *this;
}

ActorRef::ActorRef(const ObjectRef & _other)
{
    setValue(_other.getValue());
#ifdef GPP_DEBUG
    m_pointer = getActor();
#endif // GPP_DEBUG
}

void Actor::createParentBind( bbool _temporaryBind,
                              Actor* _parent, 
                              ActorBind::Type _type, 
                              u32 _typeData, 
                              bbool _useParentFlip /*=btrue*/, 
                              bbool _useParentScale/*=bfalse*/, 
                              bbool _removeWithParent /*=bfalse*/ )
{
    ITF_ASSERT( _parent != NULL );
    if ( _parent == NULL )
    {
        return;
    }

    ActorBind* pBind = newAlloc(mId_Editor, ActorBind);

    pBind->m_temporaryBind      = _temporaryBind;
    pBind->m_type               = _type;
    pBind->m_typeData           = _typeData;
    pBind->m_useParentFlip      = _useParentFlip;
    pBind->m_useParentScale     = _useParentScale;
    pBind->m_removeWithParent   = _removeWithParent;

    Vec3d localPos;
    f32 localAngle;
    if( _parent->getChildrenBindHandler().computeLocalCoordinates(pBind, getPos(), getAngle(), localPos, localAngle))
    {
        pBind->setInitialAngleOffset(localAngle);
        pBind->setInitialPositionOffset(localPos);

        SceneObjectPathUtils::getRelativePathFromObject(this, _parent, pBind->m_parent);

        if ( !pBind->m_parent.isValid() )
        {
            SceneObjectPathUtils::getAbsolutePathFromObject( _parent, pBind->m_parent );
        }

        setParentBind(pBind);
        _parent->getChildrenBindHandler().updateWorldCoordinates(this, pBind);
        storeCurrentToInitial();
    }
    else
    {
        ITF_ASSERT(0);

        SF_DEL(pBind);
    }
}

void Actor::setParentBind( ActorBind* _newParent )
{
    unbindFromParent(btrue);

    m_pParentBind = _newParent;

    if(_newParent)
    {
        Pickable* pPickable = NULL;
        const ObjectPath& parentPath = _newParent->m_parent;
        if(parentPath.getIsAbsolute())
        {
            pPickable = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
        }
        else
        {
            pPickable = SceneObjectPathUtils::getObjectFromRelativePath(this, parentPath);
        }

        ITF_ASSERT_CRASH( pPickable != NULL , "Can't find parent object !");

        if ( pPickable != NULL )
        {
            if ( Actor* pParent = pPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                //if(Actor* pParent = static_cast<Actor*>(SceneObjectPathUtils::getObjectFromRelativePath(this, _newParent->m_parent)))
            {
                ITF_ASSERT_CRASH(pParent != this , "Binding to 'this' !!!");
                pParent->getChildrenBindHandler().addChild(this);
            }
        }
    }
}

void Actor::unbindFromParent(bbool _changeInitialBind /*= bfalse*/)
{
    // Unbind from parent
    if(m_pParentBind)
    {
        if(Actor* pParent = static_cast<Actor*>(m_pParentBind->m_runtimeParent.getObject()))
        {
            pParent->getChildrenBindHandler().removeChild(this);
        }
    }

    if(_changeInitialBind)
    {
        SF_DEL(m_pParentBind);
    }
}

void Actor::clearBinds(bbool _clearInitial)
{
    unbindFromParent(_clearInitial);

    // Unbind childs from "this"
    {
        SafeArray<Actor*> childToUnregister;
        const u32 uChildCount = m_childrenHandler.getChildren().size();
        for(u32 iChild = 0; iChild < uChildCount; ++iChild)
        {
            if(Actor* pChild = static_cast<Actor*>(m_childrenHandler.getChildren()[iChild].getObject()))
            {
                childToUnregister.push_back(pChild);
            }
        }

        const u32 uExistingChildCount = childToUnregister.size();
        for(u32 iChild = 0; iChild < uExistingChildCount; ++iChild)
        {
            getChildrenBindHandler().removeChild(childToUnregister[iChild]);
        }

        m_childrenHandler.clear();
    }
}

void Actor::updateWorldCoordinates()
{
    ITF_ASSERT_CRASH(m_pParentBind, "you shouldn't updateWorldCoordinates if you don't have a parent!");

    Actor* parent = getParent().getActor();
    ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
    if ( parent )
    {
        parent->getChildrenBindHandler().updateWorldCoordinates(this, m_pParentBind);
    }
}

const Vec2d& Actor::getLocalInitialScale() const
{
    if ( m_templateActor && m_templateActor->getScaleForced() != Vec2d::Zero )
    {
        return m_templateActor->getScaleForced();
    }
    else
    {
        return Super::getLocalInitialScale();
    }
}

f32 Actor::getLocalInitialZ() const
{
    if ( m_templateActor && m_templateActor->isZForced() )
    {
        return m_templateActor->getForcedZ();
    }
    else
    {
        return Super::getLocalInitialZ();
    }
}

void Actor::setScale( const Vec2d & _scale )
{
    Vec2d scaleToApply = _scale;
    if(const Actor_Template* pTemplate = getTemplate())
    {
        const Vec2d& scaleMin = pTemplate->getScaleMin();
        if(scaleMin.m_x != 0.0f)
            scaleToApply.m_x = std::max(scaleToApply.m_x, scaleMin.m_x);
        if(scaleMin.m_y != 0.0f)
            scaleToApply.m_y = std::max(scaleToApply.m_y, scaleMin.m_y);

        const Vec2d& scaleMax = pTemplate->getScaleMax();
        if(scaleMax.m_x != 0.0f)
            scaleToApply.m_x = std::min(scaleToApply.m_x, scaleMax.m_x);
        if(scaleMax.m_y != 0.0f)
            scaleToApply.m_y = std::min(scaleToApply.m_y, scaleMax.m_y);
    }

    Super::setScale(scaleToApply);
}

void Actor::setDepth( f32 _depth )
{
    if(m_templateActor)
    {
        const f32 prevDepth = getDepth();

        if ( prevDepth != _depth )
        {
            Super::setDepth(_depth);

            const u32 numComponents = m_components.size();

            for ( u32 i = 0; i < numComponents; i++ )
            {
                m_components[i]->onDepthChanged(prevDepth,_depth);
            }
        }
    }
}

Vec3d Actor::getBoundWorldInitialPos() const
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d worldPos;
            f32 worldAngle;
            if(parent->getChildrenBindHandler().computeWorldCoordinates(
                m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                worldPos, worldAngle))
                return worldPos;
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute world initial position");
            }
        }
    }
    else
    {
        // same as Pickable::getWorldInitialPos except we use the scene's currentPos instead of initialPos
        Scene* myScene = getScene();
        Vec2d myPos = getLocalInitialPos().truncateTo2D();
        f32 myZ = getLocalInitialZ();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                Vec3d subScenePos = subActor->getPos();
                f32 subSceneRot = subActor->getAngle();

                myPos = myPos.Rotate(subSceneRot) + subScenePos.truncateTo2D();
                myZ += subScenePos.m_z;
            }
        }

        return myPos.to3d(myZ);
    }

    return getWorldInitialPos();
}

void Actor::setBoundWorldInitialPos( const Vec3d& _pos )
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, _pos, getAngle(), localPos, localAngle))
            {
                m_pParentBind->setInitialPositionOffset(localPos);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial position");
            }
        }
    }
    else
    {
        setWorldInitialPos(_pos);
    }
}


void Actor::setBoundWorldPos( const Vec3d& _pos )
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, _pos, getAngle(), localPos, localAngle))
            {
                m_pParentBind->setPosOffset(localPos);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial position");
            }
        }
    }
    else
    {
        setPos(_pos);
    }
}
f32 Actor::getBoundWorldInitialAngle() const
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d worldPos;
            f32 worldAngle;
            parent->getChildrenBindHandler().computeWorldCoordinates(
                m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                worldPos, worldAngle);
            return worldAngle;
        }
    }

    // same as Pickable::getWorldInitialRot except we use the scene's currentPos instead of initialPos
    Scene* myScene = getScene();
    f32 myRot = getLocalInitialRot();

    if ( myScene )
    {
        SubSceneActor* subActor = myScene->getSubSceneActorHolder();

        if ( subActor )
        {
            myRot += subActor->getAngle();
        }
    }

    return myRot;
}

Vec3d Actor::getBoundLocalInitialPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getInitialPosOffset();
    }

    return getLocalInitialPos();
}


const Vec2d& Actor::getBoundLocalInitial2DPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getInitialPosOffset().truncateTo2D();
    }

    return m_initialPos;
}

Vec3d Actor::getBoundLocalPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getPosOffset();
    }

    return getLocalPos();
}

Vec2d Actor::getBoundLocal2DPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getPosOffset().truncateTo2D();
    }

    return getLocal2DPos();
}

void Actor::setBoundLocalPos( const Vec3d& _pos )
{
    if ( m_pParentBind )
    {
        m_pParentBind->setPosOffset(_pos);
        updateWorldCoordinates();
    }
    else
    {
        setLocalPos(_pos);
    }
}

void Actor::setBoundLocal2DPos( const Vec2d& _pos )
{
    if ( m_pParentBind )
    {
        m_pParentBind->set2DPosOffset(_pos);
        updateWorldCoordinates();
    }
    else
    {
        setLocal2DPos(_pos);
    }
}

f32 Actor::getBoundLocalInitialAngle() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getInitialAngleOffset();
    }

    return m_initialRot;
}

f32 Actor::getBoundLocalAngle() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getAngleOffset();
    }

    return getLocalAngle();
}

void Actor::setBoundWorldInitialAngle(f32 _angle)
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, getPos(), _angle, localPos, localAngle))
            {
                m_pParentBind->setInitialAngleOffset(localAngle);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial angle");
            }
        }
    }
    else
    {
        setWorldInitialRot(_angle);
    }
}


void Actor::setBoundWorldAngle(f32 _angle)
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, getPos(), _angle, localPos, localAngle))
            {
                m_pParentBind->setAngleOffset(localAngle);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial angle");
            }
        }
    }
    else
    {
        setAngle(_angle);
    }
}

void Actor::setBoundLocalAngle( f32 _angle )
{
    if ( m_pParentBind )
    {
        m_pParentBind->setAngleOffset(_angle);
        updateWorldCoordinates();
    }
    else
    {
        setLocalAngle(_angle);
    }
}

const Path* Actor::getTemplatePath() const
{
    const Actor_Template* actTemplate = getTemplate();
    if (!actTemplate)
        return NULL;
    return &actTemplate->getFile();
}

bbool Actor::isZForced() const
{
    if(getTemplate() && getTemplate()->isZForced())
        return btrue;
    return bfalse;
}


void Actor::getLocalToWorldTransform( Vec3d& _parentPos, f32& _parentAngle ) const
{
    if ( m_pParentBind )
    {
        const Actor* parent = getParent().getActor();
        ITF_ASSERT(parent);
        if (parent)
        {
            parent->getChildrenBindHandler().getTransform(m_pParentBind, _parentPos, _parentAngle);
            return;
        }
    }
    else
    {
        // TODO: move to pickable?
        if ( const Scene* scene = getScene() )
        {
            if ( const Actor* parent = scene->getSubSceneActorHolder() )
            {
                _parentPos = parent->getPos();
                _parentAngle = parent->getAngle();
                return;
            }
        }
    }

    _parentPos = Vec3d::Zero;
    _parentAngle = 0.f;
}

Vec2d Actor::transformToWorld( const Vec2d& _localPos ) const
{
    Vec3d parentPos;
    f32 parentAngle;
    getLocalToWorldTransform(parentPos, parentAngle);

    return parentPos.truncateTo2D() + _localPos.Rotate(parentAngle);
}

Vec3d Actor::transformToWorld( const Vec3d& _localPos ) const
{
    Vec3d parentPos;
    f32 parentAngle;
    getLocalToWorldTransform(parentPos, parentAngle);

    Vec3d worldPos = _localPos;
    Vec3d_Rotate(&worldPos, parentAngle);
    worldPos += parentPos;
    return worldPos;
}

    void Actor::addObjectInGroupForDraw()
    {
        if (is2DActor())
            return;
        Super::addObjectInGroupForDraw();
    }

    u32 Actor::addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent /*= bfalse*/)
    {
        ITF_ASSERT(is2DActor() || !WorldManager::is2DPass());

        u32 myGroup = getObjectGroupIndex();

        if (WorldManager::is2DPass() != is2DActor())
            return myGroup;

        if (!getBinding()->isBound() || isParentBindRunTimeDisabled() || _fromParent) // bound objects can't register themselves, unless from thei parent querry
        {
            ITF_ASSERT( !isParentBindRunTimeDisabled() || !_fromParent ); //check that if parent bind is runtime disabled, my parent is not trying to push me in list
            u32 myGroup = Super::addObjectInGroupForUpdate(_listToFill, _fromParent);
            if (m_binding.hasChildren())
            {
                u32 childGroup = m_binding.addChildrenInGroupForUpdate(_listToFill);
                if (childGroup < myGroup)
                {
                    ITF_ASSERT_MSG(bfalse,"not an issue, just wondering when this case happens, just let Fred know");
                    _listToFill[myGroup].removeElement(this);
                    myGroup = childGroup;
                    setObjectGroupIndex(myGroup);
                    Super::addObjectInGroupForUpdate(_listToFill, _fromParent);
                }
            }
        }
        return myGroup;
    }
    
    void Actor::registerContainedPickables(SafeArray<ActorRef>& _actorList, SafeArray<ObjectRef>& _frizeList) const
    {
        if (!isDestructionRequested())
            _actorList.push_back(getRef());
    }

    void Actor::deleteSpawnedActors()
    {
        if (isSpawned() && !isDestructionRequested())
            requestDestruction();
    }

    void Actor::onChildChanged (ObjectRef _changedObject, PickableEditorChangeType _type)
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* pActorComponent = m_components[it];
            pActorComponent->onChildChanged(_changedObject, _type);
        }
    }
    
    void Actor::onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type)
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* pActorComponent = m_components[it];
            pActorComponent->onParentChanged(_changedObject, _type);
        }
    }

    void Actor::notifyParentsAndChildren(PickableEditorChangeType _type )
    {
        // notify parent
        if(m_pParentBind)
        {
            Pickable* pParent = NULL;
            const ObjectPath& parentPath = m_pParentBind->m_parent;
            if(parentPath.getIsAbsolute())
                pParent = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
            else
                pParent = SceneObjectPathUtils::getObjectFromRelativePath(this, parentPath);

            if(pParent)
            {
                ITF_ASSERT_CRASH(pParent->getObjectType() == BaseObject::eActor, "bad type");
                Actor* pActorParent = (Actor*)pParent;
                pActorParent->onChildChanged(getRef(), _type);
            }
        }

        // notify children
        ObjectRefList childrenList = m_childrenHandler.getChildren();
        for( u32 i = 0; i < childrenList.size(); i++)
        {
            BaseObject* pObj = childrenList[i].getObject();
            if (pObj && pObj->getObjectType() == BaseObject::eActor)
            {
                ((Actor*)pObj)->onParentChanged(getRef(), _type);
            }
        }

        // notify linked children
        LinkComponent* pLink = GetComponent<LinkComponent>();
        if (pLink)
        {
            LinkComponent::ChildrenList list = pLink->getMutableChildren();
            for (u32 i = 0; i < list.size(); i++)
            {
                Pickable* pPick = SceneObjectPathUtils::getObjectFromAbsolutePath(list[i].getPath());
                if (pPick)
                {
                    pPick->onParentChanged(getRef(), _type);
                }
            }
        }
    }

#ifndef ITF_FINAL
    u32 Actor::computeNbBones() const
    {
        u32 Bones = U32_INVALID;
        AnimLightComponent* pAnimLightComponent =  GetComponent<AnimLightComponent>();
        if (pAnimLightComponent!=0)
        {
            Bones = pAnimLightComponent->getBoneCount();
        }

        if (Bones !=U32_INVALID)
            m_nbBones = Bones;
        else
            m_nbBones = 0;

        return m_nbBones;
    }
#endif //ITF_FINAL


    #ifdef ITF_SUPPORT_EDITOR
    void Actor::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags)
    {
        if (!updatePhysicalReady() || hasDataError())
            return;

        u32 numComponents = m_components.size();

        for ( u32 it = 0; it < numComponents; ++it )
        {
            ActorComponent* component = m_components[it];

            if(!component->isPaused())
            {
                // Do not draw if it's paused, the validation process may not have passed
                component->drawEdit(drawInterface, _flags);
            }
        }
    }

    void Actor::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (!updatePhysicalReady() || hasDataError())
            return;

        /*if(m_lastUpdateFrame >= CURRENTFRAME)
            return;*/

        u32 numComponents = m_components.size();

        for ( u32 it = 0; it < numComponents; ++it )
        {
            ActorComponent* component = m_components[it];

            if(!component->isPaused())
            {
                // Do not draw if it's paused, the validation process may not have passed
                component->onEditorMove(_modifyInitialPos);
            }
        }
        
        getChildrenBindHandler().onEditorMove(_modifyInitialPos);
        m_binding.onEditorMove(_modifyInitialPos);

        m_lastUpdateFrame = CURRENTFRAME; // avoids crashes when updating twice in the same frame
    }

    void Actor::onEditorCreated( Pickable* _original /*= NULL*/ )
    {
        Actor* original = (Actor*)_original;

        u32 numComponents = m_components.size();
        for ( u32 it = 0; it < numComponents; ++it )
        {
            ActorComponent* component = m_components[it];
            component->onEditorCreated(original);
        }
    }
#endif // ITF_SUPPORT_EDITOR

} //namespace ITF

