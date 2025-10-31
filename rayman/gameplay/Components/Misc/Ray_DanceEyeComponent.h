#ifndef _ITF_RAY_DANCEEYECOMPONENT_H_
#define _ITF_RAY_DANCEEYECOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class AnimatedComponent;
    class FXControllerComponent;

    class Ray_DanceEyeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceEyeComponent, ActorComponent,1624614149)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_DanceEyeComponent();
        virtual ~Ray_DanceEyeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onBecomeActive();
        virtual void        onResourceReady();


                bbool       isOccupiedByNPC() const;
                bbool       isOccupiedByPlayer() const;
                bbool       isOccupied() const;
                bbool       isUnlock() const;
                bbool       isReadyForDoorOpen() const;
                ActorRef    getDoor() const { return m_parent; }
                ObjectRef   getStone() const { return m_refStone; }
                void        setStone( ObjectRef _stone ) { m_refStone = _stone; }

                void        updateStone( ObjectRef _stone, Vec3d _stonePos, bbool _occupiedByNPC, bbool _occupiedByPlayer, bbool _unlocked );

    private:
                void        changeState( int _newState );
                bbool       playAnim( const StringID& _anim );
                void        getAngleAndDist( Vec3d _target );
                void        updateGodrayParticles(f32 _dt);

                void        moveFX(Vec3d _pos);
                void        startFX();
                void        stopFX();

    private:
        ITF_INLINE const class Ray_DanceEyeComponent_Template* getTemplate() const;

        AnimatedComponent* m_animComponent;
        FXControllerComponent* m_fxControllerComponent;

        ActorRef            m_parent;
        ActorRef            m_godray;
        u32                 m_state;
        
        bbool               m_alreadyRegistred;

        bbool               m_occupiedByNPC;
        bbool               m_occupiedByNPC_Temp;

        bbool               m_occupiedByPlayer;
        bbool               m_occupiedByPlayer_Temp;
        
        bbool               m_unlock;        
        bbool               m_unlock_Temp;        

        Vec3d               m_posStone;
        Vec3d               m_posStone_Temp;
        ObjectRef           m_refStone;

        bbool               m_doorIsReadyForOpen;
        bbool               m_isReadyForDoorOpen;

        f32                 m_angle;
        f32                 m_dist;

        // Particles
        f32                 m_curDistPartciles;
        u32                 m_handleFX;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_DanceEyeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceEyeComponent_Template, TemplateActorComponent,1445965177);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DanceEyeComponent);

    public:

        Ray_DanceEyeComponent_Template();
        virtual ~Ray_DanceEyeComponent_Template();

        ITF_INLINE StringID getLockAnim() const         { return m_lockAnim; }
        ITF_INLINE StringID getOccupiedAnim() const     { return m_occupiedAnim; }
        ITF_INLINE StringID getUnlockAnim() const       { return m_unlockAnim; }

        ITF_INLINE StringID getLock_to_occupied_Anim() const       { return m_lock_to_occupied_Anim; }
        ITF_INLINE StringID getOccupied_to_unlock_Anim() const     { return m_occupied_to_unlock_Anim; }
        ITF_INLINE StringID getUnlock_to_occupied_Anim() const     { return m_unlock_to_occupied_Anim; }
        ITF_INLINE StringID getOccupied_to_lock_Anim() const       { return m_occupied_to_lock_Anim; }
        ITF_INLINE StringID getUnlock_to_off_Anim() const          { return m_unlock_to_off_Anim; }

        ITF_INLINE f32      getSpeedMoveParticles() const { return m_speedMoveParticles; }


    private:

        // Main animations
        StringID    m_lockAnim;
        StringID    m_occupiedAnim;
        StringID    m_unlockAnim;

        // Transition animations
        StringID    m_lock_to_occupied_Anim;
        StringID    m_occupied_to_unlock_Anim;
        StringID    m_unlock_to_occupied_Anim;
        StringID    m_occupied_to_lock_Anim;
        StringID    m_unlock_to_off_Anim;

        // Misc
        f32         m_speedMoveParticles;

    };

    //*****************************************************************************

    ITF_INLINE const Ray_DanceEyeComponent_Template* Ray_DanceEyeComponent::getTemplate() const
    {
        return static_cast<const Ray_DanceEyeComponent_Template*>(m_template);
    }

}; // end namespace

#endif //_ITF_RAY_DANCEEYECOMPONENT_H_