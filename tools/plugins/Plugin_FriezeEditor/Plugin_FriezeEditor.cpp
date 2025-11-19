// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_


#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_


#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PLUGIN_KATANACUT_H_
#include "tools/plugins/Plugin_KatanaCut/Plugin_KatanaCut.h"
#endif //_ITF_PLUGIN_KATANACUT_H_

#ifdef DAN_TEXTURE_SELECTOR
#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif//_ITF_ANIMATIONMANAGER_H_
#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_
#endif //DAN_TEXTURE_SELECTOR
namespace ITF
{

#define CREATE_POINT_KEY                    KEY_LSHIFT

FriezeEditor*  FriezeEditor::m_instance = NULL;
bbool          FriezeEditor::m_multiCollisionEdition = bfalse;
bbool          FriezeEditor::m_showAllCollidableFriezes = bfalse;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

FriezeEditor::PickingShapeData::PickingShapeData( PickingShape* _pParentShape, Plugin* _pPlugin )
: PickingShapeData_RotationScale(_pParentShape, _pPlugin)
{
}


bbool  isFriezeUnderMouse(const Frise* _pFrieze)
{

    if (!EDITOR->getPickingShapeOfObjectUnderMouse())
        return bfalse;
    if (EDITOR->getPickingShapeOfObjectUnderMouse()->getOwnerPtr() !=  _pFrieze)
        return bfalse;

    return btrue;
}

bbool FriezeEditor::isMultiCollisionEdition(const Frise* _pFrieze)
{
    if (!m_multiCollisionEdition)
        return bfalse;
    if (!_pFrieze)
        return m_multiCollisionEdition;
    const SafeArray<PolyLine*>* pCollisionData = _pFrieze->getCollisionData_WorldSpace();
    if ( !pCollisionData || !pCollisionData->size() )
        return bfalse;
    if (m_showAllCollidableFriezes)
        return btrue;
    return  isFriezeUnderMouse(_pFrieze);
}


bbool replaceOneFriezeConfig(Frise* _target, const String& _path)
{
    if (!FILEMANAGER->fileExists(_path))
    {
        ITF_ERROR("Can't change frieze config, file not found:\n%ls ", _path.cStr());
        return bfalse;
    }

    UNDOREDO_MANAGER->preChange(_target);

    String frzFriendly = _target->getUserFriendly();
    PathString_t fileName;

    _target->getConfigNamePath().getString(fileName);

    if(frzFriendly.strstr(fileName.cStr()))
    {
        _target->generateNewUserFriendly(_path.cStr());
        PLUGINGATEWAY->onPickableRenamed(_target, frzFriendly.cStr());
    }

    _target->setFriseConfig(_path);
    PLUGINGATEWAY->onObjectChanged(_target);
    return btrue;
}


void FriezeEditor::pickColor(const Frise* _from)
{
    if (DEBUGINFO)
        DEBUGINFO->addDebugInfoDisplay("picked color", 3.0f);

    EDITOR->getCopiedProperties().m_color = _from->getColorFactor();
}

void FriezeEditor::pasteColor()
{
    UndoRedoSequenceScope scope("ItemId_FriezeEd_PasteProperties");
    for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
    {
        PickingShape* shape = EDITOR->getSelectionAt(iSel);
        if (shape)
        {
            BaseObject* owner = shape->getOwnerPtr();
            if (owner && owner->getObjectType() == BaseObject::eFrise)
            {
                UNDOREDO_MANAGER->preChange(((Frise*)owner));
                ((Frise*)owner)->setColorFactor(EDITOR->getCopiedProperties().m_color);
			    PLUGINGATEWAY->onObjectChanged(owner);
            }
        }
    }
}

void FriezeEditor::pickFogColor(const Frise* _from)
{
    if (DEBUGINFO)
        DEBUGINFO->addDebugInfoDisplay("picked Fog color", 3.0f);

    EDITOR->getCopiedProperties().m_colorFog = _from->getColorFog();
}

void FriezeEditor::pasteFogColor()
{
    UndoRedoSequenceScope scope("ItemId_FriezeEd_PasteFogProperties");
    for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
    {
        PickingShape* shape = EDITOR->getSelectionAt(iSel);
        if (shape)
        {
            BaseObject* owner = shape->getOwnerPtr();
            if (owner && owner->getObjectType() == BaseObject::eFrise)
            {
                UNDOREDO_MANAGER->preChange(((Frise*)owner));
                ((Frise*)owner)->setColorFog(EDITOR->getCopiedProperties().m_colorFog);
			    PLUGINGATEWAY->onObjectChanged(owner);
            }
        }
    }
}

void replaceCurrentSelectionKit(const String &_newFolder)
{
    UndoRedoSequenceScope scope("ItemId_FriezeEd_ReplaceKit");

    String newFolder(_newFolder);
    newFolder += '/';
    if (EDITOR->getSelectionSize() > 0)
    {
        String pth;

        for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
        {
            PickingShape* shape = EDITOR->getSelectionAt(iSel);
            if (shape)
            {
                BaseObject* owner = shape->getOwnerPtr();
                if (owner && owner->getObjectType() == BaseObject::eFrise)
                {
                    Frise* pOwner = (Frise*)owner;
                    pOwner->getConfigNamePath().getString(pth);
                    pth = FilePath::getFilename(pth);
                    replaceOneFriezeConfig((Frise*)owner, newFolder+pth);
                }
            }
        }
    }
}

i32 frzSubPointCompare(const PickingShape_Disc* _A, const PickingShape_Disc* _B)
{
    const FriezeExtraInfo dataA(_A->getData());
    const FriezeExtraInfo dataB(_B->getData());

    if (dataA.m_flags.m_subItemIndex < dataB.m_flags.m_subItemIndex)
    {
        return -1;
    }
    else if (dataB.m_flags.m_subItemIndex < dataA.m_flags.m_subItemIndex)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void FriezeEditor::PickingShapeData::update()
{
    PickingShapeData_RotationScale::update();

    const FriezeExtraInfo data(m_pickingShape->getData());

    if(!data.m_flags.m_isEdge)
    {
        PickingShape_Disc* disc = (PickingShape_Disc*)m_pickingShape;
        Frise* pFrz = static_cast<Frise*>(disc->getOwnerPtr());
        disc->setAngle(pFrz->getAngle()); // default value

        // TODO can be optimized, the operation will be done multiple time if multiple subpoint of the same frz are selected (one time is enought in the same frame)
        // This code is done because in edition, to rotate subpoints, we need the notion of 'angle' for a disc shape.
        if(disc->isSubAnchor() && EDITOR->getIndexOfShapeInSelection(m_pickingShape) != -1)
        {
            if(const SafeArray<PickingShape*>* pShapes = EDITOR->getPickingShapesForObject(m_pickingShape->getOwner()))
            {
                SafeArray<PickingShape_Disc*> selectedShapes;
                for(u32 i = 0; i < pShapes->size(); ++i)
                {
                    PickingShape* pShape = (*pShapes)[i];

                    if(pShape->getShapeType() == PickingShape::ShapeType_Edge)
                        continue;

                    const FriezeExtraInfo data2(pShape->getData());

                    if(data2.m_flags.m_isPivot)
                        continue;

                    if(EDITOR->getIndexOfShapeInSelection(pShape) != -1)
                    {
                        selectedShapes.push_back(static_cast<PickingShape_Disc*>(pShape));
                    }
                }

                selectedShapes.quickSort(frzSubPointCompare);

                if(selectedShapes.size() > 1)
                {
                    for(u32 i = 0; i < selectedShapes.size(); ++i)
                    {
                        PickingShape_Disc* pPrev = (i > 0) ? selectedShapes[i - 1] : NULL;
                        PickingShape_Disc* pCurr = selectedShapes[i];
                        PickingShape_Disc* pNext = (i < selectedShapes.size() - 1) ? selectedShapes[i + 1] : NULL;

                        if(!pPrev)
                        {
                            Vec3d dirCN = pNext->getPos() - pCurr->getPos();
                            dirCN.normalize();

                            const f32 angleCN = dirCN.truncateTo2D().getAngle();
                            pCurr->setAngle(angleCN);
                        }
                        else
                        {
                            Vec3d dirPC = pCurr->getPos() - pPrev->getPos();
                            dirPC.normalize();
                            const f32 anglePC = dirPC.truncateTo2D().getAngle();

                            if(pNext)
                            {
                                Vec3d dirCN = pNext->getPos() - pCurr->getPos();
                                dirCN.normalize();

                                const f32 angleCN = dirCN.truncateTo2D().getAngle();

                                pCurr->setAngle((anglePC + angleCN) / 2.0f);
                            }
                            else
                            {
                                pCurr->setAngle(anglePC);
                            }
                        }
                    }
                }
            }
        }
    }
}

void FriezeEditor::PickingShapeData::draw()
{
    PickingShapeData_RotationScale::draw();

    ITF_ASSERT(m_pickingShape);
    if(!m_pickingShape)
        return;

    const Pickable* pObj = static_cast<const Pickable*>(m_pickingShape->getOwnerPtr());
    if (!pObj) // yes, this happened on 2010, Oct 22nd
        return;

    // Only draw if anchors are visibles
    if(
        (EditorDraw::getPickableDetailMode() < EditorDraw::PickableDetails_ShowAnchors) &&
        (!pObj->isSelected()) &&
        (!getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_SubAnchors)) &&
        (!getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_MainAnchor))
      )
    {
        return;
    }

    if(!m_pickingShape)
        return;

    if ( (m_pickingShape->isSubAnchor()) &&
        (!getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_SubAnchors)) &&
        (EDITOR->getSubEditedItem() != m_pickingShape->getOwner())
        )
        return;

    if ( !m_pickingShape->isSubAnchor() && !getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_MainAnchor) && !pObj->isSelected())
        return;

    const FriezeExtraInfo data(m_pickingShape->getData());

    // Detect mouse over
    bbool mouseIsOver = bfalse;
    if(EDITOR->getPickingShapeOfObjectUnderMouse() == m_pickingShape)
    {
        const PickingInfo2D* pUnderMouseInfo = m_pickingShape->getPickingInfoPtr();
        if(EDITOR->getSubEditedItem() == ITF_INVALID_OBJREF || data.m_raw == FriezeExtraInfo(pUnderMouseInfo->m_extraInformation).m_raw)
            mouseIsOver = btrue;
    }

    bbool someoneIsSubEditedAndThatsNotMe = bfalse;
    bbool bSubEdited = EDITOR->isSubEdited(pObj->getRef());
    if (!bSubEdited && EDITOR->getSubEditedItem().getObject())
        someoneIsSubEditedAndThatsNotMe = btrue;

    float R = 1.f;
    float G = 1.f;
    float B = 1.f;
    float A = 1.f;
    if (someoneIsSubEditedAndThatsNotMe)
    {
        R = 0.5f;
        G = 0.5f;
        B = 0.5f;
        A = 0.7f;
    }
    u32 iconColor = Color(A, R, G, B).getAsU32();

    if(data.m_flags.m_isEdge)
        drawEdge(mouseIsOver, data, iconColor);
    else
        drawPoint(mouseIsOver, data, iconColor);
}

void FriezeEditor::PickingShapeData::drawPoint(bbool _mouseOver, const FriezeExtraInfo& _data, u32 _color)
{
    Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_PolyLinePoint;
    const Vec3d& pos = static_cast<PickingShape_Disc*>(m_pickingShape)->getPos();
    const Frise* pPoly = static_cast<const Frise*>(m_pickingShape->getOwnerPtr());

    if(_data.m_flags.m_isPivot)
    {
        bbool isDynamicFrieze=bfalse;
        if (pPoly->getObjectType()==BaseObject::eFrise)
        {
            isDynamicFrieze = static_cast<const Frise*>(pPoly)->isDynamic();
        }
        if(pPoly->isSelected())
        {
            icon = isDynamicFrieze?(Editor_IconStock::Icon_SelectedDynamicFrise):(Editor_IconStock::Icon_SelectedFrise);
        }
        else
        {
            icon = isDynamicFrieze?(Editor_IconStock::Icon_DynamicFrise):(Editor_IconStock::Icon_Frise);
        }

        if(pPoly->hasDataError())
        {
            icon = Editor_IconStock::Icon_RedSquare;
        }

        if(pPoly->isSelected())
        {
            GFX_ADAPTER->drawDBGCircle(pPoly->getWorldInitialPos(), 2.f, 1.f, 0.f, 0.f);

            // Don't display initial pos if it's almost the same some pos
            if((pPoly->getWorldInitialPos() - pPoly->getPos()).sqrnorm() > MTH_EPSILON)
            {
                GFX_ADAPTER->drawDBG3DLine(pPoly->getWorldInitialPos(), pPoly->getPos(),0.7f,0,0);
            }

            // Display difference between initial and current angle
            if(fabs(pPoly->getWorldInitialRot() - pPoly->getAngle()) > MTH_EPSILON)
            {
                Vec2d offset = (Vec2d::XAxis / 2.0f).Rotate(pPoly->getWorldInitialRot() + MTH_PIBY2);
                GFX_ADAPTER->drawDBG3DLine(pPoly->getPos(), pPoly->getPos() + offset.to3d(), 0, 0.7f, 0);
            }
        }
    }
    else // is poly point
    {
        if ( _data.m_flags.m_subItemIndex == 0)
        {
            _color = Color::yellow().getAsU32();
        }

        const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();

        if(selection.find(m_pickingShape) != -1)
            icon = Editor_IconStock::Icon_PolyLinePoint_Selected;
        else // the sub point is not selected
            icon = Editor_IconStock::Icon_PolyLinePoint;
    }

    f32 fIconSize = Anchor_DefaultSize;
    if(_mouseOver)
        fIconSize += (Anchor_DefaultSize * 0.1f) * cosf(ELAPSEDTIME*5.0f);

    EDITOR->getIconStock().drawIcon(icon, pos, fIconSize, pPoly->getAngle(), bfalse, bfalse, _color);
}

void FriezeEditor::PickingShapeData::drawEdge(bbool _mouseOver, const FriezeExtraInfo& _data, u32 _color)
{
}

