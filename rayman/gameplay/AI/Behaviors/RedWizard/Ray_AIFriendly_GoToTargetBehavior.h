#ifndef _ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_
#define _ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

namespace ITF
{
    class Ray_AIFriendly_GoToTargetBehavior : public Ray_AIGroundRoamBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_GoToTargetBehavior, Ray_AIGroundRoamBehavior,1803666002)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIFriendly_GoToTargetBehavior();
        virtual ~Ray_AIFriendly_GoToTargetBehavior();

        virtual void            setData( Event* _event );
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _dt );

    private:
        ITF_INLINE const class Ray_AIFriendly_GoToTargetBehavior_Template* getTemplate() const;

        bbool                   checkReachedTarget( const Vec2d& _target ) const;

        ActorRef            m_target;               // the object we're currently going to
        f32                 m_targetReachedRadius;  // how close we wanna get to it before we consider it reached
        //StringID            m_targetBone;           // a bone on the object we're going on
        //bbool               m_targetFlipped;
    };

    class Ray_AIFriendly_GoToTargetBehavior_Template : public Ray_AIGroundRoamBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_GoToTargetBehavior_Template,Ray_AIGroundRoamBehavior_Template,3839109425);
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFriendly_GoToTargetBehavior);
        DECLARE_SERIALIZE();

    public:

        Ray_AIFriendly_GoToTargetBehavior_Template();
        ~Ray_AIFriendly_GoToTargetBehavior_Template();

    private:

    };

    ITF_INLINE const Ray_AIFriendly_GoToTargetBehavior_Template* Ray_AIFriendly_GoToTargetBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIFriendly_GoToTargetBehavior_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_