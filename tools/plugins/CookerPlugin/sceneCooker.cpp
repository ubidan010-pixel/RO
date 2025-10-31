#include "core/enginecommon.h"

#include <algorithm>

#include "sceneCooker.h"

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_
#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_ANIMATIONCOOKER_H_
#include "tools/plugins/CookerPlugin/AnimationCooker.h"
#endif //_ITF_ANIMATIONCOOKER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_MUSICMANAGER_H_
#include "rayman/gameplay/Ray_MusicManager.h"
#endif //_ITF_RAY_MUSICMANAGER_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

namespace ITF
{

static bbool resolveMusicTheme( World* world, Path& themePath )
{
    StringID id;

    for ( u32 i = 0; i < world->getSceneCount(); ++i )
    {
        Scene* scene = world->getSceneAt( i );

        if ( scene->getMusicTheme() != StringID::Invalid )
        {
            if ( !id.isValid() )
            {
                id = scene->getMusicTheme();
                continue;
            }
            ITF_ASSERT_MSG( id == scene->getMusicTheme(), "Music theme id conflict" );
        }
    }
    if ( id != StringID::Invalid )
    {
        const ITF_MAP< StringID, u32 >&             themes = GAMEMANAGER->getMusicThemes();
        ITF_MAP< StringID, u32 >::const_iterator    it = themes.find( id );

        if ( it != themes.end() )
        {
            themePath = GAMEMANAGER->getMusicPath( it->second );
            return btrue;
        }
    }
    return bfalse;
}

enum MapType
{
    UnmanagedMap = 0,
    GameplayMap,
    WorldMap,
    LoadingScreen,
};

static bool findPathFrom( const String& lowerFilename, ITF_VECTOR< String > paths )
{
    for ( u32 i = 0; i < paths.size(); ++i )
        paths[i].toLower();

    for ( u32 i = 0; i < paths.size(); ++i )
        if ( lowerFilename == paths[i] )
            return true;
    return false;
}

static MapType getMapType( const String& filename )
{
    String lower = filename;

    lower.toLower();

    PathString_t pathName;

    GAMEMANAGER->getWorldMap().getString(pathName);

    if ( pathName == lower )
        return WorldMap;

    ITF_VECTOR< String > paths;

    RAY_GAMEMANAGER->getMissionMaps( paths );
    if ( findPathFrom( lower, paths ) )
        return GameplayMap;
    RAY_GAMEMANAGER->getLoadingScreens( paths );
    if ( findPathFrom( lower, paths ) )
        return LoadingScreen;
    return UnmanagedMap;
}

static void retrieveCustomCostumesDependencies( cookerFactory* factory, const String& platform, DepCollector& col )
{
    const ITF_VECTOR< PlayerIDInfo* >& ids = GAMEMANAGER->getPlayerIDInfo();
    PathString_t pathName;

    for ( u32 i = 4; i < ids.size(); ++i )
    {
        PlayerIDInfo* pInfo = ids[i];

        if ( pInfo == NULL )
            continue;

        for ( u32 j = 0; j < pInfo->getNumGameScreens(); ++j )
        {
            const PlayerIDInfo::GameScreenInfo& gsInfo = pInfo->getGameScreenInfoAt( j );

            for ( u32 k = 0; k < gsInfo.getNumActors(); ++k )
            {
                const PlayerIDInfo::ActorInfo& aInfo = gsInfo.getActorInfo( k );
                aInfo.getFile().getString(pathName);
                col.add( pathName );
            }
        }
    }
    factory->resolveDependencyFiles( platform, col );
}

static void retrievePlayableCharacterFeedbacksDependencies( cookerFactory* factory, const String& platform, DepCollector& global )
{
    static const char* s_wavExt[] =
    {
        "wav",
        NULL
    };

    DepCollector    local;
    DepCollection   col;

    // this file is hardcoded because it does not need to be managed by dependency process
    factory->getDependencyFiles( "GameConfig/costumes.isg", platform, local );
    factory->resolveDependencyFiles( platform, local );
    col = local.getProcessed();
    col.initFilters( s_wavExt );
    col.filterExt();
    global.add( col );
}

static void excludeDependencies( DepCollector& from, DepCollector& what )
{
    DepCollection::iterator end = what.getProcessed().end();

    for ( DepCollection::iterator it = what.getProcessed().begin(); it != end; ++it )
        from.getProcessed().remove( it->path );
}

static void appendDenpencies( File* file, const ITF_VECTOR< Path >& dependencies )
{
    PathString8_t pathName;

    for ( u32 i = 0; i < dependencies.size(); ++i )
    {
        dependencies[i].getString(pathName);

        file->write( pathName.cStr(), pathName.getLen() );
        file->write( "\r\n", 2 );
    }
}

static void append( const String& ext, ITF_MAP< String, ITF_VECTOR< Path > >& extSort, ITF_VECTOR< Path >& dependencies )
{
    dependencies.insert( dependencies.end(), extSort[ext].begin(), extSort[ext].end() );
    extSort.erase( ext );
}

struct FileSize
{
    Path    filename;
    u32     fileSize;
};

static bool sizeSortPred( const FileSize& left, const FileSize& right )
{
    return left.fileSize < right.fileSize;
}

static void sortStreamBySize( const String& _szPlatform, ITF_VECTOR< Path >& dependencies )
{
    ITF_VECTOR< FileSize >  files;
    String                  cookedName;
    PathString_t pathName;

    for ( u32 i = 0; i < dependencies.size(); ++i )
    {
        FileSize info;

        info.filename = dependencies[i];
        info.filename.getString(pathName);
        cookedName = FILESERVER->getCookedNamePlatform( pathName, _szPlatform, bfalse );
        if ( FILEMANAGER->fileExists( cookedName ) )
            info.fileSize = static_cast< u32 >( FILEMANAGER->getFileSize( cookedName ) );
        else
            info.fileSize = static_cast< u32 >( FILEMANAGER->getFileSize( pathName ) );
        files.push_back( info );
    }
    std::sort( files.begin(), files.end(), sizeSortPred );
    dependencies.clear();
    for ( u32 i = 0; i < files.size(); ++i )
        dependencies.push_back( files[i].filename );
}

static void sortLinear( ITF_VECTOR< Path >& dependencies )
{
    ITF_MAP< String, ITF_VECTOR< Path > > extSort;
    PathString_t pathName;

    for ( u32 i = 0; i < dependencies.size(); ++i )
    {
        dependencies[i].getString(pathName);
        extSort[FilePath::getExtension( pathName )].push_back( dependencies[i] );
    }

    dependencies.clear();
    append( "act", extSort, dependencies );
    append( "fcg", extSort, dependencies );
    append( "gmt", extSort, dependencies );
    append( "dep", extSort, dependencies );
    append( "isc", extSort, dependencies );
    append( "tsc", extSort, dependencies );
    while ( !extSort.empty() )
        append( extSort.begin()->first, extSort, dependencies );
}

static void resolveMusicThemeDependencies( const Path& themePath, DepCollector& collector )
{
    TemplateClientHandler               templateClientHandler;
    const Ray_MusicManager_Template*    config = NULL;

    templateClientHandler.addUsedTemplate( themePath.getStringID() );
    config = TEMPLATEDATABASE->getTemplate< Ray_MusicManager_Template >( &templateClientHandler, themePath, btrue );
    if ( config == NULL )
        return;

    const MusicPartSet_Template* partConfig = config->getMusicPartSetTemplate();

    if ( partConfig == NULL )
        return;

    const ITF_VECTOR< MusicPart_Template >& parts = partConfig->getMusicPartList();
    PathString_t pathName;

    for ( u32 i = 0; i < parts.size(); ++i )
    {
        parts[i].getPath().getString(pathName);
        collector.add( pathName );
    }

    config->getLumMusicManager().getString(pathName);
    collector.add( pathName );

    TEMPLATEDATABASE->unloadTemplate( themePath.getStringID() );
    templateClientHandler.freeUsedTemplates();
}

static void toVector( const DepCollection& col, ITF_VECTOR< Path >& res )
{
    DepCollection::const_iterator end = col.end();

    res.clear();
    for ( DepCollection::const_iterator it = col.begin(); it != end; ++it )
        res.push_back( Path( it->path ) );
}

static void computeLinearisationData( Scene* pScene, World* pWorld, const String& _szFilename, const String& _szPlatform )
{
    static const char* loadingFilter[] =
    {
        "imt",
        "act",
        "fcg",
        "gmt",
        "dep",
        "isc",
        "tsc",
        NULL
    };

    static const char* streamingFilter[] =
    {
        "png",
        "tga",
        "wav",
        "pbk",
        "anm",
        "skl",
        "fnt",
        NULL
    };

    cookerFactory* factory = cookerFactory::getInstance();

    ITF_ASSERT( factory != NULL );
    if ( factory == NULL )
        return;

    MapType type = getMapType( _szFilename );

    if ( type == UnmanagedMap )
        return;

    DepCollector    collector;
    Path            themePath;

    if ( resolveMusicTheme( pWorld, themePath ) )
    {
        pScene->setMusicThemePath( themePath );
        resolveMusicThemeDependencies( themePath, collector );
    }

    DepCollector customCostums;

    retrieveCustomCostumesDependencies( factory, _szPlatform, customCostums );
    factory->getDependencyFiles( _szFilename, _szPlatform, collector );
    factory->resolveDependencyFiles( _szPlatform, collector );
    if ( type == WorldMap )
        collector.add( customCostums.getProcessed() );
    else
        excludeDependencies( collector, customCostums );
    retrievePlayableCharacterFeedbacksDependencies( factory, _szPlatform, collector );
    factory->resolveDependencyFiles( _szPlatform, collector );

    DepCollection loadingCol = collector.getProcessed();
    DepCollection streamingCol = collector.getProcessed();
    PathString_t pathName;

    GAMEMANAGER->getBaseMapPath().getString(pathName);

    loadingCol.add( pathName );

    if ( !pScene->getMusicThemePath().isEmpty() )
    {
        pScene->getMusicThemePath().getString(pathName);
        loadingCol.add( pathName );
    }

    loadingCol.initFilters( loadingFilter );
    loadingCol.filterExt();

    streamingCol.initFilters( streamingFilter );
    streamingCol.filterExt();

    StringConverter     converter;
    File*               output = FILEMANAGER->openFile( FilePath::getDirectory( _szFilename ) + FilePath::getFilenameWithoutExtension( _szFilename ) + "." + _szPlatform + ".bundle", ITF_FILE_ATTR_CREATE_NEW | ITF_FILE_ATTR_WRITE );
    ITF_VECTOR< Path >& dependencies = pScene->getDependenciesRef();

    converter.setData( _szFilename );
    output->write( converter.getChar(), converter.getCharLength() );
    output->write( "\r\n", 2 );
    toVector( loadingCol, dependencies );
    sortLinear( dependencies );
    appendDenpencies( output, dependencies );

    ITF_VECTOR< Path > streamDep;

    toVector( streamingCol, streamDep );
    //sortStream( factory, _szPlatform, streamDep );
    sortStreamBySize( _szPlatform, streamDep );
    appendDenpencies( output, streamDep );

    output->close();
    SF_DEL( output );
}

bbool sceneCooker::cookFile(const String& _szPlatform,const String& _szFilename)
{
    bbool result = bfalse;
    String cookfile = FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform,bfalse);

