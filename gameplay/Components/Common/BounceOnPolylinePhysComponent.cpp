#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_
#include "gameplay/Components/Common/BounceOnPolylinePhysComponent.h"
#endif //_ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/Polyline.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BounceOnPolylinePhysComponent)

BEGIN_SERIALIZATION_CHILD(BounceOnPolylinePhysComponent)
END_SERIALIZATION()

BounceOnPolylinePhysComponent::BounceOnPolylinePhysComponent()
: m_collisionFilter(ECOLLISIONFILTER_NONE)
, m_phantom(NULL)
, m_shape(0.65f)
, m_bouncingSpeed(Vec2d::Zero)
, m_hitDirection(Vec2d::Zero)
, m_prevPos(Vec2d::Zero)
, m_totalWeight(0.0f)
, m_hitLevel(0)
, m_objectsRegistered(bfalse)
{
    m_passengers.reserve(GAMEMANAGER->getMaxPlayerCount());
    setRadiusMultiplier(0.65f);
}

BounceOnPolylinePhysComponent::~BounceOnPolylinePhysComponent()
{
    clear();
}

void BounceOnPolylinePhysComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    clear();
    initPhantom();

    // handle hits
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
}

void BounceOnPolylinePhysComponent::Update( f32 _deltaTime )
{
    if (m_disabled )
    {
        return;
    }

    // actor-bound + physics = unsafe
    ITF_WARNING(
        m_actor,
        !m_actor->getParentBind() || m_actor->getParentBind()->isRuntimeDisabled(),
        "This actor shouldn't be bound because its BounceOnPolylinePhysComponent "
        "updates its position in world coordinates.");

    computeForces(_deltaTime);
    applyForces(_deltaTime);
    alignWithGravity(_deltaTime);
    resetForces();

    checkContacts();

#if defined(DEVELOPER_ALEXIS) && 0
    {
        Vec2d p1 = m_actor->get2DPos();
        Vec2d p2 = p1 + Vec2d(0.0f, .75f);

        // actor position
        GFX_ADAPTER->drawDBGLine(p1, p2 , 1.0f, 0.1f, 0.1f, 1.0f);
        GFX_ADAPTER->drawDBGCircle(p1.m_x, p1.m_y, 0.15f, 0.8f, 0.0f, 0.0f, _deltaTime);
    }
#endif
}

void BounceOnPolylinePhysComponent::onEvent( Event * _event )
{
    if ( HitStim* hitStim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
    {
        storeHitInfo(hitStim);
    }
    else if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processNewUser(stickEvent);
    }

    Super::onEvent(_event);
}

void BounceOnPolylinePhysComponent::updatePhantom()
{
    if (m_phantom)
    {
        m_phantom->setPos3D(m_actor->getPos());
        m_phantom->setAngle(m_actor->getAngle());
        m_phantom->setRecomputeAABB();
        m_phantom->updateAABB();
    }
}

void BounceOnPolylinePhysComponent::storeHitInfo( HitStim* _hit )
{
    if ( !m_phantom )
    {
        return;
    }

    const PhysContactsContainer& contacts = _hit->getContacts();

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( m_phantom->getRef() == contact.m_collidable )
        {
            m_hitDirection = _hit->getDirection();
            m_hitLevel = _hit->getLevel();
            break;
        }
    }
}

void BounceOnPolylinePhysComponent::computeForces( f32 _deltaTime )
{
    PhysActorForceArray windForces;
    Vec2d totalWindForce = Vec2d::Zero;

    PHYSWORLD->getWindForces(m_actor->get2DPos(), m_speed, m_actor->getDepth(), windForces);

    for ( u32 i = 0; i < windForces.size(); i++ )
    {
        totalWindForce += windForces[i].m_force;
    }

    Vec2d wind = totalWindForce * m_windMultiplier;
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth());

    addForce(wind);
    addForce(gravity * m_gravityMultiplier);

    if ( m_bouncingSpeed != Vec2d::Zero )
    {
        setSpeed(Vec2d::Zero);
        addForce(m_bouncingSpeed / _deltaTime);
        m_bouncingSpeed = Vec2d::Zero;
    }

    if ( m_hitDirection != Vec2d::Zero )
    {
        addForce(m_hitDirection * (m_hitLevel + 1.0f) * getTemplate()->getHitMultiplier());
        m_hitDirection = Vec2d::Zero;
        m_hitLevel = 0;
    }
}

