#include "precompiled_editor.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef ITF_CONSOLE_FINAL

///////////////////////////////////////////////////////////////////////////////
///////////////////////       Editor's main class implementation
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_


#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EDITOR_GUI_TEXTINPUT_H_
#include "editor/GUI.h"
#endif // _ITF_EDITOR_GUI_TEXTINPUT_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_REPORTISSUEUTILS_H_
#include "engine/debug/reportIssueUtils.h"
#endif //_ITF_REPORTISSUEUTILS_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_SELECTION_H_
#include "editor/selection.h"
#endif //_ITF_SELECTION_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#if !defined(ITF_FINAL)
#ifndef _ITF_COMMANDHANDLER_H_
#include "engine/debug/CommandHandler.h"
#endif //_ITF_COMMANDHANDLER_H_
#endif

namespace ITF
{


DECLARE_RASTER(editor_Draw, RasterGroup_Misc, Color::magenta());
DECLARE_RASTER(updateFramebufferedPicking, RasterGroup_Misc, Color::yellow());

const char* helpRelativePath    = "editor/help/index.html";
f32 Editor::fCharMaxHeight      = 15.0f;
f32 Editor::fTextSubOffset      = 5.0f;
f32 Editor::fCharMaxWidth       = 7.0f;
f32 Editor::fCharMaxHeight_Big  = 15.0f;
f32 Editor::fTextSubOffset_Big  = 5.0f;
f32 Editor::fCharMaxWidth_Big   = 7.0f;
bbool Editor::m_2DSelection = bfalse;

void DumpAllFriezesResources();

Editor::Editor()
: m_bIsPanActive(bfalse)
, m_defaultGridSize(0.5f)
, m_state(EditorState_Idle)
, m_camBiasHorizAngle(0.0f)
, m_camBiasMouseX(0)
, m_camBiasMouseY(0)
, m_camBiasVertAngle(0.0f)
, m_bDragAfterDuplicate(bfalse)
, m_pCopyDataContainer(NULL)
, m_feedbackRequest(CursorIcon::Count)
, m_feedbackRequestFrame(0)
, m_menuWorlds(NULL)
, m_snapByDefault(btrue)
, m_showSubsceneContentShapes(bfalse)
, m_applicationHasFocus(btrue)
, m_requestedHDScreenshot(bfalse)
, m_requestedNormalScreenshot(bfalse)
, m_storePickableInitData(btrue)
, m_cancelClickInitTime(1.f)
, m_displayMemory(bfalse)
, m_pSelectionObject(NULL)
, m_showWorldOrigin(bfalse)
, m_selectionDeltaToSnap(Vec2d::Zero)
, m_previousMouse2d(Vec2d::Zero)
, m_currentMouse2d(Vec2d::Zero)
, m_zoneSelectionStart(Vec2d::Zero)
, m_frameDeltaMouse2d(Vec2d::Zero)
{
    EditorContextMenu::registerMenu(&m_topBarMenu, btrue);
    EditorContextMenu::registerMenu(&m_contextMenu[0], bfalse);

    m_iconStock = newAlloc(mId_Editor,Editor_IconStock());
    m_zoneSelectionStarted = bfalse;
    m_bDragSelection = bfalse;
    m_fDragSqrDistance = 0.0f;
    m_bRotateSelection = bfalse;
    m_fRotationQty = 0.0f;
    m_bUnselectOrDuplicate = bfalse;
    m_SaveIconTimer = 0.0f;
    m_InitialInfoTimer = 0.0f;
    m_gridPoint = Vec3d::Zero;
    m_gridSize = m_defaultGridSize;
    m_angleStepDegree = 22.5f;
    m_gridMode = 0;
    m_lastHoveredActorOrFrise_Frame = (u32)-1;
    m_lastCameraRotationFrame =  (u32)-1;
    m_hoveredFriseOrActorProjectedZ = 1.f;
    m_bRequireHoveredActorOrFriseDetection = false;
    m_resetCameraRotation = bfalse; 
    m_bMayDuplicateSelectedAfterDrag = bfalse;
    m_hide2d = 0;

    setUnderMouseRealPickingShape(NULL);
    setPickingShapeOfObjectUnderMouse(NULL);
    setLastClickedShape(NULL);
    m_usePluginsInfoFlags = 0;
    m_cancelClickTime = -1.f;
    m_authorizeDeactivatedObjects = btrue;
    m_DisplayedFamilies = 0xffffffff;
    m_debugCellsCamMode = bfalse;
    m_pauseCount = 0;
    m_autoPause = bfalse;
    m_pausedByUser = bfalse;
    m_joinGroupDest = NULL;
    m_FrameBufferPickingAlphaRef = 80;
    m_FrameBufferPickingObjectsFamilies = 0xffff;
    m_isUpdateMultiFilter = bfalse;
}

Editor::~Editor()
{
    stop();

    m_iconStock->unloadIcons();

    deletePickingShapes();


    SF_DEL(m_iconStock);
}

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
    bbool isHiddenBecauseOfEditor(const Pickable* _pick);
    extern bbool (*isHiddenBecauseOfEditorFilter)(const Pickable*);
#endif
void Editor::start()
{
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        isHiddenBecauseOfEditorFilter = isHiddenBecauseOfEditor;
    #endif

    fCharMaxHeight      = (f32)GFX_ADAPTER->getDebugFontSize();
    fTextSubOffset      = fCharMaxHeight / 3.f;
    fCharMaxWidth       = fCharMaxHeight / 2.14285f;
    fCharMaxHeight_Big  = (f32)GFX_ADAPTER->getBigDebugFontSize();
    fTextSubOffset_Big  = fCharMaxHeight_Big / 3.f;
    fCharMaxWidth_Big   = fCharMaxHeight_Big / 2.14285f;
    m_joinGroupDest     = NULL;
    m_hide2d = 0;
    setState(EditorState_Idle);

    resetPickingInfo();

    m_iconStock->loadIcons();
    SCENE_MANAGER->addListener(this);
    WorldManager::addListener(this);
    
    m_pSelectionObject = newAlloc(mId_Editor, Selection);

    m_nextMenu.m_active = bfalse;
    m_lastHandleContextMenuFrame = CURRENTFRAME;
    m_cancelClickTime = -1.f;

#if 0
    //DEMO PRESS 15 avril
    if (CONFIG->m_noWarnings)
    {
        String c("-ptools -ftodo");
        friendlyFinder(&c);
    }
#endif // 0


#ifdef ITF_WINDOWS
 //   launchExe("UA_FrameWork.exe", NULL, btrue);
#endif

    World::setCellProcessCB(editorDraw_cellProcessCB);

    GFX_ADAPTER->setSafeFrameMode(SF_MINIMUM);
}

void Editor::stop()
{
    destroyCopyPasteData();

    unselectAll();

    // Clear sleeping tabs
    {
        const World* pCurrentWorld = CURRENTWORLD;
        Vector<TabEditInfo> listCopy = m_tabList;
        for(u32 i = 0; i < listCopy.size(); ++i)
        {
            TabEditInfo infos = listCopy[i];
            if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(infos.m_worldUniqueName))
            {
                // Do not delete the current world as the gamemanager will handle it's destruction
                if(pWorld != pCurrentWorld)
                {
                    pWorld->stopPrefetch();
                    ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));

                    const u32 uSize = infos.m_undoRedoData.m_sequenceList.size();
                    for(u32 uSeq = 0; uSeq < uSize; ++uSeq)
                    {
                        SF_DEL(infos.m_undoRedoData.m_sequenceList[uSeq]);
                    }
                }
            }
        }

        ITF_ASSERT(m_tabList.size() == 1); // size() == 0 when exiting during loading of a directly launched map
    }

    if(SCENE_MANAGER)
        SCENE_MANAGER->removeListener(this);
    WorldManager::removeListener(this);

    SF_DEL(m_pSelectionObject);
}

bbool Editor::checkAndAskForUnsavedWork()
{
    bbool cancel = bfalse;

#if defined(ITF_WINDOWS)
    if(UNDOREDO_MANAGER->thereIsSomeModificationSinceLastSave())
    {
        switch(SYSTEM_ADAPTER->messageBox("Unsaved changes", "You will loose your work if you don't save now.\n\n Save now ?", ITF_MSG_ICONWARNING | ITF_MSG_YESNOCANCEL,ITF_IDNO))
        {
        case ITF_IDCANCEL:
            cancel = btrue;
            break;

        case ITF_IDYES:
                ITF_VERIFY_MSG(saveCurrentWorld(bfalse, bfalse), "Failed to save the current world");
            break;

        default:
            break;
        }
    }
#endif // ITF_WINDOWS

    return !cancel;
}

bbool Editor::validateExitRequest()
{
    // DEMO ONLY
    if(SYSTEM_ADAPTER->isFullScreenMode())
        return btrue;

    return checkAndAskForUnsavedWork();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////       Editor's general methods
///////////////////////////////////////////////////////////////////////////////

void Editor::clearScene()
{
    PLUGINGATEWAY->onClearScene();
    unselectAll();

    if (GFX_ADAPTER)
        GFX_ADAPTER->cancelAllTasks();
}


bbool Editor::onKey (i32 _key, InputAdapter::PressStatus _keyStatus) { 
    return btrue; 
}

// return true to let CommandHandler handle the key
bbool Editor::onKeyProcess (i32 _key, InputAdapter::PressStatus _keyStatus)
{
    updatePanModeStatus();

    if (_keyStatus == InputAdapter::Pressed)
    {
        switch (_key)
        {
        case KEY_ESC:
            {
                const bbool keepEditorStuff = INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT);
                showContextMenu(bfalse);
                leaveEditMode(btrue, keepEditorStuff);
                backToGame(keepEditorStuff);
                return bfalse;
            }
            break;
            
        case KEY_BACKSPACE:
            {
                goBackOneStep();
                showContextMenu(bfalse);
                return bfalse;
            }
            break;

        case KEY_TAB:
            {
                swapTab(!INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT));
                return bfalse;
            }
            break;
/*
        case KEY_F1:
            SYSTEM_ADAPTER->launchFromExplorer(DATA_FOLDER + helpRelativePath);
            break;
*/
        case KEY_F2:
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))// && INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    //simulate a crash
#ifndef ITF_FINAL
                    if (SYSTEM_ADAPTER->messageBox("Crash Test","Are you sure you want to make the game crash ?",ITF_MSG_YESNO,ITF_IDNO)== ITF_IDYES)
                    {
                        int *crashTest = NULL;
                        (*crashTest)++;
                    }
#endif //ITF_FINAL
                }
                else
                {
                    renameSelectionFriendlyNames();
                    return bfalse;
                }
            }
            break;
            
        case KEY_F3:
            {
#ifndef ITF_CONSOLE
                EditorDraw::cyclePickableDetailModes();
#endif
                return bfalse;
            }
            break;

        case KEY_F4:
            {
                teleportCheat();
                return bfalse;               
            }
            break;

        case KEY_F5:
            m_pickablesForPauseCellUpdate.clear();
            if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                reloadCurrentMap();
                return bfalse;
            }
            else
            {
                #if !defined(ITF_FINAL)
                    COMMANDHANDLER->loadLastCheckpoint(INPUT_ADAPTER->isKeyPressed(KEY_LALT));                    
                #endif
                return bfalse;
            }
            break;

        case KEY_F6:
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    DumpAllFriezesResources();
                }
                else
                {
                    m_hide2d++;
                    m_hide2d &= hide2d_mask;
                    GFX_ADAPTER->setHideFlags(0);
                    if (m_hide2d & hide2d_dbg)
                        GFX_ADAPTER->addHideFlags(GFXAdapter::HideFlag_DBGPrimitive | GFXAdapter::HideFlag_2DBox);
                    if (m_hide2d & hide2d_2dActors)
                        GFX_ADAPTER->addHideFlags(GFXAdapter::HideFlag_Font);
                }
                return bfalse;
            }
            break;

        case KEY_F7:
            {
                toggleDisplayMemory();
                return bfalse;
            }
            break;

        case KEY_F8:
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    clearMeasures();
                    return bfalse;
                }
                else
                {
                    toggleMeasureMode();
                    return bfalse;
                }
            }
            break;

#if defined(ITF_WINDOWS) 
    #if !defined(ITF_FINAL)
        case KEY_F9:
            {   
                //don t use it reserved for developper (actoreditor on picking)
            }
            break;

        case 'X':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    UndoRedoSequenceScope changes("Cut selection");

                    copySelectionData();
                    deleteSelection();
                    return bfalse;
                }
                else
                {
                    toggleSelectionFlipX();
                    return bfalse;
                }

            }
            break;

        case 'C':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    copySelectionData();
                    return bfalse;
                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    CHEATMANAGER->ResetTimeStepMult();
                    return bfalse;
                }
                else
                {
                    EditorDraw::toggleDrawCells();
                    return bfalse;
                }
            }
            break;

        case 'T':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    reportIssueUtils::generateTrac();
                    return bfalse;
                }
                else
                {
                    teleportSelectionToMouse();
                    return bfalse;
                }
            }
            break;

        case 'V':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    if(m_pCopyDataContainer)
                    {
                        pasteCopyData();
                        return bfalse;
                    }
                }
            }
            break;
            
        case 'G':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    if(getSelectionSize() > 0)
                    {
                        groupSelection(getEditorSelectionAsObjRefList(), getFirstObjectSelectedScene());
                        return bfalse;
                    }
                    else
                    {
                        createEmptyGroup(getMouse3d(), getEditedScene());
                        return bfalse;
                    }
                }
                else
                { 
                    if (m_joingroupList.size())
                    {
                        m_joingroupList.clear();
                        m_joinGroupDest = NULL;
                    }
                    else if (getSelectionSize())
                    {
                        for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
                        {
                            PickingShape* shape = EDITOR->getSelectionAt(iSel);
                            if (shape)
                            {
                                BaseObject* owner = shape->getOwnerPtr();
                                if (owner)
                                {
                                    if (m_joingroupList.find(owner->getRef()) < 0)
                                        m_joingroupList.push_back(owner->getRef());
                                }
                            }
                        }
                    }
                    return bfalse;
                }
            }
            break;

        case 'U':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    ungroupSelection(getFirstSelectedObjRef(), bfalse);
                    return bfalse;
                }
                else
                {
                    ObjectRefList selection = getEditorSelectionAsObjRefList();
                    for(u32 i = 0; i < selection.size(); ++i)
                    {
                        ungroupSelection(selection[i], btrue);
                    }
                    return bfalse;
                }
            }
            break;

        case 'K':
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LALT))
                {
                    CAMERACONTROLLERMANAGER->switchCheatDezoomMax();
                    return bfalse;
                }
                else
                {
                    CAMERACONTROLLERMANAGER->switchDebugDrawModifiers();
                    if ( CAMERACONTROLLERMANAGER->isDebugDrawInfo() != CAMERACONTROLLERMANAGER->isDebugDrawModifiers() )
                    {
                        CAMERACONTROLLERMANAGER->switchDebugDrawInfo();
                    }
                    return bfalse;
                }

                break;
            }
            
            
    #endif // !ITF_FINAL

        case KEY_F11:
            {
                if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    SYSTEM_ADAPTER->toggleShowTitleBar();
                    return bfalse;
                }
                else if(INPUT_ADAPTER->isKeyPressed(KEY_LALT))
                {
                    toggleWindowRatio();
                    return bfalse;
                }
                else
                {
                    SYSTEM_ADAPTER->toggleFullScreen();
                    return bfalse;
                }
            }
            break;

        case KEY_F12:
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    requestNormalScreenshot();
                    return bfalse;
                }
            }
            break;

#endif // ITF_WINDOWS

        case KEY_KP_0:
        {
            static u32 gridMode = m_gridMode;

            if (m_usePluginsInfoFlags)
            {
                gridMode = m_gridMode;
                m_gridMode = 0;

                PLUGINGATEWAY->pushInfoFlags(btrue);
                m_usePluginsInfoFlags = 0;
                return bfalse;
            }
            else
            {
                m_gridMode = gridMode;

                PLUGINGATEWAY->popInfoFlags();
                m_usePluginsInfoFlags = 0xffffffff;
                return bfalse;
            }
            break;
        }

        case KEY_KP_1: // Actors
            {
                toggleActorIconDisplay();
                return bfalse;
            }
            break;

        case KEY_KP_2: // Friezes
            {
                toggleFriezePivotDisplay();
                return bfalse;
            }
            break;

        case KEY_KP_3: // Grid
            if(INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                // Mode grid object
                m_gridMode = (m_gridMode == 2 ? 0 : 2);
                return bfalse;
            }
            else
            {
                // Mode grid scene (default)
                m_gridMode = (m_gridMode == 1 ? 0 : 1);
                return bfalse;
            }
            break;

        case KEY_KP_4:
            {
                String newFilter;
                static const String customFilter = "-ptools -ftodo";

                // If the procedural filter is not found in current filter field
                if (m_lastCtrlFString.strstr(customFilter.cStr()) == NULL)
                {
                    if(m_lastCtrlFString.getLen())
                        newFilter = m_lastCtrlFString + " ";

                    newFilter += customFilter;
                }
                else
                {
                    // Revert filter changes
                    newFilter = m_lastCtrlFString;
                    newFilter.replace(customFilter, "");
                }

                while(newFilter.strstr(String("  ").cStr()))
                    newFilter.replace("  ", " ");

                friendlyFinder(&newFilter);
                return bfalse;
            }
            break;

        case KEY_KP_5: // Friezes points
            {
                toggleFriezePointsDisplay();
                return bfalse;
            }
            break;

        case KEY_KP_6: // Friezes lines
            {
                toggleFriezeCollisionsDisplay();
                return bfalse;
            }
            break;

        case KEY_KP_7:    
            if (m_authorizeDeactivatedObjects)
            {
                m_authorizeDeactivatedObjects = bfalse;
                return bfalse;
            }
            else
            {
                m_authorizeDeactivatedObjects = btrue;
                return bfalse;
            }
            break;

        case KEY_KP_8:
            {
                switchDisplayedFamilies(Pickable::ObjectFamily_LevelDesign);
                return bfalse;
            }
            break;

        case KEY_KP_9:
            {
                switchDisplayedFamilies(Pickable::ObjectFamily_Art);
                return bfalse;
            }
            break;

        case KEY_KP_MULTIPLY: // Reset the grid
            {
                m_gridSize = m_defaultGridSize;
                m_gridPoint = Vec3d::Zero;
                return bfalse;
            }
            break;

        case KEY_KP_ADD: // Increase grid size
            {
                increaseGridSize();
                return bfalse;
            }
            break;

        case KEY_KP_SUBTRACT: // Decrease grid size
            {
                decreaseGridSize();
                return bfalse;
            }
            break;

        case EDITOR_CAMERA_KEY:
            {
                CAMERA->setMode(Camera::Camera_Editor);
                return bfalse;
            }
            break;

        case KEY_HOME:
            {
                camFocusOnSelection();
                return bfalse;
            }
            break;

        case KEY_DEL:
            {
                deleteSelection();
                return bfalse;
            }
            break;

        case 'M':
            {
                m_snapByDefault = !m_snapByDefault;

                if (DEBUGINFO)
                {
                    String text("Snap by default: ");
                    if(m_snapByDefault)
                        text += "ENABLED";
                    else
                        text += "DISABLED";

                    DEBUGINFO->addDebugInfoDisplay(text, 5.0f);
                }
                return bfalse;
            }
            break;

        case '6':
            {
                GFX_ADAPTER->startScreenCapture(); 
                return bfalse;
            }
            break;

        case '7':
            {
                GFX_ADAPTER->stopScreenCapture(); 
                return bfalse;
            }
            break;
        case '8':
            {
                GFX_ADAPTER->cancelScreenCapture(); 
                return bfalse;
            }
            break;
        case '9':
            {
                GFX_ADAPTER->switchSafeFrameMode();
                return bfalse;
            }
            break;

        case 'H':
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    unhideAll();
                    return bfalse;
                }
                else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    hideUnselected();
                    return bfalse;
                }
                else
                {
                    hideSelected();
                    return bfalse;
                }
            }
            break;
/*
        case 'N':
            {
                World* pWorld = CURRENTWORLD;
                if(pWorld)
                {
                    pWorld->lockCamera();
                    return bfalse;
                }
            }
            break;

        case 'B':
            {
                World* pWorld = CURRENTWORLD;
                if(pWorld)
                {
                    pWorld->assignGlobalCamera();
                    return bfalse;
                }
            }
            break;*/

        case 'P':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                requestUHDScreenshot();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LALT))
            {
                changeAutoPauseMode();
                return bfalse;
            }
            else
            {
                CHEATMANAGER->togglePause();
                m_pausedByUser = CHEATMANAGER->getPause();
                m_pauseCount = 0;
                return bfalse;
            }
            break;

        case 'O':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                GFX_ADAPTER->switchOverDrawMode();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                openScene(btrue);
                return bfalse;
            }
            else
            {
                if ( CHEATMANAGER->getPause() )
                {
                    CHEATMANAGER->setPauseStep();
                }
                return bfalse;
            }
            break;

        case 'D':
            {
#ifndef ITF_FINAL
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    GFX_ADAPTER->toggleDebugInfo();
                    return bfalse;
                }
                else
#endif //ITF_FINAL
                {
                    bbool debug = CHEATMANAGER->getDebugCharacters();
                    CHEATMANAGER->setDebugCharacters(!debug);
                    return bfalse;
                }
            }
            break;

        case 'Q':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                SYSTEM_ADAPTER->launchExe("UA_PropertiesEditor.exe", NULL, btrue, btrue);
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                CHEATMANAGER->DecrTimeStepMult();
                return bfalse;
            }
            break;

        case 'W':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                CHEATMANAGER->IncrTimeStepMult();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                GFX_ADAPTER->switchWireFrameMode();
                return bfalse;
            }
            break;

        case 'R':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    RASTER_CYCLERASTERGROUP();
                    return bfalse;
                }

#ifndef ITF_FINAL
                else
                {
                    GFX_ADAPTER->toggleRaster();
                    return bfalse;
                }
#endif //ITF_FINAL
            }
            {
                // Used to rotate selection
            }
            break;

        case 'J':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                RASTER_DUMP(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT));
            }
            break;

        case 'E':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
#if defined(ITF_SUPPORT_RAKI)
                SOUND_ADAPTER->toggleDebug();
