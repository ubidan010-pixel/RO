#pragma once

//////////////////////////////////////////////////////////////////////////
///
/// File manipulation class
///

#include <kernel.h>
#include "core/EngineCommon.h"
#include "core/file/File.h"

namespace ITF
{

    #define FILE_BEGIN           SCE_KERNEL_SEEK_SET
    #define FILE_CURRENT         SCE_KERNEL_SEEK_CUR
    #define FILE_END             SCE_KERNEL_SEEK_END

    struct FILETIME
    {
        u32 dwLowDateTime;
        u32 dwHighDateTime;
    };

    class hwFile  : public File
    {
    public:
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
        bbool open(const String& path, u32 attribute,u32 _flagsAttribute = 0) override;

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
        bbool read(void *pBuffer, u32 size,u32* sizeRead) override;

        bbool read(void *pBuffer, u32 size) override;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// write file data
        /// @param pBuffer pointer to the user buffer that is to receive the data to write on the file
        /// @param size maximum bytes to write
        /// @return the number of byte written
        /// 
        bbool write(const void *pBuffer, u32 size,u32* sizeWritten) override;


        bbool write(const void *pBuffer, u32 size) override;
        //////////////////////////////////////////////////////////////////////////
        ///
        /// seek file
        /// @param offset number of bytes to move the pointer
        /// @param from movement mode
        /// @return offset from the beginning of the file
        /// 
        u64 seek(i64 offset, u16 from) override;

        FILE_HANDLE getHandle() override { return static_cast<FILE_HANDLE>(m_openedFileDescriptor); }

        // Not implemented on PS5 -> as it's not yet used, prefer a compile error
        //static bbool CopyFile(const String& szExistingFilename,const String& szNewFilename);

        static bbool MoveFile(const String& szExistingFilename,const String& szNewFilename);

        /// @fn static u64 getLastTimeWriteAccess(const String& szExistingFilename)
        /// @brief  Gets the last time write access. 
        /// @param  szExistingFilename  Filename . 
        /// @return The last time write access. 
        static u64 getLastTimeWriteAccess(const String& szExistingFilename);

        /// @fn static void flushTimeWriteAccess(const String& szExistingFilename,u64& _lastWrite)
        /// @brief  Flushes a time write access for a specific file. 
        /// @param  szExistingFilename  Filename .
        static void flushTimeWriteAccess(const String& szExistingFilename, u64 _lastWrite);

        /// @brief Test if a file exists
        /// @param  _fullfileName   Full path of the file.
        /// @return btrue if the file exists or an error occured.
        static bbool exists(const String& _fullfileName);

        /// @brief  Deletes the file given by _fullfilePath. 
        /// @param  _fullfilePath   Full pathname of the file.
        /// @return btrue if it succeeds, false if it fails.
        static bbool deleteFile(const String& _fullfilePath);


        static u64 getFileSize(const String& _fullfilePath);

    protected:
        bool isValidOpenedFileDescriptor()  const { return m_openedFileDescriptor >= 0; }
        static SceKernelStat getFileStat(const String& _fileName);

        static constexpr int DEFAULT_INVALID_OPENED_FILE_DESCRIPTOR = SCE_KERNEL_ERROR_EIO;
        int m_openedFileDescriptor = DEFAULT_INVALID_OPENED_FILE_DESCRIPTOR;


        static u64      convertTimestamp(const FILETIME& _specific);
        static FILETIME convertTimestamp(u64 _generic);
    };

} // namespace ITF
