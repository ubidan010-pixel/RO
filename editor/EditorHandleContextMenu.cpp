#include "precompiled_editor.h"

#include <shellapi.h>

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_ANIMATIONSTATS_H_
#include "engine/animation/AnimationStats.h"
#endif //_ITF_ANIMATIONSTATS_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif //_ITF_FRIEZESTATS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_TEMPLATECOPIER_H_
#include "editor/TemplateCopier.h"
#endif // _ITF_TEMPLATECOPIER_H_

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _CELLLAYERSSETTINGS_H_
#include "engine/scene/CellsLayersSettings.h"
#endif // _CELLLAYERSSETTINGS_H_

#ifndef _ITF_PLUGIN_KATANACUT_H_
#include "tools/plugins/Plugin_KatanaCut/Plugin_KatanaCut.h"
#endif //_ITF_PLUGIN_KATANACUT_H_

#ifndef _ITF_ONLINETRACKING_PLUGIN_H
#include "tools/plugins/Plugin_OnlineTracking/OnlineTrackingPlugin.h"
#endif //_ITF_ONLINETRACKING_PLUGIN_H

#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_


namespace ITF
{

const char* platforms[] =
{
    /* do not add "PC" except if "PC" become different than a new one called "EDITOR" */
    "WII",
    "PS3",
    "X360",
    "IPAD",
    "CTR",
    "VITA",
    "PS5",
    "NX",
    "OUNCE"
};

static void setLeftMenuItemColor(ContextMenuItem* item)
{
    item->m_backR = 1.f;
    item->m_backG = 0.7f;
    item->m_backB = 0.7f;
}

void Editor::fillTopItemsContextMenu(ObjectRef _target)
{
    if (CHEATMANAGER->getPause())
    {
        m_contextMenu[0].addItem(EditorContextMenu::ItemId_Unpause, "unpause (p)");
    }
    u32 backId  =(u32)EditorContextMenu::ItemId_Back1;
    BaseObject* target = GETOBJECT(_target);
    PathString_t pathName;
    while (target && _target != EDITOR->getSubEditedItem())
    {
        if (target->getObjectType() == BaseObject::eActor || target->getObjectType() == BaseObject::eFrise)
        {
            String label = "Back to: scene ";
            Scene* pScene = ((Pickable*)target)->getScene();
            if (pScene)
            {
                pScene->getPath().getString(pathName);
                label += FilePath::getFilenameWithoutExtension(pathName);
            }
            ContextMenuItem* item = m_contextMenu[1].addItem((EditorContextMenu::ItemId)backId, label);
            setLeftMenuItemColor(item);
            backId++;
            if (backId >= (u32)EditorContextMenu::ItemId_Back8)
                break;
            if (pScene)
                item->m_userData = u64(pScene->getRef().getValue());
            else
                item->m_userData = U64_INVALID;
            target = pScene;
        }
        else
        {
            if (target->getObjectType() == BaseObject::eScene)
            {
                Scene* pScene = (Scene*)target;
                if (pScene->getSubSceneActorHolder())
                {
                    target = pScene->getSubSceneActorHolder()->getScene();
                    if (target)
                    {
                        String label = "Back to: scene ";
                        ((Scene*)target)->getPath().getString(pathName);
                        label += FilePath::getFilenameWithoutExtension(pathName);
                        ContextMenuItem* item = m_contextMenu[1].addItem((EditorContextMenu::ItemId)backId, label);
                        item->m_userData = u64(target->getRef().getValue());
                        setLeftMenuItemColor(item);

                        backId++;
                        if (backId >= (u32)EditorContextMenu::ItemId_Back8)
                            break;
                    }
                }
                else
                    break;
            }
            else
                break;
        }
    }
    if (_target == EDITOR->getSubEditedItem())
    {
        ContextMenuItem* item = m_contextMenu[1].addItem((EditorContextMenu::ItemId)backId, "Back");
        item->m_userData = u64(_target.getValue());
        setLeftMenuItemColor(item);
    }

    ContextMenuItem* item = m_contextMenu[1].addItem(EditorContextMenu::ItemId_Unselect, "Close");
    setLeftMenuItemColor(item);
}




void Editor::fillNormalContextMenu(ObjectRef _target)
{
    if(EDITOR->getSelectionSize() > 1)
    {
        ContextMenuItem* menu = m_contextMenu[0].addItem(EditorContextMenu::ItemId_AlignSelection, "Align objects on");
        menu->m_subMenu.addItem(EditorContextMenu::ItemId_AlignSelectionX, "X");
        menu->m_subMenu.addItem(EditorContextMenu::ItemId_AlignSelectionY, "Y");
        menu->m_subMenu.addItem(EditorContextMenu::ItemId_AlignSelectionZ, "Z");
    }

    ContextMenuItem* filter = NULL;
    ContextMenuItem* unfilter = NULL;

    static const u32 uSize = sizeof(platforms) / sizeof(platforms[0]);
    bbool filterByPlatform[uSize];
    bbool unfilterByPlatform[uSize];
    memset(filterByPlatform, 0, sizeof(bbool) * uSize);
    memset(unfilterByPlatform, 0, sizeof(bbool) * uSize);

    const ObjectRefList selection = getEditorSelectionAsObjRefList();
    SafeArray<BaseObject*> resolvedRefs;
    ID_SERVER->getObjectListNoNull(selection, resolvedRefs);

    for(u32 i = 0; i < uSize; ++i)
    {
        const String targetName = platforms[i];

        for(u32 iSel = 0; iSel < resolvedRefs.size(); ++iSel)
        {
            const Pickable* selectedObj = static_cast<const Pickable*>(resolvedRefs[iSel]);
            const Scene* pScene = selectedObj->getScene();

            ITF_ASSERT(pScene);
            if (!pScene)    // Happened July 13th, see RO-8068
                continue;

            if(pScene->isFilteringObject(selectedObj->getUserFriendly(), targetName))
            {
                if(!unfilter)
                    unfilter = m_contextMenu[0].addItem(EditorContextMenu::ItemId_UnfilterTarget, "Bring back for target");
                
                if(!unfilterByPlatform[i])
                {
                    ContextMenuItem* item = unfilter->m_subMenu.addItem(EditorContextMenu::ItemId_UnfilterTarget, targetName);
                    unfilterByPlatform[i] = btrue;
                    item->m_userData = u64(i);
                }
            }
            else
            {
                if(!filter)
                    filter = m_contextMenu[0].addItem(EditorContextMenu::ItemId_FilterTarget, "Remove for target");
                
                if(!filterByPlatform[i])
                {
                    ContextMenuItem* item = filter->m_subMenu.addItem(EditorContextMenu::ItemId_FilterTarget, targetName);
                    filterByPlatform[i] = btrue;
                    item->m_userData = u64(i);
                }
            }
        }
    }



/*    //if (getSubEditedItem() == ITF_INVALID_OBJREF && !PLUGINGATEWAY->getExclusiveFocus())
    if (_target == ITF_INVALID_OBJREF && !PLUGINGATEWAY->getExclusiveFocus())
        m_contextMenu[0].addItem(EditorContextMenu::ItemId_LoadScene, "Load Scene");
        */
/*
    if (getSelectionSize())
    {
        m_contextMenu[0].addItem(EditorContextMenu::ItemId_HideSelected, "Hide selected");
        m_contextMenu[0].addItem(EditorContextMenu::ItemId_HideUnSelected, "Hide Unselected");
    }
*/
#ifdef ITF_WINDOWS
/*
#if !defined(ITF_FINAL)
    {
        String reason;
        if(EDITOR->canGroupSelection(reason))
            m_contextMenu[0].addItem(EditorContextMenu::ItemId_GroupSelection, "Group");
        if(EDITOR->canUngroupSelection(reason))
            m_contextMenu[0].addItem(EditorContextMenu::ItemId_UngroupSelection, "Ungroup");
    }
#endif // !ITF_FINAL
*//*
    const BaseObject* target = GETOBJECT(_target);
    if(target)
    {
        const SubSceneActor* pSSA = target->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        if (target->getObjectType() != BaseObject::eScene)
        {
            if(!pSSA)
                m_contextMenu[0].addSubItem(EditorContextMenu::ItemId_LaunchFromExplorer, "Launch", EditorContextMenu::ItemId_Editor, "Edit");
            if (EDITOR->getSubEditedItem() && PLUGINGATEWAY->isSubEditable(target))
                m_contextMenu[0].addItem(EditorContextMenu::ItemId_SubEdit, "SubEdit");
        }
        else
            m_contextMenu[0].addSubItem(EditorContextMenu::ItemId_SceneExplorer, "Scene Explorer", EditorContextMenu::ItemId_Editor, "Edit");

        m_contextMenu[0].addSubItem(EditorContextMenu::ItemId_PropertiesEditor, "Properties Editor", EditorContextMenu::ItemId_Editor, "Edit");
        
        if(!pSSA || !pSSA->getEmbedScene())
            m_contextMenu[0].addSubItem(EditorContextMenu::ItemId_FindInExplorer, "Find", EditorContextMenu::ItemId_File, "File");
    }*/
#endif // ITF_WINDOWS
/*
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
        m_contextMenu[0].addItem(EditorContextMenu::ItemId_UnHideAll, "Unhide all");
*/
}

#ifndef ITF_FINAL
void fillAltContextMenu_Core(EditorContextMenu& _menu)
{
    // drawcells
    _menu.addItem(EditorContextMenu::ItemId_DBG_DrawCells, "DBG DrawCells");

    // assert
    if (AssertManager::isFlagEnabled(AssertManager::m_FlagAll))
        _menu.addItem(EditorContextMenu::ItemId_DBG_Assert, "Disable Assert");
    else
        _menu.addItem(EditorContextMenu::ItemId_DBG_Assert, "Enable Assert");

    // camera run
    ContextMenuItem* item = _menu.addItem(EditorContextMenu::ItemId_DBG_CameraRun, "CameraRun");
    if (CameraRunStateTest::isRunning())
        item->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_CameraRunStop, "Stop");
    else
        item->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_CameraRunStart, "Start");
}

