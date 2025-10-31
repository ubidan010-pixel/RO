#ifndef ITF_BYTE_ENDIAN_H_
#define ITF_BYTE_ENDIAN_H_

#pragma once

#include "core/types.h"

namespace ITF
{

class Endian
{
private:
    static bbool     m_isLittleEndian;
    static Endian    m_instance;            // used so that the constructor initializes m_isLittleEndian

public:

    static void findEndian()
    {
        // Do a simple test to determine the hardware's endianness
        i32 endianInt = 1;
        i8* endianTest = (i8*)&endianInt;

        if (endianTest[0] == 1)    // Least significant byte in lowest address
            m_isLittleEndian = btrue;
        else
            m_isLittleEndian = bfalse;
    }

    static ITF_INLINE bbool isLittleEndian()
    {
        return m_isLittleEndian;
    }

    Endian()
    {
        findEndian();
    }


    template <typename  T> static void swapBigEndian(T value, u8* pDest)
    {
        u8* v = (u8*)&value;
        for (u32 i=0;i<sizeof(T);++i)
        {
            pDest[i] = v[sizeof(T)-i-1];
        }
    }     

    template <typename  T> static void restoreLittleEndian(T& dest,u8* v)
    {
        u8* pDest = (u8*)&dest;
        for (u32 i=0;i<sizeof(T);++i)
        {
            pDest[i] = v[sizeof(T)-i-1];
        }
    }     

  
    template <typename  T> static void hideEndian(T value, u8* pDest)
    {
        u8* v = (u8*)&value;
        if (m_isLittleEndian)
        {
            for (u32 i=0;i<sizeof(T);++i)
            {
                pDest[i] = v[i];
            }
        }
        else
        {
            for (u32 i=0;i<sizeof(T);++i)
            {
                pDest[i] = v[sizeof(T)-i-1];
            }
        }
    }        
    

    template <typename  T> static void restoreEndian(u8* pValue, T& dest)
    {
        dest = 0;
        u8* pDest = (u8*)&dest;
        if (m_isLittleEndian)
        {
            for (u32 i=0;i<sizeof(T);++i)
            {
                pDest[i] = pValue[i];
            }
        }
        else
        {
            for (u32 i=0;i<sizeof(T);++i)
            {
                pDest[i] = pValue[sizeof(T)-i-1];
            }
        }
    }
};


} // namespace ITF



#endif // _ITF_ENDIAN_H_
