#include "precompiled_core.h"

#ifndef _INC_IO
    #include <io.h>
#endif // _INC_IO

#ifndef _INC_ERRNO
    #include <errno.h>
#endif // _INC_ERRNO

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_DIRECTORY_H_
#include  "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_ 

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#include <strsafe.h>

//ENABLE THIS FLAGS TO BREAK MULTIPLE ACCESS
//#define TRACK_FILE_VIOLATION 1

namespace ITF
{
    hwFile::hwFile()
    {
        m_Handle = INVALID_HANDLE_VALUE;
    }
    
    hwFile::~hwFile()
    {
       if (m_Handle!=INVALID_HANDLE_VALUE)
            close();
    }

    void LogLastError(LPTSTR lpszFunction) 
    { 
        // Retrieve the system error message for the last-error code

        return ;
    /*    LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
            (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
        StringCchPrintf((LPTSTR)lpDisplayBuf, 
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"), 
            lpszFunction, dw, lpMsgBuf); 
        String8 mess((ITF::u16*)lpDisplayBuf);
        LOG(mess.cStr());
        ITF_ASSERT_MSG(0, mess.cStr());
        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
        */
    }

    bbool hwFile::open(const String& _absolutePath, u32 attribute,u32 _flagsAttribute)
    {
        ITF_ASSERT_MSG(_absolutePath.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _absolutePath.cStr());

        DWORD access = 0;
        DWORD creation = 0;
        DWORD dwShareMode = 0;
        DWORD flagsAttribute = FILE_ATTRIBUTE_NORMAL;
        DWORD dwRetries = 0;
        bbool bSuccess  = bfalse;

        #define MAXRETRIES  5
        #define RETRYDELAY  250

        dwShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;

        if (attribute & ITF_FILE_ATTR_READ)         {access |= GENERIC_READ;}
        if (attribute & ITF_FILE_ATTR_WRITE)       
        {
            access |= GENERIC_WRITE;
            //get the directory and try to make it
            Directory::create(FilePath::getDirectory(_absolutePath));
        
        }
        if (attribute & ITF_FILE_ATTR_CREATE_NEW)   creation |= CREATE_ALWAYS;
        else                                        creation |= OPEN_EXISTING;
        
        //we need to prevent failure in case of ERROR_SHARING_VIOLATION
        //we retry MAXRETRIES for RETRYDELAY ms to see if any process have released the lock...
        do
        {
            m_Handle = CreateFile((LPCWSTR)_absolutePath.cStr(), access, dwShareMode, NULL, creation, flagsAttribute, NULL);
            if (m_Handle == INVALID_HANDLE_VALUE)  
            {
                DWORD   dwErr     = 0;
                dwErr = GetLastError();

                if ( ERROR_SHARING_VIOLATION == dwErr )
                {
                    dwRetries += 1;
                    #ifdef TRACK_FILE_VIOLATION
                    ITF_ASSERT(0);
                    #endif //TRACK_FILE_VIOLATION
                    LOG("RETRY ERROR_SHARING_VIOLATION %S %d\n", _absolutePath.cStr(), dwRetries);
                    sleep(RETRYDELAY);
                    continue;
                }
                else
                {
                    LogLastError((LPTSTR)_absolutePath.cStr());
                // An error occurred.
                    return bfalse;
                }

             //  unreachable code m_Handle = INVALID_HANDLE_VALUE;

              // unreachable code  return bfalse;
            }

            bSuccess = btrue;
            break;

        } while ( dwRetries < MAXRETRIES );

        
#ifdef ENABLE_FILE_REGISTERING
        if (bSuccess)
            FILEMANAGER->registerFile(_absolutePath, uPtr(this));
#endif //ENABLE_FILE_REGISTERING

        return btrue;
    }
        