void FriezeEditor::PickingShapeData::rotateAroundAxisZ( f32 _deltaAngle, const Vec3d* _pRotationCenter /*= NULL*/ )
{
    Frise* pFrise = (Frise*)m_pickingShape->getOwnerPtr();
	if (!pFrise)
		return;

	Vec3d rotationCenter;
	if (_pRotationCenter)
		rotationCenter = *_pRotationCenter;
	else
	{
		const RotationScaleInProgress* data = FriezeEditor::getInstance()->addRotationScaleData(pFrise);
		rotationCenter.m_x = data->m_center.m_x;
		rotationCenter.m_y = data->m_center.m_y;
		rotationCenter.m_z = pFrise->getDepth();
	}

    PickingInfo2D* pInfo = m_pickingShape->getPickingInfoPtr();
    FriezeExtraInfo extraInfo(pInfo->m_extraInformation);
    if(!extraInfo.m_flags.m_isPivot)
    {
        const Vec3d rotationCenterLocal = transformPointToLocalFriezePoint(pFrise, rotationCenter);
        const u32 index = extraInfo.m_flags.m_subItemIndex;
        pFrise->rotatePoint(index, _deltaAngle, rotationCenterLocal);
        if(extraInfo.m_flags.m_isEdge)
            pFrise->rotatePoint(index + 1, _deltaAngle, rotationCenterLocal);
    }
    else
	{
        pFrise->rotate(_deltaAngle, btrue, &rotationCenter);	// for initial pos
        pFrise->rotate(_deltaAngle, bfalse, &rotationCenter);	// for pos
	}
}


FriezeEditor::RotationScaleInProgress* FriezeEditor::getRotationScaleData(const Frise* _pFrise)
{
	ObjectRef ref = _pFrise->getRef();
	for (u32 i = 0; i < m_rotationScaleInProgress.size(); i++)
	{
		if (ref == m_rotationScaleInProgress[i].m_target)
			return &m_rotationScaleInProgress[i];
	}
	return NULL;
}

void FriezeEditor::removeRotationScaleData(const Frise* _pFrise)
{
	ObjectRef ref = _pFrise->getRef();
	for (u32 i = 0; i < m_rotationScaleInProgress.size(); i++)
	{
		if (ref == m_rotationScaleInProgress[i].m_target)
		{
			m_rotationScaleInProgress.eraseNoOrder(i);
			return;
		}
	}
	ITF_ASSERT_MSG(bfalse, "rotation data not found");
}

FriezeEditor::RotationScaleInProgress* FriezeEditor::addRotationScaleData(const Frise* _pFrise)
{
	RotationScaleInProgress* ret = getRotationScaleData(_pFrise);
	if (ret)
		return ret;

	ObjectRef ref = _pFrise->getRef();

	RotationScaleInProgress& data = m_rotationScaleInProgress.incrSize();
	data.m_target = ref;
	data.m_center = _pFrise->getAnchorPos().truncateTo2D();
	//data.m_center = _pFrise->getAABB().getCenter();
	return &data;
}

void FriezeEditor::PickingShapeData::scale( const Vec2d& _newScale )
{
    // Only scale if the shape used is the pivot's shape
    PickingInfo2D* pInfo = m_pickingShape->getPickingInfoPtr();
    FriezeExtraInfo extraInfo(pInfo->m_extraInformation);
    if(!extraInfo.m_flags.m_isPivot)
        return;

    Frise* pFrise = (Frise*)m_pickingShape->getOwnerPtr();

    m_prevHelperPos = FriezeEditor::PickingShapeData::scale( pFrise, this, _newScale, m_prevHelperPos, pFrise->getAnchorPos() );
}

Vec2d FriezeEditor::PickingShapeData::scale( Frise* _target, PickingShapeData_RotationScale* _scaler, const Vec2d& _newScale, const Vec2d& _prevHelperPos, const Vec3d& _scaleCenter)
{
    Vec2d current;
    if(!_scaler->getCurrentHelperUsedPos(current))
    {
        ITF_ASSERT_MSG(0, "Error in pickingShape plugin state");
        return _prevHelperPos;
    }

    _target->setScale(_newScale);
    _target->setWorldInitialScale(_newScale);

    Vec2d diffHelper = current - _prevHelperPos;
    if(diffHelper.sqrnorm() < MTH_EPSILON)
        return _prevHelperPos;

    // Get 2D Z of the object
    Vec3d pos2D;
    GFX_ADAPTER->compute3DTo2D(_target->getPos(), pos2D);

    // Project helpers at object Z
    Vec3d prevHelperPos, currentHelperPos;
    GFX_ADAPTER->compute2DTo3D(_prevHelperPos.to3d(pos2D.m_z), prevHelperPos);
    GFX_ADAPTER->compute2DTo3D(current.to3d(pos2D.m_z), currentHelperPos);

    prevHelperPos       -= _scaleCenter;
    currentHelperPos    -= _scaleCenter;

    f32 prevLen = prevHelperPos.truncateTo2D().norm();
    f32 currLen = currentHelperPos.truncateTo2D().norm();
    f32 coef    = currLen / prevLen;

    Vec2d scaleDelta = Vec2d::Zero;

    // Translate the point according to the used helpers
    switch(_scaler->m_state)
    {
        case State_ClickedScaleXBar:
        case State_ClickedScaleRotationXBar:
            scaleDelta.m_x = coef;
            break;

        case State_ClickedScaleYBar:
        case State_ClickedScaleRotationYBar:
            scaleDelta.m_y = coef;
            break;

        default:
            scaleDelta.m_x = coef;
            scaleDelta.m_y = coef;
            break;
    }

	RotationScaleInProgress* data = FriezeEditor::getInstance()->addRotationScaleData(_target);

    _target->scale(data->m_center, scaleDelta, btrue);
    return current;
}


void FriezeEditor::PickingShapeData::handleMouseWheel( i32 _wheelDelta )
{
    // Only modify under mouse shape
    PickingShape* pUnderMouse = EDITOR->getUnderMouseRealPickingShape();
    if(pUnderMouse != m_pickingShape)
        return;

    if(INPUT_ADAPTER->isKeyPressed(KEY_SPACE))
        return;

    // Only scale if the shape used is a subpoint
    PickingInfo2D* pInfo = m_pickingShape->getPickingInfoPtr();
    FriezeExtraInfo extraInfo(pInfo->m_extraInformation);
    if(extraInfo.m_flags.m_isEdge || extraInfo.m_flags.m_isPivot)
        return;

    Frise* pFrise = (Frise*)m_pickingShape->getOwnerPtr();

    if(UNDOREDO_MANAGER)
    {
        static f32 lastMouseWheelTime = ELAPSEDTIME;
        if(ELAPSEDTIME - lastMouseWheelTime < 0.125f)
        {
            UNDOREDO_MANAGER->preChange(pFrise);
        }
        lastMouseWheelTime = ELAPSEDTIME;
    }

    // to implement on frieze

    // reset scale A VIRER d'ici
    if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
    {
        if ( INPUT_ADAPTER->isKeyPressed(KEY_LALT))
        {
            pFrise->resetAllPointsScale();
            return;
        }

        pFrise->resetPointScaleAt(extraInfo.m_flags.m_subItemIndex);
        return;
    }

    f32 currScale = pFrise->getPointScaleAt(extraInfo.m_flags.m_subItemIndex);
    f32 newScale = currScale + _wheelDelta * 0.05f; //0.025f
    pFrise->setPointScaleAt(extraInfo.m_flags.m_subItemIndex, newScale);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void FriezeEditor::startMultiCol()
{
    m_holeMode = PolyLineEdge::Hole_Collision;
    setHoldState(HoldState_HolesFromMenu);
    m_multiCollisionEdition = btrue;
}

void FriezeEditor::onKey(   i32 _key, InputAdapter::PressStatus _status )
{
    Frise* pSelectedFrise = NULL;
    if (EDITOR && EDITOR->getSelectionSize())
    {
        PickingShape* pick = EDITOR->getSelectionAt(0);
        if (pick)
        {
            BaseObject* pObj = pick->getOwnerPtr();
            if (pObj && pObj->getObjectType() == BaseObject::eFrise)
            {
                pSelectedFrise = (Frise*)pObj;
            }
        }
    }

    if (_status == InputAdapter::Pressed)
    {
        switch (_key)
        {
            case KEY_LALT:
                m_showAllCollidableFriezes = ! m_showAllCollidableFriezes;
            break;

            case KEY_BACKSPACE:
                cancelCurrentAction(bfalse);
                m_multiCollisionEdition = bfalse;
                m_showAllCollidableFriezes = bfalse;
                setHoldState(HoldState_Nothing);
                break;

            case KEY_ESC:
                m_multiCollisionEdition = bfalse;
                m_showAllCollidableFriezes = bfalse;
                setHoldState(HoldState_Nothing);
                break;

            case 'C':
                if(
                    (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT)) ||
                    (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                    )
                {
                    if (pSelectedFrise)
                    {
                        pickColor(pSelectedFrise);
                        pickFogColor(pSelectedFrise);
                    }
                }
                break;

            case 'V':
                if(
                    (INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT)) ||
                    (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                    )
                {
                    pasteColor();
                    pasteFogColor();
                }
                break;

            default:
                break;
        }
    }

    if ( EDITOR )
    {
        bbool canAddPoints = bfalse;
        if (EDITOR->getSubEditedItem() == getCurFriezeRef() && m_currentSelectionIsLocked)
            canAddPoints = btrue;

        if (hasFriezeInfoFlag(FriezeInfoFlag_Any))
            canAddPoints = btrue;

        bbool addPointsUserInput = bfalse;
        if ((KEY_LSHIFT == _key) && (InputAdapter::Pressed == _status))
            addPointsUserInput = btrue;

        if (isHoldState(HoldState_AddPtsFromMenu))
            addPointsUserInput = btrue;

        if (canAddPoints && addPointsUserInput)
        {
            m_state = AddPoints;
        }
    }
}

void FriezeEditor::onObjectCreated( BaseObject* _pObject )
{
    if (_pObject && _pObject->getObjectType() == BaseObject::eFrise)
    {
        Frise* pFrise = (Frise*)_pObject;

        // Create the shape if :
        // - it's not already done (for exemple the object was in the sleep scene, then in an active scene, then in the sleep scene then in_pScene)
        if(EDITOR->getMainPickingShapeForObject(pFrise->getRef()) == NULL)
        {
            createPickingShapeForFrieze(pFrise);
        }

        pFrise->onEditorCreated();
    }
}

void FriezeEditor::onObjectChanged( BaseObject* _pObject )
{
    if (_pObject && _pObject->getObjectType() == BaseObject::eFrise)
    {
        Pickable* pFrise = (Pickable*)_pObject;
        pFrise->onEditorMove(EDITOR->isAllowStorePickableInitData(pFrise));
    }
}

void FriezeEditor::onObjectsDuplicated( const SafeArray <BaseObject*>& _from, const SafeArray <BaseObject*>& _to )
{
    const u32 count = _to.size();
    for(u32 iNew = 0; iNew < count; ++iNew)
    {
        BaseObject* objectTo = _to[iNew];
        Frise* friseTo = objectTo->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));

        // Only handle actors
        if(!friseTo)
            continue;

        PLUGINGATEWAY->onObjectCreated(friseTo);
    }
}

void FriezeEditor::closeCurrentFrieze(bbool _keepEditorStuff)
{
    setHoldState(HoldState_Nothing);

    if(!_keepEditorStuff)
    {
        m_state = Idle;
        if (m_currentSelectionIsLocked)
        {
            m_currentSelectionIsLocked = bfalse;

            if(EDITOR->getSubEditedItem() == getCurFriezeRef())
                EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
        }
        else
            setCurrentFrieze(NULL, btrue);
    }
}

FriezeEditor::FriezeEditor()
{
    setHoldState(HoldState_Nothing);
    m_creationRequestFrame = 0;
    m_buildFriezeNextClickDefaultConfig = "";
	m_curFriezeRef.invalidate();
    m_state = Idle;
    m_editorPeer = NULL;
    buildFriezeNextClick = bfalse;
    m_currentSelectionIsLocked = bfalse;
    m_cutRequestFrame = 0;
    m_cutIconTimer = -1.f;
    m_holeMode = PolyLineEdge::Hole_None;
    m_showFriezeInfoMask = 0;
    m_savedFriezeInfoMask = 0;
    FILEMANAGER->TranslatePath(m_lastSaveFriezePath, "");
    m_showInfoMode = ShowInfoMode_None;
    ITF_ASSERT(!m_instance);
    m_instance = this;
    m_multiCollisionEdition = bfalse;
}


void    FriezeEditor::setShowInfoMode(ShowInfoMode _mode)
{
    if (_mode == m_showInfoMode)
    {
        pushInfoFlags(btrue);
        m_showInfoMode = ShowInfoMode_None;
        return;
    }

    if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) || INPUT_ADAPTER->isKeyPressed(KEY_RCTRL))
    {
        switch(_mode)
        {
        case ShowInfoMode_None:
            break;
        case ShowInfoMode_MainAnchorOnly:
            switchFriezeInfoFlag(FriezeInfoFlag_MainAnchor);
            break;
        case ShowInfoMode_LinesOnly:
            switchFriezeInfoFlag(FriezeInfoFlag_Lines);
            break;
        case ShowInfoMode_SubAnchorsAndLines:
            switchFriezeInfoFlag(FriezeInfoFlag_Lines);
            break;
        default:
            ITF_ASSERT_MSG(bfalse, "unhandled case");
            break;
        }
    }
    else
    {
        clearFriezeInfoFlag(FriezeInfoFlag_Any);
        switch(_mode)
        {
        case ShowInfoMode_None:
            break;
        case ShowInfoMode_MainAnchorOnly:
            setFriezeInfoFlag(FriezeInfoFlag_MainAnchor);
            break;
        case ShowInfoMode_LinesOnly:
            setFriezeInfoFlag(FriezeInfoFlag_Lines);
            break;
        case ShowInfoMode_SubAnchorsAndLines:
            setFriezeInfoFlag(FriezeInfoFlag_SubAnchors | FriezeInfoFlag_Lines);
            break;
        default:
            ITF_ASSERT_MSG(bfalse, "unhandled case");
            break;
        }
        m_showInfoMode = _mode;
    }
    pushInfoFlags(bfalse);
}