#endif
                return bfalse;
            }
            break;

        case 'A':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                selectAll();
                //GFX_ADAPTER->toggleAfterFx();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                toggleSubSceneContentShapesDisplay();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LALT))
            {
                switchObjectFamilyAsPickableForFrameBuffer(Pickable::ObjectFamily_Alpha);
                return bfalse;
            }
            break;

        case 'Z':
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    UNDOREDO_MANAGER->requestUndo();
                    return bfalse;
                }
                else
                {
                    if(getState() == EditorState_EditMode)
                        setState(EditorState_EditMode_SelectDepthRange);
                    return bfalse;
                }
            }
            break;

        case 'Y':
            {
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
                {
                    UNDOREDO_MANAGER->requestRedo();
                    return bfalse;
                }
            }
            break;

        case 'S':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) || INPUT_ADAPTER->isKeyPressed(KEY_RCTRL))
            {
                PLUGINGATEWAY->onSceneSave();
                saveCurrentWorld(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT), bfalse);
                return bfalse;
            }     
            else if(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                PLUGINGATEWAY->onSceneSave();
                saveCurrentWorld(bfalse, btrue);
                return bfalse;
            }
            break;

        case 'F':
            if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            {
                friendlyFinder();
                return bfalse;
            }
            else if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            {
                const PickingShape* pPS = getPickingShapeOfObjectUnderMouse();
                if(pPS)
                {
                    String absPath;
                    if(getFullPath(pPS->getOwnerPtr(), absPath))
                        SystemAdapter::findInExplorer(absPath);
                }
                return bfalse;
            }
            break;

        case KEY_UP:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // Cam and seq editor also use this key
            {
                offsetSelection(Vec3d(0.0f, m_gridSize, 0.0f));
                return bfalse;
            }
            break;

        case KEY_DOWN:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // Cam and seq editor also use this key
            {
                offsetSelection(Vec3d(0.0f, -m_gridSize, 0.0f));
                return bfalse;
            }
            break;

        case KEY_LEFT:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // Cam and seq editor also use this key
            {
                offsetSelection(Vec3d(-m_gridSize, 0.0f, 0.0f));
                return bfalse;
            }
            break;

        case KEY_RIGHT:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // Cam and seq editor also use this key
            {
                offsetSelection(Vec3d(m_gridSize, 0.0f, 0.0f));
                return bfalse;
            }
            break;

        case KEY_PAGEUP:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed('L')) // Camera also use this key
            {
                offsetSelection(Vec3d(0.0f, 0.0f, -m_gridSize));
                return bfalse;
            }
            break;

        case KEY_PAGEDOWN:
            if(!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && !INPUT_ADAPTER->isKeyPressed('L')) // Camera also use this key
            {
                offsetSelection(Vec3d(0.0f, 0.0f, m_gridSize));
                return bfalse;
            }
            break;

        default:
            break;
        }
    }


    return btrue;
}

void Editor::teleportCheat()
{
    UndoRedoSequenceScope teleportSequence("Teleport players");

    Vec3d pos = getMouseOnCurrentWorkingDepth();
    bbool allScaled = btrue;

    if ( INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) )
    {
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if(Actor* pActor = GAMEMANAGER->getActivePlayer(i))
            {
                allScaled &= pActor->getWorldInitialScale().m_x != pActor->getScale().m_x;
            }
        }
    }

    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
    {
        if(Actor* pActor = GAMEMANAGER->getActivePlayer(i))
        {
            GAMEMANAGER->teleportPlayer(i, pos.truncateTo2D(),pos.m_z, btrue);

            if ( INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) )
            {
                if ( allScaled )
                {
                    pActor->setScale(Vec2d(pActor->getWorldInitialScale().m_x,pActor->getWorldInitialScale().m_x));
                }
                else
                {
                    pActor->setScale(Vec2d(pActor->getWorldInitialScale().m_x*0.5f,pActor->getWorldInitialScale().m_x*0.5f));
                }
            }

            PLUGINGATEWAY->onObjectChanged(pActor);
            pActor->onEditorMove(isAllowStorePickableInitData(pActor));
        }
    }
}

void Editor::resetPickingInfo(bbool _bUnselect/* = bfalse*/)
{
    if(_bUnselect)
        unselectAll();

    setUnderMouseRealPickingShape(NULL);
    setPickingShapeOfObjectUnderMouse(NULL);
}

void Editor::backToGame(bbool _bKeepEditorStuffs/* = bfalse*/)
{
    if(!_bKeepEditorStuffs)
    {
        clearMeasures();
        unselectAll();
        resetPickingInfo(btrue);
        EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_ShowDbg);
        m_gridMode = 0;
        resetForcedWorkingDepth();
        leaveEditMode(btrue, bfalse);
    }
    m_joingroupList.clear();
    m_joinGroupDest = NULL;
    setState(EditorState_Idle);
    PLUGINGATEWAY->setExclusiveFocus(NULL);
    if(CAMERA->getMode() != Camera::Camera_InGame)
    {
        CAMERA->setMode(Camera::Camera_InGame);
    }
    CAMERA->cancelBiasMode();
    CAMERACONTROLLERMANAGER->backToGame();
    setTopBarMenuShowable(bfalse);
    setContextMenuShowable(bfalse);
}

void Editor::goBackOneStep()
{
    m_joingroupList.clear();
    m_joinGroupDest = NULL;
    switch(m_state)
    {
    case EditorState_EditMode:
        leaveEditMode();
        break;

    case EditorState_Idle:
        backToGame();
        break;

    default:
        break;
    }
}

void Editor::enterEditMode()
{
    m_usePluginsInfoFlags = 0xffffffff;
//    PLUGINGATEWAY->popInfoFlags();
    setTopBarMenuShowable(btrue);
    m_topBarMenu.fold();

    if (PLUGINGATEWAY->bCanEnterEditMode() && !isPanActive() && WORLD_MANAGER->getCurrentWorld().isValid())
    {
        LOG_OLIV("[Editor] Entering edit mode");

        setState(EditorState_EditMode);

        refillPickableList();
        //updateFramebufferedPicking();
        fillPickingInfos();
    }

    if (!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && ActorEditor::getInstance())
    {
        if (!ActorEditor::getInstance()->hasActorInfoFlag(ActorEditor::ActorInfoFlag_Anchors))
            toggleActorIconDisplay();
        FriezeEditor::getInstance()->addShowInfoMode(FriezeEditor::ShowInfoMode_MainAnchorOnly);
        if (m_autoPause)
        {
            CHEATMANAGER->setPause(btrue);
            m_pausedByUser = btrue;
        }
    }
}

void Editor::leaveEditMode(bbool _bForce /*= bfalse*/, bbool keepEditorStuff /*= bfalse*/)
{
    PLUGINGATEWAY->cancelCurrentAction(keepEditorStuff);

    if (_bForce || PLUGINGATEWAY->bCanLeaveEditMode())
    {
        if(getEditedSubSceneHolder().isValid() && m_editedSubSceneHolderRef != getWorkingSubScene())
        {
            if(!keepEditorStuff)
                endSubSceneEdition();
        }
        else
        {
            LOG_OLIV("[Editor] Leave edit mode");

            resetPause();
            resetPickingInfo(bfalse);

            // PLUGINGATEWAY->pushInfoFlags(btrue);
            m_usePluginsInfoFlags = 0;

            PLUGINGATEWAY->setExclusiveFocus(NULL);
            setState(EditorState_Idle);
        }
    }
}

// return true if the event is handled by menus
bbool Editor::handleContextMenu()
{
    if (abs((i32)CURRENTFRAME - (i32)m_lastHandleContextMenuFrame) < 20)
        return bfalse;

    m_lastHandleContextMenuFrame = CURRENTFRAME;

    if (isTopBarMenuShowable())
    {
        ContextMenuItem* item = m_topBarMenu.onLeftMB(getMouse2d());
        if (item)
        {
            bbool result = handleContextMenuItemClicked(item, ITF_INVALID_OBJREF);
            if (item->m_subMenu.getItemsCount() == 0)
                m_topBarMenu.fold(); // fold only if selected item doesn't have children
            if (result)
                return btrue;
        }
        else m_topBarMenu.fold();
        if (!m_topBarMenu.isFolded())
            return btrue; // whatever happens, if top bar menu is visible, don't allow anything else to interpret clicks
    }

#ifndef ITF_FINAL
    if (isContextMenuShowable())
    {
        for (u32 i=0; i<2; i++)
        {
            ContextMenuItem* item = m_contextMenu[i].onLeftMB(getMouse2d());
            if (item)
            {
                if (item->m_subMenu.getItemsCount() == 0)
                    showContextMenu(bfalse); // hide only if selected item doesn't have children
                bbool result = handleContextMenuItemClicked(item, m_contextMenu[i].m_lastTarget);
                if (!result)
                    PLUGINGATEWAY->onContextMenuItemSelected(item, m_contextMenu[i].m_lastTarget); // the event was not handled by the main menu, try plugins
            }
        }
        showContextMenu(bfalse);
        return btrue; // whatever happens, if menu is visible, don't allow anything else to interpret clicks
    }
#endif
    return bfalse;
}

///////////////////////////////////////////////////////////////////////////////
bbool Editor::convert2dTo3dPositionOnPlane(const Vec2d &_2dPos, f32 _z, Vec3d &_3dpos)
{
    Vec3d pointWithZ(_2dPos.m_x, _2dPos.m_y, 0.1f);
    f32 dummy;
    Vec3d linePoint[2];


    GFX_ADAPTER->compute2DTo3D(pointWithZ, linePoint[0]);
    linePoint[1] = CAMERA->getCorrectedPos();

    Plane gridPlane(Vec3d(0,0,1), _z);
    Vec3d mouseOnGridPlane;
    return gridPlane.testLineIntersection(linePoint[0], linePoint[1]-linePoint[0], _3dpos, dummy);
}

///////////////////////////////////////////////////////////////////////////////
bbool Editor::snapToGrid_3d(Vec3d &_3dpos)
{
    if (m_gridMode == 0)
        return bfalse;

    Vec3d snapped3d;
    Vec3d projectedPos;
    Vec2d projectedPos_xy;

    GFX_ADAPTER->compute3DTo2D(_3dpos, projectedPos);
    if (projectedPos.m_z<0 || projectedPos.m_z>1.f)
        return bfalse;

    const f32 oldGridZ = m_gridPoint.m_z;
    projectedPos.truncateTo2D(projectedPos_xy);
    const bbool snapped = snapToGrid(projectedPos_xy, &snapped3d);
    m_gridPoint.m_z = oldGridZ;
    if (snapped)
        _3dpos = snapped3d;
    return snapped;
}

///////////////////////////////////////////////////////////////////////////////
bbool Editor::snapToGrid(Vec2d &_2dPos, Vec3d *_snapped3DPos)
{
    if (m_gridMode == 0)
        return bfalse;

    const f32 SnapDist = getSnappingDistanceInPixel();

    //Compute 3D distance which corresponds to a 2D distance of SnapDist
    //For this, we take a grid point which is in front of the viewer

    const f32 gridZ = m_gridPoint.m_z;
    bbool wasSnapped = bfalse;

    Vec3d mouseOnGridPlane;
    if (convert2dTo3dPositionOnPlane(_2dPos, gridZ, mouseOnGridPlane))
    {
        Vec3d nearGridPoint(mouseOnGridPlane-m_gridPoint);
        nearGridPoint.m_x = ((i32)(nearGridPoint.m_x/m_gridSize))*m_gridSize + m_gridPoint.m_x;
        nearGridPoint.m_y = ((i32)(nearGridPoint.m_y/m_gridSize))*m_gridSize + m_gridPoint.m_y;
        nearGridPoint.m_z = gridZ;
        Vec3d nearGridPoint2d;
        GFX_ADAPTER->compute3DTo2D(nearGridPoint, nearGridPoint2d);
        if (nearGridPoint2d.m_z<0 || nearGridPoint2d.m_z>1)
            return bfalse;
        //we now have a grid vertex near _2dPos
        
        //rotate around the 2d point to get typical 3D distance for snapping
        const i32 seedCount = 4;
        static Vec2d seedDeltas[seedCount]=
        {
            Vec2d(SnapDist, 0),
            Vec2d(-SnapDist, 0),
            Vec2d(0,SnapDist),
            Vec2d(0,-SnapDist)
        };

        f32 minDistPos3d = 1000;
        for (i32 seedIndex=0; seedIndex<seedCount; seedIndex++)
        {
            const Vec2d &seedDelta = seedDeltas[seedIndex];
            Vec3d seedPos2d(nearGridPoint2d.m_x+seedDelta.m_x,
                nearGridPoint2d.m_y+seedDelta.m_y,
                nearGridPoint2d.m_z);
            Vec3d seedPos3d;
            GFX_ADAPTER->compute2DTo3D(seedPos2d, seedPos3d);
            
            f32 distance = (seedPos3d-nearGridPoint).norm();
            if (distance<minDistPos3d)
                minDistPos3d = distance;
        }
        //look around this point, on grid, for horizontal grid lines
        for (i32 stepY=-1; stepY<=1; stepY++)
        {
            f32 y = nearGridPoint.m_y + stepY*m_gridSize;
            if (f32_Abs(y-mouseOnGridPlane.m_y)<minDistPos3d)
            {
                mouseOnGridPlane.m_y = y; //snap if near enough
                wasSnapped = btrue;
            }
        }
        //Same thing for vertical grid lines
        for (i32 stepX=-1; stepX<=1; stepX++)
        {
            f32 x = nearGridPoint.m_x + stepX*m_gridSize;
            if (f32_Abs(x-mouseOnGridPlane.m_x)<minDistPos3d)
            {
                mouseOnGridPlane.m_x = x;
                wasSnapped = btrue;
            }

        }
        //reproject in 2d
        Vec3d pos2d;
        GFX_ADAPTER->compute3DTo2D(mouseOnGridPlane, pos2d);
        if (wasSnapped && _snapped3DPos!=NULL)
            *_snapped3DPos = mouseOnGridPlane;
        _2dPos.m_x = pos2d.m_x;
        _2dPos.m_y = pos2d.m_y;
    }

    return wasSnapped;
}

///////////////////////////////////////////////////////////////////////////////
bbool Editor::snapToNearestShape( Vec3d &_posWorld, PickingShape* _positionOwner, bbool _ignoreSelection, PickingShape** _targetShape )
{
    bbool result = bfalse;
    const u32 count = m_activeShapesListOrdered.size();
    for(u32 i = 0; i < count; ++i)
    {
        const PickingShape* pShape = m_activeShapesListOrdered[i];
        if(!pShape)
            continue;
        
        if(pShape == _positionOwner || pShape->getShapeType() != PickingShape::ShapeType_Disc)
            continue;

        if(_positionOwner && _positionOwner->getOwner() == pShape->getOwner() && !_positionOwner->isSubAnchor() && !pShape->canSnapWithMainShape())
            continue;

        // Do not snap the shape with a shape present in the selection
        if(_ignoreSelection && getSelectionList().find(const_cast<PickingShape*>(pShape)) != -1)
            continue;

        // Do not snap a subscene/group with his content
        if(_positionOwner)
            if(SubSceneActor* pSSA = _positionOwner->getOwnerPtr()->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                if(Pickable* pPickable = static_cast<Pickable*>(pShape->getOwnerPtr()))
                    if(pPickable->getScene() == pSSA->getSubScene())
                        continue;

        const PickingShape_Disc* pShapeDisc = static_cast<const PickingShape_Disc*>(pShape);

        Vec3d pos2D, shapePos2D;
        GFX_ADAPTER->compute3DTo2D(_posWorld, pos2D);
        GFX_ADAPTER->compute3DTo2D(pShapeDisc->getPos(), shapePos2D);

        const f32 fDistScreen = (pos2D - shapePos2D).norm();

        if(fDistScreen < getSnappingDistanceInPixel())
        {
            shapePos2D.m_z = pos2D.m_z;
            GFX_ADAPTER->compute2DTo3D(shapePos2D, _posWorld);
            result = btrue;
            if(_targetShape)
                *_targetShape = (PickingShape*)pShapeDisc;
            break;
        }
    }
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////
bbool Editor::snap( Vec3d &_posWorld, PickingShape* _positionOwner, bbool _ignoreSelection/* = btrue*/ )
{
    bbool result = bfalse;
    
    if(m_snapByDefault != INPUT_ADAPTER->isKeyPressed(KEY_LALT))
    {
        result = snapOnMeasurePoint(_posWorld);

        if(!result)
        {
            result = snapToNearestShape(_posWorld, _positionOwner, _ignoreSelection, NULL);
        }

        if(!result && m_gridMode != 0)
            result = snapToGrid_3d(_posWorld);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void Editor::dispatchDrawForPickingShapes(PickingShapeVector *_orderedShapes)
{
    m_pSelectionObject->getShape()->draw();

    if (!_orderedShapes)
        _orderedShapes = &m_activeShapesListOrdered;

    PickingShapeVector::iterator it = _orderedShapes->begin();
    PickingShapeVector::iterator itEnd = _orderedShapes->end();
    while(it != itEnd)
    {
        (*it)->draw();
        ++it;
    }
}

Editor::EditorState Editor::getState() const
{
    return m_state;
}

i32 Editor::setState(EditorState _state)
{
    m_state = _state;
    return 1;
}

void Editor::getClipboardData()
{
#ifdef ITF_WINDOWS
    if (OpenClipboard(NULL))
    {
        u32 uLabelFormat = ITF::SYSTEM_ADAPTER->getClipboardFormat();

        if (IsClipboardFormatAvailable(uLabelFormat))
        {
            
            HGLOBAL   hglb = GetClipboardData(uLabelFormat); 
            if (hglb != NULL) 
            {
                using std::byte;
                byte* ptr;
                ptr = (byte*)GlobalLock(hglb); 
                if (ptr != NULL) 
                { 
                    //get a blob...
                    ITF::u32 blobsize = 0;
                    ITF_Memcpy(&blobsize,ptr,sizeof(blobsize));
                    ITF::Blob blob(ITF::BLOB_READONLY);
                    u8* pData = new u8[blobsize];
                    ITF_Memcpy(pData,ptr+sizeof(blobsize),blobsize);
                    blob.setData(pData,blobsize);

                    ITF::String poui;
                    i64 selected; 
                    blob.extractString(poui);

                    selected = blob.extractInt64();
                    GlobalUnlock(hglb); 
                } 
            } 

        }

        CloseClipboard();
    }
#endif //ITF_WINDOWS
}



bbool Editor::deletePickingShapesForObject(const ObjectRef _obj)
{
    ITF_MAP<ObjectRef, SafeArray<PickingShape*> >::iterator it = m_pickingShapes.find(_obj);
    if (it == m_pickingShapes.end())
        return bfalse;

    // Make sure the selection doesn't hold deleted shapes
    const Pickable* pObj = static_cast<const Pickable*>(GETOBJECT(_obj));
    if(pObj && pObj->isSelected())
    {
        cancelEditActions();

        unselectObject(_obj);
    }

    SafeArray<PickingShape*>& shapes = it->second;
    for (u32 i = 0; i < shapes.size(); i++)
    {
        PickingShape* pShape = shapes[i];
        PickingShapeVector::iterator it = std::find(m_activeShapesListOrdered.begin(), m_activeShapesListOrdered.end(), pShape);
        if(it != m_activeShapesListOrdered.end())
            m_activeShapesListOrdered.erase(it);

        if(getLastClickedShape() == pShape)
            setLastClickedShape(NULL);

        if(getPickingShapeOfObjectUnderMouse() == pShape)
            resetPickingInfo();

        SF_DEL(pShape);
    }
    m_pickingShapes.erase(it);
    return btrue;
}

bbool Editor::deletePickingShape(PickingShape* _shape)
{
    ITF_MAP<ObjectRef, SafeArray<PickingShape*> >::iterator it = m_pickingShapes.find(_shape->getOwner());
    if (it == m_pickingShapes.end())
        return bfalse;

    // Make sure the selection doesn't hold deleted shapes
    const i32 index = m_selection.find(_shape);
    if(index != -1)
    {
        cancelEditActions();

        unselectShape(_shape);
    }

    SafeArray<PickingShape*>& shapes = it->second;
    for (u32 i = 0; i < shapes.size(); i++)
    {
        PickingShape* pShape = shapes[i];

        if (_shape == pShape)
        {
            shapes.eraseKeepOrder(i);

            PickingShapeVector::iterator it = std::find(m_activeShapesListOrdered.begin(), m_activeShapesListOrdered.end(), pShape);
            if(it != m_activeShapesListOrdered.end())
                m_activeShapesListOrdered.erase(it);

            if(getLastClickedShape() == pShape)
                setLastClickedShape(NULL);

            if(getPickingShapeOfObjectUnderMouse() == pShape)
                resetPickingInfo();

            SF_DEL(pShape);
            return btrue;
        }

    }

    return bfalse;

}

const SafeArray<PickingShape*>* Editor::getPickingShapesForObject(const ObjectRef _obj)const
{
    ITF_MAP<ObjectRef, SafeArray<PickingShape*> >::const_iterator it = m_pickingShapes.find(_obj);
    if (it == m_pickingShapes.end())
        return NULL;
    return &it->second;
}

void Editor::addPickingShapeForObject(const ObjectRef _obj, PickingShape* _pick)
{
    _pick->setOwner(_obj);
    m_pickingShapes[_obj].push_back(_pick);
    _pick->allocatePluginData();
}

void convertOldScale(const Scene* pScene)
{
    // These must be done because :
    // Previously scaled group had no effect on their content position, now the position will be scale dependent
    // Also the actorBind offset become scale dependent, so this conversion is done to prevent positioning chaos over all maps...

    static const u32 newScaleVersion = 18557;
    if(pScene->getEngineVersionWhenSaved() < newScaleVersion)
    {
        bbool changes = bfalse;

        if(SubSceneActor* pParent = pScene->getSubSceneActorHolder())
        {
            const Vec2d& scale = pParent->getScale();

            const PickableList& content = pScene->getPickableList();
            for(u32 iContent = 0; iContent < content.size(); ++iContent)
            {
                Pickable* pObj = content[iContent];
                
                Vec3d localPos = pObj->getLocalInitialPos();
                localPos.setX(localPos.getX() / scale.m_x);
                localPos.setY(localPos.getY() / scale.m_y);

                pObj->setLocalInitialPos(localPos);
                pObj->setPos(pObj->getWorldInitialPos());

                changes = btrue;
            }
        }

        const PickableList& actors = pScene->getActors();
        for(u32 iAct = 0; iAct < actors.size(); ++iAct)
        {
            Actor* pActor = static_cast<Actor*>(actors[iAct]);

            if(ActorBind* pParentBind = pActor->getParentBind())
            {
                if(Actor* pParent = static_cast<Actor*>(pParentBind->m_runtimeParent.getObject()))
                {
                    const Vec2d& scale = pParent->getScale();

                    Vec3d posOffset = pParentBind->getPosOffset();
                    posOffset.setX(posOffset.getX() / scale.m_x);
                    posOffset.setY(posOffset.getY() / scale.m_y);
                    pParentBind->setPosOffset(posOffset);

                    Vec3d initPosOffset = pParentBind->getInitialPosOffset();
                    initPosOffset.setX(initPosOffset.getX() / scale.m_x);
                    initPosOffset.setY(initPosOffset.getY() / scale.m_y);
                    pParentBind->setInitialPositionOffset(initPosOffset);

                    pParent->getChildrenBindHandler().updateWorldCoordinates(pActor, pParentBind);

                    changes = btrue;
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
        }

        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(pScene->getSubSceneActors(), resolvedRefs);
        for(u32 iSSA = 0; iSSA < resolvedRefs.size(); ++iSSA)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[iSSA]);

            if(const Scene* pSubScene = pSSA->getSubScene())
                convertOldScale(pSubScene);
        }

        if(changes)
            EDITOR->setSceneAsModified(pScene);
    }
}

void checkOfficialSaveVersion(const Scene& _scene)
{
    ITF_WARNING(&_scene, _scene.getEngineVersionWhenSaved() != 0, "%ls : The scene was saved with a BETA build but should be saved with an official one", _scene.getPath().getString().cStr());

    SafeArray<BaseObject*> resolvedRefs;
    ID_SERVER->getObjectListNoNull(_scene.getSubSceneActors(), resolvedRefs);
    for(u32 iSSA = 0; iSSA < resolvedRefs.size(); ++iSSA)
    {
        SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[iSSA]);

        if(const Scene* pSubScene = pSSA->getSubScene())
            checkOfficialSaveVersion(*pSubScene);
    }
}

void Editor::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
{
    m_pickablesForPauseCellUpdate.clear();
    #if defined (ITF_WINDOWS) && !defined (ITF_FINAL)
        reportIssueUtils::m_investigateMapStuck = bfalse;
    #endif

    clearMeasures();
    m_activeShapesListOrdered.clear();
    m_pickableObjects.clear();

    endRotationScaleHelperMode();
    resetSceneModifiedList();
    setWorkingSubscene(NULL);
    if(getEditedSubSceneHolder().isValid())
        endSubSceneEdition();

    if(!_bSwitchDone)
    {
        unselectAll();
        resetPickingInfo();
    }
    else
    {
        if(_pWorld)
        {
            updateDebugInfo();

            const Scene* rootScene = _pWorld->getRootScene();

            setAppTitleScene(rootScene);

            const u32 currentEngineVersion = Versioning::getEngineVersion();
            if(currentEngineVersion > 0)
            {
                if(isOfficialScene(*rootScene))
                    checkOfficialSaveVersion(*rootScene);

                convertOldScale(rootScene);

                // Check for loaded scene saved with a more recent engine version
                // And warn user
                for(u32 i = 0; i < _pWorld->getSceneCount(); ++i)
                {
                    const Scene* pScene = _pWorld->getSceneAt(i);
                    
                    if(currentEngineVersion < pScene->getEngineVersionWhenSaved())
                    {
                        ITF_ERROR("Scene '%ls' was saved with engine version %u.\n\nThe engine may have an unknown behavior.\n\nSaving is not allowed for this scene.", pScene->getPath().getString().cStr(), pScene->getEngineVersionWhenSaved());
                    }
                }
            }
        }

        if(!WORLD_MANAGER->getAutoUnloadPreviousWorld())
        {
            // Add previous world
            const World* pCurrWorld = CURRENTWORLD;

            if(pCurrWorld &&
                std::find(m_tabList.begin(), m_tabList.end(), pCurrWorld->getUniqueName()) == m_tabList.end())
            {
                m_tabList.push_back(TabEditInfo());
                TabEditInfo& infos = m_tabList.back();
                infos.m_worldUniqueName = pCurrWorld->getUniqueName();
            }
        }
        else
        {
            // We are loading a map without tabs stuff
            // So unload all previous world still loaded

            ITF_VECTOR<String> worldNamesToDelete;
            for(u32 iTab = 0; iTab < m_tabList.size(); ++iTab)
            {
                TabEditInfo infos = m_tabList[iTab];

                worldNamesToDelete.push_back(infos.m_worldUniqueName);
                const u32 uSize = infos.m_undoRedoData.m_sequenceList.size();
                for(u32 uSeq = 0; uSeq < uSize; ++uSeq)
                {
                    SF_DEL(infos.m_undoRedoData.m_sequenceList[uSeq]);
                }
            }
            m_tabList.clear();

            for(u32 iWorld = 0; iWorld < worldNamesToDelete.size(); ++iWorld)
            {
                World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(worldNamesToDelete[iWorld]);
                ITF_ASSERT(pWorld);
                if(pWorld)
                {
                    pWorld->stopPrefetch();
                    ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));
                }
            }

#ifndef ITF_CONSOLE
            // If we are switching to a new world, load asynchronousely it's bank scenes
            if(_pWorld)
            {
                const Vector<Path>& tabPaths = _pWorld->getRootScene()->getAssociatedTabs();
                if(tabPaths.size())
                {
                    String info;
                    PathString_t pathName;

                    DEBUGINFO->addDebugInfoDisplay("Bank scenes:", 10.0f);
                    for(u32 iTab = 0; iTab < tabPaths.size(); ++iTab)
                    {
                        const Path& path = tabPaths[iTab];
                        if(loadTabInBackground(path, _pWorld))
                        {
                            info = " - ";
                            tabPaths[iTab].getString(pathName);
                            info += pathName;

                            DEBUGINFO->addDebugInfoDisplay(info, 10.0f);
                        }
                        else
                        {
                            ITF_ERROR("Missing bank scene: '%ls'", path.getString().cStr());
                        }
                    }
                }
            }
#endif // !ITF_CONSOLE
        }

        // Add new world
        if(_pWorld && 
            std::find(m_tabList.begin(), m_tabList.end(), _pWorld->getUniqueName()) == m_tabList.end())
        {
            m_tabList.push_back(TabEditInfo());
            TabEditInfo& infos = m_tabList.back();
            infos.m_worldUniqueName = _pWorld->getUniqueName();
        }

        if(_pWorld)
        {
            WORLD_MANAGER->setAutoUnloadPreviousWorld(btrue);

            // Check for pickable unique name at same level
            renameDuplicateUserFriendly(_pWorld);

            // Repopulate tab list with kept tabs
            while(m_keepTabList.size())
            {
                m_tabList.push_back(m_keepTabList.front());
                m_keepTabList.erase(m_keepTabList.begin());
            }

            Scene* rootScene = _pWorld->getRootScene();

            // Scan actors to detect template changes
            SafeArray<Scene*> sceneToScan;
            sceneToScan.push_back(rootScene);
            while(sceneToScan.size())
            {
                Scene* pScene = sceneToScan[0];
                sceneToScan.eraseKeepOrder(0);

                // Recursive feed the while monster
                ObjectRefList ssas = pScene->getSubSceneActors();
                for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                    if(BaseObject* pBO = ssas[iSSA].getObject())
                        if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            if(Scene* pSubScene = pSSA->getSubScene())
                                sceneToScan.push_back(pSubScene);

                const PickableList& actors = pScene->getActors();
                for(u32 iA = 0; iA < actors.size(); ++iA)
                {
                    Actor* pActor = static_cast<Actor*>(actors[iA]);
                    if(pActor->isTemplateChanged())
                    {
                        setSceneAsModified(pScene); // don't break;
                    }
                    else if ( !pActor->getTemplate() &&
                         TEMPLATEDATABASE->isTemplateMarkedForRemove(pActor->getLua()) )
                    {
                        pScene->unregisterPickable(pActor);
                        setSceneAsModified(pScene);
                    }

                    pActor->setTemplateChanged(bfalse);
                }
            }

#ifdef ITF_WINDOWS
            m_gridSize = std::max(0.01f, rootScene->getGridUnit());
#endif //ITF_WINDOWS
        }
    }
}

void Editor::onDeleteScene(const Scene* _pScene)
{
    m_pickablesForPauseCellUpdate.clear();
    resetPickingInfo();
}

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::dispatchUpdateForPickingShapes()
{    
    PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
    PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
    while(it != itEnd)
    {
        (*it)->update();
        ++it;
    }
}

bbool Editor::openScene(bbool _checkForUnsavedChanges)
{
    if(_checkForUnsavedChanges && !checkAndAskForUnsavedWork())
        return bfalse;

    String path;
    if(FILEMANAGER->fileChooser(bfalse, path, L"Scene (*.isc)\0*.isc\0Bank scene (*.bsc)\0*.bsc\0Template scene (*.tsc)\0*.tsc\0"))
    {
            String relativePath;
            if (!FILESERVER->getPathFromDataFolder(relativePath, path))
                return bfalse;

            const String extension = FilePath::getExtension(relativePath);
            if (extension == "isc" || extension == "isd" || extension == "tsc" || extension == "bsc")
            {
                setPendingDragAndDropMap(relativePath);
            }

        return btrue;
    }
    return bfalse;
}

void fillSubSceneMenuRecursive(ContextMenuItem* _parentMenu, Scene* _pScene)
{
    bbool needRootMenu = _pScene->getSubSceneActorHolder() == NULL;
    bbool needSeparator = !needRootMenu;

    const ObjectRefList& ssaList = _pScene->getSubSceneActors();
    for(u32 i = 0; i < ssaList.size(); ++i)
    {
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(ssaList[i].getObject()))
        {
            if(pSSA->getEmbedScene())
                continue;

            if(needRootMenu)
            {
                _parentMenu = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_SubScenesMenu, "SubScenes");
                needRootMenu = bfalse;
            }
            else 
            {
                if(needSeparator)
                {
                    _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_None, "", btrue);
                    needSeparator = bfalse;
                }
            }
            
            SafeArray<ContextMenuItem*> menuList;
            ContextMenuItem* sceneItem = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_SubScenesMenuScene, pSSA->getUserFriendly());

            menuList.push_back(sceneItem);
            
            menuList.push_back(sceneItem->m_subMenu.addItem(EditorContextMenu::ItemId_SubSceneGoto, "Goto"));

            if(EDITOR->allowEditSubScene(pSSA))
            {
                if(EDITOR->getWorkingSubScene() != pSSA->getRef())
                    menuList.push_back(sceneItem->m_subMenu.addItem(EditorContextMenu::ItemId_SubSceneLock, "Lock inside"));
                else if(EDITOR->getWorkingSubScene().isValid())
                    menuList.push_back(sceneItem->m_subMenu.addItem(EditorContextMenu::ItemId_SubSceneUnlock, "Unlock"));
            
                if(EDITOR->getEditedSubSceneHolder() != pSSA->getRef())
                {
                    bbool allowEnter = !EDITOR->getWorkingSubScene().isValid();

                    SubSceneActor* pCurrentAncestor = pSSA->getScene()->getSubSceneActorHolder();
                    while(!allowEnter && pCurrentAncestor)
                    {
                        if(EDITOR->getWorkingSubScene() == pCurrentAncestor->getRef())
                            allowEnter = btrue;

                        pCurrentAncestor = pCurrentAncestor->getScene()->getSubSceneActorHolder();
                    }

                    if(allowEnter)
                        menuList.push_back(sceneItem->m_subMenu.addItem(EditorContextMenu::ItemId_SubSceneOpen, "Enter"));
                }
            }

            for(u32 j = 0; j < menuList.size(); ++j)
                menuList[j]->m_userData = u64(pSSA->getRef().getValue());

            fillSubSceneMenuRecursive(sceneItem, pSSA->getSubScene());
        }
    }
}

