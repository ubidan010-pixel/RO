#ifndef _ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_
#define _ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class AIJumpToTargetAction;
    class AIJumpToTargetAction_Template;

    class Ray_AIFriendly_JumpToTargetBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_JumpToTargetBehavior, Ray_AIGroundBaseBehavior,625134645)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:
        Ray_AIFriendly_JumpToTargetBehavior();
        virtual ~Ray_AIFriendly_JumpToTargetBehavior();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        setData( Event* _event );
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        onActionFinished();

    private:
        ITF_INLINE const class Ray_AIFriendly_JumpToTargetBehavior_Template* getTemplate() const;

        AIJumpToTargetAction* m_jump;

        ActorRef            m_target;

        void                startJump();
    };

    class Ray_AIFriendly_JumpToTargetBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_JumpToTargetBehavior_Template, Ray_AIGroundBaseBehavior_Template,4048975673);
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFriendly_JumpToTargetBehavior);
        DECLARE_SERIALIZE();

    public:

        Ray_AIFriendly_JumpToTargetBehavior_Template();
        ~Ray_AIFriendly_JumpToTargetBehavior_Template();

        const AIJumpToTargetAction_Template* getJump() const { return m_jump; }

    private:

        AIJumpToTargetAction_Template*    m_jump;
    };

    ITF_INLINE const Ray_AIFriendly_JumpToTargetBehavior_Template* Ray_AIFriendly_JumpToTargetBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIFriendly_JumpToTargetBehavior_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_