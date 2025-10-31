#ifndef _ITF_RAY_SHOOTERSTIMCOMPONENT_H_
#define _ITF_RAY_SHOOTERSTIMCOMPONENT_H_

#ifndef _ITF_RAY_BASICBULLET_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BasicBullet.h"
#endif //_ITF_RAY_BASICBULLET_H_

namespace ITF
{
    class Ray_ShooterStimComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterStimComponent, ActorComponent,1054562797)

    public:
        DECLARE_SERIALIZE();

        Ray_ShooterStimComponent();
        virtual ~Ray_ShooterStimComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update(f32 _dt);

    private:
        ITF_INLINE const class Ray_ShooterStimComponent_Template * getTemplate() const;

		void				stop();
		Ray_BasicBullet		            m_bulletAbility;
        f32                             m_fxTimerBeforeDestroy;
        bbool                           m_bulletObsolete;
        class FXControllerComponent *   m_fxcontroller;
        bbool                           m_stopped;

    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterStimComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterStimComponent_Template,TemplateActorComponent,3632123545);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterStimComponent);

    public:

        Ray_ShooterStimComponent_Template();
        ~Ray_ShooterStimComponent_Template();

        ITF_INLINE const Ray_BasicBullet_Template * getBasicBulletTemplate() const { return &m_basicBullet; }
        ITF_INLINE const f32                        getFxDelayBeforeDestroy() const { return m_fxDelayBeforeDestroy; }
        
    private:
		Ray_BasicBullet_Template	m_basicBullet;
        f32                         m_fxDelayBeforeDestroy;

    };

    ITF_INLINE const class Ray_ShooterStimComponent_Template * Ray_ShooterStimComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterStimComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_SHOOTERSTIMCOMPONENT_H_