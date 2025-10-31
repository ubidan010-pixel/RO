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

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


//ENABLE THIS FLAGS TO BREAK MULTIPLE ACCESS
//#define TRACK_FILE_VIOLATION 1


namespace ITF
{
	hwFile::hwFile()
	{
	}

	hwFile::~hwFile()
	{
	}

    static String normalizeCTRPath(const String &_path0)
    {
        String path=_path0;
        path.replace('\\', '/');
        if (path.getLen()>0)
        {
            if (path.cStr()[0]==(u16)L'/')
            {
                path = String("rom:")+path;
            }
            else
            {
                path = String("rom:/")+path;
            }            
        }

        //sessediki: no need to lower path name!!
        //path.toLower();
        return path;
    }

	bbool hwFile::open(const String& path0, u32 attribute,u32 _flagsAttribute)
	{
        if (attribute & ITF_FILE_ATTR_WRITE)
        {
            return bfalse;
        }

        m_file.Finalize();
        String path=normalizeCTRPath(path0);
        nn::Result r = m_file.TryInitialize((wchar_t*)path.cStr());
        if (r.IsFailure())
        {
            LOG("Could not open file %ls", (wchar_t*)path.cStr());
        }
        return r.IsSuccess();
	}

	void hwFile::close()
	{
        m_file.Finalize();		
	}

	u64 hwFile::getLength()
	{
        return (u64)m_file.GetSize();	
    }

	bbool hwFile::read(void *pBuffer, u32 size,u32* sizeRead)
	{
        *sizeRead = (u32)m_file.Read(pBuffer, size);
        return *sizeRead == size;
	}

	bbool hwFile::read(void *pBuffer, u32 size)
	{
		u32 sizeRead;
		return read(pBuffer, size, &sizeRead);
	}

	bbool hwFile::write(const void *pBuffer, u32 size, u32* sizeWritten)
	{
		return bfalse;
	}

	bbool hwFile::write(const void *pBuffer, u32 size)
	{
        return bfalse;
	}

	u64 hwFile::seek(i64 offset, u16 from)
	{
        nn::fs::PositionBase base;

        switch(from)
        {
        case FILE_CURRENT: base = nn::fs::POSITION_BASE_CURRENT; break;
        case FILE_BEGIN : base = nn::fs::POSITION_BASE_BEGIN; break;
        case FILE_END : base = nn::fs::POSITION_BASE_END; break;
        default: base = nn::fs::POSITION_BASE_BEGIN; ITF_ASSERT_MSG(bfalse,"unknown argument"); break;
        }

        m_file.Seek(offset, base);
        return (u64)m_file.GetPosition();		
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

	bbool hwFile::exists(const String& _fullfileName0)
	{
        nn::fs::FileInputStream f;
        String path = normalizeCTRPath(_fullfileName0);
        nn::Result r = f.TryInitialize((wchar_t*)path.cStr());
        return (bbool)r.IsSuccess();
	}

	bbool hwFile::deleteFile(const String& _fullfilePath)
	{
		ITF_ASSERT_MSG(0, "Can't delete files on CTR");
		return bfalse;
	}

	u64 hwFile::getFileSize(const String& _fullfilePath)
	{
        nn::fs::FileInputStream f;

        nn::Result r = f.TryInitialize((wchar_t*)_fullfilePath.cStr());
        if (r.IsFailure())
            return 0;
        
        return (u64)f.GetSize();
	}
	
}