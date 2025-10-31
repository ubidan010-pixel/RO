///////////////////////////////////////////////////////////////////////////////
///////////////////////       polyline class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_PHYSTYPES_H_
#define _ITF_PHYSTYPES_H_

#ifndef _ITF_ANGLE_H_
#include "core/math/Angle.h"
#endif //_ITF_ANGLE_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
    struct SCollidableContact
    {
        SCollidableContact()
            : m_edgeIndex(U32_INVALID)
            , m_t(-1)
            , m_contactPoint(Vec2d::Zero)
            , m_testPosition(Vec2d::Zero)
            , m_normal(Vec2d::Zero)
        {
        }

        ObjectRef           m_collidable;           // obj we are colliding with
        ObjectRef           m_collidableUserData;   // obj user data (to access a polyline, actor, etc...)
        Vec2d               m_contactPoint;         // point of contact
        Vec2d               m_testPosition;         // the position of the test
        Vec2d               m_normal;               // normal of the contact
        u32                 m_edgeIndex;            // for polylines
        f32                 m_t;                    // time of the collision
    };

    struct SRayCastContact
    {
        SRayCastContact()
            : m_edgeIndex0(U32_INVALID)
            , m_edgeIndex1(U32_INVALID)
            , m_t0(-1)
            , m_t1(-1)
        {
        }

        ObjectRef           m_collidable;
        ObjectRef           m_collidableUserData;
        u32                 m_edgeIndex0;
        u32                 m_edgeIndex1;
        f32                 m_t0;
        f32                 m_t1;
    };

    enum ECollisionGroup : u32
    {
        ECOLLISIONGROUP_NONE                = 0x1,
        ECOLLISIONGROUP_POLYLINE            = 0x2,
        ECOLLISIONGROUP_CHARACTER           = 0x4,
        ECOLLISIONGROUP_ITEMS               = 0x8,
        ECOLLISIONGROUP_ABYSSLIGHT          = 0x10,
    };

    enum ECollisionFilter : u32
    {
        ECOLLISIONFILTER_ALL                = ECOLLISIONGROUP_POLYLINE|
                                              ECOLLISIONGROUP_CHARACTER|
                                              ECOLLISIONGROUP_ITEMS|
                                              ECOLLISIONGROUP_ABYSSLIGHT,
        ECOLLISIONFILTER_CHARACTERS         = ECOLLISIONGROUP_CHARACTER,
        ECOLLISIONFILTER_ENVIRONMENT        = ECOLLISIONGROUP_POLYLINE,
        ECOLLISIONFILTER_ITEMS              = ECOLLISIONGROUP_ITEMS,
        ECOLLISIONFILTER_ABYSSLIGHT          = ECOLLISIONGROUP_ABYSSLIGHT,
        ECOLLISIONFILTER_NONE               = 0,
    };

#ifdef ITF_SUPPORT_DEBUGFEATURE
    #define SET_OWNER(initInfo, owner) \
        initInfo.m_owner = owner;
#else
    #define SET_OWNER(initInfo, owner)
#endif

    struct PhysBodyInitCommonInfo
    {
        PhysBodyInitCommonInfo()
            : m_static(bfalse)
            , m_pos(Vec2d::Zero)
            , m_angle(-1)
            , m_weight(-1)
            , m_userData(0)
            , m_collisionGroup(ECOLLISIONGROUP_NONE)
            , m_collisionFilter(ECOLLISIONFILTER_NONE)
        {
        }

        bbool               m_static;
        Vec2d               m_pos;
        f32                 m_angle;
        f32                 m_weight;
        ObjectRef           m_userData;
        ECollisionGroup     m_collisionGroup;
        ECollisionFilter    m_collisionFilter;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        ObjectRef           m_owner;
#endif //ITF_SUPPORT_DEBUGFEATURE
    };

    struct PhysPhantomInitCommonInfo
    {
        PhysPhantomInitCommonInfo()
            : m_angle(-1)
            , m_pos(Vec2d::Zero)
            , m_shape(NULL)
            , m_collisionGroup(ECOLLISIONGROUP_NONE)
        {
        }

        Vec2d                   m_pos;
        f32                     m_angle;
        ObjectRef               m_userData;
        const class PhysShape*  m_shape;
        ECollisionGroup         m_collisionGroup;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        ObjectRef               m_owner;
#endif //ITF_SUPPORT_DEBUGFEATURE
    };

    struct PhysConstraintInitCommonInfo
    {
        PhysConstraintInitCommonInfo()
            : m_bodyA(NULL)
            , m_bodyB(NULL)
            , m_length(-1)
            , m_minLength(-1)
            , m_maxLength(-1)
            , m_stiff(-1)
            , m_damp(-1)
            , m_limitAngle(bfalse)
            , m_relaxLength(bfalse)
        {
        }

        class PhysBody*     m_bodyA;
        class PhysBody*     m_bodyB;
        f32                 m_length;
        Angle               m_minAngle;
        Angle               m_maxAngle;
        f32                 m_minLength;
        f32                 m_maxLength;
        f32                 m_stiff;
        f32                 m_damp;
        bbool               m_limitAngle;
        bbool               m_relaxLength;
    };

    struct PhysSweepInfo
    {
        PhysSweepInfo()
            : m_prevPos(Vec2d::Zero)
            , m_pos(Vec2d::Zero)
            , m_movement(Vec2d::Zero)
            , m_dir(Vec2d::Zero)
            , m_shape(NULL)
        {
        }

        Vec2d               m_prevPos;
        Vec2d               m_pos;
        f32                 m_angle;
        Vec2d               m_movement;
        Vec2d               m_dir;
        f32                 m_moveSize;
        bbool               m_move;
        const PhysShape*    m_shape;
    };

    typedef SafeArray <ObjectRef> PhysRemovedBodiesContainer;
    typedef FixedArray<SCollidableContact,15> PhysContactsContainer;
    typedef FixedArray<SRayCastContact,15> PhysRayCastContactsContainer;

    typedef DepthRange IslandId;

} //namespace ITF


#endif //_ITF_PHYSTYPES_H_
