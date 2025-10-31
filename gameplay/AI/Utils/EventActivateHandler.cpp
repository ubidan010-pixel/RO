#include "precompiled_gameplay.h"

#ifndef _ITF_EVENTACTIVATEHANDLER_H_
#include "gameplay/AI/Utils/EventActivateHandler.h"
#endif //_ITF_EVENTACTIVATEHANDLER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{

//------------------------------------------------------------------------------
void EventActivateHandler::onEvent(Event* _event)
{
    // activate/deactivate on link
    if ( EventOnLink* linkEvent = _event->DynamicCast<EventOnLink>(ITF_GET_STRINGID_CRC(EventOnLink,1558255516)) )
    {
        Actor* child = AIUtils::getActor(linkEvent->getChild());
        if (child)
        {
            EventActivate eventActivate;
            eventActivate.setActivated(!linkEvent->getLinked());
            child->onEvent(&eventActivate);
        }
    }
}

//------------------------------------------------------------------------------
void EventActivateHandler::deactivateChildren(LinkComponent* _linkComponent)
{
    // should be done only once (on scene load or reload), hence the assert
    ITF_ASSERT(!m_deactivationDone);
    if (!m_deactivationDone)
    {
        m_deactivationDone = btrue;

        // deactivate children (immediately)
        EventActivate deactivate;
        deactivate.setActivated(bfalse);
        _linkComponent->sendEventToChildren(&deactivate);
    }
}

//------------------------------------------------------------------------------
void EventActivateHandler::activateChildrenIfNeeded(LinkComponent* _linkComponent)
{
    if (!m_activationDone)
    {
        m_activationDone = btrue;

        // activate children (with delay)
        EventActivate activate;
        activate.setActivated(btrue);
        EVENTDELAY_HANDLER->sendEventToChildren(_linkComponent, &activate);
    }
}

} // namespace ITF