void    FriezeEditor::addShowInfoMode(ShowInfoMode _mode)
{
    switch(_mode)
    {
    case ShowInfoMode_None:
        break;
    case ShowInfoMode_MainAnchorOnly:
        setFriezeInfoFlag(FriezeInfoFlag_MainAnchor);
        break;
    case ShowInfoMode_LinesOnly:
        setFriezeInfoFlag(FriezeInfoFlag_Lines);
        break;
    case ShowInfoMode_SubAnchorsAndLines:
        setFriezeInfoFlag(FriezeInfoFlag_SubAnchors | FriezeInfoFlag_Lines);
        break;
    default:
        ITF_ASSERT_MSG(bfalse, "unhandled case");
        break;
    }
    m_showInfoMode = _mode;
}

void    FriezeEditor::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
{
    String message;
    _blob.extractString(message);

    ITF_ASSERT(0); // unknown command
}

void FriezeEditor::cancelCurrentAction(bbool _keepEditorStuff)
{
    setHoldState(HoldState_Nothing);
    m_holeMode = PolyLineEdge::Hole_None;
    buildFriezeNextClick = bfalse;
    closeCurrentFrieze(_keepEditorStuff);
}

void FriezeEditor::switchSelectState()
{
    Frise* curPol = getCurrentFrieze();
    if (curPol)
    {
        if (m_currentSelectionIsLocked)
        {
            cancelCurrentAction(bfalse);
            return;
        }
        m_currentSelectionIsLocked = btrue;
        getCurrentFrieze()->setSelected(&m_currentFriezePickingInfo);
        PLUGINGATEWAY->setExclusiveFocus(this);
        return;
    }
}

void FriezeEditor::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
{
    if (_but == InputAdapter::MB_Left && _status == InputAdapter::Released)
		m_rotationScaleInProgress.clear();

    Frise* curPol = getCurrentFrieze();

    // Lock the selection => edit mode
    if (_but == InputAdapter::MB_Left && _status == InputAdapter::Double_Press)
    {
        if (curPol)
        {
            if (m_currentSelectionIsLocked)
            {
                if ( (m_state == Idle)&& (m_holdState == HoldState_Nothing))
                {
                    cancelCurrentAction(bfalse);
                    EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
                    EDITOR->selectObject(ITF_INVALID_OBJREF, btrue);
                    return;
                }
                else
                    _status = InputAdapter::Pressed;
            }
            else
            {
                enterSubEdit(curPol->getRef());
                return;
            }
        }
    }

    if (_but == InputAdapter::MB_Right && _status == InputAdapter::Released)
    {
#ifndef ITF_FINAL
        if (!isHoldState(HoldState_Nothing))
        {
            cancelCurrentAction(btrue);
            setHoldState(HoldState_Nothing);
            if (curPol)
            {
                EDITOR->setSubEditedItem(curPol->getRef());
                enterSubEdit(curPol->getRef());
                EDITOR->forceShowContextMenu(btrue, btrue, curPol->getRef(), btrue);
            }
            else
                EDITOR->forceShowContextMenu(bfalse);
        }
#endif
    }

    if (_status != InputAdapter::Pressed)
    {
        return;
    }

    if (_but == InputAdapter::MB_Left)
    {
        switch (m_state)
        {
        case AddPoints :
        case Create :
            if (m_creationRequestFrame < CURRENTFRAME-1)
            addPosToFrieze();
            break;
        case CutFrieze:
            cutFrieze();
            break;
        default:
            break;
        }
        switch (m_holdState)
        {
            case HoldState_DelPtsFromMenu:
                {
                    PickingShape* last = EDITOR->getLastClickedShape();
                    if (last && last->isSubAnchor() && last->getOwner() == getCurFriezeRef() && m_currentSelectionIsLocked)
                    {
                        EDITOR->deleteSelection();
                    }
                    else
                        setHoldState(HoldState_Nothing);
                }
                break;

            case  HoldState_SwitchTextureFromMenu:
                {
                    PickingShape* last = EDITOR->getLastClickedShape();

                    const PickingInfo2D* pPickingInfo = last ? last->getPickingInfoPtr() : NULL;
                    if(pPickingInfo && last->getOwner() == getCurFriezeRef() && m_currentSelectionIsLocked)
                    {
                        const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
                        if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
                        {
                            Frise* pol = (Frise*)last->getOwnerPtr();
                            if (pol)
                            {
                                UNDOREDO_MANAGER->preChange(pol);
                                pol->switchTextureAt(subItemIndex);
                                pol->switchTexturePipeExtremity();
                                PLUGINGATEWAY->onObjectChanged(pol);
                            }
                        }
                    }
                    else
                        setHoldState(HoldState_Nothing);
                }
                break;

            case HoldState_HolesFromMenu:
                {
                    PickingShape* last = EDITOR->getLastClickedShape();

                    const PickingInfo2D* pPickingInfo = last ? last->getPickingInfoPtr() : NULL;
                    if ((pPickingInfo && last->getOwner() == getCurFriezeRef() && m_currentSelectionIsLocked) || (pPickingInfo && m_multiCollisionEdition))
                    {
                        const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
                        if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
                        {
                            BaseObject* pOwnerObj = last->getOwnerPtr();
                            if (pOwnerObj && pOwnerObj->getObjectType() == BaseObject::eFrise)
                            {
                                Frise* pol = (Frise*)pOwnerObj;

                                {
                                    UNDOREDO_MANAGER->preChange(pol);
                                    pol->setHoleAt(subItemIndex, (PolyLineEdge::HoleMode)((u32)pol->getHoleAt(subItemIndex)^(u32)m_holeMode));
                                    PLUGINGATEWAY->onObjectChanged(pol);
                                }
                            }
                        }
                    }
                    else
                        setHoldState(HoldState_Nothing);
                }
                break;

            case HoldState_SetCustomPivot:
                {
                    Frise* pol = (Frise*)(m_userPivotStateTarget.getObject());
                    if (pol)
                    {
                        UNDOREDO_MANAGER->preChange(pol);
                        const Vec2d& mouse(EDITOR->getMouse2d());
                        Vec3d ref3d(pol->getPos());
                        Vec3d ref2d;
                        GFX_ADAPTER->compute3DTo2D(ref3d, ref2d);
                        ref2d.m_x = mouse.m_x;
                        ref2d.m_y = mouse.m_y;
                        GFX_ADAPTER->compute2DTo3D(ref2d, ref3d);
                        EDITOR->snap(ref3d, NULL, bfalse);
                        pol->setUserCustomPivot(ref3d.truncateTo2D());
                        PLUGINGATEWAY->onObjectChanged(pol);
                    }
                    setHoldState(HoldState_Nothing);
                    m_userPivotStateTarget.invalidate();
                    closeCurrentFrieze(bfalse);
                }

            default:
                break;
        }
    }


}

bbool FriezeEditor::bCanEnterEditMode ()
{
    return (!m_currentSelectionIsLocked);
}

bbool FriezeEditor::bCanLeaveEditMode ()
{
    return (!getCurrentFrieze());
}


void FriezeEditor::cutFriezeAtIndex( Frise* pBase, Vec2d _pos, i32 _index )
{
    if (!pBase)
        return;
    ITF_ASSERT((i32)pBase->getPosCount() > _index);


    const UndoRedoSequenceScope undoScope("Cut a polyline");
    Frise* pNew = static_cast<Frise*>(pBase->clone(pBase->getScene(), btrue));
    onObjectCreated(pNew);

    UNDOREDO_MANAGER->preChange(pBase);
    UNDOREDO_MANAGER->postAdd(pNew);

    while (pNew->getPosCount() > 2)
    {
        pNew->erasePosAt(0);
    }

    _index++;
    pNew->setPosAt(_pos, 0);
    pNew->setPosAt(pBase->getPosAt(_index), 1);
    bbool first = btrue;
    while((i32)pBase->getPosCount() > _index)
    {
        Vec2d pos = pBase->getPosAt(_index);
        if (first)
            first = bfalse;
        else
        {
            pNew->addPoint(pos);
            pNew->transferEdgeProperties(pNew->getPosCount()-1, pBase->getEdgeAt(_index));
        }
        pBase->erasePosAt(_index);
    }
    pBase->addPoint(_pos);

    pNew->setDepth(pBase->getDepth());
    pBase->forceRecomputeData();
    pNew->forceRecomputeData();
    m_cutIconTimer = 0.f;
    m_state = Idle;
	PLUGINGATEWAY->onObjectChanged(pBase);
	PLUGINGATEWAY->onObjectChanged(pNew);
}

void    FriezeEditor::cutFrieze()
{
    if (m_cutRequestFrame >= CURRENTFRAME-1)
        return;

    Frise* currentPolyLine = getCurrentFrieze();
    if (!currentPolyLine)
        return;

    Vec2d mouse(EDITOR->getMouse2d());
    Vec3d ref3d(currentPolyLine->getPos());
    Vec3d ref2d;
    GFX_ADAPTER->compute3DTo2D(ref3d, ref2d);
    ref2d.m_x = mouse.m_x;
    ref2d.m_y = mouse.m_y;
    GFX_ADAPTER->compute2DTo3D(ref2d, ref3d);
    Vec3d bestProj = ref3d;

    u32 selectedEdgeIndex = U32_INVALID;

    // Find the hovered edge
    {
        PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
        const PickingInfo2D* pPickingInfo = pShape ? pShape->getPickingInfoPtr() : NULL;
        if(pPickingInfo && pShape->getOwner() == currentPolyLine->getRef())
        {
            const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
            if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
            {
                selectedEdgeIndex = subItemIndex;
            }
        }
    }

    UNDOREDO_MANAGER->preChange(currentPolyLine);
    if (selectedEdgeIndex != U32_INVALID)
    {
        // cut original frieze
        PolyLineEdge edg = currentPolyLine->getEdgeAt(selectedEdgeIndex);
		bestProj = transformPointToLocalFriezePoint(currentPolyLine, bestProj);
        f32 dot = edg.m_normalizedVector.dot(Vec2d(bestProj.m_x, bestProj.m_y)-edg.getPos());
        Vec2d proj = edg.getPos() + edg.m_normalizedVector * dot;
        cutFriezeAtIndex(getCurrentFrieze(), proj, selectedEdgeIndex);
    }
}


PickingShape_Disc* FriezeEditor::getDiscAtIndex(Frise * _pFrise, u32 _index)
{
    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_pFrise->getRef());
    if (!shapes)
        return NULL;

    const u32 count = shapes->size();
    for (u32 index=0; index<count; index++)
    {
        PickingShape *shape = (*shapes)[index];
        if (!shape)
            continue;

        if (!shape->isSubAnchor())
            continue;

        if (shape->getShapeType() != PickingShape::ShapeType_Disc)
            continue;

        FriezeExtraInfo extraInfo;
        extraInfo.m_raw = shape->getData();
        if ((u32)extraInfo.m_flags.m_subItemIndex == _index)
            return (PickingShape_Disc*)shape;
    }
    return NULL;
}

