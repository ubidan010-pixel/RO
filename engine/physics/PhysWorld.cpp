#include "precompiled_engine.h"

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSCONSTRAINT_H
#include "engine/physics/PhysConstraint.h"
#endif //_ITF_PHYSCONSTRAINT_H

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSBROADPHASE
#include "engine/physics/PhysBroadPhase.h"
#endif //_ITF_PHYSBROADPHASE

#ifndef _ITF_PHYSISLAND_H_
#include "engine/physics/PhysIsland.h"
#endif //_ITF_PHYSISLAND_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{
DECLARE_RASTER(physWorld_Update, RasterGroup_Physics, Color::yellow());

PhysWorld::PhysWorld()
: m_gravity(Vec2d(0.f,-40.f))
, m_paused(bfalse)
, m_idPoolBody(0)
, m_idPoolPhantom(0)
, m_idPoolConstraint(0)
{
    m_shapesFactory = newAlloc(mId_Phys, ObjectFactory);

    m_shapesFactory->RegisterObject<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
    m_shapesFactory->RegisterObject<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112));
    m_shapesFactory->RegisterObject<PhysShapeBox>(ITF_GET_STRINGID_CRC(PhysShapeBox,3520962473));
    m_shapesFactory->RegisterObject<PhysShapePolyline>(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535));

    PhysBody* bodyPool = createPool<PhysBody>(m_idPoolBody);
    PhysWorldPool worldPoolBody;

    worldPoolBody.m_free = COLLIDEABLES_POOL_SIZE;
    worldPoolBody.m_pool = bodyPool;
    worldPoolBody.m_id = m_idPoolBody++;
    worldPoolBody.m_first = &bodyPool[0];
    worldPoolBody.m_last = &bodyPool[COLLIDEABLES_POOL_SIZE-1];

    m_bodyPoolList.push_back(worldPoolBody);

    PhysPhantom* phantomPool = createPool<PhysPhantom>(m_idPoolPhantom);
    PhysWorldPool worldPoolPhantom;

    worldPoolPhantom.m_free = COLLIDEABLES_POOL_SIZE;
    worldPoolPhantom.m_pool = phantomPool;
    worldPoolPhantom.m_id = m_idPoolPhantom++;
    worldPoolPhantom.m_first = &phantomPool[0];
    worldPoolPhantom.m_last = &phantomPool[COLLIDEABLES_POOL_SIZE-1];

    m_phantomPoolList.push_back(worldPoolPhantom);

    PhysConstraint* constraintPool = createConstraintPool(m_idPoolConstraint);
    PhysWorldPool worldPoolConstraint;

    worldPoolConstraint.m_free = CONSTRAINTS_POOL_SIZE;
    worldPoolConstraint.m_pool = constraintPool;
    worldPoolConstraint.m_id = m_idPoolConstraint++;
    worldPoolConstraint.m_first = &constraintPool[0];
    worldPoolConstraint.m_last = &constraintPool[CONSTRAINTS_POOL_SIZE-1];

    m_contraintPoolList.push_back(worldPoolConstraint);

    m_constraintActiveFirst = NULL;
    m_constraintActiveLast = NULL;
}

PhysWorld::~PhysWorld()
{
    for ( IslandList::iterator it = m_islands.begin(); it != m_islands.end(); ++it )
    {
        delete *it;
    }

    u32 numBodyPools = m_bodyPoolList.size();
    u32 numPhantomPools = m_phantomPoolList.size();
    u32 numConstraintsPools = m_contraintPoolList.size();

    for ( u32 i = 0; i < numConstraintsPools; i++ )
    {
        ITF_ASSERT(m_contraintPoolList[i].m_free==CONSTRAINTS_POOL_SIZE);
        PhysConstraint* constraintPool = static_cast<PhysConstraint*>(m_contraintPoolList[i].m_pool);
        delete []constraintPool;
    }

    for ( u32 i = 0; i < numBodyPools; i++ )
    {
        ITF_ASSERT(m_bodyPoolList[i].m_free==COLLIDEABLES_POOL_SIZE);
        PhysBody* bodyPool = static_cast<PhysBody*>(m_bodyPoolList[i].m_pool);
        delete []bodyPool;
    }

    for ( u32 i = 0; i < numPhantomPools; i++ )
    {
        ITF_ASSERT(m_phantomPoolList[i].m_free==COLLIDEABLES_POOL_SIZE);
        PhysPhantom* phantomPool = static_cast<PhysPhantom*>(m_phantomPoolList[i].m_pool);
        delete []phantomPool;
    }

    ITF_ASSERT(m_constraintActiveFirst==NULL&&m_constraintActiveLast==NULL);
    ITF_ASSERT(m_gravityModifiers.empty());

    delete m_shapesFactory;
}

