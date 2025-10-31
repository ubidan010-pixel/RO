#ifndef _ITF_CHARACTERDEBUGGER_PLUGIN_H_
#define _ITF_CHARACTERDEBUGGER_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF
{
    class CharacterDebugger_Plugin : public Interface_ActorEditorListener
    {
        static CharacterDebugger_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();

        virtual void    onDraw();

        virtual void    fillContextMenu( EditorContextMenu& /*_menu*/, Actor* /*_actor*/, ActorComponent* /*_component*/ ) {}
        virtual void    onContextMenuItemSelected( ContextMenuItem* /*_item*/, Actor* /*_actor*/, ActorComponent* /*_component*/ ) {}
        virtual void    fillAltContextMenu( EditorContextMenu& /*_menu*/ ) {}
        virtual void    onAltContextMenuItemSelected( ContextMenuItem* /*_item*/ ) {}
        virtual void    fillMainMenu( EditorContextMenu& /*_menu*/ ) {}
        virtual void    onMainMenuItemSelected( ContextMenuItem* /*_item*/ ) {}
        virtual bbool   onDeletePickingShape(PickingShape* /*_pShape*/) { return bfalse; }
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectDuplicated(Actor* /*_pActor*/, Actor* /*_pActorSrc*/, ActorComponent* /*_pComp*/) {}
        virtual void    onObjectChanged( Actor* /*_actor*/, ActorComponent* /*_component*/ ) {}
        virtual void    onUpdate() {}
        virtual void    onUpdateSelectedActor( Actor* /*_actor*/, ActorComponent* /*_component*/ ) {}
        virtual void    addComponentSpecificIcons( Actor* /*_actor*/, ActorComponent* /*_component*/, SafeArray<Editor_IconStock::EditorIconID>& /*_icons*/, f32& /*_IconSize*/ ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* /*_shape*/, SafeArray<Editor_IconStock::EditorIconID>& /*_icons*/, f32& /*_IconSize*/ ) {}
        virtual void    fillEditorActiveList( ObjectRefList &/*_pickablesObjects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/ ) const {}
        virtual bbool   onSetShapePosition(PickingShape* /*_pPickingShape*/, const Vec2d& /*_screenspacepos*/) { return bfalse; }
        virtual bbool   onOffsetShapePosition(PickingShape* /*_pPickingShape*/, const Vec3d& /*_deltaPos*/ ) { return bfalse; }
        virtual void    onShapePosChanged(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, PickingShape* /*_pShape*/, const Vec3d& /*_deltaPos*/) { }
        virtual bbool   needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {return bfalse;}
        virtual void    addComponentSpecificPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {}
        virtual void    updateComponentSpecificPickingShape(Actor* /*_pActor*/, PickingShape* /*_pShape*/, PickingShapeVector& /*_activeShapes*/) {}
    };

} // namespace ITF


#endif // _ITF_CHARACTERDEBUGGER_PLUGIN_H_
