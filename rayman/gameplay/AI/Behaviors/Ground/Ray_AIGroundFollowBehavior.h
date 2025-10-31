#ifndef _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

namespace ITF
{
    class Ray_AvoidanceComponent;

    class Ray_AIGroundFollowBehavior : public Ray_AIGroundRoamBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundFollowBehavior, Ray_AIGroundRoamBehavior,4022173239);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIGroundFollowBehavior();
        virtual ~Ray_AIGroundFollowBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            onActionFinished();
        virtual void            updateAnimInputs();
        ITF_INLINE ActorRef     getTarget() const { return m_target; }
        ITF_INLINE void         setTarget(ObjectRef _val) { m_target = _val; }

    protected:

        virtual void        updateIdle( f32 _dt );
        virtual bbool       updateMove( f32 _dt );
        virtual void        startIdle();

    private:
        ITF_INLINE const class Ray_AIGroundFollowBehavior_Template* getTemplate() const;

        void                startDetect();
        void                startGiveUp();

        void                onFinishedDetect();
        void                onFinishedGiveUp();

        bbool               checkTarget( f32 _dt );
        void                avoidOtherActors();

        Ray_AvoidanceComponent* m_avoidanceComponent;
        AIAction*           m_detect;
        AIAction*           m_giveUp;

        // follow / avoidance data
        f32                 m_avoidanceTimer;
        bbool               m_neededToMove;

        ActorRef            m_target;       // the object we're currently following
        bbool               m_targetIsLeft;
    };

    class Ray_AIGroundFollowBehavior_Template : public Ray_AIGroundRoamBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundFollowBehavior_Template,Ray_AIGroundRoamBehavior_Template,1128740171);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIGroundFollowBehavior);

    public:

        Ray_AIGroundFollowBehavior_Template();
        ~Ray_AIGroundFollowBehavior_Template();

        const AIAction_Template*    getDetect() const { return m_detect; }
        const AIAction_Template*    getGiveUp() const { return m_giveUp; }
        f32                         getGiveUpDistance() const { return m_giveUpDistance; }
        f32                         getMinIdleTime() const { return m_minIdleTime; }
        f32                         getMinWalkTime() const { return m_minWalkTime; }

    private:

        AIAction_Template*  m_detect;
        AIAction_Template*  m_giveUp;
        f32                 m_giveUpDistance;
        f32                 m_minIdleTime;
        f32                 m_minWalkTime;
    };

    ITF_INLINE const class Ray_AIGroundFollowBehavior_Template * Ray_AIGroundFollowBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundFollowBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_