#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class AIJumpAction;
    class AIFallAction;

    class Ray_AIGroundRunAwayBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundRunAwayBehavior, Ray_AIGroundBaseMovementBehavior,1165175256);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIGroundRunAwayBehavior();
        virtual ~Ray_AIGroundRunAwayBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        update( f32 _delta  );
        virtual void                        onActionFinished();

        void                                setData( ObjectRef _attacker ) { m_attacker = _attacker; }

    private:

        ITF_INLINE const class Ray_AIGroundRunAwayBehavior_Template* getTemplate() const;

        AIJumpAction*                       m_jumpOverObstacle;
        AIJumpAction*                       m_jumpOverVoid;
        AIFallAction*                       m_fall;

        void                                updateRunning( f32 _dt );

        bbool                               hasEnemyInFront() const;
        void                                performJump( bbool _jumpToVoid );

        ObjectRef                           m_attacker;
    };

    class Ray_AIGroundRunAwayBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundRunAwayBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,466747632);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIGroundRunAwayBehavior);

    public:

        Ray_AIGroundRunAwayBehavior_Template();
        ~Ray_AIGroundRunAwayBehavior_Template();

        const class AIJumpAction_Template*          getJumpOverObstacle() const { return m_jumpOverObstacle; }
        const class AIJumpAction_Template*          getJumpOverVoid() const { return m_jumpOverVoid; }
        const class AIFallAction_Template*          getFall() const { return m_fall; }
        f32                                         getJumpOverEnemyDetection() const { return m_jumpOverEnemyDetection; }
        f32                                         getJumpOverVoidDetection() const { return m_jumpOverVoidDetection; }

    private:

        class AIJumpAction_Template*                m_jumpOverObstacle;
        class AIJumpAction_Template*                m_jumpOverVoid;
        class AIFallAction_Template*                m_fall;
        f32                                         m_jumpOverEnemyDetection;
        f32                                         m_jumpOverVoidDetection;
    };

    ITF_INLINE const Ray_AIGroundRunAwayBehavior_Template * Ray_AIGroundRunAwayBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundRunAwayBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_