void fillMusicThemes(ContextMenuItem* _parentMenu)
{
    Scene * pScene = EDITOR->getEditedScene();
    bbool _bFoundInactive = bfalse;

    // Add empty elem to be able to remove music theme on the scene
    ContextMenuItem* unsetItem = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_SetMusicTheme, "Unset");
    unsetItem->m_userData = u64(StringID::InvalidId);

    const ITF_MAP<StringID,u32>& musicMap = GAMEMANAGER->getMusicThemes();
    ITF_MAP<StringID,u32>::const_iterator it = musicMap.begin();
    for ( ; it != musicMap.end() ; ++it )
    {
        ContextMenuItem* sceneItem = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_SetMusicTheme, it->first.getDebugString());
        sceneItem->m_userData = u64(it->first.GetValue());

        if ( pScene != NULL && pScene->getMusicTheme() == it->first )
        {
            sceneItem->setInactiveColor();
            _bFoundInactive = btrue;
        }
    }

    // Set unset item to inactive if nothing as been set previously
    if ( !_bFoundInactive )
        unsetItem->setInactiveColor();
}

void fillMuteMusic(ContextMenuItem* _parentMenu)
{
    //Mute elem
    ContextMenuItem* muteItem = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_MuteMusic, "Mute");
    muteItem->m_userData = 1;

    //Unmute elem
    ContextMenuItem* unmuteItem = _parentMenu->m_subMenu.addItem(EditorContextMenu::ItemId_MuteMusic, "Unmute");
    unmuteItem->m_userData = 0;


    // Set unset item to inactive if nothing as been set previously
    if ( MUSICMANAGER->isMuted() )
        muteItem->setInactiveColor();
    else
        unmuteItem->setInactiveColor();
}

void Editor::updateTopBarMenu()
{
    if(m_menuWorlds)
    {
        m_menuWorlds->m_subMenu.clearItems();

        // New tab
        if(ContextMenuItem* pItem = m_menuWorlds->m_subMenu.addItem(EditorContextMenu::ItemId_TopBar_Scenes, "New..."))
            pItem->m_userData = 0;

        const u32 listSize = m_tabList.size();

        if(listSize > 1) 
        {
            // Close current tab
            if(ContextMenuItem* pItem = m_menuWorlds->m_subMenu.addItem(EditorContextMenu::ItemId_TopBar_Scenes, "Close current"))
            {
                pItem->m_userData = 1;
            }
            
            // Close other tabs
            if(ContextMenuItem* pItem = m_menuWorlds->m_subMenu.addItem(EditorContextMenu::ItemId_TopBar_Scenes, "Close all but current"))
            {
                pItem->m_userData = 2;
            }
        }
        
        if(listSize)
            m_menuWorlds->m_subMenu.addItem(EditorContextMenu::ItemId_None, "", btrue);

        for(u32 i = 0; i < listSize; ++i)
        {
            const String& worldName = m_tabList[i].m_worldUniqueName;

            if(ContextMenuItem* pItem = m_menuWorlds->m_subMenu.addItem(EditorContextMenu::ItemId_TopBar_Scenes, worldName.cStr()))
            {
                pItem->m_userData = u64(StringID(worldName).GetValue());

                World* pCurrentWorld = CURRENTWORLD;
                if(pCurrentWorld && pCurrentWorld->getUniqueName() == worldName)
                    pItem->setInactiveColor();
            }
        }
    }

    if (!isTopBarMenuShowable())
        return;

    if (
        (m_topBarMenu.getItemsCount() == 0) ||
        (m_topBarMenu.getItemAtPos(getMouse2d()) == m_topBarMenu.getItemAt(0))
        )
    {
        m_topBarMenu.clearItems();
        // TOOLS
        ContextMenuItem* item  = m_topBarMenu.addItem(EditorContextMenu::ItemId_TopBar_Tools, "Tools");
        EditorContextMenu& menu = item->m_subMenu;

        // FILE
        {
            menu.addSubItem(EditorContextMenu::ItemId_NewScene, "New Scene", EditorContextMenu::ItemId_TopBar_File, "File");
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_Load, "Open Scene (ctrl+o)", EditorContextMenu::ItemId_TopBar_File, "File");
            ContextMenuItem* it = menu.getItem(EditorContextMenu::ItemId_TopBar_File);
            if (it) it->m_subMenu.addItem(EditorContextMenu::ItemId_None, "", btrue);
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_Save, "Save Scene (ctrl+s)", EditorContextMenu::ItemId_TopBar_File, "File");
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_SaveAs, "Save Scene As (ctrl+shift+s)", EditorContextMenu::ItemId_TopBar_File, "File");
            if (it) it->m_subMenu.addItem(EditorContextMenu::ItemId_None, "", btrue);
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_ImportScene, "Import Scene", EditorContextMenu::ItemId_TopBar_File, "File");
#ifdef ITF_WINDOWS
#if !defined(ITF_FINAL)
            if (getSelectionSize())
            {
                menu.addSubItem(EditorContextMenu::ItemId_ExportSelectionToScene, "Export selection to Scene", EditorContextMenu::ItemId_TopBar_File, "File");
                menu.addSubItem(EditorContextMenu::ItemId_ExportSelectionToGroup, "Export selection to Group", EditorContextMenu::ItemId_TopBar_File, "File");
            }
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_NewFromTemplate, "New from template", EditorContextMenu::ItemId_TopBar_File, "File");

#endif // !ITF_FINAL
#endif // ITF_WINDOWS
        }

        // EDIT
        {
#ifdef ITF_WINDOWS
    #if !defined(ITF_FINAL)
            String reason;
            ObjectRefList reflist = getEditorSelectionAsObjRefList();
            if(reflist.size() && canGroupSelection(reason, reflist, getFirstObjectSelectedScene()))
                menu.addSubItem(EditorContextMenu::ItemId_GroupSelection, "Group", EditorContextMenu::ItemId_TopBar_Edit, "Edit");
            if(canUngroupSelection(reason, getFirstSelectedObjRef()))
                menu.addSubItem(EditorContextMenu::ItemId_UngroupSelection, "Ungroup", EditorContextMenu::ItemId_TopBar_Edit, "Edit");
    #endif // !ITF_FINAL
#endif // ITF_WINDOWS
        }
            
        if(World* currentWorld = CURRENTWORLD)
            fillSubSceneMenuRecursive(item, currentWorld->getRootScene());

        // DISPLAY
        {
            menu.addSubItem(EditorContextMenu::ItemId_ToggleHighlightOnMouseOver, "Toggle highlight on mouse over", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_FullScreen, "Toggle fullscreen (F11)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_FriezeMeshToggleDisplay, "Toggle frieze mesh", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_ShowPickables, "Show pickables (F3)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_ToggleActorIconDisplay, "Toggle actors icon (NUMPAD_1)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_FriezePivotToggleDisplay, "Toggle frieze's pivot (NUMPAD_2)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_ShowGrid_SceneDepth, "Toggle scene grid (NUMPAD_3)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_ShowGrid_ObjectDepth, "Toggle object grid (CTRL + NUMPAD_3)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_FriezePointsToggleDisplay, "Toggle frieze's points (NUMPAD_5)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_FriezeCollisionsToggleDisplay, "Toggle frieze's collisions (NUMPAD_6)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            menu.addSubItem(EditorContextMenu::ItemId_SubSceneContentShapesToggleDisplay, "Toggle subscene/group content's icon (SHIFT + A)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            
            menu.addSubItem(EditorContextMenu::ItemId_2DItems, "Toggle 2D items (F6)", EditorContextMenu::ItemId_TopBar_Display, "Display");
            if (getSelectionSize())
            {
                menu.addSubItem(EditorContextMenu::ItemId_HideSelected, "Hide selected", EditorContextMenu::ItemId_TopBar_Display, "Display");
                menu.addSubItem(EditorContextMenu::ItemId_HideUnSelected, "Hide Unselected", EditorContextMenu::ItemId_TopBar_Display, "Display");
            }

            menu.addSubItem(EditorContextMenu::ItemId_ShowRewards, "Show rewards", EditorContextMenu::ItemId_TopBar_Display, "Display");

            //Check if hidden object
            bbool hasHiddenObjects=bfalse;
            for (u32 index=0; index<m_hiddenObjectList.size(); index++)
            {
                Pickable *pickable = (Pickable*)GETOBJECT(m_hiddenObjectList[index]);
                if (pickable && pickable->getHiddenForEditorFlag())
                {
                    hasHiddenObjects = btrue;
                    break;
                }
            }
            if (hasHiddenObjects)
                menu.addSubItem(EditorContextMenu::ItemId_UnHideAll, "Unhide all", EditorContextMenu::ItemId_TopBar_Display, "Display");

        }

        // OPTIONS
        {
            menu.addSubItem(EditorContextMenu::ItemId_ChooseGridUnit, "Edit grid unit", EditorContextMenu::ItemId_TopBar_Options, "Options");
            menu.addSubItem(EditorContextMenu::ItemId_ChooseAngleStep, "Edit angle step", EditorContextMenu::ItemId_TopBar_Options, "Options");

            // MUSIC
            ContextMenuItem* it = menu.getItem(EditorContextMenu::ItemId_TopBar_Options);
            if (it) it->m_subMenu.addItem(EditorContextMenu::ItemId_None, "", btrue);
            ContextMenuItem* subItem = menu.addSubItem(EditorContextMenu::ItemId_SetMusicTheme, "Choose music theme", EditorContextMenu::ItemId_TopBar_Options, "Options");
            fillMusicThemes(subItem);
            subItem = menu.addSubItem(EditorContextMenu::ItemId_MuteMusic, "Music", EditorContextMenu::ItemId_TopBar_Options, "Options");
            fillMuteMusic(subItem);
        }

        // TOOLS
        {
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_Katana_Cut, "Friezes Cutter", EditorContextMenu::ItemId_TopBar_Utils, "Utils");
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_FriezeMultiCol, "Friezes Collisions", EditorContextMenu::ItemId_TopBar_Utils, "Utils");
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_Select2DActors, "Switch 2D Actors Selection", EditorContextMenu::ItemId_TopBar_Utils, "Utils");
            if (m_autoPause)
                menu.addSubItem(EditorContextMenu::ItemId_TopBar_AutoPause, "Switch Auto-Pause off (alt+p)", EditorContextMenu::ItemId_TopBar_Utils, "Utils");
            else
                menu.addSubItem(EditorContextMenu::ItemId_TopBar_AutoPause, "Switch Auto-Pause on (alt+p)", EditorContextMenu::ItemId_TopBar_Utils, "Utils");

            if (isObjectFamilyPickableForFrameBuffer(Pickable::ObjectFamily_Alpha))
                menu.addSubItem(EditorContextMenu::ItemId_TopBar_PickAlphaObjects, "Don't pick alpha objects (alt+a)", EditorContextMenu::ItemId_TopBar_Utils, "Utils");
            else
                menu.addSubItem(EditorContextMenu::ItemId_TopBar_PickAlphaObjects, "Pick alpha objects (alt+a)", EditorContextMenu::ItemId_TopBar_Utils, "Utils");

			
			menu.addSubItem(EditorContextMenu::ItemId_OT_LaunchTrackingTool,"Launch Tracking Tool",EditorContextMenu::ItemId_TopBar_Utils, "Utils");

			menu.addSubItem(EditorContextMenu::ItemId_OT_ClearTrackingData,"Clear Tracking Data",EditorContextMenu::ItemId_TopBar_Utils, "Utils");

			menu.addSubItem(EditorContextMenu::ItemId_OT_SendLevelName,"Send Level Name",EditorContextMenu::ItemId_TopBar_Utils, "Utils");
			




        }

        menu.addItem(EditorContextMenu::ItemId_None,"", btrue);
        menu.addItem(EditorContextMenu::ItemId_TopBar_Play, "Play (esc)");
        if (CHEATMANAGER->getPause())
            menu.addItem(EditorContextMenu::ItemId_TopBar_Pause, "Unpause (p)");
        else
            menu.addItem(EditorContextMenu::ItemId_TopBar_Pause, "Pause (p)");
        menu.addItem(EditorContextMenu::ItemId_TopBar_Reinit, "Load last checkpoint (F5)");
        menu.addItem(EditorContextMenu::ItemId_TopBar_Reload, "Reload (ctrl+F5)");
        menu.addItem(EditorContextMenu::ItemId_None,"", btrue);
        menu.addItem(EditorContextMenu::ItemId_TopBar_Undo, "Undo (ctrl+z)");
        menu.addItem(EditorContextMenu::ItemId_TopBar_Redo, "Redo (ctrl+y)");


		

#ifndef ITF_FINAL
        if (!WarningManager::isEmpty())
        {
            menu.addItem(EditorContextMenu::ItemId_TopBar_ClearWarnings, "Clear warnings");
        }
#endif
        menu.addSubItem(EditorContextMenu::ItemId_TopBar_GotoWorldOrigin, "Goto origin", EditorContextMenu::ItemId_TopBar_WorldOrigin, "World Origin");
        if (m_showWorldOrigin)
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_HideWorldOrigin, "Hide world origin", EditorContextMenu::ItemId_TopBar_WorldOrigin, "World Origin");
        else
            menu.addSubItem(EditorContextMenu::ItemId_TopBar_ShowWorldOrigin, "Show world origin", EditorContextMenu::ItemId_TopBar_WorldOrigin, "World Origin");
        menu.addSubItem(EditorContextMenu::ItemId_TopBar_CenterOnWorldOrigin, "Center on origin", EditorContextMenu::ItemId_TopBar_WorldOrigin, "World Origin");

        m_menuWorlds = m_topBarMenu.addItem(EditorContextMenu::ItemId_TopBar_ScenesRoot, "Tabs");

        if (ActorEditor::getInstance())
        {
            ActorEditor::getInstance()->onFillMainMenu(menu);
        }
        menu.addItem(EditorContextMenu::ItemId_SceneExplorer, "Scene Explorer");
        menu.addItem(EditorContextMenu::ItemId_DANSceneExplorer, "DAN Scene Explorer");
    }
    m_topBarMenu.draw(getMouse2d());
}

