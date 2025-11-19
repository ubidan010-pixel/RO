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

	// $GS: Legacy UAF behavior, key/value as string pairs concat by &.
    // This isn't quite useful since we'd need to re-split strings and parse to use with US SDK
    // 
    // Add tag into queue.
	// If network is available, queued tags will be sent regularly.
	// If network is not available, they will be kept until you go online.
	// @param tag Tag
	// @param attributes Values
	// @param controllerIndex Identify the player if applicable
	virtual void sendTag(const char *tag, const char *attributes, int controllerIndex=-1);

    // New API in engine\AdaptersInterfaces\OnlineAdapter\TrackingService.h
    virtual void setAttributeBool(const char* attr, bool val);
    virtual void setAttributeInt(const char* attr, i32 val);
    virtual void setAttributeLong(const char* attr, i64 val);
    virtual void setAttributeFloat(const char* attr, f32 val);
    virtual void setAttributeDouble(const char* attr, f64 val);
    virtual void setAttributeString(const char* attr, const char *val);

    virtual int sendSignal(const char* signal);

    virtual void updatePlayTime(u32 _sessionTime);
};

}

#endif //_ITF_ONLINETRACKINGADAPTER_H
