#ifndef ITF_ONLINE_ADAPTER_UBISERVICES_H
#define ITF_ONLINE_ADAPTER_UBISERVICES_H

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

        US_NS::Facade* getUbiservicesFacade() const { ITF_ASSERT_CRASH(m_usFacade, "US Facade is NULL!!"); return m_usFacade; }

        void recreateFacade();

    private:
        void initializeUbiservices();
        void configureUbiservices(const ubiservices::String& _buildId);
        void terminateUbiservices();

        String8 generateBuildId();

        US_NS::Facade* m_usFacade;
        US_NS::GameConfig *m_gameConfig;
        US_NS::SystemConfig *m_sysConfig;

        bool m_initialized;
    };

#define ONLINE_ADAPTER_UBISERVICES (static_cast<OnlineAdapter_Ubiservices*>(ONLINE_ADAPTER))

} // namespace ITF

#endif // ITF_ONLINE_ADAPTER_UBISERVICES_H
