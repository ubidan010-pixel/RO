
#ifndef _ITF_PHYSBROADPHASE_H
#define _ITF_PHYSBROADPHASE_H

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSCOLLIDABLE_H_
#include "engine/physics/PhysCollidable.h"
#endif //_ITF_PHYSCOLLIDABLE_H_

#include "core/utility.h"

namespace ITF
{

class PhysBroadphase
{

public:

    PhysBroadphase();
    ~PhysBroadphase();

    template <class COLLIDEABLESTYPE>
    void        collide( const AABB& _aabb,
                         COLLIDEABLESTYPE* _bodies,
                         ECollisionFilter _filter,
                         FixedArray <COLLIDEABLESTYPE*,128>& _results ) const;

private:

};

template <class COLLIDEABLESTYPE>
void PhysBroadphase::collide( const AABB& _aabb,
                              COLLIDEABLESTYPE* _collideables,
                              ECollisionFilter _filter,
                              FixedArray <COLLIDEABLESTYPE*,128>& _results ) const
{
    const AABB aabb(_aabb.getMin() - MTH_EPSILON, _aabb.getMax() + MTH_EPSILON);
   
    for ( COLLIDEABLESTYPE* col = _collideables; col != NULL; col = static_cast<COLLIDEABLESTYPE*>(col->getNextCollideable()) )
    {
        if ( (toUnderlying(_filter) & toUnderlying(col->getCollisionGroup())) == 0 )
        {
            continue;
        }

        const AABB& bBoxB = col->getAABB();

        if ( !_aabb.checkOverlap(bBoxB) )
        {
            continue;
        }

        _results.push_back(static_cast<COLLIDEABLESTYPE*>(col));

#if !defined (ITF_FINAL) && defined (ITF_WINDOWS)
        if ( _results.full() )
        {
            ITF_ERROR("There are more than 128 objects overlapping each other");
            break;
        }
#endif
    }
}

}

#endif // _ITF_PHYSBROADPHASE_H

