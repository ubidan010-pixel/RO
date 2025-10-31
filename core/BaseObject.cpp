#include "precompiled_core.h"

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDServer.h"
#endif //_ITF_OBJECTID_SERVER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BaseObject)

    BEGIN_SERIALIZATION(BaseObject)
        SERIALIZE_MEMBER("OBJECTID",m_id);
        SERIALIZE_FUNCTION(postload,ESerialize_Data_Load);
    END_SERIALIZATION()

void BaseObject::getObjectId(String8& _dest) const
{
    char buf[256];
	SPRINTF_S(buf,256,"%016zx", getObjectId().m_id);
    _dest = buf;
}

void ObjectId::toString(char* _str, size_t _size) const
{
	SPRINTF_S(_str, _size,"%016zx", m_id);
}

void BaseObject::postload()
{
    ITF_ASSERT_MSG(m_id.isValid(), "Serialization assigned bad ObjectID");
}

bbool BaseObject::isCellObjectType(   )
{   // this method should not be set as virtual. Keeping the list here is better for visibility and secures unregisterFromWorldCells().
    switch (m_ObjectType)
    {
    case eFrise:
    case eActor:
    case eEditorDummy:
        return btrue;
    default:
        return bfalse;
    }
}

} // namespace ITF
