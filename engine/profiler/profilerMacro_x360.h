#ifndef _ITF_PROFILERMACROX360_H_
#define _ITF_PROFILERMACROX360_H_


#define PROFILER_QUERYPERFORMANCECOUNTER(__val) {LARGE_INTEGER li;QueryPerformanceCounter(&li);__val = li.QuadPart;}
#define PROFILER_TYPECOUNT __int64

#define PROFILER_DELTA(__a,__b ) (__a - __b)
#define PROFILER_CONVERT_TOMS(__val,__freqSeconds ) ( (f64)(( __val ) *1000.0) / ( f64 )__freqSeconds)

namespace ITF
{
    extern f64 getProfilerFrequencySeconds();
}

#define PROFILER_GETFREQUENCY()                 getProfilerFrequencySeconds();

#define PROFILER_BEGIN_EVENT(__ref)             {PIXBeginNamedEvent(0xffff0000,"event :%d",__ref);}
#define PROFILER_STOP_EVENT()                   {PIXEndNamedEvent();}


#define PROFILER_BEGIN_EVENT_NAME(__name)       {PIXBeginNamedEvent(0xff00ff00,__name);}
#define PROFILER_STOP_EVENT_NAME()              {PIXEndNamedEvent();}



#endif //_ITF_PROFILERMACROX360_H_