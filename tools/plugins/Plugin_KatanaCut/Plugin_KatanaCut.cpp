// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_PLUGIN_KATANACUT_H_
#include "tools/plugins/Plugin_KatanaCut/Plugin_KatanaCut.h"
#endif //_ITF_PLUGIN_KATANACUT_H_

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

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(DummyEditorObj)
BEGIN_SERIALIZATION_CHILD(DummyEditorObj)
END_SERIALIZATION()

DummyEditorObj::DummyEditorObj()
{
    AABB aabb;
    aabb.setMin(-Vec2d::One);
    aabb.setMax(Vec2d::One);

    setObjectType(BaseObject::eEditorDummy);
    setAlwaysVisible(btrue);
    changeRelativeAABB(aabb);
    setAllowedCell(bfalse);
}

void DummyEditorObj::update(f32 /*_deltaTime*/)
{
}

void DummyEditorObj::draw()
{
}

ITF_VECTOR<PickingShape*> Plugin_KatanaCut::m_shapes;
Plugin_KatanaCut::CutMode Plugin_KatanaCut::m_mode;
Pickable*        Plugin_KatanaCut::m_pickingShapesHolder = NULL;
PickingInfo2D    Plugin_KatanaCut::m_currentPickingInfo;
SafeArray<class Frise*> Plugin_KatanaCut::m_selection;
SafeArray<ObjectRef> Plugin_KatanaCut::m_postSelection;
Plugin_KatanaCut* Plugin_KatanaCut::m_instance = NULL;

void Plugin_KatanaCut::reset()
{
    m_instance = this;
    m_deleteShapes = -1;
    m_go = bfalse;
    m_shapeMode = ShapeMode_Free;
    m_cutMode = CutZMode_CurZ;
    UnselectAll(btrue);
    m_selection.clear();
}

void Plugin_KatanaCut::createHolder()
{
    if (m_pickingShapesHolder)
        return;
    
    m_pickingShapesHolder = newAlloc(mId_System, DummyEditorObj()); 
    m_pickingShapesHolder->setUserFriendly("Frieze Cutter");
    m_currentPickingInfo.m_type = BaseObject::eEditorDummy;
    m_currentPickingInfo.m_targetRef = m_pickingShapesHolder->getRef();
    m_currentPickingInfo.m_anchor            = Vec2d::Zero;
    m_currentPickingInfo.m_sqrDist2D         = 0.0f;
    m_currentPickingInfo.m_zDist             = 0.0f;
    m_currentPickingInfo.m_targetRadius      = 40.0f;
    m_currentPickingInfo.m_angle             = 0.0f;
    m_currentPickingInfo.m_extraInformation  = 0;

    m_currentShapeIndex = -1;
}


Plugin_KatanaCut::Plugin_KatanaCut()
{
    m_instance = this;
    reset();
}

Plugin_KatanaCut::~Plugin_KatanaCut()
{
    UnselectAll(btrue);
    if (m_pickingShapesHolder)
    {
        m_pickingShapesHolder->setScene(ITF_INVALID_OBJREF);
        delete m_pickingShapesHolder;
    }
}

void    Plugin_KatanaCut::stopEditor()
{
    for (u32 i = 0; i< m_shapes.size(); i++)
    {
        EDITOR->deletePickingShape(m_shapes[i]);
    }

    

    m_currentShapeIndex = -1;
    m_shapes.clear();
    m_deleteShapes--;
    if (m_pickingShapesHolder)
    {
        m_pickingShapesHolder->setScene(ITF_INVALID_OBJREF); 
        delete m_pickingShapesHolder;
        m_pickingShapesHolder = NULL;
    }
}

void   Plugin_KatanaCut::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
{
    if (!CURRENTWORLD || !CURRENTWORLD->getRootScene())
        return;

    if (CutMode_Idle == m_mode)
        return;

    if (m_shapes.size() >= 2)
        return;

    if (CutMode_StartNextClick == m_mode)
    {
        setMode(CutMode_Active);
        return;
    }
    

    if (CutMode_Active == m_mode)
    {
        setMode(CutMode_Active);
        if (_but == InputAdapter::MB_Left && _status == InputAdapter::Pressed)
        {
            createHolder();
            Vec3d snappedMouseWorld = EDITOR->getMouse3d();
            EDITOR->snap(snappedMouseWorld, NULL, bfalse);    

            SelectionDisc* pick = newAlloc(mId_Editor, SelectionDisc());
            pick->setPos(snappedMouseWorld);
            pick->m_position = snappedMouseWorld;
            pick->setSubAnchor(m_shapes.size() > 0);
            pick->setData(m_shapes.size());
            pick->setOwner(m_pickingShapesHolder->getRef());
            EDITOR->addPickingShapeForObject(m_pickingShapesHolder->getRef(), pick);
            pick->setPriority(1);
            m_shapes.push_back(pick);
            m_pickingShapesHolder->setScene(CURRENTWORLD->getRootScene());
        }
    }
}

