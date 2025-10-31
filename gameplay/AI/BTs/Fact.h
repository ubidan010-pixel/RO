////////////////////////////////////////////////////////////////////////////////
//
// Borrowed from Dunia Engine
//
// Description: A fact is a placeholder to be part of terms/blackboards.
//              Used for the inter-agent coordination mechanism.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_FACT_H_
#define _ITF_FACT_H_

namespace ITF
{

struct EventPtr
{
    EventPtr() : m_event(NULL) {}
    EventPtr( class Event* _event ) : m_event(_event) { }
    class Event* m_event;
};

enum EValueType
{
    // WARNING, this enum should match the BlackBox file: BlackBox2/Elemental/Plugins/Parameters/Parameter.cs
    EValueType_Unknown = 0,
    EValueType_Boolean,
    EValueType_Integer32,
    EValueType_UInteger32,
    EValueType_Float,
    EValueType_StringId,
    EValueType_Vec2,
    EValueType_Vec3,
    EValueType_ObjectRef,
    EValueType_Event,
    EValueType_COUNT
};

template <typename T> struct ValueTypeResolver { static EValueType Get() { return EValueType_Unknown; } };
#define DEFINE_VALUETYPE(type, valueType) template <> struct ValueTypeResolver<type> { static EValueType Get() { return valueType; } };
DEFINE_VALUETYPE(bool, EValueType_Boolean);
DEFINE_VALUETYPE(i32, EValueType_Integer32);
DEFINE_VALUETYPE(u32, EValueType_UInteger32);
DEFINE_VALUETYPE(f32, EValueType_Float);
DEFINE_VALUETYPE(StringID, EValueType_StringId);
DEFINE_VALUETYPE(Vec2d, EValueType_Vec2);
DEFINE_VALUETYPE(Vec3d, EValueType_Vec2);
DEFINE_VALUETYPE(ObjectRef, EValueType_ObjectRef);
DEFINE_VALUETYPE(EventPtr, EValueType_Event);
template <typename T> EValueType ComputeValueType() { return ValueTypeResolver<T>::Get(); }

#define SERIALIZE_FACTVALUETYPE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(EValueType_Boolean); \
        SERIALIZE_ENUM_VAR(EValueType_Integer32); \
        SERIALIZE_ENUM_VAR(EValueType_UInteger32); \
        SERIALIZE_ENUM_VAR(EValueType_Float); \
        SERIALIZE_ENUM_VAR(EValueType_StringId); \
        SERIALIZE_ENUM_VAR(EValueType_Vec2); \
        SERIALIZE_ENUM_VAR(EValueType_Vec3); \
        SERIALIZE_ENUM_VAR(EValueType_ObjectRef); \
        SERIALIZE_ENUM_VAR(EValueType_Event); \
    SERIALIZE_ENUM_END();

class BaseFact
{
public:

	virtual ~BaseFact() {}
	virtual BaseFact* Clone() const = 0;

    EValueType GetValueType() { return m_valueType; }
    virtual void* GetDataPtr() { return NULL; }

protected:

	BaseFact()	: m_valueType(EValueType_Unknown) {}

	EValueType     m_valueType;
};

template <typename T>
class Fact : public BaseFact
{
	friend class Blackboard;

protected:
	Fact() 
	{
		m_valueType = ComputeValueType<T>(); 
}	
public:
	const T& getData() const
	{
		return m_data;
	}

	T& getData()
	{
		return m_data;
	}

	void setData(const T& data)
	{
		m_data = data;
	}

	bbool isEqualTo(const T& val)
	{
		return (m_data == val);
	}

	BaseFact* Clone() const
	{
		return newAlloc(mId_Gameplay, Fact(*this) );
	}

	static BaseFact* CreateObject(const StringID&) 
	{ 
		return newAlloc( mId_Gameplay, Fact<T> );
	}

    virtual void* GetDataPtr() { return &m_data; }

private:

	T m_data;
};

}

#endif // #ifndef _ENGINESERVICES_FACT_H_