    SCENE_MANAGER->setIsInMinimalLoadingMode(btrue); // Prevent onLoaded() call

    const String& _absoluteRootPath = FILESERVER->getWorkingDepot();

    LoadInfo info;
    info.m_mapPath      = _szFilename;
    info.m_bAsyncLoad   = bfalse;
    info.m_disconnectFromCells = btrue;
    World* pWorld       = (World*)WORLD_MANAGER->createAndLoadWorld(info).getObject();

    if (!pWorld) //no world let the cooking fails
        return bfalse;

    String prefTarget = Scene::getBinaryTarget();
    Scene::setBinaryTarget(_szPlatform);
    ACTORSMANAGER->setActorCooking(btrue);

    const u32 uSceneCount = pWorld->getSceneCount();

    u64 rootSceneTimeStamp = 0;
    u32 rootSceneEngineVersionWhenSaved = 0;
    PathString_t pathName;

    for(u32 i = 0; i < uSceneCount; ++i)
    {
        Scene* pScene = pWorld->getSceneAt(i);
        pScene->addSerializationFlag(Scene::flag_isCooking);

        pScene->getPath().getString(pathName);

        // Frieze connection presaved data
        {
            bbool failBecauseOfResaveNeeded = bfalse;
            const u32 sceneEngineVersionWhenSaved = pScene->getEngineVersionWhenSaved();
            if(i == 0) // if it's the root scene
            {
                rootSceneEngineVersionWhenSaved = sceneEngineVersionWhenSaved;

                if(rootSceneEngineVersionWhenSaved < 20719) // if saved with a too old engine
                    failBecauseOfResaveNeeded = btrue;

                rootSceneTimeStamp = FILEMANAGER->getLastTimeWriteAccess(pathName);
            }
            else // not the root scene
            {
                if(rootSceneEngineVersionWhenSaved < sceneEngineVersionWhenSaved) // if the subscene is saved with a more recent engine
                {
                    failBecauseOfResaveNeeded = btrue;
                }
                else // only testing the engine CL is not enought, so test the timestamp
                {
                    const u64 sceneTimeStamp = FILEMANAGER->getLastTimeWriteAccess(pathName);

                    if(rootSceneTimeStamp < sceneTimeStamp) // if the subscene has been saved after the root scene
                        failBecauseOfResaveNeeded = btrue;
                }
            }

            if(failBecauseOfResaveNeeded) // the map need to be resaved for console friezes's connections
            {
                ITF_ASSERT_MSG(0, "The map needs to be resaved, so frieze's connection will be saved inside the scene");
                
#if 0           // temp disabled 
                if(Versioning::getEngineVersion() != 0) // only for official build
                    return bfalse;
#endif // 0
            }
        }

        if ( !i )
            computeLinearisationData( pScene, pWorld, _szFilename, _szPlatform );

        String filenameCooked = FILESERVER->getCookedNamePlatform( pathName,_szPlatform);

        // Skip groups
        SubSceneActor* pSSA = pScene->getSubSceneActorHolder();
        if(!pSSA || (pSSA && !pSSA->getEmbedScene()))
        {
            pScene->reorderSerializedLists();

            result |= XMLAll::saveScene(_absoluteRootPath + filenameCooked, pScene, btrue);
        }
        else
        {
            result |= btrue; // group are not saved individually, so say it's ok
        }
    }

    ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));
    Scene::setBinaryTarget(prefTarget);

