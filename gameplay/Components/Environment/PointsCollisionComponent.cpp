#include "precompiled_gameplay.h"

#ifndef _ITF_POINTSCOLLISIONCOMPONENT_H_
#include "gameplay/Components/Environment/PointsCollisionComponent.h"
#endif //_ITF_POINTSCOLLISIONCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PointsCollisionComponent)

BEGIN_SERIALIZATION_CHILD(PointsCollisionComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(PointsCollisionComponent)

    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        VALIDATE_COMPONENT_PARAM("polylines", m_currentPolylines[i] != NULL, "Polyline %d doesn't have points",i);
    }

END_VALIDATE_COMPONENT()

PointsCollisionComponent::PointsCollisionComponent()
: m_disabled(bfalse)
, m_objectsRegistered(bfalse)
{
}

PointsCollisionComponent::~PointsCollisionComponent()
{
    detachAndClearUsers();

    u32 numPolylines = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolylines; i++ )
    {
        PolyLine* polyLine = m_currentPolylines[i];

        if ( !polyLine )
        {
            continue;
        }

        const PointsCollisionComponent_Template::PolylineData& templateData = getTemplate()->getPolylineData(i);

        if ( m_objectsRegistered )
        {
            if ( templateData.m_regionType.isValid() )
            {
                REGIONS_MANAGER->removeRegion(polyLine->getRef());
            }

            PHYSWORLD->removeBody(polyLine->m_physBody);
        }

        SF_DEL(polyLine->m_physShape);
        SF_DEL(polyLine);
    }
}

void PointsCollisionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    u32 numPolyData = getTemplate()->getNumPolylines();
    transform2d xf;
    bbool myFlip = m_actor->isFlipped();
    const Vec2d& myPos = m_actor->get2DPos();
    const Vec2d& myScale = m_actor->getScale();
    f32 myRot = m_actor->getAngle();

    xf.m_pos = myPos;
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(myRot);
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();
    
    m_currentPolylines.resize(numPolyData);

    for ( u32 i = 0; i < numPolyData; i++ )
    {
        const PointsCollisionComponent_Template::PolylineData& polyData = getTemplate()->getPolylineData(i);

        if ( polyData.m_gameMaterial.getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(polyData.m_gameMaterial, m_actor->getResourceGroup());
        }

        u32 numPoints = polyData.m_pointList.size();

        if ( numPoints < 2 )
        {
            m_currentPolylines[i] = NULL;
            continue;
        }

        PolyLine* pol = newAlloc(mId_Gameplay, PolyLine);
        pol->setOwnerActor(m_actor);
        m_currentPolylines[i] = pol;

        if ( myFlip )
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                u32 arrayIndex = numPoints - pointIndex - 1;
                Vec2d p = polyData.m_pointList[arrayIndex];

                p.m_x *= -1.f;

                p = mulTransform2d(xf,( p * myScale ));

                pol->addPoint(p);
            }
        }
        else
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                Vec2d p = mulTransform2d(xf,( polyData.m_pointList[pointIndex] * myScale ));

                pol->addPoint(p);
            }
        }

        pol->setLoop(polyData.m_loop);
        pol->forceRecomputeData();

        if ( polyData.m_movingPolyline )
        {
            pol->m_physShape = newAlloc(mId_Phys,PhysShapeMovingPolyline(pol));
        }
        else
        {
            pol->m_physShape = newAlloc(mId_Phys,PhysShapePolyline(pol));
        }

        PhysBodyInitCommonInfo bodyInit;

        SET_OWNER(bodyInit, m_actor->getRef());
        bodyInit.m_static = btrue;
        bodyInit.m_pos = pol->get2DPos();
        bodyInit.m_angle = pol->getAngle();
        bodyInit.m_userData = pol->getRef();
        bodyInit.m_weight = 1.f;
        bodyInit.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;
        bodyInit.m_collisionFilter = ECOLLISIONFILTER_CHARACTERS;

        pol->m_physBody = PHYSWORLD->allocBody(pol->m_physShape,bodyInit);
        pol->m_isCollisionActive = btrue;

        if ( m_objectsRegistered )
        {
            PHYSWORLD->insertBody(pol->m_physBody,m_actor->getDepth());
        }

        for ( u32 edgeIndex = 0; edgeIndex < numPoints; edgeIndex++ )
        {
            pol->setGameMaterialAt(polyData.m_gameMaterial.getStringID(),edgeIndex);
        }

        if ( m_objectsRegistered && polyData.m_regionType.isValid() )
        {
            REGIONS_MANAGER->addRegion(pol->getRef(),polyData.m_regionType,m_actor->getDepth());
        }
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);

    m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);
}

