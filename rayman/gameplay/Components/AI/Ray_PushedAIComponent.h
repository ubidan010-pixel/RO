#ifndef _ITF_RAY_PUSHEDAICOMPONENT_H_
#define _ITF_RAY_PUSHEDAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class StickToPolylinePhysComponent;
    class FXControllerComponent;
    class SoundComponent;

    class Ray_PushedAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PushedAIComponent, Ray_AIComponent,747720257)
        DECLARE_SERIALIZE();

        struct ActorSticked
        {
            ActorSticked()
            : m_frame(0)
            {
            }

            ActorRef m_actor;
            u32      m_frame;
        };


    public:

        Ray_PushedAIComponent();
        virtual ~Ray_PushedAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent( Event * _event);

#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_PushedAIComponent_Template*  getTemplate() const;

        void            addForce( Vec2d _force ); 

        void            processHit( PunchStim* _hit );
        void            processStickOnPolyline( EventStickOnPolyline* _stickEvent );
        void            processPush( EventPushForce* _push );
        void            processBlockedByPolyline( EventBlockedByPolyline * _evtBlockedOnPoly );

        bbool           canReceiveHit( PunchStim* _hit );

    private:

        StickToPolylinePhysComponent*   m_physComponent;
        FXControllerComponent*          m_fxController;
        SoundComponent*                 m_soundComponent;

        Vec2d           m_forcesThisFrame;
        bbool           m_isBlocked;
        Vec2d           m_lastSpeed;
        u32             m_handleSoundSlide;

        ITF_VECTOR<ActorSticked> m_actorSticked;
        

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_PushedAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PushedAIComponent_Template, Ray_AIComponent_Template,535185699);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PushedAIComponent);

    public:

        Ray_PushedAIComponent_Template();
        ~Ray_PushedAIComponent_Template();

        ITF_INLINE u32 getFaction() const                  { return m_faction; }

        ITF_INLINE f32 getForceMultiplier() const          { return m_forceMultiplier; }
        ITF_INLINE f32 getMaxSpeed() const                 { return m_maxSpeed; }
        ITF_INLINE f32 getBounceMultiplier() const         { return m_bounceMultiplier; }
        ITF_INLINE f32 getWeakHitForce() const             { return m_weakHitForce; }
        ITF_INLINE f32 getStrongHitForce() const           { return m_strongHitForce; }
        ITF_INLINE f32 getLandForceMultiplier() const      { return m_landForceMultiplier; }
        ITF_INLINE f32 getLandXForceMultiplier() const     { return m_landXForceMultiplier; }

    private:

        u32             m_faction;

        // General
        f32             m_maxSpeed;
        f32             m_bounceMultiplier;
        f32             m_forceMultiplier;

        // Hit
        f32             m_weakHitForce;
        f32             m_strongHitForce;
        
        // Ride
        f32             m_landForceMultiplier;
        f32             m_landXForceMultiplier;

    };

    const Ray_PushedAIComponent_Template*  Ray_PushedAIComponent::getTemplate() const {return static_cast<const Ray_PushedAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_PUSHEDAICOMPONENT_H_