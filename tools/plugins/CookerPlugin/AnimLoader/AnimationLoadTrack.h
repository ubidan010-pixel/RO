#ifndef _ITF_ANIMATIONLOADTRACK_H_
#define _ITF_ANIMATIONLOADTRACK_H_

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_

namespace ITF
{
    class AnimTrack;
    class AnimSkeleton;
    class AnimationDependenciesMap;
    class AnimationDependencies;

    class AnimationLoadTrack
    {
    public:
        static AnimTrack * readAnimTrack(  AnimSkeleton *_skeleton, const Path& _XMLFile_Path,const String& _platform, bbool _raiseError, bbool _bSaveBin);

        static bbool       fillAnimTrackResources(  AnimationDependenciesMap * dependenciesMap, const Path& _XMLFile_Path, bbool _raiseError);
        static bbool       fillDependencies(AnimationDependenciesMap * &_mapDep, AnimationDependencies * & _dep, const String & _anim, const Path & _dependencyFile, ResourceGroup *_group);

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS) && defined(ITF_DEBUG_STRINGID)
        static void        writeAnimTrack(AnimTrack *_track, const char *_path);
#endif
    };

}


#endif //_ITF_ANIMATIONLOADTRACK_H_
