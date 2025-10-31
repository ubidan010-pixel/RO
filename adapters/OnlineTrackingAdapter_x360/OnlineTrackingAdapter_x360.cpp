#include "precompiled_OnlineTrackingAdapter_x360.h"

#ifndef _ITF_ONLINETRACKINGADAPTER_X360_H_
#include "adapters/OnlineTrackingAdapter_x360/OnlineTrackingAdapter_x360.h"
#endif //_ITF_ONLINETRACKINGADAPTER_X360_H_

#if defined(ITF_SUPPORT_ONLINETRACKING)

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

//#define NO_USING_QUAZAL_DIRECTIVE
#undef UNICODE
#undef _UNICODE
#include "../extern/RendezVous/RendezVous.h"

namespace ITF
{
	const char *OnlineTrackingAdapter_x360::getOnlineConfigId() const
	{
		return "5d98360be9f0442aaded982f92a521ac"; //TEMP: UAT
	}

	const char *OnlineTrackingAdapter_x360::getSandboxAccessKey() const
	{
		return "eBL6nPH7"; //TEMP: UAT
	}

	Quazal::OnlineConfigClient *OnlineTrackingAdapter_x360::tryCreatingOnlineConfigClient()
	{
		return qNew OnlineConfigClient(getOnlineConfigId());
	}

} //ITF

#endif //ITF_SUPPORT_ONLINETRACKING

