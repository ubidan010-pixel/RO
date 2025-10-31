#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SOFTCOLLISION_H_
#include "rayman/gameplay/AI/Utils/Ray_SoftCollision.h"
#endif //_ITF_RAY_SOFTCOLLISION_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

namespace ITF
{
    BEGIN_SERIALIZATION(Ray_SoftCollision_Template)
        SERIALIZE_MEMBER("radius", m_radius);
        SERIALIZE_MEMBER("exitSpeed", m_exitSpeed);
        SERIALIZE_MEMBER("exitForce", m_exitForce);
        SERIALIZE_MEMBER("offset", m_offset);
    END_SERIALIZATION()

    Ray_SoftCollision_Template::Ray_SoftCollision_Template()
        : m_radius(1.f)
        , m_exitSpeed(5.f)
        , m_exitForce(100.f)
        , m_offset(Vec2d::Zero)
    {
    }

    Ray_SoftCollision_Template::~Ray_SoftCollision_Template()
    {
    }



    Ray_SoftCollision::Ray_SoftCollision()
        : m_template(NULL)
        , m_actor(NULL)
        , m_animComponent(NULL)
        , m_disabled(bfalse)
        , m_radius(0.f)
    {
    }

    Ray_SoftCollision::~Ray_SoftCollision()
    {
    }

    void Ray_SoftCollision::init( const Ray_SoftCollision_Template* _template, ActorComponent* _owner )
    {
        m_template = _template;
        m_actor = _owner->GetActor();
        m_radius = m_template->getRadius();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000), _owner);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    }

    void Ray_SoftCollision::onEvent( Event* _event ) const
    {
        if ( Ray_EventQueryAIData* query = _event->DynamicCast<Ray_EventQueryAIData>(ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000)) )
        {
            query->setSoftCollisionRadius(getScaledRadius());
        }
    }

    f32 Ray_SoftCollision::getScaledRadius() const
    {
        ITF_ASSERT_CRASH(m_template && m_actor, "please call init first");

        if (m_disabled)
        {
            return 0.f;
        }
        else
        {
            return m_actor->getScale().m_y * m_radius;
        }
    }

    Vec2d Ray_SoftCollision::getPos() const
    {
        ITF_ASSERT_CRASH(m_template && m_actor, "please call init first");

        Vec2d offset = m_template->getOffset() * m_actor->getScale();

        f32 angle = m_actor->getAngle();
        if (m_animComponent && m_actor->isFlipped())
        {
            angle += MTH_PI;
        }

        return m_actor->get2DPos() + offset.Rotate(angle);
    }

    Ray_SoftCollision::Orientation Ray_SoftCollision::getOrientation() const
    {
        ITF_ASSERT_CRASH(m_template && m_actor, "please call init first");

        static const f32 s_WallAngle = 65.f * MTH_DEGTORAD;
        static const f32 s_WallCos = f32_Cos(s_WallAngle);

        const f32 angle = m_actor->getAngle();
        const f32 cos = f32_Cos(angle);

        if (cos < -s_WallCos)
        {
            return Orientation_Ceiling;
        }
        else if (cos < s_WallCos)
        {
            return (canonizeAngle(angle) < 0) ? Orientation_WallRight : Orientation_WallLeft;
        }

        return Orientation_Ground;
    }

    const Color& Ray_SoftCollision::getColor( const Orientation _orientation ) const
    {
        static const Color s_ColorGround = Color::green();
        static const Color s_ColorCeiling = Color::pink();
        static const Color s_ColorWallLeft = Color::blue();
        static const Color s_ColorWallRight = Color::black();

        switch ( _orientation )
        {
        case Orientation_Ground:
            return s_ColorGround;

        case Orientation_Ceiling:
            return s_ColorCeiling;

        case Orientation_WallLeft:
            return s_ColorWallLeft;

        case Orientation_WallRight:
        default:
            return s_ColorWallRight;
        }
    }

    void Ray_SoftCollision::update() const
    {
        if (m_disabled)
        {
            return;
        }

        const f32 radius = getScaledRadius();
        if (radius == 0.f)
        {
            return;
        }

        const Orientation orientation = getOrientation();
        if (orientation == Orientation_Ceiling)
        {
            return;
        }

        const Vec2d pos = getPos();


#ifndef ITF_SUPPORT_MULTITHREADED
        static ActorList allActors;
        allActors.clear();
#else // ITF_SUPPORT_MULTITHREADED
        ActorList allActors;
#endif // ITF_SUPPORT_MULTITHREADED

        AI_MANAGER->getActorsFromLayer(m_actor->getDepth(), allActors);

        const f32 detectionRadius = 2 * radius;

        for (u32 i = 0; i < allActors.size(); ++i)
        {
            Actor* otherActor = allActors[i];

            if (!otherActor)
            {
                continue;
            }

            if (otherActor == m_actor)
            {
                continue;
            }

            if ((otherActor->get2DPos() - m_actor->get2DPos()).sqrnorm() > detectionRadius * detectionRadius)
            {
                continue;
            }

            Vec2d force;
            if ( computeForce(pos, radius, orientation, otherActor, force) )
            {
                Ray_EventAddSoftCollForce addForce;
                addForce.setSender(m_actor->getRef());
                addForce.setForce(force);
                otherActor->onEvent(&addForce);
            }
        }
    }

