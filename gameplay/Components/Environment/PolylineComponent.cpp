#include "precompiled_gameplay.h"

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

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

#ifdef ITF_IPAD
#include <string>
#endif

namespace ITF
{

PolylineComponent::ProceduralPolyline::ProceduralPolyline()
: m_poly(NULL)
, m_phantom(NULL)
, m_parametersIndex(U32_INVALID)
, m_active(bfalse)
, m_registered(bfalse)
{
}

IMPLEMENT_OBJECT_RTTI(PolylineComponent)

BEGIN_SERIALIZATION_CHILD(PolylineComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(PolylineComponent)
VALIDATE_COMPONENT_PARAM("",m_animComponent!=NULL, "You need an AnimLightComponent if you use an animation polyline");
END_VALIDATE_COMPONENT()

PolylineComponent::PolylineComponent()
: Super()
, m_disabled(bfalse)
, m_animComponent(NULL)
, m_ready(bfalse)
, m_useGameMaterial(btrue)
, m_objectsRegistered(bfalse)
{
}

PolylineComponent::~PolylineComponent()
{
    clear();
}

void PolylineComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    for (PolylineParamsList::const_iterator it = getTemplate()->getPolylineParamsList().begin(); it != getTemplate()->getPolylineParamsList().end(); ++it )
    {
        const PolylineParameters& polyParams = *it;

        if ( polyParams.m_gameMaterial.getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(polyParams.m_gameMaterial, m_actor->getResourceGroup());
        }
    }

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventForceTransfer,1043808691),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);

    m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);

    // warn if no RenderSimpleAnimComponent was found: parallel computing of
    // animations prevents this component from working properly
    // don't invalidate though, this is a a bit too much...
    ITF_WARNING(
        m_actor, m_actor->GetComponent<RenderSimpleAnimComponent>(),
        "You need a RenderSimpleAnimComponent if you use an animation polyline"
        );

    if ( GetObjectClassCRC() == ITF_GET_STRINGID_CRC(PolylineComponent,4280564469) && !getTemplate()->getComputeLocalToGlobal() )
    {
        RenderSimpleAnimComponent* renderSimpleAnim = m_actor->GetComponent<RenderSimpleAnimComponent>();

        if ( renderSimpleAnim )
        {
            u32 numComponents = m_actor->GetAllComponents().size();
            bbool found = bfalse;

            for ( u32 i = 0; i < numComponents; i++ )
            {
                ActorComponent* component = m_actor->GetAllComponents()[i];

                if ( component && component != this && component->DynamicCast<PolylineComponent>(ITF_GET_STRINGID_CRC(PolylineComponent,4280564469)) != NULL )
                {
                    found = btrue;
                    break;
                }
            }

            if ( !found )
            {
                renderSimpleAnim->setComputeLocalToGlobal(bfalse);
            }
        }
    }
}

void PolylineComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( !m_ready )
    {
        readModel();
    }
}

void PolylineComponent::onBecomeActive()
{
    Super::onBecomeActive();

    registerPolylineDatas();
}

void PolylineComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    unregisterPolylineDatas();
}

void PolylineComponent::setDisabled(bbool _val)
{
    // clear if just disabled, but don't touch the m_ready flag
    if (_val && !m_disabled)
    {
        u32 numPolylines = m_currentPolylines.size();
        for ( u32 i = 0; i < numPolylines; i++ )
        {
            ProceduralPolyline* polyData = m_currentPolylines[i];
            deletePolyline(polyData);
        }
        m_currentPolylines.clear();
        m_hanging.clear();
        m_users.clear();
    }
    
    m_disabled = _val;
}

void PolylineComponent::clear()
{
    u32 numPolylines = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolylines; i++ )
    {
        ProceduralPolyline* polyData = m_currentPolylines[i];
        deletePolyline(polyData);
    }

    m_currentPolylines.clear();
    m_hanging.clear();
    m_users.clear();
    m_ready = bfalse;
    m_objectsRegistered = bfalse;
}

