
#ifndef _ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_
#define _ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{
    class BounceOnPolylinePhysComponent: public PhysComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(BounceOnPolylinePhysComponent, PhysComponent,2164702966);
        DECLARE_SERIALIZE()

    public:
        BounceOnPolylinePhysComponent();
        virtual ~BounceOnPolylinePhysComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw()   const { return bfalse; }
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onBecomeActive();
        virtual void    onBecomeInactive();
        virtual void    Update( f32 _deltaTime );
        virtual void    onEvent( Event * _event );

    protected:


        ITF_INLINE const class BounceOnPolylinePhysComponent_Template*  getTemplate() const;

        void            updatePhantom();
        void            storeHitInfo( class HitStim* _hit );
        void            computeForces( f32 _deltaTime );
        void            applyForces( f32 _deltaTime );
        void            computeBouncingSpeed( const Vec2d& _initialSpeed, const Vec2d& _surfaceNormal );
        Vec2d           computeSurfaceNormal( ObjectRef _polyline, u32 _edgeIndex ) const;
        Vec2d           computeBoostVector(const Vec2d& _speed, const Vec2d& _surfaceNormal) const;
        void            checkContacts();
        void            processContact(const SCollidableContact& _contact);
        void            processNewUser( class EventStickOnPolyline* _stickEvent );
        void            clear();
        void            initPhantom();
        bbool           isAValidContact(const SCollidableContact& _contact, const Vec2d& _speed, f32& _normalDotSpeed) const;
        void            alignWithGravity( f32 _deltaTime );

        typedef SafeArray<ObjectRef> ActorsRefArray;

        struct ValidContact
        {
            const SCollidableContact*   m_contactPtr;
            f32                         m_normalDotSpeed;
        };
        

        // Interns
        ActorsRefArray      m_passengers;
        ECollisionFilter    m_collisionFilter;
        class PhysPhantom*  m_phantom;
        PhysShapeCircle     m_shape;
        Vec2d               m_bouncingSpeed;
        Vec2d               m_hitDirection;
        Vec2d               m_prevPos;
        f32                 m_totalWeight;
        u32                 m_hitLevel;
        bbool               m_objectsRegistered;
    };

    //---------------------------------------------------------------------------------------------------

    class BounceOnPolylinePhysComponent_Template : public PhysComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BounceOnPolylinePhysComponent_Template,PhysComponent_Template,419162262);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(BounceOnPolylinePhysComponent);

    public:

        BounceOnPolylinePhysComponent_Template();
        ~BounceOnPolylinePhysComponent_Template() {}

        f32                 getBouncingFactor() const { return m_bouncingFactor; }
        f32                 getBouncingMinYSpeed() const { return m_bouncingMinYSpeed; }
        f32                 getPassengerFactor() const { return m_passengerFactor; }
        f32                 getDefaultWeight() const { return m_defaultWeight; }
        f32                 getHitMultiplier() const { return m_hitMultiplier; }
        f32                 getAngularSmoothFactor() const { return m_angularSmoothFactor; }

    private:

        f32                 m_bouncingFactor;
        f32                 m_bouncingMinYSpeed;
        f32                 m_passengerFactor;
        f32                 m_defaultWeight;
        f32                 m_hitMultiplier;
        f32                 m_angularSmoothFactor;
    };



    const BounceOnPolylinePhysComponent_Template*  BounceOnPolylinePhysComponent::getTemplate() const {return static_cast<const BounceOnPolylinePhysComponent_Template*>(m_template);}
}

#endif // _ITF_BOUNCEONPOLYLINEPHYSCOMPONENT_
