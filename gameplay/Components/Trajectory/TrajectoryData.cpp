#include "precompiled_gameplay.h"

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_TRAJECTORYSEGMENT_H_
#include "gameplay/Components/Trajectory/TrajectorySegment.h"
#endif //_ITF_TRAJECTORYSEGMENT_H_

namespace ITF {

//------------------------------------------------------------------------------
TrajectoryLinkInfo::TrajectoryLinkInfo()
    : m_isValid(bfalse)
    , m_backwards(bfalse)
    , m_childEntry()
    , m_type(TrajectoryLinkType_Walk)
    , m_speed(-1.0f)
    , m_anim()
    , m_flip(bfalse)
{
}

//------------------------------------------------------------------------------
void TrajectoryLinkInfo::reset()
{
    m_isValid = bfalse;
    m_backwards = bfalse;
    m_childEntry = ChildEntry();
    m_type = TrajectoryLinkType_Walk;
    m_speed = -1.0f;
    m_anim.invalidate();
    m_flip = bfalse;
}

//------------------------------------------------------------------------------
void TrajectoryLinkInfo::init(const TrajectoryNodeComponent* _before, const TrajectoryNodeComponent* _after)
{
    // reset info
    reset();

    // give up if one node is missing
    if (!_before || !_after)
    {
        return;
    }
    
    // give up if child entry not found
    if (!findChildEntry(_before, _after) &&
        !findChildEntry(_after, _before))
    {
        return;
    }

    // remember if going backwards on the link
    m_backwards = (_before->GetActor()->getPos().m_x > _after->GetActor()->getPos().m_x);

    // get link type
    StringID type; 
    if (m_childEntry.getTagValue(ITF_GET_STRINGID_CRC(type,2818264794), type))
    {
        m_type = type;
    }

    // get link speed
    f32 speed; 
    if (m_childEntry.getTagValue(ITF_GET_STRINGID_CRC(speed,4218325830), speed))
    {
        m_speed = speed;
    }

    // get link anim
    StringID anim; 
    if (m_childEntry.getTagValue(ITF_GET_STRINGID_CRC(anim,152385505), anim))
    {
        m_anim = anim;
    }

    // get flip
    // NB[LaurentCou]: use *bool* (not *bbool*) to use the
    // proper specialization for getTagValue, ie. the tag has
    // to be 'true' or 'false'
    bool flip; 
    if (m_childEntry.getTagValue(ITF_GET_STRINGID_CRC(flip,534660332), flip))
    {
        m_flip = flip;
    }

    // validate
    m_isValid = btrue;
}

//------------------------------------------------------------------------------
bbool TrajectoryLinkInfo::useCurvedEnd() const
{
    return m_isValid && (
        m_type == TrajectoryLinkType_Walk ||
        m_type == TrajectoryLinkType_Fly ||
        m_type == TrajectoryLinkType_Dive ||
        m_type == TrajectoryLinkType_Swim);
}

//------------------------------------------------------------------------------
bbool TrajectoryLinkInfo::findChildEntry(const TrajectoryNodeComponent* _from, const TrajectoryNodeComponent* _to)
{
    // skip if link component is missing
    LinkComponent* linkComponent = _from->GetActor()->GetComponent<LinkComponent>();
    if (!linkComponent)
    {
        return bfalse;
    }

    // get child entry
    bbool found = bfalse;
    const LinkComponent::ChildrenList& children  = linkComponent->getChildren();
    for (u32 i = 0; i < children.size(); i++)
    {
        Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(
            _from->GetActor(),
            children[i].getPath()
            );
        if (pickable)
        {
            Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if (childActor && (childActor == _to->GetActor()))
            {
                ITF_ASSERT(childActor->GetComponent<TrajectoryNodeComponent>());

                // found, done
                m_childEntry = children[i];
                found = btrue;
                return btrue;
            }
        }
    }
    return bfalse;
}

//------------------------------------------------------------------------------
TrajectoryData::TrajectoryData()
    : m_previousNode()
    , m_currentNode()
    , m_nextNode()
    , m_nextNextNode()
    , m_previousLink()
    , m_currentLink()
    , m_nextLink()
    , m_currentNodeDist(0.0f)
    , m_linkCursor(0.0f)
    , m_isUpsideDown(bfalse)
    , m_entryPos(Vec3d::Zero)
    , m_pos(Vec3d::Zero)
    , m_angle(0.0f)
    , m_flip(bfalse)
{
}

//------------------------------------------------------------------------------
void TrajectoryData::reset()
{
    // reset
    m_previousNode.invalidate();
    m_currentNode.invalidate();
    m_nextNode.invalidate();
    m_nextNextNode.invalidate();
    m_previousLink.reset();
    m_currentLink.reset();
    m_nextLink.reset();
    m_currentNodeDist = 0.0f;
    m_linkCursor = 0.0f;
    m_isUpsideDown = bfalse;
    m_entryPos = Vec3d::Zero;
    m_pos = Vec3d::Zero;
    m_angle = 0.0f;
    m_flip = bfalse;
}

//------------------------------------------------------------------------------
void TrajectoryData::init(
    const TrajectoryNodeComponent* _previousNode,
    const TrajectoryNodeComponent* _currentNode,
    const TrajectoryNodeComponent* _nextNode,
    const TrajectoryNodeComponent* _nextNextNode
    )
{
    ITF_ASSERT(_currentNode);
    if (!_currentNode)
    {
        reset();
        return;
    }
    
    // init node refs
    m_previousNode.invalidate();
    if (_previousNode) m_previousNode = _previousNode->GetActor()->getRef();
    m_currentNode.invalidate();
    if (_currentNode) m_currentNode = _currentNode->GetActor()->getRef();
    m_nextNode.invalidate();
    if (_nextNode) m_nextNode = _nextNode->GetActor()->getRef();
    m_nextNextNode.invalidate();
    if (_nextNextNode) m_nextNextNode = _nextNextNode->GetActor()->getRef();

    // init link info
    m_previousLink.init(_previousNode, _currentNode);
    m_currentLink.init(_currentNode, _nextNode);
    m_nextLink.init(_nextNode, _nextNextNode);
    
    // reset entry pos & link cursor
    m_entryPos = _currentNode->GetActor()->getPos();
    m_linkCursor = 0.0f;
}

//------------------------------------------------------------------------------
void TrajectoryData::reverse()
{
    f32 previousCursor = m_linkCursor;
    f32 previousNodeDist = m_currentNodeDist;
    TrajectorySegment previousSegment(*this);

    TrajectoryNodeComponent* previousNode = getPreviousNode();
    TrajectoryNodeComponent* currentNode = getCurrentNode();
    TrajectoryNodeComponent* nextNode = getNextNode();
    TrajectoryNodeComponent* nextNextNode = getNextNextNode();

    reset();
    init(
        nextNextNode,
        nextNode,
        currentNode,
        previousNode
    );
    
    m_linkCursor = 1.0f - previousCursor;
    m_currentNodeDist = previousSegment.getTotalLength() - previousNodeDist;
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryData::getPreviousNode() const
{
    Actor* actor = m_previousNode.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryData::getCurrentNode() const
{
    Actor* actor = m_currentNode.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryData::getNextNode() const
{
    Actor* actor = m_nextNode.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryData::getNextNextNode() const
{
    Actor* actor = m_nextNextNode.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

} // namespace ITF
