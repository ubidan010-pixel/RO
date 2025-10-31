#include "precompiled_core.h"

#include "core/itfstring.h"
#include "core/system/Thread.h"
#include "core/UnicodeTools.h"

namespace ITF
{
	const u16 String::internalEmptyString[1] = {0};
	const String String::emptyString;
	const i32 String::npos = -1;

	String::StaticThreadData* String::m_staticThreadData = NULL;


    void String::serialize(ArchiveMemory& _Archive)
    {
        //NOTE:if you change this function,please make sure that getSerializeLen match
        if (!_Archive.isReading())
        {
            u32 len = getLen();
            _Archive.serialize(len);
            _Archive.serializeBlock16((u16*)cStr(),len);
            if (m_szContent_)
                m_szContent_[len] = 0;
        }
        else
        {
            u32 len;
            _Archive.serialize(len);
            if (len)
            findMemoryForString(len);
            _Archive.serializeBlock16((u16*)m_szContent_,len);
            if (m_szContent_)
                m_szContent_[len] = 0;
            m_length = len;
        }
    }


    void String::serialize(ArchiveMemory& _Archive) const
    {
        //NOTE:if you change this function,please make sure that getSerializeLen match
        if (!_Archive.isReading())
        {
            u32 len = getLen();
            _Archive.serialize(len);
            _Archive.serializeBlock16((u16*)cStr(),len);
        }
        else
        {
            ITF_ASSERT(0);
        }
    }

    u32 String::getSerializeSize() const
    {
        u32 len = getLen();
        return sizeof(len)+len*sizeof(u16);
    }
       



u32 String::getStringLength(const u16* _str)
{
    if (!_str)
        return 0;
    u32 count = 0;
    for(;;)
    {
        if (!*_str)
            break;
        _str++;
        count++;
    }
    return count;
}

u32 String::getStringLength(const char* _str)
{
    if (!_str)
        return 0;
    u32 count = 0;
    for(;;)
    {
        if (!*_str)
            break;
        _str++;
        count++;
    }
    return count;
}


bbool String::isSameString(const char* _str1, const char* _str2)
{
    while(*_str1)
    {
        if (*_str1 != *_str2)
            return bfalse;
        _str1++;
        _str2++;
    }
    if (*_str1 != *_str2)
        return bfalse;
    return btrue;
}

u32 String::findSlotIndex() const
{
    return m_reserveIndex;
}

bool String::findFreeSlot()
{
	if (!isOnStack() || !m_staticThreadData)
        return false;

    if (m_doNotCache)
        return false;

    u32 threadIndex = ThreadInfo::m_threadEngineIndex.getValue();

    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[threadIndex];

    u32 index = reserve.findFreeIndex();

    if (index >= reserve.m_count)
    {
        reserve.m_dictionaryWasFull = btrue;
        return false;
    }

    m_szContent_ = reserve.m_data[index].data;
    m_capacity = Predef_Memory::SPACE_PER_RESERVED-1;
    clearContent();
    m_reserveIndex  = index;
    m_reserveThread = threadIndex;
    ITF_ASSERT(m_szContent_);

    return true;
}

void String::tagSlotAsFree(u32 slotIndex, bool _clear)
{
    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[m_reserveThread];
    ITF_ASSERT(slotIndex<(i32)reserve.m_count);
    //ITF_ASSERT(findSlotIndex() == slotIndex);
    if(_clear)
        reserve.m_data[slotIndex].data[0] = 0;

    if (m_reserveThread == ThreadInfo::m_threadEngineIndex.getValue()) 
        reserve.setFree(slotIndex);
    else
        reserve.setFreeNextFrame(slotIndex);

}


void String::allocConventionalMemory(u32 capacity)
{
    m_reserveIndex = Invalid_Index;
    m_capacity = capacity;
    m_szContent_ = new_array(u16,capacity+1,mId_String16); // +1 for trailing 0
    clearContent();
}

void String::findMemoryForString(u32 capacity, bool _clear)
{
    STR_CHECK_CACHE_INSIDE
    u32 slotIndex = findSlotIndex();
    if(Invalid_Index != slotIndex)// if in slots
    {
        ITF_ASSERT(m_szContent_);
        if(capacity < Predef_Memory::SPACE_PER_RESERVED) // If it fits into a reserve's slot
        {
            if (_clear)
            {
                clearContent();
            }
            STR_CHECK_CACHE_INSIDE
            return;
        }
        else // We have to migrate the string outside of the reserve
        {
            //ITF_ASSERT(!"Migrating memory outside of the reserve");
            // We remove it from the reserve
            allocConventionalMemory(capacity);
            tagSlotAsFree(slotIndex, _clear);
        }
    }
    else // String not in slots
    {
        if ((NULL == m_szContent_) && (capacity < Predef_Memory::SPACE_PER_RESERVED))
        {
            if (findFreeSlot())
            {
                if (_clear)
                {
                    clearContent();
                }
                STR_CHECK_CACHE_INSIDE
                return;
            }
        }
        if(m_capacity < capacity) // Not enough space to grow
        {
            if ((NULL != m_szContent_) && (_clear))
            {
                ITF_ASSERT(Invalid_Index == findSlotIndex());
                SF_DEL_ARRAY(m_szContent_);
            }
            allocConventionalMemory(capacity);
        }
    }
    if (_clear)
    {
        clearContent();
    }
    STR_CHECK_CACHE_INSIDE
    return;
}

#ifdef ITF_DEBUG_STRINGS
void String::checkCache()
{
    if (!m_staticThreadData)
        return;
    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[ThreadInfo::m_threadEngineIndex.getValue()];
 
    for (u32 i = 0; i< reserve.m_count; i++)
    {
        if (reserve.m_data[i].m_cacheSlotIsAvailable){
            ITF_ASSERT(reserve.m_FreeIndexesStack.find(i) >= 0);}
        else{
            ITF_ASSERT(reserve.m_FreeIndexesStack.find(i) < 0);}
    }
}
#endif

void String::deleteThreadsData()
{
    if (!m_staticThreadData)
        return;
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        Synchronize::destroyCriticalSection(&m_staticThreadData->DataReserve[threadIdx].m_csStringToFree);
    }

