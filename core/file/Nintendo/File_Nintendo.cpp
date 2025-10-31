#include "precompiled_core.h"

#include "core/file/File.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "core/file/FileServer.h"
#include "core/UnicodeTools.h"
#include "core/itfstring.h"
#include "core/String8.h"
#include "core/error/ErrorHandler.h"

#include <nn/fs.h>

#if !defined(ITF_FINAL)
#include <nn/fs/fs_RomForDebug.h>
#endif // ITF_FINAL


namespace ITF
{
#ifndef ITF_FINAL
    static const char gs_hostMountName[] = "host";
#endif
    static const char gs_romMountName[]  = "rom";

    const char* hwFile::ms_mountedDevice = nullptr;

    bool hwFile::ms_isMounted = false;

    bool hwFile::ms_canMountROM = true;
    size_t hwFile::ms_romCacheSize = 0;
    u8* hwFile::ms_romCacheBuffer = nullptr;

    void hwFile::mountROM()
    {
        allocRomCache();
        ITF_ASSERT_CRASH(ms_canMountROM, "ROM device not available");
        nn::Result result = nn::fs::MountRom(gs_romMountName, ms_romCacheBuffer, ms_romCacheSize);
        ms_isMounted = result.IsSuccess();
        ms_mountedDevice = ms_isMounted ? gs_romMountName : nullptr;
    }

    void hwFile::mountHost(const String8& _rootPath)
    {
    #ifndef ITF_FINAL
        nn::Result result = nn::fs::MountHost(gs_hostMountName, _rootPath.cStr());
        ms_isMounted = result.IsSuccess();
        ms_mountedDevice = ms_isMounted ? gs_hostMountName : nullptr;
    #else
        ms_isMounted = false;
        ms_mountedDevice = nullptr;
    #endif
    }

    void hwFile::unmount()
    {
        if (ms_mountedDevice)
        {
            nn::fs::Unmount(ms_mountedDevice);
            ms_mountedDevice = nullptr;
            ms_isMounted = false;
        }
    }

    void hwFile::initialize()
    {
        nn::fs::SetAllocator(
            [](size_t _size){ return Memory::mallocCategory(_size, MemoryId::mId_System); },
            [](void* _ptr, size_t /*_size*/) { Memory::free(_ptr); });

        ms_canMountROM = true;

    #ifndef ITF_FINAL
        ms_canMountROM = nn::fs::CanMountRomForDebug();
    #endif  // ITF_FINAL
    }

    void hwFile::allocRomCache()
    {
        if (ms_romCacheBuffer == nullptr)
        {
            nn::Result result = nn::fs::QueryMountRomCacheSize(&ms_romCacheSize);
            ITF_UNUSED(result);
            ms_romCacheBuffer = new (MemoryId::mId_System) u8[ms_romCacheSize];
            ITF_ASSERT_CRASH(ms_romCacheBuffer, "failed to allocate fs rom cache");
        }
    }

    void hwFile::freeRomCache()
    {
        SF_DEL_ARRAY(ms_romCacheBuffer);
    }

    void hwFile::mount(const String8 & _rootPath)
    {
        if (_rootPath == "rom:")
        {
            ITF_ASSERT_CRASH(ms_canMountROM, "Rom cannot be mounted");
            mountROM();
        }
        else
        {
            mountHost(_rootPath.cStr());
        }

        ITF_ASSERT_CRASH(ms_isMounted, "Cannot mount fs with explicit path %s", _rootPath.cStr());
    }

    void hwFile::terminate()
    {
        if (ms_isMounted)
        {
            unmount();
            freeRomCache();
        }
    }

    hwFile::hwFile()
    : m_tell(0)
    {
        m_Handle.handle = nullptr;
    }

    hwFile::~hwFile()
    {
        if (m_Handle.handle)
            close();
    }

    static int convertOpenAttribute(u32 _attribute)
    {
        int openMode = 0;

        if (_attribute & ITF_FILE_ATTR_READ)
        {
            openMode |= nn::fs::OpenMode_Read;
        }

        if (_attribute & ITF_FILE_ATTR_WRITE)
        {
            openMode |= nn::fs::OpenMode_Write;
        }

        if (_attribute & ITF_FILE_ATTR_CREATE_NEW)
        {
            openMode |= nn::fs::OpenMode_Write | nn::fs::OpenMode_AllowAppend;
        }

        return openMode;
    }


    bbool hwFile::open(const String& _path, u32 _attribute, u32 /*_flagsAttribute*/)
    {
        ITF_ASSERT(m_Handle.handle == nullptr);
        if (isValidFileHandle())
            close();

        UTF16ToUTF8 utf8Path(_path.cStr());

        nn::Result result{};
        bbool fileNeedsTruncation = bfalse;

        // handle creation of a brand new file 
        if (_attribute & ITF_FILE_ATTR_CREATE_NEW)
        {
            result = nn::fs::CreateFile(utf8Path.get(), 0);
            if (nn::fs::ResultPathAlreadyExists::Includes(result))
            {
                nn::fs::DirectoryEntryType entryType;
                result = nn::fs::GetEntryType(&entryType, utf8Path.get());
                if (result.IsSuccess())
                {
                    fileNeedsTruncation = entryType == nn::fs::DirectoryEntryType_File;
                }
            }
        }

        // perform the actual open operation
        result = nn::fs::OpenFile(&m_Handle, utf8Path.get(), convertOpenAttribute(_attribute));
        m_tell = 0;

        if (result.IsSuccess())
        {
            // handle truncation of an existing file
            if (fileNeedsTruncation)
                result = nn::fs::SetFileSize(m_Handle, 0);
        }

        return result.IsSuccess() ? btrue : bfalse;
    }

