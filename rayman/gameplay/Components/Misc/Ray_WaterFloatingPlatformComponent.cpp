#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaterFloatingPlatformComponent.h"
#endif //_ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_OBJECTCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/ObjectControllerComponent.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_WaterFloatingPlatformComponent)

BEGIN_SERIALIZATION_CHILD(Ray_WaterFloatingPlatformComponent)
END_SERIALIZATION()


BEGIN_VALIDATE_COMPONENT(Ray_WaterFloatingPlatformComponent)
    VALIDATE_COMPONENT_PARAM("Ray_SinkingPlatformComponent", m_objectControllerComponent, "object controller is mandatory");
END_VALIDATE_COMPONENT()

Ray_WaterFloatingPlatformComponent::Ray_WaterFloatingPlatformComponent()
: m_objectControllerComponent(NULL)
, m_forces(0.f)
, m_currentTorque(0.f)
, m_currentAngle(0.f)
, m_targetAngle(0.f)
, m_currentSpeed(0.f)
, m_currentHeight(0.f)
, m_targetHeight(0.f)
{
}

void Ray_WaterFloatingPlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_objectControllerComponent = m_actor->GetComponent<ObjectControllerComponent>();
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);

    m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);
}

Ray_WaterFloatingPlatformComponent::~Ray_WaterFloatingPlatformComponent()
{
}

void Ray_WaterFloatingPlatformComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_users.size() > 0 )
    {
        updatePlatform(_dt);
    }
    else
    {
        updateFloat(_dt);
    }

    applyRotation(_dt);
    applyTranslation(_dt);

    m_forces = 0.f;
}

void Ray_WaterFloatingPlatformComponent::updateFloat( f32 _dt )
{
    if ( fabs(m_currentHeight - m_targetHeight) <= 0.05f )
    {
        if ( m_targetHeight == 0.f )
        {
            m_targetHeight = Seeder::getSharedSeeder().GetFloat(getTemplate()->getFloatMinHeight(),getTemplate()->getFloatMaxHeight());
        }
        else
        {
            m_targetHeight = Seeder::getSharedSeeder().GetFloat(0.f,getTemplate()->getFloatMinHeight());
        }
    }

    f32 delta = getShortestAngleDelta(m_currentAngle,m_targetAngle);

    if ( fabs(delta) <= 0.05f )
    {
        bbool negative = m_targetAngle > 0.f;

        m_targetAngle = Seeder::getSharedSeeder().GetFloat(getTemplate()->getFloatMinAngle().ToRadians(),getTemplate()->getFloatMaxAngle().ToRadians());

        if ( negative)
        {
            m_targetAngle *= -1.f;
        }
    }
}

void Ray_WaterFloatingPlatformComponent::updatePlatform( f32 _dt )
{
    f32 weightAngle = 0.f;
    f32 weightHeight = 0.f;
    u32 numUsers = m_users.size();

    for ( u32 i = 0; i < numUsers; i++ )
    {
        const User& user = m_users[i];
        Actor* actor = AIUtils::getActor(user.m_user);

        if ( actor )
        {
            Vec2d pos2d = actor->get2DPos();

            applyWeightFromPoint(pos2d,user.m_weight,weightAngle,weightHeight);
        }
    }

    m_targetAngle = weightAngle * getTemplate()->getWeightToAngle();
    m_targetAngle = ClampAngle(Angle(bfalse,m_targetAngle),-getTemplate()->getMaxAngle(),getTemplate()->getMaxAngle()).ToRadians();

    m_targetHeight = weightHeight * getTemplate()->getWeightToHeight();
    m_targetHeight = Min(m_targetHeight,getTemplate()->getMaxHeight());
}

void Ray_WaterFloatingPlatformComponent::applyRotation( f32 _dt )
{
    f32 delta = getShortestAngleDelta(m_currentAngle,m_targetAngle);
    f32 angularForce = m_forces + (delta * getTemplate()->getStiff()) - ( m_currentTorque * getTemplate()->getDamp());

    m_currentTorque += angularForce*_dt;
    m_currentAngle += m_currentTorque*_dt;

    f32 newAngle = m_actor->getWorldInitialRot() - m_currentAngle;

    m_actor->setAngle(newAngle);
}

void Ray_WaterFloatingPlatformComponent::applyTranslation( f32 _dt )
{
    f32 delta = m_targetHeight - m_currentHeight;
    f32 linearForce = m_forces + (delta * getTemplate()->getStiff()) - ( m_currentSpeed * getTemplate()->getDamp());

    m_currentSpeed += linearForce*_dt;
    m_currentHeight += m_currentSpeed*_dt;

    f32 initialAngle = m_actor->getBoundWorldInitialAngle();
    Vec2d initialDir = Vec2d::Right.Rotate(initialAngle-MTH_PIBY2);
    Vec3d newPos = m_actor->getBoundWorldInitialPos() + (initialDir*m_currentHeight).to3d();

    m_actor->setPos(newPos);
}

