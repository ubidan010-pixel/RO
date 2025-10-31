// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)


#ifndef _ITF_ACTORCREATE_ANIMLIGHT_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_AnimLight.h"
#endif //_ITF_ACTORCREATE_ANIMLIGHT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_


namespace ITF
{
/*    const SubAnim_Template* ActorCreate_AnimLight::getSubAnim(int _index)
    {
        Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
        if (!actor)
            return NULL;

        AnimLightComponent_Template *animComp = actor->GetComponent<AnimLightComponent_Template>();
        ITF_ASSERT(animComp);
        if (!animComp)
            return NULL;


        if(_index < animComp->getSubAnimTemplate()->getSubAnimList().size())
            return &animComp->getSubAnimTemplate()->getSubAnimList()[_index];
        else 
            return NULL;
    }


    void ActorCreate_AnimLight::ReceiveMsg(class Blob& _blob)
    {
        String message;
        _blob.extractString(message);

        if ( message == getRecvGetSubAnimList() )
        {
            onGetSubAnimList(_blob);
        }
        else if ( message == getRecvGetSubAnim() )
        {
            onGetSubAnim(_blob);
        }
        else if ( message == getRecvSetSubAnimAnimation() )
        {
            onSetSubAnimAnimation(_blob);
        }
    }

    void ActorCreate_AnimLight::fillHeader(class Blob& _blob)
    {
        _blob.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        _blob.pushString(GetSpViewMessage());
        _blob.pushString(GetComponentName());
    }


    void ActorCreate_AnimLight::onGetSubAnimList(class Blob& _blob)
    {
        Actor_Template *actor = m_actorCreatePlugin->getActorTemplate();
        if (!actor)
            return;

        AnimLightComponent_Template *animComp = actor->GetComponent<AnimLightComponent_Template>();
        ITF_ASSERT(animComp);
        if (!animComp)
            return;

        sendSubAnimList(animComp);
    }

    void ActorCreate_AnimLight::onGetSubAnim(class Blob& _blob)
    {
        int index = _blob.extractInt32();
        const SubAnim_Template* subAnim = getSubAnim(index);

        sendSubAnim(index, subAnim);
    }


    void ActorCreate_AnimLight::onSetSubAnimAnimation(class Blob& _blob)
    {
        int index;
        String path, relativePath;
        
        index = _blob.extractInt32();
        _blob.extractString(path);

        const SubAnim_Template* subAnimTemplate = getSubAnim(index);

        path = FilePath::normalizePath(path);
        if (!FILESERVER->getPathFromDataFolder(relativePath, path))
            return;


        //const_cast<SubAnim_Template>(
        subAnimTemplate->setAnimPath(relativePath);

        sendSubAnim(index, subAnimTemplate);
    }

    void ActorCreate_AnimLight::sendSubAnimList(AnimLightComponent_Template *_animComp)
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendGetSubAnimList());

        const SubAnimSet_Template*subAnimSet = _animComp->getSubAnimTemplate();
        int count = subAnimSet->getSubAnimList().size();
        b.pushInt32(count);
        for (int i=0; i<count; i++)
        {
#ifdef ITF_DEBUG_STRINGID
            b.pushString(subAnimSet->getSubAnimList()[i].getFriendlyName().getDebugString());
#else
            b.pushString("No StringID strings");
#endif
        }
        PLUGINGATEWAY->send(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }



    void ActorCreate_AnimLight::sendSubAnim(int index, SubAnim_Template* _subAnim)
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        fillHeader(b);
        b.pushString(getSendGetSubAnim());
        b.pushInt32(index);

        const char *animPath="Unknown anim path!";
#ifdef ITF_DEBUG_STRINGID
        //animPath = _subAnim->getTrackName().getDebugString();
#endif
        b.pushString(animPath);
        PLUGINGATEWAY->send(b, m_actorCreatePlugin->m_editorPeer, m_actorCreatePlugin->m_editorSenderID);
    }
    */
};

#endif //!ITF_FINAL