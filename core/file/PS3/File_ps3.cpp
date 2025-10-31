#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#include <cell/cell_fs.h>

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
        if (m_Handle != INVALID_HANDLE_VALUE)
            close();
    }


    bbool hwFile::open(const String& path, u32 attribute,u32 _flagsAttribute)
    {
        StringConverter str(path.cStr());
        DWORD flags = 0;

        if ((attribute & ITF_FILE_ATTR_READ) && (attribute & ITF_FILE_ATTR_WRITE))  {flags |= CELL_FS_O_RDWR;}
        if (attribute & ITF_FILE_ATTR_READ)         {flags |= CELL_FS_O_RDONLY;}
        if (attribute & ITF_FILE_ATTR_WRITE)        {flags |= CELL_FS_O_WRONLY;}
        if (attribute & ITF_FILE_ATTR_CREATE_NEW)   {flags |= CELL_FS_O_CREAT;}

        CellFsErrno err =  cellFsOpen(str.getChar(),flags, &m_Handle,NULL,0);

        err = callOnResultCallback(err, FileOp_Open);
        return (err == CELL_FS_SUCCEEDED);
    }

    void hwFile::close()
    {
        if (m_Handle!=INVALID_HANDLE_VALUE)
        {
            cellFsClose(m_Handle);
        }

        m_Handle = INVALID_HANDLE_VALUE;
    }


    u64 hwFile::getLength()
    {
        CellFsStat sb;
        CellFsErrno err = cellFsFstat(m_Handle, &sb);
        err = callOnResultCallback(err, FileOp_GetInfo);
        if (err == CELL_FS_SUCCEEDED)
        {
            return sb.st_size;
        }

        return 0;
    }
 
    bbool hwFile::read(void *pBuffer, u32 size)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;

        CellFsErrno err = cellFsRead(m_Handle,pBuffer,size,NULL);
        err = callOnResultCallback(err, FileOp_Read);
        return (err == CELL_FS_SUCCEEDED);
    }

    bbool hwFile::write(const void *pBuffer, u32 size,u32* sizeWritten)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;

        u64 nbWritten = 0;
        CellFsErrno err = cellFsWrite(m_Handle, pBuffer, u64(size), &nbWritten);
        err = callOnResultCallback(err, FileOp_Write);
        if (sizeWritten)
        {
            *sizeWritten = nbWritten;
        }
        return (err == CELL_FS_SUCCEEDED);
    }

    bbool hwFile::write(const void *pBuffer, u32 size)
    {
        return hwFile::write(pBuffer, size, NULL);
    }

    bbool hwFile::read(void *pBuffer, u32 size,u32* sizeRead)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;

        u64 val;
        CellFsErrno err = cellFsRead(m_Handle,pBuffer,size,&val);
        err = callOnResultCallback(err, FileOp_Read);
        *sizeRead = (u32) val;//well lack of data;
        return (err == CELL_FS_SUCCEEDED);
    }

    u64 hwFile::seek(i64 offset, u16 from)
    {
        if (m_Handle==INVALID_HANDLE_VALUE)       return 0;

        ITF_ASSERT(from == 0);//must fix for option
        uint64_t pos;
        CellFsErrno err ;
        ITF_VERIFY(CELL_OK == (err = cellFsLseek(m_Handle,offset,from,&pos)));
        err = callOnResultCallback(err, FileOp_Read);
        return pos;
    }

    // static methods

#if 0
    bbool hwFile::CopyFile(const String& szExistingFilename,const String& szNewFilename)
    {
        // Not (yet ?) used
        // No API available -> should be done by hand
        // Note that the modify time should probably be set to the one of the source file,
        // so that the copy operation is the same than the one an OS is doing
        NOT_IMPLEMENTED_ON_PS3
        return 0;
    }
#endif

    bbool hwFile::MoveFile(const String& szExistingFilename,const String& szNewFilename)
    {
        NOT_IMPLEMENTED_ON_PS3 // code following
        StringConverter strFrom(szExistingFilename);
        StringConverter strTo(szNewFilename);
        CellFsErrno err = cellFsRename(strFrom.getChar(), strTo.getChar());
        err = callOnResultCallback(err, FileOp_Rename);
        return (err == CELL_OK);
    }

    u64  hwFile::getLastTimeWriteAccess(const String& _szExistingFilename)
    {
        StringConverter str(_szExistingFilename);
        CellFsStat sb;
        CellFsErrno err = cellFsStat(str.getChar(), &sb);
        err = callOnResultCallback(err, FileOp_GetInfo);
        if (err == CELL_FS_SUCCEEDED)
        {
            return sb.st_mtime;
        }
        return 0;
    }

    void hwFile::flushTimeWriteAccess(const String& _szExistingFilename,const u64& _lastWrite)
    {
        NOT_IMPLEMENTED_ON_PS3 // code following
        StringConverter str(_szExistingFilename);
        CellFsUtimbuf timeInfo;
        timeInfo.actime = time_t(_lastWrite);
        timeInfo.modtime = time_t(_lastWrite);
        CellFsErrno err;
        ITF_VERIFY(CELL_OK == (err = cellFsUtime(str.getChar(), &timeInfo)));
        err = callOnResultCallback(err, FileOp_Write);
    }

    u64 hwFile::getFileSize(const String& _fullfilePath)
    {
        StringConverter str(_fullfilePath);
        CellFsStat sb;
        CellFsErrno err = cellFsStat(str.getChar(), &sb);
        err = callOnResultCallback(err, FileOp_GetInfo);
        if (err == CELL_FS_SUCCEEDED)
        {
            return sb.st_size;
        }
        return 0;
    }

    bbool hwFile::exists(const String& _fullfileName)
    {
        StringConverter str(_fullfileName.cStr());
        CellFsStat sb;
        CellFsErrno err = cellFsStat(str.getChar(),&sb);
        return (err == CELL_FS_SUCCEEDED);
    }

    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
        NOT_IMPLEMENTED_ON_PS3 // code following
        StringConverter str(_fullfilePath.cStr());
        CellFsErrno err = cellFsUnlink(str.getChar());
        err = callOnResultCallback(err, FileOp_Delete);
        return (err == CELL_FS_SUCCEEDED);
    }

    u64 hwFile::convertTimestamp(const FILETIME& _specific)
    {
        return (u64(_specific.dwHighDateTime) << 32ull) | u64(_specific.dwLowDateTime);
    }

    FILETIME hwFile::convertTimestamp(u64 _generic)
    {            
        NOT_IMPLEMENTED_ON_PS3 // code following
        FILETIME ft;
        ft.dwHighDateTime   = (DWORD)(_generic >> 32ull);
        ft.dwLowDateTime    = (DWORD)(_generic & 0xFFffFFffull);
        return ft;
    }
}