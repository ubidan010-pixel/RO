
#ifndef _ITF_PHYSPHANTOM
#define _ITF_PHYSPHANTOM

#ifndef _ITF_PHYSCOLLIDABLE_H_
#include "engine/physics/PhysCollidable.h"
#endif //_ITF_PHYSCOLLIDABLE_H_

namespace ITF
{

class PhysPhantom : public PhysCollidable
{
    DECLARE_OBJECT_CHILD_RTTI(PhysPhantom,PhysCollidable,126559815);

public:

    virtual void        setPos3D( const Vec3d& _pos );
    void                setShape( class PhysShape* _shape );

protected:

    friend class PhysWorld;
    friend class PhysIsland;

    PhysPhantom() {}
    virtual ~PhysPhantom();

    void init( const struct PhysPhantomInitCommonInfo& _initCommonInfo );
};

}

#endif // _ITF_PHYSPHANTOM