    if (m_staticThreadData->DataReserveBuffer)
    {
        SF_DEL_ARRAY(m_staticThreadData->DataReserveBuffer);
    }

    SF_DEL(m_staticThreadData);
}

void String::createThreadsData()
{
    ITF_ASSERT(!m_staticThreadData);
    m_staticThreadData =  newAlloc(mId_String16,StaticThreadData());

    u32 totalCount = 0;
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        totalCount += ThreadSettings::m_settings[threadIdx].m_maxString;
    }

    m_staticThreadData->DataReserveBuffer = newAlloc(mId_String16,Predef_Memory[totalCount]);

    totalCount = 0;
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        Thread_DataReserve& reserve = m_staticThreadData->DataReserve[threadIdx];
        reserve.m_threadIndex = (eThreadId)threadIdx;
        Synchronize::createCriticalSection(&reserve.m_csStringToFree);

        u32 elemCount = ThreadSettings::m_settings[threadIdx].m_maxString;
        reserve.m_count = elemCount;
        reserve.m_data = &m_staticThreadData->DataReserveBuffer[totalCount];
        reserve.m_FreeIndexesStack.reserve(elemCount);
        for (u32 i = 0; i < elemCount; i++)
            reserve.m_FreeIndexesStack.push_back(i);
        totalCount += elemCount;
    }
    STR_CHECK_CACHE
}



String::String()
{
    m_reserveIndex = Invalid_Index;
    m_capacity = 0;
    m_szContent_ = NULL;
    m_reserveThread = 0;
    m_length = 0;
    m_doNotCache = bfalse;
}

bool String::isEmpty() const
{
    return !m_szContent_ || (m_szContent_[0] == 0);
}

void String::copyToContent(const u16* source, u32 nbChars)
{
    ITF_ASSERT(NULL != source);
    ITF_ASSERT(NULL != m_szContent_);
    ITF_ASSERT(m_szContent_ != source);
    ITF_ASSERT(m_capacity >= nbChars);

    ITF_Memcpy(m_szContent_, source, sizeof(u16) * nbChars);
    m_szContent_[nbChars] = 0;
    m_length = nbChars;
}


void String::truncate(const u32 newsize)
{
    if(newsize>=(u32)getLen()) return; // no need to truncate, keep the current string
    m_szContent_[newsize] = 0; // memory nor reallocated. We just set a 0, memory will be freed anyway.
    m_length = newsize;
}    

void String::clearContent()
{
    if (NULL != m_szContent_)
    {
        m_szContent_[0] = 0;
    }
    m_length = 0;
}

void String::clear()
{
    this->freeMemoryFromString(m_szContent_);

    m_reserveIndex = Invalid_Index;
    m_capacity = 0;
    m_szContent_ = NULL;
    m_reserveThread = 0;
    m_length = 0;
}

void String::setTextFormat(const char* arg_format, ...)
{
    va_list argptr;
    va_start(argptr, arg_format);

    static constexpr int LOCAL_STR_SIZE = 1024;
    char str[LOCAL_STR_SIZE];

    while (
#ifdef ITF_WINDOWS
        _vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) < 0
#elif defined(ITF_X360)
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) < 0
#elif defined(ITF_PS3)
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) >= LOCAL_STR_SIZE-1
#else
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) >= LOCAL_STR_SIZE-1
#endif
        )
    {
        ITF_ASSERT_CRASH(bfalse, "please increase LOCAL_STR_SIZE");
    }
    str[LOCAL_STR_SIZE - 1] = 0;
    va_end(argptr);
    setText(str);

    updateLength();
}

