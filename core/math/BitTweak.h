#ifndef ITF_CORE_BITTWEAK_H_
#define ITF_CORE_BITTWEAK_H_

#pragma once

#include "core/Types.h"

namespace ITF
{

namespace BitTweak
{
#if defined(ITF_MICROSOFT)

    inline u32 countLeadingZero(u32 _value)
    {
        u32 bsr;
        _BitScanReverse((unsigned long*)&bsr, (unsigned long)_value);
        return (_value == 0) ? 32 : 31 - bsr;
    }

    inline u32 countTrailingZero(u32 _value)
    {
        u32 bsf;
        _BitScanForward((unsigned long*)&bsf, _value);
        return (_value == 0) ? 32 : 31 - bsf;
    }

    inline u32 GetLog2(u32 value)
    {
        return 31u - countLeadingZero(value);
    }

#elif defined(ITF_CLANG) && defined(ITF_AMD64_CPU)

    inline u32 countLeadingZero(u32 _value)
    {
        return (_value == 0) ? 32 : __builtin_ia32_lzcnt_u32(_value);
    }

    inline u32 countTrailingZero(u32 _value)
    {
        return (_value == 0) ? 32 : __tzcnt_u32(_value);
    }

    inline u32 GetLog2(u32 value)
    {
        return 31u - countLeadingZero(value);
    }

#elif defined(ITF_CLANG) && defined(ITF_ARM64_CPU)

    inline u32 countLeadingZero(u32 _value)
    {
        return (_value == 0) ? 32 : __builtin_clz(_value);
    }

    inline u32 countTrailingZero(u32 _value)
    {
        return (_value == 0) ? 32 : __builtin_ctz(_value);
    }

    inline u32 GetLog2(u32 value)
    {
        return 31u - countLeadingZero(value);
    }

#else

    inline constexpr u32 countTrailingZero(u32 v)
    {
        u32 c = 32; // c will be the number of zero bits on the right
        v &= -i32(v);
        if (v) c--;
        if (v & 0x0000FFFF) c -= 16;
        if (v & 0x00FF00FF) c -= 8;
        if (v & 0x0F0F0F0F) c -= 4;
        if (v & 0x33333333) c -= 2;
        if (v & 0x55555555) c -= 1;
        return c;
    }

    inline u32 GetLog2(u32 nValue) // the pow2 "floor"
    {
        u32 r = 0;
        u32 shift;

        shift = ((nValue & 0xFFFF0000) != 0) << 4; nValue >>= shift; r |= shift;
        shift = ((nValue & 0xFF00) != 0) << 3; nValue >>= shift; r |= shift;
        shift = ((nValue & 0xF0) != 0) << 2; nValue >>= shift; r |= shift;
        shift = ((nValue & 0xC) != 0) << 1; nValue >>= shift; r |= shift;
        shift = ((nValue & 0x2) != 0) << 0; nValue >>= shift; r |= shift;

        return  r;
    }

