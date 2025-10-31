#ifndef _ITF_PROFILERMACROWII_H_
#define _ITF_PROFILERMACROWII_H_


#define PROFILER_QUERYPERFORMANCECOUNTER(__val)     {__val=OSGetTime();}
#define PROFILER_TYPECOUNT                          OSTime

#define PROFILER_DELTA(__a,__b)                     ((__a) - (__b))
#define PROFILER_CONVERT_TOMS(__val,__freqSeconds)  (((f64)(__val)/(f64)OS_TIMER_CLOCK)*__freqSeconds*1000.0)

#define PROFILER_GETFREQUENCY()                     1.0
#define PROFILER_BEGIN_EVENT(__name)                {}
#define PROFILER_STOP_EVENT(__name)                 {}

#endif //_ITF_PROFILERMACROWII_H_