void    FriezeEditor::addPosToFrieze()
{
    UndoRedoSequenceScope scope("addPosToFrieze");

    Frise* currentFrieze = getCurrentFrieze();

    if (buildFriezeNextClick)
    {
        Frise* pol = new Frise();

        Scene* pScene = EDITOR->getEditedScene();
        ITF_ASSERT(pScene);
        if (!pScene)
            return;

        pol->setPos(EDITOR->getMouseOnCurrentWorkingDepth());
        pol->setWorldInitialPos(EDITOR->getMouseOnCurrentWorkingDepth());
        pol->setDepth(EDITOR->getWorkingDepth().getReferenceDepth());
        pScene->registerPickable(pol);
        pScene->processRegisterPending();
        setCurrentFrieze(pol);
        buildFriezeNextClick = bfalse;
        m_currentSelectionIsLocked = btrue;
        if (m_buildFriezeNextClickDefaultConfig.getLen())
        {
            String newFrieze = m_buildFriezeNextClickDefaultConfig;
            pol->setFriseConfig(newFrieze);
        }

        bbool loopStatusWanted = btrue;
        if ( FriseConfig* config = pol->getConfig() )
        {
            switch ( config->m_methode)
            {
            case Frise::InExtremity:
            case Frise::InExtremitySimple:
            case Frise::InGeneric:
            case Frise::InFluid:
            case Frise::InString:
            case Frise::InAtlas:
            case Frise::InOverlay:
                loopStatusWanted = bfalse;
                break;

            default:
                loopStatusWanted = btrue;
                break;
            }
        }
        pol->setLoop(loopStatusWanted);

        PLUGINGATEWAY->onObjectCreated(pol);
        UNDOREDO_MANAGER->postAdd(pol);
        EDITOR->selectObject(pol->getRef(), btrue);
        EDITOR->setSceneAsModified(pScene);
        enterSubEdit(pol->getRef());
        setHoldState(HoldState_AddPtsFromMenu);

        m_state = AddPoints;
        currentFrieze = getCurrentFrieze();
    }
    else if (! (isHoldState(HoldState_AddPtsFromMenu) && currentFrieze))
    {
        PickingShape* pShape = EDITOR->getPickingShapeOfObjectUnderMouse();
        const PickingInfo2D* pPickingInfo = pShape ? pShape->getPickingInfoPtr() : NULL;
        if (pShape)
        {
            ObjectRef owner = pShape->getOwner();
            BaseObject* pOwner = owner.getObject();
            if(pPickingInfo && pOwner && pOwner->getObjectType() == BaseObject::eFrise)
            {
                currentFrieze = (Frise*)pOwner;
                setCurrentFrieze(currentFrieze);
            }
        }
    }

    if (!currentFrieze)
        return;

    const Vec2d& mouse(EDITOR->getMouse2d());
    Vec3d ref3d(currentFrieze->getPos());
    Vec3d ref2d;
    GFX_ADAPTER->compute3DTo2D(ref3d, ref2d);
    ref2d.m_x = mouse.m_x;
    ref2d.m_y = mouse.m_y;
    GFX_ADAPTER->compute2DTo3D(ref2d, ref3d);
    EDITOR->snap(ref3d, NULL, bfalse);

    Vec3d bestProj = ref3d;

    if (m_state == AddPoints || m_state == Create)
    {
        u32 selectedEdgeIndex = U32_INVALID;

        if (currentFrieze->getPosCount() == 0)
        {
            currentFrieze->setPos(EDITOR->getMouseOnCurrentWorkingDepth());
            currentFrieze->setWorldInitialPos(EDITOR->getMouseOnCurrentWorkingDepth());
            GFX_ADAPTER->drawDBGCircle(EDITOR->getMouseOnCurrentWorkingDepth(), 1.f, 1.f, 1.f, 1.f, 5.f);
            GFX_ADAPTER->drawDBGCircle(EDITOR->getMouseOnCurrentWorkingDepth(), 2.f, 1.f, 1.f, 1.f, 5.f);
            GFX_ADAPTER->drawDBGCircle(EDITOR->getMouseOnCurrentWorkingDepth(), 4.f, 1.f, 1.f, 1.f, 5.f);
        }


        PickingShape* pFoundEdge = NULL;
        // Find the hovered edge
        {
            pFoundEdge = EDITOR->getUnderMouseRealPickingShape();
            const PickingInfo2D* pPickingInfo = NULL;
            if (pFoundEdge)
            {
                pPickingInfo = pFoundEdge->getPickingInfoPtr();
            }
            if(pPickingInfo && pFoundEdge->getOwner() == currentFrieze->getRef())
            {
                const u32 subItemIndex = FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_subItemIndex;
                if(FriezeExtraInfo(pPickingInfo->m_extraInformation).m_flags.m_isEdge)
                {
                    selectedEdgeIndex = subItemIndex;
                }
                else
                {
                    pFoundEdge = NULL;
                }
            }
            else
                pFoundEdge = NULL;
        }

        if (currentFrieze->getPosCount() >  0)
            UNDOREDO_MANAGER->preChange(currentFrieze);
        if (pFoundEdge)
        {
            PickingShape_Edge* pEdge = (PickingShape_Edge*)pFoundEdge;
            FriezeExtraInfo extraInfo;
            extraInfo.m_raw = pEdge->getData();

            //project in final world space
            const PolyLineEdge &polyLineEdge = currentFrieze->getEdgeAt(extraInfo.m_flags.m_subItemIndex);
            const Vec2d &p1 = pEdge->getPt1();
            Vec2d direction = pEdge->getPt2()-p1;
            direction.normalize();
            f32 dot = direction.dot(Vec2d(bestProj.m_x, bestProj.m_y)-p1);
            //build new point in local space
            Vec2d proj = polyLineEdge.getPos() + polyLineEdge.m_normalizedVector * dot;
            currentFrieze->addPoint(proj, selectedEdgeIndex+1);

            m_currentFriezePickingInfo.m_extraInformation = selectedEdgeIndex+1;
        }
        else
        {
            bbool appendAtEnd = btrue;
            f32 depth = currentFrieze->getDepth();
            u32 neededCount = 1;
            if (currentFrieze->isLooping())
                neededCount++;
            if (currentFrieze->getPosCount() > neededCount)
            {
                Vec3d firstSubPointPos_Local;
                firstSubPointPos_Local.setFromVec2d(currentFrieze->getPosAt(0), &depth);
                Vec3d firstSubPointPos = transformFriezePoint(currentFrieze, firstSubPointPos_Local);

                u32 index  = currentFrieze->getPosCount()-1;
                if (currentFrieze->isLooping())
                    index--;
                Vec3d lastSubPointPos_Local;
                lastSubPointPos_Local.setFromVec2d(currentFrieze->getPosAt(index), &depth);

                Vec3d lastSubPointPos = transformFriezePoint(currentFrieze, lastSubPointPos_Local);

                f32 distToOrigin = (firstSubPointPos.truncateTo2D() - bestProj.truncateTo2D()).norm();
                f32 distToEnd = (lastSubPointPos.truncateTo2D() - bestProj.truncateTo2D()).norm();
                if (distToOrigin < distToEnd)
                    appendAtEnd = bfalse;
            }
            if (currentFrieze->isLooping() && currentFrieze->getPosCount() > 2)
            {
                currentFrieze->addPoint(transformPointToLocalFriezePoint(currentFrieze, bestProj).truncateTo2D(), currentFrieze->getPosCount()-1);
                m_currentFriezePickingInfo.m_extraInformation = currentFrieze->getPosCount()-2;
            }
            else
            {
                if (appendAtEnd)
                {
                    currentFrieze->addPoint(transformPointToLocalFriezePoint(currentFrieze,bestProj).truncateTo2D(), -1);
                    m_currentFriezePickingInfo.m_extraInformation = currentFrieze->getPosCount()-1;
                }
                else
                {

                    m_currentFriezePickingInfo.m_extraInformation = 0; // select 1st point
                    currentFrieze->addPoint(currentFrieze->getPosAt(currentFrieze->getPosCount()-1), -1);
                    for (u32 i = currentFrieze->getPosCount()-1; i > 0 ; i--)
                    {
                        currentFrieze->setPosAt(currentFrieze->getPosAt(i-1), i);
                    }
                    currentFrieze->setPosAt(transformPointToLocalFriezePoint(currentFrieze,bestProj).truncateTo2D(), 0);
                }
            }
        }
		if (currentFrieze->getPosCount() > 1 && m_state == Create)
			m_state = AddPoints;

		PLUGINGATEWAY->onObjectChanged(currentFrieze);
        EDITOR->endDrag();
    }
}

void FriezeEditor::createFrise()
{
    m_creationRequestFrame = CURRENTFRAME;
    buildFriezeNextClick = btrue;
    setCurrentFrieze(NULL);
    PLUGINGATEWAY->setExclusiveFocus(this);

    m_state = Create;
}

void FriezeEditor::UnselectAll(bbool /*_force*/)
{
    closeCurrentFrieze(bfalse);

    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString(COMMAND_WATCH);
    blob.pushInt32(0);
    PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}

void FriezeEditor::enterSubEdit( ObjectRef _or )
{
    BaseObject* bo = GETOBJECT(_or);
    if (!bo || bo->getObjectType() != BaseObject::eFrise)
        return;

    Frise* _pFrise = static_cast<Frise*>(bo);
    lockFrieze(_pFrise);
    EDITOR->setSubEditedItem(_or);
}

bbool FriezeEditor::isSubEditable( const BaseObject* _pObj )const
{
    if(_pObj && _pObj->getObjectType() == BaseObject::eFrise)
        return btrue;

    return bfalse;
}

void FriezeEditor::update()
{
    // Create icons for runtime added actors
    for(u32 iObj = 0; iObj < m_addedActiveFriezesLastFrame.size(); ++iObj)
    {
        ObjectRef objRef = m_addedActiveFriezesLastFrame[iObj];
        Frise* pFrise = static_cast<Frise*>(objRef.getObject());
        if(pFrise && !EDITOR->getPickingShapesForObject(objRef))
            createPickingShapeForFrieze(pFrise);
    }
    m_addedActiveFriezesLastFrame.clear();

    // Basic inits
    const Plugin* focus = PLUGINGATEWAY->getExclusiveFocus();
    Frise* currentPolyLine = getCurrentFrieze();
    if (currentPolyLine && m_currentSelectionIsLocked && (!focus))
        PLUGINGATEWAY->setExclusiveFocus(this);

    if (!currentPolyLine || !m_currentSelectionIsLocked)
    {
        if (!isHoldState(HoldState_SetCustomPivot) && !m_multiCollisionEdition)
        setHoldState(HoldState_Nothing);
    }

    if (focus)
    {
        if (focus == this)
        {
            if (currentPolyLine)
            {
            }
            else if (!buildFriezeNextClick)
                PLUGINGATEWAY->setExclusiveFocus(NULL);
        }
        else
            setCurrentFrieze(NULL);
    }

    // draw icons
    if (m_cutIconTimer > -1.f)
    {
        m_cutIconTimer += LOGICDT;
        EDITOR->showMouseIcon(Editor_IconStock::Icon_Interrogation, (1.f + m_cutIconTimer) * (1.f + m_cutIconTimer) * 16.f);
        if (m_cutIconTimer > 0.15f)
            m_cutIconTimer = -1.f;
    }

    if ((m_state == AddPoints) && (!INPUT_ADAPTER->isKeyPressed(CREATE_POINT_KEY)) && (!isHoldState(HoldState_AddPtsFromMenu)))
        m_state = Idle;


    if (buildFriezeNextClick || isHoldState(HoldState_SetCustomPivot))
    {
        // Preview the future snapped position
        Vec3d snappedMouseWorld = EDITOR->getMouse3d();
        EDITOR->snap(snappedMouseWorld, NULL, bfalse);
        Vec3d snappedMouseScreen;
        GFX_ADAPTER->compute3DTo2D(snappedMouseWorld, snappedMouseScreen);
        Vec2d snappedMouse = snappedMouseScreen.truncateTo2D();

        if (isHoldState(HoldState_SetCustomPivot))
            EDITOR->showMouseIcon(Editor_IconStock::Icon_Frise, 0.0f, &snappedMouse);
        else
        EDITOR->showMouseIcon(Editor_IconStock::Icon_New, 0.0f, &snappedMouse);
    }


    if (!currentPolyLine && !m_multiCollisionEdition)
    {
        if (!buildFriezeNextClick)
        {
            m_state = Idle;
            m_currentSelectionIsLocked = bfalse;
        }
        return;
    }

    // Preview the future snapped position
    Vec3d snappedMouseWorld = EDITOR->getMouse3d();
    EDITOR->snap(snappedMouseWorld, NULL, bfalse);
    Vec3d snappedMouseScreen;
    GFX_ADAPTER->compute3DTo2D(snappedMouseWorld, snappedMouseScreen);
    Vec2d snappedMouse = snappedMouseScreen.truncateTo2D();

    switch(m_state)
    {
        case AddPoints:
                EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLine_NewPoint, 0.0f, &snappedMouse);
            break;
        case Create:
                EDITOR->showMouseIcon(Editor_IconStock::Icon_Plus, 0.0f, &snappedMouse);
            break;
        case CutFrieze:
                EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLine_Cut);
            break;
        default:
            if (isHoldState(HoldState_DelPtsFromMenu))
                EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLinePoint_Delete);
            else if (isHoldState(HoldState_HolesFromMenu))
                EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLine_Hole);
            else if (isHoldState(HoldState_SwitchTextureFromMenu))
                EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLine_Hole);
        break;
    }
}

bbool FriezeEditor::onPickShape(PickingShape* _shape)
{
    if (m_state != Idle && m_state != AddPoints)
    {
        return bfalse;
    }

    Pickable* pp  = (Pickable*)_shape->getOwnerPtr();

    if (!pp)
        return bfalse;

    if (pp->getObjectType() != BaseObject::eFrise)
        return bfalse;

    Frise* oldCurrent = getCurrentFrieze();
    if (EDITOR->getState() == Editor::EditorState_Idle)
    {
        if ((oldCurrent) && (oldCurrent != pp) && m_currentSelectionIsLocked)
            return bfalse;
    }

    setCurrentFrieze((Frise*)pp);
    ITF_Memcpy(&m_currentFriezePickingInfo, _shape->getPickingInfoPtr(), sizeof(PickingInfo2D));
    pp->setSelected(&m_currentFriezePickingInfo);

    if(pp->getObjectType() == BaseObject::eFrise)
        PLUGINGATEWAY->onFrisePicked(pp->getRef()); // inform other plugins

    return btrue;
}

bbool FriezeEditor::onEditorRemove( const SafeArray<PickingShape*>& _shapesWithSameOwner   )
{
    SafeArray<u32> ptsToRemove;
    bbool bResult = bfalse;
    Frise* pFrise = NULL;
    Scene* pScene = NULL;
    Pickable* pPickable = NULL;
    bbool deleteFrieze = bfalse;

    for(u32 i = 0; i < _shapesWithSameOwner.size(); ++i)
    {
        if(PickingShape* pShape = _shapesWithSameOwner[i])
        {
            pPickable = static_cast<Pickable*>(pShape->getOwnerPtr());
            if(!pPickable) // This should never append
                break;

            pFrise = pPickable->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
            if(!pFrise) // This part of code only handle shape with owner type Frise
                break;

            pScene = pFrise->getScene();
            ITF_ASSERT(pScene);
            if(!pScene)  // This should never happen
                break;

            if(pShape->isSubAnchor())
            {
                const FriezeExtraInfo info = FriezeExtraInfo(pShape->getPickingInfoPtr()->m_extraInformation);
                const u32 index = info.m_flags.m_subItemIndex;
                ptsToRemove.push_back(index);
            }
            else
            {
                deleteFrieze = btrue;
                break;
            }
        }
    }

    if(pFrise)
    {
        const u32 remainingPtsAfterRemove = pFrise->getPointsData().getPosCount() - ptsToRemove.size();
        const u32 minRemaining = 1 + (pFrise->isLooping() ? 1 : 0);
        if(remainingPtsAfterRemove <= minRemaining)
            deleteFrieze = btrue;
    }

    if(deleteFrieze && pScene)
    {
        PLUGINGATEWAY->onObjectDeleted(pFrise);
        UNDOREDO_MANAGER->preDelete(pFrise);
        EDITOR->setSceneAsModified(pScene);
        pScene->unregisterPickable(pFrise);
        EDITOR->deletePickingShapesForObject(pFrise->getRef());
        bResult = btrue;
        pFrise = NULL;
    }
    else if(pFrise)
    {
        const UndoRedoSequenceScope scope("Remove frieze's sub points");

        ptsToRemove.quickSort(); // increasing order

        // reverse loop
        for(i32 i = ptsToRemove.size() - 1; i >= 0; --i)
        {
            const u32 index = ptsToRemove[i];

            UNDOREDO_MANAGER->preChange(pFrise);

            // If this call unregisters the frise from the scene
            ITF_VERIFY(!pFrise->erasePosAt(index));
        }

        PLUGINGATEWAY->onObjectChanged(pFrise);
        bResult = btrue;
    }

    return bResult;
}

Frise* FriezeEditor::getCurrentFrieze() const
{
	return static_cast<Frise*>(getCurFriezeRef().getObject());
}

void FriezeEditor::setCurrentFrieze( Frise* _pNewPolyline, bbool _allowUnselect /*= btrue*/ )
{
    Frise* oldPolyline = getCurrentFrieze();
    if (EDITOR && oldPolyline && (_pNewPolyline != oldPolyline))
    {
        if(oldPolyline->getRef() == EDITOR->getSubEditedItem())
            EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
    }

    if (!_pNewPolyline)
    {
        if (_allowUnselect && oldPolyline)
            oldPolyline->setSelected(NULL);
        m_curFriezeRef.invalidate();
        if (PLUGINGATEWAY->hasExclusiveFocus(this))
            PLUGINGATEWAY->setExclusiveFocus(NULL);
    }
    else
    {
        setCurFriezeRef(_pNewPolyline->getRef());
        m_currentFriezePickingInfo.m_extraInformation = 0;
        m_currentFriezePickingInfo.m_targetRef = getCurFriezeRef();
    }
}

