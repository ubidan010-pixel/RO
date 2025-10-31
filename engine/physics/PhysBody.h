
#ifndef _ITF_PHYSBODY_H_
#define _ITF_PHYSBODY_H_

#ifndef _ITF_PHYSCOLLIDABLE_H_
#include "engine/physics/PhysCollidable.h"
#endif //_ITF_PHYSCOLLIDABLE_H_

namespace ITF
{

class PhysBody : public PhysCollidable
{
    DECLARE_OBJECT_CHILD_RTTI(PhysBody,PhysCollidable,1110232516);

public:

    virtual void                    setPos3D( const Vec3d& _pos );

    const Vec2d&                    getSpeed() const {return m_speed;}
    void                            setSpeed(const Vec2d& _spd) {m_speed = _spd;}

    const Vec2d&                    getForce() const { return m_force; }
    void                            addForce( const Vec2d& _force ) { m_force += _force; }
    void                            resetForce() { m_force = Vec2d::Zero; }

    bbool                           isStatic() const { return m_static; }

    ECollisionFilter                getCollisionFilter() const { return m_collisionFilter; }

    f32                             getWeight() const { return m_weight; }
    void                            setWeight( f32 _weight ) { m_weight = _weight; }

    f32                             getGravityMultiplier() const { return m_gravityMultiplier; }
    void                            setGravityMultiplier( f32 _mult ) { m_gravityMultiplier = _mult; }

    f32                             getWindMultiplier() const { return m_windMultiplier; }
    void                            setWindMultiplier( f32 _mult ) { m_windMultiplier = _mult; }

    void                            startFrame();
    void                            update( f32 _deltaTime );
    void                            postUpdate( f32 _deltaTime );

private:

    friend class PhysWorld;
    friend class PhysIsland;

    PhysBody();
    ~PhysBody();
    
    void init( class PhysShape*  _shape, const struct PhysBodyInitCommonInfo& _initInfo );

    Vec2d                           m_force;
    Vec2d                           m_speed;
    f32                             m_weight;
    f32                             m_gravityMultiplier;
    f32                             m_windMultiplier;
    ECollisionFilter                m_collisionFilter;
    bbool                           m_static;

    PhysBody*                       m_nextUpdate;
    PhysBody*                       m_prevUpdate;
};

}

#endif // _ITF_PHYSBODY_H_

