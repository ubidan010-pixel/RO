#ifndef ITF_TRACKING_SERVICE_H
#define ITF_TRACKING_SERVICE_H

#include "core/templateSingleton.h"
#include "core/types.h"

namespace ITF
{

class TrackingService
{
public:
    virtual ~TrackingService() {}

    virtual bool init() = 0;
    virtual void term() = 0;
    virtual void update() = 0;

    virtual void updatePlayTime(u32 _sessionTime) = 0;

    // 2024.x US SDK Dynamic Events support
    virtual void setAttributeBool(const char* attr, bool val) = 0;
    virtual void setAttributeInt(const char* attr, i32 val) = 0;
    virtual void setAttributeLong(const char* attr, i64 val) = 0;
    virtual void setAttributeFloat(const char* attr, f32 val) = 0;
    virtual void setAttributeDouble(const char* attr, f64 val) = 0;
    virtual void setAttributeString(const char* attr, const char* val) = 0;

    virtual int sendSignal(const char* signal) = 0;
};

} // namespace ITF


#endif // ITF_TRACKING_SERVICE_H
