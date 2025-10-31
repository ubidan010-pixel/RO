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

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

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

    bbool hwFile::open(const String& path, u32 attribute,u32 _flagsAttribute)
    {
        DWORD access = 0;
        DWORD creation = 0;
        DWORD dwShareMode = 0;
        DWORD flagsAttribute = FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_READONLY;
        DWORD dwRetries = 0;
        bbool bSuccess  = bfalse;

        String finalPath;

        if (FILEMANAGER)
        {
            if(!FILEMANAGER->TranslatePath(finalPath, path))
            {
                ITF_ASSERT_MSG(0, "Could not translate path : '%ls'", path.cStr());
                return bfalse;
            }
        }
        else
        {
            finalPath = path;
        }


#ifdef _DEBUG
        if (FilePath::getDrive(FilePath::normalizePath(finalPath)) == "")
        {
            ITF_ASSERT(0);
        }
#endif


        #define MAXRETRIES  5
        #define RETRYDELAY  250

        dwShareMode = FILE_SHARE_READ;

        if ( _flagsAttribute != 0 )
        {
            flagsAttribute = 0;
            if ( _flagsAttribute & ITF_FILE_FLAGS_ATTR_RANDOM ) { flagsAttribute |= FILE_FLAG_RANDOM_ACCESS; }
            if ( _flagsAttribute & ITF_FILE_FLAGS_ATTR_SEQUENTIAL ) { flagsAttribute |= FILE_FLAG_SEQUENTIAL_SCAN; }
            if ( _flagsAttribute & ITF_FILE_FLAGS_ATTR_READONLY ) { flagsAttribute |= FILE_ATTRIBUTE_READONLY; }
        }
        if (attribute & ITF_FILE_ATTR_READ)         {access |= GENERIC_READ;}
        if (attribute & ITF_FILE_ATTR_WRITE)        {access |= GENERIC_WRITE;}
        if (attribute & ITF_FILE_ATTR_CREATE_NEW)   creation |= CREATE_ALWAYS;
        else                                        creation |= OPEN_EXISTING;
        
        //we need to prevent failure in case of ERROR_SHARING_VIOLATION
        //we retry MAXRETRIES for RETRYDELAY ms to see if any process have released the lock...
        do
        {
            m_Handle = CreateFile((LPCSTR)StringConverter(finalPath.cStr()).getChar(), access, dwShareMode, NULL, creation, flagsAttribute, NULL);
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
                    LOG("RETRY ERROR_SHARING_VIOLATION %ls %d\n", finalPath.cStr(), dwRetries);
                    sleep(RETRYDELAY);
                    continue;
                }
             
                return bfalse;
            }

#ifdef ENABLE_FILE_REGISTERING
            if (FILEMANAGER)
            {
                FILEMANAGER->registerFile(finalPath,(u32) this);
            }
#endif // ENABLE_FILE_REGISTERING

            bSuccess = btrue;
            break;

        } while ( dwRetries < MAXRETRIES );

        return btrue;
    }
        
    void hwFile::close()
    {
        if (m_Handle!=INVALID_HANDLE_VALUE)
        {
#ifdef ENABLE_FILE_REGISTERING
            if (FILEMANAGER)
            {
                FILEMANAGER->unRegisterFile((u32) this);
            }
#endif // ENABLE_FILE_REGISTERING
            
            CloseHandle(m_Handle);
        }
        m_Handle = INVALID_HANDLE_VALUE;
    }

    u64 hwFile::getLength()
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        return (u64)GetFileSize(m_Handle, NULL);
    }

    bbool hwFile::read(void *pBuffer, u32 size)
    {
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
    
    bbool hwFile::read(void *pBuffer, u32 size,u32* sizeRead)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;
        return ReadFile(m_Handle, pBuffer, size, (DWORD*) sizeRead, NULL);
    }

    u64 hwFile::seek(i64 offset, u16 from)
    {
        SIMULATE_TIME
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;

        LARGE_INTEGER li;
        li.QuadPart = offset;
        LARGE_INTEGER newPos;
        SetFilePointerEx(m_Handle, li, &newPos, from); 
        return newPos.QuadPart;
    }

    bbool hwFile::CopyFile(const String& szExistingFilename,const String& szNewFilename)
    {
        return ::CopyFile( LPCSTR(szExistingFilename.cStr()), LPCSTR(szNewFilename.cStr()), 0);
    }

    bbool hwFile::MoveFile(const String& szExistingFilename,const String& szNewFilename)
    {
        return ::MoveFile( LPCSTR(szExistingFilename.cStr()), LPCSTR(szNewFilename.cStr()));
    }

    u64  hwFile::getLastTimeWriteAccess(const String& _szExistingFilename)
    {
        if (!hwFile::exists(_szExistingFilename))
        return 0;

        hwFile file;
        file.open(_szExistingFilename, ITF_FILE_ATTR_READ);
        FILETIME dum1,dum2, lastWrite;

        BOOL res = GetFileTime(file.getHandle(),&dum1,&dum2,&lastWrite);
        if (!res)
            return 0;

        return convertTimestamp(lastWrite);
    }

    void hwFile::flushTimeWriteAccess(const String& _szExistingFilename,const u64& _lastWrite)
    {
        hwFile file;
        file.open(_szExistingFilename, ITF_FILE_ATTR_READWRITE);
        FILETIME dum1,dum2,dum3;
        GetFileTime(file.getHandle(),&dum1,&dum2,&dum3);
        FILETIME timeStamp = convertTimestamp(_lastWrite);
        ITF_VERIFY( SetFileTime(file.getHandle(),&dum1,&dum2, &timeStamp));    
    }

    bbool hwFile::exists(const String& _fullfileName)
    {
        return (_waccess_s((const wchar_t *)_fullfileName.cStr(), 0) == 0);
    }

    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
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
        struct _stat64i32  filestatus = {0};
        if (::_wstat((const wchar_t *)_fullfilePath.cStr(), &filestatus )==-1)
            return 0;

        return filestatus.st_size;
    }
}
