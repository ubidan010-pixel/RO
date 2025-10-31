#include "precompiled_core.h"

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#if defined(EXPORT_ASSERTS)

    // #include "curl/curl.h"

    #ifndef _ITF_CONFIG_H_
    #include "core/Config.h"
    #endif //_ITF_CONFIG_H_

    #ifndef _ITF_THREADALLOCATIONTRACKER_H_
    #include "core/memory/threadAllocatorStacker.h"
    #endif //_ITF_THREADALLOCATIONTRACKER_H_

#endif // EXPORT_ASSERTS

namespace ITF
{
#if defined(EXPORT_ASSERTS)

    char AssertManager::assertServerInsertPage[512] = "";

    void AssertManager::init()
    {
        // Not thread safe
        /*CURLcode res = */ //curl_global_init(CURL_GLOBAL_ALL);
    }

    void AssertManager::cleanup()
    {
        // Not thread safe
        //curl_global_cleanup();
    }

    /*
    void escapeString(CURL *_curl, const char* _str, String8& _output)
    {
        char *s = curl_easy_escape(_curl,_str, strlen(_str));
        _output = s;
        curl_free(s);
    }

    size_t dummyWriteFunction( void *ptr, size_t size, size_t nmemb, void *stream)
    {
        return size*nmemb;
    }
    */
    
    bool AssertManager::exportAssertToWebStatPage(const char* _file, const int _line, const char* _assertion, const char* _message, ... )
    {
        bool skipNextExportsForThisAssert = false;

        if(strlen(assertServerInsertPage) > 0)
        {
            String8 request, file, assertion, message, callstack;

            /*
            CURL *curl = curl_easy_init();
            if(curl)
            {
                String8 callstack_custom;
 
                #define MAX_STACK_DEPTH 40

                u32 stack[MAX_STACK_DEPTH];
                ITF_MemSet(stack, 0, sizeof(stack));
               
                static StackWalker SW;
                SW.initialize();
                const u8 getCount = SW.retrieveStackFast(2, stack ,MAX_STACK_DEPTH);
                ProcessAddressDescriptor desc;

                const char *lastBackslash =strrchr(_file, '\\');
                if (lastBackslash)
                    _file = lastBackslash+1;
                const char *lastSlash = strrchr(_file, '/');
                if (lastSlash)
                    _file = lastSlash+1;


                for(u32 i = 0; i < getCount; ++i)
                {
                    u64 addr = stack[i];
                    if (addr)
                    {
                        SW.fillDescriptor(addr,desc);
                        if (desc.m_line > 0)
                        {
                            if(i != 0)
                                callstack_custom += "(...)|";
                            callstack_custom += desc.m_symbolName;
                        }
                    }
                }

                escapeString(curl, callstack_custom.cStr(), callstack);
                escapeString(curl, _file, file);
                if(_message)
                {
                    static const int tmpSIZE = 4096;
                    char buffer[tmpSIZE];

                    va_list list;
                    va_start(list,_message);
                    vsnprintf_s(buffer, tmpSIZE, tmpSIZE, _message, list);
                    va_end(list);

                    escapeString(curl, buffer, message);
                }
                escapeString(curl, _assertion, assertion);

                request.setTextFormat("%s?%s=%s&%s=%s&%s=%i&%s=%s&%s=%s&%s=%u"
                    , assertServerInsertPage
                    , "assertion", assertion.cStr()
                    , "file", file.cStr()
                    , "line", _line
                    , "message", message.cStr()
                    , "callstack", callstack.cStr()
                    , "CL", Versioning::getEngineVersion()
                    );

                curl_easy_setopt(curl, CURLOPT_URL, request.cStr());
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE); // Disable cert check
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummyWriteFunction);

                // Blocking call
                // Maybe we should put this in a separate thread
                CURLcode res = curl_easy_perform(curl);

                skipNextExportsForThisAssert = (res == CURLE_OK);
            }
            curl_easy_cleanup(curl);
            */
        }
        else
        {
            skipNextExportsForThisAssert = true;
        }

        return skipNextExportsForThisAssert;
    }

#endif // EXPORT_ASSERTS

#if !defined(ROBOT_COMPILATION) // Enable all asserts by default for developpers
    FlagType AssertManager::m_bAssertEnabled = AssertManager::m_FlagAll;
#else // ROBOT_COMPILATION
    FlagType AssertManager::m_bAssertEnabled = 0;
#endif // ROBOT_COMPILATION

