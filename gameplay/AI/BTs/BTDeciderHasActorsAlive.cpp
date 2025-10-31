
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDERHASACTORSALIVE_H_
#include "gameplay/ai/BTs/BTDeciderHasActorsAlive.h"
#endif // _ITF_BTDECIDERHASACTORSALIVE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDeciderHasActorsAlive);

BTDeciderHasActorsAlive::BTDeciderHasActorsAlive()
{
}

BTDeciderHasActorsAlive::~BTDeciderHasActorsAlive()
{
}

bbool BTDeciderHasActorsAlive::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFactActor().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factActor",getTemplate()->getFactActor().isValid(),"%s: the fact to check the actor status is not set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool BTDeciderHasActorsAlive::decide( f32 _dt )
{
    ObjectRef actorCheck = ITF_INVALID_OBJREF;

    if ( !m_tree->getBlackBoard().getFact(getTemplate()->getFactActor(),actorCheck) )
    {
        return bfalse;
    }

    Actor* checkActor = AIUtils::getActor(actorCheck);

    if ( !checkActor )
    {
        return bfalse;
    }

    LinkComponent* linkComponent = checkActor->GetComponent<LinkComponent>();
    bbool invert = getTemplate()->getInvert();
    EventQueryIsDead query;

    if ( !linkComponent )
    {
        checkActor->onEvent(&query);

        if ( invert )
        {
            return query.getIsDead();
        }
        else
        {
            return !query.getIsDead();
        }
    }

    const LinkComponent::ChildrenList& children = linkComponent->getChildren();
    u32 numChildren = children.size();
    u32 numMatching = 0;
    u32 numTotal = 0;

    for ( u32 i = 0; i < numChildren; i++ )
    {
        Pickable* child;
        const ObjectPath& childPath = children[i].getPath();

        if ( childPath.getIsAbsolute() )
        {
            child = SceneObjectPathUtils::getObjectFromAbsolutePath(childPath);
        }
        else
        {
            child = SceneObjectPathUtils::getObjectFromRelativePath(checkActor,childPath);
        }

        if ( child )
        {
            Actor* childActor = child->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            if ( childActor )
            {
                numTotal++;

                childActor->onEvent(&query);

                if ( invert )
                {
                    if ( query.getIsDead() )
                    {
                        numMatching++;
                    }
                }
                else
                {
                    if ( !query.getIsDead() )
                    {
                        numMatching++;
                    }
                }
            }
        }
    }

    if ( getTemplate()->getCheckAll() )
    {
        return numMatching == numTotal;
    }
    else
    {
        return numMatching > 0;
    }
}

IMPLEMENT_OBJECT_RTTI(BTDeciderHasActorsAlive_Template);

BEGIN_SERIALIZATION_CHILD(BTDeciderHasActorsAlive_Template)

    SERIALIZE_MEMBER("invert",m_invert);
    SERIALIZE_MEMBER("checkAll",m_checkAll);
    SERIALIZE_MEMBER("factActor",m_factActor);

END_SERIALIZATION()

}
