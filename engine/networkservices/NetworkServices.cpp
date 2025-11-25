#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifdef ITF_SUPPORT_NETWORKSERVICES

namespace ITF
{
    NetworkServices::NetworkServices()
        : m_ipAddress()
        , m_networkStatus(ENetworkStatus_Preparing)
    {
    }

    NetworkServices::~NetworkServices()
    {
    }

    void NetworkServices::setNetworkStatus(ENetworkStatus _networkStatus)
    {
        if (_networkStatus != m_networkStatus)
        {
            m_networkStatus = _networkStatus;

            switch (_networkStatus)
            {
            case ENetworkStatus_Preparing:
                LOG("[NetworkServices] preparing");
                break;

            case ENetworkStatus_Ready:
                LOG("[NetworkServices] ready");
                break;

            case ENetworkStatus_Error:
                LOG("[NetworkServices] error: %u", getPlatformSpecificErrorCode());
                break;
            }
        }
    }
} // namespace ITF
#endif

