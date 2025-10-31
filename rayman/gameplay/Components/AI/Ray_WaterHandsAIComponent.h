#ifndef _ITF_RAY_WATERHANDSAICOMPONENT_H_
#define _ITF_RAY_WATERHANDSAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#define STATE_ATTACK_DISABLED       0
#define STATE_ATTACK_FW             1
#define STATE_ATTACK_ANTICIP        2
#define STATE_ATTACK_CHARGE         3
#define STATE_ATTACK_CHARGE_MISS    4
#define STATE_ATTACK_BW             5
#define STATE_ATTACK_WAIT           6
#define STATE_ATTACK_WAIT_CAUGHT    7
#define STATE_ATTACK_CATCH_BW       8
#define STATE_ATTACK_DESTROY        9

#define STATE_LINKATTACK_DISABLED   10
#define STATE_LINKATTACK_BW         11


#define TYPE_ANIM_MOUTH             0
#define TYPE_ANIM_HAND              1
#define TYPE_ANIM_HAND_2            2
#define TYPE_ANIM_COUNT             3

namespace ITF
{
    class FXControllerComponent;
    class PatchCurveComponent;
    class LinkComponent;
    class AnimLightComponent;

    class Ray_WaterHandsAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterHandsAIComponent, Ray_AIComponent,687583937)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    private:

        struct SlotAttack
        {

            SlotAttack()
            : m_state(STATE_ATTACK_DISABLED)
            , m_time(0.0f)
            , m_targetAngle(0.0f)
            , m_targetAngleIntermedA(0.0f)
            , m_typeAnim(TYPE_ANIM_HAND)
            , m_handleFx(U32_INVALID)
            , m_alpha(1.0f)
            , m_hasCatch(bfalse)
            , m_mustReturn(bfalse)
            , m_isAttackSpawned(btrue)
            , m_distCharge(0.0f)
            , m_countTrameCharge(0)
            , m_isKillAttack(btrue)
            , m_missAttack(bfalse)
            {
                // none
            }

            // Patch curve
            BezierPatch  m_patchCurve;

            // Handle
            ActorRef    m_playerRef;
            ActorRef    m_handRef;
            u32         m_handleFx;

            // Move
            Vec2d       m_targetPos;
            Vec2d       m_RootPos;
            Vec2d       m_targetIntermedA;

            // Rotate
            f32         m_targetAngle;
            f32         m_targetAngleIntermedA;

            // Misc
            u32         m_typeAnim;
            u32         m_state;
            f32         m_time;
            f32         m_alpha;

            // Charge
            f32         m_distCharge;
            u32         m_countTrameCharge;
            Vec2d       m_posStartCharge;

            // bool
            bbool       m_hasCatch;
            bbool       m_mustReturn;
            bbool       m_isAttackSpawned;
            bbool       m_isKillAttack;
            bbool       m_missAttack;

        };

    public:

        Ray_WaterHandsAIComponent();
        virtual ~Ray_WaterHandsAIComponent();

        virtual bbool       needsUpdate()   const { return btrue; }
        virtual bbool       needsDraw()     const { return btrue; }
        virtual bbool       needsDraw2D()   const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
		virtual void		onBecomeInactive();
        virtual void        onResourceReady();
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event * _event );

		virtual void		onCheckpointLoaded();


#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void        Draw();

                f32         getRangeMaxDetect() const { return m_rangeMaxDetect; }
                void        setRangeMaxDetect(f32 _range) { m_rangeMaxDetect = _range; }

    private:
 
                void        updateDetect( f32 _dt);
                void        updateAttack( f32 _dt );
                void        updateRotation( f32 _dt );
#ifdef ITF_SUPPORT_DEBUGFEATURE
                void        updateDebug( f32 _dt );
