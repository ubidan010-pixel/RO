#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

//ENABLE THIS FLAGS TO BREAK MULTIPLE ACCESS
//#define TRACK_FILE_VIOLATION 1


namespace ITF
{
	hwFile::hwFile()
	{
		m_Handle = NULL;
	}

	hwFile::~hwFile()
	{
		close();
	}

	bbool hwFile::open(const String& path, u32 attribute,u32 _flagsAttribute)
	{
		close();
		NSString* resourcePath = GetApplicationBunddleFullName(path);
		NSString* settingsPath = GetSettingsBunddleFullName(path);
		if(attribute & ITF_FILE_ATTR_CREATE_NEW)
		{
			String			dir = FilePath::getDirectory(path);
			NSString*		settingsPathDir = hwFile::GetSettingsBunddleFullName(dir);
			NSFileManager*	pFileManager = [NSFileManager defaultManager];
			BOOL			isDirectory;
			BOOL			exists;
			
			exists = [pFileManager fileExistsAtPath:settingsPathDir isDirectory:&isDirectory];
			
			if(!exists || (exists && !isDirectory))
			{
				Directory::create(dir);
			}
			
			[pFileManager createFileAtPath: settingsPath contents:NULL attributes:nil];
		}
		
		if((attribute & (ITF_FILE_ATTR_WRITE | ITF_FILE_ATTR_READWRITE)) && (attribute != ITF_FILE_ATTR_READ))
		{
			m_Handle = [NSFileHandle fileHandleForUpdatingAtPath: settingsPath];
		}
		else if(attribute & ITF_FILE_ATTR_READ)
		{
			m_Handle = [NSFileHandle fileHandleForReadingAtPath: settingsPath];
		   if(!m_Handle)
		   {
			   m_Handle = [NSFileHandle fileHandleForReadingAtPath: resourcePath];
		   }
		}
		
		if(!m_Handle)
		{
			SYSTEM_ADAPTER->messageBox(String("File not found:"), path, 0);
		}

		return m_Handle != NULL;
	}

	void hwFile::close()
	{
		if(m_Handle)
		{
			[(NSFileHandle*)m_Handle closeFile];
			m_Handle = NULL;
		}
	}

	u64 hwFile::getLength()
	{
		ITF_ASSERT_MSG(m_Handle, "File not opened!");
		
		u64		pointer = [(NSFileHandle*)m_Handle offsetInFile];
		u64		size	= [(NSFileHandle*)m_Handle seekToEndOfFile];
		
		[(NSFileHandle*)m_Handle seekToFileOffset:pointer];
		
		return size;
	}

	bbool hwFile::read(void *pBuffer, u32 size,u32* sizeRead)
	{
		ITF_ASSERT_MSG(m_Handle, "File not opened!");
		ITF_ASSERT_MSG(pBuffer && size, "Invalid read buffer/size");
		
		NSData* readData = [(NSFileHandle*)m_Handle readDataOfLength: size];
		
		*sizeRead = [readData length];
		if(*sizeRead)
		{
			[readData getBytes: pBuffer length: *sizeRead];
		}
		
		return *sizeRead == size;
	}

	bbool hwFile::read(void *pBuffer, u32 size)
	{
		u32 sizeRead;
		return read(pBuffer, size, &sizeRead);
	}

	bbool hwFile::write(const void *pBuffer, u32 size, u32* sizeWritten)
	{
		ITF_ASSERT_MSG(m_Handle, "File not opened!");
		ITF_ASSERT_MSG(pBuffer && size, "Invalid write buffer/size");
		
		NSData* writeData = [NSData dataWithBytes:pBuffer length:size];
		ITF_ASSERT_MSG(writeData, "Failed when creating writeData");

		[(NSFileHandle*)m_Handle writeData: writeData];
		
		*sizeWritten = size;
		
		return true;
	}

	bbool hwFile::write(const void *pBuffer, u32 size)
	{
		u32 sizeWritten;
		return write(pBuffer, size, &sizeWritten);
	}

	u64 hwFile::seek(i64 offset, u16 from)
	{
		ITF_ASSERT_MSG(m_Handle, "File not opened!");
		switch(from)
		{
			case FILE_CURRENT:
				[(NSFileHandle*)m_Handle seekToFileOffset: [(NSFileHandle*)m_Handle offsetInFile] + offset];
				break;
			case FILE_BEGIN:
				[(NSFileHandle*)m_Handle seekToFileOffset: offset];
				break;
			case FILE_END:
				[(NSFileHandle*)m_Handle seekToFileOffset: getLength() - offset];
				break;
		}
		
		return [(NSFileHandle*)m_Handle offsetInFile];
	}

	FILE_HANDLE hwFile::getHandle()
	{
		return m_Handle;
	}

	bbool hwFile::CopyFile(const String& szExistingFilename,const String& szNewFilename)
	{
		ITF_ASSERT_MSG(0, "Can't copy files on iPad");
		return bfalse;
	}

	bbool hwFile::MoveFile(const String& szExistingFilename,const String& szNewFilename)
	{
		ITF_ASSERT_MSG(0, "Can't move files on iPad");
		return bfalse;
	}

	u64 hwFile::getLastTimeWriteAccess(const String& szExistingFilename)
	{
		return 0;
	}

	void hwFile::flushTimeWriteAccess(const String& szExistingFilename, const u64& _lastWrite)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	bbool hwFile::exists(const String& _fullfileName)
	{
		NSString*	settingsPath = GetSettingsBunddleFullName(_fullfileName);
		NSString*	resourcePath = GetApplicationBunddleFullName(_fullfileName);
		BOOL		isDirectory;
		
		NSFileManager* pFileManager = [NSFileManager defaultManager];
		
		if([pFileManager fileExistsAtPath:settingsPath isDirectory:&isDirectory])
		{
			if(!isDirectory)
			{
				return btrue;
			}
		}
		
		if([pFileManager fileExistsAtPath:resourcePath isDirectory:&isDirectory])
		{
			if(!isDirectory)
			{
				return btrue;
			}
		}
		
		return bfalse;
	}

	bbool hwFile::deleteFile(const String& _fullfilePath)
	{
		ITF_ASSERT_MSG(0, "Can't delete files on iPad");
		return bfalse;
	}

	u64 hwFile::getFileSize(const String& _fullfilePath)
	{
		hwFile file;
		file.open(_fullfilePath, ITF_FILE_ATTR_READ);
		
		return file.getLength();
	}
	
	NSString* hwFile::GetApplicationBunddleFullName(const String& path)
	{
		NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
		NSString *fullPath;
		if(path.cStr()[0] == '/')
		{
			fullPath = bundlePath;
		}
		else
		{
			fullPath = [bundlePath stringByAppendingString:@"/"];
		}
		NSString *temp = [NSString stringWithFormat:@"%ls", path.cStr()];
		NSString *low = [temp lowercaseString];
		fullPath = [fullPath stringByAppendingString:low];
	
		return fullPath;
	}
	
	NSString* hwFile::GetSettingsBunddleFullName(const String& path)
	{
		NSString *bundlePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES) objectAtIndex:0];
		NSString *fullPath;
		if(path.cStr()[0] == '/')
		{
			fullPath = bundlePath;
		}
		else
		{
			fullPath = [bundlePath stringByAppendingString:@"/"];
		}
		NSString *temp = [NSString stringWithFormat:@"%ls", path.cStr()];
		NSString *low = [temp lowercaseString];
		fullPath = [fullPath stringByAppendingString:low];
		
		return fullPath;
	}
}