void FriezeEditor::lockFrieze( Frise* _p, PickingInfo2D* _pPickingInfo2D /*= null*/)
{
    if(!_p)
        return;

    if(_pPickingInfo2D)
        ITF_MEMCOPY(&m_currentFriezePickingInfo, &_pPickingInfo2D, sizeof(PickingInfo2D));

    setCurrentFrieze(_p);
    m_currentSelectionIsLocked = btrue;

    PLUGINGATEWAY->setExclusiveFocus(this);

    EDITOR->removeMainShapeFromSelection(_p->getRef());
}


Vec3d FriezeEditor::transformFriezePoint(const Frise *frieze, const Vec3d &_p)
{
	Vec3d res;
    if (frieze->isDynamic())
	{
		Matrix44 m;
		frieze->computeDynamicDisplayMatrix(m);
		M44_matrixCoordTransform(&res, &m, &_p);
	}
	else
	{
		res = frieze->transformPos(_p);
	}
    return res;
}

Vec3d FriezeEditor::transformPointToLocalFriezePoint(const Frise *_line, const Vec3d &_p)
{
    return _line->inverseTransform(_p - _line->transformPos(Vec3d::Zero));
}

void FriezeEditor::updateFriezePickingShape(ObjectRef _or, ObjectRefList &_objects, PickingShapeVector& _orderedShapes)
{
    const BaseObject* pObj = GETOBJECT(_or);

    if(!pObj || pObj->getObjectType() != BaseObject::eFrise)
        return;

    Frise* pFrise = (Frise*)pObj;

    if (FriezeEditor::isMultiCollisionEdition(pFrise))
    {
        if (!pFrise->isVisible(CAMERA) )
             return;

        const SafeArray<PolyLine*>* pCollisionData = pFrise->getCollisionData_WorldSpace();
        if ( !pCollisionData || !pCollisionData->size() )
            return;
    }
    else
    {
        if (!EDITOR->checkObjectVisible(_or))
            return;
    }

    bbool fitsMultiCollision = m_multiCollisionEdition;
    u32 basePrio = 0;
    bbool someoneIsSubEditedAndThatsNotMe = bfalse;
    bbool bSubEdited = EDITOR->isSubEdited(pObj->getRef());
    if (!bSubEdited && EDITOR->getSubEditedItem().getObject())
        someoneIsSubEditedAndThatsNotMe = btrue;

    if (someoneIsSubEditedAndThatsNotMe)
        basePrio = PickingShape::m_UnselectablePriority;


	Vec2d basePos = pFrise->get2DPos();

    bbool isSelected = pFrise->isSelected() || fitsMultiCollision;

    const bbool showShapes =
        EditorDraw::PickableDetail_CanShowAnchors() ||
        isSelected ||
        getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_Any) ||
        fitsMultiCollision;

    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_or);
    _objects.push_back(pObj->getRef());

    ITF_ASSERT(shapes);
    if (shapes)
    {
        u32 expectedShapesCount  = pFrise->getPosCount() * 2; // ( 1 for anchor + pts count for points + pts count - 1 for edges)
        if (pFrise->isLooping())
            expectedShapesCount--;

        if (expectedShapesCount != shapes->size())
        { // handle the case when a point has been added or deleted to the Frieze
            bbool isCurrentlySelected = pFrise->isSelected();
            static PickingInfo2D selectionInfo;

            if (isCurrentlySelected)
            {
                selectionInfo.m_anchor = Vec2d::Zero;
                selectionInfo.m_angle = 0.f;
                selectionInfo.m_extraInformation = 0;
                selectionInfo.m_sqrDist2D = 0.f;
                selectionInfo.m_targetRadius = 1.f;
                selectionInfo.m_targetRef = _or;
                selectionInfo.m_type = BaseObject::eFrise;
                selectionInfo.m_zDist = 0.f;
            }

            EDITOR->deletePickingShapesForObject(_or);
            if (isCurrentlySelected)
            {
                pFrise->setSelected(&selectionInfo);
            }
            createPickingShapeForFrieze(pFrise);
            shapes = EDITOR->getPickingShapesForObject(_or);

            // If the object is subedited it must stay selected, so reselect it (because old shapes were deleted)
            if(EDITOR->getSubEditedItem() == _or)
            {
                EDITOR->selectObject(_or, btrue);
            }
        }
        const SafeArray<PickingShape*>& shapesList = *shapes;
        u32 iShape = 0;

        {  // don't include subshapes if not sub-editing the frieze
            ITF_ASSERT(shapesList[iShape]->getShapeType() == PickingShape::ShapeType_Disc);
            PickingShape_Disc* pick = (PickingShape_Disc*)(shapesList[iShape++]);

            // update pos shape
            ITF_ASSERT(!pick->isSubAnchor());
            pick->setPos(pFrise->getAnchorPos());
			RotationScaleInProgress* rotData = FriezeEditor::getInstance()->getRotationScaleData(pFrise);
			if (rotData)
			{
				Vec3d center;
				center.m_x = rotData->m_center.m_x;
				center.m_y = rotData->m_center.m_y;
				center.m_z = pFrise->getDepth();
	            pick->setPos(center);
			}

            pick->setlastUpdateFrame(CURRENTFRAME);
            pick->setPriority(basePrio + 1);

            const bbool subEdited = EDITOR->isSubEdited(pObj->getRef());
            if (showShapes && !subEdited && (getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_MainAnchor) || isSelected))
            {
                if (getInstance()->getShowInfoMode() != ShowInfoMode_SubAnchorsAndLines && !fitsMultiCollision) // don't display main anchor in that mode
                _orderedShapes.push_back(pick);
            }

            ((Pickable*)pFrise)->setPickingInfo(pick->getPickingInfoPtr());

            if (pFrise->getConfig() && !subEdited && !getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_SubAnchors) && !fitsMultiCollision)
                return; // add only main anchor except if:
                        // - the frieze is a collision only frieze with no visual (no config)
                        // - the frieze is a triggerShape frieze with no visual
                        // - the frieze is subedited
                        // - the info flags want to see all anchors
        }

        const f32 fZ = pFrise->getDepth();

        // update sub shapes for edges
        for (i32 iEdge = 0; iEdge < i32(pFrise->getPosCount()) - 1; iEdge++)
        {
            ITF_ASSERT(shapesList[iShape]->getShapeType() == PickingShape::ShapeType_Edge);
            PickingShape_Edge* pick = (PickingShape_Edge*)(shapesList[iShape++]);
            const PolyLineEdge &edge = pFrise->getEdgeAt(iEdge);
            Vec3d point1, point2;

			point1.setFromVec2d(edge.getPos(), &fZ);
            point2.setFromVec2d(edge.getPos()+edge.m_vector, &fZ);
            point1 = transformFriezePoint(pFrise, point1);
            point2 = transformFriezePoint(pFrise, point2);

            pick->setPts((Vec2d&)point1, (Vec2d&)point2);
            pick->setDepth(fZ/*point1.m_z*/);

            pick->setlastUpdateFrame(CURRENTFRAME);
            if(showShapes)
            {
                pick->setPriority(basePrio + 2);
                _orderedShapes.push_back(pick);
            }
        }

        // update sub shapes for points
        i32 ptsCount = i32(pFrise->getPosCount());
        if (pFrise->isLooping() && ptsCount > 2)
            ptsCount--;
        for (i32 iPoint = 0; iPoint < ptsCount; iPoint++)
        {
            if (iShape < shapesList.size())
            {
                ITF_ASSERT(shapesList[iShape]->getShapeType() == PickingShape::ShapeType_Disc);
                PickingShape_Disc* pick = (PickingShape_Disc*)(shapesList[iShape++]);
			    Vec3d resultPos = transformFriezePoint(pFrise, (pFrise->getPosAt(iPoint)).to3d());
                pick->setPos(resultPos);
                pick->setlastUpdateFrame(CURRENTFRAME);
                if(showShapes)
                {
                    pick->setPriority(basePrio + 2);
                    _orderedShapes.push_back(pick);
                }
            }
        }
    }
}

void FriezeEditor::fillEditorForcedList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes)const
{
    if (m_state == AddPoints && getCurrentFrieze() && m_currentSelectionIsLocked)
        updateFriezePickingShape(getCurFriezeRef(), _objects, _orderedShapes);
}

void FriezeEditor::fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene )const
{
    if (Editor::is2DSelection())
        return;

    if (!getInstance()->hasFriezeInfoFlag(FriezeInfoFlag_Any))
    {
        if (getCurrentFrieze() && m_currentSelectionIsLocked)
        {
            updateFriezePickingShape(getCurFriezeRef(), _objects, _orderedShapes);
            return;
        }

        if(EDITOR->getSubEditedItem() != ITF_INVALID_OBJREF)
            return;
    }

    const World* pWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
    if (!pWorld)
        return;

    BaseObjectList pObjList;
    if(_pFilterScene)
        pObjList = _pFilterScene->getActiveObjects();
    else
    {
        Scene* pRoot = pWorld->getRootScene();
        pObjList.mergeArray(pRoot->getActiveObjects());

        // Recursive feed the while monster
        ObjectRefList ssas = pRoot->getSubSceneActors();
        for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
            if(BaseObject* pBO = ssas[iSSA].getObject())
                if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    if(Scene* pSubScene = pSSA->getSubScene())
                        if(EDITOR->allowEditSubScene(pSSA))
                        {
                            pObjList.mergeArray(pSubScene->getActiveObjects());
                            ssas.mergeArray(pSubScene->getSubSceneActors()); // feed the loop
                        }
    }

    // For each selected ssa|grp display content and selected frises and persistant subscene
    ObjectRefList additionalDisplayable = EDITOR->getEditorSelectionAsObjRefList();
    additionalDisplayable.mergeArray(EDITOR->getSubSceneDisplayList());
    const u32 uAddDisplayableSize = additionalDisplayable.size();
    for(u32 iList = 0; iList < uAddDisplayableSize; ++iList)
    {
        ObjectRef ownerRef = additionalDisplayable[iList];
        if(Pickable* pObj = static_cast<Pickable*>(ownerRef.getObject()))
        {
            switch(pObj->getObjectType())
            {
                case BaseObject::eFrise:
                        pObjList.addObject(ownerRef, bfalse);
                break;

                case BaseObject::eActor:
                    if(SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        if(Scene* pSubScene = pSSA->getSubScene())
                        {
                            if(EDITOR->allowEditSubScene(pSSA))
                            {
                        const BaseObjectList& subList = pSubScene->getActiveObjects();
                        ID_SERVER->getObjectListNoNullV2(subList);

                        for(u32 iSub = 0; iSub < subList.m_solvedPointers.size(); ++iSub)
                        {
                            BaseObject* pSubObj = subList.m_solvedPointers[iSub];
                            switch(pSubObj->getObjectType())
                            {
                                case BaseObject::eFrise:
                                        pObjList.addObject(pSubObj->getRef(), bfalse);
                                break;

                                default:
                                break;
                            }
                        }
                    }
                    }
                    }
                break;

                default:
                break;
            }
        }
    }

    ID_SERVER->getObjectListNoNullV2(pObjList);
    const u32 uCount = pObjList.m_solvedPointers.size();
    for(u32 i = 0; i < uCount; ++i)
    {
        const BaseObject* pObj = pObjList.m_solvedPointers[i];

        if(pObj->getObjectType() != BaseObject::eFrise)
            continue;


     /*   bbool forceUpdate = bfalse;
        if ((m_state == AddPoints) || (m_state == Create))
        {
            if (getCurrentFrieze() == pFrise)
                forceUpdate = btrue;
        }
        if (&forceUpdate && !EDITOR->checkObjectVisible(oRef))
            continue;
*/
        updateFriezePickingShape(pObj->getRef(), _objects, _orderedShapes);
    }
}


bbool FriezeEditor::isPickingAllowed()
{
/*    if (m_multiCollisionEdition)
        return bfalse;*/
    switch (m_state)
    {
    case AddPoints:
    case CutFrieze:
    case Create:
        return bfalse;

    default:
    return btrue;
    }
}

void FriezeEditor::onClearScene   (   )
{
    setCurrentFrieze(NULL);
}

void FriezeEditor::onFileDrop (const String& _fileName)
{
    const String ext = FilePath::getExtension(_fileName);
    if (ext == "fcg")
    {
        String relativePath;
        if (!FILESERVER->getPathFromDataFolder(relativePath, _fileName))
            return;
        m_buildFriezeNextClickDefaultConfig = relativePath.cStr();

        createFrise();
    }
    else if(ext == "frz")
    {
        insertFriezeFRZ(_fileName);
    }
    else if(ext.isEmpty()) // folder
    {
        String toLocal(_fileName);
        FilePath::transformToRelative(toLocal);
        replaceCurrentSelectionKit(toLocal);
    }
}