PhysConstraint* PhysWorld::createConstraintPool( u32 _poolId ) const
{
    PhysConstraint* constraintPool = newAlloc(mId_Phys,PhysConstraint[CONSTRAINTS_POOL_SIZE]);
    PhysConstraint* prevConstraint = NULL;

    for ( u32 i = 0; i < CONSTRAINTS_POOL_SIZE-1; i++ )
    {
        PhysConstraint& col = constraintPool[i];
        col.m_prevConstraint = prevConstraint;
        col.m_nextConstraint = &constraintPool[i+1];
        col.m_poolId = _poolId;
        prevConstraint = &col;
    }

    PhysConstraint& col = constraintPool[CONSTRAINTS_POOL_SIZE-1];
    col.m_prevConstraint = prevConstraint;
    col.m_nextConstraint = NULL;
    col.m_poolId = _poolId;

    return constraintPool;
}

PhysBody* PhysWorld::allocBody( PhysShape* _shape, const PhysBodyInitCommonInfo& _info )
{
    PhysWorldPool* usedPool = NULL;
    u32 numPools = m_bodyPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_bodyPoolList[i];

        if ( pool.m_first )
        {
            ITF_ASSERT(pool.m_free>0);
            pool.m_free--;
            usedPool = &pool;
            break;
        }
    }

    if ( !usedPool )
    {
        PhysBody* bodyPool = createPool<PhysBody>(m_idPoolBody);
        PhysWorldPool worldPoolBody;

        worldPoolBody.m_free = COLLIDEABLES_POOL_SIZE-1;
        worldPoolBody.m_pool = bodyPool;
        worldPoolBody.m_id = m_idPoolBody++;
        worldPoolBody.m_first = &bodyPool[0];
        worldPoolBody.m_last = &bodyPool[COLLIDEABLES_POOL_SIZE-1];

        m_bodyPoolList.push_back(worldPoolBody);

        usedPool = &m_bodyPoolList[m_bodyPoolList.size()-1];
    }

    PhysBody* newBody = static_cast<PhysBody*>(usedPool->m_first);

    ITF_ASSERT(newBody->m_prevCollideable==NULL);

    if ( newBody->m_nextCollideable )
    {
        newBody->m_nextCollideable->m_prevCollideable = NULL;
        usedPool->m_first = newBody->m_nextCollideable;
    }
    else
    {
        ITF_ASSERT(usedPool->m_last==newBody);
        usedPool->m_first = NULL;
        usedPool->m_last = NULL;
    }

    newBody->init(_shape,_info);
    newBody->m_nextCollideable = NULL;
    newBody->m_prevCollideable = NULL;

    return newBody;
}

