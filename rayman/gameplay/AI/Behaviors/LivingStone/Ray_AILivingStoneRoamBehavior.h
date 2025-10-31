#ifndef _ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_
#define _ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class AIPlayAnimAction;
    class AIFallAction;
    class AIJumpAction;

    class Ray_AILivingStoneRoamBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneRoamBehavior, Ray_AIGroundBaseMovementBehavior,1446134511);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AILivingStoneRoamBehavior();
        virtual ~Ray_AILivingStoneRoamBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta  );
        virtual void                        onEvent( Event* _event );
        virtual void                        onActionFinished();
        virtual void                        updateAnimInputs();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_AILivingStoneRoamBehavior_Template * getTemplate() const;


        AIAction*                           m_fall;
        AIAction*                           m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)
        AIAction*                           m_attack;     // default silly pause animation (optional, immediate u-turn if null)

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
        ActorRef                            m_obstacle;

    };

    class Ray_AILivingStoneRoamBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneRoamBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,2140270652);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILivingStoneRoamBehavior);

    public:

        Ray_AILivingStoneRoamBehavior_Template();
        ~Ray_AILivingStoneRoamBehavior_Template();

        ITF_INLINE const class AIAction_Template* getFall() const { return m_fall; }
        ITF_INLINE const class AIAction_Template* getDefaultPause() const { return m_defaultPause; }
        ITF_INLINE f32 getUturnDelay() const { return m_uturnDelay; }
        ITF_INLINE const class AIAction_Template* getAttack() const { return m_attack; }
        ITF_INLINE const AABB & getAttackRange() const { return m_attackRange; }

    private:
        class AIAction_Template* m_fall;
        class AIAction_Template* m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)
        class AIAction_Template* m_attack;     // default silly pause animation (optional, immediate u-turn if null)
        f32                      m_uturnDelay;
        AABB                     m_attackRange;
    };

    ITF_INLINE const class Ray_AILivingStoneRoamBehavior_Template * Ray_AILivingStoneRoamBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AILivingStoneRoamBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_