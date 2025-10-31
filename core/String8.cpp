#include "precompiled_core.h"

#ifndef _ITF_STRING8_H_
#include "core/string8.h"
#endif //_ITF_STRING8_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#include <ctype.h>

namespace ITF
{
	const char String8::internalEmptyString[1] = {0};
	const String8 String8::emptyString;
	String8::StaticThreadData* String8::m_staticThreadData = NULL;

    void String8::serialize(ArchiveMemory& _Archive)
    {
        if (!_Archive.isReading())
        {
            u32 len = getLen();
            _Archive.serialize(len);
            _Archive.serializeBlock8((u8*)cStr(),len);
            if (m_szContent_)
                m_szContent_[len] = 0;
        }
        else
        {
            u32 len;
            _Archive.serialize(len);
            findMemoryForString(len);
            _Archive.serializeBlock8((u8*)m_szContent_,len);
            if (m_szContent_)
                m_szContent_[len] = 0;
            m_length = len;
        }
    }

    void String8::serialize(ArchiveMemory& _Archive) const
    {
        if (!_Archive.isReading())
        {
            u32 len = getLen();
            _Archive.serialize(len);
            _Archive.serializeBlock8((u8*)cStr(),len);
        }
        else
        {
           ITF_ASSERT(0);
        }
    } 



u32 String8::findSlotIndex8() const
{
    return m_reserveIndex;
}

bool String8::findFreeSlot()
{
    if (!isOnStack() || !m_staticThreadData)
        return false;

    u32 threadIndex = ThreadInfo::m_threadEngineIndex.getValue();
    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[threadIndex];

    u32 index = reserve.findFreeIndex();

    if (index >= reserve.m_count)
        return false;

    m_szContent_ = reserve.m_data[index].data;
    m_capacity = Predef_Memory::SPACE_PER_RESERVED-1;
    clear();
    m_reserveIndex = index;
    m_reserveThread = threadIndex;
    ITF_ASSERT(m_szContent_);

    return true;
}

void String8::tagSlotAsFree(u32 slotIndex, bbool _clear)
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

void String8::allocConventionalMemory(u32 capacity)
{
    m_reserveIndex = Invalid_Index;
    m_capacity = capacity;
    m_szContent_ = new_array(char,capacity+1,mId_String8); // +1 for trailing 0
    clear();
}

void String8::findMemoryForString(u32 capacity, bool _clear)
{
    u32 slotIndex = findSlotIndex8();
    if(Invalid_Index != slotIndex)// if in slots
    {
        ITF_ASSERT(m_szContent_);
        if(capacity < Predef_Memory::SPACE_PER_RESERVED) // If it fits into a reserve's slot
        {
            if (_clear)
            {
                clear();
            }
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
                    clear();
                }
                return;
            }
        }
        if(m_capacity < capacity) // Not enough space to grow
        {
            if ((NULL != m_szContent_) && (_clear))
            {
                ITF_ASSERT(Invalid_Index == findSlotIndex8());
                SF_DEL_ARRAY(m_szContent_);
            }
            allocConventionalMemory(capacity);
        }
    }
    if (_clear)
    {
        clear();
    }
    return;
}

void String8::deleteThreadsData()
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

void String8::updateThreadsData()
{
    if (!m_staticThreadData)
        return; // not yet created

    Thread_DataReserve& reserve = m_staticThreadData->DataReserve[ThreadInfo::m_threadEngineIndex.getValue()];

    reserve.flushToBeFreed();
}

void String8::createThreadsData()
{
    ITF_ASSERT(!m_staticThreadData);
    m_staticThreadData =  newAlloc(mId_String8,StaticThreadData());

    u32 totalCount = 0;
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        totalCount += ThreadSettings::m_settings[threadIdx].m_maxString8;
    }

    m_staticThreadData->DataReserveBuffer = newAlloc(mId_String8,Predef_Memory[totalCount]);

    totalCount = 0;
    for (u32 threadIdx = 0; threadIdx < eThreadId_count; threadIdx++)
    {
        Thread_DataReserve& reserve = m_staticThreadData->DataReserve[threadIdx];
        reserve.m_threadIndex = (eThreadId)threadIdx;
        Synchronize::createCriticalSection(&reserve.m_csStringToFree);

        u32 elemCount = ThreadSettings::m_settings[threadIdx].m_maxString8;
        reserve.m_count = elemCount;
        reserve.m_data = &m_staticThreadData->DataReserveBuffer[totalCount];
        reserve.m_FreeIndexesStack.reserve(elemCount);
        for (u32 i = 0; i < elemCount; i++)
            reserve.m_FreeIndexesStack.push_back(i);
        totalCount += elemCount;
    }
}

