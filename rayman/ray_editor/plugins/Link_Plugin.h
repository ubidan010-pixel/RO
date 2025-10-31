#ifndef _ITF_LINK_PLUGIN_H_
#define _ITF_LINK_PLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    class Link_Plugin : public Interface_ActorEditorListener, public Interface_InputListener
    {
        friend class LinkEditor;

        static Link_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        Link_Plugin();
        virtual ~Link_Plugin();

        virtual bbool   onDeletePickingShape(PickingShape* _shape);
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectDuplicated(Actor* /*_pActor*/, Actor* /*_pActorSrc*/, ActorComponent* /*_pComp*/) {}
        virtual void    onObjectChanged( Actor* _actor, ActorComponent* _component ) {}
        virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component );
        virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component );
        virtual void    fillAltContextMenu( EditorContextMenu& _menu );
        virtual void    onAltContextMenuItemSelected( ContextMenuItem* _item );
        virtual void    onUpdate();
        virtual void    onDraw();

        void            addSelectionAndHoveredToDrawList( const bbool _backtrackParents );
        virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component ) {}
        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize ) {}
        virtual void    fillMainMenu (EditorContextMenu& /*_menu*/) {}
        virtual void    onMainMenuItemSelected(ContextMenuItem* /*_item*/) {}
        virtual void    fillEditorActiveList( ObjectRefList &_pickablesObjects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const;
        virtual bbool   onSetShapePosition(PickingShape* /*_pPickingShape*/, const Vec2d& /*_screenspacepos*/) { return bfalse; }
        virtual bbool   onOffsetShapePosition(PickingShape* /*_pPickingShape*/, const Vec3d& /*_deltaPos*/ ) { return bfalse; }
        virtual void    onShapePosChanged(Actor* /*_pActor*/, ActorComponent* /*_pComp*/, PickingShape* /*_pShape*/, const Vec3d& /*_deltaPos*/) { }

        virtual bbool   needsInitPosPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {return bfalse;}
        virtual void    addComponentSpecificPickingShape(Actor* /*_pActor*/, ActorComponent* /*_pComp*/) {};
        virtual void    updateComponentSpecificPickingShape(Actor* /*_pActor*/, PickingShape* /*_pShape*/, PickingShapeVector& /*_activeShapes*/) {};

        // From InputInterface
        virtual bbool   onMouseButton   (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
        virtual bbool   onMousePos      (   i32 /*_x*/, i32 /*_y*/ )                                            { return btrue; }
        virtual bbool   onMouseWheel    (   i32 /*_wheelValue*/, i32 /*_wheelDelta*/    )                       { return btrue; }
        virtual bbool   onKey           (   i32 /*_key*/, InputAdapter::PressStatus /*_status*/ )               { return btrue; }

    private:

        void            fillDebugModeMenuItems( EditorContextMenu& _menu );
        void            onDebugModeMenuItemSelected( ContextMenuItem* _item );
        void            addLinkToDrawList( LinkComponent* _linkComponent, u32 _childIndex, const bbool _isSelected );
        bbool           otherComponentDrawsTheLink( const Actor* parent, const Actor* child );
        void            addAllLinksToDrawList();
        bbool           canLinkActorToPickable(const Actor* _pActorParent, const Pickable* _pChild, String* _tip = NULL)const;
        bbool           isLinkedActorToPickable(const Actor* _pActorParent, const Pickable* _pChild)const;
        void            pasteObjectPathFromClipboard( ObjectRef _target );
        void            removeObjectPathFromClipboard( ObjectRef _target );
        void            clearChildren( Actor* _actor );

        void            getDeadLinks( LinkComponent* _linkComponent, LinkComponent::ChildrenList& _deadChildren );
        void            clearDeadLinks( Actor* _actor );
        void            getAllDeadLinks( LinkComponent::ChildrenList& _deadChildren );
        void            clearAllDeadLinks();

        bbool           m_linkingFromShortcut;
        bbool           m_linkingFromContextMenu;
        ObjectRef       m_lastHoveredSubSceneActor;

        struct DisplayedLink
        {
            ObjectRef   parent, child;
            Vec2d       parentScreenPos, childScreenPos;
            Vec3d       parentWorldPos, childWorldPos;
            Color       color;
            u32         drawDbgIndex;
        };
        ITF_VECTOR<DisplayedLink> m_displayedListCurrentFrame;
    };

} // namespace ITF


#endif // _ITF_LINK_PLUGIN_H_
