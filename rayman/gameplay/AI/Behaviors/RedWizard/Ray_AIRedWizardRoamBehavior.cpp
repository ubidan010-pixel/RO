#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardRoamBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardRoamBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIRedWizardRoamBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("greet", m_greet, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("greetRange", m_greetRange);
        SERIALIZE_MEMBER("greetCooldown", m_greetCooldown);
    END_SERIALIZATION()

    Ray_AIRedWizardRoamBehavior_Template::Ray_AIRedWizardRoamBehavior_Template()
    : m_greet(NULL)
    , m_greetRange(2.f)
    , m_greetCooldown(1.f)
    {
    }

    Ray_AIRedWizardRoamBehavior_Template::~Ray_AIRedWizardRoamBehavior_Template()
    {
        SF_DEL(m_greet);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardRoamBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIRedWizardRoamBehavior)
        VALIDATE_BEHAVIOR_PARAM("greet", m_greet, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AIRedWizardRoamBehavior::Ray_AIRedWizardRoamBehavior()
        : Super()
        , m_greet(NULL)
        , m_greetTimer(0.f)
    {
    }

    Ray_AIRedWizardRoamBehavior::~Ray_AIRedWizardRoamBehavior()
    {
    }

    void Ray_AIRedWizardRoamBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_greet = createAiAction(getTemplate()->getGreet());

        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIRedWizardRoamBehavior::onActivate()
    {
        Super::onActivate();

        m_greetTimer = getTemplate()->getGreetCooldown();
    }

    void Ray_AIRedWizardRoamBehavior::onActionFinished()
    {
        if ( m_currentAction == m_greet )
        {
            onActionFinishedGreet();
        }
        else
        {
            Super::onActionFinished();
        }
    }

    bbool Ray_AIRedWizardRoamBehavior::updateMove( f32 _dt )
    {
        m_greetTimer += _dt;


        if ( !Super::updateMove(_dt) )
        {
            return bfalse;
        }

        if (checkGreet())
        {
            return bfalse;
        }


        return btrue;
    }

    void Ray_AIRedWizardRoamBehavior::startGreet()
    {
        setAction(m_greet);
    }

    void Ray_AIRedWizardRoamBehavior::onActionFinishedGreet()
    {
        // resume walking in the same dir
        startMove(m_actor->isFlipped());
        m_greetTimer = 0;
    }

    // is there a friendly nearby that we can say hello to?
    bbool Ray_AIRedWizardRoamBehavior::checkGreet()
    {
        if ( m_currentAction == m_move &&
             m_greetTimer > getTemplate()->getGreetCooldown() )
        {
            bbool hasFriendlyInFront = AIUtils::hasActorInRange(
                INTERACTION_ALLY,
                m_actor, m_actor->isFlipped(),
                getTemplate()->getGreetRange(), bfalse, btrue);

            if (hasFriendlyInFront)
            {
                startGreet();
                return btrue;
            }
        }

        return bfalse;
    }

}