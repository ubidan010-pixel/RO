#pragma once

//////////////////////////////////////////////////////////////////////////
///
/// File manipulation class
///

namespace ITF
{
 
    class hwFile : public File
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
        ~hwFile();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// open a file 
        /// @param path string path to access the file
        /// @param attribute or (|) mask for openning
        /// @return  btrue if succeed
        /// 
        bbool open(const String& path, u32 attribute,u32 _flagsAttribute = 0);
        
        //////////////////////////////////////////////////////////////////////////
        ///
        /// close an opended file 
        ///         
        void close();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// get lenght of an opended file
        /// @return file lenght if opended file is correct, 0 insted
        /// 
        u64 getLength();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// read file data
        /// @param pBuffer pointer to the user buffer that is to receive the data read from file
        /// @param size maximum bytes to read
        /// @return the number of byte readed
        /// 
        bbool read(void *pBuffer, u32 size,u32* sizeRead);

        bbool read(void *pBuffer, u32 size);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// write file data
        /// @param pBuffer pointer to the user buffer that is to receive the data to write on the file
        /// @param size maximum bytes to write
        /// @return the number of byte writed
        /// 
        virtual bbool write(const void *pBuffer, u32 size,u32* sizeWritten);

        virtual bbool write(const void *pBuffer, u32 size);

         //////////////////////////////////////////////////////////////////////////
        ///
        /// seek file
        /// @param offset number of bytes to move the pointer
        /// @param from movement mode
        /// @return offset from the beginning of the file
        /// 
        u64 seek(i64 offset, u16 from);
    
        FILE_HANDLE getHandle() {return m_Handle;}

        static bbool CopyFile(const String& szExistingFilename,const String& szNewFilename);

        static bbool MoveFile(const String& szExistingFilename,const String& szNewFilename);

        /// @fn static u64 getLastTimeWriteAccess(const String& szExistingFilename)
        /// @brief  Gets the last time write access. 
        /// @param  szExistingFilename  Filename . 
        /// @return The last time write access. 
        static u64 getLastTimeWriteAccess(const String& szExistingFilename);


        virtual u64 getLastTimeWriteAccess();
        /// @fn static void flushTimeWriteAccess(const String& szExistingFilename,u64& _lastWrite)
        /// @brief  Flushes a time write access for a specific file. 
        /// @param  szExistingFilename  Filename .
        static void flushTimeWriteAccess(const String& szExistingFilename, const u64& _lastWrite);

        /// @brief Test if a file exists
        /// @param  _fullfileName   Full path of the file.
        /// @return btrue if the file exists.
        static bbool exists(const String& _fullfileName);

        /// @brief  Deletes the file given by _fullfilePath. 
        /// @param  _fullfilePath   Full pathname of the file.
        /// @return btrue if it succeeds, false if it fails.
        static bbool deleteFile(const String& _fullfilePath);

        static u64 getFileSize(const String& _fullfilePath);

    protected:
        FILE_HANDLE   m_Handle;

        static u64      convertTimestamp(const FILETIME& _specific);
        static FILETIME convertTimestamp(u64 _generic);
    };


} // namespace ITF

