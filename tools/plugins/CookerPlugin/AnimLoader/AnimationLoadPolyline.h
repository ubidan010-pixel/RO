#ifndef _ITF_ANIMATIONLOADPOLYLINE_H_
#define _ITF_ANIMATIONLOADPOLYLINE_H_

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_

namespace ITF
{
    class AnimPolylineBank;
    class AnimPolyline;

    class AnimationLoadPolyline
    {
    public:
        static AnimPolylineBank * readAnimPolylineBank(  const Path& _szXMLFile,const String& _szPlatform, bbool _raiseError, bbool _bSaveBin);

    private:
        static bbool              xmlReadAnimPolyline(PugiXMLWrap * _XML, AnimPolyline* _polyline, PugiXMLWrap::ErrorBehavior errBehavior);
    };

};


#endif //_ITF_ANIMATIONLOADPOLYLINE_H_