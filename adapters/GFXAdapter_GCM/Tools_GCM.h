#ifndef ITF_TOOLS_GCM_H_
#define ITF_TOOLS_GCM_H_

#include <sys/sys_time.h>
#include <sys/time_util.h>

#include "core/error/ErrorHandler.h"
#include "Engine/profiler/profilerMacro_PS3.h"

namespace ITF
{
    namespace GCM
    {
        // IsPow2(0) = true, not fully correct as 0 is reported as a pow2 but is not
        template <typename UInt>
        inline bool IsPow2(UInt _value)
        {
            return (_value&(_value-1))==0;
        }

        inline u32 GetLog2(u32 _value) 
        {
            // not efficient -> not constant shift
            ITF_ASSERT(_value > 0); // Log(0) is not defined
            u32 r = 0;
            u32 shift;
            shift = ( ( _value & 0xFFFF0000 ) != 0 ) << 4; _value >>= shift; r |= shift;
            shift = ( ( _value & 0xFF00     ) != 0 ) << 3; _value >>= shift; r |= shift;
            shift = ( ( _value & 0xF0       ) != 0 ) << 2; _value >>= shift; r |= shift;
            shift = ( ( _value & 0xC        ) != 0 ) << 1; _value >>= shift; r |= shift;
            shift = ( ( _value & 0x2        ) != 0 ) << 0; _value >>= shift; r |= shift;
            return  r;
        }

        // Round value to ABOVE alignment
        // Alignment MUST BE POWER OF 2 and not 0 (align on 0 means nothing)
        template <typename UInt>
        inline UInt GetAboveAlign(UInt _value, UInt _align)
        {
            ITF_ASSERT(IsPow2(_align) && _align>0);
            return (_value + (_align - 1)) & ~(_align - 1);
        }

        //////////////////////////////////////////////////////////////////////////
        // the 2 following classes use HighPerfTimer services to offer nicer
        // services

        // This class is a simple profiler start/stop class to profile using explicit method calls
        class TimerProfiler
        {
        public:
            void Start()
            {
                m_startTime = PS3HighPerfTimer::GetTimeValue();
            }

            // Stop() return the time elapsed in seconds since the last Start() call
            // Calling Stop() without previously Start return undefined value.
            f32 Stop()
            {
                return PS3HighPerfTimer::GetDeltaInSeconds(m_startTime, PS3HighPerfTimer::GetTimeValue());
            }

            // The original start time
            u64 GetStartTime()
            {
                return m_startTime;
            }

        protected :
            u64 m_startTime;
        };

        // This class is used for automatic profiling of a section of the code.
        // Create an instance of this class, and get the elapsed time at the code section end
        class TimerScopeProfiler
        {
        public:
            TimerScopeProfiler(f32 & timeMeasure)
                :	m_timeMeasure(timeMeasure),
                m_startTime(PS3HighPerfTimer::GetTimeValue())
            {
            }

            ~TimerScopeProfiler()
            {
                m_timeMeasure += PS3HighPerfTimer::GetDeltaInSeconds(m_startTime, PS3HighPerfTimer::GetTimeValue());
            }

        private:
            f32 & m_timeMeasure;
            const u64 m_startTime;
        };

        // This class is used for automatic profiling of a section of the code, outputted using printf.
        class TimerScopeProfilerLog
        {
        public:
            TimerScopeProfilerLog(const char* tag)
                : m_timerTag(tag)
                , m_startTime(PS3HighPerfTimer::GetTimeValue())
            {
            }

            ~TimerScopeProfilerLog()
            {
                f32 seconds = PS3HighPerfTimer::GetDeltaInSeconds(m_startTime, PS3HighPerfTimer::GetTimeValue());
                LOG("%s: %.5f seconds (%.3fms)\n", m_timerTag, seconds, seconds*1000.f);
            }

        private:
            const char*     m_timerTag;
            const u64    m_startTime;
        };    

    }
}

#endif // ITF_TOOLS_GCM_H_
