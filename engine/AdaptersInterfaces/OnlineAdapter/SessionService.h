#pragma once

#include "core/container/extendedVector.h"
#include "core/StringID.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/PopulationInfo.h"

namespace ITF
{

class OnlineError;

class SessionService
{
public:
    SessionService() {}
    virtual ~SessionService() {}

    enum ESessionStatus
    {
        ESessionStatus_Idle,
        ESessionStatus_Preparing,
        ESessionStatus_Valid,
        ESessionStatus_Error,
        ESessionStatus_Offline,
        ESessionStatus_Count
    };

    virtual void update() = 0;

    virtual ESessionStatus getSessionStatus() const = 0;
    virtual OnlineError getSessionError() = 0;
    virtual const ITF_VECTOR<PopulationInfo>& getPopulations() const = 0;
    virtual bool isInPopulation(const String8& _subject, const String8& _popName) const = 0;

    virtual bool isClubFeatureSwitchEnabled() const = 0;
    virtual bool isNewsFeatureSwitchEnabled() const = 0;

    virtual bool launchConnect(const String8& _deepLink = "", std::list<std::pair<String8, String8> > _params = {}) = 0;

    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, f32 &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, u32 &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, i32 &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, String8 &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, bool &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<String8> &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<u32> &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<i32> &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<bool> &_fieldValue) const = 0;
    virtual bool getConstantValue(String8 _parameterGroupName, StringID _parameterName, StringID _fieldName, ITF_VECTOR<StringID> &_fieldValue) const = 0;

    // Apply the request. The number of requests is limited to one per defined period, other requests are bypassed.
    virtual void requestRefresh() {}
    // Apply the request without any restriction (warning: don't use by default and be sure you are NX compliant)
    virtual void forceRequestRefresh() {}

    enum SessionResult {};

    virtual void initialize() = 0;
    virtual void terminate() = 0;
    inline bool isSessionValid() const { return (getSessionStatus() == ESessionStatus_Valid); }
    virtual bool isTransitioningToForeground() const = 0;
};


} // namespace ITF
