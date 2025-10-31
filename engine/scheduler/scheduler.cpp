#include "precompiled_engine.h"

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif // _ITF_MEMORY_H_

#ifndef _ITF_JOBTHREAD_H_
#include "engine/scheduler/jobThread.h"
#endif //_ITF_JOBTHREAD_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_SCHEDULERMONITOR_PLUGIN_H_
#include "tools/plugins/SchedulerMonitorPlugin/SchedulerMonitorPlugin.h"
#endif //_ITF_SCHEDULERMONITOR_PLUGIN_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifdef ITF_WINDOWS
#include <intrin.h>

#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)

#endif //ITF_WINDOWS

namespace ITF
{
    class Job;

    Scheduler* Scheduler::m_Instance = NULL;

    void Scheduler::initSingleton()
    {
        m_Instance = newAlloc(mId_System,Scheduler());
    }

    void Scheduler::destroySingleton()
    {
        if (m_Instance)
            m_Instance->destroy();
        SF_DEL(m_Instance);
    }

#ifdef SUPPORTED_SCHEDULING
    Scheduler::Scheduler():m_bCaptureRequested(bfalse),m_jobThreadCounter(0),m_currentJobPending(0),m_jobRequestedByFrame(0),
                           m_immediateMode(bfalse),m_bFirstPush(bfalse),m_bLackOfJobDetected(bfalse),m_bEnableLock(btrue),pListHeadJobThread(0),
                           m_processorCount(0),m_availableJobThreadAll(0),m_fTicksPerMicrosecond(0.f),m_availableJobThread(0), m_bInit(bfalse)
    {
        m_Instance = this;
        m_immediateModeDirty = m_immediateMode;

#ifdef DEBUG_SCHEDULER
        Synchronize::createCriticalSection(&m_csDebug);
#endif // DEBUG_SCHEDULER

        m_availableJobThread = 0;

        ITF_MemSet((void*)&m_jobPendingByTag[0],0,sizeof(m_jobPendingByTag));
    }

    Scheduler::~Scheduler()
    {
        destroyAllJob();

        m_Instance = NULL;
#ifdef DEBUG_SCHEDULER
        Synchronize::destroyCriticalSection(&m_csDebug);
#endif // DEBUG_SCHEDULER
    }

    u32 Scheduler::registerJob(Job* _pJob)
    {
        u32 newid = m_vJobFactory.size();
        m_vJobFactory.push_back(_pJob);
        return newid;
    }

    void Scheduler::synchronize()
    {
        if (!m_immediateMode)
        {
            for (u32 index = 0;index<m_vJobSynchronize.size();++index)
            {
                m_vJobSynchronize[index]->synchronize();
            }
        }
    }

    u32 WINAPI runJobber(void* user)
    {
        JobThread* pJobThread = (JobThread*) user;
        pJobThread->run();
        return 0;
    }

