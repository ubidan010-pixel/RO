#ifndef _ITF_CORETYPES_H_
#define _ITF_CORETYPES_H_

#include "Macros.h"

#include <stdint.h>
#include <string.h>
#include <float.h>

// Do not set it to max 32 bit int to allow -1 in enum (otherwise, need 64bit)
#define ENUM_FORCE_SIZE_32(rr) align_##rr = 0x0FFFFFFFu

namespace ITF
{
    class BaseObject;

    enum DataType
    {
        DataType_Unknown=0,
        DataType_Blob,
        DataType_i8,
        DataType_u8,
        DataType_i16,
        DataType_u16,
        DataType_i32,
        DataType_u32,
        DataType_i64,
        DataType_u64,
        DataType_String,
        DataType_Bool,
        DataType_BinaryBlock,
        DataType_FlexibleValue,
        DataType_f32,
        DataType_f64,
        DataType_String8,
        DataType_Count, // must remain last enum element
        ENUM_FORCE_SIZE_32(DataType)
    };

//Scheduling support
#define SUPPORTED_SCHEDULING

/////////////////////////////////////////////////////////////////////////////
// Declaration of basic types
/////////////////////////////////////////////////////////////////////////////
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using BYTE = u8;
using bbool = int;
using f32 = float;
using f64 = double;

// Types to handle pointers as integer values (to use wisely)
using uPtr = uintptr_t;
using iPtr = intptr_t;

// size of objects 
// should be kept equivalent to ux/ix
using uSize = u32;
using iSize = i32;

// integer of undefined size, use it for array loop/indexing
using ux = uSize;
using ix = iSize;

// Specific value of unsigned integers that is assumed invalid by some systems.
// Roughly equal to the maximum value
constexpr   u8              U8_INVALID = u8(i8(-1));
constexpr   u16             U16_INVALID = u16(i16(-1));
constexpr   u32             U32_INVALID = u32(i32(-1));
constexpr   u64             U64_INVALID = u32(i32(-1));
constexpr   ux              UX_INVALID = ux(ix(-1));
constexpr   uSize           USIZE_INVALID = uSize(iSize(-1));
constexpr   uPtr            UPTR_INVALID = uPtr(iPtr(-1));

// Float max
constexpr   f32             F32_INFINITY = 3.402823466e+38F; // See FLT_MAX
constexpr   f32             F32_BIG_EPSILON = 0.0005f;
constexpr   f32             F32_INVALID = F32_INFINITY;

// Min/Max values for integers
constexpr   i32             I32_MIN = (-2147483647 - 1);
constexpr   i32             I32_MAX = 2147483647;

constexpr   bbool           btrue = 1;
constexpr   bbool           bfalse = 0;

#if !defined(ITF_WINDOWS) && !defined(ITF_XBOX_SERIES)
    // typedefs for the windows type compatibility
    typedef u32        DWORD;
    typedef u64        HANDLE;

    union LARGE_INTEGER
    {
        struct {
            u32 LowPart;
            u32 HighPart;
        };
        struct {
            u32 LowPart;
            u32 HighPart;
        } u;
        u64 QuadPart;
    };
    typedef LARGE_INTEGER * PLARGE_INTEGER;
#endif

class ObjectRef
{
public:
    ObjectRef()
    {
        invalidate();
    }
    ObjectRef(u32 _val) : m_directValue(_val) { ITF_ASSERT_CRASH(_val != 0xffffffff, "U32_INVALID is forbidden for ObjectRefs, use ITF_INVALID_OBJREF");  }; // assert that U32_INVALID is not used, this is not the right default value

    BaseObject* getObject() const;
    ITF_INLINE  bool isValid() const {return getValue() != ITF_INVALID_OBJREF;}
    ITF_INLINE  void invalidate() {m_directValue = ITF_INVALID_OBJREF;}
    ITF_INLINE  bool operator == (const ObjectRef& _ref) const {return getValue() == _ref.getValue();}
    ITF_INLINE  bool operator != (const ObjectRef& _ref) const {return getValue() != _ref.getValue();}
    ITF_INLINE  bool operator < (const ObjectRef& _ref) const {return getValue() < _ref.getValue();}
    ITF_INLINE  void setValue(u32 _value) {ITF_ASSERT(_value != 0xffffffff); m_directValue = _value;}
    ITF_INLINE  u32 getValue() const {return m_directValue;}

#ifdef GPP_DEBUG
    const BaseObject* m_pointer;
#endif //GPP_DEBUG

