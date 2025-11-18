#ifndef _ITF_ERRORHANDLER_H_
#define _ITF_ERRORHANDLER_H_


#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#include "core/system/Synchronize.h"

namespace ITF
{

/*
    Simple error handling and logging class
*/

class ErrorHandler :public TemplateSingleton<ErrorHandler>
{
public:

    using CBError = void (*)(const char* _szErrortext);
    using CBLogWithPluginInfo = void (*)(const String& _szPluginname, const String& _szLogtext);
    using CBLog = void (*)(const String& _text);
    using CBLogCooker = void (*)(const String& _text);

    ErrorHandler();
    virtual ~ErrorHandler();

    void            LogFromPlugin       (  const String& _szexeName,   const String& _szString  );
    void            Log                 (    const char* _szFormat, ...   );
    void            FatalError          (    const char* _szFormat, ...   );
    void            Error               (    bbool _showMessage, const char* _szFormat, ...   );
    void            setDumpFile         (    const char* _szDumpFileName  );

#if defined (ITF_WINDOWS) && !defined(ITF_FINAL)
    void            LogCooker           (    const char* _szFormat, ...   );
#endif //defined (ITF_WINDOWS) &&  !defined(ITF_FINAL)

    void            setErrorCallback(CBError _func) { m_ErrorCB = _func; }
    CBError         getErrorCallback() const { return m_ErrorCB; }

    void            setFatalErrorCallback(CBError _func) { m_FatalErrorCB = _func; }
    CBError         getFatalErrorCallback() const { return m_FatalErrorCB; }

    void            setLogWithPluginCallback(CBLogWithPluginInfo _func) { m_LogWithPluginCB = _func; }
    CBLogWithPluginInfo getLogWithPluginCallback() { return m_LogWithPluginCB; }

    void            setLogCallback(CBLog _func) { m_LogCB = _func;}
    CBLog           getLogCallback() const { return m_LogCB; }

    void            setLogCookerCallback(CBLogCooker _func) { m_LogCookerCB = _func;}
    CBLogCooker     getLogCookerCallback() const { return m_LogCookerCB; }

private:
    void            MSG_Reset           (                                   );
    void            MSG_AddString       (    const String& _szString        );
    void            MSG_Display         (    bbool _toErrorStream=bfalse    );

    String          m_szMessage;
#ifdef ITF_WINDOWS
    FILE*           m_LogFile = nullptr;    // Used to create a log of all errors
#endif //ITF_WINDOWS

    CBError m_FatalErrorCB = nullptr;
    CBError m_ErrorCB = nullptr;
    CBLogWithPluginInfo m_LogWithPluginCB = nullptr;
    CBLog m_LogCB = nullptr;
    CBLogCooker m_LogCookerCB = nullptr;

    ITF_THREAD_CRITICAL_SECTION m_csLog;
};

#define ERROR_HANDLER ErrorHandler::getptr()

#if defined (ITF_FINAL) || defined (ITF_DISABLE_LOG)
#if defined(ITF_PS5) || defined(ITF_NINTENDO)
    #define LOG(_format, ...)                           ITF::ERROR_HANDLER->Log(_format, ## __VA_ARGS__)
#elif defined(ITF_WII)
#define LOG(...) {}
#define ITF_FATAL_ERROR(...) {}
#define ITF_ERROR(...) {}
#define ITF_ERROR_SHOW(_showMessage, ...) {}
#define LOG_COOKER(_format, ...)   {}
#else
#define LOG(_format, ...)  {}
#define ITF_FATAL_ERROR(_format, ...) {}
#define ITF_ERROR(_format, ...) {}
#define ITF_ERROR_SHOW(_showMessage, _format, ...) {}
#define LOG_COOKER(_format, ...)           {}
#endif // !defined(ITF_WII)
#else // defined(ITF_FINAL)
#if	defined(ITF_PS3) || defined(ITF_VITA) || defined(ITF_PS5) || defined(ITF_NINTENDO)
#define LOG(_format, ...)                           ITF::ERROR_HANDLER->Log( _format, ##  __VA_ARGS__)
#define ITF_FATAL_ERROR(_format, ...)               ITF::ERROR_HANDLER->FatalError(_format,## __VA_ARGS__)
#define ITF_ERROR(_format, ...)                     ITF::ERROR_HANDLER->Error(true, _format,## __VA_ARGS__)
#define ITF_ERROR_SHOW(_showMessage, _format, ...)  ITF::ERROR_HANDLER->Error(_showMessage, _format, ## __VA_ARGS__)
#elif defined(ITF_IPAD) || defined(ITF_CTR)
#define LOG(_format, ...)                           ITF::ERROR_HANDLER->Log(_format, ##  __VA_ARGS__)
#define ITF_FATAL_ERROR(_format, ...)               ITF::ERROR_HANDLER->FatalError(_format,## __VA_ARGS__)
#define ITF_ERROR(_format, ...)                     ITF::ERROR_HANDLER->Error(true, _format,## __VA_ARGS__)
#define ITF_ERROR_SHOW(_showMessage, _format, ...)  ITF::ERROR_HANDLER->Error(_showMessage, _format, ## __VA_ARGS__)
#elif defined(ITF_WII)
#define LOG(...)                           			ITF::ERROR_HANDLER->Log(__VA_ARGS__)
#define ITF_FATAL_ERROR(...)               			ITF::ERROR_HANDLER->FatalError(__VA_ARGS__)
#define ITF_ERROR(...)                     			ITF::ERROR_HANDLER->Error(true, __VA_ARGS__ )
#define ITF_ERROR_SHOW(_showMessage, ...)  			ITF::ERROR_HANDLER->Error(_showMessage, __VA_ARGS__)
#else
#define LOG(_format, ...)                           ITF::ERROR_HANDLER->Log(_format, __VA_ARGS__)
#define ITF_FATAL_ERROR(_format, ...)               ITF::ERROR_HANDLER->FatalError(_format, __VA_ARGS__)
#define ITF_ERROR(_format, ...)                     ITF::ERROR_HANDLER->Error(true, _format, __VA_ARGS__)
#define ITF_ERROR_SHOW(_showMessage, _format, ...)  ITF::ERROR_HANDLER->Error(_showMessage, _format, __VA_ARGS__)
#define LOG_COOKER(_format, ...)                    ITF::ERROR_HANDLER->LogCooker(_format, __VA_ARGS__)
#endif //defined(ITF_PS3) || defined(ITF_IPAD)
#endif //ITF_FINAL
} //namespace ITF



#endif // _ITF_ERRORHANDLER_H_
