#include "precompiled_engine.h"

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif

#ifndef _ITF_ONLINEADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineAdapter.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/TrackingService.h"
#endif //_ITF_ONLINEADAPTER_H_

#if defined(ITF_SUPPORT_UBISERVICES) && defined(ITF_SUPPORT_ONLINETRACKING)
namespace ITF
{
    OnlineTrackingAdapter::OnlineTrackingAdapter()
    {
    }

    OnlineTrackingAdapter::~OnlineTrackingAdapter()
    {
    }

    void OnlineTrackingAdapter::init()
    {
#ifdef ITF_SUPPORT_NETWORKSERVICES
        // Try connecting (silently) if network is available
        if (NETWORKSERVICES)
        {
            if (NETWORKSERVICES->isUserSignedIn(0))
                onSignIn(0); // not called from game manager if already signed in when game starts 
        }
#endif // ITF_SUPPORT_NETWORKSERVICES
    }

    void OnlineTrackingAdapter::onSignIn(u32 dwController)
    {
    }

    void OnlineTrackingAdapter::update()
    {
    }

    void OnlineTrackingAdapter::term()
    {
    }

    void OnlineTrackingAdapter::sendTag(const char* tag, const char* attributes, int controllerIndex)
    {
        // no-op. Use new API sendSignal
        LOG("[sendTag] %s / %s / %d", tag, attributes, controllerIndex);
    }

    void OnlineTrackingAdapter::setAttributeBool(const char* attr, bool val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeBool(attr, val);
    }

    void OnlineTrackingAdapter::setAttributeInt(const char* attr, i32 val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeInt(attr, val);
    }

    void OnlineTrackingAdapter::setAttributeLong(const char* attr, i64 val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeLong(attr, val);
    }

    void OnlineTrackingAdapter::setAttributeFloat(const char* attr, f32 val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeFloat(attr, val);
    }

    void OnlineTrackingAdapter::setAttributeDouble(const char* attr, f64 val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeDouble(attr, val);
    }

    void OnlineTrackingAdapter::setAttributeString(const char* attr, const char* val)
    {
        auto srv = ONLINE_ADAPTER->getTrackingService();
        if (srv)
            srv->setAttributeString(attr, attr);
    }

    int OnlineTrackingAdapter::sendSignal(const char* signal)
    {
        if (!CONFIG->m_enableonlineTracking)
            return -10;

        auto srv = ONLINE_ADAPTER->getTrackingService();
        return srv ? srv->sendSignal(signal) : -1;
    }

    void OnlineTrackingAdapter::updatePlayTime(u32 _sessionTime)
    {
        if (!CONFIG->m_enableonlineTracking)
            return;

        auto srv = ONLINE_ADAPTER->getTrackingService();
        srv->updatePlayTime(_sessionTime);
    }
}

#else
namespace ITF
{
	OnlineTrackingAdapter::OnlineTrackingAdapter() {}
	OnlineTrackingAdapter::~OnlineTrackingAdapter() {}
	void OnlineTrackingAdapter::init() {}
	void OnlineTrackingAdapter::term() {}
	void OnlineTrackingAdapter::sendTag(const char *tag, const char *attributes, int controllerIndex) {}
	void OnlineTrackingAdapter::update() {}
	void OnlineTrackingAdapter::onSignIn(u32 dwController) {}

    void setAttributeBool(const char* attrName, bool bVal) {}
    void setAttributeInt(const char* attrName, int iVal) {}
    void setAttributeLong(const char* attrName, long lVal) {}
    void setAttributeFloat(const char* attrName, float fVal) {}
    void setAttributeDouble(const char* attrName, double dVal) {}
    void setAttributeString(const char* attrName, const char* sVal) {}
    int sendSignal(const char* signal) { return 0; }
    void updatePlayTime(u32 _sessionTime) {}
}

#endif //ITF_SUPPORT_UBISERVICES
