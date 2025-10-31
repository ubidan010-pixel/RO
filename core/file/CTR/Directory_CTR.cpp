#include "precompiled_core.h"

#include "core/file/Directory.h"
#include "File_CTR.h"

//there are 2 bundles 
// -application bundle where all files are readonly
// -settings bundle where file can be created/read/changed
//directories will be created in settings bundle

namespace ITF
{
	bbool Directory::create(const String& dirName)
	{
		/*
		NSString* settingsPath = hwFile::GetSettingsBunddleFullName(dirName);
				NSFileManager* pFileManager = [NSFileManager defaultManager];
				[pFileManager createDirectoryAtPath:settingsPath withIntermediateDirectories:YES attributes:nil error:nil];*/
		

		return DirectoryExists(dirName);
	}

    bbool Directory::HasDot(const FILE_FIND_DATA& t_Data)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPAd");
		return btrue;
    }

    HANDLE Directory::FindFirstFileStr(const String& _pz_FileName, FILE_FIND_DATA *_pt_Data)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
		return NULL;
    }

    bbool Directory::exists(const String& _fulldirName)
    {
        return DirectoryExists(_fulldirName);
    }

    bbool Directory::DirectoryExists(const String& _fulldirName)
    {
/*
		NSString*	settingsPath = hwFile::GetSettingsBunddleFullName(_fulldirName);
		NSString*	resourcePath = hwFile::GetApplicationBunddleFullName(_fulldirName);
		BOOL		isDirectory;

		NSFileManager* pFileManager = [NSFileManager defaultManager];
		
		if([pFileManager fileExistsAtPath:settingsPath isDirectory:&isDirectory])
		{
			if(isDirectory)
			{
				return btrue;
			}
		}
		
		if([pFileManager fileExistsAtPath:resourcePath isDirectory:&isDirectory])
		{
			if(isDirectory)
			{
				return btrue;
			}
		}
*/
		
		return bfalse;
    }
}