PhysPhantom* PhysWorld::allocPhantom( const PhysPhantomInitCommonInfo& _info )
{
    PhysWorldPool* usedPool = NULL;
    u32 numPools = m_phantomPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_phantomPoolList[i];

        if ( pool.m_first )
        {
            ITF_ASSERT(pool.m_free>0);
            pool.m_free--;
            usedPool = &pool;
            break;
        }
    }

    if ( !usedPool )
    {
        PhysPhantom* phantomPool = createPool<PhysPhantom>(m_idPoolPhantom);
        PhysWorldPool worldPoolPhantom;

        worldPoolPhantom.m_free = COLLIDEABLES_POOL_SIZE-1;
        worldPoolPhantom.m_pool = phantomPool;
        worldPoolPhantom.m_id = m_idPoolPhantom++;
        worldPoolPhantom.m_first = &phantomPool[0];
        worldPoolPhantom.m_last = &phantomPool[COLLIDEABLES_POOL_SIZE-1];

        m_phantomPoolList.push_back(worldPoolPhantom);

        usedPool = &m_phantomPoolList[m_phantomPoolList.size()-1];
    }

    PhysPhantom* newPhantom = static_cast<PhysPhantom*>(usedPool->m_first);

    ITF_ASSERT(newPhantom->m_prevCollideable==NULL);

    if ( newPhantom->m_nextCollideable )
    {
        newPhantom->m_nextCollideable->m_prevCollideable = NULL;
        usedPool->m_first = newPhantom->m_nextCollideable;
    }
    else
    {
        ITF_ASSERT(usedPool->m_last==newPhantom);
        usedPool->m_first = NULL;
        usedPool->m_last = NULL;
    }

    newPhantom->init(_info);
    newPhantom->m_nextCollideable = NULL;
    newPhantom->m_prevCollideable = NULL;

    return newPhantom;
}

PhysConstraint* PhysWorld::allocConstraint( const PhysConstraintInitCommonInfo& _info )
{
    PhysWorldPool* usedPool = NULL;
    u32 numPools = m_contraintPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_contraintPoolList[i];

        if ( pool.m_first )
        {
            ITF_ASSERT(pool.m_free>0);
            pool.m_free--;
            usedPool = &pool;
            break;
        }
    }

    if ( !usedPool )
    {
        PhysConstraint* constraintPool = createConstraintPool(m_idPoolConstraint);
        PhysWorldPool worldPoolConstraint;

        worldPoolConstraint.m_free = CONSTRAINTS_POOL_SIZE-1;
        worldPoolConstraint.m_pool = constraintPool;
        worldPoolConstraint.m_id = m_idPoolConstraint++;
        worldPoolConstraint.m_first = &constraintPool[0];
        worldPoolConstraint.m_last = &constraintPool[CONSTRAINTS_POOL_SIZE-1];

        m_contraintPoolList.push_back(worldPoolConstraint);

        usedPool = &m_contraintPoolList[m_contraintPoolList.size()-1];
    }

    PhysConstraint* newConstraint = static_cast<PhysConstraint*>(usedPool->m_first);

    ITF_ASSERT(newConstraint->m_prevConstraint==NULL);

    if ( newConstraint->m_nextConstraint )
    {
        newConstraint->m_nextConstraint->m_prevConstraint = NULL;
        usedPool->m_first = newConstraint->m_nextConstraint;
    }
    else
    {
        ITF_ASSERT(usedPool->m_last==newConstraint);
        usedPool->m_first = NULL;
        usedPool->m_last = NULL;
    }

    newConstraint->init(_info);
    newConstraint->m_prevConstraint = NULL;
    newConstraint->m_nextConstraint = NULL;

    return newConstraint;
}

void PhysWorld::deallocBody( PhysBody* _body )
{
    if ( !_body )
    {
        return;
    }

    ITF_ASSERT(_body->m_nextCollideable==NULL);
    ITF_ASSERT(_body->m_prevCollideable==NULL);
    ITF_ASSERT(_body->m_nextUpdate==NULL);
    ITF_ASSERT(_body->m_prevUpdate==NULL);

    u32 numPools = m_bodyPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_bodyPoolList[i];

        if ( pool.m_id == _body->m_poolId )
        {
            ITF_ASSERT(pool.m_free<COLLIDEABLES_POOL_SIZE);

            pool.m_free++;

            if ( pool.m_last )
            {
                PhysBody* lastBody = static_cast<PhysBody*>(pool.m_last);
                ITF_ASSERT(lastBody->m_nextCollideable==NULL);
                lastBody->m_nextCollideable = _body;
                _body->m_prevCollideable = lastBody;
                pool.m_last = _body;
            }
            else
            {
                pool.m_first = _body;
                pool.m_last = _body;
                _body->m_nextCollideable = NULL;
                _body->m_prevCollideable = NULL;
            }

            if ( pool.m_id > 0 && pool.m_free == COLLIDEABLES_POOL_SIZE )
            {
                PhysBody* bodyPool = static_cast<PhysBody*>(pool.m_pool);
                delete []bodyPool;
                m_bodyPoolList.eraseNoOrder(i);
                return;
            }

            break;
        }
    }
}

