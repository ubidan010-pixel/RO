#ifndef _ITF_ACTORCREATE_ANIMLIGHT_H_
#define _ITF_ACTORCREATE_ANIMLIGHT_H_

#ifndef _ITF_ACTORCREATEPLUGIN_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreatePlugin.h"
#endif //_ITF_ACTORCREATEPLUGIN_H_

namespace ITF
{
    class ActorCreate_AnimLight : public ActorCreateSpProcess
    {
    public:
    //    static char *               GetComponentNameStatic() { return "AnimLightComponent"; }
    //    virtual char *              GetComponentName() { return GetComponentNameStatic(); };
    //    void                        ReceiveMsg(class Blob& _blob);
    //    virtual void                ReceiveActorReload();

    //    
    //    static const char*          getRecvGetSubAnimList()                 {return "getSubAnimList";}
    //    static const char*          getRecvGetSubAnim()                     {return "getSubAnim";}
    //    static const char*          getRecvSetSubAnimAnimation()            {return "setSubAnimAnimation";}

    //    static const char*          getSendGetSubAnimList()                 {return "SubAnimList";}
    //    static const char*          getSendGetSubAnim()                     {return "SubAnim";}

    //protected:
    //    const class  SubAnim_Template*    getSubAnim(int _index);
    //    
    //    void                        onGetSubAnimList(class Blob& _blob);
    //    void                        onGetSubAnim(class Blob& _blob);
    //    void                        onSetSubAnimAnimation(class Blob& _blob);

    //    void                        fillHeader(class Blob& _blob);
    //    void                        sendSubAnimList(class AnimLightComponent_Template *_animComp);
    //    void                        sendSubAnim(int index, class SubAnim_Template *subAnim);
    };
}

#endif //_ITF_ACTORCREATE_ANIMLIGHT_H_