#include "core/enginecommon.h"

#include "textureCooker_3DS.h"

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_


#include "TextureCookerCTR.h"


namespace ITF
{

   bbool textureCooker_3DS::compress(const String& _szFilenameSRC,const String& _szFilenameDST)
   {
	   String commandline;
	   String fullFilePathDST;
	   String fullFilePathSRC;
	   TextureCookerCTR _textureCooker;

	   //here dstFile is the temporary dds file
	   FILEMANAGER->TranslatePath(fullFilePathDST, _szFilenameDST);
	   FILEMANAGER->TranslatePath(fullFilePathSRC, _szFilenameSRC);
	 /*  StringConverter destAbsolute(fullFilePathDST);*/

	   
	  fullFilePathSRC.charCopyToBuffer(_textureCooker.m_pathFileSrc,512);
	  fullFilePathDST.charCopyToBuffer(_textureCooker.m_pathFileDst,512);
	  
	  _textureCooker.Cooking();
	  return btrue;

   }

}