
#ifndef _ITF_PHYSFORCEMODIFIER_H_
#define _ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

struct lua_State;

namespace ITF
{
    class PhysForceModifier
    {
    public:

        enum Type : i32
        {
            TYPE_CIRCLE,
            TYPE_BOX,
            TYPE_POLYLINE,
        };

        struct BoxData
        {
            DECLARE_SERIALIZE()

            BoxData() : m_width(1.f), m_height(1.f) {}

            f32     m_width;
            f32     m_height;
        };

        struct CircleData
        {
            DECLARE_SERIALIZE()

            CircleData() : m_radius(1.f), m_start(bfalse,0.f), m_end(bfalse,0.f) {}

            f32     m_radius;
            Angle   m_start;
            Angle   m_end;
        };

        struct PolylineData
        {
            DECLARE_SERIALIZE()

            PolylineData() {}

            StringID            m_animId;
        };

    public:

        DECLARE_SERIALIZE()

        PhysForceModifier();

        ITF_INLINE const BoxData&       getBox() const { return m_box; }
        ITF_INLINE const CircleData&    getCircle() const { return m_circle; }
        ITF_INLINE const PolylineData&  getPolyline() const { return m_polyline; }
        ITF_INLINE PolylineData&        getPolylineForUpdate() { ITF_ASSERT(m_type == TYPE_POLYLINE); return m_polyline; }
        ITF_INLINE const Vec2d&         getForce() const { return m_force; }
        ITF_INLINE const Vec2d&         getOffset() const { return m_offset; }
        ITF_INLINE const Angle&         getRotation() const { return m_rotation; }
        ITF_INLINE Type                 getType() const { return m_type; }
        ITF_INLINE f32                  getGradientPercent() const { return m_gradientPercent; }
        ITF_INLINE f32                  getCenterForce() const { return m_centerForce; }
        ITF_INLINE f32                  getCenterForceMaxSpeed() const { return m_centerForceMaxSpeed; }
        ITF_INLINE f32                  getCenterForceSpeed2Force() const { return m_centerForceSpeed2Force; }
        ITF_INLINE f32                  getSpeedMultiplierX() const { return m_speedMultiplierX; }
        ITF_INLINE f32                  getSpeedMultiplierY() const { return m_speedMultiplierY; }
        ITF_INLINE bbool                getIsPoint() const { return m_point; }
        ITF_INLINE bbool                isBox() const { return m_type == TYPE_BOX; }
        ITF_INLINE bbool                isCircle() const { return m_type == TYPE_CIRCLE; }
        ITF_INLINE bbool                isPoint() const { return m_point; }
        ITF_INLINE bbool                isInverted() const { return m_inverted; }

        static void applySpeedLimitation( f32 _force2Speed, f32 _speedLimit, f32 _maxOppScale,
                                          const Vec2d& _currentSpeed, Vec2d& _force );

    private:

        
        BoxData         m_box;
        CircleData      m_circle;
        PolylineData    m_polyline;

        Type            m_type;                 // If it's a box or a circle
        Vec2d           m_force;                // The force modifier
        f32             m_gradientPercent;      // Percentage of the size to interpolate the gravity from 100% to 0%
        bbool           m_point;                // If the force will go from pos to the center of the shape or in the direction set in the force
        bbool           m_inverted;             // If the result force will be inverted
        Vec2d           m_offset;               // A position offset
        Angle           m_rotation;             // A rotation offset

        f32             m_centerForce;          // Force to pull us toward the center
        f32             m_centerForceMaxSpeed;  // Max speed to apply max force towards the center
        f32             m_centerForceSpeed2Force;// Multiplier to transform the speed to a force

        f32             m_speedMultiplierX;     // Multiplier to speed on X
        f32             m_speedMultiplierY;     // Multiplier to speed on Y
    };

    class PhysForceModifier_Instance
    {
    public:

        struct PolylineData
        {
            DECLARE_SERIALIZE()

            PolylineData() : m_dir(Vec2d::Right), m_center(Vec2d::Zero), m_start(0.f), m_end(0.f) {}

            PhysShapePolygon        m_polygon;
            AABB                    m_aabb;
            Vec2d                   m_dir;
            Vec2d                   m_center;
            f32                     m_start;
            f32                     m_end;
        };

        PhysForceModifier_Instance()
            : m_template(NULL)
            , m_scale(Vec2d::One)
            , m_gradientPercent(0.f)
        {}
        ~PhysForceModifier_Instance() {}
        
#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                drawDebug( const Vec2d& _pos, f32 _angle, f32 z ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE
        bbool               checkForce( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const;
        ITF_INLINE PolylineData& getPolyline() { ITF_ASSERT(m_template->getType() == PhysForceModifier::TYPE_POLYLINE); return m_polylineData; }
        ITF_INLINE const PolylineData& getPolyline() const { ITF_ASSERT(m_template->getType() == PhysForceModifier::TYPE_POLYLINE); return m_polylineData; }
        void                setScale( const Vec2d& _scale ) { m_scale = _scale; }

        void                setTemplate( const PhysForceModifier* _template )  { m_template = _template; m_gradientPercent = _template->getGradientPercent(); }
        const PhysForceModifier* getTemplate() const { return m_template; }

        ITF_INLINE f32      getGradientPercent() const { return m_gradientPercent; }
        ITF_INLINE void     setGradientPercent( const f32 _value ) { m_gradientPercent = _value; }

    private:

        bbool               checkForceCircle( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, Vec2d& _force, Vec2d& _speedMultiplier ) const;
        bbool               checkForceBox( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const;
        bbool               checkForcePolyline( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const;

        const PhysForceModifier*    m_template;
        PolylineData                m_polylineData;
        Vec2d                       m_scale;
        f32                         m_gradientPercent;
    };

    struct ActorForceModifier
    {
        ObjectRef                           m_actor;
        class PhysIsland*                   m_island;
        const PhysForceModifier_Instance*   m_modifier;
    };

    typedef ITF_LIST <ActorForceModifier*> ActorForceModifiersList;

    struct PhysActorForce
    {
        ObjectRef                   m_actor;
        Vec2d                       m_force;
        Vec2d                       m_speedMultiplier;
    };

    typedef FixedArray <PhysActorForce,10>  PhysActorForceArray;
    typedef ITF_VECTOR <PhysForceModifier> ForceModifierArray;
    typedef ITF_VECTOR <PhysForceModifier_Instance> ForceModifierInstanceArray;
}

#endif // _ITF_PHYSFORCEMODIFIER_H_

