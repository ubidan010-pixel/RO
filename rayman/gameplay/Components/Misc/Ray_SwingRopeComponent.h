#ifndef _ITF_RAY_SWINGROPECOMPONENT_H_
#define _ITF_RAY_SWINGROPECOMPONENT_H_

#ifndef _ITF_SOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/SoftPlatformComponent.h"
#endif //_ITF_SOFTPLATFORMCOMPONENT_H_

namespace ITF
{
    class Ray_SwingRopeComponent : public SoftPlatformComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(Ray_SwingRopeComponent,SoftPlatformComponent,1789060767)
        DECLARE_SERIALIZE()

        Ray_SwingRopeComponent();
        virtual ~Ray_SwingRopeComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

    private:

        ITF_INLINE const class Ray_SwingRopeComponent_Template*  getTemplate() const;

        virtual void                createSoftPlatform();

        void                        postChangeProperties();

        Angle                       m_initialAngle;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_SwingRopeComponent_Template : public SoftPlatformComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwingRopeComponent_Template,SoftPlatformComponent_Template,3572690544);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SwingRopeComponent);

    public:

        Ray_SwingRopeComponent_Template();
        ~Ray_SwingRopeComponent_Template() {}

        const Angle&                    getMaxSwingAngle() const { return m_maxSwingAngle; }
        f32                             getStiffGravityMultiplier() const { return m_stiffGravityMultiplier; }
        f32                             getStiffImpulseMultiplier() const { return m_stiffImpulseMultiplier; }
        f32                             getStiffSwingSpeedFriction() const { return m_stiffSwingSpeedFriction; }
        f32                             getNonStiffSwingSpeedFriction() const { return m_nonStiffSwingSpeedFriction; }
        f32                             getLandDragMultiplier() const { return m_landDragMultiplier; }
        f32                             getLandDragDuration() const { return m_landDragDuration; }
        bbool                           getCanUseSmall() const { return m_canUseSmall; }
        bbool                           getCanUseNormal() const { return m_canUseNormal; }
        bbool                           getRepositionWithBones() const { return m_repositionWithBones; }

    private:

        Angle                           m_maxSwingAngle;
        f32                             m_stiffGravityMultiplier;
        f32                             m_stiffImpulseMultiplier;
        f32                             m_stiffSwingSpeedFriction;
        f32                             m_nonStiffSwingSpeedFriction;
        f32                             m_landDragMultiplier;
        f32                             m_landDragDuration;
        bbool                           m_canUseSmall;
        bbool                           m_canUseNormal;
        bbool                           m_repositionWithBones;
    };



    ITF_INLINE const Ray_SwingRopeComponent_Template*  Ray_SwingRopeComponent::getTemplate() const {return static_cast<const Ray_SwingRopeComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_SWINGROPECOMPONENT_H_