bbool FriezeEditor::fillContextMenu(EditorContextMenu& _menu, ObjectRef _target, bbool _topItems)
{
    Frise* pol = NULL;
    BaseObject* tgt = GETOBJECT(_target);
    if (!tgt)
        return btrue;

    pol = tgt->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));

    if (_topItems)
    {
        if (m_state == Create)
        {
            _menu.addItem(EditorContextMenu::ItemId_FriezeDone, "Done");
        }
    }
    else
    {
        if (m_state == Create)
            return btrue;

        if (pol && pol->getObjectType() == BaseObject::eFrise)
        {
            EditorContextMenu* Menu_Edit = NULL;
#ifdef DAN_TEXTURE_SELECTOR
            EditorContextMenu* Menu_SamplerPicker = NULL;
#endif
            u32 friezeCount = 0;
            u32 subAnchorCount = 0;
            if (EDITOR->getSelectionSize() > 1)
            {
                for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
                {
                    PickingShape* shape = EDITOR->getSelectionAt(iSel);
                    if (shape)
                    {
                        BaseObject* owner = shape->getOwnerPtr();
                        if (owner && owner->getObjectType() == BaseObject::eFrise)
                        {
                            if (shape->isSubAnchor())
                                subAnchorCount++;
                            else
                                friezeCount++;
                        }
                    }
                }
            }

            // LEVEL 0
            if (_target == EDITOR->getSubEditedItem())
            {
                Menu_Edit   = &_menu;
                // EDIT
                // ctrl pts
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_AddPts, "Add (shift)");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_DeletePts, "Delete (del)");
                Menu_Edit->addItem(EditorContextMenu::ItemId_CutFrieze, "Split Frieze");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeInvertPoints, "Invert points");
                Menu_Edit->addSubItem(EditorContextMenu::ItemId_FriezeHole_Visual, "Visual Only",EditorContextMenu::ItemId_FriezeHole, "Create/Delete Holes");
                Menu_Edit->addSubItem(EditorContextMenu::ItemId_FriezeHole_Collision, "Collision Only",EditorContextMenu::ItemId_FriezeHole, "Create/Delete Holes");
                Menu_Edit->addSubItem(EditorContextMenu::ItemId_FriezeHole_Both, "Visual & Collision",EditorContextMenu::ItemId_FriezeHole, "Create/Delete Holes");
                Menu_Edit->addItem(EditorContextMenu::ItemId_Frieze_SwitchTexture, "Switch Texture");

                if ( FriseConfig* config = pol->getConfig() )
                {
                    switch ( config->m_methode)
                    {
                    case Frise::InExtremity:
                    case Frise::InExtremitySimple:
                    case Frise::InAtlas:
                    break;

                    default:
                        {
                            if ( pol->isLooping())
                                Menu_Edit->addItem(EditorContextMenu::ItemId_LoopFrieze, "open");
                            else
                                Menu_Edit->addItem(EditorContextMenu::ItemId_LoopFrieze, "close");
                        }
                    break;
                    }
                }
            }
            else
            {
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Export, "Export", EditorContextMenu::ItemId_File, "File");
                _menu.addSubItem(EditorContextMenu::ItemId_FindInExplorer, "Find configuration in databank", EditorContextMenu::ItemId_File, "File");
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_ReplaceConfig, "Replace configuration", EditorContextMenu::ItemId_File, "File");
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_ReplaceKit, "Replace kit", EditorContextMenu::ItemId_File, "File");
#ifdef DAN_TEXTURE_SELECTOR
                Menu_SamplerPicker   = &_menu.addItem(EditorContextMenu::ItemId_SamplerPicker, "Sampler Picker")->m_subMenu;
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemID_FindFriseLocation, "Find File");
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemId_ReCook_RSampler, "ReCook Rsampler");
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemId_ReCook_Sampler, "ReCook Sampler");
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemId_ReCook_USampler, "ReCook USampler");
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemId_ReCook_ESRGAN, "ReCook ESRGAN");
                Menu_SamplerPicker->addItem(EditorContextMenu::ItemID_ReCook_Reset, "ReCook Reset");
#endif
                Menu_Edit    = &_menu.addItem(EditorContextMenu::ItemId_Edit, "Edit")->m_subMenu;


                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeToggleDynamic, "Toggle dynamic");

                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_CopyProperties, "Pick Color (Shift+C)");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_PasteProperties, "Paste Color (Shift+V)");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_CopyFogProperties, "Pick Fog");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_PasteFogProperties, "Paste Fog");
                Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeEd_CtrlPts, "Control points");
                Menu_Edit->addItem(EditorContextMenu::ItemId_PropertiesEditor, "Properties Editor (Ctrl+Q)");
                Menu_Edit->addItem(EditorContextMenu::ItemId_LaunchFromExplorer, "Configuration");
                {
                    // pivot
                    ContextMenuItem* center =  Menu_Edit->addSubItem(EditorContextMenu::ItemId_CenterFriezePivot, "Center", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    ContextMenuItem* topleft =  Menu_Edit->addSubItem(EditorContextMenu::ItemId_TopLeftFriezePivot, "Top Left", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    ContextMenuItem* topright =  Menu_Edit->addSubItem(EditorContextMenu::ItemId_TopRightFriezePivot, "Top Right", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    ContextMenuItem* bottomleft =  Menu_Edit->addSubItem(EditorContextMenu::ItemId_BottomLeftFriezePivot, "Bottom Left", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    ContextMenuItem* bottomright =  Menu_Edit->addSubItem(EditorContextMenu::ItemId_BottomRightFriezePivot, "Bottom Right", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    Menu_Edit->addSubItem(EditorContextMenu::ItemId_UserCustomFriezePivot, "Custom", EditorContextMenu::ItemId_FriezePivot, "Pivot");
                    Menu_Edit->addSubItem(EditorContextMenu::ItemId_RepairPivot, "Repair", EditorContextMenu::ItemId_FriezePivot, "Pivot");

                    switch(((Frise*)pol)->getPivotMode())
                    {
                        case Frise::PivotMode_Center:
                            center->setInactiveColor();
                            break;
                        case Frise::PivotMode_TopLeft:
                            topleft->setInactiveColor();
                            break;
                        case Frise::PivotMode_TopRight:
                            topright->setInactiveColor();
                            break;
                        case Frise::PivotMode_BottomLeft:
                            bottomleft->setInactiveColor();
                            break;
                        case Frise::PivotMode_BottomRight:
                            bottomright->setInactiveColor();
                            break;
                        default:
                            break;
                    }
                }

                // transform
                Menu_Edit->addSubItem(EditorContextMenu::ItemId_Transform_XMirror, "X Mirror", EditorContextMenu::ItemId_Transform, "Transform");
                Menu_Edit->addSubItem(EditorContextMenu::ItemId_Transform_YMirror, "Y Mirror", EditorContextMenu::ItemId_Transform, "Transform");

                if (friezeCount > 1)
                {
                    if (friezeCount == 2)
                        Menu_Edit->addItem(EditorContextMenu::ItemId_SnapFriezes, "Snap friezes pivot");
                }

                // edit
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Copy, "Copy (ctrl+c)", EditorContextMenu::ItemId_Edit, "Edit");
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Paste, "Paste (ctrl+v)", EditorContextMenu::ItemId_Edit, "Edit");
                _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Delete, "Delete (del)", EditorContextMenu::ItemId_Edit, "Edit");
                ContextMenuItem* item = _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Merge, "Merge", EditorContextMenu::ItemId_Edit, "Edit");
                item->setInactiveColor();
                item = _menu.addSubItem(EditorContextMenu::ItemId_FriezeEd_Replace, "Replace", EditorContextMenu::ItemId_Edit, "Edit");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_FriezeEd_Replace_MP, "Match pivot");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_FriezeEd_Replace_MC, "Match center");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_FriezeEd_Replace_MT, "Match top-left");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_FriezeEd_Replace_No, "Match nothing");
            }

            if (subAnchorCount > 1)
            {
                ContextMenuItem* item = Menu_Edit->addItem(EditorContextMenu::ItemId_AlignFriezePoints, "Align subpoints");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_AlignFriezePointsX, "X");
                item->m_subMenu.addItem(EditorContextMenu::ItemId_AlignFriezePointsY, "Y");
            }

            // get shape under mouse
            PickingShape* pShapeUnderMouse = EDITOR->getUnderMouseRealPickingShape();
            if ( pShapeUnderMouse &&
                pShapeUnderMouse->isSubAnchor() &&
                pShapeUnderMouse->getOwnerPtr()->getObjectType() == BaseObject::eFrise &&
                pShapeUnderMouse->getShapeType() == PickingShape::ShapeType_Disc )
            {
                if (pol && pol->isLooping())
                    Menu_Edit->addItem(EditorContextMenu::ItemId_FriezeSetAtFisrtPoint, "Set as 1st point");
            }

        }
    }

    return btrue;
}