// UTF16 version
void String::setStringFormat(String arg_format, ...)
{
    va_list argptr;
    va_start(argptr, arg_format);

    static constexpr u32 LOCAL_STR_SIZE = 1024;

#if defined(ITF_WCHAR_IS_16BIT)
    u16 str[LOCAL_STR_SIZE];
#else
    char str[LOCAL_STR_SIZE];
#endif

    while (
#ifdef ITF_WINDOWS
        vswprintf_s(reinterpret_cast<wchar_t*>(str), LOCAL_STR_SIZE-1, reinterpret_cast<const wchar_t*>(arg_format.cStr()), argptr) < 0
#elif ITF_X360
        vswprintf_s(reinterpret_cast<wchar_t*>(str), LOCAL_STR_SIZE-1, reinterpret_cast<const wchar_t*>(arg_format.cStr()), argptr) < 0
#elif defined(ITF_PS3)
        vswprintf(reinterpret_cast<wchar_t*>(str), LOCAL_STR_SIZE-1, reinterpret_cast<const wchar_t*>(arg_format.cStr()), argptr) >= (int)(LOCAL_STR_SIZE-1)
#elif defined(ITF_WCHAR_IS_16BIT)
        vswprintf_s(reinterpret_cast<wchar_t*>(str), LOCAL_STR_SIZE - 1, reinterpret_cast<const wchar_t*>(arg_format.cStr()), argptr) >= (int)(LOCAL_STR_SIZE - 1)
#else
        vsnprintf(str, LOCAL_STR_SIZE - 1, UTF16ToUTF8(arg_format.cStr()).get(), argptr) >= (int)(LOCAL_STR_SIZE - 1)
#endif
        )
    {
        ITF_ASSERT_CRASH(bfalse, "please increase LOCAL_STR_SIZE");
    }

    va_end(argptr);

    str[LOCAL_STR_SIZE - 1] = 0;

    setText(str);

    updateLength();
}



void String::setText(const u16* szString)
{
    u32 nbMaxChar = getStringLength(szString);

    if(0 == nbMaxChar) // Source doesn't have any chars
    {
        clearContent();
        return;
    }
    findMemoryForString(nbMaxChar, true);

    copyToContent(szString, nbMaxChar);
    m_length = nbMaxChar;
}


void String::setText ( const u16* szString,u32 _length)
{
    
    u32 nbMaxChar = _length;

    if(0 == nbMaxChar) // Source doesn't have any chars
    {
        clearContent();
        return;
    }
    findMemoryForString(nbMaxChar, true);

    copyToContent(szString, nbMaxChar);
    m_length = nbMaxChar;
}

void String::setText(const String& oString)
{
    u32 len = 0;
    len = oString.getLen();
    if(0 == len)
    {
        clear();
        return;
    }
    if(m_capacity < len) // Not enough space to stock that text
    {
        findMemoryForString(len, true);
    }
    copyToContent(oString.cStr(), len);
    m_length = len;
}

void String::setText(const char* szString)
{
    u32 nbMaxChar = getStringLength(szString);
    clearContent();
    if(!szString) return;
    if(0 == nbMaxChar)
    {
        clear();
        return;
    }
    if(m_capacity < nbMaxChar) // Not enough space to stock that text
    {
        findMemoryForString(nbMaxChar, true);
    }

    for (u32 i=0; i<nbMaxChar; i++)
    {
        m_szContent_[i] = (u16)convertUtf8toUnicodeChar((u8*)szString+i, sizeof(char));
    }
    m_szContent_[nbMaxChar] = 0;
    m_length = nbMaxChar;
}

String::String(const u16* szString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    m_doNotCache = bfalse;
    m_reserveThread = 0;
    setText(szString);
}

String::String(const String& oString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    m_doNotCache = bfalse;
    m_reserveThread = 0;
    setText(oString);
}

String::String(const char* szString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    m_doNotCache = bfalse;
    m_reserveThread = 0;
    setText(szString);
}

String::String(const String8& _szString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    m_doNotCache = bfalse;
    m_reserveThread = 0;
    setText(_szString.cStr());
}


String::String( const String& oString ,u32 _length)
{
    m_reserveIndex = Invalid_Index;

    m_capacity = 0;
    m_szContent_ = NULL;
    m_doNotCache = bfalse;

    u32 nbMaxChar = _length;

    if(0 == nbMaxChar) // Source doesn't have any chars
    {
        clear();
        m_reserveThread = 0;
        return;
    }
    findMemoryForString(nbMaxChar, true);

    copyToContent(oString.cStr(), nbMaxChar);
    m_length = nbMaxChar;
}

const String& String::operator =(const String& oString)
{
    if(this != &oString)
    {
        setText(oString);
    }
    return *this;       
}

void String::freeMemoryFromString(u16* p_strData)
{
    if(p_strData)
    {
        if (m_reserveIndex != Invalid_Index)
        {
            if (!m_staticThreadData)    
                return; // string is deleted after thread data is freed
            ITF_ASSERT(m_reserveThread < eThreadId_count);
            Thread_DataReserve& reserve = m_staticThreadData->DataReserve[m_reserveThread];
            ITF_ASSERT(p_strData == reserve.m_data[m_reserveIndex].data);
            m_capacity = 0;
            m_szContent_ = NULL;
            clearContent();
            if (m_reserveThread == ThreadInfo::m_threadEngineIndex.getValue()) 
                reserve.setFree(m_reserveIndex);
            else
                reserve.setFreeNextFrame(m_reserveIndex);
            return;
        }

        delete[] p_strData;
        p_strData = NULL;
    }
}