#if 0
    {
        // Try to reload from binary, WORK ONLY on win32
        LoadInfo infoReloadTest;
        infoReloadTest.m_mapPath     = _szFilename;
        infoReloadTest.m_bAsyncLoad  = bfalse;
        infoReloadTest.m_bBinary     = btrue;

        World* pWorldReloadTest = (World*)WORLD_MANAGER->newWorld(infoReloadTest).getObject();
        ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorldReloadTest));
    }
#endif

    
    if(result)
    {
       LOG_COOKER("[SCENE_COOKER] %ls => COOKED\n", _szFilename.cStr());
    }
    else
    {
       LOG_COOKER("[SCENE_COOKER] %ls => FAILED\n", _szFilename.cStr());
    }

    ACTORSMANAGER->setActorCooking(bfalse);
    return result;
    
}

/*
format:
------
u32 chunid magic id (0x1A2B3C)
u32 number of polylines
for each
    u64 owner frise objectID
    u32 pos count
    for each
        f32 x
        f32 y
    end for
end for
u32 chunid magic id (0x1A2B3C)
*/

void sceneCooker::CookPolyline(PolyLine* _pol, ArchiveMemory* _ArchiveMem)
{
    PolyLine* owner = (PolyLine*)_pol->m_ownerFrise;
    u64 id = owner->getObjectId().m_id;
    _ArchiveMem->serialize(id);
    u32 size = _pol->getPosCount();
    _ArchiveMem->serialize(size);
    for (u32 i = 0; i < size ; i++)
    {
        const Vec2d& pos = _pol->getPosAt(i);
        f32 fvalue = pos.m_x;
        _ArchiveMem->serialize(fvalue);
        fvalue = pos.m_y;
        _ArchiveMem->serialize(fvalue);
    }
}

}
