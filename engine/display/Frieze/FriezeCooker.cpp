#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_EDITOR

#ifndef ITF_CTR

#ifndef _ITF_FRIEZECOOKER_H_
#include "engine/display/frieze/FriezeCooker.h"
#endif //_ITF_FRIEZECOOKER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{


bbool FriseConfigCooker::cookFile(const String& _szPlatform,const String& _relativeFilename)
{
    return FriseConfigCooker::cook( _szPlatform, _relativeFilename, bfalse);
}


bbool FriseConfigCooker::cook(const String& _szPlatform,const String& _relativeFilename, bbool _bRaiseError)
{
    String normalizedPath   = FilePath::normalizePath(_relativeFilename);
    String cookFile         = FILESERVER->getCookedNamePlatform(_relativeFilename,_szPlatform);

    bbool bResult = btrue;

    if (cookerFactory::fileNeedsCook(normalizedPath,cookFile,_szPlatform,Versioning::friezeConfigVer()))
    {
        FriseConfig* config = XMLAll::loadFriseConfig(normalizedPath, bfalse, btrue);
     
        if (!config)
        {
            bResult = bfalse;
        }
        else
        {
            ArchiveMemory* _ArchiveMem = newAlloc(mId_Frieze, ArchiveMemory());
            config->serialize(*_ArchiveMem);

            //save to a file;
            File* writeFile = FILEMANAGER->openFile(cookFile,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

            if (writeFile)
            {
                writeFile->write(_ArchiveMem->getData(),_ArchiveMem->getSize());
                FILEMANAGER->closeFile(writeFile);
                writeFile = NULL;
            }

            SF_DEL(_ArchiveMem);

            FILEMANAGER->flushTimeWriteAccess(cookFile,FILEMANAGER->getLastTimeWriteAccess(normalizedPath));

        }


    } 

    String result;
    result.setTextFormat("[FCG_COOKER] %s %s", StringToUTF8(_relativeFilename).get(), bResult ? "COOKED" : "FAILED" );
    LOG_COOKER("%s", StringToUTF8(result).get());
 
    return bResult;
}


};

#endif // ITF_SUPPORT_EDITOR


#endif
