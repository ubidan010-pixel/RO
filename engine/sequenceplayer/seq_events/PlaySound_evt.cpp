#include "precompiled_engine.h"

#ifndef _ITF_PLAYSOUND_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySound_evt.h"
#endif //_ITF_PLAYSOUND_EVT_H_


#include "engine/actors/components/SoundComponent.h"

#include "engine/sound/SoundDescriptor.h"

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H


namespace ITF
{

PlaySound_evt::PlaySound_evt()
: m_soundHandle(SoundHandle::getInvalidHandle())
{
}

SequenceEvent * PlaySound_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlaySound_evt *pNew    = newAlloc(mId_Gameplay, PlaySound_evt);
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void PlaySound_evt::init(const Vec3d& _pos)
{
}

void PlaySound_evt::forceCurrentFrame(   i32 _frame  )
{
    PlaySound_evtTemplate *evtTemplate = getTemplate();
    ITF_UNUSED(evtTemplate);
    ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));
}


void PlaySound_evt::forceStop()
{
    if (!m_soundHandle.isValid())
        return;

    Actor* actor = getBindedActor();
    if (!actor)
        return;

    SoundComponent* soundComp = actor->GetComponent<SoundComponent>();            
    if (!soundComp)       
        return;

    soundComp->stopSound(m_soundHandle);
    m_soundHandle.invalidate();
}

void PlaySound_evt::onBecomeActive()
{
    Super::onBecomeActive();
    Actor* actor = getBindedActor();
    if (!actor)
        return;

    SoundComponent* soundComp = actor->GetComponent<SoundComponent>();            
    if (!soundComp)       
        return;

    PathString_t soundName;

    getTemplate()->getSoundPath().getString(soundName);

    m_soundHandle = soundComp->playSound(FilePath::getFilenameWithoutExtension(soundName));
}

void PlaySound_evt::onBecomeInactive()
{
    Super::onBecomeInactive();

    forceStop();
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(PlaySound_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlaySound_evtTemplate)
    SERIALIZE_MEMBER("Sound",m_sound);
    SERIALIZE_OBJECT("Params",m_soundParams);
    SERIALIZE_MEMBER("Volume",m_volume);
    SERIALIZE_MEMBER("Category",m_category);
    SERIALIZE_MEMBER("IsStrem",m_isStream);
    SERIALIZE_MEMBER("WwiseEventGuid", m_wwiseEventGuid);
END_SERIALIZATION()

PlaySound_evtTemplate::PlaySound_evtTemplate()
: m_volume(0)
, m_category("SFX")
, m_isStream(bfalse)
{
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   PlaySound_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlaySound_evtTemplate* pNew = newAlloc(mId_Gameplay, PlaySound_evtTemplate);
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlaySound_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
    serializeFriendlyToBlob(_blob);

    // sound
    PathString_t soundName;
    m_sound.getString(soundName);
    _blob->pushString(soundName);

    SerializerToolXML serializer;
    char* buffer = NULL;
    uSize size = 0;

    serializer.OpenToWrite(&buffer, &size);
    m_soundParams.Serialize(&serializer, (ESerializeFlags)(ESerialize_PropertyEdit_Save | ESerialize_Data_Save));
    serializer.Close();
    _blob->pushString(buffer);

	if(size)
		SF_DEL_ARRAY(buffer);

    _blob->pushFloat32(m_volume);
    _blob->pushString8(m_category);
    _blob->pushBool(m_isStream);
}

void PlaySound_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);

    // sound
    String sound_str;
    _blob->extractString(sound_str);
    m_sound = Path(sound_str);

    String params;
    _blob->extractString(params);

    StringConverter converted(params);
    SerializerToolXML serializer;
    serializer.OpenToRead(converted.getChar());
    m_soundParams.Serialize(&serializer, (ESerializeFlags)(ESerialize_PropertyEdit_Load | ESerialize_Data_Load));
    serializer.Close();

    m_volume = _blob->extractFloat32();
    _blob->extractString8(m_category);
    m_isStream = _blob->extractBool();
}
#endif  //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