void PolylineComponent::readModel()
{
    if ( !m_animComponent )
    {
        return;
    }

    if ( !m_animComponent->isLoaded() )
    {
        return;
    }

    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene )
    {
        return;
    }

    SafeArray<AnimBoneDyn> & boneDynList = animMeshScene->m_AnimInfo.getCurrentBoneList();

    if ( boneDynList.size() == 0 )
    {
        return;
    }

    m_ready = btrue;
}

void PolylineComponent::onEvent( Event* _event)
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
    else if ( EventHangUpdate* hangUpdate = _event->DynamicCast<EventHangUpdate>(ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002)) )
    {
        processHangingUpdate(hangUpdate);
    }
    else if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
    {
        processHitStim(hit);
    }
    else if ( EventAnimUpdated* animUpdated = _event->DynamicCast<EventAnimUpdated>(ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305)) )
    {
        if ( m_ready && !m_disabled )
        {
            processAnimUpdated(animUpdated);
        }
    }
    else if ( EventForceTransfer* physBodyForce = _event->DynamicCast<EventForceTransfer>(ITF_GET_STRINGID_CRC(EventForceTransfer,1043808691)) )
    {
        processPhysBodyForceEvent(physBodyForce);
    }
    else if ( EventQueryBlocksHits* blockHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)) )
    {
        processQueryBlockHits(blockHits);
    }
    else if ( EventDisableCollision* disableCollisionEvent = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
    {
        processDisableCollision(disableCollisionEvent);
    }
}

void PolylineComponent::processNewUser( class EventStickOnPolyline* _event )
{
    if ( _event->isSticked() )
    {
        u32 numPolylines = m_currentPolylines.size();
        ProceduralPolyline* foundPoly = NULL;

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            foundPoly = m_currentPolylines[i];

            if ( foundPoly->m_poly->getRef() == _event->getPolyline() )
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
            onNewUser(_event,foundPoly);
        }
    }
    else
    {
        onRemoveUser(_event);
    }
}

