#ifndef _ITF_HASH_H_
#define _ITF_HASH_H_

//#include "MEMory/MEM_macros.h"
//#include "MEMory/MEM_allocator_class.h"
//#include "TOOls/TOOvector/TOOvector_class.h"
//#include "BASe/BAS_String.h"

#ifndef _ITF_FASTARRAY_H_
#include "core/container/FastArray.h"
#endif //_ITF_FASTARRAY_H_

namespace ITF
{

#ifdef _WIN32_
#pragma warning (push)
#pragma warning (disable : 4251)
#endif // _WIN32_


// Used to iterate over the hash table
typedef u32 HashIterator;

// Invalid iterator value (used to stop iteration)
#define HashIteratorInvalid 0xFFFFFFFF



// Hash function
namespace Hash_Utils
{
	inline u32 GenericHashFunction(int _typeSize, int _keyBytes, const void *_Key);

	template<int typeSize, int keyBytes>
	inline u32 HashFunction(const void *_Key)
	{
		return GenericHashFunction(typeSize, keyBytes, _Key);
	}

	// Specialization for 32 bit keys, 32 bits used
	template<>
	inline u32 HashFunction<4, 4>(const void *_Key)
	{
		// Faster version for 32 bit keys (cf. http://www.concentric.net/~Ttwang/tech/inthash.htm)
		u32 hash = *(u32*)_Key;
		hash = ~hash + (hash << 15); // hash = (hash << 15) - hash - 1;
		hash = hash ^ (hash >> 12);
		hash = hash + (hash << 2);
		hash = hash ^ (hash >> 4);
		hash = (hash + (hash << 3)) + (hash << 11); // hash = hash * 2057;
		hash = hash ^ (hash >> 16);
		return hash;
	};

	template <class Key, class Value>
	class AlwaysTrue
	{
	public:
		bbool operator ()(Key, const Value &)
		{
			return btrue;
		}
	};
}

//  ***********************************************************************************************************************
//  Hash table.
// 
//  Constraints :
//  The keys can have any size (in bytes), but they must be a fixed size (no strings allowed in this implementation).
//  You must provide the table a key that you will never use (_UnusedKey).
//  The keys must know how to be copied ( = ) and compared (!= and ==)
//  The values must know how to be copied ( = ).
// 
//  Services :
//  The table stores the keys and the values, so that you don't need to store the key on the client side.
//  You can add, remove or get values from the table providing a key.
//  You can parse the whole table with iterators (providing that you do not add or remove keys/values while doing so).
// 
//  Performance :
//  The search cost is constant in average, most of the time the searched value will be found in one shot.
//  The memory cost is : table size * (sizeof (key) + sizeof (value)).
// 
//  Resize :
//  The table resizes itself (doubles its size) when it's 75% full, in order to keep good performance.
//  This means that the table is always between 37.5% and 75% full.
//  This resize takes some time so it's better to know the correct size in advance.
// 
//  Parameters passed as in reference :
//  When Key or Value can be passed as parameters, a version with a reference parameter is possible to allow better performance (if Key and Value are structs for example).
//
//  significantKeyBytes is the number of relevant bytes in the Key for hash computation. This is useful with complex
//  keys, when padding is added at the end of the key structure. When used, only the first significantKeyBytes
//  bytes are used. Be sure to have no padding within the first significantKeyBytes bytes of you key (struct of structs for example)!
// 
//  Implementation is strongly inspired by http://en.wikipedia.org/wiki/Hash_table
//  and uses open adressing and linear probing.
//  ***********************************************************************************************************************
#define tTemplateHashDecl template <class Key, class Value, int significantKeyBytes >
#define tTemplateHashInst Hash<Key, Value, significantKeyBytes>
template <class Key, class Value, int significantKeyBytes = sizeof(Key) >
class Hash 
{

    //      -----------------------------------------------------------------------------------------------------------------------
    //      PUBLIC INTERFACE
    //      -----------------------------------------------------------------------------------------------------------------------

public:
    Hash() : mu_ElementNb(0), mu_SizePower(0), mu_HashMask(0), m_UnusedKey()
    {}

    ~Hash() {destroy();}

    // Initialization
    // The size is a power of 2 (2^_u_SizePower).
    // For better performance, it should be greater than the number of values that will be put in the hash table,
    // or the table could be forced to grow (which is not optimal).
    // size is internally set to at least 8 because smaller sizes lead to issues in the search function
    // You also must provide a key that will never be used.
    void create(u32 _u_SizePower, Key _UnusedKey);

