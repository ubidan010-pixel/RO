#ifndef _ITF_STRING_H_
#define _ITF_STRING_H_

#include <wchar.h>

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#include "core/UnicodeTools.h"

namespace ITF
{

class String;
class String8;

#define ITF_STRING_RESERVE_BIT_COUNT 24
//#define ITF_DEBUG_STRINGS
#ifdef  ITF_DEBUG_STRINGS
    #define STR_CHECK_CACHE          String::checkCache();
    #define STR_CHECK_CACHE_INSIDE   if (!m_doNotCache) String::checkCache();
#else
    #define STR_CHECK_CACHE
    #define STR_CHECK_CACHE_INSIDE
#endif //ITF_DEBUG_STRINGS

//////////////////////////////////////////////////////////////////////////
///
/// String manipulation class
///
class String
{
protected:
    bbool   m_doNotCache;

private:
    u16*    m_szContent_;
    u32     m_capacity, m_length;
    u32         m_reserveIndex : ITF_STRING_RESERVE_BIT_COUNT,
                m_reserveThread : 8;
    static const u32  Invalid_Index = (1<<ITF_STRING_RESERVE_BIT_COUNT)-1; //We changed that because of compiler issues

    // Definition for the preallocated memory chunks
    struct Predef_Memory
    {
        Predef_Memory()
#ifdef ASSERT_ENABLED
            : m_cacheSlotIsAvailable(btrue)
#endif // ASSERT_ENABLED
        {
            ITF_MemSet(&data, 0, sizeof(u16) * SPACE_PER_RESERVED);
        }

        static const u32 SPACE_PER_RESERVED = 128;

        u16         data[SPACE_PER_RESERVED];

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
        bbool           m_dictionaryWasFull;
        ITF_THREAD_CRITICAL_SECTION m_csStringToFree;


        Thread_DataReserve()
        {
            m_threadIndex = eThreadId_count;
            m_count = 0;
            m_data = NULL;
            m_FreeIndexesStack.clear();
            m_dictionaryWasFull = bfalse;
        }

        #ifdef ASSERT_ENABLED
            ITF_INLINE bbool isFree(u32 _index)const
            {

                ITF_ASSERT((u32)ThreadInfo::m_threadEngineIndex.getValue() == (u32)m_threadIndex);
                #ifdef ITF_DEBUG_STRINGS
                    int index = m_FreeIndexesStack.find(_index);
                    if (m_data[_index].m_cacheSlotIsAvailable)
                    {
                        ITF_ASSERT(index >= 0);
                    }
                    else
                    {
                        ITF_ASSERT(index < 0);
                    }
                    return index >= 0; 
                #endif
                return m_data[_index].m_cacheSlotIsAvailable;
            }
        #endif

        void setFreeNextFrame(u32 _index) ;
        void flushToBeFreed();

        ITF_INLINE void setFree(u32 _index) 
        {
            STR_CHECK_CACHE

            #ifdef ASSERT_ENABLED
                ITF_ASSERT(!isFree(_index));
                m_data[_index].m_cacheSlotIsAvailable = btrue;
            #endif // ASSERT_ENABLED

            m_FreeIndexesStack.push_back(_index);
            STR_CHECK_CACHE
        }


        ITF_INLINE u32 findFreeIndex() 
        {
            STR_CHECK_CACHE

            ITF_ASSERT((u32)ThreadInfo::m_threadEngineIndex.getValue() == (u32)m_threadIndex);
            if (!m_FreeIndexesStack.size())
                return 0xFFFFFFFF;
            u32 ret = m_FreeIndexesStack.back();

#ifdef ASSERT_ENABLED
            ITF_ASSERT(isFree(ret)); 
            m_data[ret].m_cacheSlotIsAvailable = bfalse; 
#endif // ASSERT_ENABLED
            m_FreeIndexesStack.pop_back();
            STR_CHECK_CACHE

            return ret;
        }
    };

