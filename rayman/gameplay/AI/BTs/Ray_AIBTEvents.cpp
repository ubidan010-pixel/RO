#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_EventAIOrderBT)
BEGIN_SERIALIZATION_CHILD(Ray_EventAIOrderBT)

    SERIALIZE_RAY_BTAIORDERTYPE("type",m_type);

END_SERIALIZATION()


IMPLEMENT_OBJECT_RTTI(Ray_EventAIBTSetDetectionArea)
BEGIN_SERIALIZATION_CHILD(Ray_EventAIBTSetDetectionArea)
END_SERIALIZATION()


IMPLEMENT_OBJECT_RTTI(Ray_EventDisableAIOrderBT)
BEGIN_SERIALIZATION_CHILD(Ray_EventDisableAIOrderBT)

    SERIALIZE_MEMBER("disable",m_disable);

END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(Ray_EventAINetworkWaypoint)
BEGIN_SERIALIZATION_CHILD(Ray_EventAINetworkWaypoint)
END_SERIALIZATION()

}