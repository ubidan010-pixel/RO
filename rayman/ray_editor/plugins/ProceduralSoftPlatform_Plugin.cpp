#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_PROCEDURALSOFTPLATFORM_PLUGIN_H_
#include "rayman/ray_editor/plugins/ProceduralSoftPlatform_Plugin.h"
#endif //_ITF_PROCEDURALSOFTPLATFORM_PLUGIN_H_



#ifndef _ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/ProceduralSoftPlatformComponent.h"
#endif //_ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef ITF_CONSOLE_FINAL

namespace ITF
{
    ProceduralSoftPlatform_Plugin* ProceduralSoftPlatform_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* ProceduralSoftPlatform_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, ProceduralSoftPlatform_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, ProceduralSoftPlatformComponent::GetClassCRCStatic());
        return s_instance;
    }

    void ProceduralSoftPlatform_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    ProceduralSoftPlatform_Plugin::ProceduralSoftPlatform_Plugin()
    {
    }

    ProceduralSoftPlatform_Plugin::~ProceduralSoftPlatform_Plugin()
    {
    }

    void ProceduralSoftPlatform_Plugin::addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize )
    {
        _iconSize = 32.0f;
        _icons.clear();

        if ( isSelected(_shape) )
        {
            _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNode);
        }
        else
        {
            _icons.push_back(Editor_IconStock::Icon_TrajectoryNode);
        }
    }

    void ProceduralSoftPlatform_Plugin::addComponentSpecificPickingShape( Actor* _actor, ActorComponent* _component )
    {
        PickingShape_Disc* disc = newAlloc(mId_Editor, PickingShape_Disc(this));

        ProceduralSoftPlatformComponent* component = static_cast<ProceduralSoftPlatformComponent*>(_component);

        disc->setPos( transformToWorld(component, component->getEndPos() ));
        disc->setSubAnchor(btrue);

        EDITOR->addPickingShapeForObject(_actor->getRef(), disc);
    }

    void ProceduralSoftPlatform_Plugin::updateComponentSpecificPickingShape( Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes )
    {
        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        ProceduralSoftPlatformComponent* component = getComponent(_shape);


        disc->setPos( transformToWorld(component, component->getEndPos()) );
        disc->setlastUpdateFrame(CURRENTFRAME);
        if (_actor->isSelected())
        {
            _activeShapes.push_back(disc);
        }
    }

    bbool ProceduralSoftPlatform_Plugin::onSetShapePosition( PickingShape* _shape, const Vec2d& _screenSpacePos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        ProceduralSoftPlatformComponent* component = getComponent(_shape);
  
        const Vec3d worldPos = getWorldPosition(disc, _screenSpacePos);
        const Vec3d localPos = transformToLocal( component, worldPos );
        component->editor_setPos( localPos);


        return btrue;
    }

    bbool ProceduralSoftPlatform_Plugin::onOffsetShapePosition( PickingShape* _shape, const Vec3d& _deltaPos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        ProceduralSoftPlatformComponent* component = getComponent(_shape);

        const Vec3d worldPos = getWorldPosition(disc, _deltaPos);
        const Vec3d localPos = transformToLocal( component, worldPos );
        component->editor_setPos( localPos );

        return btrue;
    }


    ProceduralSoftPlatformComponent* ProceduralSoftPlatform_Plugin::getComponent( PickingShape* _shape )
    {
        return getActor(_shape)->GetComponent<ProceduralSoftPlatformComponent>();
    }

    Actor* ProceduralSoftPlatform_Plugin::getActor( PickingShape* _shape )
    {
        return static_cast<Actor*>(_shape->getOwnerPtr());
    }

    bbool ProceduralSoftPlatform_Plugin::isSelected( PickingShape* _shape )
    {
        for (u32 i = 0; i < EDITOR->getSelectionSize(); i++)
        {
            if (EDITOR->getSelectionAt(i) == _shape)
            {
                return btrue;
            }
        }

        return bfalse;
    }


    Vec3d ProceduralSoftPlatform_Plugin::getWorldPosition( PickingShape_Disc* _disc, const Vec2d& _screenSpacePos )
    {
        Vec3d screenPos;
        GFX_ADAPTER->compute3DTo2D(_disc->getPos(), screenPos);
        screenPos.m_x = _screenSpacePos.m_x + _disc->getPickingInfoPtr()->m_anchor.m_x;
        screenPos.m_y = _screenSpacePos.m_y + _disc->getPickingInfoPtr()->m_anchor.m_y;

        Vec3d worldPos;
        GFX_ADAPTER->compute2DTo3D(screenPos, worldPos);

        return worldPos;
    }

    Vec3d ProceduralSoftPlatform_Plugin::getWorldPosition( PickingShape_Disc* _disc, const Vec3d& _deltaPos )
    {
        return _disc->getPos() + _deltaPos;
    }


    Vec3d ProceduralSoftPlatform_Plugin::transformToLocal( ProceduralSoftPlatformComponent* _component, const Vec3d& _worldPos )
    {
        return ITF::inverseTransformPos(_component->GetActor()->getPos(), _component->GetActor()->getAngle(), _worldPos);
    }

    Vec3d ProceduralSoftPlatform_Plugin::transformToWorld( ProceduralSoftPlatformComponent* _component, const Vec3d& _localPos )
    {
        return ITF::transformPos(_component->GetActor()->getPos(), _component->GetActor()->getAngle(), _localPos);
    }

} // namespace ITF

#endif !ITF_CONSOLE_FINAL