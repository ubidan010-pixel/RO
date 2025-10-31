#ifndef ITF_FILE_NINTENDO_H_
#define ITF_FILE_NINTENDO_H_

#pragma once

//////////////////////////////////////////////////////////////////////////
///
/// File manipulation class
///

#include "core/file/File.h"
#include "core/types.h"
#include "core/String8.h"
#include <nn/fs.h>

namespace ITF
{
    const u32 FILE_BEGIN    = 0;
    const u32 FILE_CURRENT  = 1;
    const u32 FILE_END      = 2;

    struct FILETIME
    {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
    };

    class hwFile  : public File
    {
    public:

        // initialize/terminate fs library
        static void initialize();
        static void mount(const String8& rootPath);
        static void terminate();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// default constructor
        ///
        hwFile();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// destructor
        ///
        ~hwFile() override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// open a file 
        /// @param path string path to access the file
        /// @param attribute or (|) mask for opening
        /// @return  btrue if succeed
        /// 
        bbool open(const String & path, u32 attribute, u32 _flagsAttribute = 0) override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// close an opened file 
        ///         
        void close() override;

        u64 getLength() override;

        u64 getLastTimeWriteAccess() override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// read file data
        /// @param pBuffer pointer to the user buffer that is to receive the data read from file
        /// @param size maximum bytes to read
        /// @return the number of byte read
        /// 
        bbool read(void *pBuffer, u32 size, u32* pSizeRead) override;
        bbool read(void *pBuffer, u32 size) override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// write file data
        /// @param pBuffer pointer to the user buffer that is to receive the data to write on the file
        /// @param size maximum bytes to write
        /// @return the number of byte written
        /// 
        bbool write(const void *pBuffer, u32 size,u32* pSizeWritten) override;
        bbool write(const void *pBuffer, u32 size) override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// seek file
        /// @param offset number of bytes to move the pointer
        /// @param from movement mode
        /// @return offset from the beginning of the file
        /// 
        u64 seek(i64 offset, u16 from) override;

        FILE_HANDLE getHandle() override { return reinterpret_cast<FILE_HANDLE>(m_Handle.handle); }

        /// @brief Test if a file exists
        /// @param  _fullfileName   Full path of the file.
        /// @return btrue if the file exists or an error occured.
        static bbool exists(const String & _fileName);

        /// @brief  Deletes the file given by _fullfilePath. 
        /// @param  _fullfilePath   Full pathname of the file.
        /// @return btrue if it succeeds, false if it fails.
        static bbool deleteFile(const String& _fullfilePath);

        static u64 getFileSize(const String & _filePath);

        static u64 getLastTimeWriteAccess(const String& szExistingFilename);

        static void flushTimeWriteAccess(const String& szExistingFilename, u64 _lastWrite);

    protected:

        bool isValidFileHandle()  const { return m_Handle.handle != nullptr; }

        nn::fs::FileHandle m_Handle{};
        int64_t m_tell = 0; // read/write position
        
        static bool ms_isMounted;

        static size_t ms_romCacheSize;
        static u8* ms_romCacheBuffer;

        static const char* ms_mountedDevice;
        static bool ms_canMountROM;

        static void mountROM();
        static void allocRomCache();
        static void freeRomCache();
        static void mountHost(const String8& rootPath);
        static void unmount();

        static u64 convertTimestamp(const FILETIME& _specific);
        static FILETIME convertTimestamp(u64 _generic);
    };

} // namespace ITF

#endif // ITF_FILE_NINTENDO_H_
