#ifndef _ITF_STRINGID_H_
#define _ITF_STRINGID_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef ITF_MAP_H_
#include "core/container/extendedMap.h"
#endif //ITF_MAP_H_

#include <string>

#if !defined(ITF_FINAL) && (!defined(ITF_CONSOLE) || defined(ITF_DEBUG_STRINGID))
#define ITF_ENABLE_STRINGID_CRC_CHECK
#endif

namespace ITF
{

////////////////////////////////////////////////////////////////////////////
// Converts a string to a unique ID
// http://cowboyprogramming.com/2007/01/04/practical-hash-ids/

class String;
class String8;
class StringIDDatabase;

class StringID
{
public:

    typedef u32 StringIdValueType;

    StringID();
    StringID( const char* _str );
    StringID( const ITF_STDSTRING& _str );
    StringID( const String& _str );
    StringID( const String8& _str );
    StringID( const StringIdValueType _value );

    StringIdValueType   GetValue() const        { return m_id; }

    bbool               isValid()const;
    void                invalidate();

    void                copyFrom( StringID _id );

    operator const StringIdValueType& () const { return m_id; }

    bool operator==(const StringID& rhs) const
    {
        return m_id == rhs.m_id;
    }

    bool operator==(StringIdValueType _crc) const
    {
        return m_id == _crc;
    }

    void serialize( class ArchiveMemory& _archive );

    // Computing the str's CRC using uppercase version of str
    static StringID::StringIdValueType StrToCRC( const char* _str);
    static StringID::StringIdValueType StrToCRC( const u16* _str);
    static StringID::StringIdValueType StrToCRC( const char* _str, u32 _len);
    static StringID::StringIdValueType StrToCRC( const u16* _str, u32 _len);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get **DEBUG** text representation of this string ID. Only available if ITF_DEBUG_STRINGID is defined for this platform/configuration
    /// DO NOT USE THIS FUNCTION IN NORMAL CODE, EXCEPT IN WARNING/ERROR
    const char*         getDebugString() const;

#ifdef ITF_DEBUG_STRINGID
    static void dbDestroy();
#endif

protected:
    template<u32 stride> void setStr( const char* _str, u32 _len );

    StringIdValueType   m_id;
    
    // We leave the method c_str() const but we want to be able to update the dictionary index
    // in case the string wasn't entered on the dictionary when we read the id
#ifdef ITF_DEBUG_STRINGID
    mutable u32         m_dictionaryIndex;
    static StringIDDatabase*    m_database;
    static StringIDDatabase*    dbInstance();
#endif

public:
    static const StringIdValueType InvalidId;
    static const StringID Invalid;
    friend class CSerializerObjectBinary;
};

VECTAGSET(class StringID, ITF::MemoryId::mId_VectorString)

class StringIDDatabase
{
public:

    u32                         enterString( StringID::StringIdValueType _id, const char* _string, u32 _stride );
    u32                         getStringIndex( StringID::StringIdValueType _id );
    const char*                 getString( u32 _index );
    StringIDDatabase();
    ~StringIDDatabase();
private:



    void                        grow( u32 _size );
    
    typedef ITF_MAP <StringID::StringIdValueType,int> Dictionary;

    static StringIDDatabase*    m_this;
    static const u32            m_blockSize = 10240;

    ITF_THREAD_CRITICAL_SECTION m_cs; // Critical section

    Dictionary                  m_dictionary;
    char*                       m_buffer;
    u32                         m_size;
    u32                         m_index;
};

ITF_INLINE StringID::StringID( const StringIdValueType _value )
{
    m_id = _value;

#ifdef ITF_DEBUG_STRINGID
    if ( m_id != InvalidId )
    {
        m_dictionaryIndex = dbInstance()->getStringIndex(m_id);
    }
    else
    {
        m_dictionaryIndex = U32_INVALID;
    }
#endif
}

ITF_INLINE void StringID::invalidate()
{
    m_id = InvalidId;

#ifdef ITF_DEBUG_STRINGID
    m_dictionaryIndex = U32_INVALID;
#endif
}

ITF_INLINE bbool StringID::isValid()const
{
    return m_id != InvalidId;
}

#ifdef ITF_ENABLE_STRINGID_CRC_CHECK
ITF_INLINE StringID::StringIdValueType stringID_GetCRC( const char* _className, StringID::StringIdValueType _value )
{
    ITF_ASSERT(StringID(_className).GetValue()==_value);
    return _value;
}

#define ITF_GET_STRINGID_CRC(object,crc) stringID_GetCRC(#object,crc)
#else // ITF_ENABLE_STRINGID_CRC_CHECK
#define ITF_GET_STRINGID_CRC(object,crc) ((StringID::StringIdValueType)crc)
#endif // ITF_ENABLE_STRINGID_CRC_CHECK

} // namespace ITF

#endif // _ITF_STRINGID_H_
