#ifndef ITF_THREAD_PS5_H_
#define ITF_THREAD_PS5_H_

#pragma once

#include <kernel.h>
#include <sce_atomic.h>

namespace ITF
{
    // Due to wrong initial design, TLS implementation is limited.
    class ThreadLocalStorage
    {
    private:
        enum : u32
        {
            Max_TLS_Slots = 20, // When overflowed, everything go to Max_TLS_Slots idx
        };

        // Add one slot for overflow
        static __thread void* ms_threadLocalStorage[Max_TLS_Slots+1];
        static volatile int64_t ms_currentIndex;

        static u32 acquireIndex()
        {
            u32 newIdx = sceAtomicIncrement64(&ms_currentIndex);
            ITF_ASSERT(newIdx < Max_TLS_Slots);
            newIdx = newIdx > Max_TLS_Slots ? u32(Max_TLS_Slots) : newIdx;
            return newIdx;
        }

    public:

        ThreadLocalStorage() = default;

        ThreadLocalStorage(u64 _value)
        {
            setValue(_value);
        }

        ~ThreadLocalStorage()
        {
            // TODO: do a proper implementation and reuse unused slots
            ITF_ASSERT(m_index < Max_TLS_Slots);
        }


        const void* pGetValue() const
        {
            ITF_ASSERT(m_index < Max_TLS_Slots);
            return ms_threadLocalStorage[m_index];
        }

        void* pGetPtrValue() const
        {
            ITF_ASSERT(m_index < Max_TLS_Slots);
            return ms_threadLocalStorage[m_index];
        }

        u64 getValue() const
        {
            ITF_ASSERT(m_index < Max_TLS_Slots);
            return reinterpret_cast<u64>(ms_threadLocalStorage[m_index]);
        }

        void setNULL()
        {
            ITF_ASSERT(m_index < Max_TLS_Slots);
            ms_threadLocalStorage[m_index] = nullptr;
        }

        void setValue(void* _pValue)
        {
            setValue(reinterpret_cast<u64>(_pValue));
        }

        void setValue(u64 _value)
        {
            ITF_ASSERT(m_index < Max_TLS_Slots);
            ms_threadLocalStorage[m_index] = reinterpret_cast<void*>(_value);
        }

    private:
        u32	m_index = acquireIndex();
    };

    // thread utilities

    void sleep(u32 uMilliseconds);
    void YieldProcessor();


    struct FuncParameter
    {
    public:
        using PThreadFunc = u32(*)(void*);
        PThreadFunc m_pThreadFunc;
        void* m_pThreadFuncParam;
        void* m_pThread;
        u32 m_engineIndex;
    };

    class ThreadSettings;

    class Thread final
    {
    public:
        Thread() = default;

        Thread(const Thread&) = delete;
        Thread& operator = (const Thread&) = delete;

        void                    create(FuncParameter::PThreadFunc pThreadFunc, void* pThreadFuncParam, const ThreadSettings& _settings);

        static u32              getPriorityHW(Thread_Priority ePriority);
        static u64              getAffinityHW(Thread_HardwareThreadId eAffinity);

        void                    setAffinity(Thread_HardwareThreadId eThreadId);
        Thread_HardwareThreadId getAffinity()  const { return m_eThreadAffinity; }

        Thread_Priority         getPriority() const { return m_ePriority; }

        const ThreadSettings* getSettings() const { return &m_settings; }

    private:
        ScePthread              m_handle = nullptr;

        FuncParameter           m_Parameter{};
        Thread_Priority         m_ePriority = Thread_Priority_Normal;
        Thread_HardwareThreadId m_eThreadAffinity = static_cast<Thread_HardwareThreadId>(-1);

        void                    setSettings(const ThreadSettings& _settings) { m_settings = _settings; }
        ThreadSettings          m_settings{};
    };

} // namespace ITF

#endif // ITF_THREAD_PS5_H_
