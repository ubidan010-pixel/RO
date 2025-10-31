#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneDrowningBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneDrowningBehavior_Template)

BEGIN_SERIALIZATION_CHILD(Ray_AILivingStoneDrowningBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("dive", m_diveAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("drown", m_drownAction, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

Ray_AILivingStoneDrowningBehavior_Template::Ray_AILivingStoneDrowningBehavior_Template()
: m_diveAction(NULL)
, m_drownAction(NULL)
{

}

Ray_AILivingStoneDrowningBehavior_Template::~Ray_AILivingStoneDrowningBehavior_Template()
{
    SF_DEL(m_diveAction);
    SF_DEL(m_drownAction);

}
IMPLEMENT_OBJECT_RTTI(Ray_AILivingStoneDrowningBehavior)

Ray_AILivingStoneDrowningBehavior::Ray_AILivingStoneDrowningBehavior()
: Super()
, m_diveAction(NULL)
, m_drownAction(NULL)
{
}

Ray_AILivingStoneDrowningBehavior::~Ray_AILivingStoneDrowningBehavior()
{
}

void Ray_AILivingStoneDrowningBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_diveAction = createAiAction(getTemplate()->getDiveAction());
    m_drownAction = createAiAction(getTemplate()->getDrownAction());

    Super::onActorLoaded(_hotReload);
}

void Ray_AILivingStoneDrowningBehavior::update( f32 _delta  )
{
    Super::update(_delta);
 }

void Ray_AILivingStoneDrowningBehavior::onActivate()
{
    Super::onActivate();
    setAction(m_diveAction);
}

void Ray_AILivingStoneDrowningBehavior::onDeactivate()
{
    Super::onDeactivate();

}


void Ray_AILivingStoneDrowningBehavior::onActionFinished()
{
    Super::onActionFinished();
    if (m_currentAction == m_diveAction)
        setAction(m_drownAction);
    else 
    {
        m_aiComponent->setHealth(0);
        m_aiComponent->onBehaviorFinished();
    }
}

}