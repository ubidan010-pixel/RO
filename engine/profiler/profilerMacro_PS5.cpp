#include "precompiled_engine.h"
#include "profilerMacro_PS5.h"

namespace ITF
{
    const u64 PS5HighPerfTimer::ms_frequency = sceKernelGetTscFrequency();
    const f64 PS5HighPerfTimer::ms_period = 1.0/ sceKernelGetTscFrequency();
}

