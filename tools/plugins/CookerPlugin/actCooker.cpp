#include "core/enginecommon.h"

#include "actCooker.h"

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
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

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_ZSERIALIZER_DEP_H_
#include "core/serializer/ZSerializerDep.h"
#endif //_ITF_ZSERIALIZER_DEP_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_COMMON_DEP_FUNCTIONS_H_
#include "tools/plugins/CookerPlugin/CommonDepFunctions.h"
#endif //_ITF_COMMON_DEP_FUNCTIONS_H_

namespace ITF
{
    bbool actCooker::cookFile(const String& _vPlatform,const String& _lowerName)
    {
        //assume file is lowed
        bbool bResult = bfalse;
        LOG_COOKER("[ACT_COOKER] %ls  COOKING", _lowerName.cStr());

        // No targeted big endian platform for the moment...
        if (_vPlatform == "UNKNOWN")
            LOGICDATABASE->setBigEndian(btrue);
        else
            LOGICDATABASE->setBigEndian(bfalse);
     
       bResult = LOGICDATABASE->cookFile(_lowerName,_vPlatform);
       LOG_COOKER("[ACT_COOKER] %ls  %ls   \n", _lowerName.cStr(), bResult ? L"COOKED" : L"FAILED" );
       return bResult;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool actCooker::actDependenciesFile( const String& cacheKey, const String& _vPlatform, const String& _szFilename, DepCollection& _vDependencies )
    {
        if ( _vDependencies.load( cacheKey, _szFilename, _vPlatform ) )
        {
            _vDependencies.filterExt();
            return true;
        }

        if ( LOGICDATABASE->createFile( _szFilename, _vPlatform, bfalse ) == NULL )
            return bfalse;

        CSerializerDep< CSerializerObjectLogicDB > dbSerializer;

        dbSerializer.init( _szFilename, _vPlatform );
        if ( !dbSerializer.isValid() || dbSerializer.isEmpty() )
            return bfalse;

        Actor_Template tpl;

        tpl.Serialize( &dbSerializer, ESerialize_Data_Load );
        LOGICDATABASE->clear();
        //if ( !cookFile( _vPlatform, _szFilename ) )
        //    return bfalse;

        //CSerializerDep< CSerializerObjectBinaryFile > serializer;

        //serializer.open( _szFilename, _vPlatform, btrue );
        //if ( !serializer.isValid() )
        //    return bfalse;

        //Actor_Template tpl;

        //tpl.Serialize( &serializer, ESerialize_Data_Load );
        retrieveLocalisationDependencies( dbSerializer, _vDependencies );
        LOGICDATABASE->dependenciesFile( _vPlatform, _szFilename, _vDependencies );
        _vDependencies.save( cacheKey, _szFilename, _vPlatform );
        _vDependencies.filterExt();
        return btrue;
    }

    bbool actCooker::dependenciesFile( const String& cacheKey, const String& _vPlatform, const String& _szFilename, DepCollection& _vDependencies )
    {
        if ( _vDependencies.load( cacheKey, _szFilename, _vPlatform ) )
        {
            _vDependencies.filterExt();
            return true;
        }

        LOGICDATABASE->dependenciesFile( _vPlatform, _szFilename, _vDependencies );
        _vDependencies.save( cacheKey, _szFilename, _vPlatform );
        _vDependencies.filterExt();
        return btrue;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------
}
