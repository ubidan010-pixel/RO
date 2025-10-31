#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_NEEDSINITPOSPICKINGSHAPE_PLUGIN_H_
#include "rayman/ray_editor/plugins/NeedsInitPosPickingShape_Plugin.h"
#endif //_ITF_NEEDSINITPOSPICKINGSHAPE_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{
    NeedsInitPosPickingShape_Plugin* NeedsInitPosPickingShape_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* NeedsInitPosPickingShape_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, NeedsInitPosPickingShape_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, TweenComponent::GetClassCRCStatic());
        return s_instance;
    }

    void NeedsInitPosPickingShape_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    NeedsInitPosPickingShape_Plugin::NeedsInitPosPickingShape_Plugin()
    {
    }

    NeedsInitPosPickingShape_Plugin::~NeedsInitPosPickingShape_Plugin()
    {
    }

    bbool NeedsInitPosPickingShape_Plugin::needsInitPosPickingShape( Actor* _pActor, ActorComponent* _component )
    {
        return _pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) == NULL;
    }

    bbool NeedsInitPosPickingShape_Plugin::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
    {
        Actor* pActor = static_cast<Actor*>(_pPickingShape->getOwnerPtr());

        // Do nothing if the main shape is also selected, the main shape has priority on multiselection
        if(PickingShape* pMainShape = EDITOR->getMainPickingShapeForObject(pActor->getRef()))
            if(EDITOR->getIndexOfShapeInSelection(pMainShape) != -1)
                return btrue;

        Vec3d p1New3DPos;
        PickingInfo2D* pPickingInfo = _pPickingShape->getPickingInfoPtr();
        const Vec3d p1New2DPos(_screenspacepos.m_x + pPickingInfo->m_anchor.m_x, _screenspacepos.m_y + pPickingInfo->m_anchor.m_y, pPickingInfo->m_zDist);
        GFX_ADAPTER->compute2DTo3D(p1New2DPos, p1New3DPos);
        p1New3DPos.m_z = pActor->getWorldInitialPos().getZ();

        Vec3d offset = pActor->getPos() - pActor->getWorldInitialPos();
        pActor->setWorldInitialPos(p1New3DPos);
        pActor->setPos(p1New3DPos + offset);        
        pActor->onEditorMove(EDITOR->isAllowStorePickableInitData(pActor));

        return btrue;
    }

    bbool NeedsInitPosPickingShape_Plugin::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
    {
        Actor* pActor = static_cast<Actor*>(_pPickingShape->getOwnerPtr());

        // Do nothing if the main shape is also selected, the main shape has priority on multiselection
        if(PickingShape* pMainShape = EDITOR->getMainPickingShapeForObject(pActor->getRef()))
            if(EDITOR->getIndexOfShapeInSelection(pMainShape) != -1)
                return btrue;

        Vec3d delta = _deltaPos;
        if(pActor->isZForced())
            delta.m_z = 0.0f;

        Vec3d offset = pActor->getPos() - pActor->getWorldInitialPos();
        pActor->setLocalInitialPos(pActor->getLocalInitialPos() + delta);
        pActor->setPos(pActor->getWorldInitialPos() + offset);
        pActor->onEditorMove(EDITOR->isAllowStorePickableInitData(pActor));

        return btrue;
    }

    void NeedsInitPosPickingShape_Plugin::addComponentSpecificPickingShape( Actor* _pActor, ActorComponent* _pComp )
    {
        if (needsInitPosPickingShape(_pActor, _pComp))
        {
            PickingShape_Disc* initPick = newAlloc(mId_Editor, PickingShape_Disc(this));
            initPick->setPos(_pActor->getBoundWorldInitialPos());
            initPick->setSubAnchor(btrue);
            initPick->setPosType(PickingShape_Disc::InitPos);
            initPick->setPriority(2);
            initPick->setSnapWithMainShape(bfalse);
            EDITOR->addPickingShapeForObject(_pActor->getRef(), initPick);
        }
    }

    void NeedsInitPosPickingShape_Plugin::updateComponentSpecificPickingShape( Actor* _pActor, PickingShape* _pShape, PickingShapeVector& _activeShapes )
    {
        PickingShape_Disc* pDisc = static_cast<PickingShape_Disc*>(_pShape);
        pDisc->setPos(_pActor->getBoundWorldInitialPos());
        _activeShapes.push_back(pDisc);
    }

} // namespace ITF
