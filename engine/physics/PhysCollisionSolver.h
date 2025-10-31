
#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#define _ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

namespace ITF
{
class PhysShape;
class PhysShapeCircle;
class PhysShapePolygon;
class PhysShapePolyline;

class PhysCollisionSolver
{

public:

    PhysCollisionSolver();
    ~PhysCollisionSolver();

    template <class COLLIDEABLETYPE>
    bbool           checkCollideables( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                       const PhysShape* _shape,
                                       const FixedArray <COLLIDEABLETYPE*,128>& _collideables,
                                       PhysContactsContainer& _contacts ) const;

    template <class COLLIDEABLETYPE>
    bbool           rayCastCollideables( const Vec2d& _p0, const Vec2d& _p1,
                                         const FixedArray <COLLIDEABLETYPE*,128>& _collideables,
                                         PhysRayCastContactsContainer& _contacts ) const;

    static bbool    collide( const PhysSweepInfo& _sweepInfoA,
                             const PhysSweepInfo& _sweepInfoB,
                             PhysContactsContainer& _contacts );

    static bbool    rayCast( const Vec2d& _p0, const Vec2d& _p1,
                             const PhysShape* _shape, const Vec2d& _pos, f32 _angle,
                             PhysRayCastContactsContainer& _contacts );

    static void     calculateSweepInfo( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                        const PhysShape* _shape, PhysSweepInfo& _ret );

private:

    // Circle - Circle
    static bbool    collideCircleCircle( const PhysSweepInfo& _sweepInfoA,
                                         const PhysSweepInfo& _sweepInfoB,
                                         PhysContactsContainer& _contacts );

    // Circle - Polyline
    static bbool    collideCirclePolyline( const PhysSweepInfo& _sweepInfoA,
                                           const PhysSweepInfo& _sweepInfoB,
                                           PhysContactsContainer& _contacts );
    static bbool    collideCircleMovingPolyline( const PhysSweepInfo& _sweepInfoA,
                                                 const PhysSweepInfo& _sweepInfoB,
                                                 PhysContactsContainer& _contacts );

    // Circle - Polygon
    static bbool    collideCirclePolygon( const PhysSweepInfo& _sweepInfoA,
                                          const PhysSweepInfo& _sweepInfoB,
                                          PhysContactsContainer& _contacts );

    // PolyLine - Polygon
    static bbool    collidePolylinePolygon( const PhysSweepInfo& _sweepInfoA,
                                            const PhysSweepInfo& _sweepInfoB,
                                            PhysContactsContainer& _contacts );

    // Polygon - Polygon
    static bbool    collidePolygonPolygon( const PhysSweepInfo& _sweepInfoA,
                                           const PhysSweepInfo& _sweepInfoB,
                                           PhysContactsContainer& _contacts );

    // Raycasting
    static bbool    rayCast( const Vec2d& _p0, const Vec2d& _p1,
                             const PhysShapeCircle* _shape, const Vec2d& _pos, f32 _angle,
                             PhysRayCastContactsContainer& _contacts );
    static bbool    rayCast( const Vec2d& _p0, const Vec2d& _p1,
                             const PhysShapePolyline* _shape, const Vec2d& _pos, f32 _angle,
                             PhysRayCastContactsContainer& _contacts );
    static bbool    rayCast( const Vec2d& _p0, const Vec2d& _p1,
                             const PhysShapePolygon* _shape, const Vec2d& _pos, f32 _angle,
                             PhysRayCastContactsContainer& _contacts );
};

template <class COLLIDEABLETYPE>
bbool PhysCollisionSolver::checkCollideables( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                              const class PhysShape* _shape,
                                              const FixedArray <COLLIDEABLETYPE*,128>& _collideables,
                                              PhysContactsContainer& _contacts ) const
{
    PhysSweepInfo sweepInfoA;

    calculateSweepInfo(_prevPos,_pos,_angle,_shape,sweepInfoA);

    u32 numContacts = _contacts.size();

    for (u32 index = 0; index<_collideables.size(); ++index) 
    {
        COLLIDEABLETYPE* collideable = _collideables[index];

        if ( collide(sweepInfoA,collideable->getSweepInfo(),_contacts) )
        {
            for ( u32 i = numContacts; i < _contacts.size(); i++ )
            {
                _contacts[i].m_collidable = collideable->getRef();
                _contacts[i].m_collidableUserData = collideable->getUserData();
            }

            numContacts = _contacts.size();
        }
    }

    // Sort the contacts by time
    bbool swap = btrue;
    SCollidableContact tempContact;

    for( u32 i = 0; (i < _contacts.size()) && swap; i++ )
    {
        swap = bfalse;

        for ( u32 j = 0; j < (_contacts.size()-1); j++ )
        {
            if ( _contacts[j+1].m_t < _contacts[j].m_t )
            { 
                tempContact = _contacts[j];
                _contacts[j] = _contacts[j+1];
                _contacts[j+1] = tempContact;
                swap = btrue;
            }
        }
    }

    return _contacts.size() != 0;
}

template <class COLLIDEABLETYPE>
bbool PhysCollisionSolver::rayCastCollideables( const Vec2d& _p0, const Vec2d& _p1,
                                               const FixedArray <COLLIDEABLETYPE*,128>& _collideables,
                                               PhysRayCastContactsContainer& _contacts ) const
{
    u32 numContacts = _contacts.size();

    for (u32 index = 0; index<_collideables.size(); ++index) 
    {
        COLLIDEABLETYPE* collideable = _collideables[index];

        if ( rayCast(_p0,_p1,collideable->getShape(),collideable->getPos(),collideable->getAngle(),_contacts) )
        {
            for ( u32 i = numContacts; i < _contacts.size(); i++ )
            {
                _contacts[i].m_collidable = collideable->getRef();
                _contacts[i].m_collidableUserData = collideable->getUserData();
            }

            numContacts = _contacts.size();

            if ( _contacts.full() )
            {
                break;
            }
        }
    }

    return _contacts.size() != 0;
}

}

#endif // _ITF_PHYSCOLLISIONSOLVER_H

