#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterCloseAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_

#ifndef _ITF_AIPERFORMHITACTION_H_
#include "gameplay/AI/Actions/AIPerformHitAction.h"
#endif //_ITF_AIPERFORMHITACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterCloseAttackBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIShooterCloseAttackBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    Ray_AIShooterCloseAttackBehavior_Template::Ray_AIShooterCloseAttackBehavior_Template()
    : m_attack(NULL)
    {
    }

    Ray_AIShooterCloseAttackBehavior_Template::~Ray_AIShooterCloseAttackBehavior_Template()
    {
        SF_DEL(m_attack);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterCloseAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterCloseAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("attack", m_attack, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()


    Ray_AIShooterCloseAttackBehavior::Ray_AIShooterCloseAttackBehavior()
        : Super()
        , m_attack(NULL)
        , m_victim(ITF_INVALID_OBJREF)
    {
    }

    Ray_AIShooterCloseAttackBehavior::~Ray_AIShooterCloseAttackBehavior()
    {
    }

    void Ray_AIShooterCloseAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_attack = static_cast<AIPerformHitAction*>(createAiAction(getTemplate()->getAttack()));

        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIShooterCloseAttackBehavior::onActivate()
    {
        ITF_ASSERT_MSG(m_victim != ITF_INVALID_OBJREF, "no victim!");
        m_attack->setData(m_victim);
        m_animComponent->resetCurTime();    // TODO: should this go in setAction?
        setAction(m_attack,btrue);
    }

    void Ray_AIShooterCloseAttackBehavior::onDeactivate()
    {
        m_victim = ITF_INVALID_OBJREF;
    }

    void Ray_AIShooterCloseAttackBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
    }

    void Ray_AIShooterCloseAttackBehavior::onActionFinished()
    {
        setNullAction();
        m_aiComponent->onBehaviorFinished();
    }


}
