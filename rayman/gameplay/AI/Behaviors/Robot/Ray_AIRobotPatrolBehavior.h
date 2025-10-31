#ifndef _ITF_RAY_AIROBOTPATROLBEHAVIOR_H_
#define _ITF_RAY_AIROBOTPATROLBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class AIPlayAnimAction;
    class AIFallAction;

    class Ray_AIRobotPatrolBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRobotPatrolBehavior, Ray_AIGroundBaseMovementBehavior,713607306)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIRobotPatrolBehavior();
        virtual ~Ray_AIRobotPatrolBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update( f32 _delta  );
        virtual void                        onEvent( Event* _event );
        virtual void                        onActionFinished();
        virtual void                        updateAnimInputs();

#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_AIRobotPatrolBehavior_Template * getTemplate() const;


        AIAction*                           m_fall;
        AIAction*                           m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)
        AIAction*                           m_attack;

        virtual void                        updateIdle( f32 _dt );
        virtual bbool                       updateMove( f32 _dt );

        virtual void                        startIdle();
        void                                startMove( bbool _walkLeft );
        void                                startFall();
        virtual void                        startAnimPause(  );
        void                                startAttack();

        void                                onActionFinishedMove();
        virtual void                        onActionFinishedUturn();
        void                                onActionFinishedFall();
        void                                onActionFinishedPause();
        virtual void                        onActionFinishedBounceToLayer();
        virtual void                        onActionFinishedBounce();
        void                                onActionFinishedAttack();

        void                                processEventBlockedByPolyline( EventBlockedByPolyline* _blockedEvent );
        bbool                               checkPolylineObstacles( PolyLine* poly, u32 edgeIndex );
        bbool                               checkBlockingContacts();
        bbool                               checkBlockingContact(ObjectRef _poly, u32 _edgeIndex, Vec2d _contact);
        bbool                               checkStuck( f32 _dt );
        bbool                               checkUturn();
        bbool                               checkAttack();

        bbool                               checkHole( PolyLine* poly );
        bbool                               isValidSlope( f32 _slopeAngle ) const;
#ifndef ITF_FINAL
        void                                debugContacts();
#endif // ITF_FINAL

        // we came in contact with a blocking polyline
        ObjectRef                           m_blockingPoly;
        u32                                 m_blockingEdgeIndex;
        Vec2d                               m_blockingContact;
        class PhantomComponent *            m_phantomComponent;
        f32                                 m_timer;
        u32                                 m_flameHandle;
        ActorRef                            m_obstacle;

    };

    class Ray_AIRobotPatrolBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRobotPatrolBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,126022324);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRobotPatrolBehavior);

    public:

        Ray_AIRobotPatrolBehavior_Template();
        ~Ray_AIRobotPatrolBehavior_Template();

        ITF_INLINE const class AIAction_Template* getFall() const { return m_fall; }
        ITF_INLINE const class AIAction_Template* getDefaultPause() const { return m_defaultPause; }
        ITF_INLINE const class AIAction_Template* getAttack() const { return m_attack; }
        ITF_INLINE f32 getUturnDelay() const { return m_uturnDelay; }
        ITF_INLINE const AABB & getDetectionRange() const { return m_detectionRange; }

    private:
        class AIAction_Template* m_fall;
        class AIAction_Template* m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)
        class AIAction_Template* m_attack;
        AABB                     m_detectionRange;
        f32                      m_uturnDelay;
    };

    ITF_INLINE const class Ray_AIRobotPatrolBehavior_Template * Ray_AIRobotPatrolBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRobotPatrolBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIROBOTPATROLBEHAVIOR_H_