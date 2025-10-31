#include "precompiled_engine.h"

#ifndef _ITF_PLAYSPAWN_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySpawn_evt.h"
#endif //_ITF_PLAYSPAWN_EVT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#ifndef _ITF_PLAYSOUND_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySound_evt.h"
#endif //_ITF_PLAYSOUND_EVT_H_

#ifndef _ITF_PLAYTEXT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayText_evt.h"
#endif //_ITF_PLAYTEXT_EVT_H_

#ifndef _ITF_KEEPALIVECOMPONENT_H_
#include "engine/actors/components/KeepAlivecomponent.h"
#endif //_ITF_KEEPALIVECOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlaySpawn_evt)

PlaySpawn_evt::PlaySpawn_evt()
: m_actorSpawnDone(bfalse)
{
}


SequenceEvent * PlaySpawn_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlaySpawn_evt *pNew  = newAlloc(mId_Gameplay, PlaySpawn_evt);
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void PlaySpawn_evt::init(const Vec3d& _pos)
{
}

PlaySpawn_evt::~PlaySpawn_evt() {
    if (m_ownerSequence)
        m_ownerSequence->deleteSpawnActor(getBindedActor());
}




void PlaySpawn_evt::loadActorResources()
{
    Actor* pActor = getBindedActor();
    ITF_ASSERT(pActor);
    if (pActor)
        pActor->loadResources();
}


void PlaySpawn_evt::unloadActorResources()
{
    Actor* pActor = getBindedActor();
    ITF_ASSERT(pActor);
    if (pActor)
        pActor->unloadResources();
}

void    PlaySpawn_evt::forceCurrentFrame(   i32 _time  )
{
    Actor* pActor = getBindedActor();
    if (!pActor || pActor->isEnabled())
        return;

    // actor has been disabled -> recompute a onBecomeActive ...
    if (!m_actorSpawnDone)
        onBecomeActive();
}


void PlaySpawn_evt::onBecomeActive()
{
    Super::onBecomeActive();
    m_actorSpawnDone = bfalse;
    
    Actor* pActor = getBindedActor();

    if(pActor && !pActor->isEnabled())
    {
        m_ownerSequence->controlActor(pActor->getRef(), bfalse);

        if(getTemplate()->getDisableComponentsActor())
        {
            EventSequenceActorActivate sequenceEventActivate(btrue);
            pActor->onEvent(&sequenceEventActivate);
        }
        pActor->enable();
        m_actorSpawnDone = btrue;
    }
}

void PlaySpawn_evt::onBecomeInactive()
{
    Super::onBecomeInactive();
    m_actorSpawnDone = bfalse;

    Actor* pActor = getBindedActor();

    if(pActor && !getTemplate()->isStay())
    {
        if (pActor->getComponentFromStaticClassCRC(ITF_GET_STRINGID_CRC(KeepAliveComponent,4149685283)))
        {
            EventDie eventDie(m_ownerSequence->isEditionMode());
            pActor->onEvent(&eventDie);
        } else
        {
            m_ownerSequence->releaseActor(pActor->getRef(), bfalse);

            pActor->disable();
            if (!m_ownerSequence->isEditionMode())
            {
                unloadActorResources();
            }
        }
    }
}

void PlaySpawn_evt::spawnActor()
{
    PlaySpawn_evtTemplate * evtTemplate = getTemplate();
    if (m_ownerSequence->spawnActor(evtTemplate->getObjectPath(),
                                    evtTemplate->getObjectID(),
                                    evtTemplate->getFileName(),
                                    evtTemplate->getSpawnOffset().to3d(),
                                    evtTemplate->getFlipped()))
        linkBindedActor();
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(PlaySpawn_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlaySpawn_evtTemplate)
    SERIALIZE_MEMBER("Visible",m_visible);
    SERIALIZE_MEMBER("Stay",m_stay);
    SERIALIZE_MEMBER("FileName",m_fileName);
	SERIALIZE_MEMBER("SpawnOffset",m_spawnOffset);
	SERIALIZE_MEMBER("Flipped",m_flipped);
END_SERIALIZATION()

PlaySpawn_evtTemplate::PlaySpawn_evtTemplate()
: m_stay(bfalse)
, m_visible(btrue)
, m_fileName("")
, m_spawnOffset(1.0, 1.0)
, m_flipped(false)
{

}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   PlaySpawn_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlaySpawn_evtTemplate* pNew = newAlloc(mId_Gameplay, PlaySpawn_evtTemplate);
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}


void PlaySpawn_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
    serializeFriendlyToBlob(_blob);

    _blob->pushBool  ( m_visible );
    _blob->pushBool  ( m_stay );
    _blob->pushString( m_fileName );
}

void PlaySpawn_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);

    m_visible = _blob->extractBool();
    m_stay    = _blob->extractBool();

    // file name
    String fileName_str;
    _blob->extractString(fileName_str);
    m_fileName = String8(fileName_str.cStr());

}
#endif //ITF_SUPPORT_BLOBEVENT


} // namespace ITF