    // Remove all elements (but does not destroy the table elements)
    void clear();

    // Free the table
    void destroy(void);

    // Is there a value corresponding to this key in the table ?
    bbool bFind(Key _Key) const {return pGetElement<Key>(_Key) ? btrue : bfalse;}

    // If key exists in table, change value and return false.
    // else add (key/value) in table and return true.
    bbool bAddValue(Key _Key, Value _Value);

    // return the Iterator on the added element
    HashIterator itAddValue(Key _Key, Value _Value);

	// Add a key. The value is a pointer to the key. Returns true if key was not found in hash table.
	bbool bAddKey(Key _Key);

	// If key is not in table, return false
    // else, fill _pValue with value stored in table and return true.
    bbool bGetValue(Key _Key, Value *_pValue) const;

    // If key is not in table, return NULL
    const Value* getPtrValue(const Key _Key) const;
    Value* getPtrValue(const Key _Key);

    // If key is not in table, return HashIteratorInvalid
    HashIterator itFindValue(Key _Key) const;

    // Tries to remove an element.
    // Returns true if the key was in the table and has been removed.
    bbool bRemoveValue(Key _Key);

    // Return the number of valid elements (with a valid key) in table
    u32 uGetElementNb() const {return mu_ElementNb;}
    u32 size() const {return mu_ElementNb;}

	// Return the power of 2 of the hash table.
	u32 uGetSizePower() const {return mu_SizePower;}

    // Tests the table coherency
    bbool bIsTableCoherent() const;

    // ITERATORS :
    // Code sample using iterator :
    // HashIterator it;
    // for(it = hash.GetFirstIterator(); it != HashIteratorInvalid; it = hash.GetNextIterator(it))
    // {
    //      my_value = hash.GetValue(it);
    //      my_key = hash.GetKey(it);
    // }

    // Get the first iterator
    HashIterator GetFirstIterator() const;

    // Get the next iterator
    HashIterator GetNextIterator(HashIterator _iterator) const;

    // Remove value and get next iterator.
    HashIterator RemoveValue(HashIterator _iterator);

    // Given an iterator, get value
    Value GetValue(HashIterator _iterator) const;

    Value & GetValueRef(HashIterator _iterator);
    const Value & GetValueRef(HashIterator _iterator) const ;

    // Given an iterator, get key
    Key GetKey(HashIterator _iterator) const;

	// Apply a function on all the elements of the hash table for which the predicate is true.
	template <class TOOPred , class TOOFunc>
	void Apply(TOOPred &_predicate, TOOFunc &_functor);

	template <class TOOFunc>
	void Apply(TOOFunc &_functor);
	
	// Copy another table
    void CopyFrom(const Hash *_po_Src);
	
	// The algorithm ensures that the hash table is at least 75% as big as the number of elements in it
	// (in order to reduce collisions), by doubling the table size if (number_of_elements > table_size*(3/4))
	bbool bNeedResize() { return ma_Elements.size()*3 < mu_ElementNb*4;}

	// Will need resize after next add
	bbool bSoonNeedResize() { return (ma_Elements.size() + 1)*3 < mu_ElementNb*4;}

	// Increases the size of the hash table (without adding elements)
	void ForceResize() {resize<Key, Value>();}


    //      -----------------------------------------------------------------------------------------------------------------------
    //      PRIVATE
    //      -----------------------------------------------------------------------------------------------------------------------

private: // Internal struct
    struct Element
    {
        Value	m_Value;	// The value corresponding to the key.
        Key		m_Key;		// The key.
    };

private: // METHODS

    // Gets element corresponding to key. Returns NULL if not found.
    template <class K>  const Element *pGetElement(const K _Key) const;
    template <class K>        Element *pGetElement(const K _Key);

    // Add an element to table (does not resize table)
    // Returns true if element did not exist
    template<class K, class V> inline bbool bAddElement(K _Key, V _Value, HashIterator& _iterator);

    // Remove a value
    void RemoveWithoutCheck(HashIterator _i);

    // Hash function, which transforms a key into an index into the hash table.
    // Does not manage collisions.
	template <class K> u32 KeyToIndex(const K &_Key) const;
	