    static const ObjectRef InvalidRef;
    u32         m_directValue;  // PLEASE USE setValue() AND getValue() !!!! This member is public because of the serialization
};
typedef void (*processObjectRef) (ObjectRef _actorRef);

#ifdef ITF_WINDOWS
    typedef void (_stdcall *callbackProgressBuild)(float _percent);
#endif

#if !defined(ITF_MICROSOFT)
#define WINAPI   
#endif

// ITFCOLOR_AS_XYZW represent the representation of the color when stored in an 32 bits integer.
// On little endian, the storage as a byte array will be W, Z, Y X, and on big endian it will be X, Y, Z, W
#if defined(ITF_WINDOWS) || defined(ITF_X360) || defined(ITF_CTR) || defined(ITF_PS5)
    #define ITFCOLOR_AS_ARGB
#elif defined(ITF_NINTENDO) || defined(ITF_XBOX_SERIES)
    #define ITFCOLOR_AS_ABGR
#else // defined(ITF_WINDOWS) || defined(ITF_X360)
    #define ITFCOLOR_AS_RGBA
#endif // defined(ITF_WINDOWS) || defined(ITF_X360)

class dbId
{
private:
    u64                 m_id;
public:
                        dbId    (         ) {    m_id = 0;}
                        dbId    ( u64 _id ) {    m_id = _id;}
                        dbId    ( const dbId& _id ) { m_id = _id.getId();}
    ITF_INLINE u64      getId   (          ) const {return m_id;}
    bool    operator    ==      (   const dbId& oId    ) const {return oId.getId() == getId();}
    bool    operator    <       (   const dbId& oId    ) const {  return getId() < oId.getId();}
};

#ifndef GPP_DEBUG
ITF_COMPILE_TIME_ASSERT (ObjectRef_size_error, sizeof(ObjectRef) == 4);
#endif
ITF_COMPILE_TIME_ASSERT (u8_size_error, sizeof(u8) == 1);
ITF_COMPILE_TIME_ASSERT (i8_size_error, sizeof(i8) == 1);
ITF_COMPILE_TIME_ASSERT (u16_size_error, sizeof(u16) == 2);
ITF_COMPILE_TIME_ASSERT (i16_size_error, sizeof(i16) == 2);
ITF_COMPILE_TIME_ASSERT (u32_size_error, sizeof(u32) == 4);
ITF_COMPILE_TIME_ASSERT (i32_size_error, sizeof(i32) == 4);

#if defined(ITF_PS3) || defined(ITF_X360) || defined(ITF_WII)
#define ITF_BITFIELDS_LITTLE_ENDIAN
#endif

// No targeted big endian platform for the moment...
#define ITF_LITTLE_ENDIAN 1

#ifdef ITF_BITFIELDS_LITTLE_ENDIAN
#define DECL_BF(A,B) B A
#else
#define DECL_BF(A,B) A B
#endif

#if (defined(ITF_WINDOWS) && !defined(ITF_FINAL)) || ((defined (ITF_X360)||defined (ITF_PS3)||defined(ITF_PS5)||defined(ITF_NINTENDO)) && defined(_DEBUG))
#define ITF_DEBUG_STRINGID
#endif

struct ObjectId
{
    static const u32 RESERVED_BITS = 10;
    static const u32 USER_BITS = 12;
    static const u32 YEAR_BITS = 4;
    static const u32 MONTH_BITS = 4;
    static const u32 DAY_BITS = 5;
    static const u32 HOUR_BITS = 5;
    static const u32 MINUTE_BITS = 6;
    static const u32 SECOND_BITS = 6;
    static const u32 SESSION_BITS = 12;
    ITF_COMPILE_TIME_ASSERT(invalid_bit_count_for_ObjectId_bitfield,(RESERVED_BITS + USER_BITS + YEAR_BITS + MONTH_BITS + DAY_BITS + HOUR_BITS + MINUTE_BITS + SECOND_BITS + SESSION_BITS) == 64);

#ifdef ITF_MSC_VER
    #pragma warning(push,3)
    #pragma warning(disable: 4201)
#endif

#ifdef ITF_WII
	static const u32 MONTH_WII_BITS = 6;
	static const u32 DAY_WII_BITS = 3;
    ITF_COMPILE_TIME_ASSERT(invalid_bit_count_for_ObjectId_wii_bitfield,(RESERVED_BITS + USER_BITS + YEAR_BITS + MONTH_WII_BITS + DAY_WII_BITS + HOUR_BITS + MINUTE_BITS + SECOND_BITS + SESSION_BITS) == 64);
	ITF_COMPILE_TIME_ASSERT(invalid_bit_count_for_ObjectId_details_H_bitfield,(RESERVED_BITS + USER_BITS + YEAR_BITS + MONTH_WII_BITS) == 32);
	ITF_COMPILE_TIME_ASSERT(invalid_bit_count_for_ObjectId_details_L_bitfield,(DAY_WII_BITS + HOUR_BITS + MINUTE_BITS + SECOND_BITS + SESSION_BITS) == 32);
	struct details
	{
		DECL_BF(    u32 m_month     : MONTH_WII_BITS;,
			DECL_BF(    u32 m_year      : YEAR_BITS;,
			DECL_BF(    u32 m_user      : USER_BITS;,
			DECL_BF(    u32 m_reserved  : RESERVED_BITS;,
			))))
		DECL_BF(    u32 m_session   : SESSION_BITS;,
			DECL_BF(    u32 m_second    : SECOND_BITS;,
			DECL_BF(    u32 m_minute    : MINUTE_BITS;,
			DECL_BF(    u32 m_hour      : HOUR_BITS;,
			DECL_BF(    u32 m_day       : DAY_WII_BITS;,
			)))))
	};

