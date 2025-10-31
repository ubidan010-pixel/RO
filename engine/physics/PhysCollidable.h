
#ifndef _ITF_PHYSCOLLIDABLE_H_
#define _ITF_PHYSCOLLIDABLE_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

namespace ITF
{

    class PhysCollidable : public BaseObject
    {
        DECLARE_OBJECT_CHILD_RTTI(PhysCollidable,BaseObject,1764848679);

    public:

        const AABB&                     getAABB() const { return m_aabb; }
        void                            setRecomputeAABB() { m_recomputeAABB = btrue; }

        const PhysSweepInfo&            getSweepInfo() const { return m_sweep; }
        virtual void                    setPos3D( const Vec3d& _pos );
        void                            setPos( const Vec2d& _pos );
        const Vec2d&                    getPos() const { return m_sweep.m_pos; }
        const Vec2d&                    getPrevPos() const { return m_sweep.m_prevPos; }
        void                            synchronizePos();

        void                            setAngle( f32 _angle ) { m_sweep.m_angle = _angle; }
        const f32                       getAngle() const { return m_sweep.m_angle; }

        const class PhysShape*          getShape() const { return m_sweep.m_shape; }
        class PhysIsland*               getIsland() const { return m_island; }
        void                            setIsland( class PhysIsland* _island ) { m_island = _island; }

        ObjectRef                       getUserData() const { return m_userData; }
        ECollisionGroup                 getCollisionGroup() const { return m_collisionGroup; }

        void                            updateAABB();

        PhysCollidable*                 getNextCollideable() const { return m_nextCollideable; }
        PhysCollidable*                 getPrevCollideable() const { return m_prevCollideable; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ObjectRef                       m_owner;
#endif //ITF_SUPPORT_DEBUGFEATURE
    protected:

        friend class PhysWorld;
        friend class PhysIsland;

        PhysCollidable() : m_island(NULL), m_nextCollideable(NULL), m_prevCollideable(NULL), m_poolId(U32_INVALID) {}
        virtual ~PhysCollidable();

        void init(
            const PhysShape* _shape,
            const Vec2d& _pos,
            f32 _angle,
            ObjectRef _userData,
            ECollisionGroup _collisionGroup
#ifdef ITF_SUPPORT_DEBUGFEATURE
            , ObjectRef _owner
#endif //ITF_SUPPORT_DEBUGFEATURE
            );

        class PhysIsland*               m_island;
        PhysSweepInfo                   m_sweep;
        AABB                            m_aabb;
        ObjectRef                       m_userData;
        ECollisionGroup                 m_collisionGroup;
        bbool                           m_recomputeAABB;

        PhysCollidable*                 m_nextCollideable;
        PhysCollidable*                 m_prevCollideable;
        u32                             m_poolId;
    };

}

#endif // _ITF_PHYSCOLLIDABLE_H_