void FriezeEditor::onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget)
{
    if (_item->isExternal())
        return;

    BaseObject* bo = GETOBJECT(_menuTarget);
    if (!bo)
        return;

    if (bo->getObjectType() == BaseObject::eScene)
    {
        switch(_item->getId())
        {
            case EditorContextMenu::ItemId_FriezeDone: // this entry is duplicated on purpose
                m_state = Idle;
                buildFriezeNextClick = bfalse;
            break;
        }
        return;
    }

    if (bo->getObjectType() == BaseObject::eFrise)
    {
        Frise* target = static_cast<Frise*>(bo);

        switch(_item->getId())
        {
        case EditorContextMenu::ItemId_FriezeEd_Paste:
        case EditorContextMenu::ItemId_FriezeEd_Copy:
            ITF_ERROR("Not implemented yet. Use ctrl+c / ctrl+v");
                break;

        case EditorContextMenu::ItemId_FriezeEd_AddPts:
            EDITOR->setSubEditedItem(target->getRef());
            enterSubEdit(_menuTarget);
            setHoldState(HoldState_AddPtsFromMenu);
            m_state = AddPoints;
            break;

        case EditorContextMenu::ItemId_FriezeEd_DeletePts:
            EDITOR->setSubEditedItem(target->getRef());
            enterSubEdit(_menuTarget);
            setHoldState(HoldState_DelPtsFromMenu);
            break;

        case EditorContextMenu::ItemId_FriezeHole_Collision:
            setHoleMode(_menuTarget, PolyLineEdge::Hole_Collision);
            break;

        case EditorContextMenu::ItemId_FriezeHole_Visual:
            setHoleMode(_menuTarget, PolyLineEdge::Hole_Visual);
            break;

        case EditorContextMenu::ItemId_FriezeHole_Both:
            setHoleMode(_menuTarget, PolyLineEdge::Hole_Both);
            break;

        case EditorContextMenu::ItemId_Frieze_SwitchTexture:
            setSwitchTextureMode(_menuTarget);
            break;

        case EditorContextMenu::ItemId_AlignFriezePointsX:
        {
            UndoRedoSequenceScope scope("ItemId_AlignFriezePointsX");
            bbool posfound = bfalse;
            f32 xToApply = 0.f;
             for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(iSel);
                if (shape)
                {
                    BaseObject* owner = shape->getOwnerPtr();
                    if (owner && owner->getObjectType() == BaseObject::eFrise)
                    {
                        if (shape->isSubAnchor())
                        {
                            UNDOREDO_MANAGER->preChange(((Frise*)owner));
                            const FriezeExtraInfo* extraInfo = (FriezeExtraInfo*)shape->getPickingInfoPtr();
                            const u32 index = extraInfo->m_flags.m_subItemIndex;
                            Vec2d pos = ((Frise*)owner)->getPosAt(index);
                            if (posfound)
                            {
                                pos.m_x = xToApply;
                                ((Frise*)owner)->setPosAt(pos, index);
                            }
                            else
                            {
                                posfound = btrue;
                                xToApply = pos.m_x;
                            }
                        }
                    }
                }
            }
            break;
        }

        case EditorContextMenu::ItemId_AlignFriezePointsY:
        {
            UndoRedoSequenceScope scope("ItemId_AlignFriezePointsY");
            bbool posfound = bfalse;
            f32 yToApply = 0.f;
             for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(iSel);
                if (shape)
                {
                    BaseObject* owner = shape->getOwnerPtr();
                    if (owner && owner->getObjectType() == BaseObject::eFrise)
                    {
                        if (shape->isSubAnchor())
                        {
                            UNDOREDO_MANAGER->preChange(((Frise*)owner));
                            const FriezeExtraInfo* extraInfo = (FriezeExtraInfo*)shape->getPickingInfoPtr();
                            const u32 index = extraInfo->m_flags.m_subItemIndex;
                            Vec2d pos = ((Frise*)owner)->getPosAt(index);
                            if (posfound)
                            {
                                pos.m_y = yToApply;
                                ((Frise*)owner)->setPosAt(pos, index);
                            }
                            else
                            {
                                posfound = btrue;
                                yToApply = pos.m_y;
                            }
                        }
                    }
                }
            }
            break;
        }

        case EditorContextMenu::ItemId_SnapFriezes:
        {
            Frise* pFrise[2];
            u32 friezeCount = 0;
            for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(iSel);
                if (shape)
                {
                    BaseObject* owner = shape->getOwnerPtr();
                    if (owner && owner->getObjectType() == BaseObject::eFrise)
                    {
                        pFrise[friezeCount++] = (Frise*)owner;
                        if (friezeCount >=2 )
                            break;
                    }
                }
            }
            if (friezeCount == 2)
            {
                Frise* pMovedFrise = pFrise[1];
                UNDOREDO_MANAGER->preChange(pMovedFrise);
                Vec3d delta = pFrise[0]->getAnchorPos() - pMovedFrise->getAnchorPos();
                pMovedFrise->setPos(pMovedFrise->getPos() + delta);
                PLUGINGATEWAY->onObjectChanged(pMovedFrise);
            }
        }
        break;

        case EditorContextMenu::ItemId_FriezeToggleDynamic:
            UNDOREDO_MANAGER->preChange(target);
            target->setDynamic(!target->isDynamic());
            PLUGINGATEWAY->onObjectChanged(target);
            target->onEditorMove();
            break;

        case EditorContextMenu::ItemId_FriezeEd_CopyProperties:
            pickColor(target);
            break;

        case EditorContextMenu::ItemId_FriezeEd_PasteProperties:
            pasteColor();
            break;

        case EditorContextMenu::ItemId_FriezeEd_CopyFogProperties:
            pickFogColor(target);
            break;

        case EditorContextMenu::ItemId_FriezeEd_PasteFogProperties:
            pasteFogColor();
            break;

        case EditorContextMenu::ItemId_FriezeEd_CtrlPts:
        {
            u32 friezeCount = 0;
            for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
            {
                PickingShape* shape = EDITOR->getSelectionAt(iSel);
                if (shape)
                {
                    BaseObject* owner = shape->getOwnerPtr();
                    if (owner && owner->getObjectType() == BaseObject::eFrise)
                        friezeCount++;
                }
            }
            {
                EDITOR->setSubEditedItem(target->getRef());
                enterSubEdit(_menuTarget);
                EDITOR->forceShowContextMenu(btrue, btrue, target->getRef(), btrue);
            }
        }
            break;


        case EditorContextMenu::ItemId_CenterFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setPivotMode(Frise::PivotMode_Center);
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;

        case EditorContextMenu::ItemId_TopLeftFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setPivotMode(Frise::PivotMode_TopLeft);
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;

        case EditorContextMenu::ItemId_TopRightFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setPivotMode(Frise::PivotMode_TopRight);
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;

        case EditorContextMenu::ItemId_BottomLeftFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setPivotMode(Frise::PivotMode_BottomLeft);
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;

        case EditorContextMenu::ItemId_BottomRightFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setPivotMode(Frise::PivotMode_BottomRight);
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;

        case EditorContextMenu::ItemId_UserCustomFriezePivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                m_currentSelectionIsLocked = btrue;
                setCurrentFrieze(target);
                setHoldState(HoldState_SetCustomPivot);
                m_userPivotStateTarget = target->getRef();
                enterSubEdit(target->getRef());
            }
        break;

        case EditorContextMenu::ItemId_RepairPivot:
            {
                UNDOREDO_MANAGER->preChange(target);
                m_currentSelectionIsLocked = btrue;
                setCurrentFrieze(target);
                target->repairPivot();
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;


        case EditorContextMenu::ItemId_Transform_XMirror:
            mirror(target, btrue, bfalse);
        break;

        case EditorContextMenu::ItemId_Transform_YMirror:
            mirror(target, bfalse, btrue);
        break;

        case EditorContextMenu::ItemId_CutFrieze:
                Plugin_KatanaCut::setMode(Plugin_KatanaCut::CutMode_Active);
        break;

        case EditorContextMenu::ItemId_FriezeInvertPoints:
            UNDOREDO_MANAGER->preChange(target);
            target->invertPoints();
            PLUGINGATEWAY->onObjectChanged(target);
        break;

        case EditorContextMenu::ItemId_LoopFrieze:
            {
                UNDOREDO_MANAGER->preChange(target);
                target->setLoop(!target->isLooping());
                PLUGINGATEWAY->onObjectChanged(target);
            }
        break;


        case EditorContextMenu::ItemId_FriezeDone: // this entry is duplicated on purpose
            m_state = Idle;
            buildFriezeNextClick = bfalse;
        break;

        case EditorContextMenu::ItemId_EditFrieze:
            switchSelectState();
        break;

        case EditorContextMenu::ItemId_FriezeEd_Delete:
            EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
            EDITOR->selectObject(_menuTarget, btrue);
            EDITOR->deleteSelection();
            break;
        case EditorContextMenu::ItemId_FriezeEd_Merge:
            ITF_ERROR("not yet impemented");
            break;
        case EditorContextMenu::ItemId_FriezeEd_Export:
            {
                String path;
                target->getConfigNamePath().getString(path);
                path = FilePath::getDirectory(path);
                path += "*.frz";
                if (FILEMANAGER->fileChooser(btrue, path, L"Frieze (*.frz)\0*.frz\0"))
                {
                    String pathRel = path;
                    FilePath::transformToRelative(pathRel);
                    m_lastSaveFriezePath = FilePath::getDirectory(pathRel);

                    pugi::xml_document doc;

                    pugi::xml_node root = doc.append_child( "root" );

                    CSerializerObjectParser parser(root);
                    target->Serialize(&parser,ESerialize_Data_Save);
                    doc.save_file( StringConverter(path).getChar() );
                }
            }
            break;

        case EditorContextMenu::ItemId_FriezeEd_Replace_MP:
        case EditorContextMenu::ItemId_FriezeEd_Replace_MC:
        case EditorContextMenu::ItemId_FriezeEd_Replace_MT:
        case EditorContextMenu::ItemId_FriezeEd_Replace_No:
            {
                String path;
                target->getConfigNamePath().getString(path);
                path = FilePath::getDirectory(path);
                path += "*.frz";
                if (FILEMANAGER->fileChooser(bfalse, path, L"Frieze (*.frz)\0*.frz\0"))
                {
                    FilePath::transformToRelative(path);

                    PugiXMLWrap XML;

                    if (XML.openFile(path,bfalse, PugiXMLWrap::LogWarning))
                    {
                        const UndoRedoSequenceScope undoScope("replace Frieze");

                        // retrieve data from previous frieze and delete it
                        Vec3d prevPivot = target->getPos();
                        Vec3d prevCenter = target->getAABB().getCenter().to3d();
                        Vec3d prevTopLeft = Vec3d(target->getAABB().getMin().m_x,  target->getAABB().getMax().m_y, 0.f);
                        Scene* pScene = target->getScene();
                        ITF_ASSERT(pScene);
                        ObjectId prevID = target->getObjectId();
                        EDITOR->deleteSelection();

                        // create new frieze
                        Frise* pol = new Frise();
                        CSerializerObjectParser parser(&XML);
                        pol->Serialize(&parser,ESerialize_Data_Load);
                        pol->forceObjectId(prevID);
                        if (pScene)
                        {
                            pScene->registerPickable(pol);
                            pScene->processRegisterPending();
                        }
                        setCurrentFrieze(pol);
                        pol->onLoaded();

                        // apply position modifications
                        if (_item->getId() == EditorContextMenu::ItemId_FriezeEd_Replace_MP)
                        {
                            Vec3d newPivot = pol->getPos();
                            pol->offsetPosition(prevPivot-newPivot, btrue);
                            pol->setDepth(prevPivot.m_z);
                            pol->recomputeData();
                        }
                        else if (_item->getId() == EditorContextMenu::ItemId_FriezeEd_Replace_MC)
                        {
                            Vec3d newCenter = pol->getAABB().getCenter().to3d();
                            pol->offsetPosition(prevCenter-newCenter, btrue);
                            pol->setDepth(prevPivot.m_z);
                            pol->recomputeData();
                        }
                        else if (_item->getId() == EditorContextMenu::ItemId_FriezeEd_Replace_MT)
                        {
                            Vec3d newTopLeft = Vec3d(pol->getAABB().getMin().m_x,  pol->getAABB().getMax().m_y, 0.f);
                            pol->offsetPosition(prevTopLeft-newTopLeft, btrue);
                            pol->setDepth(prevPivot.m_z);
                            pol->recomputeData();
                        }
                        UNDOREDO_MANAGER->postAdd(pol);
                        EDITOR->setSceneAsModified(pScene);
                    }
                }
            }
            break;

            case EditorContextMenu::ItemId_FriezeEd_ReplaceKit:
            {
                String dest;
                String path;
                target->getConfigNamePath().getString(path);
                path = FilePath::getDirectory(path);
                path.replace(DATA_FOLDER, "");
                if (FILEMANAGER->folderChooser("CHOOSE KIT PATH", path, dest))
                {
                    replaceCurrentSelectionKit(dest);
                }

            }
            break;

            case EditorContextMenu::ItemId_FriezeEd_ReplaceConfig:
            {
                String path;
                target->getConfigNamePath().getString(path);
                path = FilePath::getDirectory(path);
                path += "*.fcg";
                if (FILEMANAGER->fileChooser(bfalse, path, L"Frieze config (*.fcg)\0*.fcg\0"))
                {
                    UndoRedoSequenceScope scope("ItemId_FriezeEd_ReplaceConfig");

                    FilePath::transformToRelative(path);
                    replaceOneFriezeConfig(target, path);
                    if (EDITOR->getSelectionSize() > 1)
                    {
                        for (u32 iSel = 0; iSel < EDITOR->getSelectionSize(); iSel++)
                        {
                            PickingShape* shape = EDITOR->getSelectionAt(iSel);
                            if (shape)
                            {
                                BaseObject* owner = shape->getOwnerPtr();
                                if (owner && owner->getObjectType() == BaseObject::eFrise)
                                {
                                    replaceOneFriezeConfig((Frise*)owner, path);
                                }
                            }
                        }
                    }
                }
            }
            break;

            case EditorContextMenu::ItemId_FriezeSetAtFisrtPoint:
            {
                PickingShape* pShapeUnderMouse = EDITOR->getUnderMouseRealPickingShape();
                if ( pShapeUnderMouse &&
                    pShapeUnderMouse->isSubAnchor() &&
                    pShapeUnderMouse->getOwnerPtr()->getObjectType() == BaseObject::eFrise &&
                    pShapeUnderMouse->getShapeType() == PickingShape::ShapeType_Disc )
                {
                    const FriezeExtraInfo data2(pShapeUnderMouse->getData());

                    UNDOREDO_MANAGER->preChange(target);
                    target->setAsFirstPoint( data2.m_flags.m_subItemIndex);
                    PLUGINGATEWAY->onObjectChanged(target);
                }
            }
            break;
#ifdef DAN_TEXTURE_SELECTOR
        case EditorContextMenu::ItemID_FindFriseLocation:
            {
                ITF_VECTOR<String> paths = GetTexturePathFromFrise(target);
                for (u32 u = 0; u < paths.size();u++)
                {
                    String& path = paths[u];
                    if (FILEMANAGER->fileExists(path))
                    {
                        String fullPath;
                        if(FILEMANAGER->TranslatePath(fullPath, path))
                        {
                            fullPath.replace('/', '\\');
                            SystemAdapter::findInExplorer(fullPath);
                        }
                    }
                }
                break;
            }
        case EditorContextMenu::ItemId_ReCook_USampler:
            RecookWithSampler(target,samplerFiles[SAMPLER_TYPE::USAMPLER]);
            break;
        case EditorContextMenu::ItemId_ReCook_RSampler:
            RecookWithSampler(target,samplerFiles[SAMPLER_TYPE::RSAMPLER]);
            break;
        case EditorContextMenu::ItemId_ReCook_Sampler:
            {
                RecookWithSampler(target,samplerFiles[SAMPLER_TYPE::SAMPLER]);
                break;
            }
        case EditorContextMenu::ItemId_ReCook_ESRGAN:
            {
                RecookWithSampler(target,samplerFiles[SAMPLER_TYPE::ESRGAN]);
                break;
            }
        case EditorContextMenu::ItemID_ReCook_Reset:
            {
                RecookReset(GetTexturePathFromFrise(target));
                break;
            }
#endif
        }

        return;
    }
}
#ifdef DAN_TEXTURE_SELECTOR

    void FriezeEditor::RecookWithSampler(Frise* target,String& sampler)
    {
        if (!target) return;
        Vector<String> paths = GetTexturePathFromFrise(target);
        RecookReset(paths);
        bbool foundSampler = bfalse;
        for (u32 u = 0; u < paths.size();u++)
        {
            if (FILEMANAGER->fileExists(paths[u]))
            {

                String folderPath = FilePath::getDirectory(paths[u]);
                String fileNameWithoutExtension =FilePath::getFilenameWithoutExtension(paths[u]);
                bbool hasExitSamplerFile = bfalse;
                for (const String& extension : samplerExtensions)
                {
                    if (FILEMANAGER->fileExists(folderPath + fileNameWithoutExtension +"_"+ sampler +"."+extension))
                    {
                        hasExitSamplerFile = btrue;
                    }
                }
                if (!hasExitSamplerFile)
                {
                    continue;
                }
                foundSampler = btrue;
                for (const String& extension : samplerExtensions)
                {
                    for (const String& samplerfile : samplerFiles)
                    {
                        if (sampler.equals(samplerfile,true)) continue;
                        String newPath;
                        FILESERVER->getAbsolutePath(folderPath + fileNameWithoutExtension +"_"+ samplerfile +"."+extension,newPath);
                        renameFile(newPath,newPath+".delete",false);
                    }
                }

                FILEMANAGER->flushTimeWriteAccess(paths[u],true);
            }
        }
        if (!foundSampler)
        {
            ITF_ERROR_SHOW(true, "sampler %ls not found !", sampler.cStr());
        }
        PLUGINGATEWAY->onObjectChanged(target);
    }
    ITF_VECTOR<String> FriezeEditor::GetTexturePathFromFrise(Frise* target)
    {
        ITF_VECTOR<String> paths;
        if (!target) return paths;
        ResourceGroup::ResourceList list = target->getResourceGroup()->getResourcesList();
        for (u32 u = 0; u < list.size();u++)
        {
            const FriseConfig* res = static_cast<FriseConfig*>(list[u].getResource());
            if (res)
            {
                for (u32 i = 0; i < res->m_textureConfigs.size();i++)
                {
                    FriseTextureConfig textureConfig = res->m_textureConfigs[i];
                    String texturePath =  textureConfig.getPath();

                    if (!texturePath.isEmpty() && FILEMANAGER->fileExists(texturePath))
                    {
                        const String ext = FilePath::getExtension(texturePath);
                        if (ext =="anm")
                        {
                            if (auto resource = static_cast<AnimTrackResource*>(textureConfig.m_textureData.getResource()))
                            {
                                for (auto pathID : resource->m_texturePathList)
                                {
                                    paths.push_back(ANIM_MANAGER->getString(pathID));
                                }
                            }
                        }
                        else
                        {
                            paths.push_back(texturePath);
                        }

                    }
                }
            }
        }
        return paths;
    }

#endif
void FriezeEditor::setSwitchTextureMode(ObjectRef _target)
{
    EDITOR->setSubEditedItem(_target);
    enterSubEdit(_target);
    setHoldState(HoldState_SwitchTextureFromMenu);
}


void FriezeEditor::setHoleMode(ObjectRef _target, PolyLineEdge::HoleMode _holeMode)
{
    m_holeMode = _holeMode;
    EDITOR->setSubEditedItem(_target);
    enterSubEdit(_target);
    setHoldState(HoldState_HolesFromMenu);
}

