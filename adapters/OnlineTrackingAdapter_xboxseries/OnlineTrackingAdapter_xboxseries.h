#ifndef _ITF_ONLINETRACKINGADAPTER_XBOXSERIES_H_
#define _ITF_ONLINETRACKINGADAPTER_XBOXSERIES_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_xboxseries : public OnlineTrackingAdapter
    {
    public:

        OnlineTrackingAdapter_xboxseries(){}
        ~OnlineTrackingAdapter_xboxseries(){}
    };


	#define ONLINETRACKING_ADAPTER_XBOXSERIES       (static_cast< OnlineTrackingAdapter_win32* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_XBOXSERIES_H_