void BounceOnPolylinePhysComponent::applyForces( f32 _deltaTime )
{
    Vec2d speed = getForce() * _deltaTime;
    setSpeed(getSpeed() + speed);

    m_prevPos = m_actor->get2DPos();
    m_actor->setPos(m_actor->getPos() + getSpeed().to3d() * _deltaTime);

    updatePhantom();
}

void BounceOnPolylinePhysComponent::computeBouncingSpeed( const Vec2d& _initialSpeed, const Vec2d& _surfaceNormal )
{
    Vec2d normal = _surfaceNormal;

    f32 norm = _initialSpeed.norm() * getTemplate()->getBouncingFactor();

    Vec2d initialDirection = _initialSpeed;
    initialDirection.normalize();
    f32 dotResult = normal.dot( -initialDirection );
    Vec2d direction = initialDirection + normal * (2 * dotResult);

    if (direction.dot(_surfaceNormal) < 0)
    {
        m_bouncingSpeed = Vec2d::Zero;
        return;
    }

    m_bouncingSpeed = direction * norm;

    Vec2d boost = computeBoostVector(m_bouncingSpeed, _surfaceNormal);
    m_bouncingSpeed += boost;

#if defined(DEVELOPER_ALEXIS) && 0
    {
        Vec2d p = m_actor->get2DPos();
        Vec2d p1 = p + Vec2d(-0.10f, 0.0f);
        Vec2d p2 = p + Vec2d(0.10f, 0.0f);

        // display bouncing speed
        GFX_ADAPTER->drawDBGLine(p1, p1 + (m_bouncingSpeed / m_bouncingSpeed.norm()), 0.0f, 1.0f, 0.0f, 1.0f);
        GFX_ADAPTER->drawDBGCircle(p1.m_x, p1.m_y, 0.15f, 0.0f, 1.0f, 0.0f, 1.0f);

        // display normal
        GFX_ADAPTER->drawDBGLine(p2, p2 + _surfaceNormal, 0.0f, 0.0f, 1.0f, 1.0f);
        GFX_ADAPTER->drawDBGCircle(p2.m_x, p2.m_y, 0.10f, 0.0f, 0.0f, 1.0f, 1.0f);
    }
#endif
}

Vec2d BounceOnPolylinePhysComponent::computeSurfaceNormal( ObjectRef _polyline, u32 _edgeIndex ) const
{
    BaseObject* obj = GETOBJECT(_polyline);
    
    if ( !obj )
    {
        ITF_ASSERT_MSG(obj, "This polyline doesn't exist.");
        return Vec2d::Zero;
    }

    PolyLine* poly = static_cast<PolyLine*>(obj);
    Vec2d normal = poly->getEdgeAt(_edgeIndex).m_normalizedVector.getPerpendicular();

    return normal;
}

Vec2d BounceOnPolylinePhysComponent::computeBoostVector(const Vec2d& _speed, const Vec2d& _surfaceNormal) const
{
    ITF_ASSERT(_surfaceNormal.isNormalized());

    f32 weightFactor = powf(getTemplate()->getPassengerFactor(), m_totalWeight / getTemplate()->getDefaultWeight());

    if ( weightFactor < 0.17f)
    {
        return Vec2d::Zero;
    }

    f32 minYSpeed = getTemplate()->getBouncingMinYSpeed() * weightFactor;
    f32 YSpeed = _surfaceNormal.dot(_speed);

    if (YSpeed > minYSpeed)
    {
        return Vec2d::Zero;
    }

    f32 YBoost = minYSpeed - YSpeed;

    // don't boost on walls and ceilings
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth());
    gravity.normalize();
    f32 gravDotNormal = gravity.dot(_surfaceNormal);

    if (gravDotNormal > 0.0f)
    {
        return Vec2d::Zero;
    }
    
    YBoost *= -gravDotNormal;

    return _surfaceNormal * YBoost;
}

