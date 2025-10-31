#ifndef ITF_PLUGINGATEWAY_H_
#define ITF_PLUGINGATEWAY_H_

#pragma once

#ifdef ITF_SUPPORT_PLUGIN


#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_NETPACKETCLIENT_H_
#include "engine/network/NETPacketClient.h"
#endif //_ITF_NETPACKETCLIENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_EDITOR_TYPES_H_
#include "editor/Editor_Types.h"
#endif //_ITF_EDITOR_TYPES_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef ITF_SUPPORT_NET
#error ITF_SUPPORT_NET is required to use PluginGateway
#endif

namespace ITF
{
class Scene;
class Plugin;
class NETPeer;
class TCPAdapter;
class PickingShape;
struct PickingInfo2D;
struct ContextMenuItem;
class EditorContextMenu;
class Transaction_Serialize_Send;

#define COMMAND_WATCH       "watch"
#define COMMAND_DEPOTPATH   "depotpath"

class PluginGateWay : public TemplateSingleton<PluginGateWay>,public NETPacketClient, public Interface_InputListener, public Interface_SceneEventsListener, public Interface_WorldEventsListener
{
public:
    static void               createSingleton (   );
    static void               destroySingleton();
    static PluginGateWay*     getSingleton (   );
    static u16                m_defaultPort;

    PluginGateWay();

    void            setPort(u16 _port)  {m_defaultPort = _port;}
    u16             getPort() const     {return m_defaultPort;}

    const NETPeer*        getHost() const;
    const String*   getPluginName   ( const Plugin* _p) const;
    bbool           init            ( const char* _ip, u16 _port, bbool _isServer ,bbool bRaiseError = btrue  );
    void            shutdown        (   );


    bbool sendPacket(Blob& _blob,const NETPeer* _peer, u32 _receiverID = 0)const;
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Registers a plugin
    /// @param _name the plugin's name. Must be unique among all registered plugins.
    /// @param _plug plugin's instance address.
    /// @param _isMainEditorforType (default: BaseObject::eInvalid). This information must be unique along all registered plugins. Indicates "_plug" is the primary editor for object type "_isMainEditorforType"
    /// @param _getEventsForTypes (default: NULL). Indicates that "_plug" should receive edition events (picking) for these types. You don't need to add "_isMainEditorforType" to "_getEventsForTypes" list. This will be done automatically    
    void            addPlugin       ( const String& _name, Plugin* _plug, BaseObject::ObjectType _isMainEditorforType = BaseObject::eInvalid, const SafeArray<BaseObject::ObjectType>* _getEventsForTypes = NULL  );
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get list of plugins
    /// @param _pluginList this list will be filled by pointers to plugins
    void            getPluginList   ( ITF_VECTOR<Plugin*> &_pluginList);

    void            startEditor     ();
    void            stopEditor      ();

    //
    void            update          (   );
    void            draw            (   );
    bbool           sendToHost      ( Blob& _b  );
    Plugin*         getPluginByName ( const String& _name   );
    Plugin*         getPluginForObject ( const BaseObject* _obj   );
    bbool           receive         (   Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener  );
                     ~PluginGateWay (   );


    bbool           onMouseButton   (InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    bbool           onMousePos      (   i32 _x, i32 _y );
    bbool           onMouseWheel    (   i32 _wheelValue, i32 _wheelDelta           );
    bbool           onKey           (   i32 _key, InputAdapter::PressStatus _status );
    bbool           onLog           (const String& szExeName, const String& _szLogText);
    bbool           isPickingAllowed(  );
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// ask each plugin a list of pickable objects/picking shapes (not used when picking is not allowed)
    /// @param [out] _objects the object list
    /// @param [out] _orderedShapes the shape list
    /// @param [in] _pFilterScene the scene filter
    void            fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene) const;
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// ask each plugin a list of items that will be forced into the picking shape list, even when picking is not allowed
    /// @param [out] _objects the object list
    /// @param [out] _orderedShapes the shape list
    void            fillEditorForcedList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes ) const;

    /// @brief  Called whenever a shape is picked
    /// @param [in] _shape   The picking shape.
    /// @return btrue if a plugin handle the picking action
    bbool           onPickShape       (   PickingShape* _shape  );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Called by editor when the user try to delete an picking shape
    /// @param _shapesWithSameOwner the picking shapes to be delete
    /// @return btrue if a plugin handle the event
    bbool           onEditorRemove  (   const SafeArray<PickingShape*>& _shapesWithSameOwner   );

    void            onClearScene    (                       );
    
