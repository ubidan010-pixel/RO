#include "precompiled_engine.h"

#ifndef _ITF_REDIRECTPATCHBANK_EVT_H_
#include "engine/sequenceplayer/seq_events/RedirectPatchBank_evt.h"
#endif //_ITF_REDIRECTPATCHBANK_EVT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

namespace ITF
{

RedirectPatchBank_evt::RedirectPatchBank_evt()
: m_index(U32_INVALID)
{
    ITF_UNUSED(m_index);
}

SequenceEvent * RedirectPatchBank_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    RedirectPatchBank_evt *pNew    = new RedirectPatchBank_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void RedirectPatchBank_evt::init(const Vec3d& _pos)
{
}

void RedirectPatchBank_evt::forceCurrentFrame(   i32 _frame  )
{
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
BEGIN_SERIALIZATION(BankChange_TemplateList)
SERIALIZE_CONTAINER_OBJECT("redirectList",m_bankList);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(RedirectPatchBank_evtTemplate)

BEGIN_SERIALIZATION_CHILD(RedirectPatchBank_evtTemplate)
    SERIALIZE_OBJECT("banks",m_bankChangeList);
END_SERIALIZATION()

RedirectPatchBank_evtTemplate::RedirectPatchBank_evtTemplate()
{
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   RedirectPatchBank_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    RedirectPatchBank_evtTemplate* pNew = new RedirectPatchBank_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void RedirectPatchBank_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
    serializeFriendlyToBlob(_blob);

    SerializerToolXML serializer;
    char* buffer = NULL;
    uSize size = 0;

    if (!m_bankChangeList.m_bankList.size())
        m_bankChangeList.m_bankList.push_back(BankChange_Template());

    serializer.OpenToWrite(&buffer, &size);
    m_bankChangeList.Serialize(&serializer, ESerialize_PropertyEdit_Save);
    serializer.Close();
    _blob->pushString(buffer);

	if(size)
		SF_DEL_ARRAY(buffer);
}

void RedirectPatchBank_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);

    // text
    String params;
    _blob->extractString(params);

    StringConverter converted(params);
    SerializerToolXML serializer;

    serializer.OpenToRead(converted.getChar());
    m_bankChangeList.Serialize(&serializer, ESerialize_PropertyEdit_Load);
    serializer.Close();
}
#endif  //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