void PhysWorld::deallocPhantom( PhysPhantom* _phantom )
{
    if ( !_phantom )
    {
        return;
    }

    ITF_ASSERT(_phantom->m_nextCollideable==NULL);
    ITF_ASSERT(_phantom->m_prevCollideable==NULL);

    u32 numPools = m_phantomPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_phantomPoolList[i];

        if ( pool.m_id == _phantom->m_poolId )
        {
            ITF_ASSERT(pool.m_free<COLLIDEABLES_POOL_SIZE);

            pool.m_free++;

            if ( pool.m_last )
            {
                PhysPhantom* lastPhantom = static_cast<PhysPhantom*>(pool.m_last);
                ITF_ASSERT(lastPhantom->m_nextCollideable==NULL);
                lastPhantom->m_nextCollideable = _phantom;
                _phantom->m_prevCollideable = lastPhantom;
                pool.m_last = _phantom;
            }
            else
            {
                pool.m_first = _phantom;
                pool.m_last = _phantom;
                _phantom->m_nextCollideable = NULL;
                _phantom->m_prevCollideable = NULL;
            }

            if ( pool.m_id > 0 && pool.m_free == COLLIDEABLES_POOL_SIZE )
            {
                PhysPhantom* phantomPool = static_cast<PhysPhantom*>(pool.m_pool);
                delete []phantomPool;
                m_phantomPoolList.eraseNoOrder(i);
                return;
            }

            break;
        }
    }
}

void PhysWorld::deallocConstraint( PhysConstraint* _constraint )
{
    if ( !_constraint )
    {
        return;
    }

    ITF_ASSERT(_constraint->m_nextConstraint==NULL);
    ITF_ASSERT(_constraint->m_prevConstraint==NULL);

    u32 numPools = m_contraintPoolList.size();

    for ( u32 i = 0; i < numPools; i++ )
    {
        PhysWorldPool& pool = m_contraintPoolList[i];

        if ( pool.m_id == _constraint->m_poolId )
        {
            ITF_ASSERT(pool.m_free<CONSTRAINTS_POOL_SIZE);

            pool.m_free++;

            if ( pool.m_last )
            {
                PhysConstraint* lastConstraint = static_cast<PhysConstraint*>(pool.m_last);
                ITF_ASSERT(lastConstraint->m_nextConstraint==NULL);
                lastConstraint->m_nextConstraint = _constraint;
                _constraint->m_prevConstraint = lastConstraint;
                pool.m_last = _constraint;
            }
            else
            {
                pool.m_first = _constraint;
                pool.m_last = _constraint;
                _constraint->m_nextConstraint = NULL;
                _constraint->m_prevConstraint = NULL;
            }

            if ( pool.m_id > 0 && pool.m_free == CONSTRAINTS_POOL_SIZE )
            {
                PhysConstraint* constraintPool = static_cast<PhysConstraint*>(pool.m_pool);
                delete []constraintPool;
                m_contraintPoolList.eraseNoOrder(i);
                return;
            }

            break;
        }
    }
}

void PhysWorld::insertBody( PhysBody* _body, f32 _z )
{
    ITF_ASSERT(_body != NULL);

    PhysIsland* island = getIsland(_z);

        if ( !island )
        {
        island = newAlloc(mId_Phys, PhysIsland(_z));
            m_islands.push_back(island);
        }

        island->insertBody(_body);
    }

void PhysWorld::removeBody( PhysBody* _body )
    {
    PhysIsland* island = _body->getIsland();

        ITF_ASSERT(island);

        if ( island )
        {
            island->removeBody(_body);
        }
    }

