#include "precompiled_core.h"

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_STRING8_H_
#include "core/String8.h"
#endif //_ITF_STRING8_H_

#if defined(ITF_IPAD) || defined(ITF_CTR) || defined(ITF_WII)
#include <string>
#include <ctype.h>
#endif

namespace ITF
{

/////////////////////////////////////////////////////////////////////////////

const StringID::StringIdValueType StringID::InvalidId = U32_INVALID;
const StringID StringID::Invalid = StringID();

#define mix(a,b,c)                      \
{                                       \
    a -= b; a -= c; a ^= (c>>13);       \
    b -= c; b -= a; b ^= (a<<8);        \
    c -= a; c -= b; c ^= (b>>13);       \
    a -= b; a -= c; a ^= (c>>12);       \
    b -= c; b -= a; b ^= (a<<16);       \
    c -= a; c -= b; c ^= (b>>5);        \
    a -= b; a -= c; a ^= (c>>3);        \
    b -= c; b -= a; b ^= (a<<10);       \
    c -= a; c -= b; c ^= (b>>15);       \
}

inline char ToUp(char c)
{
    return (c<'a' || c>'z') ? (c) : (c+'A'-'a');
}

// Credits to the guys from Ubi Montreal
// Based on Gear::Hash::Dobbs that shouldn't give names collisions.
template<u32 _stride> inline StringID::StringIdValueType StrToCRC_Template( const char* _str, u32 _len ) 
{
    /* Set up the internal state */
    u32 Len = _len;
    u32 len = Len;
    u32 a = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    u32 b = a;
    u32 c = 0;

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a += (ToUp(_str[0*_stride]) + ((DWORD)ToUp(_str[1*_stride]) << 8) + ((DWORD)ToUp(_str[2*_stride]) << 16) + ((DWORD)ToUp(_str[3*_stride]) << 24));
        b += (ToUp(_str[4*_stride]) + ((DWORD)ToUp(_str[5*_stride]) << 8) + ((DWORD)ToUp(_str[6*_stride]) << 16) + ((DWORD)ToUp(_str[7*_stride]) << 24));
        c += (ToUp(_str[8*_stride]) + ((DWORD)ToUp(_str[9*_stride]) << 8) + ((DWORD)ToUp(_str[10*_stride]) << 16) + ((DWORD)ToUp(_str[11*_stride]) << 24));
        mix(a,b,c);
        _str += 12*_stride;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += Len;
    switch(len)              /* all the case statements fall through */
    {
    case 11: c+=((DWORD)ToUp(_str[10*_stride])<<24);
    case 10: c+=((DWORD)ToUp(_str[9*_stride])<<16);
    case 9 : c+=((DWORD)ToUp(_str[8*_stride])<<8);
        /* the first byte of c is reserved for the length */
    case 8 : b+=((DWORD)ToUp(_str[7*_stride])<<24);
    case 7 : b+=((DWORD)ToUp(_str[6*_stride])<<16);
    case 6 : b+=((DWORD)ToUp(_str[5*_stride])<<8);
    case 5 : b+=ToUp(_str[4*_stride]);
    case 4 : a+=((DWORD)ToUp(_str[3*_stride])<<24);
    case 3 : a+=((DWORD)ToUp(_str[2*_stride])<<16);
    case 2 : a+=((DWORD)ToUp(_str[1*_stride])<<8);
    case 1 : a+=ToUp(_str[0*_stride]);
        /* case 0: nothing left to add */
    }
    mix(a,b,c);

    return c;
}

template<u32 stride> void StringID::setStr( const char* _str, u32 _len )
{
    if ( _str && _len!=0 )
    {
        m_id = StrToCRC_Template<stride>(_str, _len);
#ifdef ITF_DEBUG_STRINGID
        m_dictionaryIndex = dbInstance()->enterString(m_id,_str, stride);
#endif
    }
    else
    {
        m_id = InvalidId;
#ifdef ITF_DEBUG_STRINGID
        m_dictionaryIndex = U32_INVALID;
#endif
    }
}

StringID::StringID()
: m_id(InvalidId)
#ifdef ITF_DEBUG_STRINGID
, m_dictionaryIndex(U32_INVALID)
#endif
{
}

StringID::StringID( const char* _str )
{
    if (_str!=NULL)
    {
        setStr<1>(_str, u32(strlen(_str)));
    }
    else
    {
        setStr<1>(_str, 0);
    }
}

StringID::StringID( const ITF_STDSTRING& _str )
{
    setStr<1>(_str.c_str(), u32(_str.length()));
}

StringID::StringID( const String& _str )
{
    //char buffer[256];
    //_str.charCopyToBuffer(buffer, sizeof(buffer));
    //setStr<1>(buffer, _str.getLen());
#ifdef ITF_LITTLE_ENDIAN
    setStr<2>((const char*)_str.cStr(), _str.getLen());
#else
    setStr<2>(((const char*)_str.cStr()) + 1, _str.getLen());
#endif
}

StringID::StringID( const String8& _str )
{
    setStr<1>(_str.cStr(), _str.getLen());
}

void StringID::serialize( ArchiveMemory& _archive )
{
    _archive.serialize(m_id);

#ifdef ITF_DEBUG_STRINGID
    if ( m_id != InvalidId )
    {
        m_dictionaryIndex = dbInstance()->getStringIndex(m_id);
    }
#endif
}



StringID::StringIdValueType StringID::StrToCRC( const char* _str) 
{
    return StrToCRC_Template<1>(_str, u32(strlen(_str)));
}

StringID::StringIdValueType StringID::StrToCRC( const char* _str, u32 _len) 
{
    return StrToCRC_Template<1>(_str, _len);
}

static u32 u16StrLen(const u16* _str)
{
    if (!_str)
        return 0;
    
    u32 len = 0;
    while (_str[len] != 0)
    {
        len++;
    }
    return len;
}

StringID::StringIdValueType StringID::StrToCRC( const u16* _str) 
{
#ifdef ITF_LITTLE_ENDIAN
    return StrToCRC_Template<2>((const char *)_str, u16StrLen(_str));
#else
    return StrToCRC_Template<2>((const char *)_str + 1, u16StrLen(_str));
#endif
}

StringID::StringIdValueType StringID::StrToCRC( const u16* _str, u32 _len) 
{
#ifdef ITF_LITTLE_ENDIAN
    return StrToCRC_Template<2>((const char *)_str, _len);
#else
    return StrToCRC_Template<2>((const char *)_str + 1, _len);
#endif
}

const char* StringID::getDebugString() const
{
    static const char* s_notFound = "";

#ifdef ITF_DEBUG_STRINGID
    const char* ret = NULL;

    if ( m_dictionaryIndex == U32_INVALID )
    {
        m_dictionaryIndex = dbInstance()->getStringIndex(m_id);

        if ( m_dictionaryIndex != U32_INVALID )
        {
            ret = dbInstance()->getString(m_dictionaryIndex);
        }
    }
    else
    {
        ret = dbInstance()->getString(m_dictionaryIndex);
    }

    if ( !ret )
    {
        ret = s_notFound;
    }

    return ret;
#else
    return s_notFound;
#endif
}

void StringID::copyFrom( StringID _id )
{
    m_id = _id.m_id;
#ifdef ITF_DEBUG_STRINGID
    m_dictionaryIndex = _id.m_dictionaryIndex;
#endif
}

#ifdef ITF_DEBUG_STRINGID
StringIDDatabase * StringID::dbInstance()
{
    if (!m_database)
    {
        m_database = newAlloc(mId_StringIDDatabase,StringIDDatabase);
    }
    return m_database;
}

void StringID::dbDestroy()
{
    SF_DEL(m_database);
}

StringIDDatabase* StringID::m_database = NULL;

#endif

/////////////////////////////////////////////////////////////////////////////

StringIDDatabase* StringIDDatabase::m_this = NULL;

StringIDDatabase::StringIDDatabase()
: m_buffer(NULL)
, m_size(0)
, m_index(0)
{
    ITF_ASSERT(m_this == NULL);

    Synchronize::createCriticalSection(&m_cs);
}

StringIDDatabase::~StringIDDatabase()
{
    SF_DEL(m_buffer);
    m_dictionary.clear();

    Synchronize::destroyCriticalSection(&m_cs);
}

u32 StringIDDatabase::enterString( StringID::StringIdValueType _id, const char* _string, u32 _stride )
{
    csAutoLock cs(m_cs);

    Dictionary::const_iterator it = m_dictionary.find(_id);

    if ( it != m_dictionary.end() )
    {
        return it->second;
    }

    String8 string;
    if (_stride==1)
        string = _string;
    else
    {
        char buffer[512];
        char *bufferPos = buffer;
        char val;
        do 
        {
            val = *_string;
            *(bufferPos++) = val;
            _string += _stride;
        } while (val);
        ITF_ASSERT(bufferPos<=(buffer+sizeof(buffer)));
        string = buffer;
    }

    u32 stringSize = string.getLen() + 1;

    grow(m_index+stringSize);

    u32 ret = m_index;

    strcpy(&m_buffer[ret], string.cStr());
    m_dictionary[_id] = ret;

    m_index += stringSize;

    return ret;
}

u32 StringIDDatabase::getStringIndex( StringID::StringIdValueType _id )
{
    Dictionary::const_iterator it = m_dictionary.find(_id);

    if ( it != m_dictionary.end() )
    {
        return it->second;
    }

    return U32_INVALID;
}

const char* StringIDDatabase::getString( u32 _index )
{
    ITF_ASSERT(_index<m_size);
    return &m_buffer[_index];
}

void StringIDDatabase::grow( u32 _size )
{
    if ( m_size >= _size )
    {
        return;
    }

    u32 blocksNeeded = ( _size / m_blockSize ) + 1;
    u32 blocks = ( _size / m_blockSize ) + blocksNeeded;
    u32 size = sizeof(char)*(blocks*m_blockSize);

    char* buffer = newAlloc(mId_StringIDDatabase, char[size]);

    if ( m_buffer )
    {
        ITF_Memcpy(buffer,m_buffer,m_size);
        SF_DEL(m_buffer);
    }

    m_buffer = buffer;
    m_size = size;
}

} // namespace ITF
