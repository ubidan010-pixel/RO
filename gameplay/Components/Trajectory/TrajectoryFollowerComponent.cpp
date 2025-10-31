#include "precompiled_gameplay.h"

#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryFollowerComponent.h"
#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef ITF_CONSOLE_FINAL
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectoryFollowerComponent)
BEGIN_SERIALIZATION_CHILD(TrajectoryFollowerComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(TrajectoryFollowerComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
// NB[LaurentCou]: please update deactivate() accordingly
TrajectoryFollowerComponent::TrajectoryFollowerComponent()
    : Super()
    , m_activated(bfalse)
    , m_nodeList()
    , m_data()
    , m_linkCursorJustReset(bfalse)
    , m_speed(0.0f)
    , m_targetSpeed(0.0f)
    , m_useRuntimeSpeed(bfalse)
    , m_isWaiting(bfalse)
    , m_flipDirection(bfalse)
    , m_cameraPreviousPos(Vec2d::Zero)
    , m_cameraOffset(Vec2d::Zero)
    , m_activateNextTime(bfalse)
    , m_linkComponent(NULL)
    , m_animatedComponent(NULL)
{
}

//------------------------------------------------------------------------------
TrajectoryFollowerComponent::~TrajectoryFollowerComponent()
{
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    
    // mandatory, validated
    m_linkComponent = m_actor->GetComponent<LinkComponent>();
    
    // optional
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    
    // register to anim input update
    // NB[LaurentCou]: only one component can update anim input at a time,
    // so for actors that also have an AIComponent, our updateAnimInput
    // method has to be called from it
    if (getTemplate()->getUpdatesAnimInput() && m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }
    
    m_data.setPos(m_actor->getPos());
    m_data.setAngle(m_actor->getAngle());
    m_data.setFlip(m_actor->isFlipped());
    m_flipDirection = m_actor->isFlipped();

    m_speed = 0.0f;
    m_targetSpeed = getTemplate()->getSpeed();
    m_activateNextTime = getTemplate()->getStartsActivated();

}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);
    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        processTrigger(triggerEvt);
    }
}


