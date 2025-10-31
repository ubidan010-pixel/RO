#ifndef _ITF_CURVE_PLUGIN_H_
#define _ITF_CURVE_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

namespace ITF
{

    union TrajectoryExtraInfo
    {
        explicit TrajectoryExtraInfo(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
        struct 
        {
            u32 m_index:30;
            u32 m_type:2;
        };
    };

    // ********************************************************************

    class Curve_Plugin : public Interface_ActorEditorListener
    {
        static Curve_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        Curve_Plugin();
        virtual ~Curve_Plugin();

        virtual bbool   onDeletePickingShape(PickingShape* _shape);
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectChanged( Actor* _actor, ActorComponent* _component );
        virtual void    onObjectDuplicated(Actor* _pActor, Actor* _pActorSrc, ActorComponent* _pComp) {}
        virtual void    fillContextMenu (EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component );
        virtual void    onContextMenuItemSelected(ContextMenuItem* _item, Actor* _actor, ActorComponent* _component );
        virtual void    fillAltContextMenu (EditorContextMenu& /*_menu*/) {};
        virtual void    onAltContextMenuItemSelected(ContextMenuItem* /*_item*/) {};
        virtual void    onUpdate();
        virtual void    onDraw() {}
        virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component );
        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize );
        virtual void    fillMainMenu (EditorContextMenu& /*_menu*/);
        virtual void    onMainMenuItemSelected(ContextMenuItem* /*_item*/);
        virtual void    fillEditorActiveList( ObjectRefList &/*_pickablesObjects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/ ) const {};
        virtual bbool   needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {return bfalse;}
        virtual void    addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component);
        virtual void    updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes);
        virtual bbool   onSetShapePosition(PickingShape* _pickingShape, const Vec2d& _screenspacepos);
        virtual bbool   onOffsetShapePosition(PickingShape* _pickingShape, const Vec3d& _deltaPos );
        virtual void    onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& /*_deltaPos*/);


       

    private:

                void    addNode(Actor* _actor, ActorComponent* _component, u32 _indexNode, bbool _insertAfter);

    };

} // namespace ITF


#endif // _ITF_CURVE_PLUGIN_H_
