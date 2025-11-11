#ifndef _ITF_ONLINETRACKINGADAPTER_PS3_H_
#define _ITF_ONLINETRACKINGADAPTER_PS3_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_ps3 : public OnlineTrackingAdapter
    {
    public:

		OnlineTrackingAdapter_ps3() {}
		~OnlineTrackingAdapter_ps3() {}
    };


	#define ONLINETRACKING_ADAPTER_PS3		(static_cast< OnlineTrackingAdapter_ps3* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_PS3_H_
