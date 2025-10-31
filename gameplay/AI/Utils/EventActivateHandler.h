#ifndef _ITF_EVENTACTIVATEHANDLER_H_
#define _ITF_EVENTACTIVATEHANDLER_H_

namespace ITF
{

class EventDelayHandler;
class LinkComponent;

//------------------------------------------------------------------------------
class EventActivateHandler
{
public:
    EventActivateHandler()
        : m_deactivationDone(bfalse)
        , m_activationDone(bfalse)
    {}

    void onEvent(Event* _event);
    void deactivateChildren(LinkComponent* _linkComponent);
    void activateChildrenIfNeeded(LinkComponent* _linkComponent);

private:
    bbool m_deactivationDone;
    bbool m_activationDone;
};

} // namespace ITF

#endif //_ITF_EVENTACTIVATEHANDLER_H_