void PolylineComponent::processUserEdgeChange( EventStickOnPolylineChangeEdge* _event )
{
    for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        PolyUsers& user = *it;

        if ( user.m_actor == _event->getActor() )
        {
            ProceduralPolyline* poly = getProcPolylineFromObjRef(_event->getPolyline());

            if ( poly )
            {
                user.m_polylineRef = poly->m_animRef;
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

void PolylineComponent::processNewHanging( EventHanging* _event )
{
    if ( _event->isHanging() )
    {
        ProceduralPolyline* foundPoly = getProcPolylineFromObjRef(_event->getPolyline());

        if ( foundPoly )
        {
            onNewHangActor(_event,foundPoly);
        }
    }
    else
    {
        onRemoveHangActor(_event);
    }
}

void PolylineComponent::processHangingUpdate( EventHangUpdate* _event )
{
    for ( HangingList::iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        HangingActor& hang = *it;

        if ( hang.m_actor == _event->getActor() )
        {
            ProceduralPolyline* foundPoly = getProcPolylineFromObjRef(_event->getPolyline());

            if ( foundPoly )
            {
                hang.m_edgeIndex = _event->getEdge();
                hang.m_t = _event->getT();
                hang.m_weight = _event->getWeight();
                hang.m_polylineRef = foundPoly->m_animRef;

                const PolyLineEdge& edge = foundPoly->m_poly->getEdgeAt(_event->getEdge());
                Vec2d newPos = edge.getPos() + (edge.m_vector*hang.m_t);

                hang.m_speed = ( newPos - hang.m_pos ) / LOGICDT;
                hang.m_pos = newPos;
            }

            return;
        }
    }
}

void PolylineComponent::onNewHangActor( EventHanging* _hang, ProceduralPolyline* _procPoly )
{
    HangingActor newHanging;

    newHanging.m_actor = _hang->getActor();
    newHanging.m_polylineRef = _procPoly->m_animRef;
    newHanging.m_edgeIndex = _hang->getEdge();
    newHanging.m_t = _hang->getT();
    newHanging.m_weight = _hang->getWeight();
    newHanging.m_speed = _hang->getSpeed();

    const PolyLineEdge& edge = _procPoly->m_poly->getEdgeAt(_hang->getEdge());
    newHanging.m_pos = edge.getPos() + (edge.m_vector*newHanging.m_t);

    m_hanging.push_back(newHanging);
}

void PolylineComponent::onRemoveHangActor( class EventHanging* _hang )
{
    for ( HangingList::iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        HangingActor& hang = *it;

        if ( hang.m_actor == _hang->getActor() )
        {
            m_hanging.erase(it);
            break;
        }
    }
}

void PolylineComponent::onNewUser( EventStickOnPolyline* _stick, ProceduralPolyline* _procPoly )
{
    PolyUsers newUser;

    newUser.m_actor = _stick->getActor();
    newUser.m_polylineRef = _procPoly->m_animRef;
    newUser.m_edgeIndex = _stick->getEdge();
    newUser.m_weight = _stick->getWeight();

    m_users.push_back(newUser);
}

void PolylineComponent::onRemoveUser( EventStickOnPolyline* _stick )
{
    for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        PolyUsers& user = *it;

        if ( user.m_actor == _stick->getActor() )
        {
            m_users.erase(it);
            break;
        }
    }
}

void PolylineComponent::processAnimUpdated( EventAnimUpdated* _animUpdated )
{
    updatePolylines(_animUpdated->getDt());
}

void PolylineComponent::processQueryBlockHits( EventQueryBlocksHits* _query )
{
    const PhysContactsContainer* contacts = _query->getContacts();

    if ( !contacts )
    {
        return;
    }

    u32 numContacts = contacts->size();
    u32 numPolylines = m_currentPolylines.size();

    for ( u32 contactIndex = 0; contactIndex < numContacts; contactIndex++ )
    {
        const SCollidableContact& contact = (*contacts)[contactIndex];

        if ( contact.m_edgeIndex != U32_INVALID )
        {
            for ( u32 polylineIndex = 0; polylineIndex < numPolylines; polylineIndex++ )
            {
                ProceduralPolyline* procPoly = m_currentPolylines[polylineIndex];

                if ( procPoly->m_poly && contact.m_collidableUserData == procPoly->m_poly->getRef() )
                {
                    const PolylineParameters* params = getTemplate()->getPolylineParameters(procPoly->m_parametersIndex);

                    if ( params && params->m_forceNoBlockHit )
                    {
                        _query->setCanBlockHits(bfalse);
                        return;
                    }
                    else
                    {
                        const PolyLineEdge& edge = procPoly->m_poly->getEdgeAt(contact.m_edgeIndex);
                        const GameMaterial_Template* mat = World::getGameMaterial(edge.getGameMaterial());
                        bbool solid = !mat || mat->isSolid();

                        _query->setCanBlockHits(solid);
                        return;
                    }
                }
            }
        }
    }
}

void PolylineComponent::processDisableCollision( class EventDisableCollision* _disableCollisionEvent )
{
    setDisabled(_disableCollisionEvent->getDisable());
}

PolylineComponent::ProceduralPolyline* PolylineComponent::getProcPolylineFromObjRef( ObjectRef _ref )
{
    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        ProceduralPolyline* poly = m_currentPolylines[i];

        if ( poly->m_poly->getRef() == _ref )
        {
            return poly;
        }
    }

    return NULL;
}

const PolyLine* PolylineComponent::getPolylineFromAnimRef( const StringID& _id ) const
{
    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        ProceduralPolyline* poly = m_currentPolylines[i];

        if ( poly->m_animRef == _id )
        {
            return poly->m_poly;
        }
    }

    return NULL;
}

const PolylineComponent::ProceduralPolyline* PolylineComponent::getProcPolylineFromObjRef( ObjectRef _ref ) const
{
    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        const ProceduralPolyline* poly = m_currentPolylines[i];

        if ( poly->m_poly->getRef() == _ref )
        {
            return poly;
        }
    }

    return NULL;
}

PolylineComponent::ProceduralPolyline* PolylineComponent::getProcPolylineFromAnimRef( const StringID& _ref )
{
    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        ProceduralPolyline* poly = m_currentPolylines[i];

        if ( poly->m_animRef == _ref )
        {
            return poly;
        }
    }

    return NULL;
}