void PhysWorld::insertBodies( const ITF_MAP<ObjectRef,PhysBody*>& _vBodies, f32 _z )
{
    PhysIsland* island = getIsland(_z);

        if ( !island )
        {
        island = newAlloc(mId_Phys,PhysIsland(_z));
            m_islands.push_back(island);
        }

        for (ITF_MAP<ObjectRef,PhysBody*>::const_iterator iter = _vBodies.begin();iter!=_vBodies.end();++iter)
        {
            PhysBody* physBody = iter->second;
            island->insertBody(physBody);
        }
    }

void  PhysWorld::deleteBodies( const ITF_MAP<ObjectRef,PhysBody*>& _vBodies )
{
    for (ITF_MAP<ObjectRef,PhysBody*>::const_iterator iter = _vBodies.begin();iter!=_vBodies.end();++iter)
    {
        PhysBody* physBody = iter->second;
        PhysIsland* island = physBody->getIsland();

        ITF_ASSERT(island);
        if ( island )
        {
                island->removeBody(physBody);
            }
        }
    }

void PhysWorld::moveBodyToLayer( PhysBody* _body, f32 _z )
{
    PhysIsland* prevIsland = _body->getIsland();

    if ( prevIsland )
    {
        prevIsland->removeBody(_body);
    }

    insertBody(_body,_z);
}

void PhysWorld::insertPhantom( PhysPhantom* _phantom, f32 _z )
    {
    PhysIsland* island = getIsland(_z);

        if ( !island )
        {
        island = newAlloc(mId_Phys,PhysIsland(_z));
            m_islands.push_back(island);
        }

        island->insertPhantom(_phantom);
    }

void PhysWorld::removePhantom( PhysPhantom* _phantom )
    {
    PhysIsland* island = _phantom->getIsland();

        ITF_ASSERT(island);

        if ( island )
        {
            island->removePhantom(_phantom);
        }
    }

void PhysWorld::movePhantomToLayer( PhysPhantom* _phantom, f32 _z  )
{
    PhysIsland* prevIsland = _phantom->getIsland();

    if ( prevIsland )
    {
        prevIsland->removePhantom(_phantom);
    }

    insertPhantom(_phantom,_z);
}

void PhysWorld::insertConstraint( PhysConstraint* _constraint )
{
    ITF_ASSERT(_constraint->m_nextConstraint==NULL);
    ITF_ASSERT(_constraint->m_prevConstraint==NULL);

    if ( m_constraintActiveLast )
    {
        ITF_ASSERT(m_constraintActiveLast->m_nextConstraint==NULL);
        m_constraintActiveLast->m_nextConstraint = _constraint;
        _constraint->m_prevConstraint = m_constraintActiveLast;
        m_constraintActiveLast = _constraint;
    }
    else
    {
        m_constraintActiveFirst = _constraint;
        m_constraintActiveLast = _constraint;
    }
}

void PhysWorld::removeConstraint( PhysConstraint* _constraint )
{
    if ( _constraint->m_prevConstraint )
    {
        _constraint->m_prevConstraint->m_nextConstraint = _constraint->m_nextConstraint;
    }

    if ( _constraint->m_nextConstraint )
    {
        _constraint->m_nextConstraint->m_prevConstraint = _constraint->m_prevConstraint;
    }

    if ( m_constraintActiveLast == _constraint )
    {
        m_constraintActiveLast = _constraint->m_prevConstraint;
    }

    if ( m_constraintActiveFirst == _constraint )
    {
        m_constraintActiveFirst = _constraint->m_nextConstraint;
    }

    _constraint->m_nextConstraint = NULL;
    _constraint->m_prevConstraint = NULL;
}

void PhysWorld::addGravityModifier( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier, f32 _z )
{
    PhysIsland* island = getIsland(_z);

        if ( !island )
        {
            island = newAlloc(mId_Phys,PhysIsland(_z));
            m_islands.push_back(island);
        }

        ActorForceModifier* grav = newAlloc(mId_Phys,ActorForceModifier);

        grav->m_actor = _actor;
        grav->m_modifier = _modifier;
        grav->m_island = island;

        m_gravityModifiers.push_back(grav);

        island->insertGravityModifier(grav);
    }

