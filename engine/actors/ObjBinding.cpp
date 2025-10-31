#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

ObjBinding::ObjBinding()
: m_parent(ITF_INVALID_OBJREF)
, m_owner(NULL)
{
    #ifndef ITF_FINAL
        m_isUpdating = bfalse;
    #endif
}

ObjBinding::~ObjBinding()
{
    clear();
}

void ObjBinding::bindChild( const ObjectRef& _ref )
{
    ObjBinding* childBinding = getBindingFromObj(_ref);

    // If we assert here we are trying to bind something that is not compatible
    ITF_ASSERT(childBinding);
    // children can only be actors, we can change it if we need to bind other things
    ITF_ASSERT(childBinding->getOwner()->IsClassCRC(Actor::GetClassCRCStatic()));

    u32 numRemoved = m_removed.size();

    for ( u32 i = 0; i < numRemoved; i++ )
    {
        if ( m_removed[i] == _ref )
        {
            // If it was removed just undo the remove and return
            m_removed.eraseNoOrder(i);
            return;
        }
    }

    if ( childBinding )
    {
        bbool childIsAncestor = isAncestor(_ref);
        // Careful not to bind an object on another object that's bound to it
        ITF_ASSERT_MSG(!childIsAncestor, "THIS IS BAD! FIND OUT WHY IT HAPPENED!");
        // You are binding to the same parent, you don't have to bind every frame an actor !
        ITF_ASSERT(m_owner->getRef() != childBinding->m_parent );

        if ( childIsAncestor )
        {
            unbindFromParent();
        }

        BindData data;
        data.m_child = _ref;

        // insert the object at the beginning or at the end of the list depending on its priority
        BaseObject* pRefObj = _ref.getObject();
        ITF_ASSERT(pRefObj && pRefObj->IsClassCRC(Pickable::GetClassCRCStatic()));
        if (((Pickable*)pRefObj)->getObjectGroupIndex() == 0)
            m_children.insert(m_children.begin(), data);
        else
        {
            ITF_ASSERT_MSG(ObjectGroup::GroupIndex_Max == 2, "only works for 2 priorities");
            m_children.push_back(data); // if more than 2 priorities, a scan if needed here
        }

        if ( childBinding->isBound() )
        {
            childBinding->unbindFromParent();
        }

        childBinding->m_parent = m_owner->getRef();
    }
}

void ObjBinding::removePending()
{
    u32 numRemoved = m_removed.size();

    for ( u32 itRemoved = 0; itRemoved < numRemoved; ++itRemoved )
    {
        for ( ChildrenList::iterator it = m_children.begin(); it != m_children.end(); ++it )
        {
            BindData& data = *it;

            if ( data.m_child == m_removed[itRemoved] )
            {
                ObjBinding* childBinding = getBindingFromObj(data.m_child);

                if ( childBinding )
                {
                    if (childBinding->m_parent == m_owner->getRef())  // in case we switched parent in the same frame
                    {
                        childBinding->m_parent = ITF_INVALID_OBJREF;
                    }
                }

                m_children.erase(it);
                break;
            }
        }
    }

    m_removed.clear();
}

void ObjBinding::unbindFromParent()
{
    if ( m_parent != ITF_INVALID_OBJREF )
    {
        ObjBinding* parentBinding = getBindingFromObj(m_parent);

        if ( parentBinding )
        {
            parentBinding->unbindChild(m_owner->getRef());
        }
        else
        {
            m_parent = ITF_INVALID_OBJREF;
        }
    }
}

ObjBinding* ObjBinding::getBindingFromObj( const ObjectRef& _ref )
{
    BaseObject* obj = GETOBJECT(_ref);

    if ( obj )
    {
        if ( Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
        {
            return actor->getBinding();
        }
        else if ( PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
        {
            return poly->getBinding();
        }
    }

    return NULL;
}

void ObjBinding::unbindAllChildren( )
{
    for (ChildrenList::const_iterator it = m_children.begin(); it !=m_children.end(); ++it)
        m_removed.push_back((*it).m_child);
}

u32 ObjBinding::addChildrenInGroupForUpdate( ObjectGroup* _listToFill )
{
    // must return the highest priority group among all the children
    u32 group = ObjectGroup::GroupIndex_Max;

    #ifndef ITF_FINAL
        m_isUpdating = btrue;
    #endif

    removePending();

    for ( ChildrenList::const_iterator it = m_children.begin(); it != m_children.end(); ++it )
    {
        const BindData& data = *it;
        Actor* actor = static_cast<Actor*>(GETOBJECT(data.m_child));

        if ( actor )
        {
            // It's possible that the actor is not active. For example an npc bound to a polyline, the polyline
            // can be active because it's big and can be activated before the NPC
            if ( actor->isActive() && actor->isEnabled() && actor->isPhysicalReady() && !actor->isParentBindRunTimeDisabled() )
            {
                u32 actorGroup = actor->addObjectInGroupForUpdate(_listToFill, btrue);
                if (actorGroup < group)
                    group = actorGroup;
            }
        }
    }

    removePending();

    #ifndef ITF_FINAL
        m_isUpdating = bfalse;
    #endif

    return group;
}

void ObjBinding::updateChildren( f32 _deltaTime )
{
    #ifndef ITF_FINAL
        m_isUpdating = btrue;
    #endif

    removePending();

    for ( ChildrenList::const_iterator it = m_children.begin(); it != m_children.end(); ++it )
    {
        const BindData& data = *it;
        Actor* actor = static_cast<Actor*>(GETOBJECT(data.m_child));

        if ( actor )
        {
            // It's possible that the actor is not active. For example an npc bound to a polyline, the polyline
            // can be active because it's big and can be activated before the NPC
            if ( actor->isActive() && actor->isEnabled() )
                actor->update(_deltaTime);
        }
    }

    removePending();

    #ifndef ITF_FINAL
        m_isUpdating = bfalse;
    #endif
}

void ObjBinding::forceRemovePending() 
{
#ifndef ITF_FINAL
    ITF_ASSERT(!m_isUpdating);
    if (!m_isUpdating)
#endif
        removePending();
}


// We need to force it when we reload the actor
void ObjBinding::clear()
{
    unbindFromParent();
    unbindAllChildren();
    removePending();
}

bbool ObjBinding::isAncestor( ObjectRef _actor )
{
    if (_actor == m_parent)
    {
        return btrue;
    }

    if ( ObjBinding* parentBinding = getBindingFromObj(m_parent) )
    {
        return parentBinding->isAncestor(_actor);
    }

    return bfalse;
}

bbool ObjBinding::isDescendant( ObjectRef _actor )
{
    for ( ChildrenList::const_iterator it = m_children.begin(); it != m_children.end(); ++it )
    {
        ObjectRef child = (*it).m_child;

        if ( _actor == child )
        {
            return btrue;
        }

        if ( ObjBinding* childBinding = getBindingFromObj(child) )
        {
            if ( childBinding->isDescendant(_actor) )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

#ifdef ITF_SUPPORT_EDITOR
void ObjBinding::onEditorMove(bbool _modifyInitialPos)
{
    removePending();

    for ( ChildrenList::const_iterator it = m_children.begin(); it != m_children.end(); ++it )
    {
        const BindData& data = *it;
        Actor* actor = static_cast<Actor*>(data.m_child.getObject());

        if ( actor )
            actor->onEditorMove(_modifyInitialPos);
    }

    removePending();
}
#endif // ITF_SUPPORT_EDITOR

}