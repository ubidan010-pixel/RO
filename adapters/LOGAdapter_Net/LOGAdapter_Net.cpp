// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "enginecommon.h"
#endif // _ENGINECOMMON_H_
//

#include "adapters/LOGAdapter_Net/LOGAdapter_Net.h"
#ifndef _ITF_UDPADAPTER_H_
    #include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif // _ITF_UDPADAPTER_H_
#ifndef _ITF_SINGLETONS_H_
    #include "engine/core/Singletons.h"
#endif // _ITF_SINGLETONS_H_


namespace ITF
{

void LOGAdapter_Net::init()
{
    ITF_ASSERT(UDP_ADAPTER != NULL);
}

void LOGAdapter_Net::close()
{
}

void LOGAdapter_Net::msg( const String& _msg  )
{
    UDP_ADAPTER->sendPacketToPeer(UDPAdapter::PeerType_LogClient, _msg.cStr(), _msg.getUnicodeLength());
}

} // namespace ITF