PolylineComponent::ProceduralPolyline* PolylineComponent::createPolylineFromAnim( AnimPolyline* _animPolyline ) const
{
    u32 parametersIndex = getTemplate()->getPolylineParametersIndex(_animPolyline->m_Name,StringID::Invalid);
    const PolylineParameters* params = getTemplate()->getPolylineParameters(parametersIndex);

    if ( !params || ( params->m_environment == bfalse && params->m_usePhantom == bfalse && !params->m_regionType.isValid() ) )
    {
        return NULL;
    }

    const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(_animPolyline->m_Name);

    if ( !points )
    {
        return NULL;
    }

    ProceduralPolyline* newPoly = newAlloc(mId_Gameplay, ProceduralPolyline);
    u32 numPoints = _animPolyline->m_PolylinePointList.size();

    newPoly->m_animRef = _animPolyline->m_Name;
    newPoly->m_poly = newAlloc(mId_Gameplay, PolyLine);
    newPoly->m_poly->setOwnerActor(m_actor);
    newPoly->m_registered = bfalse;

    newPoly->m_poly->setLoop(_animPolyline->m_isLooping);

    for ( u32 i = 0; i < numPoints; i++ )
    {
        newPoly->m_poly->addPoint(points[i]);
    }

    newPoly->m_poly->forceRecomputeData();

    newPoly->m_pointsInfo.resize(numPoints);

    NamesMap::Iterator it(_animPolyline->m_NameToPolylinePoint);

    while ( !it.isEnd() )
    {
        u32 nameId = it.first();
        u32 index = it.second();
        PolylinePoint& pPoint = newPoly->m_pointsInfo[index];
        AnimPolylinePoint& animPoint = _animPolyline->m_PolylinePointList[index];

        pPoint.m_pointName = nameId;
        pPoint.m_boneName = animPoint.m_BoneUID;
        pPoint.m_boneIndex = animPoint.m_BonePtr->getIndex();
        pPoint.m_parametersIndex = getTemplate()->getPolylineParametersIndex(_animPolyline->m_Name,it.first());

        it.increment();
    }

    newPoly->m_parametersIndex = parametersIndex;
    newPoly->m_regionType = params->m_regionType;

    if ( params->m_useMovingPolyline )
    {
        newPoly->m_poly->m_physShape = newAlloc(mId_Phys,PhysShapeMovingPolyline(newPoly->m_poly));
    }
    else
    {
        newPoly->m_poly->m_physShape = newAlloc(mId_Phys,PhysShapePolyline(newPoly->m_poly));
    }

    if ( params->m_environment )
    {
        PhysBodyInitCommonInfo bodyInit;

        SET_OWNER(bodyInit, m_actor->getRef());
        bodyInit.m_static = btrue;
        bodyInit.m_pos = newPoly->m_poly->get2DPos();
        bodyInit.m_angle = newPoly->m_poly->getAngle();
        bodyInit.m_userData = newPoly->m_poly->getRef();
        bodyInit.m_weight = 1.f;
        bodyInit.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;
        bodyInit.m_collisionFilter = ECOLLISIONFILTER_CHARACTERS;

        newPoly->m_poly->m_physBody = PHYSWORLD->allocBody(newPoly->m_poly->m_physShape,bodyInit);
        newPoly->m_poly->m_isCollisionActive = btrue;
    }
    else
    {
        newPoly->m_poly->m_isCollisionActive = bfalse;
    }

    if ( params->m_usePhantom )
    {
        createPolylinePhantom(newPoly);
    }

    if (m_useGameMaterial)
    {
        for ( u32 edgeIndex = 0; edgeIndex < newPoly->m_pointsInfo.size(); edgeIndex++ )
        {
            const PolylineParameters* params = getPolylineParametersFromPolyPoint(newPoly,edgeIndex);
            newPoly->m_poly->setGameMaterialAt(params->m_gameMaterial.getStringID(),edgeIndex);
        }
    }
    else
    {
        for ( u32 edgeIndex = 0; edgeIndex < newPoly->m_pointsInfo.size(); edgeIndex++ )
        {
            newPoly->m_poly->setGameMaterialAt(GameMaterial_Template::s_nullMatID,edgeIndex);
        }
    }

    return newPoly;
}

