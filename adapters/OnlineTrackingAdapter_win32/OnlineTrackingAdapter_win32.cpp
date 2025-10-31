#include "precompiled_OnlineTrackingAdapter_win32.h"

#ifndef _ITF_ONLINETRACKINGADAPTER_WIN32_H_
#include "adapters/OnlineTrackingAdapter_win32/OnlineTrackingAdapter_win32.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H_

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
	const char *OnlineTrackingAdapter_win32::getOnlineConfigId() const
	{
		return "58c47d2bd1934c97960ab373c14bc417";
	}

	const char *OnlineTrackingAdapter_win32::getSandboxAccessKey() const
	{
		return "mbAvckU3";
	}

	Quazal::OnlineConfigClient *OnlineTrackingAdapter_win32::tryCreatingOnlineConfigClient()
	{
		return qNew OnlineConfigClient(getOnlineConfigId());
	}

} //ITF

#endif //ITF_SUPPORT_ONLINETRACKING

