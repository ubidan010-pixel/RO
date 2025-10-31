#include "precompiled_core.h"

#if defined(ITF_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "windows.h"
#endif //ITF_WINDOWS

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#if !defined(ITF_CONSOLE)

namespace ITF
{
    const u32 BUFFER_SIZE = 4096;

    const String& ProcessSpawner::getLog() const
    {
         return m_LogText;
    }



    ProcessSpawner::~ProcessSpawner()
    {
        if (m_piProcessInfo.hProcess)
            CloseHandle(m_piProcessInfo.hProcess); 
 
        if (m_piProcessInfo.hThread)
            CloseHandle(m_piProcessInfo.hThread); 
    }

    bbool ProcessSpawner::poll()
    {
        DWORD exitcode;
#ifdef ASSERT_ENABLED
        BOOL res = 
#endif
            GetExitCodeProcess(m_piProcessInfo.hProcess,&exitcode);
        ITF_ASSERT(res);

        return (exitcode == STILL_ACTIVE);
    }

    void ProcessSpawner::terminate()
    {
        TerminateProcess(m_piProcessInfo.hProcess,0);

        CloseHandle(m_piProcessInfo.hProcess); 
        CloseHandle(m_piProcessInfo.hThread); 

        m_piProcessInfo.hProcess = NULL;
        m_piProcessInfo.hThread  = NULL;
    }

    i32 ProcessSpawner::create(const String& applicationName,const String& Parameters,const String& logFile,bbool bAsynchronous,bbool bMaximizeWindow)
    {
        DWORD dwExitCode = 0; 
        String sTempStr; 

        m_bFinished = bfalse;
        bbool hasLogFile = logFile.getLen()>0;

        /* The first parameter needs to be the executable itself */ 
        sTempStr = applicationName;
        sTempStr += "  ";
        sTempStr += Parameters; 
        HANDLE out = NULL;
        /* CreateProcess can modify Parameters thus we allocate needed memory */ 
        StringConverter str(sTempStr);
        char * pwszParam = new char[str.getCharLength() + 1]; 
        if (pwszParam == 0) 
        { 
            SF_DEL_ARRAY(pwszParam);
            return 1; 
        } 
        const char* pchrTemp = str.getChar(); 
        strcpy(pwszParam, pchrTemp); 
        SECURITY_ATTRIBUTES sap,sat,sao;
        //sec attributes for the output file
        sao.nLength=sizeof(SECURITY_ATTRIBUTES);
        sao.lpSecurityDescriptor=NULL;
        sao.bInheritHandle=1;

        /* CreateProcess API initialization */ 
        STARTUPINFOA siStartupInfo; 
        ITF_MemSet(&siStartupInfo, 0, sizeof(siStartupInfo)); 
        ITF_MemSet(&m_piProcessInfo, 0, sizeof(m_piProcessInfo)); 
        siStartupInfo.cb = sizeof(siStartupInfo); 
        siStartupInfo.dwFlags       = STARTF_USESHOWWINDOW;
        
        if (bMaximizeWindow)
        {
            siStartupInfo.wShowWindow   = SW_SHOWMAXIMIZED|SW_MAXIMIZE;
        }

        //delete the file to avoid any confusion
        DeleteFile((LPCWSTR)logFile.cStr());

        if (hasLogFile)
        {
            //open the output file on the server's tmp folder (for that test will be on the C:/ root)
            if((out=CreateFile((LPCWSTR)logFile.cStr(),GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
                &sao,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
            {
                return bfalse;
            }

            siStartupInfo.dwFlags|=STARTF_USESTDHANDLES;
            siStartupInfo.hStdOutput=out;//for writing
            siStartupInfo.hStdError=out;//for the errors, to the same file
        }

        //proc sec attributes
        sap.nLength=sizeof(SECURITY_ATTRIBUTES);
        sap.lpSecurityDescriptor=NULL;
        sap.bInheritHandle=1;
        
        //thread sec attributes
        sat.nLength=sizeof(SECURITY_ATTRIBUTES);
        sat.lpSecurityDescriptor=NULL;
        sat.bInheritHandle=1;

        DWORD lpExitCode = U32_INVALID;

        if (CreateProcessA(NULL, 
            pwszParam, &sap, &sat, btrue, 
            0   , 0, 0, //CREATE_DEFAULT_ERROR_MODE
            &siStartupInfo, &m_piProcessInfo) != bfalse) 
        { 
            /* Watch the process. */ 
            if (!bAsynchronous)
            {
                dwExitCode = WaitForSingleObject(m_piProcessInfo.hProcess, INFINITE); 
                m_bFinished = btrue;
                GetExitCodeProcess(m_piProcessInfo.hProcess,&lpExitCode);
            }
            
        } 
        else 
        { 
            /* CreateProcess failed */ 
            lpExitCode = GetLastError();
            String8 errorMsg = "CreateProcess failed with command: ";
            errorMsg += pwszParam;
            ITF_ASSERT_MSG(0, errorMsg.cStr());
        }

        /* Free memory */ 
        SF_DEL_ARRAY(pwszParam);

        /* Release handles */ 
         if (!bAsynchronous)
         {
             CloseHandle(m_piProcessInfo.hProcess); 
             CloseHandle(m_piProcessInfo.hThread); 

             m_piProcessInfo.hProcess = NULL;
             m_piProcessInfo.hThread  = NULL;
         }

        if (hasLogFile)
        {
            CloseHandle(out);

            HANDLE outputFile;

            //open the output file on the server's tmp folder (for that test will be on the C:/ root)
            if((outputFile=CreateFile((LPCWSTR)logFile.cStr(),GENERIC_READ,FILE_SHARE_READ,
                &sao, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
            {
                return bfalse;
            }
            else
            {
                char buffer[BUFFER_SIZE + 1];
                DWORD readByte = 1;
                while (readByte)
                {
                    ReadFile(outputFile,buffer,BUFFER_SIZE,&readByte,NULL);
                    buffer[readByte] = '\0';
                    m_LogText += buffer;
                }
            }
 
            CloseHandle(outputFile);
            
        }
        return lpExitCode; 


    }
    
} // namespace ITF


#endif // !ITF_CONSOLE
