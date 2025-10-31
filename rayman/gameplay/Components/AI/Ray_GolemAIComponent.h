#ifndef _ITF_RAY_GOLEMAICOMPONENT_H_
#define _ITF_RAY_GOLEMAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif
    
#define STATE_GOLEM_SLEEP                   0
#define STATE_GOLEM_WAKEUP                  1
#define STATE_GOLEM_STAND                   2
#define STATE_GOLEM_HURTING                 3
#define STATE_GOLEM_LAST_HURTING            4
#define STATE_GOLEM_STAND_BROKEN            5
#define STATE_GOLEM_DEATH                   6
#define STATE_GOLEM_ANTICIP_ATTACK          7
#define STATE_GOLEM_ATTACK                  8
#define STATE_GOLEM_ANTICIP_ATTACK_BROKEN   9
#define STATE_GOLEM_ATTACK_BROKEN           10


#define INDEX_EYEBROW_LEFT      1
#define INDEX_EYEBROW_RIGHT     2
#define INDEX_EYE_LEFT          3
#define INDEX_EYE_RIGHT         4
#define INDEX_NOSE              5

#define GOLEM_EXPULSE_ANGLE       MTH_PIBY4
#define GOLEM_PERCENT_START_ALPHA 0.75f

#define GOLEM_PHASE_1   0
#define GOLEM_PHASE_2   1
#define GOLEM_PHASE_3   2
#define GOLEM_PHASE_4   3
#define GOLEM_PHASE_5   4


namespace ITF
{

    class AnimatedComponent;
    class PolylineComponent;
    class LinkComponent;

    class Ray_GolemAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GolemAIComponent, Ray_AIComponent,1489747707)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_GolemAIComponent();
        virtual ~Ray_GolemAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    onBecomeActive();

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);


