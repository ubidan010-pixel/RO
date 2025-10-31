#ifndef _ITF_PROFILERMACROWIN32_H_
#define _ITF_PROFILERMACROWIN32_H_


#define PROFILER_QUERYPERFORMANCECOUNTER(__val) {LARGE_INTEGER li;QueryPerformanceCounter(&li);__val = li.QuadPart;}
#define PROFILER_TYPECOUNT __int64

#define PROFILER_DELTA(__a,__b ) (__a - __b)
#define PROFILER_CONVERT_TOMS(__val,__freqSeconds ) ( (f64)(( __val ) *1000.0) / ( f64 )__freqSeconds)

namespace ITF
{
    extern f64 getProfilerFrequencySeconds();
}


#define PROFILER_GETFREQUENCY() getProfilerFrequencySeconds()

#define PROFILER_BEGIN_EVENT(__ref)     {}
#define PROFILER_STOP_EVENT()           {}

#endif //_ITF_PROFILERMACROWIN32_H_