void PhysWorld::removeGravityModifier( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier )
{
    for ( ActorForceModifiersList::iterator it = m_gravityModifiers.begin(); it != m_gravityModifiers.end(); ++it )
    {
        ActorForceModifier* mod = *it;

        if ( mod->m_actor == _actor && mod->m_modifier == _modifier )
        {
            mod->m_island->removeGravityModifier(mod);
            delete mod;
            m_gravityModifiers.erase(it);
            break;
        }
    }
}

void PhysWorld::moveGravityModifierToLayer( const ObjectRef& _actor, f32 _z )
{
    for ( ActorForceModifiersList::iterator it = m_gravityModifiers.begin(); it != m_gravityModifiers.end(); ++it )
    {
        ActorForceModifier* mod = *it;

        if ( mod->m_actor == _actor )
        {
            mod->m_island->removeGravityModifier(mod);
            mod->m_island = getIsland(_z);

            if ( mod->m_island )
            {
                mod->m_island->insertGravityModifier(mod);
            }
            else
            {
                delete mod;
                m_gravityModifiers.erase(it);
            }

            break;
        }
    }
}

void PhysWorld::addWindForce( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier, f32 _z )
{
    PhysIsland* island = getIsland(_z);

    if ( !island )
    {
        island = newAlloc(mId_Phys,PhysIsland(_z));
        m_islands.push_back(island);
    }

    ActorForceModifier* grav = newAlloc(mId_Phys,ActorForceModifier);

    grav->m_actor = _actor;
    grav->m_modifier = _modifier;
    grav->m_island = island;

    m_windForces.push_back(grav);

    island->insertWindForce(grav);
}

void PhysWorld::removeWindForce( const ObjectRef& _actor, const PhysForceModifier_Instance* _modifier )
{
    for ( ActorForceModifiersList::iterator it = m_windForces.begin(); it != m_windForces.end(); ++it )
    {
        ActorForceModifier* mod = *it;

        if ( mod->m_actor == _actor && mod->m_modifier == _modifier )
        {
            mod->m_island->removeWindForce(mod);
            delete mod;
            m_windForces.erase(it);
            break;
        }
    }
}

void PhysWorld::moveWindForceToLayer( const ObjectRef& _actor, f32 _z )
{
    for ( ActorForceModifiersList::iterator it = m_windForces.begin(); it != m_windForces.end(); ++it )
    {
        ActorForceModifier* mod = *it;

        if ( mod->m_actor == _actor )
        {
            mod->m_island->removeWindForce(mod);
            mod->m_island = getIsland(_z);

            if ( mod->m_island )
            {
                mod->m_island->insertWindForce(mod);
            }
            else
            {
                delete mod;
                m_windForces.erase(it);
            }

            break;
        }
    }
}

void PhysWorld::update( f32 _deltaTime )
{
    TIMERASTER_SCOPE(physWorld_Update);
    if ( !_deltaTime )
    {
        return;
    }

    // Apply constraint forces for limits
    for ( PhysConstraint* c = m_constraintActiveFirst; c != NULL; c = c->m_nextConstraint )
    {
        c->preUpdate(_deltaTime);
    }

    // Update AABBs, clear contacts, apply forces and then move non static bodies
    for ( IslandList::const_iterator it = m_islands.begin(); it != m_islands.end(); ++it )
    {
        (*it)->update(_deltaTime);
    }

    // solve constraints
    solveConstraints();

    // reset forces, update previous position
    for ( IslandList::const_iterator it = m_islands.begin(); it != m_islands.end(); ++it )
    {
        (*it)->postUpdate(_deltaTime);
    }
}

void PhysWorld::solveConstraints()
{
    static u32 NUM_ITERATIONS = 2;

    for ( u32 i = 0; i < NUM_ITERATIONS; i++ )
    {
        for ( PhysConstraint* c = m_constraintActiveFirst; c != NULL; c = c->m_nextConstraint )
        {
            c->solve();
        }
    }
}

