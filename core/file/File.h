#ifndef _ITF_FILE_H_
#define _ITF_FILE_H_

#include "core/Macros.h"
#include "core/Types.h"

#define ITF_FILE_ATTR_CREATE_NEW    0x1
#define ITF_FILE_ATTR_READ          0x2
#define ITF_FILE_ATTR_WRITE         0x4
#define ITF_FILE_ATTR_READWRITE     (ITF_FILE_ATTR_READ | ITF_FILE_ATTR_WRITE)

#define ITF_FILE_FLAGS_ATTR_SEQUENTIAL  0x01
#define ITF_FILE_FLAGS_ATTR_RANDOM      0x02
#define ITF_FILE_FLAGS_ATTR_READONLY    0x04
#define ITF_FILE_FLAGS_ATTR_TMP		    0x08

namespace ITF
{
    class Blob;

    struct FileInfo
    {
        bbool   m_bExist;
        u64     m_LastUpdateTime;

        FileInfo();
        void    serialize(Blob &_blob)const;
        void    deserialize(Blob &_blob);
    };

    enum SeekMode
    {
        begin,
        current,
        end
    };

    enum FileOperation
    {
        FileOp_Write=0,
        FileOp_Read,
        FileOp_GetInfo,
        FileOp_Open,
        FileOp_Rename,
        FileOp_Delete
    };    
    typedef i32 (*OnFileOperationResultCallback_t)(FileOperation context, i32 res);

    using FILE_HANDLE = HANDLE;
    
    class File
    {
    public:

        virtual ~File() {};

        virtual bbool open(const String& path, u32 attribute,u32 _flagsAttribute = 0)
        {
            ITF_ASSERT(0);
            return bfalse;
        }

        virtual u64 getLastTimeWriteAccess() {return 0;}

        virtual bbool read(void *pBuffer, u32 size,u32* sizeRead) {ITF_ASSERT(0);return btrue;}

        virtual bbool read(void *pBuffer, u32 size) {ITF_ASSERT(0);return btrue;}

        virtual bbool write(const void *pBuffer, u32 size,u32* sizeWritten) {ITF_ASSERT(0);return btrue;}

        virtual bbool write(const void *pBuffer, u32 size) {ITF_ASSERT(0);return btrue;};

        virtual void close() {ITF_ASSERT(0);};

        virtual u64 getLength() {ITF_ASSERT(0);return 0;}

        virtual u64 seek(i64 offset, u16 from) {ITF_ASSERT(0);return 0;}

        virtual  FILE_HANDLE getHandle() { ITF_ASSERT(0); return 0;}

        // this callback is used to catch all results from IO operation, 
        // and returns the new result returned by operation process (changed or not)
        // this function returns the last registered callback, and will replace it by the new one
        static OnFileOperationResultCallback_t setOnFileOperationResultCallback(OnFileOperationResultCallback_t callback);

		// Create a buffer containing the whole file content. The buffer must be freed by the caller
		bbool fileToBuffer(const char *name, void*& buffer, u32 &size, u32 fileAttribute, u32 memFlag);

		// Write the whole buffer to a file. On Wii, the buffer address and size must be 32 bytes aligned.
		bbool bufferToFile(const char *name, void* buffer, u32 size, u32 fileAttribute);

#ifdef ITF_SUPPORT_CHEAT
        static void simulateTime() { if (_simulateSeekTime>0) { sleep(_simulateSeekTime); } }
        static void incSimulateSeekTime(i32 _val) {_simulateSeekTime+=_val;if (_simulateSeekTime<0) _simulateSeekTime=0;}
        static i32                                 _simulateSeekTime;
#endif //ITF_SUPPORT_CHEAT

    protected:
        static OnFileOperationResultCallback_t     _onFileOperationResultCallback;

        static i32 callOnResultCallback(i32 res, FileOperation op);
    };

} // namespace ITF


#ifdef ITF_SUPPORT_CHEAT
#define SIMULATE_TIME File::simulateTime();
#else
#define SIMULATE_TIME
#endif

#if defined (ITF_WINDOWS) || defined(ITF_XBOX_SERIES)
#include "Microsoft/File_microsoft.h"
#elif defined (ITF_PS3)
#include "PS3/File_ps3.h"
#elif defined (ITF_X360)
#include "X360/File_x360.h"
#elif defined (ITF_IPAD)
#include "iPad/File_iPad.h"
#elif defined (ITF_CTR)
#include "CTR/File_CTR.h"
#elif defined (ITF_WII)
#include "WII/File_WII.h"
#elif defined (ITF_PS5)
#include "PS5/File_PS5.h"
#elif defined (ITF_NINTENDO)
#include "Nintendo/File_Nintendo.h"
#else
#error Platform not supported
#endif //ITF_WINDOWS

#endif // _ITF_FILE_H_
