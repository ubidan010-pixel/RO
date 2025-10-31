 #include "precompiled_engine.h"

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_JOBTHREAD_H_
#include "engine/scheduler/jobThread.h"
#endif //_ITF_JOBTHREAD_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

namespace ITF
{
#if defined (SUPPORTED_SCHEDULING)
    void JobThread::wakeUp()
    {
        m_jobWakeCount++;
        Synchronize::setEvent(&m_waitAction);
    }

    void JobThread::exit()
    {
        m_requestExit = btrue;
        Synchronize::setEvent(&m_waitAction);
    }

    void JobThread::destroyJobs()
    {
        for (u32 index = 0;index<m_jobsToDestroy.size();++index)
            Job::destroySelf(m_jobsToDestroy[index]);


        m_jobsToDestroy.clear();
    }


    void JobThread::pushAsDestroy(Job* _job)
    {
        if (_job && !_job->isManaged())
            m_jobsToDestroy.push_back(_job);
    }

    void JobThread::run()
    {
        while (!m_requestExit)
        {
            Synchronize::waitEvent(&m_waitAction);
            Synchronize::resetEvent(&m_waitAction);
      
            //execute the current job;
            for (;;)
            {
                if (m_CurrentJobToRun)
                {
                    m_jobDoneByFrame++;

                    //time the frame

                    LARGE_INTEGER Start;
                    LARGE_INTEGER End;
                    
                    QueryPerformanceCounter( &Start );
                    m_CurrentJobToRun->execute(bfalse);
                    QueryPerformanceCounter( &End );

                    JobStats stats(Start,End);
                    m_jobStats.push_back(stats);

                    Scheduler::get()->completeJob(m_CurrentJobToRun);
                }

                //flags the job as complete and decrement pending
                
                pushAsDestroy(m_CurrentJobToRun);
              
                m_CurrentJobToRun = NULL;//TODO use an interlock for null

                //done!
             
                if (m_requestExit)
                {
                    break;
                }

                Job* pJob = Scheduler::get()->getANewJob();

                if (pJob)
                {       
                    m_CurrentJobToRun = pJob;
                }
                else
                {
                    ITF_MemoryBarrier();
                    
                    Scheduler::get()->pushBackAvailable(this);
                    LOG_SCHEDULER("SLEEPING %d",this->m_JobThreadId);
                    
                    break;//return to event waiting
                }
            }
        }
    }
#endif //SUPPORTED_SCHEDULING
}