void PolylineComponent::createPolylinePhantom( ProceduralPolyline* _proceduralPoly ) const
{
    PhysPhantomInitCommonInfo initInfo;

    SET_OWNER(initInfo, m_actor->getRef());
    initInfo.m_shape = _proceduralPoly->m_poly->m_physShape;
    initInfo.m_pos = _proceduralPoly->m_poly->get2DPos();
    initInfo.m_angle = _proceduralPoly->m_poly->getAngle();
    initInfo.m_userData = _proceduralPoly->m_poly->getRef();
    initInfo.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;

    _proceduralPoly->m_phantom = PHYSWORLD->allocPhantom(initInfo);
    _proceduralPoly->m_phantom->updateAABB();
}

void PolylineComponent::deletePolyline( ProceduralPolyline* _proceduralPoly )
{
    FixedArray <ObjectRef,32> detachActors;

    UsersList::iterator userIt = m_users.begin();

    while ( userIt != m_users.end() )
    {
        PolyUsers& user = *userIt;

        if ( user.m_polylineRef == _proceduralPoly->m_animRef )
        {
            detachActors.push_back(user.m_actor);

            if ( detachActors.full() )
            {
                break;
            }
        }

        ++userIt;
    }

    HangingList::iterator hangingIt = m_hanging.begin();

    while ( hangingIt != m_hanging.end() )
    {
        HangingActor& hanger = *hangingIt;

        if ( hanger.m_polylineRef == _proceduralPoly->m_animRef )
        {
            detachActors.push_back(hanger.m_actor);

            if ( detachActors.full() )
            {
                break;
            }
        }

        ++hangingIt;
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

    deletePolylinePhantom(_proceduralPoly);

    if ( m_objectsRegistered && _proceduralPoly->m_registered )
    {
        if ( _proceduralPoly->m_regionType.isValid() )
        {
            REGIONS_MANAGER->removeRegion(_proceduralPoly->m_poly->getRef());
        }

        if ( _proceduralPoly->m_poly->m_isCollisionActive )
        {
            PHYSWORLD->removeBody(_proceduralPoly->m_poly->m_physBody);
        }
    }

    SF_DEL(_proceduralPoly->m_poly->m_physShape);
    SF_DEL(_proceduralPoly->m_poly);
    SF_DEL(_proceduralPoly);
}

void PolylineComponent::deletePolylinePhantom( ProceduralPolyline* _proceduralPoly )
{
    if ( _proceduralPoly->m_phantom )
    {
        if ( m_objectsRegistered && _proceduralPoly->m_registered )
        {
            PHYSWORLD->removePhantom(_proceduralPoly->m_phantom);
        }

        PHYSWORLD->deallocPhantom(_proceduralPoly->m_phantom);
    }
}

void PolylineComponent::updatePolylines( f32 _dt )
{
    AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();

    if ( !animMeshScene ) 
    {
        return;
    }

    u32 numProcedurals = m_currentPolylines.size();

    for ( u32 i = 0; i < numProcedurals; i++ )
    {
        m_currentPolylines[i]->m_active = bfalse;
    }

    AnimSkeleton*           skeleton    = animMeshScene->m_AnimInfo.getCurrentSkeleton();
    ITF_VECTOR<StringID>&   polyRefList = animMeshScene->m_AnimInfo.getCurrentPolyList();
    AnimPolyline* pPolyline;

    for ( ITF_VECTOR<StringID>::const_iterator iter = polyRefList.begin();
         iter != polyRefList.end(); iter++ )
    {
        const StringID& polyRef = *iter;

        if ( skeleton->getPolyline(polyRef, pPolyline) )
        {
            ProceduralPolyline* foundPoly = NULL;

            for ( u32 i = 0; i < numProcedurals; i++ )
            {
                ProceduralPolyline* proceduralPoly = m_currentPolylines[i];

                if ( proceduralPoly->m_animRef == polyRef )
                {
                    foundPoly = proceduralPoly;
                    break;
                }
            }

            if ( !foundPoly )
            {
                foundPoly = createPolylineFromAnim(pPolyline);

                if ( foundPoly )
                {
                    m_currentPolylines.push_back(foundPoly);
                    numProcedurals++;
                }
                else
                {
                    continue;
                }
            }

            updateAnimPolyline(_dt,foundPoly,pPolyline, m_actor->isFlipped() != m_animComponent->getDefaultAnimFlip());

            if ( m_objectsRegistered && !foundPoly->m_registered )
            {
                if ( foundPoly->m_poly->m_physBody )
                {
                    PHYSWORLD->insertBody(foundPoly->m_poly->m_physBody,m_actor->getDepth());
                }

                if ( foundPoly->m_phantom )
                {
                    PHYSWORLD->insertPhantom(foundPoly->m_phantom,m_actor->getDepth());
                }

                if ( foundPoly->m_regionType.isValid() )
                {
                    REGIONS_MANAGER->addRegion(foundPoly->m_poly->getRef(),foundPoly->m_regionType,m_actor->getDepth());
                }

                foundPoly->m_registered = btrue;
            }
        }
    }

    for ( i32 i = 0; i < static_cast<i32>(numProcedurals); i++ )
    {
        ProceduralPolyline* proceduralPoly = m_currentPolylines[i];

        if ( !proceduralPoly->m_active )
        {
            deletePolyline(proceduralPoly);
            m_currentPolylines.eraseNoOrder(i);
            numProcedurals--;
        }
    }
}

void PolylineComponent::updateAnimPolyline( f32 _dt,
                                            ProceduralPolyline* _proceduralPoly,
                                            AnimPolyline* _animPoly,
                                            bbool _flipped ) const
{
    const PolylineParameters* params = getTemplate()->getPolylineParameters(_proceduralPoly->m_parametersIndex);
    u32 numPoints = _animPoly->m_PolylinePointList.size();
    PolyLine* poly = _proceduralPoly->m_poly;
    PhysShapeMovingPolyline* movingPoly;
    
    if ( params && params->m_useMovingPolyline && poly->m_physShape )
    {
        movingPoly = static_cast<PhysShapeMovingPolyline*>(poly->m_physShape);
        movingPoly->sync();
    }
    else
    {
        movingPoly = NULL;
    }

    _proceduralPoly->m_active = btrue;

    const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(_animPoly->m_Name);
    ITF_ASSERT_MSG(points, "Could not get polyline's points named : '%s'", _animPoly->m_Name.getDebugString());
    if(points)
    {
        if ( !_flipped )
        {
            for ( u32 i = 0; i < numPoints; i++ )
            {
                poly->setPosAt(points[i],i);
            }
        }
        else
        {
            for ( i32 i = static_cast<i32>(numPoints)-1; i >= 0; i-- )
            {
                poly->setPosAt(points[i],numPoints-i-1);
            }
        }
    }

    poly->update(_dt);

    if ( poly->m_physBody )
    {
        poly->m_physBody->updateAABB();
        poly->m_physBody->setPos3D(m_actor->getPos());
    }

    if ( _proceduralPoly->m_phantom )
    {
        _proceduralPoly->m_phantom->setPos3D(m_actor->getPos());
        _proceduralPoly->m_phantom->setRecomputeAABB();
        _proceduralPoly->m_phantom->updateAABB();
    }

    if ( movingPoly )
    {
        movingPoly->updateMoved();
    }
}

bbool PolylineComponent::containsPolyline( ObjectRef _polylineRef ) const
{
    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        ProceduralPolyline* procPoly = m_currentPolylines[i];

        if ( procPoly->m_poly->getRef() == _polylineRef )
        {
            return true;
        }
    }

    return bfalse;
}

