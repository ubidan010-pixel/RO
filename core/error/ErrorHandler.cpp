#include "precompiled_core.h"

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifdef ITF_WINDOWS
    #include <process.h>
#endif // ITF_WINDOWS

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifdef ITF_NINTENDO
#include <nn/nn_Log.h>
#endif

#include "core/UnicodeTools.h"

#ifndef _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 0
#endif

namespace ITF
{
#ifndef ITF_FINAL
    static const u32 ErrorBufferSize = 131072; // buffer is pre-allocated to avoir huge stack allocations deep in call stack
    char error_str_buffer[ErrorBufferSize];
#endif 

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///                                PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

ErrorHandler::ErrorHandler()
{
    Synchronize::createCriticalSection(&m_csLog);
}

ErrorHandler::~ErrorHandler()
{
#ifdef ITF_WINDOWS
    {
        csAutoLock cs(m_csLog);
        if(m_LogFile != NULL)
        {
            ::fclose(m_LogFile);
        }
    }

    Synchronize::destroyCriticalSection(&m_csLog);
#endif //ITF_WINDOWS
}

void ErrorHandler::setDumpFile(const char* _szDumpFileName)
{
#if ITF_WINDOWS
   ::fopen_s(&m_LogFile, _szDumpFileName, "wt");
#endif //ITF_WINDOWS
}


/*
    Raises an error.
    @param    _szFormat    printf - like format
    @throws ITF_ASSERT    if unhandled tag is met in _szFormat
*/

void ErrorHandler::FatalError(const char* _szFormat, ...)
{

    csAutoLock cs(m_csLog);
#ifndef ITF_FINAL
    va_list args;
    va_start(args, _szFormat);
    MSG_Reset();

    char *buffer = error_str_buffer;

#if _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
    if(::_vsprintf_s_l(buffer, ErrorBufferSize, _szFormat, NULL, args) >= 0)
#else
    if(::vsprintf(buffer, _szFormat, args) >= 0)
#endif
    {
#ifdef ITF_WINDOWS
        HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleTextAttribute(hConsole, 0x0c);
#endif
        MSG_AddString(buffer);
        MSG_Display(btrue);
    }
    va_end(args);

    OutputDebugger dbg;
    dbg << buffer << "\n";
    
    ITF_ASSERT(SYSTEM_ADAPTER);
    
    if(SYSTEM_ADAPTER)
	{
		if(SYSTEM_ADAPTER->IsSilent() && m_FatalErrorCB != nullptr)
            m_FatalErrorCB(buffer);
		else
			SYSTEM_ADAPTER->messageBox("FATAL ERROR", m_szMessage, ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDYES);
	}

#ifdef ITF_WINDOWS
    ITF_ASSERT_MSG(0, "Ready to launch showLog.bat, maybe you want to break in the code before ?");
    if(m_LogFile != NULL)
        ::fclose(m_LogFile);
    _execl("showLog.bat", "showLog.bat", NULL );
#endif

    //also assert
    ITF_ASSERT_MSG_FILTER(bfalse, AssertManager::m_FlagWarningAndError, "%s", buffer);

#ifdef ITF_X360
#ifndef ITF_FINAL
    if ( ISDEBUGGER_PRESENT)
    {
        DebugBreak();
    }
#endif //ITF_FINAL
    XLaunchNewImage( "", 0 );
#endif //ITF_X360

#else
    NOT_IMPLEMENTED_ON_PS3
#endif //ITF_FINAL
}

/*
Raises an error.
@param    _szFormat    printf - like format
@throws ITF_ASSERT    if unhandled tag is met in _szFormat
*/
void ErrorHandler::Error(bbool _showMessage, const char* _szFormat, ...)
{
#ifndef ITF_FINAL
    csAutoLock cs(m_csLog);
    MSG_Reset();

    va_list args;
    va_start(args, _szFormat);

    char *buffer = error_str_buffer;

#if _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
    if(::_vsprintf_s_l(buffer, ErrorBufferSize, _szFormat, NULL, args) >= 0)
#else
    if(::vsprintf(buffer, _szFormat, args) >= 0)
#endif
    {
#ifdef ITF_WINDOWS
        HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleTextAttribute(hConsole, 0x0c);
#endif 
        MSG_AddString(buffer);
        MSG_Display(btrue);
    }
    va_end(args);

    OutputDebugger dbg;
    dbg << buffer << "\n";

#ifdef ITF_WINDOWS
    ITF_ASSERT(SYSTEM_ADAPTER);
    if(SYSTEM_ADAPTER && _showMessage)
	{
		if(SYSTEM_ADAPTER->IsSilent() && m_ErrorCB)
		{
            m_ErrorCB(buffer);
		}		
        SYSTEM_ADAPTER->messageBox("ERROR", m_szMessage, ITF_MSG_ICONERROR | ITF_MSG_OK,ITF_IDYES);
	}
    if (m_LogFile)
        ::fflush(m_LogFile);
#endif

    //also assert
    ITF_ASSERT_MSG_FILTER(bfalse, AssertManager::m_FlagWarningAndError, "%s", buffer);

#else
    NOT_IMPLEMENTED_ON_PS3
#endif //ITF_FINAL

}
/*
    Logs an error (non blocking, while Raise() is blocking).
    @param    _szFormat    printf - like format.
    @throws ITF_ASSERT if unhandled tag is met in _szFormat
*/
void ErrorHandler::Log(const char* _szFormat, ...)
{
#ifndef ITF_FINAL
    csAutoLock cs(m_csLog);
    MSG_Reset();

    va_list args;
    va_start(args, _szFormat);

    char *buffer = error_str_buffer;

#if _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
    if (::vsprintf_s(buffer, ErrorBufferSize, _szFormat, args)>=0)
#else
    if (::vsprintf(buffer, _szFormat, args)>=0)
#endif
    {
#ifdef ITF_WINDOWS
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 0x0f);
        MSG_AddString(buffer);
        MSG_Display();
#endif
    }
    va_end(args);

