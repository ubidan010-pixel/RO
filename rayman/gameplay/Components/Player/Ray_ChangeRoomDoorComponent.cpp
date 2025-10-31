#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHANGEROOMDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ChangeRoomDoorComponent.h"
#endif //_ITF_RAY_CHANGEROOMDOORCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ChangeRoomDoorComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ChangeRoomDoorComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
    
        SERIALIZE_MEMBER("goIn",m_goIn);
        SERIALIZE_MEMBER("goOut",m_goOut);
        
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_ChangeRoomDoorComponent)
    VALIDATE_COMPONENT_PARAM("shape",getTemplate()->getShape(), "Shape not set");
END_VALIDATE_COMPONENT()

Ray_ChangeRoomDoorComponent::Ray_ChangeRoomDoorComponent()
: m_scaledShape(NULL)
, m_canActivate(btrue)
, m_goIn(btrue)
, m_goOut(btrue)
{
}

Ray_ChangeRoomDoorComponent::~Ray_ChangeRoomDoorComponent()
{
    SF_DEL(m_scaledShape);
}

void Ray_ChangeRoomDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if ( m_goIn )
    {
        RAY_GAMEMANAGER->setChangeRoomDoor(m_actor->getRef());
    }
}

void Ray_ChangeRoomDoorComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

	if ( m_actor )
	{
		RAY_GAMEMANAGER->resetChangeRoomDoor(m_actor->getRef());
	}
}

void Ray_ChangeRoomDoorComponent::updateShape()
{
    if ( !m_scaledShape ||
          m_scaledShape->GetObjectClassCRC() != getTemplate()->getShape()->GetObjectClassCRC() )
    {
        SF_DEL(m_scaledShape);
        m_scaledShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getShape()->GetObjectClassCRC());
    }

    PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_scaledShape, m_actor->getScale());
}

Vec2d Ray_ChangeRoomDoorComponent::getShapePos() const
{
    Vec2d ret = getTemplate()->getShapeOffset() * m_actor->getScale().m_x;

    if ( m_actor->isFlipped() )
    {
        ret *= -1.f;
    }

    ret = ret.Rotate(m_actor->getAngle());

    return m_actor->get2DPos() + ret;
}

void Ray_ChangeRoomDoorComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( RAY_GAMEMANAGER->isSequencePlaying() )
    {
        return;
    }

    if ( m_goOut )
    {
        updateShape();
        triggerActors();
    }
}

void Ray_ChangeRoomDoorComponent::triggerActors()
{
    AABB myAABB;
    Vec2d shapePos = getShapePos();

    m_scaledShape->calculateAABB(shapePos, shapePos, m_actor->getAngle(), myAABB);

    const DepthRange depthRange(m_actor->getDepth());
    const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();
    FixedArray <ActorRef,32> actorsInside;

    for (i32 i = 0; i < playerCount; ++i)
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if ( !player->getActive() || player->isDead() )
        {
            continue;
        }

        Actor* playerActor = player->getActor();
        if ( !playerActor || depthRange != DepthRange(playerActor->getDepth()) )
        {
            continue;
        }

        EventQueryPhysShape queryShape;

        playerActor->onEvent(&queryShape);

        if ( !queryShape.getPhysShape() )
        {
            continue;
        }

        AABB playerAABB;

        queryShape.getPhysShape()->calculateAABB(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), playerAABB);

        if ( !myAABB.checkOverlap(playerAABB) )
        {
            continue;
        }

        PhysContactsContainer contacts;
        PhysSweepInfo sweepA, sweepB;

        PhysCollisionSolver::calculateSweepInfo(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), queryShape.getPhysShape(), sweepA);
        PhysCollisionSolver::calculateSweepInfo(shapePos, shapePos, m_actor->getAngle(), m_scaledShape, sweepB);
        PhysCollisionSolver::collide(sweepA, sweepB, contacts);

        if ( contacts.size() == 0 )
        {
            continue;
        }

        actorsInside.push_back(playerActor->getRef());

        if ( m_canActivate )
        {
            activate();
            break;
        }
    }

    if ( actorsInside.size() == 0 && !m_canActivate )
    {
        m_canActivate = btrue;
    }
}

void Ray_ChangeRoomDoorComponent::activate()
{
    RAY_GAMEMANAGER->triggerChangeRoomSequence(bfalse);
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_ChangeRoomDoorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
    {
        return;
    }

    if ( m_scaledShape )
    {
        DebugDraw::shape(getShapePos(), m_actor->getPos().m_z, m_actor->getAngle(), m_scaledShape,
            Color::white(), 0.f, "trigger", 85 * MTH_DEGTORAD);
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ChangeRoomDoorComponent_Template)
BEGIN_SERIALIZATION(Ray_ChangeRoomDoorComponent_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("shapeOffset",m_shapeOffset);

END_SERIALIZATION()

Ray_ChangeRoomDoorComponent_Template::Ray_ChangeRoomDoorComponent_Template()
: m_shape(NULL)
, m_shapeOffset(Vec2d::Zero)
{

}
Ray_ChangeRoomDoorComponent_Template::~Ray_ChangeRoomDoorComponent_Template()
{
    SF_DEL(m_shape);
}
}