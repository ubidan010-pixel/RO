#ifndef MEMMNG_PRIVATE_PS3_H_
#define MEMMNG_PRIVATE_PS3_H_

#include "Core/system/Synchronize.h"
#include "MemMngConfig_PS3.h"

//////////////////////////////////////////////////////////////////////////
// File with some specific define, inline functions, etc. Used internally.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Critical Section and other parallel stuff
//////////////////////////////////////////////////////////////////////////

#define MEMMNG_AUTO_COUNT_LOCK() ScopeLockMutex AL_csLock_(count.lock)
#define MEMMNG_AUTO_COUNT_UNLOCK() ScopeUnLockMutex AL_csUnLock_(count.lock)

//--------------------------------------------------------------------------------------------------
// undef malloc and co so we can use it internally
//--------------------------------------------------------------------------------------------------
#undef malloc
#undef free
#undef calloc
#undef realloc

// Platform specific part of the file. Set after the CS because it could need it
#include "MemMngPrivatePlatform_PS3.h"

//////////////////////////////////////////////////////////////////////////
// Count of memory 
//////////////////////////////////////////////////////////////////////////
#if MEMMNG_MEM_COUNT>=1
	#define DECL_MEM_USAGE(nTrueSize) unsigned int nLastMemUsage= (nTrueSize)
	#define GET_MEM_USAGE(nTrueSize) nLastMemUsage= (nTrueSize)
	#define LAST_MEM_USAGE				nLastMemUsage
	#if MEMMNG_MEM_COUNT>=1
		#define COUNT_ALLOC(var)	MemMngTools::MemCountAtomicAdd(g_MemUsageStat->var, nLastMemUsage)
		#define COUNT_FREE(var)		MemMngTools::MemCountAtomicSub(g_MemUsageStat->var, nLastMemUsage)
	#endif
#else
	#define DECL_MEM_USAGE(nTrueSize)					((void)0)
	#define GET_MEM_USAGE(nTrueSize)					((void)0)
	#define COUNT_ALLOC(var)							((void)0)
	#define COUNT_FREE(var)								((void)0)
	#define LAST_MEM_USAGE								(0)
#endif

//////////////////////////////////////////////////////////////////////////
// Fill memory
//////////////////////////////////////////////////////////////////////////
#if MEMMNG_FILL_MEMORY_AT_ALLOC==1
	#define FILL_ALLOC(pBlock, nSize)	memset((pBlock), ALLOCMEM_PATTERN, (nSize))
#else
	#define FILL_ALLOC(pBlock, nSize)	((void)0)
#endif

#if MEMMNG_FILL_MEMORY_AT_FREE==1
	#define FILL_FREE(pBlock, nSize)	memset((pBlock), FREEMEM_PATTERN, (nSize))
#else
	#define FILL_FREE(pBlock, nSize)	((void)0)
#endif

namespace ITF
{

//////////////////////////////////////////////////////////////////////////
// Mem usage globals
//////////////////////////////////////////////////////////////////////////

extern MemMng::MemCounts * g_MemUsageStat;

//////////////////////////////////////////////////////////////////////////
//				Debug functions
//////////////////////////////////////////////////////////////////////////

namespace MemMngDebug
{
	//////////////////////////////////////////////////////////////////////////
	// Out of mem
	//////////////////////////////////////////////////////////////////////////

	#if MEMNG_TEST_OUT_OF_MEM==1
		void OutOfMem(u32 nSize);
		#define MEMNG_OUT_OF_MEM(pAdrs, nSize)		\
		do															\
		{															\
		if (pAdrs==NULL) MemMngDebug::OutOfMem((nSize));	\
		} while(0)
	#else
		#define MEMNG_OUT_OF_MEM(pAdrs, nSize)		((void)0)
	#endif

	//////////////////////////////////////////////////////////////////////////
	// Memory info from platform
	//////////////////////////////////////////////////////////////////////////
	unsigned int GetCurrentSystemMemoryAvailable(); // get the currently free physical memory

	//////////////////////////////////////////////////////////////////////////
	// Memory tracer
	//////////////////////////////////////////////////////////////////////////
	#if MEMMNG_MEMORY_TRACER==1
		void InitMemoryTracer();
		int	GetCallStack(void *addrContainer[], int containerSize);
	#else
		inline void InitMemoryTracer() {};
		inline int	GetCallStack(void *addrContainer[], int containerSize)
		{
			for(int i= 0; i <containerSize; ++i)
			{
				addrContainer[i]= NULL;
			}
			return 0;
		};	
	#endif

	//////////////////////////////////////////////////////////////////////////
	// Quarantine
	//////////////////////////////////////////////////////////////////////////
	#if MEMMNG_QUARANTINE==1
		void InitQuarantine();
		void * AllocInQuarantine(u32 iSize, u32 uAlign);
		unsigned int SizeInQuarantine(void * pDataInQuarantine);
		void FreeInQuarantine(void * pDataInQuarantine);
		bool FindInQuarantine(void * pDataInQuarantine);
	#else
		inline void InitQuarantine() {}
		inline void * AllocInQuarantine(u32 iSize, u32 uAlign) { return NULL; }
		inline unsigned int SizeInQuarantine(void * pDataInQuarantine) { return 0; }
		inline void FreeInQuarantine(void * pDataInQuarantine) {}
		inline bool FindInQuarantine(void * pDataInQuarantine) { return false; }
	#endif
	
	//////////////////////////////////////////////////////////////////////////
	// Assert/trace
	//////////////////////////////////////////////////////////////////////////
	#if MEMMNG_ENABLE_ASSERTS==1
		void AssertTrace(const char * msg, const char * expr);
		#define	MEMMNG_ASSERT(expr)												\
			do																	\
			{																	\
				if (!(expr))													\
				{																\
					MemMngDebug::AssertTrace("MEMMNG assertion failed: ", #expr);\
					ITF_DEBUG_BREAK();											\
				}																\
			} while(0)
		
	#else
        #define	MEMMNG_ASSERT(expr) do {} while(0)
	#endif
	
} // namespace MemMngDebug


//////////////////////////////////////////////////////////////////////////
// tools function
//////////////////////////////////////////////////////////////////////////
namespace MemMngTools
{
	// GetAlignFromAdrs returns the delta to add to an address to be aligned on a boundary
	// uAlign MUST BE POWER OF 2
	inline unsigned int GetAlignFromAdrs(void *pAdrs, unsigned int uAlign)
	{
		if (!uAlign) return 0;

		unsigned int v = (unsigned int)pAdrs;
		int delta = v&(uAlign-1);
		int ret = int(uAlign)-delta;
		if (ret==int(uAlign)) ret = 0;
		MEMMNG_ASSERT(ret>=0);
		return (unsigned int)ret;
	}
}

} // namespace ITF

#endif // MEMMNG_PRIVATE_PS3_H_
