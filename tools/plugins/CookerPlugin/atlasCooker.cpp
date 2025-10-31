#include "core/enginecommon.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ATLASCOOKER_H_
#include "atlasCooker.h"
#endif //_ITF_ATLASCOOKER_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


namespace ITF
{
	bbool atlasCooker::cookFile(const String& _szPlatform,const String& _relativeFilename)
	{
		return atlasCooker::cookFile( _szPlatform, _relativeFilename, bfalse);
	}

    bbool atlasCooker::cookFile(const String& _vPlatform,const String& _relativeFilename, bbool _bRaiseError)
    {
        //assume file is lowed
       
        LOG_COOKER("[ATLAS_COOKER] %ls  COOKING", _relativeFilename.cStr());

		String normalizedPath   = FilePath::normalizePath(_relativeFilename);
		String cookFile         = FILESERVER->getCookedNamePlatform(_relativeFilename,_vPlatform);

        bbool bResult = btrue;

		if (cookerFactory::fileNeedsCook(_relativeFilename,cookFile,_vPlatform,Versioning::atlasVer()))
		{
			File* atlasFile = FILEMANAGER->openFile(_relativeFilename,ITF_FILE_ATTR_READ);

			UVAtlas uvAtlas;

			if (atlasFile)
			{
				u32 fileSize = (u32)atlasFile->getLength();
				char * readBuffer = newAlloc(mId_Temporary,char[fileSize+1]);
				u32 sizeRead = 0;
				atlasFile->read(readBuffer,fileSize,&sizeRead);
				ITF_ASSERT(fileSize == sizeRead);
				readBuffer[fileSize] = '\0';
				FILEMANAGER->closeFile(atlasFile);
				LUA_HANDLERLOADING->executeString(readBuffer);
				SF_DEL_ARRAY(readBuffer);
			}
			else
			{
				LOG_COOKER("[ATLAS_COOKER]  %ls  COOK FAILED\n", _relativeFilename.cStr());
				bResult = bfalse;
			}

			lua_State* L = LUA_HANDLERLOADING->getLuaState();

			i32 numUV;
			if (LUAHandler::openGlobalTable(L, "ITF_GFX_UV_ATLAS", numUV))
			{
				for (i32 i = 0; i < numUV; ++i)
				{
					if (LUAHandler::openTable(L,i+1))
					{
						UVdata data;
						u32 index = LUAHandler::getU32FromTable(L,"index", 0);
						Vec2d uv0, uv1;
						Vec2d p1;
						data.setUV0(LUAHandler::getVec2dFromTable(L,"uv0", uv0));
						data.setUV1(LUAHandler::getVec2dFromTable(L,"uv1", uv1));
						LUAHandler::closeTable(L);
						uvAtlas.addUVdata(index, data);
					}
				}

				LUAHandler::closeGlobalTable(L);
			}
	        

			ArchiveMemory amw;
			amw.createLinker();
			uvAtlas.serialize(amw);

			File* writeBinary = FILEMANAGER->openFile(cookFile,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

			if (writeBinary)
			{
				writeBinary->write(amw.getData(),amw.getSize());
				FILEMANAGER->closeFile(writeBinary);
				LOG_COOKER("[ATLAS_COOKER] %ls  COOKED   \n", _relativeFilename.cStr());

				FILEMANAGER->flushTimeWriteAccess(cookFile,FILEMANAGER->getLastTimeWriteAccess(_relativeFilename));
			}
            else
            {
                LOG_COOKER("[ATLAS_COOKER] %ls  COOK FAILED\n", _relativeFilename.cStr());
			    bResult = bfalse;
            }
		}

		return bResult;
    }

}