    inline u32 countLeadingZero(u32 _value)
    {
        if (_value == 0)
            return 32;
        if ((_value & (1 << 31)) != 0)
            return 0;
        u32 log2 = GetLog2(_value);
        return 31 - log2;
    }
#endif

// Return the highest pow2 value below or equal to _value such as (_value % getPow2Alignment(_value)) == 0
// This is the 'natural alignment' for _value
inline constexpr u32 GetPow2Alignment(u32 _value)
{
    return _value == 0 ? 1u : (1u << countTrailingZero(_value));
}

// Given an integer n, returns its power of two (i.e. the number p which respects 2^p <= n < 2^(p+1) )
// powerOfTwo(0) returns -1. 
inline i32 powerOfTwo(u32 value)
{
	return 31 - (i32) countLeadingZero(value);
}


// put here tools fcts to deal with bits
// cf. http://graphics.stanford.edu/~seander/bithacks.html 

// IsPow2(0) = true, not fully correct as 0 is reported as a pow2 but is not
inline bool IsPow2(u32 nValue)
{
    return (nValue&(nValue-1u))==0;
}

inline bool IsPow2(u64 nValue)
{
    return (nValue&(nValue-1ull))==0;
}


// Compile-time version
template<u64 x> struct Pow2
{
    enum { value = (x & (x - 1ull)) == 0 };
};

template<> struct Pow2<0>
{
    enum { value = false };
};

// Round value to ABOVE alignment
// Alignment MUST BE POWER OF 2 and not 0 (align on 0 means nothing)
inline u32 GetAboveAlign(u32 _value, u32 _align)
{
    ITF_ASSERT(IsPow2(_align) && _align>0);
    return (_value + (_align - 1u)) & ~(_align - 1u);
}

inline u64 GetAboveAlign(u64 _value, u64 _align)
{
    ITF_ASSERT(IsPow2(_align) && _align>0);
    return (_value + (_align - 1ull)) & ~(_align - 1ull);
}

// Round value to BELOW alignment
// Alignment MUST BE POWER OF 2 and not 0 (align on 0 means nothing)
inline u32 GetBelowAlign(u32 _value, u32 _align)
{
    ITF_ASSERT(BitTweak::IsPow2(_align) && _align > 0);
    return (_value & ~(_align - 1u));
}

inline u64 GetBelowAlign(u64 _value, u64 _align)
{
    ITF_ASSERT(BitTweak::IsPow2(_align) && _align > 0);
    return (_value & ~(_align - 1ull));
}

inline bbool IsAligned(u32 _value, u32 _align)
{
    ITF_ASSERT(BitTweak::IsPow2(_align) && _align > 0);
    return (_value & (_align - 1u)) == 0;
}

inline bbool IsAligned(u64 _value, u64 _align)
{
    ITF_ASSERT(BitTweak::IsPow2(_align) && _align > 0);
    return (_value & (_align - 1ull)) == 0;
}

// Pointer versions
template< class T > inline T* GetAboveAlign(T* _value, ux align)
{
    return reinterpret_cast<T*>(GetAboveAlign(uintptr_t(_value), uintptr_t(align)));
}
template< class T > inline T* GetBelowAlign(T* _value, ux align)
{
    return reinterpret_cast<T*>(GetBelowAlign(uintptr_t(_value), uintptr_t(align)));
}
template< class T > inline bbool IsAligned(T* _value, ux align)
{
    return IsAligned(uintptr_t(_value), uintptr_t(align));
}

// Compile-time version
// ex: AboveAlign<sizeof(ndVec3), 16>::value
template<u64 x, u64 align> struct AboveAlign
{
    ITF_COMPILE_TIME_ASSERT(Alignment_Not_A_Pow2, Pow2<align>::value);
    enum { value = (x + (align - 1ull)) & ~(align - 1ull) };
};

template<u32 v>
struct CeilPow2
{
    enum
    {
        v0 = v - 1,
        v1 = v0 | ( v0 >> 1 ),
        v2 = v1 | ( v1 >> 2 ),
        v3 = v2 | ( v2 >> 4 ),
        v4 = v3 | ( v3 >> 8 ),
        v5 = v4 | ( v4 >> 16 ),
        value = v5 + 1
    };

    operator u32 () const { return value; }
};

// Give the lowest pow2 such than GetCeilPow2(n)>=n
// ex. GetCeilPow2(3) = 4, GetCeilPow2(8) = 8, etc.
// Another way to write it -> GetCeilPow2(v) : 1 << (log2(v - 1) + 1)
// Note that GetCeilPow2(0) = 0 (and 0 not a pow2, but who care ^^)
inline u32 GetCeilPow2(u32 v)
{
	// Test this optimized version if you need it
	// return 1<<(1+powerOfTwo(v-1))
    v--;
    v|= v >> 1;
    v|= v >> 2;
    v|= v >> 4;
    v|= v >> 8;
    v|= v >> 16;
    v++;
    return v;
}

// Compile time version
template< u32 v >
struct Log2
{
    enum
    {
        r = 0,
        shift0 = ((v & 0xFFFF0000 ) != 0) << 4,  v0 = v >> shift0, r0 = r | shift0,
        shift1 = ((v0 & 0xFF00    ) != 0) << 3,  v1 = v0 >> shift1, r1 = r0 | shift1,
        shift2 = ((v1 & 0xF0      ) != 0) << 2,  v2 = v1 >> shift2, r2 = r1 | shift2,
        shift3 = ((v2 & 0xC       ) != 0) << 1,  v3 = v2 >> shift3, r3 = r2 | shift3,
        shift4 = ((v3 & 0x2       ) != 0) << 0,  v4 = v3 >> shift4, r4 = r3 | shift4,
        value = r4
    };

