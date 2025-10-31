#ifndef _ITF_ANIMATIONLOADCOMMON_H_
#define _ITF_ANIMATIONLOADCOMMON_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

class TiXmlNode;
class TiXmlElement;

namespace ITF
{
    typedef Map<void *, StringID> PtrToStrID;
    class AnimBone;
    class AnimBoneDyn;
    class AnimTemplate;
    class AnimSkeleton;

    class AnimationLoadCommon
    {
    public:
        
        static void    xmlReadBoneDyn(PugiXMLWrap *_pXML, AnimBoneDyn *_pBoneDyn, PugiXMLWrap::ErrorBehavior errBehavior);
        static String  xmlReadBone(PugiXMLWrap *_pXML, AnimBone *_pBone, PtrToStrID *_ptrToStrID,  PugiXMLWrap::ErrorBehavior errBehavior);

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID) && defined(ITF_WINDOWS)
        static pugi::xml_node   xmlWritePointer(pugi::xml_node _parentNode, const char *_name, void *_ptr);
        static pugi::xml_node   xmlWriteString(pugi::xml_node _parentNode, const char *_name, const char *_string);
        static pugi::xml_node   xmlWriteBool(pugi::xml_node _parentNode, const char *_name, bbool _value);
        static pugi::xml_node   xmlWriteFloat(pugi::xml_node _parentNode, const char *_name, f32 _value);
        static pugi::xml_node   xmlWriteVector(pugi::xml_node _parentNode, const char *_name, const Vec2d &_vec);
        static void       xmlWriteBone(pugi::xml_node _parentNode, AnimBone *_bone, AnimSkeleton *_skeleton);
        static void       xmlWriteBoneDyn(pugi::xml_node _parentNode, AnimBoneDyn *_bone, AnimTemplate *_template);
#endif

    };

};


#endif //_ITF_ANIMATIONLOADCOMMON_H_
