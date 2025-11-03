///////////////////////////////////////////////////////////////////////////////
///////////////////////       Editor's class definition
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_EDITOR_H_
#define _ITF_EDITOR_H_

#pragma once

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_
#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_EDITOR_ICONSTOCK_H_
#include "editor/Editor_IconStock.h"
#endif //_ITF_EDITOR_ICONSTOCK_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_EDITOR_TYPES_H_
#include "editor/Editor_Types.h"
#endif //_ITF_EDITOR_TYPES_H_

#ifndef _ITF_SELECTION_H_
#include "editor/selection.h"
#endif //_ITF_SELECTION_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

#if !defined(ITF_FINAL)
#ifndef _ITF_COMMANDHANDLER_H_
#include "engine/debug/CommandHandler.h"
#endif //_ITF_COMMANDHANDLER_H_
#endif

namespace ITF
{

// Class predeclarations
class Actor;
class Texture;
class ColScene;
class KeyFrame;
class Selection;
class ColParticle;
class ArchiveMemory;
class OGLMouseHandler;
class PickingShape_Disc;
class SubSceneComponent;

#define MAX_KEY                             512
#define MULTI_SELECTION_KEY                 KEY_LCTRL
#ifndef EDITOR_CAMERA_KEY                   //So that you can overwrite it
#define EDITOR_CAMERA_KEY                   KEY_SPACE
#endif
#define EDITOR_SMOOTH_KEY                   KEY_LSHIFT
#define MULTI_DUPLICATION_KEY               KEY_LCTRL
#define MIN_MOUSE_MOVE_PIXEL_SQR_DRAG       1.0f
#define MIN_MOUSE_MOVE_PIXEL_SQR_COPY       100.0f
#define MIN_MOUSE_MOVE_PIXEL_SQR_ROTATE     25.0f


class Editor : public Interface_InputListener
    , public Interface_SceneEventsListener
    , public Interface_WorldEventsListener
    ,public TemplateSingleton<Editor>
#if !defined(ITF_FINAL)
    , public Interface_CommandHandler
#endif
{

public:

    Editor();
    ~Editor();

    static const i32 hide2d_dbg = 1;
    static const i32 hide2d_2dActors = 2;
    static const i32 hide2d_mask = 3;
    i32 m_hide2d;

    typedef struct copyProperties
    {
        Color m_color;
        Color m_colorFog;
    } copyProperties;

    typedef struct contextMenuParams
    {
        bbool m_show;
        bbool m_reFillContent;
        ObjectRef m_forceTarget;
        bbool m_keepPos;
        bbool m_active;
    } contextMenuParams;

    const Editor& operator = ( const Editor& /*oEd*/ )
    {
        ITF_ASSERT_MSG(0, "forbidden operation");
        return *this;
    }

    // available edition states
    enum EditorState
    {
        EditorState_Idle    =   0,
        EditorState_EditMode,
        EditorState_EditMode_SelectDepthRange,
        EditorState_EditMode_Measure,
        EditorState_PluginFocus,
        EditorState_Count,
        ENUM_FORCE_SIZE_32(0)
    };

    static f32 fCharMaxHeight;
    static f32 fTextSubOffset;
    static f32 fCharMaxWidth;
    static f32 fCharMaxHeight_Big;
    static f32 fTextSubOffset_Big;
    static f32 fCharMaxWidth_Big;

    static  Editor* getSingleton();

    static bbool   m_2DSelection;

    static void switch2DSelection() {m_2DSelection = !m_2DSelection;}
    static bbool is2DSelection() {return m_2DSelection;}

    void                    start();
    void                    stop();
    virtual EditorState     getState                (   ) const;
    i32                     setState                (   const EditorState _state    );

    void                    clearScene              (   );
    void                    startFrame              (   );
    void                    update                  (   double _elapsed );
    void                    preDraw                 (   );
    void                    draw                    (   );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// preExit is called if the engine is about to be closed, it's used to check for unsaved changes or cancel exit
    /// @return btrue if the exit process should continue
    bbool                   validateExitRequest     (   );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// request a UHD screenshot next frame
    void                    requestUHDScreenshot    (   ) { m_requestedHDScreenshot = btrue;}
    void                    requestNormalScreenshot    (   ) { m_requestedNormalScreenshot = btrue;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// test and clear UHD screenshot flag
    bbool                   testAndClearUHDScreenshotFlag() {bbool old = m_requestedHDScreenshot; m_requestedHDScreenshot = bfalse; return old;}
    bbool                   testAndClearNormalScreenshotFlag() {bbool old = m_requestedNormalScreenshot; m_requestedNormalScreenshot = bfalse; return old;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// show given icon at mouse position. Intended for mouse (special display attributes)
    /// @param _id the icon ID
    /// @param _angle the icon angle
    /// @param _forcedPos the 2D screen pos
    void                    showMouseIcon       (Editor_IconStock::EditorIconID _id, f32 _angle = 0.f, const Vec2d* _forcedPos = NULL);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get Icon Stock for editor
    Editor_IconStock&       getIconStock        () {ITF_ASSERT(m_iconStock); return *m_iconStock;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the screen space mouse, (0, 0) is the top left screen corner
    /// @return the screen space mouse
    ITF_INLINE const Vec2d& getMouse2d() const      { return m_currentMouse2d; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the projected mouse (see updateMouse3d())
    /// @return the projected mouse
    ITF_INLINE const Vec3d& getMouse3d() const      { return m_currentMouse3d; }


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Returns 3D mouse, but without the projection on Working Depth plane
    ITF_INLINE const Vec3d& getMouse3d_NoWorkingDepth() const    { return m_currentMouse3d_NoWorkingDepth;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the mouse projected at the current working depth
    /// @return the projected mouse
    ITF_INLINE const Vec3d& getMouseOnCurrentWorkingDepth() const      { return m_mouseOnCurrentWorkingDepth; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// convert a 2d position to a 3D position, on a plane. 
    /// NB : CAMERA->apply() must be called before
    /// @param _2dpos the 2D position
    /// @param _z the plane Z
    /// @param _3dpos the 3D position (if any)
    /// @return btrue if intersection
    bbool                   convert2dTo3dPositionOnPlane(const Vec2d &_2dpos, f32 _z, Vec3d &_3dpos);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// snap the position to the grid or a shape or a measure point, if necessary
    /// @param _posWorld the position to snap
    /// @param _positionOwner the owner of the position
    /// @param _ignoreSelection if true do not snap on selection
    /// @return btrue if snapped
    bbool                    snap(Vec3d &_posWorld, PickingShape* _positionOwner, bbool _ignoreSelection = btrue);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Compute the delta 3D move for a biased cameragetDeltaMoveForBiasedCamera
    /// @param _x the x delta in screen space
    /// @param _y the y delta in screen space
    /// @param _z the Z factor in world space
    /// @param zoomDirection the zoom direction
    /// @param _z2D the projected Z under the mouse
    static Vec3d            getDeltaMoveForBiasedCamera(f32 _x, f32 _y, f32 _z, const Vec3d &zoomDirection, f32 _z2D = -1.0f);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get the main picking shape (the anchor) for the given object
    /// @param _obj the object
    /// @param the main picking shape, or NULL if none
    PickingShape*           getMainPickingShapeForObject(const ObjectRef _obj)const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// is the object in the m_pickableObjects list? (updated at each update() )
    /// @param _obj the object to test
    bbool                   isPickableObject(ObjectRef _obj);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// In some cases, when an object is in a group, selecting the object will actually select its owner group/subscene.
    /// @param _object : the ObjectRef to check
    /// @param _resultingContainer : the container, if the function returns btrue.
    /// @return btrue if this object should not be selected individually, but its containing scene should be instead
    bbool                   checkIfMustSelectContainerForObject(const ObjectRef &_object, ObjectRef &_resultingContainer);


    bbool                   isPickingInfoValid()                                    { return getPickingShapeOfObjectUnderMouse() != NULL; }

    PickingShape*           getUnderMouseRealPickingShape() const                   { return m_underMouseRealPickingShape; }
    void                    setUnderMouseRealPickingShape(PickingShape *_shape)     { m_underMouseRealPickingShape = _shape; }
    PickingShape*           getPickingShapeOfObjectUnderMouse()const                { return m_pickingShapeOfObjectUnderMouse; }
    void                    setPickingShapeOfObjectUnderMouse(PickingShape* _ps)    { m_pickingShapeOfObjectUnderMouse = _ps; }
    PickingShape*           getLastClickedShape()const                              { return m_lastClickedShape; }
    void                    setLastClickedShape(PickingShape* _ps)                  { m_lastClickedShape = _ps; }

    i32                     getIndexOfShapeInSelection(const PickingShape* _pShape)const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the scene pointer currently edited
    /// @return a scene pointer
    Scene*                  getEditedScene()const;

    void                    setSubEditedItem(ObjectRef _ref);
    ITF_INLINE ObjectRef    getSubEditedItem()const                 { return m_subEdited; }
    ITF_INLINE bbool        isSubEdited(ObjectRef _ref)const        { return getSubEditedItem() == _ref; }

    
    void                    setCancelClickInitTime(f32 _time)       { m_cancelClickInitTime = _time; }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// getEditedSubSceneHolder
    /// @return the sub scene actor ref holder
    ITF_INLINE ObjectRef    getEditedSubSceneHolder()const              { return m_editedSubSceneHolderRef; }
    ITF_INLINE ObjectRef    getWorkingSubScene()const                   { return m_forcedWorkingSubScene; }

    void                    enterEditMode();
    void                    leaveEditMode(bbool _bForce = bfalse, bbool keepEditorStuff = bfalse);
    void                    backToGame(bbool _bKeepEditorStuffs = bfalse);

    bbool                   isMultiSelectionActive()const               { return getSelectionSize() > 0; }
    const SafeArray<PickingShape*>& getSelectionList() const            { return m_selection; }

    virtual void            onDeleteWorld(World* _pWorld); // inherited from Interface_WorldEventsListener
    virtual void            onSwitchToWorld(World* _pWorld, bbool _bSwitchDone); // inherited from Interface_WorldEventsListener
    virtual void            onStartDrawProcess(); // inherited from Interface_WorldEventsListener
    virtual void            onEndDrawProcess(); // inherited from Interface_WorldEventsListener
    virtual void            onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
    virtual void            onEndUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
    virtual void            onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener

    // behave as if the parameter was the only selected object and not subedited
    void                    onLoseFocus();
    void                    onGainFocus();
    void                    selectUnderMouse();
    void                    selectObject(ObjectRef _selection, bbool _bClearPreviousSelection);
    void                    selectObjects(const ObjectRefList &_selection, bbool _bClearPreviousSelection);
    void                    selectShape(PickingShape* _shape, bbool _bClearPreviousSelection);
    void                    selectShapes(const SafeArray<PickingShape*>& _shapes, bbool _bClearPreviousSelection);
    void                    selectAll();
    void                    unselectShape(PickingShape* _shape);
    void                    unselectObject(ObjectRef _selection);
    void                    unselectObjects(const ObjectRefList &_selection);
    void                    unselectAll();

    bbool                   deleteGeneric(const SafeArray<PickingShape*>& _shapesWithSameOwner);

    void                    updateFramebufferedPicking(   );

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get the last Hovered frieze or actor for frame buffer picking.
    /// CAUTION : frame buffer active is not always active, and in some cases objectref may be the one of the object's container
    /// @return the object ref of hovered frieze/actor. May be replaced by its container (see code for details)
    ObjectRef               getLastHoveredActorOrFriseObjRef() const {return m_lastHoveredFriseObjRef;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// return last frame when frame buffer picking was run
    u32                     getLastHoveredActorOrFrise_Frame() const {return m_lastHoveredActorOrFrise_Frame;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool                   saveCurrentWorld(bbool _saveAs, bbool _forceSaveUnmodified);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// If there is some unsaved work then it will ask the user for immediate save
    /// @return btrue if the user didn't canceled
    virtual bbool           checkAndAskForUnsavedWork();

    const SafeArray<PickingShape*>*      getPickingShapesForObject(const ObjectRef _obj)const;
    void                    addPickingShapeForObject(const ObjectRef _obj, PickingShape* _pick);
    bbool                   deletePickingShapesForObject(const ObjectRef _obj);
    bbool                   deletePickingShape(PickingShape* _shape);

    bbool                   checkObjectVisible(ObjectRef _or);
    void                    deletePickingShapes();

    bbool                   isContextMenuVisible();

    u32                     getSelectionSize() const            { return getSelectionList().size(); }
    PickingShape*           getSelectionAt( u32 _i ) const      { return m_selection[_i]; }
    bbool                   removeMainShapeFromSelection( ObjectRef _obj );

    ObjectRef               getContextMenuTarget() {return m_contextMenu[0].m_lastTarget;}
    void                    onFileDrop (const String& _fileName);
    void                    resetcontextMenus();
    EditorContextMenu&      getContextMenu();
    void                    forceShowContextMenu(bbool _show, bbool _reFillContent = bfalse, ObjectRef _forceTarget = ITF_INVALID_OBJREF, bbool _keepPos = bfalse);

    void                    setForcedWorkingDepth(const DepthRange& _dr);
    const DepthRange&       getForcedWorkingDepth()const                    { return m_forcedWorkingDepth; }
    bbool                   isWorkingDepthForced()const                     { return m_forcedWorkingDepth.isValid(); }
    void                    resetForcedWorkingDepth();
    DepthRange              getWorkingDepth()const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Called to startToEditSubScene with a custom selection
    /// @param _pSSA The subscene actor holder
    /// @param _newStartingSelection the new custom selection
    void                    startToEditSubScene(const SubSceneActor* _pSSA, const ObjectRefList& _newStartingSelection);

    void                    setContextMenuShowable(bbool _show);
    void                    setTopBarMenuShowable(bbool _show);
    bbool                   isContextMenuShowable();
    bbool                   isTopBarMenuShowable();

    contextMenuParams       m_nextMenu;
    void                    deleteSelection(const ObjectRefList* _refList = NULL);
    void                    newScene();
    bbool                   openScene(bbool _checkForUnsavedChanges);
    void                    reloadCurrentMap();
    void                    fillPickingInfos();

    void                    teleportCheat();

    SubSceneActor*          createEmptyGroup(const Vec3d& _worldPos, Scene* _pDestScene)const;

    void                    newTabFromMenu();

    void                    registerInWorldCellsDuringPause(ObjectRef _or) {m_pickablesForPauseCellUpdate.push_back(_or);}


    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Transform a selection into an embedded sub scene
    ObjectRef               groupSelection(const ObjectRefList& _refList, Scene* _dstScene, SubSceneActor* _pSubSceneActor = NULL);
    bbool                   canGroupSelection(String& _reason, ObjectRefList& _refList, Scene* _dstScene)const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Transform an embedded subscene into a regular selection
    void                    ungroupSelection(ObjectRef _ref, bbool _popFromGroup);
    bbool                   canUngroupSelection(String& _reason, ObjectRef _ref);


    bbool                   canTransfertSelection(String& _reason, ObjectRefList& _refList, Scene* _dstScene)const;

    bbool                   isDragActive()const                         { return m_bDragSelection; }
    bbool                   isDragMinDistReached(f32 _limitSqr)const    { if (m_cancelClickTime > 0.f) return bfalse; return m_fDragSqrDistance > _limitSqr; }
    void                    endDrag();
    bbool                   zoneSelectionInProgress()const              { return m_zoneSelectionStarted; }
    void                    endZoneSelection();
    void                    startRotation(Vec3d* _rotationCenter = NULL);
    bbool                   isRotationActive()const                     { return m_bRotateSelection; }
    void                    endRotation();

    void                    onPickingShapeDeleted(PickingShape* _pick);

    // For helpers
    bbool                   isInRotationScaleHelperMode()const              { return m_rotationScale_focus.isValid(); }
    void                    startRotationScaleHelperMode(ObjectRef _object) { m_rotationScale_focus = _object; }
    void                    endRotationScaleHelperMode()                    { m_rotationScale_focus = ITF_INVALID_OBJREF; }

    Vec3d                   getCentroidSelection(const ObjectRefList& _refList, bbool _skipPlayers) const;
    ObjectRefList           getEditorSelectionAsObjRefList()const;

    bbool                   isPanActive()const                              { return m_bIsPanActive; }

    void                    requestMouseFeedback(CursorIcon::Type _type);

    void                    importSceneAsMerge(Scene *_dstScene, const Vec3d& _pos, const Scene* _srcScene);
    void                    importSceneAsMerge(Scene *_dstScene, const Vec3d& _pos, const Path& _srcScenePath);
    SubSceneActor*          importSceneAsInstance(Scene *_scene, const Vec3d& _pos, const String& _srcScenePath);
    void                    copySelectionData();
    void                    pasteCopyData();
    void                    pasteSceneContentAtMouse(Scene* _sceneToPaste);

    void                    addToHiddenList(Pickable* _pObj);
    void                    removeFromHiddenList(Pickable* _pObj);
    u32                     getPluginsInfoFlags() {return m_usePluginsInfoFlags;}
    bbool                   authorizeDeactivatedObjects() {return m_authorizeDeactivatedObjects;}
    void                    applyPendingDragAndDropMap();
    void                    clearExtraSelectableObjects() {m_extraSelectableObjects.clear();}
    void                    addExtraSelectableObject(ObjectRef _or) {if (m_extraSelectableObjects.find(_or) < 0) m_extraSelectableObjects.push_back(_or);}
    const ObjectRefList&    getExtraSelectableObjects() {return m_extraSelectableObjects;}
    u32                     getDisplayedFamilies() const {return m_DisplayedFamilies;}
    void                    setDisplayedFamilies(const u32 _displayed) {m_DisplayedFamilies = _displayed;}
    bbool                   hasDisplayedFamilies(const u32 _displayed) {return (m_DisplayedFamilies & _displayed) != 0;}
    void                    addDisplayedFamilies(const u32 _displayed) {m_DisplayedFamilies |=  _displayed;}
    void                    removeDisplayedFamilies(const u32 _displayed) {m_DisplayedFamilies &=  ~_displayed;}
    void                    switchDisplayedFamilies(const u32 _displayed) {m_DisplayedFamilies ^=  _displayed;}
    const ITF_VECTOR<SMultiFilter>&  getMultiFilter() const {return m_multiFilter;}
    f32                     getAngleStepDegree()const                   { return m_angleStepDegree; }
    bbool                   snapAngle(f32& angle)const;
    bbool                   copyFileToDataFolder(const String& _absoluteFilePath, Path& _newRelativeFilePath)const;
    u32                     getGridMode()const                          { return m_gridMode; }
    f32                     getGridUnit()const                          { return m_gridSize; }
    void                    swapCurrentWorldWith(ObjectRef _worldRef, TabEditInfo* _infos);
    void                    switchDebugCellsMode();
    bbool                   isDebugCellsCamMode() const                 { return m_debugCellsCamMode; }
    bbool                   applicationHasFocus()const                  { return m_applicationHasFocus; }

    void                    setWorkingSubscene(const SubSceneActor* pSSA);
    void                    toggleSelectionFlipX()const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Rename duplicate UserFriendly in the given World at same level of hierarchy (same scene)
    /// @param _context (World *) the world containing the pickables
    static void             renameDuplicateUserFriendly(World* _context);

    void                    toggleActorIconDisplay();
    void                    toggleFriezePivotDisplay();
    void                    toggleFriezePointsDisplay();
    void                    toggleFriezeCollisionsDisplay();
    void                    toggleSubSceneContentShapesDisplay();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Switch on/off autopause mode
    void                    changeAutoPauseMode();
    void                    setAutoPauseMode(bbool _autoPause);

    void                    addPickableToRefresh(ObjectRef _ref) {if (m_pickablesToRefresh.find(_ref) <0 ) m_pickablesToRefresh.push_back(_ref);}

    static bbool            isOfficialScene(const Scene& _scene);

public: // Interface_SceneEventsListener
    virtual void onSceneAsyncLoadFinished(const Scene* _pScene);
    virtual void onScenePreDestroy(Scene* /*_pScene*/) {}
    virtual void onDeleteScene(const Scene*_pScene);
    virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {}
    virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {}
    virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/) {}

    void                        resetPause();
    void                        pushPause();
    void                        popPause();

public:
    bbool                       isAllowStorePickableInitData    (Pickable *_pick);
    void                        storePickableCurrentToInitData  (Pickable *_pick);

    bbool                       allowEditSubScene(const SubSceneActor* _pSSA)const;

    void                        setStorePickableInitData        (bbool _store)          { m_storePickableInitData = _store; }
    void                        ignoreStorePickableInitData     (Pickable *_pick);
    void                        allowStorePickableInitData      (Pickable *_pick);
    void                        allowStoreAllPickableInitData   ()                      { m_ignoreStorePickableInitDataList.clear(); }

    copyProperties&             getCopiedProperties             ()                      { return m_copiedProperties; }

    bbool                       displayAllSubContent()const                             { return m_showSubsceneContentShapes; }
    const ObjectRefList&        getSubSceneDisplayList()const                           { return m_displaySubSceneContent; }
	void						setObjectFamilyAsPickableForFrameBuffer(u32 _fam)       {m_FrameBufferPickingObjectsFamilies |= _fam;}
	void						unsetObjectFamilyAsPickableForFrameBuffer(u32 _fam)     {m_FrameBufferPickingObjectsFamilies &= ~_fam;}
	bbool						isObjectFamilyPickableForFrameBuffer(u32 _fam)          {return (m_FrameBufferPickingObjectsFamilies & _fam) != 0;}
	void						switchObjectFamilyAsPickableForFrameBuffer(u32 _fam)    {if (isObjectFamilyPickableForFrameBuffer(_fam)) unsetObjectFamilyAsPickableForFrameBuffer(_fam); else setObjectFamilyAsPickableForFrameBuffer(_fam);}

    virtual void                setSceneAsModified(const Scene* _pScene);
    bbool                       sceneHasBeenModified(const Scene* _pScene)const;

    void                        preUndoRedoOperation();
    void                        postUndoRedoOperation();

    u32                         getTabsCount()const                                     { return m_tabList.size(); }
    const TabEditInfo&          getTabAt(u32 _idx)const                                 { return m_tabList[_idx]; }
    bbool                       getTabEditInfo(struct TabEditInfo& _infos);

    void                        fillAssociatedTabList(Scene* _pScene)const;
    Scene*                      getFirstObjectSelectedScene()const;
    bbool                       isUpdateMultiFilter() {return m_isUpdateMultiFilter;}
    bbool                       mustShowWorldOrigin() {return m_showWorldOrigin;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Request editor loading of a scene or a worldfile
    /// @param _relativeMapPath (const String &) the relative path to the map to be loaded
    void                        setPendingDragAndDropMap(const String& _relativeMapPath) { m_pendingDropMap = _relativeMapPath; }

    void                        onObjectDeleted(BaseObject* pBO);

private:

    bbool                       m_storePickableInitData;
    SafeArray<ObjectRef>        m_ignoreStorePickableInitDataList;
    void                    addMultiFilter(const SMultiFilter& _filter);
    void                    clearMultiFilter() {m_multiFilter.clear();m_isUpdateMultiFilter = bfalse;}

private:
    void                    processPickableToRefresh();
    void                    processJoinGroupList();
    void                    setJoingroupListTarget(SubSceneComponent* _pScene);
    void                    setAppTitleScene(const Scene* _pScene)const;
    void                    setPanActive(bbool _bStatus)                { m_bIsPanActive = _bStatus; }
    void                    updateWarningFilter();
    void                    setOrphanTabsOwner(World* _owner);
    void                    processRegisterInWorldCellsDuringPause();

    bbool                   loadTabInBackground(const Path& _scenePath, World* _ownerWorld);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Toggle the ratio between 4/3 and 16/9
    /// @return (u32) the new window ratio
    f32                     toggleWindowRatio();

    bbool                   m_debugCellsCamMode;
    Vec3d                   m_debugCellsCamPos, m_debugCellsRestorePos;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Find duplicate friendlies at same level (in same scene)
    /// @param _context (World *) the world containing the pickable to check
    /// @param _duplicates (PickableList &) the resulting duplicate
    /// @return (u32) the number of results
    static u32              findDuplicateFriendly(World* _context, PickableList& _duplicates);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// snap a 3D position onto the grid
    /// @param _3dpos [in,out] the 3D position. m_z is used to temporarily affect the grid
    /// btrue if snapping occured
    bbool                   snapToGrid_3d(Vec3d &_3dpos);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// snap mouse to grid, if necessary
    /// @param _pos the 2D position to snap
    /// @param _snapped3DPos [out] if non-null and btrue is returned, contains the snapped 3D position on the grid
    /// @return btrue if snapped
    bbool                   snapToGrid(Vec2d &_pos, Vec3d *_snapped3DPos=NULL);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// snap position to nearest shape position, if necessary
    /// @param _pos the position to snap, in out
    /// @param _positionOwner do not snap with this object
    /// @param _ignoreSelection do not snap with selection ?
    /// @param _targetShape the shape used to snap, unmodified if false is returned
    /// @return btrue if snapped
    bbool                   snapToNearestShape(Vec3d &_pos, PickingShape* _positionOwner, bbool _ignoreSelection, PickingShape** _targetShape);

    f32                     getSnappingDistanceInPixel()const   { return 8.0f; }
    bbool                   m_snapByDefault;
    bbool                   m_showSubsceneContentShapes;

    ITF_MAP<ObjectRef, SafeArray<PickingShape*> > m_pickingShapes;
    Editor_IconStock        *m_iconStock;

    ObjectRef               m_lastHoveredFriseObjRef;
    PickingShape*           m_pickingShapeOfObjectUnderMouse;
    PickingShape*           m_underMouseRealPickingShape;
    PickingInfo2D           m_underMouseShapePickingInfo; // use only if isPickingInfoValid()
    f32                     m_SaveIconTimer, m_InitialInfoTimer;

    EditorState             m_state;
    bbool                   m_bIsPanActive;
    Vec3d                   m_vPanProjectedOrigin;
    ObjectRef               m_subEdited;
    ObjectRef               m_editedSubSceneHolderRef;
    int                     m_camBiasMouseX, m_camBiasMouseY;
    f32                     m_camBiasHorizAngle, m_camBiasVertAngle;
    u32                     m_lastCameraRotationFrame;


    // General
    void                    refillPickableList();
    void                    goBackOneStep();
    void                    handleRightMB(bbool _isPressed);
    void                    handleLeftMB(InputAdapter::PressStatus _status);
    void                    handleLeftMB_Pressed(bbool _bMultiSelectionOrDuplicationKeyPressed);
    void                    handleLeftMB_Release(bbool _bMultiSelectionOrDuplicationKeyPressed);
    void                    dispatchDrawForPickingShapes(PickingShapeVector *_orderedShapes=NULL/*if null, use m_activeShapesListOrdered*/);
    void                    dispatchMousePressedForPickingShapes();
    void                    dispatchMouseReleasedForPickingShapes();
    void                    dispatchMouseMoveForPickingShapes();
    void                    dispatchMouseWheelForPickingShapes(i32 _wheelDelta);
    void                    handleMiddleMB(InputAdapter::PressStatus _status);
    bbool                   handleCameraRotationMode(bbool _reset);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Compute and store in m_mouse3d the projection of mouse2d at the current selected depth range, could be improved by projecting at undermouse depth
    /// @param _mouse2d the mouse position in screen space
    void                    updateMouse3d(const Vec2d& _mouse2d);

    void                    updatePanModeStatus();
    void                    resetPickingInfo(bbool _bUnselect = bfalse);
    void                    update_handleJoystick(double _elapsed);
    void                    update_handleTranslation(double _elapsed);
    void                    update_handleMouseDragged();
    bbool                   computeSelectionCentroidWorldPosition(Vec3d& _centroid)const;
    bbool                   isPickableUnderPoint(const Pickable *_object, const Vec2d &_point, Vec3d &_intersection);
    void                    updateSelectionPickingInfos();
    void                    getClipboardData();
    void                    drawObj(BaseObject* pObj, bbool mouseOver);
    void                    dispatchUpdateForPickingShapes();
    void                    displayObjectEditingInfos(ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Pickable *_underMouse);

    // Inputs Listener inherited
    bbool                   onMouseButton   (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    bbool                   onMousePos      (   i32 _x, i32 _y );
    bbool                   onMouseWheel    (   i32 _wheelValue, i32 _wheelDelta );
    bbool                   onKey           (   i32 _key, InputAdapter::PressStatus _status );

    // Interface_CommandHandler
    bbool                   onKeyProcess    (i32 _key, InputAdapter::PressStatus _keyStatus);

    // Multiselection stuff
    i32                         getRefIndexInSelection(const ObjectRef& _ref)const;

    void                        startZoneSelection();

    bbool                       m_zoneSelectionStarted;
    Vec2d                       m_zoneSelectionStart; // in screen coord

    Vec2d                       m_previousMouse2d;
    Vec2d                       m_currentMouse2d;
    Vec3d                       m_currentMouse3d;
    Vec3d                       m_mouseOnCurrentWorkingDepth;
    Vec2d                       m_frameDeltaMouse2d;

    Selection*                  m_pSelectionObject;

    SafeArray<PickingShape*>    m_selection;            // contains picking shape's ref

    void                        startDrag();
    bbool                       m_bDragSelection;
    f32                         m_fDragSqrDistance;

    bbool                       m_bRotateSelection;
    f32                         m_fRotationQty;
    Vec3d                       m_RotationOrigin;

    void                        startDuplicateSelection();
    bbool                       m_bDragAfterDuplicate;
    void                        endDuplicateSelection();

    PickingShape*               m_lastClickedShape;

    bbool                       m_bUnselectOrDuplicate;

    void                        handleSelectionDuplication();
    void                        handleSelectionRotation();
    void                        handleSelectionOffset();

    void                        offsetSelection(const Vec3d& _offset, bbool _singleAction = btrue)const;
    void                        alignSelection(bbool _x, bbool _y, bbool _z)const;

    ObjectRef                   getFirstSelectedObjRef()const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// called to end the sub scene edition, can lead to sub scene reloading
    void                        endSubSceneEdition();

    void                        toggleMeasureMode();
    void                        clearMeasures()                 { m_MeasureLines.clear(); }
    bbool                       snapOnMeasurePoint(Vec3d& _pos)const;

    SafeArray<Vec3d>            m_MeasureLines;

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)

    // Import / Export selection

    World*                      exportSelection(bbool _transformToRelative)const;
    bbool                       exportSelectionToScene(const String& _absolutePath)const;
    void                        importScene();

    void                        convertSelectionToScene();
    void                        mergeSelectionOnMainScene();

#endif // ITF_FINAL

    void                        destroyCopyPasteData();

    ObjectRefList               m_copySourceObjects;
    ArchiveMemory*              m_pCopyDataContainer;
    Vec3d                       m_selectionCentroidWhenCopy;

    
    void                        increaseGridSize() { m_gridSize *= 2.0f; }
    void                        decreaseGridSize() { if(m_gridSize > 0.01f) m_gridSize *= 0.5f; }

    u32                         m_gridMode; // 0 hide, 1 scene depth, 2 object depth
    Vec3d                       m_gridPoint;
    const f32                   m_defaultGridSize;
    f32                         m_gridSize;
    f32                         m_angleStepDegree;
    u32                         m_lastHoveredActorOrFrise_Frame;
    f32                         m_hoveredFriseOrActorProjectedZ;
    bbool                       m_bRequireHoveredActorOrFriseDetection;
    bbool                       m_resetCameraRotation;
    bbool                       m_bMayDuplicateSelectedAfterDrag;
    
    // Double-click detection to prevent unwanted duplication
    f32                         m_lastClickTime;
    bbool                       m_justDoubleClicked;

    // Contextual menu
    void                        showContextMenu(bbool _onOff, ObjectRef _forceTarget = ITF_INVALID_OBJREF, bbool _keepPos = bfalse);
    void                        switchContextMenu();
    bbool                       handleContextMenu();
    void                        fillTopItemsContextMenu(ObjectRef _target);
    void                        fillNormalContextMenu(ObjectRef _target);
    void                        fillAltContextMenu(ObjectRef _target);
    bbool                       handleContextMenuItemClicked(ContextMenuItem* _item, ObjectRef _menuTarget);
    void                        hideSelected();
    void                        unhideAll();
    void                        hideUnselected();
    EditorContextMenu           m_contextMenu[2];   // 0 : Right menu. 1: left menu
    EditorContextMenu           m_topBarMenu;
    void                        updateTopBarMenu();
    void                        teleportSelectionToMouse()const;

    void                        camFocusOnSelection()const;
    static bbool                getFullPath(const BaseObject* _pObject, String& _fullPath);

    Vec3d                       limitRayToWorldAABB(const Vec3d &v0, const Vec3d &v1, bbool mustLimitWithGreatestDistance)const;
    void                        getWorldAABB(AABB &_aabb, f32 &_minZ, f32 &_maxZ)const;

	void						toggleDisplayMemory() {m_displayMemory =!m_displayMemory;}

    bbool                       updateTabEditInfo(TabEditInfo& _infos);

    void                        cancelEditActions();

    void                        updateDebugInfo()const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Show a input text dialog and search for a matching friendly and focus on the result
    void                        friendlyFinder(const String* _search = NULL);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Show a input text dialog to change the current grid unit
    void                        chooseGridUnit();

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Show a input text dialog to change the current angle step
    void                        chooseAngleStep();

	///////////////////////////////////////////////////////////////////////////////////////////
	/// Set the whole map music theme
	void                        setMusicTheme(const StringID& _theme);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Set the whole map music theme
    void                        MuteMusic(bbool _mute);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Rename the selection, also add a counter if selection is multiple
    void                        renameSelectionFriendlyNames();

    void                        setTabMainScenesModified();

    void                        resetSceneModifiedList()                            { m_modifiedSceneRefList.clear(); }

    u32                         m_lastHandleContextMenuFrame; // used to avoid rebounds when changing menus target


    // After filling this container must be sorted by shape priority, the first element has the priority over following shapes
    PickingShapeVector          m_activeShapesListOrdered;
    ObjectRefList               m_pickableObjects, m_joingroupList;

    ObjectRefList               m_hiddenObjectList;
    ObjectRefList               m_pickablesToRefresh;
    ObjectRefList               m_pickablesForPauseCellUpdate;

    ObjectRef                   m_rotationScale_focus;

    CursorIcon::Type            m_feedbackRequest;
    u32                         m_feedbackRequestFrame;
    bbool                       m_bcancelFirstClick;
    f32                         m_cancelClickInitTime;
    f32                         m_cancelClickTime;
    u32                         m_usePluginsInfoFlags;
    bbool                       m_authorizeDeactivatedObjects;
    String                      m_pendingDropMap;
    ObjectRefList               m_extraSelectableObjects;
    u32                         m_DisplayedFamilies;
    Vec2d                       m_selectionDeltaToSnap;
    ITF_VECTOR<SMultiFilter>    m_multiFilter;
    bbool                       m_isUpdateMultiFilter;
    String                      m_lastCtrlFString;
    u32                         m_pauseCount;
    void                        swapTab(bbool _next);
    void                        preSwapCurrentWorld(bbool preventCamModification);
    ContextMenuItem*            m_menuWorlds;    
    bbool                       m_autoPause, m_pausedByUser;

    Vector<TabEditInfo>         m_tabList;      // list of swapable worlds
    Vector<TabEditInfo>         m_keepTabList;  // list of swapable ketp over a map reload

    bbool                       m_applicationHasFocus;
    bbool                       m_requestedHDScreenshot;
    bbool                       m_requestedNormalScreenshot;
	bbool						m_displayMemory;
    DepthRange                  m_forcedWorkingDepth;
    SubSceneComponent*          m_joinGroupDest;
    u32                         m_FrameBufferPickingAlphaRef;
    ObjectRefList               m_displaySubSceneContent;
	u32                         m_FrameBufferPickingObjectsFamilies;
    Vec3d                       m_currentMouse3d_NoWorkingDepth;
    ObjectRefList               m_modifiedSceneRefList;
    ObjectRefList               m_tabLoadingPendingList;
    ObjectRefList               m_tabOwnerPendingList;
    ObjectRef                   m_forcedWorkingSubScene;

    copyProperties              m_copiedProperties;
    bbool                       m_showWorldOrigin;
};

#define EDITOR                  Editor::getptr()

} // namespace ITF


#endif // _ITF_EDITOR_H_
