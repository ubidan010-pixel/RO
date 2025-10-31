#ifndef _ITF_SYSTEMADAPTER_X360_H_
#define _ITF_SYSTEMADAPTER_X360_H_

#ifdef ITF_X360

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SYSTEMMESSAGELISTENER_X360_H_
#include "adapters/SystemAdapter_x360/SystemMessageListener_x360.h"
#endif

namespace ITF
{


class SystemAdapter_X360 : public SystemAdapter
{
public:    
    bbool   initialize          (   );
    bbool   openGraphics        (   int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL=btrue );
    void    terminate           (   );
    void    present             (   );
    f64     getTime             (   );
    f64     getElapsedTime      (   );
    f32     getfPs              (   );
    void    update              (   bbool _minimal  );
    bbool   getWindowSize       (   int& _width, int& _height   );

    void    timerStart          (   );
    void    timerStop           (   );

    String  userName()const;
    ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const;
    virtual String  dateTime() const;
    void    getTime(Time & _time) const;

    bbool   isTrialMode         (   );

    const char* getPlatformName() {return "x360";}
    SystemAdapter_X360    (   );
    ~SystemAdapter_X360   (   );

    LARGE_INTEGER   GetAdjustedCurrentTime();

    void  queryPerformanceCounter(LARGE_INTEGER *counter);
    void  queryPerformanceFrequency(LARGE_INTEGER *counter);

    i32 messageBox( const String& _title, const String& _msg, u32 _type);

    i32 messageBox( const String& /*_title*/, const String& /*_msg*/, u32 /*_type*/,ITF_MESSAGEBOX_RESULT /*silentResult*/ ) {return -1;}

    void *allocatePhysicalMemory(u32 _size, u32 _alignment);
    void freePhysicalMemory(void *_pointer);

#ifndef ITF_FINAL
    static void WriteStackBackTrace(const void* _userData, size_t _userDataSize);
#endif // ITF_FINAL

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// fetch System Messages
    virtual void fetchSystemMessages();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// register a SystemMessageListener_x360. It is important it's a virtual fn because of potential dependencies.
    /// @param _listener the listener to register
    virtual void registerSystemMessageListener(SystemMessageListener_x360* _listener);
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// unregister a SystemMessageListener_x360
    /// @param _listener the listener to register
    virtual void unregisterSystemMessageListener(SystemMessageListener_x360* _listener);


protected:
    i64 m_QPFTicksPerSec;
    i64 m_stopTime;
    i64 m_lastElapsedTime;
    i64 m_baseTime;
    f64 m_LastFPSTime;
    bbool m_timerStopped;
    u64 m_NumFrames;
    f64 m_fPs;

private:
    void                initMemory   (   );
    void                checkLicense();
    bbool               m_licenseChecked;


#ifndef ITF_FINAL
    static void         createDumpFile  (   );
    static void         WriteAddresses  ( ULONG numEntries, VOID** pAddresses, const void* _userData, size_t _userDataSize);

    static const CHAR*  g_filename;
    static BOOL         s_initialized;
    static BOOL         s_initTried;
    static HANDLE       hFile;
    static DWORD        writeCount;
#endif // ITF_FINAL

    mutable ITF_THREAD_CRITICAL_SECTION m_csDialog; // Critical section for dialog box

    SafeArray<SystemMessageListener_x360*> m_systemMessageListeners;
    HANDLE m_hNotification;                // listener to accept notifications
    SystemMessageListener_x360 m_messageListener;
};

} // namespace ITF

#endif //ITF_WIN32

#endif //_ITF_SYSTEMADAPTER_X360_H_