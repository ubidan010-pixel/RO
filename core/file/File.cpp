#include "precompiled_core.h"

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/wii/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif //ITF_WII

namespace ITF
{
    OnFileOperationResultCallback_t File::_onFileOperationResultCallback = NULL;

#ifdef ITF_SUPPORT_CHEAT
    i32                             File::_simulateSeekTime = 0;
#endif //ITF_SUPPORT_CHEAT

    FileInfo::FileInfo()
    {
        ITF_MemSet(this, 0, sizeof(FileInfo));
    }

    void FileInfo::serialize(Blob &_blob)const
    {
        _blob.pushBool(m_bExist);
        _blob.pushUInt64(m_LastUpdateTime);
    }

    void FileInfo::deserialize(Blob &_blob)
    {
        m_bExist            = _blob.extractBool();
        m_LastUpdateTime    = _blob.extractUint64();
    }


    i32 File::callOnResultCallback(i32 res, FileOperation op)
    {
        if(_onFileOperationResultCallback)
            return _onFileOperationResultCallback(op, res);
        //ITF_ASSERT_MSG(0, "You should set a callback on all IO access. If you already did, do not try an IO operation before its registration");
        return res;
    }

    OnFileOperationResultCallback_t File::setOnFileOperationResultCallback( OnFileOperationResultCallback_t callback )
    {
        OnFileOperationResultCallback_t oldCallback = _onFileOperationResultCallback;
        _onFileOperationResultCallback = callback;

        return oldCallback;
    }

	bbool File::fileToBuffer(const char *name, void*& buffer, u32 &size, u32 fileAttribute, u32 memFlag)
	{
		if (!open(name,ITF_FILE_ATTR_READ, fileAttribute))
			return bfalse;

		size = (u32) getLength();
		if (size)
		{
#ifdef ITF_WII
			size = OSRoundUp32B(size);
			MEM_M_PushExt(memFlag);
			buffer = ITF::Memory::alignedMalloc(size,32);
			MEM_M_PopExt();
#else
			buffer = ITF::Memory::malloc(size);
#endif 
			if (read(buffer,size))
			{
				close();
				return true;
			}
		}

		close();
		return false;
	}

	bbool File::bufferToFile(const char *name, void* buffer, u32 size, u32 fileAttribute)
	{
		if (!open(name,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW,fileAttribute))
			return bfalse;

		bbool success = write(buffer,size);
		close();
		return success;
	}

} // namespace ITF