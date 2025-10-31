#include "precompiled_gameplay.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TRAJECTORYSEGMENT_H_
#include "gameplay/Components/Trajectory/TrajectorySegment.h"
#endif //_ITF_TRAJECTORYSEGMENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectoryNodeComponent)
BEGIN_SERIALIZATION_CHILD(TrajectoryNodeComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(TrajectoryNodeComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent != NULL, "LinkComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
TrajectoryNodeComponent::TrajectoryNodeComponent()
    : Super()
    , m_linkComponent(NULL)
{
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent::~TrajectoryNodeComponent()
{
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    m_linkComponent = m_actor->GetComponent<LinkComponent>();
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::Update( f32 _dt )
{
    Super::Update(_dt);

#ifdef ITF_SUPPORT_DEBUGFEATURE
    if (s_drawDebug)
    {
        // NOTE: this method fails if the root is not updated.
        //if ( !hasParentNode() )
        //{
        //    ProcessedNodeList processedNodes;
        //    drawChildren(NULL, processedNodes);
        //}

        // NOTE: this will cause one complete draw per node... never mind.
        ProcessedNodeList processedNodes;
        drawDebugRoot(processedNodes);
    }
#endif // ITF_SUPPORT_DEBUGFEATURE
}

//------------------------------------------------------------------------------
bbool TrajectoryNodeComponent::useCurvedEnd() const
{
    return getTemplate()->useCurvedEnd();
}

//------------------------------------------------------------------------------
bbool TrajectoryNodeComponent::hasParentNode() const
{
    NodeList parents;
    getParentNodes(parents, btrue);
    return (parents.size() > 0);
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::getParentNodes( NodeList& _nodes, bbool _bOnlyFirst ) const
{
    // get parents
    ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef());
    if (!parents)
    {
        return;
    }

    // iterate on parents
    for (u32 i=0; i<parents->size(); ++i)
    {
        if (BaseObject* parentObj = (*parents)[i].getObject())
        {
            if (Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
            {
                // get object path from parent to self
                ObjectPath path;
                SceneObjectPathUtils::getRelativePathFromObject(parent, m_actor, path);

                // link tag "disabled" used to skip some links
                LinkComponent* linkComponent = parent->GetComponent<LinkComponent>();
                bool disabled = false;
                if (linkComponent->getTagValue(path, ITF_GET_STRINGID_CRC(disabled,3798487816), disabled) && disabled)
                {
                    continue;
                }
            
                // link tag "oneWay" to forbid coming up a link (e.g. Chest2 AI, RO-9905)
                bool oneWay = false;
                if (linkComponent->getTagValue(path, ITF_GET_STRINGID_CRC(oneWay,4050598902), oneWay) && oneWay)
                {
                    continue;
                }

                // append if there's a node component    
                if (TrajectoryNodeComponent* parentNode = parent->GetComponent<TrajectoryNodeComponent>())
                {
                    _nodes.push_back(parentNode);
                    if(_bOnlyFirst)
                        return;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
bbool TrajectoryNodeComponent::hasChildNode() const
{
    NodeList children;
    getChildNodes(children, btrue);
    return (children.size() > 0);
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::getChildNodes( NodeList& _nodes, bbool _bOnlyFirst ) const
{
    // iterate on children
    const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
    for (u32 i=0; i<children.size(); ++i)
    {
        // link tag "disabled" used to skip some links
        bool disabled = false;
        if (children[i].getTagValue(ITF_GET_STRINGID_CRC(disabled,3798487816), disabled) && disabled)
        {
            continue;
        }

        // append if there's a node component    
        Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
        if (pickable )
        {
            Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if (childActor)
            {
                if (TrajectoryNodeComponent* childNode = childActor->GetComponent<TrajectoryNodeComponent>())
                {
                    _nodes.push_back(childNode);
                    if(_bOnlyFirst)
                        return;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
bbool TrajectoryNodeComponent::move(
    TrajectoryData& _data,
    const bbool& _isWaiting,
    Vec3d& _newPos,
    Vec3d & _tangent,
    bbool& _flip
    )
{
    ITF_ASSERT(m_actor == _data.getCurrentNodeRef().getActor());

    TrajectorySegment segment(_data);

    bbool toNextNode = bfalse;
    bbool wasBeforeHalf = bfalse;

    f32 dist = _data.getCurrentNodeDist();
    f32 linkCursor = _data.getLinkCursor();
    
    StringID currentType = _data.getCurrentLink().getType();

    if (linkCursor < 0.5f)
    {
        wasBeforeHalf = btrue;
    }
    
    // teleport
    if (currentType == TrajectoryLinkType_Teleport)
    {
        dist = 0.0f;
        toNextNode = btrue;
        linkCursor = 1.0f;
    }
    // move on segment
    else
    {
        // update upside down flag
        if (currentType == TrajectoryLinkType_ToCeiling || currentType == TrajectoryLinkType_ToFloor)
        {
            _data.setUpsideDown(currentType == TrajectoryLinkType_ToCeiling);
        }
   
        f32 totalLength = segment.getTotalLength();
        
        // do not pass node if waiting
        if (_isWaiting)
        {
            dist = Min(totalLength, dist);
        }
        
        if (dist > totalLength)
        {
            // to next node
            ITF_ASSERT(!_isWaiting);
            dist = 0.0f;
            toNextNode = btrue;
            linkCursor = 1.0f;
        }
        else
        {
            // move on link
            segment.getPosAt(dist, _newPos);
            segment.getDirAt(dist, _tangent);
            linkCursor = dist / totalLength;
        }
    }
    
    // flip direction if upside down
    if (_data.getUpsideDown())
    {
        _tangent = -_tangent;
    }
    
    // flip actor if required by link tag
    if (_data.getCurrentLink().getFlip() && wasBeforeHalf && linkCursor >= 0.5f)
    {
        _flip = !_flip;
    }
    
    _data.setCurrentNodeDist(dist);
    _data.setLinkCursor(linkCursor);

    ITF_ASSERT(_newPos.isValid());

    return toNextNode;
}

//------------------------------------------------------------------------------
const StringID& TrajectoryNodeComponent::getName() const
{
    return getTemplate()->getName();
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void TrajectoryNodeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    ProcessedNodeList processedNodes;
    drawDebugRoot(processedNodes);
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::onEditorCreated( Actor* _original )
{
    Super::onEditorCreated(_original);

    if (_original)
    {
        // Clear links towards this
        if(ObjectRefList* pParentFromRefList = LINKMANAGER->getParents(m_actor->getRef()))
        {
            SafeArray<BaseObject*> pParentsFrom;
            ID_SERVER->getObjectListNoNull(*pParentFromRefList, pParentsFrom);

            for(u32 iP = 0; iP < pParentsFrom.size(); ++iP)
            {
                Actor* pParent = static_cast<Actor*>(pParentsFrom[iP]);
                if(LinkComponent* pLC = pParent->GetComponent<LinkComponent>())
                {
                    ObjectPath path;
                    if(SceneObjectPathUtils::getRelativePathFromObject(pParent, m_actor, path))
                        pLC->removeChild(path, btrue);
                }
            }
        }

        if (m_linkComponent)
        {
            m_linkComponent->clearLinks();
        }

        if ( TrajectoryNodeComponent* parent = _original->GetComponent<TrajectoryNodeComponent>() )
        {
            ObjectPath relPath;
            SceneObjectPathUtils::getRelativePathFromObject(_original,m_actor,relPath);
            if ( relPath.isValid() && parent->m_linkComponent )
            {
                parent->m_linkComponent->addChild(relPath);
            }
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_DEBUGFEATURE
//------------------------------------------------------------------------------
bbool TrajectoryNodeComponent::s_drawDebug = bfalse;
const f32 TrajectoryNodeComponent::s_lineWidth = 5.0f;
const f32 TrajectoryNodeComponent::s_corridorWidth = 1.0f;

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::drawDebugRoot( ProcessedNodeList& _processedNodes ) const
{
    if ( _processedNodes.find(this) != -1 )
    {
        // already processed
        return;
    }
    _processedNodes.push_back(this);


    NodeList parents;
    getParentNodes(parents);

    if (parents.size())
    {
        for (u32 i = 0; i < parents.size(); i++)
        {
            parents[i]->drawDebugRoot(_processedNodes);
        }
    }
    else
    {
        ProcessedNodeList processedNodes;
        drawChildren(NULL, processedNodes);
    }
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::drawChildren(
    const TrajectoryNodeComponent* n0,
    ProcessedNodeList& _processedNodes ) const
{
    const TrajectoryNodeComponent* n1 = this;

    NodeList children;
    getChildNodes(children);

    bbool alreadyProcessed = ( _processedNodes.find(this) != -1 );
    if (!alreadyProcessed)
    {
        _processedNodes.push_back(this);
    }

    if (children.size())
    {
        for (u32 i = 0; i < children.size(); i++)
        {
            const TrajectoryNodeComponent* n2 = children[i];
            
            NodeList grandChildren;
            n2->getChildNodes(grandChildren);
            if (grandChildren.size())
            {
                for (u32 j=0; j< grandChildren.size(); ++j)
                {
                    const TrajectoryNodeComponent* n3 = grandChildren[j];
                    drawSegment(n0, n1, n2, n3);
                }
            }
            else
            {
                drawSegment(n0, n1, n2, NULL);
            }
            
            if (!alreadyProcessed)
            {
                n2->drawChildren(n1, _processedNodes);
            }
        }
    }
    else
    {
        // no child: we're an end
        if (n0)
        {
            const Vec3d& n0Pos = n0->GetActor()->getPos();
            const Vec3d& n1Pos = m_actor->getPos();
            Vec3d dir01 = (n1Pos - n0Pos).truncateTo2D().to3d().normalize();
            f32 n1Scale = m_actor->getScale().m_x;

            // end line
            DebugDraw::line3D(n1Pos - n1Scale * dir01, n1Pos, Color::cyan(), s_lineWidth);
        }
    }
}

//------------------------------------------------------------------------------
void TrajectoryNodeComponent::drawSegment(
    const TrajectoryNodeComponent* n0,
    const TrajectoryNodeComponent* n1,
    const TrajectoryNodeComponent* n2,
    const TrajectoryNodeComponent* n3
    ) const
{
    ITF_ASSERT(n1 && n2);
    if (!(n1 && n2))
    {
        return;
    }
    
    TrajectoryData drawData;
    drawData.init(n0, n1, n2, n3);
    
    TrajectorySegment segment(drawData);
    segment.draw(drawData.getCurrentLink().getType(), s_lineWidth, s_corridorWidth);
}
#endif // ITF_SUPPORT_DEBUGFEATURE

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectoryNodeComponent_Template)
BEGIN_SERIALIZATION(TrajectoryNodeComponent_Template)
    SERIALIZE_MEMBER("useCurvedEnd", m_useCurvedEnd);
    SERIALIZE_MEMBER("name", m_name);
END_SERIALIZATION()

//------------------------------------------------------------------------------
TrajectoryNodeComponent_Template::TrajectoryNodeComponent_Template()
    : Super()
    , m_useCurvedEnd(btrue)
    , m_name()
{
}

} // namespace ITF