String8::String8()
{
    m_reserveIndex = Invalid_Index;
    m_capacity = 0;
    m_szContent_ = NULL;
    m_reserveThread = 0;
    m_length = 0;
}

void String8::copyToContent(const char* source, u32 nbChars)
{
    ITF_ASSERT(NULL != source);
    ITF_ASSERT(NULL != m_szContent_);
    ITF_ASSERT(source != m_szContent_);
    ITF_ASSERT(m_capacity >= nbChars);

    ITF_Memcpy(m_szContent_, source, sizeof(char) * nbChars);
    m_szContent_[nbChars] = 0;
    m_length = nbChars;
}

String8& String8::trimStart()
{
    if (!m_szContent_)
        return *this;


    char *p = m_szContent_;
    char *pStart = p;
    char *pEnd = p+getLen();
    while (p<pEnd && isspace(*p))
    {
        p++;
        m_length--;
    }
    if (p==pStart)
        return *this;

    while (p<=pEnd)
    {
        *pStart = *p;
        pStart++;
        p++;
    }
    return *this;
}

String8& String8::trimEnd()
{
    u32 length = getLen();
    if (length==0)
        return *this;
    const char *pStart = m_szContent_;
    const char *p = m_szContent_+length-1;
    //search for last non-space character
    while(p>=pStart && isspace(*p))
    {
        p--;
    }
    //make p the character following the last (in reverse order) non-blank character
    p++;
    truncate((u32)(p-pStart));
    return *this;
}

bool String8::startsWith(const String8& _start) const
{
    if (_start.isEmpty())
        return true;

    if (getLen() < _start.getLen())
        return false;

    return 0 == memcmp(m_szContent_, _start.cStr(), _start.getLen());
}

bool String8::endsWith(const String8& _end) const
{
    if (_end.isEmpty())
        return true;

    if (getLen() < _end.getLen())
        return false;

    return 0 == memcmp(m_szContent_ + getLen() - _end.getLen(), _end.cStr(), _end.getLen());
}

void String8::truncate(const u32 newsize)
{
    if(newsize>=(u32)getLen()) return; // no need to truncate, keep the current string
    m_szContent_[newsize] = 0; // memory nor reallocated. We just set a 0, memory will be freed anyway.
    m_length = newsize;
}    

void String8::clear()
{
    if (NULL != m_szContent_)
    {
        m_szContent_[0] = 0;
    }
    m_length = 0;
}

void String8::setTextFormat(const char* arg_format, ...)
{

#if defined(ITF_WII)
    va_list  vlist;
    char     buf[256];
    int      stringSize;

    // Get output string
    va_start( vlist, arg_format );
    
    stringSize = vsnprintf( buf, 256, arg_format, vlist );
    va_end( vlist );

	setText(buf);
#else

    #define LOCAL_STR_SIZE 1024
    char str[LOCAL_STR_SIZE];
    va_list argptr;
    va_start(argptr, arg_format);

    while (
#ifdef ITF_WINDOWS
        _vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) < 0)
#elif defined(ITF_X360)
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) < 0)
#elif defined(ITF_PS3)
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) >= (int)(LOCAL_STR_SIZE-1))
#else
        vsnprintf(str, LOCAL_STR_SIZE-1, arg_format, argptr) >= (int)(LOCAL_STR_SIZE-1))
