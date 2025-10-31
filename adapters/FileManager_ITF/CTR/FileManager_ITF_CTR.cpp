#include "precompiled_FileManager_ITF.h"

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/CTR/FileManager_ITF_CTR.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEPACK_H_
#include "engine/file/FilePack.h"
#endif //_ITF_FILEPACK_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#ifndef _ITF_FILETRACKER_H_
#include "engine/file/FileTracker.h"
#endif //_ITF_FILETRACKER_H_

namespace ITF
{
    FileManager_ITF_CTR::FileManager_ITF_CTR()
    {
        nn::fs::Initialize();

        const size_t ROMFS_BUFFER_SIZE = 1024 * 64;
        static char buffer[ROMFS_BUFFER_SIZE];
        NN_UTIL_PANIC_IF_FAILED(
        nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE,false));
    }

    FileManager_ITF_CTR::~FileManager_ITF_CTR()
    {
    }

    bbool FileManager_ITF_CTR::fileExists(const String& _szFileName)
	{
		 BundleFile* pBundleFile = getBundle();
		if (pBundleFile && pBundleFile->fileExists(_szFileName))
			return btrue;
		return hwFile::exists(_szFileName);
	}
}