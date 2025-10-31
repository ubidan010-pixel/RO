#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIUTURNACTION_H_
#include "gameplay/ai/Actions/AIUTurnAction.h"
#endif //_ITF_AIUTURNACTION_H_

#ifndef _ITF_AIBOUNCETOLAYERACTION_H_
#include "gameplay/ai/Actions/AIBounceToLayerAction.h"
#endif //_ITF_AIBOUNCETOLAYERACTION_H_

#ifndef _ITF_AIBUMPERACTION_H_
#include "gameplay/ai/Actions/AIBumperAction.h"
#endif //_ITF_AIBUMPERACTION_H_

namespace ITF
{
    class Ray_AIGroundBaseMovementBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseMovementBehavior,Ray_AIGroundBaseBehavior,2985299165);

    public:

        Ray_AIGroundBaseMovementBehavior();
        virtual ~Ray_AIGroundBaseMovementBehavior();

        virtual void                onActivate();
        virtual void                onDeactivate();
        virtual void                update( f32 _dt );

        virtual void                onActionFinished();
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onEvent(Event * _event);

    protected:
        ITF_INLINE const class Ray_AIGroundBaseMovementBehavior_Template * getTemplate() const;

        AIIdleAction*               m_idle;
        AIWalkInDirAction*          m_move;
        AIUTurnAction*              m_uturn;
        AIBounceToLayerAction*m_bounceToLayer;
        AIBumperAction*       m_bounce;


        f32                         m_stuckTimer;

        f32                         m_uturnTimer;

        bbool                       m_bouncingToLayer;
        bbool                       m_bouncing;

        bbool                       queryUTurn( f32 _dt );
        void                        performUTurn( f32 _dt );
        virtual void                performUTurnImmediate();    // TODO: is this version necessary?
        void                        startBounceToLayer();

        void                        playBounceFx();

        void                        startBounce();
        virtual void                onActionFinishedUturn();
        virtual void                onActionFinishedBounceToLayer(){}
        virtual void                onActionFinishedBounce(){}
        virtual void                onActionFinishedMove(){}

        bbool                       isBouncing() const { return m_bounce && m_currentAction == m_bounce; }
        bbool                       isBouncingToLayer() const { return m_bounceToLayer && m_currentAction == m_bounceToLayer; }
    };

    class Ray_AIGroundBaseMovementBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseMovementBehavior_Template,Ray_AIGroundBaseBehavior_Template,978018934);
        DECLARE_SERIALIZE();

    public:

        Ray_AIGroundBaseMovementBehavior_Template();
        ~Ray_AIGroundBaseMovementBehavior_Template();

        ITF_INLINE const class AIIdleAction_Template* getIdle() const { return m_idle; }
        ITF_INLINE const class AIWalkInDirAction_Template* getMove() const { return m_move; }
        ITF_INLINE const class AIUTurnAction_Template* getUturn() const { return m_uturn; }
        ITF_INLINE class AIBounceToLayerAction_Template* getBounceToLayer() const { return m_bounceToLayer; }
        ITF_INLINE class AIBumperAction_Template* getBounce() const { return m_bounce; }

        ITF_INLINE f32 getObstacleDetectionRange() const { return m_obstacleDetectionRange; }
        ITF_INLINE f32 getObstacleJumpHeight() const { return m_obstacleJumpHeight; }
        ITF_INLINE f32 getHoleDetectionRange() const { return m_holeDetectionRange; }
        ITF_INLINE f32 getHoleJumpDepth() const { return m_holeJumpDepth; }
        ITF_INLINE f32 getStuckDelay() const { return m_stuckDelay; }
        ITF_INLINE f32 getUturnDelay() const { return m_uturnDelay; }
    private:
        class AIIdleAction_Template*               m_idle;
        class AIWalkInDirAction_Template*          m_move;
        class AIUTurnAction_Template*              m_uturn;
        class AIBounceToLayerAction_Template*      m_bounceToLayer;
        class AIBumperAction_Template*             m_bounce;
        f32                         m_obstacleDetectionRange;
        f32                         m_obstacleJumpHeight;       // how high up we can jump
        f32                         m_holeDetectionRange;
        f32                         m_holeJumpDepth;            // how far down we can jump
        f32                         m_stuckDelay;               // if we don't have speed for X seconds, that means we're stuck
        f32                         m_uturnDelay;
    };

    ITF_INLINE const class Ray_AIGroundBaseMovementBehavior_Template * Ray_AIGroundBaseMovementBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundBaseMovementBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_