#ifndef _ITF_SYNCHRONIZE_CTR_H_
#define _ITF_SYNCHRONIZE_CTR_H_

#include <nn/Handle.h>
#include <nn/os.h>
#include <nn/os/os_NonRecursiveCriticalSection.h>

namespace ITF
{


#define ITF_MemoryBarrier()    nn::os::DataSynchronizationBarrier()


#define ITF_THREAD_SEMAPHORE        nn::os::Semaphore
#define ITF_THREAD_EVENT            nn::os::Event
#define ITF_THREAD_ID               bit32

#define ITF_THREAD_INVALID_EVENT    nn::Handle(0)

    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_INLINE s32 InterlockedIncrement(volatile long *_pdata)
    {
        nn::os::InstructionMemoryBarrier();
        s32 r = nn::fnd::ARMv6::Interlocked::Increment((s32*)_pdata);
        nn::os::InstructionMemoryBarrier();
        return r;
    }

    ITF_INLINE s32 InterlockedDecrement(volatile long *_pdata)
    {
        nn::os::InstructionMemoryBarrier();
        s32 r = nn::fnd::ARMv6::Interlocked::Decrement((s32*)_pdata);
        nn::os::InstructionMemoryBarrier();
        return r;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    class ITF_THREAD_FASTMUTEX
    {
    public:    
        ITF_THREAD_FASTMUTEX() {}
        void lock() {m_mutex.Enter(); } 
        void unlock() {m_mutex.Leave(); }


    private:
        nn::os::NonRecursiveCriticalSection m_mutex;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
#define ITF_THREAD_CRITICAL_SECTION nn::os::CriticalSection

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Synchronize
    {
        public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static ITF_THREAD_ID getCurrentThreadId(void)
        /// @brief  Gets the current thread identifier. 
        /// @return The current thread identifier. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static ITF_THREAD_ID getCurrentThreadId(void);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName,
        ///     u32 uInitialCount= 0, u32 uMaximumCount= 666666)
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        /// @param [in,out] szName      If non-null, name of the size. 
        /// @param  uInitialCount       The initial count for the semaphore object.. 
        /// @param  uMaximumCount       The maximum count for the semaphore object. This value must be greater than zero. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount= 0, u32 uMaximumCount= 666666);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount)
        /// @brief  Increases the count of the specified semaphore object by a specified amount.
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        /// @param  uCount              The amount by which the semaphore object's current count is to be increased.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
        /// @brief  Destroys the semaphore described by pSemaphore. 
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection)
        /// @brief  Creates a critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Destroys the critical section described by pCriticalSection. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Enter critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Leave critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createEvent(ITF_THREAD_EVENT * pEvent, char* szName)
        /// @brief  Creates an event. 
        /// @param [in,out] pEvent  If non-null, the event. 
        /// @param [in,out] szName  If non-null, name of the size. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createEvent(ITF_THREAD_EVENT * pEvent, char* szName, bbool bAutoReset=bfalse);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void resetEvent(ITF_THREAD_EVENT *evt)
        /// @brief  Resets the event described by evt. 
        /// @param [in,out] evt If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void  resetEvent(ITF_THREAD_EVENT *evt);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void setEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Sets the specified event object to the signaled state.
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void setEvent(ITF_THREAD_EVENT *pEvent);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Destroys the event described by pEvent. 
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroyEvent(ITF_THREAD_EVENT *pEvent);


        static void waitEvent(ITF_THREAD_EVENT* pEvent);
    };

}

#endif // _ITF_SYNCHRONIZE_CTR_H_