	struct lowhigh
	{
		u32	m_high;
		u32	m_low;
	};
#else // ITF_WII
    struct details
    {
        DECL_BF(    u64 m_session   : SESSION_BITS;,
            DECL_BF(    u64 m_second    : SECOND_BITS;,
            DECL_BF(    u64 m_minute    : MINUTE_BITS;,
            DECL_BF(    u64 m_hour      : HOUR_BITS;,
            DECL_BF(    u64 m_day       : DAY_BITS;,
            DECL_BF(    u64 m_month     : MONTH_BITS;,
            DECL_BF(    u64 m_year      : YEAR_BITS;,
            DECL_BF(    u64 m_user      : USER_BITS;,
            DECL_BF(    u64 m_reserved  : RESERVED_BITS;,
            )))))))))
    };

    struct lowhigh
    {
        DECL_BF( u64 m_low : 32;,
            u64 m_high : 32;)
    };
#endif // ITF_WII

    union
    {
        details m_details;
        lowhigh m_lowhigh;
        u64 m_id;
    };

#ifdef ITF_MSC_VER
    #pragma warning(pop)
#endif

    ITF_INLINE ObjectId() : m_id(U64_INVALID)                       { }
    ITF_INLINE bool operator == (const ObjectId & _objectId)        { return m_id == _objectId.m_id; }
    ITF_INLINE bool operator != (const ObjectId& _objectId)         { return !operator==(_objectId); }
    ITF_INLINE bbool isValid() const                                { return m_id != U64_INVALID; }
    ITF_INLINE void invalidate()                                    { m_id = U64_INVALID; }
    ITF_INLINE operator u64 () const                                { return m_id; }
    ITF_INLINE bool operator<(const ObjectId& _rhs) const           { return m_id < _rhs.m_id; }

    void toString(char* _str, size_t _size = 256) const;
    void set(const ObjectId & src)
    {
        m_id = src.m_id;
    }
};

struct Time
{
#ifdef ITF_WII
    DECL_BF(    u32 m_year      : 11;, 
        DECL_BF(    u32 m_month     : 3;,
        DECL_BF(    u32 m_day       : 4;,
        DECL_BF(    u32 m_hour      : 4;,
        DECL_BF(    u32 m_minute    : 5;,
        DECL_BF(    u32 m_second    : 5;,
        ))))))