#endif 
    {
        ITF_ASSERT_CRASH(bfalse, "please increase LOCAL_STR_SIZE");
/*
        delete[] str;
        size *= 2;

        if (size >= MAX_STRING_LENGTH)
        {
            if (size == MAX_STRING_LENGTH)
            {
                return;
            }  
            size = MAX_STRING_LENGTH;
        }
        str = newAlloc(mId_String8, char[size]);*/
    }
    va_end(argptr);

//    str[size-1] = 0;

    setText(str);        
 //   delete[] str;
#endif // ITF_WII

    updateLength();

#ifdef  LOCAL_STR_SIZE
    #undef LOCAL_STR_SIZE
#endif

}


void String8::setText(const char* szString)
{
    i32 nbMaxChar = String::getStringLength(szString);

    if(0 == nbMaxChar) // Source doesn't have any chars
    {
        clear();
        return;
    }
    findMemoryForString(nbMaxChar, true);

    copyToContent(szString, nbMaxChar);
    m_length = nbMaxChar;
}


void String8::setText ( const char* szString,u32 _length)
{    
    i32 nbMaxChar = _length;

    if(0 == nbMaxChar) // Source doesn't have any chars
    {
        clear();
        return;
    }
    findMemoryForString(nbMaxChar, true);

    copyToContent(szString, nbMaxChar);
    m_length = nbMaxChar;
}


void String8::setText(const String8& oString)
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

void String8::setText(const u16* szString)
{
    u32 nbMaxChar = String::getStringLength(szString);
    clear();
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
        m_szContent_[i] = (char)szString[i];
    }
    m_szContent_[nbMaxChar] = 0;
    m_length = nbMaxChar;
}

String8::String8(const u16* szString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    setText(szString);
}

String8::String8(const String8& oString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    setText(oString);
}

String8::String8(const char* szString)
{
    m_reserveIndex = Invalid_Index;
    m_szContent_ = NULL;
    m_capacity = 0;
    setText((const char*)szString);
}

const String8& String8::operator =(const String8& oString)
{
    if(this != &oString)
    {
        setText(oString);
    }
    return *this;       
}

void String8::freeMemoryFromString(char* p_strData)
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
            clear();
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

String8::~String8()
{
    this->freeMemoryFromString(m_szContent_);
    //if(m_szContent_ != NULL)
    //  delete[] m_szContent_;
    //ITF_ASSERT(m_szContent_ == NULL);
}

bool    String8::operator == (const String8& oString) const
{
 if(isEmpty())
        return oString.isEmpty(); // avoid calling memcmp on NULL pointers

    if (getLen() != oString.getLen())
        return bfalse;

   
    return 0 == memcmp(m_szContent_, oString.cStr(), getLen());
}

bool    String8::operator != (const String8& oString) const
{
    return !(*this == oString);
}

void String8::toLower()
{
    if(!m_szContent_) return;
    char* pData = m_szContent_;
    while (*pData)
    {
        // convert latin charset
        if((*pData >= (char)'A') && (*pData <= (char)'Z'))
        {
            *pData += ((char)'a' - (char)'A');
        }
        pData++;
    }
}

void String8::toUpper()
{
    if(!m_szContent_) return;
    char* pData = m_szContent_;
    while (*pData)
    {
        // convert latin charset
        if((*pData >= (char)'a') && (*pData <= (char)'z'))
        {
            *pData -= ((char)'a' - (char)'A');
        }
        pData++;
    }
}

bool    String8::equals (const String8& oString, bool ignoreCase) const
{
    if((!m_szContent_) || (!(oString.cStr())))
    {
        return false;
    }
    if (getLen() != oString.getLen())
    {
        return bfalse;
    }
    if(ignoreCase)
    {
        return _strcmpi(cStr(), oString.cStr())==0;
    }
    else
    {
        return (*this == oString);
    }
}

bool    String8::operator == (const char* szString) const
{
    if(!szString)
    {
        return false;
    }

    if(!m_szContent_ && (0 == *szString))
    {
        return true;
    }

    if (!m_szContent_)
    {
        return false;
    }

    if(0 == getLen())
    {
        return (0 == *szString);
    }

    return 0 == memcmp(m_szContent_, szString, getLen());
}


const String8 String8::operator + (const String8 &oString) const
{
    String8 str = "";
    str += (*this);
    str += oString;

    return str;
}

