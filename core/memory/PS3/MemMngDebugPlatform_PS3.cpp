#include "precompiled_core.h"
#include <sys/memory.h>
#include "MemMng_PS3.h"
#include "MemMngPrivate_PS3.h"

namespace ITF
{

//////////////////////////////////////////////////////////////////////////
// ASSERT
//////////////////////////////////////////////////////////////////////////
#if MEMMNG_ENABLE_ASSERTS==1
namespace MemMngDebug
{
	void AssertTrace(const char * msg, const char * expr)
	{
		fprintf(stderr, msg);
		fprintf(stderr, expr);
		fprintf(stderr, "\n");
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// Memory info from platform
//////////////////////////////////////////////////////////////////////////
namespace MemMngDebug
{
	unsigned int GetCurrentSystemMemoryAvailable()
	{
        sys_memory_info_t memInfo;
        sys_memory_get_user_memory_size(&memInfo);
		return memInfo.available_user_memory; // 64 MB officially announced by Sony
	}
}

//////////////////////////////////////////////////////////////////////////
// QUARANTINE
//////////////////////////////////////////////////////////////////////////
namespace MemMngDebug
{
#if MEMMNG_QUARANTINE==1

    // init quarantine stuff
	void InitQuarantine()
	{
		// PS3_TODO
	}

	void * AllocInQuarantine(u32 iSize, u32 uAlign)
	{
        // PS3_TODO
		return NULL;
	}

	void FreeInQuarantine(void * pDataInQuarantine)
	{
	}

	unsigned int SizeInQuarantine(void * pDataInQuarantine)
	{
        return 0;
	}


	bool FindInQuarantine(void * pDataInQuarantine)
	{
		return false;
	}

#endif // MEMMNG_QUARANTINE==1
} // namespace MemMngDebug

} // namespace ITF