f32 PolylineComponent::getWeight() const
{
    f32 weight = 0.0f;
    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        const HangingActor& hang = *it;
        weight += hang.m_weight;
    }

    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        const PolyUsers& user = *it;
        weight += user.m_weight;
   }
    return weight;
}

f32 PolylineComponent::getMaxUserWeight() const
{
    f32 maxWeight = 0.0f;
    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        const PolyUsers& user = *it;
        if( user.m_weight > maxWeight )
        {
            maxWeight = user.m_weight;
        }
    }
    return maxWeight;
}

const PolylineParameters* PolylineComponent::getPolylineParametersFromPolyPoint( const ProceduralPolyline* _poly,
                                                                                 u32 _edgeIndex ) const
{
    if ( _edgeIndex < _poly->m_pointsInfo.size() )
    {
        const PolylinePoint& p = _poly->m_pointsInfo[_edgeIndex];

        return getTemplate()->getPolylineParameters(p.m_parametersIndex);
    }
    else
    {
        return getTemplate()->getPolylineParameters(_poly->m_parametersIndex);
    }
}

void PolylineComponent::registerPolylineDatas()
{
    m_objectsRegistered = btrue;
}

void PolylineComponent::unregisterPolylineDatas()
{
    if ( m_objectsRegistered )
    {
        u32 numPolylines = m_currentPolylines.size();

        for ( u32 i = 0; i < numPolylines; i++ )
        {
            ProceduralPolyline* polyData = m_currentPolylines[i];

            if ( polyData->m_registered )
            {
                if ( polyData->m_phantom )
                {
                    PHYSWORLD->removePhantom(polyData->m_phantom);
                }

                if ( polyData->m_poly->m_isCollisionActive )
                {
                    PHYSWORLD->removeBody(polyData->m_poly->m_physBody);
                }

                if ( polyData->m_regionType.isValid() )
                {
                    REGIONS_MANAGER->removeRegion(polyData->m_poly->getRef());
                }

                polyData->m_registered = bfalse;
            }
        }

        m_objectsRegistered = bfalse;
    }
}

