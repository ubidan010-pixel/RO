
#ifndef _ITF_PHYSCONSTRAINT_H_
#define _ITF_PHYSCONSTRAINT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class PhysConstraint
    {
    public:

        void                init( const struct PhysConstraintInitCommonInfo& _initInfo );
        void                preUpdate( f32 _delta );
        void                solve();

        class PhysBody*     getBodyA() const { return m_bodyA; }
        class PhysBody*     getBodyB() const { return m_bodyB; }
        void                setLength( f32 _val ) { m_length = _val; }
        f32                 getLength() const { return m_length; }
        const Angle&        getMinAngle() const { return m_minAngleOffset; }
        const Angle&        getMaxAngle() const { return m_maxAngleOffset; }
        void                setMinAngle( const Angle& _val ) { m_minAngleOffset = _val; }
        void                setMaxAngle( const Angle& _val ) { m_maxAngleOffset = _val; }
        f32                 getMinLength() const { return m_minLengthOffset; }
        f32                 getMaxLength() const { return m_maxLengthOffset; }
        void                setMinLength( f32 _val ) { m_minLengthOffset = _val; }
        void                setMaxLength( f32 _val ) { m_maxLengthOffset = _val; }
        bbool               getLimitAngle() const { return m_limitAngle; }
        bbool               getRelaxLength() const { return m_relaxLength; }
        void                setLimitAngle( bbool _val ) { m_limitAngle = _val; }
        void                setRelaxLength( bbool _val ) { m_relaxLength = _val; }

        PhysConstraint*     getNextCollideable() const { return m_nextConstraint; }
        PhysConstraint*     getPrevCollideable() const { return m_prevConstraint; }

    private:

        friend class PhysWorld;
        friend class PhysIsland;

        PhysConstraint();
        ~PhysConstraint();

        class PhysBody*     m_bodyA;
        class PhysBody*     m_bodyB;
        f32                 m_length;
        Angle               m_angle;
        Angle               m_minAngleOffset;
        Angle               m_maxAngleOffset;
        f32                 m_minLengthOffset;
        f32                 m_maxLengthOffset;
        f32                 m_stiff;
        f32                 m_damp;
        bbool               m_limitAngle;
        bbool               m_relaxLength;

        PhysConstraint*     m_nextConstraint;
        PhysConstraint*     m_prevConstraint;
        u32                 m_poolId;
    };
}

#endif // _ITF_PHYSCONSTRAINT_H_
