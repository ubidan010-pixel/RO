
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONUSETELEPORT_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionUseTeleport.h"
#endif // _ITF_RAY_BTACTIONUSETELEPORT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_BASEAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/BaseAIControllerComponent.h"
#endif //_ITF_BASEAICONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionUseTeleport);

Ray_BTActionUseTeleport::Ray_BTActionUseTeleport()
: m_aiController(NULL)
, m_type(TELEPORTTYPE_UNKNOWN)
{
}

void Ray_BTActionUseTeleport::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_aiController = m_tree->getActor()->GetComponent<BaseAIControllerComponent>();
}

bbool Ray_BTActionUseTeleport::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"AIControllerComponent",m_aiController!=NULL,"%s: AIControllerComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionUseTeleport::decide( f32 _dt )
{
    return m_tree->getBlackBoard().factExists(Ray_AIFact_teleportEvent);
}

void Ray_BTActionUseTeleport::onActivate()
{
    Super::onActivate();

    EventPtr ptr;

    m_tree->getBlackBoard().getFact(Ray_AIFact_teleportEvent,ptr);

    if ( Ray_EventTriggerChangePage* changePage = ptr.m_event->DynamicCast<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481)) )
    {
        prepareChangePage(changePage);
    }
    else if ( Ray_EventTriggerEnterDoor* enterDoor = ptr.m_event->DynamicCast<Ray_EventTriggerEnterDoor>(ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494)) )
    {
        prepareEnterDoor(enterDoor);
    }
    else
    {
        m_type = TELEPORTTYPE_UNKNOWN;
    }

    ITF_ASSERT_MSG(m_type!=TELEPORTTYPE_UNKNOWN,"Teleporter type unknown");

    m_tree->getBlackBoard().removeFact(Ray_AIFact_teleportEvent);
    m_tree->getBlackBoard().setFact(Ray_AIFact_usingTeleport,btrue);
    
    if ( m_type == TELEPORTTYPE_CHANGEPAGE )
    {
        PhysComponent* physComponent = m_tree->getActor()->GetComponent<PhysComponent>();

        if ( physComponent )
        {
            physComponent->setDisabled(btrue);
            physComponent->setSpeed(Vec2d::Zero);
        }
    }
}

void Ray_BTActionUseTeleport::onDeactivate()
{
    Super::onDeactivate();

    if ( m_type == TELEPORTTYPE_CHANGEPAGE )
    {
        PhysComponent* physComponent = m_tree->getActor()->GetComponent<PhysComponent>();

        if ( physComponent )
        {
            physComponent->setDisabled(bfalse);
        }
    }

    m_tree->getBlackBoard().removeFact(Ray_AIFact_usingTeleport);
}

void Ray_BTActionUseTeleport::prepareChangePage( Ray_EventTriggerChangePage* _event )
{
    m_type = TELEPORTTYPE_CHANGEPAGE;
    m_changePage = *_event;
    m_changePageData.m_speed			= 0.0f;
    m_changePageData.m_moveDir			= Vec3d::Zero;
    m_changePageData.m_travelfinished	= bfalse;
    m_changePageData.m_enterEjectForce	= Vec2d::Zero;
    m_changePageData.m_spline.clear();

    Actor * actor = m_tree->getActor();

    // Compute actor pos on screen borders
    const ICameraController* cameraController = CAMERACONTROLLERMANAGER->getMainCameraController();

    Vec3d camPos = cameraController->getCameraPosition();
    f32 camHeight = tanf( cameraController->getCameraFocale() * 0.5f ) * cameraController->getDepth();
    f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;
    Vec3d actorPos = actor->getPos();
    Vec3d vec = camPos - actorPos;
    vec.m_y = 0.0f;
    f32 horizDistFromCamPos = ( vec ).dot( Vec3d::Right );
    vec = camPos - actorPos;
    vec.m_x = 0.0f;
    f32 vertiDistFromCamPos = ( vec ).dot( Vec3d::Up );

    Vec3d newStartPos = actorPos;
    if ( f32_Abs( horizDistFromCamPos ) - camWidth > 0.f )
    {
        newStartPos += Vec3d::Right * ( f32_Abs( horizDistFromCamPos ) - camWidth ) * getSign( horizDistFromCamPos );
    }
    if ( f32_Abs( vertiDistFromCamPos ) - camHeight > 0.f )
    {
        newStartPos += Vec3d::Up * ( f32_Abs( vertiDistFromCamPos ) - camHeight ) * getSign( vertiDistFromCamPos );
    }
    if ( !newStartPos.IsEqual( actorPos ) )
    {
        EventTeleport evtTeleport( newStartPos, 0.0f, bfalse );
        actor->onEvent( &evtTeleport );
    }

    // Compute the spline
    //--------------------
    f32 decelDuration = 0.5f;
    f32 accelDuration = 1.0f;

    Vec3d nextPoint;
    m_changePageData.m_spline.AddSplinePoint( actor->getPos(), Spline::interp_spline, 0.0f ); 
    Vec3d playerExitPoint = m_changePage.getExitPoint();

    if ( AIUtils::getPolylineRegion(actor->getDepth(),s_RegionName_Swim, playerExitPoint.truncateTo2D() ) == NULL )
    {
        playerExitPoint += ( actor->get2DPos() - AIUtils::getFeetPos(actor) ).to3d(m_changePage.getExitPoint().m_z );	//	add player feet dist to avoid beeing inside coll
    }  

    if( StickToPolylinePhysComponent * actorPhysComponent = actor->GetComponent<StickToPolylinePhysComponent>() )
    {
        const f32 speedMax = 5.0f;
        Vec2d speed = actorPhysComponent->getSpeed();
        f32 speedNorm = speed.norm();
        nextPoint = actor->getPos() + ( speed.normalize() * Min( speedNorm, 2.0f ) ).to3d(actor->getPos().m_z );
        m_changePageData.m_spline.AddSplinePoint( nextPoint, Spline::interp_spline, ( Min( speedNorm, speedMax ) / speedMax ) * decelDuration ); 
    }

    // To destination point
    //
    Vec3d enterPoint = m_changePage.getEnterPoint();
    enterPoint.m_z = actor->getDepth();
    m_changePageData.m_spline.AddSplinePoint( enterPoint, Spline::interp_spline, decelDuration + accelDuration ); 
    m_changePageData.m_spline.AddSplinePoint( playerExitPoint, Spline::interp_spline, decelDuration + accelDuration + accelDuration ); 
}