    OutputDebugger dbg;
    dbg << buffer << "\n";

#endif //ITF_FINAL
}

void    ErrorHandler::LogFromPlugin ( const String&  _szExeName,   const String& _szString  )
{
    if (m_LogWithPluginCB)
        m_LogWithPluginCB(_szExeName, _szString);
}


/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///                                PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/*
    Resets the current error message.
*/
void ErrorHandler::MSG_Reset()
{
    m_szMessage.clear();
}

/*
    Adds a string to the current message.

    @param _szString    The string.
    @throws    error    if the message's size exceeds MAX_ERROR_MESSAGE_SIZE
*/
void ErrorHandler::MSG_AddString(const String& _szString)
{
    m_szMessage += _szString;

    if (m_LogCB)
        m_LogCB(_szString);
}

/*
    Displays the current error message depending on the operating system.
    @throws ITF_ASSERT if the operating system was not properly set.
*/
//TODO swith this one to a thread safe way
void ErrorHandler::MSG_Display(bbool _toErrorStream)
{
#if defined(ITF_NINTENDO)
    NN_LOG("%s\n", StringToUTF8(m_szMessage).get());
#else
    if (_toErrorStream)
    {
    #ifdef ITF_WCHAR_IS_16BIT
        ::fprintf(stderr, "%ls\n", reinterpret_cast<const wchar_t*>(m_szMessage.cStr()));
    #else
        ::fprintf(stderr, "%s\n", UTF16ToUTF8(m_szMessage.cStr()).get());
    #endif
    }
#endif

    if (m_LogWithPluginCB)
        m_LogWithPluginCB(SystemAdapter::getModuleName(), m_szMessage);
}

#if defined (ITF_WINDOWS) && !defined(ITF_FINAL)
void ErrorHandler::LogCooker(const char* _szFormat, ...)
{
    csAutoLock cs(m_csLog);
    MSG_Reset();

    va_list args;
    va_start(args, _szFormat);

    char *buffer = error_str_buffer;

#if _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
    if (::vsprintf_s(buffer, ErrorBufferSize, _szFormat, args)>=0)
#else
    if (::vsprintf(buffer, _szFormat, args)>=0)
#endif
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 0x0f);
        MSG_AddString(buffer);
        
        if (m_LogCookerCB)
            m_LogCookerCB(buffer);

        MSG_Display();
    }
    va_end(args);

    OutputDebugger dbg;
    dbg << buffer << "\n";

}

#endif //defined (ITF_WINDOWS) && !defined(ITF_FINAL)

} // namespace ITF
