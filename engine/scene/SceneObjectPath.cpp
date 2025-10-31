#include "precompiled_engine.h"

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{

namespace SceneObjectPathUtils
{

// This functions returns the relative path from _absoluteFrom to _absoluteTo
bbool resolveRelativePath( const ObjectPath& _absoluteFrom, const ObjectPath& _absoluteTo, ObjectPath& _ret )
{
    _ret.invalidate();
    _ret.clear();

    if ( !_absoluteFrom.getIsAbsolute() || !_absoluteTo.getIsAbsolute() )
    {
        return bfalse;
    }

    u32 levelsFrom = _absoluteFrom.getNumLevels();
    u32 levelsTo = _absoluteTo.getNumLevels();
    u32 depthEqual;

    for ( depthEqual = 0; depthEqual < levelsFrom && depthEqual < levelsTo; depthEqual++ )
    {
        const ObjectPath::Level& from = _absoluteFrom.getLevel(depthEqual);
        const ObjectPath::Level& to = _absoluteTo.getLevel(depthEqual);

        if ( from != to )
        {
            break;
        }
    }

    if ( depthEqual == 0 )
    {
        // If they don't start from the same scene then we cannot get a relative path
        return bfalse;
    }

    Pickable* obj = getObjectFromAbsolutePath(_absoluteFrom);

    if ( !obj )
    {
        return bfalse;
    }

    Scene* currentScene = obj->getScene();

    if ( !currentScene )
    {
        return bfalse;
    }

    // Now we know at which part of the tree they branch, so start from the end of the source, up to where
    // they are start to branch
    ObjectPath::Level newLevel;
    SubSceneActor* subSceneActor;

    newLevel.m_parent = btrue;

    for ( u32 i = levelsFrom - 1; i >= depthEqual; i-- )
    {
        subSceneActor = currentScene->getSubSceneActorHolder();

        if ( !subSceneActor )
        {
            return bfalse;
        }

        currentScene = subSceneActor->getScene();

        _ret.addLevel(newLevel);
    }

    newLevel.m_parent = bfalse;

    // Now we are at the part where they branch, so we need to travel down to the destination
    for ( u32 i = depthEqual; i < levelsTo; i++ )
    {
        const ObjectPath::Level& currentLevel = _absoluteTo.getLevel(i);
        const ObjectRefList& subSceneList = currentScene->getSubSceneActors();
        u32 numSubScenes = subSceneList.size();
        bbool found = bfalse;

        for ( u32 sceneIndex = 0; sceneIndex < numSubScenes; sceneIndex++ )
        {
            subSceneActor = static_cast<SubSceneActor*>(GETOBJECT(subSceneList[sceneIndex]));

            if ( subSceneActor && subSceneActor->getUserFriendly() == currentLevel.m_name )
            {
                found = btrue;
                currentScene = subSceneActor->getSubScene();
                break;
            }
        }

        if ( !found || !currentScene )
        {
            return bfalse;
        }

        newLevel.m_name = currentLevel.m_name;

        _ret.addLevel(newLevel);
    }

    _ret.setStrId(_absoluteTo.getStrId());
    _ret.setObjId(_absoluteTo.getObjId());
    _ret.setIsAbsolute(bfalse);

    return btrue;
}

// This returns the absolute path of _obj
void getAbsolutePathFromObject( const Pickable* _obj, ObjectPath& _ret )
{
    FixedArray <Scene*,50> scenes;

    _ret.clear();

    Scene* currentScene = _obj->getScene();
    SubSceneActor* parentActor;
    ObjectPath::Level newLevel;

    // Store all the scenes to the root in the array
    while ( currentScene )
    {
        scenes.push_back(currentScene);

        parentActor = currentScene->getSubSceneActorHolder();

        if ( parentActor )
        {
            currentScene = parentActor->getScene();
        }
        else
        {
            currentScene = NULL;
        }
    }

    // Create the levels starting from the root all the way to the end of the tree
    i32 numScenes = static_cast<i32>(scenes.size());
    PathString8_t scenePath;

    for ( i32 i = numScenes - 1; i >= 0; i-- )
    {
        currentScene = scenes[i];
        parentActor = currentScene->getSubSceneActorHolder();

        if ( parentActor )
        {
            newLevel.m_name = parentActor->getUserFriendly();
        }
        else
        {
            currentScene->getPath().getString(scenePath);
            newLevel.m_name = scenePath;
        }

        _ret.addLevel(newLevel);
    }

    // Store the object Id at the end
    _ret.setStrId(_obj->getUserFriendly().cStr());
    _ret.setObjId(_obj->getObjectId());
    _ret.setIsAbsolute(btrue);
}

// This returns the relative path from _from to _to
bbool getRelativePathFromObject( const Pickable* _from, const Pickable* _to, ObjectPath& _ret )
{
    ObjectPath fromPath, toPath;

    getAbsolutePathFromObject(_from,fromPath);
    getAbsolutePathFromObject(_to,toPath);
    
    return resolveRelativePath(fromPath,toPath,_ret);
}

// This returns the pickable from an absolute path
Pickable* getObjectFromAbsolutePath( const ObjectPath& _path )
{
    Scene* currentScene = getSceneFromAbsolutePath(_path);

    if ( !currentScene )
    {
        return NULL;
    }

    BaseObject* baseObj;

    if ( _path.getObjId().isValid() )
    {
        baseObj = WORLD_MANAGER->getObject(currentScene,_path.getObjId());
    }
    else
    {
        baseObj = currentScene->getPickableFromUserFriendly(_path.getStrId());
    }

    if ( baseObj )
    {
        Pickable* pickable = baseObj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));

        if ( pickable )
        {
            _path.setObjId(pickable->getObjectId());
            return pickable;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

// This returns the pickable from a relative path from _obj
Pickable* getObjectFromRelativePath( const Pickable* _obj, const ObjectPath& _path )
{
    if ( _path.getIsAbsolute() )
    {
        return NULL;
    }

    u32 numLevels = _path.getNumLevels();
    Scene* currentScene = _obj->getScene();
    SubSceneActor* subSceneActor;
    u32 currentLevel = 0;

    while ( currentScene && currentLevel < numLevels )
    {
        const ObjectPath::Level& level = _path.getLevel(currentLevel);

        if ( level.m_parent )
        {
            subSceneActor = currentScene->getSubSceneActorHolder();

            if ( subSceneActor )
            {
                currentScene = subSceneActor->getScene();
            }
            else
            {
                currentScene = NULL;
            }
        }
        else
        {
            const ObjectRefList& subSceneList = currentScene->getSubSceneActors();
            u32 numSubScenes = subSceneList.size();
            bbool found = bfalse;

            for ( u32 sceneIndex = 0; sceneIndex < numSubScenes; sceneIndex++ )
            {
                subSceneActor = static_cast<SubSceneActor*>(GETOBJECT(subSceneList[sceneIndex]));

                if ( subSceneActor && subSceneActor->getUserFriendly() == level.m_name )
                {
                    found = btrue;
                    currentScene = subSceneActor->getSubScene();
                    break;
                }
            }

            if ( !found )
            {
                currentScene = NULL;
            }
        }

        currentLevel++;
    }

    if ( !currentScene )
    {
        return NULL;
    }

    BaseObject* baseObj;

    if ( _path.getObjId().isValid() )
    {
        baseObj = WORLD_MANAGER->getObject(currentScene,_path.getObjId());
    }
    else
    {
        baseObj = currentScene->getPickableFromUserFriendly(_path.getStrId());
    }

    if ( baseObj )
    {
        Pickable* pickable = baseObj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));

        if ( pickable )
        {
            _path.setObjId(pickable->getObjectId());
            return pickable;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

void getWorldFromAbsolutePath( const ObjectPath& _path, String8& _worldName )
{
    _worldName.clear();

    ITF_ASSERT(_path.getIsAbsolute());

    if ( !_path.getIsAbsolute() )
    {
        return;
    }

    _worldName = _path.getLevel(0).m_name;
}

Scene* getSceneFromAbsolutePath( const ObjectPath& _path )
{
    if ( !_path.getIsAbsolute() )
    {
        return NULL;
    }

    u32 numLevels = _path.getNumLevels();

    if ( numLevels == 0 )
    {
        return NULL;
    }

    Scene* currentScene = NULL;
    const u32 numWorlds = WORLD_MANAGER->getWorldCount();
    const ObjectPath::Level& rootLevel = _path.getLevel(0);
    PathString8_t pathName;

    for ( u32 i = 0; i < numWorlds; i++ )
    {
        World* world = WORLD_MANAGER->getWorldAt(i);
        Scene* rootScene = world->getRootScene();

        if ( rootScene )
        {
            rootScene->getPath().getString(pathName);
            
            if ( pathName == rootLevel.m_name )
            {
                currentScene = rootScene;
                break;
            }
        }
    }

    if ( !currentScene )
    {
        return NULL;
    }

    u32 currentLevel = 1;
    SubSceneActor* subSceneActor = NULL;

    while ( currentScene && currentLevel < numLevels )
    {
        const ObjectPath::Level& level = _path.getLevel(currentLevel);
        const ObjectRefList& subSceneList = currentScene->getSubSceneActors();
        const u32 numSubScenes = subSceneList.size();
        bbool found = bfalse;

        for ( u32 sceneIndex = 0; sceneIndex < numSubScenes; sceneIndex++ )
        {
            subSceneActor = static_cast<SubSceneActor*>(GETOBJECT(subSceneList[sceneIndex]));

            if ( subSceneActor && subSceneActor->getUserFriendly() == level.m_name )
            {
                found = btrue;
                currentScene = subSceneActor->getSubScene();
                break;
            }
        }

        if ( !found )
        {
            currentScene = NULL;
            break;
        }

        currentLevel++;
    }

    return currentScene;
}

}

}