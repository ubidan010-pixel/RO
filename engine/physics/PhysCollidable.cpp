#include "precompiled_engine.h"

#ifndef _ITF_PHYSCOLLIDABLE_H_
#include "engine/physics/PhysCollidable.h"
#endif //_ITF_PHYSCOLLIDABLE_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PhysCollidable)

void PhysCollidable::init(
    const PhysShape* _shape,
    const Vec2d& _pos,
    f32 _angle,
    ObjectRef _userData,
    ECollisionGroup _collisionGroup
#ifdef ITF_SUPPORT_DEBUGFEATURE
    , ObjectRef _owner
#endif
)
{
    m_island = NULL;
    m_userData = _userData;
    m_collisionGroup = _collisionGroup;
    m_recomputeAABB = btrue;
    m_nextCollideable = NULL;
    m_prevCollideable = NULL;
#ifdef ITF_SUPPORT_DEBUGFEATURE
    m_owner = _owner;
#endif //ITF_SUPPORT_DEBUGFEATURE

    m_sweep.m_prevPos = m_sweep.m_pos = _pos;
    m_sweep.m_angle = _angle;
    m_sweep.m_move = bfalse;
    m_sweep.m_movement = Vec2d::Zero;
    m_sweep.m_moveSize = 0.f;
    m_sweep.m_shape = _shape;

#ifdef ITF_SUPPORT_DEBUGFEATURE
    if (m_sweep.m_shape == NULL)
    {
        String8 message("unknown");
        if ( BaseObject* obj = m_owner.getObject() )
        {
            if ( Pickable* pickable = obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)) )
            {
                message = pickable->getUserFriendly();
            }
        }
        message = String8("setting a null shape! owner: ") + message;
        DEBUGINFO->addGenericInfo(message);
        ITF_ASSERT_CRASH(0, message.cStr());
    }
#endif //ITF_SUPPORT_DEBUGFEATURE
}

PhysCollidable::~PhysCollidable()
{
#ifndef ITF_FINAL
    ITF_ASSERT_CRASH(!m_island, "The object is deleted but not removed from island's lists...");
#endif // ITF_FINAL
}

void PhysCollidable::setPos3D( const Vec3d& _pos )
{
    setPos(_pos.truncateTo2D());
}

void PhysCollidable::setPos( const Vec2d& _pos )
{
    Vec2d movement = _pos - m_sweep.m_prevPos;
    f32 movementNorm = movement.norm();

    if ( movementNorm )
    {
        m_sweep.m_dir = movement/movementNorm;
        m_sweep.m_move = btrue;
    }
    else
    {
        m_sweep.m_dir = Vec2d::Zero;
        m_sweep.m_move = bfalse;
    }

    m_sweep.m_movement = movement;
    m_sweep.m_moveSize = movementNorm;
    m_sweep.m_pos = _pos;
    m_recomputeAABB = btrue;
}

void PhysCollidable::synchronizePos()
{
    m_sweep.m_prevPos = m_sweep.m_pos;
    m_sweep.m_move = bfalse;
    m_sweep.m_movement = Vec2d::Zero;
    m_sweep.m_moveSize = 0.f;
}

void PhysCollidable::updateAABB()
{
    if ( m_recomputeAABB )
    {
        m_sweep.m_shape->calculateAABB(m_sweep.m_pos,m_sweep.m_prevPos,m_sweep.m_angle,m_aabb);
        m_recomputeAABB = bfalse;
    }
}

}