bbool  Plugin_KatanaCut::isSelected()
{
    for (u32 i=0; i<EDITOR->getSelectionSize(); i++)
    {
        for (u32 j=0; j<m_shapes.size(); j++)
        {
            if (EDITOR->getSelectionAt(i) == m_shapes[j])
                return btrue;
        }
        
    }
    return bfalse;
}

void   Plugin_KatanaCut::update()
{
    u32 size = m_shapes.size();
    if (size > 0 && m_deleteShapes < 0)
    {
        SelectionDisc* pdisc = (SelectionDisc*)m_shapes[0];
        Vec3d p1 = pdisc->m_position;
        pdisc->setPos(p1);
        if (size > 1)
        {
            SelectionDisc* pdisc = (SelectionDisc*)m_shapes[1];
            Vec3d p2 = pdisc->m_position;
            pdisc->setPos(p2);
            if (size >= 2)
            {
                GFX_ADAPTER->drawDBG3DLine(p1, p2, 1, 0, 0, 0, 5);
                {
                    SelectionDisc* pShape0 = static_cast<SelectionDisc*>(m_shapes[0]);
                    SelectionDisc* pShape1 = static_cast<SelectionDisc*>(m_shapes[1]);
                    switch(m_shapeMode)
                    {
                    case ShapeMode_Vertical:
                        pShape1->m_position.m_x = pShape0->m_position.m_x;
                        pShape1->setPos(pShape1->m_position);
                        break;
                    case ShapeMode_Hozizontal:
                        pShape1->m_position.m_y = pShape0->m_position.m_y;
                        pShape1->setPos(pShape1->m_position);
                        break;
                    default:
                        break;
                    }
                }

            }
        }
        if (m_pickingShapesHolder)
        {
            m_pickingShapesHolder->setPos(p1);
            m_pickingShapesHolder->setSelected(&m_currentPickingInfo);
            if (!isSelected())
            {
                EDITOR->selectObject(m_pickingShapesHolder->getRef(), btrue);
                EDITOR->setSubEditedItem(m_pickingShapesHolder->getRef());
            }
        }
    }

    switch (m_mode)
    {
    case CutMode_Idle:
        {
            if (m_deleteShapes > 0)
            {
                m_deleteShapes--;
                if (m_deleteShapes == 0)
                {
                    for (u32 i = 0; i< m_shapes.size(); i++)
                    {
                        EDITOR->deletePickingShape(m_shapes[i]);
                    }

                    m_currentShapeIndex = -1;
                    m_shapes.clear();
                    m_deleteShapes--;
                    if (m_pickingShapesHolder)
                    {
                        m_pickingShapesHolder->disable();
                    }
                }
            }
        }
        return;
    default:
        {
            if (m_shapes.size() >= 2)
            {
               // setMode(CutMode_Idle);
                break;
            }
            Vec3d snappedMouseWorld = EDITOR->getMouse3d();
            EDITOR->snap(snappedMouseWorld, NULL, bfalse);    
            Vec3d snappedMouseScreen;
            GFX_ADAPTER->compute3DTo2D(snappedMouseWorld, snappedMouseScreen);
            Vec2d snappedMouse = snappedMouseScreen.truncateTo2D();
            if (m_pickingShapesHolder)
            {
                ((Pickable*)m_pickingShapesHolder)->setPos(snappedMouseWorld);
                ((Pickable*)m_pickingShapesHolder)->setWorldInitialPos(snappedMouseWorld);
            }
            EDITOR->showMouseIcon(Editor_IconStock::Icon_PolyLine_Cut, 0.0f, &snappedMouse);
        }
        break;
    }

    // DRAW FOUND COLLISIONS
    if (m_shapes.size() >= 2)
    {
        createTargetsList();
        processTargetsList();
    }
}

bbool   Plugin_KatanaCut::onPickShape(PickingShape* _shape)
{
    i32 foundIndex = getShapeIndex(_shape);
    if (foundIndex == -1)
        return bfalse;

    ITF_Memcpy(&m_currentPickingInfo, _shape->getPickingInfoPtr(), sizeof(PickingInfo2D));

    m_currentShapeIndex = foundIndex;
    return btrue;
}

bbool   Plugin_KatanaCut::onEditorRemove(const SafeArray<PickingShape*>& _shapesWithSameOwner)
{
    for (u32 i = 0; i < _shapesWithSameOwner.size(); i++)
    {
        if (getShapeIndex(_shapesWithSameOwner[i]) >= 0)
        {
            UnselectAll(btrue);
            return btrue;
        }
    }    
    return bfalse;
}

