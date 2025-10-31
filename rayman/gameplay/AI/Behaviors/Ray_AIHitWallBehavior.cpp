#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIHITWALLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIHitWallBehavior.h"
#endif //_ITF_RAY_AIHITWALLBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIRECEIVEHITACTION_H_
#include "gameplay/AI/Actions/AIReceiveHitAction.h"
#endif //_ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIHitWallBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIHitWallBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("hitWall", m_hitWall, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("halfWallHeight", m_halfWallHeight);
        SERIALIZE_MEMBER("minimumHeightForWallHit", m_minimumHeightForWallHit);
        SERIALIZE_MEMBER("flipOnHitWall", m_flipOnHitWall);
        SERIALIZE_MEMBER("minIncidenceAngleForWallHit", m_minIncidenceAngleForWallHit);
        SERIALIZE_MEMBER("minSpeedForWallHit", m_minSpeedForWallHit);
    END_SERIALIZATION()

    Ray_AIHitWallBehavior_Template::Ray_AIHitWallBehavior_Template()
    : m_hitWall(NULL)
    , m_halfWallHeight(1.0f)
    , m_minimumHeightForWallHit(0.5f)
    , m_flipOnHitWall(bfalse)
    , m_minIncidenceAngleForWallHit(btrue,5.0f)
    , m_minSpeedForWallHit(2.0f)
    {
    }

    Ray_AIHitWallBehavior_Template::~Ray_AIHitWallBehavior_Template()
    {
        SF_DEL(m_hitWall);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIHitWallBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIHitWallBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_phantomComponent, "Ray_AIHitWallBehavior requires a PhantomComponent");
    END_VALIDATE_BEHAVIOR()

    Ray_AIHitWallBehavior::Ray_AIHitWallBehavior()
        : Super()
        , m_phantomComponent(NULL)
        , m_physComponent(NULL)
        , m_hitWall(NULL)
        , m_isInAir(bfalse)
        , m_angleBeforeHitWall(0.0f)
        , m_wallOrientation(0)
        , m_justEntered(btrue)
        , m_flippedOnHitWall(bfalse)
        , m_edgeIndex(U32_INVALID)
    {
    }

    Ray_AIHitWallBehavior::~Ray_AIHitWallBehavior()
    {
    }

    void Ray_AIHitWallBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_hitWall = createAiAction(getTemplate()->getHitWall());

        Super::onActorLoaded(_hotReload);

        m_phantomComponent = m_actor->GetComponent<PhantomComponent>(); // mandatory, validated
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>(); // not mandatory
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768), m_aiComponent);
    }

    void Ray_AIHitWallBehavior::onActivate()
    {
        m_justEntered = btrue;
        m_flippedOnHitWall = bfalse;

        startHitWall();
    }

    void Ray_AIHitWallBehavior::onDeactivate()
    {
        Super::onDeactivate();


        restoreAfterHitWall();
    }



    void Ray_AIHitWallBehavior::setData( class EventBlockedByPolyline* _event )
    {
        m_polyline = _event->getPolyline();
        m_edgeIndex = _event->getEdge();
        m_pos = _event->getContact();
    }
    void Ray_AIHitWallBehavior::onActionFinished()
    {
        Super::onActionFinished();

        m_aiComponent->onBehaviorFinished();
    }

    void Ray_AIHitWallBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsInAir,1210226103), m_isInAir);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WallOrientation,4046989041), m_wallOrientation);
        }
    }

    void Ray_AIHitWallBehavior::startHitWall(   )
    {

        BaseObject * obj = m_polyline.getObject();
        ITF_ASSERT(obj);

        if (!obj)
            return;
 
        PolyLine * poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
        ITF_ASSERT(poly);

        const PolyLineEdge* _wallEdge = &poly->getEdgeAt(m_edgeIndex);

        //get distance 

        Vec2d projPos = m_pos;


        bbool left = _wallEdge->m_vector.getPerpendicular().dot(Vec2d::Right) > 0;  // if the edge normal goes right, that means we're hitting it from the left

        AIUtils::EdgeType edgeType = AIUtils::getEdgeType(*_wallEdge);

        if (edgeType != AIUtils::EdgeType_Ceiling)
        {
            f32 wallHeight = 0.0f;
            Vec2d wallTop = Vec2d::Zero;
            AIUtils::isWall(left,m_pos,poly,m_edgeIndex,3.0f,m_aiComponent->getCurrentGravityDir(),wallHeight,wallTop);

            if (wallHeight < getTemplate()->getMinimumHeightForWallHit())
                m_aiComponent->onBehaviorFinished();

            //half wall
            //if we are very close to the edge top use halfwall

            if (wallHeight < getTemplate()->getHalfWallHeight())
            {
                projPos = wallTop;
                m_wallOrientation = 2;
            }
            //wall
            else
            {
                m_wallOrientation = 0;
            }
        }
        //ceiling
        else
            m_wallOrientation = 1;

        m_physComponent->setDisabled(btrue, bfalse, btrue);    // disable physics so we can snap, but don't unstick
        m_posBeforeHitWall = m_actor->get2DPos();
        m_angleBeforeHitWall = m_actor->getAngle();

        Vec3d pos(projPos.m_x,projPos.m_y,m_actor->getDepth());
        m_animatedComponent->lockMagicBox(pos);
        // orient along the wall
        f32 wallAngle = _wallEdge->m_normalizedVector.getPerpendicular().getAngle()+ (!m_actor->isFlipped() ? 0 : MTH_PI);

        m_actor->setAngle(wallAngle);

        if (getTemplate()->getFlipOnHitWall() && edgeType != AIUtils::EdgeType_Ceiling)
        {
            m_flippedOnHitWall = btrue;
            m_actor->setFlipped(!m_actor->isFlipped());
        }
        setAction(m_hitWall);
    }

    void Ray_AIHitWallBehavior::restoreAfterHitWall()
    {
        if (m_flippedOnHitWall)
        {
            m_flippedOnHitWall = bfalse;
            m_actor->setFlipped(!m_actor->isFlipped());
        }

        if (m_physComponent->getDisabled())
        {
            //m_actor->set2DPos(m_posBeforeHitWall);
            m_actor->setAngle(m_angleBeforeHitWall);    // just to be safe, but the physics should set the angle anyway
            m_animatedComponent->resetLockMagicBox();
            m_physComponent->setDisabled(bfalse);
            m_physComponent->resetForces();
            m_physComponent->setSpeed(Vec2d::Zero);
        }
    }

    void Ray_AIHitWallBehavior::update( f32 _dt )
    {
        Super::update(_dt);
        if (m_justEntered)
        {
            m_justEntered = bfalse;
        }
        else
        {
            m_isInAir = m_physComponent ? (m_physComponent->getStickedPolyline() == NULL) : bfalse;
        }


    }

    ITF::bbool Ray_AIHitWallBehavior::canHitWall( class EventBlockedByPolyline* _event )
    {
        if (m_aiComponent->getLastHitLevel() > 0)
        {
            BaseObject* obj = _event->getPolyline().getObject();
            if ( obj )
            {
                PolyLine* myPoly = static_cast<PolyLine*>(obj);
                const PolyLineEdge* myEdge = &myPoly->getEdgeAt(_event->getEdge());


                const Vec2d edge = myEdge->m_normalizedVector;
                const Vec2d speed = m_physComponent->getSpeed();
                Vec2d speedN = speed;
                speedN.normalize();

                const f32 dotP = f32_Abs(speedN.dot(edge));
                const f32 angle = f32_ACos(dotP);
                if ( (angle < -getTemplate()->getMinIncidenceAngleForWallHit().Radians()|| angle > getTemplate()->getMinIncidenceAngleForWallHit().Radians()))
                {
                    AIUtils::EdgeType edgeType = AIUtils::getEdgeType(*myEdge);

                    if (edgeType == AIUtils::EdgeType_Wall || edgeType == AIUtils::EdgeType_Ceiling)
                    {

                        if (edgeType == AIUtils::EdgeType_Wall)
                        {
                            f32 wallHeight = 0.0f;
                            Vec2d wallTop = Vec2d::Zero;
                            bbool left = myEdge->m_vector.getPerpendicular().dot(Vec2d::Right) > 0;  // if the edge normal goes right, that means we're hitting it from the left
                            AIUtils::isWall(left,_event->getContact(),myPoly,_event->getEdge(),3.0f,m_aiComponent->getCurrentGravityDir(),wallHeight,wallTop);

                            if (wallHeight < getTemplate()->getMinimumHeightForWallHit())
                            {
                                return bfalse;
                            }
                        }
                        return btrue;
                    }
                }
            }
        }
        return bfalse;
    }
}