//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if (m_activateNextTime)
    {
        m_activateNextTime = bfalse;
        activate();
    }

    // don't touch actor if not activated; fixes annoying snap-back
    if (!m_activated)
    {
        return;
    }

    if (getTemplate()->getCameraRelative())
    {
        const Vec2d pos = CAMERA->getPos().truncateTo2D();
        const Vec2d offset = (pos - m_cameraPreviousPos);
        m_cameraPreviousPos = pos;

        m_cameraOffset += offset;
    }

    updatePosAndAngle(_deltaTime,m_cameraOffset.to3d());

    f32 angle;
    if (getTemplate()->getUpdatesAngle())
        angle = m_data.getAngle();
    else
        angle = m_actor->getAngle();

    Vec3d pos = m_data.getPos();
    if (!getTemplate()->getUpdatesZ())
    {
        pos.m_z = m_actor->getDepth();
    }
    m_actor->updateComponentPosAngle(pos, angle, NULL);
    m_actor->setFlipped(m_data.getFlip());
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::updateAnimInput()
{
    if (!m_animatedComponent)
    {
        return;
    }

    if (m_linkCursorJustReset)
    {
        // NB[LaurentCou]: if link cursor was just reset, wait one frame
        // before affecting the link cursor, as actors having an AIComponent
        // will need one more frame to reflect the node change in anim
        m_linkCursorJustReset = bfalse;
    }
    else
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(LinkCursor,2567273014), m_data.getLinkCursor());
    }
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::activate()
{
    AIUtils::LinkIterator iterator(m_linkComponent, btrue);
    while (Actor* child = iterator.getNextRelativeActor())
    {
        if (TrajectoryNodeComponent* node = child->GetComponent<TrajectoryNodeComponent>())
        {
            activate(node);
            break;
        }
    }
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::activate(const ActorRefList& _nodeList)
{
    ITF_ASSERT(_nodeList.size() > 0);
    if (_nodeList.size() == 0)
    {
        return;
    }

    m_nodeList = _nodeList; // copy refs

    TrajectoryNodeComponent* node = getNode(m_nodeList[0]);
    activate(node);
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::initFlip()
{
    // flip actor and direction if going backwards on a link
    // NB: only if was given a node list, use the flip tag for regular,
    // straight-ahead cases
    if (m_nodeList.size() > 0)
    {
        if (m_data.getCurrentLink().isBackwards() && !m_actor->isFlipped())
        {
            m_actor->setFlipped(btrue);
        }
        else if (!m_data.getCurrentLink().isBackwards() && m_actor->isFlipped())
        {
            m_actor->setFlipped(bfalse);
        }
    }

    m_data.setFlip(m_actor->isFlipped());
    m_flipDirection = m_actor->isFlipped();
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::activate(TrajectoryNodeComponent* _node)
{
    ITF_ASSERT(!m_activated);

    TrajectoryNodeComponent* next = selectNextNode(_node);
    TrajectoryNodeComponent* nextNext = selectNextNode(next);
    m_data.init(NULL, _node, next, nextNext);

    initFlip();

    m_cameraPreviousPos = CAMERA->getPos().truncateTo2D();
    m_cameraOffset = Vec2d::Zero;

    m_activated = btrue;
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::deactivate()
{
    ITF_ASSERT(m_activated);

    m_activated = bfalse;
    m_nodeList.clear();
    m_data.reset();
    m_linkCursorJustReset = bfalse;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_useRuntimeSpeed = bfalse;
    m_isWaiting = bfalse;
    m_flipDirection = bfalse;
    m_cameraPreviousPos = Vec2d::Zero;
    m_cameraOffset = Vec2d::Zero;
    m_activateNextTime = bfalse;
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::reverse()
{
    u32 count = m_nodeList.size();
    ITF_ASSERT(count >= 2);
    if (count < 2)
    {
        return;
    }

    // reverse node list
    u32 halfCount = count / 2;
    for (u32 i=0; i<halfCount; ++i)
    {
        std::swap(m_nodeList[i], m_nodeList[count - i - 1]);
    }

    // reverse trajectory data
    m_data.reverse();

    // reset speed
    m_speed = m_targetSpeed = 0.0f;

    // flip
    initFlip();
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void TrajectoryFollowerComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const
{
    Super::drawEdit(_drawInterface, _flags);
    
    // debug draw speed & target speed
    DebugDraw::text(
        m_actor->getPos(),
        Color::black(), "speed=%f, target=%f", m_speed, m_targetSpeed
        );
}
#endif

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::setRuntimeSpeed(f32 _speed, bbool _smooth)
{
    if (_smooth)
    {
        m_speed = m_targetSpeed = _speed;
    }
    else
    {
        m_targetSpeed = _speed;
    }
    m_useRuntimeSpeed = btrue;
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::processTrigger(EventTrigger* _triggerEvt)
{
    // skip deactivate event
    if (!_triggerEvt->getActivated())
    {
        return;
    }

    // skip re-trigger
    if (m_activated && getTemplate()->getSkipRetrigger())
    {
        return;
    }

    // we're supposed to link to a node: get it
    activate();
    ITF_WARNING(
        m_actor, m_activated,
        "Could not get the initial node, did you link to it?"
        );
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryFollowerComponent::selectNextNode(TrajectoryNodeComponent* _node)
{
    if (!_node)
    {
        return NULL;
    }

    if (m_nodeList.size() == 0)
    {
        // if no node list was specified, select the first children
        TrajectoryNodeComponent::NodeList children;
        _node->getChildNodes(children, btrue);
        if (children.size() > 0)
        {
            return children[0];
        }
    }
    else
    {
        // if a node list was specified, select the next node in the list
        u32 nodeListSize = m_nodeList.size();
        for (u32 i=0; i<(nodeListSize-1); ++i) // to penultimate
        {
            TrajectoryNodeComponent* node = getNode(m_nodeList[i]);
            if (node == _node)
            {
                return getNode(m_nodeList[i+1]);
            }
        }
    }

    return NULL; // no next node
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* TrajectoryFollowerComponent::getNode(const ActorRef& _nodeActor)
{
    Actor* actor = _nodeActor.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

//------------------------------------------------------------------------------
void TrajectoryFollowerComponent::updatePosAndAngle(f32 _dt, const Vec3d & _cameraOffset)
{
    // current gives us the order type, next gives us the destination
    TrajectoryNodeComponent* current = m_data.getCurrentNode();
    TrajectoryNodeComponent* next = m_data.getNextNode();

    ITF_ASSERT(current);
    if (!current)
    {
        return;
    }
    
    if (!next)
    {
        m_data.setPos(current->GetActor()->getPos());
        m_data.setAngle(current->GetActor()->getAngle());
        return;
    }

    if (!m_useRuntimeSpeed)
    {
        if (m_data.getCurrentLink().getSpeed() != -1.0f)
        {
            m_targetSpeed = m_data.getCurrentLink().getSpeed();
        }
        else if (getTemplate()->getSafeDistance() > 0.0f)
        {
            // keep safe distance in a similar fashion than Chest2 AI (RO-13180)
            f32 minDot;
            AIUtils::getClosestPursuer(m_actor, minDot);
            f32 distanceFactor = minDot / getTemplate()->getSafeDistance();
            m_targetSpeed = Interpolate(
                getTemplate()->getMinSpeed(),
                getTemplate()->getMaxSpeed(),
                Clamp(1.0f - distanceFactor, 0.0f, 1.0f));
        }
        else
        {
            m_targetSpeed = getTemplate()->getSpeed();
        }
    }

    // blend current speed to target speed
    AIUtils::michelSmooth(
        m_speed,
        m_targetSpeed,
        getTemplate()->getSpeedSmoothFactor());
    
    m_data.setCurrentNodeDist(m_data.getCurrentNodeDist() + m_speed * _dt);
    
    bbool flip = m_actor->isFlipped();
    
    Vec3d newPos;
    Vec3d dir3d = Vec3d::Zero;
    while (current->move(m_data, m_isWaiting, newPos, dir3d, flip))
    {
        // flip direction *after* node change
        m_flipDirection = flip;

        // set current node/data
        TrajectoryNodeComponent* previous = current;
        current = next;
        next = selectNextNode(current);
        TrajectoryNodeComponent* nextNext = selectNextNode(next);
        m_data.init(previous, current, next, nextNext);
        
        // start anim if set
        if (m_animatedComponent &&
            m_data.getCurrentLink().getAnim().isValid())
        {
            m_animatedComponent->setAnim(m_data.getCurrentLink().getAnim());
        }
        else
        {
            // delay of one frame only if anim is handled by another component
            m_linkCursorJustReset = btrue;
        }
        
        if (!next)
        {
            if (getTemplate()->getDestroyAtEnd())
            {
                m_actor->requestDestruction();
            }
        
            return;
        }
    }

    Vec2d dir = dir3d.truncateTo2D();
    dir.normalize();
    f32 angle = dir.getAngle();
    if (m_flipDirection)
    {
        angle += MTH_PI;
    }
    NormalizeAngle2PI(angle);
    
    // re-apply forced Z
    const Actor_Template* actorTemplate = m_actor->getTemplate();
    if (actorTemplate->isZForced())
    {
        newPos.m_z += actorTemplate->getForcedZ();
    }

    m_data.setPos(newPos + _cameraOffset);
    m_data.setAngle(angle);
    m_data.setFlip(flip);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TrajectoryFollowerComponent_Template)
BEGIN_SERIALIZATION(TrajectoryFollowerComponent_Template)
    SERIALIZE_MEMBER("speed",m_speed);
    SERIALIZE_MEMBER("speedSmoothFactor", m_speedSmoothFactor);
    SERIALIZE_MEMBER("cameraRelative", m_cameraRelative);
    SERIALIZE_MEMBER("skipRetrigger", m_skipRetrigger);
    SERIALIZE_MEMBER("destroyAtEnd", m_destroyAtEnd);
    SERIALIZE_MEMBER("updatesAnimInput", m_updatesAnimInput);
    SERIALIZE_MEMBER("startsActivated", m_startsActivated);
    SERIALIZE_MEMBER("updatesAngle", m_updatesAngle);
    SERIALIZE_MEMBER("updatesZ", m_updatesZ);
    SERIALIZE_MEMBER("safeDistance", m_safeDistance);
    SERIALIZE_MEMBER("minSpeed", m_minSpeed);
    SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
END_SERIALIZATION()

//------------------------------------------------------------------------------
TrajectoryFollowerComponent_Template::TrajectoryFollowerComponent_Template()
    : m_speed(1.0f)
    , m_speedSmoothFactor(1.0f)
    , m_cameraRelative(bfalse)
    , m_skipRetrigger(bfalse)
    , m_destroyAtEnd(bfalse)
    , m_updatesAnimInput(btrue)
    , m_startsActivated(bfalse)
    , m_updatesAngle(btrue)
    , m_updatesZ(btrue)
    , m_safeDistance(0.0f)
    , m_minSpeed(0.0f)
    , m_maxSpeed(1.0f)
{
}

} // namespace ITF
