#include "precompiled_engine.h"
#include "profilerMacro_PS3.h"

namespace ITF
{
    const u64 PS3HighPerfTimer::ms_frequency = sys_time_get_timebase_frequency();
    const f64 PS3HighPerfTimer::ms_period = 1.0/f64(sys_time_get_timebase_frequency()); // 1/frequency
}

