#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_XBOXSERIES_H_
#include "engine/NetworkServices/Xboxseries/NetworkServices_xboxseries.h"
#endif //_ITF_NETWORKSERVICES_XBOXSERIES_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#include "adapters/SystemAdapter_xboxseries/SystemAdapter_xboxseries.h"

#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef ITF_SUPPORT_NETWORKSERVICES

namespace ITF
{
	NetworkServices_xboxseries::NetworkServices_xboxseries()
        : NetworkServices()
        , m_platformErrorCode(0)
        , m_connectivityHint()
        , m_isWSAStartupInitialized(false)
    {
	}

    NetworkServices_xboxseries::~NetworkServices_xboxseries()
	{
        WSACleanup();
	}

    void NetworkServices_xboxseries::initialize( u32 dwMinUsers,u32 dwMaxUsers,bbool bRequireOnlineUsers,u32 dwSignInPanes )
    {
	}
        
    void NetworkServices_xboxseries::terminate()
    {
    }

    u32 NetworkServices_xboxseries::update()
    {
        if (!IsNetworkInitialized())
        {
            setNetworkStatus(ENetworkStatus_Error);
            return 0;
        }

        if (!m_isWSAStartupInitialized)
        {
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSADATA wsaData;
            int err = WSAStartup(wVersionRequested, &wsaData);
            if (err == 0)
            {
                m_isWSAStartupInitialized = true;
            }
            else
            {
                m_isWSAStartupInitialized = false;
                setNetworkStatus(ENetworkStatus_Error);
                return 0;
            }
        }

        // If interface changed, the new IP should replace the previous one
        static uint32_t previousIanaInterfaceType = 0;
        if (previousIanaInterfaceType != m_connectivityHint.ianaInterfaceType)
        {
            logConnectivityHint(m_connectivityHint);

            previousIanaInterfaceType = m_connectivityHint.ianaInterfaceType;
            m_ipAddress.clear();
        }

        if (m_ipAddress.isEmpty())
        {
            setIpAddress();
            if (!m_ipAddress.isEmpty())
            {
                setNetworkStatus(ENetworkStatus_Ready);
            }
            else
            {
                setNetworkStatus(ENetworkStatus_Error);
            }
        }
        else
        {
            setNetworkStatus(ENetworkStatus_Ready);
        }

        return 0;
    }

    u32 NetworkServices_xboxseries::getPlatformSpecificErrorCode()
    {
        return (u32)m_platformErrorCode;
    }

    void NetworkServices_xboxseries::logConnectivityHint(XNetworkingConnectivityHint &hint)
    {
        LOG("[NetworkServices] network initialized %u\n", hint.networkInitialized);
        LOG("[NetworkServices] network connectivity level hint %u\n", hint.connectivityLevel);
        LOG("[NetworkServices] network connectivity cost hint %u\n", hint.connectivityCost);
        LOG("[NetworkServices] network approaching data limit %u\n", hint.approachingDataLimit);
        LOG("[NetworkServices] network over data limit %u\n", hint.overDataLimit);
        LOG("[NetworkServices] device is roaming %u\n", hint.roaming);

        switch (hint.ianaInterfaceType) {
        case IF_TYPE_ETHERNET_CSMACD:
            LOG("[NetworkServices] network type is wired\n");
            break;
        case IF_TYPE_IEEE80211:
            LOG("[NetworkServices] network type is wireless\n");
            break;
        case IF_TYPE_WWANPP:
        case IF_TYPE_WWANPP2:
            LOG("[NetworkServices] network type is broadband\n");
            break;
        default:
            LOG("[NetworkServices] network type is unusually esoteric %u\n", hint.connectivityLevel);
            break;
        }
    }

    bool NetworkServices_xboxseries::IsNetworkInitialized()
    {
        if (SUCCEEDED(XNetworkingGetConnectivityHint(&m_connectivityHint)))
        {
            logConnectivityHint(m_connectivityHint);
            return m_connectivityHint.networkInitialized;
        }
        return false;
    }

    void NetworkServices_xboxseries::setIpAddress()
    {
        // See doc https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipaddrtable

        m_ipAddress.clear(); // Reset previous ip

        PMIB_IPADDRTABLE pIPAddrTable;
        DWORD dwSize = 0;
        DWORD dwRetVal = 0;

        pIPAddrTable = (MIB_IPADDRTABLE*) Memory::mallocCategory(sizeof(MIB_IPADDRTABLE), MemoryId::mId_OnlineServices);
        if (pIPAddrTable)
        {
            if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
            {
                Memory::free(pIPAddrTable);
                pIPAddrTable = (MIB_IPADDRTABLE*) Memory::mallocCategory(dwSize, MemoryId::mId_OnlineServices);
            }

            if (pIPAddrTable == NULL) {
                LOG("[NetworkServices] Memory allocation failed for GetIpAddrTable\n");
                return;
            }
        }

        if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR)
        {
            LOG("[NetworkServices] GetIpAddrTable failed with error %d\n", dwRetVal);
            return;
        }

        // Got through all interfaces with IP set and pickup the first one (bypass loopback IP address 127.x.x.x)
        for (int i = 0; pIPAddrTable && i < (int)pIPAddrTable->dwNumEntries; i++)
        {
            IN_ADDR IPAddr;
            IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;

            if (IPAddr.S_un.S_un_b.s_b1 != 127)
            {
                CHAR addr_str[16]; // 12 + 3 + 1 (xxx, dots, \0)
                inet_ntop(AF_INET, &IPAddr, addr_str, 16);
                m_ipAddress = addr_str;
                break;
            }
        }

        if (pIPAddrTable)
        {
            Memory::free(pIPAddrTable);
        }
    }
} // namespace ITF

#endif