void fillAltContextMenu_LD(EditorContextMenu& _menu)
{
}

void fillAltContextMenu_Gameplay(EditorContextMenu& _menu)
{
    // physics

    // camera
    _menu.addItem(EditorContextMenu::ItemId_DBG_Physics, "DBG Physics");

    // Debug Draw Camera Modifiers
    if (CAMERACONTROLLERMANAGER->isDebugDrawModifiers())
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_CameraModifiers_Show, "Hide Modifiers", EditorContextMenu::ItemId_DBG_Camera, "Camera");
    else
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_CameraModifiers_Show, "Show Modifiers", EditorContextMenu::ItemId_DBG_Camera, "Camera");

    // Debug Draw Camera Controllers
    if (CAMERACONTROLLERMANAGER->isDebugDrawInfo())
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_Camera_Show, "Hide info", EditorContextMenu::ItemId_DBG_Camera, "Camera");
    else
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_Camera_Show, "Show info", EditorContextMenu::ItemId_DBG_Camera, "Camera");

    // Camera Screen Format 4/3
    ContextMenuItem* showScreen4By3 =  _menu.addSubItem(EditorContextMenu::ItemId_DBG_Camera_Screen4By3, "Format 4/3", EditorContextMenu::ItemId_DBG_Camera, "Camera");
    switch (CAMERACONTROLLERMANAGER->getDebugDrawScreen4By3())
    {
    case CameraControllerManager::None:
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Opaque, "Show Opaque");
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Transparent, "Show Transparent");   
    	break;
    case CameraControllerManager::Transparent:
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_HideScreen4By3, "Hide");
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Opaque, "Show Opaque");
        break;
    case CameraControllerManager::Opaque:
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_HideScreen4By3, "Hide");
        showScreen4By3->m_subMenu.addItem(EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Transparent, "Show Transparent"); 
        break;
    default:
        break;
    } 

    // Debug Draw Camera Shake
    if (CAMERACONTROLLERMANAGER->isShakeDebugDraw())
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_Camera_Shake, "Shake curves off", EditorContextMenu::ItemId_DBG_Camera, "Camera");
    else
        _menu.addSubItem(EditorContextMenu::ItemId_DBG_Camera_Shake, "Shake curves on", EditorContextMenu::ItemId_DBG_Camera, "Camera");
}