String::~String()
{
    this->freeMemoryFromString(m_szContent_);
    //if(m_szContent_ != NULL)
    //  delete[] m_szContent_;
    //ITF_ASSERT(m_szContent_ == NULL);
}

bool    String::operator == (const String& oString) const
{
  	bool amEmpty = isEmpty();
	if (amEmpty != oString.isEmpty())
		return false;
	if (amEmpty)
		return true;

    if (getLen() != oString.getLen())
        return false;
	
    return 0 == memcmp(m_szContent_, oString.cStr(), getLen() * sizeof(u16));
}

void String::toLower()
{
    if(!m_szContent_) return;
    u16* pData = m_szContent_;
    u16 delta = ((u16)'a' - (u16)'A');

    while (*pData)
    {
        // convert latin charset
        if((*pData >= (u16)'A') && (*pData <= (u16)'Z'))
        {
            *pData += delta;
        }
        pData++;
    }
}

void String::updateThreadsData()
{
    if (!m_staticThreadData)
        return; // not yet created

    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[ThreadInfo::m_threadEngineIndex.getValue()];

    reserve.flushToBeFreed();
}

void String::toUpper()
{
    if(!m_szContent_) return;
    u16* pData = m_szContent_;
    while (*pData)
    {
        // convert latin charset
        if((*pData >= (u16)'a') && (*pData <= (u16)'z'))
        {
            *pData -= ((u16)'a' - (u16)'A');
        }
        pData++;
    }
}

bool    String::equals (const String& oString, bool ignoreCase) const
{
    if((!m_szContent_) || (!(oString.cStr())))
    {
        return false;
    }
    if(ignoreCase)
    {
        if (getLen() != oString.getLen())
            return false;

        String s1(*this);
        String s2(oString);
        s1.toLower();
        s2.toLower();
        return (s1 == s2);
    }
    else
    {
        return (*this == oString);
    }
}

bool    String::operator == (const u16* szString) const
{
    if(!szString)
    {
        return false;
    }

    if(!m_szContent_ && (0 == *szString) )
    {
        return true;
    }

    if (!m_szContent_)
    {
        return false;
    }

    return 0 == memcmp(m_szContent_, szString, getLen()*sizeof(u16));
}

bool    String::operator == (const char* szString) const
{
    if(!szString)
    {
        return false;
    }

    if(!m_szContent_ && (!szString  || (0 == *szString)))
    {
        return true;
    }

    if (!m_szContent_)
    {
        return false;
    }


    const u16* mContent = m_szContent_;
    const char* oContent = szString;

    for(;;)
    {
        if(*mContent != (u16)(*oContent))
        {
            return false;
        }
        if(0 == *mContent)
        {
            break;
        }
        mContent++;
        oContent++;
    }
    return true;
}

bool String::operator != (const String& oString) const
{
    return !(*this == oString);
}

const String String::operator + (const String &oString) const
{
    String str = "";
    str += (*this);
    str += oString;

    return str;
}


const String String::operator + (const u16* _oString) const
{
    String str = "";
    str += (*this);
    str += _oString;

    return str;
}

const String String::operator + (const char* _oString) const
{
    String str = "";
    str += (*this);
    str += _oString;

    return str;
}



void    String::operator += (const u16* wString)
{
    ITF_ASSERT(wString);
    if(!m_szContent_)
    {
        setText(wString);
        return;
    }
    String oString(wString);
    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    u16* old_data = m_szContent_;
    u32 mylen = getLen();
    u32 olen = oString.getLen();
    u32 len = mylen + olen;
    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    ITF_Memcpy(&m_szContent_[mylen], oString.cStr(), sizeof(u16) * olen);
    m_szContent_[len] = 0;
    m_length = len;

    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}

void    String::operator += (const u16 wString)
{
    if(!m_szContent_)
    {
        u16 str[2] = {wString, 0};
        setText(str);
        return;
    }
    String oString;

    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    u16* old_data = m_szContent_;
    u32 mylen = getLen();
    
    u32 len = mylen + 1;

    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    m_szContent_[mylen] = wString;
    m_szContent_[len] = 0;
    m_length = len;
    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}

void    String::operator += (const String& oString)
{
    if(!m_szContent_)
    {
        setText(oString);
        return;
    }
    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    u16* old_data = m_szContent_;
    u32 mylen = getLen();
    u32 olen = oString.getLen();
    u32 len = mylen + olen;
    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    ITF_Memcpy(&m_szContent_[mylen], oString.cStr(), sizeof(u16) * olen);
    m_szContent_[len] = 0;
    m_length = len;
    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}

void    String::operator += (const char* oString)
{
    ITF_ASSERT(oString);
    if(!m_szContent_)
    {
        setText(oString);
        return;
    }
    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    u16* old_data = m_szContent_;
    u32 mylen = getLen();
    u32 olen = getStringLength(oString);
    u32 len = mylen + olen;
    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    for(u32 i=0; i<olen; i++)
    {
        m_szContent_[mylen+i] = (u16)oString[i];
    }
    m_szContent_[len] = 0;   
    m_length = len;
    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}