void Ray_WaterFloatingPlatformComponent::applyLand( ObjectRef _poly, const Vec2d& _pos, const Vec2d& _speed, f32 _weight )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());

    gravityDir.normalize();

    Vec2d localPos;

    localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

    if ( localPos != Vec2d::Zero )
    {
        f32 force = _speed.norm() * _weight * getTemplate()->getWeightMultiplier() * getTemplate()->getForceMultiplier();
        f32 radius = localPos.norm();

        localPos.normalize();

        f32 dotProd = localPos.dot(gravityDir.getPerpendicular());
        f32 angleSign = localPos.cross(gravityDir) > 0.f ? -1.f : 1.f;
        f32 angle = f32_ACos(dotProd)*angleSign;
        
        force *= radius * f32_Sin(angle);

        m_forces += force;
    }
}

void Ray_WaterFloatingPlatformComponent::applyWeightFromPoint( const Vec2d& _pos, f32 _weight, f32& _weightAngle, f32& _weightHeight )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());

    gravityDir.normalize();

    Vec2d localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

    if ( localPos != Vec2d::Zero )
    {
        f32 dotProd = localPos.dot(gravityDir.getPerpendicular());
        f32 totalWeight = _weight * getTemplate()->getWeightMultiplier();

        const LinkComponent::ChildrenList& linkChildren = m_objectControllerComponent->getChildren();
        u32 numChildren = linkChildren.size();
        f32 width = 1.f;

        for (u32 i=0; i<numChildren; i++)
        {
            const ObjectPath &objpath=linkChildren[i].getPath();
            Pickable * obj = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objpath);
            if (obj)
            {
                BaseObject::ObjectType type = obj->getObjectType();

                if (type == BaseObject::eFrise)
                {
                    width = fabsf(obj->getRelativeAABB().getMax().m_x-obj->getRelativeAABB().getMin().m_x)*0.5f;

                    if ( width == 0.f )
                    {
                        width = 1.f;
                    }
                }
            }
        }

        f32 t = dotProd/width;

        t = Clamp(t,-1.f,1.f);

        _weightAngle += totalWeight * t;

        _weightHeight += totalWeight * (1.f-fabsf(t));

    }
}

void Ray_WaterFloatingPlatformComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processStickEvent(stickEvent);
    }
    else if ( EventHanging* hangingEvent = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
    {
        processHangEvent(hangingEvent);
    }
}

void Ray_WaterFloatingPlatformComponent::processStickEvent( EventStickOnPolyline* _stickEvent )
{
    if (_stickEvent->isSticked())
    {
        User user;
        user.m_user = _stickEvent->getActor();
        user.m_weight = _stickEvent->getWeight();
        m_users.push_back(user);

        applyLand(_stickEvent->getPolyline(),_stickEvent->getPos(),_stickEvent->getSpeed(),_stickEvent->getWeight());
    }
    else
    {
        for (u32 i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i].m_user == _stickEvent->getActor())
            {
                m_users.eraseNoOrder(i);
                break;
            }
        }
    }
}

void Ray_WaterFloatingPlatformComponent::processHangEvent( EventHanging* _hangEvent )
{
    if (_hangEvent->isHanging())
    {
        User user;
        user.m_user = _hangEvent->getActor();
        user.m_weight = _hangEvent->getWeight();
        m_users.push_back(user);

        const PolyLine* poly = AIUtils::getPolyLine(_hangEvent->getPolyline());

        if ( poly )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(_hangEvent->getEdge());
            Vec2d pos = edge.getPos() + ( edge.m_vector * _hangEvent->getT() );

            applyLand(_hangEvent->getPolyline(),pos,_hangEvent->getSpeed(),_hangEvent->getWeight());
        }
    }
    else
    {
        for (u32 i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i].m_user == _hangEvent->getActor())
            {
                m_users.eraseNoOrder(i);
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WaterFloatingPlatformComponent_Template)
BEGIN_SERIALIZATION(Ray_WaterFloatingPlatformComponent_Template)
    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_MEMBER("damp",m_damp);
    SERIALIZE_MEMBER("weightToAngle",m_weightToAngle);
    SERIALIZE_MEMBER("weightToHeight",m_weightToHeight);
    SERIALIZE_MEMBER("maxAngle",m_maxAngle);
    SERIALIZE_MEMBER("maxHeight",m_maxHeight);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("forceMultiplier",m_forceMultiplier);
    SERIALIZE_MEMBER("floatMinHeight",m_floatMinHeight);
    SERIALIZE_MEMBER("floatMaxHeight",m_floatMaxHeight);
    SERIALIZE_MEMBER("floatMinAngle",m_floatMinAngle);
    SERIALIZE_MEMBER("floatMaxAngle",m_floatMaxAngle);
END_SERIALIZATION()

Ray_WaterFloatingPlatformComponent_Template::Ray_WaterFloatingPlatformComponent_Template()
: m_stiff(5.f)
, m_damp(2.5f)
, m_weightToAngle(0.05f)
, m_weightToHeight(0.4f)
, m_maxAngle(bfalse,0.785f)
, m_maxHeight(2.f)
, m_weightMultiplier(1.f)
, m_forceMultiplier(0.005f)
, m_floatMinHeight(1.f)
, m_floatMaxHeight(2.5f)
, m_floatMinAngle(bfalse,0.15f)
, m_floatMaxAngle(bfalse,0.3f)
{
}

Ray_WaterFloatingPlatformComponent_Template::~Ray_WaterFloatingPlatformComponent_Template()
{
}

}

