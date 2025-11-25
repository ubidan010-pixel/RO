#ifndef ITF_TRACKINGSERVICE_UBISERVICES_H
#define ITF_TRACKINGSERVICE_UBISERVICES_H

#if defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
#include <ubiservices/ubiservices.h>
#include <ubiservices/common/config.h>

#include "core/Macros.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/TrackingService.h"

using namespace ubiservices;

namespace ITF
{
    class TrackingService_Ubiservices : public TrackingService
    {
    public:
        TrackingService_Ubiservices();
        virtual ~TrackingService_Ubiservices();

        virtual bool init() override;
        virtual void term() override;
        virtual void update() override;

        virtual void updatePlayTime(u32 _sessionTime) override;

        virtual void setAttributeBool(const char* attr, bool val) override;
        virtual void setAttributeInt(const char* attr, i32 val) override;
        virtual void setAttributeLong(const char* attr, i64 val) override;
        virtual void setAttributeFloat(const char* attr, f32 val) override;
        virtual void setAttributeDouble(const char* attr, f64 val) override;
        virtual void setAttributeString(const char* attr, const char* val) override;

        virtual int sendSignal(const char* signal) override;

    private:
        US_NS::TimeSeconds m_playTime;
    };

} // namespace ITF

#endif // defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)

#endif // ITF_TRACKINGSERVICE_UBISERVICES_H
