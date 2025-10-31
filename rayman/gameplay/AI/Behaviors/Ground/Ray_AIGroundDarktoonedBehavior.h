#ifndef _ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    // just a standard roam + sends stims when going fast

    class Ray_AIGroundDarktoonedBehavior : public Ray_AIGroundRoamBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundDarktoonedBehavior, Ray_AIGroundRoamBehavior,4108703206);
        DECLARE_SERIALIZE()

    public:

        Ray_AIGroundDarktoonedBehavior();
        virtual ~Ray_AIGroundDarktoonedBehavior();

        virtual void        onActivate();
        virtual void        onEvent( Event* _event );
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:

        virtual bbool       updateMove( f32 _dt );

    private:

        ITF_INLINE const class Ray_AIGroundDarktoonedBehavior_Template * getTemplate() const;

        void        sendStim( const Vec2d& _pos, const Vec2d& _posEnd );

        Vec2d       m_prevPos;
        Vec2d       m_prevPosEnd;

        PhysShapePolygon                m_stimShape;
        PhysShapePolygon::PointsList    m_stimPoints;
    };

    class Ray_AIGroundDarktoonedBehavior_Template : public Ray_AIGroundRoamBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundDarktoonedBehavior_Template,Ray_AIGroundRoamBehavior_Template,3449732300);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIGroundDarktoonedBehavior);

    public:

        Ray_AIGroundDarktoonedBehavior_Template();
        ~Ray_AIGroundDarktoonedBehavior_Template(){}

        f32                         getStimEmissionSpeedLimit() const { return m_stimEmissionSpeedLimit; }
        const Vec2d&                getStimEmissionOffset() const { return m_stimEmissionOffset; }
        const Vec2d&                getStimEmissionOffsetEnd() const { return m_stimEmissionOffsetEnd; }
        u32                         getStimLevel() const { return m_stimLevel; }
        EReceivedHitType            getStimType() const { return m_stimType; }

    private:

        f32                         m_stimEmissionSpeedLimit;     // start emitting stims if we go faster than this
        Vec2d                       m_stimEmissionOffset;
        Vec2d                       m_stimEmissionOffsetEnd;
        u32                         m_stimLevel;
        EReceivedHitType            m_stimType;
    };

    ITF_INLINE const class Ray_AIGroundDarktoonedBehavior_Template * Ray_AIGroundDarktoonedBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundDarktoonedBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_