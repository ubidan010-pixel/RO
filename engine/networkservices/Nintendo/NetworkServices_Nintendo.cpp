#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_NINTENDO_H_
#include "engine/NetworkServices/Nintendo/NetworkServices_Nintendo.h"
#endif //_ITF_NETWORKSERVICES_NINTENDO_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef ITF_SYSTEMADAPTER_NINTENDO_H_
#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"
#endif //ITF_SYSTEMADAPTER_NINTENDO_H_

#include <nn/socket.h>
#include <nn/nifm.h>
#include <nn/nifm/nifm_ApiIpAddress.h>
#include <nn/nifm/nifm_ApiNetworkConnection.h>

#ifdef ITF_SUPPORT_NETWORKSERVICES
namespace ITF
{
    const f64 NetworkServices_Nintendo::m_statusUpdateTimeInterval = 1.0; // seconds

    NetworkServices_Nintendo::NetworkServices_Nintendo()
        :NetworkServices()
        , m_memoryPoolBuffer(nullptr)
        , m_connectionRequested(false)
        , m_nextStatusUpdateTime(0.0)
    {
    }

    NetworkServices_Nintendo::~NetworkServices_Nintendo()
    {
    }

    void NetworkServices_Nintendo::initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool bRequireOnlineUsers, u32 dwSignInPanes)
    {
        nn::Result result = nn::nifm::Initialize();
        if (result.IsFailure())
        {
            LOG("nn::nifm::Initialize() failed with result 0x%08X", result.GetInnerValueForDebug());
        }

#if defined(ITF_NX)
        const u32 poolAlignment = 4 * 1024;
        m_memoryPoolBuffer = Memory::alignedMallocCategory(nn::socket::DefaultSocketMemoryPoolSize, poolAlignment, MemoryId::mId_OnlineServices);
        if (!m_memoryPoolBuffer)
        {
            LOG("Failed to allocate socket memory pool");
        }

        result = nn::socket::Initialize(m_memoryPoolBuffer, nn::socket::DefaultSocketMemoryPoolSize, nn::socket::MinSocketAllocatorSize, nn::socket::DefaultConcurrencyLimit);
        if (!result.IsSuccess())
        {
            LOG("nn::socket::Initialize() failed with result 0x%08X", result.GetInnerValueForDebug());
            return;
        }
#elif defined(ITF_OUNCE)
        result = nn::socket::Initialize();
        if (!result.IsSuccess())
        {
            LOG("nn::socket::Initialize() failed with result 0x%08X", result.GetInnerValueForDebug());
            return;
        }
#endif
    }

    void NetworkServices_Nintendo::terminate()
    {
        if (m_connectionRequested)
            nn::nifm::CancelNetworkRequest();

        nn::Result result = nn::socket::Finalize();
        ITF_ASSERT_CRASH(result.IsSuccess(), "nn::socket::Finalize() failed with result 0x%08X", result.GetInnerValueForDebug());
        ITF_UNUSED(result);

        Memory::alignedFree(m_memoryPoolBuffer);
        m_memoryPoolBuffer = nullptr;
    }

    u32 NetworkServices_Nintendo::update()
    {
        switch (getNetworkStatus())
        {
        case ENetworkStatus_Preparing:
            updateNetworkWhenPreparing();
            break;

        case ENetworkStatus_Ready:
            updateNetworkWhenReady();
            break;

        case ENetworkStatus_Error:
            updateNetworkWhenError();
            break;
        }
        return 0;
    }

    void NetworkServices_Nintendo::requestRefresh()
    {
        switch (getNetworkStatus())
        {
        case ENetworkStatus_Preparing:
            // A refresh is already in progress
            break;

        case ENetworkStatus_Ready:
            m_nextStatusUpdateTime = SYSTEM_ADAPTER->getTime();
            break;

        case ENetworkStatus_Error:
            m_connectionRequested = true;
            break;
        }

        update();
    }

    void NetworkServices_Nintendo::updateNetworkWhenPreparing()
    {
        f64 currentTime = SYSTEM_ADAPTER->getTime();

        if (!nn::nifm::IsNetworkRequestOnHold())
        {
            if (nn::nifm::IsNetworkAvailable())
            {
                nn::socket::InAddr ipAddress;
                nn::socket::InAddr subnetMask;
                nn::socket::InAddr defaultGateway;
                nn::socket::InAddr preferredDns;
                nn::socket::InAddr alternateDns;

                nn::Result result = nn::nifm::GetCurrentIpConfigInfo(&ipAddress, &subnetMask, &defaultGateway, &preferredDns, &alternateDns);
                if (result.IsSuccess())
                {
                    m_ipAddress = nn::socket::InetNtoa(ipAddress);

                    LOG("IP configuration:\n"
                        "  - IP address.....: %s\n"
                        "  - Subnet mask....: %s\n"
                        "  - Default gateway: %s\n"
                        "  - Preferred DNS..: %s\n"
                        "  - Alternate DNS..: %s",
                        m_ipAddress.cStr(),
                        nn::socket::InetNtoa(subnetMask),
                        nn::socket::InetNtoa(defaultGateway),
                        nn::socket::InetNtoa(preferredDns),
                        nn::socket::InetNtoa(alternateDns));

                    setNetworkStatus(ENetworkStatus_Ready);
                    m_nextStatusUpdateTime = currentTime + m_statusUpdateTimeInterval;
                }
                else
                {
                    LOG("nn::nifm::GetCurrentIpConfigInfo() failed with result 0x%08X", result.GetInnerValueForDebug());
                    setNetworkStatus(ENetworkStatus_Error);
                }
            }
            else
            {
                setNetworkStatus(ENetworkStatus_Error);
            }
        }
        else if (m_connectionRequested)
        {
            nn::nifm::SubmitNetworkRequest();
            m_connectionRequested = false;
        }
    }


    void NetworkServices_Nintendo::updateNetworkWhenReady()
    {
        f64 currentTime = SYSTEM_ADAPTER->getTime();
        if (currentTime >= m_nextStatusUpdateTime)
        {
            if (nn::nifm::IsNetworkAvailable())
            {
                m_nextStatusUpdateTime = currentTime + m_statusUpdateTimeInterval;
            }
            else
            {
                memset(&m_ipAddress, 0, sizeof(m_ipAddress));
                setNetworkStatus(ENetworkStatus_Error);
            }
        }
    }

    void NetworkServices_Nintendo::updateNetworkWhenError()
    {
        if (m_connectionRequested)
        {
            setNetworkStatus(ENetworkStatus_Preparing);
        }
    }

    u32 NetworkServices_Nintendo::getPlatformSpecificErrorCode()
    {
        return m_platformErrorCode;
    }
} // namespace ITF

#endif
