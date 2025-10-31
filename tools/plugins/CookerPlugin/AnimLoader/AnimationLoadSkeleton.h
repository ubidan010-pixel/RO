#ifndef _ITF_ANIMATIONLOADSKELETON_H_
#define _ITF_ANIMATIONLOADSKELETON_H_

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_


namespace ITF
{
    class AnimSkeleton;

    class AnimationLoadSkeleton
    {
    public:
        static AnimSkeleton * readAnimSkeleton(const Path& _XMLFile_Path,const String& _szPlatform, bbool _raiseError, bbool _bSaveBin);
    };

};


#endif //_ITF_ANIMATIONLOADSKELETON_H_