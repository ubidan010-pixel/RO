#ifndef ITF_ONLINE_ADAPTER_UBISERVICES_H
#define ITF_ONLINE_ADAPTER_UBISERVICES_H

#ifdef ITF_SUPPORT_UBISERVICES

#include <ubiservices/ubiservices.h>
#include <ubiservices/facade.h>

#include <ealmem.h>
#include <eallog.h>

#include "core/Macros.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineAdapter.h"

namespace ITF
{
    class OnlineAdapter_Ubiservices : public OnlineAdapter
    {
    public:
        OnlineAdapter_Ubiservices();
        virtual ~OnlineAdapter_Ubiservices();

        virtual void initialize();
        virtual void update();
        virtual void terminate();

        US_NS::SharedPtr<US_NS::Session> getSession() { return m_session; }

    private:
        void initializeUbiservices();
        void configureUbiservices(const ubiservices::String& _buildId);
        void terminateUbiservices();
        void createSession();
        void closeSession();

        String8 generateBuildId();

        US_NS::UniquePtr<US_NS::UbiservicesSdk> m_sdk;

        // configureSdk / uninitializeSdk try to destruct strings inside if we pass configs on stack
        US_NS::GameConfig *m_gameConfig;
        US_NS::SystemConfig *m_sysConfig;

        US_NS::SharedPtr<US_NS::Session> m_session;

        bool m_initialized;
    };

#define ONLINE_ADAPTER_UBISERVICES (static_cast<OnlineAdapter_Ubiservices*>(ONLINE_ADAPTER))

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES

#endif // ITF_ONLINE_ADAPTER_UBISERVICES_H