    void Scheduler::init(bbool bEnable)
    {
        //get the number of processor available,that tell you max thread you can create according the hardware

        //create thread according the scheduler:
        //TODO ,make a settings core list
#if defined(ITF_X360)
        const Thread_HardwareThreadId coreList[] = {Thread_HardwareThread1,Thread_HardwareThread3,Thread_HardwareThread4,Thread_HardwareThread5};
        m_processorCount = sizeof(coreList)/sizeof(coreList[0]);
#elif defined(ITF_PS3)
        m_processorCount = 1;
#else
        m_processorCount = 4;
#endif 

        LOG(">>>Scheduler Processor count :%d",m_processorCount);

        m_immediateMode = !bEnable;//let's do the thread creation and list in case you want to enable it runtime

        LARGE_INTEGER TicksPerSecond;
        QueryPerformanceFrequency( &TicksPerSecond );
        m_fTicksPerMicrosecond = (double)TicksPerSecond.QuadPart * 0.000001;


        m_arrayJobThread = newAlloc(mId_System,JobThread[m_processorCount]);
        u32 jobThreadCount = m_processorCount;//TODO apply a coef
        m_jobThreadCounter = jobThreadCount;

        //decide how much thread you want to create;

        pListHeadJobThread = (PSLIST_HEADER)Memory::alignedMallocCategory(sizeof(SLIST_HEADER), 8,MemoryId::mId_System);
        pListHeadPendingJob = (PSLIST_HEADER)Memory::alignedMallocCategory(sizeof(SLIST_HEADER), 8,MemoryId::mId_System);

        InitializeSListHead(pListHeadJobThread);
        InitializeSListHead(pListHeadPendingJob);

        {
            m_settings = newAlloc(mId_System,ThreadSettings[m_processorCount]);

            for (u32 index=0;index <m_processorCount;++index)
            {
                char* pName =  new_array(char,255,mId_System);
                sprintf(pName,"JobThread%d",index);
                m_settings[index].m_name = pName;
                m_settings[index].m_priority = Thread_Priority_Normal;
#ifdef ITF_X360
                m_settings[index].m_hwId = coreList[index];
#else
                m_settings[index].m_hwId = (Thread_HardwareThreadId) (Thread_HardwareThread2 + index);
#endif //ITF_X360


                m_arrayJobThread[index].setIndex(index);

                long  mask =  1<<index;
                JobThread *pJobThread = &m_arrayJobThread[index]; 
                pJobThread->m_mask =  mask;

                THREADMANAGER->createThread(runJobber, &m_arrayJobThread[index], m_settings[index]);

                //make it available
                m_availableJobThread |= mask;

                PJOBTHREAD_ITEM pJobThreadItem = (PJOBTHREAD_ITEM)Memory::alignedMallocCategory(sizeof(JOBTHREAD_ITEM), 8,MemoryId::mId_System);

                pJobThreadItem->pJobThread = &m_arrayJobThread[index];
                pJobThreadItem->pJobThread->m_pEntry = &pJobThreadItem->ItemEntry;

                InterlockedPushEntrySList(pListHeadJobThread,&pJobThreadItem->ItemEntry);
            }
        }

        m_availableJobThreadAll = m_availableJobThread;

        m_bInit = btrue;
    }

#ifdef DEBUG_SCHEDULER
    void Scheduler::lockDebug()
    {
        if (m_bEnableLock)
            Synchronize::enterCriticalSection(&m_csDebug);
    }

    void Scheduler::unlockDebug()
    {
        if (m_bEnableLock)
            Synchronize::leaveCriticalSection(&m_csDebug);
    }
#endif // DEBUG_SCHEDULER

    JobThread* Scheduler::popBackAvailable()
    {
        PSLIST_ENTRY    pListEntry  = InterlockedPopEntrySList(pListHeadJobThread);
        PJOBTHREAD_ITEM pJobItem    = (PJOBTHREAD_ITEM)pListEntry;

        if (pJobItem)
        {
            // There is an issue with intellisense to define _InterlockedAnd on not windows platforms.
            // Indeed, _InterlockedAnd is a compiler reserved keyword, but on windows we want to use it to get intrinsics.
        #ifdef ITF_WINDOWS
            _InterlockedAnd(&m_availableJobThread, ~pJobItem->pJobThread->m_mask);
        #else
            InterlockedAnd(&m_availableJobThread, ~pJobItem->pJobThread->m_mask);
        #endif 
        }
        
        return pJobItem ? pJobItem->pJobThread : NULL;
    }

#ifdef ITF_SUPPORT_SCHEDULECAPTURE
    void Scheduler::addMarker(const String& _name)
    {
        LARGE_INTEGER MarkerStart;
        QueryPerformanceCounter( &MarkerStart );
        m_MarkerList.push_back(MarkerScheduler(_name,MarkerStart));
    }

    void Scheduler::addMarkerJob(const String& _name)
    {
        LARGE_INTEGER MarkerStart;
        QueryPerformanceCounter( &MarkerStart );
        m_MarkerJobList.push_back(MarkerSchedulerJob(_name,MarkerStart));
    }
#endif //ITF_SUPPORT_SCHEDULECAPTURE