    operator u32 () const { return value; }
};

// Log2<0>::value is not defined and won't compile
template<> struct Log2<0>
{
};

inline u32 GetNbBitSet(u32 v)
{
    v = v - ((v >> 1) & 0x55555555);                        // reuse input as temporary
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);         // temp
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;  // count
}


//////////////////////////////////////////////////////////////////////////
// The two functions below are used to convert between number of bits and
// number of byte (one version is obvious as it's just a multiply by 8)

// calculation of number of bits
inline u32 Bits2Bytes(u32 n)
{
    return     (n & 0x7u)?((n>>3u) + 1U):(n>>3u);
}

inline u32 Bytes2Bits(u32 n)
{
    return n << 3u;
}

//////////////////////////////////////////////////////////////////////////
// The functions below are used to get values stacked in bits
// (platform independent bitfield)

template<u32 firstBit, u32 nbBit, class T>
T GetBitRange(T val)
{
    return (val >> firstBit) & ((T(1)<<nbBit)-T(1));
}

template<u32 firstBit, u32 nbBit, class T>
T BuildBitRange(T val)
{
    return (val & (((T(1))<<nbBit)-T(1))) << firstBit;
}

template<u32 firstBit, u32 nbBit, class T>
T SetBitRange(T _varToModify, T _val)
{
    T masked = _varToModify & (~BuildBitRange<firstBit, nbBit, T>(0));
    return masked | BuildBitRange<firstBit, nbBit, T>(_val);
}


template<class T>
T GetBitRange(T val, u32 firstBit, u32 nbBit)
{
    return (val >> firstBit) & ((T(1)<<nbBit)-T(1));
}

template<class T>
T BuildBitRange(T val, u32 firstBit, u32 nbBit)
{
    return (val & (((T(1))<<nbBit)-T(1))) << firstBit;
}

template<class T>
T SetBitRange(T _varToModify, T _val, u32 firstBit, u32 nbBit)
{
    T masked = _varToModify & (~BuildBitRange<T>(0, firstBit, nbBit));
    return masked | BuildBitRange<T>(_val, firstBit, nbBit);
}


//////////////////////////////////////////////////////////////////////////
// 32bit <-> 64 bit conversion fcts (little and big endian compliant)
// Faster on hardware with 64 bits registers (specially ppc-like)

inline i64 Merge32Bit(i32 lowVal, i32 * highVal)
{
    i64 value= i64(lowVal);
    if (highVal)
    {
        value += (i64(*highVal))<<32LL;
    }
    return value;
}

inline i64 Merge32Bit(i32 lowVal, i32 highVal)
{
    return i64(lowVal) + ((i64(highVal))<<32LL);
}

inline u64 Merge32Bit(u32 lowVal, u32 highVal)
{
    return u64(lowVal) + ((u64(highVal))<<32ULL);
}

inline i32 Split64Bit(i64 value, i32 * highValue)
{
    if (highValue)
    {
        (*highValue) = i32(value>>32LL);
    }

    return i32(value & 0xFFFFFFFFLL);
}

inline u32 Split64Bit(u64 value, u32 * highValue)
{
    if (highValue)
    {
        (*highValue) = u32(value>>32ULL);
    }

    return u32(value & 0xFFFFFFFFULL);
}

inline void Split64Bit(u64 value, u32 * lowValue, u32 * highValue)
{
    ITF_ASSERT(lowValue!=NULL);
    (*lowValue) = Split64Bit(value, highValue);
}

//////////////////////////////////////////////////////////////////////////
// MaskBits< val >::Value
//
// Give smallest mask that includes 'val'
//
// For example, MaskBits< 8 >::Value gives 15 (4 first bits set)
// or MaskBits< 6 >::Value gives 7 (3 first bits set)
//
// MaskBits< 7 >::Value also gives 7
//
template< u32 bits, u32 shiftedBits >
class MaskBitsInternal;

template< u32 bits >
class MaskBitsInternal< bits, 0 >
{
public:
    enum
    {
        Value = bits
    };
};

template< u32 bits, u32 shiftedBits >
class MaskBitsInternal
{
    enum
    {
        nextBits        = bits | shiftedBits,
        nextShiftedBits = shiftedBits >> 1u
    };

public:
    enum
    {
        Value = MaskBitsInternal< nextBits, nextShiftedBits >::Value
    };
};

template< u32 bits >
class MaskBits
{
    enum
    {
        nextShiftedBits = bits >> 1u
    };

public:
    enum
    {
        Value = MaskBitsInternal< bits, nextShiftedBits >::Value
    };
};


//////////////////////////////////////////////////////////////////////////
// BitCount< val >::Value
//
// Counts how many bits are set in 'val'
//

template< u32 bits >
class BitCount;

template< >
class BitCount< 0 >
{
public:
    enum
    {
        Value = 0
    };
};

template< u32 bits >
class BitCount
{
    enum
    {
        shiftedBits = bits >> 1
    };

public:
    enum
    {
        Value = ( bits & 1u ) + BitCount< shiftedBits >::Value
    };
};

} // namespace BitTweak

} // namespace ITF

#endif // #ifndef ITF_CORE_BITTWEAK_H_
