#include "precompiled_core.h"

#ifdef ITF_TRACKING_MEMORY_ENABLE

#ifndef ITF_MEMORYEVENT_H_
#include "core/memory/memoryEvent.h"
#endif //ITF_MEMORYEVENT_H_

#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_


namespace ITF
{
    u32 memoryEvent::m_TaggerEvent = 0; 
    u64 memoryEvent::m_CurrentContextTag = 0;


    memoryEventScope::~memoryEventScope()
    {
        u64 currentContext = memoryEvent::m_CurrentContextTag;
        u64 myTag = m_Event->m_Tag;
        u64 mask = ~ (1<<(myTag-1));
        memoryEvent::m_CurrentContextTag = memoryEvent::m_CurrentContextTag & mask;

        threadAllocatorTracker::getInstance()->popBackEvent(m_Event,currentContext);
    }


    memoryEventScope::memoryEventScope(memoryEvent* _memEvent)
    {
        m_Event = _memEvent;

        u64 myTag = m_Event-> m_Tag;
        u64 mask =  (1ull<<(myTag-1));
        memoryEvent::m_CurrentContextTag = memoryEvent::m_CurrentContextTag | mask;
        //LOG("currentContextTag:%d",m_CurrentContextTag);

        //active the context or make a new one
        threadAllocatorTracker::getInstance()->pushEvent(_memEvent);
    }

}
#endif //ITF_TRACKING_MEMORY_ENABLE
