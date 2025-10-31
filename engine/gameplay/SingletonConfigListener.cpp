#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONCONFIGLISTENER_H_
#include "engine/gameplay/SingletonConfigListener.h"
#endif //_ITF_SINGLETONCONFIGLISTENER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

//------------------------------------------------------------------------------
SingletonConfigListener::SingletonConfigListener()
    : WatchDirectoryListener()
{
}

//------------------------------------------------------------------------------
SingletonConfigListener::~SingletonConfigListener()
{
}

//------------------------------------------------------------------------------
void SingletonConfigListener::onModifiedFile(const String& _absoluteFilename)
{
    EventSingletonConfigChanged configChanged(_absoluteFilename);
    EVENTMANAGER->broadcastEvent(&configChanged);
}


} // namespace ITF


