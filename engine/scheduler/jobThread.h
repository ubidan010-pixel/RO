#ifndef _ITF_JOBTHREAD_H_
#define _ITF_JOBTHREAD_H_

namespace ITF
{
    class Job;
    class JobThread;
#ifdef SUPPORTED_SCHEDULING
    typedef struct _JOBTHREAD_ITEM {
        SLIST_ENTRY ItemEntry;
        JobThread* pJobThread; 
    } JOBTHREAD_ITEM, *PJOBTHREAD_ITEM;


    typedef struct _JOB_ITEM {
        SLIST_ENTRY ItemEntry;
        Job* pJob; 
    } JOB_ITEM, *PJOB_ITEM;


    class JobStats
    {
    public:

        JobStats(LARGE_INTEGER& _start,LARGE_INTEGER& _end)
        {
            Start   = _start;
            End     = _end;
        };


        LARGE_INTEGER Start;
        LARGE_INTEGER End;
    };


    class JobThread
    {
    public:

        JobThread():m_available(btrue),m_requestExit(bfalse),m_jobWakeCount(0),
                    m_CurrentJobToRun(NULL), m_jobDoneByFrame(0),m_pEntry(NULL)
        {
            Synchronize::createEvent(&m_waitAction,NULL);
            Synchronize::resetEvent(&m_waitAction);
        };

        void setIndex(u32 _JobThreadId)
        {
            m_JobThreadId = _JobThreadId;
        }

        void pushAsDestroy(Job* _job);
   
        void clearStats()
        {
            m_jobWakeCount = 0;
            m_jobStats.clear();
        }

        void destroyJobs();

        ~JobThread()
        {
            Synchronize::destroyEvent(&m_waitAction);
        };

        void run();
        void wakeUp();

        Job* m_CurrentJobToRun; //interlocked ptr;

        ITF_INLINE void setAvailable(bbool _available)  {m_available = _available;}
        ITF_INLINE bbool isAvailable()                  {return m_available;}
        void exit();

        ITF_THREAD_EVENT   m_waitAction;
        bbool m_available;
        bbool m_requestExit;
        u32 m_JobThreadId;
        u32 m_jobDoneByFrame;
        u32 m_jobWakeCount;
        long  m_mask;

        PSLIST_ENTRY m_pEntry;
        ITF_VECTOR<JobStats> m_jobStats;

    protected:

        SafeArray<Job*> m_jobsToDestroy;
    };
#endif //SUPPORTED_SCHEDULING
}

#endif // _ITF_JOBTHREAD_H_

