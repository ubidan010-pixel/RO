#ifndef _ITF_EVENTLISTENER_H_
#define _ITF_EVENTLISTENER_H_


namespace ITF
{
    class Event; 

    class IEventListener
    {
    public:
        virtual void onEvent(Event * _event) = 0;
    };

} // namespace ITF

#endif //_ITF_EVENTLISTENER_H_