	// Specialization for strings
	template <>
	u32 KeyToIndex<ITF::String>(const ITF::String &_Key) const
	{
		u32 length = _Key.getLen();
		return Hash_Utils::GenericHashFunction(2*length, 2*length, _Key.cStr()) & mu_HashMask;
	}

	template <>
	u32 KeyToIndex<ITF::String8>(const ITF::String8 &_Key) const
	{
		u32 length = _Key.getLen();
		return Hash_Utils::GenericHashFunction(length, length, _Key.cStr()) & mu_HashMask;
	}
	
    // Is the index in the table occupied ?
    bbool bIsOccupied(u32 _u_Index) const {return ma_Elements[_u_Index].m_Key != m_UnusedKey;}

    //void swap( Hash< Key, Value, _InternalAllocator<Element,align> >& other );
    void swap( Hash& other );
    template<class K, class V> void resize();
private : // ATTRIBUTES

    // A key that will never be used (useful to check if an element is "empty" or "occupied")
    Key m_UnusedKey;

    // The element array.
    FastArray<Element> ma_Elements;

    // The number of used elements
    u32 mu_ElementNb;

    // The array's size is 2^mu_SizePower
    u32 mu_SizePower;

    // mu_HashMask = 1-(2^mu_SizePower).
    // It is used to limit the index returned by the hash function.
    u32 mu_HashMask;
};


//  -----------------------------------------------------------------------------------------------------------------------
//  IMPLEMENTATION
//  -----------------------------------------------------------------------------------------------------------------------

tTemplateHashDecl
inline void tTemplateHashInst::create(u32 _u_SizePower, Key _UnusedKey)
{
    //ITF_ASSERT(_u_SizePower>1);
	mu_SizePower = _u_SizePower > 3 ? _u_SizePower : 3;
	u32 uSize = 1<<mu_SizePower;
    mu_HashMask = uSize-1;

    m_UnusedKey = _UnusedKey;

    ma_Elements.resize(uSize);

    clear();
}


tTemplateHashDecl
inline void tTemplateHashInst::clear()
{
    u32 uSize = ma_Elements.size();
    u32 i;
    for (i=0; i<uSize; i++)
        ma_Elements[i].m_Key = m_UnusedKey;

    mu_ElementNb = 0;
}

tTemplateHashDecl
inline void tTemplateHashInst::destroy(void)
{
    ma_Elements.destroy();
    mu_SizePower = 0;
    mu_HashMask = 0;
    mu_ElementNb = 0;
}

// --------------------------------------------------------------------------------------
// bAddValue... members
// --------------------------------------------------------------------------------------
tTemplateHashDecl
inline bbool tTemplateHashInst::bAddValue(Key _Key, Value _Value)
{
    ITF_ASSERT(_Key != m_UnusedKey);

    if (bNeedResize())
    {
        resize<Key, Value>();
    }

    // Add new element.
    HashIterator itUnused;
    if (bAddElement(_Key,_Value, itUnused))
    {
        mu_ElementNb++;
        return btrue;
    }
    else
        return bfalse;
}


// --------------------------------------------------------------------------------------
// itAddValue... members
// --------------------------------------------------------------------------------------
tTemplateHashDecl
inline HashIterator tTemplateHashInst::itAddValue(Key _Key, Value _Value)
{
    ITF_ASSERT(_Key != m_UnusedKey);

    if (bNeedResize())
    {
        resize<Key, Value>();
    }

    // Add new element.
    HashIterator itElement;
    if (bAddElement<Key, Value>(_Key,_Value, itElement))
    {
        mu_ElementNb++;
    }
    return itElement;
}


// Add a key. The value is a pointer to the key
tTemplateHashDecl
inline bbool tTemplateHashInst::bAddKey(Key _Key)
{
	ITF_ASSERT(_Key != m_UnusedKey);

	// Resize is forbidden (it changes the pointers)
	ITF_ASSERT(!bNeedResize());

	u32 uIndex = KeyToIndex<Key>(_Key);
	Element *pElem = &ma_Elements[uIndex];

	// Manage collisions
	for(;;)
	{
		if (pElem->m_Key == m_UnusedKey)
		{
			// Found an empty element -> fill value and key
			pElem->m_Key = _Key;
			pElem->m_Value = &pElem->m_Key;
			mu_ElementNb++;
			return btrue;
		}
		else if (pElem->m_Key == _Key)
		{
			// Found the element
			return bfalse;
		}
		else
		{
			// The element is used, try the next one.
			uIndex = (uIndex+1) & mu_HashMask;
			pElem = &ma_Elements[uIndex];
		}
	}
}


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tTemplateHashDecl
bbool tTemplateHashInst::bGetValue(Key _Key, Value *_pValue) const
{
    ITF_ASSERT(_Key != m_UnusedKey);

    const Element *pElem = pGetElement<Key>(_Key);
    if (pElem)
    {
        *_pValue = pElem->m_Value;
        return btrue;
    }
    else
    {
        return bfalse;
    }
}

tTemplateHashDecl
inline const Value* tTemplateHashInst::getPtrValue(const Key _Key) const
{
    if(_Key != m_UnusedKey)
    {
    const Element *pElem = pGetElement(_Key);
    if (pElem)
    {
        return &(pElem->m_Value);
    }
    }

    return NULL;
}

tTemplateHashDecl
inline Value* tTemplateHashInst::getPtrValue(const Key _Key)
{
    if(_Key != m_UnusedKey)
    {
    Element *pElem = pGetElement(_Key);
    if (pElem)
    {
        return &(pElem->m_Value);
    }
    }
    return NULL;
}

// If key is not in table, return HashIteratorInvalid
tTemplateHashDecl
inline HashIterator tTemplateHashInst::itFindValue(Key _Key) const
{
    ITF_ASSERT(_Key != m_UnusedKey);

    const Element *pElem = pGetElement(_Key);
    if (pElem)
    {
        return (pElem - ma_Elements.getDataBuffer());
    }
    else
    {
        return HashIteratorInvalid;
    }
}

tTemplateHashDecl
inline bbool tTemplateHashInst::bRemoveValue(Key _Key)
{
    ITF_ASSERT(_Key != m_UnusedKey);

    u32 i = KeyToIndex<Key>(_Key);

    // Search index where key really is
    Element *pElemI = &ma_Elements[i];
    while ((_Key != pElemI->m_Key) && (m_UnusedKey != pElemI->m_Key))
    {
        i = (i+1) & mu_HashMask;
        pElemI = &ma_Elements[i];
    }

    if (pElemI->m_Key == m_UnusedKey)
        return bfalse; // Did not find _Key in table.

    // We found pElemI so that (_Key == pElemI->m_Key) -> Remove element.
    RemoveWithoutCheck(i);

    return btrue;
}

tTemplateHashDecl
inline void tTemplateHashInst::RemoveWithoutCheck(HashIterator _i)
{
    Element *pElemI = &ma_Elements[_i];

    // Remove element i.
    pElemI->m_Key = m_UnusedKey;

    // We created a hole at pElemI -> we need to keep the hash table coherency :
    // the elements following pElemI and before the first following hole should be
    // moved back so that they have no vacant slots between
    // their natural hash position and their current position.
    u32 j = (_i+1) & mu_HashMask;
    Element *pElemJ = &ma_Elements[j];
    while (pElemJ->m_Key != m_UnusedKey)
    {
        u32 jNatural = KeyToIndex<Key>(pElemJ->m_Key);

        // The hole is at i
        // The pElemJ is at j, but would be at jNatural if there were no collisions.

        if (jNatural != j)
        {
            // We move pElemJ to the hole (at i) only if it is between j and jNatural (jNatural included).
            bbool bJ_Sup_I =    (j>=_i);
            bbool bJ_Sup_Jn =   (j>=jNatural);
            bbool bI_Sup_Jn =   (_i>=jNatural);

            if ((bJ_Sup_Jn && bJ_Sup_I &&  bI_Sup_Jn) // "normal" case (no modulo) :  we can copy j to i if i > jn.
                || (!bJ_Sup_Jn && bJ_Sup_I) // case "jn > j" -> we can always copy j to i (whereas in the case "i > j", we can never copy j to i).
                || (!bJ_Sup_Jn && !bJ_Sup_I && bI_Sup_Jn)) // case where both jn and i are > j. Same as normal case : we can copy j to i if i > jn.
            {
                // Copy j to i.
                *pElemI = *pElemJ;

                // Now hole is at j
                _i = j;
                pElemI = pElemJ;
                pElemI->m_Key = m_UnusedKey;
            }
        }

        // Next element.
        j = (j+1) & mu_HashMask;
        pElemJ = &ma_Elements[j];
    }

    mu_ElementNb--;
}




//#define TOOHASH_PARANOID_CHECK
tTemplateHashDecl
inline bbool tTemplateHashInst::bIsTableCoherent() const
{
    u32 i,count = 0;
    for(i=0; i<ma_Elements.size(); i++)
    {
        if (ma_Elements[i].m_Key != m_UnusedKey)
            count++;
    }
    bbool b_res = true;
#ifdef TOOHASH_PARANOID_CHECK
    if(size>1)
    {
        for(ux i=0; i<size-1; ++i)
        {
            const Key pKeyi = pElts->r_GetValAt(i).m_Key;
            if (pKeyi != *pUnusedKey)
            {
                for(ux j=i+1; j<size; ++j)
                {
                    if (pKeyi == pElts->r_GetValAt(j).m_Key)
                    {
                        b_res = bfalse;
                    }
                }
            }
        }
    }
#endif
    if(count != mu_ElementNb)
        b_res = bfalse;

    return b_res;
}

tTemplateHashDecl
inline HashIterator tTemplateHashInst::GetFirstIterator() const
{
    return GetNextIterator((u32)-1);
}

tTemplateHashDecl
inline HashIterator tTemplateHashInst::GetNextIterator(HashIterator _iterator) const
{
    u32 i;
    for (i=_iterator+1; i< ma_Elements.size(); i++)
    {
        if (ma_Elements[i].m_Key != m_UnusedKey)
        {
            return i;
        }
    }
    return HashIteratorInvalid;
}

tTemplateHashDecl
inline HashIterator tTemplateHashInst::RemoveValue(HashIterator _iterator)
{
    RemoveWithoutCheck(_iterator);

    // Find next valid iterator
    HashIterator nextIterator = _iterator;
    while ((nextIterator < ma_Elements.size()) && (ma_Elements[nextIterator].m_Key == m_UnusedKey))
    {
        nextIterator++;
    }

    return (nextIterator == ma_Elements.size() ? HashIteratorInvalid : nextIterator);
}

tTemplateHashDecl
inline Value tTemplateHashInst::GetValue(HashIterator _iterator) const
{
    return ma_Elements[_iterator].m_Value;
}

tTemplateHashDecl
inline Value & tTemplateHashInst::GetValueRef(HashIterator _iterator)
{
    return ma_Elements[_iterator].m_Value;
}

tTemplateHashDecl
inline const Value & tTemplateHashInst::GetValueRef(HashIterator _iterator) const
{
    return ma_Elements[_iterator].m_Value;
}

tTemplateHashDecl
inline Key tTemplateHashInst::GetKey(HashIterator _iterator) const
{
    return ma_Elements[_iterator].m_Key;
}

tTemplateHashDecl
template <class TOOPred , class TOOFunc>
inline void tTemplateHashInst::Apply(TOOPred &_predicate, TOOFunc &_functor)
{
	u32 u32_Size = ma_Elements.size();
	for(u32 i = 0; i < u32_Size; ++i)
	{
		Element &elem = ma_Elements[i];
		if(elem.m_Key != m_UnusedKey && _predicate(elem.m_Key, elem.m_Value))
		{
			_functor(elem.m_Key, elem.m_Value);
		}
	}
}

tTemplateHashDecl
template <class TOOFunc>
inline void tTemplateHashInst::Apply(TOOFunc &_functor)
{
	Apply(Hash_Utils::AlwaysTrue<Key, Value>(), _functor);
}

tTemplateHashDecl
inline void tTemplateHashInst::CopyFrom(const Hash *_po_Src)
{
    m_UnusedKey = _po_Src->m_UnusedKey;
	ma_Elements.CopyFrom(&_po_Src->ma_Elements);
	mu_ElementNb = _po_Src->mu_ElementNb;
	mu_SizePower = _po_Src->mu_SizePower;
	mu_HashMask = _po_Src->mu_HashMask;
}

tTemplateHashDecl
template <class K>
const typename tTemplateHashInst::Element * tTemplateHashInst::pGetElement(const K _Key) const
{
    u32 uIndex = KeyToIndex<Key>(_Key);
    const Element *pElem = &ma_Elements[uIndex];

    // Manage collisions
    while ((pElem->m_Key != m_UnusedKey) && (pElem->m_Key != _Key))
    {
        uIndex = (uIndex+1) & mu_HashMask;
        pElem = &ma_Elements[uIndex];
    }

    // Return NULL if element was not found.
    return (pElem->m_Key != m_UnusedKey ? pElem : NULL);
}

tTemplateHashDecl
template <class K>
typename tTemplateHashInst::Element * tTemplateHashInst::pGetElement(const K _Key)
{
    u32 uIndex = KeyToIndex<Key>(_Key);
    Element *pElem = &ma_Elements[uIndex];

    // Manage collisions
    while ((pElem->m_Key != m_UnusedKey) && (pElem->m_Key != _Key))
    {
        uIndex = (uIndex+1) & mu_HashMask;
        pElem = &ma_Elements[uIndex];
    }

    // Return NULL if element was not found.
    return (pElem->m_Key != m_UnusedKey ? pElem : NULL);
}

tTemplateHashDecl
template<class K, class V>
inline bbool tTemplateHashInst::bAddElement(K _Key, V _Value, HashIterator& _iterator)
{
    u32 uIndex = KeyToIndex<Key>(_Key);
    Element *pElem = &ma_Elements[uIndex];

    // Manage collisions
    for(;;)
    {
        // Found an empty element -> fill value and key
        if (pElem->m_Key == m_UnusedKey)
        {
            pElem->m_Key = _Key;
            pElem->m_Value = _Value;
            _iterator = uIndex;
            return btrue;
        }
        else if (pElem->m_Key == _Key)
        {
            // Found the element -> replace value
            pElem->m_Value = _Value;
            _iterator = uIndex;
            return bfalse;
        }
        else
        {
            // The element is used, try the next one.
            uIndex = (uIndex+1) & mu_HashMask;
            pElem = &ma_Elements[uIndex];
        }
    }
}

tTemplateHashDecl
template <class K>
inline u32 tTemplateHashInst::KeyToIndex(const K &_Key) const
{
	return (Hash_Utils::HashFunction<sizeof(K), significantKeyBytes>(&_Key) & mu_HashMask);
}

inline u32 Hash_Utils::GenericHashFunction(int _typeSize, int _keyBytes, const void *_Key)
{
	ITF_ASSERT_CRASH(_keyBytes <= _typeSize, "cannot use more bytes than the key size");
	u32 hash = 0;

	// Parse all bytes one by one.
	for (ux i = 0; i < (ux)_keyBytes; i++)
	{
		hash += ((u8 *)_Key)[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	// Final mix of the index.
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

tTemplateHashDecl
inline void tTemplateHashInst::swap( Hash<Key, Value, significantKeyBytes >& other )
{
    ma_Elements.swap(other.ma_Elements);
    ITF::Swap(mu_ElementNb, other.mu_ElementNb);
    ITF::Swap(mu_SizePower, other.mu_SizePower);
    ITF::Swap(mu_HashMask, other.mu_HashMask);

    // swap is currently only used by resize. In that process, we know that other as the same m_UnusedKey and 'this'.
    // that's why this attribute is not swapped. Add this swap if you need it.
}

tTemplateHashDecl
template <class K, class V>
inline void tTemplateHashInst::resize()
{
    tTemplateHashInst newHash;
    HashIterator itUnused;
    newHash.create( mu_SizePower+1, m_UnusedKey );

    //1. create a new hash, 2 times bigger than the current one, and insert all elements in it.
    //2. swap hashs attributes (number of elements, array pointers, ...)
    //3. delete old hash. This will be done when deleting "newHash", which is on stack, as attributes are swapped, not simply
    //copied from newHash.. 
    u32 uCurrentSize = ma_Elements.size();  
    for (u32 i=0; i<uCurrentSize; i++)
    {
        const Element &elem = ma_Elements[i];
        if (elem.m_Key != m_UnusedKey)
        {
            // directly call "bAddElement" instead of "bAddValue", as we know that "newHash" is big enough, and
            // does not need capacity check on each insertion.
            newHash.bAddElement<K, V>(elem.m_Key, elem.m_Value, itUnused);
            newHash.mu_ElementNb++;
        }
    }
    swap(newHash);
}

#ifdef _WIN32_
#pragma warning( pop) 
#endif // _WIN32_

} //namespace ITF

#endif // !_ITF_HASH_H_ 
