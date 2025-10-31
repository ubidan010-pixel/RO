#ifndef ITF_THREAD_NX_H_
#define ITF_THREAD_NX_H_

#pragma once

#include "core/Types.h"
#include "core/String8.h"
#include <nn/os/os_ThreadLocalStorage.h>

namespace ITF
{
    class ThreadLocalStorage
    {
    public:

        ThreadLocalStorage() = default;
        ~ThreadLocalStorage() = default;

        explicit ThreadLocalStorage(void* _value)
        {
            setValue(_value);
        }

        ThreadLocalStorage(u32 _value)
        {
            m_storage.SetValue(static_cast<uintptr_t>(_value));
        }

        const void * pGetValue() const
        {
            return reinterpret_cast<const void*>(m_storage.GetValue());
        }

        void * pGetPtrValue() const
        {
            return const_cast<void *>(pGetValue());
        }

        u32 getValue() const
        {
            return u32(get64BValue());
        }

        u64 get64BValue() const
        {
            return m_storage.GetValue();
        }


        void setValue(void *_pValue)
        {
	        m_storage.SetValue(reinterpret_cast<uintptr_t>(_pValue));
        }


        void setNULL()
        {
            setValue(nullptr);
        }

        void setValue(u32 _value)
        {
            m_storage.SetValue(static_cast<uintptr_t>(_value));
        }

        void set64BValue(u64 _value)
        {
            m_storage.SetValue(_value);
        }

    private:
        nn::os::ThreadLocalStorage m_storage;
    };

    // utilities
    void sleep(u32 uMilliseconds);
    void YieldProcessor();

    class ThreadSettings;

    class Thread final
    {
    public:
        Thread();
        ~Thread();

        Thread(const Thread&) = delete;
        Thread & operator = (const Thread&) = delete;

        using ThreadFunc = u32(*)(void*);

        void create(ThreadFunc _threadFunc, void* _threadFuncParam, const ThreadSettings& _settings);

        static int getPriorityHW(Thread_Priority ePriority);
        static int getCoreIdHW(Thread_HardwareThreadId eAffinity);

        const Thread_Priority getPriority() const { return m_ePriority; }
        void setAffinity(Thread_HardwareThreadId eThreadId);
        Thread_HardwareThreadId getAffinity() const { return m_eThreadAffinity; }
        const ThreadSettings * getSettings() const { return &m_settings; }

    private:
        u32 join();
        void destroyThread();

        void threadMain();

        Thread_Priority m_ePriority = Thread_Priority_Normal;
        Thread_HardwareThreadId m_eThreadAffinity = Thread_HardwareThread_Unknown;

        nn::os::ThreadType m_osThread{};
        void* m_stack = nullptr;
        ThreadFunc m_threadFunc = nullptr;
        void* m_threadFuncParam = nullptr;

        bool m_created = false;
        bool m_finished = false;
        bool m_waited = false;
        u32 m_returnValue = U32_INVALID;

        void setSettings(const ThreadSettings & _settings) { m_settings = _settings; }
        ThreadSettings m_settings{};
    };

} // namespace ITF

#endif // ITF_THREAD_NX_H_
