
#ifndef _ITF_PHYSCOMPONENT_H_
#define _ITF_PHYSCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class PhysComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(PhysComponent,ActorComponent,1286338096);

public:
    DECLARE_SERIALIZE()

    PhysComponent();
    virtual ~PhysComponent();

    virtual bbool               needsUpdate() const { return bfalse; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }
    virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                onCheckpointLoaded();
    virtual void                onEvent( Event * _event);
#ifdef ITF_SUPPORT_EDITOR
    virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    void                        setRadiusMultiplier( f32 _mult ) { m_radiusMultiplier = _mult; }
    f32                         getRadius() const;

    virtual   void              setDisabled( bbool _val) { m_disabled = _val; }
    ITF_INLINE   bbool          getDisabled() const { return m_disabled; }

    virtual void                setSpeed( const Vec2d& _speed ) { m_speed = _speed; }
    ITF_INLINE   const Vec2d&   getSpeed() const { return m_speed; }

    ITF_INLINE   void           addForce( const Vec2d& _force ) { m_force += _force; }
    ITF_INLINE   const Vec2d&   getForce() const { return m_force; }
    ITF_INLINE   void           resetForces() { m_force = Vec2d::Zero; }

                 f32            getWeight() const;
    ITF_INLINE   void           setWeightMultiplier(f32 _mult) { m_weightMultiplier = _mult; }
    ITF_INLINE   void           setGravityMultiplier( f32 _mult ) { m_gravityMultiplier = _mult; }
    ITF_INLINE   f32            getGravityMultiplier() const { return m_gravityMultiplier; }
    ITF_INLINE   f32            getInitialGravityMultiplier() const;

    ITF_INLINE   void           setWindMultiplier( f32 _mult ) { m_windMultiplier = _mult; }
    ITF_INLINE   f32            getWindMultiplier() const { return m_windMultiplier; }

    ITF_INLINE   void           setWaterMultiplier( f32 _mult ) { m_waterMultiplier = _mult; }
    ITF_INLINE   f32            getWaterMultiplier() const { return m_waterMultiplier; }

    ITF_INLINE   void           setApplyWindSpeedLimiter( bbool _val ) { m_applyWindSpeedLimiter = _val; }

    const Vec2d&                getCurrentGravity() const { return m_currentGravity; }
    const Vec2d&                getCurrentGravityDir() const { return m_currentGravityDir; }

    virtual void                onForceMove();

protected:

    ITF_INLINE const class      PhysComponent_Template*  getTemplate() const;

    virtual void                processQueryPhysics( class EventQueryPhysicsData* _query ) const;

    //////


    f32                         m_gravityMultiplier;        // Gravity Multiplier for this body
    f32                         m_windMultiplier;           // Wind Multiplier for this body
    f32                         m_waterMultiplier;           // Wind Multiplier for this body
    f32                         m_weightMultiplier;
    f32                         m_radiusMultiplier;

    Vec2d                       m_speed;                    // Current Speed
    Vec2d                       m_force;                    // The force we add to move it
    Vec2d                       m_totalForcesThisFrame;     // The total forces this frame
    Vec2d                       m_currentGravity;           // The current gravity
    Vec2d                       m_currentGravityDir;        // The current gravity direction

    bbool                       m_applyWindSpeedLimiter;    // If the speed limiter needs to be applied on wind

    bbool                       m_disabled;                 // If the physics are disabled
};

//---------------------------------------------------------------------------------------------------

class PhysComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(PhysComponent_Template,TemplateActorComponent,2661944379);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(PhysComponent);

public:

    PhysComponent_Template();
    ~PhysComponent_Template() {}

    f32                         getRadius() const { return m_radius; }
    f32                         getWeight() const { return m_weight; }
    f32                         getInitialGravityMultiplier() const { return m_initialGravityMultiplier; }
    f32                         getInitialWindMultiplier() const { return m_initialWindMultiplier; }
    f32                         getInitialWaterMultiplier() const { return m_initialWaterMultiplier; }
    f32                         getForce2Speed() const { return m_force2speed; }
    f32                         getWindSpeedLimiter() const { return m_windSpeedLimiter; }
    f32                         getWindMaxScaleOpposite() const { return m_windMaxScaleOpposite; }
    ITF_INLINE f32              getFanForceMultiplier() const { return m_fanForceMultiplier; }

private:

    f32                         m_radius;                   // Radius of the object
    f32                         m_weight;                   // Weight of the object
    f32                         m_initialGravityMultiplier;
    f32                         m_initialWindMultiplier;
    f32                         m_initialWaterMultiplier;
    f32                         m_force2speed;              // Multiplier to transform from force to speed
    f32                         m_windSpeedLimiter;         // Speed limit when applying wind
    f32                         m_windMaxScaleOpposite;     // Maximum scale when our speed is opposite to the wind
    f32                         m_fanForceMultiplier;       // Multiplier applied on received fan forces
};



const PhysComponent_Template*  PhysComponent::getTemplate() const {return static_cast<const PhysComponent_Template*>(m_template);}

f32            PhysComponent::getInitialGravityMultiplier() const { return getTemplate()->getInitialGravityMultiplier(); }


}

#endif // _ITF_PHYSCOMPONENT_H_

