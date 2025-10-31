#ifndef _ITF_AIDEATHBEHAVIOR_H_
#define _ITF_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AIPLAYACTIONSBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIPlayActionsBehavior.h"
#endif //_ITF_AIPLAYACTIONSBEHAVIOR_H_

namespace ITF
{
    class AIDeathBehavior : public AIPlayActionsBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDeathBehavior, AIPlayActionsBehavior,3197791561);

    public:

        AIDeathBehavior();
        virtual ~AIDeathBehavior();

        virtual void            onActivate();
        virtual void            onActionFinished();

    private:
        ITF_INLINE const class AIDeathBehavior_Template * getTemplate() const;

    };

    class AIDeathBehavior_Template : public AIPlayActionsBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDeathBehavior_Template,AIPlayActionsBehavior_Template,2518331708);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(AIDeathBehavior);

    public:

        AIDeathBehavior_Template();
        ~AIDeathBehavior_Template(){}

        ITF_INLINE bbool getPauseComponentWhenDone() const { return m_pauseComponentWhenDone; }
        ITF_INLINE bbool getPauseActorWhenDone() const { return m_pauseActorWhenDone; }
        ITF_INLINE bbool getDestroyActorWhenDone() const { return m_destroyActorWhenDone; }
        ITF_INLINE bbool getDeactivatePhysics() const { return m_deactivatePhysics; }
        ITF_INLINE bbool getNullWeight() const { return m_nullWeight; }

    private:
        bbool                   m_pauseComponentWhenDone;
        bbool                   m_pauseActorWhenDone;
        bbool                   m_destroyActorWhenDone;
        bbool                   m_deactivatePhysics;
        bbool                   m_nullWeight;
    };

    ITF_INLINE const class AIDeathBehavior_Template * AIDeathBehavior::getTemplate() const
    {
        return static_cast<const class AIDeathBehavior_Template *>(m_template);
    }

}


#endif // _ITF_AIDEATHBEHAVIOR_H_