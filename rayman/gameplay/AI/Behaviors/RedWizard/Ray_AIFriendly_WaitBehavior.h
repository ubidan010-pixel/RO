#ifndef _ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_
#define _ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIFriendly_WaitBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_WaitBehavior, AIBehavior,358517205)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:
        Ray_AIFriendly_WaitBehavior();
        virtual ~Ray_AIFriendly_WaitBehavior();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        setData( Event* _event );
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _dt );
        virtual void        onEvent( Event* _event );
        virtual void        onActionFinished();

    protected:
        ITF_INLINE const class Ray_AIFriendly_WaitBehavior_Template* getTemplate() const;

        void                setRandomActionCounter();

        AIAction*           m_idle;
        AIAction*           m_randomAction;

        ActorRef            m_target;
        Ray_EventAIOrder::Type m_orderType;
        f32                 m_playerDetectRadius;
        f32                 m_randomActionCounter;

        void                triggerTarget();
    };

    class Ray_AIFriendly_WaitBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFriendly_WaitBehavior_Template, TemplateAIBehavior,4187183285);
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFriendly_WaitBehavior);
        DECLARE_SERIALIZE();

    public:

        Ray_AIFriendly_WaitBehavior_Template();
        ~Ray_AIFriendly_WaitBehavior_Template();

        const AIAction_Template*    getIdle() const { return m_idle; }
        const AIAction_Template*    getRandomAction() const { return m_randomAction; }
        ITF_INLINE bbool            getTriggerTarget() const { return m_triggerTarget; }
        f32                         getRandomActionDelayMin() const { return m_randomActionDelayMin; }
        f32                         getRandomActionDelayMax() const { return m_randomActionDelayMax; }

    private:

        AIAction_Template*          m_idle;
        AIAction_Template*          m_randomAction;
        f32                         m_randomActionDelayMin;
        f32                         m_randomActionDelayMax;
        bbool                       m_triggerTarget;
    };

    ITF_INLINE const Ray_AIFriendly_WaitBehavior_Template* Ray_AIFriendly_WaitBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIFriendly_WaitBehavior_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_