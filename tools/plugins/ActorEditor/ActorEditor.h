#ifndef _ITF_ACTOREDITOR_H_
#define _ITF_ACTOREDITOR_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor\PickingShape\PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

#ifndef _ITF_EDITOR_ICONSTOCK_H_
#include "editor/Editor_IconStock.h"
#endif //_ITF_EDITOR_ICONSTOCK_H_

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    class Actor;
    class Pickable;
    class SubSceneActor;
    class ActorComponent;
    struct PickingInfo2D;
    struct ContextMenuItem;
    class EditorContextMenu;

    /**
    * Interface for component specific extensions (plugins). Only pure virtual methods please.
    */
    class Interface_ActorEditorListener : public Interface_PickingShapePlugin
    {
    public:
        /**
        * don't destroy the plugin's singleton here
        */
        virtual void            destroy() = 0;

        /**
        * called when an actor is selected
        * @param    _pActor pointer to the selected actor
        */
        virtual void            onSelected(Actor* _pActor) = 0;

        /**
        * called when an actor is duplicated by the editor
        * @param    _pActor pointer to the selected actor
        * @param    _pActorSrc pointer to the source actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        */
        virtual void            onObjectDuplicated(Actor* _pActor, Actor* _pActorSrc, ActorComponent* _pComp) = 0;

        /**
        * called when an actor is modified (position, rotation, scale, properties)
        * @param    _pActor pointer to the selected actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        */
        virtual void            onObjectChanged(Actor* _pActor, ActorComponent* _pComp) = 0;

        /**
        * called when a shape is moved
        * @param    _pPickingShape   the moved shape
        * @param    _screenspacepos  the futur position of the shape in screen space
        * @return   btrue if the callback is used to modify some data
        */
        virtual bbool           onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos) = 0;

        /**
        * called when a shape is moved
        * @param    _pPickingShape   the moved shape
        * @param    _deltaPos       the world offset position
        * @return   btrue if the callback is used to modify some data
        */
        virtual bbool           onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos )  = 0;

        /**
        * called when the main menu is filled (global scope, no specific actor needs to be selected)
        * @param    _menu   the menu to populate
        */
        virtual void            fillMainMenu (EditorContextMenu& /*_menu*/) = 0;

        /**
        * called when a main menu item is selected
        * @param    _item   the selected item
        */
        virtual void            onMainMenuItemSelected(ContextMenuItem* /*_item*/) = 0;


        /**
        * called when the contextual menu is filled for a specific actor
        * @param    _menu   the menu to populate
        * @param    _pActor pointer to the selected actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        */
        virtual void            fillContextMenu (EditorContextMenu& /*_menu*/, Actor* /*_pActor*/, ActorComponent* /*_pComp*/) = 0;

        /**
        * called when a contextual menu item is selected
        * @param    _item   the selected item
        * @param    _pActor pointer to the selected actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        */
        virtual void            onContextMenuItemSelected(ContextMenuItem* /*_item*/, Actor* /*_pActor*/, ActorComponent* /*_pComp*/) = 0;

        /**
        * called when the alt (debug) contextual menu is filled
        * @param    _menu   the menu to populate
        */
        virtual void            fillAltContextMenu (EditorContextMenu& /*_menu*/) = 0;

        /**
        * called when the actor editor plugin must fill the editor's lists
        * @param    _pickablesObjects   the list of pickables objects
        * @param    _orderedShapes      the list of shapes to display
        * @param    _pFilterScene       the filter scene
        */
        virtual void            fillEditorActiveList( ObjectRefList &_pickablesObjects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const = 0;

        /**
        * called when an alt (debug) contextual menu item is selected
        * @param    _item   the selected item
        */
        virtual void            onAltContextMenuItemSelected(ContextMenuItem* /*_item*/) = 0;

        /**
        * called every frame for all plugins
        */
        virtual void            onUpdate() = 0;

        /**
        * called every frame for all plugins
        */
        virtual void            onDraw() = 0;

        /**
        * called every frame if one of the selected actors features one of the watched components
        * @param    _pActor pointer to the selected actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        */
        virtual void            onUpdateSelectedActor(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) = 0;

        /**
        * add icons to the main anchor
        * @param    _pActor pointer to the selected actor
        * @param    _pComp  pointer to the actor's component listened by the plugin
        * @param    _icons      the list of icons to update
        */
        virtual void addComponentSpecificIcons(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& /*_IconSize*/) = 0;

        /**
        * add icons to a shape that was created by the plugin
        */
        virtual void addPickingShapeSpecificIcons(PickingShape* /*_shape*/, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& /*_IconSize*/) = 0;

        /**
        * @param    _pShape pointer to the deleted shape
        * @return   btrue if the shape is deleted
        */
        virtual bbool onDeletePickingShape(PickingShape* /*_pShape*/) = 0;

        virtual bbool           needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) = 0;
        virtual void            addComponentSpecificPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) = 0;
        virtual void            updateComponentSpecificPickingShape(Actor* /*_pActor*/, PickingShape* /*_pShape*/, PickingShapeVector& /*_activeShapes*/) = 0;

        virtual void            onShapePosChanged(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, PickingShape* /*_pShape*/, const Vec3d& /*_deltaPos*/) = 0;


        virtual ~Interface_ActorEditorListener(){};
    };
