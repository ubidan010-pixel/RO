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

#ifndef _ITF_ONLINEADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineAdapter.h"
#include "engine/AdaptersInterfaces/OnlineAdapter/TrackingService.h"
#endif //_ITF_ONLINEADAPTER_H_

#if defined(ITF_SUPPORT_UBISERVICES)
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
        auto srv = ONLINE_ADAPTER->getTrackingService();
        srv->startTag(tag);
        srv->appendAttribute("test", "12", TrackingService::E_ATTRIB_DATATYPE_INT32);
        srv->sendTag();
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
}

#endif //ITF_SUPPORT_UBISERVICES
