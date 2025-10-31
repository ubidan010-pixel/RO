#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_LINK_PLUGIN_H_
#include "rayman/ray_editor/plugins/Link_Plugin.h"
#endif //_ITF_LINK_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    Link_Plugin* Link_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* Link_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, Link_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, LinkComponent::GetClassCRCStatic());
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->addListener(s_instance, 0);
        return s_instance;
    }

    void Link_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    Link_Plugin::Link_Plugin()
        : m_linkingFromShortcut(bfalse)
        , m_linkingFromContextMenu(bfalse)
    {
    }

    Link_Plugin::~Link_Plugin()
    {
    }


    bbool Link_Plugin::onDeletePickingShape(PickingShape* _shape)
    {
        if (_shape->isSubAnchor())
        {
            return bfalse;
        }

        if ( ObjectRefList* parents = LINKMANAGER->getParents(_shape->getOwner()) )
        {
            for (u32 i = 0; i < parents->size(); i++)
            {
                if ( BaseObject* parentObj = (*parents)[i].getObject() )
                {
                    if ( Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( LinkComponent* link = parent->GetComponent<LinkComponent>() )
                        {
                            ObjectPath relPath;

                            SceneObjectPathUtils::getRelativePathFromObject(parent,(const Pickable*)_shape->getOwnerPtr(),relPath);

                            if ( relPath.isValid() )
                            {
                                link->removeChild(relPath, bfalse);
                            }
                        }
                    }
                }
            }
        }

        LINKMANAGER->removeActor(_shape->getOwner());

        return bfalse;
    }

    void Link_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        ContextMenuItem* linkMenu = _menu.addItemFromExternalPlugin(Link_Menu, "Links");
        String menuText;

        if ( EDITOR->getSelectionSize() == 1 )
        {
            if ( LinkComponent* linkComponent = _actor->GetComponent<LinkComponent>() )
            {
                linkMenu->m_subMenu.addItemFromExternalPlugin(Link_StartCreate, "Start link edition (alt)");
                linkMenu->m_subMenu.addItemFromExternalPlugin(Link_ClearChildren, "Clear children links");

                LinkComponent::ChildrenList deadChildren;
                getDeadLinks(linkComponent, deadChildren);
                menuText.setTextFormat("Clear dead links (%d)", deadChildren.size());
                linkMenu->m_subMenu.addItemFromExternalPlugin(Link_ClearDeadLinks, menuText);
                //ContextMenuItem* deadLinksMenu = linkMenu->m_subMenu.addItemFromExternalPlugin(Link_ClearDeadLinks, menuText);


                ObjectPath objectPathInClipboard = ActorEditor::getObjectPathFromClipboard();

                if(objectPathInClipboard.isValid())
                {
                    Pickable* pObjClipboard = NULL;
                    if(objectPathInClipboard.getIsAbsolute())
                    {
                        pObjClipboard = SceneObjectPathUtils::getObjectFromAbsolutePath(objectPathInClipboard);
                    }
                    else
                    {
                        pObjClipboard = SceneObjectPathUtils::getObjectFromRelativePath(_actor, objectPathInClipboard);
                    }

                    if(pObjClipboard && _actor != pObjClipboard)
                    {
                        if ( linkComponent->hasChild(objectPathInClipboard) )
                        {
                            linkMenu->m_subMenu.addItemFromExternalPlugin(Link_RemoveObjectPathChild, "Remove ObjectPath in clipboard from children list");
                        }
                        else
                        {
                            linkMenu->m_subMenu.addItemFromExternalPlugin(Link_AddObjectPathChild, "Paste ObjectPath from clipboard into children list");
                        }
                    }
                }
            }
        }

        fillDebugModeMenuItems(linkMenu->m_subMenu);
    }

    void Link_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        switch(_item->getIdFromExternal())
        {
        case Link_StartCreate:
                m_linkingFromContextMenu = btrue;
                m_linkingFromShortcut = bfalse;
            break;

        case Link_ClearChildren:
                clearChildren(_actor);
            break;

        case Link_ClearDeadLinks:
                clearDeadLinks(_actor);
            break;

        case Link_RemoveObjectPathChild:
                removeObjectPathFromClipboard(_actor->getRef());
            break;

        case Link_AddObjectPathChild:
                pasteObjectPathFromClipboard(_actor->getRef());
            break;
        }

        onDebugModeMenuItemSelected(_item);
    }

    void Link_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        ContextMenuItem* LDMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuLD);
        if (LDMenuItem)
        {
            EditorContextMenu& LDmenu = LDMenuItem->m_subMenu;
            EditorContextMenu& linkMenu = LDmenu.addItemFromExternalPlugin(Link_Menu, "Links")->m_subMenu;
            String menuText;

            LinkComponent::ChildrenList deadChildren;
            getAllDeadLinks(deadChildren);
            menuText.setTextFormat("Clear dead links (%d)", deadChildren.size());
            linkMenu.addItemFromExternalPlugin(Link_ClearDeadLinks, menuText);

            fillDebugModeMenuItems(linkMenu);
        }
    }

    void Link_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch(_item->getIdFromExternal())
        {
        case Link_ClearDeadLinks:
            clearAllDeadLinks();
            break;
        }

        onDebugModeMenuItemSelected(_item);
    }

    void Link_Plugin::fillDebugModeMenuItems( EditorContextMenu& _menu )
    {
        // separator
        _menu.addItemFromExternalPlugin(U32_INVALID, String(), btrue);

        {
            ContextMenuItem* item = _menu.addItemFromExternalPlugin(Link_CycleDrawDebug, "Draw off");
            item->m_userData = LinkManager::DrawDebug_Off;
            if (LINKMANAGER->m_drawDebug == LinkManager::DrawDebug_Off)
            {
                item->setInactiveColor();
            }
        }
        {
            ContextMenuItem* item = _menu.addItemFromExternalPlugin(Link_CycleDrawDebug, "Draw all");
            item->m_userData = LinkManager::DrawDebug_All;
            if (LINKMANAGER->m_drawDebug == LinkManager::DrawDebug_All)
            {
                item->setInactiveColor();
            }
        }
        {
            ContextMenuItem* item = _menu.addItemFromExternalPlugin(Link_CycleDrawDebug, "Draw selection (both ways)");
            item->m_userData = LinkManager::DrawDebug_Selection_BothWays;
            if (LINKMANAGER->m_drawDebug == LinkManager::DrawDebug_Selection_BothWays)
            {
                item->setInactiveColor();
            }
        }
        {
            ContextMenuItem* item = _menu.addItemFromExternalPlugin(Link_CycleDrawDebug, "Draw selection (forward only)");
            item->m_userData = LinkManager::DrawDebug_Selection_ForwardOnly;
            if (LINKMANAGER->m_drawDebug == LinkManager::DrawDebug_Selection_ForwardOnly)
            {
                item->setInactiveColor();
            }
        }
    }

    void Link_Plugin::onDebugModeMenuItemSelected( ContextMenuItem* _item )
    {
        if (_item->getIdFromExternal() == Link_CycleDrawDebug)
        {
            LINKMANAGER->m_drawDebug = (LinkManager::DrawDebug_t)_item->m_userData;
        }
    }


    void Link_Plugin::onUpdate()
    {
        m_displayedListCurrentFrame.clear();

        switch (LINKMANAGER->m_drawDebug)
        {
        case LinkManager::DrawDebug_All:
            addAllLinksToDrawList();
            break;
        case LinkManager::DrawDebug_Selection_BothWays:
            addSelectionAndHoveredToDrawList(btrue);
            break;
        case LinkManager::DrawDebug_Selection_ForwardOnly:
            addSelectionAndHoveredToDrawList(bfalse);
            break;
        }

        m_linkingFromShortcut = bfalse;
        if(EDITOR->getState() != Editor::EditorState_Idle && !EDITOR->isInRotationScaleHelperMode() && !EDITOR->isRotationActive() && ActorEditor::getInstance()->getState() != ActorEditor::CreateBind)
        {
            if(!m_linkingFromContextMenu && INPUT_ADAPTER->isKeyPressed(KEY_LALT))
            {
                m_linkingFromShortcut = btrue;
            }
        }
        else
        {
            m_linkingFromContextMenu = bfalse;
        }

        if(m_linkingFromShortcut || m_linkingFromContextMenu)
        {
            if(PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse())
            {
                if(BaseObject* pObj = pShape->getOwnerPtr())
                {
                    if(SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(EDITOR->allowEditSubScene(pSSA))
                        {
                            m_lastHoveredSubSceneActor = pObj->getRef();
                        }
                    }
                }
            }
        }
        else
        {
            m_lastHoveredSubSceneActor.invalidate();
        }
    }
    
    void Link_Plugin::addAllLinksToDrawList()
    {
        ObjectMultimap parentsToChildren = LINKMANAGER->getParentsToChildren();
        ObjectMultimap childrenToParents = LINKMANAGER->getChildrenToParents();

#ifdef DEVELOPER_BEN
        String s;s.setTextFormat("%d parents, %d children", parentsToChildren.size(), childrenToParents.size());
        GFX_ADAPTER->drawDBGText(s);
#endif

        World* currentWorld = CURRENTWORLD;

        for (ObjectMultimap::iterator it = parentsToChildren.begin(); it != parentsToChildren.end(); it++)
        {
            ObjectRef oRef = it->first;
            if ( Pickable* parentObj = static_cast<Pickable*>(oRef.getObject()) )
            {
                if ( Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                {
                    if ( parent->getWorld() != currentWorld )
                    {
                        continue;
                    }

                    if(LinkComponent* pLC = parent->GetComponent<LinkComponent>())
                    {
                        const ObjectRefList& children = it->second;
                        for (u32 i = 0; i < children.size(); i++)
                            addLinkToDrawList(pLC, i, btrue);
                    }
                }
            }
        }
    }


    void Link_Plugin::addSelectionAndHoveredToDrawList( const bbool _backtrackParents )
    {
        static ObjectRefList alreadyReached;
        static ObjectRefList itemList;
        static ObjectRefList roots;

        alreadyReached.clear();
        itemList = EDITOR->getEditorSelectionAsObjRefList();
        if(PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse())
            itemList.push_back(pShape->getOwner());
        roots = itemList;

        // For each unreached item
        // Add neighbors (children and parents) to items list and draw links to children
        while(itemList.size())
        {
            ObjectRef oRef = itemList[0];
            itemList.eraseNoOrder(0);
            if(alreadyReached.find(oRef) != -1)
                continue;            
            alreadyReached.push_back(oRef);

            Pickable* pObj = static_cast<Pickable*>(oRef.getObject());
            if(!pObj)
                continue;

            if (_backtrackParents)
            {
                if ( ObjectRefList* parents = LINKMANAGER->getParents(oRef) )
                {
                    for (u32 i = 0; i < parents->size(); i++)
                    {
                        ObjectRef parentRef = (*parents)[i];
                        if(alreadyReached.find(parentRef) == -1 && itemList.find(parentRef) == -1)
                            itemList.push_back(parentRef);
                    }
                }
            }
            
            if( Actor* pActor = pObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                if ( ObjectRefList* children = LINKMANAGER->getChildren(oRef) )
                {
                    LinkComponent* pLC = pActor->GetComponent<LinkComponent>();
                    ITF_ASSERT_CRASH(pLC, "The actor has children but not linkcomponent ?");

                    for (u32 i = 0; i < children->size(); i++)
                    {
                        ObjectRef childRef = (*children)[i];
                        if(alreadyReached.find(childRef) == -1 && itemList.find(childRef) == -1)
                            itemList.push_back(childRef);

                        if ( Pickable* childObj = static_cast<Pickable*>(childRef.getObject()) )
                        {
                            const bbool isSelected = (roots.find(oRef) != -1 || roots.find(childRef) != -1);
                            addLinkToDrawList(pLC, i, isSelected);
                        }
                    }
                }
            }
        }
    }

    bbool Link_Plugin::canLinkActorToPickable( const Actor* _pParent, const Pickable* _pChild, String* _tip /*= NULL*/ ) const
    {
        if ( !_pParent || !_pChild ||
              _pParent == _pChild )
        {
            return bfalse;
        }

        // only link to actors or frises
        if ( _pChild->getObjectType() != BaseObject::eActor &&
             _pChild->getObjectType() != BaseObject::eFrise )
        {
            return bfalse;
        }

        // both must be serializable
        if ( !_pParent->isSerializable() ||
             !_pChild->isSerializable() )
        {
            if (_tip)
            {
                *_tip = "Cannot link spawned objects";
            }
            return bfalse;
        }

        // don't link to the base world
        if (World* pWorld = _pChild->getWorld())
        {
            if (pWorld->getRef() == GAMEMANAGER->getBaseMapWorld())
            {
                if (_tip)
                {
                    *_tip = "Cannot link to the base world";
                }
                return bfalse;
            }
        }

        return btrue;
    }

    bbool Link_Plugin::isLinkedActorToPickable( const Actor* _pActorParent, const Pickable* _pChild ) const
    {
        if(_pActorParent && _pChild)
        {
            if ( LinkComponent* linkComponent = _pActorParent->GetComponent<LinkComponent>() )
            {
                ObjectPath path;

                SceneObjectPathUtils::getRelativePathFromObject(_pActorParent,_pChild,path);

                if ( !path.isValid() )
                {
                    SceneObjectPathUtils::getAbsolutePathFromObject(_pChild,path);
                }

                return linkComponent->hasChild(path);
            }
        }

        return bfalse;
    }

    bbool Link_Plugin::onMouseButton( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (_but == InputAdapter::MB_Left && _status == InputAdapter::Pressed)
        {
            if(m_linkingFromShortcut || m_linkingFromContextMenu)
            {
                const ObjectRefList selection = EDITOR->getEditorSelectionAsObjRefList();
                for(u32 iSelection = 0; iSelection < selection.size(); ++iSelection)
                {
                    Pickable* pObj = static_cast<Pickable*>(selection[iSelection].getObject());
                    if(!pObj || pObj->getObjectType() != BaseObject::eActor)
                        continue;

                    Actor* pActor = static_cast<Actor*>(pObj);
                    LinkComponent* pLinkComp = pActor->GetComponent<LinkComponent>();
                    if(pLinkComp)
                    {
                        PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
                        if(pShape)
                        {
                            if(Pickable* underMouse = static_cast<Pickable*>(pShape->getOwnerPtr()))
                            {
                                if(canLinkActorToPickable(pActor, underMouse))
                                {
                                    UNDOREDO_MANAGER->preChange(pActor);

                                    ObjectPath pathToObject;
                                    SceneObjectPathUtils::getRelativePathFromObject(pActor, underMouse, pathToObject);

                                    if ( !pathToObject.isValid() )
                                    {
                                        SceneObjectPathUtils::getAbsolutePathFromObject(underMouse, pathToObject);
                                    }

                                    if(isLinkedActorToPickable(pActor, underMouse))
                                    {
                                        pLinkComp->removeChild(pathToObject);
                                    }
                                    else
                                    {
                                        pLinkComp->addChild(pathToObject);
                                    }
                                    PLUGINGATEWAY->onObjectChanged(pActor);
                                    m_linkingFromShortcut = bfalse;
                                    m_linkingFromContextMenu = bfalse;
                                    return bfalse;
                                }
                            }
                        }
                    }
                }
            }
        }
        return btrue;
    }
    
    void Link_Plugin::pasteObjectPathFromClipboard( ObjectRef _target )
    {
        ObjectPath absPath = ActorEditor::getObjectPathFromClipboard();

        if ( absPath.isValid() )
        {
            BaseObject* obj = _target.getObject();

            if ( obj )
            {
                Actor* actorTarget = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( actorTarget )
                {
                    if ( LinkComponent* linkComponent = actorTarget->GetComponent<LinkComponent>() )
                    {
                        UNDOREDO_MANAGER->preChange(actorTarget);

                        linkComponent->addChild(absPath);

                        PLUGINGATEWAY->onObjectChanged(actorTarget);

                        if (DEBUGINFO)
                        {
                            DEBUGINFO->addDebugInfoDisplay("Added ObjectPath as child", 5.0f);
                        }
                    }
                }
            }
        }
    }

    void Link_Plugin::removeObjectPathFromClipboard( ObjectRef _target )
    {
        ObjectPath absPath = ActorEditor::getObjectPathFromClipboard();

        if ( absPath.isValid() )
        {
            BaseObject* obj = _target.getObject();

            if ( obj )
            {
                Actor* actorTarget = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( actorTarget )
                {
                    if ( LinkComponent* linkComponent = actorTarget->GetComponent<LinkComponent>() )
                    {
                        UNDOREDO_MANAGER->preChange(actorTarget);

                        linkComponent->removeChild(absPath);

                        PLUGINGATEWAY->onObjectChanged(actorTarget);

                        if (DEBUGINFO)
                        {
                            DEBUGINFO->addDebugInfoDisplay("Removed ObjectPath as child", 5.0f);
                        }
                    }
                }
            }
        }
    }

    void Link_Plugin::clearChildren( Actor* _actor )
    {
        LinkComponent* linkComponent = _actor->GetComponent<LinkComponent>();
        ITF_ASSERT_CRASH(linkComponent, "Bad context actor");

        UNDOREDO_MANAGER->preChange(_actor);
        linkComponent->clearLinks();
        PLUGINGATEWAY->onObjectChanged(_actor);

        if (DEBUGINFO)
        {
            DEBUGINFO->addDebugInfoDisplay("Cleared links", 5.0f);
        }
    }

    void Link_Plugin::getDeadLinks( LinkComponent* _linkComponent, LinkComponent::ChildrenList& _deadChildren )
    {
        const LinkComponent::ChildrenList& children = _linkComponent->getChildren();
        for (u32 iChild = 0; iChild < children.size(); iChild++)
        {
            const ChildEntry& childEntry = children[iChild];

            Pickable* pickable = NULL;
            if (childEntry.getPath().getIsAbsolute())
                pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(childEntry.getPath());
            else
                pickable = SceneObjectPathUtils::getObjectFromRelativePath(_linkComponent->GetActor(), childEntry.getPath());

            if ( !pickable )
            {
                _deadChildren.push_back(childEntry);
            }
        }
    }

    void Link_Plugin::clearDeadLinks( Actor* _actor )
    {
        LinkComponent* linkComponent = _actor->GetComponent<LinkComponent>();
        ITF_ASSERT_CRASH(linkComponent, "Bad context actor");

        UNDOREDO_MANAGER->preChange(_actor);

        LinkComponent::ChildrenList deadChildren;
        getDeadLinks(linkComponent, deadChildren);

        for (u32 i = 0; i < deadChildren.size(); i++)
        {
            linkComponent->removeChild(deadChildren[i].getPath());
        }

        PLUGINGATEWAY->onObjectChanged(_actor);

        if (DEBUGINFO)
        {
            String s;s.setTextFormat("Cleared %d dead links", deadChildren.size());
            DEBUGINFO->addDebugInfoDisplay(s, 5.0f);
        }
    }

    void Link_Plugin::getAllDeadLinks( LinkComponent::ChildrenList& _deadChildren )
    {
        World* world = CURRENTWORLD;
        if (!world)
        {
            return;
        }

        const u32 sceneCount = world->getSceneCount();
        for (u32 i = 0; i < sceneCount; i++)
        {
            Scene* scene = world->getSceneAt(i);
            const PickableList& actors = scene->getActors();
            for (u32 j = 0; j < actors.size(); j++)
            {
                Actor* actor = (Actor*)actors[j];
                LinkComponent* linkComponent = actor->GetComponent<LinkComponent>();
                if (linkComponent)
                {
                    getDeadLinks(linkComponent, _deadChildren);
                }
            }
        }
    }

    void Link_Plugin::clearAllDeadLinks()
    {
        World* world = CURRENTWORLD;
        if (!world)
        {
            return;
        }

        u32 deadLinksCount = 0;

        const u32 sceneCount = world->getSceneCount();
        for (u32 i = 0; i < sceneCount; i++)
        {
            Scene* scene = world->getSceneAt(i);
            const PickableList& actors = scene->getActors();
            for (u32 j = 0; j < actors.size(); j++)
            {
                Actor* actor = (Actor*)actors[j];
                LinkComponent* linkComponent = actor->GetComponent<LinkComponent>();
                if (linkComponent)
                {
                    LinkComponent::ChildrenList deadChildren;
                    getDeadLinks(linkComponent, deadChildren);
                    deadLinksCount += deadChildren.size();

                    for (u32 i = 0; i < deadChildren.size(); i++)
                    {
                        linkComponent->removeChild(deadChildren[i].getPath());
                    }
                }
            }
        }

        if (DEBUGINFO)
        {
            String s;s.setTextFormat("Cleared %d dead links", deadLinksCount);
            DEBUGINFO->addDebugInfoDisplay(s, 5.0f);
        }
    }
    
    void Link_Plugin::fillEditorActiveList( ObjectRefList &_pickablesObjects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const
    {
        if(m_linkingFromShortcut || m_linkingFromContextMenu)
        {
            if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(m_lastHoveredSubSceneActor.getObject()))
            {
                if(Scene* pScene = pSSA->getSubScene())
                {
                    const BaseObjectList& objects = pScene->getActiveObjects();

                    ID_SERVER->getObjectListNoNull(objects);
                    SafeArray<BaseObject*>& resolvedRefs = objects.m_solvedPointers;

                    for(u32 iSub = 0; iSub < resolvedRefs.size(); ++iSub)
                    {
                        BaseObject* pObj = resolvedRefs[iSub];
                        switch(pObj->getObjectType())
                        {
                        case BaseObject::eActor:
                            ActorEditor::updateActorPickingShape(pObj->getRef(), _pickablesObjects, _orderedShapes);
                            break;

                        case BaseObject::eFrise:
                            FriezeEditor::updateFriezePickingShape(pObj->getRef(), _pickablesObjects, _orderedShapes);
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
        }
    }

    void Link_Plugin::addLinkToDrawList( LinkComponent* _linkComponent, u32 _childIndex, const bbool _isSelected )
    {
        const Color& linkColor = _isSelected ? _linkComponent->getTemplate()->getDebugColorSelected() : _linkComponent->getTemplate()->getDebugColor();

        const Actor* parent = _linkComponent->GetActor();
        const ObjectRef parentRef = parent->getRef();

        ObjectRefList* children = LINKMANAGER->getChildren(parentRef);
        ITF_ASSERT_MSG(children, "No children!");
        if(!children)
            return;

        const ObjectRef& childRef = (*children)[_childIndex];
        const Pickable* pChild = static_cast<const Pickable*>(childRef.getObject());
        ITF_ASSERT_MSG(pChild, "Trying to draw a dead link!");
        if(!pChild)
            return;

        if (const Actor* child = pChild->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            if (otherComponentDrawsTheLink(parent, child))
            {
                return;
            }
        }


        DisplayedLink dl;
        dl.color = linkColor;

        if(_linkComponent->getTemplate()->getDebugChildIndex())
            dl.drawDbgIndex = _childIndex;
        else
            dl.drawDbgIndex = U32_INVALID;

        bbool twoWayLink = bfalse;
        if(ObjectRefList* child_children = LINKMANAGER->getChildren(childRef))
        {
            twoWayLink = (*child_children).find(parentRef) != -1;
        }

        if (twoWayLink)
        {
            for (u32 i = 0; i < m_displayedListCurrentFrame.size(); i++)
            {
                DisplayedLink& dl = m_displayedListCurrentFrame[i];
                if ( dl.parent == parentRef && dl.child == childRef ||
                     dl.parent == childRef && dl.child == parentRef )
                {
                    return;
                }
            }

            static const f32 s_normalOffset = 15.f;    // pixel

            Vec3d pos = parent->getAnchorPos();
            Vec3d childPos = pChild->getAnchorPos();

            Vec3d posScreen;
            GFX_ADAPTER->compute3DTo2D(pos, posScreen);

            Vec3d childPosScreen;
            GFX_ADAPTER->compute3DTo2D(childPos, childPosScreen);

            Vec2d dir2d = (childPosScreen.truncateTo2D() - posScreen.truncateTo2D());
            Vec3d normal = dir2d.normalize().getPerpendicular().to3d();

            // forward link
            {
                posScreen += normal * s_normalOffset;
                GFX_ADAPTER->compute2DTo3D(posScreen, pos);

                childPosScreen += normal * s_normalOffset;
                GFX_ADAPTER->compute2DTo3D(childPosScreen, childPos);

                dl.parent = parentRef;
                dl.child = childRef;
                dl.parentScreenPos = posScreen.truncateTo2D();
                dl.childScreenPos = childPosScreen.truncateTo2D();
                dl.parentWorldPos = pos;
                dl.childWorldPos = childPos;

                m_displayedListCurrentFrame.push_back(dl);
            }

            // back link
            {
                posScreen -= normal * s_normalOffset * 2;
                GFX_ADAPTER->compute2DTo3D(posScreen, pos);

                childPosScreen -= normal * s_normalOffset * 2;
                GFX_ADAPTER->compute2DTo3D(childPosScreen, childPos);

                dl.parent = childRef;
                dl.child = parentRef;
                dl.parentScreenPos = childPosScreen.truncateTo2D();
                dl.childScreenPos = posScreen.truncateTo2D();
                dl.parentWorldPos = childPos;
                dl.childWorldPos = pos;

                m_displayedListCurrentFrame.push_back(dl);
            }
        }
        else
        {
            Vec3d pos = parent->getAnchorPos();
            Vec3d childPos = pChild->getAnchorPos();

            dl.parent = parentRef;
            dl.child = childRef;

            Vec3d parentScreen;
            GFX_ADAPTER->compute3DTo2D(pos, parentScreen);
            dl.parentScreenPos = parentScreen.truncateTo2D();
            dl.parentWorldPos = pos;

            Vec3d childScreen;
            GFX_ADAPTER->compute3DTo2D(childPos, childScreen);
            dl.childScreenPos = childScreen.truncateTo2D();
            dl.childWorldPos = childPos;

            m_displayedListCurrentFrame.push_back(dl);
        }
    }

    bbool Link_Plugin::otherComponentDrawsTheLink( const Actor* parent, const Actor* child )
    {
        if (parent->isSelected())
        {
            return bfalse;
        }

        if ( parent->getComponentFromStaticClassCRC(ITF_GET_STRINGID_CRC(Ray_BossMorayNodeComponent,733639517)) &&
             child->getComponentFromStaticClassCRC(ITF_GET_STRINGID_CRC(Ray_BossMorayNodeComponent,733639517)) )
        {
            return btrue;
        }

        //if ( parent->getComponentFromStaticClassName("Ray_BossPlantNodeComponent") &&
        //     child->getComponentFromStaticClassName("Ray_BossPlantNodeComponent") )
        //{
        //    return btrue;
        //}

        return bfalse;
    }

    void Link_Plugin::onDraw()
    {
        for(u32 i = 0; i < m_displayedListCurrentFrame.size(); ++i)
        {
            const DisplayedLink& linkInfo = m_displayedListCurrentFrame[i];
            
            DebugDraw::arrow3D(linkInfo.parentWorldPos, linkInfo.childWorldPos, linkInfo.color);
            if (linkInfo.drawDbgIndex != U32_INVALID)
            {
                Color childIndexColor(
                    linkInfo.color.m_a,
                    1.0f - linkInfo.color.m_r,
                    1.0f - linkInfo.color.m_g,
                    1.0f - linkInfo.color.m_b
                );
                DebugDraw::text(
                    linkInfo.parentWorldPos + (linkInfo.childWorldPos - linkInfo.parentWorldPos) * 0.5f,
                    childIndexColor, "%d", linkInfo.drawDbgIndex);
            }
        }

        // preview link
        if(m_linkingFromShortcut || m_linkingFromContextMenu)
        {
            const ObjectRefList selection = EDITOR->getEditorSelectionAsObjRefList();
            for(u32 iSelection = 0; iSelection < selection.size(); ++iSelection)
            {
                Pickable* pObj = static_cast<Pickable*>(selection[iSelection].getObject());
                if(!pObj || pObj->getObjectType() != BaseObject::eActor)
                    continue;

                const Actor* pActor = static_cast<const Actor*>(pObj);

                if(pActor->GetComponent<LinkComponent>())
                {
                    f32 lineSize = 5.0f;
                    f32 arrowWidth = 30.0f;
                    f32 arrowLength = 40.0f;
                    Color arrowColor = Color::red();

                    String tip;

                    PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
                    if(pShape && pShape->getShapeType() == PickingShape::ShapeType_Disc)
                    {
                        PickingShape_Disc* pShapeDisc = static_cast<PickingShape_Disc*>(pShape);
                        if(Pickable* underMouse = static_cast<Pickable*>(pShapeDisc->getOwnerPtr()))
                        {
                            if(canLinkActorToPickable(pActor, underMouse, &tip))
                            {
                                arrowColor = Color::green();

                                if(isLinkedActorToPickable(pActor, underMouse))
                                {
                                    lineSize *= 1.2f;
                                    arrowLength *= 1.2f;
                                    arrowWidth *= 1.2f;

                                    tip = "Unlink from:";
                                }
                                else
                                {
                                    tip = "Link to:";
                                }
                            }
                        }
                    }

                    if (!tip.isEmpty())
                    {
                        const Vec2d& mouse = EDITOR->getMouse2d() + Vec2d(20.0f, 0.0f);
                        GFX_ADAPTER->drawDBGText(tip, Color::white(), mouse.m_x, mouse.m_y);
                        GFX_ADAPTER->draw2dBox(mouse, tip.getLen() * Editor::fCharMaxWidth, Editor::fCharMaxHeight, EditorDraw::uNormalColor, EditorDraw::uNormalColor, EditorDraw::uDarkColor, EditorDraw::uNormalColor);
                    }

                    DebugDraw::arrow3D(pActor->getAnchorPos(), EDITOR->getMouse3d(), arrowColor, lineSize, arrowLength, arrowWidth);
                }
            }
        }
    }

} // namespace ITF