    void Scheduler::pushBackAvailable(JobThread* pJobThread)
    {
        InterlockedPushEntrySList(pListHeadJobThread,pJobThread->m_pEntry);
    #ifdef ITF_WINDOWS
        _InterlockedOr(&m_availableJobThread,pJobThread->m_mask);
    #else
        InterlockedOr(&m_availableJobThread, pJobThread->m_mask);
    #endif
    }

    void Scheduler::beginFrame()
    {
        if (m_immediateModeDirty != m_immediateMode)
        {
            m_immediateMode = m_immediateModeDirty;
        }

        //time the frame
        QueryPerformanceCounter( &Start );
    }


    void Scheduler::startCapture()
    {
        m_bCaptureRequested = btrue;
    }

    void Scheduler::endFrame()
    {
        //get information from jobThread;
        QueryPerformanceCounter( &End );

#ifdef ITF_SUPPORT_SCHEDULECAPTURE 
        i64 Ticks = End.QuadPart - Start.QuadPart;
        DOUBLE microSeconds = Ticks / getTicksPerMicroSecond();

        if (m_bCaptureRequested)
        {
            ITF::Blob blobWriter(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);

            blobWriter.pushString(SchedulerMonitor_Plugin::getPluginName());
            blobWriter.pushString("CaptureDone");

            const u32 markerSize = m_MarkerList.size();
            blobWriter.pushUInt32(markerSize);

            for (ITF_VECTOR<MarkerScheduler>::iterator iter = m_MarkerList.begin();iter!=m_MarkerList.end();++iter)
            {
                MarkerScheduler& marker = (*iter);
                 i64 startMarker = (*iter).m_Start.QuadPart -  Start.QuadPart;
                 float msStart = (float) (startMarker / getTicksPerMicroSecond()*0.001f);

                blobWriter.pushString(marker.m_Name);
                blobWriter.pushFloat32(msStart);
            }

            const u32 markerSizeJob = m_MarkerJobList.size();
            blobWriter.pushUInt32(markerSizeJob);

            for (ITF_VECTOR<MarkerSchedulerJob>::iterator iter = m_MarkerJobList.begin();iter!=m_MarkerJobList.end();++iter)
            {
                MarkerSchedulerJob& marker = (*iter);
                i64 startMarker = (*iter).m_Start.QuadPart -  Start.QuadPart;
                float msStart = (float) (startMarker / getTicksPerMicroSecond()*0.001f);

                blobWriter.pushString(marker.m_Name);
                blobWriter.pushFloat32(msStart);
            }

            blobWriter.pushUInt32(m_jobThreadCounter);
            blobWriter.pushBool(m_bLackOfJobDetected);
            blobWriter.pushUInt32(m_jobRequestedByFrame);

            float msSeconds =(float) microSeconds*0.001f;
            blobWriter.pushFloat32(msSeconds);

            for (u32 index=0;index<m_jobThreadCounter;++index)
            {
                JobThread* pJobThread = &m_arrayJobThread[index];
                blobWriter.pushUInt32(pJobThread->m_jobStats.size());
                blobWriter.pushUInt32(pJobThread->m_jobWakeCount);
      
                //foreach of them push the time relative and duration
                for (ITF_VECTOR<JobStats>::iterator iter = pJobThread->m_jobStats.begin();iter!=pJobThread->m_jobStats.end();++iter)
                {
                    //start relative
                    i64 startJobThread = (*iter).Start.QuadPart -  Start.QuadPart;
                    i64 durationJobThread = (*iter).End.QuadPart -  (*iter).Start.QuadPart;

                    float msStart = (float) (startJobThread / getTicksPerMicroSecond()*0.001f);
                    float msDuration = (float) (durationJobThread / getTicksPerMicroSecond()*0.001f);

                    blobWriter.pushFloat32(msStart);
                    blobWriter.pushFloat32(msDuration);
                }
            }

#if defined (ITF_X360) && !defined(ITF_FINAL)
            SchedulerMonitor_Plugin* plug = static_cast<SchedulerMonitor_Plugin*>(PLUGINGATEWAY->getPluginByName(SchedulerMonitor_Plugin::getPluginName()));
            plug->send(blobWriter);
#endif //(ITF_X360) && !defined(ITF_FINAL)

            m_bCaptureRequested = bfalse;
        }
#endif //ITF_SUPPORT_SCHEDULECAPTURE

        m_MarkerList.clear();
        m_MarkerJobList.clear();
        m_bLackOfJobDetected = bfalse;
        m_bFirstPush = bfalse;
        m_jobRequestedByFrame = 0;

        for (u32 index=0;index<m_jobThreadCounter;++index)
        {
            JobThread* pJobThread = &m_arrayJobThread[index];
            pJobThread->m_jobDoneByFrame = 0;
            pJobThread->clearStats();
            pJobThread->destroyJobs();
        }

        ITF_ASSERT(getANewJob() == NULL);

        destroyAllJob();

        m_vJobSynchronize.clear();

#ifdef DEBUG_SCHEDULER
        m_vJobDebug.clear();
#endif //DEBUG_SCHEDULER
    }