void FriezeEditor::createPickingShapeForFrieze (const Frise* _frieze)
{
    ObjectRef objRef = _frieze->getRef();
    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(objRef);
    if (shapes)
    {
        ITF_ASSERT_MSG(0, "Trying to create shapes for an object that already has shapes!");
        EDITOR->deletePickingShapesForObject(objRef);
    }

    // create main shape for pivot
    {
        PickingShape_Disc* pick = newAlloc(mId_Editor, PickingShape_Disc(NULL));
        pick->setPos(_frieze->getAnchorPos());
        pick->setSubAnchor(bfalse);
        FriezeExtraInfo extraInfo;
        extraInfo.m_flags.m_subItemIndex = 0;
        extraInfo.m_flags.m_isPivot = 1;
        extraInfo.m_flags.m_isEdge = bfalse;
        pick->setData(extraInfo.m_raw); // add point index
        EDITOR->addPickingShapeForObject(objRef, pick);
        pick->setPriority(1);
    }

    const f32 fZ = _frieze->getDepth();

    // create sub shapes for edges
    for (i32 i = 0; i < i32(_frieze->getPosCount()) - 1; i++)
    {
        PickingShape_Edge* pick = newAlloc(mId_Editor, PickingShape_Edge(NULL));

        Vec3d point1, point2;
        const PolyLineEdge &edge = _frieze->getEdgeAt(i);
        point1.setFromVec2d(edge.getPos(), &fZ);
        point2.setFromVec2d(edge.getPos()+edge.m_vector, &fZ);
        point1 = transformFriezePoint(_frieze, point1);
        point2 = transformFriezePoint(_frieze, point2);

        pick->setPts((Vec2d&)point1, (Vec2d&)point2);
        pick->setDepth(point1.m_z);

        pick->setSubAnchor(btrue);
        FriezeExtraInfo extraInfo;
        extraInfo.m_flags.m_subItemIndex = i;
        extraInfo.m_flags.m_isEdge = btrue;
        extraInfo.m_flags.m_isPivot = 0;
        pick->setData(extraInfo.m_raw); // push the edge index
        EDITOR->addPickingShapeForObject(objRef, pick);
        pick->setPriority(2);
    }

    // create sub shapes for points
    i32 ptsCount = i32(_frieze->getPosCount());
    if (_frieze->isLooping() && ptsCount > 2)
        ptsCount--;
    for (i32 i = 0; i < ptsCount; i++)
    {
        PickingShape_Disc* pick = newAlloc(mId_Editor, PickingShape_Disc(NULL));
        pick->setPos(transformFriezePoint(_frieze,_frieze->getPosAt(i).to3d(fZ)));
        pick->setSubAnchor(btrue);
        FriezeExtraInfo extraInfo;
        extraInfo.m_flags.m_subItemIndex = i;
        extraInfo.m_flags.m_isEdge = bfalse;
        extraInfo.m_flags.m_isPivot = 0;

        pick->setData(extraInfo.m_raw); // add point index
        EDITOR->addPickingShapeForObject(objRef, pick);
        pick->setPriority(2);
    }
}

PickingShapePluginData* FriezeEditor::createPickingShapeData( PickingShape *_shape )
{
    if(_shape->getShapeType() == PickingShape::ShapeType_Disc && _shape->getOwnerPtr()->getObjectType() == BaseObject::eFrise)
        return newAlloc(mId_Editor,FriezeEditor::PickingShapeData(_shape, this));
    else
        return NULL;
}

void FriezeEditor::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
{
    if(!_bSwitchDone)
        UnselectAll(btrue);
}

void FriezeEditor::onDeleteScene(const Scene* _pScene)
{
    World* pWorld = CURRENTWORLD;
    if(pWorld && pWorld->hasScene(_pScene))
    {
        Frise* polyline = getCurrentFrieze();
        if(polyline && polyline->getScene() == _pScene)
        {
            UnselectAll(btrue);
        }
    }

    if(EDITOR)
    {
        const PickableList& friezes = _pScene->getFrises();
        const u32 uSize = friezes.size();
        for (u32 i = 0; i < uSize; i++)
        {
            EDITOR->deletePickingShapesForObject(friezes[i]->getRef());
        }
    }
}

void FriezeEditor::onObjectSpawned (const BaseObject* _newObject)
{
    if (const Pickable* pPickable = _newObject->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
    {
        Scene* pScene = pPickable->getScene();
        if(pScene->isActive())
        {
            if(const Frise* pFrise = _newObject->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664)))
            {
                createPickingShapeForFrieze(pFrise);
            }
        }
    }
}

bbool FriezeEditor::isHoldState(HoldState _state)
{
    return _state == m_holdState;
}

void FriezeEditor::setHoldState(HoldState _state)
{
    if (!m_multiCollisionEdition)
        m_holdState = _state;
}

void FriezeEditor::onPostSceneActivation( const Scene* _pScene, bbool _bActive )
{
   ITF_ASSERT_MSG(_pScene, "Invalid parameters");

    if(_bActive)
    {
        const PickableList& frises = _pScene->getFrises();
        for(u32 i = 0; i < frises.size(); ++i)
        {
            const Frise* pFrise = static_cast<const Frise*>(frises[i]);
            if(EDITOR->getMainPickingShapeForObject(pFrise->getRef()) == NULL)
                createPickingShapeForFrieze(pFrise);
        }
    }
}

u32 FriezeEditor::getFriezeInfoMask()
{
    return EDITOR->getPluginsInfoFlags() & m_showFriezeInfoMask;
}

bbool FriezeEditor::hasFriezeInfoFlag(u32 _flag)
{
    return (m_showFriezeInfoMask & _flag & EDITOR->getPluginsInfoFlags()) != 0;
}

bbool FriezeEditor::insertFriezeFRZ( const String& _absolutePath )
{
    String temp;
    if (!FILESERVER->getPathFromDataFolder(temp, _absolutePath))
        return bfalse;

    Scene* pDestScene = EDITOR->getEditedScene();
    if(!pDestScene)
        return bfalse;

    PugiXMLWrap XML;

    StringConverter cvt(temp);
    if (XML.openFile(cvt.getChar(),bfalse, PugiXMLWrap::LogWarning))
    {
        Vec3d ref3d = EDITOR->getMouseOnCurrentWorkingDepth();

        const UndoRedoSequenceScope undoScope("load Frieze");

        Frise* pol = new Frise();
        CSerializerObjectParser parser(&XML);
        pol->Serialize(&parser,ESerialize_Data_Load);
        pol->regenerateObjectId(); // avoid doublond on ObjectID when drag'n dropping several times the same .frz
        pDestScene->registerPickable(pol);
        pDestScene->processRegisterPending();
        pol->offsetPosition(ref3d - pol->getPos(), btrue);
        pol->setDepth(EDITOR->getWorkingDepth().getReferenceDepth());
        setCurrentFrieze(pol);
        pol->onLoaded();

        UNDOREDO_MANAGER->postAdd(pol);
        EDITOR->setSceneAsModified(pDestScene);
        return btrue;
    }
    return bfalse;
}

Vec3d transformToLocalCoordinates(Frise *_line, const Vec3d &_p)
{
    Frise *f = _line;
	Vec3d res;
	Matrix44 dynamicTransform;
    if (f->isDynamic())
		_line->computeDynamicDisplayMatrix(dynamicTransform);
	else
		_line->computeStaticDisplayMatrix(dynamicTransform);

	Matrix44 dynamicNoTrans(dynamicTransform);
	Matrix44 inverseMatrix;
	dynamicNoTrans.setLineAt(3,0,0,0,1);
	M44_matrixTranspose(&inverseMatrix, &dynamicNoTrans);
	Float4 translation = makeFloat4(dynamicTransform.fa16[12],dynamicTransform.fa16[13],dynamicTransform.fa16[14],1.f);
	Float4 invTrans;
	M44_matrixCoordTransform(&invTrans, &inverseMatrix, &translation);
	Vec3d &invTrans3=(Vec3d&)invTrans;
	inverseMatrix.setLineAt(3,-invTrans3.m_x, -invTrans3.m_y, -invTrans3.m_z,1.f);

	M44_matrixCoordTransform(&res, &inverseMatrix, &_p);
	return res;
}

Vec3d transformToLocalTranslation(Frise *_line, const Vec3d &_p)
{
    Frise *f = _line;

    //We use fast inversion, because we assume we have an orthogonal matrix. This is for accuracy
    Vec3d res;
    Matrix44 dynamicTransform;
    if (f->isDynamic())
		_line->computeDynamicDisplayMatrix(dynamicTransform);
	else
		_line->computeStaticDisplayMatrix(dynamicTransform);

    Matrix44 dynamicNoTrans(dynamicTransform);

    Matrix44 inverseMatrix;
    dynamicNoTrans.setLineAt(3,0,0,0,1);
    M44_matrixTranspose(&inverseMatrix, &dynamicNoTrans);

    M44_matrixVec3dTransform(&res, &inverseMatrix, &_p);
    return res;
}

void FriezeEditor::mirror(Frise* _target, bbool _x, bbool _y)
{
    UNDOREDO_MANAGER->preChange(_target);
    _target->mirror(_x, _y, _target->getAnchorPos().truncateTo2D());
    PLUGINGATEWAY->onObjectChanged(_target);
}

bbool FriezeEditor::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
{
    if (m_multiCollisionEdition || (m_holdState != HoldState_Nothing))    // don't allow moving points on holdstate
        return bfalse;

    BaseObject* pOwner = _pPickingShape->getOwnerPtr();
    if(pOwner->getObjectType() != BaseObject::eFrise)
        return bfalse;

    Frise* pFrieze = pOwner->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
    if (!pFrieze)
        return bfalse;

    PickingInfo2D* pPickInfo = _pPickingShape->getPickingInfoPtr();
    const bbool moveParent = !_pPickingShape->isSubAnchor();

    ITF_ASSERT(pPickInfo->m_targetRef == pFrieze->getRef());
    pFrieze->setPickingInfo(pPickInfo);
    const FriezeExtraInfo extraInfo = FriezeExtraInfo(pPickInfo->m_extraInformation);

    const f32 z = pFrieze->getDepth();
    Vec3d p1NewAnchorPos3D;
    // pPickInfo->m_anchor is the 2D delta between the mouse and the pickin shape's position (vector from mouse to picking shape)
    const Vec3d p1NewAnchorPos2D(_screenspacepos.m_x + pPickInfo->m_anchor.m_x, _screenspacepos.m_y + pPickInfo->m_anchor.m_y, pPickInfo->m_zDist);
    GFX_ADAPTER->compute2DTo3D(p1NewAnchorPos2D, p1NewAnchorPos3D);
    Vec3d vdelta;

    const bbool canChangeInitData = EDITOR->isAllowStorePickableInitData(pFrieze);

    if (moveParent)
    {
        ITF_ASSERT(extraInfo.m_flags.m_isPivot);
        Vec3d finalPos = p1NewAnchorPos3D - (pFrieze->getAnchorPos() - pFrieze->getPos());

        finalPos.m_z = z;
	    pFrieze->setPos(finalPos);
        if(canChangeInitData)
	        pFrieze->setLocalInitialPos(pFrieze->getLocalPos());
/*
        GFX_ADAPTER->drawDBGCircle(p1NewAnchorPos3D, 0.3f, Color::yellow(), 5);
        GFX_ADAPTER->drawDBGCircle(finalPos, 0.4f, Color::black(), 5);
        GFX_ADAPTER->drawDBGCircle(finalPos + pFrieze->getLocalPos(), 0.5f, Color::green(), 5);
*/
    }
    else
    {
        ITF_ASSERT(!extraInfo.m_flags.m_isPivot);
        Vec3d newAnchorLocalPos = transformToLocalCoordinates(pFrieze, p1NewAnchorPos3D);
        const i32 iPosCount = pFrieze->getPosCount();
        const u32 index = extraInfo.m_flags.m_subItemIndex;
        ITF_ASSERT(!extraInfo.m_flags.m_isPivot);

        if (extraInfo.m_flags.m_isEdge && (index < u32(iPosCount-1)))
        {
            Vec2d vDelta = pFrieze->getPosAt(index+1) - pFrieze->getPosAt(index);
            pFrieze->setPosAt(newAnchorLocalPos.truncateTo2D(), index);
            pFrieze->setPosAt(newAnchorLocalPos.truncateTo2D() + vDelta, index + 1);
        }
        else if (!extraInfo.m_flags.m_isEdge)
        {
            pFrieze->setPosAt(newAnchorLocalPos.truncateTo2D(), index);
        }
    }
    pFrieze->onEditorMove(canChangeInitData);

    return btrue;
}

bbool FriezeEditor::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
{
    if (m_multiCollisionEdition || (m_holdState != HoldState_Nothing))    // don't allow moving points on holdstate
    return bfalse;

    BaseObject* pOwner = _pPickingShape->getOwnerPtr();
    if(pOwner->getObjectType() != BaseObject::eFrise)
        return bfalse;

    Frise* pFrieze = pOwner->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
    if (!pFrieze)
        return bfalse;

    PickingInfo2D* pPickInfo = _pPickingShape->getPickingInfoPtr();
    const bbool moveParent = !_pPickingShape->isSubAnchor();

    Vec3d delta = _deltaPos;
    if(pFrieze->isZForced())
        delta.m_z = 0.0f;

    Vec3d deltaPos;
    ITF_ASSERT(pPickInfo->m_targetRef == pFrieze->getRef());
    pFrieze->setPickingInfo(pPickInfo);
    deltaPos = transformToLocalTranslation(pFrieze, delta);

    const bbool canEditInitial = EDITOR->isAllowStorePickableInitData(pFrieze);
    if (moveParent)
    {
//            pFrieze->offsetPosition(deltaPos, canEditInitial);
        deltaPos = delta;//transformToLocalTranslation(pFrieze, delta);

	    pFrieze->setPos(pFrieze->getPos() + deltaPos);
        if(canEditInitial)
	        pFrieze->setLocalInitialPos(pFrieze->getLocalPos());
    }
    else
    {
        const i32 iPosCount                 = pFrieze->getPosCount();
        const FriezeExtraInfo extraInfo     = FriezeExtraInfo(pPickInfo->m_extraInformation);
        const u32 index                     = extraInfo.m_flags.m_subItemIndex;

        const Vec2d deltaPosTruncated       = deltaPos.truncateTo2D();

        if (extraInfo.m_flags.m_isPivot)
        {
			ITF_ASSERT_MSG(bfalse, "no more supported");
        }
        else if (extraInfo.m_flags.m_isEdge && (index < u32(iPosCount-1)))
        {
            const PolyLineEdge& pe  = pFrieze->getEdgeAt(index);
            pFrieze->setPosAt(pFrieze->getPosAt(index) + deltaPosTruncated, index);
            pFrieze->setPosAt(pFrieze->getPosAt(index) + pe.m_vector, index + 1);
        }
        else
        {
            pFrieze->setPosAt(pFrieze->getPosAt(index) + deltaPosTruncated, index);
        }
    }

    pFrieze->onEditorMove(EDITOR->isAllowStorePickableInitData(pFrieze));
    return btrue;
}

void FriezeEditor::onObjectAdded( const Scene* _pScene, BaseObject* _pObj )
{
    if(_pScene->isActive() && _pObj->getObjectType() == BaseObject::eFrise)
        m_addedActiveFriezesLastFrame.push_back(_pObj->getRef());
}

void FriezeEditor::onObjectRemoved( const Scene* _pScene, BaseObject* _pObj, bbool _delete )
{
    if(_delete && EDITOR && EDITOR->getSubEditedItem() == _pObj->getRef())
        EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
}

} // namespace ITF


