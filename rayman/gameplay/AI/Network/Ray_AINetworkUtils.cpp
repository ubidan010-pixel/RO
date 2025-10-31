
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINETWORKUTILS_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkUtils.h"
#endif //_ITF_RAY_AINETWORKUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

namespace Ray_AINetworkUtils
{

struct PathStep
{
    Pickable*           m_from;
    Pickable*           m_to;
    WaypointLinkType    m_linkType;
    f32                 m_distToTarget;
    bbool               m_processed;
};

typedef FixedArray <PathStep,64> StepsList;
typedef ITF_MAP <ObjectRef,bbool> VisitedNodesTable;

bbool processLink( Pickable* _from, Pickable* _to, Pickable* _end, WaypointLinkType _linkType, StepsList& _steps )
{
    PathStep newStep;

    newStep.m_from = _from;
    newStep.m_to = _to;
    newStep.m_linkType = _linkType;
    newStep.m_processed = bfalse;

    if ( _to == _end )
    {
        newStep.m_distToTarget = 0.f;
        newStep.m_processed = btrue;
        _steps.insert(newStep,0);
        return btrue;
    }

    if ( _to->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) == NULL )
    {
        return bfalse;
    }

    newStep.m_distToTarget = ( _to->get2DPos() - _end->get2DPos() ).sqrnorm();
    u32 numSteps = _steps.size();

    for ( u32 i = 0; i < numSteps; i++ )
    {
        PathStep& step = _steps[i];

        if ( step.m_distToTarget > newStep.m_distToTarget )
        {
            _steps.insert(newStep,i);
            return bfalse;
        }
    }

    _steps.push_back(newStep);

    return bfalse;
}

WaypointLinkType getWaypointLinkType( const StringID& _name )
{
    if ( _name == ITF_GET_STRINGID_CRC(jump,2374255179) )
    {
        return WAYPOINTLINKTYPE_JUMP;
    }
    else if ( _name == ITF_GET_STRINGID_CRC(wallJump,2370399201) )
    {
        return WAYPOINTLINKTYPE_WALLJUMP;
    }
    else if ( _name == ITF_GET_STRINGID_CRC(wallRun,1710053959) )
    {
        return WAYPOINTLINKTYPE_WALLRUN;
    }
    else
    {
        return WAYPOINTLINKTYPE_WALK;
    }
}

bbool processActor( Actor* _actor, Pickable* _end, StepsList& _steps, VisitedNodesTable& _visitedNodes )
{
    _visitedNodes[_actor->getRef()] = btrue;

    LinkComponent* linkComponent = _actor->GetComponent<LinkComponent>();

    if ( linkComponent )
    {
        const LinkComponent::ChildrenList& children = linkComponent->getChildren();
        u32 numChildren = children.size();

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
                child = SceneObjectPathUtils::getObjectFromRelativePath(_actor,childPath);
            }

            if ( child )
            {
                StringID tagValue;

                linkComponent->getTagValue(childPath,ITF_GET_STRINGID_CRC(waypointLinkType,3578390329),tagValue);
                
                WaypointLinkType linkType = getWaypointLinkType(tagValue);

                if ( processLink(_actor,child,_end,linkType,_steps) )
                {
                    return btrue;
                }
            }
        }
    }

    return bfalse;
}

void buildPath( Actor* _start, const StepsList& _steps, WaypointPath& _path )
{
    PathWaypointInfo info;

    info.m_obj = _steps[0].m_from;
    info.m_linkType = _steps[0].m_linkType;
    _path.push_back(info);

    info.m_obj = _steps[0].m_to;
    _path.push_back(info);

    if ( _steps[0].m_from != _start )
    {
        bbool done = bfalse;
        u32 numSteps = _steps.size();
        Pickable* prevStep = _steps[0].m_from;

        while ( !done )
        {
            for ( u32 i = 0; i < numSteps; i++ )
            {
                const PathStep& step = _steps[i];

                if ( step.m_to == prevStep )
                {
                    info.m_obj = step.m_from;
                    info.m_linkType = step.m_linkType;
                    _path.insert(info,0);
                    prevStep = step.m_from;

                    if ( info.m_obj == _start )
                    {
                        done = btrue;
                    }

                    break;
                }
            }
        }
    }
}

bbool pathFind( Actor* _start, Pickable* _end, WaypointPath& _path )
{
    VisitedNodesTable visitedNodes;

    _path.clear();

    StepsList steps;
    
    if ( processActor(_start,_end,steps,visitedNodes) )
    {
        buildPath(_start,steps,_path);
    }
    else
    {
        bbool done = steps.empty();
        bbool someProcessed;

        while ( !done )
        {
            u32 numSteps = steps.size();

            someProcessed = bfalse;

            for ( u32 i = 0; i < numSteps; i++ )
            {
                PathStep& step = steps[i];

                if ( step.m_processed )
                {
                    continue;
                }
                else
                {
                    someProcessed = btrue;
                    step.m_processed = btrue;

                    if ( visitedNodes.find(step.m_to->getRef()) != visitedNodes.end() )
                    {
                        continue;
                    }

                    if ( processActor(static_cast<Actor*>(step.m_to),_end,steps,visitedNodes) )
                    {
                        buildPath(_start,steps,_path);
                        done = btrue;
                    }
                    break;
                }
            }

            if ( !someProcessed )
            {
                done = btrue;
            }
        }
    }

    return !_path.empty();
}

}

}

