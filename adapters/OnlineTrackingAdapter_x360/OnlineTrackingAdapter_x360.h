#ifndef _ITF_ONLINETRACKINGADAPTER_X360_H_
#define _ITF_ONLINETRACKINGADAPTER_X360_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_x360 : public OnlineTrackingAdapter
    {
    public:

        OnlineTrackingAdapter_x360() {}
        ~OnlineTrackingAdapter_x360() {}

		Quazal::OnlineConfigClient *tryCreatingOnlineConfigClient();

	protected:
		const char *getOnlineConfigId() const;
		const char *getSandboxAccessKey() const;
	};


	#define ONLINETRACKING_ADAPTER_X360		(static_cast< OnlineTrackingAdapter_x360* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_X360_H_