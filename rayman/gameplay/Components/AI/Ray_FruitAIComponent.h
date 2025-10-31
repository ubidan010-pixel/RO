#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#define _ITF_RAY_AIFRUITCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    class AIBehavior;
    class Ray_AIReceiveHitBehavior;
    class Ray_AISleepBehavior;
    class StickToPolylinePhysComponent;
    class PhantomComponent;
    class PolylineComponent;
    class Ray_AIFruitRoamingBehavior;

    class Ray_FruitAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FruitAIComponent, Ray_AIComponent,1618051749);

    public:
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

        Ray_FruitAIComponent();
        virtual ~Ray_FruitAIComponent();

        void                        clear();
        void                        postLoad();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onResourceReady();
        virtual void                onCheckpointLoaded();
        virtual void                Update( f32 _deltaTime );

        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);

        virtual void                updateAnimInput();

        virtual void                onBecomeInactive();

        void                        requestDelayedStickToCharacter(const ActorRef &_character);
        void                        requestDelayedSnapToCharacter(const ActorRef &_character);
        bbool                       isActorInMemory( ObjectRef _ref ) const;
        f32                         getUnstickableRemainingTime() const {return m_timer;}
        bbool                       isActorOnPolyLine(const ActorRef &_actor) const {return m_actorsOnPolyline.find(_actor)!=-1;}
        bbool                       isActorHangingOnPolyline(const ActorRef &_actor) const {return m_actorsHangingOnPolyline.find(_actor)!=-1;}
        const ActorRef&             getCharacterOnWhichSnappedOrStuck() const {return m_snappedCharacter;}
        bbool                       tryStickOnPolyline( ObjectRef _polyline, u32 _edgeIndex );


    protected:
        // INTERRUPTIONS
        virtual void                processHit( PunchStim* _hit );

        //check death conditions
        virtual bbool               isInDeathCondition() const;
        void                        leaveSleep();

    private:
        ITF_INLINE const class Ray_FruitAIComponent_Template * getTemplate() const;

        struct AttachedActorsMemory
        {
            AttachedActorsMemory() : m_ref(ITF_INVALID_OBJREF), m_timer(0.f) {}
            AttachedActorsMemory( ObjectRef _ref, f32 _timer ) : m_ref(_ref), m_timer(_timer) {}

            ObjectRef       m_ref;
            f32             m_timer;
        };

        enum DelayedChangeType {DelayedChange_None=0, DelayedChange_StartSnap, DelayedChange_StartStuck};

        struct DelayedChangeFromBehavior
        {
            DelayedChangeType m_delayedChange;
            ActorRef m_targetActor;

            DelayedChangeFromBehavior() {init();}

            void init() {m_delayedChange = DelayedChange_None;m_targetActor.invalidate();}            
        };

        typedef FixedArray <AttachedActorsMemory,4> AttachedMemory;

        // BEHAVIORS
    protected:        
        Ray_AIFruitRoamingBehavior*     m_roamBehavior;
        PhantomComponent*               m_phantomComponent;

    private:
        class Ray_AIWaterBaseBehavior*  m_floatingBehavior;
        Ray_AIReceiveHitBehavior*       m_receiveHitBehavior;
        Ray_AISleepBehavior*            m_sleepBehavior;
        AIBehavior*                     m_stuckBehavior;
        AIBehavior*                     m_snappedBehavior;
        AIBehavior*                     m_deathBehavior;

        StickToPolylinePhysComponent*   m_physComponent;
        PolylineComponent*              m_polyLineComponent;
        ActorRef                        m_snappedCharacter;
        bbool                           m_snappedCharacterPreviousLookRight;
        bbool                           m_snappedCharacterLookRight;
        DelayedChangeFromBehavior       m_delayedChangeFromBehavior;
        bbool                           m_wasTriggered;

        void                startRoam(bbool _restart = bfalse);
        void                startReceiveHit( HitStim* _hit );
        void                startSleep();
        void                startStuck( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex );
        void                startStuck( Actor* _actor );
        void                startSnapped();
        void                startFloating( ObjectRef _poly );
        void                updateCheckDeath();

        void                updateRoam( f32 _dt );
        void                updateReceiveHit(f32 _dt);
        void                updateSwim( f32 _dt );
        void                updateCollisions();
        void                updateStuck( f32 _dt );

        void                onFinishedReceiveHit();

        void                processDelayedChangesFromBehaviors();

        // INTERRUPTIONS
        void                processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query );
        void                processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit );
        void                processQueryCanDarktoonify( Ray_EventQueryCanDarktoonify* _queryCanDarktoonify );
        void                processStickOnPolyline( EventStickOnPolyline* _stickOnPolyline );
        void                processNewHanging( EventHanging* _hanging );
        bbool               processStick( PolyLine* _poly, const PolyLineEdge* edge, u32 _edgeIndex );
        void                processTrigger(EventTrigger *_trigger);

        // Polyline
        void                activatePolyline();
        void                deactivatePolyline();
        void                resetState();

        void                stick();
        void                unstick();

        void                updateMemory( f32 _dt );
        void                removeFromMemory( ObjectRef _ref );

        //
        void                startDeath();


        PolyLine            m_polyLine;
        bbool               m_polyLineActive;
        bbool               m_isStuck;
        f32                 m_timer;
        SafeArray<ActorRef> m_actorsOnPolyline;
        SafeArray<ActorRef> m_actorsHangingOnPolyline;
        AttachedMemory      m_attachedActorsMemory;
        StringID            m_snapBone;
        u32                 m_snapBoneIndex;
        f32                 m_firstBounceDirection;

        ObjectRef           m_stuckPolyline;
        u32                 m_stuckEdge;
        ObjectRef           m_stuckActor;
        Vec2d               m_stuckLocalPos;

        f32                 m_shooterHitTimer;                
    };

    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_FruitAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FruitAIComponent_Template, Ray_AIComponent_Template,392815706);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FruitAIComponent);

    public:

        Ray_FruitAIComponent_Template();
        ~Ray_FruitAIComponent_Template();


        ITF_INLINE bbool    getHangingFruit() const             {return m_hangingFruit;}

        ITF_INLINE f32      getNonStickableTime() const         {return m_nonStickableTime;}
        ITF_INLINE const class Ray_AIFruitRoamingBehavior_Template* getRoamBehavior() const { return m_roamBehavior; }
        ITF_INLINE const class Ray_AIWaterBaseBehavior_Template* getFloatingBehavior() const { return m_floatingBehavior; }
        ITF_INLINE const class Ray_AIReceiveHitBehavior_Template* getReceiveHitBehavior() const { return m_receiveHitBehavior; }
        ITF_INLINE const class Ray_AISleepBehavior_Template* getSleepBehavior() const { return m_sleepBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getStuckBehavior() const { return m_stuckBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getSnappedBehavior() const { return m_snappedBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getDeathBehavior() const { return m_deathBehavior; }
        ITF_INLINE bbool getReinitWhenBecomesInactive() const {return m_reinitWhenBecomesInactive;}
        ITF_INLINE bbool getCanReceiveHit () const {return m_canReceiveHit;}
        ITF_INLINE f32 getShooterHitForce() const {return m_shooterHitForce;}
        ITF_INLINE f32 getShooterHitSpeedMultiplier() const {return m_shooterHitSpeedMultiplier;}
        ITF_INLINE f32 getSquashPenetrationRadiusPercent() const {return m_squashPenetrationRadiusPercent;}

    private:

        bbool               m_hangingFruit;

        f32                 m_nonStickableTime;
        bbool               m_reinitWhenBecomesInactive;
        bbool               m_canReceiveHit;
        f32                 m_shooterHitForce;
        f32                 m_shooterHitSpeedMultiplier;
        f32                 m_squashPenetrationRadiusPercent;

        class Ray_AIFruitRoamingBehavior_Template*      m_roamBehavior;
        class Ray_AIWaterBaseBehavior_Template*         m_floatingBehavior;
        class Ray_AIReceiveHitBehavior_Template*        m_receiveHitBehavior;
        class Ray_AISleepBehavior_Template*             m_sleepBehavior;
        class TemplateAIBehavior*                       m_stuckBehavior;
        class TemplateAIBehavior*                       m_snappedBehavior;
        class TemplateAIBehavior*                       m_deathBehavior;
    };

    ITF_INLINE const Ray_FruitAIComponent_Template * Ray_FruitAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_FruitAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIFRUITCOMPONENT_H_
