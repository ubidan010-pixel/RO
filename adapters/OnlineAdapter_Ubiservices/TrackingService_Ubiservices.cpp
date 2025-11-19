#include "precompiled_OnlineAdapter_Ubiservices.h"

#if defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"
#include "adapters/OnlineAdapter_Ubiservices/TrackingService_Ubiservices.h"

#include "core/error/ErrorHandler.h"

#include <ubiservices/services/event/eventClient.h>
#include <ubiservices/services/event/types/eventInfoContextStart.h>
#include <ubiservices/services/event/types/eventInfoContextStop.h>
#include <ubiservices/services/event/types/eventInfoCustom.h>
#include <ubiservices/services/event/types/eventInfoGameLocalization.h>
#include <ubiservices/services/event/types/eventInfoPlayerProgression.h>
#include <ubiservices/services/event/types/eventInfoPlayerAchievement.h>
#include <ubiservices/services/event/types/eventInfoService.h>
#include <ubiservices/services/event/types/types.h>

using namespace ubiservices;

namespace ITF
{
    TrackingService_Ubiservices::TrackingService_Ubiservices()
        :TrackingService()
        , m_playTime(0)
    {
    }

    TrackingService_Ubiservices::~TrackingService_Ubiservices()
    {

    }

    bool TrackingService_Ubiservices::init()
    {
        return true;
    }

    void TrackingService_Ubiservices::term() {}
    void TrackingService_Ubiservices::update() {}

    void TrackingService_Ubiservices::updatePlayTime(u32 _sessionTime)
    {
        if (m_playTime == _sessionTime)
            return;

        m_playTime = _sessionTime;

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
            return;

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.updatePlayTimeClock(m_playTime);
    }

    void TrackingService_Ubiservices::setAttributeBool(const char* attr, bool val)
    {
        ITF_ASSERT(attr);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeBoolean(attr, val);
    }

    void TrackingService_Ubiservices::setAttributeInt(const char* attr, i32 val)
    {
        ITF_ASSERT(attr);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeInt(attr, val);
    }

    void TrackingService_Ubiservices::setAttributeLong(const char* attr, i64 val)
    {
        ITF_ASSERT(attr);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeLong(attr, val);
    }

    void TrackingService_Ubiservices::setAttributeFloat(const char* attr, f32 val)
    {
        ITF_ASSERT(attr);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeFloat(attr, val);
    }

    void TrackingService_Ubiservices::setAttributeDouble(const char* attr, f64 val)
    {
        ITF_ASSERT(attr);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeDouble(attr, val);
    }
    void TrackingService_Ubiservices::setAttributeString(const char* attr, const char* val)
    {
        ITF_ASSERT(attr);
        ITF_ASSERT(val);

        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            return;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        module.setAttributeString(attr, val);
    }

    int TrackingService_Ubiservices::sendSignal(const char* signal)
    {
        auto session = ONLINE_ADAPTER_UBISERVICES->getSession();
        if (!session)
        {
            LOG("[TrackingService] sendSignal NO ubi session for (%s)", signal);
            return -1;
        }

        PlayerEventModule& module = PlayerEventModule::module(*session);
        return (int)module.sendSignal(signal);
    }
} // namespace ITF

#endif //defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