    void            PausePlugin     (  Plugin* _plug        );
    void            UnpausePlugin   (  Plugin* _plug        );
    void            PauseAllExcept  (  Plugin* _plug        );
    void            setExclusiveFocus( Plugin* _plug        );
    bbool           hasExclusiveFocus( Plugin* _plug        );
    bbool           bCanEnterEditMode (  );
    bbool           bCanLeaveEditMode (  );
    void            UnselectAll       (bbool _force, const Plugin* _exception = NULL);   // _exception is the plugin that should not unselect
    Plugin*         getExclusiveFocus ();
    void            onFrisePicked   (ObjectRef _frise);
    void            onLayerPicked   (ObjectRef _layer);
    void            onActorPicked   (ObjectRef _actor); // called by the ActorEditor whenever an actor is picked. This information may interest other plugins (such as the sequence editor)
    void            onActorSubEdit   (ObjectRef _actor); // called by the ActorEditor whenever an actor is subedited. This information may interest other plugins (such as the sequence editor)
    void            onCameraModeChange (int _prevMode, int newMode);
    void            onSceneSave  (); // when the user explicitely wants to save
    void            onStartRotation  (); // when a selection starts to be rotated by the editor
    void            onEndRotation  (); // when a selection stops to be rotated by the editor
    bbool           isSubEditAllowed ();
    void            onFileDrop (const String& _fileName);
    bbool           fillContextMenu (EditorContextMenu& /*_menu*/, ObjectRef /*_target*/, bbool /*_topItems*/);
    void            fillAltContextMenu(EditorContextMenu& /*_menu*/);
    void            onContextMenuItemSelected(ContextMenuItem* /*_item*/, ObjectRef /*_menuTarget*/);
    void            cancelCurrentAction(bbool keepEditorStuff);
    void            enterSubEdit( ObjectRef _or );
    bbool           isSubEditable( const BaseObject* _pObj )const;
    void            pushInfoFlags(bbool /*_clear*/);
    void            popInfoFlags();
    void            switchInfoFlags();
    void            onPickableRenamed(Pickable* _pObj, const String8& _previousUserFriendly);

    void            onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
    void            onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );

    // from Interface_SceneEventsListener
    virtual void    onDeleteScene(const Scene* _pScene);
    virtual void    onObjectAdded(const Scene* _pScene, BaseObject* _pObj);
    virtual void    onObjectRemoved(const Scene* _pScene, BaseObject* _pObj, bbool _delete);
    virtual void    onPostSceneActivation(const Scene* _pScene, bbool _bActive);
    virtual void    onSceneAsyncLoadFinished(const Scene* /*_pScene*/);
    virtual void    onScenePreDestroy(Scene* /*_pScene*/) {};

    // from Interface_WorldEventsListener
    virtual void    onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
    virtual void    onDeleteWorld(World* _pWorld);
    virtual void    onStartDrawProcess(){}; // inherited from Interface_WorldEventsListener
    virtual void    onEndDrawProcess(){}; // inherited from Interface_WorldEventsListener
    virtual void    onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
    virtual void    onEndUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
    virtual void    onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener

    void            onObjectCreated(BaseObject* pBO)const;  // Called when an object is added by edition
    void            onObjectDeleted(BaseObject* pBO)const;  // Called when an object is removed by edition
    void            onObjectChanged(BaseObject* pBO)const;
    void            onObjectSpawned (const BaseObject* /*_newObject*/) const;                           // called when an object is spawned at runtime
    void            onPickableSceneChanged(Pickable* _pPickable, Scene* _src, Scene *_dst);

    // called before Pickable::onLoaded() when an object is duplicated at runtime, warning : "_from[i]" MUST be the source of the duplicate "_to[i]"
    void            onObjectsDuplicated (const SafeArray <BaseObject*>& /*_from*/, const SafeArray <BaseObject*>& /*_to*/) const;
    void            onCloseApp( ) const;
    void            forwardMouseEvents( bbool _fwd ) {m_forwardMouseEvents = _fwd;}
	void			OnFileMissing(const String& _actorFileName, Path& _luaPath );
	void			OnRefMissing(const char* filename,const String& reference);
	virtual void	OnDataError(const String& _str,Pickable* pickable);
	virtual void    OnMapLoaded(Scene* rootScene);
	virtual void  OnFatalError(const char* msg);
	virtual void  OnWarning(const char* msg);
	virtual void  OnError(const char* msg);
    virtual void  OnWorldUpdated();

private:

    struct  PlugEntry
    {
        PlugEntry (const String& _name, Plugin* _plug, BaseObject::ObjectType _primaryType, const SafeArray<BaseObject::ObjectType>* _secondaryTypes)
        {
            m_name = _name;
            m_plug = _plug;
            m_isPaused = bfalse;
            m_hasFocus = btrue;
            m_hasExclusiveFocus = bfalse;
            m_primaryObjType = _primaryType;
            if (_secondaryTypes)
                m_secondaryObjTypes = *_secondaryTypes;
            if (m_secondaryObjTypes.find(_primaryType) == -1)
                m_secondaryObjTypes.push_back(_primaryType);
        }

        String      m_name;
        bbool       m_isPaused;
        bbool       m_hasFocus;
        bbool       m_hasExclusiveFocus;
        Plugin*     m_plug;
        BaseObject::ObjectType  m_primaryObjType;
        SafeArray<BaseObject::ObjectType> m_secondaryObjTypes;
    };

    TCPAdapter*             m_TCPAdapter;
    ITF_VECTOR<PlugEntry>   m_plugins;
    bbool                   m_isRunningAsServer;
    bbool                   m_forwardMouseEvents;
};

#define PLUGINGATEWAY PluginGateWay::getptr()

} // namespace ITF

#endif // ITF_SUPPORT_PLUGIN

#endif // _ITF_PLUGINGATEWAY_H_
