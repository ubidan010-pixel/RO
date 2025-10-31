#include "precompiled_engine.h"

#ifndef _ITF_TASKMANAGER__H_
#include "engine/taskmanager/TaskManager.h"
#endif //_ITF_TASKMANAGER__H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_


namespace ITF
{
    TaskManager::TaskManager() : 
        m_workingTask(NULL) , m_hasToExit(bfalse), m_waitingForATask(btrue)
    {
        Synchronize::createEvent(&m_exitEvent,"TaskManagerExit");
        Synchronize::resetEvent(&m_exitEvent);

        Synchronize::createEvent(&m_newTaskEvent,"TaskManagerNewTask");
        Synchronize::resetEvent(&m_newTaskEvent);

        m_thread = THREADMANAGER->createThread(threadLoop, this, ThreadSettings::m_settings[eThreadId_TaskManager]);
    }

    TaskManager::~TaskManager()
    {
        m_hasToExit = btrue;
        ITF_MemoryBarrier();

        // Wait for thread
        if ( m_thread )
        {
            Synchronize::setEvent(&m_newTaskEvent);
            Synchronize::waitEvent(&m_exitEvent);
        }
        
        // Delete all tasks if any
        TaskList::iterator ite = m_tasksList.begin();
        for (; ite != m_tasksList.end(); ++ite)
        {
            Task* pTask = (*ite);
            SF_DEL (pTask);
        }
        m_tasksList.clear();

        Synchronize::destroyEvent(&m_exitEvent);
        Synchronize::destroyEvent(&m_newTaskEvent);
    }

    void TaskManager::pushTask(execTask _execTask, onTaskFinish _onTaskFinish, void* _param, TaskPriority _priority)
    {
        Task*  newTask = newAlloc(mId_System, Task (_execTask, _onTaskFinish, _param));
        
        if(_priority == Hight)
            m_tasksList.push_front(newTask);
        else
            m_tasksList.push_back(newTask);
    }

    void TaskManager::update()
    {
        if (m_waitingForATask)
        {
            if (m_workingTask)
            {
                // a task is finished, call the finish callback and free the current task
                m_workingTask->end();
                SF_DEL(m_workingTask);
            }

            if (!m_tasksList.empty())
            {
                 // get a new task
                 m_workingTask = m_tasksList.front( );
                 m_tasksList.pop_front();

                 m_waitingForATask = bfalse;
                 ITF_MemoryBarrier();

                 Synchronize::setEvent(&m_newTaskEvent);
            }            
        }        
    }

    void TaskManager::cancelAllTasks()
    {
        m_hasToExit = btrue;
        ITF_MemoryBarrier();

        // Wait for thread completion
        if ( m_thread )
        {
            Synchronize::setEvent(&m_newTaskEvent); // be sure to wake up
            Synchronize::waitEvent(&m_exitEvent);
        }

        m_thread = NULL; // non joinable threads -> nothing to delete (!?)
    }

    u32 WINAPI TaskManager::threadLoop (void* pParams)
    {
        if(!pParams) return 0;
        TaskManager*    pTaskManager = (TaskManager*) pParams;
        
        while ( !pTaskManager->hasToExit() )
        {
            ThreadSettings::threadStartFrame();

            Synchronize::waitEvent(&pTaskManager->m_newTaskEvent);

            if(pTaskManager->m_workingTask)
                pTaskManager->m_workingTask->run();
            
            pTaskManager->m_waitingForATask = btrue;
            ITF_MemoryBarrier();

            Synchronize::resetEvent(&pTaskManager->m_newTaskEvent);
        }

        if ( pTaskManager->hasToExit() )
        {
            Synchronize::setEvent(&pTaskManager->m_exitEvent);
        }
        
        return 0;
    }

} // namespace ITF

