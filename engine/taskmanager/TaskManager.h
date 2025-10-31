#ifndef _ITF_TASKMANAGER__H_
#define _ITF_TASKMANAGER__H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif

namespace ITF
{
    // ------------------------------------------------------------------------
    // A classic TaskManager ...
    // ------------------------------------------------------------------------
    class TaskManager
    {
    public:
        typedef i32  (*execTask) (void* pParam) ;
        typedef void (*onTaskFinish) (void* pParam, i32 _result) ;

        enum TaskPriority 
        {
            Normal = 0,
            Hight  = 1
        };

        // Used if the TaskManager has to kill the task in the list (during destructor call)
        static const i32 TaskAborted = -509;

        // Create the thread in the constructor
        TaskManager();
        ~TaskManager();

        // ------------------------------------------------------------------------
        // push a task a list
        // @param1: function which will be called in the TaskManager thread 
        // @param2: callback when the task is finished, the result can be "TaskManager::TaskAborted" 
        // if the task is aborted (when TaskManager is killed)
        // @param3: user param
        // @param4: priority of the task, set "Hight" to be the next task in the list
        // ------------------------------------------------------------------------
        void    pushTask(execTask _execTask, onTaskFinish _onTaskFinish, void* _param = NULL, TaskPriority _priority = Normal);

        // ------------------------------------------------------------------------
        // update the list of tasks
        // and calls the finish callback if necessary
        // ------------------------------------------------------------------------
        void    update ();

        // ------------------------------------------------------------------------
        // cancel all pending tasks, try to cancel the current one,
        // and wait until no more tasks are running.
        // ------------------------------------------------------------------------
        void cancelAllTasks();

        // ------------------------------------------------------------------------
        // for Internal uses, to store task data
        // ------------------------------------------------------------------------
        class Task
        {
        public:
            Task(execTask _run, onTaskFinish _onFinish, void* _param) :
                m_run(_run), m_onFinish(_onFinish), m_res(0), m_running(bfalse), m_param(_param) {}
            ~Task(){}
            // To be called from a thread
            ITF_INLINE void run ()
            {
                m_running = btrue;
                m_res = m_run(m_param);                 
                m_running = bfalse;
            }
            // To be called from the main thread
            ITF_INLINE void end ()
            {
                if(m_onFinish)
                    m_onFinish (m_param, m_res);                
            }
            ITF_INLINE void abort()
            {
                if(m_onFinish)
                    m_onFinish (m_param, TaskAborted);
            }
            bbool isRunning() const { return m_running; }
        private:
            execTask        m_run;
            onTaskFinish    m_onFinish;
            i32             m_res;
            bbool           m_running;
            void*           m_param;
        };

    private:

        // Used to kill the thread
        ITF_INLINE bbool  hasToExit() const { return m_hasToExit; }

        // ------------------------------------------------------------------------
        // TaskManager thread 
        // Wait for new task and run them
        // This function doesn't use the list of task
        // ------------------------------------------------------------------------
        static u32 WINAPI threadLoop (void* pParams);

        // To communicate between task thread and main thread
        volatile bbool      m_hasToExit;
        volatile bbool      m_waitingForATask;
                
        // A standard list (front() and back())
        typedef ITF_LIST<Task*> TaskList;
        TaskList            m_tasksList;

        Task*               m_workingTask;
        Thread*             m_thread;

        // Thread event
        ITF_THREAD_EVENT    m_exitEvent;
        ITF_THREAD_EVENT    m_newTaskEvent;
    };
} // namespace ITF

#endif //_ITF_TASKMANAGER__H_