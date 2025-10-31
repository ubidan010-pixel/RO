#ifndef _ITF_STRING8_H_
#define _ITF_STRING8_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#include "core/UnicodeTools.h"

namespace ITF
{

#define ITF_STRING8_RESERVE_BIT_COUNT 24


//////////////////////////////////////////////////////////////////////////
///
/// String manipulation class
///
class String8
{
private:
    char*       m_szContent_;
    u32         m_capacity, m_length;
    u32         m_reserveIndex : ITF_STRING8_RESERVE_BIT_COUNT,
                m_reserveThread : 8;
    static const u32  Invalid_Index = (1<<ITF_STRING8_RESERVE_BIT_COUNT)-1; //We changed that because of compiler issues

    // Definition for the preallocated memory chunks
    struct Predef_Memory
    {
        Predef_Memory()
#ifdef ASSERT_ENABLED
            : m_cacheSlotIsAvailable(btrue)
#endif // ASSERT_ENABLED
        {
            ITF_MemSet(&data, 0, sizeof(char) * SPACE_PER_RESERVED);
        }

        static const u32 SPACE_PER_RESERVED = 256;

        char         data[SPACE_PER_RESERVED];

#ifdef ASSERT_ENABLED
        bbool       m_cacheSlotIsAvailable;
#endif // ASSERT_ENABLED
    };

    struct Thread_DataReserve
    {
        eThreadId       m_threadIndex;
        u32             m_count;
        Predef_Memory*  m_data;
        SafeArray<u32>  m_toBeFreed;
        SafeArray<u32>  m_FreeIndexesStack;
        ITF_THREAD_CRITICAL_SECTION m_csStringToFree;


        Thread_DataReserve()
        {
            m_threadIndex = eThreadId_count;
            m_count = 0;
            m_data = NULL;
            m_FreeIndexesStack.clear();
        }

        ITF_INLINE bbool isFree(u32 _index)const
        {
            int index = m_FreeIndexesStack.find(_index);
            #ifdef ASSERT_ENABLED
            #ifdef ITF_DEBUG_STRINGS
            if (m_data[_index].m_cacheSlotIsAvailable){
                ITF_ASSERT(index >= 0);}
            else{
                ITF_ASSERT(index < 0);}
            #endif // ITF_DEBUG_STRINGS
            #endif // ASSERT_ENABLED

            return index >= 0; 
        }

        void setFreeNextFrame(u32 _index) ;
        void flushToBeFreed();

        ITF_INLINE void setFree(u32 _index) 
        {
          //  ITF_ASSERT(!isFree(_index));

#ifdef ASSERT_ENABLED
            m_data[_index].m_cacheSlotIsAvailable = btrue;
#endif // ASSERT_ENABLED
            #ifdef ITF_DEBUG_STRINGS
                ITF_ASSERT(m_FreeIndexesStack.find(_index) < 0);               
            #endif //ITF_DEBUG_STRINGS
            m_FreeIndexesStack.push_back(_index);
        }


        ITF_INLINE u32 findFreeIndex() 
        {
            if (!m_FreeIndexesStack.size())
                return 0xFFFFFFFF;
            u32 ret = m_FreeIndexesStack.back();

#ifdef ASSERT_ENABLED
            ITF_ASSERT(m_data[ret].m_cacheSlotIsAvailable); 
            m_data[ret].m_cacheSlotIsAvailable = bfalse; 
#endif // ASSERT_ENABLED
            m_FreeIndexesStack.pop_back();

            return ret;
        }
    };

    struct StaticThreadData
    {
        Predef_Memory*  DataReserveBuffer;
        Thread_DataReserve DataReserve[eThreadId_count];
    };

    bool isOnStack() const
	{ 
        int a;
        iPtr stackDistance = iPtr(this) - iPtr(&a);
        return abs(stackDistance) < 4096;
	} 


public:

    ITF_INLINE bool isEmpty() const
    {
        return !m_szContent_ || (m_szContent_[0] == 0);
    }


    static const char internalEmptyString[1];
    static const String8 emptyString;
    static void createThreadsData();
    static void deleteThreadsData();
    static void updateThreadsData();
    static StaticThreadData* m_staticThreadData;