void Ray_BTActionUseTeleport::prepareEnterDoor( Ray_EventTriggerEnterDoor* _event )
{
    m_type = TELEPORTTYPE_ENTERDOOR;
    m_enterDoor = *_event;
    m_enterDoorData.m_state = ENTERDOORSTATE_NONE;

    Actor* teleporter = AIUtils::getActor(m_enterDoor.getSender());

    if ( teleporter )
    {
        LinkComponent* linkComponent = teleporter->GetComponent<LinkComponent>();

        if ( linkComponent )
        {
            const LinkComponent::ChildrenList& children = linkComponent->getChildren();

            if ( children.size() )
            {
                const ObjectPath& objPath = children[0].getPath();
                Pickable* pickable;

                if ( objPath.getIsAbsolute() )
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(teleporter,objPath);
                }

                if ( pickable )
                {
                    Ray_EventQueryDoorData doorQuery;

                    teleporter->onEvent(&doorQuery);

                    m_enterDoorData.m_prevDirToTarget = doorQuery.getEntry() - m_tree->getActor()->get2DPos();
                    m_enterDoorData.m_prevDirToTarget.normalize();
                    m_enterDoorData.m_state = ENTERDOORSTATE_WALKIN;
                    m_enterDoorData.m_targetActor = pickable->getRef();
                }
            }
        }
    }
}

bbool Ray_BTActionUseTeleport::update( f32 _dt )
{
    Super::update(_dt);

    bbool retVal;

    if ( m_tree->getBlackBoard().factExists(Ray_AIFact_usingTeleport) )
    {
        switch(m_type)
        {
            case TELEPORTTYPE_CHANGEPAGE:
                retVal = updateChangePage(_dt);
                break;
            case TELEPORTTYPE_ENTERDOOR:
                retVal = updateEnterDoor(_dt);
                break;
            default:
                retVal = bfalse;
                break;
        }
    }
    else
    {
        // someone aborted this task
        retVal = bfalse;
    }

    return retVal;
}

