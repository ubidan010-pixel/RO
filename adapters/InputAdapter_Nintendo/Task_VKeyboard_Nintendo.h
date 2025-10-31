#ifndef _ITF_TASK_VKEYBOARD_NX_H_
#define _ITF_TASK_VKEYBOARD_NX_H_

// ITF_PASTA not same place in RO2
//#include "core/taskmanager/Task.h"
#include "engine/taskmanager/TaskManager.h"

namespace ITF
{
    class Task_VKeyboard_NX 
		// ITF_PASTA TODO : in RO2 tasks are no objects
		//: public Task
    {
    public:
        Task_VKeyboard_NX(const InputAdapter::VKeyboardOpenParams& params, const String8& guideText);
        virtual ~Task_VKeyboard_NX();

        const String8& getText() const { return m_text; }


    private:
        const InputAdapter::VKeyboardOpenParams m_params;
        String8                                 m_text;

        bool initialize() { return true; }
		// ITF_PASTA TODO : in RO2 tasks are not objects
		void run() ;
		//EResult run();
		void finalize() {}

	public:
		// ITF_PASTA TODO : in RO2 tasks are no objects, so API not in RO2
		void acquire() { ITF_ASSERT(false); }
		void release() { ITF_ASSERT(false); }
    };
}

#endif // _ITF_TASK_VKEYBOARD_NX_H_