void    String8::operator += (const char* wString)
{
    ITF_ASSERT(wString);
    if(!m_szContent_)
    {
        setText(wString);
        return;
    }
    String8 oString(wString);
    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    char* old_data = m_szContent_;
    int mylen = getLen();
    int olen = oString.getLen();
    int len = mylen + olen;
    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    ITF_Memcpy(&m_szContent_[mylen], oString.cStr(), sizeof(char) * olen);
    m_szContent_[len] = 0;
    m_length = len;

    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}

void    String8::operator += (const char wString)
{
    if(!m_szContent_)
    {
        char str[2] = {wString, 0};
        setText(str);
        return;
    }
    String8 oString;

    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    char* old_data = m_szContent_;
    int mylen = getLen();
    
    int len = mylen + 1;

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

void    String8::operator += (const String8& oString)
{
    if(!m_szContent_)
    {
        setText(oString);
        return;
    }
    bbool oldDataIsInReserve = (m_reserveIndex != Invalid_Index);
    char* old_data = m_szContent_;
    int mylen = getLen();
    int olen = oString.getLen();
    int len = mylen + olen;
    findMemoryForString(len, false);
    if (old_data != m_szContent_)
        copyToContent(old_data, mylen);
    ITF_Memcpy(&m_szContent_[mylen], oString.cStr(), sizeof(char) * olen);
    m_szContent_[len] = 0;
    m_length = len;
    if(old_data != m_szContent_ && (!oldDataIsInReserve)) // if old data was in reserve, it's already been freed by findMemoryForString
    {
        this->freeMemoryFromString(old_data);
    }
}



const char* String8::cStr() const
{
    // NULL IS VALID, DON'T ASSERT IN CASE OF NULL CONTENT
    if(NULL == m_szContent_)
    {
        return (const char*)String8::internalEmptyString;
    }
    return m_szContent_;
}


bool String8::isInteger() const
{
    u32 size   = getLen();
    u32 offset = 0;
//    bool negate      = false;

    // check for minus sign
    if (m_szContent_[offset] == '-')
    {
        offset++;
    }

    for (; offset < size; offset++)
    {
        if (m_szContent_[offset] < '0' ||
            m_szContent_[offset] > '9')
        {
            return false;
        }
    }
    
    return true;
}

i32 String8::atoi32() const
{
    return (i32)atoi64();
}

u32 String8::atoui32() const 
{
    return (u32)atoui64();
}

i64 String8::atoi64() const
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
    if (m_szContent_[offset] == '-')
    {
        negate = true;
        offset++;
    }

    while (offset < size)
    {
        if (m_szContent_[offset] >= '0' &&
            m_szContent_[offset] <= '9')
        {
            ret = ret * 10 + m_szContent_[offset] - '0';
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

u64 String8::atoui64 () const
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
        if (m_szContent_[offset] >= '0' &&
            m_szContent_[offset] <= '9')
        {
            ret = ret * 10 + m_szContent_[offset] - '0';
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

f32 String8::atof32() const
{
    return (f32)atof((const char*)cStr());
}

void String8::i32toa(const i32 rhs)
{
    setTextFormat("%d", rhs);
}

void String8::ui32toa(const u32 rhs)
{ 
    setTextFormat("%u", rhs);
}

void String8::i64toa(const i64 rhs)
{  
    #ifdef ITF_PS3
        setTextFormat("%lld", rhs);
    #else
        setTextFormat("%I64d", rhs);
    #endif
}

void String8::ui64toa(const u64 rhs)
{  
    #ifdef ITF_PS3
        setTextFormat("%llu", rhs);
    #else
        setTextFormat("%I64u", rhs);
    #endif
}

void String8::f32toa(const f32 rhs)
{  
    setTextFormat("%f", rhs);
}

u32 String8::getLen() const
{
/*
    if(!m_szContent_)
    {
        return 0;
    }
    int len=String::getStringLength(m_szContent_);
    ITF_ASSERT((int)m_length == len);*/
    return m_length;
}

char* String8::getCharCopy() const
{
    int size = getLen();
    if(!size)
    {
        char* ret = new char[1];
        *ret = 0;
        return ret;
    }

    char* ret = new_array(char,size+1,mId_String8);
    ITF_Memcpy(ret,m_szContent_,size);
    ret[size]=0;
    return ret;
}


char*   String8::strstr(char character, const bbool reverse, i32* index) const
{
    i32 idx = -1;
    if (index)
        *index = -1;

    if(!m_szContent_) 
        return NULL;

    char* ret = NULL;
    if (reverse)
    {
        ret = m_szContent_ + getLen();
        ITF_ASSERT(*ret == 0);
        ret--;
        idx = getLen()-1;
        while ((ret >= m_szContent_) && (((char)*ret) != character)) 
        {
            idx--;
            ret--;
        }
        if(((char)*ret) == character) 
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
        while ((*ret) && (((char)*ret) != character))
        {
            idx++;
            ret++;
        }
        if(((char)*ret) == character)
        {
            if (index)
                *index = idx;
            return ret;
        }
    }

    return NULL;
}   

char*   String8::strstr(const char* szString, const bbool reverse, i32* index) const
{
    i32 idx = -1;
    if (index)
        *index = -1;

    if(!szString)
        return NULL;

    if(!m_szContent_) 
        return NULL;

    int len=String::getStringLength(szString);
    int size = getLen();

    if(size < len)
        return NULL;

    // Brute Force Algorithm
    char* ret;
    bool stringFound = false;

    if (reverse)
    {
        ret = m_szContent_ + getLen();
        ITF_ASSERT(*ret == 0);
        ret -= len;
        idx = getLen()-len;
        while (ret >= m_szContent_)
        {
            if(memcmp(szString, ret, len*sizeof(char))== 0)
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
        for(ret=m_szContent_; ((*ret != 0) && ((ret + len) <= (m_szContent_ + size))); ret++)
        {
            if(memcmp(szString, ret, len*sizeof(char))== 0)
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




void String8::replace (const char find, const char replace)
{
    if(!m_szContent_) 
        return;

    char* scan = m_szContent_;
    while (*scan)
    {
        if ((char)(*scan) == find)
            *scan = (char)replace;
        scan++;
    }
}

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable: 4706) // assignment within conditional expression
#endif

bool String8::operator <  (   const String8& oString    ) const
{
    int ret = 0 ;
    const char* mdata = cStr();
    const char* odata = oString.cStr();


    // magic voodoo code - don't change
    while( ! (ret = *(char *)mdata - *(char *)odata) && *odata)
        ++mdata, ++odata;

    

    return ( ret<0 );
}
#ifdef _MSC_VER
#pragma warning( pop ) 
#endif

String8 String8::substr(u32 _pos,u32 _length) const
{
    String8 res;

    if ((_pos+_length)<= getLen())
    {
        res.setText(m_szContent_+_pos,_length);
    }

    return res;
}

String8 String8::substr(u32 _pos) const
{
    String8 res;

    if (_pos< getLen())
    {
        res.setText(m_szContent_+_pos);
    }

    return res;
}

i32 String8::rfind(char val) const
{
    i32 len = getLen();
    if (len == 0 || !m_szContent_)
        return String8::npos;

    i32 i= len-1;
    while (i >=0)
    {
        if (val == *(m_szContent_+i) )
            return i;

        --i;
    }

    return String8::npos;
}

void String8::Thread_DataReserve::setFreeNextFrame(u32 _index)
{
    csAutoLock cs(m_csStringToFree);
    m_toBeFreed.push_back(_index);
}

void String8::Thread_DataReserve::flushToBeFreed()
{
    csAutoLock cs(m_csStringToFree);
    for (u32 i = 0; i < m_toBeFreed.size(); i++)
        setFree(m_toBeFreed[i]);
    m_toBeFreed.clear();
}

void String8::updateLength()
{
    if(!m_szContent_)
        m_length = 0;
    else
        m_length = String::getStringLength(m_szContent_);
}

void String8::flush()
{
    this->freeMemoryFromString(m_szContent_);
    m_szContent_ = NULL;
    m_length = 0;
}


} // namespace ITF