    struct StaticThreadData
    {
        Predef_Memory*  DataReserveBuffer;
        Thread_DataReserve DataReserve[eThreadId_count];
    };

    #ifdef  ITF_DEBUG_STRINGS
            static void checkCache();
    #endif //ITF_DEBUG_STRINGS

public:

    //ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(eXALLOCAllocatorId_String16);
    static void StressTest();
    void* operator new[](size_t size) throw()			
    {													
         return ITF::Memory::malloc(size);		        
    }	

    bool isOnStack() const
	{ 
        int a;
        iPtr stackDistance = iPtr(this) - iPtr(&a);
        return abs(stackDistance) < 4096;
	}

    static void createThreadsData();
    static void deleteThreadsData();
    static void updateThreadsData();
    static StaticThreadData* m_staticThreadData;
    static const u16 internalEmptyString[1];
    static const String emptyString;

    static u32 getStringLength(const u16* _str);
    static u32 getStringLength(const char* _str);
    static u32 getStringLength(const u8* _str) {return getStringLength((const char*)_str);}
    static bbool isSameString(const char* _str1, const char* _str2);

    static const i32 npos;


    //////////////////////////////////////////////////////////////////////////
    ///
    /// remapped +, += and == operators
    ///
    const String operator+(const String &oString) const;
    const String operator+(const u16* _oString) const;
    const String operator+(const char* _oString) const;
    void operator += (   const String& oString       );
    void operator += (   const char* oString         );
    void operator += (   const u16 wString     );
    void operator += (   const u16* wString     );
    bool operator == (   const String& oString       ) const;
    bool operator != (   const String& oString       ) const;
    bool operator <  (   const String& oString    ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Equality operator
    /// @param szString content to check for equality 
    /// @return return true if both string content are equal; return false otherwise or if szString is NULL.
    bool operator == (   const char* szString        ) const;
    bool operator == (   const u16* szString    ) const;

    const String& operator = ( const String& oString );

    #if ITF_UNITTEST > 0
        static void unitTest();
    #endif // ITF_UNITTEST

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Constructor. Builds a String from a wide string
    /// @param szString the wide string to store in the String
    ///
    String ( const u16* szString );

#ifdef ITF_WCHAR_IS_16BIT
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Variant for wchar_t assuming wchar_t as a 16 bits object
    ///
    String(const wchar_t* szString)
        : String(reinterpret_cast<const u16*>(szString))
    {
    }
#endif

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Constructor. Builds a String from a char string
    /// @param szString the char string to store in the String
    ///
    String ( const char* szString );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Constructor. Builds a String from a char string
    /// @param szString the char string to store in the String
    ///
    String ( const String8& _szString );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Constructor. Builds a String from another String
    /// @param oString the other string
    ///
    String ( const String& oString );

    String( const String& oString,u32 _length);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// default constructor
    ///
    String ( );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// destructor
    ///
    ~String ( );


    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from a wide string
    /// @param szString the wide string to store in the String
    /// The string will be cleared if szString is NULL
    void setText ( const u16* szString);
    void setText ( const u16* szString,u32 _length);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from a char* string
    /// @param szString the wide string to store in the String
    /// The string will be cleared if szString is NULL
    ///
    void setText ( const char* szString);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from another String
    /// @param oString the other string
    ///
    void setText ( const String& oString);


    void serialize(ArchiveMemory& _archive) const;
    void serialize(ArchiveMemory& _archive);
    u32  getSerializeSize() const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from arg list (printf-like)
    /// @param arg_format see printf documentation
    ///
    void setTextFormat ( const char* arg_format, ... );                                        

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from arg list (printf-like) Unicode Mode
    /// @param arg_format see printf documentation
    ///
    void setStringFormat(String arg_format, ...);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// clears the contents of a String
    ///
    void clear ();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// compares a String to another
    /// @param oString the other string to be compared to
    /// @param ignoreCase if true, 'a' and 'A' will be considered as identical
    /// @return true if Strings are equal
    ///
    bool equals ( const String& oString, bool ignoreCase ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Returns the data buffer of the string (unicode wide chars)
    /// @return the buffer
    ///
    const u16* cStr ( ) const;

    //////////////////////////////////////////////////////////////////////////
    /// Check if the string starts with the specified string
    bool startsWith(const String& _start) const;

    //////////////////////////////////////////////////////////////////////////
    /// Check if the string ends with the specified string
    bool endsWith(const String& _end) const;

#ifdef ITF_WCHAR_IS_16BIT
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Variants using wchar_t assuming wchar_t as a 16 bits object
    ///
    static_assert(sizeof(wchar_t) == sizeof(u16));

    void setText(const wchar_t* szString)
    {
        setText(reinterpret_cast<const u16*>(szString));
    }

    void setText(const wchar_t* szString, u32 _length)
    {
        setText(reinterpret_cast<const u16*>(szString), _length);
    }

    const wchar_t* wcharCStr() const { return reinterpret_cast<const wchar_t*>(cStr()); }
#endif

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Copies the content of a String inside a char buffer. Values over 255 will be truncated.
    /// @return a char buffer. don't forget to MemFree it!
    ///
    char* getCharCopy ( ) const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// copy a char version into the given output bufer
    /// @param _outBuffer The output buffer
    /// @param _bufferSize The size of the buffer
    void charCopyToBuffer(char _outBuffer[], u32 _bufferSize) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Copies the content of a String inside a wide char buffer.
    /// @return a wide char buffer. don't forget to MemFree it!
    ///
    u16* getWordCopy ( ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns the int value represented by the String
    /// @return the int value
    ///
    i32 atoi32 () const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns the u32 value represented by the String
    /// @return the int value
    ///
    u32 atoui32 () const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns the i64 value represented by the String
    /// @return the int value
    ///
    i64 atoi64 () const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns the u64 value represented by the String
    /// @return the int value
    ///
    u64 atoui64 () const;


    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns the f32 value represented by the String
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
    /// Returns the length (in # of characters) of the String
    /// @return the String's length
    ///
    u32 getLen ( ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Tries and finds the first occurrence of a character inside the String
    /// @param reverse set to btrue to reverse-search from the end of the string
    /// @param index the address or the variable to set with the index of the found char (filled with -1 if not found)
    /// @return the address of the occurrence or NULL
    ///
    u16* strstr ( char character, const bbool reverse = bfalse, i32* index = NULL ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Tries and finds the first occurrence of wide char buffer inside the String
    /// @param szString the string to look for
    /// @param reverse set to btrue to reverse-search from the end of the string
    /// @param index the address or the variable to set with the index of the found occurrence (filled with -1 if not found)
    /// @return the address of the occurrence or NULL
    ///
    u16* strstr ( const u16* szString, const bbool reverse = bfalse, i32* index = NULL, i32 startIndex=0 ) const;

    String substr(u32 _pos, u32 _lenght) const;
    String substr(u32 _pos) const;

    i32 rfind(u16 val) const;

    void truncate ( const u32 newsize );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// This determines the buffer size needed to store a Sting 
    /// as a UTF-8 sequence byte string, does not include null termination
    /// @return the buffer size needed for a String's conversion to UTF-8 format
    ///
    u32 getSizeOfStringClassAsUtf8 ( ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Converts a String class to string of UTF-8 sequence byte null 
    /// terminated string 
    /// @param pSize outparam that gets filled with the length of string without null termination char
    /// @param ret outparam is the buffer receiving result
    /// @return the String's conversion to UTF-8 format. Don't forget to MemFree it if you dont have set the ret buffer!
    ///
    u8* convertStringClasstoUtf8Str ( u32 *pSize = (u32*)0, u8 *ret = (u8*)0 ) const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Builds a String from an UTF-8 string
    /// @param Utf8String the UTF-8 string to store in the String
    ///
    void setUTF8Text ( u8* Utf8String );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Replaces a character by another
    /// @param find the character to find
    /// @param replace the character to replace with
    ///
    void replace (const char find, const char replace);
    void replace (const String& _toFind, const String& _replace);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Convert a unicode value to UTF-8 sequence byte string
    /// @param _pRes is of 4Bytes buffer to receive convertin result.
    /// @param unicode the unicode character to be converted
    /// @return the character's conversion to UTF-8 format.
    /// @throws assert if the character can't be converted
    ///
    static u8* convertUnicodeChartoUtf8 ( u8* _pRes, const u32 szString );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Convert an encoded UTF-8 byte sequence to unicode codepoint value
    /// @param utf8Str the UTF-8 char to convert to wide char
    /// @param olen length of UTF-8 byte sequence
    /// @return the converted wide string character
    ///
    static u32 convertUtf8toUnicodeChar ( const u8* utf8Str, size_t olen );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Returns the size of the UTF-8 encoding for a given unicode wide string character
    /// @param unicode 32-bit unicode character
    /// @return 1..4 or 0 if unicode is asurrogate or not unicode code point 
    /// @throws assert if character is not supported
    ///
    static int getUnicodeLength ( const u32 unicode );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Returns the number of code units used for the UTF-8 encoding
    /// @param utf8 8-bit code unit a lead or a single byte
    /// @return 1..4 or 0 if not a lead or single byte 
    /// @throws assert if the character is not supported
    ///
    static int getU8Len ( u8 utf8 );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Does this code a UTF-8 encode a code point by itself (US-ASCII 0..0x7F)?
    /// @param utf8 8-bit code unit (byte)
    /// @return true if character is in the US-ASCII range. returns false otherwise
    ///
    static bool isU8Single ( u8 utf8 );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Is this code a UTF-8 lead byte?
    /// @param utf8 8-bit code unit (byte)
    /// @return true if character is UFT-8 lead. returns false otherwise
    ///
    static bool isU8Lead ( u8 utf8 );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Is this code a UTF-8 trail byte?
    /// @param utf8 8-bit code unit (byte)
    /// @return true if character is UFT-8 trial. returns false otherwise
    ///
    static bool isU8Trial ( u8 utf8 );
    static char* wCharToUtf8 ( const u16 *string );

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
    /// calculate a CRC based on the string
    ///
    u64 hash();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// returns true if the string is empty
    /// @return returns true if the string is empty
    ///
    bool isEmpty() const;


    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return string capacity
    ///
    ITF_INLINE u32 capacity() const { return m_capacity; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// test if dictionary was once full. Only meaningfull when debugging (not final)
    static bbool wasDictionaryFull_DEBUG() {if (!m_staticThreadData) return bfalse; return m_staticThreadData->DataReserve[ThreadInfo::m_threadEngineIndex.getValue()].m_dictionaryWasFull;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// clear the flag which was set when dictionary is full
    static void clearFullDictionaryFlag_DEBUG()  {if (m_staticThreadData) m_staticThreadData->DataReserve[ThreadInfo::m_threadEngineIndex.getValue()].m_dictionaryWasFull = bfalse;}

    template <typename Operator>
    static void tokenize(Operator& op, const wchar_t* input, const wchar_t* delimiters)
    {  
        const wchar_t* s = input;   
        const wchar_t* e = s;   

        while (*e != 0) 
        { 
            e = s;     
            while (*e != 0 && wcschr(delimiters, *e) == 0) ++e;   
            if (e - s > 0) 
            {
                op(s, u32(e - s));    
            } 

            s = e + 1;   
        }
    } 

private:

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// empty the string content if already allocated
    void clearContent();
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
    void copyToContent(const u16* source, u32 nbChars);

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

    void freeMemoryFromString(u16* p_strData);


    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the index of the slot allocated for the string or -1 if the string uses conventional memory 
    ///
    u32 findSlotIndex() const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// flags the slot as free
    /// @param slotIndex the index of the slot to free
    ///
    void tagSlotAsFree(u32 slotIndex, bool _clear = true);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Mallocs the required amount of characters
    /// @param capacity the capacity in # of characters (not bytes)
    ///
    void allocConventionalMemory(u32 capacity);

    void updateLength();
};


// UTF16 to any char type conversion

template <typename CharType>
class StringToCharType;

template <>
class StringToCharType<wchar_t> final
{
public:
    StringToCharType(const String& str)
        : m_converter{ str.cStr() }
    {
    }

    const wchar_t* get() const
    {
        return m_converter.get();
    }

private:
    UTF16ToWChar m_converter;
};

template <>
class StringToCharType<char16_t> final
{
public:
    StringToCharType(const String& str)
        : m_copy{ str }
    {
    }

    const char16_t* get() const
    {
        return reinterpret_cast<const char16_t*>(m_copy.cStr());
    }

private:
    String m_copy;
};

// UTF16 to UTF8 conversion
template <>
class StringToCharType<char> final
{
public:
    StringToCharType(const String& str)
        : m_converter(str.cStr())
    {
    }

    const char* get() const
    {
        return m_converter.get();
    }

private:
    UTF16ToUTF8 m_converter;
};

// UTF16 to UTF32 conversion
template <>
class StringToCharType<char32_t> final
{
public:
    StringToCharType(const String& str)
        : m_converter(str.cStr())
    {
    }

    const char32_t* get() const
    {
        return m_converter.get();
    }

private:
    UTF16ToUTF32 m_converter;
};

using StringToWChar = StringToCharType<wchar_t>;
using StringToChar16 = StringToCharType<char16_t>;
using StringToUTF8 = StringToCharType<char>;
using StringToChar32 = StringToCharType<char32_t>;

///////////////////////////////////////////////////////////////////////////////////////////
class String_Uncached : public String
{
public:
    String_Uncached() : String() {m_doNotCache = btrue;}

    String_Uncached ( const String_Uncached& _oString ) : String() {m_doNotCache = btrue;setText(_oString);}

    String_Uncached ( const u16* _str ) : String()  {m_doNotCache = btrue;setText(_str);}

    String_Uncached ( const char* _str ) : String()  {m_doNotCache = btrue;setText(_str);}

    String_Uncached ( const String8&  ) : String()  {ITF_ASSERT(bfalse);}

    String_Uncached ( const String& _oString ) : String()  {m_doNotCache = btrue;setText(_oString);}

    String_Uncached( const String& ,u32 ) : String()  {ITF_ASSERT(bfalse);}

};

///////////////////////////////////////////////////////////////////////////////////////////
class StringConverter
{
public:
    StringConverter( const String& source );
    StringConverter( const u16* source );
    StringConverter( const u8* source );
    StringConverter(  );
    ~StringConverter();

    const StringConverter& operator = ( const StringConverter& _str );

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the char* version of the String
    ///
    const char* getChar() const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the length (in bytes) of the String's char version
    ///
    u32 getCharLength () const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the UTF8 version of the String
    ///
    const char* getUTF8() const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the length (in bytes) of the String's UTF8 version
    ///
    u32 getUTF8Length () const;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Sets the source data
    /// @param source the source String
    ///
    void setData(const String& source, bool _generateUTF8 = bfalse);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Sets the source data
    /// @param source the source String
    ///
    void setData(const u16* source, bool _generateUTF8 = bfalse);

private:
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Frees the memory
    ///
    void clear();

    const StringConverter& operator = ( const String& oString );


    char*   m_pUTF8Data;
    char*   m_pcharData;
};

} // namespace ITF



#endif //_ITF_STRING_H_
