#ifndef _ITF_ONLINETRACKINGADAPTER_H
#define _ITF_ONLINETRACKINGADAPTER_H

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_


// Undef this to use the final server environment
// (after you got it from TG Ops)
// (Win32à) While this is defined, the dev/test environment is used (except in ITF_FINAL)
#define USE_UAT_SANDBOX

namespace Quazal
{
	class OnlineConfigClient;
}

namespace ITF
{

class OnlineConfigFetcher;

class OnlineTrackingAdapter: public TemplateSingleton<OnlineTrackingAdapter>
{
public:
	OnlineTrackingAdapter();
	virtual ~OnlineTrackingAdapter();  // always declare virtual destructor for adapters
	virtual void	init();
	virtual void    term();

	virtual void	update();
	virtual void	onSignIn(u32 dwController);

	// Add tag into queue.
	// If network is available, queued tags will be sent regularly.
	// If network is not available, they will be kept until you go ontil using MainConnectionManager.
	// @param tag Tag (must belong to the list configured on the server)
	// @param attributes Argument line (see format in Rendez-Vous Tracking documentation)
	// @param controllerIndex To identify the player if applicable
	virtual void sendTag(const char *tag, const char *attributes, int controllerIndex=-1);

#ifdef ITF_SUPPORT_ONLINETRACKING
protected:
	void requestOnlineConfig();
	virtual const char *getOnlineConfigId() const = 0;
	virtual const char *getSandboxAccessKey() const = 0;
	virtual Quazal::OnlineConfigClient *tryCreatingOnlineConfigClient() = 0;
	friend class OnlineConfigFetcher;

private:
	OnlineConfigFetcher	*m_OnlineConfigFetcher;
#endif
};

}

#endif //_ITF_ONLINETRACKINGADAPTER_H