#ifdef DAN_TEXTURE_SELECTOR
    using BasePlugin = SamplerPlugin;
#else
    using BasePlugin = Plugin;
#endif

    class ActorEditor : public BasePlugin
    {
    public:

        enum state
        {
            Idle,
            Select,
            CreateActor,
            SubEdit,
            CreateScene,
            RotationScaleHelperMode,
            CreateBind,
            SetSubScenePivot
        };

        // beware, only powers of 2
         static const u32 ActorInfoFlag_None = 0;
         static const u32 ActorInfoFlag_Anchors = 1;
         static const u32 ActorInfoFlag_Any = 0xffffffff;

        //////////////////////////////////////////////////////////////////////////
        ///Internal class for picking shape of actor
        class PickingShapeData : public PickingShapeData_RotationScale
        {
        public:
            //////////////////////////////////////////////////////////////////////////
            ///Construction
            PickingShapeData(PickingShape*, Plugin *);

            //////////////////////////////////////////////////////////////////////////
            ///update component
            virtual void        update();

            //////////////////////////////////////////////////////////////////////////
            ///Draw component
            virtual void        draw();

            //////////////////////////////////////////////////////////////////////////
            /// Rotate the actor
            virtual void        rotateAroundAxisZ(f32 _deltaAngle, const Vec3d* _pRotationCenter = NULL);

            //////////////////////////////////////////////////////////////////////////
            /// Scale the actor
            virtual void        scale(const Vec2d& _newScale);
        };

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        static const char*      getPluginName               (   ) {return  "ActorEditor_Plugin";}
        static const float      getUpdateRate               (   ) {return 0.5f;} // in seconds
        ActorEditor (   );
        ~ActorEditor (   );

        void                    startEditor                 (   );
        void                    stopEditor                  (   );

        void                    enterCreateActorState       (const String& _file, bbool _useBuffer = bfalse );
        void                    enterCreateSceneState       (const String& _absolutePath, bbool _isMerge, bbool _isGroup);
        bbool                   isPickingAllowed            (   );
        void                    clear                       (   );
        // Plugin inherited
        void                    receive                     (   Blob& _blob, const NETPeer* _peer, u32 _senderID );
        void                    onMouseButton               (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
        void                    update                      (               );
        void                    draw                        (               );
        bbool                   onPickShape                 (   PickingShape* _shape  );
        bbool                   onEditorRemove              (   const SafeArray<PickingShape*>& _shapesWithSameOwner );
        void                    onKey                       (   i32 _key, InputAdapter::PressStatus _status );
        void                    onMouseWheel                (i32 _mouseWheel, i32 _wheelDelta );
        bbool                   bCanEnterEditMode            ();
        bbool                   bCanLeaveEditMode            ();
        void                    UnselectAll                 (bbool _force);
        virtual void            fillEditorActiveList       ( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const;
        virtual void            onFileDrop (const String& _fileName);
        virtual bbool           fillContextMenu (EditorContextMenu& /*_menu*/, ObjectRef _target, bbool /*_topItems*/);
        virtual void            fillAltContextMenu (EditorContextMenu& /*_menu*/);
        virtual void            onContextMenuItemSelected(ContextMenuItem* /*_item*/, ObjectRef /*_menuTarget*/);
        virtual void            onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
        virtual void            onDeleteScene(const Scene* _pScene);
        virtual void            onObjectAdded(const Scene* _pScene, BaseObject* _pObj);
        virtual void            onObjectRemoved(const Scene* _pScene, BaseObject* _pObj, bbool _delete);
        virtual void            onPostSceneActivation(const Scene* _pScene, bbool _bActive);
        virtual void            onObjectSpawned (const BaseObject* /*_newObject*/);
        virtual void            onObjectsDuplicated (const SafeArray <BaseObject*>& /*_from*/, const SafeArray <BaseObject*>& /*_to*/);
        virtual void            cancelCurrentAction(bbool _keepEditorStuff);
        virtual void            enterSubEdit( ObjectRef _or );
        virtual bbool           isSubEditable( const BaseObject* _pObj )const;
        virtual void            onObjectCreated(BaseObject* _pObject);
        virtual void            onObjectChanged(BaseObject* _pObject);
        virtual void            onPickableRenamed(Pickable* _pObj, const String8& _previousUserFriendly);
        virtual void            onPickableSceneChanged(Pickable* _pPickable, Scene* _src, Scene* _dst);

        state                   getState()const { return m_state; }
        ITF_INLINE void         resetState () {m_state = Idle;}

        static void             createPickingShapeForActor (const ObjectRef _actor);
        static void             updateActorPickingShape(ObjectRef _or, ObjectRefList &_objects, PickingShapeVector& _orderedShapes);
        bbool                   needsInitPosPickingShape( Actor* pActor );

        PickingShapePluginData  *createPickingShapeData(PickingShape *);

        u32                     getActorInfoMask();
        bbool                   hasActorInfoFlag(u32 _flag);
        void                    setActorInfoFlag(u32  _flag) {pushInfoFlags(bfalse); m_showActorInfoMask |= _flag;}
        void                    switchActorInfoFlag(u32  _flag) {pushInfoFlags(bfalse); m_showActorInfoMask ^= _flag;}
        void                    clearActorInfoFlag(u32  _flag) {pushInfoFlags(bfalse); m_showActorInfoMask &= (~_flag);}
        virtual void            pushInfoFlags(bbool _clear) {m_savedActorInfoMask = m_showActorInfoMask; if (_clear) m_showActorInfoMask = 0;}
        virtual void            popInfoFlags() {m_showActorInfoMask = m_savedActorInfoMask;}
        virtual void            switchInfoFlags() {u32 f = m_showActorInfoMask; m_showActorInfoMask = m_savedActorInfoMask; m_savedActorInfoMask = f;}
        static ActorEditor*     getInstance() {ITF_ASSERT_CRASH(m_instance, "no editor"); return m_instance;}

        void                    registerListener(Interface_ActorEditorListener* _listener, StringID::StringIdValueType _classCRC, bbool _isExternal = btrue);
        static ObjectPath       getObjectPathFromClipboard();
        void                    onFillMainMenu(EditorContextMenu& _menu);
        void                    onMainMenuItemSelected(ContextMenuItem* _item);
        virtual bbool           onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
        virtual bbool           onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );

        void                    changeTemplate(const String & _relativeFilePath, Actor_Template *_template, bbool _useEditorCallbacks);
        bbool                   modifyOverridedTemplate(const String & _relativeFilePath, Actor_Template *_template);
        bbool                   modifyOverridedTemplate(const String & _relativeFilePath, Actor_Template *_copyTemplate, ESerializeFlags _saveFlag, ESerializeFlags _loadFlag);

        void                    addActorToIconCreationList(ObjectRef oref);

    private:

        bbool                   replaceOneActorTemplate(Actor* _target, const String& _path);
        void                    xFlipSelectedActors()const;
        void                    spawnFromCreateActorBuffer();
        void                    enterCreateBind(const Actor* _parent);
        void                    exitCreateBind();
        static bbool            findBindTarget(const Actor* _pObj, const Vec2d& _screenSpacePos, ActorBind& _result);
        static bbool            getBindTargetInfo(const Actor* _pChild, Vec3d& _targetPosition, f32& _targetAngle, bbool& _targetFlip);
        static bbool            canBindActors(const Actor* pParent, const Actor* pChild);
        static void             unbindActor(Actor* pActor, bbool _parent, bbool _children); // generic way to unbind
        void                    toggleSubSceneLock(SubSceneActor* _pSSA)const;
        void                    pickFogColor();
        void                    pasteFogColor();
        void                    pickFactorColor();
        void                    pasteFactorColor();

        struct ActorEdListener
        {
            Interface_ActorEditorListener*          m_listener;
            SafeArray<StringID::StringIdValueType>  m_listenedComponents;
            bbool                                   m_isExternal;
        };

        ITF_VECTOR<ActorEdListener>  m_listeners;

        const ITF_VECTOR<ActorEdListener>& getActorEditorListeners()const { return m_listeners; }

        void                    unregisterAllListeners      ();
        void                    selectActor                 ( Actor* _pActor, bbool _bDoSelect, const PickingInfo2D* _info );
        void                    copyObjectPathToClipboard   () const;
        void                    beginBindActorToFrieze      ();
        void                    leaveSubEdit                ( Actor* _pActor );

        bbool                       m_nextCreateSceneIsGroup;
        bbool                       m_nextCreateSceneIsMerge;
        Path                        m_nextCreateScenePath;

        state                       m_state;
        SafeArray<PickingInfo2D>    m_selectionInfoList;
        String                      m_nextCreationScriptRef;
        String                      m_lastSpawnedPath;
        u32                         m_creationRequestFrame;
        bbool                       m_isRotationScaleHelperMode;
        String                      m_lastDragnDropFileName_Relative;
        bbool                       m_willCheckForMultipleCreation;

        bbool                       m_createBindState_enabledByShortcut;
        bbool                       m_keepBindTarget;
        ActorBind                   m_bindTargetParent;
        ActorRef                    m_potentialChildForBind;
        ActorRef                    m_setSubScenePivotActor;

        static u32                  m_showActorInfoMask;
        u32                         m_savedActorInfoMask;
        static ActorEditor*         m_instance;
        ArchiveMemory*              m_createActorBuffer;

        ObjectRefList               m_addedActiveActorsLastFrame; // used to create picking shapes the next frame at their creation when modified by engine process
#ifdef DAN_TEXTURE_SELECTOR
    private :
        Vector<String> GetTexturePathFromActor(Actor* target);
        void RecookWithSampler(Actor* target, String& sampler);
#endif
    };

    void    onActorHotreload    (Actor* _actor, bbool _before);

} // namespace ITF


#endif // _ITF_ACTOREDITOR_H_
