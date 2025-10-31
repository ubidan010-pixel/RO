#ifndef _ITF_PROCESSSPAWNER_H_
#define _ITF_PROCESSSPAWNER_H_

#if !defined(ITF_CONSOLE)

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Process Spawn class
    ///

    class ProcessSpawner
    {
    public:

        /// @fn ~ProcessSpawner()
        /// @brief  Finaliser. 
        ~ProcessSpawner();

        /// create a child process
        /// @param applicationName name of the pathexe
        /// @param Parameters command line passed to the application
        /// @param logFile filename for the result output
        /// @return i32 value ,0 for success otherwise the value returned by the child process
        i32     create(const String& applicationName,const String& Parameters,const String& logFile,bbool bAsynchronous = bfalse,bbool bMaximizeWindow = bfalse);

        const   String& getLog() const;

        /// @fn bbool poll()
        /// @brief  Polls the process to get its status. 
        /// @return true if still pending ,false in otherwise . 
        bbool   poll();

        /// @fn void terminate()
        /// @brief  Terminate the process. 
        void    terminate();

    protected:
        String  m_LogText;
        bbool   m_bFinished;

        PROCESS_INFORMATION m_piProcessInfo; 

    };

    
} // namespace ITF

#endif  //ITF_WINDOWS

#endif // _ITF_PROCESSSPAWNER_H_
