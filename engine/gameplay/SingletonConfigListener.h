#ifndef _ITF_SINGLETONCONFIGLISTENER_H_
#define _ITF_SINGLETONCONFIGLISTENER_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H


namespace ITF
{

//------------------------------------------------------------------------------
/// Hot reload for global singleton config files (.isg)
class SingletonConfigListener : public WatchDirectoryListener
{
public:
    SingletonConfigListener();
    virtual ~SingletonConfigListener();

    virtual void onModifiedFile(const String& _absoluteFilename);
};

} // namespace ITF

#endif // _ITF_SINGLETONCONFIGLISTENER_H_
