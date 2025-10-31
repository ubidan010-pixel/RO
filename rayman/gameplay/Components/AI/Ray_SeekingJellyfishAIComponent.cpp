#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SeekingJellyfishAIComponent.h"
#endif //_ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

namespace ITF
{
    // TODO: move to PhysCollisionSolver or AIUtils
    bbool rayCast( const Vec2d& _p0, const Vec2d& _p1,
                   const PolyLine* _poly,
                   PhysRayCastContactsContainer& _contacts )
    {
        u32 numEdges = _poly->getVectorsCount();

        if ( numEdges == 0 )
        {
            return bfalse;
        }

        bbool intersect = bfalse;
        Vec2d intersectP;
        Vec2d pA, pB;
        f32 t;
        f32 t0 = 0.f;
        f32 t1 = 0.f;
        u32 edge0 = U32_INVALID;
        u32 edge1 = U32_INVALID;

        for ( u32 i = 0; i < numEdges; i++ )
        {
            const PolyLineEdge& edge = _poly->getEdgeAt(i);
            pA = edge.getPos();
            pB = pA + edge.m_vector;

            if ( Segment_Segment(_p0,_p1,pA,pB,intersectP,&t) )
            {
                if ( !intersect )
                {
                    t0 = t1 = t;
                    edge0 = i;
                    edge1 = i;
                    intersect = btrue;
                }
                else if ( t < t0 )
                {
                    t0 = t;
                    edge0 = i;
                }
                else if ( t > t1 )
                {
                    t1 = t;
                    edge1 = i;
                }
            }
        }

        if ( intersect )
        {
            SRayCastContact newContact;

            newContact.m_collidableUserData = ITF_INVALID_OBJREF;
            newContact.m_collidable = ITF_INVALID_OBJREF;
            newContact.m_edgeIndex0 = edge0;
            newContact.m_edgeIndex1 = edge1;
            newContact.m_t0 = t0;
            newContact.m_t1 = t1;

            _contacts.push_back(newContact);

            return btrue;
        }
        else
        {
            return bfalse;
        }
    }


    IMPLEMENT_OBJECT_RTTI(Ray_SeekingJellyfishAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_SeekingJellyfishAIComponent_Template)
        SERIALIZE_MEMBER("minSpeed", m_minSpeed);
        SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
        SERIALIZE_MEMBER("acceleration", m_acceleration);
        SERIALIZE_MEMBER("minAngularSpeed", m_minAngularSpeed);
        SERIALIZE_MEMBER("maxAngularSpeed", m_maxAngularSpeed);
        SERIALIZE_MEMBER("angularAcceleration", m_angularAcceleration);
        SERIALIZE_MEMBER("initialSpeedBoostMultiplier", m_initialSpeedBoostMultiplier);
        SERIALIZE_MEMBER("initialSpeedBoostDuration", m_initialSpeedBoostDuration);
        SERIALIZE_MEMBER("fleeSpeedMultiplier", m_fleeSpeedMultiplier);
        SERIALIZE_MEMBER("fleeZSpeed", m_fleeZSpeed);

        SERIALIZE_MEMBER("targetChangeTimeMin", m_targetChangeTimeMin);
        SERIALIZE_MEMBER("targetChangeTimeMax", m_targetChangeTimeMax);
        SERIALIZE_MEMBER("targetAnticipationMultiplier", m_targetAnticipationMultiplier);

