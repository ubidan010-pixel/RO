#include "precompiled_engine.h"

#ifndef _ITF_PLAYTEXT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayText_evt.h"
#endif //_ITF_PLAYTEXT_EVT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlayText_evt)

PlayText_evt::PlayText_evt()
: m_index(U32_INVALID)
{
}

SequenceEvent * PlayText_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlayText_evt *pNew    = new PlayText_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void PlayText_evt::init(const Vec3d& _pos)
{
}

void PlayText_evt::forceCurrentFrame(   i32 _frame  )
{
    PlayText_evtTemplate *evtTemplate = getTemplate();
    ITF_UNUSED(evtTemplate);
    ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));
}

void PlayText_evt::onBecomeActive()
{
    Super::onBecomeActive();
    Actor* actor = getBindedActor();
    if (!actor)
        return;

    SimpleTextComponent * actorSimpleText = actor->GetComponent<SimpleTextComponent>();
    if (!actorSimpleText)
        return;

    BinaryClone((SimpleTextComponent *)&getTemplate()->getParams(), actorSimpleText, 
        (ESerializeFlags)(ESerialize_PropertyEdit_Save | ESerialize_Data_Save), 
        (ESerializeFlags)(ESerialize_PropertyEdit_Load | ESerialize_Data_Load));

    actorSimpleText->clearInstanceData(actor);
}

void PlayText_evt::onBecomeInactive()
{
    Super::onBecomeInactive();

    if (m_index == U32_INVALID)
        return;

    Actor* actor = getBindedActor();
    if (!actor)
        return;

    SimpleTextComponent * actorSimpleText = actor->GetComponent<SimpleTextComponent>();
    if (!actorSimpleText)
        return;

    actorSimpleText->setText(String::emptyString);
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(PlayText_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlayText_evtTemplate)
    SERIALIZE_MEMBER("Text",m_text);
    SERIALIZE_OBJECT("Params",m_textParams);
END_SERIALIZATION()

PlayText_evtTemplate::PlayText_evtTemplate()
{
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   PlayText_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlayText_evtTemplate* pNew = new PlayText_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlayText_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
    serializeFriendlyToBlob(_blob);

    // text
    PathString_t textName;
    m_text.getString(textName);
    _blob->pushString(textName);

    SerializerToolXML serializer;
    char* buffer = NULL;
    uSize size = 0;

    serializer.OpenToWrite(&buffer,&size);
    m_textParams.Serialize(&serializer, (ESerializeFlags)(ESerialize_PropertyEdit_Save | ESerialize_Data_Save));
    serializer.Close();
    _blob->pushString(buffer);

	if(size)
		SF_DEL_ARRAY(buffer);
}

void PlayText_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);

    // text
    String text_str;
    _blob->extractString(text_str);
    m_text = Path(text_str);

    String params;
    _blob->extractString(params);

    StringConverter converted(params);
    SerializerToolXML serializer;
    serializer.OpenToRead(converted.getChar());
    m_textParams.Serialize(&serializer, (ESerializeFlags)(ESerialize_PropertyEdit_Load | ESerialize_Data_Load));
    serializer.Close();
}
#endif  //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


