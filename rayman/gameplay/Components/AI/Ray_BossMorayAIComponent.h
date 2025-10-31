#ifndef _ITF_RAY_BOSSMORAYAICOMPONENT_H_
#define _ITF_RAY_BOSSMORAYAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class TweenComponent;

    class Ray_BossMorayAIComponent : public Ray_SnakeAIComponent
    {
        friend class Ray_BossMorayBodyPart;
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMorayAIComponent, Ray_SnakeAIComponent,2045799600)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_BossMorayAIComponent();
        virtual ~Ray_BossMorayAIComponent();

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

        ITF_INLINE bbool    getIsDead() const { return m_state != State_Node && m_state != State_NodeHit; }

        const StringID&     getCurrentBubo();
        PolyLine*           getMissileRegion( u32 _regionIndex );

    private:
        ITF_INLINE const class Ray_BossMorayAIComponent_Template* getTemplate() const;

        class Sequence
        {
            DECLARE_SERIALIZE();
        public:
            StringID m_tweenSet;
            StringID m_buboId;
            ObjectPath m_objectToTriggerOnEnter;
            ObjectPath m_objectToTriggerOnExit;
            ITF_VECTOR<ObjectPath> m_missileRegions;
        };
        typedef ITF_VECTOR<Sequence> SequenceList;

        TweenComponent* m_tweenComponent;

        bbool isRealBoss() const { return m_tweenComponent != NULL; }   // the pursuit moray use the same AI
        void proccessTrigger();
        void processBuboHit();

        void startCurrentSequence( const bbool _trigger );
        void stopCurrentSequence();
        void startDeath();

        void triggerObject( const ObjectPath& _objectPath );
        void disableCollision( const bbool _disable );
        class Ray_BossMorayBodyPart* getHead();

        SequenceList m_sequences;
        u32 m_sequenceIndex;
        bbool m_starting;

        enum State
        {
            State_Node,
            State_NodeHit,
            State_ShrinkAndShake,
            State_Retract,
            State_FinalCine,
        };
        State m_state;
        f32 m_timer;
        void changeState( State _newState );

        void updateShrinkAndShake( const f32 _dt );

        void updateRetract( const f32 _dt );
        f32 m_retractTargetDist;
        f32 m_retractDist;
        ObjectPath m_finalCinematic;


        void updateSplashFX();
        bbool checkIsInWater() const;
        SpawneeGenerator m_splashFX;
        bbool m_wasInWater;

        void playMusic( const u32 _sequenceIndex );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void cheatCycleSequence();
#endif // ITF_SUPPORT_DEBUGFEATURE
    };

    //-------------------------------------------------------------------------------------
    class Ray_BossMorayAIComponent_Template : public Ray_SnakeAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossMorayAIComponent_Template, Ray_SnakeAIComponent_Template,1024689649);
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossMorayAIComponent);
        DECLARE_SERIALIZE();

    public:

        typedef ITF_VECTOR<EventPlayMusic> MusicList;

        Ray_BossMorayAIComponent_Template()
            : m_speedOnBuboHit(3.f)
            , m_accelerationOnBuboHit(1.f)

            , m_shrinkDuration(1.f)
            , m_shrinkSize(1.f)
            , m_shakeFrequency(1.f)
            , m_shakeAmplitude(1.f)

            , m_retractDelay(1.f)
            , m_retractSpeed(3.f)
            , m_retractTargetSmooth(0.01f)
            , m_retractSmooth(0.03f)
            , m_retractOffset(0.f)
            , m_retractFinishedLimit(1.f)
        {}
        ~Ray_BossMorayAIComponent_Template() {}

        ITF_INLINE f32 getSpeedOnBuboHit() const { return m_speedOnBuboHit; }
        ITF_INLINE f32 getAccelerationOnBuboHit() const { return m_accelerationOnBuboHit; }

        ITF_INLINE f32 getShrinkDuration() const { return m_shrinkDuration; }
        ITF_INLINE f32 getShrinkSize() const { return m_shrinkSize; }
        ITF_INLINE f32 getShakeFrequency() const { return m_shakeFrequency; }
        ITF_INLINE f32 getShakeAmplitude() const { return m_shakeAmplitude; }

        ITF_INLINE f32 getRetractDelay() const { return m_retractDelay; }
        ITF_INLINE f32 getRetractSpeed() const { return m_retractSpeed; }
        ITF_INLINE f32 getRetractTargetSmooth() const { return m_retractTargetSmooth; }
        ITF_INLINE f32 getRetractSmooth() const { return m_retractSmooth; }
        ITF_INLINE f32 getRetractOffset() const { return m_retractOffset; }
        ITF_INLINE f32 getRetractFinishedLimit() const { return m_retractFinishedLimit; }

        ITF_INLINE const Path& getSplashFX() const { return m_splashFX; }
        ITF_INLINE const MusicList& getMusics() const { return m_musics; }

    private:

        f32 m_speedOnBuboHit;
        f32 m_accelerationOnBuboHit;

        f32 m_shrinkDuration;
        f32 m_shrinkSize;
        f32 m_shakeFrequency;
        f32 m_shakeAmplitude;

        f32 m_retractDelay;
        f32 m_retractSpeed;
        f32 m_retractTargetSmooth;
        f32 m_retractSmooth;
        f32 m_retractOffset;
        f32 m_retractFinishedLimit;

        Path m_splashFX;
        MusicList m_musics;
    };

    ITF_INLINE const Ray_BossMorayAIComponent_Template* Ray_BossMorayAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BossMorayAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSMORAYAICOMPONENT_H_