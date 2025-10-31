
#ifndef _ITF_PHYSISLAND_H_
#define _ITF_PHYSISLAND_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

namespace ITF
{

    class PhysIsland
    {

    public:

        PhysIsland( IslandId _id );
        ~PhysIsland();

        void                            update( f32 _delta );
        void                            postUpdate( f32 _delta );
        void                            insertBody( class PhysBody* _body );
        void                            removeBody( class PhysBody* _body );
        void                            insertPhantom( class PhysPhantom* _phantom );
        void                            removePhantom( class PhysPhantom* _phantom );
        void                            insertGravityModifier( struct ActorForceModifier* _modifier );
        void                            removeGravityModifier( struct ActorForceModifier* _modifier );
        void                            insertWindForce( struct ActorForceModifier* _modifier );
        void                            removeWindForce( struct ActorForceModifier* _modifier );

        IslandId                        getId() const { return m_id; }
        PhysBody*                       getCollidingBodies() const { return m_firstBody; }
        PhysPhantom*                    getPhantoms() const { return m_firstPhantom; }
        f32                             getZ() const { return m_id.getReferenceDepth(); }

        bbool                           getGravityModifier( const Vec2d& _pos, Vec2d& _gravity, Vec2d& _speedMultiplier ) const;
        void                            getWindForces( const Vec2d& _pos, const Vec2d& _speed, PhysActorForceArray& _res ) const;
        void                            drawPhantoms();
        void                            dumpCollidingBodies();
        void                            dumpPhantoms();

        void                            iterateSystem( ObjectRef _userData, bbool _end );

    private:

        PhysBody*                           m_firstBodyUpdate;
        PhysBody*                           m_lastBodyUpdate;
        PhysBody*                           m_firstBody;
        PhysBody*                           m_lastBody;
        PhysPhantom*                        m_firstPhantom;
        PhysPhantom*                        m_lastPhantom;

        ActorForceModifiersList             m_gravityModifiers;
        ActorForceModifiersList             m_windForces;
        IslandId                            m_id;
    };

}

#endif // _ITF_PHYSISLAND_H_