void fillAltContextMenu_Sound(EditorContextMenu& _menu)
{
    _menu.addItem(EditorContextMenu::ItemId_DBG_Sound, "DBG Sound");
}

void fillAltContextMenu_Stats(EditorContextMenu& _menu)
{
    _menu.addSubItem(EditorContextMenu::ItemId_DBG_DumpStats, "Dump Stats", EditorContextMenu::ItemId_DBG_Stats, "Stats");
    _menu.addSubItem(EditorContextMenu::ItemId_DBG_UndoRedo, "Dump undo/redo manager", EditorContextMenu::ItemId_DBG_Stats, "Stats");
    _menu.addSubItem(EditorContextMenu::ItemId_DBG_ReferenceIsFrame, "Set reference line to typical frame", EditorContextMenu::ItemId_DBG_Rasters, "Rasters");
    _menu.addSubItem(EditorContextMenu::ItemId_DBG_ReferenceIs5ms, "Set reference line to 5 ms", EditorContextMenu::ItemId_DBG_Rasters, "Rasters");
}

void fillAltContextMenu_Views(EditorContextMenu& _menu)
{
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_SwitchViewsLock, "Lock Views");
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_EMulatePCView, "Target PC");
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_EMulateXBOXView, "Target XBOX");
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_EMulatePS3View, "Target PS3");
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_EMulate3DSView, "Target 3DS");
    _menu.addItem(EditorContextMenu::ItemId_DBG_Views_EMulateVitaView, "Target VITA");
}

#endif // ITF_FINAL

void Editor::fillAltContextMenu(ObjectRef /*_target*/)
{
#ifndef ITF_FINAL
    ContextMenuItem* iCore = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_MenuCore, "Core");
    ContextMenuItem* iLD = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_MenuLD, "LD");
    ContextMenuItem* iGameplay = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_MenuGameplay, "Gameplay");
    ContextMenuItem* iSound = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_MenuSound, "Sound");
    ContextMenuItem* iStats = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_MenuStats, "Stats");
    ContextMenuItem* iViews = m_contextMenu[0].addItem(EditorContextMenu::ItemId_DBG_Views, "Views");

    fillAltContextMenu_Core(iCore->m_subMenu);
    fillAltContextMenu_LD(iLD->m_subMenu);
    fillAltContextMenu_Gameplay(iGameplay->m_subMenu);
    fillAltContextMenu_Sound(iSound->m_subMenu);
    fillAltContextMenu_Stats(iStats->m_subMenu);
    fillAltContextMenu_Views(iViews->m_subMenu);
#endif    
}

