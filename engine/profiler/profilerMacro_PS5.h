#ifndef ITF_PROFILERMACROPS5_H_
#define ITF_PROFILERMACROPS5_H_

#include <razorcpu.h>

namespace ITF
{
    class PS5HighPerfTimer
    {
    public:
        static const u64 ms_frequency;
        static const f64 ms_period; // 1/frequency
        static u64 GetTimeValue() // platform specific
        {
            return sceKernelReadTsc();
        }

        static f32 GetInSeconds(u64 timeRef)
        {
            return f32(f64(timeRef)*ms_period);
        }

        static f64 GetInMilliSeconds(u64 timeRef)
        {
            return f64(timeRef)*ms_period*1000.;
        }

        static f64 GetInMilliSeconds(u64 timeRef, u64 frequency)
        {
            ITF_ASSERT(frequency == ms_frequency);
            return f64(timeRef) * (1. / f64(frequency)) * 1000.;
        }


        static f32 GetDeltaInSeconds(u64 timeInf, u64 timeSup)
        {
            return f32(f64(timeSup-timeInf)*ms_period);
        }
    };
}

#define PROFILER_QUERYPERFORMANCECOUNTER(val_)     do{ val_ = ITF::PS5HighPerfTimer::GetTimeValue(); }while(0)
#define PROFILER_TYPECOUNT                          u64

#define PROFILER_DELTA(a_, b_)                     ((a_) - (b_))
#define PROFILER_CONVERT_TOMS(val_,freqSeconds_)   ITF::PS5HighPerfTimer::GetInMilliSeconds(val_, freqSeconds_)

#define PROFILER_GETFREQUENCY()                    ITF::PS5HighPerfTimer::ms_frequency

#define PROFILER_SCOPED_EVENT(txt, ...)

#if !defined(ITF_FINAL)

    #define ITF_USE_RAZOR_PROFILER

    #define PROFILER_BEGIN_EVENT(ref_)             do{ char eventName[256]; snprintf(eventName, 256, "event :%d", ref_); sceRazorCpuPushMarker(eventName, SCE_RAZOR_CPU_COLOR_CYAN, 0); }while(0)
    #define PROFILER_STOP_EVENT()                  sceRazorCpuPopMarker()

    #define PROFILER_BEGIN_EVENT_NAME(name_)       sceRazorCpuPushMarker(name_, SCE_RAZOR_CPU_COLOR_RED, 0)
    #define PROFILER_STOP_EVENT_NAME()             sceRazorCpuPopMarker()

#else

    #define PROFILER_BEGIN_EVENT(__ref)             do{}while(0)
    #define PROFILER_STOP_EVENT()                   do{}while(0)

    #define PROFILER_BEGIN_EVENT_NAME(__name)       do{}while(0)
    #define PROFILER_STOP_EVENT_NAME()              do{}while(0)

#endif

#endif // ITF_PROFILERMACROPS5_H_
