#ifndef _ITF_FILE_WII_H_
#define _ITF_FILE_WII_H_

#include <revolution.h>

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#define FILE_BIG_WAIT

namespace ITF
{
	#define FILE_BEGIN           	0
	#define FILE_CURRENT         	1
	#define FILE_END             	2

	#define INVALID_HANDLE_VALUE	(FILE_HANDLE)-1
	#define INVALID_INDEX_VALUE		-1

	struct FileCache
	{
		u32				m_CacheBlockSize;	// block size
		u8				*m_cache;			// start of buffer
		u32				m_cachepos;			// position in file that corresponds to cache begin
		u32				m_cachesize;		// size in cache that has been read from disk
		u32				m_cachecount;		// number of bytes that can still be read at end of cache buffer for current read.
		u32				m_readpos;			// current read position in file (corresponds to m_cache + m_CacheBlockSize - m_cachecount).
	};
	
    struct ReadRequest
    {
        volatile i32    mi32_ReadResult;
        OSThread        *m_pThread;
    };

    class hwFile : public File
	{
		public:
            hwFile();
			~hwFile();

		public:
			bbool			open(const String& path, u32 attribute,u32 _flagsAttribute = 0);
			void			close();
			u64				getLength();
			bbool			read(void *pBuffer, u32 size,u32* sizeRead);
			bbool			read(void *pBuffer, u32 size);
			bbool			write(const void *pBuffer, u32 size,u32* sizeWritten);
			bbool			write(const void *pBuffer, u32 size);
			u64				seek(i64 offset, u16 from);
			FILE_HANDLE		getHandle();

		public:
		    static String	normalizeWIIPath(const String &_path);

			static bbool	CopyFile(const String& szExistingFilename,const String& szNewFilename);
			static bbool	MoveFile(const String& szExistingFilename,const String& szNewFilename);
			static u64		getLastTimeWriteAccess(const String& szExistingFilename);
			static void		flushTimeWriteAccess(const String& szExistingFilename, const u64& _lastWrite);
			static bbool	exists(const String& _fullfileName);
			static bbool	deleteFile(const String& _fullfilePath);
			static u64		getFileSize(const String& _fullfilePath);
			static String8  getNANDHomeDir();
            static bbool    checkAvailableSpace(u32 fsblock, u32 inode, u32* answer);

		private:
			void			updateCacheCount();
			
            static s32      DVDReadAsyncWait(hwFile* _hwFile, void* _addr, s32 _length, s32 _offset);
			s32				DVDReadToCache(s32 _length, s32 _offset);
			s32				DVDReadFromCache(i32 *_piSize, u8 **_ppDest);

		private:
			// File infos
			DVDFileInfo		m_FileInfo;	// SDK info
			s32				m_entry;	// dvd entry index

			// Cache buffer
			FileCache		m_FileCache;

            // To handle I/O errors
            u32             m_retryOperationCount;

#ifdef FILE_BIG_WAIT
		public:
			volatile static bbool	m_BigWait; 		// Should we trigger a "reading disk" message ?
			volatile static long	m_ReadCounter; 	// Number of parallel reads.
			static OSAlarm			m_BigWaitAlarm; // Used to track reads that last too long
			const static u32		m_BigWaitTime;  // Time before we trigger a "reading disk" message
			static bbool			m_BigWaitInitDone; 
#endif //FILE_BIG_WAIT
    };

	class NandFile : public File
	{
	public:
		NandFile();
		virtual ~NandFile();

		virtual bbool open(const String& path, u32 attribute,u32 _flagsAttribute = 0);
		virtual bbool read(void *pBuffer, u32 size,u32* sizeRead);
		virtual bbool read(void *pBuffer, u32 size);
		virtual bbool write(const void *pBuffer, u32 size,u32* sizeWritten);
		virtual bbool write(const void *pBuffer, u32 size);
		virtual void close();
		virtual u64 getLength();
		virtual u64 seek(i64 offset, u16 from);
		virtual FILE_HANDLE getHandle() {return (void *)&m_fileInfo;}

	private:
		u64		tell();
		bbool	create(const char *name, u32 attribute);

#ifndef ITF_FINAL
		ITF::String8 m_name;
#endif //ITF_FINAL
		bbool m_isOpened;
		NANDFileInfo m_fileInfo;
	};

} // namespace ITF

#endif // _ITF_FILE_WII_H_
