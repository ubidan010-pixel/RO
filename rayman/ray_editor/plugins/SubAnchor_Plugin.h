#ifndef _ITF_SUBANCHOR_PLUGIN_H_
#define _ITF_SUBANCHOR_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF {

class Ray_SubAnchorComponent;

//------------------------------------------------------------------------------
/// Adds named additional picking shapes for actors
class SubAnchor_Plugin : public Interface_ActorEditorListener
{
    static SubAnchor_Plugin* s_instance;

public:
    static Interface_ActorEditorListener*   create();
    virtual void                            destroy();

    SubAnchor_Plugin();
    virtual ~SubAnchor_Plugin();

    virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component ) {}
    virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component ) {}
    virtual void    fillAltContextMenu( EditorContextMenu& _menu ) {}
    virtual void    onAltContextMenuItemSelected( ContextMenuItem* _item ) {}
    virtual void    fillMainMenu( EditorContextMenu& _menu ) {}
    virtual void    onMainMenuItemSelected( ContextMenuItem* _item ) {}

    virtual bbool   onDeletePickingShape( PickingShape* _shape ) { return bfalse; }
    virtual void    onSelected( Actor* _actor ) {}
    virtual void    onObjectDuplicated( Actor* _actor, Actor* _actorSrc, ActorComponent* _component ) {}
    virtual void    onObjectChanged( Actor* _actor, ActorComponent* _component ) {}
    virtual void    onUpdate() {}
    virtual void    onDraw() {}
    virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component ) {}

    virtual void    fillEditorActiveList( ObjectRefList& _pickablesObjects, PickingShapeVector& _orderedShapes, Scene* _filterScene ) const {}
    virtual bbool   needsInitPosPickingShape( Actor* _actor, ActorComponent* _component ) { return bfalse; }

    virtual void    addComponentSpecificPickingShape( Actor* _actor, ActorComponent* _component );
    virtual void    updateComponentSpecificPickingShape( Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes );
    virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize );
    virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize ) {}

    virtual bbool   onSetShapePosition( PickingShape* _shape, const Vec2d& _screenSpacePos );
    virtual bbool   onOffsetShapePosition( PickingShape* _shape, const Vec3d& _deltaPos );
    virtual void    onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& _deltaPos);

private:
    PickingShape_Disc* getDisc(PickingShape* _shape);
    Ray_SubAnchorComponent* getComponent(PickingShape* _shape);
    bbool isSelected(PickingShape* _shape);
    Vec3d getWorldPosition(PickingShape_Disc* _disc, const Vec2d& _screenSpacePos);
};

} // namespace ITF

#endif // _ITF_SUBANCHOR_PLUGIN_H_
