#ifndef _ITF_RAY_BULLETAICOMPONENT_H_
#define _ITF_RAY_BULLETAICOMPONENT_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class PolylineComponent;

    class Ray_BulletAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BulletAIComponent, Ray_AIComponent,2316478316)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_BulletAIComponent();
        virtual ~Ray_BulletAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void        Update(f32 _deltaTime);
        virtual void        onEvent( Event* _event);
        virtual void        updateAnimInput();
        virtual void        onCheckpointLoaded();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_BulletAIComponent_Template * getTemplate() const;

        void                reset();

        void                setDirection( const Vec2d& _direction );

        void                checkNoseCollisions();
        void                checkCollisions(const class PhysShape* _shape);
        virtual void        processHit( HitStim* _hit );
        void                sendDeathStim(Actor* target);
        void                die(bbool _useStim = btrue);
        void                checkOutOfScreen( f32 _dt );
        void                directDestroy();

        virtual void        updateMovement( f32 _dt );

        AIBehavior*         m_launchBehavior;
        AIBehavior*         m_behavior;
        AIBehavior*         m_deathBehavior;
        AIBehavior*         m_deathNoStimBehavior;

        PhysComponent*      m_physComponent;
        PolylineComponent*  m_polylineComponent;

        ActorRef            m_originalSender;
        Vec2d               m_direction;

        f32                 m_timer;
        f32                 m_noColltimer;
        bbool               m_isEnvironmentDeath;
        Vec2d               m_stimPrevPos;

        bbool               m_hasEnteredScreen;
        f32                 m_outOfScreenSafeTimer;
        
        // nose
        StringID                        m_nosePolyline;
        PhysShapePolygon                m_noseShape;
        PhysShapePolygon::PointsList    m_noseShapePoints;
        PhysShapePolygon::PointsList    m_prevNosePoints;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_BulletAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BulletAIComponent_Template, Ray_AIComponent_Template,419291702);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BulletAIComponent);

    public:

        Ray_BulletAIComponent_Template();
        ~Ray_BulletAIComponent_Template();

        ITF_INLINE f32      getSpeed()const                  {return m_speed;}
        ITF_INLINE f32      getLifeTime()const               {return m_lifeTime;}
        ITF_INLINE bbool    getCollideWithEnvironment()const {return m_collideWithEnvironment;}
        ITF_INLINE bbool    getCollideWithPhantoms() const   {return m_collideWithPhantoms; }
        ITF_INLINE f32      getActivateNoCollDuration()const {return m_activateNoCollDuration;}
        ITF_INLINE bbool    getSendDeathStim() const         {return m_sendDeathStim; }
        ITF_INLINE bbool    getCollideWithSolidEdges() const { return m_collideWithSolidEdges; }
        ITF_INLINE bbool    getDisablePolylineOnDeath() const { return m_disablePolylineOnDeath; }

        ITF_INLINE class TemplateAIBehavior* getLaunchBehavior() const { return m_launchBehavior; }
        ITF_INLINE class TemplateAIBehavior* getBehavior() const { return m_behavior; }
        ITF_INLINE class TemplateAIBehavior* getDeathBehavior() const { return m_deathBehavior; }
        ITF_INLINE class TemplateAIBehavior* getDeathNoStimBehavior() const { return m_deathNoStimBehavior; }

        ITF_INLINE const bbool isCameraRelative() const { return m_isCameraRelative; }
        ITF_INLINE const bbool getDestroyOnExitScreen() const { return m_destroyOnExitScreen; }
        ITF_INLINE const bbool getDestroyOnBecomeInactive() const { return m_destroyOnBecomeInactive; }

    private:
        class TemplateAIBehavior*               m_launchBehavior;
        class TemplateAIBehavior*               m_behavior;
        class TemplateAIBehavior*               m_deathBehavior;
        class TemplateAIBehavior*               m_deathNoStimBehavior;
        
        f32                 m_speed;
        f32                 m_lifeTime;
        bbool               m_collideWithEnvironment;
        bbool               m_collideWithPhantoms;
        f32                 m_activateNoCollDuration;
        bbool               m_sendDeathStim;
        bbool               m_isCameraRelative;
        bbool               m_collideWithSolidEdges;
        bbool               m_disablePolylineOnDeath;
        bbool               m_destroyOnExitScreen;
        bbool               m_destroyOnBecomeInactive;
    };

    ITF_INLINE const Ray_BulletAIComponent_Template * Ray_BulletAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BulletAIComponent_Template *>(m_template);
    }

};

#endif //_ITF_RAY_BULLETAICOMPONENT_H_