        SERIALIZE_MEMBER("regionId", m_regionId);
        SERIALIZE_MEMBER("avoidRegionId", m_avoidRegionId);
        SERIALIZE_MEMBER("raycastDepth", m_raycastDepth);
        SERIALIZE_MEMBER("goBackToRegionTimeLimit", m_goBackToRegionTimeLimit);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_SeekingJellyfishAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SeekingJellyfishAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SeekingJellyfishAIComponent)
        VALIDATE_COMPONENT_PARAM("maxSpeed", getTemplate()->getMaxSpeed() > 0.f, "must be positive!");
    END_VALIDATE_COMPONENT()

    Ray_SeekingJellyfishAIComponent::Ray_SeekingJellyfishAIComponent()
        : m_disabled(bfalse)
        , m_state(State_TargetPlayer)
        , m_targetPos(Vec2d::Zero)
        , m_targetChangeTimer(0.f)
        , m_targetDir(Vec2d::Right)
        , m_speed(0.f)
        , m_angularSpeed(0.f)
        , m_initialSpeedBoostTimer(0.f)

        , m_ignoreRegions(bfalse)
        , m_lastValidPos(Vec2d::Zero)
        , m_outOfRegionTimer(0.f)
    {
    }

    Ray_SeekingJellyfishAIComponent::~Ray_SeekingJellyfishAIComponent()
    {
    }


    void Ray_SeekingJellyfishAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172), this);

        m_speed = getTemplate()->getMinSpeed();
        m_initialSpeedBoostTimer = getTemplate()->getInitialSpeedBoostDuration();
    }

    void Ray_SeekingJellyfishAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            m_disabled = !eventActivate->getActivated();
        }
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172)) )
        {
            m_state = State_Flee;
            AIUtils::disableCollision(m_actor);

            Actor* sender = (Actor*)_event->getSender().getObject();
            ITF_ASSERT(sender);
            if (sender)
            {
                const Vec2d dispatchPoint = sender->get2DPos();
                m_targetDir = (m_actor->get2DPos() - dispatchPoint).normalize();
                //DebugDraw::line3D(m_actor->getPos(), m_actor->getPos() + m_targetDir.to3d() * 5, Color::white(), 10, 3);
            }
        }
    }

    static const f32 s_angleOffset = -MTH_PIBY2;

    void Ray_SeekingJellyfishAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_disabled)
        {
            return;
        }


        const Vec2d myPos = m_actor->get2DPos();
        const f32 myAngle = m_actor->getAngle() - s_angleOffset;
        const Vec2d myDir = Vec2d::Right.Rotate(myAngle);


        Vec2d targetDir = m_targetDir;  // default: keep going

        if (m_state == State_TargetPlayer)
        {
            bbool targetPlayer = m_ignoreRegions;


            if (!m_ignoreRegions)
            {
                // if we already have a region, always stay in it; otherwise, find one
                PolyLine* region = (PolyLine*)m_region.getObject();
                bbool isInside = bfalse;

                if (region)
                {
                    isInside = region->isPointInside(myPos);
                }
                else
                {
                    region = AIUtils::getPolylineRegion(m_actor->getDepth(), getTemplate()->getRegionId(), myPos);
                    if (region)
                    {
                        m_region = region->getRef();
                        isInside = btrue;
                    }
                }

                if ( isInside )
                {
                    // we're in a region: avoid its borders
                    PhysRayCastContactsContainer contacts;
                    if ( rayCast( myPos, myPos + myDir * getTemplate()->getRaycastDepth(),
                                  region, contacts) )
                    {
                        const PolyLineEdge& edge = region->getEdgeAt(contacts[0].m_edgeIndex0);
                        m_targetDir = edge.m_normalizedVector.getPerpendicular();
                    }
                    else
                    {
                        // target the player, except if we need to avoid other regions
                        targetPlayer = !avoidRegions(myPos, myDir);
                    }

                    m_lastValidPos = m_actor->get2DPos();
                    m_outOfRegionTimer = 0.f;
                }
                else if ( region )
                {
                    // we just left a region

                    m_outOfRegionTimer += _dt;
                    if (m_outOfRegionTimer < getTemplate()->getGoBackToRegionTimeLimit())
                    {
                        m_targetDir = (m_lastValidPos - myPos).normalize();

                        //const Vec3d targetPos3d = m_lastValidPos.to3d(region->getDepth());
                        //DebugDraw::line3D(m_actor->getPos(), targetPos3d, Color::yellow());
                        //DebugDraw::circle(targetPos3d, 0.3f, Color::yellow());
                    }
                    else
                    {
                        // we've been out for too long: go back to the region barycenter
                        m_targetDir = (region->get2DPos() - myPos).normalize();

                        //DebugDraw::line3D(m_actor->getPos(), region->getPos(), Color::red());
                        //DebugDraw::circle(region->getPos(), 0.3f, Color::red());
                    }
                    // else: keep going away from the last border we crossed
                }
                else
                {
                    // target the player, except if we need to avoid other regions
                    targetPlayer = !avoidRegions(myPos, myDir);
                }
            }


            if (targetPlayer)
            {
                if (m_targetChangeTimer <= 0.f)
                {
                    if ( Actor* target = AIUtils::getClosestLivePlayer(m_actor) )
                    {
                        if ( const PhysComponent* targetPhys = target->GetComponent<PhysComponent>() )
                        {
                            m_targetChangeTimer = Seeder::getSharedSeeder().GetFloat(getTemplate()->getTargetChangeTimeMin(), getTemplate()->getTargetChangeTimeMax());
                            m_targetPos = target->get2DPos() + targetPhys->getSpeed() * m_targetChangeTimer * getTemplate()->getTargetAnticipationMultiplier();
                        }

                        //const f32 randomMax = getTemplate()->getTargetOffsetMax();
                        //m_targetOffset.m_x = Seeder::getSharedSeeder().GetFloat(-randomMax, randomMax);
                        //m_targetOffset.m_y = Seeder::getSharedSeeder().GetFloat(-randomMax, randomMax);
                    }
                }

                targetDir = (m_targetPos - myPos).normalize();
                m_targetDir = -targetDir;   // in case we loose the target, we'll go in the opposite direction

                //DebugDraw::line3D(m_actor->getPos(), m_targetPos.to3d(m_actor->getDepth()), Color::yellow());
                //DebugDraw::circle(m_targetPos.to3d(m_actor->getDepth()), 0.3f, Color::yellow());
            }
            else
            {
                targetDir = m_targetDir;
            }

            m_targetChangeTimer -= _dt;
        }


        f32 speedMultiplier = (m_initialSpeedBoostTimer > 0.f) ? getTemplate()->getInitialSpeedBoostMultiplier() : 1.f;
        m_initialSpeedBoostTimer -= _dt;
        if (m_state == State_Flee)
        {
            speedMultiplier = getTemplate()->getFleeSpeedMultiplier();
        }

        const f32 angleDelta = getShortestAngleDelta(myAngle, targetDir.getAngle());

        const f32 speedRatio = Clamp(InverseInterpolate(getTemplate()->getMinSpeed() * speedMultiplier, getTemplate()->getMaxSpeed() * speedMultiplier, m_speed), 0.f, 1.f);



        // rotate

        f32 targetAngularSpeed = 0.f;

        // the slower we go, the faster we turn
        const f32 angularSpeedRatioed = Interpolate(getTemplate()->getMinAngularSpeed(), getTemplate()->getMaxAngularSpeed(), 1 - speedRatio);
        const f32 angularAccRatioed = Interpolate(getTemplate()->getAngularAcceleration(), getTemplate()->getAngularAcceleration() * 3, 1 - speedRatio);
        //const f32 angularAccRatioed = Interpolate(getTemplate()->getAcceleration(), getTemplate()->getAcceleration() * 3, 1 - speedRatio);

        // the closer we are to our target angle, the slower we turn
        const f32 angleRatio = Min(fabsf(angleDelta) / MTH_PIBY2, 1.f);

        if (angleDelta > 0)
        {
            targetAngularSpeed = angularSpeedRatioed * angleRatio;
        }
        else if (angleDelta < 0)
        {
            targetAngularSpeed = -angularSpeedRatioed * angleRatio;
        }

        if (m_angularSpeed < targetAngularSpeed)
        {
            m_angularSpeed += angularAccRatioed * _dt;
        }
        else if (m_angularSpeed > targetAngularSpeed)
        {
            m_angularSpeed -= angularAccRatioed * _dt;
        }



        const f32 targetSpeed = (myDir.dot(targetDir) > 0) ? getTemplate()->getMaxSpeed() : getTemplate()->getMinSpeed();

        if (m_speed < targetSpeed * speedMultiplier)
        {
            // accelerate
            m_speed += getTemplate()->getAcceleration() * speedMultiplier * _dt;
        }
        else
        {
            // decelerate
            m_speed -= getTemplate()->getAcceleration() * speedMultiplier * _dt;
        }



        // apply rotation
        const f32 newAngle = myAngle + m_angularSpeed * _dt;
        m_actor->setAngle(newAngle + s_angleOffset);

        // move forward
        m_actor->set2DPos(myPos + Vec2d(m_speed * _dt, 0).Rotate(newAngle));

        if (m_state == State_Flee)
        {
            m_actor->setDepth(m_actor->getDepth() + getTemplate()->getFleeZSpeed() * _dt);
        }




