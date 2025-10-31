#include "precompiled_core.h"
#include <kernel.h>

#include "MemMng.h"
#include "MemMngPrivate.h"
#include "MemMngPage.h"


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
        fprintf(stderr, "%s", msg);
        fprintf(stderr, "%s", expr);
        fprintf(stderr, "\n");
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
// Memory info from platform
//////////////////////////////////////////////////////////////////////////
namespace MemMngDebug
{
    uPtr GetCurrentPhysicalMemoryUsed()
    {
        uPtr totalMemory = 0;
        SceKernelVirtualQueryInfo info;
        void * addr = nullptr;
        int32_t ret = 0;
        do 
        {
            ret = sceKernelVirtualQuery( addr, SCE_KERNEL_VQ_FIND_NEXT, &info, sizeof(SceKernelVirtualQueryInfo));

            if (ret != SCE_OK)
            {
                break;
            }

            totalMemory += uPtr(info.end) - uPtr(info.start);

            addr = info.end;

        } while (true);
        
        return totalMemory;
    }

    uPtr GetTotalPhysicalMemory()
    {
        return uPtr(SCE_KERNEL_MAIN_DMEM_SIZE);
    }
} // namespace MemMngDebug

} // namespace ITF
