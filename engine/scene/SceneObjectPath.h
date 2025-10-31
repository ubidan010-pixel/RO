#ifndef _ITF_SCENEOBJECTPATH_H_
#define _ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{

namespace SceneObjectPathUtils
{

    bbool       resolveRelativePath( const ObjectPath& _absoluteFrom, const ObjectPath& _absoluteTo, ObjectPath& _ret );
    void        getAbsolutePathFromObject( const Pickable* _obj, ObjectPath& _ret );
    bbool       getRelativePathFromObject( const Pickable* _from, const Pickable* _to, ObjectPath& _ret );
    Pickable*   getObjectFromAbsolutePath( const ObjectPath& _path );
    Pickable*   getObjectFromRelativePath( const Pickable* _obj, const ObjectPath& _path );
    void        getWorldFromAbsolutePath( const ObjectPath& _path, String8& _worldName );
    Scene*      getSceneFromAbsolutePath( const ObjectPath& _path );

} // SceneObjectPathUtils

} // namespace ITF

#endif // _ITF_SCENEOBJECTPATH_H_