void PointsCollisionComponent::onBecomeActive()
{
    Super::onBecomeActive();

    registerPolylineDatas();
}

void PointsCollisionComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    unregisterPolylineDatas();
}

void PointsCollisionComponent::onEvent( Event* _event)
{
    Super::onEvent(_event);

    if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processNewUser(stickEvent);
    }
    else if ( EventStickOnPolylineChangeEdge* edgeChange = _event->DynamicCast<EventStickOnPolylineChangeEdge>(ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239)) )
    {
        processUserEdgeChange(edgeChange);
    }
    else if ( EventHanging* hanging = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
    {
        processNewHanging(hanging);
    }
    else if ( EventDisableCollision* disable = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
    {
        setDisabled(disable->getDisable());
    }
}

void PointsCollisionComponent::processNewUser( class EventStickOnPolyline* _event )
{
    if ( _event->isSticked() )
    {
        u32 numPolylines = m_currentPolylines.size();
        PolyLine* foundPoly = NULL;

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            foundPoly = m_currentPolylines[i];

            if ( foundPoly->getRef() == _event->getPolyline() )
            {
                break;
            }
            else
            {
                foundPoly = NULL;
            }
        }

        if ( foundPoly )
        {
            onNewUser(_event->getActor(),foundPoly,_event->getEdge());
        }
    }
    else
    {
        onRemoveUser(_event->getActor());
    }
}

void PointsCollisionComponent::processUserEdgeChange( EventStickOnPolylineChangeEdge* _event )
{
    for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        PolyUsers& user = *it;

        if ( user.m_actor == _event->getActor() )
        {
            PolyLine* foundPoly = NULL;
            u32 numPolylines = m_currentPolylines.size();

            for ( u32 i = 0; i < numPolylines; i++ )
            {
                foundPoly = m_currentPolylines[i];

                if ( foundPoly->getRef() == _event->getPolyline() )
                {
                    break;
                }
                else
                {
                    foundPoly = NULL;
                }
            }

            if ( foundPoly )
            {
                user.m_polylineRef = foundPoly->getRef();
                user.m_edgeIndex = _event->getEdge();
            }
            else
            {
                m_users.erase(it);
            }
            
            break;
        }
    }
}

void PointsCollisionComponent::processNewHanging( EventHanging* _event )
{
    if ( _event->isHanging() )
    {
        u32 numPolylines = m_currentPolylines.size();

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            PolyLine* foundPoly = m_currentPolylines[i];

            if ( foundPoly->getRef() == _event->getPolyline() )
            {
                onNewUser(_event->getActor(),foundPoly,_event->getEdge());
                break;
            }
        }
    }
    else
    {
        onRemoveUser(_event->getActor());
    }
}

void PointsCollisionComponent::onNewUser( ActorRef _actor, class PolyLine* _poly, u32 _edgeIndex )
{
    PolyUsers newUser;

    newUser.m_actor = _actor;
    newUser.m_polylineRef = _poly->getRef();
    newUser.m_edgeIndex = _edgeIndex;

    m_users.push_back(newUser);
}

void PointsCollisionComponent::onRemoveUser( ActorRef _actor )
{
    for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        PolyUsers& user = *it;

        if ( user.m_actor == _actor )
        {
            m_users.erase(it);
            break;
        }
    }
}

void PointsCollisionComponent::Update(f32 _dt )
{
    Super::Update(_dt);

    if (m_disabled)
    {
        return;
    }

    u32 numPolylines = m_currentPolylines.size();
    const Vec2d& myPos = m_actor->get2DPos();
    const Vec2d& myScale = m_actor->getScale();
    f32 myRot = m_actor->getAngle();
    bbool myFlip = m_actor->isFlipped();

    transform2d xf;

    xf.m_pos = myPos;
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(myRot);
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();
    
    for ( u32 i = 0; i < numPolylines; i++ )
    {
        PolyLine* pol = m_currentPolylines[i];
        const PointsCollisionComponent_Template::PolylineData& templateData = getTemplate()->getPolylineData(i);
        PhysShapeMovingPolyline* movingPoly = NULL;

        if ( templateData.m_movingPolyline )
        {
            movingPoly = static_cast<PhysShapeMovingPolyline*>(pol->m_physShape);
            movingPoly->sync();
        }
        else
        {
            movingPoly = NULL;
        }

        u32 numPoints = templateData.m_pointList.size();

        if ( myFlip )
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                u32 arrayIndex = numPoints - pointIndex - 1;
                Vec2d p = templateData.m_pointList[arrayIndex];

                p.m_x *= -1.f;
                
                p = mulTransform2d(xf,( p * myScale ));

                pol->setPosAt(p,pointIndex);
            }
        }
        else
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                Vec2d p = mulTransform2d(xf,( templateData.m_pointList[pointIndex] * myScale ));

                pol->setPosAt(p,pointIndex);
            }
        }

        pol->update(_dt);
        pol->m_physBody->updateAABB();
        pol->m_physBody->setPos3D(m_actor->getPos());

        if ( movingPoly )
        {
            movingPoly->updateMoved();
        }
    }
}