    static const i32 npos = -1;

//////////////////////////////////////////////////////////////////////////
///
/// remapped +, += and == operators
///
const String8 operator+(const String8 &oString) const;
void operator += (   const String8& oString       );
void operator += (   const char* oString         );
void operator += (   const char wString     );
bool operator == (   const String8& oString       ) const;
bool operator != (   const String8& oString       ) const;
bool operator <  (   const String8& oString    ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Equality operator
/// @param szString content to check for equality 
/// @return return true if both string content are equal; return false otherwise or if szString is NULL.
bool operator == (   const char* szString        ) const;

const String8& operator = ( const String8& oString );


//////////////////////////////////////////////////////////////////////////
///
/// Constructor. Builds a String8 from a wide string
/// @param szString the wide string to store in the String8
///
String8 ( const u16* szString );

//////////////////////////////////////////////////////////////////////////
///
/// Constructor. Builds a String8 from a char string
/// @param szString the char string to store in the String8
///
String8 ( const char* szString );

//////////////////////////////////////////////////////////////////////////
///
/// Constructor. Builds a String8 from another String8
/// @param oString the other string
///
String8 ( const String8& oString );

//////////////////////////////////////////////////////////////////////////
///
/// default constructor
///
String8 ( );

//////////////////////////////////////////////////////////////////////////
///
/// destructor
///
~String8 ( );

//////////////////////////////////////////////////////////////////////////
/// remove starting spaces. Returns THIS
String8& trimStart();

//////////////////////////////////////////////////////////////////////////
/// remove trailing spaces. Returns THIS
String8& trimEnd();

//////////////////////////////////////////////////////////////////////////
/// remove trailing spaces
String8 &trim() {trimStart(); return trimEnd();}

//////////////////////////////////////////////////////////////////////////
/// Check if the string starts with the specified string
bool startsWith(const String8& _start) const;

//////////////////////////////////////////////////////////////////////////
/// Check if the string ends with the specified string
bool endsWith(const String8& _end) const;

//////////////////////////////////////////////////////////////////////////
///
/// Builds a String8 from a char buffer
/// @param szString the string to store in the String8
/// The string will be cleared if szString is NULL
///
void setText ( const char* szString);
void setText ( const char* szString, u32 _length);

void flush();

//////////////////////////////////////////////////////////////////////////
///
/// Builds a String8 from a wide char string
/// @param szString the wide string to store in the String8
/// The string will be cleared if szString is NULL
///
void setText ( const u16* szString);

//////////////////////////////////////////////////////////////////////////
///
/// Builds a String8 from another String8
/// @param oString the other string
///
void setText ( const String8& oString);


void serialize(ArchiveMemory& _archive) const;
void serialize(ArchiveMemory& _archive);

//////////////////////////////////////////////////////////////////////////
///
/// Builds a String8 from arg list (printf-like)
/// @param arg_format see printf documentation
///
void setTextFormat ( const char* arg_format, ... );                                        


//////////////////////////////////////////////////////////////////////////
///
/// clears the contents of a String8
///
void clear ();

//////////////////////////////////////////////////////////////////////////
///
/// compares a String8 to another
/// @param oString the other string to be compared to
/// @param ignoreCase if true, 'a' and 'A' will be considered as identical
/// @return true if Strings are equal
///
bool equals ( const String8& oString, bool ignoreCase ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Returns the data buffer of the string (unicode wide chars)
/// @return the buffer
///
const char* cStr ( ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Copies the content of a String8 inside a char buffer. Values over 255 will be truncated.
/// @return a char buffer. don't forget to MemFree it!
///
char* getCharCopy ( ) const;

//////////////////////////////////////////////////////////////////////////
///
/// returns if the String8 can be converted in number
/// @return the true if the conversion is possible
///
bool isInteger() const;

//////////////////////////////////////////////////////////////////////////
///
/// returns the int value represented by the String8
/// @return the int value
///
i32 atoi32 () const;

//////////////////////////////////////////////////////////////////////////
///
/// returns the u32 value represented by the String8
/// @return the int value
///
u32 atoui32 () const;

//////////////////////////////////////////////////////////////////////////
///
/// returns the i64 value represented by the String8
/// @return the int value
///
i64 atoi64 () const;

//////////////////////////////////////////////////////////////////////////
///
/// returns the u64 value represented by the String8
/// @return the int value
///
u64 atoui64 () const;


//////////////////////////////////////////////////////////////////////////
///
/// returns the f32 value represented by the String8
/// @return the float value
///
f32 atof32 () const;

//////////////////////////////////////////////////////////////////////////
///
/// convert the specified number to string and assign it to this.
/// @param rhs the int value
///
void i32toa(const i32 rhs);

//////////////////////////////////////////////////////////////////////////
///
/// convert the specified number to string and assign it to this.
/// @param rhs the int value
///
void ui32toa(const u32 rhs);

//////////////////////////////////////////////////////////////////////////
///
/// convert the specified number to string and assign it to this.
/// @param rhs the int value
///
void i64toa(const i64 rhs);

//////////////////////////////////////////////////////////////////////////
///
/// convert the specified number to string and assign it to this.
/// @param rhs the int value
///
void ui64toa(const u64 rhs);

//////////////////////////////////////////////////////////////////////////
///
/// convert the specified number to string and assign it to this.
/// @param rhs the int value
///
void f32toa(const f32 rhs);

//////////////////////////////////////////////////////////////////////////
///
/// Returns the length (in # of characters) of the String8
/// @return the String8's length
///
u32 getLen ( ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Tries and finds the first occurrence of a character inside the String8
/// @param reverse set to btrue to reverse-search from the end of the string
/// @param index the address or the variable to set with the index of the found char (filled with -1 if not found)
/// @return the address of the occurrence or NULL
///
char* strstr ( char character, const bbool reverse = bfalse, i32* index = NULL ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Tries and finds the first occurrence of wide char buffer inside the String8
/// @param szString the string to look for
/// @param reverse set to btrue to reverse-search from the end of the string
/// @param index the address or the variable to set with the index of the found occurrence (filled with -1 if not found)
/// @return the address of the occurrence or NULL
///
char* strstr ( const char* szString, const bbool reverse = bfalse, i32* index = NULL ) const;

void truncate ( const u32 newsize );

//////////////////////////////////////////////////////////////////////////
///
/// This determines the buffer size needed to store a Sting 
/// as a UTF-8 sequence byte string, does not include null termination
/// @return the buffer size needed for a String8's conversion to UTF-8 format
///
u32 getSizeOfStringClassAsUtf8 ( ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Converts a String8 class to string of UTF-8 sequence byte null 
/// terminated string 
/// @param pSize outparam that gets filled with the length of string without null termination char
/// @return the String8's conversion to UTF-8 format. Don't forget to MemFree it!
///
char* convertStringClasstoUtf8Str ( u32 *pSize = (u32*)0 ) const;

//////////////////////////////////////////////////////////////////////////
///
/// Builds a String8 from an UTF-8 string
/// @param Utf8String the UTF-8 string to store in the String8
///
void setUTF8Text ( char* Utf8String );

//////////////////////////////////////////////////////////////////////////
///
/// Replaces a character by another
/// @param find the character to find
/// @param replace the character to replace with
///
void replace (const char find, const char replace);

String8 substr(u32 _pos,u32 _lenght) const;
String8 substr(u32 _pos) const;

i32 rfind(char val) const;

//////////////////////////////////////////////////////////////////////////
///
/// converts all the letters of a string to lower case
///
void toLower ( );

//////////////////////////////////////////////////////////////////////////
///
/// converts all the letters of a string to upper case
///
void toUpper ( );

//////////////////////////////////////////////////////////////////////////
///
/// @return string capacity
///
ITF_INLINE u32 capacity() const { return m_capacity; }


private:

//////////////////////////////////////////////////////////////////////////
///
/// tries to find a free slot and initializes the data members
/// @return true if a slot was found
///
bool findFreeSlot();

//////////////////////////////////////////////////////////////////////////
///
/// Copies source string to the content
/// @param source the source to copy from
/// @param nbChars the count of characters to copy
///
void copyToContent(const char* source, u32 nbChars);

//////////////////////////////////////////////////////////////////////////
///
/// Returns a pointer to allocated memory
/// @return a pointer to preallocated memory in the array
///
bool isStringInReserve( );

//////////////////////////////////////////////////////////////////////////
///
/// allocates memory for the required capacity.
/// @param capacity the needed capacity (in chars count, not in bytes)
/// @param clear clear the string when reallocated

void findMemoryForString(u32 capacity, bool clear=true);

//////////////////////////////////////////////////////////////////////////
///
/// Frees the memory associated to a string, whether in the reserve or with a free
/// @param address of the data of the string to remove 
///

void freeMemoryFromString(char* p_strData);


//////////////////////////////////////////////////////////////////////////
///
/// @return the index of the slot allocated for the string or -1 if the string uses conventional memory 
///
u32 findSlotIndex8() const;

//////////////////////////////////////////////////////////////////////////
///
/// flags the slot as free
/// @param slotIndex the index of the slot to free
///
void tagSlotAsFree(u32 slotIndex, bbool _clear = btrue);

//////////////////////////////////////////////////////////////////////////
///
/// Mallocs the required amount of characters
/// @param capacity the capacity in # of characters (not bytes)
///
void allocConventionalMemory(u32 capacity);

void updateLength();

};

// UTF8 to any char type conversion

template <typename CharType>
class String8ToCharType;

template <>
class String8ToCharType<wchar_t> final
{
public:
    String8ToCharType(const String8& str)
        : m_converter{ str.cStr() }
    {
    }

    const wchar_t* get() const
    {
        return m_converter.get();
    }

private:
    UTF8ToWChar m_converter;
};

// UTF16 to UTF16 conversion
template <>
class String8ToCharType<char16_t> final
{
public:
    String8ToCharType(const String8& str)
        : m_converter{ str.cStr() }
    {
    }

    const char16_t* get() const
    {
        return m_converter.get();
    }

private:
    UTF8ToUTF16 m_converter;
};

// UTF8 to UTF32 conversion
template <>
class String8ToCharType<char32_t> final
{
public:
    String8ToCharType(const String8& str)
        : m_converter(str.cStr())
    {
    }

    const char32_t* get() const
    {
        return m_converter.get();
    }

private:
    UTF8ToUTF32 m_converter;
};

using String8ToWChar = String8ToCharType<wchar_t>;
using String8ToChar16 = String8ToCharType<char16_t>;
using String8ToChar32 = String8ToCharType<char32_t>;

} // namespace ITF




#endif //_ITF_STRING8_H_