                    u32 m_reserved;
#else // ITF_WII
    DECL_BF(    u64 m_reserved  : 27;,
        DECL_BF(    u32 m_year      : 11;, 
        DECL_BF(    u32 m_month     : 4;,
        DECL_BF(    u32 m_day       : 5;,
        DECL_BF(    u32 m_hour      : 5;,
        DECL_BF(    u32 m_minute    : 6;,
        DECL_BF(    u32 m_second    : 6;,
        )))))))
#endif // ITF_WII

        //m_year:  0000-2048
        //m_month: 01-12
};

#if defined (ITF_PS3) || defined (ITF_VITA) || defined (ITF_PS5)

#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define TLS __thread
//TODO PS3
///TMP we will real value from Sony SDK 
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  260 /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

#elif defined(ITF_IPAD)	//TODO_iPad define TLS
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#elif defined(ITF_CTR)	//TODO_CTR define TLS
#define TLS             //CASA::TODO:: c koi ca
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  8   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#elif defined(ITF_WII)
//#define _MAX_PATH   1024 /* max. length of full pathname */ -> already define by CW
#define _MAX_DRIVE  8   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#elif defined(ITF_NINTENDO)
// $GB: these numbers could be come from SDK, but not found it.
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  8   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */
#else
#endif //ITF_PS3
} //namespace ITF

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <list>
#include <stack>
#include <string>

#include "core/memory/STDAllocatorOnITFMemory.h"

namespace ITF
{
    #define ObjectIdList SafeArray<ObjectId>
    #define ObjectRefList SafeArray<ObjectRef>
    #define ActorRefList SafeArray<ActorRef>
    #define ActorList SafeArray<Actor*>

    /////////////////////////////////////////////////////////////////////////////
    // Declaration of container types mapped on std ones
    /////////////////////////////////////////////////////////////////////////////
    template <typename Key>
    using ITF_SET = std::set< Key, std::less<Key>, STDAllocatorOnITFMemory<Key, MemoryId::mId_Map >>;

    template <typename T>
    using ITF_DEQUE = std::deque<T, STDAllocatorOnITFMemory<T, MemoryId::mId_Vector>>;

    template <typename T>
    using ITF_QUEUE = std::queue<T, ITF_DEQUE<T>>;

    template <typename T>
    using ITF_STACK = std::stack<T, ITF_DEQUE<T>>;

    using ITF_STDSTRING = std::basic_string<char, std::char_traits<char>, STDAllocatorOnITFMemory<char, MemoryId::mId_String8>>;

    // the following codes are used to get info on a type
    template<typename T>
    struct TypeInfo
    {
        enum
        {
            isPtr = 0,
            isBaseType = 0
        };
    };

    // specialized for pointer
    template<typename T>
    struct TypeInfo<T *>
    {
        enum
        {
            isPtr = 1,
            isBaseType = 1
        };
        typedef T ValueType;
    };
    