void   Plugin_KatanaCut::onKey(i32 _key, InputAdapter::PressStatus _status)
{
    if (_status == InputAdapter::Pressed)
    {
        switch (_key)
        {
            case KEY_BACKSPACE:
            case KEY_ESC:
                if (m_mode != CutMode_Idle)
                {
                    PLUGINGATEWAY->setExclusiveFocus(NULL);
                    EDITOR->unselectAll();
                    EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
                    EDITOR->unselectAll();
                    m_mode = CutMode_Idle;
                    m_deleteShapes = 4;
                }
                break;
            default:
                break;
        }
    }
}

void   Plugin_KatanaCut::UnselectAll(bbool _force)
{
}

void   Plugin_KatanaCut::fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const
{
    if (m_deleteShapes < 0)
        pushShapesToVector(_orderedShapes);
}

bbool   Plugin_KatanaCut::fillContextMenu(EditorContextMenu& _menu, ObjectRef _target, bbool _topItems)
{
    if (m_mode == CutMode_Idle)
        return btrue;

    _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Go, "Go!");

    switch(m_cutMode)
    {   
    case CutZMode_CurZ:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_AllZ, "Cut all Z");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Selection, "Cut Selection");
        break;
    case CutZMode_AllZ:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_CurZ, "Cut at current Z");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Selection, "Cut Selection");
        break;
    case CutZMode_Selection:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_CurZ, "Cut at current Z");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_AllZ, "Cut all Z");
        break;
    default:
        ITF_ASSERT(bfalse);
        break;
    }

    switch(m_shapeMode)
    {
    case ShapeMode_Free:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Vertical, "Vertical");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Horizontal, "Horizontal");
        break;
    case ShapeMode_Vertical:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Horizontal, "Horizontal");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Free, "Free");
        break;
    case ShapeMode_Hozizontal:
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Vertical, "Vertical");
        _menu.addItem(EditorContextMenu::ItemId_CutFrieze_Free, "Free");
        break;
    default:
        ITF_ASSERT(bfalse);
        break;
    }
    return btrue;
}

void   Plugin_KatanaCut::onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget)
{
    if (_item->isExternal())
        return;

    BaseObject* bo = GETOBJECT(_menuTarget);
    if (!bo)
        return;

    if (bo->getObjectType() == BaseObject::eEditorDummy)
    {
        switch(_item->getId())
        {
        case EditorContextMenu::ItemId_CutFrieze_CurZ:
            m_cutMode = CutZMode_CurZ;
        break;

        case EditorContextMenu::ItemId_CutFrieze_AllZ:
            m_cutMode = CutZMode_AllZ;
        break;

        case EditorContextMenu::ItemId_CutFrieze_Selection:
            m_cutMode = CutZMode_Selection;
            break;

        case EditorContextMenu::ItemId_CutFrieze_Vertical:
            m_shapeMode = ShapeMode_Vertical;
        break;

        case EditorContextMenu::ItemId_CutFrieze_Horizontal:
            m_shapeMode = ShapeMode_Hozizontal;
        break;

        case EditorContextMenu::ItemId_CutFrieze_Free:
            m_shapeMode = ShapeMode_Free;
        break;

        case EditorContextMenu::ItemId_CutFrieze_Go:
            m_go = btrue;
        break;        
        }
    }
}

void   Plugin_KatanaCut::cancelCurrentAction(bbool _keepEditorStuff)
{
    UnselectAll(btrue);
}

bbool   Plugin_KatanaCut::onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos)
{
    i32 index = getShapeIndex(_pPickingShape);
    if (index < 0)
        return bfalse;

    PickingInfo2D* pPickInfo = _pPickingShape->getPickingInfoPtr();
    Vec3d p1NewAnchorPos3D; // pPickInfo->m_anchor is the 2D delta between the mouse and the pickin shape's position (vector from mouse to picking shape)
    const Vec3d p1NewAnchorPos2D(_screenspacepos.m_x + pPickInfo->m_anchor.m_x, _screenspacepos.m_y + pPickInfo->m_anchor.m_y, pPickInfo->m_zDist);
    GFX_ADAPTER->compute2DTo3D(p1NewAnchorPos2D, p1NewAnchorPos3D);

    onShapeNewPos(index, p1NewAnchorPos3D);
    return btrue;
}

bbool   Plugin_KatanaCut::onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos )
{
    i32 index = getShapeIndex(_pPickingShape);
    if (index < 0)
        return bfalse;

    Vec3d p1NewAnchorPos3D = m_shapes[index]->getCenter() + _deltaPos;
    onShapeNewPos(index, p1NewAnchorPos3D);
    return btrue;
}

i32     Plugin_KatanaCut::getShapeIndex(const PickingShape* _pick) const
{
    for (i32 i = 0; i < (i32)m_shapes.size(); i++)
    {
        if (_pick == m_shapes[i])
        {
            return i;
        }
    }
    return -1;
}


