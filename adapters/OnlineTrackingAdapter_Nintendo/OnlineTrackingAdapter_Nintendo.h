#ifndef _ITF_ONLINETRACKINGADAPTER_NINTENDO_H_
#define _ITF_ONLINETRACKINGADAPTER_NINTENDO_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_Nintendo : public OnlineTrackingAdapter
    {
    public:

        OnlineTrackingAdapter_Nintendo(){}
        ~OnlineTrackingAdapter_Nintendo(){}
    };


	#define ONLINETRACKING_ADAPTER_NINTENDO		(static_cast< OnlineTrackingAdapter_Nintendo* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_NINTENDO_H_