void Editor::reloadCurrentMap()
{
    m_pickablesForPauseCellUpdate.clear();
    if(GAMEMANAGER->isPlayableScreen())
    {
        if(checkAndAskForUnsavedWork())
        {
            showContextMenu(bfalse);
            leaveEditMode(btrue);
            backToGame();

            GAMEMANAGER->emptyCurrentLevelGameState();

            // Build the list of tab to be kept while reloading the current map
            m_keepTabList.clear();
            World* pCurrentWorld = CURRENTWORLD;
            Vector<TabEditInfo> tabListCopy = m_tabList;
            for(u32 uTab = 0; uTab < tabListCopy.size(); ++uTab)
            {
                TabEditInfo tabInfo = tabListCopy[uTab];
                if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(tabInfo.m_worldUniqueName))
                {
                    if(pWorld != pCurrentWorld && tabInfo.m_owner != pCurrentWorld->getRef())
                    {
                        m_keepTabList.push_back(tabInfo);

                        Vector<TabEditInfo>::iterator it = std::find(m_tabList.begin(), m_tabList.end(), tabInfo.m_worldUniqueName);
                        if(it != m_tabList.end())
                            m_tabList.erase(it);
                        else
                        {
                            ITF_ASSERT(0);
                        }
                    }
                }
            }

            // Stop all music
            MUSICMANAGER->stopAll(0.0f);

            ITF_VERIFY(GAMEMANAGER->loadGameplayMap(pCurrentWorld->getRootScene()->getPath(), btrue, btrue ));
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Editor::startFrame()
{
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
    if(applicationHasFocus())
        updateFramebufferedPicking();
#endif // ITF_WINDOWS && !ITF_FINAL


    //CAUTION : it is important to update frame buffer picking BEFORE updateMouse3D
    updateMouse3d(getMouse2d());
}

//////////////////////////////////////////////////////////////////////////
void Editor::update(double _elapsed)
{
    processPickableToRefresh();
    processJoinGroupList();
    processRegisterInWorldCellsDuringPause();

    if (m_cancelClickTime > 0.f)
        m_cancelClickTime -= (f32)_elapsed;

    PLUGINGATEWAY->forwardMouseEvents(!isContextMenuVisible());

    if (m_nextMenu.m_active)
    {
        m_nextMenu.m_active = bfalse;
        if (m_nextMenu.m_show && m_nextMenu.m_reFillContent)
        {
            setContextMenuShowable(bfalse);
            showContextMenu(btrue, m_nextMenu.m_forceTarget, m_nextMenu.m_keepPos);
        }
        else 
            setContextMenuShowable(m_nextMenu.m_show);
    }

    m_frameDeltaMouse2d = getMouse2d() - m_previousMouse2d;

    m_activeShapesListOrdered.clear();
    m_pickableObjects.clear();
    
    //  back to camera in game with joyButton
    for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
    {
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buttons, JOY_MAX_BUT);
        
        if ( buttons[m_joyButton_ThumbRight] == InputAdapter::JustPressed && (!buttons[m_joyButton_LB]==InputAdapter::Pressed || !buttons[m_joyButton_RB]==InputAdapter::Pressed))
        {
            showContextMenu(bfalse);
            leaveEditMode(btrue);
            backToGame();
            break;
        }
    }

    updateTopBarMenu();

    // update camera.
    CAMERA->apply();

    if (m_SaveIconTimer > 0.f)
    {
        GFX_ADAPTER->drawDBGCircle(CAMERA->getX(), CAMERA->getY(), 1.f + 0.5f * cosf(ELAPSEDTIME * 16.f), 0.5f + 0.2f * cosf(ELAPSEDTIME * 4.f) , 0.5f + 0.2f * cosf(ELAPSEDTIME * 2.f), 0.5f + 0.2f * cosf(ELAPSEDTIME * 8.f), 0.1f);
        showMouseIcon(Editor_IconStock::Icon_Save);
        m_SaveIconTimer -= LOGICDT;
    }
    
    if (m_InitialInfoTimer > 0.f)
    {
        showMouseIcon(Editor_IconStock::Icon_Ok);
        m_InitialInfoTimer -= LOGICDT;
    }

    if(getState() == EditorState_EditMode_SelectDepthRange && !INPUT_ADAPTER->isKeyPressed('Z'))
    {
        setState(EditorState_EditMode);
    }

    if(/*getSubEditedItem() == ITF_INVALID_OBJREF && */!isPanActive())
    {
        handleSelectionOffset();
        handleSelectionDuplication();
        handleSelectionRotation();
    }

    m_previousMouse2d = getMouse2d();

    // handle events
    update_handleTranslation(_elapsed);
    update_handleMouseDragged();
    update_handleJoystick(_elapsed);

    switch(getState())
    {
    case EditorState_EditMode_Measure:
    case EditorState_EditMode_SelectDepthRange:
        fillPickingInfos();
            // break; INTENTIONNAL!

    case EditorState_EditMode:
        {
            fillPickingInfos();
            if (!isContextMenuVisible())
                dispatchMouseMoveForPickingShapes();
        }
        break;

    default:
        break;
    }

    dispatchUpdateForPickingShapes();

    m_pSelectionObject->update((f32)_elapsed);

    //update grid position
    if (getSelectionSize() > 0)
    {
        const PickingShape* pPickingShape = getSelectionAt(0);
        if (const Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr()))
        {
            if(m_gridMode == 1)
            {
                m_gridPoint.m_z = getWorkingDepth().getReferenceDepth();
            }
            else
            {
                m_gridPoint.m_z = pPickable->getDepth();
            }
        }
        else
        {
            ITF_ASSERT_MSG(pPickable, "The first shape of the selection has a NULL owner");
        }
    }

    // The pickable may have been deleted, update selection (ex:in subedit, removing a point can lead to remove the whole poly)
    for(i32 i = 0; i < i32(getSelectionSize()); ++i)
    {
        const PickingShape* pPickingShape = getSelectionAt(i);

        Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());

        if (!pPickable)
        {
            LOG_OLIV("[Editor] A pickable in selection was deleted, so : force update of selection");
            m_selection.eraseNoOrder(i--);
            continue;
        }

        if(!pPickable->isSelected())
        {
            LOG_OLIV("[Editor] A selected item have been brutaly unselect by a plugin ?... so remove it from selection.");
            m_selection.eraseNoOrder(i--);
            continue;
        }
    }

    if (isContextMenuShowable())
    {
/*        m_contextMenu[0].setPos(m_contextMenu[0].m_launchPos);
        m_contextMenu[1].setPos(m_contextMenu[1].m_launchPos); remain at the same position if moved*/

        AABB aabb[2];
        const f32 pixBetweenMenus = 20.f;

        if (m_contextMenu[1].getItemsCount())
        {
            m_contextMenu[1].getAABB(aabb[1]);
            m_contextMenu[1].setPos(m_contextMenu[0].getPos() - Vec2d(aabb[1].getWidth() + pixBetweenMenus, 0.f));
        }

        m_contextMenu[0].getAABB(aabb[0]);
        if (m_contextMenu[1].getItemsCount())
            m_contextMenu[1].getAABB(aabb[1]);
        f32 screenW = (f32)GFX_ADAPTER->getScreenWidth();
        f32 screenH = (f32)GFX_ADAPTER->getScreenHeight();

        if (aabb[0].getMax().m_x > screenW)
        {
            m_contextMenu[0].setPos(Vec2d(screenW - aabb[0].getWidth(), m_contextMenu[0].getPos().m_y));
        }
        else if (aabb[1].getMin().m_x < 0.f && m_contextMenu[1].getItemsCount())
        {
            m_contextMenu[0].setPos(Vec2d(aabb[1].getWidth() + pixBetweenMenus, m_contextMenu[0].getPos().m_y));
        }
        if (m_contextMenu[1].getItemsCount())
            aabb[0].grow(aabb[1]);
        else
            aabb[1] = aabb[0];

        if (aabb[0].getMax().m_y > screenH)
        {
            m_contextMenu[0].setPos(Vec2d(m_contextMenu[0].getPos().m_x, screenH - aabb[0].getHeight()));
        }
        else if (aabb[0].getMin().m_y < 0.f)
        {
            m_contextMenu[0].setPos(Vec2d(m_contextMenu[0].getPos().m_x, 0.f));
        }


        m_contextMenu[1].setPos(m_contextMenu[0].getPos() - Vec2d(m_contextMenu[1].getWidth() + pixBetweenMenus, 0.f));

      

        
        m_contextMenu[0].getAABB(aabb[0]);

        if (m_contextMenu[1].getItemsCount())
        {
            m_contextMenu[1].getAABB(aabb[1]);
            aabb[0].grow(aabb[1]);
        }
        else
            aabb[1] = aabb[0];

        String name("no selection"), path;
        BaseObject* target = GETOBJECT(m_contextMenu[0].m_lastTarget);
        if (target)
        {
            switch (target->getObjectType())
            {
            case BaseObject::eFrise:
            case BaseObject::eActor:
                path = ((Pickable*)target)->getUserFriendly();
                break;
            case BaseObject::eScene:
                 ((Scene*)target)->getPath().getString(path);
                break;
            }


            switch (target->getObjectType())
            {
            case BaseObject::eFrise:
                name = "Frieze ";
                break;
            case BaseObject::eActor:
                if(target->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    name = "SubScene ";
                else
                    name = "Actor ";
                break;
            case BaseObject::eScene:
                name = "Scene ";
                break;
            default:
                name = "unknown";
                break;
            }

            const f32 interstice = 2.f;
            const f32 barWidth = 15.f;

            i32 posDot = path.rfind('.');
            if (posDot != String::npos)
                path = FilePath::getFilenameWithoutExtension(path);
            name += path;
            f32 startX = aabb[1].getMin().m_x + aabb[1].getWidth() - name.getLen() * fCharMaxWidth_Big * 0.5f;
            f32 startY = aabb[0].getMin().m_y - fCharMaxHeight_Big;
            const u32 uColor      = ITFCOLOR_TO_U32(200, 20, 20, 100);
    //            const u32 uDarkColor  = ITFCOLOR_TO_U32(200, 50, 50, 50);
            Vec2d boxStart(aabb[0].getMin() + Vec2d(0.f, -fCharMaxHeight_Big*1.5f));
            if (!m_contextMenu[1].getItemsCount())
            {
                startX = aabb[0].getMin().m_x;
            }
            if (startX < 0.f) startX = 0.f;
            if (boxStart.m_x > startX)
                boxStart.m_x = startX;  // if the label text is larger than the menus texts
            if (boxStart.m_x < 0.f) boxStart.m_x = 0.f;
            f32 boxWidth = aabb[0].getMax().m_x - boxStart.m_x;
            f32 textWidth = name.getLen() * fCharMaxWidth_Big;
            if (boxWidth < textWidth)
                boxWidth = textWidth;
            {   // draw background rect
                aabb[0].grow(boxStart);
                aabb[0].grow(boxStart + Vec2d(boxWidth, 0.f));
                f32 backval = cosf((f32)CURRENTFRAME * 0.05f) * 10.f;
                u32 ubackkColor = ITFCOLOR_TO_U32(100, 10+(u32)backval, 10+(u32)backval, 10+(u32)backval);
                GFX_ADAPTER->draw2dBox(aabb[0].getMin(), aabb[0].getWidth(), aabb[0].getHeight(), ubackkColor, ubackkColor, ubackkColor, ubackkColor);
            }
/*
            Vec2d drawMenu1Pos = m_contextMenu[1].m_pos;
            if (drawMenu1Pos.m_x > aabb[0].getMin().m_x && aabb[0].getMin().m_x >= 0.f)
                drawMenu1Pos.m_x = aabb[0].getMin().m_x;

            m_contextMenu[0].draw(getMouse2d(), NULL, NULL);
            m_contextMenu[1].draw(getMouse2d(), NULL, &drawMenu1Pos);
*/
            m_contextMenu[0].draw(getMouse2d(), NULL, NULL);
            m_contextMenu[1].draw(getMouse2d(), NULL, NULL);


            GFX_ADAPTER->draw2dBox(boxStart, boxWidth, fCharMaxHeight_Big*1.5f, uColor, uColor, uColor, uColor);
            f32 barMidX = (m_contextMenu[0].m_pos.m_x + aabb[1].getMax().m_x )*0.5f;
            if (m_contextMenu[1].getItemsCount())
                GFX_ADAPTER->draw2dBox(Vec2d(barMidX-barWidth*0.5f, aabb[1].getMin().m_y), barWidth, aabb[0].getHeight()-fCharMaxHeight_Big*1.5f, uColor, uColor, uColor, uColor);
            GFX_ADAPTER->drawDBGText(name, startX + fTextSubOffset, startY - interstice, 1.f, 1.f, 1.f, btrue);
        }
        else 
        {
            {   // draw background rect
                f32 backval = cosf((f32)CURRENTFRAME * 0.05f) * 10.f;
                u32 ubackkColor = ITFCOLOR_TO_U32(100, 10+(u32)backval, 10+(u32)backval, 10+(u32)backval);
                GFX_ADAPTER->draw2dBox(aabb[0].getMin(), aabb[0].getWidth(), aabb[0].getHeight(), ubackkColor, ubackkColor, ubackkColor, ubackkColor);
            }
            for (u32 i=0; i<2; i++)
                m_contextMenu[i].draw(getMouse2d());
        }

    }

    // Change mouse cursor according to current state or request
    CursorIcon::Type icon = CursorIcon::Count;
    if(isRotationActive())
        icon = CursorIcon::Rotate;
    else if(isDragActive())
        icon = CursorIcon::Move;
    else if(m_feedbackRequestFrame >= CURRENTFRAME - 1)
    {
        icon = m_feedbackRequest;
        m_feedbackRequest = CursorIcon::Count;
    }

    if(icon != CursorIcon::Count)
        SYSTEM_ADAPTER->setCursorIcon(icon);
}

void Editor::showMouseIcon   (Editor_IconStock::EditorIconID _id, f32 _angle, const Vec2d* _forcedPos/* = NULL*/)
{
    const Vec2d* pos = _forcedPos;
    if(!pos)
        pos = &getMouse2d();

    m_iconStock->showMouseIcon(_id, *pos, _angle);
}

