#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_LandOfTheDeadTeleport.h"
#endif //_ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_

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

IMPLEMENT_OBJECT_RTTI(Ray_LandOfTheDeadTeleportComponent)

BEGIN_SERIALIZATION_CHILD(Ray_LandOfTheDeadTeleportComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_LandOfTheDeadTeleportComponent)

    VALIDATE_COMPONENT_PARAM("AnimLightComponent", m_animComponent != NULL, "AnimLightComponent is mandatory");
    VALIDATE_COMPONENT_PARAM("mapTag", getTemplate()->getMapTag().isValid(), "You need to set the tag of the map to load");
    VALIDATE_COMPONENT_PARAM("shape",getTemplate()->getShape(), "Shape not set");

END_VALIDATE_COMPONENT()

Ray_LandOfTheDeadTeleportComponent::Ray_LandOfTheDeadTeleportComponent()
: m_animComponent(NULL)
, m_scaledShape(NULL)
, m_enabled(bfalse)
, m_firstUpdate(btrue)
, m_activated(bfalse)
, m_transition(bfalse)
{
}

Ray_LandOfTheDeadTeleportComponent::~Ray_LandOfTheDeadTeleportComponent()
{
    SF_DEL(m_scaledShape);
}

void Ray_LandOfTheDeadTeleportComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    if ( m_animComponent )
    {
        m_animComponent->enableDraw(bfalse);
    }
}

void Ray_LandOfTheDeadTeleportComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    bbool enable = canTrigger();

    if ( m_enabled != enable )
    {
        m_enabled = enable;

        m_animComponent->enableDraw(m_enabled);

        if ( m_enabled )
        {
            if ( m_firstUpdate )
            {
                m_animComponent->setAnim(getTemplate()->getIdleAnim());
            }
            else
            {
                m_animComponent->setAnim(getTemplate()->getAppearAnim());
                m_transition = btrue;
            }
        }
    }
    else
    {
        if ( m_transition && !m_animComponent->isAnimLooped() && m_animComponent->isSubAnimFinished() )
        {
            m_animComponent->setAnim(getTemplate()->getIdleAnim());
            m_transition = btrue;
        }
    }

    if ( m_enabled && !m_activated )
    {
        updateShape();
        triggerActors();
    }

    m_firstUpdate = bfalse;
}

bbool Ray_LandOfTheDeadTeleportComponent::canTrigger() const
{
    return RAY_GAMEMANAGER->hasAllTeethBeenReturned();
}

void Ray_LandOfTheDeadTeleportComponent::updateShape()
{
    if ( !m_scaledShape ||
          m_scaledShape->GetObjectClassCRC() != getTemplate()->getShape()->GetObjectClassCRC() )
    {
        SF_DEL(m_scaledShape);
        m_scaledShape = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getShape()->GetObjectClassCRC());
    }

    PhysShape::CopyShapeScaled(getTemplate()->getShape(), m_scaledShape, m_actor->getScale());
}

Vec2d Ray_LandOfTheDeadTeleportComponent::getShapePos() const
{
    Vec2d ret = getTemplate()->getShapeOffset() * m_actor->getScale().m_x;

    if ( m_actor->isFlipped() )
    {
        ret *= -1.f;
    }

    ret = ret.Rotate(m_actor->getAngle());

    return m_actor->get2DPos() + ret;
}

void Ray_LandOfTheDeadTeleportComponent::triggerActors()
{
    AABB myAABB;
    Vec2d shapePos = getShapePos();

    m_scaledShape->calculateAABB(shapePos, shapePos, m_actor->getAngle(), myAABB);

    const DepthRange depthRange(m_actor->getDepth());
    const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();

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

        RAY_GAMEMANAGER->stopAllSequences();

        // unlock world map spot + teleport back to it
        RAY_GAMEMANAGER->setWMSpotState(s_TRIGGER_DEATH, SPOT_STATE_COMPLETED);
        // Special case for land of the dead as it's a world with level properties
        // so current world and level must be set correctly
        RAY_GAMEMANAGER->setWMCurrentWorld(RAY_GAMEMANAGER->getWMSpotObjectPath(s_DE_A1), s_DE_A1);
        RAY_GAMEMANAGER->setWMCurrentLevel(RAY_GAMEMANAGER->getWMSpotObjectPath(s_DE_A1), s_DE_A1);

        m_activated = btrue;
        RAY_GAMEMANAGER->loadMapThroughLoadingScreen(getTemplate()->getMapTag());
        break;
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_LandOfTheDeadTeleportComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
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

IMPLEMENT_OBJECT_RTTI(Ray_LandOfTheDeadTeleportComponent_Template)
BEGIN_SERIALIZATION(Ray_LandOfTheDeadTeleportComponent_Template)

    SERIALIZE_MEMBER("mapTag",m_mapTag);
    SERIALIZE_MEMBER("appearAnim",m_appearAnim);
    SERIALIZE_MEMBER("idleAnim",m_idleAnim);
    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("shapeOffset",m_shapeOffset);

END_SERIALIZATION()

Ray_LandOfTheDeadTeleportComponent_Template::Ray_LandOfTheDeadTeleportComponent_Template()
: m_shape(NULL)
, m_shapeOffset(Vec2d::Zero)
{

}

Ray_LandOfTheDeadTeleportComponent_Template::~Ray_LandOfTheDeadTeleportComponent_Template()
{
    SF_DEL(m_shape);
}

}

