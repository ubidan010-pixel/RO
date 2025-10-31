#ifndef _ITF_PROFILERMACRODEFAUT_H_
#define _ITF_PROFILERMACRODEFAUT_H_


#define PROFILER_QUERYPERFORMANCECOUNTER(__val) {}
#define PROFILER_TYPECOUNT u64

#define PROFILER_DELTA(__a,__b ) (__a - __b)
#define PROFILER_CONVERT_TOMS(__val,__freqSeconds ) __val*__freqSeconds

#define PROFILER_GETFREQUENCY_SECONDS() 1.0f

#define PROFILER_BEGIN_EVENT(__name) {}
#define PROFILER_STOP_EVENT(__name) {}

#define PROFILER_GETFREQUENCY()                1.0f

#define PROFILER_BEGIN_EVENT_NAME(__name) {}
#define PROFILER_STOP_EVENT_NAME() {}
#endif //_ITF_PROFILERMACRODEFAUT_H_