// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ACTOREDITOR_H_
    #include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF
{
    void ActorEditor::unregisterAllListeners()
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            m_listeners[i].m_listener->destroy();
            SF_DEL(m_listeners[i].m_listener);
        }
        m_listeners.clear();
    }

    void ActorEditor::registerListener(Interface_ActorEditorListener* _listener, StringID::StringIdValueType _classCRC, bbool _isExternal)
    {
        for (u32 i = 0 ; i < m_listeners.size(); i++)
        {
            if (m_listeners[i].m_listener == _listener)
            {
                i32 index = m_listeners[i].m_listenedComponents.find(_classCRC);
                if (index < 0)
                {
                    m_listeners[i].m_listenedComponents.push_back(_classCRC);
                }
                return;
            }
        }

        ActorEdListener l;
        l.m_listener = _listener;
        l.m_isExternal = _isExternal;
        l.m_listenedComponents.push_back(_classCRC);
        m_listeners.push_back(l);
    }


} // namespace ITF