    void hwFile::close()
    {
        if (m_Handle!=INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_Handle);

#ifdef ENABLE_FILE_REGISTERING
            FILEMANAGER->unRegisterFile(uPtr(this));
#endif //ENABLE_FILE_REGISTERING
        }
        m_Handle = INVALID_HANDLE_VALUE;
    }

    u64 hwFile::getLength()
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        return (u64)GetFileSize(m_Handle, NULL);
    }

    
    bbool hwFile::read(void *pBuffer, u32 size,u32* sizeRead)
    {
        SIMULATE_TIME

        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        return ReadFile(m_Handle, pBuffer, size, (DWORD*) sizeRead, NULL);
    }

    bbool hwFile::read(void *pBuffer, u32 size)
    {
        SIMULATE_TIME

        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        DWORD sizeRead;
        return ReadFile(m_Handle, pBuffer, size, (DWORD*) &sizeRead, NULL);
    }

    bbool hwFile::write(const void *pBuffer, u32 size,u32* sizeWritten)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        return WriteFile(m_Handle, pBuffer, size, (DWORD*) sizeWritten, NULL);
    }

    bbool hwFile::write(const void *pBuffer, u32 size)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        DWORD sizeWritten;
        return WriteFile(m_Handle, pBuffer, size, (DWORD*) &sizeWritten, NULL);
    }

    u64 hwFile::seek(i64 offset, u16 from)
    {
        if(m_Handle==INVALID_HANDLE_VALUE)
            return 0;

        LARGE_INTEGER li;
        li.QuadPart = offset;
        LARGE_INTEGER newPos;
        SetFilePointerEx(m_Handle, li, &newPos, from); 
        return newPos.QuadPart;
    }

    bbool hwFile::CopyFile(const String& szExistingFilename,const String& szNewFilename)
    {
        ITF_ASSERT_MSG(szExistingFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", szExistingFilename.cStr());
        ITF_ASSERT_MSG(szNewFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", szNewFilename.cStr());
        return ::CopyFile( LPCWSTR(szExistingFilename.cStr()), LPCWSTR(szNewFilename.cStr()), 0);
    }

    bbool hwFile::MoveFile(const String& szExistingFilename,const String& szNewFilename)
    {
        ITF_ASSERT_MSG(szExistingFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", szExistingFilename.cStr());
        ITF_ASSERT_MSG(szNewFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", szNewFilename.cStr());
        return ::MoveFile( LPCWSTR(szExistingFilename.cStr()), LPCWSTR(szNewFilename.cStr()));
    }

    u64  hwFile::getLastTimeWriteAccess(const String& _szExistingFilename)
    {
        ITF_ASSERT_MSG(_szExistingFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _szExistingFilename.cStr());
        if (!hwFile::exists(_szExistingFilename))
            return 0;

        hwFile file;
        if (file.open(_szExistingFilename, ITF_FILE_ATTR_READ) == btrue)
        {
            FILETIME dum1,dum2, lastWrite;
            
            BOOL res = GetFileTime(file.getHandle(),&dum1,&dum2,&lastWrite);
            if (!res)
                return 0;

            return convertTimestamp(lastWrite);
        }

        return 0;
    }

    u64 hwFile::getLastTimeWriteAccess()
    {
        FILETIME dum1,dum2, lastWrite;

        BOOL res = GetFileTime(getHandle(),&dum1,&dum2,&lastWrite);
        if (!res)
            return 0;

        return convertTimestamp(lastWrite);
    }

    void hwFile::flushTimeWriteAccess(const String& _szExistingFilename,const u64& _lastWrite)
    {
        ITF_ASSERT_MSG(_szExistingFilename.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _szExistingFilename.cStr());

        // change time whatever is the attribute
        DWORD saveAttrib = ::GetFileAttributes((LPCWSTR)_szExistingFilename.cStr());
        if (saveAttrib != DWORD(-1))
        {
            DWORD noROAttrib = (saveAttrib & ~FILE_ATTRIBUTE_READONLY); // remove read only attrib
            ITF_VERIFY(::SetFileAttributes((LPCWSTR)_szExistingFilename.cStr(), noROAttrib));
        }

        hwFile file;
        if (file.open(_szExistingFilename, ITF_FILE_ATTR_READWRITE))
        {
            FILETIME dum1,dum2,dum3;
            if (::GetFileTime(file.getHandle(),&dum1,&dum2,&dum3))
            {
                FILETIME timeStamp = convertTimestamp(_lastWrite);
                ITF_VERIFY( ::SetFileTime(file.getHandle(),&dum1,&dum2, &timeStamp));
            }
        }

        if (saveAttrib != DWORD(-1))
        {
            ITF_VERIFY(::SetFileAttributes((LPCWSTR)_szExistingFilename.cStr(), saveAttrib));
        } 
    }

    bbool hwFile::exists(const String& _fullfileName)
    {
        ITF_ASSERT_MSG(_fullfileName.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _fullfileName.cStr());
        return (_waccess_s((const wchar_t *)_fullfileName.cStr(), 0) == 0);
    }

    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
        ITF_ASSERT_MSG(_fullfilePath.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _fullfilePath.cStr());
        int r = ::_wremove((const wchar_t*)_fullfilePath.cStr());

        if(r != 0)
        {
            String err = (const u16 *)_wcserror(errno);
            return bfalse;
        }

        return btrue;
    }

    u64 hwFile::convertTimestamp(const FILETIME& _specific)
    {
        return (u64(_specific.dwHighDateTime) << 32) | u64(_specific.dwLowDateTime);
    }

    FILETIME hwFile::convertTimestamp(u64 _generic)
    {
        FILETIME ft;
        ft.dwHighDateTime   = _generic >> 32;
        ft.dwLowDateTime    = (_generic << 32) >> 32;
        return ft;
    }

    u64 hwFile::getFileSize(const String& _fullfilePath)
    {
        ITF_ASSERT_MSG(_fullfilePath.cStr()[1] == (u16)':', "[hwFile] Parameter is not an absolute path : %ls", _fullfilePath.cStr());
        struct _stat64i32  filestatus;
        ::_wstat((const wchar_t *)_fullfilePath.cStr(), &filestatus );

        return filestatus.st_size;
    }
}
