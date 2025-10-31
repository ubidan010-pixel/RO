#ifndef _ITF_MISC_PLUGIN_H_
#define _ITF_MISC_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF
{
    /*
        Rayman-specifc stuff that didn't deserve their own plugin:
            - Ray_AvoidanceManager
     */
    class Misc_Plugin : public Interface_ActorEditorListener
    {
        static Misc_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        Misc_Plugin();
        virtual ~Misc_Plugin();

        virtual bbool   onDeletePickingShape(PickingShape* /*_shape*/) { return bfalse; }
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectDuplicated( Actor* /*_actor*/, Actor* /*_actorSrc*/, ActorComponent* /*_component*/) {}
        virtual void    onObjectChanged( Actor* _actor, ActorComponent* _component ) {}
        virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component );
        virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component );
        virtual void    fillAltContextMenu( EditorContextMenu& _menu );
        virtual void    onAltContextMenuItemSelected( ContextMenuItem* _item );
        virtual void    fillMainMenu( EditorContextMenu& _menu ) {}
        virtual void    onMainMenuItemSelected( ContextMenuItem* _item ) {}
        virtual void    onUpdate() {}
        virtual void    onDraw() {}
        virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component ) {}
        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    fillEditorActiveList( ObjectRefList &/*_pickablesObjects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/ ) const {}
        virtual bbool   onSetShapePosition(PickingShape* /*_pPickingShape*/, const Vec2d& /*_screenspacepos*/) { return bfalse; }
        virtual bbool   onOffsetShapePosition(PickingShape* /*_pPickingShape*/, const Vec3d& /*_deltaPos*/ ) { return bfalse; }
        virtual void    onShapePosChanged(Actor* /*_actor*/, ActorComponent* /*_component*/, PickingShape* /*_shape*/, const Vec3d& /*_deltaPos*/) {}
    
        virtual bbool   needsInitPosPickingShape(Actor* /*_actor*/, ActorComponent* /*_component*/) { return bfalse; }
        virtual void    addComponentSpecificPickingShape(Actor* /*_actor*/, ActorComponent* /*_component*/) {}
        virtual void    updateComponentSpecificPickingShape(Actor* /*_actor*/, PickingShape* /*_shape*/, PickingShapeVector& /*_activeShapes*/) {}

    private:

        void randomizeSyncOffsetOnSelection();
    };

} // namespace ITF


#endif // _ITF_MISC_PLUGIN_H_
