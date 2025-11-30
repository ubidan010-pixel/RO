#ifdef ITF_NINTENDO
#ifndef _ITF_NETWORKSERVICES_NINTENDO_H_
#define _ITF_NETWORKSERVICES_NINTENDO_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifdef ITF_SUPPORT_NETWORKSERVICES
#include <nn/nifm.h>
#include <nn/nifm/nifm_ApiIpAddress.h>
#include <nn/socket.h>

namespace ITF
{
    class NetworkServices_Nintendo : public NetworkServices
    {

    public:

        NetworkServices_Nintendo();
        ~NetworkServices_Nintendo();

        virtual u32 update() override;
        virtual void initialize(u32 dwMinUsers, u32 dwMaxUsers, bbool  bRequireOnlineUsers, u32 dwSignInPanes) override;
        virtual void terminate() override;

        void requestRefresh() override;

        virtual u32 getPlatformSpecificErrorCode() override;

    private:
        int m_platformErrorCode;

        void* m_memoryPoolBuffer;
        bool m_connectionRequested;
        f64 m_nextStatusUpdateTime;
        static const f64 m_statusUpdateTimeInterval;

        void updateNetworkWhenPreparing();
        void updateNetworkWhenReady();
        void updateNetworkWhenError();
    };

} // namespace ITF

#endif // ITF_SUPPORT_NETWORKSERVICES
#endif //_ITF_NETWORKSERVICES_NINTENDO_H_
#endif //ITF_NINTENDO
