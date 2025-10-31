#ifndef ITF_PROFILERMACRO_NINTENDO_H_
#define ITF_PROFILERMACRO_NINTENDO_H_

#include <nn/os/os_TickApi.h>

#define PROFILER_QUERYPERFORMANCECOUNTER(val_)         { val_ = nn::os::GetSystemTick(); }
#define PROFILER_TYPECOUNT                              nn::os::Tick

#define PROFILER_DELTA(a_, b_)                         ((a_) - (b_))
#define PROFILER_CONVERT_TOMS(val_, freqSeconds_)      (f64(val_.GetInt64Value()/freqSeconds_)*1000.0)

#define PROFILER_GETFREQUENCY()                        nn::os::GetSystemTickFrequency()
#define PROFILER_BEGIN_EVENT(name_)                    {}
#define PROFILER_STOP_EVENT(name_)                     {}


#ifdef ITF_SUPPORT_GPU_METRICS
    #define PROFILER_GPU_ENABLE
    #define PROFILER_GPU_ACTIVATE(__on)                         GFX_ADAPTER_NINTENDO->activateGpuProfile(__on)

    #define PROFILER_GPU_DATAREADYTOSEND()                      GFX_ADAPTER_NINTENDO->isMetricsJustReady()

    #define PROFILER_GPU_TYPECOUNT                              u32
    #define PROFILER_GPU_TYPECOUNT_INVALID                      U32_INVALID

    #define PROFILER_GPU_INIT_COUNTER(__val)                    GFX_ADAPTER_NINTENDO->getNewMetricsTag(&__val)

    #define PROFILER_GPU_QUERYSTARTCOUNTER(__val)               GFX_ADAPTER_NINTENDO->startMetricsTag(__val)
    #define PROFILER_GPU_QUERYENDCOUNTER(__val)                 GFX_ADAPTER_NINTENDO->endMetricsTag(__val)

    #define PROFILER_GPU_COUNTER_VALUE(__val)                   ((f64)GFX_ADAPTER_NINTENDO->getMetricU64(GX2_PERF_U64_GPU_TIME,__val)/1000.0)
#endif // ITF_SUPPORT_GPU_METRICS

#endif // ITF_PROFILERMACRO_NINTENDO_H_
