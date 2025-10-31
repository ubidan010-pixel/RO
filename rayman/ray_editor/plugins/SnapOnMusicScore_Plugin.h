#ifndef _ITF_SNAPONMUSICSCORE_PLUGIN_H_
#define _ITF_SNAPONMUSICSCORE_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

namespace ITF
{
    class Ray_MusicScoreSnapComponent;

    // ********************************************************************

    class SnapOnMusicScore_Plugin : public Interface_ActorEditorListener
    {
        static SnapOnMusicScore_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        SnapOnMusicScore_Plugin();
        virtual ~SnapOnMusicScore_Plugin();

        virtual bbool   onDeletePickingShape(PickingShape* _shape) { return bfalse; }
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectChanged( Actor* _pActor, ActorComponent* _pComp );
        virtual void    onObjectDuplicated(Actor* _pActor, Actor* _pActorSrc, ActorComponent* _pComp) {}
        virtual void    fillContextMenu (EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component ) {} 
        virtual void    onContextMenuItemSelected(ContextMenuItem* _item, Actor* _actor, ActorComponent* _component ) {} 
        virtual void    fillAltContextMenu (EditorContextMenu& /*_menu*/) {}
        virtual void    onAltContextMenuItemSelected(ContextMenuItem* /*_item*/) {}
        virtual void    onUpdate() {}
        virtual void    onDraw() {}
        virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component )  {}
        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize )  {} 
        virtual void    fillMainMenu (EditorContextMenu& /*_menu*/)  {} 
        virtual void    onMainMenuItemSelected(ContextMenuItem* /*_item*/)  {} 
        virtual void    fillEditorActiveList( ObjectRefList &/*_pickablesObjects*/, PickingShapeVector& /*_orderedShapes*/, Scene* /*_pFilterScene*/ ) const {}
        virtual bbool   needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {return bfalse;}
        virtual void    addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component)  {} 
        virtual void    updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes)  {} 
        virtual bbool   onSetShapePosition(PickingShape* _pickingShape, const Vec2d& _screenspacepos)  { return bfalse; } 
        virtual bbool   onOffsetShapePosition(PickingShape* _pickingShape, const Vec3d& _deltaPos ) { return bfalse; }      
        virtual void    onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& /*_deltaPos*/);

         static void    snap( Actor* _pActor, Ray_MusicScoreSnapComponent* _pComp, bbool _fromMusicScore = bfalse );

    private:


    };

} // namespace ITF


#endif // _ITF_SNAPONMUSICSCORE_PLUGIN_H_
