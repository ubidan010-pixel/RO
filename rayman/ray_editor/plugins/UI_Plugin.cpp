#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_UI_PLUGIN_H_
#include "rayman/ray_editor/plugins/UI_Plugin.h"
#endif // _ITF_UI_PLUGIN_H_

namespace ITF
{
    UI_Plugin* UI_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* UI_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, UI_Plugin);
        ActorEditor::getInstance()->registerListener(s_instance, UIComponent::GetClassCRCStatic());
        return s_instance;
    }

    void UI_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    UI_Plugin::UI_Plugin()
    {
    }

    UI_Plugin::~UI_Plugin()
    {
    }

    void UI_Plugin::addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize )
    {
        //_icons.clear();
        //_icons.push_back(Editor_IconStock::Icon_TrajectoryNode);
        _iconSize /= 2.0f;
    }

    bbool UI_Plugin::onUpdateShape( Actor* _actor, UIComponent* _component, PickingShape_Disc* _shape, const UIData _shapeData )
    {
        if(isSelected(_shape))
        {
            _component->onEditorSelected();
        }
        return btrue;
    }

    void UI_Plugin::onUpdate()
    {

    }

    void UI_Plugin::updateComponentSpecificPickingShape( Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes )
    {
        BaseComponentPlugin<UIComponent, UIData>::updateComponentSpecificPickingShape(_actor, _shape, _activeShapes);

        //if(isSelected(_shape))
        //{
        //    _component->onEditorSelected();
        //}
    }

    void UI_Plugin::onCreateShapes( Actor* _actor, UIComponent* _component )
    {

    }



} // namespace ITF
