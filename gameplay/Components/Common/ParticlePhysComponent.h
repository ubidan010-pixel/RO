
#ifndef _ITF_PARTICLEPHYSCOMPONENT_H_
#define _ITF_PARTICLEPHYSCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_


namespace ITF
{

    class ParticlePhysComponent : public PhysComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ParticlePhysComponent,PhysComponent,2772003471);

    public:
        DECLARE_SERIALIZE()

        ParticlePhysComponent();
        ~ParticlePhysComponent();

        virtual bbool           needsUpdate() const { return btrue; }
        virtual bbool           needsDraw() const { return bfalse; }
        virtual void            Update( f32 _deltaTime );
        virtual void            onForceMove( );
        
    private:

        ITF_INLINE const class ParticlePhysComponent_Template*  getTemplate() const;

        void    calculateSpeedFromForces( f32 _deltaTime, const Vec2d& _pos );
        void    updateCurrentGravity( const Vec2d& _pos );
        f32     calculateAngularSpeedFromLinearSpeed() const;
        void    move( f32 _delta, Vec3d& _pos, f32 _angle );
        void    rotate( f32 _deltaTime, const Vec2d& _pos, f32& _angle );
        void    applyPosAndAngle( const Vec3d& _pos, f32 _angle );
    };

    //---------------------------------------------------------------------------------------------------

    class ParticlePhysComponent_Template : public PhysComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ParticlePhysComponent_Template,PhysComponent_Template,3309029528);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ParticlePhysComponent);

    public:

        ParticlePhysComponent_Template();
        ~ParticlePhysComponent_Template() {}


        ITF_INLINE f32  getFriction()                         const   {return m_friction;}
        ITF_INLINE f32  getAngularSpeedMinLinear()            const   {return m_angularSpeedMinLinear;}
        ITF_INLINE f32  getAngularSpeedMaxLinear()            const   {return m_angularSpeedMaxLinear;}
        ITF_INLINE const Angle&   getAngularSpeedMinAngular() const   {return m_angularSpeedMinAngular;}
        ITF_INLINE const Angle&   getAngularSpeedMaxAngular() const   {return m_angularSpeedMaxAngular;}

    private:

        f32     m_friction;
        f32     m_angularSpeedMinLinear;
        f32     m_angularSpeedMaxLinear;
        Angle   m_angularSpeedMinAngular;
        Angle   m_angularSpeedMaxAngular;
    };


    ITF_INLINE const ParticlePhysComponent_Template*  ParticlePhysComponent::getTemplate() const {return static_cast<const ParticlePhysComponent_Template*>(m_template);}


}

#endif // _ITF_PARTICLEPHYSCOMPONENT_H_