void Editor::drawObj(BaseObject* pObj, bbool bMouseIsOver)
{
    ITF_ASSERT(pObj);

    if(pObj)
    {
        switch(pObj->getObjectType())
        {
        case BaseObject::eFrise:
            EditorDraw::drawFrise(static_cast<const Frise *>(pObj), FriezeEditor_LineSize, bMouseIsOver);
            break;

        case BaseObject::eActor:
            EditorDraw::drawActor(static_cast<Actor *>(pObj), bMouseIsOver);
            break;

        case BaseObject::eTrajectory:
            break;

        case BaseObject::eSelection:
            break;

        case BaseObject::eEditorDummy:
            break;

        default:
            ITF_ASSERT(0); // todo
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::displayObjectEditingInfos(ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Pickable *_underMouse)
{
    if (isContextMenuShowable())
        return; // avoid context menu display to be overriden on screen by debug info

    SafeArray<BaseObject*> pObjects;
    ID_SERVER->getObjectListNoNull(_objects, pObjects);

    for (u32 index=0; index<pObjects.size(); index++)
    {
        BaseObject *pObject = pObjects[index];
            drawObj(pObject, (pObject == _underMouse));
    }

    dispatchDrawForPickingShapes(&_orderedShapes);
}

void applyRenderColor(const Color& _color, Pickable* _obj)
{
    switch(_obj->getObjectType())
    {
    case BaseObject::eFrise:
        {
            Frise* pFrise = static_cast<Frise*>(_obj);
            pFrise->setGlobalColorToMesh(_color);
        }
        break;

    case BaseObject::eActor:
        {
            Actor* pActor = static_cast<Actor*>(_obj);
            if(AnimLightComponent* pALC = pActor->GetComponent<AnimLightComponent>())
                if(AnimMeshScene* pMeshScene = pALC->getAnimMeshScene())
                {
                    pMeshScene->m_AnimInfo.m_color = Color::zero();
                    pMeshScene->m_AnimInfo.m_selfIllumColor = _color;
                }
        }
        break;

    default:
        break;
    }
}

void Editor::preDraw()
{
    BaseObject* pUnderMouseObj = NULL;

    if(EditorDraw::getHighlightOnMouseOver())
    {
        if(PickingShape* pShapeUnderMouse = getUnderMouseRealPickingShape())
            pUnderMouseObj = pShapeUnderMouse->getOwnerPtr();

        if(!pUnderMouseObj)
            if(CURRENTFRAME == getLastHoveredActorOrFrise_Frame())
                pUnderMouseObj = getLastHoveredActorOrFriseObjRef().getObject();

        if(pUnderMouseObj)
        {
            const f32 var = ((cosf(ELAPSEDTIME * 5.f) + 1.f) * 0.25f);
            Color highlightColor = Color::white();
            highlightColor.m_r = highlightColor.m_g = highlightColor.m_b = 1.0f - var;
            
            applyRenderColor(highlightColor, static_cast<Pickable*>(pUnderMouseObj));

            if(SubSceneActor* pSSA = pUnderMouseObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                // Scan actors to detect template changes
                SafeArray<Scene*> sceneToScan;
                sceneToScan.push_back(pSSA->getSubScene());
                while(sceneToScan.size())
                {
                    Scene* pScene = sceneToScan[0];
                    sceneToScan.eraseKeepOrder(0);

                    if(pScene)
                    {
                        // Recursive feed the while monster
                        ObjectRefList ssas = pScene->getSubSceneActors();
                        for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                            if(BaseObject* pBO = ssas[iSSA].getObject())
                                if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                                    if(Scene* pSubScene = pSSA->getSubScene())
                                        sceneToScan.push_back(pSubScene);

                        const PickableList& content = pScene->getPickableList();
                        for(u32 iObj = 0; iObj < content.size(); ++iObj)
                            applyRenderColor(highlightColor, content[iObj]);
                    }
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//list objects which have picking shapes
//And draw their debug info
void Editor::draw()
{
    TIMERASTER_SCOPE(editor_Draw);

    EditorDraw::startDraw();

    EditorDraw::drawCurrentMode();

#ifndef ITF_CONSOLE_FINAL
    if(GFX_ADAPTER->getSafeFrameMode())
        GFX_ADAPTER->requestSafeFrameDraw();
#endif // ITF_CONSOLE_FINAL

    if (CAMERA->isBiased() && (CURRENTFRAME>=m_lastCameraRotationFrame) && (CURRENTFRAME-m_lastCameraRotationFrame<=1))
    {
        Vec3d pos;
        CAMERA->apply();
        GFX_ADAPTER->compute3DTo2D(CAMERA->m_bias_LookAt_RotationCenter, pos);
        GFX_ADAPTER->drawDBGText("X", pos.m_x, pos.m_y);
    }
    if(INPUT_ADAPTER->isKeyPressed(KEY_F1))
        EditorDraw::drawHelp();

    if (m_displayMemory)
        EditorDraw::drawMemory();

    // Draw measure lines in all modes
    const u32 uCount = m_MeasureLines.size();
    for(u32 i = 0; i < uCount; ++i)
    {
        const Vec3d& vB1 = m_MeasureLines[i];
        getIconStock().drawIcon(Editor_IconStock::Icon_Light, vB1, Anchor_DefaultSize / 5.0f);

        // If the index is even
        if((i+1) % 2 == 0)
        {
            const Vec3d& vA1 = m_MeasureLines[i-1];

            GFX_ADAPTER->drawDBG3DLine(vA1, vB1);
            
            const Vec3d textPos3d = (vA1+vB1)/ 2.0f;
            Vec3d proj2d;
            GFX_ADAPTER->compute3DTo2D(textPos3d, proj2d);

            // if in screen
            if(proj2d.m_x > 0.0 && proj2d.m_y > 0.0f && proj2d.m_x < GFX_ADAPTER->getScreenWidth() && proj2d.m_y < GFX_ADAPTER->getScreenHeight())
            {
                const f32 dist = (vA1 - vB1).norm();
                String distance;
                distance.setTextFormat("%.3f", dist);
                GFX_ADAPTER->drawDBGText(distance, proj2d.m_x, proj2d.m_y);
            }

            for(u32 j = i + 2; j < uCount; j += 2)
            {
                const Vec3d& vB2 = m_MeasureLines[j];

                if((j+1) % 2 == 0)
                {
                    const Vec3d& vA2 = m_MeasureLines[j-1];

                    const Vec3d *p1, *p2, *p3;
                    p1 = p2 = p3 = NULL;

                    if((vB1 - vB2).norm() < MTH_EPSILON)
                    {
                        p1 = &vA1;
                        p2 = &vB1;
                        p3 = &vA2;
                    }
                    else if((vA1 - vB2).norm() < MTH_EPSILON)
                    {
                        p1 = &vB1;
                        p2 = &vA1;
                        p3 = &vA2;
                    }
                    else if((vB1 - vA2).norm() < MTH_EPSILON)
                    {
                        p1 = &vA1;
                        p2 = &vB1;
                        p3 = &vB2;
                    }
                    else if((vA1 - vA2).norm() < MTH_EPSILON)
                    {
                        p1 = &vB1;
                        p2 = &vA1;
                        p3 = &vB2;
                    }

                    if(p1 && p2 && p3)
                    {
                        Vec3d v21 = (*p1 - *p2).normalize();
                        Vec3d v23 = (*p3 - *p2).normalize();

                        Vec2d v21_2D = v21.truncateTo2D();
                        Vec2d v23_2D = v23.truncateTo2D();

                        String angleText;
                        angleText.setTextFormat("%.2f deg", v21_2D.getShortestAngle(v23_2D) * MTH_RADTODEG);

                        const Vec3d vAngleLine1 = *p2 + v21 * 0.15f;
                        const Vec3d vAngleLine2 = *p2 + v23 * 0.15f;

                        const Vec3d displayPoint = *p2 + 2.0f * (((vAngleLine1 + vAngleLine2) / 2.0f) - *p2);
                        GFX_ADAPTER->compute3DTo2D(displayPoint, proj2d);
                        GFX_ADAPTER->drawDBGText(angleText, proj2d.m_x, proj2d.m_y);

                        GFX_ADAPTER->drawDBG3DLine(vAngleLine1, vAngleLine2);
                    }
                }                
            }
        }
    }

    //Fast exit if plain game
    if (getState() == EditorState_Idle && !EditorDraw::pickableDetail_canShowAnyDetail())
        return;

    Pickable* underMouse = NULL;

    // Update mouse over
    if (isPickingInfoValid())
        underMouse = static_cast<Pickable*>(getPickingShapeOfObjectUnderMouse()->getOwnerPtr());

    // If in game mode but must display info
    if (getState() == EditorState_Idle)
    {
        if(EditorDraw::pickableDetail_CanShowDbg())
        {
            ObjectRefList _objects;
            PickingShapeVector _orderedShapes;
            
            Scene* pFilterScene = NULL;

            if(displayAllSubContent())
            {
                if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(getEditedSubSceneHolder().getObject()))
                    pFilterScene = pSSA->getSubScene();
            }
            else
                pFilterScene = getEditedScene();
          
            PLUGINGATEWAY->fillEditorActiveList(_objects, _orderedShapes, pFilterScene);

            std::sort(_orderedShapes.begin(), _orderedShapes.end(), PickingShape::compareShapes);

            displayObjectEditingInfos(_objects, _orderedShapes, underMouse);
        }
    }
    else
    {
        switch(getState())
        {
        case EditorState_EditMode_SelectDepthRange:
            displayObjectEditingInfos(m_pickableObjects, m_activeShapesListOrdered, underMouse);
            break;

        default:
            {
                if (getSubEditedItem().isValid() )
                {
                    if (m_pickableObjects.find(getSubEditedItem()) == -1)
                    {
                        BaseObject* pObj = GETOBJECT(getSubEditedItem());
                        if (pObj)
                            drawObj(pObj, btrue);
                    }
                }

                displayObjectEditingInfos(m_pickableObjects, m_activeShapesListOrdered, underMouse);

                EditorDraw::drawCells();

                if(isPanActive())
                    endZoneSelection();

                if(m_zoneSelectionStarted)
                    EditorDraw::drawSelectionZone(m_zoneSelectionStart, getMouse2d());

                EditorDraw::drawOrigin();
            }
            break;
        }
    }
    
    m_iconStock->draw();
    
    if(m_gridMode != 0)
        EditorDraw::drawGrid(m_gridPoint, m_gridSize);

    clearExtraSelectableObjects();
}
//
void Editor::getWorldAABB(AABB &_aabb, f32 &_minZ, f32 &_maxZ)const
{
    _aabb.invalidate();
    _minZ = FLT_MAX;
    _maxZ = -FLT_MAX;

    //
    static ObjectRefList objects;
    static PickingShapeVector pickingShapes;
    objects.clear();
    pickingShapes.clear();
    PLUGINGATEWAY->fillEditorActiveList(objects, pickingShapes, NULL);
    //    
    for (u32 i=0; i<objects.size(); i++)
    {
        const ObjectRef & o = objects[i];
        BaseObject *obj = o.getObject();
        if (obj)
        {
            Pickable *pickable = obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
            if (pickable)
            {
                const AABB &objAABB = pickable->getAABB();
                if (objAABB.isValid())
                {
                    f32 z = pickable->getDepth();
                    _aabb.grow(objAABB);
                    _minZ = std::min(_minZ, z);
                    _maxZ = std::max(_maxZ, z);
                }
            }
        }
    }
    //    
}

Vec3d Editor::limitRayToWorldAABB(const Vec3d &v0, const Vec3d &v1, bbool mustLimitWithGreatestDistance)const
{
    AABB worldAABB;
    f32 minZ = FLT_MAX;
    f32 maxZ = -FLT_MAX;

    Vec3d result(v1);

    worldAABB.invalidate();

    getWorldAABB(worldAABB, minZ, maxZ);
    if (worldAABB.isValid() && minZ<=maxZ)
    {
        if (v1.m_z>=minZ && v1.m_z<=maxZ && worldAABB.contains(v1))
        {
            return result; //EARLY EXIT IF OK
        }

        Vec3d direction = v1-v0;
        Vec3d intersection;
        f32 lambda;
        f32 maxLambda = 0;
        
        Vec2d worldCorners2D[4]= {worldAABB.getMin(), worldAABB.getMinXMaxY(), worldAABB.getMaxXMinY(), worldAABB.getMax()};
        Vec3d zminCorner, zmaxCorner;
        f32 maxCornerDistance = 10.f; //dont limit less than this distance
        Vec3d camToCorner;
        for (u32 cornerIndex=0; cornerIndex<4; cornerIndex++)
        {
            const Vec2d &corner=worldCorners2D[cornerIndex];

            zminCorner.setFromVec2d(corner, &minZ);
            camToCorner = CAMERA->getCorrectedPos()-zminCorner;
            maxCornerDistance = std::max(maxCornerDistance, camToCorner.norm());


            zmaxCorner.setFromVec2d(corner, &maxZ);
            camToCorner = CAMERA->getCorrectedPos()-zmaxCorner;
            maxCornerDistance = std::max(maxCornerDistance, camToCorner.norm());
        }


        //intersection with minZ face
        {
            Plane p(Vec3d(0,0,1), minZ);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1) && worldAABB.contains(intersection))
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //intersection with maxZ face
        {
            Plane p(Vec3d(0,0,1), maxZ);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1) && worldAABB.contains(intersection))
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //intersection with minX face
        {
            Plane p(Vec3d(1,0,0), worldAABB.getMin().m_x);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1)
                    && (intersection.m_z>=minZ && intersection.m_z<=maxZ)
                    && (intersection.m_y>=worldAABB.getMin().m_y && intersection.m_y<=worldAABB.getMax().m_y)
                    )
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //intersection with maxX face
        {
            Plane p(Vec3d(1,0,0), worldAABB.getMax().m_x);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1)
                    && (intersection.m_z>=minZ && intersection.m_z<=maxZ)
                    && (intersection.m_y>=worldAABB.getMin().m_y && intersection.m_y<=worldAABB.getMax().m_y)
                    )
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //intersection with minY face
        {
            Plane p(Vec3d(1,0,0), worldAABB.getMin().m_y);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1)
                    && (intersection.m_z>=minZ && intersection.m_z<=maxZ)
                    && (intersection.m_x>=worldAABB.getMin().m_x && intersection.m_x<=worldAABB.getMax().m_x)
                    )
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //intersection with maxY face
        {
            Plane p(Vec3d(1,0,0), worldAABB.getMax().m_y);
            if (p.testLineIntersection(v0, direction, intersection, lambda))
            {
                if ((lambda>=0) && (lambda<=1)
                    && (intersection.m_z>=minZ && intersection.m_z<=maxZ)
                    && (intersection.m_x>=worldAABB.getMin().m_x && intersection.m_x<=worldAABB.getMax().m_x)
                    )
                {
                    maxLambda = std::max(lambda, maxLambda);
                }
            }
        }
        //
        if (maxLambda!=0)
        {
            result = v0 + maxLambda*(v1-v0);
        }
        //further limitation
        if (mustLimitWithGreatestDistance)
        {
            f32 limitation = maxCornerDistance * 1.1f;
            if ((result-v0).norm()>limitation)
            {
                Vec3d direction = result - v0;
                direction.normalize();
                result = v0 + direction*limitation;
            }
        }
    }

    return result;

}

//Camera rotation mode (when the user holds right mouse button + Space). Returns btrue if required delayed execution
bbool Editor::handleCameraRotationMode(bbool _reset)
{
    ObjectRefList objList;
    PickingInfo2D localInfo2D;
    const Vec2d& mousePos = getMouse2d();

    f32 deltaY = -(mousePos.m_y - (f32)m_camBiasMouseY) / (f32)GFX_ADAPTER->getScreenHeight();
    f32 deltaX = (mousePos.m_x - (f32)m_camBiasMouseX) / (f32)GFX_ADAPTER->getScreenWidth();

    CAMERA->setBiasAngles(m_camBiasHorizAngle - deltaX * MTH_PI, m_camBiasVertAngle + deltaY * MTH_PI);

    if (!CAMERA->isBiased() || _reset ) //Special processing when entering the rotation mode
    {
        bbool mustReevaluateFramebufferedPicking = bfalse;
        BaseObject *pObject = (BaseObject*)GETOBJECT(getLastHoveredActorOrFriseObjRef());
        if (CURRENTFRAME>=m_lastHoveredActorOrFrise_Frame)
        {
            u32 deltaFrame = CURRENTFRAME-m_lastHoveredActorOrFrise_Frame;
            if (deltaFrame>3)
                mustReevaluateFramebufferedPicking = btrue;
        }
        else
            mustReevaluateFramebufferedPicking = btrue;

        Vec3d rotationCenter;
        Vec3d eyePosition;

        CAMERA->apply();
        if (mustReevaluateFramebufferedPicking)
        {
            m_bRequireHoveredActorOrFriseDetection = btrue;
            return btrue; ///AND RETRY
        }
        
        //if not from pickable, take depth range
        rotationCenter = getMouse3d_NoWorkingDepth();

        
        rotationCenter = limitRayToWorldAABB(CAMERA->getCorrectedPos(), rotationCenter, bfalse /*this is temporary*/);
        //

        if (pObject)
        {
            //Get Z of object under mouse or get a default Z
            BaseObject::ObjectType typeOfObjectUnderMouse = BaseObject::eInvalid;
            if (pObject)
                typeOfObjectUnderMouse = pObject->getObjectType();

            if (typeOfObjectUnderMouse==BaseObject::eActor || typeOfObjectUnderMouse == BaseObject::eFrise)
            {
                const f32 hoveredZ = m_hoveredFriseOrActorProjectedZ;
                
                //The rotation center will be computed as the 3D point under the mouse
                GFX_ADAPTER->compute2DTo3D( 
                    Vec3d(mousePos.m_x, mousePos.m_y, hoveredZ),
                    rotationCenter
                    );
        }
        }

        //Set parameters : start a new rotation from current view
        eyePosition = CAMERA->getCorrectedPos();
 
        CAMERA->setBiasedRotationMode(rotationCenter, CAMERA->m_lookAt-eyePosition);
        showMouseIcon(Editor_IconStock::Icon_CameraRotate);
        CAMERA->m_resultingBiasedPosition = eyePosition;
    }


    if (CAMERA->isBiased())
        showMouseIcon(Editor_IconStock::Icon_CameraRotate);

    m_lastCameraRotationFrame = CURRENTFRAME;
    return bfalse;
}


void Editor::updatePanModeStatus()
{
    static bbool bLastPanActive     = bfalse;

    const bbool bSpacePressed       = INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY);

#ifdef ITF_WINDOWS
    const bbool bLeftMousePressed   = ((GetKeyState(VK_LBUTTON) & 0x80) != 0);
#else
    const bbool bLeftMousePressed   = INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Left);
#endif

    setPanActive(bSpacePressed && bLeftMousePressed);

    if (isPanActive())
        showMouseIcon(Editor_IconStock::Icon_CameraPan);

    // Detect pan activation change
    if(!bLastPanActive && isPanActive())
    {
        if(isPanActive())
        {
            LOG_OLIV("[Editor] Start pan mode");
        }

        Vec3d referencePoint = getMouse3d_NoWorkingDepth();
        referencePoint = limitRayToWorldAABB(CAMERA->getCorrectedPos(), referencePoint, btrue);
        //GFX_ADAPTER->drawDBGCircle(referencePoint, 1.5f,1,1,1,1);

        GFX_ADAPTER->compute3DTo2D(referencePoint, m_vPanProjectedOrigin);
        CAMERA->setMode(Camera::Camera_Editor);
    }
    else if (bLastPanActive && !isPanActive())
    {
        LOG_OLIV("[Editor] End pan mode");
    }

    bLastPanActive = isPanActive();

#ifdef ITF_WINDOWS
    const bbool bRightMousePressed   = ((GetKeyState(VK_RBUTTON) & 0x80) != 0);
#else
    const bbool bRightMousePressed   = INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Right);
#endif

    if (bRightMousePressed && bSpacePressed)
    {
        m_resetCameraRotation = handleCameraRotationMode(m_resetCameraRotation);
    }
    else
    {
        m_resetCameraRotation = btrue;
        m_camBiasMouseX = i32(getMouse2d().m_x);
        m_camBiasMouseY = i32(getMouse2d().m_y);
        m_camBiasHorizAngle = CAMERA->m_biasHorizAngle;
        m_camBiasVertAngle = CAMERA->m_biasVertAngle;
    }

    if (!CAMERA->isBiased())
    {
        CAMERA->m_biasHorizAngle = 0.f;
        CAMERA->m_biasVertAngle = 0.f;
        m_camBiasMouseX = i32(getMouse2d().m_x);
        m_camBiasMouseY = i32(getMouse2d().m_y);
        m_camBiasHorizAngle = 0.f;
        m_camBiasVertAngle = 0.f;
    }
}

void Editor::refillPickableList()
{
    m_activeShapesListOrdered.clear();
    m_pickableObjects.clear();
    
    if(isInRotationScaleHelperMode())
        m_pickableObjects.push_back(m_rotationScale_focus);

    Scene* pFilterScene = NULL;

    if(displayAllSubContent())
    {
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(getEditedSubSceneHolder().getObject()))
            pFilterScene = pSSA->getSubScene();
    }
    else
        pFilterScene = getEditedScene();

    // Add conventionnal objects
    PLUGINGATEWAY->fillEditorActiveList( m_pickableObjects, m_activeShapesListOrdered, pFilterScene);

    std::sort(m_activeShapesListOrdered.begin(), m_activeShapesListOrdered.end(), PickingShape::compareShapes);
}

bbool Editor::isPickableUnderPoint(const Pickable *_object, const Vec2d &_point, Vec3d &_intersection)
{
    Vec3d pointWithZ(_point.m_x, _point.m_y, 0.1f);
    Vec3d linePoint[2];
    f32 dummy;

    //Test intersection between line corresponding to given _point on screen, and the object plane
    GFX_ADAPTER->compute2DTo3D(pointWithZ, linePoint[0]);
    linePoint[1] = CAMERA->getCorrectedPos();

    const f32 z = _object->getDepth();
    Plane objectPlane(Vec3d(0,0,1), z);
    if (objectPlane.testLineIntersection(linePoint[0], linePoint[1]-linePoint[0], _intersection, dummy))
    {
        _intersection.m_z = z;
        AABB aABB(_object->getAABB());

        // Get visual AABB
        if (_object->getObjectType()==BaseObject::eActor)
        {
            Actor *actor = (Actor*)_object;
            GraphicComponent *pGraphComponent = actor->GetComponent<GraphicComponent>();
            if (pGraphComponent)
                aABB.grow(pGraphComponent->getVisualAABB());
        }

        return aABB.contains(_intersection);
    }
    return bfalse;
}

//Do framebuffer-assisted picking preparation : set m_lastHoveredFriseObjRef with the frise under the mouse cursor
//Normal picking will use m_lastHoveredFriseObjRef afterwards
void Editor::updateFramebufferedPicking()
{
#ifdef ITF_WINDOWS
    bbool pickOnlyCollisionFriezes = bfalse;
    if (FriezeEditor::getInstance() && FriezeEditor::getInstance()->isMultiCollisionEdition(NULL))
        pickOnlyCollisionFriezes = btrue;
    TIMERASTER_SCOPE(updateFramebufferedPicking);
    GFX_ADAPTER->startRendering();
    CAMERA->apply();

    const Vec2d& mouseXY = getMouse2d();
    m_hoveredFriseOrActorProjectedZ = 1.f;

    f32 maxPriority = -F32_INFINITY;
    Pickable *bestPickable = NULL;

    //
    const bbool isEditMode = ((getState() != Editor::EditorState_Idle));
    if (isEditMode || m_bRequireHoveredActorOrFriseDetection || (CAMERA->getMode()==Camera::Camera_Editor))
    {
        static BaseObjectList hoverTestList;
        hoverTestList.clear();

        if (isEditMode)
        {
            hoverTestList.mergeArray(m_pickableObjects);
        }
        else
        {
            const World* pWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
            if (pWorld)
                hoverTestList.mergeArray(pWorld->getActiveObjects());
        }

        ID_SERVER->getObjectListNoNullV2(hoverTestList);
        SafeArray<BaseObject*>& hoverList = hoverTestList.m_solvedPointers;

        if(!displayAllSubContent())
        {
            // Recursive search
            SafeArray<SubSceneActor*> ssaList;

            // Find starting subsceneactors
            for(u32 i = 0; i < hoverList.size(); ++i)
            {
                if(SubSceneActor* pSSA = hoverList[i]->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    ssaList.push_back(pSSA);
            }

            // For each add content
            while(ssaList.size())
            {
                SubSceneActor* pSSA = ssaList[0];
                ssaList.eraseNoOrder(0);

                if(Scene* pSubScene = pSSA->getSubScene())
                {
                    const BaseObjectList& list = pSubScene->getActiveObjects();
                    ID_SERVER->getObjectListNoNullV2(list);
                    SafeArray<BaseObject*>& subList = list.m_solvedPointers;
                    hoverList.mergeArray(subList);

                    for(u32 i = 0; i < subList.size(); ++i)
                        if(SubSceneActor* pSubSSA = subList[i]->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            ssaList.push_back(pSubSSA);
                }
            }
        }

        GFX_ADAPTER->setRenderTarget_Picking();    
        GFX_ADAPTER->clear( GFX_CLEAR_COLOR , 0,0,0, 0);

        GFX_RECT rcScissor;
        f32 delta = 0.0f;
        const f32 moveDelta     = 4.0f;
        const u32 moveDeltaU32  = 4;
        rcScissor.left = (u32) delta;
        rcScissor.right = (u32) delta+moveDeltaU32;
        rcScissor.top =  0;
        rcScissor.bottom =  moveDeltaU32;

        //For each pickable object
        const u32 uSize = hoverList.size();

        static SafeArray<Pickable*> vPickRequest;
        vPickRequest.clear();
        vPickRequest.reserve(uSize);

        for (u32 index = 0; index < uSize; index++)
        {        
            Pickable *object = (Pickable*)(hoverList[index]);
            if (!object)
                continue;
            //Check whether it's a frise or an actor
            BaseObject::ObjectType type = object->getObjectType();
            const bbool isFrise = type==BaseObject::eFrise;
            const bbool isActor = type==BaseObject::eActor;
            const bbool isSubScene = object->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817))!=NULL;

            if (pickOnlyCollisionFriezes && !isFrise)
                continue;

            if (isFrise)
            {
                Frise* pFrise = (Frise*)object;
                if (pickOnlyCollisionFriezes)
                {
                    const SafeArray<PolyLine*>* pCollisionData = pFrise->getCollisionData_WorldSpace();
                    if ( !pCollisionData || !pCollisionData->size() )
                        continue;
                }
                FriseConfig* pConfig = pFrise->getConfig();
                if (pConfig)
                {
                    if ((pConfig->m_objectFamily & Pickable::ObjectFamily_Alpha) != 0)
                    {
                        if (!isObjectFamilyPickableForFrameBuffer(Pickable::ObjectFamily_Alpha))
                            continue;
                    }
                }
            }

            if ( isFrise || (isActor && !isSubScene))
            {
                if (!object->isVisible(CAMERA))
                    continue;
                Vec3d intersection;              

                //If under mouse cursor
                if (isPickableUnderPoint(object, mouseXY, intersection) )
                {
                    //only pick opaque enough objects
                    //render and get alpha in back buffer

                    GFX_ADAPTER->setPickingMode(btrue,Vec2d(mouseXY.m_x,mouseXY.m_y),delta);
                    GFX_ADAPTER->setScissorRect(&rcScissor);

                    vPickRequest.push_back(object);

                    delta+= moveDelta;
                    rcScissor.left+=moveDeltaU32;
                    rcScissor.right+=moveDeltaU32;

                    const bbool isAlphaTestEnabled = GFX_ADAPTER->isAlphaTestEnabled();

                    GFX_ADAPTER->setAlphaRef(m_FrameBufferPickingAlphaRef);
                    //    GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
                    GFX_ADAPTER->setAlphaTest(btrue);
                    object->draw(); 
                    GFX_ADAPTER->drawPrimitives();
                    GFX_ADAPTER->resetSceneResolvedFlag();
                    GFX_ADAPTER->setAlphaTest(isAlphaTestEnabled);
                }
            }
        }

        void* backbufferPicking = GFX_ADAPTER->resolvePicking();
        delta = 0.0f;

        if (backbufferPicking)
        {
            for (u32 index = 0; index < vPickRequest.size(); index++)
            {        
                Pickable *object = vPickRequest[index];

                Vec3d intersection;              

                //If under mouse cursor

                const Color backbufferColor = GFX_ADAPTER->getBufferValue(backbufferPicking,(u32)delta, (u32) 0.0f);
                delta+= moveDelta;

                if (backbufferColor.m_a != 0)
                {
                    Vec3d projected;
                    ITF_VERIFY(isPickableUnderPoint(object, mouseXY, intersection));
                    GFX_ADAPTER->compute3DTo2D(intersection, projected);

                    const f32 Z = projected.m_z;

                    //Get object with biggest priority
                    if (Z >= 0.0f && Z <= 1.0f)
                    {
                        const f32 depth = object->getDepth();

                        if (m_hoveredFriseOrActorProjectedZ>Z)
                            m_hoveredFriseOrActorProjectedZ = Z;
                        if (depth >= maxPriority)
                        {
                            maxPriority = depth;
                            bestPickable = object;
                        }
                    }
                }

                GFX_ADAPTER->setRenderTarget_BackBuffer();
            }

            m_lastHoveredActorOrFrise_Frame = CURRENTFRAME;
            m_bRequireHoveredActorOrFriseDetection = bfalse;

            //Update m_lastHoveredFriseObjRef with best frise
            if (bestPickable)
            {
                m_lastHoveredFriseObjRef = bestPickable->getRef();

                ObjectRef resultingContainer;
                if (checkIfMustSelectContainerForObject(m_lastHoveredFriseObjRef, resultingContainer))
                {
                    m_lastHoveredFriseObjRef = resultingContainer;
                }
            }
            else
                m_lastHoveredFriseObjRef.invalidate();

            GFX_ADAPTER->setRenderTarget_BackBuffer();
            //Restore normal operation, for real beginning of the game frame
            GFX_ADAPTER->renderContext_SetDefault();
            GFX_ADAPTER->setAlphaRef();
            GFX_ADAPTER->clear( GFX_CLEAR_COLOR , CONFIG->m_BGColor[0], CONFIG->m_BGColor[1], CONFIG->m_BGColor[2], 0);

            GFX_ADAPTER->endRendering();
        }

        //restore previous context
        rcScissor.left= 0;
        rcScissor.top = 0;
        rcScissor.right = GFX_ADAPTER->getScreenWidth();
        rcScissor.bottom = GFX_ADAPTER->getScreenHeight();
        GFX_ADAPTER->setScissorRect(&rcScissor);
        GFX_ADAPTER->setPickingMode(bfalse,Vec2d(0.0f,0.0f),0.0f);
    }
#endif//ITF_WINDOWS
}

bbool Editor::computeSelectionCentroidWorldPosition(Vec3d& _centroid) const
{
    Vec3d vTarget;
    u32 validSelectionCount = 0;
    const u32 uSelectionSize = getSelectionSize();
    for(u32 i = 0; i < uSelectionSize; ++i)
    {
        PickingShape* pPickingShape = getSelectionAt(i);
        const Pickable* pPickable = static_cast<const Pickable*>(pPickingShape->getOwnerPtr());
        if(pPickable)
        {
            Vec3d pos = pPickable->getAABB().getCenter().to3d();

            /*
            if(pPickable->getObjectType() == BaseObject::eFrise && pPickable->getPickingInfo())
            {
                const Frise* pFrise = static_cast<const Frise*>(pPickable);
                PickingInfo2D* info = pFrise->getPickingInfo();
                u32 index = FriezeExtraInfo(info->m_extraInformation).m_flags.m_subItemIndex;

                Vec3d p2D;
                GFX_ADAPTER->compute3DTo2D(pFrise->get3DPosAt(index), p2D);
                p2D.m_x -= info->m_anchor.m_x;
                p2D.m_y -= info->m_anchor.m_y;
                GFX_ADAPTER->compute2DTo3D(p2D, pos);
            }
            */

            if(validSelectionCount == 0)
                vTarget =  pos;
            else
                vTarget += pos;

            validSelectionCount++;
        }
    }

    if(validSelectionCount > 0)
    {
        vTarget /= f32(validSelectionCount);
        _centroid = vTarget;
        return btrue;
    }
    return bfalse;
}