    void Scheduler::destroyAllJob()
    {
        for (u32 index = 0;index< m_jobsToDestroy.size();++index)
        {
            Job* pJob = m_jobsToDestroy[index];
            if (!pJob->isManaged())
                Job::destroySelf(pJob);
            else
                Job::destroyManaged(pJob);
        }

        m_jobsToDestroy.clear();
    }

    void Scheduler::clearStats()
    {
        for (u32 index=0;index <m_jobThreadCounter;++index)
        {
           m_arrayJobThread[index].clearStats();
        }
    }

    void Scheduler::completeJob(Job* _pJob)
    {
        const u32 tag = _pJob->getTag();
        InterlockedDecrement(&m_jobPendingByTag[tag]);
        InterlockedDecrement(&m_currentJobPending);
        LOG_SCHEDULER("DONE Request for m_jobPendingByTag[%d] = %d // %d",tag,m_jobPendingByTag[tag],m_currentJobPending);
    }

    void Scheduler::waitTag(u32 _tag)
    {
        //wait until a specific is completed
        ITF_MemoryBarrier();

        while (m_jobPendingByTag[_tag]!=0)
        {          
            checkNewJob();
            LOG_SCHEDULER("m_jobPendingByTag[%d] = %d m_currentJobPending:%d",_tag,m_jobPendingByTag[_tag],m_currentJobPending);
        }


        LOG_SCHEDULER("PASSSED m_jobPendingByTag[%d] = %d ",_tag,m_jobPendingByTag[_tag]);
    }

    void Scheduler::checkNewJob()
    {
        //wait..
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();
        YieldProcessor();

        if (m_availableJobThreadAll == m_availableJobThread && m_currentJobPending)
        {
            //wake up
            JobThread* jobThreadAvailable = popBackAvailable();

            if (jobThreadAvailable)
            {
                //lets the jobThread found a job
                jobThreadAvailable->m_CurrentJobToRun = NULL;
                jobThreadAvailable->wakeUp();
            }
        }
        else
        {
#ifdef ITF_X360
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
            YieldProcessor();
#elif defined(ITF_PS3)
            sys_timer_usleep(100);
#else
            sleep(1);
#endif //ITF_X360

        }
    }

    void Scheduler::waitAllCompleted()
    {
        ADD_MARKER("WAIT_ALL");
        ITF_MemoryBarrier();

        while (m_currentJobPending != 0 && m_availableJobThreadAll != m_availableJobThread)
        {
            checkNewJob();
        }

        ADD_MARKER("FINISH_WAIT");

    }