    AssertManager::Choice AssertManager::CallAssert( const char * condition, const char * file, int line, const char * message, ... )
    {
        static bool reentrancyTest = bfalse;
        if(reentrancyTest)
            return Break;
        reentrancyTest = btrue;

        AssertManager::Choice resultChoice = Break;

        //!! Assertion is not Threadsafe;
        static const unsigned int tmpSIZE = 4096;
        static char tmpID[tmpSIZE];

    #ifdef ITF_PS5
        const char buttonSelectMsg[] = "Choose \"Yes\" to break, \"No\" to ignore once or O button to ignore always";
    #elif defined(ITF_NINTENDO)
        const char buttonSelectMsg[] = "Choose Y to break, A to ignore once or B button to ignore always";
    #else
        const char buttonSelectMsg[] = "Choose \"Yes\" to break, \"No\" to ignore once or \"Cancel\" to ignore always";
    #endif

        if(message)
        {
            char buffer[tmpSIZE];

            va_list list;
            va_start(list,message);
            vsnprintf(buffer, tmpSIZE-1, message, list);
            va_end(list);
            snprintf(tmpID, tmpSIZE-1, "File: %s(%i)\n\nMessage: \"%s\"\n\nCondition: \"%s\"\n\n%s", file, line, buffer, condition, buttonSelectMsg);
            tmpID[tmpSIZE-1] = 0;
        }
        else
        {
            snprintf(tmpID, tmpSIZE-1, "File: %s(%i)\n\nCondition: \"%s\"\n\n%s", file, line, condition, buttonSelectMsg);
            tmpID[tmpSIZE-1] = 0;
        }

        u32 result = 0;

        if (SYSTEM_ADAPTER)
        {
            if(SYSTEM_ADAPTER->IsSilent())
            {
                OutputDebugMsg("Assert :\n");
                OutputDebugMsg(tmpID);
                OutputDebugMsg("\nSilent mode active, ignore always\n");
                reentrancyTest = bfalse;
                return IgnoreAlways;
            }

            result = SYSTEM_ADAPTER->messageBox("Assert", tmpID, ITF_MSG_ICONERROR | ITF_MSG_YESNOCANCEL,ITF_IDCANCEL);
            switch(result)
            {
                case ITF_IDYES:
                    resultChoice = Break;
                    break;

                case ITF_IDNO:
                    resultChoice = IgnoreOnce;
                    break;

                case ITF_IDCANCEL:
                    resultChoice = IgnoreAlways;
                    break;

                default:
                    resultChoice = Break;
                    break;
            }

            reentrancyTest = bfalse;
            return resultChoice;
        }

    //keep it in case something happens very early at boot
    #ifdef ITF_WINDOWS
        switch(MessageBoxA(NULL, tmpID, "Assert", MB_ICONERROR | MB_YESNOCANCEL))
        {
            case IDYES:
                resultChoice = Break;
                break;

            case IDNO:
                resultChoice = IgnoreOnce;
                break;

            case IDCANCEL:
                resultChoice = IgnoreAlways;
                break;

            default:
                resultChoice = Break;
                break;
        }
    #else
        resultChoice = Break;
    #endif  //ITF_WINDOWS

        reentrancyTest = bfalse;
        return resultChoice;
    }
    
    void AssertManager::displayAssert( const char * condition, const char * file, int line, const char * message, ... )
    {
        //!! Assertion is not Threadsafe;
        static const unsigned int tmpSIZE = 512;
        static char tmpID[tmpSIZE];

        if ( message )
        {
            char buffer[tmpSIZE];

            va_list list;
            va_start(list,message);
            vsnprintf(buffer, tmpSIZE-1, message, list);
            va_end(list);
            
            SPRINTF_S(tmpID, tmpSIZE - 1, "File: %s(%i)\n\nMessage: \"%s\"\n\nCondition: \"%s\"", file, line, buffer, condition);
        }
        else
        {
            SPRINTF_S(tmpID, tmpSIZE - 1, "File: %s(%i)\n\nCondition: \"%s\"", file, line, condition);
        }

        if (SYSTEM_ADAPTER)
        {
            SYSTEM_ADAPTER->messageBox( "Assert",tmpID, ITF_MSG_ICONERROR | ITF_MSG_OK,ITF_IDYES);
            return;
        }

    #ifdef ITF_WINDOWS //keep it in case something happens very early at boot (SYSTEM_ADAPTER not available)
        MessageBoxA(NULL, tmpID, "Assert", MB_ICONERROR | ITF_MSG_OK);
    #endif  //ITF_WINDOWS
    }

    void AssertManager::OutputDebugMsg( const char * _str)
    {
    #if !defined(ITF_FINAL)
        #if defined(ITF_WINDOWS) || defined(ITF_X360)
            OutputDebugStringA(_str);
        #else
            fprintf(stderr, "%s", _str);
        #endif
    #endif
    }

    void AssertManager::OutputDebugMsg( const char * condition, const char * file, int line, int skipCount, const char * message, ... )
    {
    #if defined(_DEBUG) && !defined(WRAPPER_C)
        if (skipCount < 10000)
        {
            static const unsigned int tmpSIZE = 4096;
            char temp[tmpSIZE];

            if ( message )
            {
                char buffer[tmpSIZE];
                va_list list;
                va_start(list,message);
                SPRINTF_S_TRUNC(buffer,tmpSIZE,message,list);

                va_end(list);
                
                SPRINTF_S(temp, tmpSIZE, "%s(%i) - ASSERT : %i time(s)\n\tMessage: \"%s\"\n\tCondition: \"%s\"\n", file, line, skipCount, buffer, condition);
            }
            else
            {
                SPRINTF_S(temp, tmpSIZE, "%s(%i) - ASSERT : %i time(s)\n\tCondition: \"%s\"\n", file, line, skipCount, condition);
            }
            OutputDebugMsg(temp);
        }
    #endif
    }

} // namespace ITF
