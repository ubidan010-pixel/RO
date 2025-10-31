#ifndef __MTH_INT_WII_H__
#define __MTH_INT_WII_H__

namespace ITF
{

ITF_INLINE u32 u32_CountLeadingZero(register u32 _u32_Value)
{
    register u32 u32_result;
    asm 
    {
        cntlzw  u32_result, _u32_Value
    }
    return u32_result;
}

// Given an integer n, returns its power of two (ie the number p which respects 2^p <= n < 2^(p+1) )
// ex : 1 -> 0; 2 -> 1; 3 -> 1; 8 -> 3
ITF_INLINE i32 MTH_PowerOfTwo(u32 n)
{
    return 31 - (i32) u32_CountLeadingZero(n);
}

} // namespace ITF

#endif // __MTH_INT_WII_H__