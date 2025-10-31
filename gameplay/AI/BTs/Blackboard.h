////////////////////////////////////////////////////////////////////////////////
//
// Borrowed from Dunia Engine
//
// Description: A blackboard is an inter-object coordination mechanism.
//              Contains fact, list of facts and/or other blackboards.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_BLACKBOARD_H_
#define _ITF_BLACKBOARD_H_

#ifndef _ITF_FACT_H_
#include "gameplay/ai/BTs/Fact.h"
#endif // _ITF_FACT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

namespace ITF
{

class Blackboard
{
public:
	Blackboard() {};
	~Blackboard()
	{
		clear();
	};

	// removes every facts from the blackboard
	ITF_INLINE void clear();

	// return true if fact exists
	ITF_INLINE bbool factExists(const StringID& _id);

	// if fact does not exist, creates a new fact.
	// use permanent for best performance when it is the case
	template <typename T>
	ITF_INLINE BaseFact* setFact(const StringID& _id, const T& _in_fact);

	// returns the first fact item 
	template <typename T>
	inline bbool getFact(const StringID& _id, T& _out_fact);
    ITF_INLINE BaseFact* getFact(const StringID& _id);

	// returns the first fact item 
	template <typename T>
	ITF_INLINE T& getFactReference(const StringID& _id);

	// remove fact from blackboard
	ITF_INLINE void removeFact(const StringID& _id);

	ITF_INLINE bbool moveFact(const StringID& _oldID, const StringID& _newID);
	ITF_INLINE bbool copyFact(const StringID& _id, const StringID& _copy);

	static void registerFacts();
	static void unRegisterFacts();

	typedef ITF_MAP<StringID, BaseFact*> FactsMap;

	FactsMap::iterator beginIterator() { return m_facts.begin(); }
	FactsMap::iterator endIterator() { return m_facts.end(); }
private:

	FactsMap m_facts;

	template <typename T>
	ITF_INLINE BaseFact* setInternalFact(const StringID& _id, const T& _in_fact);

    template <typename T>
	ITF_INLINE void createNewFact(const StringID& _id, Fact<T>** _ret );

