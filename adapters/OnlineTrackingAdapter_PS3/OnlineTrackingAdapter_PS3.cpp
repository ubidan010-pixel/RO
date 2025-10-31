#include "precompiled_OnlineTrackingAdapter_PS3.h"

#ifndef _ITF_ONLINETRACKINGADAPTER_PS3_H_
#include "adapters/OnlineTrackingAdapter_ps3/OnlineTrackingAdapter_ps3.h"
#endif //_ITF_ONLINETRACKINGADAPTER_PS3_H_

#if defined(ITF_SUPPORT_ONLINETRACKING)

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif

#ifndef _ITF_NETWORKSERVICES_PS3_H_
#include "engine/NetworkServices/PS3/NetworkServices_PS3.h"
#endif //_ITF_NETWORKSERVICES_PS3_H_

//#define NO_USING_QUAZAL_DIRECTIVE
#undef UNICODE
#undef _UNICODE
#include "../extern/RendezVous/RendezVous.h"

namespace ITF
{
	const char *OnlineTrackingAdapter_ps3::getOnlineConfigId() const
	{
		return "823b55f539ce40e3a8b3604eaeac88d9"; //TEMP: UAT
	}

	const char *OnlineTrackingAdapter_ps3::getSandboxAccessKey() const
	{
		return "07WRok17"; //TEMP: UAT
	}

	Quazal::OnlineConfigClient *OnlineTrackingAdapter_ps3::tryCreatingOnlineConfigClient()
	{
		char *npTicket;
		size_t npTicketSize;
		if (!(static_cast<NetworkServices_PS3*>(NETWORKSERVICES))->getRequestTicket(&npTicket, &npTicketSize))
			return NULL;

		return qNew OnlineConfigClient(getOnlineConfigId(), (qByte*)npTicket, npTicketSize);
	}

} //ITF

#endif //ITF_SUPPORT_ONLINETRACKING

