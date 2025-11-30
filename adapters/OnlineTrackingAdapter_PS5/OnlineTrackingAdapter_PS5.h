#ifndef _ITF_ONLINETRACKINGADAPTER_PS5_H_
#define _ITF_ONLINETRACKINGADAPTER_PS5_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H_

namespace ITF
{
    class OnlineTrackingAdapter_PS5 : public OnlineTrackingAdapter
    {
    public:

        OnlineTrackingAdapter_PS5(){}
        ~OnlineTrackingAdapter_PS5(){}
    };


	#define ONLINETRACKING_ADAPTER_PS5		(static_cast< OnlineTrackingAdapter_PS5* > (ONLINETRACKING_ADAPTER))

} // namespace ITF

#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H_