#endif // ITF_SUPPORT_DEBUGFEATURE

                void        growAABB();

                void        prepareAttack();
                void        newSpawnAttack(Actor* _player);
                void        newLinkAttack(Actor* _handChild);

                u32         getSlotIndexByHand(Actor* _hand);
                u32         getSlotIndexByTarget(Actor* _target);

                void        launchFinalAttack(Actor* _hand, Actor* _target);
                void        switchTarget(u32 _indexSlot, Actor* _newTarget);
                void        lockAttack(u32 _indexSlot);

                void        chargeOnPlayer(Actor* _hand);
                bbool       catchPlayer(Actor* _hand);

                void        updateDeath();

                bbool       isAttackable(Vec2d _pos);
                bbool       playerIsCatchable(Actor* _player );
                bbool       handIsFree( u32 _index );
                bbool       queryIsDead( Actor* _player );
                bbool       queryIsCaught( Actor* _player );

                void        changeState( SlotAttack& _slot,  u32 _newState );

                bbool       moveHand( SlotAttack& _slot, f32 _dt, f32 _ratio);
                f32         getAngle(Actor* _source, Actor* _dest);
                f32         getBezierAngle( SlotAttack& _slot );
                void        idleSin( SlotAttack& _slot, f32 _dt);

                void        playAnim( SlotAttack& _slot , u32 _state);

                u32         playFx(Vec3d _pos);
                void        stopFx(u32 _indexFx);

                void        snapPlayerCaught( SlotAttack& _slot );
                Vec2d       getPosHandbySnapBone( SlotAttack& _slot );

                u32         getHP( Actor* _player );

                void        releaseAll();
                void        receiveHit(HitStim* _hitEvent);


    private:

        ITF_INLINE const class Ray_WaterHandsAIComponent_Template * getTemplate() const;

        PatchCurveComponent*    m_patchCurveComponent;
        FXControllerComponent*  m_fxControllerComponent;
        LinkComponent*          m_linkComponent;
        AnimLightComponent*     m_animLightComponent;

        Frise*                  m_friseSwim;
        SpawneeGenerator        m_handsSpawner;
        ITF::Vector<SlotAttack> m_listAttack;
        f32                     m_rangeMaxDetect;

        // Death
        bbool                   m_mustDie;
        bbool                   m_isDying;
		bbool					m_isDead;

		u32						m_typeAnimCount;
		u32						m_handleFxRoot;
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_WaterHandsAIComponent_Template : public Ray_AIComponent_Template
    { 
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterHandsAIComponent_Template, Ray_AIComponent_Template,249265877);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WaterHandsAIComponent);

    public:

        Ray_WaterHandsAIComponent_Template();
        ~Ray_WaterHandsAIComponent_Template();

        ITF_INLINE const AABB&  getEnemyDetectionRange() const      { return m_enemyDetectionRange; }
        ITF_INLINE const Path&  getSpawnHandPath() const            { return m_spawnHandPath; }
        
        ITF_INLINE f32          getSpeedAttack() const              { return m_speedAttack; }
        ITF_INLINE f32          getSpeedReturn() const              { return m_speedReturn; }

        ITF_INLINE f32          getTimeWaitOnPlayerEscape() const   { return m_timeWaitOnPlayerEscape; }
        ITF_INLINE f32          getTimeWaitAfterCaught() const      { return m_timeWaitAfterCaught; }

        ITF_INLINE f32          getTimeAnticip() const      { return m_timeAnticip; }

        ITF_INLINE bbool        getUseRadiusMaxAttack() const       { return m_useRadiusMaxAttack; }
        ITF_INLINE bbool        getLineareMode() const              { return m_linearMode; }

        ITF_INLINE f32          getDistMaxCharge() const      { return m_distMaxCharge; }

		ITF_INLINE bbool        getRandomHands() const              { return m_randomHands; }

		
    private:
  
        AABB    m_enemyDetectionRange;
        Path    m_spawnHandPath;

        f32     m_speedAttack;
        f32     m_speedReturn;

        f32     m_timeWaitOnPlayerEscape;
        f32     m_timeWaitAfterCaught;
        f32     m_timeAnticip;

        bbool   m_useRadiusMaxAttack;
        
        bbool   m_linearMode;

        f32     m_distMaxCharge;

		bbool   m_randomHands;

    };


    ITF_INLINE const class Ray_WaterHandsAIComponent_Template * Ray_WaterHandsAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_WaterHandsAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_WATERHANDSAICOMPONENT_H_
