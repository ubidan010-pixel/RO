#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_GEYSERPLATFORM_PLUGIN_H_
#include "rayman/ray_editor/plugins/GeyserPlatform_Plugin.h"
#endif //_ITF_GEYSERPLATFORM_PLUGIN_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

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
    GeyserPlatform_Plugin* GeyserPlatform_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* GeyserPlatform_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, GeyserPlatform_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, Ray_GeyserPlatformAIComponent::GetClassCRCStatic());
        return s_instance;
    }

    void GeyserPlatform_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    GeyserPlatform_Plugin::GeyserPlatform_Plugin()
    {
    }

    GeyserPlatform_Plugin::~GeyserPlatform_Plugin()
    {
    }

    void GeyserPlatform_Plugin::addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize )
    {
        _iconSize = 32.0f;
        _icons.clear();

        Ray_GeyserPlatformAIComponent* geyser = getComponent(_shape);
        if (geyser->editor_getTemplate()->getLockLength())
        {
            _icons.push_back(Editor_IconStock::Icon_PolyLine_Cut);
        }
        else if ( isSelected(_shape) )
        {
            _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNode);
        }
        else
        {
            _icons.push_back(Editor_IconStock::Icon_TrajectoryNode);
        }
    }

    void GeyserPlatform_Plugin::addComponentSpecificPickingShape( Actor* _actor, ActorComponent* _component )
    {
        PickingShape_Disc* disc = newAlloc(mId_Editor, PickingShape_Disc(this));

        Ray_GeyserPlatformAIComponent* geyser = static_cast<Ray_GeyserPlatformAIComponent*>(_component);
        disc->setPos( geyser->editor_getTarget() );
        disc->setSubAnchor(btrue);

        EDITOR->addPickingShapeForObject(_actor->getRef(), disc);
    }

    void GeyserPlatform_Plugin::updateComponentSpecificPickingShape( Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes )
    {
        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        Ray_GeyserPlatformAIComponent* geyser = getComponent(_shape);

        disc->setPos( geyser->editor_getTarget() );
        disc->setlastUpdateFrame(CURRENTFRAME);
        if (_actor->isSelected())
        {
            _activeShapes.push_back(disc);
        }
    }

    bbool GeyserPlatform_Plugin::onSetShapePosition( PickingShape* _shape, const Vec2d& _screenSpacePos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        Ray_GeyserPlatformAIComponent* geyser = getComponent(_shape);

        const Vec3d worldPos = getWorldPosition(disc, _screenSpacePos);
        geyser->editor_setTarget(worldPos);


        return btrue;
    }

    bbool GeyserPlatform_Plugin::onOffsetShapePosition( PickingShape* _shape, const Vec3d& _deltaPos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        Ray_GeyserPlatformAIComponent* geyser = getComponent(_shape);

        const Vec3d worldPos = getWorldPosition(disc, _deltaPos);
        geyser->editor_setTarget(worldPos);

        return btrue;
    }


    Ray_GeyserPlatformAIComponent* GeyserPlatform_Plugin::getComponent( PickingShape* _shape )
    {
        return getActor(_shape)->GetComponent<Ray_GeyserPlatformAIComponent>();
    }

    Actor* GeyserPlatform_Plugin::getActor( PickingShape* _shape )
    {
        return static_cast<Actor*>(_shape->getOwnerPtr());
    }

    bbool GeyserPlatform_Plugin::isSelected( PickingShape* _shape )
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


    Vec3d GeyserPlatform_Plugin::getWorldPosition( PickingShape_Disc* _disc, const Vec2d& _screenSpacePos )
    {
        Vec3d screenPos;
        GFX_ADAPTER->compute3DTo2D(_disc->getPos(), screenPos);
        screenPos.m_x = _screenSpacePos.m_x + _disc->getPickingInfoPtr()->m_anchor.m_x;
        screenPos.m_y = _screenSpacePos.m_y + _disc->getPickingInfoPtr()->m_anchor.m_y;

        Vec3d worldPos;
        GFX_ADAPTER->compute2DTo3D(screenPos, worldPos);

        return worldPos;
    }

    Vec3d GeyserPlatform_Plugin::getWorldPosition( PickingShape_Disc* _disc, const Vec3d& _deltaPos )
    {
        return _disc->getPos() + _deltaPos;
    }

} // namespace ITF

#endif !ITF_CONSOLE_FINAL