#ifndef ITF_PROFILERMACROPS3_H_
#define ITF_PROFILERMACROPS3_H_

#include <libsntuner.h>

namespace ITF
{
    class PS3HighPerfTimer
    {
    public:
        static const u64 ms_frequency;
        static const f64 ms_period; // 1/frequency
        static u64 GetTimeValue() // platform specific
        {
            u64 curTime;
            SYS_TIMEBASE_GET(curTime);
            return curTime;
        }

        static f32 GetInSeconds(u64 timeRef)
        {
            return f32(f64(timeRef)*ms_period);
        }

        static f64 GetInMilliSeconds(u64 timeRef)
        {
            return f64(timeRef)*ms_period*1000.f;
        }


        static f32 GetDeltaInSeconds(u64 timeInf, u64 timeSup)
        {
            return f32(f64(timeSup-timeInf)*ms_period);
        }
    };
}

#define PROFILER_QUERYPERFORMANCECOUNTER(__val)     do{ __val = ITF::PS3HighPerfTimer::GetTimeValue(); }while(0)
#define PROFILER_TYPECOUNT                          u64

#define PROFILER_DELTA(__a,__b)                     ((__a) - (__b))
#define PROFILER_CONVERT_TOMS(__val,__freqSeconds)  ITF::PS3HighPerfTimer::GetInMilliSeconds(__val)

#define PROFILER_GETFREQUENCY()                     ITF::PS3HighPerfTimer::ms_frequency

#if !defined(_DEBUG) && !defined(ITF_FINAL)

    #define PROFILER_BEGIN_EVENT(__ref)             do{char eventName[256]; _snprintf(eventName, 256, "event :%d", __ref); snPushMarker(eventName);}while(0)
    #define PROFILER_STOP_EVENT()                   do{snPopMarker();}while(0)

    #define PROFILER_BEGIN_EVENT_NAME(__name)       do{snPushMarker(__name);}while(0)
    #define PROFILER_STOP_EVENT_NAME()              do{snPopMarker();}while(0)

#else

    #define PROFILER_BEGIN_EVENT(__ref)             do{}while(0)
    #define PROFILER_STOP_EVENT()                   do{}while(0)

    #define PROFILER_BEGIN_EVENT_NAME(__name)       do{}while(0)
    #define PROFILER_STOP_EVENT_NAME()              do{}while(0)

#endif

#endif // ITF_PROFILERMACROPS3_H_
