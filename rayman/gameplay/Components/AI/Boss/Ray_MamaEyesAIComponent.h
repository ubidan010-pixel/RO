#ifndef _ITF_RAY_MAMAEYESAICOMPONENT_H_
#define _ITF_RAY_MAMAEYESAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    class AnimGameplayEvent;
    class Ray_MamaEyesAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MamaEyesAIComponent, Ray_AIComponent,1789470946)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_MamaEyesAIComponent();
        virtual ~Ray_MamaEyesAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);
        virtual void        Update( f32 _dt );

    private:

        void                setInitSpeed();
        void                move(f32 _dt);
        void                checkDeath();

        Vec3d               m_speed;
        f32                 m_rotation;
        
        ITF_INLINE const class Ray_MamaEyesAIComponent_Template* getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class Ray_MamaEyesAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MamaEyesAIComponent_Template, Ray_AIComponent_Template,2499315905)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MamaEyesAIComponent)

    public:

        Ray_MamaEyesAIComponent_Template();
        ~Ray_MamaEyesAIComponent_Template() {}

        ITF_INLINE const Vec3d&  getSpeed() const      { return m_initSpeed;         }
        ITF_INLINE f32           getRotation() const   { return m_rotation;          }
        ITF_INLINE f32           getGravity() const    { return m_gravity;           }
        ITF_INLINE f32           getScaleMin() const   { return m_scaleMin;          }
        ITF_INLINE f32           getScaleMax() const   { return m_scaleMax;          }

    private:

        Vec3d       m_initSpeed;
        f32         m_rotation;
        f32         m_gravity;
        f32         m_scaleMin;
        f32         m_scaleMax;
 };

    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_MamaEyesAIComponent_Template* Ray_MamaEyesAIComponent::getTemplate() const
    {
        return static_cast<const Ray_MamaEyesAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_MAMAEYESAICOMPONENT_H_