void BounceOnPolylinePhysComponent::checkContacts()
{
    PhysContactsContainer contacts;
    PHYSWORLD->checkEncroachment(m_prevPos, m_actor->get2DPos(), m_actor->getAngle(),
                                 m_phantom->getShape(), m_collisionFilter, m_actor->getDepth(), contacts);

    if (contacts.size() == 0)
    {
        return;
    }

    SafeArray<ValidContact> validContacts(contacts.size());

    // check all contacts and look for valid ones
    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& contact = contacts[i];
        ValidContact valid;

        if (!isAValidContact(contact, getSpeed(), valid.m_normalDotSpeed))
        {
            continue;
        }

        {
#if defined(DEVELOPER_ALEXIS) && 0 && ITF_DEBUG_LEVEL > 0
            // display input speed
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos().m_x, m_actor->get2DPos().m_y, getRadius(),
                0.0f, 1.0f, .0f, 1.0f);
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos().m_x, m_actor->get2DPos().m_y, 0.15f,
                0.0f, 1.0f, .0f, 1.0f);
            GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() + (getSpeed() / getSpeed().norm()),
                .0f, 1.0f, .0f, 1.0f);

            // display contact point and contact edge
            GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint.m_x, contact.m_contactPoint.m_y, 0.15f,
                1.0f, .0f, .0f, 1.0f);
            PolyLine* line = static_cast<PolyLine*>(GETOBJECT(contact.m_collidableUserData));
            PolyLineEdge edge = line->getEdgeAt(contact.m_edgeIndex);
            GFX_ADAPTER->drawDBGLine(edge.getPos(), edge.getPos() + edge.m_vector, 1.0f, .0f, .0f, 1.0f);
#endif
        }
        
        valid.m_contactPtr = &contact;
        validContacts.push_back(valid);
    }

    // look for the most logical contact
    if (validContacts.size() == 0)
    {
        return;
    }

    u32 bestIndex = 0;

    for (u32 i = 1; i < validContacts.size(); ++i)
    {
        if (validContacts[i].m_normalDotSpeed < validContacts[bestIndex].m_normalDotSpeed)
        {
            bestIndex = i;
        }
    }

    processContact(*validContacts[bestIndex].m_contactPtr);
}

void BounceOnPolylinePhysComponent::processContact(const SCollidableContact& _contact)
{

    // Check whether we bounce on the extremity of the edge or not
    PolyLine* polyline = static_cast<PolyLine*>(GETOBJECT(_contact.m_collidableUserData));
    ITF_ASSERT(polyline);
    PolyLineEdge edge = polyline->getEdgeAt(_contact.m_edgeIndex);
    Vec2d edgeToActor = m_actor->get2DPos() - edge.getPos();
    f32 dist = edge.m_normalizedVector.dot(edgeToActor);

    Vec2d normal;

    if ( dist >= 0.f && dist <= edge.m_length )
    {
        // if it's within margins it's ok
        normal = computeSurfaceNormal(_contact.m_collidableUserData, _contact.m_edgeIndex);
    }
    else
    {
        Vec2d contactToActor = m_actor->get2DPos() - _contact.m_contactPoint;
        normal = contactToActor.normalize();
    }

    { // repulse the actor from the wall
        Vec2d actorPos = m_actor->get2DPos();
        Vec2d actorToContact = _contact.m_contactPoint - actorPos;
        Vec2d i;
        intersectionLineLine(actorPos, actorPos + getSpeed(), edge.getPos(), edge.getPos() + edge.m_vector, i);
        Vec2d iToActor = m_actor->get2DPos() - i;
        Vec2d iToActorNormalized = iToActor;
        iToActorNormalized.normalize();
        f32 distiToRealReboundPos = iToActor.norm() * getRadius() / actorToContact.norm();
        Vec2d RealReboundPos = i + iToActorNormalized * distiToRealReboundPos;
        m_actor->set2DPos(RealReboundPos);
    }

    computeBouncingSpeed(getSpeed(), normal);

    {
#if defined(DEVELOPER_ALEXIS) && 0
        Vec2d p1 = m_actor->get2DPos();
        Vec2d p2 = _contact.m_contactPoint;

        // display body radius
        GFX_ADAPTER->drawDBGCircle(p1.m_x, p1.m_y, getRadius(), 1.0f, 1.0f, 1.0f, 1.0f);

        // display Contact point
        GFX_ADAPTER->drawDBGCircle(p2.m_x, p2.m_y, 0.15f, 1.0f, .0f, .0f, 1.0f);
#endif
    }
}