    // we put here all the C++ (or C99) legacy basic types,
    // any other custom base type can be converted in one of them
    template<> struct TypeInfo< bool >              { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< char >              { enum { isPtr = 0, isBaseType = 1 }; }; // "char" is not "signed char" or "unsigned char"
    template<> struct TypeInfo< unsigned char >     { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< signed char >       { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< unsigned short >    { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< signed short >      { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< unsigned int >      { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< signed int >        { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< unsigned long >     { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< signed long >       { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< unsigned long long >{ enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< signed long long >  { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< float >             { enum { isPtr = 0, isBaseType = 1 }; };
    template<> struct TypeInfo< double >            { enum { isPtr = 0, isBaseType = 1 }; };

    // define ParamType preffered flavor
    template <class T, int IsBaseType>
    struct ParamType
    {
        typedef const T & Input;
        typedef const T & Return;
    };

    template <class T>
    struct ParamType<T, 1>
    {
        typedef T Input;
        typedef T Return;
    };

} //namespace ITF

#if defined(ITF_CTR)
    #define STD_VECTOR vector
    #define STD_LIST list
    #define STD_MAP map
    #define STD_SET set
    #define STD_STRING string
    #define STD_LESS less
    #define STD_PAIR pair

    #define STD_VECTOR_QUALIFIED std::vector
    #define STD_LIST_QUALIFIED std::list
    #define STD_MAP_QUALIFIED std::map
    #define STD_SET_QUALIFIED std::set
    #define STD_STRING_QUALIFIED std::string
    #define STD_LESS_QUALIFIED std::less
    #define STD_PAIR_QUALIFIED std::pair
#else
    #define STD_VECTOR std::vector
    #define STD_MAP std::map
    #define STD_SET std::set
    #define STD_STRING std::string
    #define STD_LESS std::less
    #define STD_PAIR std::pair
    #define STD_LIST std::list

    #define STD_VECTOR_QUALIFIED std::vector
    #define STD_LIST_QUALIFIED std::list    
    #define STD_MAP_QUALIFIED std::map
    #define STD_SET_QUALIFIED std::set
    #define STD_STRING_QUALIFIED std::string
    #define STD_LESS_QUALIFIED std::less
    #define STD_PAIR_QUALIFIED std::pair

#endif

#ifdef ITF_IPAD
    #define ITF_CUSTOM_MAP      ITF::Map
    #define ITF_CUSTOM_VECTOR   ITF::vector
#else
    #define ITF_CUSTOM_MAP      ITF::Map
    #define ITF_CUSTOM_VECTOR   ITF::vector
//    #define ITF_CUSTOM_MAP      _STLP_STD_NAME::map         // customized STL port version
 //   #define ITF_CUSTOM_VECTOR   _STLP_STD_NAME::vector      // customized STL port version
#endif

#ifdef ITF_GCC
    #undef _PDFT
	typedef unsigned int UINT_PTR, *PUINT_PTR;
	#define _FARQ
	#define _PDFT	long
	#define _SIZT	size_t
#endif //ITF_GCC

#if !defined(ITF_MICROSOFT)
#define _strcmpi strcasecmp
#define _strnicmp strncasecmp
#endif  //ITF_PS3


#ifdef ITF_IPAD
	#define _strcmpi	strcasecmp
	#define sscanf_s	sscanf
	#define memcpy_s	memcpy
	#define NOT_IMPLEMENTED_ON_GCM
	#define NOT_IMPLEMENTED_ON_PS3
#endif //ITF_IPAD

#ifdef ITF_CTR
    #define _TRUNCATE ((size_t)-1)
    typedef unsigned int UINT_PTR, *PUINT_PTR;
    #define _FARQ
    #define _PDFT	long
    #define _SIZT	size_t

    #define _strcmpi	strcasecmp
    #define _strnicmp	strncasecmp
    #define sscanf_s	sscanf
    #define memcpy_s	memcpy
    #define _strcmpi   strcasecmp
    #define towlower(a) a  //CASA::TODO

#endif

    void VFormatTruncate(char* buff, unsigned int size, const char* formatstring, va_list argList);
    void FormatTruncate(char* buff,unsigned int size,const char* formatstring, ...);

#ifdef ITF_VITA
#define _TRUNCATE ((size_t)-1)
#endif //ITF_VITA

#ifdef ITF_WII
	#define _TRUNCATE ((size_t)-1)
    typedef unsigned int UINT_PTR, *PUINT_PTR;
    #define _FARQ
    #define _PDFT	long
    //typedef unsigned int	size_t;
    	#define _SIZT	size_t

    #define _strcmpi	strcasecmp
    #define sscanf_s	sscanf
    #define memcpy_s	memcpy
    #define _strnicmp	strncasecmp

    #ifndef NULL
    #ifdef __cplusplus
    #define NULL    0
    #else
    #define NULL    ((void *)0)
    #endif
    #endif
    
	#define NOT_IMPLEMENTED_ON_GCM
	#define NOT_IMPLEMENTED_ON_PS3
#endif

#if defined (ITF_X360)
    #define ITF_SUPPORT_FETCH 1 
    #define ITF_CACHELINE 128
	#define ITF_CACHEFETCH(offset,base)  __dcbt(offset,base);
	#define ITF_CACHECLEAR(offset,base)  __dcbz128(offset,base)
    

    ITF_INLINE void* ITF_MemcpyWriteCombined(void* _dst,const void* _src,size_t _size)
    {
        return XMemCpyStreaming_WriteCombined(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_Memcpy(void* _dst,const void* _src,size_t _size)
    {
        return XMemCpy(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_MemSet(void* _dst,int _val,size_t _count)
    {
        return XMemSet(_dst,_val,_count);
    }

#elif defined (ITF_PS3)
    #define ITF_SUPPORT_FETCH 1
    #define ITF_CACHELINE 128
#ifdef __GCC__
    #define ITF_CACHEFETCH(offset, base)  __builtin_prefetch(((u8*)(base))+u32(offset), 0)
#else // __SNC__
    #define ITF_CACHEFETCH(offset, base)  __dcbt(((u8*)(base))+u32(offset))
#endif 
	#define ITF_CACHECLEAR(offset,base)  __dcbz(((char*)base) + offset)


    ITF_INLINE void* ITF_MemcpyWriteCombined(void* _pDest,const void* _pSrc,size_t _size)
    {
        return memcpy(_pDest,_pSrc,_size);
    }

    ITF_INLINE void * ITF_Memcpy(void *_pDest, const void *_pSrc, size_t _size)
    {
        return memcpy(_pDest, _pSrc, _size);
    }

    ITF_INLINE void * ITF_MemSet(void *_pMem, int _set, size_t _size)
    {
        return memset(_pMem, _set, _size);
    }

#elif defined (ITF_VITA)

    #define ITF_CACHEFETCH(offset,base)  {}
	#define ITF_CACHECLEAR(offset,base)  

	ITF_INLINE void* ITF_MemcpyWriteCombined(void* _pDest,const void* _pSrc,size_t _size)
	{
		return memcpy(_pDest,_pSrc,_size);
	}

	ITF_INLINE void ITF_Memcpy(void *_pDest, const void *_pSrc, size_t _size)
	{
		memcpy(_pDest, _pSrc, _size);
	}

	ITF_INLINE void ITF_MemSet(void *_pMem, int _set, size_t _size)
	{
		memset(_pMem, _set, _size);
	}
#elif defined (ITF_WII)
    #define ITF_SUPPORT_FETCH 1 
    #define ITF_CACHELINE 32
    #define ITF_CACHEFETCH(offset,base)  __dcbt((void*)base,offset);
	#define ITF_CACHECLEAR(offset,base)  __dcbz((void*)base,offset)

	extern void *WII_memcpy(void * __restrict dst, const void * __restrict src, size_t n);
	extern void *WII_memset(void * dst, int val, unsigned long n);

    ITF_INLINE void* ITF_MemcpyWriteCombined(void* __restrict _dst, const void* __restrict _src, size_t _size)
    {
        return WII_memcpy(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_Memcpy(void* __restrict _dst,const void* __restrict _src,size_t _size)
    {
        return WII_memcpy(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_MemSet(void* _dst,int _val,size_t _count)
    {
        return WII_memset(_dst,_val,_count);
    }
#else
    
    #define ITF_CACHEFETCH(offset,base)  do {} while(0)
	#define ITF_CACHECLEAR(offset,base)

    ITF_INLINE void* ITF_MemcpyWriteCombined(void* _dst,const void* _src,size_t _size)
    {
        return memcpy(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_Memcpy(void* _dst,const void* _src,size_t _size)
    {
        return memcpy(_dst,_src,_size);
    }

    ITF_INLINE void* ITF_MemSet(void* _dst,int _val,size_t _count)
    {
        return memset(_dst,_val,_count);
    }
#endif 

#if !defined (ITF_FINAL) && !defined (ITF_CONSOLE)
#define MODE_HRESTEXTURE_SUPPORTED
#endif  //!defined (ITF_FINAL) && !defined (ITF_CONSOLE)


#ifndef _ITF_FIXEDARRAY_H_
#include "core/container/FixedArray.h"
#endif //_ITF_FIXEDARRAY_H_

#endif // _ITF_CORETYPES_H_

