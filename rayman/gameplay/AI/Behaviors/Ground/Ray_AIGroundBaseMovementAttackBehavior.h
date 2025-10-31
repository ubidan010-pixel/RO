#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class Ray_AIGroundBaseMovementAttackBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseMovementAttackBehavior,Ray_AIGroundBaseMovementBehavior,4287255385);

    public:

        Ray_AIGroundBaseMovementAttackBehavior();
        virtual ~Ray_AIGroundBaseMovementAttackBehavior();

        ActorRef            getTarget() const { return m_targetRef; }
        void                setTarget(ActorRef _val) { m_targetRef = _val; }

        virtual bbool               canBeInterrupted() const { return btrue; }

        virtual void                HACK_setDarktoonifyAction(){}


    protected:
        ITF_INLINE const class Ray_AIGroundBaseMovementAttackBehavior_Template * getTemplate() const;

        ActorRef                    m_targetRef;

    };

    class Ray_AIGroundBaseMovementAttackBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIGroundBaseMovementAttackBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,3798905523);
        DECLARE_SERIALIZE();

    public:

        Ray_AIGroundBaseMovementAttackBehavior_Template();
        ~Ray_AIGroundBaseMovementAttackBehavior_Template();

    private:
    };

    ITF_INLINE const class Ray_AIGroundBaseMovementAttackBehavior_Template * Ray_AIGroundBaseMovementAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundBaseMovementAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_