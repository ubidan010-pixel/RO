#ifndef _ITF_ANIMATIONLOADPATCHBANK_H_
#define _ITF_ANIMATIONLOADPATCHBANK_H_

#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#include "tools/plugins/CookerPlugin/AnimLoader/AnimationLoadCommon.h"
#endif //_ITF_ANIMATIONLOADCOMMON_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

namespace ITF
{
    class AnimPatchBank;
    class AnimTemplate;
    
    class AnimationLoadPatchBank
    {
    public:
        static AnimPatchBank * readAnimPatchBank(  const Path& _XMLFile_Path,const String& _platform, bbool _raiseError, bbool _bSaveBin);

    private:
        static bbool           xmlReadTemplateBoneListT(  PugiXMLWrap * _XML, AnimTemplate* _template, PtrToStrID * _ptrToStrID, NamesMap * _boneIDToIndex, PugiXMLWrap::ErrorBehavior errBehavior);
        static bbool           xmlReadAnimPatchPoints(PugiXMLWrap * _XML, AnimTemplate* _template, PtrToStrID * _ptrToStrID, NamesMap * _boneIDToIndex, PugiXMLWrap::ErrorBehavior errBehavior);

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)
    public:
        static void            writeAnimPatchBank(AnimPatchBank *_pbk, AnimSkeleton *_skeleton, const char *_path);

    private:

        static void            xmlWriteTemplateBoneList(pugi::xml_node _parentNode, AnimTemplate *_template, AnimSkeleton *_skeleton);
        static void            xmlWriteAnimPatchPoints(pugi::xml_node _parentNode, AnimTemplate *_template, const String8 &_templateName);
#endif
    };

};


#endif //_ITF_ANIMATIONLOADPATCHBANK_H_
