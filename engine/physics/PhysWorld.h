#ifndef _ITF_PHYSWORLD_H_
#define _ITF_PHYSWORLD_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_PHYSBROADPHASE
#include "engine/physics/PhysBroadPhase.h"
#endif //_ITF_PHYSBROADPHASE

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{

#define COLLIDEABLES_POOL_SIZE 300
#define CONSTRAINTS_POOL_SIZE 200

struct PhysWorldPool
{
    void*               m_pool;
    u32                 m_free;
    u32                 m_id;
    void*               m_first;
    void*               m_last;
};

class PhysWorld:public TemplateSingleton<PhysWorld>
{

public:

    PhysWorld();
    ~PhysWorld();

    class PhysBody*                 allocBody( PhysShape* _shape, const PhysBodyInitCommonInfo& _info );
    class PhysPhantom*              allocPhantom( const PhysPhantomInitCommonInfo& _info );
    class PhysConstraint*           allocConstraint( const PhysConstraintInitCommonInfo& _info );
    void                            deallocBody( PhysBody* _body );
    void                            deallocPhantom( PhysPhantom* _phantom );
    void                            deallocConstraint( PhysConstraint* _phantom );

    void                            insertBody( class PhysBody* _body, f32 _z );
    void                            removeBody( class PhysBody* _body );
    void                            insertBodies( const ITF_MAP<ObjectRef,PhysBody*>& _vBodies, f32 _z );
    void                            deleteBodies( const ITF_MAP<ObjectRef,PhysBody*>& _vBodies );
    void                            moveBodyToLayer( class PhysBody* _body, f32 _z  );

    void                            insertPhantom( class PhysPhantom* _phantom, f32 _z );
    void                            removePhantom( class PhysPhantom* _phantom  );
    void                            movePhantomToLayer( class PhysPhantom* _phantom, f32 _z  );

    void                            insertConstraint( class PhysConstraint* _constraint );
    void                            removeConstraint( class PhysConstraint* _constraint );

    void                            addGravityModifier( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifiers,
                                                        f32 _z );
    void                            removeGravityModifier( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier );
    void                            moveGravityModifierToLayer( const ObjectRef& _actor, f32 _z );

    void                            setGravity( const Vec2d& _gravity ) { m_gravity = _gravity; }
    Vec2d                           getGravity( const Vec2d& _pos, f32 _z ) const;

    void                            addWindForce( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifiers,
                                                  f32 _z );
    void                            removeWindForce( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier );
    void                            moveWindForceToLayer( const ObjectRef& _actor, f32 _z );

    void                            getWindForces( const Vec2d& _pos, const Vec2d& _speed, f32 _z, PhysActorForceArray& _res ) const;

    void                            update( f32 _deltaTime );
    void                            pause( bbool _pause ) { m_paused = _pause; }

    bbool                           checkEncroachment( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                                       const class PhysShape* _shape,
                                                       ECollisionFilter _filter,
                                                       f32 _z,
                                                       PhysContactsContainer& _contacts ) const;

    bbool                           collidePhantoms( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                                     const class PhysShape* _shape,
                                                     f32 _z,
                                                     ECollisionFilter _filter,
                                                     PhysContactsContainer& _contacts ) const;

    bbool                           rayCastEnvironment( const Vec2d& _p0, const Vec2d& _p1,
                                                        ECollisionFilter _filter,
                                                        f32 _z,
                                                        PhysRayCastContactsContainer& _contacts );

    bbool                           rayCastPhantoms( const Vec2d& _p0, const Vec2d& _p1,
                                                     ECollisionFilter _filter,
                                                     f32 _z,
                                                     PhysRayCastContactsContainer& _contacts );

    ObjectFactory*                  getShapesFactory() const { return m_shapesFactory; }

    void                            iterateSystem( ObjectRef _userData );

private:

    typedef SafeArray <PhysWorldPool> PhysPool;

    template <class COLLIDEABLETYPE>
    void                            getCollisionContacts( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                                          const class PhysShape* _shape,
                                                          ECollisionFilter _filter,
                                                          f32 _z,
                                                          COLLIDEABLETYPE* _collideables,
                                                          PhysContactsContainer& _contacts ) const;

    template <class COLLIDEABLETYPE>
    void                            getRaycastContacts( const Vec2d& _p0, const Vec2d& _p1,
                                                        ECollisionFilter _filter,
                                                        f32 _z,
                                                        COLLIDEABLETYPE* _collideables,
                                                        PhysRayCastContactsContainer& _contacts ) const;

    template <class COLLIDEABLETYPE>
    COLLIDEABLETYPE*                createPool( u32 _poolId ) const;
    class PhysConstraint*           createConstraintPool( u32 _poolId ) const;

    typedef ITF_LIST <class PhysIsland*> IslandList;

    void                            step( f32 _deltaTime );
    void                            solveConstraints();
    class PhysIsland*               getIsland( IslandId _id ) const;

    PhysPool                        m_bodyPoolList;
    PhysPool                        m_phantomPoolList;
    PhysPool                        m_contraintPoolList;
    PhysConstraint*                 m_constraintActiveFirst;
    PhysConstraint*                 m_constraintActiveLast;

    ActorForceModifiersList         m_gravityModifiers;
    ActorForceModifiersList         m_windForces;
    IslandList                      m_islands;
    PhysCollisionSolver             m_collisionSolver;
    ObjectFactory*                  m_shapesFactory;
    Vec2d                           m_gravity;
    bbool                           m_paused;

    u32                             m_idPoolBody;
    u32                             m_idPoolPhantom;
    u32                             m_idPoolConstraint;
};

template <class COLLIDEABLETYPE>
void PhysWorld::getCollisionContacts( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                       const PhysShape* _shape,
                                       ECollisionFilter _filter,
                                       f32 _z,
                                       COLLIDEABLETYPE* _collideables,
                                       PhysContactsContainer& _contacts ) const
{
    PhysBroadphase bPhase;
    FixedArray <COLLIDEABLETYPE*,128> tempCollideableList;

    AABB bBox;

    _shape->calculateAABB(_pos,_prevPos,_angle,bBox);

    bPhase.collide(bBox,_collideables,_filter,tempCollideableList);
    m_collisionSolver.checkCollideables(_prevPos,_pos,_angle,_shape,tempCollideableList,_contacts);
}

template <class COLLIDEABLETYPE>
void PhysWorld::getRaycastContacts( const Vec2d& _p0, const Vec2d& _p1,
                                     ECollisionFilter _filter,
                                     f32 _z,
                                     COLLIDEABLETYPE* _collideables,
                                     PhysRayCastContactsContainer& _contacts ) const
{
    PhysBroadphase bPhase;
    FixedArray <COLLIDEABLETYPE*,128> tempCollideableList;

    AABB bBox(_p0);

    bBox.grow(_p1);

    bPhase.collide(bBox,_collideables,_filter,tempCollideableList);
    m_collisionSolver.rayCastCollideables(_p0,_p1,tempCollideableList,_contacts);
}

template <class COLLIDEABLETYPE>
COLLIDEABLETYPE* PhysWorld::createPool( u32 _poolId ) const
{
    COLLIDEABLETYPE* colPool = newAlloc(mId_Phys,COLLIDEABLETYPE[COLLIDEABLES_POOL_SIZE]);
    COLLIDEABLETYPE* prevCol = NULL;

    for ( u32 i = 0; i < COLLIDEABLES_POOL_SIZE-1; i++ )
    {
        COLLIDEABLETYPE& col = colPool[i];
        col.m_poolId = _poolId;
        col.m_prevCollideable = prevCol;
        col.m_nextCollideable = &colPool[i+1];
        prevCol = &col;
    }

    COLLIDEABLETYPE& col = colPool[COLLIDEABLES_POOL_SIZE-1];
    col.m_poolId = _poolId;
    col.m_prevCollideable = prevCol;
    col.m_nextCollideable = NULL;

    return colPool;
}

#define PHYSWORLD PhysWorld::getptr()

}

#endif // _ITF_PHYSWORLD_H_

