#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonLaunchBehavior.h"
#endif //_ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_

#ifndef _ITF_AIBALLISTICSACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsAction.h"
#endif //_ITF_AIBALLISTICSACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonLaunchBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIDarktoonLaunchBehavior)
    VALIDATE_BEHAVIOR_PARAM("ballistics", getTemplate()->getBallistics(), "AIBallisticsAction mandatory");
    VALIDATE_BEHAVIOR_PARAM("landing", getTemplate()->getLanding(), "AIAction mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIDarktoonLaunchBehavior::Ray_AIDarktoonLaunchBehavior()
    : Super()
    , m_started(bfalse)
    , m_origin(Vec3d::Zero)
    , m_target(Vec3d::Zero)
    , m_ballistics(NULL)
    , m_landing(NULL)
    , m_phantomWasDisabled(bfalse)
    , m_phantomComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIDarktoonLaunchBehavior::~Ray_AIDarktoonLaunchBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonLaunchBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // create ballistics action from template
    m_ballistics = createAiActionAndCast<AIBallisticsAction>(getTemplate()->getBallistics());
    m_landing = createAiAction(getTemplate()->getLanding());
    
    // get phantom component
    m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonLaunchBehavior::onActivate()
{
    // disable phantom: launched darktoon are completely invincible and harmless
    if (m_phantomComponent)
    {
        m_phantomWasDisabled = m_phantomComponent->getDisabled();
        m_phantomComponent->setDisabled(btrue);
    }
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonLaunchBehavior::onDeactivate()
{
    // restore phantom
    if (m_phantomComponent)
    {
        m_phantomComponent->setDisabled(m_phantomWasDisabled);
        m_phantomWasDisabled = bfalse;
    }
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonLaunchBehavior::update(f32 _dt)
{
    Super::update(_dt);

    // actually start ballistics action
    // CAVEAT[Laurent]: we can't start this right upon receiving the "launch
    // triggered" event, it would set the actor's position before it's actually
    // enabled, which just breaks it (actor disappears, no longer updated, no
    // longer in the scene...)
    if (!m_started)
    {
        m_actor->setPos(m_origin);
        m_ballistics->setTarget(m_target);
        ITF_ASSERT(m_ballistics && !m_currentAction);
        setAction(m_ballistics);
        m_started = btrue;
    }
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonLaunchBehavior::onActionFinished()
{
    Super::onActionFinished();

    // ballistics -> landing
    if (m_currentAction == m_ballistics)
    {
        setAction(m_landing);
    }
    // landing -> done
    else if (m_currentAction == m_landing)
    {
        m_aiComponent->onBehaviorFinished();
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonLaunchBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIDarktoonLaunchBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("ballistics", m_ballistics, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("landing", m_landing, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIDarktoonLaunchBehavior_Template::Ray_AIDarktoonLaunchBehavior_Template()
    : Super()
    , m_ballistics(NULL)
    , m_landing(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIDarktoonLaunchBehavior_Template::~Ray_AIDarktoonLaunchBehavior_Template()
{
    SF_DEL(m_ballistics);
    SF_DEL(m_landing);
}

} // namespace ITF