const u16* String::cStr() const
{
    // NULL IS VALID, DON'T ASSERT IN CASE OF NULL CONTENT
    if(NULL == m_szContent_)
    {
        return String::internalEmptyString;
    }
    return m_szContent_;
}

bool String::startsWith(const String& _start) const
{
    if (_start.isEmpty())
        return true;

    if (getLen() < _start.getLen())
        return false;

    return 0 == memcmp(m_szContent_, _start.cStr(), _start.getLen() * sizeof(u16));
}

bool String::endsWith(const String& _end) const
{
    if (_end.isEmpty())
        return true;

    if (getLen() < _end.getLen())
        return false;

    return 0 == memcmp(m_szContent_ + getLen() - _end.getLen(), _end.cStr(), _end.getLen() * sizeof(u16));
}


i32 String::atoi32() const
{
    return (i32)atoi64();
}

u32 String::atoui32() const 
{
    return (u32)atoui64();
}

i64 String::atoi64() const
{
    if (isEmpty())
    {
        ITF_ASSERT(!isEmpty());
        return 0;
    }

    u32 size   = getLen();
    i64 ret     = 0;
    u32 offset = 0;
    bool negate      = false;

    // check for minus sign
    if (m_szContent_[offset] == L'-')
    {
        negate = true;
        offset++;
    }

    while (offset < size)
    {
        if (m_szContent_[offset] >= L'0' &&
            m_szContent_[offset] <= L'9')
        {
            ret = ret * 10 + m_szContent_[offset] - L'0';
        }
        else
        {
            // invalid char found, stop
            ITF_ASSERT(!"Atoi64: invalid char found, stop");
            break;
        }

        offset++;
    }

    if (negate)
    {
        return -ret;
    }
    else
    {
        return ret;
    }
}

u64 String::atoui64 () const
{
    if (isEmpty())
    {
        ITF_ASSERT(!isEmpty());
        return 0;
    }

    u32 size   = getLen();
    i64 ret     = 0;
    u32 offset = 0;

    while (offset < size)
    {
        if (m_szContent_[offset] >= L'0' &&
            m_szContent_[offset] <= L'9')
        {
            ret = ret * 10 + m_szContent_[offset] - L'0';
        }
        else
        {
            // invalid char found, stop
            ITF_ASSERT(!"Atoi64: invalid char found, stop");
            break;
        }

        offset++;
    }

    return ret;
}

f32 String::atof32() const
{
    StringConverter s(*this);
    return (f32)atof(s.getChar());
}

void String::i32toa(const i32 rhs)
{
    setTextFormat("%d", rhs);
}

void String::ui32toa(const u32 rhs)
{ 
    setTextFormat("%u", rhs);
}

void String::i64toa(const i64 rhs)
{  
    #if defined(ITF_PS3) || defined(ITF_PS5)
        setTextFormat("%lld", rhs);
    #else
        setTextFormat("%I64d", rhs);
    #endif
}

void String::ui64toa(const u64 rhs)
{  
    #if defined(ITF_PS3) || defined(ITF_PS5)
        setTextFormat("%llu", rhs);
    #else
        setTextFormat("%I64u", rhs);
    #endif
}

u32 String::getLen() const
{
 /*   if(!m_szContent_)
    {
        return 0;
    }
    int len=getStringLength(m_szContent_);
    ITF_ASSERT((int)m_length == len);*/
    return m_length;
}

void String::charCopyToBuffer(char _outBuffer[], u32 _bufferSize) const
{
    const u16 *theString = cStr();
    char *pastOutBuffer = _outBuffer+_bufferSize;
    char *outPtr = _outBuffer;

    char val;
    do
    {
        if (outPtr==pastOutBuffer)
        {
            pastOutBuffer[-1] =  0;
            ITF_ASSERT_MSG(bfalse, "String is too long");
            return;
        }
        val = (char)*theString;
        theString++;
        *outPtr = val;
        outPtr++;
    } while (val);
}

char* String::getCharCopy() const
{
    int size = getLen();
    if(!size)
    {
        char* ret =  new_array(char,1,mId_String16);
        *ret = 0;
        return ret;
        }
    
    char* ret = new_array(char,size+1,mId_String16);
    size=0;
    while(m_szContent_[size])
    {
        ret[size] = (char)m_szContent_[size];
        size++;
    }
    ret[size]=0;
    return ret;
}

u16* String::getWordCopy() const
{
    if(!m_szContent_) return NULL;
    int size = getLen();
    if(!size) return NULL;
    u16* ret = new_array(u16,size+1,mId_String16);
    size=0;
    while(m_szContent_[size])
    {
        ret[size] = (u16)m_szContent_[size];
        size++;
    }
    ret[size]=0;
    return ret;
}


