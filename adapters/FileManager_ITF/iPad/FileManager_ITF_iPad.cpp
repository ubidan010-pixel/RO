#include "precompiled_FileManager_ITF.h"

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/iPad/FileManager_ITF_iPad.h"
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
	bbool FileManager_ITF_iPad::fileExists(const String& _szFileName)
	{
		if (m_pBundle && m_pBundle->fileExists(_szFileName))
			return btrue;
		return hwFile::exists(_szFileName);
	}
}