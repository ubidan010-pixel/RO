#include "precompiled_core.h"

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

// Some types validation.

// Obvious for int and other types:
static_assert(true == std::is_signed_v<int>);
static_assert(true == std::is_same_v<signed int, int>);
static_assert(false == std::is_same_v<unsigned int, int>);

// But for char, there is 3 types (C++ standart requirements) :
static_assert(false == std::is_same_v<char, unsigned char>);
static_assert(false == std::is_same_v<char, signed char>);

// The sign of the char type is implementation-defined, and is different on Nintendo Switch (1 and 2).
// Also the size of wchar_t is 4 bytes and not 2.
// Unfortunately, Intellisense is not setup to handle these differences.
#if (defined(ITF_OUNCE) || defined(ITF_NX)) && !defined(__INTELLISENSE__)
    // Note that according to the Nintendo documentation, it is NOT allowed to change the sign of the char type using the compiler command line option -fsigned-char.
    // see: https://developer.nintendo.com/html/online-docs/p67qwg2b/sdk/sdk-document/Pages/Page_1746296854.en.html
    static_assert(true == std::is_unsigned_v<char>);
    // wchar_t is 4 bytes so is more suitable to contains UTF32
    static_assert(4 == sizeof(wchar_t));
#else
    static_assert(true == std::is_signed_v<char>);
    // wchar_t is 2 bytes so is more suitable to contains UTF16 (Windows default unicode character)
    static_assert(2 == sizeof(wchar_t));
#endif

const ITF::ObjectRef ITF::ObjectRef::InvalidRef;


void VFormatTruncate(char* buff, unsigned int size, const char* formatstring, va_list args)
{
#ifdef ITF_WII
    _vsnprintf_s(buff, _TRUNCATE, formatstring, args);
#elif defined(ITF_CTR)
    vsnprintf(buff, _TRUNCATE, formatstring, args);//CASA::TODO::Check this function
#elif defined (ITF_VITA)
    vsnprintf(buff, _TRUNCATE, formatstring, args);//CASA::TODO::Check this function //TODO VITA
#elif defined (ITF_SONY) 
    vsnprintf(buff, size, formatstring, args);
#elif defined(ITF_NINTENDO)
    vsnprintf(buff, size, formatstring, args);
#else
    vsnprintf_s(buff, size, _TRUNCATE, formatstring, args);
#endif    
}

void FormatTruncate(char* buff, unsigned int size, const char* formatstring, ...)
{
    va_list args;
    va_start(args, formatstring);

    VFormatTruncate(buff, size, formatstring, args);

    va_end(args);
}

#ifdef ITF_WII

#ifdef DEBUG
#define CHECK_MEMCPY 1	// this is to find places that call memcpy when moving data ...
#endif

void *WII_memcpy(void * __restrict dst, const void * __restrict src, size_t n)
{

/*Assumptions:		990129 BLC  
 * 
 * - Miss aligned reads are supported (on a G3 miss aligned reads seem to 
 * preform better than the code for manually aligning the data). To kept the 
 * penalty to a minimum, I use miss-aligned reads but aligned writes.
 * 
 * - The processor supports floating point, and if the both operands can 
 * become double aligned it is much faster to use doubles to copy the data.
 *
 * - The 2.2 compiler is required for optimal performance since it will
 *   unroll the loops.
 * 
 */
#if CHECK_MEMCPY
	const size_t saved_n = n;
	int bad_check = 0;
#endif

	const   unsigned char    *p;
			unsigned char    *q;

	if (n < 3 *_MSL_LONG_COPY_MIN) 
	{
		/* short copy, word by word, unaligned */
		const unsigned long *p4 = (const unsigned long *)	src;
		unsigned long 		*q4 = (unsigned long *)			dst;
		const unsigned char *lastp  = ((unsigned char *)src) + n;
		const unsigned long *lastp4 = p4 + n/4;
		while(p4!=lastp4)
			*q4++ = *p4++;
		const unsigned char *p	= (unsigned char *) p4;
		unsigned char 		*q 	= (unsigned char *) q4;
		while(p!=lastp)
			*q++ = *p++;

	}
	else
	{
		/* doing a large copy */
		q = (unsigned char *) dst;
		p = ((const unsigned char *) src);

		unsigned char *q32aligned = (unsigned char *)OSRoundUp32B((u32)q);
		n -= q32aligned - dst;
		while(q!=q32aligned)
    		*q++ = *p++;

		/* dst is now 32 byte aligned */
		if ( (((unsigned long) p) & 0x07) != 0 )
		{
			/* src is not double aligned */

			//+addendum
			// dst is 32 byte aligned
			size_t n3 = n / 32;
			n -= 32 * n3;
			unsigned long *lq = (unsigned long *)(q-4);
			unsigned long *lp = (unsigned long *)(p-4);
			for (size_t i = 0; i < n3; i++) 
			{
				ITF_ASSERT((((unsigned long)lq) & 0x1f) == 0x1c);
				__dcbz(lq,4);
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
				*++lq = *++lp;
    			//n -= 32;
			}
			//-addendum

			size_t n2 = n / 4;
			for (size_t i = 0; i < n2; i++) 
			{
				*++lq = *++lp;
    			n = n - 4;
			}

			q = (unsigned char *)++lq;
			p = (unsigned char *)++lp;

		} 
		else 
		{
			/* dst and src are double aligned */

			//+addendum
			// dst is 32 byte aligned
			
			size_t n3 = n / 32;
			double *dq = (double *)(q-8);
			double *dp = (double *)(p-8);
			for (size_t i = 0; i < n3; i++) 
			{
			
				ITF_ASSERT((((unsigned long)dq) & 0x1f) == 0x18);
				ITF_ASSERT((((unsigned long)dp) & 0x07) == 0);
				__dcbz(dq,8);
				*++dq = *++dp;
				*++dq = *++dp;
				*++dq = *++dp;
				*++dq = *++dp;
    			n -= 32;
			}
			//-addendum

			size_t n2 = n / 8;
			for (size_t i = 0; i < n2; i++)
			{
				*++dq = *++dp;
    			n -= 8;
			}

			q = (unsigned char *)++dq;
			p = (unsigned char *)++dp;
			
		}
		p--;
		q--;
		for (size_t i = 0; i < n; i++) 
		//while(n--!=0)
		{
			*++q = *++p;
		}
	}
memcpy_end:

#if CHECK_MEMCPY
				
	// check result is ok
	p = (const unsigned char *)src-1;
	q = (unsigned char *)dst-1; 
	for(size_t i = 0; i<saved_n; ++i)
		if(! (*++p == *++q))
		{
			bad_check = 1;
			break;
		}
	
	if(bad_check)
	{
		OSReport("wrong memcpy !");
		*(int*) 0 = 0; // Crash here.
	}
#endif /* CHECK_MEMCPY */

	return dst;
}