u16*   String::strstr(char character, const bbool reverse, i32* index) const
{
    i32 idx = -1;
    if (index)
        *index = -1;

    if(!m_szContent_) 
        return NULL;

    u16* ret = NULL;
    if (reverse)
    {
        ret = m_szContent_ + getLen();
        ITF_ASSERT(*ret == 0);
        ret--;
        idx = getLen()-1;
        while ((ret >= m_szContent_) && ((*ret) != (u16)(u8)character)) 
        {
            idx--;
            ret--;
        }
        if((*ret) == (u16)(u8)character) 
        {
            if (index)
                *index = idx;
            return ret;
        }
    }
    else
    {
        ret = m_szContent_;
        idx = 0;
        while ((*ret) && ((*ret) != (u16)(u8)character))
        {
            idx++;
            ret++;
        }
        if((*ret) == (u16)(u8)character)
        {
            if (index)
                *index = idx;
            return ret;
        }
    }

    return NULL;
}   

u16*   String::strstr(const u16* szString, const bbool reverse, i32* index, i32 startIndex) const
{
    i32 idx = -1;
    if (index)
        *index = -1;


    if(!szString)
        return NULL;

    if(!m_szContent_) 
        return NULL;

    int len=getStringLength(szString);
    int size = getLen();

    if(size < len)
        return NULL;

    // Brute Force Algorithm
    u16* ret;
    bool stringFound = false;

    if (reverse)
    {
        ret = m_szContent_ + getLen();
        ITF_ASSERT(*ret == 0);
        ret -= len;
        idx = getLen()-len;
        while (ret >= m_szContent_)
        {
            if(memcmp(szString, ret, len*sizeof(u16))== 0)
            {
                stringFound = true;
                break;
            }
            ret--;
            idx--;
        }
    }
    else
    {
        idx = 0;
        for(ret=&m_szContent_[startIndex]; ((*ret != 0) && ((ret + len) <= (m_szContent_ + size))); ret++)
        {
            if(memcmp(szString, ret, len*sizeof(u16))== 0)
            {
                stringFound = true;
                break;
            }
            idx++;
        }
    }

    if(stringFound)
    {
        if (index)
            *index = idx;
        return ret;
    }
    else
        return NULL;
}   



String String::substr(u32 _pos,u32 _length) const
{
    String res;

    if ((_pos+_length)<= getLen())
    {
        res.setText(m_szContent_+_pos,_length);
    }

    return res;
}

String String::substr(u32 _pos) const
{
    String res;

    if (_pos < getLen())
    {
        res.setText(m_szContent_ + _pos);
    }

    return res;
}

u32  String::getSizeOfStringClassAsUtf8() const
{
    ITF_ASSERT(m_szContent_);
    size_t size=0;
    u16* string = m_szContent_;
    while(*string)
    {
        size += getUnicodeLength(*string++);
    }
    return u32(size);
}

void String::replace (const char find, const char replace)
{
    if(!m_szContent_) 
        return;

    u16* scan = m_szContent_;
    while (*scan)
    {
        if ((*scan) == (u16)(u8)find)
            *scan = (u16)(u8)replace;
        scan++;
    }
}


void String::replace (const String& _toFind, const String& _replace)
{
    if(!m_szContent_) 
        return;

    SafeArray<u16, 512> output;
    i32 index = -1;
    i32 scanIndex = 0;
    u32 replaceSize = _replace.getLen();
    u32 toFindSize = _toFind.getLen();
    u32 mySize = getLen();

    u16* found = m_szContent_;
    do
    {
        found = strstr(_toFind.cStr(), bfalse, &index, scanIndex);
        if (!found)
            break;
        index += scanIndex;
        for (; scanIndex < index; scanIndex++)
        {
            output.push_back(m_szContent_[scanIndex]);            
        }
        for (u32 j = 0; j < replaceSize; j++)
        {
            output.push_back(_replace.m_szContent_[j]);
        }
        scanIndex += toFindSize;
        if (scanIndex >= (i32)mySize)
            break;
    } while(found);
    for (; scanIndex < (i32)mySize; scanIndex++)
    {
        output.push_back(m_szContent_[scanIndex]);            
    }
    m_length = output.size();
    output.push_back(0);
    *this = &output[0];
}

u8*  String::convertStringClasstoUtf8Str(u32 *pSize, u8 *ret) const
{

    if (m_szContent_ == 0)
    {
        if(!ret)
            ret = newAlloc(mId_String8,u8[1]);
        *ret = 0;
        return ret;
    }

    ITF_ASSERT(m_szContent_);

    u32 size = this->getSizeOfStringClassAsUtf8();

    if(!ret)
        ret = newAlloc(mId_String8,u8[size+1]);
    u16* string = m_szContent_;
    u32 offset = 0;
    u8 utf_char[4];
    while(*string)
    {
        u32 local_size = getUnicodeLength(*string);
        convertUnicodeChartoUtf8(utf_char, (u32)*string);
        ITF_Memcpy(&ret[offset], utf_char, local_size);
        offset += local_size;
        string++;
    }
    ITF_ASSERT( size == offset );
    ret[size] = 0;
    if(pSize)
    {
        *pSize = size;
    }
    return ret;
}

