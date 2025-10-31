#include "precompiled_gameplay.h"

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    BEGIN_SERIALIZATION(TagValue)
        SERIALIZE_MEMBER("Tag", m_tag);
        SERIALIZE_MEMBER("Value", m_value);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(ChildEntry)
        SERIALIZE_MEMBER("Path", m_childPath);
        SERIALIZE_CONTAINER_OBJECT("TagValues", m_tagValueList);
    END_SERIALIZATION()

    bbool ChildEntry::removeTag( const StringID& _tag )
    {
        bbool ret = bfalse;
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            m_tagValueList.erase(it);
            ret = btrue;
        }
        return ret;
    }
    
    void ChildEntry::removeTags()
    {
        m_tagValueList.clear();
    }
    
    bbool ChildEntry::hasTag( const StringID& _tag ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            ret = btrue;
        }
        return ret;
    }

    // f32 specialization
    template<>
    bbool ChildEntry::getTagValue<f32>( const StringID& _tag, f32& _value ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            const String8& rawValue = it->m_value;
            _value = rawValue.atof32();
            ret = btrue;
        }
        return ret;
    }

    template<>
    void ChildEntry::setTagValue<f32>( const StringID& _tag, const f32& _value )
    {
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            String8& rawValue = it->m_value;
            rawValue.f32toa(_value);
        }
        else
        {
            TagValue newTag;
            newTag.m_tag = _tag;
            newTag.m_value.f32toa(_value);
            m_tagValueList.push_back(newTag);
        }
    }
    
    // i32 specialization
    template<>
    bbool ChildEntry::getTagValue<i32>( const StringID& _tag, i32& _value ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            const String8& rawValue = it->m_value;
            _value = rawValue.atoi32();
            ret = btrue;
        }
        return ret;
    }

    template<>
    void ChildEntry::setTagValue<i32>( const StringID& _tag, const i32& _value )
    {
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            String8& rawValue = it->m_value;
            rawValue.i32toa(_value);
        }
        else
        {
            TagValue newTag;
            newTag.m_tag = _tag;
            newTag.m_value.i32toa(_value);
            m_tagValueList.push_back(newTag);
        }
    }
    
    // bbool specialization
    template<>
    bbool ChildEntry::getTagValue<bool>( const StringID& _tag, bool& _value ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            const String8& rawValue = it->m_value;
            static const String8 s_true = "true";
            static const String8 s_false = "false";
            if (rawValue.equals(s_true, btrue))
            {
                _value = btrue;
                ret = btrue;
            }
            else if (rawValue.equals(s_false, btrue))
            {
                _value = bfalse;
                ret = btrue;
            }
            else
            {
                ITF_WARNING(
                    NULL, bfalse, "Invalid tag value for '%s': '%s', use '%s' or '%s'",
                    _tag.getDebugString(), rawValue.cStr(), s_true.cStr(), s_false.cStr());
                ret = bfalse;
            }
        }
        return ret;
    }

    template<>
    void ChildEntry::setTagValue<bool>( const StringID& _tag, const bool& _value )
    {
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        static const String8 s_true = "true";
        static const String8 s_false = "false";
        if(it != m_tagValueList.end())
        {
            String8& rawValue = it->m_value;
            rawValue = _value ? s_true : s_false;
        }
        else
        {
            TagValue newTag;
            newTag.m_tag = _tag;
            newTag.m_value = _value ? s_true : s_false;
            m_tagValueList.push_back(newTag);
        }
    }
    
    // StringID specialization
    template<>
    bbool ChildEntry::getTagValue<StringID>( const StringID& _tag, StringID& _value ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            _value = it->m_value;
            ret = btrue;
        }
        return ret;
    }

    template<>
    void ChildEntry::setTagValue<StringID>( const StringID& _tag, const StringID& _value )
    {
        ITF_ASSERT_MSG(0,"This is not supported");

        /*
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            it->m_value.ui64toa(_value.GetValue());
        }
        else
        {
            TagValue newTag;
            newTag.m_tag = _tag;
            newTag.m_value.ui64toa(_value.GetValue());
            m_tagValueList.push_back(newTag);
        }
        */
    }
    
    // String8 specialization
    template<>
    bbool ChildEntry::getTagValue<String8>( const StringID& _tag, String8& _value ) const
    {
        bbool ret = bfalse;
        TagValueList::const_iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            _value = it->m_value;
            ret = btrue;
        }
        return ret;
    }

    template<>
    void ChildEntry::setTagValue<String8>( const StringID& _tag, const String8& _value )
    {
        TagValueList::iterator it = std::find(m_tagValueList.begin(), m_tagValueList.end(), _tag);
        if(it != m_tagValueList.end())
        {
            it->m_value = _value;
        }
        else
        {
            TagValue newTag;
            newTag.m_tag = _tag;
            newTag.m_value = _value;
            m_tagValueList.push_back(newTag);
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(LinkComponent)

    BEGIN_SERIALIZATION_CHILD(LinkComponent)
            
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_CONTAINER_OBJECT("Children", m_children);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
            SERIALIZE_CONTAINER("ChildrenObjId", m_linkedChildrenIds);
            SERIALIZE_CONTAINER("LinkedChildren", m_childrenPath);
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(convertFromPathList, ESerialize_Data_Load);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(LinkComponent)
        VALIDATE_COMPONENT_PARAM("", m_actor->GetComponents<LinkComponent>().size() == 1, "You cannot have more than one LinkComponent!");
        //VALIDATE_COMPONENT_PARAM("", GetClassNameStatic() == LinkComponent::GetClassNameStatic(), "You cannot inherit from LinkComponent!");
    END_VALIDATE_COMPONENT()

    LinkComponent::LinkComponent()
        : Super()
    {
    }

    LinkComponent::~LinkComponent()
    {
        clearLinks();
    }


    void LinkComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if ( m_linkedChildrenIds.size() )
        {
            convertFromOldFormat();
        }

        registerLinks();
    }

    void LinkComponent::convertFromOldFormat()
    {
        u32 numChildren = m_linkedChildrenIds.size();
        Scene* scene = m_actor->getScene();
        Actor* child;

        for ( u32 i = 0; i < numChildren; i++ )
        {
            child = WorldManager::getActor(scene, m_linkedChildrenIds[i]);

            if ( child )
            {
                ObjectPath relPath;

                SceneObjectPathUtils::getRelativePathFromObject(m_actor,child,relPath);

                if ( relPath.isValid() )
                {
                    ITF_ASSERT(!hasChild(relPath));
                    ChildEntry child;
                    child.setPath(relPath);
                    m_children.push_back(child);
                }
            }
        }

        m_linkedChildrenIds.clear();

        m_actor->replaceReinitDataByCurrent();
    }

    void LinkComponent::clearLinks()
    {
        if (!m_actor)
        {
            m_children.clear();
            return;
        }

        if (!LINKMANAGER)   // most likely exiting the application
        {
            return;
        }

        LINKMANAGER->removeChildLinks(m_actor->getRef());

        // copy children list, we want the list to be clean when EventOnLink is
        // processed by listeners; not really efficient but this is *not*
        // supposed to be called in performance-sensitive contexts
        ChildrenList pastChildren = m_children;
        m_children.clear();

        // notify listeners with EventOnLink
        u32 numPastChildren = pastChildren.size();
        Pickable* child;
        for (u32 i = 0; i<numPastChildren; ++i)
        {
            const ObjectPath& path = pastChildren[i].getPath();
            if (path.getIsAbsolute())
            {
                child = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
            }
            else
            {
                child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, path);
            }
            if (child)
            {
                EventOnLink onLink;
                onLink.setChild(child->getRef());
                onLink.setLinked(bfalse);
                m_actor->onEvent(&onLink);
            }
        }
    }

    void LinkComponent::registerLinks()
    {
        LINKMANAGER->removeChildLinks(m_actor->getRef());

        u32 numChildren = m_children.size();
        Pickable* child;

        for ( u32 i = 0; i < numChildren; i++ )
        {
            const ObjectPath& path = m_children[i].getPath();

            if ( path.getIsAbsolute() )
            {
                child = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
            }
            else
            {
                child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, path);
            }

            if ( child )
            {
                LINKMANAGER->addLink(m_actor->getRef(), child->getRef());
            }
        }
    }

    bbool LinkComponent::hasChild( const ObjectPath& _child )
    {
        return std::find(m_children.begin(), m_children.end(), _child) != m_children.end();
    }

    void LinkComponent::addChild( const ObjectPath& _child, bbool _addToLinkManager /*= btrue*/ )
    {
        if ( hasChild(_child) )
        {
            return;
        }

        ChildEntry childEntry;
        childEntry.setPath(_child);
        m_children.push_back(childEntry);


        Pickable* child;
        
        if ( _child.getIsAbsolute() )
        {
            child = SceneObjectPathUtils::getObjectFromAbsolutePath(_child);
        }
        else
        {
            child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,_child);
        }

        if ( child )
        {
            if ( _addToLinkManager )
            {
                LINKMANAGER->addLink(m_actor->getRef(), child->getRef());
            }

            EventOnLink onLink;
            onLink.setChild(child->getRef());
            onLink.setLinked(btrue);
            m_actor->onEvent(&onLink);
        }
    }

    void LinkComponent::removeChild( const ObjectPath& _child, bbool _removeFromLinkManager /*= btrue*/ )
    {
        ChildrenList::iterator it = m_children.begin();

        while ( it != m_children.end() )
        {
            const ObjectPath& path = it->getPath();

            if ( path == _child )
            {
                m_children.erase(it);

                Pickable* child;

                if ( _child.getIsAbsolute() )
                {
                    child = SceneObjectPathUtils::getObjectFromAbsolutePath(_child);
                }
                else
                {
                    child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,_child);
                }

                if ( child )
                {
                    if (_removeFromLinkManager)
                    {
                        LINKMANAGER->removeLink(m_actor->getRef(), child->getRef());
                    }

                    EventOnLink onLink;
                    onLink.setChild(child->getRef());
                    onLink.setLinked(bfalse);
                    m_actor->onEvent(&onLink);
                }


                break;
            }
            else
            {
                ++it;
            }
        }
    }

    void LinkComponent::sendEventToChildren( Event* _event, bbool _discardChildWithTags )
    {
        for (u32 i = 0; i < m_children.size(); i++)
        {
            ChildEntry& childEntry = m_children[i];

            if ( _discardChildWithTags && !childEntry.getTagValueList().empty() )
            {
                continue;
            }

            if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, childEntry.getPath()) )
            {
                child->onEvent(_event);
            }
        }
    }

    void LinkComponent::sendEventToChildren( Event* _event, StringID _filterTag )
    {
        for (u32 i = 0; i < m_children.size(); i++)
        {
            const ChildEntry & entry = m_children[i];
            if (entry.hasTag(_filterTag))
            {
                const ObjectPath & path = entry.getPath();

                Pickable * child = NULL;

                if ( path.getIsAbsolute() )
                {
                    child = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                }
                else
                {
                    child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                }
                if ( child)
                {
                    child->onEvent(_event);
                }
            }
        }
    }

    bbool LinkComponent::removeTag( const ObjectPath& _child, const StringID& _tag )
    {
        bbool ret = bfalse;
        ChildrenList::iterator it = std::find(m_children.begin(), m_children.end(), _child);
        if(it != m_children.end())
            ret = it->removeTag(_tag);
        return ret;
    }

    void LinkComponent::onActorRemovedFromScene(Scene* /*_pScene*/, bbool _forDeletion)
    {
        if (_forDeletion)
            clearLinks();
    }

    bbool LinkComponent::removeTags( const ObjectPath& _child )
    {
        bbool ret = bfalse;
        ChildrenList::iterator it = std::find(m_children.begin(), m_children.end(), _child);
        if(it != m_children.end())
        {
            it->removeTags();
            ret = btrue;
        }
        return ret;
    }

    void LinkComponent::convertFromPathList()
    {
        if(m_childrenPath.size())
        {
            m_children.clear();
            for(u32 i = 0; i < m_childrenPath.size(); ++i)
            {
                ChildEntry child;
                child.setPath(m_childrenPath[i]);
                m_children.push_back(child);
            }
        }
        m_childrenPath.clear();
    }

    BaseObject * LinkComponent::getChildObject( const ObjectPath& _childPath ) const
    {
        BaseObject* pChild = NULL;
        if( _childPath.getIsAbsolute() )
            pChild = SceneObjectPathUtils::getObjectFromAbsolutePath(_childPath);
        else
            pChild = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, _childPath);

        return pChild;

    }    

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(LinkComponent_Template)

    BEGIN_SERIALIZATION(LinkComponent_Template)
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
        #ifdef ITF_SUPPORT_DEBUGFEATURE
            SERIALIZE_MEMBER("debugColor", m_debugColor);
            SERIALIZE_MEMBER("debugColorSelected", m_debugColorSelected);
            SERIALIZE_MEMBER("debugChildIndex", m_debugChildIndex);
        #endif //!ITF_SUPPORT_DEBUGFEATURE
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    LinkComponent_Template::LinkComponent_Template()
#ifdef ITF_SUPPORT_DEBUGFEATURE
        : m_debugColor(Color::green())
        , m_debugColorSelected(Color::red())
        , m_debugChildIndex(bfalse)
#endif //!ITF_SUPPORT_DEBUGFEATURE
    {
    }

}
