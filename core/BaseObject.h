#ifndef _ITF_BASEOBJECT_H_
#define _ITF_BASEOBJECT_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H   

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDServer.h"
#endif //_ITF_OBJECTID_SERVER_H_

namespace ITF
{

#define GOOD_BASEREF 0xBBAAFFDD

#define CHECKPOINTER(p) if (p->m_CheckValidate != GOOD_BASEREF) _debugbreak();

/**
 * BaseObject is the base class for all objects that can be referenced by an
 * ObjectRef or an ObjectID.
 */
class BaseObject : public IRTTIObject
{
public:
    /** Declare serialization methods and allow child classes to use serialization */
    DECLARE_SERIALIZE_VIRTUAL()   

    /** Custom RTTI */
    DECLARE_OBJECT_CHILD_RTTI(BaseObject,IRTTIObject,3217444603)

    /** Object types returned by getObjectType(). */
    enum ObjectType
    {
        ePolyLine = 0,
        eRessource,
        eFrise,
        eActor,
        eTrajectory,
        eScene,
        eSelection,
        eEditorDummy,

        /** eCount & eInvalid MUST remain the last entries in the ObjectType enum */
        eCount,
        eInvalid = eCount,
        ENUM_FORCE_SIZE_32(0)
    };


                                BaseObject          (   );
                                BaseObject          (   const BaseObject& _other    );
    virtual                     ~BaseObject         (   );


    ITF_INLINE  ObjectRef       getRef              (   )   const { return m_reference; }
    ITF_INLINE  ObjectType      getObjectType       (   )   const { ITF_ASSERT((u32)m_ObjectType  < eCount); return m_ObjectType; }

    ITF_INLINE  void            forceObjectId       (   const ObjectId& _id ) { m_id.set(_id);}
    ITF_INLINE  const ObjectId& getObjectId         (   ) const { return m_id; }
                void            getObjectId         (   String8& _dest  ) const;
    virtual     void            regenerateObjectId  (   );
                bbool           isCellObjectType    (   );

#ifndef ITF_FINAL
    u32             m_CheckValidate;
#endif //ITF_FINAL

protected:
    ITF_INLINE  void            setObjectType       (   ObjectType _type    ) { m_ObjectType = _type; }

private:

                
                ObjectRef       m_reference;
                ObjectId        m_id;
                ObjectType      m_ObjectType;
                void            postload            (   );
};

ITF_INLINE BaseObject::BaseObject()
{
    m_ObjectType = eInvalid;

#ifndef ITF_FINAL
    m_CheckValidate = GOOD_BASEREF;
#endif  //ITF_FINAL

    // generate default ObjectID. This ID may be overwritten by a serialized ID for this object.
    m_id.set(OBJECTID_SERVER->generateObjectId());
    // register runtime ref
    m_reference = ID_SERVER->registerObject(this);
}

ITF_INLINE BaseObject::BaseObject(const BaseObject& _other)
{
    m_ObjectType = _other.getObjectType();

#ifndef ITF_FINAL
    m_CheckValidate = GOOD_BASEREF;
#endif //ITF_FINAL

    // generate default ObjectID. This ID may be overwritten by a serialized ID for this object.
    m_id.set(OBJECTID_SERVER->generateObjectId());

    // register runtime ref
    m_reference = ID_SERVER->registerObject(this);
}

ITF_INLINE void BaseObject::regenerateObjectId()
{
    m_id.set(OBJECTID_SERVER->generateObjectId());
}

ITF_INLINE BaseObject::~BaseObject()
{
#ifndef ITF_FINAL
    m_CheckValidate = 0xBABEFEED;
#endif //ITF_FINAL

    ITF_ASSERT(ID_SERVER);
    ID_SERVER->unregisterObject(m_reference);
}

ITF_INLINE BaseObject* ObjectRef::getObject() const
{
    return GETOBJECT(getValue());
}

} // namespace ITF



#endif //_ITF_BASEOBJECT_H_

