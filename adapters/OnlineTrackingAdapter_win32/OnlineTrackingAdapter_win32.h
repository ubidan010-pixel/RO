#ifndef _ITF_ONLINETRACKINGADAPTER_WIN32_H_
#define _ITF_ONLINETRACKINGADAPTER_WIN32_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_win32 : public OnlineTrackingAdapter
    {
    public:

        OnlineTrackingAdapter_win32(){}
        ~OnlineTrackingAdapter_win32(){}

		Quazal::OnlineConfigClient *tryCreatingOnlineConfigClient();

	protected:
		const char *getOnlineConfigId() const;
		const char *getSandboxAccessKey() const;
    };


	#define ONLINETRACKING_ADAPTER_WIN32		(static_cast< OnlineTrackingAdapter_win32* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H_