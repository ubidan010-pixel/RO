#include "precompiled_engine.h"

#ifndef _ITF_NETWORKSERVICES_PS5_H_
#include "engine/NetworkServices/PS5/NetworkServices_PS5.h"
#endif //_ITF_NETWORKSERVICES_PS5_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_PS5_H_
#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"
#endif //_ITF_SYSTEMADAPTER_PS5_H_

#include <libnetctl.h>

namespace ITF
{
	NetworkServices_PS5::NetworkServices_PS5(): m_platformErrorCode(SCE_OK)
    {
	}

	NetworkServices_PS5::~NetworkServices_PS5()
	{
	}

    // References https://confluence.ubisoft.com/display/onlineservices/UbiServices+Client+SDK+-+Initializing+PS4+Cross-gen+and+PS5
    static int libnetctlCid = -1;

    static const char* getStateStr(int state)
    {
        static const char* STATE_STR[] = { "Disconnected", "Connecting", "IP Obtaining", "IP Obtained" };
        return STATE_STR[state];
    }

    static const char* getEventTypeStr(int eventType)
    {
        static const char* EVENT_TYPE_STR[] = { "INVALID EVENT TYPE", "DISCONNECTED", "DISCONNECT_REQ_FINISHED", "IPOBTAINED" };
        return EVENT_TYPE_STR[eventType];
    }

    u32 NetworkServices_PS5::getPlatformSpecificErrorCode()
    {
        return m_platformErrorCode;
    }

    void NetworkServices_PS5::initialize( u32 dwMinUsers,u32 dwMaxUsers,bbool bRequireOnlineUsers,u32 dwSignInPanes )
    {
        ITF_VERIFY_SCE_CALL(sceNetCtlInit());

        for (int i = 0; i < 1000; i++)
        {
            int state;
            int result = sceNetCtlGetState(&state);

            if (result != SCE_OK)
            {
                LOG("[NetworkServices_PS5] sceNetCtlGetState() failed: %X\n", result);
                m_platformErrorCode = result;
                return;
            }
            else if (state == SCE_NET_CTL_STATE_DISCONNECTED)
            {
                LOG("[NetworkServices_PS5] sceNetCtlGetState result is SCE_NET_CTL_STATE_DISCONNECTED\n");
                setNetworkStatus(NetworkServices::ENetworkStatus_Error);
            }
            else if (state == SCE_NET_CTL_STATE_CONNECTING)
            {
                LOG("[NetworkServices_PS5] sceNetCtlGetState result is SCE_NET_CTL_STATE_CONNECTING\n");
                setNetworkStatus(NetworkServices::ENetworkStatus_Preparing);
            }
            else if (state == SCE_NET_CTL_STATE_IPOBTAINING)
            {
                LOG("[NetworkServices_PS5] sceNetCtlGetState result is SCE_NET_CTL_STATE_IPOBTAINING\n");
            }
            else if (state == SCE_NET_CTL_STATE_IPOBTAINED)
            {
                LOG("[NetworkServices_PS5] sceNetCtlGetState result is SCE_NET_CTL_STATE_IPOBTAINED\n");
                setNetworkStatus(NetworkServices::ENetworkStatus_Ready);
                break;
            }

            sleep(50); // Arbitrary value
        }

        ITF_VERIFY_SCE_CALL(sceNetCtlRegisterCallback(libnetctlCallback, this, &libnetctlCid));
	}
        
    void NetworkServices_PS5::terminate()
    {
        LOG("[NetworkServices_PS5] Shutting down network...\n");

        ITF_VERIFY_SCE_CALL(sceNetCtlUnregisterCallback(libnetctlCid));
        sceNetCtlTerm();

        LOG("[NetworkServices_PS5] Network interface down.\n");
    }

    u32 NetworkServices_PS5::update()
    {
        return 0;
    }

    void NetworkServices_PS5::libnetctlCallback(int eventType, void* arg)
    {
        NetworkServices_PS5* pThis = static_cast<NetworkServices_PS5*>(arg);
        {
            int state;
            const int result = sceNetCtlGetState(&state);
            if (pThis)
                pThis->m_platformErrorCode = result;

            if (result < SCE_OK)
            {
                LOG("[NetworkServices_PS5] sceNetCtlInetGetState failed: 0x%x\n", result);
                return;
            }
            else
            {
                LOG("[NetworkServices_PS5] %s: eventType = %s, state = %s\n", __FUNCTION__, getEventTypeStr(eventType), getStateStr(state));
            }
        }

        if (eventType == SCE_NET_CTL_EVENT_TYPE_DISCONNECTED)
        {
            int netResult;
            const int result = sceNetCtlGetResult(eventType, &netResult);
            if (result < SCE_OK)
            {
                LOG("[NetworkServices_PS5] sceNetCtlInetGetResult failed: 0x%x\n", result);
            }
            else
            {
                LOG("[NetworkServices_PS5] %s: result = 0x%x\n", __FUNCTION__, netResult);
            }

            if (pThis)
            {
                pThis->setNetworkStatus(NetworkServices::ENetworkStatus_Error);
                pThis->m_platformErrorCode = result;
            }
        }
        else if (eventType == SCE_NET_CTL_EVENT_TYPE_IPOBTAINED)
        {
            pThis->setNetworkStatus(NetworkServices::ENetworkStatus_Ready);
        }
    }
} // namespace ITF

