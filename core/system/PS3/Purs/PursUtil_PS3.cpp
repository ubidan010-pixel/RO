#include "precompiled_core.h"

#include <sys/ppu_thread.h>
#include "PursUtil_PS3.h"

namespace ITF
{
namespace PursUtil
{
    int GetCurrentThreadPriority()
    {
        sys_ppu_thread_t tidPPU;
        int prio = 1;
        SYS_ERROR_CHECK(sys_ppu_thread_get_id(&tidPPU));
        SYS_ERROR_CHECK(sys_ppu_thread_get_priority(tidPPU, &prio));
        return prio;
    }
}
}

