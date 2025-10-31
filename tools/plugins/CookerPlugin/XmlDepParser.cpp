#include "core/enginecommon.h"

#include "XmlDepParser.h"

#ifndef HEADER_PUGIXML_HPP
#include "pugixml/src/pugixml.hpp"
#endif //HEADER_PUGIXML_HPP

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ZSERIALIZER_DEP_H_
#include "core/serializer/ZSerializerDep.h"
#endif //_ITF_ZSERIALIZER_DEP_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_COMMON_DEP_FUNCTIONS_H_
#include "tools/plugins/CookerPlugin/CommonDepFunctions.h"
#endif //_ITF_COMMON_DEP_FUNCTIONS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

using namespace pugi;

namespace ITF {

static const char* s_isdTags[] =
{
    "scenePath",
    NULL
};

static const char* s_fcgTags[] =
{
    "path",
    "gameMaterial",
    "backsideGameMaterial",
    "gameMaterialExtremityStart",
    "gameMaterialExtremityStop",
    "normaltexture",
    NULL
};

static const char* s_iscTags[] =
{
    "ConfigName",
    "LUA",
    "RELATIVEPATH",
    "texture",
    "animInstance",
    NULL
};

static void processXml( xml_node xml, const char** tags, DepCollection& col )
{
    if ( xml.empty() )
        return;
    if ( xml.attributes_begin() != xml.attributes_end() )
        for ( u32 i = 0; tags[i] != NULL; ++i )
        {
            xml_attribute xmlAttr = xml.attribute( tags[i] );

            if ( !xmlAttr.empty() )
                col.add( xmlAttr.value() );
        }
    processXml( xml.first_child(), tags, col );
    processXml( xml.next_sibling(), tags, col );
}

static bbool getDependencies( const String& cacheKey, const String& platform, const String& filename, const char** tags, DepCollection& col )
{
    ITF_ASSERT( tags != NULL );
    if ( tags == NULL )
        return bfalse;
    ITF_ASSERT( FILEMANAGER != NULL );
    if ( FILEMANAGER == NULL )
        return bfalse;

    String          finalPath;
    xml_document    doc;

    if ( !FILEMANAGER->TranslatePath( finalPath, filename ) )
        return bfalse;
    if ( doc.load_file( StringConverter( finalPath ).getChar() ).status != status_ok )
        return bfalse;
    processXml( doc, tags, col );
    return btrue;
}

static void resolveMedalSceneDependency( const String& filename, String& path )
{
    const StringID& tag = RAY_GAMEMANAGER->getMapConfigTagByPath( filename );

    path.clear();
    if ( !tag.isValid() )
        return;

    const StringID& type = RAY_GAMEMANAGER->getMapConfigType( tag );

    if ( !type.isValid() )
        return;

    const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType( type, bfalse );

    if ( config == NULL )
        return;

    config->m_medalPath.getString(path);
}

bbool XmlDepParser::iscDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col )
{
    if ( col.load( cacheKey, filename, platform ) )
    {
        col.filterExt();
        return true;
    }

    CSerializerDep< CSerializerObjectParser >   serializer;
    bool                                        success = btrue;
    
    ACTORSMANAGER->setActorCooking(btrue);

    Scene::addStaticSerializationFlag( Scene::flag_dependency );
    {
        Scene                                       scene;
        StringConverter                             converter( filename );

        if ( !XMLAll::readScene( &scene, filename, &serializer ) )
            success = bfalse;
    }
    Scene::removeStaticSerializationFlag( Scene::flag_dependency );
    if ( !success )
        return bfalse;
    retrieveLocalisationDependencies( serializer, col );
    //if ( !getDependencies( cacheKey, platform, filename, s_iscTags, col ) )
    //    return bfalse;

    String medalDep;

    resolveMedalSceneDependency( filename, medalDep );
    if ( !medalDep.isEmpty() )
        col.add( medalDep );
    col.save( cacheKey, filename, platform );
    col.filterExt();
    return btrue;
}

bbool XmlDepParser::isdDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col )
{
    if ( col.load( cacheKey, filename, platform ) )
    {
        col.filterExt();
        return true;
    }
    if ( !getDependencies( cacheKey, platform, filename, s_isdTags, col ) )
        return bfalse;
    col.save( cacheKey, filename, platform );
    col.filterExt();
    return btrue;
}

bbool XmlDepParser::fcgDependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& col )
{
    if ( col.load( cacheKey, filename, platform ) )
    {
        col.filterExt();
        return true;
    }
    if ( !getDependencies( cacheKey, platform, filename, s_fcgTags, col ) )
        return bfalse;
    col.save( cacheKey, filename, platform );
    col.filterExt();
    return btrue;
}

}
