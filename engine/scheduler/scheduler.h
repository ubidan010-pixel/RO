#ifndef _ITF_SCHEDULER_H_
#define _ITF_SCHEDULER_H_

//#define TRACE_SCHEDULER

#if defined  (ITF_WINDOWS) && !defined (ITF_FINAL)
#define DEBUG_SCHEDULER
#endif //defined  (ITF_WINDOWS) && !defined (ITF_FINAL)

#ifdef TRACE_SCHEDULER
#define LOG_SCHEDULER(_format, ...) LOG(_format,__VA_ARGS__)
#else
#define LOG_SCHEDULER(_format, ...) ((void)0)
#endif

namespace ITF
{
class Job;
class JobThread;

#define MAX_USERJOBALLOWED 32


//#define ITF_SUPPORT_SCHEDULECAPTURE

class MarkerScheduler
{
public:
    MarkerScheduler(const String& _marker,LARGE_INTEGER& _start):m_Name(_marker)
    {
        m_Start = _start;
    }

    String m_Name;
    LARGE_INTEGER m_Start;


};

class MarkerSchedulerJob
{
public:
    MarkerSchedulerJob(const String& _marker,LARGE_INTEGER& _start):m_Name(_marker)
    {
        m_Start = _start;
    }

    String m_Name;
    LARGE_INTEGER m_Start;
};


class Scheduler
{
    public:
        static void initSingleton();
        static void destroySingleton();

        static Scheduler* get() {return m_Instance;}

        void pushJob(Job* _pJob);
        void destroyJob(Job* _pJob);

    protected:
        static Scheduler* m_Instance;

#ifdef SUPPORTED_SCHEDULING
    public:
        Scheduler();
        virtual ~Scheduler();

        void init(bbool _bEnable);
        void destroy();

        void beginFrame();
        void endFrame();

        void completeJob(Job* _pJob);
        void clearStats();

        void dump(Blob& _blob);

#ifdef DEBUG_SCHEDULER
        void lockDebug();
        void unlockDebug();
#endif // DEBUG_SCHEDULER

        void setImmediateMode(bbool _bState)    {m_immediateModeDirty = _bState;}
        void swapImmediateMode()                {m_immediateModeDirty = !m_immediateMode;}
                
        JobThread* popBackAvailable();

        void pushBackAvailable(JobThread* pJobThread);

        void waitAllCompleted();
        Job* getANewJob();
        void startCapture();
        u32  registerJob(Job* _pJob);

#ifdef ITF_SUPPORT_SCHEDULECAPTURE
        void addMarker(const String& _marker);
        void addMarkerJob(const String& _name);
#endif //ITF_SUPPORT_SCHEDULECAPTURE

        void checkNewJob();

        void waitTag(u32 _tag);
        void synchronize();

        double getTicksPerMicroSecond() {return m_fTicksPerMicrosecond;}
        void destroyAllJob();

    protected:

        ThreadSettings*  m_settings;
        JobThread*  m_arrayJobThread;
        u32         m_processorCount;
        u32         m_jobThreadCounter;
        bbool       m_bCaptureRequested;
        bbool       m_bInit;
            
        SafeArray<Job*>   m_vJobList;

        SafeArray<Job*>   m_vJobFactory;

#ifdef DEBUG_SCHEDULER
        ITF_THREAD_CRITICAL_SECTION m_csDebug;
#endif // DEBUG_SCHEDULER

        long  m_availableJobThreadAll;
        volatile long  m_currentJobPending;
        u32             m_jobRequestedByFrame;
        bbool           m_immediateMode;
        bbool           m_immediateModeDirty;
        bbool           m_bEnableLock;
        bbool           m_bLackOfJobDetected;
        bbool           m_bFirstPush;

        volatile long    m_jobPendingByTag[MAX_USERJOBALLOWED];

        volatile long  m_availableJobThread;
        PSLIST_HEADER pListHeadJobThread;
        PSLIST_HEADER pListHeadPendingJob;

        ITF_VECTOR<MarkerScheduler> m_MarkerList;
        ITF_VECTOR<MarkerSchedulerJob> m_MarkerJobList;

        SafeArray<Job*> m_jobsToDestroy;

        SafeArray<Job*>   m_vJobSynchronize;
                
        double m_fTicksPerMicrosecond;
        LARGE_INTEGER Start;
        LARGE_INTEGER End;

#ifdef DEBUG_SCHEDULER
        SafeArray<Job*>   m_vJobDebug;
#endif //!DEBUG_SCHEDULER

#ifdef ITF_SUPPORT_SCHEDULECAPTURE
#define ADD_MARKER(__marker)  addMarker(__marker);
#else
#define ADD_MARKER(__marker) {};
#endif //ITF_SUPPORT_SCHEDULECAPTURE

#else
    public:
        ITF_INLINE Scheduler::Scheduler()
        {}

        ITF_INLINE Scheduler::~Scheduler()
        {
            m_Instance = NULL;
        }

        ITF_INLINE void Scheduler::init(bbool _bEnable) {}
        ITF_INLINE void Scheduler::beginFrame(void)     {}
        ITF_INLINE void Scheduler::endFrame(void)       {}
        ITF_INLINE void Scheduler::destroy(void)        {}
        ITF_INLINE void Scheduler::waitTag(u32 _tag)    {}

        ITF_INLINE void setImmediateMode(bbool _bState) {}
        ITF_INLINE void synchronize()                   {}
#endif // SUPPORTED_SCHEDULING
};



}

#endif // _ITF_SCHEDULER_H_

