#ifndef _ITF_RAY_ALTRANQUILOAICOMPONENT_H_
#define _ITF_RAY_ALTRANQUILOAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_


namespace ITF
{
    class Ray_GeyserPlatformAIComponent;

    class Ray_alTranquiloAiComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_alTranquiloAiComponent, AIComponent,3434288248)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_alTranquiloAiComponent();
        virtual ~Ray_alTranquiloAiComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        void onBubbleReleased( ActorRef _bubble );

    private:

        ITF_INLINE const class Ray_alTranquiloAiComponent_Template * getTemplate() const;

        void startIdle();
        void startSquashed();
        void startResquash();
        void processStick( EventStickOnPolyline * stickEvent );
        void processPunch( PunchStim * punch );

        void updateIdle();
        void updateSquashed(f32 _dt);
        void sendBounce( ActorRef _otherActor );
        Vec2d getBouncePos() const;

        enum State
        {
            State_Idle,
            State_Squashed,
        };
        State m_state;
    
        SafeArray<ActorRef> m_stickedActors;
        f32 m_staySquashedTimer;
        bbool m_fastScale;



        enum Mode
        {
            Mode_None,
            Mode_Bubble,
            Mode_Geyser,
        };
        Mode m_mode;

        void onSquash();
        void onResquash();
        void onRelease();


        // Mode_Bubble

        void onSquash_bubble();
        void onResquash_bubble();
        void onRelease_bubble();
        void update_bubble( const f32 _dt );
        void sendBounce_bubble( Actor* _otherActor ) const;
        Vec2d getBouncePos_bubble() const;
        void spawnBubble();

        SpawneeGenerator m_bubbleSpawner;
        u32 m_spawnBoneIndex;
        ActorRef m_currentBubble;
        f32 m_respawnTimer;
        f32 m_bubbleLifetime;   // instance-serialized


        // Mode_Geyser

        void onSquash_geyser();
        void onRelease_geyser();
        void sendBounce_geyser( Actor* _otherActor ) const;
        Vec2d getBouncePos_geyser() const;

        Ray_GeyserPlatformAIComponent* m_geyser;
    };

    //-------------------------------------------------------------------------------------
    class Ray_alTranquiloAiComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_alTranquiloAiComponent_Template, AIComponent_Template,3178361393);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_alTranquiloAiComponent);

    public:

        Ray_alTranquiloAiComponent_Template();
        ~Ray_alTranquiloAiComponent_Template() {}

        ITF_INLINE StringID getIdleAnim() const { return m_idleAnim; }
        ITF_INLINE StringID getSquashedAnim() const { return m_squashedAnim; }
        ITF_INLINE StringID getStaySquashedAnim() const { return m_staySquashedAnim; }
        ITF_INLINE f32 getStaySquashedTime() const { return m_staySquashedTime; }
        ITF_INLINE bbool getAutoBounce() const { return m_autoBounce; }
        ITF_INLINE const Vec2d& getBouncePos() const { return m_bouncePos; }
        ITF_INLINE bbool getMini() const { return m_isMini; }
        ITF_INLINE const Path& getBubblePath() const { return m_bubblePath; }
        ITF_INLINE StringID getSpawnBone() const { return m_spawnBone; }
        ITF_INLINE f32 getRespawnTime() const { return m_respawnTime; }
        ITF_INLINE ITF::Angle getBounceAngle() const { return m_bounceAngle; }
        ITF_INLINE ITF::BounceType getBounceType() const { return m_bounceType; }

    private:
        StringID m_idleAnim;
        StringID m_squashedAnim;
        StringID m_staySquashedAnim;
        f32 m_staySquashedTime;
        Vec2d m_bouncePos;
        bbool m_autoBounce;
        bbool m_isMini;
        Angle m_bounceAngle;
        BounceType m_bounceType;
        // specific to Mode_Bubble
        Path m_bubblePath;
        StringID m_spawnBone;
        f32 m_respawnTime;   // if still squashed, autospawn another bubble after a while (0 = disable auto-spawn)
    };

    ITF_INLINE const Ray_alTranquiloAiComponent_Template* Ray_alTranquiloAiComponent::getTemplate() const
    {
        return static_cast<const Ray_alTranquiloAiComponent_Template*>(m_template);
    }

};

#endif //_ITF_RAY_ALTRANQUILOAICOMPONENT_H_