void Editor::showContextMenu(bbool _onOff, ObjectRef _forceTarget, bbool _keepPos ) 
{
    if (_onOff)
        m_joingroupList.clear();

    ObjectRef targetRef = _forceTarget;
    if (!isContextMenuShowable() && _onOff)
    {
        BaseObject* target = GETOBJECT(_forceTarget);
        if (!target)
        {
            target = GETOBJECT (getSubEditedItem());
            if (!target)
            {
                PickingShape* pick = getLastClickedShape();
                if (!pick)
                {
                    if (getSelectionSize())
                        pick = getSelectionAt(0);
                    if (!pick)
                        pick = getPickingShapeOfObjectUnderMouse();
                }
                if (pick)
                    target = pick->getOwnerPtr();
            }
        }
        if (target && _forceTarget.isValid())
        {
            if (target->getObjectType() == BaseObject::eActor || target->getObjectType() == BaseObject::eFrise)
                selectObject(targetRef, btrue);
            else
                unselectAll();
        }
        if (!target)
        {
            target = getEditedScene();
        }
        if (target)
            targetRef = target->getRef();


        for (u32 i=0; i<2; i++)
        {
            m_contextMenu[i].m_lastTarget = targetRef;
            m_contextMenu[i].clearItems();
            if (!_keepPos)
            {
                m_contextMenu[i].setPos(getMouse2d());
                m_contextMenu[i].m_launchPos = getMouse2d();
            }
        }


        if (INPUT_ADAPTER->isKeyPressed(KEY_LALT)) // debug contextual menu
        {
            fillAltContextMenu(targetRef);
            PLUGINGATEWAY->fillAltContextMenu(m_contextMenu[0]);
            m_contextMenu[0].m_lastTarget.invalidate();
        }
        else    // normal contextual menu
        {
            fillTopItemsContextMenu(targetRef);
            _onOff &= PLUGINGATEWAY->fillContextMenu(m_contextMenu[0], targetRef, btrue);
            fillNormalContextMenu(targetRef);
            _onOff &= PLUGINGATEWAY->fillContextMenu(m_contextMenu[0], targetRef, bfalse);
        }
    }

    setContextMenuShowable(_onOff);
}
           
void Editor::switchContextMenu() 
{
    showContextMenu(!isContextMenuShowable());
}


void Editor::onGainFocus()
{
    m_applicationHasFocus = btrue;

    m_cancelClickTime = m_cancelClickInitTime;
}

void Editor::onLoseFocus()
{
    m_applicationHasFocus = bfalse;
}

 
bbool Editor::checkObjectVisible(ObjectRef _or)
{
    const Pickable* pObj = (Pickable*)_or.getObject();
    if (!pObj)
        return bfalse;

    if(pObj->isSelected())
        return btrue;

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
    if (isHiddenBecauseOfEditorFilter(pObj))
        return bfalse;
#endif
  
    if (!pObj->isVisible(CAMERA) )
         return bfalse;

    if (!(getDisplayedFamilies() & pObj->getObjectFamily()))
        return bfalse;


    if (pObj->getObjectType() == BaseObject::eFrise && FriezeEditor::getInstance()->hasFriezeInfoFlag(FriezeEditor::FriezeInfoFlag_Any))
        return btrue;

    if (pObj->getObjectType() == BaseObject::eActor && ActorEditor::getInstance()->hasActorInfoFlag(ActorEditor::ActorInfoFlag_Any))
        return btrue; 

    Pickable * subEditedObj = (Pickable*)GETOBJECT(getSubEditedItem());

    if (subEditedObj)
    {
        if (!subEditedObj->IsClassCRC(ITF_GET_STRINGID_CRC(Actor,2546623115)) && !isSubEdited(_or))
        {
           return bfalse;
        }
    }
    return btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////
bbool Editor::checkIfMustSelectContainerForObject(const ObjectRef &_object, ObjectRef &_resultingContainer)
{
    _resultingContainer.invalidate();

    if (BaseObject *baseObject = _object.getObject())
    {
        if (Pickable* pickable = baseObject->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
        {
            Scene *scene = pickable->getScene();
            if (!scene)
            {
                return bfalse;
            }
            SubSceneActor *holder = scene->getSubSceneActorHolder();
            if (!holder)
            {
                return bfalse;
            }
            if (isPickableObject(_object)) //this is the core of the test. It must be individually selectable
            {
                return bfalse;
            }

            _resultingContainer = holder->getRef();
            return btrue;
        }
        else
            return bfalse;
    }
    else
    {
        return bfalse;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
bbool Editor::isPickableObject(ObjectRef _obj)
{
    return m_pickableObjects.find(_obj)>=0;
}

///////////////////////////////////////////////////////////////////////////////////////////
bbool Editor::removeMainShapeFromSelection( ObjectRef _obj )
{
    bbool bResult = bfalse;
    const PickingShape* pShape = getMainPickingShapeForObject(_obj);

    if(pShape)
    {
        const u32 uSelectionSize = getSelectionSize();
        for(u32 i = 0; i < uSelectionSize; ++i)
        {
            if(getSelectionAt(i) == pShape)
            {
                m_selection.eraseNoOrder(i);                
                bResult = btrue;
                break;
            }
        }
    }

    return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////////
PickingShape* Editor::getMainPickingShapeForObject(const ObjectRef _obj)const
{
    const SafeArray<PickingShape*>* shapes = getPickingShapesForObject(_obj);
    if (!shapes)
        return NULL;

    for (u32 iShape = 0; iShape < shapes->size(); iShape++)
    {
        if (!(*shapes)[iShape]->isSubAnchor())
            return (*shapes)[iShape];
    }

    return NULL;
}

void Editor::startToEditSubScene(const SubSceneActor* _pSSA, const ObjectRefList& _newStartingSelection)
{
    LOG_OLIV("[Editor::startToEditSubScene]");

    if(!allowEditSubScene(_pSSA))
        return;

    // Do not startToEditSubScene the same scene twice
    const bbool twiceCheck = !getEditedSubSceneHolder().isValid() || (getEditedSubSceneHolder() != _pSSA->getRef());
    ITF_ASSERT_MSG(twiceCheck, "Do not startToEditSubScene the same scene twice");
    if(!twiceCheck)
        return;

    ITF_ASSERT_MSG(_pSSA, "The first param must be a subsceneactor reference");
    if(!_pSSA)
        return;

    m_editedSubSceneHolderRef = _pSSA->getRef();
#ifdef ITF_WINDOWS
    m_gridSize = _pSSA->getSubScene()->getGridUnit();
#endif //ITF_WINDOWS
    selectObjects(_newStartingSelection, btrue);
    
    // Auto show picking shapes
    if(EditorDraw::getPickableDetailMode() < EditorDraw::PickableDetails_ShowAnchors)
        EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_ShowAnchors);
}

void Editor::endSubSceneEdition()
{
    LOG_OLIV("[Editor::endSubSceneEdition]");

    // Do not endSubSceneEdition twice
    ITF_ASSERT_MSG(getEditedSubSceneHolder().isValid(), "Do not endSubSceneEdition twice");
    SubSceneActor* pSSA = (SubSceneActor*)getEditedSubSceneHolder().getObject();
    if(!pSSA)
        return;

    m_editedSubSceneHolderRef = getWorkingSubScene();
#ifdef ITF_WINDOWS
    m_gridSize = CURRENTWORLD->getRootScene()->getGridUnit();
#endif //ITF_WINDOWS
    if(!m_editedSubSceneHolderRef.isValid())
    selectObject(pSSA->getRef(), btrue);
}

void Editor::deletePickingShapes()
{
    ITF_MAP<ObjectRef, SafeArray<PickingShape*> >::iterator it = m_pickingShapes.begin();
    ITF_MAP<ObjectRef, SafeArray<PickingShape*> >::iterator end = m_pickingShapes.end();
    for (; it != end; ++it)
    {
        SafeArray<PickingShape*>& shapes = it->second;
        for (u32 i = 0; i < shapes.size(); i++)
        {
            // don't assert here, plugins are destroyed before final scene quen exiting the app
            SF_DEL(shapes[i]);
        }
    }
}

Scene* Editor::getEditedScene() const
{
    Scene* pScene = NULL;
    SubSceneActor* pCurrentSubSceneHolder = static_cast<SubSceneActor*>(getEditedSubSceneHolder().getObject());
    if(pCurrentSubSceneHolder)
    {
        pScene = pCurrentSubSceneHolder->getSubScene();
    }
    else
    {
        World* pWorld = CURRENTWORLD;
        if(pWorld)
            pScene = pWorld->getRootScene();
    }

    return pScene;
}

    void Editor::applyPendingDragAndDropMap()
    {
        if(!SCENE_MANAGER->isLoadingPending())
        {
            if (!m_pendingDropMap.isEmpty())
            {
                bbool proceed = bfalse;

                if(!WORLD_MANAGER->getAutoUnloadPreviousWorld())
                {
                    proceed = btrue;
                    preSwapCurrentWorld(bfalse);
                }
                else
                    proceed = checkAndAskForUnsavedWork();

                if(proceed)
                {
                    // Stop all music
                    MUSICMANAGER->stopAll(0.0f);

                    backToGame();

                    ITF_VERIFY(GAMEMANAGER->loadGameplayMap(m_pendingDropMap, btrue, bfalse));
                }
                
                m_pendingDropMap.clear();
            }
        }
    }

    void Editor::onFileDrop (const String& _absolutePath)
    {
        String relativePath;
        if (!FILESERVER->getPathFromDataFolder(relativePath, _absolutePath))
            return;

        const String extension = FilePath::getExtension(relativePath);
        if (extension == "isc" || extension == "isd")
        {
            setPendingDragAndDropMap(relativePath);
        }
        else if (extension == "bsc")
        {
            ITF_VERIFY(loadTabInBackground(relativePath, NULL));
            setTabMainScenesModified();
        }
        else 
        {
            ActorEditor* pActorEditor = ActorEditor::getInstance();
            if(pActorEditor)
            {
                if(extension == "tsc")
                {
                    pActorEditor->enterCreateSceneState(_absolutePath, bfalse, bfalse);
                }
                else if(extension == "grp")
                {
                    pActorEditor->enterCreateSceneState(_absolutePath, bfalse, btrue);
                }
            }
        }
    }


    void Editor::resetcontextMenus()
    {
        forceShowContextMenu(bfalse);
        for (u32 i=0; i<2; i++)
        {
            m_contextMenu[i].m_lastTarget = ITF_INVALID_OBJREF;
            m_contextMenu[i].clearItems();
            m_contextMenu[i].setPos(getMouse2d());
            m_contextMenu[i].m_launchPos = getMouse2d();
        }
    }
    
    EditorContextMenu& Editor::getContextMenu()
    {
        return m_contextMenu[0];
    }
    
    void Editor::forceShowContextMenu(bbool _show, bbool _reFillContent, ObjectRef _forceTarget, bbool _keepPos)
    {
        if (_show)
        {
            m_nextMenu.m_show = _show;
            m_nextMenu.m_reFillContent = _reFillContent;
            m_nextMenu.m_forceTarget = _forceTarget;
            m_nextMenu.m_keepPos = _keepPos;
            m_nextMenu.m_active = btrue;
        }
        else
        {
            m_nextMenu.m_active = bfalse;
            setContextMenuShowable(bfalse);
        }
    }

    void Editor::friendlyFinder(const String* _search)
    {
#if defined(ITF_WINDOWS)
        String friendlyToSearch = m_lastCtrlFString;
        if (!_search)
        {
            if (!GUI_TextInput::inputText("Friendly finder", "Enter a friendly for search :", friendlyToSearch))
                return;
        }
        else
        {
            friendlyToSearch = *_search;
        }

        clearMultiFilter();
        m_lastCtrlFString = friendlyToSearch;
        if (!friendlyToSearch.getLen())
            return;

        u32 firstValidChar = 0;
        for(u32 i = 0; i < friendlyToSearch.getLen(); ++i)
        {
            if(friendlyToSearch.cStr()[i] != ' ')
            {
                firstValidChar = i;
                break;
            }
        }

        if (friendlyToSearch.cStr()[firstValidChar] == L'-' || friendlyToSearch.cStr()[firstValidChar] == L'+')
        { // expression parser
            #define CHECK_END if((safeiter >= 1024) || (*parser == 0)) {ITF_ERROR("Error while parsing expression: unexpected end"); break;}
            #define NEXT_CHAR parser++; safeiter++; CHECK_END
            #define NEXT_CHAR_NO_CHECK parser++; safeiter++;
            #define SKIP_SPACES while(*parser == ' ' || *parser == '\t') {NEXT_CHAR} CHECK_END

            StringConverter expr(friendlyToSearch) ;
            const char* parser = expr.getChar();
            u32 safeiter = 0;
            SMultiFilter::FilterOp currentOp = SMultiFilter::FilterOp_none;
            u32 nextOpEnumAdd = 2;
            bbool setLabelToLower = btrue;
            m_isUpdateMultiFilter = bfalse;

            while (*parser)
            {
                SKIP_SPACES
                switch (*parser)
                {
                case '+':
                    nextOpEnumAdd = 0;
                    break;
                case '-':
                    nextOpEnumAdd = 1;
                    break;
                }
                if (nextOpEnumAdd > 1)
                {
                    ITF_ERROR("any new instruction in find option should begin with a '+' or a '-'");
                    return;
                }
                NEXT_CHAR
                switch (*parser)
                {
                case 'f':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includeFriendly + nextOpEnumAdd);
                    setLabelToLower = btrue;
                    break;
                case 't':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includeTarget + nextOpEnumAdd);
                    setLabelToLower = btrue;
                    break;
                case 'p':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includePath + nextOpEnumAdd);
                    setLabelToLower = btrue;
                    break;
                case 'c':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includeComponent + nextOpEnumAdd);
                    setLabelToLower = bfalse;
                    break;
                case 'z':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includeZ + nextOpEnumAdd);
                    setLabelToLower = bfalse;
                    break;
                case 'l':
                    currentOp = (SMultiFilter::FilterOp)((u32)SMultiFilter::FilterOp_includeCellLayer + nextOpEnumAdd);
                    setLabelToLower = bfalse;
                    break;
                case 'u':
                    m_isUpdateMultiFilter = (nextOpEnumAdd == 0);
                    return;
                default:
                    ITF_ERROR("unknown instruction in find option");
                    return;
                }

                NEXT_CHAR
                SKIP_SPACES

                SMultiFilter filter;
                filter.m_op = currentOp;
                if (currentOp == SMultiFilter::FilterOp_includeZ || currentOp == SMultiFilter::FilterOp_excludeZ)
                {
                    if(*parser)
                    {
                        CHECK_END
                        SKIP_SPACES
                        switch (*parser)
                        {
                        case '>':
                            filter.m_arithmeticOp = SMultiFilter::Op_Bigger;
                            break;
                        case '<':
                            filter.m_arithmeticOp = SMultiFilter::Op_Lower;
                            break;
                        case '=':
                            filter.m_arithmeticOp = SMultiFilter::Op_Equals;
                            break;
                        default:
                            ITF_ERROR("error while parsing +/-z operator arguments : looking for '<', '>', or '='");
                            break;
                        }
                        NEXT_CHAR
                        SKIP_SPACES
                        while(*parser && *parser!=' ' && *parser != '\t')
                        {
                            CHECK_END
                            filter.m_label += *parser; 
                            NEXT_CHAR_NO_CHECK
                        }
                        filter.m_arithmeticArg = filter.m_label.atof32();
                    }                    
                }
                else
                {
                    while(*parser && *parser!=' ' && *parser != '\t')
                    {
                        CHECK_END
                        filter.m_label += *parser; 
                        NEXT_CHAR_NO_CHECK
                    }
                    if (setLabelToLower)
                        filter.m_label.toLower();
                }
                if (nextOpEnumAdd)
                    filter.m_isIncludeQuerry = bfalse;
                else
                    filter.m_isIncludeQuerry = btrue;

                if (currentOp == SMultiFilter::FilterOp_includeCellLayer || currentOp == SMultiFilter::FilterOp_excludeCellLayer)
                    filter.m_arithmeticArg = filter.m_label.atof32();
                addMultiFilter(filter);

                if (!*parser)
                    return;

                NEXT_CHAR_NO_CHECK
            }
            return;
        }
        /*
        if (friendlyToSearch.cStr()[0] == L'-' || friendlyToSearch.cStr()[0] == L'+')
        {
            u32 safeiter = 0;
            friendlyToSearch.toLower();
            friendlyToSearch.replace(" ","");
            i32 index = 0;
            String plus("+");
            String minus("-");
            for (;;)
            {
                safeiter++;
                if (safeiter > 1024)
                    break;
                i32 startindex = index;
                u16* ptr = friendlyToSearch.strstr(plus.cStr(), bfalse, &index, startindex);
                if (!ptr)
                    break;
                ptr++;
                index+=startindex+1;
                SMultiFilter filter;
                filter.m_op = SMultiFilter::FilterOp_includeFriendly;
                while(*ptr && *ptr!=L' ' && *ptr != L'+' && *ptr != L'-')
                {
                    filter.m_label += (char)(*ptr++); 
                    index++;
                }
                filter.m_label.toLower();
                addMultiFilter(filter);
                if (!(*ptr))
                    break;
            }
            index = 0;
            safeiter = 0;
            for (;;)
            {
                safeiter++;
                if (safeiter > 1024)
                    break;
                i32 startindex = index;
                u16* ptr = friendlyToSearch.strstr(minus.cStr(), bfalse, &index, startindex);
                if (!ptr)
                    break;
                ptr++;
                index+=startindex+1;
                SMultiFilter filter;
                filter.m_op = SMultiFilter::FilterOp_excludeFriendly;
                while(*ptr && *ptr!=L' ' && *ptr != L'+' && *ptr != L'-')
                {
                    filter.m_label += (char)(*ptr++); 
                    index++;
                }
                filter.m_label.toLower();
                addMultiFilter(filter);
                if (!(*ptr))
                    break;
            }
            return;
        }
*/
        LOG_OLIV("[Editor] Searching : %S...", friendlyToSearch.cStr());
        String8 friendlyToSearch8(friendlyToSearch.cStr());
        friendlyToSearch8.toLower();

        u32 uBestScore = U32_INVALID;
        ObjectRefList bestMatchs;

        const u32 wCount = WORLD_MANAGER->getWorldCount();
        for(u32 iW = 0; iW <wCount; ++iW)
        {
            World* pWorld = WORLD_MANAGER->getWorldAt(iW);
            if(pWorld && pWorld->isActive())
            {
                const u32 sceneCount = pWorld->getSceneCount();
                for(u32 iS = 0; iS < sceneCount; ++iS)
                {
                    const Scene* pScene = pWorld->getSceneAt(iS);
                    const PickableList& objs = pScene->getPickableList();
                    const u32 uObjCount = objs.size();
                    for(u32 iP = 0; iP < uObjCount; ++iP)
                    {
                        const Pickable* pObj = objs[iP];

                        if (!pObj)
                            continue;

                        if (!pObj->isEnabled() && !m_authorizeDeactivatedObjects)
                            continue;

                        if(!pObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) && !pObj->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664)) /*&& !pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817))*/)
                            continue;

                        PickingShape* pPS = getMainPickingShapeForObject(pObj->getRef());
                        if(!pPS) // TODO dig around it's smelling bad
                            continue;

                        String8 objFriendly         = pObj->getUserFriendly();
                        objFriendly.toLower();

                        const i32 inputLen          = friendlyToSearch8.getLen();
                        const i32 objFriendlyLen    = objFriendly.getLen();
                        const u32 diffLen           = abs(inputLen - objFriendlyLen);

                        // First search as substring
                        if(objFriendly.strstr(friendlyToSearch8.cStr()) != NULL)
                        {
                            if(diffLen <= uBestScore)
                            {
                                if(diffLen < uBestScore)
                                {
                                    LOG_OLIV("[Editor] Clear current best matches");
                                    bestMatchs.clear();
                                }
                                LOG_OLIV("[Editor] Temp found : %s...", objFriendly.cStr());

                                bestMatchs.push_back(pObj->getRef());
                                uBestScore = diffLen;
                            }
                        }                            
                    }
                }
            }
        }

        if (DEBUGINFO)
        {
            String userInfo;
            userInfo.setTextFormat("Search by Friendly: found %u objects", bestMatchs.size());
            DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
        }

        if(bestMatchs.size() > 0)
        {
            PLUGINGATEWAY->setExclusiveFocus(NULL);
            selectObjects(bestMatchs, btrue);
            camFocusOnSelection();
        }
