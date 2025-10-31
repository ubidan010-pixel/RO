
#include "precompiled_engine.h"

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSISLAND_H_
#include "engine/physics/PhysIsland.h"
#endif //_ITF_PHYSISLAND_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(PhysPhantom)

void PhysPhantom::init( const PhysPhantomInitCommonInfo& _initCommonInfo )
{
    Super::init(_initCommonInfo.m_shape,
                _initCommonInfo.m_pos,
                _initCommonInfo.m_angle,
                _initCommonInfo.m_userData,
                _initCommonInfo.m_collisionGroup
#ifdef ITF_SUPPORT_DEBUGFEATURE
                ,_initCommonInfo.m_owner
#endif
                );
}

PhysPhantom::~PhysPhantom()
{
}

void PhysPhantom::setPos3D( const Vec3d& _pos )
{
    if(PhysIsland* pIsland = getIsland())
    {
        if(pIsland->getId() != DepthRange(_pos.m_z))
        {
            PHYSWORLD->removePhantom(this);
            PHYSWORLD->insertPhantom(this, _pos.m_z);
        }
    }

    PhysCollidable::setPos3D(_pos);
}

void PhysPhantom::setShape( class PhysShape* _shape )
{
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

}