u8*  String::convertUnicodeChartoUtf8(u8* _pRes, const u32 unicode)
{
    int len=getUnicodeLength(unicode);

    if((!len) || (len>4))
    {
        ITF_ASSERT(!"Can't convert character to UTF");
        return NULL;
    }
    switch (len)
    {
    case 1: 
        *_pRes = (u8) unicode;
        break;
    case 2:
        *_pRes = (u8) ((unicode>>6) | 0xC0);
        *(_pRes+1) = (u8) ((unicode & 0x3F) | 0x80);
        break;
    case 3:
        *_pRes = (u8) ((unicode>>12) | 0xE0);
        *(_pRes+1) = (u8) (((unicode>>6) & 0x3F) | 0x80);
        *(_pRes+2) = (u8) ((unicode & 0x3F) | 0x80);
        break;
    case 4:
        *_pRes = (u8) ((unicode >>18) | 0xF0 );
        *(_pRes+1) = (u8) (((unicode>>12)&0x3F) | 0x80 );
        *(_pRes+2) = (u8) (((unicode>>6) & 0x3F) | 0x80);
        *(_pRes+3) = (u8) ((unicode & 0x3F) | 0x80);
        break;
    }
    return _pRes;
}

void String::setUTF8Text( u8* Utf8String)
{
    ITF_ASSERT(Utf8String);

    u32 size=0;

    u8* string = Utf8String;

    // Find the size of Wchar pointer
    while(*string)
    {
        if(isU8Lead(*string))
        {
            size ++;
            string +=  getU8Len(*string);
        }
        else
        {
            size++;
            string++;
        }
    }

    clearContent();

    findMemoryForString(size + 1, true);

    string = Utf8String;
    u32 index = 0;
    u32 offset = 0;

    while(*string)
    {
        if(isU8Lead(*string))
        {
            m_szContent_[index] = (u16)convertUtf8toUnicodeChar(&Utf8String[offset], getU8Len(*string));
            string +=  getU8Len(Utf8String[offset]); 
            offset += getU8Len(Utf8String[offset]);
            index++;
        }
        else if(isU8Single(*string))
        {
            m_szContent_[index] = (u16)convertUtf8toUnicodeChar(&Utf8String[offset], getU8Len(*string));
            string ++; 
            offset ++;
            index++;
        }
        else
        {
            string ++; 
            offset ++;
        }
    }

    m_szContent_[size]=0;
    m_length = size;
}

u32 String::convertUtf8toUnicodeChar(const u8* utf8Str,  size_t olen)
{
    ITF_ASSERT(utf8Str);

    u32 ret = 0;

    switch (olen)
    {
    case 1: 
        ret = (u32) utf8Str[0];
        break;

    case 2:
        ret = (u32) (((utf8Str[0]-0xC0)<<6) | (utf8Str[1]&0x3F));
        break;

    case 3:
        ret = (u32) (((utf8Str[0]-0xE0)<<12) | 
            ((utf8Str[1]&0x3F)<<6) | (utf8Str[2]&0x3F));
        break;

    case 4:
        ret = (u32) (((utf8Str[0]-0xF0)<<18) | 
            ((utf8Str[1]&0x3F)<<12) | ((utf8Str[2]&0x3F)<<6) | 
            (utf8Str[3]&0x3F));
        break;

    }

    return ret;
}

int String::getUnicodeLength(const u32 unicode )
{
    if(unicode <= 0x7F)
    {
        return 1;
    }
    else if(unicode <= 0x7FF)
    {
        return 2;
    }
    else if(unicode <= 0xd7FF)
    {
        return 3;
    }
    else if((unicode <= 0xdFFF) || (unicode > 0x10FFFF))
    {
        ITF_ASSERT(!"character is not supported");
        return 0;
    }
    else if(unicode <= 0xFFFF)
    {
        return 3;
    }
    else 
    {
        return 4;
    }
}

int    String::getU8Len(u8 utf8)
{
    if(isU8Single(utf8))
    {
        return 1;
    }
    else if(isU8Lead(utf8))
    {
        if((utf8 & 0xE0) == 0xC0)
        {
            return 2;
        }
        else if((utf8 & 0xF0) == 0xE0)
        {
            return 3;
        }
        else if((utf8 & 0xF8) == 0xF0)
        {
            return 4;
        }
        else
        {
            ITF_ASSERT(!"character is not supported");
            return 0;
        }
    }
    else
    {
        ITF_ASSERT(!"character is not supported");
        return 0;
    }
}

