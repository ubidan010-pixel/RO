

#ifndef _ITF_RAY_AIBTEVENTS_H_
#define _ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{

class Ray_EventAIOrderBT : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAIOrderBT, Event,2497541985)
    DECLARE_SERIALIZE()

public:
    Ray_EventAIOrderBT() : m_type(BTAIORDER_UNKNOWN) {}
    Ray_EventAIOrderBT( ObjectRef _sender, BTAIOrderType _type ) : Super(_sender), m_type(_type) {}
    ~Ray_EventAIOrderBT() {}

    BTAIOrderType           getType() const { return m_type; }
    void                    setType( BTAIOrderType _type ) { m_type = _type; }

private:

    BTAIOrderType           m_type;
};

class Ray_EventAIBTSetDetectionArea : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAIBTSetDetectionArea, Event,208226879)
    DECLARE_SERIALIZE()

public:
    Ray_EventAIBTSetDetectionArea() : m_area(ITF_INVALID_OBJREF), m_set(bfalse) {}
    ~Ray_EventAIBTSetDetectionArea() {}

    ActorRef                getArea() const { return m_area; }
    void                    setArea( ActorRef _ref ) { m_area = _ref; }

    const StringID&         getId() const { return m_id; }
    void                    setId( const StringID& _id ) { m_id = _id; }

    bbool                   getIsSet() const { return m_set; }
    void                    setIsSet( bbool _set ) { m_set = _set; }

private:

    ActorRef                m_area;
    StringID                m_id;
    bbool                   m_set;
};

class Ray_EventDisableAIOrderBT : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventDisableAIOrderBT, Event,2752846623)
    DECLARE_SERIALIZE()

public:
    Ray_EventDisableAIOrderBT() : m_disable(btrue) {}
    Ray_EventDisableAIOrderBT( ObjectRef _sender, bbool _disable ) : Super(_sender), m_disable(_disable) {}
    ~Ray_EventDisableAIOrderBT() {}

    bbool                   getDisable() const { return m_disable; }

private:

    bbool                   m_disable;
};

class Ray_EventAINetworkWaypoint : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAINetworkWaypoint, Event,3032038608)
    DECLARE_SERIALIZE()

public:
    Ray_EventAINetworkWaypoint() {}
    ~Ray_EventAINetworkWaypoint() {}

private:

};

}

#endif // _ITF_RAY_AIBTEVENTS_H_