bbool Ray_BTActionUseTeleport::updateChangePage( f32 _dt )
{
    Actor* actor = m_tree->getActor();
    i32 p1, p2, p3;
    m_changePageData.m_spline.getPointsAtTime( m_changePageData.m_speed, p1, p2 );
    p3 = p2;
    if ( p1 >= 0 )
    {
        p3 = Min( p1 + 2, m_changePageData.m_spline.GetNumPoints() - 1 );
    }
    Vec3d p3Pos = m_changePageData.m_spline.GetNthPoint(p3);
    Vec3d dir = ( p3Pos - actor->getPos() ).normalize();
    f32 factor = Max( 0.0f, m_changePageData.m_moveDir.dot( dir ) );
    f32 accel = 0.0;
    if ( p1 > 0 )
    {
        factor = 1.0f;
        accel = m_changePageData.m_speed * 3.0f;
    }

    // Compute the move speed
    f32 interPointDuration = 0.0;

    if ( p1 >= 0 )
    {
        interPointDuration = ( m_changePageData.m_spline.getTimeAt( Min( p1 + 1, m_changePageData.m_spline.GetNumPoints() - 1 ) ) - m_changePageData.m_spline.getTimeAt( p1 ) );
    }

    f32 wantedSpeed = 1.0f;
    f32 speed = interPointDuration / wantedSpeed;
    f32 speedAdd = 0.0f;
    if ( interPointDuration != 0.0f )
    {
        speedAdd = ( accel / interPointDuration );
    }

    //_travelInfo.m_speed += ( _dt * interPointDuration / 1.0f )  +  ( _dt * speed * factor ) + ( ( accel / interPointDuration ) * _dt );
    m_changePageData.m_speed += _dt  +  ( _dt * speed * factor ) + (  speedAdd * _dt );

    // get the interpolated position
    Vec3d newPos;
    m_changePageData.m_spline.GetInterpolatedAtTime( m_changePageData.m_speed, newPos );

    m_changePageData.m_moveDir = newPos - actor->getPos(); 
    m_changePageData.m_moveDir.normalize();

    EventTeleport evtTeleport( newPos, 0.0f, bfalse );
    actor->onEvent( &evtTeleport );

    // Check if the travel is finished ( if the previous spline point is invalid)
    if ( !m_changePageData.m_travelfinished )
    {
        if ( ( p1 < 0 ) || ( p1 == p2 ) )
        {
            m_changePageData.m_travelfinished = btrue;
        }
    }

    return !m_changePageData.m_travelfinished;
}

bbool Ray_BTActionUseTeleport::updateEnterDoor( f32 _dt )
{
    bbool retVal = btrue;

    if ( m_enterDoorData.m_state != ENTERDOORSTATE_NONE )
    {
        Ray_EventQueryDoorData doorQuery;
        Vec2d walkDir;

        switch(m_enterDoorData.m_state)
        {
            case ENTERDOORSTATE_WALKIN:
                {
                    Actor* teleporter = AIUtils::getActor(m_enterDoor.getSender());

                    if ( teleporter )
                    {
                        teleporter->onEvent(&doorQuery);
                        walkDir = doorQuery.getEntry() - m_tree->getActor()->get2DPos();
                        walkDir.normalize();

                        if ( walkDir.dot(m_enterDoorData.m_prevDirToTarget) < 0.f )
                        {
                            Actor* targetActor = AIUtils::getActor(m_enterDoorData.m_targetActor);

                            if ( targetActor )
                            {
                                targetActor->onEvent(&doorQuery);

                                f32 sceneZ = AIUtils::getSceneZFromPickable(targetActor);
                                Vec3d teleportPos3d = doorQuery.getEntry().to3d(sceneZ);

                                if ( m_tree->getActor()->getTemplate()->isZForced() )
                                {
                                    teleportPos3d.m_z += m_tree->getActor()->getTemplate()->getForcedZ();
                                }

                                EventTeleport evtTeleport( teleportPos3d, 0.0f, bfalse );
                                m_tree->getActor()->onEvent( &evtTeleport );

                                m_enterDoorData.m_prevDirToTarget = doorQuery.getExit() - doorQuery.getEntry();
                                m_enterDoorData.m_prevDirToTarget.normalize();
                                m_enterDoorData.m_state = ENTERDOORSTATE_WALKOUT;
                            }
                            else
                            {
                                retVal = bfalse;
                            }
                        }
                        else
                        {
                            m_aiController->setMoveDir(walkDir);
                        }
                    }
                    else
                    {
                        retVal = bfalse;
                    }
                }
                break;
            case ENTERDOORSTATE_WALKOUT:
                Actor* teleporter = AIUtils::getActor(m_enterDoorData.m_targetActor);

                if ( teleporter )
                {
                    teleporter->onEvent(&doorQuery);
                    walkDir = doorQuery.getExit() - m_tree->getActor()->get2DPos();
                    walkDir.normalize();

                    if ( walkDir.dot(m_enterDoorData.m_prevDirToTarget) < 0.f )
                    {
                        retVal = bfalse;
                    }
                    else
                    {
                        m_aiController->setMoveDir(walkDir);
                    }
                }
                else
                {
                    retVal = bfalse;
                }
                break;
        }
    }
    else
    {
        retVal = bfalse;
    }

    return retVal;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionUseTeleport_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionUseTeleport_Template)

END_SERIALIZATION()

Ray_BTActionUseTeleport_Template::Ray_BTActionUseTeleport_Template()
{
}

}
