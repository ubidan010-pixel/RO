#include "precompiled_engine.h"

#if defined(ITF_SUPPORT_VIDEOCAPTURE)

#ifndef _ITF_VIDEOCAPTURECOMMAND_H_
#include "engine/misc/videocaptureCommand.h"
#endif //_ITF_VIDEOCAPTURECOMMAND_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_


namespace  ITF
{

    ///ONLY ON STEP BY STEP COMMAND
    void VideoCaptureCommand::startStepByStep()
    {
        GFX_ADAPTER->setThreadUsageScreenCapture(4,btrue);
        GFX_ADAPTER->startScreenCapture();

        RESOURCE_MANAGER->disableDefaultResourceUsage();

        World* pWorld = CURRENTWORLD;
        if (pWorld)
            pWorld->forceLoadResourcesAndDisableUnload();
    }

    void VideoCaptureCommand::stop()
    {
        VideoCapture* pVideoInstance = VideoCapture::getInstance();
        bbool wasPlaying = pVideoInstance ? pVideoInstance->isEnable() : bfalse;

        if (pVideoInstance)
        {
            if (pVideoInstance->isStepByStep())
            {
                GFX_ADAPTER->stopScreenCapture();

                if (DEBUGINFO &&  wasPlaying && (pVideoInstance->isEnable()== bfalse))
                {
                    String text;
                    text.setTextFormat("Video capture done in %s",pVideoInstance->getScreenCaptureDirectory().cStr());
                    DEBUGINFO->addDebugInfoDisplay(text.cStr(),3.0f);
                }
            }
        }
    }

}

#endif // defined(ITF_SUPPORT_VIDEOCAPTURE)