void PointsCollisionComponent::registerPolylineDatas()
{
    if ( !m_objectsRegistered )
    {
        u32 numPolylines = m_currentPolylines.size();

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            PolyLine* polyLine = m_currentPolylines[i];
            const PointsCollisionComponent_Template::PolylineData& templateData = getTemplate()->getPolylineData(i);

            PHYSWORLD->insertBody(polyLine->m_physBody,m_actor->getDepth());

            if ( templateData.m_regionType.isValid() )
            {
                REGIONS_MANAGER->addRegion(polyLine->getRef(),templateData.m_regionType,m_actor->getDepth());
            }
        }

        m_objectsRegistered = btrue;
    }
}

void PointsCollisionComponent::unregisterPolylineDatas()
{
    if ( m_objectsRegistered )
    {
        u32 numPolylines = m_currentPolylines.size();

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            PolyLine* polyLine = m_currentPolylines[i];
            const PointsCollisionComponent_Template::PolylineData& templateData = getTemplate()->getPolylineData(i);

            PHYSWORLD->removeBody(polyLine->m_physBody);

            if ( templateData.m_regionType.isValid() )
            {
                REGIONS_MANAGER->removeRegion(polyLine->getRef());
            }
        }

        m_objectsRegistered = bfalse;
    }
}

void PointsCollisionComponent::setDisabled( const bbool _disabled )
{
    if (_disabled == m_disabled)
    {
        return;
    }

    m_disabled = _disabled;

    if (_disabled)
    {
        unregisterPolylineDatas();
        detachAndClearUsers();
    }
    else
    {
        registerPolylineDatas();
    }
}

void PointsCollisionComponent::detachAndClearUsers()
{
    FixedArray <ObjectRef,32> detachActors;
    UsersList::iterator userIt = m_users.begin();

    while ( userIt != m_users.end() )
    {
        PolyUsers& user = *userIt;

        detachActors.push_back(user.m_actor);

        if ( detachActors.full() )
        {
            break;
        }

        ++userIt;
    }

    EventDetach detachEvent;

    for ( u32 i = 0; i < detachActors.size(); i++ )
    {
        Actor* actor = AIUtils::getActor(detachActors[i]);

        if ( actor )
        {
            actor->onEvent(&detachEvent);
        }
    }

    m_users.clear();
}



#ifdef ITF_SUPPORT_EDITOR
void PointsCollisionComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
        return;

    if (m_disabled)
    {
        return;
    }

    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        PolyLine* pol = m_currentPolylines[i];

        _drawInterface->drawPolyLine(pol,btrue);
    }
}

void PointsCollisionComponent::onEditorMove(bbool _modifyInitialPos)
{
    u32 numPolylines = m_currentPolylines.size();
    const Vec2d& myPos = m_actor->get2DPos();
    const Vec2d& myScale = m_actor->getScale();
    f32 myRot = m_actor->getAngle();

    for ( u32 i = 0; i < numPolylines; i++ )
    {
        PolyLine* pol = m_currentPolylines[i];
        const PointsCollisionComponent_Template::PolylineData& templateData = getTemplate()->getPolylineData(i);

        u32 numPoints = templateData.m_pointList.size();

        for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
        {
            Vec2d p = ( templateData.m_pointList[pointIndex] * myScale ).Rotate(myRot) + myPos;
            pol->setPosAt(p,pointIndex);
        }

        pol->forceRecomputeData();
        pol->m_physBody->updateAABB();
        pol->m_physBody->setPos3D(m_actor->getPos());
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PointsCollisionComponent_Template)
BEGIN_SERIALIZATION(PointsCollisionComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("polylines",m_polylines);

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(PointsCollisionComponent_Template,PolylineData)

    SERIALIZE_CONTAINER("points",m_pointList);
    SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
    SERIALIZE_MEMBER("regionType",m_regionType);
    SERIALIZE_MEMBER("loop",m_loop);
    SERIALIZE_MEMBER("movingPolyline",m_movingPolyline);

END_SERIALIZATION()

}