void BounceOnPolylinePhysComponent::processNewUser( EventStickOnPolyline* _stickEvent )
{
    ObjectRef actorRef = _stickEvent->getActor();
    Actor* actorPtr = static_cast<Actor*>(GETOBJECT(actorRef));
    ITF_ASSERT(actorPtr);
    StickToPolylinePhysComponent* physComponent = actorPtr->GetComponent<StickToPolylinePhysComponent>();
    f32 actorWeight = 0.0f;

    if (physComponent)
    {
        actorWeight = physComponent->getWeight();
    }
    else
    {
        actorWeight = getTemplate()->getDefaultWeight();
    }

    if (_stickEvent->isSticked())
    {
        m_passengers.push_back(actorRef);
        addForce(_stickEvent->getSpeed() / LOGICDT);
        m_totalWeight += actorWeight;
    }
    else
    {
        i32 actorIndex = m_passengers.find(actorRef);

        if (actorIndex != -1)
        {
            m_passengers.eraseNoOrder(actorIndex);
            m_totalWeight -= actorWeight;
        }
    }
}

void BounceOnPolylinePhysComponent::clear()
{
    if ( m_phantom )
    {
        if ( m_objectsRegistered )
        {
            PHYSWORLD->removePhantom(m_phantom);
        }

        PHYSWORLD->deallocPhantom(m_phantom);
    }

    m_objectsRegistered = bfalse;
}

void BounceOnPolylinePhysComponent::initPhantom()
{
    m_shape.setRadius(getRadius());

    if (!m_phantom)
    {
        PhysPhantomInitCommonInfo info;

        SET_OWNER(info, m_actor->getRef());
        info.m_angle = m_actor->getAngle();
        info.m_pos   = m_actor->get2DPos();
        info.m_shape = &m_shape;
        info.m_userData = m_actor->getRef();
        info.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;

        m_collisionFilter = ECOLLISIONFILTER_ENVIRONMENT;

        m_phantom = PHYSWORLD->allocPhantom(info);
        m_phantom->updateAABB();
    }
}

void BounceOnPolylinePhysComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( !m_objectsRegistered &&
          m_phantom )
    {
        PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
        m_objectsRegistered = btrue;
    }
}

void BounceOnPolylinePhysComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_objectsRegistered )
    {
        PHYSWORLD->removePhantom(m_phantom);
        m_objectsRegistered = bfalse;
    }
}

bbool BounceOnPolylinePhysComponent::isAValidContact(const SCollidableContact& _contact, const Vec2d& _speed,
                                                     f32& _normalDotSpeed) const
{
    if ( _contact.m_edgeIndex == U32_INVALID )
    {
        return bfalse;
    }

    // skip if we collide with ourselves
    PolylineComponent* polyComponent = m_actor->GetComponent<PolylineComponent>();

    if ( polyComponent && polyComponent->containsPolyline(_contact.m_collidableUserData) )
    {
        return bfalse;
    }

    Vec2d normal = computeSurfaceNormal(_contact.m_collidableUserData, _contact.m_edgeIndex);

    _normalDotSpeed = normal.dot(_speed);
    if (_normalDotSpeed >= 0.0f)
    {
        return bfalse;
    }

    return btrue;
}

void BounceOnPolylinePhysComponent::alignWithGravity( f32 _deltaTime )
{
    f32 zeroAngle = getVec2Angle(PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth())) + MTH_PIBY2;
    f32 curAngle = m_actor->getAngle();
    f32 deltaAngle = getShortestAngleDelta(curAngle, zeroAngle);
    f32 newAngle = curAngle + (deltaAngle * getTemplate()->getAngularSmoothFactor());

    m_actor->setAngle(newAngle);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(BounceOnPolylinePhysComponent_Template)
BEGIN_SERIALIZATION_CHILD(BounceOnPolylinePhysComponent_Template)

    SERIALIZE_MEMBER("bouncingFactor", m_bouncingFactor);
    SERIALIZE_MEMBER("bouncingMinYSpeed", m_bouncingMinYSpeed);
    SERIALIZE_MEMBER("bouncingPassengerFactor", m_passengerFactor);
    SERIALIZE_MEMBER("bouncingDefaultActorWeight", m_defaultWeight);
    SERIALIZE_MEMBER("bouncingPunchMultiplier", m_hitMultiplier);
    SERIALIZE_MEMBER("physAngularRealignSmoothFactor", m_angularSmoothFactor);

END_SERIALIZATION()

BounceOnPolylinePhysComponent_Template::BounceOnPolylinePhysComponent_Template()
: m_bouncingFactor(0.7f)
, m_bouncingMinYSpeed(15.0f)
, m_passengerFactor(0.82f)
, m_defaultWeight(1.0f)
, m_hitMultiplier(160.0f)
, m_angularSmoothFactor(0.2f)
{
}

}