bbool PhysWorld::checkEncroachment( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                    const PhysShape* _shape,
                                    ECollisionFilter _filter,
                                    f32 _z,
                                    PhysContactsContainer& _contacts ) const
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        getCollisionContacts(_prevPos,_pos,_angle,_shape,_filter,_z,island->getCollidingBodies(),_contacts);
    }

    return _contacts.size() != 0;
}

bbool PhysWorld::collidePhantoms( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle, const class PhysShape* _shape,
                                  f32 _z, ECollisionFilter _filter,
                                  PhysContactsContainer& _contacts ) const
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        getCollisionContacts(_prevPos,_pos,_angle,_shape,_filter,_z,island->getPhantoms(),_contacts);
    }

    return _contacts.size() != 0;
}

bbool PhysWorld::rayCastEnvironment( const Vec2d& _p0, const Vec2d& _p1,
                                     ECollisionFilter _filter,
                                     f32 _z,
                                     PhysRayCastContactsContainer& _contacts )
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        getRaycastContacts(_p0,_p1,_filter,_z,island->getCollidingBodies(),_contacts);
    }

    return _contacts.size() != 0;
}

bbool PhysWorld::rayCastPhantoms( const Vec2d& _p0, const Vec2d& _p1,
                                  ECollisionFilter _filter,
                                  f32 _z,
                                  PhysRayCastContactsContainer& _contacts )
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        getRaycastContacts(_p0,_p1,_filter,_z,island->getPhantoms(),_contacts);
    }

    return _contacts.size() != 0;
}

Vec2d PhysWorld::getGravity( const Vec2d& _pos, f32 _z ) const
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        Vec2d gravity, speedMultiplier;

        if ( island->getGravityModifier(_pos,gravity,speedMultiplier) )
        {
            return gravity;
        }
    }

    return m_gravity;
}

void PhysWorld::getWindForces( const Vec2d& _pos, const Vec2d& _speed, f32 _z, PhysActorForceArray& _res ) const
{
    PhysIsland* island = getIsland(_z);

    if ( island )
    {
        island->getWindForces(_pos,_speed,_res);
    }
}

PhysIsland* PhysWorld::getIsland( IslandId _id ) const
{
    for ( IslandList::const_iterator it = m_islands.begin(); it != m_islands.end(); ++it )
    {
        if ( (*it)->getId() == _id )
        {
            return *it;
        }
    }

    return NULL;
}

void PhysWorld::iterateSystem( ObjectRef _userData )
{
    static u32 NUM_ITERATIONS = 750;

    for ( u32 i = 0; i < NUM_ITERATIONS; i++ )
    {
        for ( PhysConstraint* constraint = m_constraintActiveFirst; constraint != NULL; constraint = constraint->m_nextConstraint )
        {
            if ( constraint )
            {
                PhysBody* bodyA = constraint->getBodyA();
                PhysBody* bodyB = constraint->getBodyB();

                if ( bodyA && bodyB && bodyA->getUserData() == _userData && bodyB->getUserData() == _userData )
                {
                    constraint->preUpdate(LOGICDT);
                }
            }
        }

        for ( IslandList::iterator it = m_islands.begin(); it != m_islands.end(); ++it )
        {
            PhysIsland* island = *it;

            if ( island )
            {
                island->iterateSystem(_userData,bfalse);
            }
        }

        for ( u32 j = 0; j < 2; j++ )
        {
            for ( PhysConstraint* constraint = m_constraintActiveFirst; constraint != NULL; constraint = constraint->m_nextConstraint )
            {
                PhysBody* bodyA = constraint->getBodyA();
                PhysBody* bodyB = constraint->getBodyB();

                if ( bodyA && bodyB && bodyA->getUserData() == _userData && bodyB->getUserData() == _userData )
                {
                    constraint->solve();
                }
            }
        }

        for ( IslandList::iterator it = m_islands.begin(); it != m_islands.end(); ++it )
        {
            PhysIsland* island = *it;

            if ( island )
            {
                island->iterateSystem(_userData,btrue);
            }
        }
    }
}

}

