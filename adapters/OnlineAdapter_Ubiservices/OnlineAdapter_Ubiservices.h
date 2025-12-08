#ifndef ITF_ONLINE_ADAPTER_UBISERVICES_H
#define ITF_ONLINE_ADAPTER_UBISERVICES_H

#ifdef ITF_SUPPORT_UBISERVICES

#include <ubiservices/ubiservices.h>
#include <ubiservices/facade.h>

#include <ealmem.h>
#include <eallog.h>

#include "core/Macros.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
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
        virtual void setOfflineMode(bbool _isOffline);

        US_NS::SharedPtr<US_NS::Session> getSession();

        bool convertITFtoUSLanguage(ITF_LANGUAGE _itfLang, String8& _lang, String8& _locale);

    private:
        void initializeUbiservices();
        void configureUbiservices(const ubiservices::String& _buildId);
        void terminateUbiservices();

        void initFirstParty();
        void uninitFirstParty();

        String8 generateBuildId();
        const char* getApplicationId();
        const char* getSku();

        // EAL area
        eal_log_dll_interface m_ealLogInterface;
        eal_mem_dll_interface m_ealMemInterface;

        US_NS::UniquePtr<US_NS::UbiservicesSdk> m_sdk;
        US_NS::SharedPtr<US_NS::Session> m_session;

        bool m_initialized;
    };

#define ONLINE_ADAPTER_UBISERVICES (static_cast<OnlineAdapter_Ubiservices*>(ONLINE_ADAPTER))

} // namespace ITF

#endif // ITF_SUPPORT_UBISERVICES

#endif // ITF_ONLINE_ADAPTER_UBISERVICES_H