    ITF_INLINE void freeFact( BaseFact* _fact );
};

void Blackboard::clear()
{
	// to avoid re-entrance.
	FactsMap::iterator i = m_facts.begin();
	FactsMap::iterator end = m_facts.end();

	for ( ; i != end; ++i)
	{
        freeFact((*i).second);
	}

	m_facts.clear();
}

bbool Blackboard::factExists(const StringID& _id)
{
	ITF_ASSERT_MSG(_id.isValid(), "Trying to verify is a fact exists with an invalid fact name");
	FactsMap::iterator iter(m_facts.find(_id));
	if (m_facts.end() == iter)
	{
		return bfalse;
	}
	return btrue;
}

template <typename T>
void Blackboard::createNewFact(const StringID& _id, Fact<T>** _ret )
{
	*_ret = newAlloc(mId_Gameplay,Fact<T>);
	m_facts[_id] = *_ret;
}

template <typename T>
BaseFact* Blackboard::setInternalFact(const StringID& _id, const T& _in_fact)
{
	ITF_ASSERT_MSG(_id.isValid(), "Trying to set a fact with an invalid fact name");
	
    Fact<T>* typeFact;

	FactsMap::iterator iter(m_facts.find(_id));

	if (iter != m_facts.end())
	{
		typeFact = static_cast< Fact<T>* >(iter->second);

        if ( typeFact->GetValueType() == EValueType_Event )
        {
            Fact <EventPtr>* eventFact = static_cast<Fact <EventPtr>*>(iter->second);
            SF_DEL(eventFact->m_data.m_event);
        }
	}
	else
	{
		createNewFact<T>(_id,&typeFact);
	}

    ITF_ASSERT(typeFact->GetValueType()!=EValueType_Unknown);

	typeFact->setData(_in_fact);

    return typeFact;
}

void Blackboard::freeFact( BaseFact* _fact )
{
    if ( _fact->GetValueType() == EValueType_Event )
    {
        Fact<EventPtr>* fact = static_cast<Fact<EventPtr>*>(_fact);
        EventPtr& data = fact->getData();
        SF_DEL(data.m_event);
    }

    SF_DEL(_fact);
}

template <typename T>
BaseFact* Blackboard::setFact(const StringID& _id, const T& _in_fact)
{
	return setInternalFact(_id, _in_fact);
}

template <>
ITF_INLINE BaseFact* Blackboard::setFact<f32>(const StringID& _id, const f32& _in_float)
{
	ITF_ASSERT(ITF_finite(_in_float));
	return setInternalFact(_id, _in_float);
}

template <>
ITF_INLINE BaseFact* Blackboard::setFact<Vec2d>(const StringID& _id, const Vec2d& _in_vec)
{
	ITF_ASSERT(ITF_finite(_in_vec.m_x)&&ITF_finite(_in_vec.m_y));
	return setInternalFact(_id, _in_vec);
}

template <>
ITF_INLINE BaseFact* Blackboard::setFact<Vec3d>(const StringID& _id, const Vec3d& _in_vec)
{
    ITF_ASSERT(ITF_finite(_in_vec.m_x)&&ITF_finite(_in_vec.m_y)&&ITF_finite(_in_vec.m_z));
    return setInternalFact(_id, _in_vec);
}

template <>
inline BaseFact* Blackboard::setFact<EventPtr>(const StringID& _id, const EventPtr& _in_event)
{
    EventPtr ptr;
    ptr.m_event = _in_event.m_event->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
    BinaryClone(const_cast<Event*>(_in_event.m_event),ptr.m_event);
    return setInternalFact(_id, ptr);
}

template <typename T>
bbool Blackboard::getFact(const StringID& _id, T& _out_fact)
{
	ITF_ASSERT_MSG(_id.isValid(), "Trying to retrieve a fact with an invalid fact name");

	FactsMap::iterator iter(m_facts.find(_id));

	if (m_facts.end() == iter)
	{
		return bfalse;
	}

    BaseFact* fact = iter->second;

    if ( fact->GetValueType() != ValueTypeResolver<T>::Get() )
    {
        ITF_ASSERT_MSG(0, "Trying to get a fact with a different value type");
        return bfalse;
    }

	Fact<T>* typeFact = static_cast< Fact<T>* >(fact);

	_out_fact = typeFact->getData();

	return btrue;
}

BaseFact* Blackboard::getFact(const StringID& _id)
{
    FactsMap::iterator iter(m_facts.find(_id));

    if (m_facts.end() == iter)
    {
        return NULL;
    }

    return iter->second;
}

template <typename T>
T& Blackboard::getFactReference(const StringID& _id)
{
	ITF_ASSERT_MSG(_id.isValid(), "Trying to retrieve a fact with an invalid fact name");

	Fact<T>* typeFact;
	auto iter = m_facts.find(_id);

	if (m_facts.end() == iter)
	{
		typeFact = createNewFact<T>(_id);
	}
	else
	{
		typeFact = static_cast< Fact<T>* >(iter->second);
	}
		return typeFact->GetData();
	}

void Blackboard::removeFact(const StringID& _id)
{
	ITF_ASSERT_MSG(_id.isValid(), "Trying to remove a fact with an invalid fact name");

	FactsMap::iterator iter(m_facts.find(_id));

	if (iter == m_facts.end())
	{
		return;
	}

	// to support remove fact re-entrance.
	BaseFact* baseFact = iter->second;

	m_facts.erase(iter);

	freeFact(baseFact);
}

bbool Blackboard::moveFact(const StringID& _oldID, const StringID& _newID)
{
	ITF_ASSERT_MSG(_oldID.isValid() && _newID.isValid(), "Trying to move facts with an invalid facts name");

	FactsMap::iterator iter(m_facts.find(_oldID));

	if (m_facts.end() == iter)
	{
		return bfalse;
	}

    BaseFact* fact = iter->second;

    m_facts.erase(iter);
	m_facts[_newID] = fact;

	return btrue;
}

bbool Blackboard::copyFact(const StringID& _id, const StringID& _copy)
{
	ITF_ASSERT_MSG(_id.isValid() && _copy.isValid(), "Trying to copy facts with an invalid facts name");

	if(_id == _copy)
    {
		return btrue;
    }

	FactsMap::iterator iter(m_facts.find(_id));

	if (m_facts.end() == iter)
	{
		return bfalse;
	}

	FactsMap::iterator iterCopy(m_facts.find(_copy));

	if (m_facts.end() != iterCopy)
	{
		freeFact(iterCopy->second);
		iterCopy->second = iter->second->Clone();
	}
	else
	{
		m_facts[_copy] = iter->second->Clone();
	}

	return btrue;
}

} // ITF

#endif // _ITF_BLACKBOARD_H_
