#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_WIN_H_
#include "engine/NetworkServices/Win/NetworkServices_Win.h"
#endif //_ITF_NETWORKSERVICES_WIN_H_

#include "core/Config.h"

namespace ITF
{
    NetworkServices_Win::NetworkServices_Win()
        : NetworkServices()
        , m_platformErrorCode(0)
    {
    }

    NetworkServices_Win::~NetworkServices_Win()
    {
    }

    u32 NetworkServices_Win::update()
    {
        return 0;
    }

    void NetworkServices_Win::initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes)
    {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        m_platformErrorCode = WSAStartup(wVersionRequested, &wsaData);
        if (m_platformErrorCode == 0)
        {
            if (CONFIG->m_enableNetwork == bfalse)
            {
                setNetworkStatus(ENetworkStatus_Preparing);
            }
            else
            {
                setNetworkStatus(ENetworkStatus_Ready);
            }
        }
        else
        {
            setNetworkStatus(ENetworkStatus_Error);
        }
    }

    u32 NetworkServices_Win::getPlatformSpecificErrorCode()
    {
        return m_platformErrorCode;
    }

    void NetworkServices_Win::terminate()
    {
        WSACleanup();
    }

} // namespace ITF