u32 PolylineComponent::getUserCount( const StringID& _polylineID ) const
{
    u32 count = 0;

    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); it++)
    {
        if (it->m_polylineRef == _polylineID)
        {
            count++;
        }
    }

    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); it++)
    {
        if (it->m_polylineRef == _polylineID)
        {
            count++;
        }
    }

    return count;
}

void PolylineComponent::getUserList( PolylineUserList& _userList ) const
{
    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); it++)
    {
        _userList.push_back(it->m_actor);
    }

    for ( HangingList::const_iterator it = m_hanging.begin(); it != m_hanging.end(); it++)
    {
        _userList.push_back(it->m_actor);
    }
}

#ifdef ITF_SUPPORT_EDITOR
void PolylineComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
        return;

    if ( !m_ready )
    {
        return;
    }

    u32 numPolys = m_currentPolylines.size();

    for ( u32 i = 0; i < numPolys; i++ )
    {
        ProceduralPolyline* poly = m_currentPolylines[i];
        PolyLine* pol = poly->m_poly;
        const PolylineParameters* params = getTemplate()->getPolylineParameters(poly->m_parametersIndex);

        drawInterface->drawPolyLine(pol,btrue);

        if ( params && params->m_useMovingPolyline && pol->m_physShape )
        {
            const PhysShapeMovingPolyline* movingPoly = static_cast<PhysShapeMovingPolyline*>(pol->m_physShape);
            drawInterface->drawPolyLine(movingPoly->getPreviousPolyline(),btrue);
        }

        if(drawInterface->showTextTips() && m_actor->isSelected())
        {
            for (u32 polEdgIt = 0; polEdgIt < pol->getVectorsCount(); polEdgIt++)
            {
                const PolyLineEdge& edg = pol->getEdgeAt(polEdgIt);
                Vec3d pos3d, res2d;
                Vec2d midPt = edg.getPos() + edg.m_vector * 0.5f;
                pos3d.m_x = midPt.m_x;
                pos3d.m_y = midPt.m_y;
                pos3d.m_z = pol->getDepth();
                GFX_ADAPTER->compute3DTo2D(pos3d, res2d);    

#ifdef ITF_DEBUG_STRINGID
                const StringID& gameMat = edg.getGameMaterial();
                String8 matName = gameMat.getDebugString();
                const GameMaterial_Template* pMat = World::getGameMaterial(matName);
                if (!pMat)
                    matName += " !!! NOT FOUND !!!";
                GFX_ADAPTER->drawDBGText(matName.cStr(), res2d.m_x, res2d.m_y);
#endif
            }
            for ( u32 j = 0; j < poly->m_pointsInfo.size(); j++ )
            {
                Vec3d bonePos = pol->getEdgeAt(j).getPos().to3d(); 
                bonePos.m_z = m_actor->getDepth();

                Vec3d res2d;
                GFX_ADAPTER->compute3DTo2D(bonePos, res2d);     

#ifdef ITF_DEBUG_STRINGID
                GFX_ADAPTER->drawDBGText(poly->m_pointsInfo[j].m_pointName.getDebugString(), res2d.m_x, res2d.m_y);
#endif
            }
        }
    }
}

void PolylineComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    if ( !m_animComponent )
    {
        return;
    }

    unregisterPolylineDatas();
    registerPolylineDatas();

    updatePolylines(0.f);
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PolylineComponent_Template)
BEGIN_SERIALIZATION(PolylineComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("polylineParams",m_polylineParams);
    SERIALIZE_MEMBER("computeLocalToGlobal", m_computeLocalToGlobal);

END_SERIALIZATION()

const PolylineParameters* PolylineComponent_Template::getPolylineParameters( u32 _index ) const
{
    if ( _index == U32_INVALID )
    {
        return &s_defaultPolylineParams;
    }
    else
    {
        return &m_polylineParams[_index];
    }
}

u32 PolylineComponent_Template::getPolylineParametersIndex( const StringID& _poly, const StringID& _point ) const
{
    u32 numParams = m_polylineParams.size();
    u32 defaultIndex = U32_INVALID;

    for ( u32 i = 0; i < numParams; i++ )
    {
        const PolylineParameters* params = &m_polylineParams[i];
        u32 numPoly = params->m_polylineIds.size();
        u32 numPoints = params->m_pointsIds.size();

        if ( numPoly == 0 && numPoints == 0 )
        {
            defaultIndex = i;
        }
        else
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                if ( _point == params->m_pointsIds[pointIndex] )
                {
                    return i;
                }

            }
            for ( u32 polyIndex = 0; polyIndex < numPoly; polyIndex++ )
            {
                if ( _poly == params->m_polylineIds[polyIndex] )
                {
                    return i;
                }
            }
        }
    }

    return defaultIndex;
}

const PolylineParameters PolylineComponent_Template::s_defaultPolylineParams;

PolylineParameters::PolylineParameters()
: m_speedLoss(0.f)
, m_weightMultiplier(1.f)
, m_landSpeedMultiplier(1.f)
, m_hitForce(5.f)
, m_impulseMultiplier(1.f)
, m_windMultiplier(1.f)
, m_gameMaterial(GameMaterial_Template::nullMatName)
, m_environment(btrue)
, m_usePhantom(bfalse)
, m_useMovingPolyline(bfalse)
, m_forceNoBlockHit(bfalse)
{
}

BEGIN_SERIALIZATION(PolylineParameters)

    SERIALIZE_MEMBER("speedLoss",m_speedLoss);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("landSpeedMultiplier",m_landSpeedMultiplier);
    SERIALIZE_MEMBER("hitForceMultiplier",m_hitForce);
    SERIALIZE_MEMBER("impulseMultiplier",m_impulseMultiplier);
    SERIALIZE_MEMBER("windMultiplier",m_windMultiplier);
    SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
    SERIALIZE_MEMBER("environment",m_environment);
    SERIALIZE_MEMBER("usePhantom",m_usePhantom);
    SERIALIZE_MEMBER("useMovingPolyline",m_useMovingPolyline);
    SERIALIZE_MEMBER("regionType",m_regionType);
    SERIALIZE_MEMBER("forceNoBlockHit",m_forceNoBlockHit);
    SERIALIZE_CONTAINER("polylines",m_polylineIds);
    SERIALIZE_CONTAINER("points",m_pointsIds);

END_SERIALIZATION()

}
