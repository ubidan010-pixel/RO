#include "precompiled_gameplay.h"

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
#define GETFRIENDLY(pPickable) (pPickable ? static_cast<Pickable*>(pPickable)->getUserFriendly().cStr() : "NULL")

    LinkManager* LinkManager::s_instance = NULL;

    LinkManager::LinkManager()
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
#ifdef DEVELOPER_LCOU
        m_drawDebug = DrawDebug_Off;
#else
        m_drawDebug = DrawDebug_Selection_BothWays;
#endif
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    LinkManager::~LinkManager()
    {
    }

    void LinkManager::destroy()
    { 
        SCENE_MANAGER->removeListener(s_instance);
        SF_DEL(s_instance); 
    }

    void LinkManager::init()
    {
        SCENE_MANAGER->addListener(this);
    }


    void LinkManager::addLink( const ObjectRef& _parent, const ObjectRef& _child )
    {
        // parent-to-children
        {
            ObjectMultimap::iterator it = m_parentsToChildren.find(_parent);
            if (it == m_parentsToChildren.end())
            {
                m_parentsToChildren.insert(ObjectMultimapPair(_parent, ObjectRefList()));
            }

            ObjectRefList& children = m_parentsToChildren[_parent];
            ITF_WARNING_CATEGORY(LD, _parent.getObject(), children.find(_child) == -1, "duplicate link to child %s", GETFRIENDLY(_child.getObject()));
            children.push_back(_child);
        }

        // child-to-parents
        {
            ObjectMultimap::iterator it = m_childrenToParents.find(_child);
            if (it == m_childrenToParents.end())
            {
                m_childrenToParents.insert(ObjectMultimapPair(_child, ObjectRefList()));
            }

            ObjectRefList& parents = m_childrenToParents[_child];
            ITF_WARNING_CATEGORY(LD, _child.getObject(), parents.find(_parent) == -1, "duplicate link from parent %s", GETFRIENDLY(_parent.getObject()));
            parents.push_back(_parent);
        }
    }

    void LinkManager::removeLink( const ObjectRef& _parent, const ObjectRef& _child )
    {
        removeParentToChildrenLink(_parent, _child);
        removeChildToParentsLink(_parent, _child);
    }

    void LinkManager::removeParentToChildrenLink( const ObjectRef& _parent, const ObjectRef& _child )
    {
        ITF_ASSERT_CRASH( m_parentsToChildren.find(_parent) != m_parentsToChildren.end(), "removeParentToChildrenLink: parent has no registered links! - %s", GETFRIENDLY(_parent.getObject()) );
        ITF_ASSERT_CRASH( m_parentsToChildren[_parent].find(_child) != -1, "removeParentToChildrenLink: %s is not a parent of %s", GETFRIENDLY(_parent.getObject()), GETFRIENDLY(_child.getObject()) );

        ObjectRefList& children = m_parentsToChildren[_parent];
        if (children.size() > 1)
        {
            i32 index = children.find(_child);
            children.eraseNoOrder(index);
        }
        else
        {
            m_parentsToChildren.erase(_parent);
        }
    }

    void LinkManager::removeChildToParentsLink( const ObjectRef& _parent, const ObjectRef& _child )
    {
        ITF_ASSERT_CRASH( m_childrenToParents.find(_child) != m_childrenToParents.end(), "removeChildToParentsLink: child has no registered links! - %s", GETFRIENDLY(_child.getObject()) );
        ITF_ASSERT_CRASH( m_childrenToParents[_child].find(_parent) != -1, "removeChildToParentsLink: %s is not a child of %s", GETFRIENDLY(_child.getObject()), GETFRIENDLY(_parent.getObject()) );

        ObjectRefList& parents = m_childrenToParents[_child];
        if (parents.size() > 1)
        {
            i32 index = parents.find(_parent);
            parents.eraseNoOrder(index);
        }
        else
        {
            m_childrenToParents.erase(_child);
        }
    }

    void LinkManager::removeActor( const ObjectRef& _actor )
    {
        removeParentLinks(_actor);
        removeChildLinks(_actor);
    }

    void LinkManager::removeParentLinks( const ObjectRef& _child )
    {
        if ( ObjectRefList* parents = getParents(_child) )
        {
            for (u32 i = 0; i < parents->size(); i++)
            {
                ObjectRef& parent = (*parents)[i];
                removeParentToChildrenLink(parent, _child);
            }

            m_childrenToParents.erase(_child);
        }
    }

    void LinkManager::removeChildLinks( const ObjectRef& _parent )
    {
        if ( ObjectRefList* children = getChildren(_parent) )
        {
            for (u32 i = 0; i < children->size(); i++)
            {
                ObjectRef& child = (*children)[i];
                removeChildToParentsLink(_parent, child);
            }

            m_parentsToChildren.erase(_parent);
        }
    }

    void LinkManager::onScenePreDestroy( Scene* _scene )
    {
        // clear all links when a scene is unloaded

        // parent-to-children
        {
            ObjectMultimap::iterator it = m_parentsToChildren.begin();
            while ( it != m_parentsToChildren.end() )
            {
                if ( BaseObject* parentObj = it->first.getObject() )
                {
                    if( Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( parent->getScene() == _scene )
                        {
                            ObjectRefList& children = it->second;
                            for (u32 i = 0; i < children.size(); i++)
                            {
                                ObjectRef& child = children[i];
                                removeChildToParentsLink(parent->getRef(), child);
                            }
#ifdef ITF_CTR //CASA::ToCheck erase function
                            ObjectMultimap::iterator it0 = it;
                            m_parentsToChildren.erase(it0);
                            it++;
#else
                            it = m_parentsToChildren.erase(it);
#endif
                            continue;
                        }
                    }
                }

                it++;
            }
        }

        // child-to-parents
        {
            ObjectMultimap::iterator it = m_childrenToParents.begin();
            while ( it != m_childrenToParents.end() )
            {
                if ( BaseObject* childObj = it->first.getObject() )
                {
                    if( Actor* child = childObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( child->getScene() == _scene )
                        {
                            ObjectRefList& parents = it->second;
                            for (u32 i = 0; i < parents.size(); i++)
                            {
                                ObjectRef& parent = parents[i];
                                removeParentToChildrenLink(parent, child->getRef());
                            }

#ifdef ITF_CTR //CASA::ToCheck erase function
                            ObjectMultimap::iterator it0 = it;
                            m_parentsToChildren.erase(it0);
                            it++;
#else
                            it = m_childrenToParents.erase(it);
#endif
                            continue;
                        }
                    }
                }

                it++;
            }
        }
    }



    ObjectRefList* LinkManager::getParents( const ObjectRef& _child )
    {
        ObjectMultimap::iterator it = m_childrenToParents.find(_child);
        if (it != m_childrenToParents.end())
        {
            return &it->second;
        }

        return NULL;
    }

    ObjectRefList* LinkManager::getChildren( const ObjectRef& _parent )
    {
        ObjectMultimap::iterator it = m_parentsToChildren.find(_parent);
        if (it != m_parentsToChildren.end())
        {
            return &it->second;
        }

        return NULL;
    }

    void LinkManager::getTree( const ObjectRef& _root, Node& _tree )
    {
        ProcessedNodeList processedNodes;
        getTree(_root, _tree, processedNodes);
    }

    void LinkManager::getTree( const ObjectRef& _parent, Node& _node, ProcessedNodeList& _processedNodes )
    {
        _node.m_object = _parent;

        if ( _processedNodes.find(_parent) != -1 )
        {
            // already processed
            return;
        }
        _processedNodes.push_back(_parent);

        if ( ObjectRefList* children = getChildren(_parent) )
        {
            for (u32 i = 0; i < children->size(); i++)
            {
                ObjectRef& child = (*children)[i];

                _node.m_childNodes.push_back(Node());
                Node& childNode = _node.m_childNodes.back();
                getTree(child, childNode, _processedNodes);
            }
        }
    }
    
    void LinkManager::getFlatTree( const ObjectRef& _parent, ObjectMultimap& _tree )
    {
        if ( _tree.find(_parent) != _tree.end() )
        {
            // already processed
            return;
        }

        if ( ObjectRefList* children = getChildren(_parent) )
        {
            _tree.insert(ObjectMultimapPair(_parent, *children));

            for (u32 i = 0; i < children->size(); i++)
            {
                ObjectRef& child = (*children)[i];
                getFlatTree(child, _tree);
            }
        }
    }

    void LinkManager::onPickableSceneChange( Pickable* _pObj, Scene* _src, Scene *_dst, PickableList& _changedObjects )
    {
        if(Actor* pActor = _pObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            ObjectRef objRef = pActor->getRef();

            // Update parent data to avoid dead links
            if(ObjectRefList* pParentList = getParents(objRef))
            {
                ObjectRefList parentListCopy = *pParentList;
                for (u32 i = 0; i < parentListCopy.size(); i++)
                {
                    ObjectRef parent = parentListCopy[i];

                    if(Actor* pParent = static_cast<Actor*>(parent.getObject()))
                    {
                        if(LinkComponent* pLC = pParent->GetComponent<LinkComponent>())
                        {
                            LinkComponent::ChildrenList& childrenList = pLC->getMutableChildren();

                            for(u32 iChild = 0; iChild < childrenList.size(); ++iChild)
                            {
                                ChildEntry& childRef = childrenList[iChild];
                                const ObjectPath& path = childRef.getPath();

                                const bbool isAbsolute = path.getIsAbsolute();
                                Pickable* pChild = NULL;
                                if(isAbsolute)
                                    pChild = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                                else
                                    pChild = SceneObjectPathUtils::getObjectFromRelativePath(pParent, path);

                                if(!pChild && path.getObjId() == pActor->getObjectId() || String8(path.getStrId()) == pActor->getUserFriendly())
                                {
                                    _changedObjects.push_back(pParent);

                                    ObjectPath newPath;
                                    if(isAbsolute)
                                        SceneObjectPathUtils::getAbsolutePathFromObject(pActor, newPath);
                                    else
                                        SceneObjectPathUtils::getRelativePathFromObject(pParent, pActor, newPath);

                                    childRef.setPath(newPath);
                                }
                            }
                        }
                        else
                        {
                            ITF_ASSERT(0);
                        }
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
            }

            // Update child relative paths
            if(LinkComponent* pLC = pActor->GetComponent<LinkComponent>())
            {
                if(ObjectRefList* pChildrenList = getChildren(objRef))
                {
                    _changedObjects.push_back(pActor);

                    ObjectRefList childrenListCopy = *pChildrenList;
                    LinkComponent::ChildrenList& childpathListRef = pLC->getMutableChildren();

                    for (u32 iRef = 0; iRef < childrenListCopy.size(); iRef++)
                    {
                        ObjectRef child = childrenListCopy[iRef];
                        if(Pickable* pChildObj = static_cast<Pickable*>(child.getObject()))
                        {
                            for(u32 iPath = 0; iPath < childpathListRef.size(); ++iPath)
                            {
                                ChildEntry& childRef = childpathListRef[iPath];
                                const ObjectPath& oldPath = childRef.getPath();

                                if(oldPath.getObjId() == pChildObj->getObjectId())
                                {
                                    ObjectPath path;
                                    if(oldPath.getIsAbsolute())
                                        SceneObjectPathUtils::getAbsolutePathFromObject(pChildObj, path);
                                    else
                                        SceneObjectPathUtils::getRelativePathFromObject(pActor, pChildObj, path);

                                    if(path.isValid())
                                        childRef.setPath(path);

                                    break;
                                }
                                ITF_ASSERT(0); // should not 
                            }
                        }
                    }
                }
            }
        }
    }

} //namespace ITF
