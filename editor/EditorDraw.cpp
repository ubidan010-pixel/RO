#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "tools/plugins/SequenceEditorPlugin/SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

    bbool EditorDraw::m_bShowAnchors             = btrue;
    bbool EditorDraw::m_bShowTextTips            = btrue;
    bbool EditorDraw::m_bShowFriseCollisions     = btrue;
    bbool EditorDraw::m_bShowFriseTriangles      = bfalse;
    bbool EditorDraw::m_bShowObjectAABBs         = btrue;
    u32   EditorDraw::m_showCells                = 0;
    bbool EditorDraw::m_bShowPolylineEgdeIndices = bfalse;
    bbool EditorDraw::m_bHighlightOnMouseOver    = bfalse;

    EditorDraw::PickableDetailMode EditorDraw::m_pickableDetailsMode = EditorDraw::PickableDetails_ShowDbg;

    ITF_VECTOR<EditorDraw::FriezeColorData> EditorDraw::m_modifiedFriezes;
    extern bbool  isFriezeUnderMouse(const Frise* _pFrieze);

    // DirectX values
    const u32 EditorDraw::uNormalColor        = ITFCOLOR_TO_U32(150, 20, 20, 200);
    const u32 EditorDraw::uSelectedColor      = ITFCOLOR_TO_U32(150, 20, 200, 20);
    const u32 EditorDraw::uSubEditedColor     = ITFCOLOR_TO_U32(150, 200, 20, 20);
    const u32 EditorDraw::uDarkColor          = ITFCOLOR_TO_U32(150, 50, 50, 50);

    const Vec2d vTextOffset       = Vec2d(15.0f, Editor::fCharMaxHeight);

    const i32 KEY_FOR_DISPLAY_FRISE_NAME = KEY_F3;

    ITF_VECTOR<AABB> EditorDraw::m_loadedCells;
    ITF_VECTOR<AABB> EditorDraw::m_updatedCells;

    ///////////////////////////////////////////////////////////////////////////////////////////
    void EditorDraw::drawCurrentMode()
    {
#ifndef ITF_FINAL
        bbool isSequencePlaying=bfalse;

#if defined(ITF_DEBUG_STRINGID)
        //Check whether a sequence is playing
        SequenceEditorPlugin* plugin = static_cast<SequenceEditorPlugin*>(PLUGINGATEWAY->getPluginByName(SequenceEditorPlugin::getPluginName()));
        if (plugin)
        {
            Actor *sequenceActor = plugin->getCurrentSequenceActor();
            if (sequenceActor)
            {
                SequencePlayerComponent *seqPlayComp = sequenceActor->GetComponent<SequencePlayerComponent>();
                isSequencePlaying = (seqPlayComp && seqPlayComp->getState() == SequencePlayerComponent::State_Playing);
            }
        }
#endif

        //if not, display info about current mode
        if (!isSequencePlaying && !EDITOR->isContextMenuVisible())
        {
            const char *cameraModeString = "";
            const char *editorModeString = "";
            
            // Grid mode
            String gridModeString;
            switch(EDITOR->getGridMode())
            {
            case 1:
                gridModeString.setTextFormat("Scene grid, unit : %.4f", EDITOR->getGridUnit());
                break;
            case 2:
                gridModeString.setTextFormat("Object grid, unit : %.4f", EDITOR->getGridUnit());
                break;
            default:
                gridModeString.setTextFormat("Unit : %.4f", EDITOR->getGridUnit());
                break;
            }

            //Camera mode
            switch(CAMERA->getMode())
            {
            case Camera::Camera_Editor:
                cameraModeString = "Camera: Editor (press esc to leave)";                
                break;
            case Camera::Camera_InGame:                 
                break;
            case Camera::Camera_Slave:
                cameraModeString = "Camera: Cinematic (click to switch to editor)";
                break;
            default:
                cameraModeString = "Camera: unknown";
                break;
            }

            const bbool bSubEditSubScene = EDITOR->getEditedSubSceneHolder().isValid();
            if(bSubEditSubScene && EDITOR->getState() != Editor::EditorState_Idle)
            {
                SubSceneActor* pSSA = static_cast<SubSceneActor*>(GETOBJECT(EDITOR->getEditedSubSceneHolder()));
                ITF_ASSERT_MSG(pSSA, "EDITOR->getEditedSubSceneHolder() refers to a deleted subsceneactor");
                if(pSSA)
                    GFX_ADAPTER->drawDBGAABB(pSSA->getAABB(), 0.1f, 0.8f, 0.1f, 0.0f, AABB_LineSize, pSSA->getDepth());
            }

            String workingDepth;
            if(EDITOR->isWorkingDepthForced())
                workingDepth.setTextFormat("Current forced working depth: %f", EDITOR->getForcedWorkingDepth().getReferenceDepth());

            //editor mode
            switch(EDITOR->getState())
            {
            case Editor::EditorState_EditMode:
                if(EDITOR->getSubEditedItem() != ITF_INVALID_OBJREF)
                    editorModeString = "Sub editing item";
                else if(bSubEditSubScene)
                    editorModeString = "Sub scene editing";
                else
                    editorModeString = "Editing";
                break;

            case Editor::EditorState_EditMode_SelectDepthRange:
                editorModeString = "Selecting working depth";
                break;

            case Editor::EditorState_EditMode_Measure:
                editorModeString = "Measure mode";
                break;

            default:
                gridModeString.clear();
                workingDepth.clear();
                break;
            }
            
            String displayed;
            if (cameraModeString[0] && editorModeString[0])
                displayed.setTextFormat("%s - %s", editorModeString, cameraModeString);
            else
                displayed.setTextFormat("%s%s", editorModeString, cameraModeString);
            
            f32 y = f32(GFX_ADAPTER->getScreenHeight() - 30);

            if(EDITOR->getSelectionSize() == 1)
            {
                if(Pickable* pObj = static_cast<Pickable*>(EDITOR->getSelectionAt(0)->getOwnerPtr()))
                {
                    String curSelName("Currently selected: ");
                    curSelName += pObj->getUserFriendly();
                    GFX_ADAPTER->drawDBGText(curSelName, ITF_POS2D_AUTO, y, 1.0f, 1.0f, 1.0f, bfalse, FONT_ALIGN_RIGHT);
                    y -= 15.0f;
                }
            }

            if (displayed.getLen())
            {
                GFX_ADAPTER->drawDBGText(displayed, ITF_POS2D_AUTO, y, 1.0f, 1.0f, 1.0f, bfalse, FONT_ALIGN_RIGHT);
                y -= 15.0f;
            }

            if(workingDepth.getLen())
            {
                GFX_ADAPTER->drawDBGText(workingDepth, ITF_POS2D_AUTO, y, 1.0f, 1.0f, 1.0f, bfalse, FONT_ALIGN_RIGHT);
                y -= 15.0f;
            }

            if(gridModeString.getLen())
            {
                GFX_ADAPTER->drawDBGText(gridModeString, ITF_POS2D_AUTO, y, 1.0f, 1.0f, 1.0f, bfalse, FONT_ALIGN_RIGHT);
                y -= 15.0f;
            }

            if(EDITOR->getState() != Editor::EditorState_Idle && UNDOREDO_MANAGER->thereIsSomeModificationSinceLastSave())
            {
                GFX_ADAPTER->drawDBGText("UNSAVED CHANGES", GFX_ADAPTER->getScreenWidth() / 2.0f, 5.0f, 1.0f, 0.5f, 0.5f, bfalse, FONT_ALIGN_LEFT);
            }
        }
#endif // ITF_FINAL
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void EditorDraw::startDraw()
    {
        ITF_VECTOR<FriezeColorData>::iterator it = m_modifiedFriezes.begin();
        ITF_VECTOR<FriezeColorData>::iterator end = m_modifiedFriezes.end();
        
        for (; it != end; ++it)
        {
            FriezeColorData& data = *it;
            Frise* pFrise = (Frise*)GETOBJECT(data.m_frieze);
            if (pFrise)
               pFrise->setGlobalColor(data.m_meshColor); // restore colors for frises modified during the previous frame
        }
        m_modifiedFriezes.clear();

        if (EDITOR->mustShowWorldOrigin())
        {
            GFX_ADAPTER->drawDBGCircle(Vec3d::Zero, 0.2f);
            GFX_ADAPTER->drawDBGCircle(Vec3d::Zero, 0.4f);
            GFX_ADAPTER->drawDBGCircle(Vec3d::Zero, 0.8f);
            GFX_ADAPTER->drawDBGCircle(Vec3d::Zero, 1.5f);
            GFX_ADAPTER->drawDBGCircle(Vec3d::Zero, 3.f);
            f32 size = fabsf(CAMERA->getZ() * 0.5f);
            GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d(size, 0.f, 0.f), 1, 0, 0, 0.f, 5.f);
            GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d(0.f, size, 0.f), 0, 1, 0, 0.f, 5.f);
            GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d(0.f, 0.f, size), 0, 0, 1, 0.f, 5.f);
        }
    }
    
    //increment m_friseDetailsMode, from 0 to max value, and cycle
    void EditorDraw::setPickableDetailModes(PickableDetailMode _mode)
    {
        m_pickableDetailsMode = _mode;
        if (m_pickableDetailsMode >= PickableDetails_Count)
            m_pickableDetailsMode = (PickableDetailMode)0;

        if (m_pickableDetailsMode < PickableDetails_ShowDbg)
        {
            EDITOR->m_hide2d |= Editor::hide2d_dbg;
            EDITOR->setTopBarMenuShowable(bfalse);
            EDITOR->forceShowContextMenu(bfalse);
        }
        else
        {
            EDITOR->m_hide2d &= ~Editor::hide2d_dbg;
            EDITOR->setTopBarMenuShowable(btrue);
        }
    }

    void EditorDraw::cyclePickableDetailModes()
    {
        setPickableDetailModes((PickableDetailMode)((int)m_pickableDetailsMode+1));
    }

    bbool EditorDraw::checkMustHidePickableDetails(const Pickable *_pickable)
    {
        ITF_ASSERT(_pickable!=NULL);

        if (m_pickableDetailsMode == PickableDetails_ShowAll)
            return bfalse;

        return 
        (
            (m_pickableDetailsMode<PickableDetails_ShowDbg) 
            || 
            (
                EDITOR->getSubEditedItem()!=_pickable->getRef() && 
                !_pickable->isSelected()
            )            
        );
    }

    void EditorDraw::drawActor( Actor* _pActor, bbool _bMouseIsOver, bbool _bSubElement/* = bfalse*/)
    {
        if (!EditorDraw::pickableDetail_canShowAnyDetail())
        {
            return;
        }

        const bbool isSubEdited = EDITOR->isSubEdited(_pActor->getRef());

        bbool canShowDetails = bfalse;
        if((EDITOR->getSubEditedItem() == ITF_INVALID_OBJREF || isSubEdited) && (EDITOR->getSelectionSize() < 5 || _bMouseIsOver))
        {
            if (_bMouseIsOver)
                canShowDetails = btrue;
            if (EditorDraw::pickableDetail_CanShowAll())
                canShowDetails = btrue;
            if (!_bSubElement && (_pActor->isSelected() || _bMouseIsOver))
                canShowDetails = btrue;
        }
        
        if(!_pActor->hasDataError())
        {
            if(canShowDetails)
            {
                EditorDraw edit;
                _pActor->drawEdit(&edit, (u32)ActorComponent::DrawEditFlag_All);
            }
            else if (FriezeEditor::getInstance()->hasFriezeInfoFlag(FriezeEditor::FriezeInfoFlag_Lines))
            {
                EditorDraw edit;
                _pActor->drawEdit(&edit, (u32)ActorComponent::DrawEditFlag_Collisions);
            }
        }

        if(EditorDraw::getShowObjectAABBs())
        {
            if(isSubEdited)
            {
                GFX_ADAPTER->drawDBGAABB(_pActor->getAABB(), 0.1f, 0.8f, 0.1f, 0.0f, AABB_LineSize, _pActor->getDepth());
            }
            else if(_pActor->isSelected() && !_bSubElement)
            {
                GFX_ADAPTER->drawDBGAABB(_pActor->getAABB(), 0.8f, 0.1f, 0.1f, 0.0f, AABB_LineSize, _pActor->getDepth());
            }
            else
            {
                if (_bMouseIsOver && _pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    GFX_ADAPTER->drawDBGAABB(_pActor->getAABB(), 0.1f, 0.1f, 0.8f, 0.0f, AABB_LineSize, _pActor->getDepth());
            }
        }

        if(canShowDetails)
        {
            // Text tip
            if(EditorDraw::getShowTextTips() && !EDITOR->isContextMenuVisible() && (EDITOR->getSubEditedItem() != _pActor->getRef()))
            {
                String name = _pActor->getUserFriendly();

                const bbool anchorIsVisible = CAMERA->isRectVisible(AABB(_pActor->getPos()), _pActor->getDepth());

                Vec2d pos;
                if (_bMouseIsOver)
                {
                    pos = EDITOR->getMouse2d();
                }
                else // when everyone is showing its texttip
                {
                    if(anchorIsVisible)
                    {
                        Vec3d pos2d;
                        GFX_ADAPTER->compute3DTo2D(_pActor->getPos(), pos2d);
                        pos = pos2d.truncateTo2D();
                    }
                    else
                        pos = Vec2d::Zero;
                }
                pos += Vec2d(vTextOffset.m_x, vTextOffset.m_y);

                u32 uColor = EditorDraw::uNormalColor;
                if(isSubEdited)
                    uColor = EditorDraw::uSubEditedColor;
                else if (_pActor->isSelected())
                    uColor = EditorDraw::uSelectedColor;

                if(const SubSceneActor* pSSA = _pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                {
                    if(!pSSA->getEmbedScene())
                    {
                        PathString_t pathName;
                        pSSA->getSceneRelativePath().getString(pathName);
                        name.setTextFormat("%s (file: %ls)", pSSA->getUserFriendly().cStr(), pathName.cStr());
                    }
                }

                if (_pActor->hasDataError())
                {
                    name += " --> ";
                    name += _pActor->getDataErrorLabel();
                }

                GFX_ADAPTER->draw2dBox(pos, name.getLen() * Editor::fCharMaxWidth_Big + 2.0f * Editor::fTextSubOffset, Editor::fCharMaxHeight_Big, uColor, uColor, EditorDraw::uDarkColor, uColor);
                GFX_ADAPTER->drawDBGText(name, pos.m_x + Editor::fTextSubOffset, pos.m_y, 1.0f, 1.0f, 1.0f, btrue);
#ifdef _DEBUG
                GFX_ADAPTER->draw2dBox(Vec2d(pos.m_x, pos.m_y + Editor::fCharMaxHeight), 40 * Editor::fCharMaxWidth + Editor::fTextSubOffset, Editor::fCharMaxHeight, uColor, uColor, EditorDraw::uDarkColor, uColor);
                String str;
                str.setTextFormat("ObjectRef: %u - Adrs: %p", _pActor->getRef().getValue(), _pActor);
                GFX_ADAPTER->drawDBGText(str, pos.m_x + Editor::fTextSubOffset, pos.m_y + Editor::fCharMaxHeight);
#endif // _DEBUG
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

	void drawPointList(const PolyPointList& _points, const Matrix44 *_optTransform, f32 _z, f32 _lineSize, Color _color)
	{
		// Build transformation matrix
        Matrix44 transform;
        if (_optTransform)
        {
            transform = *_optTransform;
        }
        else
        {
            transform.setIdentity();
        }

        const u32 posCount = _points.getPosCount();

		// Draw pts & edges
        if (posCount)
        {
            if ( posCount == 1 )
            {
                Vec3d ptIn, ptTransformed;
                ptIn.setFromVec2d(_points.getPosAt(0), &_z);
                M44_matrixCoordTransform(&ptTransformed, &transform, &ptIn);

                GFX_ADAPTER->drawDBGCircle(ptTransformed, 1.f, 0, 1, 0);
            }
            else
            {
                for (u32 pt = 0; pt < posCount-1; pt++ )
                {
                    const Vec2d& p1_untransformed_plane = _points.getPosAt(pt);
                    const Vec2d& p2_untransformed_plane =_points.getPosAt(pt+1);
                    Vec3d p1_untransformed_3d, p2_untransformed_3d, p1_3d, p2_3d;
                    p1_untransformed_3d.setFromVec2d(p1_untransformed_plane,&_z);
                    p2_untransformed_3d.setFromVec2d(p2_untransformed_plane,&_z);

                    M44_matrixCoordTransform(&p1_3d, &transform, &p1_untransformed_3d);
                    M44_matrixCoordTransform(&p2_3d, &transform, &p2_untransformed_3d);


                    
                    // Draw lines
					GFX_ADAPTER->drawDBGLine((Vec2d&)p1_3d, (Vec2d&)p2_3d, _color.getRed(), _color.getGreen(), _color.getBlue(), 0, _lineSize * 1.5f, _z/*p1_3d.m_z*/, _color.getAlpha());

                    // draw normals
                    Vec3d PCenter = (p1_3d + p2_3d)*0.5f;
                    Vec3d PNormal_untransformed;
                    Vec3d PNormal;
                    f32 zero=0.f;
                    PNormal_untransformed.setFromVec2d(_points.getEdgeAt(pt).m_normalizedVector.getPerpendicular()*0.25f, &zero);
                    M44_matrixVec3dTransform(&PNormal, &transform, &PNormal_untransformed);
                    Vec3d normalEnd = PCenter + PNormal;
                    GFX_ADAPTER->drawDBGLine((Vec2d&)PCenter, (const Vec2d&)normalEnd, _color.getRed(), _color.getGreen(), _color.getBlue(), 0, _lineSize, _z/*PCenter.m_z*/, _color.getAlpha());

                    // draw indices
                    if (EditorDraw::getShowPolylineEdgeIndices())
                    {
                        const Vec2d screenOffset(PNormal.m_x * 50, -PNormal.m_y * 50);
                        DebugDraw::text(normalEnd, screenOffset,
                            _color, DebugDraw::defaultBackgroundColor(),
                            " %d ", pt);
                    }
                }
            }
        }

	}

    void drawWallRunStopIcon( const PolyLine* _pPolyLine, const PolyLineEdge* _pCurEdge, const PolyLineEdge* _pPrevEdge, const Vec3d& _pos, bbool _bShowFriseCollisions )
    {
        if ( !_bShowFriseCollisions )
            return;

        f32 angle = _pCurEdge->m_normalizedVector.getOrientedAngle(_pPrevEdge->m_normalizedVector);
        if ( angle <= 0.f )
            return;

        const Vec2d gravity(0.f,-1.f);
        StickToPolylinePhysComponent::EdgeOrientationType curEdgeOrientation = (StickToPolylinePhysComponent::EdgeOrientationType)AIUtils::getEdgeOrientationType(_pCurEdge->m_normalizedVector, gravity);
        StickToPolylinePhysComponent::EdgeOrientationType prevEdgeOrientation = (StickToPolylinePhysComponent::EdgeOrientationType)AIUtils::getEdgeOrientationType(_pPrevEdge->m_normalizedVector, gravity);

        if ( curEdgeOrientation != prevEdgeOrientation )
        {
            if ( ( curEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND && prevEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
                || ( curEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL && prevEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND ) )
            {
                if( !AIUtils::isWallRunEnabledOnPolyline( _pCurEdge, _pPrevEdge ) )
                {
                    Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_WallRunSTop;
                    EDITOR->getIconStock().drawIcon(icon, _pos, Anchor_DefaultSize);
                }
            }
        }
    }

	void drawPolyLineData(const PolyLine* _pPolyLine, f32 _fZ, Matrix44 *_optTransform, bbool _bShowWallRunIcons)
	{
		u32 posCount = _pPolyLine->getPosCount();

		Matrix44 transform;
        if (_optTransform)
        {
            transform = *_optTransform;
        }
        else
        {
            transform.setIdentity();
        }

        if ( posCount == 1 )
        {
            Vec3d ptIn, ptTransformed;
            ptIn.setFromVec2d(_pPolyLine->getPosAt(0), &_fZ);
            M44_matrixCoordTransform(&ptTransformed, &transform, &ptIn);

            GFX_ADAPTER->drawDBGCircle(ptTransformed, 1.f, 0, 1, 0);
			return;
        }
		else
        {
			for (u32 pt = 0; pt < posCount-1; pt++ )
            {
				const Vec2d& p1_untransformed_plane = _pPolyLine->getPosAt(pt);
                const Vec2d& p2_untransformed_plane = _pPolyLine->getPosAt(pt+1);
                Vec3d p1_untransformed_3d, p2_untransformed_3d, p1_3d, p2_3d;
                p1_untransformed_3d.setFromVec2d(p1_untransformed_plane,&_fZ);
                p2_untransformed_3d.setFromVec2d(p2_untransformed_plane,&_fZ);

                M44_matrixCoordTransform(&p1_3d, &transform, &p1_untransformed_3d);
                M44_matrixCoordTransform(&p2_3d, &transform, &p2_untransformed_3d);

                // draw connections and wall run icons
                if ( _pPolyLine->m_ownerFrise )
                {                    
                    if ( pt == 0 )
                    {
                        if ( _pPolyLine->getPrevious() || _pPolyLine->isLooping() )
                        {
                            // connection
                            Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_Polyline_Connected;
                            EDITOR->getIconStock().drawIcon(icon, p1_3d, Connection_IconSize);

                            // wall run
                            const PolyLineEdge* prevEdge = NULL;
                            if ( _pPolyLine->getPrevious() )
                                prevEdge = &_pPolyLine->getPrevious()->getEdgeAt(_pPolyLine->getPrevious()->getPosCount()-2);
                            else
                                prevEdge =& _pPolyLine->getEdgeAt(posCount -2);

                            drawWallRunStopIcon( _pPolyLine, &_pPolyLine->getEdgeAt(pt), prevEdge, p1_3d, _bShowWallRunIcons );
                        }
                    }
                    else
                    {
                        // wall run
                         drawWallRunStopIcon( _pPolyLine, &_pPolyLine->getEdgeAt(pt), &_pPolyLine->getEdgeAt(pt-1), p1_3d, _bShowWallRunIcons );
                    }

                    if ( _pPolyLine->getNext() && pt == posCount -2 )
                    {
                        // connection
                        Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_Polyline_Connected;
                        EDITOR->getIconStock().drawIcon(icon, p2_3d, Connection_IconSize);

                        // wall run
                        drawWallRunStopIcon( _pPolyLine, &_pPolyLine->getNext()->getEdgeAt(0), &_pPolyLine->getEdgeAt(pt), p2_3d, _bShowWallRunIcons );
                    }
                }
            }
        }
	}

	void drawFriezeData(const Frise* _pFrise, const Matrix44 *_optTransform)
	{
		if (_pFrise->isSelected())
			GFX_ADAPTER->drawDBGAABB(_pFrise->getAABB(), 0, 1, 1, 0, 2, _pFrise->getDepth());
/*		else
			GFX_ADAPTER->drawDBGAABB(_pFrise->getAABB(), 0, 1, 0, 0, 2, _pFrise->getDepth());*/

		i32 selectedEdgeIndex = -1;
        // Find the hovered edge
        {
            PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
            const PickingInfo2D* pPickingInfo = pShape ? pShape->getPickingInfoPtr() : NULL;
            if(pPickingInfo && pShape->getOwner() == _pFrise->getRef())
            {
                const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
                if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
                    selectedEdgeIndex = subItemIndex;
            }
        }

		if (selectedEdgeIndex >= 0)
		{
			f32 z = _pFrise->getDepth();
			const Vec2d& p1_untransformed_plane = _pFrise->getPosAt(selectedEdgeIndex);
			const Vec2d& p2_untransformed_plane =_pFrise->getPosAt(selectedEdgeIndex+1);
			Vec3d p1_untransformed_3d, p2_untransformed_3d, p1_3d, p2_3d;
			p1_untransformed_3d.setFromVec2d(p1_untransformed_plane,&z);
			p2_untransformed_3d.setFromVec2d(p2_untransformed_plane,&z);

			M44_matrixCoordTransform(&p1_3d, _optTransform, &p1_untransformed_3d);
			M44_matrixCoordTransform(&p2_3d, _optTransform, &p2_untransformed_3d);

			GFX_ADAPTER->drawDBGLine((Vec2d&)p1_3d, (Vec2d&)p2_3d, 1.f, 0.f, 1.f, 0, 10.f, z, 1.f);
		}

	}

    void EditorDraw::drawPolyPointList( bbool _isSelected, ObjectRef _ref, const PolyPointList* _pPolyLine, f32 _lineSize, bbool _bMouseIsOver, bbool _bSubElement/* = bfalse*/, Matrix44 *_optTransform, f32 _heightMult, const String& _addPath, const String& _name)
	{
#if 0
        ITF_ASSERT(_pPolyLine);
        if(!_pPolyLine)
            return;

        Matrix44 transform;
        if (_optTransform)
        {
            transform = *_optTransform;
        }
        else
        {
            transform.setIdentity();
        }

        // make the normal line half as thick as the edges
        f32 normalLineSize = Max(1.f, _lineSize * 0.5f);


        bbool isSubEditedCollision = bfalse;
        bbool someoneIsSubEditedAndThatsNotMe = bfalse;
        bbool bSubEdited = EDITOR->isSubEdited(_ref);

        if (!bSubEdited && _pPolyLine->m_ownerFrise && EDITOR->isSubEdited(_pPolyLine->m_ownerFrise->getRef()))
            isSubEditedCollision = btrue;

        if (!bSubEdited && !isSubEditedCollision && EDITOR->getSubEditedItem().getObject())
            someoneIsSubEditedAndThatsNotMe = btrue;

        bbool showInfoFlags_lines = FriezeEditor::getInstance()->hasFriezeInfoFlag(FriezeEditor::FriezeInfoFlag_Lines);

        if (EDITOR->getSubEditedItem() != ITF_INVALID_OBJREF)
        {
            if (!bSubEdited)
            {
                if (_pPolyLine->m_ownerFrise)
                {
                    if(!EDITOR->isSubEdited(_pPolyLine->m_ownerFrise->getRef()))
                        return;
                }
            }
        }

        const f32 fZ = _pPolyLine->getDepth();

        _bSubElement = _bSubElement || _pPolyLine->m_ownerFrise;

        u32 showMask  = 0;
        if (m_pickableDetailsMode >= PickableDetails_ShowAll)
            showMask = 7;   // bit 0 = shape, bit 1 = anchor, bit 2 = text

        if (m_pickableDetailsMode >= PickableDetails_ShowAnchors)
            showMask |= 2;

        if (_isSelected  /*|| _bMouseIsOver*/)
            showMask |= 2;

        if (!_bSubElement && (_isSelected /*|| _bMouseIsOver*/))
            showMask |= 7;

        if (!_bSubElement && _bMouseIsOver)
            showMask |= 4;

        if (bSubEdited)
            showMask |= 3;

        if (showInfoFlags_lines)
            showMask |= 1;


/*        if (isSubEdited || bOwnerSubEdit) // never show anchors when subedited
            showMask &= ~2;*/

    /*    if (isSubEdited)
            showMask &= ~4;
*/
        f32 R, G, B, A;
        A = 1.f;
        if(_pPolyLine->getObjectType() == BaseObject::eFrise)
        {
            if(_bMouseIsOver || (_isSelected && EDITOR->isMultiSelectionActive()))
            {
                R = 0.5f + ((cos(ELLASPEDTIME * 2.0f) + 1.f) * 0.25f);
                G = 0.5f + ((cos(ELLASPEDTIME * 1.5f) + 1.f) * 0.25f);
                B = 0.5f + ((cos(ELLASPEDTIME * 1.0f) + 1.f) * 0.25f);
            }
            else
            {
                R = 0.f;
                G = 0.f;
                B = 1.f;
            }
        }
        else
        {
            showMask &= ~2;
            showMask |= 1;
            if(_bMouseIsOver || (_isSelected && EDITOR->isMultiSelectionActive()))
            {
                R = 0.5f + ((cos(ELLASPEDTIME * 1.0f) + 1.f) * 0.25f);
                G = 0.5f + ((cos(ELLASPEDTIME * 2.0f) + 1.f) * 0.25f);
                B = 0.5f + ((cos(ELLASPEDTIME * 1.5f) + 1.f) * 0.25f);
            }
            else
            {
                R = 0.f;
                G = 1.f;
                B = 0.f;
            }
        }
        if (bSubEdited)
        {
            R = 1.f;
            G = 1.f;
            B = 1.f;
        }
        else if (isSubEditedCollision)
        {
            R = 0.f;
            G = 1.f;
            B = 1.f;
        }
        if (someoneIsSubEditedAndThatsNotMe)
        {
            A = 0.5f;
        }
//        u32 iconColor = Color(A, R, G, B).getAsU32();

        u32 selectedEdgeIndex = U32_INVALID;

        // Find the hovered edge
        {
            PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
            const PickingInfo2D* pPickingInfo = pShape ? pShape->getPickingInfoPtr() : NULL;
            if(pPickingInfo && pShape->getOwner() == _pPolyLine->getRef())
            {
                const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
                if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
                    selectedEdgeIndex = subItemIndex;
            }
        }

        const u32 posCount = _pPolyLine->getPosCount();
        u32 uColor = uNormalColor;
        if(bSubEdited)
            uColor = uSubEditedColor;
        else if (_isSelected)
            uColor = uSelectedColor;


        if (showMask&1 && posCount )
        {
            if ( posCount == 1 )
            {
                Vec3d ptIn, ptTransformed;
                ptIn.setFromVec2d(_pPolyLine->getPosAt(0), &fZ);
                M44_matrixCoordTransform(&ptTransformed, &transform, &ptIn);

                GFX_ADAPTER->drawDBGCircle(ptTransformed, 1.f, 0, 1, 0);
            }
            else
            {
                for (u32 pt = 0; pt < posCount-1; pt++ )
                {
                    const Vec2d& p1_untransformed_plane = _pPolyLine->getPosAt(pt);
                    const Vec2d& p2_untransformed_plane = _pPolyLine->getPosAt(pt+1);
                    Vec3d p1_untransformed_3d, p2_untransformed_3d, p1_3d, p2_3d;
                    p1_untransformed_3d.setFromVec2d(p1_untransformed_plane,&fZ);
                    p2_untransformed_3d.setFromVec2d(p2_untransformed_plane,&fZ);

                    M44_matrixCoordTransform(&p1_3d, &transform, &p1_untransformed_3d);
                    M44_matrixCoordTransform(&p2_3d, &transform, &p2_untransformed_3d);


                    // Draw lines
                    if ( _pPolyLine->getEdgeAt(pt).m_switchTexture )
                        GFX_ADAPTER->drawDBGLine((Vec2d&)p1_3d, (Vec2d&)p2_3d, 1, 1, 0, 0, _lineSize, p1_3d.m_z, A);
                    else
                    {
                        if(pt == selectedEdgeIndex)
                            GFX_ADAPTER->drawDBGLine((Vec2d&)p1_3d, (Vec2d&)p2_3d, 1.0f, 0.0f, 0.0f, 0, _lineSize * 1.5f, p1_3d.m_z, A);
                        else
                        {
                            Color color = Color::green();
                            if (_bSubElement)
                                color = AIUtils::getEdgeColor(_pPolyLine->getEdgeAt(pt));
                            GFX_ADAPTER->drawDBGLine((Vec2d&)p1_3d, (Vec2d&)p2_3d, color.getRed(), color.getGreen(), color.getBlue(), 0, _lineSize, p1_3d.m_z, color.getAlpha());
                    }
                    }

                    // draw connections
                    if ( _pPolyLine->m_ownerFrise && !_pPolyLine->isHoldsFrieze() )
                    {
                        if ( pt == 0 )
                        {
                            if ( _pPolyLine->getPrevious() || _pPolyLine->isLooping() )
                            {
                                Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_Polyline_Connected;
                                EDITOR->getIconStock().drawIcon(icon, p1_3d, Connection_IconSize);
                            }
                        }
                        if ( _pPolyLine->getNext() && pt == posCount -2 )
                        {
                            Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_Polyline_Connected;
                            EDITOR->getIconStock().drawIcon(icon, p2_3d, Connection_IconSize);
                        }
                    }
                    
                    // draw normals
                    Vec3d PCenter = (p1_3d + p2_3d)*0.5f;
                    Vec3d PNormal_untransformed;
                    Vec3d PNormal;
                    f32 zero=0.f;
                    PNormal_untransformed.setFromVec2d(_pPolyLine->getEdgeAt(pt).m_normalizedVector.getPerpendicular()*0.25f, &zero);
                    M44_matrixVec3dTransform(&PNormal, &transform, &PNormal_untransformed);
                    Vec3d normalEnd = PCenter + PNormal;
                    GFX_ADAPTER->drawDBGLine((Vec2d&)PCenter, (const Vec2d&)normalEnd, R, G, B, 0, normalLineSize, PCenter.m_z, A);
                }
            }
        }

        if(m_bShowObjectAABBs)
        {
            if (!_optTransform) //we only draw DBG AABB for untransformed polylines
            {
                if(bSubEdited)
                {
                    GFX_ADAPTER->drawDBGAABB(_pPolyLine->getAABB(), 0.1f, 0.8f, 0.1f, 0.0f, _lineSize / 2.0f, _pPolyLine->getDepth());
                }
                else if(_isSelected && !_bSubElement)
                {
                    GFX_ADAPTER->drawDBGAABB(_pPolyLine->getAABB(), 0.8f, 0.1f, 0.1f, 0.0f, _lineSize / 2.0f, _pPolyLine->getDepth());
                }
            }
        }
 

        // Text tip
        if(showMask&4 && (EDITOR->getSelectionSize() < 5 || _bMouseIsOver))
        {
            String name = _name;
            String path("Config: ");
            f32 height = Editor::fCharMaxHeight;
            bbool usePath = bfalse;
			height *= _heightMult;
			if (_addPath.getLen())
			{
                usePath = btrue;
                path += _addPath;
			}

            bool isInShowFriseNameMode = m_pickableDetailsMode==PickableDetails_ShowAll;

            if(EditorDraw::getShowTextTips() && (_bMouseIsOver || isInShowFriseNameMode) && !EDITOR->isContextMenuVisible() &&  (EDITOR->getSubEditedItem() != _pPolyLine->getRef()))
            {
                Vec2d pos;

                if(_bMouseIsOver)
                {
                    pos = EDITOR->getMouse2d() + vTextOffset;
                }
                else/* if(INPUT_ADAPTER->isKeyPressed(KEY_F3))*/
                {
                    Vec3d res2d;
                    const Vec3d center = _pPolyLine->getAABB().getCenter().to3d(fZ);
                    GFX_ADAPTER->compute3DTo2D(center, res2d);
                    Vec2d::Vec3dTo2d(res2d, pos);
                }

                String name2 = name;
/*                if (_pPolyLine->hasDataError())
                {
                    name2 += " --> ";
                    name2 += _pPolyLine->getDataErrorLabel();
                }*/
                #ifdef _DEBUG
                    String str;
                    str.setTextFormat(" -- ObjectRef: %u - Adrs: 0x%x", _pPolyLine->getRef().m_value, (u32)_pPolyLine);
                    name2 += str;
                #endif // _DEBUG
                    
                const u32 len = usePath ? path.getLen() : name2.getLen();

                GFX_ADAPTER->draw2dBox(pos, len * Editor::fCharMaxWidth + Editor::fTextSubOffset, height, uColor, uColor, uDarkColor, uColor);

                GFX_ADAPTER->drawDBGText(name2, pos.m_x + Editor::fTextSubOffset, pos.m_y);
                if (usePath)
                    GFX_ADAPTER->drawDBGText(path, pos.m_x + Editor::fTextSubOffset, pos.m_y + Editor::fCharMaxHeight);
            }
        }
		#endif

    }



    void EditorDraw::drawFrise(const Frise* _pFrise, f32 _lineSize, bbool _bMouseIsOver)
    {
        ITF_ASSERT(_pFrise);
        if(!_pFrise)
            return;

		f32 fZ = _pFrise->getDepth();
        bbool someoneIsSubEditedAndThatsNotMe = bfalse;
        bbool bSubEdited = EDITOR->isSubEdited(_pFrise->getRef());

        if (!bSubEdited && EDITOR->getSubEditedItem().getObject() && EDITOR->getSubEditedItem() != _pFrise->getRef())
            someoneIsSubEditedAndThatsNotMe = btrue;

        bbool showInfoFlags_lines = FriezeEditor::getInstance()->hasFriezeInfoFlag(FriezeEditor::FriezeInfoFlag_Lines);

        bbool hideDetails = checkMustHidePickableDetails(_pFrise);

        bbool restoreShowTri = m_bShowFriseTriangles;
        // Draw the frise
        //   if (!hideDetails || _pFrise->isSelected() || _bMouseIsOver)
        bbool isDynamic = _pFrise->isDynamic();

        if (FriezeEditor::isMultiCollisionEdition(_pFrise))
        {
            showInfoFlags_lines = btrue;
            hideDetails = bfalse;
            someoneIsSubEditedAndThatsNotMe = bfalse;
            bSubEdited = btrue;
            _bMouseIsOver = btrue;
            if (isFriezeUnderMouse(_pFrise))
                m_bShowFriseTriangles = btrue;
        }
        else if (FriezeEditor::isMultiCollisionEdition(NULL))
            return;


        if (m_pickableDetailsMode >= PickableDetails_ShowDbg)
        {
			f32 heightMult = 1.f;
			String addPath, name;
            name = _pFrise->getUserFriendly();
            _pFrise->getConfigNamePath().getString(addPath);
            heightMult *= 2.f;


			Matrix44 placement;

			if (isDynamic)
				_pFrise->computeDynamicDisplayMatrix(placement);
			else
				_pFrise->computeStaticDisplayMatrix(placement);

			if (_pFrise->isSelected() || FriezeEditor::isMultiCollisionEdition(_pFrise))
			{
				Color col = Color::white();
				if (someoneIsSubEditedAndThatsNotMe)
					col.m_a = 0.5f;
				drawPointList(_pFrise->getPointsData(), &placement, fZ, _lineSize, col);
			}
			drawFriezeData(_pFrise, &placement);
            //drawPolyPointList(_pFrise->isSelected(), &_pFrise->getPointsData(), _lineSize, _bMouseIsOver || isSubEdited, bfalse, pMatrix, heightMult, addPath, name);
        }

        // Draw collision data
        if(
            (showInfoFlags_lines) ||
            (m_bShowFriseCollisions && (!hideDetails || _pFrise->isSelected() || _bMouseIsOver))
            )
        {
			Matrix44 placement;
			placement.setIdentity();

            const SafeArray<PolyLine*>* pCollisionData = _pFrise->getCollisionData_WorldSpace();
            if ( pCollisionData )
            {
                const u32 colCount = pCollisionData->size();
                for(u32 colIdx = 0; colIdx < colCount; ++colIdx)
                {
                    PolyLine* pPolyline = (*pCollisionData)[colIdx];
                    {
                        Color col = Color::green();
                        if (someoneIsSubEditedAndThatsNotMe)
                            col.m_a = 0.5f;

                        drawPointList(pPolyline->getEdgeData(), &placement, fZ, _lineSize, col);
                        drawPolyLineData(pPolyline, fZ, &placement, showInfoFlags_lines);
                    }
                    //   drawPolyPointList(bfalse, collisions[colIdx], _lineSize, _bMouseIsOver, btrue); //collisions are in world space
                }
            }


            if (_pFrise->getVisualPoly().getObject())
            {
                drawPointList(((PolyLine*)(_pFrise->getVisualPoly().getObject()))->getEdgeData(), &placement, fZ, _lineSize, Color::magenta());
            }
        }
 
#ifdef ITF_WINDOWS
        if(_pFrise->isSelected() || _bMouseIsOver)
        {
            // Draw picking triangles
            // if( _pFrise->isSelected() && !hideDetails )
            if(m_bShowFriseTriangles && !hideDetails )            
            {
                f32 R, G, B;

                if(_bMouseIsOver)
                {
                    G = 0.5f + ((cosf(ELAPSEDTIME * 8.f) + 1.f) * 0.25f);
                    R = 1.f - G;
                    B = 0.f;
                }
                else
                {
                    R = 0.5f;
                    G = 0.5f;
                    B = 0.5f;
                }

                const u32 pickingTriangleCount = _pFrise->EditorWireframeMesh.size();

				Matrix44 placement;

				if (isDynamic)
					_pFrise->computeDynamicDisplayMatrix(placement);
				else
					_pFrise->computeStaticDisplayMatrix(placement);

                for (u32 i = 0; i < pickingTriangleCount; i+=3)
                {
                    for(u32 j = 0; j < 3; ++j)
                    {
                        Vec3d p1_untransformed, p2_untransformed;
                        Vec3d p1, p2;

                        p1_untransformed = _pFrise->EditorWireframeMesh[i+j];
                        p2_untransformed = _pFrise->EditorWireframeMesh[i+((j+1)%3)];

                        M44_matrixCoordTransform(&p1, &placement, &p1_untransformed);
                        M44_matrixCoordTransform(&p2, &placement, &p2_untransformed);

                        GFX_ADAPTER->drawDBG3DLine(p1, p2, R, G, B, 0.f, 0.5f);
                    }
                }
            }
        }

		bool isInShowFriseNameMode = m_pickableDetailsMode==PickableDetails_ShowAll;

        if(EditorDraw::getShowTextTips() && (_bMouseIsOver || isInShowFriseNameMode) && !EDITOR->isContextMenuVisible() &&  (EDITOR->getSubEditedItem() != _pFrise->getRef()))
        {
            Vec2d pos;

            if(_bMouseIsOver)
            {
                pos = EDITOR->getMouse2d() + vTextOffset;
            }
            else/* if(INPUT_ADAPTER->isKeyPressed(KEY_F3))*/
            {
                Vec3d res2d;
                const Vec3d center = _pFrise->getAABB().getCenter().to3d(fZ);
                GFX_ADAPTER->compute3DTo2D(center, res2d);
                Vec2d::Vec3dTo2d(res2d, pos);
            }

			String name2 = _pFrise->getUserFriendly();

            #ifdef _DEBUG
                String str;
                str.setTextFormat(" -- ObjectRef: %u - Adrs: %p", _pFrise->getRef().getValue(), _pFrise);
                name2 += str;
            #endif // _DEBUG
                
            const u32 len = name2.getLen();
            f32 height = Editor::fCharMaxHeight_Big;
			u32 uColor = uNormalColor;
			if(bSubEdited)
				uColor = uSubEditedColor;
			else if (_pFrise->isSelected())
				uColor = uSelectedColor;

            GFX_ADAPTER->draw2dBox(pos, len * Editor::fCharMaxWidth_Big + 2.0f * Editor::fTextSubOffset, height, uColor, uColor, uDarkColor, uColor);
            GFX_ADAPTER->drawDBGText(name2, pos.m_x + Editor::fTextSubOffset, pos.m_y, 1.0f, 1.0f, 1.0f, btrue);
        }

#endif // ITF_WINDOWS
        m_bShowFriseTriangles = restoreShowTri;

    }

    void EditorDraw::drawCells()
    {
        u32 iLayer = getDrawCellsEnable();
        if (!iLayer)
            return;

        iLayer--;


        World* pWorld = CURRENTWORLD;
        ITF_ASSERT(pWorld);

        if (!pWorld || pWorld->getIgnoreCells())
            return;

        { // draw boundaries
            AABB rect;

            if (pWorld->cellEverDataIsValid(iLayer))
            {
                rect.setMin(pWorld->getminCellPosEverRegistered(iLayer));
                rect.setMax(pWorld->getmaxCellPosEverRegistered(iLayer));
                GFX_ADAPTER->drawDBGAABB(rect, Color::green(), 0.f, 3.f);
            }

            rect.setMin(Vec2d(World::m_fminCellXCoord, World::m_fminCellYCoord));
            rect.setMax(Vec2d(World::m_fmaxCellXCoord, World::m_fmaxCellYCoord));
            GFX_ADAPTER->drawDBGAABB(rect, Color::green(), 0.f, 10.f);
        }


        const bbool& debugCell = EDITOR->isDebugCellsCamMode();
        for(u32 i = 0; i < m_loadedCells.size(); ++i)
        {
            const AABB& aabb = m_loadedCells[i];
            if (debugCell || CAMERA->isRectVisible(aabb, 0.0f))
                GFX_ADAPTER->drawDBGAABB(aabb, Color::yellow(), 0.f, 2.f);
        }

        for(u32 i = 0; i < m_updatedCells.size(); ++i)
                {
            const AABB& aabb = m_updatedCells[i];
            if (debugCell || CAMERA->isRectVisible(aabb, 0.0f))
                GFX_ADAPTER->drawDBGAABB(aabb, Color::orange(), 0.f, 2.f);
        }
       
        /*
       
        const Vec2d& mouse = EDITOR->getMouse3d().truncateTo2D();
                    
        String text;
        text.setTextFormat("%d enabled cells", m_updatedCells.size());

        Cell* pCell_selected = pWorld->getCellAtPos(iLayer, mouse);
        i32 selectedCellX, selectedCellY;
        if (pCell_selected)
        {
            pWorld->getCellIndexAtPos(iLayer, mouse, selectedCellX, selectedCellY);
            const AABB& aabbCell = pCell_selected->getAABB();
            GFX_ADAPTER->drawDBGAABB(aabbCell, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 0.0f);

            Vec2d center = aabbCell.getCenter();

            const ObjectRefList& vPickable = pCell_selected->getAllObjects();

            u32 objCount = vPickable.size();

            if (objCount)
            {
                String sCount;
                const Vec2d& pos = pCell_selected->getPos();
                sCount.setTextFormat("Objs count:%d pos:%0.02f/%0.02f %d/%d",objCount,pos.m_x,pos.m_y,
                    selectedCellX, selectedCellY);

                    text += " | ";
                text += sCount;
                
                if (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    for (u32 index = 0;index <objCount;++index)
                    {
                        const Pickable* pPickable = static_cast<Pickable*> (GETOBJECT(vPickable[index]));
                        if (pPickable)
                            GFX_ADAPTER->drawDBGAABB(pPickable->getAABB(),1.0f,0.0f,1.0f,0.0f,2.0f,pPickable ->getDepth());
                    }
                }
                
            }
        }
        */
        
        //GFX_ADAPTER->drawDBGText(text);
    }

    void EditorDraw::drawCursor()
    {
        ITF_ASSERT(0);
        /*

        const f32 size = 5.f * CAMERA->m_z;
        const f32 dist = 3.f * CAMERA->m_z;
        const f32 end = dist + size;
        const Vec2d& Pos = EDITOR->getMouse3d().truncateTo2D();

        //GFX_ADAPTER->depthTest(0);
        GFX_ADAPTER->drawLine(Vec2d(Pos.m_x - end, Pos.m_y), Vec2d(Pos.m_x - dist, Pos.m_y), 0, 1, 0);
        GFX_ADAPTER->drawLine(Vec2d(Pos.m_x + end, Pos.m_y), Vec2d(Pos.m_x + dist, Pos.m_y), 0, 1, 0);
        GFX_ADAPTER->drawLine(Vec2d(Pos.m_x, Pos.m_y - end), Vec2d(Pos.m_x, Pos.m_y - dist), 0, 1, 0);
        GFX_ADAPTER->drawLine(Vec2d(Pos.m_x, Pos.m_y + end), Vec2d(Pos.m_x, Pos.m_y + dist), 0, 1, 0);
        //GFX_ADAPTER->depthTest(1);
        
        */

        if(EditorDraw::getShowTextTips() && !EDITOR->isContextMenuVisible())
        {
            const Vec2d pos(EDITOR->getMouse2d().m_x + vTextOffset.m_x, EDITOR->getMouse2d().m_y + 3.0f * vTextOffset.m_y);

            const u32 uColor = uNormalColor;

            String8 coords;
            coords.setTextFormat("(%u, %u) => (%.3f, %.3f, %.3f)", u32(EDITOR->getMouse2d().m_x), u32(EDITOR->getMouse2d().m_y), EDITOR->getMouse3d().m_x, EDITOR->getMouse3d().m_y, EDITOR->getMouse3d().m_z);

            GFX_ADAPTER->draw2dBox(pos, coords.getLen() * Editor::fCharMaxWidth + Editor::fTextSubOffset, Editor::fCharMaxHeight, uColor, uColor, uDarkColor, uColor);
            GFX_ADAPTER->drawDBGText(coords, pos.m_x + Editor::fTextSubOffset, pos.m_y);
        }
    }

    void EditorDraw::drawPolyLine(const PolyLine* _pPolyLine, bbool _bSubElement )
    {
		drawPointList(_pPolyLine->getEdgeData(), NULL, _pPolyLine->getDepth(), FriezeEditor_LineSize, Color::blue());
     //   drawPolyPointList(bfalse, &_pPolyLine->getEdgeData(), FriezeEditor_LineSize, bfalse, _bSubElement);
    }

    void EditorDraw::drawSelectionZone( const Vec2d& _vStart, const Vec2d& _vEnd )
    {
        const Vec2d v1 = _vStart;
        const Vec2d v3 = _vEnd;

        const Vec2d v2(v1.m_x, v3.m_y);
        const Vec2d v4(v3.m_x, v1.m_y);

        GFX_ADAPTER->draw2dLine(v1, v2, 2.0f, ITFCOLOR_TO_U32(100, 20, 20, 200));
        GFX_ADAPTER->draw2dLine(v2, v3, 2.0f, ITFCOLOR_TO_U32(100, 20, 20, 200));
        GFX_ADAPTER->draw2dLine(v3, v4, 2.0f, ITFCOLOR_TO_U32(100, 20, 20, 200));
        GFX_ADAPTER->draw2dLine(v4, v1, 2.0f, ITFCOLOR_TO_U32(100, 20, 20, 200));
    }

    void EditorDraw::drawOrigin()
    {
        GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d::XAxis, 1.0f, 0, 0, 0.0f, 1.0f);
        GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d::YAxis, 0, 1.0f, 0, 0.0f, 1.0f);
        GFX_ADAPTER->drawDBG3DLine(Vec3d::Zero, Vec3d::ZAxis, 0, 0, 1.0f, 0.0f, 1.0f);
    }

    bbool isBasicUnitLine(f32 _xORy)
    {
        const f32 remaining = _xORy - floorf(_xORy);
        return remaining <= 0.00001f;
    }
    
    bbool isHalfBasicUnitLine(f32 _xORy)
    {
        const f32 remaining = _xORy - floorf(_xORy);
        return f32_Abs(0.5f - remaining) <= 0.00001f;
    }

    void EditorDraw::drawGrid( const Vec3d& _gridPoint, f32 _step )
    {
        if(EDITOR->isContextMenuVisible())
            return;

        // Find screen corners world position at gridPoint depth
        Vec3d bottomRight3d, topLeft3d;
        gridCompute(bottomRight3d, topLeft3d, _gridPoint, _step, _step);

        Vec3d vClosest = _gridPoint;

        // Find the closest point to bottomleft inside the view
        {
            while(vClosest.m_y > bottomRight3d.m_y)
                vClosest.m_y -= _step;

            while(vClosest.m_x > topLeft3d.m_x)
                vClosest.m_x -= _step;

            while(vClosest.m_x < topLeft3d.m_x)
                vClosest.m_x += _step;

            while(vClosest.m_y < bottomRight3d.m_y)
                vClosest.m_y += _step;
        }

        Vec3d start, end;
        start.m_z = end.m_z = vClosest.m_z;

        // Display horizontal lines
        {
            start.m_x   = topLeft3d.m_x;
            end.m_x     = bottomRight3d.m_x;
            start.m_y   = end.m_y = vClosest.m_y;

            while(start.m_y < topLeft3d.m_y)
            {
                const bbool isUnit          = isBasicUnitLine(start.m_y);
                const bbool isHalfUnit      = isHalfBasicUnitLine(start.m_y);

                f32 green                   = 0.5f;
                f32 lineSize                = 1.0f;

                if(isHalfUnit && _step < 0.5f)
                {
                    green = 0.75f;
                    lineSize = 1.0f;
                }
                else if(isUnit)
                {
                    green = 0.75f;
                    lineSize = 2.0f;
                }

                GFX_ADAPTER->drawDBG3DLine(start, end, 0.0f, green, 0.5f, 0.0f, lineSize);

                start.m_y += _step;
                end.m_y += _step;
            }
        }

        // Display vertical lines
        {
            start.m_y   = topLeft3d.m_y;
            end.m_y     = bottomRight3d.m_y;
            start.m_x   = end.m_x = vClosest.m_x;

            while(start.m_x < bottomRight3d.m_x)
            {
                const bbool isUnit          = isBasicUnitLine(start.m_x);
                const bbool isHalfUnit      = isHalfBasicUnitLine(start.m_x);

                f32 green                   = 0.5f;
                f32 lineSize                = 1.0f;

                if(isHalfUnit && _step < 0.5f)
                {
                    green = 0.75f;
                    lineSize = 1.0f;
                }
                else if(isUnit)
                {
                    green = 0.75f;
                    lineSize = 2.0f;
                }

                GFX_ADAPTER->drawDBG3DLine(start, end, 0.0f, green, 0.5f, 0.0f, lineSize);

                start.m_x += _step;
                end.m_x += _step;
            }
        }
    }

	void EditorDraw::drawMemory()
	{
#ifdef ITF_CATEGORY_MEMORY_ENABLE
		const f32 startY = 25.f;
		const f32 startX = 25.f;
		f32   incategory = 0.f;

		f32 x = startX;
		f32 y = startY;
		const f32 width = (f32)GFX_ADAPTER->getScreenWidth();
		const f32 height = (f32)GFX_ADAPTER->getScreenHeight();
		const f32 ylimit = height - Editor::fCharMaxHeight * 4.f;
		const f32 alpha = 5.f;
		const i32 val = 8 + (i32)alpha;
		const u32 uval = (u32)val | (((u32)val)<<4);
		const u32 ialpha = uval<<COLOR_SHIFT_ALPHA;


#define LINE_FIXED(text) GFX_ADAPTER->drawDBGTextFixed(text, x+incategory*10.f, y, 1.f, 1.f, 1.f); y += Editor::fCharMaxHeight; if (y > ylimit) {y = startY; x += width*0.3f;}
#define CATEGORY_FIXED(text) y += Editor::fCharMaxHeight; GFX_ADAPTER->drawDBGTextFixed(text, x, y, 1.f, 1.f, 1.f); y += Editor::fCharMaxHeight; if (y > ylimit) {y = startY; x += width*0.3f;} if (incategory<1.f) incategory = 1.f;
#define END_CATEGORY y += Editor::fCharMaxHeight; incategory = 0.f;


		GFX_ADAPTER->draw2dBox(Vec2d(x-5, y-5), width-startX-5, ylimit+5, 0xDD555555, 0xDD555555, ialpha|0x00555555, 0xDD555555);
		END_CATEGORY("---------- ALLOCATOR");
		ITF_VECTOR<String> strings;

		memoryStatsManager::get().dumpCategoryStats(strings);
		u32 stringsCount = strings.size();
		for (u32 i=0;i<stringsCount;i++)
		{
			LINE_FIXED(strings[i]);
		}

		CATEGORY_FIXED("---------- RESOURCES");
		strings.clear();
		ResourceManager::get().dumpActivity(strings);
		stringsCount = strings.size();
		for (u32 i=0;i<stringsCount;i++)
		{
			LINE_FIXED(strings[i]);
		}

		END_CATEGORY("---------- RESOURCES");


#endif //ITF_FINAL
	}

    void EditorDraw::drawHelp()
    {
        const f32 startY = 25.f;
        const f32 startX = 25.f;
        f32   incategory = 0.f;

        f32 x = startX;
        f32 y = startY;
        const f32 width = (f32)GFX_ADAPTER->getScreenWidth();
        const f32 height = (f32)GFX_ADAPTER->getScreenHeight();
        const f32 ylimit = height - Editor::fCharMaxHeight * 4.f;
                
#define LINE(text) GFX_ADAPTER->drawDBGText(text, x+incategory*10.f, y, 1.f, 1.f, 1.f); y += Editor::fCharMaxHeight; if (y > ylimit) {y = startY; x += width*0.3f;}
#define CATEGORY(text) y += Editor::fCharMaxHeight; GFX_ADAPTER->drawDBGText(text, x, y, 1.f, 1.f, 1.f); y += Editor::fCharMaxHeight; if (y > ylimit) {y = startY; x += width*0.3f;} if (incategory<1.f) incategory = 1.f;
#define END_CATEGORY y += Editor::fCharMaxHeight; incategory = 0.f;

        GFX_ADAPTER->draw2dBox(Vec2d(x-5, y-5), width-startX-5, ylimit+5, 0xDD555555, 0xDD555555,0xDD555555, 0xDD555555);

        LINE("ESCAPE : Back to game" );
        LINE("BACKSPACE : Back one step" );
        LINE("TAB : swap to other tabbed maps");
        LINE("F1 : this help" );
        LINE("F2 : rename selected objects" );
        LINE("F4 : teleport player under mouse" );
        LINE("F5 : load last checkpoint" );
        LINE("F5 + ALT : Same as F5 but keep players current position" );
        LINE("F5 + CTRL : reload current map" );
        LINE("F8 : toggle measure mode (use CTRL to clear measures)" );
        LINE("F11 : Toggle fullscreen");
        LINE("CTRL + F11 : Toggle Fake fullscreen");
        LINE("DEL : delete selection" );
        LINE("T : teleport the selection under the mouse" );
        LINE("SHIFT + T : create a trac" );
        LINE("CTRL + F : find object by friendly" );
        LINE("SHIFT + F : find undermouse object in explorer" );
        LINE("CTRL + S : save modified scenes (use SHIFT to save under new name)" );
        LINE("SHIFT + S : force save scenes" );
        LINE("CTRL + A : select everything" );
        LINE("SHIFT + A : toggle display of all sub icons" );
        LINE("CTRL + Z / Y : undo/redo" );
        LINE("CTRL + G / U : group/ungroup the selection");
        LINE("CTRL + Drag : duplicate selection" );
        LINE("CTRL + C/X V : copy/cut paste selection" );
        LINE("CTRL + SHIFT V : paste in place selection" );
        LINE("CTRL + Q : open Property Editor" );
        LINE("SHIFT + LMOUSE : add point to frieze in subedit");
        LINE("ALT is used to prevent position and angle snapping");
        LINE("LMOUSE : select and move(use CTRL for multiselection)" );
        LINE("Z + LMOUSE : select the working depth from the object undermouse depth (RMOUSE to unforce)" );
        LINE("MMOUSE : select by zone (use CTRL for multiselection)" );
        LINE("RMOUSE : contextual menu or cancel current duplication" );
        LINE("M : toggle snap by default");
        LINE("J : set initial info for selection in trajectory sequence" );
        LINE("X : x flip selection");
        LINE("H : to hide the selection");
        LINE("G : move selection to group");
        LINE("U : pop selection from group");
        LINE("CTRL + H : to hide unselected");
        LINE("SHIFT + H : to unhide all");
        LINE("LEFT RIGHT: move selection using grid unit on axis X");
        LINE("UP DOWN: move selection using grid unit on axis Y");
        LINE("PAGE_UP PAGE_DOWN: move selection using grid unit on axis Z");
        LINE("ALT + LMOUSE: link/unlink actors");
        LINE("CTRL + ALT + LMOUSE : bind actors");
        LINE("ALTGR + Wheel: Change picking alpha sensitivity");

        CATEGORY("---------- CAMERA")
        LINE("SPACE + LMOUSE : pan" );
        LINE("SPACE + RMOUSE : rotate camera" );
        LINE("SPACE + Wheel : zoom in/out (CTRL to smooth)" );
        LINE("HOME : center camera on selection" );
        LINE("SPACE + UP|DOWN|LEFT|RIGHT|PGUP|PGDN : move camera" );
        LINE("N : lock scene camera" );
        LINE("B : unlock scene camera" );
        LINE("K : toggle camera debug draw" );
        LINE("ALT + K : toggle camera dezoom max" );

        CATEGORY("---------- GAMEPLAY / PHYSICS")
        LINE("S : toggle stim's debug display" );
        LINE("M : toggle cell's debug display" );
        LINE("P : toggle pause" );
        LINE("D : toggle debug character" );
        LINE("O : do timestep" );
        LINE("Q : decrease timestep multiplier" );
        LINE("W : increase timestep multiplier" );
        LINE("SHIFT C : reset timestep multiplier" );

        CATEGORY("---------- DEBUG INFO")
        LINE("CTRL + R : toggle raster" );
        LINE("CTRL + SHIFT + R : cycle raster group" );
        LINE("CTRL + D : toggle gfxadapter debug info and FPS" );
        LINE("CTRL + E : toggle sound debug" );
        LINE("CTRL + K : toggle cooking time" );
        LINE("LALT + RMOUSE : debug contextual menu" );
        LINE("C : toggle cells display" );
        LINE("9 : toggle safe Frame display" );
        LINE("SHIFT + N : toggle mir display" );


        CATEGORY("---------- CAPTURE")
        LINE("3 : toggle ghost recording" );
        LINE("6 : start screen capture" );
        LINE("7 : stop screen capture" );
        LINE("CTRL + P : save high def screenshot to My Pictures");
        LINE("SHIFT + T : encode video & audio capture");

        CATEGORY("-- SHOW/HIDE (NUMERIC KEYPAD) --")
        LINE("0: switch all");
        LINE("1: actors anchors");
        LINE("2: friezes anchors");
        LINE("3: scene grid (CTRL for object grid)");
        LINE("4: level design tools");
        LINE("5: friezes control pts ");
        LINE("6: friezes lines");
        LINE("7: desactivated objects");
        LINE("8: level design objects");
        LINE("9: art objects");
        LINE("F3: cycle info display" );

        CATEGORY("---------- PAD")
        LINE("LB + RB + DPAD L/R: cycle checkpoints" );
        LINE("LB + RB + DPAD UP: go to map beginning" );
        LINE("LB + RB + RJoy: Zoom" );
        LINE("LB + RB + LT + RT: Fly mode" );
        LINE("LB + RB + Y: join game ( alive )" );
        LINE("LB + RB + B: toggle invincibility" );
        LINE("LB + RB + LThumb: toggle pause" );
        LINE("RThumb: reset to game mode" );

        CATEGORY("---------- SCREEN INFO")
        String text;
        text.setTextFormat("Resolution: %.0fx%.0f (ratio: %f)", width, height, width / height);
        LINE(text);
    }

    void EditorDraw::gridCompute( Vec3d& _bottomRight3d, Vec3d& _topLeft3d, const Vec3d& _refPoint, f32 _xStep, f32 _yStep )
    {
        const bbool oldBias=CAMERA->isBiased();
        if (oldBias)
            CAMERA->cancelBiasMode(btrue);

        Vec3d to2d;
        GFX_ADAPTER->compute3DTo2D(_refPoint, to2d);

        GFX_ADAPTER->compute2DTo3D(Vec3d(f32(GFX_ADAPTER->getScreenWidth()), f32(GFX_ADAPTER->getScreenHeight()), to2d.m_z), _bottomRight3d);
        GFX_ADAPTER->compute2DTo3D(Vec3d(0.0f, 0.0f, to2d.m_z), _topLeft3d);

        if (oldBias)
            CAMERA->restoreBiasMode(btrue);
    }

    void editorDraw_cellProcessCB( u32 _frame, const Cell* _loadedCell, u32 _iLayer )
    {
        static u32 s_lastFrame = 0;
        if(FAKE_FRAME != _frame && _frame != s_lastFrame)
        {
            EditorDraw::m_loadedCells.clear();
            EditorDraw::m_updatedCells.clear();
        }
        s_lastFrame = _frame;

        const u32 currentLayer = EditorDraw::getDrawCellsEnable();
        if(currentLayer > 0 && (currentLayer-1) == _iLayer)
        {
            if(_loadedCell->isEnable())
                EditorDraw::m_updatedCells.push_back(_loadedCell->getAABB());
            else
                EditorDraw::m_loadedCells.push_back(_loadedCell->getAABB());
        }
    }

} //namespace ITF

#endif //ITF_CONSOLE_FINAL
