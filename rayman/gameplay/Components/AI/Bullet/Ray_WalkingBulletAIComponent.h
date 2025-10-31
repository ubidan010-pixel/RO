#ifndef _ITF_RAY_WALKINGBULLETAICOMPONENT_H_
#define _ITF_RAY_WALKINGBULLETAICOMPONENT_H_

#ifndef _ITF_RAY_BULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BulletAIComponent.h"
#endif //_ITF_RAY_BULLETAICOMPONENT_H_

namespace ITF
{
    class Ray_WalkingBulletAIComponent : public Ray_BulletAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WalkingBulletAIComponent, Ray_BulletAIComponent,580133949)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_WalkingBulletAIComponent();
        virtual ~Ray_WalkingBulletAIComponent();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    Update( f32 _dt );
        virtual void    updateAnimInput();

    private:
        ITF_INLINE const class Ray_WalkingBulletAIComponent_Template* getTemplate() const;

        virtual void    updateMovement( f32 _dt );

        StickToPolylinePhysComponent*   m_physComponent;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_WalkingBulletAIComponent_Template : public Ray_BulletAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WalkingBulletAIComponent_Template, Ray_BulletAIComponent_Template,3144728615);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WalkingBulletAIComponent);

    public:

        Ray_WalkingBulletAIComponent_Template();
        ~Ray_WalkingBulletAIComponent_Template() {}

        ITF_INLINE f32 getStickForce() const {return m_stickForce;}

    private:


        f32     m_stickForce;       // the force that keeps us sticking on the polyline
    };

    const Ray_WalkingBulletAIComponent_Template* Ray_WalkingBulletAIComponent::getTemplate() const
    {
        return static_cast<const Ray_WalkingBulletAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_WALKINGBULLETAICOMPONENT_H_