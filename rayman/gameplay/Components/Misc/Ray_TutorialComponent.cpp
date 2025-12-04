#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TUTORIALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TutorialComponent.h"
#endif //_ITF_RAY_TUTORIALCOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif //_ITF_RAY_PERSISTENTGAMEDATA_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_TutorialComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TutorialComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_TutorialComponent)
    VALIDATE_COMPONENT_PARAM("lineId", getTemplate()->getLineId()!=U32_INVALID, "Line ID mandatory");
    VALIDATE_COMPONENT_PARAM("", m_playerDetector, "PlayerDetectorComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_TutorialComponent::Ray_TutorialComponent()
    : Super()
    , m_active(bfalse)
    , m_wasShown(bfalse)
    , m_playerDetector(NULL)
    , m_wasVisible(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_TutorialComponent::~Ray_TutorialComponent()
{
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( m_wasVisible )
    {
        hide();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_wasVisible )
    {
        hide();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_playerDetector = m_actor->GetComponent<PlayerDetectorComponent>();

    // starts active if required
    m_active = getTemplate()->getStartsActive();

    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

    // register sprint tutorial
    if (getTemplate()->isSprintTutorial())
    {
        RAY_GAMEMANAGER->registerSprintTutorial();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    // do nothing if inactive
    if (!m_active)
    {
        hide();
        disableSprintTutorialIfNeeded();
        return;
    }

    // disable sprint tutorial when displayed once
    if (getTemplate()->isSprintTutorial() && isSprintTutorialDisabled())
    {
        hide();
        return;
    }

    // disable tutorials when a ("locking") sequence is playing (RO-10460)
    if (CINEMATICMANAGER->getLockPlayers())
    {
        hide();
        return;
    }

    // show/hide tutorial depending on players inside
    if (m_playerDetector->getActorsInside().size() > 0)
    {
        show();
    }
    else
    {
        hide();
        disableSprintTutorialIfNeeded();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        m_active = triggerEvt->getActivated();
    }
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::show()
{
    // Show tutorial with action-based lineId (icons auto-remap based on current controller)
    RAY_GAMEMANAGER->showTutorialText(getTemplate()->getLineId());

    // remember we were shown once
    m_wasShown = btrue;
    m_wasVisible = btrue;
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::hide()
{
    if ( m_wasVisible )
    {
        RAY_GAMEMANAGER->hideTutorialText();
    }

    m_wasVisible = bfalse;
}

//------------------------------------------------------------------------------
bbool Ray_TutorialComponent::isSprintTutorialDisabled() const
{
    ITF_ASSERT(getTemplate()->isSprintTutorial());

    ITF_ASSERT(RAY_GAMEMANAGER);
    if (!RAY_GAMEMANAGER)
    {
        return bfalse;
    }

    Ray_PersistentGameData_Universe* pgd = RAY_GAMEMANAGER->getPersistentGameData();
    ITF_ASSERT(pgd);
    if (!pgd)
    {
        return bfalse;
    }

    return pgd->getSprintTutorialDisabled();
}

//------------------------------------------------------------------------------
void Ray_TutorialComponent::disableSprintTutorialIfNeeded()
{
    // not shown yet, skip
    if (!m_wasShown)
    {
        return;
    }

    // not a sprint tutorial, skip
    if (!getTemplate()->isSprintTutorial())
    {
        return;
    }

    ITF_ASSERT(RAY_GAMEMANAGER);
    if (!RAY_GAMEMANAGER)
    {
        return;
    }

    Ray_PersistentGameData_Universe* pgd = RAY_GAMEMANAGER->getPersistentGameData();
    ITF_ASSERT(pgd);
    if (!pgd)
    {
        return;
    }

    pgd->setSprintTutorialDisabled(btrue);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_TutorialComponent_Template)
BEGIN_SERIALIZATION(Ray_TutorialComponent_Template)
    SERIALIZE_MEMBER("startsActive", m_startsActive);
    SERIALIZE_MEMBER("isSprintTutorial", m_isSprintTutorial);
    SERIALIZE_MEMBER("lineId", m_lineId);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_TutorialComponent_Template::Ray_TutorialComponent_Template()
: Super()
, m_startsActive(bfalse)
, m_isSprintTutorial(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_TutorialComponent_Template::~Ray_TutorialComponent_Template()
{
}

} // namespace ITF