// - return btrue if the _item is handled in this method and the event should not be forwarded to plugins on anything else
// - return bfalse if the _item is not handled in this method (default case of the switch)
bbool Editor::handleContextMenuItemClicked(ContextMenuItem* _item, ObjectRef _menuTarget)
{
#ifdef ITF_WINDOWS
    // Normal menu
    if (_item->isExternal())
    {
        ActorEditor::getInstance()->onMainMenuItemSelected(_item);
        return bfalse;
    }

    switch(_item->getId())
    {
    case EditorContextMenu::ItemId_TopBar_Load:
    case EditorContextMenu::ItemId_LoadScene:
        openScene(btrue);
    break;

    case EditorContextMenu::ItemId_NewScene:
        newScene();
        break;

    case EditorContextMenu::ItemId_TopBar_ImportScene:
#ifndef ITF_FINAL
        importScene();
#endif
    break;

    case EditorContextMenu::ItemId_TopBar_Tools:
        // empty switch so that we don't fall into the "default" statement and "btrue" is returned
    break;

    case EditorContextMenu::ItemId_ExportSelectionToScene:
    {
#ifndef ITF_FINAL
        String savepath("exportedSelection.isc");
        if(FILEMANAGER->fileChooser(btrue, savepath, L"Scene (*.isc)\0*.isc\0"))
        {
            exportSelectionToScene(savepath);
        }
#endif
    }
    break;

    case EditorContextMenu::ItemId_ExportSelectionToGroup:
    {
#ifndef ITF_FINAL
        Scene* pScene = getFirstObjectSelectedScene();
        if(!pScene)
            pScene = getEditedScene();

        if(pScene)
        {
            String reason;
            ObjectRefList refList = getEditorSelectionAsObjRefList();
            if(!canGroupSelection(reason, refList, pScene))
            {
                ITF_ERROR("Cannot export as group because : '%ls'", reason.cStr());
            }
            else
            {
                String savepath("exportedSelection.grp");
                if(FILEMANAGER->fileChooser(btrue, savepath, L"Group (*.grp)\0*.grp\0"))
                {
                    UndoRedoSequenceScope sponge("disable");
                    sponge.cancel();

                    ObjectRef newgroup = groupSelection(refList, pScene);
                    exportSelectionToScene(savepath);
                    ungroupSelection(newgroup, bfalse);
                }
            }
        }
#endif
    }
    break;

    case EditorContextMenu::ItemId_TopBar_Save:
        PLUGINGATEWAY->onSceneSave();
        saveCurrentWorld(bfalse, bfalse);
    break;

    case EditorContextMenu::ItemId_ShowPickables:
        EditorDraw::cyclePickableDetailModes();
    break;
    
    case EditorContextMenu::ItemId_TopBar_SaveAs:
        PLUGINGATEWAY->onSceneSave();
        saveCurrentWorld(btrue, bfalse);
    break;

    case EditorContextMenu::ItemId_TopBar_NewFromTemplate:
        TemplateCopier::copyTemplateAndDependenciesWithGUI();
        break;

    case EditorContextMenu::ItemId_ShowGrid_SceneDepth:
        m_gridMode = (m_gridMode == 1 ? 0 : 1);
    break;

    case EditorContextMenu::ItemId_ShowGrid_ObjectDepth:
        m_gridMode = (m_gridMode == 2 ? 0 : 2);
    break;

    case EditorContextMenu::ItemId_2DItems:
        m_hide2d++;
        m_hide2d &= hide2d_mask;
    break;
    

    case EditorContextMenu::ItemId_NotImplemented:
        ITF_ERROR("Not yet implemented, sorry!");
    break;

    case EditorContextMenu::ItemId_Back1:
    case EditorContextMenu::ItemId_Back2:
    case EditorContextMenu::ItemId_Back3:
    case EditorContextMenu::ItemId_Back4:
    case EditorContextMenu::ItemId_Back5:
    case EditorContextMenu::ItemId_Back6:
    case EditorContextMenu::ItemId_Back7:
    case EditorContextMenu::ItemId_Back8:
        {
            resetPause();
            goBackOneStep();
            ObjectRef ref(u32(_item->m_userData));
            if (ref.isValid())
                forceShowContextMenu(btrue , btrue, ref, btrue);
            else
                forceShowContextMenu(bfalse);
            setSubEditedItem(ITF_INVALID_OBJREF);
        }
    break;


    case EditorContextMenu::ItemId_SubEdit:
        PLUGINGATEWAY->cancelCurrentAction(bfalse);
        selectObject(_menuTarget, btrue);
        PLUGINGATEWAY->enterSubEdit(_menuTarget);
    break;

    case EditorContextMenu::ItemId_PropertiesEditor:
        SYSTEM_ADAPTER->launchExe("PropertiesEditor.exe", NULL, btrue, btrue);
    break;

    case EditorContextMenu::ItemId_Unpause:
        CHEATMANAGER->setPause(bfalse);
        m_pausedByUser = bfalse;
    break;
    case EditorContextMenu::ItemId_DANSceneExplorer:
        SYSTEM_ADAPTER->launchExe("UA_DanCustomTools.exe", NULL, btrue, btrue);
        break;
    case EditorContextMenu::ItemId_SceneExplorer:
        SYSTEM_ADAPTER->launchExe("UA_SceneExplorer.exe", NULL, btrue, btrue);
    break;

    case EditorContextMenu::ItemId_Unselect:
        if (!m_pausedByUser)
            resetPause();

        if(getEditedSubSceneHolder().isValid())
            endSubSceneEdition();

        PLUGINGATEWAY->cancelCurrentAction(bfalse);
        unselectAll();
        resetPickingInfo(btrue);

        setState(EditorState_Idle);
        PLUGINGATEWAY->setExclusiveFocus(NULL);

        setTopBarMenuShowable(bfalse);
        showContextMenu(bfalse);
    break;


    case EditorContextMenu::ItemId_HideSelected:
        hideSelected();
    break;


    case EditorContextMenu::ItemId_UnHideAll:
        unhideAll();
    break;

    case EditorContextMenu::ItemId_FullScreen:
        SYSTEM_ADAPTER->toggleShowTitleBar();        
        break;

    case EditorContextMenu::ItemId_HideUnSelected:
        hideUnselected();
    break;

    case EditorContextMenu::ItemId_TopBar_Play:
        backToGame();
    break;

    case EditorContextMenu::ItemId_TopBar_Pause:
        CHEATMANAGER->togglePause();        
    break;

    case EditorContextMenu::ItemId_TopBar_Reinit:
        backToGame(btrue);
        GAMEMANAGER->emptyCurrentLevelGameState();
        GAMEMANAGER->resetToLastCheckpointAndSave();
    break;

    case EditorContextMenu::ItemId_TopBar_Reload:
        reloadCurrentMap();
    break;

    case EditorContextMenu::ItemId_TopBar_Undo:
        UNDOREDO_MANAGER->requestUndo();
    break;

    case EditorContextMenu::ItemId_TopBar_Redo:
        UNDOREDO_MANAGER->requestRedo();
    break;

    case EditorContextMenu::ItemId_TopBar_AutoPause:
        changeAutoPauseMode();
    break;

    case EditorContextMenu::ItemId_TopBar_Katana_Cut:
        Plugin_KatanaCut::setMode(Plugin_KatanaCut::CutMode_Active);
    break;

    case EditorContextMenu::ItemId_TopBar_FriezeMultiCol:
        if (FriezeEditor* frzEd = FriezeEditor::getInstance())
            frzEd->startMultiCol();
    break;  

    case EditorContextMenu::ItemId_TopBar_Select2DActors:
        Editor::switch2DSelection();
    break;

    case EditorContextMenu::ItemId_TopBar_PickAlphaObjects:
		switchObjectFamilyAsPickableForFrameBuffer(Pickable::ObjectFamily_Alpha);
    break;

#if !defined(ITF_FINAL)
    case EditorContextMenu::ItemId_GroupSelection:
        EDITOR->groupSelection(EDITOR->getEditorSelectionAsObjRefList(), getFirstObjectSelectedScene());
    break;

    case EditorContextMenu::ItemId_UngroupSelection:
        EDITOR->ungroupSelection(EDITOR->getFirstSelectedObjRef(), bfalse);
    break;

    case EditorContextMenu::ItemId_TopBar_ClearWarnings:
        WarningManager::clear();
        WarningManager::filterDisplayForObjects(NULL, 0);
        break;

    case EditorContextMenu::ItemId_TopBar_CenterOnWorldOrigin:
        CURRENTWORLD->centerOnWorldOrigin();
        saveCurrentWorld(bfalse, btrue);
        reloadCurrentMap();
        break;

    case EditorContextMenu::ItemId_TopBar_ShowWorldOrigin:
        m_showWorldOrigin = btrue;
        break;

    case EditorContextMenu::ItemId_TopBar_HideWorldOrigin:
        m_showWorldOrigin = bfalse;
        break;

    case EditorContextMenu::ItemId_TopBar_GotoWorldOrigin:
        if(CAMERA)
        {
            CAMERA->setMode(Camera::Camera_Editor);
            CAMERA->forcePosition(Vec3d(0.f, 0.f, 10.f));
        }
        break;

#endif ! ITF_FINAL 

    // Alt menu
    case EditorContextMenu::ItemId_DBG_Camera_Show:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->switchDebugDrawInfo();
#endif
        break;

    case EditorContextMenu::ItemId_DBG_CameraModifiers_Show:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->switchDebugDrawModifiers();
#endif
        break;

    case EditorContextMenu::ItemId_DBG_Camera_HideScreen4By3:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->setDebugDrawScreen4By3( CameraControllerManager::None);
#endif
        break;

    case EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Transparent:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->setDebugDrawScreen4By3( CameraControllerManager::Transparent);        
#endif
        break;

    case EditorContextMenu::ItemId_DBG_Views_SwitchViewsLock:
        View::switchLockAll();
        break;

    case EditorContextMenu::ItemId_DBG_Views_EMulatePCView:
        CellsLayersSettings::m_currentTarget = CellsLayersSettings::CellTgt_PC;
        break;

    case EditorContextMenu::ItemId_DBG_Views_EMulateXBOXView:
        CellsLayersSettings::m_currentTarget = CellsLayersSettings::CellTgt_XBOX;
        break;

    case EditorContextMenu::ItemId_DBG_Views_EMulatePS3View:
        CellsLayersSettings::m_currentTarget = CellsLayersSettings::CellTgt_PS3;
        break;

    case EditorContextMenu::ItemId_DBG_Views_EMulate3DSView:
        CellsLayersSettings::m_currentTarget = CellsLayersSettings::CellTgt_3DS;
        break;

    case EditorContextMenu::ItemId_DBG_Views_EMulateVitaView:
        CellsLayersSettings::m_currentTarget = CellsLayersSettings::CellTgt_VITA;
        break;

    case EditorContextMenu::ItemId_DBG_Camera_ShowScreen4By3_Opaque:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->setDebugDrawScreen4By3( CameraControllerManager::Opaque);        
#endif
    break;

    case EditorContextMenu::ItemId_DBG_Camera_Shake:
#ifndef ITF_FINAL
        CAMERACONTROLLERMANAGER->switchShakeDebugDraw();
#endif
    break;

    case EditorContextMenu::ItemId_DBG_DrawCells:
        switchDebugCellsMode();
    break;


    case EditorContextMenu::ItemId_DBG_Sound:
#ifdef ITF_SUPPORT_RAKI
        SOUND_ADAPTER->toggleDebug();
#endif
    break;


    case EditorContextMenu::ItemId_DBG_DumpStats:
        ANIMATIONSTAT_DUMP;
        FRIEZESTAT_DUMP;
    break;

    case EditorContextMenu::ItemId_DBG_UndoRedo:
    {
        static const char * dest = "c:\\undoRedoDump.txt";
        if(UNDOREDO_MANAGER->dumpToFile(dest))
            ShellExecuteA(0, NULL, dest, NULL, NULL, SW_NORMAL);
    }
    break;

    case EditorContextMenu::ItemId_DBG_ReferenceIsFrame:
    {
        RASTER_SETREFERENCE(LOGICDT);
    } break;

    case EditorContextMenu::ItemId_DBG_ReferenceIs5ms:
    {
        RASTER_SETREFERENCE(0.005f);
    } break;

    case EditorContextMenu::ItemId_DBG_Physics:
        STIMSMANAGER->toggleDrawStims();
    break;

    case EditorContextMenu::ItemId_DBG_CameraRunStart:
        CameraRunStateTest::startNewTest();
        break;

    case EditorContextMenu::ItemId_DBG_CameraRunStop:
        CameraRunStateTest::stopTest();
        break;

    case EditorContextMenu::ItemId_DBG_Assert:
    if (AssertManager::isFlagEnabled(AssertManager::m_FlagAll))
        AssertManager::unsetFlag(AssertManager::m_FlagAll);
    else
        AssertManager::setFlag(AssertManager::m_FlagAll);
    break;


    case EditorContextMenu::ItemId_FindInExplorer:
    {
        const BaseObject* pObj = GETOBJECT(_menuTarget);

        String absPath;
        if(getFullPath(pObj, absPath))
            SystemAdapter::findInExplorer(absPath);
    }
    break;

    case EditorContextMenu::ItemId_LaunchFromExplorer:
    {
        const BaseObject* pObj = GETOBJECT(_menuTarget);

        String absPath;
        if(getFullPath(pObj, absPath))
            SystemAdapter::launchFromExplorer(absPath);
    }
    break;

    case EditorContextMenu::ItemId_GenericInsert:
    {
        const wchar_t* filters = L"All supported files (*.act;*.fcg;*.frz;*.isc;*.tsc;*.grp;*.png;*.tga;*.anm)\0*.act;*.fcg;*.frz;*.isc;*.grp;*.tsc;*.png;*.tga;*.anm\0Actor (*.act)\0*.act\0Frise config (*.fcg)\0*.fcg\0Frise export (*.frz)\0*.frz\0Scene (*.isc)\0*.isc\0Subscene (*.tsc)\0*.tsc\0Group (*.grp)\0*.grp\0Image (*.png;*.tga)\0*.png;*.tga\0Animation (*.anm)\0*.anm\0";

        String absolutePath;
        if(FILEMANAGER->fileChooser(bfalse, absolutePath, filters))
        {
            const String absolutePathNormalized = FilePath::normalizePath(absolutePath);
            String relativePath = absolutePathNormalized;
            if(!FilePath::transformToRelative(relativePath))
            {
                ITF_ERROR("Insert from outside the data root folder is not allowed : '%ls'", absolutePathNormalized.cStr());
                break;
            }

            ActorEditor* pActorEditor = ActorEditor::getInstance();
            const String extension = FilePath::getExtension(absolutePathNormalized);
            if(extension == "isc")
            {
                if(pActorEditor)
                {
                    switch(SYSTEM_ADAPTER->messageBox("Question", "Import as reference (YES) or merge (NO) ?", ITF_MSG_ICONQUESTION | ITF_MSG_YESNOCANCEL,ITF_IDCANCEL))
                    {
                    case ITF_IDYES:
                        pActorEditor->enterCreateSceneState(absolutePathNormalized, bfalse, bfalse);
                        break;
                    case ITF_IDNO:
                        pActorEditor->enterCreateSceneState(absolutePathNormalized, btrue, bfalse);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if(extension == "grp" || extension == "tsc")
            {
                EDITOR->onFileDrop(absolutePathNormalized);
            }
            else
            {
                PLUGINGATEWAY->onFileDrop(absolutePathNormalized);
            }
        }
    }
    break;

    case EditorContextMenu::ItemId_ChooseGridUnit:
        chooseGridUnit();
    break;

	case EditorContextMenu::ItemId_SetMusicTheme:
		setMusicTheme(StringID(StringID::StringIdValueType(_item->m_userData)));
		break;

    case EditorContextMenu::ItemId_MuteMusic:
        MuteMusic(bbool(_item->m_userData));
        break;
    
    case EditorContextMenu::ItemId_ChooseAngleStep:
        chooseAngleStep();
    break;

    case EditorContextMenu::ItemId_ToggleHighlightOnMouseOver:
        {
            String msg = "Highlight on mouse over : ";
            if(EditorDraw::getHighlightOnMouseOver())
            {
                EditorDraw::setHighlightOnMouseOver(bfalse);
                msg += "OFF";
            }
            else
            {
                EditorDraw::setHighlightOnMouseOver(btrue);
                msg += "ON";
            }
            DEBUGINFO->addDebugInfoDisplay(msg, 5.0f);
        }
        break;
    
    case EditorContextMenu::ItemId_TopBar_Scenes:
        {
            if(_item->m_userData == 0) // New tab
            {
                EDITOR->newTabFromMenu();
            }
            else if(_item->m_userData == 1) // Close current tab
            {
                if(checkAndAskForUnsavedWork())
                {
                    if(World* pCurrWorld = CURRENTWORLD)
                    {
                        for(u32 i = 0; i < m_tabList.size(); ++i)
                        {
                            TabEditInfo& infos      = m_tabList[i];
                            const String& worldName = infos.m_worldUniqueName;

                            if(pCurrWorld->getUniqueName() != worldName)
                            {
                                World* matchingWorld = WORLD_MANAGER->getWorldFromUniqueName(worldName);
                                ITF_ASSERT(matchingWorld);
                                if(matchingWorld)
                                {
                                    EDITOR->swapCurrentWorldWith(matchingWorld->getRef(), &infos);
                                    ITF_VERIFY(WORLD_MANAGER->deleteWorld(pCurrWorld));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if(_item->m_userData == 2) // Close other tabs
            {
                if(checkAndAskForUnsavedWork())
                {
                    if(World* pCurrWorld = CURRENTWORLD)
                    {
                        SafeArray<World*> toDeleteList;

                        for(u32 i = 0; i < m_tabList.size(); ++i)
                        {
                            const TabEditInfo& infos    = m_tabList[i];
                            const String& worldName     = infos.m_worldUniqueName;

                            if(pCurrWorld->getUniqueName() != worldName)
                                if(World* matchingWorld = WORLD_MANAGER->getWorldFromUniqueName(worldName))
                                    toDeleteList.push_back(matchingWorld);
                        }

                        for(u32 i = 0; i < toDeleteList.size(); ++i)
                        {
                            ITF_VERIFY(WORLD_MANAGER->deleteWorld(toDeleteList[i]));
                        }
                    }
                }
            }
            // Swap to another tab
            else if (_item->m_userData < U32_INVALID)
            {
                StringID clickedWorldUniqueName(StringID::StringIdValueType(_item->m_userData));

                World* matchingWorld = WORLD_MANAGER->getWorldFromStringID(clickedWorldUniqueName);
                ITF_ASSERT(matchingWorld);
                if(matchingWorld)
                {
                    TabEditInfo infos;
                    infos.m_worldUniqueName = matchingWorld->getUniqueName();                    
                    ITF_VERIFY(EDITOR->getTabEditInfo(infos));

                    EDITOR->swapCurrentWorldWith(matchingWorld->getRef(), &infos);
                    EDITOR->updateTabEditInfo(infos);
                }
            }
        }
    break;

    case EditorContextMenu::ItemId_SubSceneLock:
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(ObjectRef(u32(_item->m_userData)).getObject()))
        {
            EDITOR->setWorkingSubscene(pSSA);
        }
        break;
    
    case EditorContextMenu::ItemId_SubSceneUnlock:
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(ObjectRef(u32(_item->m_userData)).getObject()))
        {
            ITF_ASSERT_MSG(EDITOR->getWorkingSubScene().isValid(), "Wrong call");
            EDITOR->setWorkingSubscene(NULL);
        }
        break;
    
    case EditorContextMenu::ItemId_SubSceneGoto:
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(ObjectRef(u32(_item->m_userData)).getObject()))
        {
            CAMERA->setMode(Camera::Camera_Editor);
            Vec3d target = pSSA->getPos();
            target.m_z = CAMERA->getZ();
            CAMERA->cancelBiasMode();
            CAMERA->forcePosition(target);
        }
        break;
    
    case EditorContextMenu::ItemId_SubSceneOpen:
        if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(ObjectRef(u32(_item->m_userData)).getObject()))
        {
            ObjectRefList list;
            EDITOR->startToEditSubScene(pSSA, list);
        }
        break;

    case EditorContextMenu::ItemId_FriezeMeshToggleDisplay:
        EditorDraw::showFriseTriangles(!EditorDraw::getShowFriseTriangles());
    break;
    
    case EditorContextMenu::ItemId_FriezePivotToggleDisplay:
        EDITOR->toggleFriezePivotDisplay();
    break;
    
    case EditorContextMenu::ItemId_FriezePointsToggleDisplay:
        EDITOR->toggleFriezePointsDisplay();
    break;
    
    case EditorContextMenu::ItemId_FriezeCollisionsToggleDisplay:
        EDITOR->toggleFriezeCollisionsDisplay();
    break;
    
    case EditorContextMenu::ItemId_ToggleActorIconDisplay:
        EDITOR->toggleActorIconDisplay();
    break;
    
    case EditorContextMenu::ItemId_SubSceneContentShapesToggleDisplay:
        EDITOR->toggleSubSceneContentShapesDisplay();
    break;

    case EditorContextMenu::ItemId_AlignSelectionX:
        EDITOR->alignSelection(btrue, bfalse, bfalse);
        break;

    case EditorContextMenu::ItemId_AlignSelectionY:
        EDITOR->alignSelection(bfalse, btrue, bfalse);
        break;

    case EditorContextMenu::ItemId_AlignSelectionZ:
        EDITOR->alignSelection(bfalse, bfalse, btrue);
        break;

    case EditorContextMenu::ItemId_FilterTarget:
        {
            const u32 index = u32(_item->m_userData);
            if(index != U32_INVALID)
            {
                const String platform = platforms[index];

                const ObjectRefList selection = getEditorSelectionAsObjRefList();
                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(selection, resolvedRefs);

                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    const Pickable* pObj = static_cast<const Pickable*>(resolvedRefs[i]);
                    Scene* pScene = pObj->getScene();

                    pScene->filterObject(pObj->getUserFriendly(), platform);
                    EDITOR->setSceneAsModified(pScene);
                }
            }
        }
        break;

    case EditorContextMenu::ItemId_UnfilterTarget:
        {
            const u32 index = u32(_item->m_userData);
            if(index != U32_INVALID)
            {
                const String platform = platforms[index];

                const ObjectRefList selection = getEditorSelectionAsObjRefList();
                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(selection, resolvedRefs);

                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    const Pickable* pObj = static_cast<const Pickable*>(resolvedRefs[i]);
                    Scene* pScene = pObj->getScene();

                    pScene->unfilterObject(pObj->getUserFriendly(), platform);
                    EDITOR->setSceneAsModified(pScene);
                }
            }
        }
        break;

    case EditorContextMenu::ItemId_SetWorkingSubscene:
        if(const BaseObject* pObj = GETOBJECT(_menuTarget))
            EDITOR->setWorkingSubscene(static_cast<const SubSceneActor*>(pObj));
        break;

    case EditorContextMenu::ItemId_UnsetWorkingSubscene:
        EDITOR->setWorkingSubscene(NULL);
        break;

	case EditorContextMenu::ItemId_OT_LaunchTrackingTool:
		SYSTEM_ADAPTER->launchExe("UA_OnlineTracking.exe", NULL, btrue, btrue);
		break;

	case EditorContextMenu::ItemId_OT_ClearTrackingData:
		{
			Plugin *pPlugin = PLUGINGATEWAY->getPluginByName( OnlineTrackingPlugin::getPluginName());

			if (pPlugin)
			{
				ITF::OnlineTrackingPlugin *pOTPlug = static_cast<ITF::OnlineTrackingPlugin *>(pPlugin);
				pOTPlug->clearTrackingData();
			}

		}
			break;

		
	case EditorContextMenu::ItemId_OT_SendLevelName:
		{
			Plugin *pPlugin = PLUGINGATEWAY->getPluginByName( OnlineTrackingPlugin::getPluginName());

			if (pPlugin)
			{
				ITF::OnlineTrackingPlugin *pOTPlug = static_cast<ITF::OnlineTrackingPlugin *>(pPlugin);
				pOTPlug->SendLevelNameToTool();
			}

		}
			break;

    case EditorContextMenu::ItemId_ShowRewards:
        GAMEMANAGER->showRewards();
        break;

    default:    // no assert, the command may be handled by plugins
        return bfalse;
    break;

    }
#endif // ITF_WINDOWS
    return btrue;
}    

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::addToHiddenList( Pickable* _pObj )
{
    ITF_ASSERT_MSG(_pObj, "Please provide a pointer that isn't null...");
    if(m_hiddenObjectList.find(_pObj->getRef()) == -1)
    {
        _pObj->setHiddenForEditorFlag(btrue);
        m_hiddenObjectList.push_back(_pObj->getRef());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::removeFromHiddenList( Pickable* _pObj )
{ 
    ITF_ASSERT_MSG(_pObj, "Please provide a pointer that isn't null...");
    const i32 index = m_hiddenObjectList.find(_pObj->getRef());
    if(index != -1)
    {
        _pObj->setHiddenForEditorFlag(bfalse);
        m_hiddenObjectList.eraseNoOrder(index);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::unhideAll()
{
    const UndoRedoSequenceScope scope("Unhide All");

    for (u32 index=0; index<m_hiddenObjectList.size(); index++)
    {
        if (Pickable *pickable = (Pickable*)GETOBJECT(m_hiddenObjectList[index]))
        {
            UNDOREDO_MANAGER->preChange(pickable);
            pickable->setHiddenForEditorFlag(bfalse);
        }
    }
    m_hiddenObjectList.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
void Editor::hideSelected()
{
    const UndoRedoSequenceScope scope("Hide selection");

    ObjectRefList selection = getEditorSelectionAsObjRefList();
    SafeArray<BaseObject*> resolvedRefs;
    ID_SERVER->getObjectListNoNull(selection, resolvedRefs);

    for (u32 index=0; index< resolvedRefs.size(); index++)
    {
        if (Pickable* pickable = static_cast<Pickable*>(resolvedRefs[index]))
        {
            UNDOREDO_MANAGER->preChange(pickable);
            addToHiddenList(pickable);

            if(SubSceneActor* pSSA = pickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                SafeArray<Scene*> scenesToHide;
                scenesToHide.push_back(pSSA->getSubScene());

                while(scenesToHide.size())
                {
                    Scene* pScene = scenesToHide[0];
                    scenesToHide.eraseNoOrder(0);

                    // Recursive feed the while monster
                    ObjectRefList ssas = pScene->getSubSceneActors();
                    for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                        if(BaseObject* pBO = ssas[iSSA].getObject())
                            if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                                if(Scene* pSubScene = pSSA->getSubScene())
                                    scenesToHide.push_back(pSubScene);

                    const PickableList& content = pScene->getPickableList();
                    for(u32 iPickable = 0; iPickable < content.size(); ++iPickable)
                    {
                        Pickable* pSubPickable = content[iPickable];
                        UNDOREDO_MANAGER->preChange(pSubPickable);
                        addToHiddenList(pSubPickable);
                    }
                }
            }
        }
    }
    unselectAll();
}
///////////////////////////////////////////////////////////////////////////////////////////
void Editor::hideUnselected()
{
    const UndoRedoSequenceScope scope("Hide unselected");

    if(World* pWorld = CURRENTWORLD)
    {
        SafeArray<Scene*> sceneToScan;
        sceneToScan.push_back(pWorld->getRootScene());
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

            const PickableList& pickables = pScene->getPickableList();
            for(u32 iP = 0; iP < pickables.size(); ++iP)
            {
                Pickable* pPickable = pickables[iP];

                if(!pPickable->isSelected())
                {
                    Scene* pScene = pPickable->getScene();
                    bbool bHide = btrue;
                    if(SubSceneActor* pSSA = pPickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        // It's a group|subscene so do not hide if a sub element is selected

                        SafeArray<Scene*> scenesToCheck;
                        scenesToCheck.push_back(pSSA->getSubScene());
                        while(scenesToCheck.size())
                        {
                            Scene* pSceneBis = scenesToCheck[0];
                            scenesToCheck.eraseNoOrder(0);

                            // Recursive feed the while monster
                            ObjectRefList ssas = pSceneBis->getSubSceneActors();
                            for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                                if(BaseObject* pBO = ssas[iSSA].getObject())
                                    if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                                        if(Scene* pSubScene = pSSA->getSubScene())
                                            scenesToCheck.push_back(pSubScene);

                            const PickableList& content = pSceneBis->getPickableList();
                            for(u32 iPickable = 0; iPickable < content.size(); ++iPickable)
                            {
                                if(content[iPickable]->isSelected())
                                {
                                    bHide = bfalse;
                                    break;
                                }
                            }
                        }
                    }

                    if(bHide)
                    {
                        // hide only if none of it's ancestors are selected
                        SubSceneActor* pAncestor = pScene->getSubSceneActorHolder();
                        while(pAncestor)
                        {
                            if(pAncestor->isSelected())
                            {
                                bHide = bfalse;
                                break;
                            }
                            pAncestor = pAncestor->getScene()->getSubSceneActorHolder();
                        }
                    }

                    if(bHide)
                    {
                        UNDOREDO_MANAGER->preChange(pPickable);
                        addToHiddenList(pPickable);
                    }
                }
            }
        }
    }
}


#undef START_LABEL
#undef NEXT_LABEL
} //namespace ITF

