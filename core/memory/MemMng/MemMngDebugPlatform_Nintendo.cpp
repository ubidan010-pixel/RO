#include "precompiled_core.h"

#include <nn/nn_Log.h>
#include <nn/os/os_DebugApi.h>

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
        NN_LOG("%s", msg);
        NN_LOG("%s", expr);
        NN_LOG("\n");
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
        nn::os::MemoryInfo memInfo{};
        nn::os::QueryMemoryInfo(&memInfo);

        return memInfo.totalAvailableMemorySize;
    }

    uPtr GetTotalPhysicalMemory()
    {
        nn::os::MemoryInfo memInfo{};
        nn::os::QueryMemoryInfo(&memInfo);
        return memInfo.totalAvailableMemorySize;
    }
}

} // namespace ITF
