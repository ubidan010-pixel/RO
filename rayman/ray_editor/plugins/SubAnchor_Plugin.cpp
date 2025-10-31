#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_SUBANCHOR_PLUGIN_H_
#include "rayman/ray_editor/plugins/SubAnchor_Plugin.h"
#endif //_ITF_SUBANCHOR_PLUGIN_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_


#ifndef ITF_CONSOLE_FINAL
namespace ITF {

SubAnchor_Plugin* SubAnchor_Plugin::s_instance = NULL;

//------------------------------------------------------------------------------
Interface_ActorEditorListener* SubAnchor_Plugin::create()
{
    ITF_ASSERT(!s_instance);
    s_instance = newAlloc(mId_Editor, SubAnchor_Plugin);
    ActorEditor::getInstance()->registerListener(s_instance, Ray_SubAnchorComponent::GetClassCRCStatic());
    return s_instance;
}

//------------------------------------------------------------------------------
void SubAnchor_Plugin::destroy()
{
    ITF_ASSERT(s_instance);
}

//------------------------------------------------------------------------------
SubAnchor_Plugin::SubAnchor_Plugin()
{
}

//------------------------------------------------------------------------------
SubAnchor_Plugin::~SubAnchor_Plugin()
{
}

//------------------------------------------------------------------------------
void SubAnchor_Plugin::addPickingShapeSpecificIcons(PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize)
{
    _iconSize = 32.0f;
    _icons.clear();

    if (isSelected(_shape))
    {
        _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNode);
    }
    else
    {
        _icons.push_back(Editor_IconStock::Icon_TrajectoryNode);
    }
}

//------------------------------------------------------------------------------
void SubAnchor_Plugin::addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component)
{
    Ray_SubAnchorComponent* component = _component->DynamicCast<Ray_SubAnchorComponent>(ITF_GET_STRINGID_CRC(Ray_SubAnchorComponent,571243035));
    ITF_ASSERT(component);
    if(!component)
    {
        return;
    }

    // sub anchor discs
    Ray_SubAnchorList& subAnchors = component->editor_getSubAnchors();
    Ray_SubAnchorList::iterator it;
    for (it = subAnchors.begin(); it != subAnchors.end(); ++it)
    {
        Ray_SubAnchor& subAnchor = *it;
        PickingShape_Disc* disc = newAlloc(mId_Editor, PickingShape_Disc(this));
        disc->setPos(subAnchor.getPos());
        disc->setData(subAnchor.getTemplate()->getName());
        disc->setSubAnchor(btrue);
        EDITOR->addPickingShapeForObject(_actor->getRef(), disc);
    }
}

//------------------------------------------------------------------------------
void SubAnchor_Plugin::updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes)
{
    // guards
    PickingShape_Disc* disc = getDisc(_shape);
    Ray_SubAnchorComponent* component = getComponent(_shape);
    ITF_ASSERT(disc && component);
    if (!disc || !component)
    {
        return;
    }
    
    // set position from component
    StringID name = _shape->getData();
    disc->setPos(component->getPos(name));
    disc->setlastUpdateFrame(CURRENTFRAME);

    // shape active/displayed if actor selected
    if (_actor->isSelected())
    {
        _activeShapes.push_back(disc);
    }
}

//------------------------------------------------------------------------------
bbool SubAnchor_Plugin::onSetShapePosition(PickingShape* _shape, const Vec2d& _screenSpacePos)
{
    // main anchor not affected
    if (!_shape->isSubAnchor())
    {
        return bfalse;
    }

    // guards
    PickingShape_Disc* disc = getDisc(_shape);
    Ray_SubAnchorComponent* component = getComponent(_shape);
    ITF_ASSERT(disc && component);
    if (!disc || !component)
    {
        return bfalse;
    }

    // store new position in component
    StringID name = _shape->getData();
    const Vec3d worldPos = getWorldPosition(disc, _screenSpacePos);
    component->editor_setPos(name, worldPos);

    return btrue;
}

//------------------------------------------------------------------------------
bbool SubAnchor_Plugin::onOffsetShapePosition(PickingShape* _shape, const Vec3d& _deltaPos)
{
    // main anchor not affected
    if (!_shape->isSubAnchor())
    {
        return bfalse;
    }

    // guards
    PickingShape_Disc* disc = getDisc(_shape);
    Ray_SubAnchorComponent* component = getComponent(_shape);
    ITF_ASSERT(disc && component);
    if (!disc || !component)
    {
        return bfalse;
    }

    // store new position in component
    StringID name = _shape->getData();
    component->editor_setPos(name, disc->getPos() + _deltaPos);

    return btrue;
}

//------------------------------------------------------------------------------
PickingShape_Disc* SubAnchor_Plugin::getDisc(PickingShape* _shape)
{
    // retrieve shape
    bbool isDisc = _shape && (_shape->getShapeType()==PickingShape::ShapeType_Disc);
    if (!isDisc)
    {
        return NULL;
    }
    return static_cast<PickingShape_Disc*>(_shape);
}

//------------------------------------------------------------------------------
Ray_SubAnchorComponent* SubAnchor_Plugin::getComponent(PickingShape* _shape)
{
    // retrieve component
    Actor* actor = _shape->getOwnerPtr()->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
    ITF_ASSERT(actor);
    if (!actor)
    {
        return NULL;
    }
    return actor->GetComponent<Ray_SubAnchorComponent>();
}

//------------------------------------------------------------------------------
bbool SubAnchor_Plugin::isSelected( PickingShape* _shape )
{
    // picking shape selected?
    for (u32 i=0; i<EDITOR->getSelectionSize(); ++i)
    {
        if (EDITOR->getSelectionAt(i) == _shape)
        {
            return btrue;
        }
    }
    return bfalse;
}

//------------------------------------------------------------------------------
Vec3d SubAnchor_Plugin::getWorldPosition(PickingShape_Disc* _disc, const Vec2d& _screenSpacePos)
{
    // apply anchor pos
    Vec3d screenPos;
    GFX_ADAPTER->compute3DTo2D(_disc->getPos(), screenPos);
    screenPos.m_x = _screenSpacePos.m_x + _disc->getPickingInfoPtr()->m_anchor.m_x;
    screenPos.m_y = _screenSpacePos.m_y + _disc->getPickingInfoPtr()->m_anchor.m_y;

    // to 3D
    Vec3d worldPos;
    GFX_ADAPTER->compute2DTo3D(screenPos, worldPos);
    return worldPos;
}

//------------------------------------------------------------------------------
void SubAnchor_Plugin::onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& _deltaPos)
{
    PickingShape_Disc* disc = getDisc(_pShape);
    Ray_SubAnchorComponent* component = _pComp->DynamicCast<Ray_SubAnchorComponent>(ITF_GET_STRINGID_CRC(Ray_SubAnchorComponent,571243035));
    
    // don't move sub anchors if LALT pressed
    if (disc && component
        && !_pShape->isSubAnchor()
        && INPUT_ADAPTER->isKeyPressed(KEY_LALT))
    {
        Ray_SubAnchorList& subAnchors = component->editor_getSubAnchors();
        Ray_SubAnchorList::iterator it;
        for (it = subAnchors.begin(); it != subAnchors.end(); ++it)
        {
            Ray_SubAnchor& subAnchor = *it;
            subAnchor.editor_setPos(subAnchor.getPos() - _deltaPos);
        }
    }
}

} // namespace ITF
#endif //ITF_CONSOLE_FINAL