void    Plugin_KatanaCut::onShapeNewPos(const i32 _index, const Vec3d& _newPos)
{
    switch (_index)
    {
    case 0:
        {
            SelectionDisc* pick = static_cast<SelectionDisc*>(m_shapes[0]);
            pick->setPos(_newPos);
            pick->m_position = _newPos;
            if (m_shapes.size() >= 2)
            {
                SelectionDisc* pShape0 = static_cast<SelectionDisc*>(m_shapes[0]);
                SelectionDisc* pShape1 = static_cast<SelectionDisc*>(m_shapes[1]);

                pShape1->m_position.m_z = _newPos.m_z;
                switch(m_shapeMode)
                {
                case ShapeMode_Vertical:
                    pShape1->m_position.m_x = pShape0->m_position.m_x;
                    pShape1->setPos(pShape1->m_position);
                    break;
                case ShapeMode_Hozizontal:
                    pShape1->m_position.m_y = pShape0->m_position.m_y;
                    pShape1->setPos(pShape1->m_position);
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case 1:
        {
            SelectionDisc* pShape0 = static_cast<SelectionDisc*>(m_shapes[0]);
            SelectionDisc* pShape1 = static_cast<SelectionDisc*>(m_shapes[1]);
            pShape1->setPos(_newPos);
            pShape1->m_position = _newPos;
            pShape0->m_position.m_z = _newPos.m_z;
            switch(m_shapeMode)
            {
            case ShapeMode_Vertical:
                pShape0->m_position.m_x = pShape1->m_position.m_x;
                pShape0->setPos(pShape0->m_position);
                break;
            case ShapeMode_Hozizontal:
                pShape0->m_position.m_y = pShape1->m_position.m_y;
                pShape0->setPos(pShape0->m_position);
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

void    Plugin_KatanaCut::pushShapesToVector(PickingShapeVector& _orderedShapes)
{
    u32 size = m_shapes.size();
    if (size > 0)
    {
        SelectionDisc* pdisc = (SelectionDisc*)m_shapes[0];
        Vec3d p1 = pdisc->m_position;
        pdisc->setPos(p1);
        if (size > 1)
        {
            SelectionDisc* pdisc = (SelectionDisc*)m_shapes[1];
            Vec3d p2 = pdisc->m_position;
            pdisc->setPos(p2);
        }
    }
    for (i32 i = 0; i < (i32)size; i++)
    {
        _orderedShapes.push_back(m_shapes[i]);
    }
}

void    Plugin_KatanaCut::setMode(CutMode _mode)
{
    if (_mode == CutMode_Active && m_mode == CutMode_Idle)
    {

        m_instance->reset();
        for (u32 i=0; i<EDITOR->getSelectionSize(); i++)
        {
            if (PickingShape* pShape = EDITOR->getSelectionAt(i))
            {
                if (BaseObject* pOwner = pShape->getOwnerPtr())
                {
                    if (pOwner->getObjectType() == BaseObject::eFrise )
                        m_selection.push_back(static_cast<Frise*>(pOwner));
                }
            }
        }

        if (EDITOR->getSelectionSize() == 0)
        {
            ObjectRef subOr =EDITOR->getSubEditedItem();
            if(BaseObject* pSubEdited = subOr.getObject())
            {
                if (pSubEdited->getObjectType() == BaseObject::eFrise)
                {
                    m_selection.push_back(static_cast<Frise*>(pSubEdited));
                    m_instance->m_cutMode = CutZMode_Selection;
                }
            }
        }

        EDITOR->setSubEditedItem(NULL);
        EDITOR->unselectAll();
    }

    m_mode = _mode;
}

void    Plugin_KatanaCut::SelectionDisc::draw()
{
    EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_PolyLine_Cut, m_position, 40.f, 0.f);
}


void    Plugin_KatanaCut::drawEdge()
{
    u32 size = m_shapes.size();
    if (size > 0)
    {
        SelectionDisc* pdisc = (SelectionDisc*)m_shapes[0];
        Vec3d p1 = pdisc->m_position;
        pdisc->setPos(p1);
        if (size > 1)
        {
            SelectionDisc* pdisc = (SelectionDisc*)m_shapes[1];
            pdisc->m_position.m_z = p1.m_z;
            Vec3d p2 = pdisc->m_position;
            pdisc->setPos(p2);
            if (size >= 2)
            {
                GFX_ADAPTER->drawDBG3DLine(p1, p2, 1, 0, 0, 0, 5);
            }
        }
        if (m_pickingShapesHolder)
        {
            m_pickingShapesHolder->setPos(p1);
            m_pickingShapesHolder->setSelected(&m_currentPickingInfo);
            if (!isSelected())
            {
                EDITOR->selectObject(m_pickingShapesHolder->getRef(), btrue);
                EDITOR->setSubEditedItem(m_pickingShapesHolder->getRef());
            }
        }
    }
}

bbool Plugin_KatanaCut::isPickingAllowed(   )
{
    return btrue;
}


void Plugin_KatanaCut::cutFrieze(Frise* currentPolyLine, const Vec2d& _cutPoint, u32 selectedEdgeIndex)
{
    if (currentPolyLine->isLooping())
        return;
    m_postSelection.push_back(currentPolyLine->getRef());
    Vec3d bestProj = _cutPoint.to3d();
    bestProj.m_z = currentPolyLine->getDepth();


    UNDOREDO_MANAGER->preChange(currentPolyLine);
    if (selectedEdgeIndex != U32_INVALID)
    {
        // cut original frieze
        PolyLineEdge edg = currentPolyLine->getEdgeAt(selectedEdgeIndex);
        bestProj = FriezeEditor::transformPointToLocalFriezePoint(currentPolyLine, bestProj);
        f32 dot = edg.m_normalizedVector.dot(Vec2d(bestProj.m_x, bestProj.m_y)-edg.getPos());
        Vec2d proj = edg.getPos() + edg.m_normalizedVector * dot;
        FriezeEditor::getInstance()->cutFriezeAtIndex(currentPolyLine, proj, selectedEdgeIndex);
    }
}

extern void drawPointList(const PolyPointList& _points, const Matrix44 *_optTransform, f32 _z, f32 _lineSize, Color _color);
void drawFriezeToBeCut(Frise* _pFrise)
{
    Matrix44 placement;
    f32 fZ = _pFrise->getDepth();

    if (_pFrise->isDynamic())
	    _pFrise->computeDynamicDisplayMatrix(placement);
    else
	    _pFrise->computeStaticDisplayMatrix(placement);

    Color col = Color::white();
    drawPointList(_pFrise->getPointsData(), &placement, fZ, 2.f, col);
}

void Plugin_KatanaCut::processTargetsList()
{
    m_cutData.clear();  // m_cutData = table de ce qu'il faut cutter
    m_postSelection.clear();
    Vec3d p1, p2;
    p1 = ((SelectionDisc*)m_shapes[0])->m_position; // position des 2 extrémités de la ligne de cut
    p2 = ((SelectionDisc*)m_shapes[1])->m_position;


    for (u32 i = 0; i < m_targets.size(); i++)
    {
        Frise* pCandidate = m_targets[i];

        bbool foundCollisionWithCandidate = bfalse;
        i32 edgeListCount = (i32)pCandidate->getPosCount()-1;        

        // check frise isn't looping with filling
        bbool isNotLoopingWithFilling = bfalse;
        if ( !pCandidate->isLooping() && pCandidate->isFilling() )
        {
            isNotLoopingWithFilling = btrue;
            edgeListCount++;
        }

        for (i32 j = 0; j < edgeListCount; j++) // parsing des edges de la frise candidate
        {
            Vec2d pos = pCandidate->get2DPos();
            const PolyLineEdge& edge = pCandidate->getEdgeAt(j);
            f32 ratioIntersection = 0.f;
            Vec2d intersect;
            // construction de 2 vecteur e1 et e2 qui sont les coordonnées en world space de l'edge courant dans la frise candidate
            Vec3d e1, e2;
            e1.m_x = edge.getPos().m_x;
            e1.m_y = edge.getPos().m_y;
            e1.m_z = pCandidate->getDepth();
            e1 = FriezeEditor::transformFriezePoint(pCandidate, e1);

            if ( isNotLoopingWithFilling && j == edgeListCount -1 )
            {
                const PolyLineEdge& edgeFilling = pCandidate->getEdgeAt(0);
                e2.m_x = edgeFilling.getPos().m_x;
                e2.m_y = edgeFilling.getPos().m_y;
            }
            else
            {
                e2.m_x = edge.getPos().m_x + edge.m_vector.m_x;
                e2.m_y = edge.getPos().m_y + edge.m_vector.m_y;
            }
            e2.m_z = pCandidate->getDepth();
            e2 = FriezeEditor::transformFriezePoint(pCandidate, e2); // transforme en coord world
            
            if (Segment_Segment( e1.truncateTo2D(), e2.truncateTo2D(), p1.truncateTo2D(), p2.truncateTo2D(), intersect, &ratioIntersection))
            {
                FriezeCutData* pCutData = NULL;
                for (u32 ic = 0; ic < m_cutData.size(); ic++) // cherche si on a déjà une entrée dans la liste pour cette frise
                {
                    if (m_cutData[ic].m_frise == pCandidate)
                    {
                        pCutData = &m_cutData[ic];
                        break;
                    }
                }
                if (!pCutData)
                {   // si c'est la première fois qu'on cut trouve une intersection avec cette frise
                    ITF_ASSERT(!foundCollisionWithCandidate);
                    FriezeCutData fcut;
                    fcut.m_frise = pCandidate;
                    m_cutData.push_back(fcut);  // ajout de la frise dans la table
                    pCutData = &m_cutData[m_cutData.size()-1];
                    drawFriezeToBeCut(pCandidate); // affichage des edges de la frise
                }
                // on ajoute l'edge dans la table
                foundCollisionWithCandidate = btrue;
                EdgeCutData cut;
                cut.m_edgeIndex =  j;
                cut.m_worldPos = intersect;
                cut.m_ratio = ratioIntersection;
                pCutData->m_cutData.push_back(cut);
                GFX_ADAPTER->drawDBGCircle(intersect.m_x, intersect.m_y, 0.5f, 0, 1, 0, 0, pCandidate->getDepth());
            }
        }
    }

    if (m_go)
    {   // en fin de coupe, on sort du plugin
        processKatanaCut();
        EDITOR->unselectAll();
        EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
        m_mode = CutMode_Idle;
        m_go = bfalse;
        PLUGINGATEWAY->setExclusiveFocus(NULL);
        m_deleteShapes = 4;
        EDITOR->selectObjects(m_postSelection, btrue);
    }
}
 
void Plugin_KatanaCut::createTargetsList()
{
    m_targets.clear();
    if (m_shapes.size() < 2)
        return;
    switch (m_cutMode)
    {
    case CutZMode_CurZ:
        createTargetsList_CurZ();
        break;
    case CutZMode_AllZ:
        createTargetsList_AllZ();
        break;
    case CutZMode_Selection:
        createTargetsList_Selection();
        break;
    default:
        ITF_ASSERT(bfalse);
        break;
    }
    for (u32 i = 0; i < m_targets.size();)
    {
        if (!EDITOR->checkObjectVisible(m_targets[i]->getRef()))
            m_targets.eraseNoOrder(i);
        else
            i++;
    }
}

void Plugin_KatanaCut::createTargetsList_CurZ()
{
    if (!CURRENTWORLD || !CURRENTWORLD->getRootScene())
        return;

    f32 curZ = ((SelectionDisc*)m_shapes[0])->m_position.m_z;
    Scene* pScene = CURRENTWORLD->getRootScene();
    SafeArray<ActorRef> AllActors;
    SafeArray<ObjectRef> AllFriezes;
    {
        const PickableList&  actors = pScene->getActors();
        for (u32 itActor = 0; itActor < actors.size(); itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            if (pActor->isSerializable() && !pActor->isSpawned())
            {
                pActor->registerContainedPickables(AllActors,AllFriezes);
            }
        }
        const PickableList& friezeList = pScene->getFrises();
        for (u32 i=0; i<friezeList.size(); i++)
        {
            const Frise* pFrz = (const Frise*)friezeList[i];
            AllFriezes.push_back(pFrz->getRef());
        } 
    }

    for (u32 i=0; i<AllFriezes.size(); i++)
    {
        if (Frise* pFrise = static_cast<Frise*>(AllFriezes[i].getObject()))
        {
            if (fabs(pFrise->getDepth() - curZ) < 1.f)
                m_targets.push_back(pFrise);
        }
    }
}

void Plugin_KatanaCut::createTargetsList_AllZ()
{
    if (!CURRENTWORLD || !CURRENTWORLD->getRootScene())
        return;

    Scene* pScene = CURRENTWORLD->getRootScene();
    SafeArray<ActorRef> AllActors;
    SafeArray<ObjectRef> AllFriezes;
    {
        const PickableList&  actors = pScene->getActors();
        for (u32 itActor = 0; itActor < actors.size(); itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            if (pActor->isSerializable() && !pActor->isSpawned())
            {
                pActor->registerContainedPickables(AllActors,AllFriezes);
            }
        }
        const PickableList& friezeList = pScene->getFrises();
        for (u32 i=0; i<friezeList.size(); i++)
        {
            const Frise* pFrz = (const Frise*)friezeList[i];
            AllFriezes.push_back(pFrz->getRef());
        } 
    }

    for (u32 i=0; i<AllFriezes.size(); i++)
    {
        if (Frise* pFrise = static_cast<Frise*>(AllFriezes[i].getObject()))
        {
            m_targets.push_back(pFrise);
        }
    }
}

void Plugin_KatanaCut::createTargetsList_Selection()
{
    m_targets = m_selection;
}

void Plugin_KatanaCut::onClearScene()
{
    for (u32 i = 0; i< m_shapes.size(); i++)
    {
        EDITOR->deletePickingShape(m_shapes[i]);
    }
    m_currentShapeIndex = -1;
    m_shapes.clear();
    reset();
    m_mode = CutMode_Idle;
}

void Plugin_KatanaCut::onDeleteScene()
{
    for (u32 i = 0; i< m_shapes.size(); i++)
    {
        EDITOR->deletePickingShape(m_shapes[i]);
    }
    m_currentShapeIndex = -1;
    m_shapes.clear();
    reset();
    m_mode = CutMode_Idle;
}

void Plugin_KatanaCut::addEdgeCutOnFillingVB( const Frise* _friezeToCut, Frise* frieze, f32 _ratio, u32 _indexEdgeLast )
{
    ITF_ASSERT( !_friezeToCut->isLooping());

    const Vec2d edgeFriezePosStart = _friezeToCut->getEdgeAt(0).getPos();
    const Vec2d edgeFriezePosStop = _friezeToCut->getEdgeAt(_indexEdgeLast).getPos();

    PolyLineEdge edgeFill( edgeFriezePosStop +( edgeFriezePosStart -edgeFriezePosStop) *_ratio);           
    edgeFill.m_holeMode = PolyLineEdge::Hole_Both;
    frieze->addEdge(edgeFill);
}

void Plugin_KatanaCut::buildFriezeParts( const Frise* _friezeToCut, FriezeCutData& _friezeCutData, u32 _cutDataIndex, u32 _indexEdgeLast )
{   
    EdgeCutData& edgeCutStart = _friezeCutData.m_cutData[_cutDataIndex];
    EdgeCutData& edgeCutStop = _friezeCutData.m_cutData[_cutDataIndex +1];

    Frise* frieze = static_cast<Frise*>(_friezeToCut->clone(_friezeToCut->getScene(), btrue));        
    UNDOREDO_MANAGER->postAdd(frieze);

    // init
    frieze->forceLoop(btrue);
    frieze->clearPoints();
    bbool isCutStopOnFillingVB = edgeCutStop.m_edgeIndex == _indexEdgeLast;
    
    if ( isCutStopOnFillingVB )
    {
        addEdgeCutOnFillingVB( _friezeToCut, frieze, edgeCutStop.m_ratio, _indexEdgeLast );
        frieze->forceLoop(bfalse);
    }

    // add 1st edge  
    PolyLineEdge edgeStart = _friezeToCut->getEdgeAt(edgeCutStart.m_edgeIndex);   
    edgeStart.setPos(edgeStart.getPos() +edgeStart.m_vector*edgeCutStart.m_ratio);
    edgeStart.setScale(_friezeToCut->getEdgeScaleAtRatio(edgeCutStart.m_edgeIndex, edgeCutStart.m_ratio));        
    frieze->addEdge(edgeStart);
       
    // add intermediate edges
    for (u32 idPoint= edgeCutStart.m_edgeIndex+1; idPoint<=edgeCutStop.m_edgeIndex; idPoint++)
    {
        frieze->addEdge(_friezeToCut->getEdgeAt(idPoint));
    }

    // add last edge
    if ( !isCutStopOnFillingVB )
    {
        const PolyLineEdge& edgeFriezeCut = _friezeToCut->getEdgeAt(edgeCutStop.m_edgeIndex); 
        PolyLineEdge edgeStop( edgeFriezeCut.getPos() +edgeFriezeCut.m_vector *edgeCutStop.m_ratio);
        edgeStop.m_holeMode = PolyLineEdge::Hole_Both;
        edgeStop.setScale(_friezeToCut->getEdgeScaleAtRatio(edgeCutStop.m_edgeIndex, edgeCutStop.m_ratio));
        frieze->addEdge(edgeStop);

        // loop
        PolyLineEdge edgeLoop = frieze->getEdgeAt(0);         
        frieze->addEdge(edgeLoop);
    }

    frieze->forceRecomputeData();
    PLUGINGATEWAY->onObjectChanged(frieze);
}

void Plugin_KatanaCut::buildFriezeCut(Frise* _friezeToCut, FriezeCutData& _friezeCutData, u32 _indexEdgeLast )
{           
    Frise* frieze = static_cast<Frise*>(_friezeToCut->clone(_friezeToCut->getScene(), btrue));        
    UNDOREDO_MANAGER->postAdd(frieze);

    // init, loop        
    frieze->clearPoints();   

    u32 idCutDataLast = _friezeCutData.m_cutData.size() -1;
    u32 idPoint = 0;
    u32 idCutData = 0;

    // add edge from cutData
    while ( idCutData < idCutDataLast ) // utilise les info du cut 2 par 2
    {
        u32 idPointCut = _friezeCutData.m_cutData[idCutData].m_edgeIndex;

        while ( idPoint <= idPointCut )
        {
            frieze->addEdge(_friezeToCut->getEdgeAt(idPoint));
            idPoint++;
        }

        const EdgeCutData& edgeCutData1 = _friezeCutData.m_cutData[idCutData];
        idCutData++;
        const PolyLineEdge& edgeFrizeCut1 = _friezeToCut->getEdgeAt(edgeCutData1.m_edgeIndex); 
        PolyLineEdge edge1( edgeFrizeCut1.getPos() +edgeFrizeCut1.m_vector *edgeCutData1.m_ratio);           
        edge1.setScale(_friezeToCut->getEdgeScaleAtRatio(edgeCutData1.m_edgeIndex, edgeCutData1.m_ratio));        
        edge1.m_holeMode = PolyLineEdge::Hole_Both;
        frieze->addEdge(edge1);        

        const EdgeCutData& edgeCutData2 = _friezeCutData.m_cutData[idCutData];    
        idCutData++;

        if ( edgeCutData2.m_edgeIndex == _indexEdgeLast )
        {
            addEdgeCutOnFillingVB( _friezeToCut, frieze, _friezeCutData.m_cutData[idCutDataLast].m_ratio, _indexEdgeLast );
        }
        else
        {
            const PolyLineEdge& edgeFrizeCut2 = _friezeToCut->getEdgeAt(edgeCutData2.m_edgeIndex);
            PolyLineEdge edge2 = edgeFrizeCut2; 
            edge2.setPos(edgeFrizeCut2.getPos() +edgeFrizeCut2.m_vector *edgeCutData2.m_ratio);
            edge2.setScale(_friezeToCut->getEdgeScaleAtRatio(edgeCutData2.m_edgeIndex, edgeCutData2.m_ratio));
            frieze->addEdge(edge2);
        }

        idPoint = edgeCutData2.m_edgeIndex +1;               
    }

    // add others edge
    while ( idPoint <= _indexEdgeLast )
    {
        frieze->addEdge(_friezeToCut->getEdgeAt(idPoint));
        idPoint++;
    }

    frieze->forceRecomputeData();
    PLUGINGATEWAY->onObjectChanged(frieze);
}

void Plugin_KatanaCut::splitFrieze( Frise* _friezeToCut, u32 _index, f32 _ratio )
{
    Frise* friezeLeft = static_cast<Frise*>(_friezeToCut->clone(_friezeToCut->getScene(), btrue));        
    Frise* friezeRight = static_cast<Frise*>(_friezeToCut->clone(_friezeToCut->getScene(), btrue)); 
    UNDOREDO_MANAGER->postAdd(friezeLeft);
    UNDOREDO_MANAGER->postAdd(friezeRight);

    // init
    friezeLeft->clearPoints();
    friezeRight->clearPoints();
   
    // edge cut
    const PolyLineEdge& edgeFriezeCut = _friezeToCut->getEdgeAt(_index); 
    PolyLineEdge edgeCut = edgeFriezeCut;
    edgeCut.setPos( edgeFriezeCut.getPos() +edgeFriezeCut.m_vector *_ratio);
    edgeCut.setScale(_friezeToCut->getEdgeScaleAtRatio(_index, _ratio));

    // build left frieze
    for (u32 idPoint= 0; idPoint<=_index; idPoint++)
    {
        friezeLeft->addEdge(_friezeToCut->getEdgeAt(idPoint));
    }
   
    friezeLeft->addEdge(edgeCut);

    // build right frieze
    friezeRight->addEdge(edgeCut);

    u32 indexEdgeLast = _friezeToCut->getPosCount() -1;
    for (u32 idPoint = _index+1; idPoint<=indexEdgeLast; idPoint++)
    {
        friezeRight->addEdge(_friezeToCut->getEdgeAt(idPoint));
    }
    
    friezeLeft->forceRecomputeData();
    friezeRight->forceRecomputeData();
    PLUGINGATEWAY->onObjectChanged(friezeLeft);
    PLUGINGATEWAY->onObjectChanged(friezeRight);
}

bbool Plugin_KatanaCut::isSplitable( const Frise* _friezeToCut)
{
    if ( _friezeToCut->isLooping() || _friezeToCut->isFilling())
        return bfalse;

    return btrue;
}

void Plugin_KatanaCut::processKatanaCut()
{    
    UndoRedoSequenceScope changes("KatanaCut");
    ObjectRefList  friezeRefListToDelete;

    u32 cutFriezeCount = m_cutData.size();
    for ( u32 i=0; i<cutFriezeCount; i++ )
    {
        FriezeCutData& friezeCutData = m_cutData[i];           
        u32 cutDataCount = friezeCutData.m_cutData.size();
        Frise* friezeToCut = friezeCutData.m_frise;

        if ( friezeToCut->isDynamic() )
            continue;

        if ( cutDataCount == 1 )
        {
            if ( isSplitable(friezeToCut))
            {
                splitFrieze( friezeToCut, friezeCutData.m_cutData[0].m_edgeIndex, friezeCutData.m_cutData[0].m_ratio );
                friezeRefListToDelete.push_back(friezeToCut->getRef());
            }

            continue;
        }

        if ( cutDataCount&1 )
            continue;
        
        friezeRefListToDelete.push_back(friezeToCut->getRef());
       
        u32 indexEdgeLast = friezeToCut->getPosCount() -1;
        u32 cutDataIndex = 0;
        u32 cutDataIndexStop = cutDataCount -1;

        while ( cutDataIndex < cutDataIndexStop )
        {
            buildFriezeParts(friezeToCut, friezeCutData, cutDataIndex, indexEdgeLast );
            cutDataIndex += 2;
        }

        buildFriezeCut( friezeToCut, friezeCutData, indexEdgeLast );     
    }

    EDITOR->deleteSelection(&friezeRefListToDelete);
}

} // namespace ITF


