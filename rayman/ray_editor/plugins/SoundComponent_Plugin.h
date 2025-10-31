#ifndef _ITF_SOUNDCOMPONENT_PLUGIN_H_
#define _ITF_SOUNDCOMPONENT_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_


namespace ITF
{
    class SoundComponent_Plugin : public Interface_ActorEditorListener
    {
    public:
        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();

        SoundComponent_Plugin();
        virtual ~SoundComponent_Plugin();

        virtual void                            onSelected(Actor* _pActor);
        virtual void                            onObjectDuplicated(Actor* /*_pActor*/, Actor* /*_pActorSrc*/, ActorComponent* /*_pComp*/) {}
        virtual void                            onObjectChanged(Actor* _pActor, ActorComponent* _pComp);
        virtual void                            fillContextMenu (EditorContextMenu& /*_menu*/, Actor* /*_pActor*/, ActorComponent* /*_pComp*/);
        virtual void                            onContextMenuItemSelected(ContextMenuItem* /*_item*/, Actor* /*_pActor*/, ActorComponent* /*_pComp*/);
        virtual void                            fillAltContextMenu(EditorContextMenu& _menu);
        virtual void                            onAltContextMenuItemSelected(ContextMenuItem* _item);

        virtual void                            onUpdate();
        virtual void                            onDraw();
        virtual void                            onUpdateSelectedActor(Actor* /*_pActor*/, ActorComponent* /*_pComp*/);
        virtual void                            addComponentSpecificIcons(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& /*_IconSize*/) {}
        virtual void                            addPickingShapeSpecificIcons(PickingShape* /*_shape*/, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& /*_IconSize*/) {}
        virtual void                            fillMainMenu (EditorContextMenu& /*_menu*/);
        virtual void                            onMainMenuItemSelected(ContextMenuItem* /*_item*/);
        virtual bbool                           onDeletePickingShape(PickingShape* /*_pShape*/) { return bfalse; }
        virtual void                            fillEditorActiveList( ObjectRefList &/*_pickablesObjects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/ ) const {};
        virtual bbool                           onSetShapePosition(PickingShape* /*_pPickingShape*/, const Vec2d& /*_screenspacepos*/) { return bfalse; }
        virtual bbool                           onOffsetShapePosition(PickingShape* /*_pPickingShape*/, const Vec3d& /*_deltaPos*/ ) { return bfalse; }
        virtual void                            onShapePosChanged(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, PickingShape* /*_pShape*/, const Vec3d& /*_deltaPos*/) { }

        virtual bbool                           needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {return bfalse;}
        virtual void                            addComponentSpecificPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {};
        virtual void                            updateComponentSpecificPickingShape(Actor* /*_pActor*/, PickingShape* /*_pShape*/, PickingShapeVector& /*_activeShapes*/) {};

    private:
        static SoundComponent_Plugin*           m_instance;

        EditorContextMenu&  createSubMenu(EditorContextMenu& _menu);

        void                debugMarkedActors();
        ActorRefList                            m_showDebugInfoActors;

        void                setBpm( u32 _bpm );
        void                debugMetronome();
        bbool               m_debugMetronome;
    };

} // namespace ITF


#endif // _ITF_SOUNDCOMPONENT_PLUGIN_H_