bool String::isU8Single(u8 utf8)
{
    if(!(utf8 & 0x80))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool String::isU8Lead(u8 utf8)
{
    if((u8)(utf8 - 0xC0) < 0x3E)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool String::isU8Trial(u8 utf8)
{
    if((utf8 & 0xC0) == 0x80)
    {
        return true;
    }
    else
    {
        return false;
    }

}

char* String::wCharToUtf8(const u16 *string)
{
    String localString;

    localString.setText( string );
    return (char*)localString.convertStringClasstoUtf8Str();
}

i32 String::rfind(u16 val) const
{
    i32 len = getLen();
    if (len == 0 || !m_szContent_)
        return String::npos;

    i32 i= len-1;
    while (i >=0)
    {
        if (val == *(m_szContent_+i) )
            return i;

        --i;
    }

    return String::npos;
}

u64 String::hash()
{
    ITF_ASSERT(0); // not implemented;
    if(NULL == m_szContent_)
    {
        return 0;
    }
/*    AG_EncryptionKey key;
    MD5KeyGenerator keyGen;
    keyGen.generateKey(key, (u8*)m_szContent_, GetLen()*sizeof(u16));*/
    u64 ret = 0;
/*    u64* pCipher = (u64*)key;
    ret = *pCipher;*/
    return ret;
}

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable: 4706) // assignment within conditional expression
#endif //ITF_IPAD
bool String::operator <  (   const String& oString    ) const
{
    int ret = 0 ;
    const u16* mdata = cStr();
    const u16* odata = oString.cStr();

    // magic voodoo code - don't change
   while( ! (ret = *(u16 *)mdata - *(u16 *)odata) && *odata)
        ++mdata, ++odata;

    

    return ( ret<0 );
}


void String::updateLength()
{
    if(!m_szContent_)
        m_length = 0;
    else
        m_length = getStringLength(m_szContent_);
}

#ifdef _MSC_VER
#pragma warning( pop ) 
#endif //ITF_IPAD


//////////////////////////////////////////////////////////////////////////
// 
// StringConverter implementation
//  
//////////////////////////////////////////////////////////////////////////

StringConverter::StringConverter(  )
{
    m_pcharData = NULL;
    m_pUTF8Data = NULL;
}

StringConverter::StringConverter( const String& source )
{
    m_pcharData = NULL;
    m_pUTF8Data = NULL;
    setData(source);
}

StringConverter::StringConverter( const u16* source )
{
    m_pcharData = NULL;
    m_pUTF8Data = NULL;
    setData(source);
}

void StringConverter::clear()
{
    if(m_pcharData)
    {
        delete[] m_pcharData;
    }

    if(m_pUTF8Data)
    {
        delete[] m_pUTF8Data;
    }

    m_pcharData = NULL;
    m_pUTF8Data = NULL;
}


const StringConverter& StringConverter::operator = ( const String& oString )
{
    ITF_ASSERT(0); // method declared as private so that no one ever calls it
    return *this;
};

void StringConverter::setData( const String& source, bool _generateUTF8 )
{
    clear();
    m_pcharData = source.getCharCopy();
    if (_generateUTF8)
        m_pUTF8Data = String::wCharToUtf8(source.cStr());
    else
        m_pUTF8Data = NULL;
}

void StringConverter::setData( const u16* source, bool _generateUTF8  )
{
    clear();
    m_pUTF8Data = NULL;

    if(NULL != source)        
    {
        String localStr(source);
        m_pcharData = localStr.getCharCopy();
        if (_generateUTF8)
            m_pUTF8Data = String::wCharToUtf8(source);
    } else
    {
        String localStr("");
        m_pcharData = localStr.getCharCopy();
        if (_generateUTF8)
            m_pUTF8Data = String::wCharToUtf8(source);
    }
    
}

StringConverter::~StringConverter()
{
    clear();
}

const char* StringConverter::getChar() const
{
    return m_pcharData;
}

u32 StringConverter::getCharLength() const
{
    if(!m_pcharData)
    {
        return 0;
    }
    u32 len=0;
    while (m_pcharData[len]) len++;
    return len;
}

const char* StringConverter::getUTF8() const
{
    ITF_ASSERT(m_pUTF8Data);
    return m_pUTF8Data;
}

u32 StringConverter::getUTF8Length() const
{
    if(!m_pUTF8Data)
    {
        return 0;
    }
    u32 len=0;
    while (m_pUTF8Data[len]) len++;
    return len;
}

StringConverter::StringConverter( const u8* source )
{
    clear();
    size_t size = strlen((const char*)source);
    m_pcharData = new_array(char, size + 1, mId_String8);
    strcpy(m_pcharData, (const char*)source);
    m_pcharData[size] = 0;
    m_pUTF8Data = NULL;
}

const StringConverter& StringConverter::operator = ( const StringConverter& _str )
{
    clear();
    size_t size = strlen((const char*)_str.getChar());
    m_pcharData = new_array(char, size + 1, mId_String8);
    strcpy(m_pcharData, (const char*)_str.getChar());
    m_pcharData[size] = 0;
    m_pUTF8Data = NULL;
    return *this;
}


void String::Thread_DataReserve::setFreeNextFrame(u32 _index)
{
    csAutoLock cs(m_csStringToFree);
    m_toBeFreed.push_back(_index);
}

void String::Thread_DataReserve::flushToBeFreed()
{
    csAutoLock cs(m_csStringToFree);
    for (u32 i = 0; i < m_toBeFreed.size(); i++)
        setFree(m_toBeFreed[i]);
    m_toBeFreed.clear();
}

void String::StressTest()
{/*
    DWORD start = GetTickCount();
    // overrun cache (check for early exit overhead)
    for (u32 i=0; i<1024000; i++)
    {
        String* s = new String();
    }

    DWORD stop = GetTickCount();
    DWORD delta = stop-start;
    int a = 0;*/
}

} // namespace ITF

