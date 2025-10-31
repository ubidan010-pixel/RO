#ifndef _ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_
#define _ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class StickToPolylinePhysComponent;
    class AIJumpInDirAction;
    class AIJumpInDirAction;
    class AIJumpAction;

    class Ray_AIBounceRoamingBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBounceRoamingBehavior, AIBehavior,867562451)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIBounceRoamingBehavior();
        virtual ~Ray_AIBounceRoamingBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();

    private:
        ITF_INLINE const class Ray_AIBounceRoamingBehavior_Template * getTemplate() const;

        void                                updateMove();
        void                                updateIdle();

        void                                toggleMoveIdle();

        void                                setIdleTime();
        void                                setMoveTime();
        void                                startIdle();
        void                                startMove();


        StickToPolylinePhysComponent*       m_physComponent;

        AIJumpInDirAction*                  m_bounceMove;
        AIJumpInDirAction*                  m_bounceIdle;
        AIJumpAction*                       m_bounceJump;

        f32                                 m_timeCounter;
        bbool                               m_moveLeft;
        bbool                               m_toggle;

        f32                                 m_previousSpeedLossMultiplier;
    };
    class Ray_AIBounceRoamingBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBounceRoamingBehavior_Template,TemplateAIBehavior,1001718509);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBounceRoamingBehavior);

    public:

        Ray_AIBounceRoamingBehavior_Template();
        ~Ray_AIBounceRoamingBehavior_Template();

        ITF_INLINE const class AIJumpInDirAction_Template* getBounceMove() const { return m_bounceMove; }
        ITF_INLINE const class AIJumpInDirAction_Template* getBounceIdle() const { return m_bounceIdle; }
        ITF_INLINE const class AIJumpAction_Template* getBounceJump() const { return m_bounceJump; }
        ITF_INLINE f32 getMinTimeToMove() const { return m_minTimeToMove; }
        ITF_INLINE f32 getMaxTimeToMove() const { return m_maxTimeToMove; }
        ITF_INLINE f32 getMinTimeToIdle() const { return m_minTimeToIdle; }
        ITF_INLINE f32 getMaxTimeToIdle() const { return m_maxTimeToIdle; }
        ITF_INLINE f32 getObstacleDetectionDistance() const { return m_obstacleDetectionDistance; }
        ITF_INLINE f32 getWallJumpHeight() const { return m_wallJumpHeight; }

    private:
        AIJumpInDirAction_Template*                  m_bounceMove;
        AIJumpInDirAction_Template*                  m_bounceIdle;
        AIJumpAction_Template*                       m_bounceJump;
        f32                                 m_minTimeToMove;
        f32                                 m_maxTimeToMove;
        f32                                 m_minTimeToIdle;
        f32                                 m_maxTimeToIdle;
        f32                                 m_obstacleDetectionDistance;
        f32                                 m_wallJumpHeight;
    };

    ITF_INLINE const class Ray_AIBounceRoamingBehavior_Template * Ray_AIBounceRoamingBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIBounceRoamingBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_

