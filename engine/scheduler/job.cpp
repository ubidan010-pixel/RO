#include "precompiled_engine.h"

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

namespace ITF
{

    void Job::destroy(Job* pJob)
    {
        Scheduler* pScheduler = Scheduler::get();
        if (pScheduler)
        {
            pScheduler->destroyJob(pJob);
        }
        else
        {
            SF_DEL(pJob);
        }
    }

    void Job::destroySelf(Job* pJob)
    {
        if (pJob && !pJob->isManaged())
        {
            SF_DEL(pJob);
        }
    }

    void Job::destroyManaged(Job* pJob)
    {
        if (pJob)
        {
            ITF_ASSERT(pJob->isManaged());
            SF_DEL(pJob);
        }
    }

}