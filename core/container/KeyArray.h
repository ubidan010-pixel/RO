#ifndef _ITF_KEYARRAY_H_
#define _ITF_KEYARRAY_H_


#ifdef ITF_DEBUG_STRINGID
    #ifndef _ITF_STRING8_H_
    #include "core/string8.h"
    #endif //_ITF_STRING8_H_
#endif // ITF_DEBUG_STRINGID

#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"
#endif //_ITF_CORETYPES_H_   

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifdef ITF_GCC
#ifndef _ITF_STRINGID_H_
#include "core/container/extendedMap.h"
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_
#endif //ITF_IPAD

#ifndef ITF_VECTOR_H_
#include "core/container/extendedVector.h"
#endif //ITF_VECTOR_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_


namespace ITF
{

// Association array. Not sorted, so most operations are O(n). As a consequence, do not use anymore.
template <typename T, typename K = u32>
class KeyArray
{
public:  

    class Iterator
    {
    public:
        Iterator(const KeyArray<T, K>& _from) : m_table(_from)
        {
            m_index = 0;
        }

        Iterator(const Iterator& )
            : m_table(*(const KeyArray<T, K>*)NULL)
        {
            ITF_ASSERT_CRASH(0, "Copy not allowed");
        }

        const Iterator& operator = ( const Iterator& )
        {
            ITF_ASSERT_CRASH(0, "Copy not allowed");
            return *this;
        }

        void findKey(K _key) { m_index = m_table.find(_key);}
        const T& second() {return m_table.getAt(m_index);}
        K first() { return m_table.getKeyAt(m_index);}
#ifdef ITF_DEBUG_STRINGID
        const String8& dbg() {return m_table.m_debugInfo[m_index];}
#endif
        bbool isValid() {return u32(m_index) < m_table.size();}
        void increment() {m_index++;}
        void decrement() {m_index--;}
        bbool isEnd() {return !isValid();}
    private:
        i32       m_index;
        const KeyArray<T, K> & m_table;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Set map value for given StringID
    /// @param _id
    /// @param _value
    template <typename U = K>
    std::enable_if_t<std::is_convertible_v<StringID, U>, void> setID(const StringID & _id, const T& _value)
    {
        #ifdef ITF_DEBUG_STRINGID
            setDBG(_id, _value, _id.getDebugString());
        #else
            set(_id, _value);
        #endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// serialize to given archive
    /// @param _archive
    void serialize(ArchiveMemory& _archive)
    {   
       m_keys.serialize(_archive);
       m_array.serialize(_archive);
    #ifdef ITF_DEBUG_STRINGID
       m_debugInfo.resize(m_array.size());
    #endif // ITF_DEBUG_STRINGID
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// reserve memory for N items
    /// @param _reserve reserved memory, in item count
    void reserve(u32 _reserve)
    {
        u32 currentSize = m_keys.size();
        if (currentSize>_reserve)
        {
            _reserve = currentSize;
        }
        m_keys.reserve(_reserve);
        m_array.reserve(_reserve);
#ifdef ITF_DEBUG_STRINGID
        m_debugInfo.reserve(_reserve);
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// empty KeyArray
    void clear()
    {
        m_keys.clear();
        m_array.clear();
    #ifdef ITF_DEBUG_STRINGID
       m_debugInfo.clear();
    #endif // ITF_DEBUG_STRINGID
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set value for given numeric key
    /// @param _key
    /// @param _value
    void set(K _key, const T& _value)
    {
        i32 index = m_keys.find(_key);
        if (index < 0)
        {
            m_keys.push_back(_key);
            m_array.push_back(_value);
            #ifdef ITF_DEBUG_STRINGID
                    m_debugInfo.push_back(String8::emptyString);
            #endif // ITF_DEBUG_STRINGID
        }
        else
        {
            m_keys[index] = _key;
            m_array[index] = _value;
            #ifdef ITF_DEBUG_STRINGID
                    m_debugInfo[index] = String8::emptyString;
            #endif // ITF_DEBUG_STRINGID
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// set value for given numeric key knowing that the value is new
    /// @param _key
    /// @param _value
    void setNewValue(K _key, const T& _value)
    {
        m_keys.push_back(_key);
        m_array.push_back(_value);
#ifdef ITF_DEBUG_STRINGID
        m_debugInfo.push_back(String8::emptyString);
#endif // ITF_DEBUG_STRINGID
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// find by numeric key
    /// @param _key
    i32  find(K _key) const
    {
        return m_keys.find(_key);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get size, in items
    u32  size() const {return m_array.size();}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// item accessor, by index
    /// @param _index
    const T&   getAt(u32 _index) const {return m_array[_index];}
    T&   getAt(u32 _index) {return m_array[_index];}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// item accessor, by key
    /// @param _key
    const T&   get(K _key) const
    {
        i32 index = m_keys.find(_key);
        ITF_ASSERT(index >= 0);
        return m_array[index];
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get key at given index
    /// @param _index
    u32        getKeyAt(u32 _index) const {return m_keys[_index];}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// erase an item. Indices of other objects may be changed, so you should consider they are invalidated.
    /// @param _key
    void        erase(K _key)
    {
        i32 index = m_keys.find(_key);
        ITF_ASSERT(index >= 0);
        if (index>=0)
        {
            m_keys.eraseKeepOrder((u32)index);
            m_array.eraseKeepOrder((u32)index);
#ifdef ITF_DEBUG_STRINGID
            m_debugInfo.erase(m_debugInfo.begin()+index);
#endif
        }
    }

#ifdef ITF_DEBUG_STRINGID
    void setDBG(K _key, const T& _value, const String8& _debugInfo = String8::emptyString)
    {
        i32 index = m_keys.find(_key);
        if (index < 0)
        {
            m_keys.push_back(_key);
            m_array.push_back(_value);
            m_debugInfo.push_back(_debugInfo);
        }
        else
        {
            m_keys[index] = _key;
            m_array[index] = _value;
            m_debugInfo[index] = _debugInfo;
        }
    }


    const String8&   getDebugAt(u32 _index) const {return m_debugInfo[_index];}
#endif // ITF_DEBUG_STRINGID

private:
    void    resize(u32 _size)
    {
        m_keys.resize(_size);
        m_array.resize(_size);
        #ifdef ITF_DEBUG_STRINGID
           m_debugInfo.resize(_size);
        #endif // ITF_DEBUG_STRINGID
    }
   SafeArray<K> m_keys; //CAUTION : For now, this array is not sorted. Check any eraseNoOrder if you change it.
   SafeArray<T> m_array;
#ifdef ITF_DEBUG_STRINGID
   Vector<String8> m_debugInfo;
#endif // ITF_DEBUG_STRINGID
};

} // namespace ITF

#endif // _ITF_KEYARRAY_H_