#endif // ITF_WINDOWS
    }

    void Editor::camFocusOnSelection()const
    {   
        if(CAMERA->getMode() == Camera::Camera_Editor)
        {
            Vec3d target;
            if(computeSelectionCentroidWorldPosition(target))
            {
                AABB aabb;
                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(getEditorSelectionAsObjRefList(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    const Pickable* pObj = static_cast<const Pickable*>(resolvedRefs[i]);

                    if(!aabb.isValid())
                        aabb = pObj->getAABB();
                    else
                        aabb.grow(pObj->getAABB());
                }
        
                f32 tanFocalBy2 = f32_Abs(tanf(CAMERA->getFocale()*0.5f)); 
                f32 screenRatio = 0.5f;
                if( CAMERACONTROLLERMANAGER->m_screenRatio )
                    screenRatio /= CAMERACONTROLLERMANAGER->m_screenRatio;

                target.m_z += Max( aabb.getWidth() *screenRatio, aabb.getHeight() *0.5f) /tanFocalBy2;

                CAMERA->forcePosition(target);
            }
            CAMERA->cancelBiasMode();
        }
    }

    bbool Editor::getFullPath(const BaseObject* _pObject, String& _fullPath)
    {
        ITF_ASSERT(_pObject);
        if(!_pObject)
            return bfalse;

        String relativePath;
        switch(_pObject->getObjectType())
        {
        case BaseObject::eActor:
            {
                const Actor* pActor = static_cast<const Actor*>(_pObject);
                const SubSceneActor* pSSA = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
                if(pSSA)
                    pSSA->getSceneRelativePath().getString(relativePath);
                else
                    pActor->getLua().getString(relativePath);
            }
            break;

        case BaseObject::eFrise:
            static_cast<const Frise*>(_pObject)->getConfigNamePath().getString(relativePath);
            break;

        case BaseObject::eScene:
            static_cast<const Scene*>(_pObject)->getPath().getString(relativePath);
            break;

        default:
            return bfalse;
            break;
        }

        if(FILEMANAGER->TranslatePath(_fullPath, relativePath))
        {
            _fullPath.replace('/', '\\');
            return btrue;
        }
        return bfalse;
    }

    void Editor::setAppTitleScene(const Scene* _pScene)const
    {
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        String text;
        if(_pScene)
            text.setStringFormat("map:%s",FilePath::getFilenameWithoutExtension(_pScene->getPath().getString()).cStr());
        SYSTEM_ADAPTER->addTextToTitle(text);
#endif //ITF_WINDOWS
    }



void Editor::onPickingShapeDeleted(PickingShape* _pick)
{
    i32 index = m_selection.find(_pick);
    if (index >= 0)
        m_selection.eraseNoOrder(index);

    if(_pick->getOwner() == m_rotationScale_focus)
        endRotationScaleHelperMode();

    if(getLastClickedShape() == _pick)
        setLastClickedShape(NULL);
}


ObjectRef Editor::getFirstSelectedObjRef() const
{
    if(getSelectionSize() > 0)
        return getSelectionAt(0)->getOwner();
    return ITF_INVALID_OBJREF;
}

void Editor::chooseGridUnit()
{
#if defined(ITF_WINDOWS)
    String textValue;
    textValue.setTextFormat("%f", m_gridSize);
    if (!GUI_TextInput::inputText("Grid Unit", "Edit the current grid unit :", textValue))
        return;

    f32 input = textValue.atof32();
    input = std::max(0.01f, input);
    m_gridSize = input;
#endif // ITF_WINDOWS
}

void Editor::setMusicTheme(const StringID& _theme)
{
#if defined(ITF_WINDOWS)
    Scene * pScene = EDITOR->getEditedScene();
    if ( pScene != NULL && _theme != pScene->getMusicTheme() )
    {
	    pScene->setMusicTheme(_theme);
        setSceneAsModified(pScene);
    }
#endif // ITF_WINDOWS
}

void Editor::MuteMusic(bbool _mute)
{
#if defined(ITF_WINDOWS)
    MUSICMANAGER->muteAll(_mute);
#endif // ITF_WINDOWS
}

void Editor::chooseAngleStep()
{
#if defined(ITF_WINDOWS)
    String textValue;
    textValue.setTextFormat("%f", m_angleStepDegree);
    if (!GUI_TextInput::inputText("Angle step", "Edit the current angle step :", textValue))
        return;
    
    f32 input = textValue.atof32();
    input = std::max(1.0f, input);
    input = std::min(359.0f, input);
    m_angleStepDegree = input;
#endif // ITF_WINDOWS
}

bbool Editor::snapAngle( f32& angle ) const
{
    if (!INPUT_ADAPTER->isKeyPressed(KEY_LALT))
    {
        const f32 angleStep = getAngleStepDegree() * MTH_DEGTORAD;
        const f32 div = floorf((angle / angleStep) + 0.5f);
        angle = div * angleStep;

        return btrue;
    }
    return bfalse;
}


void   Editor::requestMouseFeedback(CursorIcon::Type _type)
{
    m_feedbackRequest = _type; 
    m_feedbackRequestFrame = CURRENTFRAME;
}


bbool Editor::copyFileToDataFolder( const String& _absoluteFilePath, Path& _newRelativeFilePath ) const
{
#if defined(ITF_WINDOWS)
    String dest;
    if(FILEMANAGER->folderChooser("Choose destination folder in databank", DATA_FOLDER, dest))
    {
        const String filename       = FilePath::getFilename(_absoluteFilePath);
        const String destination    = DATA_FOLDER + dest + "/" + filename;
        if(::CopyFile(LPCTSTR(_absoluteFilePath.cStr()), LPCTSTR(destination.cStr()), FALSE))
        {
            _newRelativeFilePath = destination;
            return btrue;
        }
    }
#endif // ITF_WINDOWS

    return bfalse;
}

void Editor::toggleMeasureMode()
{
    if(getState() == EditorState_EditMode_Measure)
    {
        setState(EditorState_EditMode);
    }
    else
    {
        if(getState() == EditorState_Idle)
            enterEditMode();

        setState(EditorState_EditMode_Measure);
    }
}

bbool Editor::snapOnMeasurePoint(Vec3d& _pos)const
{
    for(u32 i = 0; i < m_MeasureLines.size(); ++i)
    {
        const Vec3d& measurePos = m_MeasureLines[i];

        Vec3d pos2D, measurePos2D;
        GFX_ADAPTER->compute3DTo2D(_pos, pos2D);
        GFX_ADAPTER->compute3DTo2D(measurePos, measurePos2D);

        const f32 fDistScreen = (pos2D - measurePos2D).norm();

        if(fDistScreen < getSnappingDistanceInPixel())
        {
            _pos = measurePos;
            return btrue;
        }
    }

    return bfalse;
}

void Editor::swapCurrentWorldWith( ObjectRef _worldRef, TabEditInfo* _infos )
{
    const bbool preventCamModification = INPUT_ADAPTER->isKeyPressed(KEY_LCTRL);

    preSwapCurrentWorld(preventCamModification);

    SafeArray<u32> activatedPlayersBeforeSwap;
    for (u32 iPlayer = 0; iPlayer < GAMEMANAGER->getMaxPlayerCount(); iPlayer++)
    {
        if (const Actor * actor = GAMEMANAGER->getActivePlayer(iPlayer))
            activatedPlayersBeforeSwap.push_back(iPlayer);
    }

    WORLD_MANAGER->setAutoUnloadPreviousWorld(bfalse);
    GAMEMANAGER->swapToWorld(_worldRef);
    WORLD_MANAGER->switchToWorld(_worldRef);
    WORLD_MANAGER->setAutoUnloadPreviousWorld(btrue);

    World* pWorld = static_cast<World*>(_worldRef.getObject());
    ITF_ASSERT(pWorld);
    pWorld->stopPrefetch();

    {
        GAMEMANAGER->resetPlayers();

        for (u32 iPlayer = 0; iPlayer < activatedPlayersBeforeSwap.size(); iPlayer++)
            GAMEMANAGER->activatePlayer(activatedPlayersBeforeSwap[iPlayer]);

        // Apply backed tab info
        if(_infos && _infos->m_initialized)
        {
            for (u32 iPlayer = 0; iPlayer < activatedPlayersBeforeSwap.size(); iPlayer++)
            {
                if(Actor* pActor = GAMEMANAGER->getActivePlayer(iPlayer))
                {
                    if(iPlayer < _infos->m_playersPos.size())
                    {
                        const Vec3d& pos = _infos->m_playersPos[iPlayer];

                        GAMEMANAGER->teleportPlayer(iPlayer, pos.truncateTo2D(),pos.m_z, btrue);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "Players saved pos issue");
                    }
                }
            }

            if(!preventCamModification)
            {
                CAMERACONTROLLERMANAGER->teleport(_infos->m_camPos);
                CAMERA->setMode((Camera::Mode)_infos->m_camMode);
                CAMERA->forcePosition(_infos->m_camPos);
            }

            UNDOREDO_MANAGER->applyWorldData(_infos->m_undoRedoData);
            if(_infos->m_currentCheckpoint.isValid())
            GAMEMANAGER->setCurrentCheckpoint(_infos->m_currentCheckpoint);

            if(_infos->m_forcedWorkingSubScene.isValid())
            {
                setWorkingSubscene(static_cast<SubSceneActor*>(_infos->m_forcedWorkingSubScene.getObject()));
            }
            else if(_infos->m_subsceneCurrentlySubEdited.isValid())
            {
                if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(_infos->m_subsceneCurrentlySubEdited.getObject()))
                {
                    ObjectRefList emptyList;
                    startToEditSubScene(pSSA, emptyList);
                }
            }

            m_modifiedSceneRefList = _infos->m_editedScenes;
        }
        else
        {
            GAMEMANAGER->teleportToFirstCheckpoint(bfalse);

            if(const Actor* pCheckpoint = GAMEMANAGER->getFirstCheckpoint(_worldRef))
                CAMERACONTROLLERMANAGER->teleport(pCheckpoint->getPos());
        }
    }
}

void Editor::onDeleteWorld( World* _pWorld )
{
    m_pickablesForPauseCellUpdate.clear();
    Vector<TabEditInfo>::iterator it = std::find(m_tabList.begin(), m_tabList.end(), _pWorld->getUniqueName());

    if(it != m_tabList.end())
    {
        m_tabList.erase(it);
        setTabMainScenesModified();
    }
}

void Editor::swapTab(bbool _next)
{
    if(m_tabList.size() <= 1)
        return;

    World* pCurrentWorld = CURRENTWORLD;
    
    const u32 count = m_menuWorlds ? m_menuWorlds->m_subMenu.getItemsCount() : 0;
    const u32 notSceneCount = count - m_tabList.size();
    const u32 offset = _next ? 1 : count - notSceneCount - 1;
    for(u32 i = notSceneCount; i < count; ++i)
    {
        ContextMenuItem* pItem = m_menuWorlds->m_subMenu.getItemAt(i);
        if(!pItem)
            continue;

        if(StringID(pItem->m_label) == pCurrentWorld->getUniqueName())
        {
            const u32 nextIndex = notSceneCount + (((i - notSceneCount) + offset) % (count - notSceneCount));

            if(nextIndex != i)
            {
                ContextMenuItem * pNextItem = m_menuWorlds->m_subMenu.getItemAt(nextIndex);
                /*bbool result = */handleContextMenuItemClicked(pNextItem, ITF_INVALID_OBJREF);
            }
        }
    }
}

void Editor::switchDebugCellsMode()
{
    if (m_debugCellsCamMode)
    {
        m_debugCellsCamMode = bfalse;
        CAMERA->forcePosition(m_debugCellsCamPos);
        return;
    }
    m_debugCellsCamMode = btrue;
    m_debugCellsCamPos = CAMERA->getPos();
    m_debugCellsRestorePos = m_debugCellsCamPos;
}

void Editor::onStartDrawProcess()
{
    if (!m_debugCellsCamMode ||!CURRENTWORLD)
        return;

    const BaseObjectList& obj =  CURRENTWORLD->getActiveObjects();
    ID_SERVER->getObjectListNoNullV2(obj);
    SafeArray<BaseObject*>& pObjs = obj.m_solvedPointers;
    String str;
    str.setTextFormat("Active Objects count: %d", pObjs.size());
    GFX_ADAPTER->drawDBGText(str); 
    for (u32 i = 0; i<pObjs.size(); i++) 
    {
        Pickable* pObj = static_cast<Pickable*>(pObjs[i]);
        if (pObj)
        {
            if (pObj->getObjectType() == BaseObject::eFrise)
            {
                GFX_ADAPTER->drawDBGCircle(pObj->getPos(), 1.f, 0, 1, 1, 0, 4);
                GFX_ADAPTER->drawDBGAABB(pObj->getAABB(), 0, 1, 1, 0, 1, pObj->getDepth());
            }
            else
            {
                GFX_ADAPTER->drawDBGCircle(pObj->getPos(), 1.f, 1, 1, 0, 0, 4);
                GFX_ADAPTER->drawDBGAABB(pObj->getAABB(), 1, 1, 0, 0, 1, pObj->getDepth());
            }
        }
    }
    CAMERA->forcePosition(m_debugCellsCamPos);
}

void Editor::onEndDrawProcess()
{
    if (!m_debugCellsCamMode)
        return;
    CAMERA->forcePosition(m_debugCellsRestorePos);
}

u32 Editor::findDuplicateFriendly( World* _context, PickableList& _duplicates )
{
    for(u32 iScene = 0; iScene < _context->getSceneCount(); ++iScene)
    {
        const Scene* pScene = _context->getSceneAt(iScene);

        Vector<String8> takenNameList;
        const PickableList& pickables = pScene->getPickableList();
        for(u32 iObj = 0; iObj < pickables.size(); ++iObj)
        {
            Pickable* pObj = pickables[iObj];

            if(std::find(takenNameList.begin(), takenNameList.end(), pObj->getUserFriendly()) != takenNameList.end())
                _duplicates.push_back(pObj);
            
            takenNameList.push_back(pObj->getUserFriendly());
        }
    }

    return _duplicates.size();
}

void Editor::renameDuplicateUserFriendly( World* _context )
{
    PickableList pickablesToRename;
    const u32 renammeCount = findDuplicateFriendly(_context, pickablesToRename);
    for(u32 i = 0; i < renammeCount; ++i)
    {
        Pickable* pObj = pickablesToRename[i];

        const String8 oldFriendly = pObj->getUserFriendly();
        pObj->generateNewUserFriendly();

#if !defined(ITF_DISABLE_WARNING)
        if(DEBUGINFO)
        {
            String userInfo;
            userInfo.setTextFormat("Auto renaming duplicate userfliendly : '%s' to '%s'", oldFriendly.cStr(), pObj->getUserFriendly().cStr());
            DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
        }
#endif // !ITF_DISABLE_WARNING
    }
}

void Editor::toggleActorIconDisplay()
{
    m_usePluginsInfoFlags = 0xffffffff;
    EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_HideAll);
    if (!EditorDraw::pickableDetail_canShowAnyDetail())
        EditorDraw::cyclePickableDetailModes();
    ActorEditor::getInstance()->switchActorInfoFlag(ActorEditor::ActorInfoFlag_Anchors);
}

void Editor::toggleFriezePivotDisplay()
{
    m_usePluginsInfoFlags = 0xffffffff;
    EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_HideAll);
    if (!EditorDraw::pickableDetail_canShowAnyDetail())
        EditorDraw::cyclePickableDetailModes();
    FriezeEditor::getInstance()->setShowInfoMode(FriezeEditor::ShowInfoMode_MainAnchorOnly);
}

void Editor::toggleFriezePointsDisplay()
{
    m_usePluginsInfoFlags = 0xffffffff;
    EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_HideAll);
    if (!EditorDraw::pickableDetail_canShowAnyDetail())
        EditorDraw::cyclePickableDetailModes();
    FriezeEditor::getInstance()->setShowInfoMode(FriezeEditor::ShowInfoMode_SubAnchorsAndLines);
}

void Editor::toggleFriezeCollisionsDisplay()
{
    m_usePluginsInfoFlags = 0xffffffff;
    EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_HideAll);
    if (!EditorDraw::pickableDetail_canShowAnyDetail())
        EditorDraw::cyclePickableDetailModes();
    FriezeEditor::getInstance()->setShowInfoMode(FriezeEditor::ShowInfoMode_LinesOnly);
}

void Editor::pushPause()
{
    if (m_pausedByUser)
        return;

    if (!CHEATMANAGER->getPause())
    {
        DEBUGINFO->addDebugInfoDisplay("Editor pause", 1.0f);
        CHEATMANAGER->setPause(btrue);
    }
    m_pauseCount++;
}


void Editor::popPause()
{
    if (m_pauseCount > 0)
    {
        m_pauseCount--;
        if (0 == m_pauseCount && !m_pausedByUser)
        {
            CHEATMANAGER->setPause(bfalse);
        }
    }
}

void Editor::resetPause()
{
    m_pauseCount = 0;
    CHEATMANAGER->setPause(bfalse);
}

void Editor::toggleSubSceneContentShapesDisplay()
{
    m_showSubsceneContentShapes = !m_showSubsceneContentShapes;

    if (DEBUGINFO)
    {
        String text("Show all sub content by default: ");
        if(m_showSubsceneContentShapes)
            text += "ENABLED";
        else
            text += "DISABLED";

        DEBUGINFO->addDebugInfoDisplay(text, 5.0f);
    }
}

void Editor::changeAutoPauseMode()
{
    if (m_autoPause)
    {
        m_autoPause = bfalse;
        DEBUGINFO->addDebugInfoDisplay("switched auto-pause off", 3.0f);
    }
    else
    {
        m_autoPause = btrue;
        DEBUGINFO->addDebugInfoDisplay("switched auto-pause on", 3.0f);
    }
}

void Editor::setAutoPauseMode( bbool _autoPause )
{
    if (_autoPause == m_autoPause)
        return;
    changeAutoPauseMode();
}

void Editor::ignoreStorePickableInitData( Pickable *_pick )
{
    if (!_pick)
        return;

    ObjectRef pickRef = _pick->getRef();
    if (m_ignoreStorePickableInitDataList.find(pickRef) != -1)
        return;

    m_ignoreStorePickableInitDataList.push_back(pickRef);
}

void Editor::allowStorePickableInitData( Pickable *_pick )
{
    if (!_pick)
        return;

    ObjectRef pickRef = _pick->getRef();
    i32 pickPos = m_ignoreStorePickableInitDataList.find(pickRef);
    if (pickPos == -1)
        return;

    m_ignoreStorePickableInitDataList.eraseNoOrder(pickPos);
}

ITF::bbool Editor::isAllowStorePickableInitData( Pickable *_pick )
{
    if (!_pick || !m_storePickableInitData)
        return bfalse;

    return m_ignoreStorePickableInitDataList.find(_pick->getRef()) == -1;
}

void Editor::storePickableCurrentToInitData( Pickable *_pick )
{
    if (!isAllowStorePickableInitData(_pick))
        return;

    _pick->storeCurrentToInitial();
}

f32 Editor::toggleWindowRatio()
{
    const u32 width = GFX_ADAPTER->getScreenWidth();
    u32 height      = GFX_ADAPTER->getScreenHeight();
    f32 ratio       = f32(width) / f32(height);

#ifdef ITF_WINDOWS
    if(!SYSTEM_ADAPTER->isFullScreenMode())
    {
        String userInfo;
        const f32 ratio43       = 4.0f / 3.0f;
        const f32 ratio169      = 16.0f / 9.0f;
        const f32 ratioLimit    = (ratio43 + ratio169) / 2.0f;

        if(ratio < ratioLimit)
        {
            ratio = ratio169;
            userInfo = "New ratio: 16/9";
        }
        else
        {
            ratio = ratio43;
            userInfo = "New ratio: 4/3";
        }

        height = u32(f32(width) / ratio);

        SYSTEM_ADAPTER->setSize(width, height);

        if (DEBUGINFO)
        {
            DEBUGINFO->addDebugInfoDisplay(userInfo, 1.0f);
        }
    }
#endif  //ITF_WINDOWS
    
    return ratio;
}

void Editor::setForcedWorkingDepth( const DepthRange& _dr )
{
    unselectAll();
    m_forcedWorkingDepth = _dr;
}

void Editor::resetForcedWorkingDepth()
{
    m_forcedWorkingDepth = DepthRange::InvalidDepthRange;
}

DepthRange Editor::getWorkingDepth() const
{
    if(isWorkingDepthForced())
        return getForcedWorkingDepth();

    if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(getEditedSubSceneHolder().getObject()))
        return DepthRange(pSSA->getDepth());

    return DepthRange(0.0f);
}

void Editor::processPickableToRefresh()
{   
    for (u32 i = 0; i < m_pickablesToRefresh.size(); i++)
    {
        Pickable* pPick = (Pickable*)(m_pickablesToRefresh[i].getObject());
        if (pPick)
        {
            pPick->resetCellUpdateInfo();
            pPick->onEditorMove(isAllowStorePickableInitData(pPick));
            PLUGINGATEWAY->onObjectChanged(pPick);
        }
    }
    m_pickablesToRefresh.clear();
}


void Editor::setJoingroupListTarget(SubSceneComponent* _pScene)
{
    ObjectRef ref = _pScene->GetActor()->getRef();

    for (u32 i = 0; i < m_joingroupList.size(); i++)
    {
        if (m_joingroupList[i] == ref)
        {
            m_joingroupList.clear();
            return;
        }
    }
    m_joinGroupDest = _pScene;
}

void Editor::processJoinGroupList()
{
    if (!m_joingroupList.size())
        return;
    if (m_joinGroupDest && m_joinGroupDest->GetActor()->getScene())
    {
        groupSelection(m_joingroupList, m_joinGroupDest->GetActor()->getScene(), (SubSceneActor*)m_joinGroupDest->GetActor());
        m_joingroupList.clear();
        m_joinGroupDest = NULL;
        return;
    }

    f32 red = 228.f/255.f;
    f32 green = 154.f/255.f;
    f32 blue = 182.f/255.f;
    const PickingShape* pPS = getPickingShapeOfObjectUnderMouse();
    if (pPS)
    {
        BaseObject* owner = pPS->getOwnerPtr();
        if (owner && owner->getObjectType() == BaseObject::eActor)
        {
            Actor* pActor = (Actor*)owner;
            if (SubSceneActor * ssa = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                if(allowEditSubScene(ssa))
                {
                    if (m_joingroupList.find(pActor->getRef()) < 0)
                    {
                        red *= 0.25f;
                        blue *= 0.25f;
                    }
                }
            }
        }
    }

/*
    for (u32 i = 0; i < m_joingroupList.size(); i++)
    {
        Pickable* pPick = (Pickable*)(m_joingroupList[i].getObject());
        if (pPick)
            GFX_ADAPTER->drawArrow(pPick->getAnchorPos(), getMouse3d(), 228.f/255.f, 154.f/255.f, 182.f/255.f, 0.1f);
    }*/
   // Alternate rendering
    u32 count = 0;
    Vec3d center(Vec3d::Zero);
    for (u32 i = 0; i < m_joingroupList.size(); i++)
    {
        Pickable* pPick = (Pickable*)(m_joingroupList[i].getObject());
        if (pPick)
        {
            center += pPick->getAnchorPos();
            count++;
        }
    }

    if (count)
    {
        center = center / (f32)count;
        Vec3d vector = getMouse3d() - center;
        f32 len = vector.norm();
        vector.normalize();
        Vec3d joinPoint = center + vector * 0.75f * len;
        Vec3d proj;
        GFX_ADAPTER->compute3DTo2D(joinPoint, proj);
        if (proj.m_z < 0.f || proj.m_z > 1.f)
            return;
        for (u32 i = 0; i < m_joingroupList.size(); i++)
        {
            Pickable* pPick = (Pickable*)(m_joingroupList[i].getObject());
            if (pPick)
            {
                GFX_ADAPTER->compute3DTo2D(pPick->getAnchorPos(), proj);
                if (proj.m_z < 0.f || proj.m_z > 1.f)
                    continue;
                GFX_ADAPTER->drawDBG3DLine(pPick->getAnchorPos(), joinPoint, red, green, blue, 0.f, 3.f);
            }
        }

        GFX_ADAPTER->drawDBG3DLine(joinPoint, getMouse3d(), red, green, blue, 0.f, 3.f);
    }
}

void Editor::addMultiFilter(const SMultiFilter& _filter)
{
    m_multiFilter.push_back(_filter);
    String text("new filter:");
    switch(_filter.m_op)
    {
    case SMultiFilter::FilterOp_includeFriendly:
        text += " include friendly ";
        break;

    case SMultiFilter::FilterOp_excludeFriendly:
        text += " exclude friendly ";
        break;

    case SMultiFilter::FilterOp_includePath:
        text += " include path ";
        break;

    case SMultiFilter::FilterOp_excludePath:
        text += " exclude path ";
        break;

    case SMultiFilter::FilterOp_includeComponent:
        text += " include component  !!! NOT IMPLEMENTED !!!";
        break;

    case SMultiFilter::FilterOp_excludeComponent:
        text += " exclude component  !!! NOT IMPLEMENTED !!! ";
        break;

    case SMultiFilter::FilterOp_includeZ:
        text += " include Z ";
        break;

    case SMultiFilter::FilterOp_excludeZ:
        text += " exclude Z ";
        break;

    case SMultiFilter::FilterOp_includeTarget:
        text += " include objects ignored on ";
        break;

    case SMultiFilter::FilterOp_excludeTarget:
        text += " exclude objects ignored on ";
        break;

    case SMultiFilter::FilterOp_includeCellLayer:
        text += " include cell layer ";
        break;

    case SMultiFilter::FilterOp_excludeCellLayer:
        text += " exclude cell layer ";
        break;

    default:
        ITF_ASSERT_MSG(bfalse, "undocumented");
        break;
    }

    switch(_filter.m_op)
    {
    case SMultiFilter::FilterOp_includeFriendly:
    case SMultiFilter::FilterOp_excludeFriendly:
    case SMultiFilter::FilterOp_includePath:
    case SMultiFilter::FilterOp_excludePath:
    case SMultiFilter::FilterOp_includeComponent:
    case SMultiFilter::FilterOp_excludeComponent:
    case SMultiFilter::FilterOp_includeTarget:
    case SMultiFilter::FilterOp_excludeTarget:
        text += _filter.m_label;
        break;

    case SMultiFilter::FilterOp_includeZ:
    case SMultiFilter::FilterOp_excludeZ:
        switch (_filter.m_arithmeticOp)
        {
        case SMultiFilter::Op_Bigger:
            text += "> ";
            break;
        case SMultiFilter::Op_Lower:
            text += "< ";
            break;
        case SMultiFilter::Op_Equals:
            text += "= ";
            break;
        default:
            ITF_ASSERT_MSG(bfalse, "undocumented");
            break;
        }
        char buf[32];
        memset(buf, 0, 32);
        SPRINTF_S(buf, 31, "%.2f", _filter.m_arithmeticArg);
        text += buf;
        break;

    default:
        ITF_ASSERT_MSG(bfalse, "undocumented");
        break;
    }

    DEBUGINFO->addDebugInfoDisplay(text, 5.0f);
}

