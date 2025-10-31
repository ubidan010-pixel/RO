#include "precompiled_core.h"

#include "core/file/File.h"
#include "core/file/PS5/File_ps5.h"

#include <kernel.h>

#include "core/UnicodeTools.h"

//ENABLE THIS FLAGS TO BREAK MULTIPLE ACCESS
//#define TRACK_FILE_VIOLATION 1

namespace ITF
{
    hwFile::hwFile() = default;

    hwFile::~hwFile()
    {
        close();
    }

    static int convertOpenFlags(u32 flags)
    {
        int openFlags = 0;
        if ((flags & ITF_FILE_ATTR_READ) && (flags & ITF_FILE_ATTR_WRITE))
        {
            openFlags |= SCE_KERNEL_O_RDWR;
        }
        else if (flags & ITF_FILE_ATTR_READ)
        {
            openFlags |= SCE_KERNEL_O_RDONLY;
        }
        else if (flags & ITF_FILE_ATTR_WRITE)
        {
            openFlags |= SCE_KERNEL_O_WRONLY;
        }
        if (flags & ITF_FILE_ATTR_CREATE_NEW)
        {
            openFlags |= SCE_KERNEL_O_CREAT;
        }
        return openFlags;
    }

    bbool hwFile::open(const String& path, u32 attribute, u32 _flagsAttribute)
    {
        if (isValidOpenedFileDescriptor())
            close();

        m_openedFileDescriptor = sceKernelOpen(UTF16ToUTF8(path.cStr()).get(), convertOpenFlags(attribute), SCE_KERNEL_S_IRWU);
        m_openedFileDescriptor = callOnResultCallback(m_openedFileDescriptor, FileOp_Open);

        return isValidOpenedFileDescriptor();
    }

    void hwFile::close()
    {
        if (isValidOpenedFileDescriptor())
        {
            ITF_VERIFY_SCE_CALL(sceKernelClose(m_openedFileDescriptor));
            m_openedFileDescriptor = DEFAULT_INVALID_OPENED_FILE_DESCRIPTOR;
        }
    }

    u64 hwFile::getLength()
    {
        ITF_ASSERT(isValidOpenedFileDescriptor());

        SceKernelStat fileStat{};
        int res = sceKernelFstat(m_openedFileDescriptor, &fileStat);
        res = callOnResultCallback(res, FileOp_GetInfo);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelFstat");

        return fileStat.st_size;
    }

    u64 hwFile::getLastTimeWriteAccess()
    {
        ITF_ASSERT(isValidOpenedFileDescriptor());

        SceKernelStat fileStat{};
        int res = sceKernelFstat(m_openedFileDescriptor, &fileStat);
        res = callOnResultCallback(res, FileOp_GetInfo);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelFstat");

        return fileStat.st_mtime;
    }

    bbool hwFile::read(void* pBuffer, u32 size, u32* sizeRead)
    {
        ITF_ASSERT(isValidOpenedFileDescriptor());

        ssize_t sizeRes = sceKernelRead(m_openedFileDescriptor, pBuffer, size);
        ITF_ASSERT_MSG(sizeRes <= ssize_t(I32_MAX), "Reading more than 32 bit integer");
        i32 res = static_cast<i32>(sizeRes < 0 ? sizeRes : SCE_OK);
        res = callOnResultCallback(res, FileOp_Read);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelRead");

        if (res >= 0 && sizeRead != nullptr)
        {
            *sizeRead = static_cast<u32>(sizeRes);
        }

        return res >= 0 && sizeRes == static_cast<ssize_t>(size);
    }


    bbool hwFile::read(void* pBuffer, u32 size)
    {
        return read(pBuffer, size, nullptr);
    }


    bbool hwFile::write(const void *pBuffer, u32 size, u32* sizeWritten)
    {
        ITF_ASSERT(isValidOpenedFileDescriptor());

        ssize_t sizeRes = sceKernelWrite(m_openedFileDescriptor, pBuffer, size);
        i32 res = static_cast<i32>(sizeRes < 0 ? sizeRes : SCE_OK);
        res = callOnResultCallback(res, FileOp_Write);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelWrite");

        if (res >= 0 && sizeWritten!=nullptr)
        {
            *sizeWritten = static_cast<u32>(sizeRes);
        }

        return res >= 0 && sizeRes == static_cast<ssize_t>(size);
    }

    bbool hwFile::write(const void *pBuffer, u32 size)
    {
        return hwFile::write(pBuffer, size, nullptr);
    }

    u64 hwFile::seek(i64 offset, u16 from)
    {
        ITF_ASSERT(isValidOpenedFileDescriptor());

        off_t resultPos = sceKernelLseek(m_openedFileDescriptor, offset, static_cast<int>(from));
        i32 res = static_cast<i32>(resultPos < 0 ? resultPos : SCE_OK);
        res = callOnResultCallback(res, FileOp_Read);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelLseek");

        return res >= 0 ? static_cast<u64>(resultPos) : 0;
    }

    u64 getLastTimeWriteAccess() { return 0; }


    // static methods

    bbool hwFile::MoveFile(const String& _szExistingFilename, const String& _szNewFilename)
    {
        int res = sceKernelRename(UTF16ToUTF8(_szExistingFilename.cStr()).get(), UTF16ToUTF8(_szNewFilename.cStr()).get());
        res = callOnResultCallback(res, FileOp_Rename);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelRename");

        return res >= 0;
    }

    SceKernelStat hwFile::getFileStat(const String& _fileName)
    {
        SceKernelStat fileStat{};
        int res = sceKernelStat(UTF16ToUTF8(_fileName.cStr()).get(), &fileStat);
        res = callOnResultCallback(res, FileOp_GetInfo);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelStat");
        return fileStat;
    }

    u64  hwFile::getLastTimeWriteAccess(const String& _szExistingFilename)
    {
        return getFileStat(_szExistingFilename).st_mtime;
    }

    void hwFile::flushTimeWriteAccess(const String& _szExistingFilename, u64 _lastWrite)
    {
        NOT_IMPLEMENTED_ON_PS5;

        // The following code do not handle the set of microseconds
        SceKernelTimeval timeVal{};
        timeVal.tv_sec = static_cast<time_t>(_lastWrite);

        int res = sceKernelUtimes(UTF16ToUTF8(_szExistingFilename.cStr()).get(), &timeVal);
        res = callOnResultCallback(res, FileOp_Write);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelUtimes");
    }

    u64 hwFile::getFileSize(const String& _fullfilePath)
    {
        return getFileStat(_fullfilePath).st_size;
    }

    bbool hwFile::exists(const String& _fullfileName)
    {
        SceKernelStat fileStat{};
        int res = sceKernelStat(UTF16ToUTF8(_fullfileName.cStr()).get(), &fileStat);
        // Assume we do not assert on any error.
        return (res >= 0);
    }

    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
        int res = sceKernelUnlink(UTF16ToUTF8(_fullfilePath.cStr()).get());
        res = callOnResultCallback(res, FileOp_Delete);
        ITF_ASSERT_SCE_RESULT_NAMED(res, "sceKernelUnlink");
        return res >= 0;
    }

    u64 hwFile::convertTimestamp(const FILETIME& _specific)
    {
        return (u64(_specific.dwHighDateTime) << 32ull) | u64(_specific.dwLowDateTime);
    }

    FILETIME hwFile::convertTimestamp(u64 _generic)
    {
        FILETIME ft;
        ft.dwHighDateTime   = static_cast<u32>(_generic >> 32ull);
        ft.dwLowDateTime    = static_cast<u32>(_generic & 0xFFffFFffull);
        return ft;
    }
}
