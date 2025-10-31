#ifndef _ITF_UI_PLUGIN_H_
#define _ITF_UI_PLUGIN_H_

#ifndef _ITF_BASECOMPONENTPLUGIN_H_
#include "rayman/ray_editor/plugins/BaseComponentPlugin.h"
#endif //_ITF_BASECOMPONENTPLUGIN_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{
    union UIData
    {
        explicit UIData(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
    };

    class UI_Plugin : public BaseComponentPlugin<UIComponent, UIData>
    {
        static UI_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();

        UI_Plugin();
        virtual ~UI_Plugin();

        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize );
        virtual void    onUpdate();
        virtual void    updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes);
        
    protected:
        
        virtual void    onCreateShapes( Actor* _actor, UIComponent* _component ) ;
        virtual bbool   onUpdateShape( Actor* _actor, UIComponent* _component, PickingShape_Disc* _shape, const UIData _shapeData ) ;
        virtual void    onSetShapePos( Actor* _actor, UIComponent* _component, PickingShape_Disc* _shape, const UIData _shapeData, const Vec3d& _worldPos ) {}

    private:

    };

} // namespace ITF


#endif // _ITF_UI_PLUGIN_H_
