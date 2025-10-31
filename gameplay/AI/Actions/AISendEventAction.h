#ifndef _ITF_AISENDEVENTACTION_H_
#define _ITF_AISENDEVENTACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AISendEventAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AISendEventAction, AIAction,2776233176);

    public:

        AISendEventAction();
        virtual ~AISendEventAction();

        virtual void        onActivate();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:
        Event *             m_eventToSend;
        ITF_INLINE const class AISendEventAction_Template * getTemplate() const;
    };

    class AISendEventAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AISendEventAction_Template,AIAction_Template,4032035978);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AISendEventAction);

    public:

        AISendEventAction_Template();
        ~AISendEventAction_Template();

        ITF_INLINE const Event *    getEventToSend() const          { return m_eventToSend; }
        ITF_INLINE const bbool      getTriggerChildren() const      { return m_triggerChildren; }
        ITF_INLINE const bbool      getTriggerSelf() const          { return m_triggerSelf; }
        ITF_INLINE const bbool      getBroadcast() const            { return m_broadcast; }
        ITF_INLINE const bbool      getTriggerBinded() const        { return m_triggerBinded; }
        ITF_INLINE const StringID & getVirtualChild() const         { return m_virtualChild; }

    private:
        Event *             m_eventToSend;
        bbool               m_triggerChildren;
        bbool               m_triggerSelf;
        bbool               m_broadcast;
        bbool               m_triggerBinded;
        StringID            m_virtualChild;
    };

    const class AISendEventAction_Template * AISendEventAction::getTemplate() const
    {
        return static_cast<const class AISendEventAction_Template *>(m_template);
    }
}

#endif // _ITF_AISENDEVENTACTION_H_