/*
    bbool Ray_SoftCollision::computeForce( const Vec2d& myPos, const f32 myRadius, Actor* otherActor, Vec2d& _force ) const
    {
        Ray_EventQueryAIData otherData;
        otherActor->onEvent(&otherData);

        if (otherData.getSoftCollisionRadius() == 0.f)
        {
            return bfalse;
        }


        const f32 totalRadius = otherData.getSoftCollisionRadius() + myRadius;
        const Vec2d toPlayer = otherActor->get2DPos() - m_actor->get2DPos();
        const f32 distToPlayer = toPlayer.norm();

        if (distToPlayer > totalRadius)
        {
            return bfalse;
        }


        EventQueryPhysicsData otherPhysics;
        otherActor->onEvent(&otherPhysics);

        if (otherPhysics.getRadius() == 0.f)
        {
            return bfalse;
        }


        Vec2d dirToPlayer = toPlayer;
        dirToPlayer.normalize();

        Vec2d dir = dirToPlayer;

        //if (!ignoreStuckOnPolyline)
        {
            PolyLine * polyline = NULL;
            const PolyLineEdge * edge = NULL;
            AIUtils::getPolyLine(otherPhysics.getStickyPolyline(), otherPhysics.getStickyEdgePolyline(), polyline, edge);

            if (edge)
            {
                dir = otherPhysics.getCurrentGroundDir();

                if (dirToPlayer.dot(dir) < 0.f)
                {
                    dir = -dir;
                }
            }
        }


        const f32 distRatio = 1.f - sqr(distToPlayer / totalRadius);
        const f32 speedDotP = otherPhysics.getSpeed().dot(dir);
        //const f32 speedRatio = speedDotP < 0.f ? 1.f : 1.f - Clamp(speedDotP / m_template->getExitSpeed(), 0.f, 1.f);

        static f32 minClamp = 0.f;
        static f32 maxClamp = 0.1f;
        const f32 speedRatio = speedDotP < 0.f ? 1.f : 1.f - Clamp(speedDotP / m_template->getExitSpeed(), minClamp, maxClamp);

        _force = dir * distRatio * speedRatio * m_template->getExitForce();
        return btrue;
    }
*/

    bbool Ray_SoftCollision::computeForce( const Vec2d& myPos, const f32 myRadius, const Orientation myOrientation, Actor* otherActor, Vec2d& _force ) const
    {
        Ray_EventQueryAIData otherData;
        otherActor->onEvent(&otherData);

        if (otherData.getSoftCollisionRadius() == 0.f)
        {
            return bfalse;
        }


        const f32 totalRadius = otherData.getSoftCollisionRadius() + myRadius;
        const Vec2d toPlayer = otherActor->get2DPos() - m_actor->get2DPos();
        const f32 distToPlayer = toPlayer.norm();

        if (distToPlayer > totalRadius)
        {
            return bfalse;
        }


        EventQueryPhysicsData otherPhysics;
        otherActor->onEvent(&otherPhysics);

        if (otherPhysics.getRadius() == 0.f)
        {
            return bfalse;
        }


        Vec2d dir;

        if (myOrientation == Orientation_WallLeft)
        {
            dir = -Vec2d::Right;
        }
        else if (myOrientation == Orientation_WallRight)
        {
            dir = Vec2d::Right;
        }
        else
        {
            Vec2d dirToPlayer = toPlayer;
            dirToPlayer.normalize();

            dir = dirToPlayer;

            //if (!ignoreStuckOnPolyline)
            {
                PolyLine * polyline = NULL;
                const PolyLineEdge * edge = NULL;
                AIUtils::getPolyLine(otherPhysics.getStickyPolyline(), otherPhysics.getStickyEdgePolyline(), polyline, edge);

                if (edge)
                {
                    dir = otherPhysics.getCurrentGroundDir();

                    if (dirToPlayer.dot(dir) < 0.f)
                    {
                        dir = -dir;
                    }
                }
            }
        }


        const f32 distRatio = 1.f - sqr(distToPlayer / totalRadius);
        const f32 speedDotP = otherPhysics.getSpeed().dot(dir);
        //const f32 speedRatio = speedDotP < 0.f ? 1.f : 1.f - Clamp(speedDotP / m_template->getExitSpeed(), 0.f, 1.f);

        static f32 minClamp = 0.f;
        static f32 maxClamp = 0.1f;
        const f32 speedRatio = speedDotP < 0.f ? 1.f : 1.f - Clamp(speedDotP / m_template->getExitSpeed(), minClamp, maxClamp);

        _force = dir * distRatio * speedRatio * m_template->getExitForce();
        return btrue;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_SoftCollision::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
        {
            return;
        }

        if (m_disabled)
        {
            return;
        }

        const f32 radius = getScaledRadius();
        if (radius == 0.f)
        {
            return;
        }

        DebugDraw::circle(getPos(), m_actor->getDepth(), radius,
            getColor(getOrientation()), 0.f, "softCol");
    }
#endif // ITF_SUPPORT_EDITOR
};
