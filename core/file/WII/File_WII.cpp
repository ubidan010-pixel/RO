#include "precompiled_core.h"

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_DIRECTORY_H_
#include  "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_ 

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef __NAND_H__
#include <revolution/nand.h>
#endif //__NAND_H__

/*
=======================================================================================================================
=======================================================================================================================
*/

namespace ITF
{
	#define CACHE_BLOCK_SIZE			(16 * 1024)
	#define ADD_READ_SIZE				( 8 * 1024)

#ifdef FILE_BIG_WAIT
	// Maximum time in milliseconds between DVDReadAsync and the call to a DVDReadAsync callback function
	// before we trigger a "reading disk" message.
	const u32 hwFile::m_BigWaitTime = 1000;

	// Should we trigger a "reading disk" message ?
	volatile bbool hwFile::m_BigWait = bfalse;

	// Number of parallel reads.
	volatile long  hwFile::m_ReadCounter = 0;

	bbool hwFile::m_BigWaitInitDone = bfalse;

	OSAlarm hwFile::m_BigWaitAlarm;
#endif //FILE_BIG_WAIT



	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    hwFile::hwFile() :
		m_entry(INVALID_INDEX_VALUE), m_retryOperationCount(0)
    {
		ITF_MemSet(&m_FileInfo, 0, sizeof(DVDFileInfo));
		ITF_MemSet(&m_FileCache, 0, sizeof(FileCache));
    }
    
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    hwFile::~hwFile()
    {
       if(m_entry != INVALID_INDEX_VALUE)
           close();
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::open(const String& _absolutePath, u32 _attribute, u32 _flagsAttribute)
    {
    	close();

        StringConverter str(normalizeWIIPath(_absolutePath).cStr());

		m_entry = DVDConvertPathToEntrynum(str.getChar());
        m_entry = callOnResultCallback(m_entry, FileOp_Open);
        
        if(m_entry == INVALID_INDEX_VALUE)
        {
            LOG("open is retrying (attempt count: %d)", m_retryOperationCount);
            if(m_retryOperationCount==0)
            {
                m_retryOperationCount++;
                return open(_absolutePath, _attribute, m_retryOperationCount);
            }
        }
        else
        {
            m_retryOperationCount = 0;
        }

		if(m_entry == INVALID_INDEX_VALUE)
		{
			LOG("Error : file '%s' not found\n", str.getChar());
		}
		else
		{
			// Alloc cache
			ITF_ASSERT(m_FileCache.m_cache == 0x0);
			m_FileCache.m_CacheBlockSize = CACHE_BLOCK_SIZE;
			m_FileCache.m_cache = (u8 *)Memory::alignedMallocCategory(sizeof(u8) * m_FileCache.m_CacheBlockSize, 32, MemoryId::mId_FileCache);
			ITF_MemSet(m_FileCache.m_cache, 0, m_FileCache.m_CacheBlockSize);

			// Open file
			if(DVDFastOpen(m_entry, &m_FileInfo))
			{
#ifdef ENABLE_FILE_REGISTERING
                if(FILEMANAGER)
				    FILEMANAGER->registerFile(_absolutePath, (u32)this);
#endif // ENABLE_FILE_REGISTERING
			}
			else
			{
				LOG("Error cannot open file '%s'\n", str.getChar());
				m_entry = INVALID_INDEX_VALUE;
				close();
			}
		}

		return m_entry != INVALID_INDEX_VALUE;
    }
        
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    void hwFile::close()
    {
        if(m_entry != INVALID_INDEX_VALUE)
        {
#ifdef ENABLE_FILE_REGISTERING
            if(FILEMANAGER)
                FILEMANAGER->unRegisterFile((u32)this);
#endif // ENABLE_FILE_REGISTERING
			DVDClose(&m_FileInfo);

            m_entry = INVALID_INDEX_VALUE;
        }

        if(m_FileCache.m_cache)
            Memory::alignedFree(m_FileCache.m_cache);
        ITF_MemSet(&m_FileCache, 0, sizeof(FileCache));

		ITF_MemSet(&m_FileInfo, 0, sizeof(DVDFileInfo));
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    u64 hwFile::getLength()
    {
        return DVDGetLength(&m_FileInfo);
    }
   
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::read(void *_pBuffer, u32 _size, u32* _sizeRead)
    {
        ITF_ASSERT_CRASH(_sizeRead != 0x0, "Bad argument");
        *_sizeRead = 0;

        if(m_entry != INVALID_INDEX_VALUE)
        {
		    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		    u8	*pDest = (u8 *) _pBuffer;
		    i32	iSize = (i32) _size;
		    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		    // Read from cache.
		    if(m_FileCache.m_cachecount != 0)
			    *_sizeRead += DVDReadFromCache(&iSize, &pDest);
			
		    if(iSize > 0)
		    {
			    // We really read in cache the wished size plus some more to avoid frequent seeks
			    u32 uReadSize = (iSize + ADD_READ_SIZE + 31) & 0xFFFFFFE0;
			    if(uReadSize > m_FileCache.m_CacheBlockSize)
				    uReadSize = m_FileCache.m_CacheBlockSize;

			    // DVDRead cannot read after the end of the file (or max 3 bytes more for alignment purposes).
                u32 u32FileLen = getLength();
			    u32 uMaxReadPos = u32FileLen;
			    if(uMaxReadPos > uReadSize)
			    {
				    uMaxReadPos = ((uMaxReadPos - uReadSize) + 3) & 0xFFFFFFFC;
			    }
			    else
			    {
				    // small file => we read the complete file
				    uReadSize = ((u32) uMaxReadPos + 31) & 0xFFFFFFE0;
				    uMaxReadPos = 0;
			    }

			    // Read from disk.
			    while((iSize > 0) && (m_FileCache.m_readpos < u32FileLen))
			    {
				    // DVDRead must read from an offset that is a multiple of 4.
				    u32 uAlignReadPos = m_FileCache.m_readpos & 0xFFFFFFFC;

				    // Real read position is limited by API constraints.
				    u32 uReadPos = Min(uMaxReadPos, uAlignReadPos);

				    // Read uReadSize starting at uReadPos.
				    DVDReadToCache(uReadSize, uReadPos);

				    // Copy to dest buffer
				    *_sizeRead += DVDReadFromCache(&iSize, &pDest);
			    }
		    }
        }

        return *_sizeRead;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::read(void *_pBuffer, u32 _size)
    {
		u32 sizeRead;
		return read(_pBuffer, _size, &sizeRead);
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::write(const void *_pBuffer, u32 size,u32* _sizeWritten)
    {
         return bfalse;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::write(const void *_pBuffer, u32 _size)
    {
         return bfalse;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    u64 hwFile::seek(i64 _offset, u16 _from)
    {
		if (m_entry == INVALID_INDEX_VALUE)
			return 0;
		
		u32 lastreadpos = m_FileCache.m_readpos;
		
		switch(_from)
		{
			case FILE_CURRENT:
				m_FileCache.m_readpos += (u32)_offset;
				break;
			case FILE_BEGIN:
				m_FileCache.m_readpos = (u32)_offset;
				break;
			case FILE_END:
				m_FileCache.m_readpos = getLength() - (u32)_offset;
				break;
		}    	

		if(m_FileCache.m_readpos != lastreadpos)
			updateCacheCount();
		
    	return (u64)m_FileCache.m_readpos;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    String hwFile::normalizeWIIPath(const String &_path)
    {
        String path = _path;
        path.replace('\\', '/');
        return path;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::CopyFile(const String& _szExistingFilename,const String& _szNewFilename)
    {
         return bfalse;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	FILE_HANDLE hwFile::getHandle()
	{
		return (m_entry == INVALID_INDEX_VALUE) ? INVALID_HANDLE_VALUE : &m_FileInfo;
	}

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::MoveFile(const String& _szExistingFilename,const String& _szNewFilename)
    {
        return bfalse;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    u64  hwFile::getLastTimeWriteAccess(const String& _szExistingFilename)
    {
        return 0;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    void hwFile::flushTimeWriteAccess(const String& _szExistingFilename,const u64& _lastWrite)
    {
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::exists(const String& _fullfileName)
    {
   		StringConverter str(normalizeWIIPath(_fullfileName).cStr());
        bbool res = DVDConvertPathToEntrynum(str.getChar()) != INVALID_INDEX_VALUE;
        return callOnResultCallback(res, FileOp_GetInfo);;
    }
        
	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::deleteFile(const String& _fullfilePath)
    {
        return bfalse;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    u64 hwFile::getFileSize(const String& _fullfilePath)
    {
        return 0;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	void hwFile::updateCacheCount()
	{
		// Is new read pos in valid cache ?
		if(		(m_FileCache.m_readpos >= m_FileCache.m_cachepos)
			&&	(m_FileCache.m_readpos < (m_FileCache.m_cachepos + m_FileCache.m_cachesize)))
		{
			// Read pos is in cache -> keep cache
			m_FileCache.m_cachecount = m_FileCache.m_cachesize - (m_FileCache.m_readpos - m_FileCache.m_cachepos);
		}
		else
		{
			// Invalidate cache for current read.
			m_FileCache.m_cachecount = 0;
		}
	}

#ifdef FILE_BIG_WAIT
    void BigWaitAlarmHandler(OSAlarm* alarm, OSContext* context)
	{
		hwFile::m_BigWait = btrue;
	}
#endif //FILE_BIG_WAIT

	void dvdReadCB(s32 result, DVDFileInfo* fileInfo)
    {
#ifdef FILE_BIG_WAIT
		const BOOL saveInterrupts = OSDisableInterrupts();
		OSCancelAlarm(&hwFile::m_BigWaitAlarm);
		InterlockedDecrement(&hwFile::m_ReadCounter);
		if (hwFile::m_ReadCounter)
			OSSetAlarm(&hwFile::m_BigWaitAlarm,OSMillisecondsToTicks(hwFile::m_BigWaitTime), BigWaitAlarmHandler);
		OSRestoreInterrupts(saveInterrupts);
#endif //FILE_BIG_WAIT

        ReadRequest *req = (ReadRequest*)DVDGetUserData(&fileInfo->cb);

        // Set "readdone" to true
        req->mi32_ReadResult = (result != 0) ? result : -1;

        // Wake up calling thread
        if(req->m_pThread)
            OSResumeThread(req->m_pThread);
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
    s32 hwFile::DVDReadAsyncWait(hwFile* _hwFile, void* _addr, s32 _length, s32 _offset)
    {
        ITF_ASSERT_CRASH(_offset>=0, "DVDRead : offset is negative!");
        ITF_ASSERT_CRASH(((u32)_addr & 0x1F)==0, "DVDRead : buffer must be 32-byte aligned");
        ITF_ASSERT_CRASH((_length & 0x1F) == 0, "DVDRead : size must be a multiple of 32.");
        ITF_ASSERT_CRASH((_offset & 0x3) == 0, "DVDRead : offset must be a multiple of 4");

#ifdef FILE_BIG_WAIT
		if (!m_BigWaitInitDone)
		{
			m_BigWaitInitDone = btrue;
			OSCreateAlarm(&m_BigWaitAlarm);
		}

		ITF_ASSERT(hwFile::m_ReadCounter>=0);
		BOOL saveInterrupts = OSDisableInterrupts();
		InterlockedIncrement(&hwFile::m_ReadCounter);
		if(hwFile::m_ReadCounter==1)
			OSSetAlarm(&hwFile::m_BigWaitAlarm,OSMillisecondsToTicks(hwFile::m_BigWaitTime), BigWaitAlarmHandler);
		OSRestoreInterrupts(saveInterrupts);
#endif //FILE_BIG_WAIT

        // Init request structure
        ReadRequest req;
        req.mi32_ReadResult = 0;
        req.m_pThread = OSGetCurrentThread();
        DVDSetUserData(&_hwFile->m_FileInfo.cb, &req);

        // Launch Read
        DVDReadAsync(&_hwFile->m_FileInfo, _addr, _length, _offset, dvdReadCB);

        // Wait end of Read
        BOOL intr = OSDisableInterrupts();
        while (req.mi32_ReadResult == 0)
            OSSuspendThread(req.m_pThread);
        OSRestoreInterrupts(intr);

        // Check errors
        s32 s32_Status = callOnResultCallback(DVDGetFileInfoStatus(&_hwFile->m_FileInfo), FileOp_Read);
    
        // Handle errors (retry when we failed)
        if(s32_Status!=DVD_STATE_END)
        {
            // Restart Read?
            LOG("DVDReadAsyncWait is retrying (attempt count: %d)", _hwFile->m_retryOperationCount);
            if(_hwFile->m_retryOperationCount == 0)
            {
                _hwFile->m_retryOperationCount ++;
                DVDCancel((DVDCommandBlock *) &_hwFile->m_FileInfo);
                return DVDReadAsyncWait(_hwFile, _addr, _length, _offset);
            }
        }
        else
        {
            _hwFile->m_retryOperationCount = 0;
        }

        return (s32_Status == DVD_STATE_END) ? ((req.mi32_ReadResult > 0) ? req.mi32_ReadResult : 0) : s32_Status;
    }

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	s32 hwFile::DVDReadToCache(s32 _length, s32 _offset)
	{
		ITF_ASSERT_CRASH((_length + _offset) <= ((getLength() + 31) & 0xFFFFFFE0), "Bad alignment");

		// Remember what's in the cache.
		m_FileCache.m_cachepos	= _offset;
		m_FileCache.m_cachesize	= _length;

		// Start address in the cache.
		void *addr = m_FileCache.m_cache + m_FileCache.m_CacheBlockSize - _length;

		updateCacheCount();

		s32 readsize = DVDReadAsyncWait(this, addr, _length, _offset);
		if(readsize < 0)
			return 0;
		return readsize;
	}

	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	s32 hwFile::DVDReadFromCache(i32 *_piSize, u8 **_ppDest)
	{
		//
		// Either empty cache (*_piSize > m_FileCache.m_cachecount)
		// or read all data from cache (m_FileCache.m_cachecount >= *_piSize)
		//
		u32 uCopySize = Min(m_FileCache.m_cachecount, *((u32 *)_piSize));

		// Copy the data from the cache to the read buffer
		ITF_Memcpy(*_ppDest, m_FileCache.m_cache + m_FileCache.m_CacheBlockSize - m_FileCache.m_cachecount, uCopySize);

		// Update sizes and buffers
		*_piSize -= uCopySize;
		*_ppDest += uCopySize;
		m_FileCache.m_cachecount -= uCopySize;
		m_FileCache.m_readpos += uCopySize;
		
		return uCopySize;
	}


	/*
	=======================================================================================================================
	=======================================================================================================================
	*/
	String8 hwFile::getNANDHomeDir()
	{
		static String8 NANDHomeDir ("");
		if(NANDHomeDir.isEmpty())
		{
			char buffer [NAND_MAX_PATH];
			if ( NANDGetHomeDir(buffer) == NAND_RESULT_OK )
			{				
				NANDHomeDir = buffer;
			} 
		}
		return NANDHomeDir;
	}

    /*
	=======================================================================================================================
	=======================================================================================================================
	*/
    bbool hwFile::checkAvailableSpace(u32 fsblock, u32 inode, u32* answer)
    {
        if (NANDCheck((unsigned long)fsblock, (unsigned long)inode, (unsigned long*)answer) == NAND_RESULT_OK)
        {
            if (*answer == 0)
                return btrue;
        }
        return bfalse;
    }

    /*
	=======================================================================================================================
	=======================================================================================================================
	*/
#ifdef ITF_DEBUG
#define LOG_NAND LOG
#else //ITF_DEBUG
#define LOG_NAND(...)  
#endif //ITF_DEBUG

	NandFile::NandFile() : File(), m_isOpened(bfalse)
	{
		ITF_MemSet(&m_fileInfo,0,sizeof(NANDFileInfo));
	}

	NandFile::~NandFile() 
	{
		if (m_isOpened)
			close();
	}

	bbool NandFile::open(const String& path, u32 attribute, u32 _flagsAttribute)
	{
		ITF_ASSERT(!m_isOpened);
		ITF_ASSERT_CRASH(path.getLen() <= 8+1+3, "NAND file names must respect the 8.3 file name format"); 
		if (m_isOpened)
		{
			close();
			m_isOpened = bfalse;
		}

		String8 name;
		if (_flagsAttribute & ITF_FILE_FLAGS_ATTR_TMP)
		{
			name = "/tmp/";
			StringConverter str(path.cStr());
			name += str.getChar();
		}
		else 
			name = path.cStr();

#ifndef ITF_FINAL
		m_name = name;
#endif //ITF_FINAL

		if (attribute & ITF_FILE_ATTR_CREATE_NEW)
		{
			if (!create(name.cStr(),attribute))
				return bfalse;
		}

		u32 access = 0;
		if (attribute & ITF_FILE_ATTR_READ)
			access |= NAND_ACCESS_READ;
		if (attribute & ITF_FILE_ATTR_WRITE)
			access |= NAND_ACCESS_WRITE;
		s32 error = NANDOpen(name.cStr(), &m_fileInfo, access);
		switch (error)
		{
		case NAND_RESULT_OK:
			LOG_NAND("Successfully opened file: %s.\n", name.cStr());
			m_isOpened = btrue;
			return btrue;
		case NAND_RESULT_NOEXISTS: 
			LOG("Can't open file %s, it doesn't exist.\n", name.cStr()); 
			break; 
		default:
			LOG("Failed to open file (error %d): %s .\n", error, name.cStr());
			break;
		}

		return bfalse;
	}

	bbool NandFile::read(void *buffer, u32 size, u32* _sizeRead)
	{
		LOG_NAND("reading %u bytes to [0x%08x] from NAND file %s.\n",size,buffer,m_name.cStr());
		ITF_ASSERT_CRASH(!((u32)buffer & 0x1F), "buffer address must 32-byte aligned");
		ITF_ASSERT_CRASH(!((u32)size & 0x1F), "buffer size must 32-byte aligned");
		ITF_ASSERT_CRASH(m_isOpened,"Cannot read file, it's not open");

		s32 readSize = NANDRead(&m_fileInfo, buffer, size);
		if (_sizeRead)
			*_sizeRead = readSize;
		
		if (readSize != size)
		{
			LOG_NAND("Failed to read file: %s.\n", m_name.cStr());
		}
		else
		{
			LOG_NAND("Successfully read %u bytes from file: %s.\n", size, m_name.cStr());
		}
		return (readSize == size);
	}

	bbool NandFile::read(void *buffer, u32 size)
	{
		return read(buffer, size,NULL);
	}

	bbool NandFile::write(const void *buffer, u32 size, u32* sizeWritten)
	{
		LOG_NAND("writing %u bytes of [0x%08x] to NAND file %s.\n",size,buffer,m_name.cStr());	
		ITF_ASSERT_CRASH(!((u32)buffer & 0x1F), "buffer must 32-byte aligned");
		ITF_ASSERT_CRASH(!((u32)size & 0x1F), "buffer must 32-byte aligned");
		ITF_ASSERT_CRASH(m_isOpened,"Cannot write, file is not open" );

		if (size && buffer)
		{
			s32 writeSize = NANDWrite(&m_fileInfo, buffer, size);

			if (sizeWritten)
				*sizeWritten = writeSize; 

			if (size == writeSize)
			{
				LOG_NAND("Successfully written %u bytes to file: %s.\n", writeSize, m_name.cStr());
				return btrue;
			}
			else
			{
				LOG_NAND("Failed to write to file: %s.\n", m_name.cStr());
			}
		}
		else if (!buffer)
			LOG_NAND("An invalid buffer [0x%08x] was set for file %s.\n",buffer,m_name.cStr());
		else if (!size)
			LOG_NAND("An invalid size (%u bytes) was set for file %s.\n",size,m_name.cStr());

		return bfalse;
	}

	bbool NandFile::write(const void *buffer, u32 size)
	{
		return write(buffer,size,NULL);
	}

	void NandFile::close()
	{
		ITF_ASSERT_CRASH(m_isOpened,"File already closed");

		if (m_isOpened)
		{
			s32 success = NANDClose(&m_fileInfo);

			if (success != NAND_RESULT_OK)
			{
				LOG_NAND("Failed to close file: %s.\n", m_name.cStr());
			}
			else
			{
				LOG_NAND("Successfully closed file: %s.\n", m_name.cStr());
			}
		}
		else
			LOG_NAND("File was already closed: %s.\n", m_name.cStr());
		m_isOpened = bfalse;
	}

	u64 NandFile::getLength()
	{
		ITF_ASSERT_CRASH(m_isOpened,"File must be open to ask length");

		unsigned long length;
		s32 error = NANDGetLength(&m_fileInfo, &length);
		if (error == NAND_RESULT_OK)
			return (u64) length;
		else
			return 0;
	}

	u64 NandFile::seek(i64 offset, u16 from)
	{
		ITF_ASSERT(m_isOpened);
		i32 position = NANDSeek(&m_fileInfo, (i32) offset, from);
		ITF_ASSERT(position >= 0);
		if (position < 0)
		{
			LOG("Failed to seek file: %s.\n",m_name.cStr());
		}
		else
		{
			LOG_NAND("Successfully sought file to position %u: %s.\n",position, m_name.cStr());
		}
		return (u64)position;
	}

	u64 NandFile::tell()
	{
		ITF_ASSERT(m_isOpened);
		unsigned long position;
		i32 success	= NANDTell(&m_fileInfo, &position);
		ITF_ASSERT(success == NAND_RESULT_OK);
		if (success != NAND_RESULT_OK)
		{
			LOG("Failed to tell seek position of file: %s.\n",m_name.cStr());
			return 0;
		}
		else
		{
			LOG_NAND("Successfully told seek position %u of file %s.\n", position, m_name.cStr());
			return (u64)position;
		}
		return 0;
	}

	bbool NandFile::create(const char *name, u32 attribute)
	{
		u32 perm = NAND_PERM_OWNER_READ; // The read permission must be granted (cf API doc)
		if (attribute & ITF_FILE_ATTR_WRITE)
			perm |= NAND_PERM_OWNER_WRITE;

		//Creating the file
		s32 createSuccess = NANDCreate(name,perm,0);
		if (createSuccess == NAND_RESULT_OK || createSuccess == NAND_RESULT_EXISTS)
		{
			if (createSuccess == NAND_RESULT_OK)
				LOG_NAND("Successfully created file: %s.\n", name.c_str());
			else
				LOG_NAND("File already created: %s.\n", name.c_str());
			return btrue; 
		}
		else
		{
			LOG_NAND("File could not be created (error %d): %s.\n", createSuccess, name.c_str());
			return bfalse;
		}
	}

} // namespace ITF