    void hwFile::close()
    {
        if (m_Handle.handle)
        {
            if(nn::fs::GetFileOpenMode(m_Handle) & (nn::fs::OpenMode_Write| nn::fs::OpenMode_AllowAppend) )
            {
                nn::fs::FlushFile(m_Handle);
            }
            nn::fs::CloseFile(m_Handle);
            m_Handle = {};
        }
    }

	u64 hwFile::getLength()
	{
        ITF_ASSERT(m_Handle.handle);

        int64_t fileSize = 0;
        nn::Result result = nn::fs::GetFileSize(&fileSize, m_Handle);

        ITF_ASSERT(result.IsSuccess());
        ITF_UNUSED(result);

        return fileSize;
    }

    u64 hwFile::getLastTimeWriteAccess()
    {
        ITF_ASSERT_CRASH(false, "No valid API available");
        // There is GetFileTimeStampForDebug(..) but it is only valid for host and sd card
        return 0;
    }
 
    bbool hwFile::read(void * _pBuffer, u32 _size)
    {
        return read(_pBuffer, _size, nullptr);
    }

    bbool hwFile::read(void* _pBuffer, u32 _size, u32* _pSizeRead)
    {
        bbool success = bfalse;

        ITF_ASSERT(isValidFileHandle());

        if (isValidFileHandle())
        {
            size_t sizeRead = 0;
            nn::Result result = nn::fs::ReadFile(&sizeRead, m_Handle, m_tell, _pBuffer, static_cast<size_t>(_size));
            ITF_ASSERT(result.IsSuccess());
            m_tell += sizeRead;

            if (_pSizeRead != nullptr)
                *_pSizeRead = sizeRead;

            success = result.IsSuccess() ? btrue : bfalse;
        }

        return success;
    }

    bbool hwFile::write(const void * _pBuffer, u32 _size, u32* _pSizeWritten)
    {
        bbool success = bfalse;

        ITF_ASSERT(isValidFileHandle());

        if (isValidFileHandle())
        {
            nn::Result result = nn::fs::WriteFile(m_Handle, m_tell, _pBuffer, size_t(_size), nn::fs::WriteOption());
            ITF_ASSERT(result.IsSuccess());
            success = result.IsSuccess() ? btrue : bfalse;

            // the write operation is either totally executed or not executed at all
            u32 sizeWritten = success ? _size : 0;

            m_tell += sizeWritten;
            
            if (_pSizeWritten!=nullptr)
                *_pSizeWritten = sizeWritten;
        }

        return success;
    }

    bbool hwFile::write(const void * _pBuffer, u32 _size)
    {
        return hwFile::write(_pBuffer, _size, nullptr);
    }

    u64 hwFile::seek(i64 _offset, u16 _from)
    {
        ITF_ASSERT(isValidFileHandle());
        ITF_ASSERT(_from>=FILE_BEGIN && _from<=FILE_END);

        if (isValidFileHandle())
        {
            switch(_from)
            {
            case FILE_CURRENT:
                m_tell += _offset;
                break;
            case FILE_BEGIN:
                m_tell = _offset;
                break;
            case FILE_END:
                m_tell = getLength() - _offset;
                break;
            }
        }

        return m_tell;
    }

    bbool hwFile::exists(const String& _filePath)
    {
        String8 utf8Path = UTF16ToUTF8(_filePath.cStr()).get();

        bbool fileExists = bfalse;

        // the system API asserts if the requested device is not mounted - avoid this
        if (ms_isMounted && utf8Path.substr(0, ::strlen(ms_mountedDevice)) == ms_mountedDevice)
        {
            nn::fs::DirectoryEntryType entryType;
            nn::Result result = nn::fs::GetEntryType(&entryType, utf8Path.cStr());

            fileExists = (result.IsSuccess() && entryType == nn::fs::DirectoryEntryType_File) ? btrue : bfalse;
        }

        return fileExists;
    }

    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
        nn::Result result = nn::fs::DeleteFile(UTF16ToUTF8(_fullfilePath.cStr()).get());
        return result.IsSuccess() ? btrue : bfalse;
    }

    u64 hwFile::getFileSize(const String & _filePath)
    {
        int64_t fileSize = 0;

        nn::Result result;
        nn::fs::FileHandle fileHandle;
        result = nn::fs::OpenFile(&fileHandle, UTF16ToUTF8(_filePath.cStr()).get(), nn::fs::OpenMode_Read);

        if(result.IsSuccess())
        {
            result = nn::fs::GetFileSize(&fileSize, fileHandle);
            nn::fs::CloseFile(fileHandle);
        }

        return fileSize;
    }

    u64 hwFile::getLastTimeWriteAccess(const String& szExistingFilename)
    {
        ITF_ASSERT_CRASH(false, "No valid API available");
        // There is GetFileTimeStampForDebug(..) but it is only valid for host and sd card
        return 0;
    }

    void hwFile::flushTimeWriteAccess(const String& szExistingFilename, u64 _lastWrite)
    {
        ITF_ASSERT_CRASH(false, "No valid API available");
    }
}
