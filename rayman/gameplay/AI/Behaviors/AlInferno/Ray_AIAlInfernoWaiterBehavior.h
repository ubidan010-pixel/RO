#ifndef _ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_
#define _ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIAlInfernoWaiterBehavior_Template;
    class AIFollowActorAction;
    class AIFollowActorAction_Template;
    class PhantomComponent;

    class Ray_AIAlInfernoWaiterBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIAlInfernoWaiterBehavior, Ray_AIGroundBaseMovementBehavior,2725792230);
        DECLARE_VALIDATE_BEHAVIOR()

    protected:
        enum WaiterState
        {
            WaiterState_UNDEFINED = -1,
            WaiterState_IDLE,
            WaiterState_ATTACK,
            WaiterState_UTURN_WALK,
            WaiterState_CATCH,
            WaiterState_PATROL,
            WaiterState_FALL,
            WaiterState_CRUSHED,
        };

    public:

        Ray_AIAlInfernoWaiterBehavior();
        virtual ~Ray_AIAlInfernoWaiterBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update(f32 _dt);
        virtual void                        onEvent(Event* _event);
        virtual void                        onActionFinished();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const Ray_AIAlInfernoWaiterBehavior_Template * getTemplate() const;

        //start actions
        void startState(const WaiterState _state);
        void startAttack();
        void startFall();
        void startUturnWalk();
        void startPatrol();
        void startCatchOnTray();
        void startCrushed();
        void startIdle();

        //out of actions
        void stopAttack();
        void stopSliding();
        void stopCrushed();
        void stopUturn();
        void stopCatch();

        //update actions
        void updateIdle();
        void updatePatrol();
        void updateCatch();
        void updateAttack();
        void updateCrushed();

        //look for a target
        void updateTarget(const f32 _dt);

        //return target to attack
        const Actor * checkLongRangeAttackTarget(bbool & _uTurn) const;

        //return true if waiter has to attack
        bbool checkAttack() const;
        //return true if there is no floor under waiter
        bbool checkImmediateHole() const;
        //return true if there is anyking of obstacle in range (if parameter is false obstacle are check with flip, else with speed dir)
        bbool checkObstacle(const bbool _useSpeedDirAsFlip = bfalse) const;
        //return true if there is an obstacle on the polyline actor is sticked to
        bbool checkStickedPolylineObstacle(const bbool _flipped) const;
        //return true if there is a contact obstacle on phys component
        bbool checkPhysObstacle(const bbool _flipped) const;
        //return true if there is a contact on a given object ref polyline
        bbool checkPolylineObstacle(const bbool _flipped, const ObjectRef &_poly, const u32 _edgeIndex, const Vec2d &_contact) const;

        //process events
        void processAnimEvent(const AnimGameplayEvent * const _event);
        void processStickEvent(const EventStickOnPolyline * const _event);
        void processPunchEvent(const PunchStim * const _event);
        void processActivateEvent(const EventActivate * const _event);
        void processDetachEvent(const EventDetach * const _detachEvent);
        void processQueryCanReceiveHit(EventQueryCanReceiveHit * const _query);

        //manage fx
        void startFlamesFX();
        void stopFlamesFX();

        //return if waiter is going to right or left
        bbool isWaiterSpeedXPositive() const;

        //Data
    private:
        static const f32 MIN_TIME_BETWEEN_ATTACK;

        WaiterState m_state;

        AIAction* m_fallAction;
        AIAction* m_attackLongRangeAction;
        AIWalkInDirAction* m_patrolAction;
        AIAction* m_crushedAction;
        AIAction* m_uturnWalkAction;
        
        PhantomComponent * m_phantomComponent;

        u32 m_flamesFXHanfle;

        ActorRef m_target;
        bbool m_targetUturn;
        f32 m_aggroTime;

        f32 m_timeSinceLastAttack;

        bbool m_linkedToActivator;
        bbool m_activated;
    };

    class Ray_AIAlInfernoWaiterBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIAlInfernoWaiterBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,3313947922);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIAlInfernoWaiterBehavior);

    public:

        Ray_AIAlInfernoWaiterBehavior_Template();
        ~Ray_AIAlInfernoWaiterBehavior_Template();

        ITF_INLINE const AIAction_Template* getFallAction() const;
        ITF_INLINE const AIAction_Template* getPatrolAction() const;
        ITF_INLINE const AIAction_Template* getAttackLongRangeAction() const;
        ITF_INLINE const AIAction_Template* getCrushedAction() const;
        ITF_INLINE const AIAction_Template* getUturnWalkAction() const;
        ITF_INLINE const AABB & getLongRangeAttackDetectionRange() const;
        ITF_INLINE const StringID & getFlamesFXName() const;
        ITF_INLINE const StringID & getCatchWalkAnimation() const;
        ITF_INLINE f32 getCrushedVerticalSpeedThreshold() const;
        ITF_INLINE f32 getAttackTimeThreshold() const;
        ITF_INLINE f32 getBrakingForce() const;

        //Data
    private:
        AIAction_Template* m_fallAction;
        AIAction_Template* m_attackLongRangeAction;
        AIAction_Template* m_patrolAction;
        AIAction_Template* m_crushedAction;
        AIAction_Template* m_uturnWalkAction;

        AABB m_longRangeAttackDetectionRange;

        StringID m_flamesFXName;
        StringID m_catchWalkAnimation;

        f32 m_crushedVerticalSpeedThreshold;
        f32 m_attackTimeThreshold;
        f32 m_brakingForce;
    };

    ITF_INLINE const Ray_AIAlInfernoWaiterBehavior_Template * Ray_AIAlInfernoWaiterBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIAlInfernoWaiterBehavior_Template *>(m_template);
    }


    ITF_INLINE const class AIAction_Template* Ray_AIAlInfernoWaiterBehavior_Template::getFallAction() const
    { 
        return m_fallAction; 
    }

    ITF_INLINE const class AIAction_Template* Ray_AIAlInfernoWaiterBehavior_Template::getPatrolAction() const 
    { 
        return m_patrolAction; 
    }

    ITF_INLINE const class AIAction_Template* Ray_AIAlInfernoWaiterBehavior_Template::getAttackLongRangeAction() const 
    { 
        return m_attackLongRangeAction; 
    }

    ITF_INLINE const AIAction_Template* Ray_AIAlInfernoWaiterBehavior_Template::getCrushedAction() const
    {
        return m_crushedAction;
    }

    ITF_INLINE const AIAction_Template* Ray_AIAlInfernoWaiterBehavior_Template::getUturnWalkAction() const
    {
        return m_uturnWalkAction;
    }
    
    ITF_INLINE const AABB & Ray_AIAlInfernoWaiterBehavior_Template::getLongRangeAttackDetectionRange() const
    {
        return m_longRangeAttackDetectionRange;
    }

    ITF_INLINE const StringID & Ray_AIAlInfernoWaiterBehavior_Template::getFlamesFXName() const
    {
        return m_flamesFXName;
    }

    ITF_INLINE const StringID & Ray_AIAlInfernoWaiterBehavior_Template::getCatchWalkAnimation() const
    {
        return m_catchWalkAnimation;
    }
    
    ITF_INLINE f32 Ray_AIAlInfernoWaiterBehavior_Template::getCrushedVerticalSpeedThreshold() const
    {
        return m_crushedVerticalSpeedThreshold;
    }

    ITF_INLINE f32 Ray_AIAlInfernoWaiterBehavior_Template::getAttackTimeThreshold() const
    {
        return m_attackTimeThreshold;
    }

    ITF_INLINE f32 Ray_AIAlInfernoWaiterBehavior_Template::getBrakingForce() const
    {
        return m_brakingForce;
    }

}

#endif //_ITF_RAY_AIALINFERNOPATROLBEHAVIOR_H_