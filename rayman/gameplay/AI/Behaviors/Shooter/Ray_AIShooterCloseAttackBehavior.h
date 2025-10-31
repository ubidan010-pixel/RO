#ifndef _ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_
#define _ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIPerformHitAction;

    class Ray_AIShooterCloseAttackBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterCloseAttackBehavior, AIBehavior,1863574358)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIShooterCloseAttackBehavior();
        virtual ~Ray_AIShooterCloseAttackBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        virtual void            onActionFinished();
        void                    setVictim( ObjectRef _victim ) { m_victim = _victim; }
        ObjectRef               getVictim() const { return m_victim; }

    private:
        ITF_INLINE const class Ray_AIShooterCloseAttackBehavior_Template * getTemplate() const;

        AIPerformHitAction*     m_attack;
        ObjectRef               m_victim;
    };

    class Ray_AIShooterCloseAttackBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterCloseAttackBehavior_Template,TemplateAIBehavior,1467164194);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterCloseAttackBehavior);

    public:

        Ray_AIShooterCloseAttackBehavior_Template();
        ~Ray_AIShooterCloseAttackBehavior_Template();

        const class AIPerformHitAction_Template*     getAttack() const { return m_attack; }

    private:

        class AIPerformHitAction_Template*     m_attack;
    };

    ITF_INLINE const class Ray_AIShooterCloseAttackBehavior_Template * Ray_AIShooterCloseAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterCloseAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_