bbool Editor::sceneHasBeenModified( const Scene* _pScene ) const
{
    if(_pScene)
        return m_modifiedSceneRefList.find(_pScene->getRef()) != -1;
    return bfalse;
}

void Editor::setSceneAsModified( const Scene* _pScene )
{
    if(_pScene)
    {
        // If it's a group, edit it's first instance ancestor scene
        if(SubSceneActor* pSSA = _pScene->getSubSceneActorHolder())
        {
            if(pSSA->getEmbedScene())
            {
                setSceneAsModified(pSSA->getScene());
                return;
            }
        }

        ObjectRef sceneRef = _pScene->getRef();
        if(m_modifiedSceneRefList.find(sceneRef) == -1)         // Prevent duplicate informations
        {
            m_modifiedSceneRefList.push_back(sceneRef);
            
#if defined(DEVELOPER_OLIV)
            if (DEBUGINFO)
            {
                String userInfo;
                userInfo.setTextFormat("Scene::setSceneAsModified('%ls')", _pScene->getPath().getString().cStr());
                DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
            }
#endif // DEVELOPER_OLIV
        }
    }
}

void Editor::preUndoRedoOperation()
{
    cancelEditActions();
}

void Editor::postUndoRedoOperation()
{
}

bbool Editor::getTabEditInfo( TabEditInfo& _infos)
{
    Vector<TabEditInfo>::iterator it = std::find(m_tabList.begin(), m_tabList.end(), _infos.m_worldUniqueName);

    if(it != m_tabList.end())
    {
        _infos = *it;
        return btrue;
    }

    return bfalse;
}

bbool Editor::updateTabEditInfo( TabEditInfo& _infos )
{
    Vector<TabEditInfo>::iterator it = std::find(m_tabList.begin(), m_tabList.end(), _infos.m_worldUniqueName);

    if(it != m_tabList.end())
    {
        *it = _infos;
        return btrue;
    }

    return bfalse;
}

void Editor::onSceneAsyncLoadFinished( const Scene* _pScene )
{
    if(World* pWorld = _pScene->getWorld())
    {
        const i32 index = m_tabLoadingPendingList.find(pWorld->getRef());
        if(index != -1)
        {
            ObjectRef owner = m_tabOwnerPendingList[index];

            m_tabLoadingPendingList.eraseNoOrder(index);
            m_tabOwnerPendingList.eraseNoOrder(index);

            pWorld->prefetchResource();

            // Add it to the available tab list
            m_tabList.push_back(TabEditInfo());
            TabEditInfo& infos = m_tabList.back();

            infos.m_worldUniqueName = pWorld->getUniqueName();
            infos.m_owner = owner;
        }
    }
}

bbool Editor::loadTabInBackground( const Path& _scenePath, World* _ownerWorld )
{
    String path;
    
    _scenePath.getString(path);

    if(FILEMANAGER->fileExists(path))
    {
        LoadInfo infos;
        infos.m_bAsyncLoad = btrue;
        infos.m_mapPath = path;
        infos.m_disconnectFromCells = bfalse;

        m_tabLoadingPendingList.push_back(WORLD_MANAGER->createAndLoadWorld(infos));

        ObjectRef ownerRef;
        if(_ownerWorld)
            ownerRef = _ownerWorld->getRef();
        m_tabOwnerPendingList.push_back(ownerRef);

        return btrue;
    }
    return bfalse;
}

void Editor::fillAssociatedTabList( Scene* _pScene ) const
{
#ifndef ITF_CONSOLE
    Vector<Path>& paths = _pScene->getAssociatedTabs();
    paths.clear();

    PathString_t pathName;
    _pScene->getPath().getString(pathName);
    const String sceneExtension = FilePath::getExtension(pathName);

    // Only root scenes
    if(!_pScene->getSubSceneActorHolder() && sceneExtension == "isc")
    {
        for(u32 iTab = 0; iTab < EDITOR->getTabsCount(); ++iTab)
        {
            const TabEditInfo& infos = EDITOR->getTabAt(iTab);

            if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(infos.m_worldUniqueName))
            {
                const Path& tabPath         = pWorld->getRootScene()->getPath();
                tabPath.getString(pathName);
                const String tabExtension   = FilePath::getExtension(pathName);

                if(tabExtension == "bsc")
                    paths.push_back(tabPath);
            }
        }
    }
#endif // !ITF_CONSOLE
}

void Editor::setTabMainScenesModified()
{
    const u32 tabCount = EDITOR->getTabsCount();
    PathString_t pathName;
    for(u32 iTab = 0; iTab < tabCount; ++iTab)
    {
        TabEditInfo infos = EDITOR->getTabAt(iTab);

        if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(infos.m_worldUniqueName))
        {
            Scene* pScene = pWorld->getRootScene();

            pScene->getPath().getString(pathName);
            const String extension = FilePath::getExtension(pathName);

            if(extension == "isc" && !pScene->getSubSceneActorHolder() && infos.m_editedScenes.find(pScene->getRef()) == -1)
            {
                infos.m_editedScenes.push_back(pScene->getRef());
            }
        }
    }

    if(tabCount)
    {
        if(World* pWorld = CURRENTWORLD)
        {
            Scene* pScene = pWorld->getRootScene();
            pScene->getPath().getString(pathName);
            const String extension = FilePath::getExtension(pathName);

            if(extension == "isc" && !pScene->getSubSceneActorHolder())
                setSceneAsModified(pScene);
        }
    }
}

void Editor::newScene()
{
    String scenePath("scene.isc");
    if(FILEMANAGER->fileChooser(btrue, scenePath, L"All supported files (*.isc;*.bsc;*.tsc)\0*.isc;*.bsc;*.tsc;\0Scene (*.isc)\0*.isc\0Bank scene (*.bsc)\0*.bsc\0Subscene (*.tsc)\0*.tsc\0"))
    {
        const String absolutePath = scenePath;

        String relativePath;
        if (!FILESERVER->getPathFromDataFolder(relativePath, absolutePath))
            return;

        const bbool bFileExistBeforeSave = FILEMANAGER->fileExists(absolutePath);
        if(SOURCECONTROL_ADAPTER && bFileExistBeforeSave)
            SOURCECONTROL_ADAPTER->checkout(absolutePath);

        Scene scene;
        if(XMLAll::saveScene(absolutePath, &scene, bfalse))
        {
            if(SOURCECONTROL_ADAPTER && !bFileExistBeforeSave)
                SOURCECONTROL_ADAPTER->add(scenePath);

            setPendingDragAndDropMap(relativePath);
        }
        else
        {
            ITF_ERROR("Error while saving the new scene at: %ls", absolutePath.cStr());
        }
    }
}

void Editor::setWorkingSubscene( const SubSceneActor* pSSA )
{
    m_forcedWorkingSubScene.invalidate();

    if(getEditedSubSceneHolder().isValid())
        endSubSceneEdition();

    if(pSSA)
    {
        m_forcedWorkingSubScene = pSSA->getRef();

        ObjectRefList list;
        startToEditSubScene(pSSA, list);
    }
}

bbool Editor::isContextMenuVisible()
{
    return (isContextMenuShowable() || (isTopBarMenuShowable() && !(m_topBarMenu.isFolded()) )); 
}


void Editor::setContextMenuShowable(bbool _show) 
{
    EditorContextMenu::ContextMenuInfo* info = EditorContextMenu::getMenuInfo(&m_contextMenu[0]);
    info->m_allowDraw = _show;
}

void Editor::setTopBarMenuShowable(bbool _show) 
{
    EditorContextMenu::ContextMenuInfo* info = EditorContextMenu::getMenuInfo(&m_topBarMenu);
    info->m_allowDraw = _show;
}

bbool   Editor::isContextMenuShowable()
{
    return EditorContextMenu::getMenuInfo(&m_contextMenu[0])->m_allowDraw;
}

bbool   Editor::isTopBarMenuShowable()
{
    return EditorContextMenu::getMenuInfo(&m_topBarMenu)->m_allowDraw;
}

void Editor::cancelEditActions()
{
    if(isDragActive())
        endDrag();

    if(zoneSelectionInProgress())
        endZoneSelection();

    if(isRotationActive())
        endRotation();

    if(isInRotationScaleHelperMode())
        endRotationScaleHelperMode();
}

Scene* Editor::getFirstObjectSelectedScene() const
{
    if(getSelectionSize())
    {
        const PickingShape* pShape    = getSelectionAt(0);
        ITF_ASSERT(pShape);
        const Pickable* pOwner        = static_cast<const Pickable*>(pShape->getOwnerPtr());
        ITF_ASSERT(pOwner);

        return pOwner ? pOwner->getScene() : NULL;
    }

    return NULL;
}

void Editor::setOrphanTabsOwner( World* _owner )
{
    PathString_t pathName;
    for(u32 iTab = 0; iTab < getTabsCount(); ++iTab)
    {
        TabEditInfo infos = getTabAt(iTab);

        if(infos.m_owner.isValid())
            continue;

        if(World* pWorld = WORLD_MANAGER->getWorldFromUniqueName(infos.m_worldUniqueName))
        {
            const Path& tabPath         = pWorld->getRootScene()->getPath();
            tabPath.getString(pathName);
            const String tabExtension   = FilePath::getExtension(pathName);

            if(tabExtension == "bsc")
            {
                infos.m_owner = _owner->getRef();

                updateTabEditInfo(infos);
            }
        }
    }
}

void Editor::updateDebugInfo() const
{
    String info;

    World* currentWorld = CURRENTWORLD;
    Scene* rootScene = currentWorld ? currentWorld->getRootScene() : NULL;
    if(rootScene)
    {
        PathString_t pathName;
        rootScene->getPath().getString(pathName);
        info += "Current scene: ";
        info += pathName;
        info += "\n";
    }

    if (info.getLen() && DEBUGINFO)
        DEBUGINFO->setEditorInfo(info);
}

void Editor::newTabFromMenu()
{
    WORLD_MANAGER->setAutoUnloadPreviousWorld(bfalse);

    EDITOR->openScene(bfalse);
}

void Editor::preSwapCurrentWorld(bbool _preventCamModification)
{
    // Update current edit tab info before the change
    if(World* pCurrWorld = CURRENTWORLD)
    {
        TabEditInfo infos;
        infos.m_worldUniqueName = pCurrWorld->getUniqueName();
        if(getTabEditInfo(infos))
        {
            if(!infos.m_initialized || !_preventCamModification)
            {
                infos.m_camPos = CAMERA->getPos();
                infos.m_camMode = CAMERA->getMode();
            }

            infos.m_editedScenes = m_modifiedSceneRefList;

            // Get all position even disable players
            infos.m_playersPos.clear();
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
                if(Player* pPlayer = GAMEMANAGER->getPlayer(i))
                    if(Actor* pActor = pPlayer->getActor())
                        infos.m_playersPos.push_back(pActor->getPos());

            infos.m_currentCheckpoint.invalidate();
            if(Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint())
                SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint, infos.m_currentCheckpoint);

            UNDOREDO_MANAGER->fillWorldData(infos.m_undoRedoData);

            infos.m_subsceneCurrentlySubEdited = getEditedSubSceneHolder();
            infos.m_forcedWorkingSubScene = getWorkingSubScene();
            setWorkingSubscene(NULL);

            infos.m_initialized = btrue;
            updateTabEditInfo(infos);
        }
    }

    // Keep undo redo data by loaded world
    UNDOREDO_MANAGER->clear(bfalse);

    // unselect all and cancel current actions TODO improve
    unselectAll();
    PLUGINGATEWAY->cancelCurrentAction(bfalse);
    if(getEditedSubSceneHolder().isValid())
        endSubSceneEdition();
    cancelEditActions();
}

bbool Editor::allowEditSubScene( const SubSceneActor* _pSSA ) const
{
    bbool result = (_pSSA != NULL);

    if(_pSSA && !_pSSA->getEmbedScene())
    {
        PathString_t pathName;
        _pSSA->getSceneRelativePath().getString(pathName);
        const String ext = FilePath::getExtension(pathName);

        result = (ext != "tsc");
    }

    return result;
}

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
    static bbool objectFitsOneFilter(const Pickable& _pick, const SMultiFilter& _argFilter)
    {
        const SubSceneActor* isSubScene = _pick.DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if (isSubScene)
            return btrue;

        switch (_argFilter.m_op)
        {
        case SMultiFilter::FilterOp_includeFriendly:  // no break is intentional
        case SMultiFilter::FilterOp_excludeFriendly:
        {
            const String8& _filter = _argFilter.m_label;
            String8 path8(_pick.getUserFriendly());
            path8.toLower();
            if (strstr(path8.cStr(), _filter.cStr()))
                return  _argFilter.m_op == SMultiFilter::FilterOp_includeFriendly;
            return  _argFilter.m_op == SMultiFilter::FilterOp_excludeFriendly;
        }
        break;

        case SMultiFilter::FilterOp_includeTarget:  // no break is intentional
        case SMultiFilter::FilterOp_excludeTarget:
        {
            String filter = _argFilter.m_label;
            filter.toUpper();

            Scene* pScene = _pick.getScene();

            if(pScene->isFilteringObject(_pick.getUserFriendly(), filter))
                return  _argFilter.m_op == SMultiFilter::FilterOp_includeTarget;
            return  _argFilter.m_op == SMultiFilter::FilterOp_excludeTarget;
        }
        break;

        case SMultiFilter::FilterOp_includePath:
        case SMultiFilter::FilterOp_excludePath:
        {
            const String8& _filter = _argFilter.m_label;
            const Path* pickTemplate = _pick.getTemplatePath();
            if (!pickTemplate)
                return btrue;
            PathString_t pathString;
            pickTemplate->getString(pathString);
            StringConverter localPath(pathString);
            if (strstr(localPath.getChar(), _filter.cStr()))
                return  _argFilter.m_op == SMultiFilter::FilterOp_includePath;
            return  _argFilter.m_op == SMultiFilter::FilterOp_excludePath;
        }
        break;

        case SMultiFilter::FilterOp_includeCellLayer:
        case SMultiFilter::FilterOp_excludeCellLayer:
        {
            if (_pick.getLastCellUpdateData().m_layerIndex == _argFilter.m_arithmeticArg)
                return  _argFilter.m_op == SMultiFilter::FilterOp_includeCellLayer;
            return  _argFilter.m_op == SMultiFilter::FilterOp_excludeCellLayer;
        }
        break;

        case SMultiFilter::FilterOp_includeComponent:
        {
        }
        break;

        case SMultiFilter::FilterOp_excludeComponent :
        {
        }
        break;

        case SMultiFilter::FilterOp_includeZ: // no break is intentional
        case SMultiFilter::FilterOp_excludeZ :
        {
            switch(_argFilter.m_arithmeticOp)
            {
            case SMultiFilter::Op_Bigger:
                if (_pick.getDepth() > _argFilter.m_arithmeticArg)
                    return _argFilter.m_op == SMultiFilter::FilterOp_includeZ;
                return _argFilter.m_op != SMultiFilter::FilterOp_includeZ;
            case SMultiFilter::Op_Lower:
                if (_pick.getDepth() < _argFilter.m_arithmeticArg)
                    return _argFilter.m_op == SMultiFilter::FilterOp_includeZ;
                return _argFilter.m_op != SMultiFilter::FilterOp_includeZ;
            case SMultiFilter::Op_Equals:
                if (f32_Abs(_pick.getDepth() - _argFilter.m_arithmeticArg) < 0.1f)
                    return _argFilter.m_op == SMultiFilter::FilterOp_includeZ;
                return _argFilter.m_op != SMultiFilter::FilterOp_includeZ;
            }
        }
        break;

        default:
            ITF_ASSERT_MSG(bfalse, "unhandled filter op");
            return btrue;
        }
        return btrue;
    }



    static bbool objectFitsMultiFilter(const Pickable& _pick, const ITF_VECTOR<SMultiFilter>& filter)
    {
        const SubSceneActor* isSubScene = _pick.DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if (isSubScene)
            return btrue;

        if (!filter.size())
            return btrue;

        bbool oneFilterSatisfied = bfalse;
        bbool onlyExcludes = btrue;
        for (u32 iflt = 0; iflt < filter.size(); iflt++)
        {
            if (filter[iflt].m_isIncludeQuerry)
                onlyExcludes = bfalse;
            if (objectFitsOneFilter(_pick, filter[iflt]))
            {
                if (filter[iflt].m_isIncludeQuerry)
                    oneFilterSatisfied = btrue;
            }
            else 
            {
                if (!filter[iflt].m_isIncludeQuerry)
                    return bfalse;
            }
        }
        return oneFilterSatisfied || onlyExcludes;
    }

    bbool isHiddenBecauseOfEditor(const Pickable* _pick)
    {
        if (_pick->isSelected())
            return bfalse;
        if (_pick->getHiddenForEditorFlag())
            return btrue;

        // check if any ancestor scene is hidden
        if(Scene* pScene = _pick->getScene())
            if (pScene->getSubSceneActorHolder())
                if (isHiddenBecauseOfEditor(pScene->getSubSceneActorHolder()))
                    return btrue;

        if (!objectFitsMultiFilter(*_pick, EDITOR->getMultiFilter()))
            return btrue;
        if (!(EDITOR->getDisplayedFamilies() & (_pick->getObjectFamily() & 0xffff)))
            return btrue;
        if(EDITOR->isWorkingDepthForced() && EDITOR->getForcedWorkingDepth() != DepthRange(_pick->getDepth()))
            return btrue;

        return bfalse;
    }

    void ObjectGroup::update( f32 _elapsed )
    {
        u32 size = m_elements.size();
        if (EDITOR->isUpdateMultiFilter())
        {
            for (u32 i = 0; i < size; i++)
            {
                if (!isHiddenBecauseOfEditor(m_elements[i]))
                    m_elements[i]->update(_elapsed);
            }
        }
        else
        {
            for (u32 i = 0; i < size; i++)
                m_elements[i]->update(_elapsed);
        }
        m_elements.clear();
    }
    
    void ObjectGroup::draw(SafeArray<class LightComponent*>& _activeLights)
    {
        u32 numberLights = _activeLights.size();
        u32 size = m_elements.size();
        if (EDITOR->isUpdateMultiFilter())
        {
            for (u32 i = 0; i < size; i++)
            {
                Pickable* pObj = m_elements[i];
                if (numberLights && pObj->getObjectType() == BaseObject::eActor)
                {
                    const Actor* pAct = static_cast<const Actor*>(pObj);
                    AnimLightComponent *pAnimLight = pAct->GetComponent<AnimLightComponent>();
                    if (pAnimLight)
                    {
                        pAnimLight->clearLights();
                        for (u32 il = 0 ; il < numberLights ; il++)
                        {
                            LightComponent *light = _activeLights[il];
                            light->processLight(m_elements[i]);
                        }
                    }
                }
                pObj->draw();
            }
        }
        else
        {
            for (u32 i = 0; i < size; i++)
            {
                if (!isHiddenBecauseOfEditor(m_elements[i]))
                {
                    Pickable* pObj = m_elements[i];
                    if (numberLights && pObj->getObjectType() == BaseObject::eActor)
                    {
                        const Actor* pAct = static_cast<const Actor*>(pObj);
                        AnimLightComponent *pAnimLight = pAct->GetComponent<AnimLightComponent>();
                        if (pAnimLight)
                        {
                            pAnimLight->clearLights();
                            for (u32 il = 0 ; il < numberLights ; il++)
                            {
                                LightComponent *light = _activeLights[il];
                                light->processLight(m_elements[i]);
                            }
                        }
                    }
                    pObj->draw();
                }
            }
        }
        m_elements.clear();
    }
#endif


    void Editor::processRegisterInWorldCellsDuringPause()
    {
        if (!CHEATMANAGER->getPause())
            m_pickablesForPauseCellUpdate.clear();
        for (u32 i = 0; i < m_pickablesForPauseCellUpdate.size(); i++)
        {
            if (Pickable* pObj = (Pickable*)m_pickablesForPauseCellUpdate[i].getObject())
                pObj->registerInWorldCells();
        }
    }

    bbool Editor::isOfficialScene( const Scene& _scene )
    {
        String path;
        _scene.getPath().getString(path);
        path.truncate(6);

        return path == "world/";
    }

    void DumpAllFriezesResources()
    {
#if defined(ITF_SUPPORT_EDITOR)
        SafeArray<FriseConfig*> AllConfigs;
        AllConfigs.reserve(100);

        for (u32 iWorld = 0; iWorld < WORLD_MANAGER->getWorldCount(); iWorld++)
        {
            World* curWorld = WORLD_MANAGER->getWorldAt(iWorld);
            if (!curWorld)
                continue;

            for (u32 iScene = 0; iScene < curWorld->getSceneCount(); iScene++)
            {
                Scene* curScene = curWorld->getSceneAt(iScene);
                if (!curScene)
                    continue;
                
                const PickableList&  friezes = curScene->getFrises();
                for (u32 itFrieze = 0; itFrieze < friezes.size(); itFrieze++)
                {
                    Frise* pFrieze = static_cast<Frise*>(friezes[itFrieze]);
                    FriseConfig* pConfig = pFrieze->getConfig();
                    if (AllConfigs.find(pConfig) < 0)
                    {
                        AllConfigs.push_back(pConfig);
                    }
                }
            }
        }

        FILE* dump = fopen("FRIEZES_RESOURCE_DUMP.TXT", "wt");
        if (dump)
        {
            PathString8_t pathName;
            fprintf(dump, "--- Configs list: ---\n");
            for (u32 i = 0; i < AllConfigs.size(); i++)
            {
                fprintf(dump, "\t%s\n", AllConfigs[i]->getPath().cStr());
            }
            fprintf(dump, "\n\n--- Textures list: ---\n");
            for (u32 i = 0; i < AllConfigs.size(); i++)
            {
                const String8& path = AllConfigs[i]->getPath();
                ResourceGroup* grp = AllConfigs[i]->getGroup();
                const ITF_VECTOR<ResourceID>& resources = grp->getResourcesList();
                fprintf(dump, "\n\tFrom: %s\n", path.cStr());
                for (u32 j = 0; j < resources.size(); j++)
                {
                    const Resource* res = resources[j].getResource();
                    if (res)
                    {
                        res->getPath().getString(pathName);
                        fprintf(dump, "\t\t%s\n", pathName.cStr());
                    }
                }
            }
            fprintf(dump, "\nTirelipinpon on ze chiwawa\n");
            fclose(dump);
            SYSTEM_ADAPTER->messageBox("Done", "Data saved to FRIEZES_RESOURCE_DUMP.TXT" , ITF_MSG_OK, ITF_IDYES);
        }
        else
        {
            SYSTEM_ADAPTER->messageBox("Error", "can't create file FRIEZES_RESOURCE_DUMP.TXT" , ITF_MSG_ICONWARNING | ITF_MSG_OK, ITF_IDYES);
        }
#endif
    }

#if defined(ITF_SUPPORT_EDITOR)
    void Editor::onObjectDeleted(BaseObject* _pBO)
    {
        WORLD_MANAGER->getUpdateGroups()[0].removeElement(_pBO);
        WORLD_MANAGER->getUpdateGroups()[1].removeElement(_pBO);
        WORLD_MANAGER->getUpdate2DGroups()[0].removeElement(_pBO);
        WORLD_MANAGER->getUpdate2DGroups()[1].removeElement(_pBO);
    }
#endif
} // namespace ITF



#endif //ITF_CONSOLE_FINAL

