#ifndef _ITF_ONLINETRACKINGADAPTER_H
#define _ITF_ONLINETRACKINGADAPTER_H

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

namespace ITF
{
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
	// If network is not available, they will be kept until you go online.
	// @param tag Tag
	// @param attributes Values
	// @param controllerIndex Identify the player if applicable
	virtual void sendTag(const char *tag, const char *attributes, int controllerIndex=-1);
};

}

#endif //_ITF_ONLINETRACKINGADAPTER_H