/*--------------------------------------------------------------------------
                                       memset
----------------------------------------------------------------------------*/

// code taken & modified from 
// CW_RVL\PowerPC_EABI_Support\Runtime\Src\__mem.c
// adding use of dcbz halved cost
// note : turning the dead code elimination on is necessary to get 
//  the debug target version to run correctly inside interrupts early during boot 
// (otherwise  the compiler curiously generate a fp register save/restore for a 
// register  that is not used ...)
// additional note: starting with CW for Wii 1.2 turning dead code elimination on does not work anymore 
//  in that case forcing to optimization level 1 does the job

#define cps	((unsigned char *) src)
#define cpd	((unsigned char *) dst)
#define lps	((unsigned long *) src)
#define lpd	((unsigned long *) dst)
#define dps	((double *) src)
#define dpd	((double *) dst)
#define deref_auto_inc(p)	*++(p)

#define CHECK_MEMSET 0

#pragma opt_dead_code on

#if defined(_DEBUG) && (__MWERKS__ >= 0x4203)
#pragma optimization_level 1
#endif

inline void *memset_generic(void * dst, unsigned long v, unsigned long n)
{
#if CHECK_MEMSET
	unsigned long saved_n = n;
#endif

	unsigned long			i;
	void 					*saved_dst = dst;
	
	if ( n >= 2*32 )
	{
		/* write longs until the first 32 byte boundary has been crossed */
		unsigned long  d32aligned= OSRoundUp32B(dst);
		const unsigned int n4 = 1 + ((d32aligned - (unsigned long)dst) >> 2);
		i = n4;
		lpd = lpd-1;
			
		do
			deref_auto_inc(lpd) = v;
		while (--i);
	
		unsigned long byte_offset = d32aligned - (unsigned long)lpd ;
		lpd = (unsigned long *)d32aligned;
		
		unsigned long rotated_v = __rlwnm(v, byte_offset*8, 0, 31);
		
		n = n - ((n4-1)<<2) - byte_offset;
		int n32 = n >> 5;
		
		/* cache clear destination and write doubles as long as possible */
		i = n32;
		if (i)
		{
			dpd = dpd - 1;
			if(v)
			{
				unsigned long long llv = rotated_v;
				llv |= llv << 32;
				double dv = *(double *)&llv;

				do
				{
					ITF_ASSERT((((unsigned long)dpd) & 0x1f) == 0x18);
					__dcbz(dpd,8);
					deref_auto_inc(dpd) = dv;
					deref_auto_inc(dpd) = dv;
					deref_auto_inc(dpd) = dv;
					deref_auto_inc(dpd) = dv;
				}
				while (--i);

			} else
				do
				{
					ITF_ASSERT((((unsigned long)dpd) & 0x1f) == 0x18);
					__dcbz(dpd,8);
					dpd+=4;
				}
				while (--i);
			

			lpd = ((unsigned long *) (dpd + 1)) - 1;
		}	else
			 	lpd = lpd - 1;
				
		n &= 31;		
	}	else 
			lpd = lpd - 1;
		
		
	int n4 = n >> 2;
	i = n4;
	if(i) do
			deref_auto_inc(lpd) = v;
		while (--i);
		
	n &= (4-1);
	cpd = ((unsigned char *) (lpd + 1)) - 1;
	if(n) do
			deref_auto_inc(cpd) = v;
		while (--n);
	
#if CHECK_MEMSET
/*
not so simple anymore !
	for(int j = 0; j<saved_n; ++j)
		ITF_ASSERT(((unsigned char *)saved_dst)[j] == (unsigned char)v);
*/
	ITF_ASSERT((unsigned long)dst == (unsigned long)saved_dst + saved_n - 1);
#endif
		
	return dst;
}

#pragma opt_dead_code reset

void *WII_memset(void * dst, int val, unsigned long n)
{
	unsigned long	v = (unsigned char) val;

	if (v)
		v |= v << 24 | v << 16 | v <<  8;

	return memset_generic(dst, v, n);	
}



#endif // ITF_WII
