#ifndef ITF_AUTOLOCK_H_
#define ITF_AUTOLOCK_H_


namespace ITF
{
    class csAutoLock
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn ITF_INLINE csAutoLock(ITF_THREAD_CRITICAL_SECTION *tcs) : p_tcs(tcs)
        /// @brief  Constructor for pointed object. 
        /// @param [in,out] tcs If non-null, the tcs. 
        /// @return . 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ITF_INLINE csAutoLock(ITF_THREAD_CRITICAL_SECTION *tcs) :
        p_tcs(tcs)
        {
            if(p_tcs != (void *) 0)
            {
                Synchronize::enterCriticalSection(p_tcs);
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn ITF_INLINE csAutoLock(ITF_THREAD_CRITICAL_SECTION &tcs) : p_tcs(&tcs)
        /// @brief  Constructor for referenced object. 
         /// @param [in,out] tcs the tcs. 
        /// @return . 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ITF_INLINE csAutoLock(ITF_THREAD_CRITICAL_SECTION &tcs) :
        p_tcs(&tcs)
        {
            Synchronize::enterCriticalSection(p_tcs);
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn ITF_INLINE ~csAutoLock()
        /// @brief  Destructor. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ITF_INLINE virtual ~csAutoLock()
        {
            if(p_tcs != (void *) 0)
            {
                Synchronize::leaveCriticalSection(p_tcs);
            };
        };

    private:
        ITF_THREAD_CRITICAL_SECTION *p_tcs;
    };

    // same as above but to unlock in the scope
    class    csAutoUnlock
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn ITF_INLINE csAutoUnlock(ITF_THREAD_CRITICAL_SECTION *tcs) : p_tcs(tcs)
        /// @brief  Constructor for pointed object. 
        /// @param [in,out] tcs If non-null, the tcs. 
        /// @return . 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ITF_INLINE csAutoUnlock(ITF_THREAD_CRITICAL_SECTION *tcs) :
          p_tcs(tcs)
          {
              if(p_tcs != (void *) 0)
              {
                  Synchronize::leaveCriticalSection(p_tcs);
              };
          };

          ////////////////////////////////////////////////////////////////////////////////////////////////////
          /// @fn ITF_INLINE csAutoUnlock(ITF_THREAD_CRITICAL_SECTION &tcs) : p_tcs(&tcs)
          /// @brief  Constructor for referenced object. 
          /// @param [in,out] tcs the tcs. 
          /// @return . 
          ////////////////////////////////////////////////////////////////////////////////////////////////////

          ITF_INLINE csAutoUnlock(ITF_THREAD_CRITICAL_SECTION &tcs) :
          p_tcs(&tcs)
          {
              Synchronize::leaveCriticalSection(p_tcs);
          };

          ////////////////////////////////////////////////////////////////////////////////////////////////////
          /// @fn ITF_INLINE ~csAutoLock()
          /// @brief  Destructor. 
          ////////////////////////////////////////////////////////////////////////////////////////////////////

          ITF_INLINE virtual ~csAutoUnlock()
          {
              if(p_tcs != (void *) 0)
              {
                  Synchronize::enterCriticalSection(p_tcs);
              };
          };

    private:
        ITF_THREAD_CRITICAL_SECTION *p_tcs;
    };

}

#endif // ITF_AUTOLOCK_H_