/*
        {
            const Color& color = (myDir.dot(targetDir) > 0) ? Color::green() : Color::red();
            const Vec3d dir = Vec2d::Right.Rotate(m_actor->getAngle()).to3d();
            DebugDraw::arrow3D(m_actor->getPos(), m_actor->getPos() + dir * 3, color);

            DebugDraw::arrow3D(m_actor->getPos(), m_actor->getPos() + dir.getPerpendicular() * getSign(m_angularSpeed));
        }
*/
    }

    bbool Ray_SeekingJellyfishAIComponent::avoidRegions( const Vec2d& _myPos, const Vec2d& _myDir )
    {
        if ( const RegionsManager::RegionDataList* allRegions = REGIONS_MANAGER->getRegions(DepthRange(m_actor->getDepth())) )
        {
            for ( u32 i = 0; i < allRegions->size(); i++ )
            {
                const RegionsManager::RegionData& regionData = (*allRegions)[i];

                if ( regionData.m_id != getTemplate()->getAvoidRegionId() )
                {
                    continue;
                }

                PolyLine* region = (PolyLine*)regionData.m_ref.getObject();
                if ( !region )
                {
                    continue;
                }

                if (region->isPointInside(_myPos))
                {
                    return btrue;
                }

                PhysRayCastContactsContainer contacts;
                if ( rayCast( _myPos, _myPos + _myDir * getTemplate()->getRaycastDepth(),
                              region, contacts) )
                {
                    const PolyLineEdge& edge = region->getEdgeAt(contacts[0].m_edgeIndex0);
                    m_targetDir = edge.m_normalizedVector.getPerpendicular();
                    return btrue;
                }
            }
        }

        return bfalse;
    }



#ifdef ITF_SUPPORT_EDITOR
    void Ray_SeekingJellyfishAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        drawDebug();
    }

    void Ray_SeekingJellyfishAIComponent::drawDebug() const
    {
        DebugDraw::arrow3D(m_actor->getPos(), m_actor->getPos() + Vec2d(5, 0).Rotate(m_actor->getAngle() + s_angleOffset).to3d());
    }

#endif // ITF_SUPPORT_EDITOR

};