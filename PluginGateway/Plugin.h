#ifndef _ITF_PLUGIN_H_
#define _ITF_PLUGIN_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_EDITOR_TYPES_H_
#include "editor/Editor_Types.h"
#endif //_ITF_EDITOR_TYPES_H_

namespace ITF
{

class NETPeer;
class EditorContextMenu;
struct ContextMenuItem;
class PickingShape;
class PickingShapePluginData;

//////////////////////////////////////////////////////////////////////////
///
/// Editor plugin class
/// All plugins MUST inherit from this class
///
class Plugin
{
public:
    enum WatchType
    {
        type_int8,
        type_uint8,
        type_int16,
        type_uint16,
        type_int32,
        type_uint32,
        type_int64,
        type_uint64,
        type_float,
        type_bool,
        type_string,
        ENUM_FORCE_SIZE_32(0)
    };

    const NETPeer*      m_editorPeer;       /// peer data for the external tool linked to this plugin
    u32                 m_editorSenderID;   /// sender ID for the external tool linked to this plugin

    Plugin()
        : m_editorPeer(nullptr)
        , m_editorSenderID(0)
    {
    }

    /// @fn virtual ~Plugin()
    virtual ~Plugin() {}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime the plugin receives data from an external tool
    /// @param _blob the received data
    virtual void receive        (   Blob& _blob, const NETPeer* _peer, u32 _senderID ){ ITF_UNUSED(_blob); ITF_UNUSED(_peer); ITF_UNUSED(_senderID); ITF_ASSERT_MSG(0, "must be overloaded"); }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called every frame (except if plugin is paused)
    virtual void update         (               ){}
    
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called every frame
    virtual void draw           (               ){}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime a mouse button is pressed or released
    /// @param _but id of the button
    /// @param _status status of the button
    virtual void onMouseButton  (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status ){ ITF_UNUSED(_but); ITF_UNUSED(_status); }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime the mouse is moved
    /// @param _x new mouse's x coordinate
    /// @param _y new mouse's y coordinate
    virtual void onMousePos     (   i32 _x, i32 _y ){ ITF_UNUSED(_x); ITF_UNUSED(_y); }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime the mouse wheel is moved
    /// @param _wheelValue new mouse's wheel value
    /// @param _wheelDelta mouse's wheel delta value
    virtual void onMouseWheel   (   i32 _wheelValue, i32 _wheelDelta            ){ ITF_UNUSED(_wheelValue); ITF_UNUSED(_wheelDelta); }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime a key is pressed or released
    /// @param _key the pressed key (see InputAdapter.h for keys list)
    /// @param _status the key's status
    virtual void onKey          (   i32 _key, InputAdapter::PressStatus _status ){ ITF_UNUSED(_key); ITF_UNUSED(_status); }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime the user picks a shape
    /// @param _shape the picked shape
    /// @return btrue if the plugin handle the picking action
    virtual bbool onPickShape      (   PickingShape* /*_shape*/  ) { return bfalse; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Called by editor when the user try to delete an picking shape
    /// @param _shapesWithSameOwner the picking shapes to be delete
    /// @return btrue if a plugin handle the event
    virtual bbool onEditorRemove  (   const SafeArray<PickingShape*>& /*_shapesWithSameOwner*/  ) { return bfalse; }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Tells the picking system is picking can be performed
    /// or if the current plugin temporarily locks picking
    virtual bbool isPickingAllowed (){ return btrue; }


    //////////////////////////////////////////////////////////////////////////
    ///
    /// Called everytime after a scene is activated/deactivated
    /// @param _scene the created scene
    /// @param _activated the new activation status
    virtual void onPostSceneActivation (const Scene* /*_scene*/, bbool /*_activated*/) {}
    virtual void onSceneAsyncLoadFinished(const Scene* /*_pScene*/) {}

    //////////////////////////////////////////////////////////////////////////
    ///Create custom data for picking shape. Data will be automatically stored in pickingshape
    virtual PickingShapePluginData *createPickingShapeData(PickingShape *) {return NULL;}


    virtual void startEditor    () {}
    virtual void stopEditor     () {}
    virtual void onClearScene   () {}

    virtual void onLog          (const String& _szPlugin,const String& _szLogText) { ITF_UNUSED(_szPlugin); ITF_UNUSED(_szLogText); }

    virtual bbool bCanEnterEditMode () { return btrue;}
    virtual bbool bCanLeaveEditMode () { return btrue;}
    virtual void  UnselectAll       (bbool /*_force*/) {}
    virtual void  fillEditorActiveList( ObjectRefList &/*_objects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/)const {}
    virtual void  fillEditorForcedList( ObjectRefList &/*_objects*/, PickingShapeVector& /*_orderedShapes*/)const {}

    virtual void  onFrisePicked   (ObjectRef /*_frise*/) {} // called by the PolyLineEditor whenever a frise is picked. This information may interest other plugins (such as the sequence editor)
    virtual void  onActorPicked   (ObjectRef /*_actor*/) {} // called by the ActorEditor whenever an actor is picked. This information may interest other plugins (such as the sequence editor)
    virtual void  onActorSubEdit  (ObjectRef /*_actor*/) {} // called by the ActorEditor whenever an actor is sub-edited. This information may interest other plugins (such as the sequence editor)
    virtual void  onNewSequence   (ObjectRef /*_actor*/) {} // unused by almost all plugins, but breaks dependency between engine and plugins, so it's cool
    virtual void  onCameraModeChange (int /*_prevMode*/, int /*_newMode*/) {}
    virtual void  onSceneSave  () {} // when the user explicitely wants to save
    virtual bbool isSubEditAllowed () {return btrue;}
    virtual void  onStartRotation  (){} // when a selection starts to be rotated by the editor
    virtual void  onEndRotation  (){} // when a selection stops to be rotated by the editor
    virtual void  onFileDrop (const String& /*_fileName*/) {}
    virtual bbool fillContextMenu (EditorContextMenu& /*_menu*/, ObjectRef /*_target*/, bbool /*_topItems*/) { return btrue; } // fill the menu and return bfalse to force hide
    virtual void  fillAltContextMenu(EditorContextMenu& /*_menu*/) {}
    virtual void  onContextMenuItemSelected(ContextMenuItem* /*_item*/, ObjectRef /*_menuTarget*/) {}
    virtual void  onDeleteScene(const Scene* /*_pScene*/) {}
    virtual void  onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {}
    virtual void  onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {}
    virtual void  onSwitchToWorld(World* /*_pWorld*/, bbool /*_bSwitchDone*/) { UnselectAll(btrue); }
    virtual void  onDeleteWorld(World* /*_pWorld*/) {}
    virtual void  onObjectCreated (BaseObject* /*_pObject*/) {}                                    // Called when an object is added by edition
    virtual void  onObjectDeleted (BaseObject* /*_pObject*/) {}                                    // Called when an object is removed by edition
    virtual void  onObjectChanged (BaseObject* /*_pObject*/) {}                                    // called when an object is changed 
    virtual void  onObjectSpawned (const BaseObject* /*_newObject*/) {}                            // called when an object is spawned at runtime
    virtual void  onObjectsDuplicated (const SafeArray<BaseObject*>& /*_from*/, const SafeArray <BaseObject*>& /*_to*/) {}   // called when an object is duplicated at runtime
    virtual void  onPickableRenamed(Pickable* /*_pObj*/, const String8& /*_previousUserFriendly*/) {} // called when a pickable is manually renamed
    virtual void  onPickableSceneChanged(Pickable* /*_pPickable*/, Scene* /*_src*/, Scene* /*_dst*/) {}
    virtual void  cancelCurrentAction(bbool /*_keepEditorStuff*/) {}
    virtual void  onCloseApp( ) {}
    virtual void  enterSubEdit( ObjectRef /*_or*/ ) {}
    virtual bbool isSubEditable( const BaseObject* /*_pObj*/ )const  { return bfalse; }
    virtual void  pushInfoFlags(bbool /*_clear*/) {}
    virtual void  popInfoFlags() {}
    virtual void  switchInfoFlags() {}
    virtual void  onToolConnect(const NETPeer* _peer, u32 _senderID) { m_editorPeer = _peer; m_editorSenderID = _senderID; }
    virtual void  onToolDisconnect(const NETPeer* _peer) { if(m_editorPeer == _peer) m_editorPeer = NULL; }
    virtual bbool onSetShapePosition(PickingShape* /*_pPickingShape*/, const Vec2d& /*_screenspacepos*/)        { return bfalse; }
    virtual bbool onOffsetShapePosition(PickingShape* /*_pPickingShape*/, const Vec3d& /*_deltaPos*/ )          { return bfalse; }
	virtual void  OnFileMissing(const String& /*_actorFileName*/, Path& /*_luaPath */){}
	virtual void  OnRefMissing(const char* /*filename*/,const String& /*reference*/){}
	virtual void  OnDataError(const String& _str,Pickable* pickable){}
	virtual void  OnMapLoaded(Scene* rootScene){}
	virtual void  OnFatalError(const char* msg){}
	virtual void  OnWarning(const char* msg){}
	virtual void  OnError(const char* msg){}
    virtual void  OnBootstrapFile(const char* filename){}
    virtual void  OnWorldUpdated(){}
};

} // namespace ITF



#endif // _ITF_PLUGIN_H_