    Job* Scheduler::getANewJob()
    {
        PSLIST_ENTRY    pListEntry  = InterlockedPopEntrySList(pListHeadPendingJob);
        if (pListEntry)
        {
            PJOB_ITEM pJobItem    = (PJOB_ITEM)pListEntry;
            Job* pJob = pJobItem->pJob;
            Memory::alignedFree(pJobItem);
            return pJob;
        }

        return NULL;
    }


    void Scheduler::destroyJob(Job* _pJob)
    {
        m_jobsToDestroy.push_back(_pJob);
    }

    void Scheduler::pushJob(Job* _pJob)
    {        
        //find a jobThread available
        /*if (m_locked)
        {
            LOG("Scheduler::locked");
            ITF_ASSERT_CRASH(0);
        }*/
  
        if (!m_immediateMode)
        {
#ifdef DEBUG_SCHEDULER
            if (m_vJobDebug.find(_pJob)!=-1)
            {
                ITF_ASSERT_CRASH(0,"A job has been already registered in this frame");
            }
            else
            {
                m_vJobDebug.push_back(_pJob);
            }
#endif //!DEBUG_SCHEDULER

            if (_pJob->isSynchronizable())
                m_vJobSynchronize.push_back(_pJob);

            m_jobRequestedByFrame++;
            if (!m_bFirstPush)
            {
                ADD_MARKER("FIRST_PUSH");
                m_bFirstPush = btrue;
            }
            
            //addMarkerJob("marker");

            ITF_MemoryBarrier();
            InterlockedIncrement(&m_currentJobPending);
             //LOG_SCHEDULER("access 0x%08x",_pJob);
            const u32 tag = _pJob->getTag();
            InterlockedIncrement(&m_jobPendingByTag[tag]);
            JobThread* jobThreadAvailable = popBackAvailable();
        
            LOG_SCHEDULER("PUSHED Request for m_jobPendingByTag[%d] = %d %d",tag,m_jobPendingByTag[tag],m_currentJobPending);
            if (jobThreadAvailable)
            {
                jobThreadAvailable->m_CurrentJobToRun = _pJob;
                jobThreadAvailable->wakeUp();
            }
            else
            {
                m_bLackOfJobDetected = btrue;
                //enqueue
                PJOB_ITEM pJobThreadItem = (PJOB_ITEM)Memory::alignedMallocCategory(sizeof(JOB_ITEM), 8,MemoryId::mId_System);

                pJobThreadItem->pJob = _pJob;
                InterlockedPushEntrySList(pListHeadPendingJob,&pJobThreadItem->ItemEntry);
            }


        }
        else
        {
           _pJob->execute(btrue);
           Job::destroySelf(_pJob);
        }
    }

    void Scheduler::destroy()
    {
        if (!m_bInit)
            return;

        for (u32 index = 0;index<m_processorCount;++index)
            m_arrayJobThread[index].exit();

        //TODO wait by events instead of time;
        sleep(500);

        SF_DEL_ARRAY(m_arrayJobThread);

        for (u32 index = 0;index<m_processorCount;++index)
        {
            SF_DEL_ARRAY(m_settings[index].m_name);
        }

        SF_DEL_ARRAY(m_settings);

        for (;;)
        {
            PSLIST_ENTRY    pListEntry  = InterlockedPopEntrySList(pListHeadJobThread);
            if (pListEntry)
                Memory::alignedFree(pListEntry);
            else
                break;
        }
        
        for (u32 index = 0 ; index<m_vJobFactory.size();index++)
        {
            Job::destroy(m_vJobFactory[index]);
        }

        Memory::alignedFree(pListHeadJobThread);
        Memory::alignedFree(pListHeadPendingJob);
    }
#else

void Scheduler::pushJob(Job* _pJob)
{
    _pJob->execute(btrue);
    Job::destroySelf(_pJob);     
}

void Scheduler::destroyJob(Job* _pJob)
{
    SF_DEL(_pJob);
}

#endif //SUPPORTED_SCHEDULING
}