#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:


        struct Golelement
        {
            Golelement()
                : m_index(U32_INVALID)
                , m_indexBone(-1)
                , m_originPos(Vec2d::Zero)
                , m_curPos(Vec2d::Zero)
                , m_curAngle(0.0f)
                , m_actor(ITF_INVALID_OBJREF)
                , m_expulsed(bfalse)
                , m_timeExpulse(0.0f)
                , m_dirExpulse(Vec2d::Zero)
                , m_zOffset(0.01f)
                , m_expulseToRight(bfalse)
                , m_alpha(1.0f)
                , m_disabled(bfalse)
                , m_factorEye(0.5f,0.5f)
                , m_zOffsetExpulse(0.0f)
            {
                // none
            }

            ~Golelement() {}

            ActorRef m_actor;

            Vec2d m_originPos;
            Vec2d m_curPos;
            Vec2d m_dirExpulse;
            Vec2d m_factorEye;
            f32   m_zOffset;
            f32   m_curAngle;
            f32   m_timeExpulse;
            f32   m_zOffsetExpulse;
            f32   m_alpha;
            u32   m_index;
            i32   m_indexBone;
            bbool m_expulsed;
            bbool m_expulseToRight;
            bbool m_disabled;



        };

        ITF_INLINE const class Ray_GolemAIComponent_Template*  getTemplate() const;

                void    init();

                void    addElement(u32 _index, SpawneeGenerator* spawnee, StringID _boneName, f32 _zOffset);
                void    processTrigger( EventTrigger* _trigger );
                void    processHit();
                void    changeState( u32 _newState );
                void    playAnim( const StringID _anim);
                void    UpdatePupil( f32 _dt);
                void    updateElement( f32 _dt );
                void    updateBossBubon();
                void    setBossBubonPos(u32 _indexBone, ActorRef _bubon);
                Vec2d   getPupilFactor(Vec2d _from, Vec2d _to);
                void    triggerBubon(bbool _open);
                void    startExpulse(u32 _indexElement, Vec2d _dirExpulse);
                bbool   updatePosBallistics(Golelement& _element, f32 _dt );

				void	playMusic();

    private:
     
        AnimatedComponent* m_animatedComponent;
        PolylineComponent* m_polylineComponent;
        LinkComponent*     m_linkComponent;



        ITF_VECTOR<Golelement> m_listGolelement;

        u32 m_state;

        f32 m_countDownAttack;
        f32 m_timeAnticip;

        bbool m_golemIsInit;
        bbool m_golemIsWakeUp;

        // 
        u32 m_countHit_left;
        u32 m_countHit_right;
        u32 m_phaseState;

        // BossBubon
        ActorRef m_bossBubonLeft;
        ActorRef m_bossBubonRight;
        ActorRef m_bossBubonUp;
        bbool m_bossBubonCanMove;
        u32 m_indexBone_BubonLeft;
        u32 m_indexBone_BubonRight;
        u32 m_indexBone_BubonUp;

        // Spawn
        SpawneeGenerator        m_eyeBrowLeft_Spawner;
        SpawneeGenerator        m_eyeBrowRight_Spawner;
        SpawneeGenerator        m_eyeLeft_Spawner;
        SpawneeGenerator        m_eyeRight_Spawner;
        SpawneeGenerator        m_nose_Spawner;

		// Music
		u32 m_sequenceIndex;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_GolemAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GolemAIComponent_Template, Ray_AIComponent_Template,3787101293);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GolemAIComponent);

    public:

        typedef ITF_VECTOR<EventPlayMusic> MusicList;

        ITF_INLINE f32 getCountDownAttack() const     { return m_countDownAttack; }
        ITF_INLINE f32 getTimeAnticipation() const     { return m_timeAnticipation; }
        
        ITF_INLINE f32 getGravityBallistics() const     { return m_gravityBallistics; }
        ITF_INLINE f32 getTimeExpulse() const           { return m_timeExpulse; }
        ITF_INLINE f32 getPowerExpulse() const           { return m_powerExpulse; }
        ITF_INLINE f32 getZOffsetExpulse() const           { return m_zOffsetExpulse; }
        ITF_INLINE const Angle& getAngleRotateExpulse() const       { return m_angleRotateExpulse; }

        ITF_INLINE f32 getFactorSmoothPupil() const           { return m_factorSmoothPupil; }

        ITF_INLINE Path getActEyeBrow_Left() const            { return m_actEyeBrow_Left; }
        ITF_INLINE StringID getBoneEyeBrow_Left() const       { return m_boneEyeBrow_Left; }
        ITF_INLINE f32 getZOffsetEyeBrow_Left() const           { return m_zOffsetEyeBrow_Left; }

        ITF_INLINE Path getActEyeBrow_Right() const            { return m_actEyeBrow_Right; }
        ITF_INLINE StringID getBoneEyeBrow_Right() const       { return m_boneEyeBrow_Right; }
        ITF_INLINE f32 getZOffsetEyeBrow_Right() const           { return m_zOffsetEyeBrow_Right; }

        ITF_INLINE Path getActEye_Left() const                { return m_actEye_Left; }
        ITF_INLINE StringID getBoneEye_Left() const           { return m_boneEye_Left; }
        ITF_INLINE f32 getZOffsetEye_Left() const           { return m_zOffsetEye_Left; }

        ITF_INLINE Path getActEye_Right() const                { return m_actEye_Right; }
        ITF_INLINE StringID getBoneEye_Right() const           { return m_boneEye_Right; }
        ITF_INLINE f32 getZOffsetEye_Right() const           { return m_zOffsetEye_Right; }

        ITF_INLINE Path getActNose() const                { return m_actNose; }
        ITF_INLINE StringID getBoneNose() const           { return m_boneNose; }
        ITF_INLINE f32 getZOffsetNose() const           { return m_zOffsetNose; }

        ITF_INLINE const MusicList& getMusics() const { return m_musics; }

        Ray_GolemAIComponent_Template();
        ~Ray_GolemAIComponent_Template();

    private:

        // Attack
        f32 m_countDownAttack;
        f32 m_timeAnticipation;

        // Ballistics
        f32 m_gravityBallistics;
        f32 m_timeExpulse;
        f32 m_powerExpulse;
        f32 m_zOffsetExpulse;
        Angle m_angleRotateExpulse;

        f32 m_factorSmoothPupil;

        Path m_actEyeBrow_Left;
        StringID m_boneEyeBrow_Left;
        f32 m_zOffsetEyeBrow_Left;

        Path m_actEyeBrow_Right;
        StringID m_boneEyeBrow_Right;
        f32 m_zOffsetEyeBrow_Right;

        Path m_actEye_Left;
        StringID m_boneEye_Left;
        f32 m_zOffsetEye_Left;

        Path m_actEye_Right;
        StringID m_boneEye_Right;
        f32 m_zOffsetEye_Right;

        Path m_actNose;
        StringID m_boneNose;
        f32 m_zOffsetNose;

		MusicList m_musics;

    };

    const Ray_GolemAIComponent_Template*  Ray_GolemAIComponent::getTemplate() const {return static_cast<const Ray_GolemAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_GOLEMAICOMPONENT_H_