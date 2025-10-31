#ifndef _ITF_BASECOMPONENTPLUGIN_H_
#define _ITF_BASECOMPONENTPLUGIN_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

namespace ITF
{
    template <class TComponent, class TShapeData>
    class BaseComponentPlugin : public Interface_ActorEditorListener
    {
    public:

        BaseComponentPlugin() {}
        virtual ~BaseComponentPlugin() {}

        virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component ) {}
        virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component ) {}
        virtual void    fillAltContextMenu( EditorContextMenu& _menu ) {}
        virtual void    onAltContextMenuItemSelected( ContextMenuItem* _item ) {}
        virtual void    fillMainMenu( EditorContextMenu& _menu ) {}
        virtual void    onMainMenuItemSelected( ContextMenuItem* _item ) {}

        virtual bbool   onDeletePickingShape( PickingShape* _shape );
        virtual void    onSelected( Actor* _actor ) {}
        virtual void    onObjectDuplicated( Actor* _actor, Actor* _actorSrc, ActorComponent* _component ) {}
        virtual void    onObjectChanged( Actor* _actor, ActorComponent* _component ) {}
        virtual void    onUpdate() {}
        virtual void    onDraw() {}
        virtual void    onUpdateSelectedActor( Actor* _actor, ActorComponent* _component ) { onUpdate(_actor, static_cast<TComponent*>(_component)); }

        virtual void    fillEditorActiveList( ObjectRefList& _pickablesObjects, PickingShapeVector& _orderedShapes, Scene* _filterScene ) const {}
        virtual bbool   needsInitPosPickingShape(Actor* _actor, ActorComponent* _component) { return bfalse; }

        virtual void    addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component);
        virtual void    updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes);
        virtual bbool   onSetShapePosition(PickingShape* _shape, const Vec2d& _screenSpacePos);
        virtual bbool   onOffsetShapePosition(PickingShape* _shape, const Vec3d& _deltaPos );
        virtual void    onShapePosChanged(Actor* _actor, ActorComponent* _component, PickingShape* _shape, const Vec3d& /*_deltaPos*/) {}

        virtual void    addComponentSpecificIcons( Actor* _actor, ActorComponent* _component, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize ) {}
        virtual void    addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize );

    protected:

        virtual void    onCreateShapes( Actor* _actor, TComponent* _component ) = 0;
        virtual bbool   onUpdateShape( Actor* _actor, TComponent* _component, PickingShape_Disc* _shape, const TShapeData _shapeData ) = 0;
        virtual void    onSetShapePos( Actor* _actor, TComponent* _component, PickingShape_Disc* _shape, const TShapeData _shapeData, const Vec3d& _worldPos ) = 0;
        virtual Editor_IconStock::EditorIconID getShapeIcon( Actor* _actor, TComponent* _component, PickingShape_Disc* _shape, const TShapeData _shapeData, f32& _iconSize );
        virtual void    onUpdate( Actor* _actor, TComponent* _component ) {}
        virtual bbool   onDeleteShape( Actor* _actor, TComponent* _component, PickingShape_Disc* _shape, const TShapeData _shapeData ) { return bfalse; }

        // HELPERS
        Actor*          getActor( PickingShape* _shape );
        TComponent*     getComponent( PickingShape* _shape );
        TShapeData      getData( PickingShape* _shape );
        PickingShape_Disc* safeCastDisc( PickingShape* _shape );
        bbool           isSelected( PickingShape* _shape );
        TComponent*     getSelectedComponent();
        PickingShape_Disc* getSelectedSubShape( TShapeData* _data = NULL );
        Vec3d           getWorldPosition( PickingShape_Disc* _shape, const Vec2d& _screenSpacePos );
        Vec3d           getWorldPosition( PickingShape_Disc* _shape, const Vec3d& _deltaPos );
        PickingShape_Disc* createShape( const Actor* _actor, const TShapeData _shapeData, bbool _allowSnapWithMainShape );
        void            recreateShapes( ActorRef _actor );
        u32             getShapeCount( ActorRef _actor );
    };



    template <class TComponent, class TShapeData>
    void BaseComponentPlugin<TComponent, TShapeData>::addComponentSpecificPickingShape( Actor* _actor, ActorComponent* _component )
    {
        TComponent* component = static_cast<TComponent*>(_component);
        onCreateShapes(_actor, component);
    }

    template <class TComponent, class TShapeData>
    void BaseComponentPlugin<TComponent, TShapeData>::updateComponentSpecificPickingShape( Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes )
    {
        if (!_actor->isSelected())
        {
            return;
        }

        TComponent* component = getComponent(_shape);
        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);

        ITF_ASSERT_MSG(component, "the shape belongs this component-specific plugin, so I really should have this component!");
        if (!component)
        {
            return;
        }

        if ( onUpdateShape(_actor, component, disc, getData(_shape)) )
        {
            disc->setlastUpdateFrame(CURRENTFRAME);
            _activeShapes.push_back(disc);
        }
    }

    template <class TComponent, class TShapeData>
    void BaseComponentPlugin<TComponent, TShapeData>::addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _iconSize )
    {
        _icons.clear();
        _iconSize = 32;
        _icons.push_back( getShapeIcon(
            getActor(_shape), getComponent(_shape), static_cast<PickingShape_Disc*>(_shape), getData(_shape), _iconSize ) );
    }

    template <class TComponent, class TShapeData>
    Editor_IconStock::EditorIconID BaseComponentPlugin<TComponent, TShapeData>::getShapeIcon( Actor* _actor, TComponent* _component, PickingShape_Disc* _shape, const TShapeData _shapeData, f32& _iconSize )
    {
        if ( isSelected(_shape) )
        {
            return Editor_IconStock::Icon_SelectedTrajectoryNode;
        }
        else
        {
            return Editor_IconStock::Icon_TrajectoryNode;
        }
    }

    template <class TComponent, class TShapeData>
    bbool ITF::BaseComponentPlugin<TComponent, TShapeData>::onDeletePickingShape( PickingShape* _shape )
    {
        if (_shape->getCreatorPickingShapePlugin() != this)
        {
            return bfalse;
        }

        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }

        return onDeleteShape(getActor(_shape), getComponent(_shape), static_cast<PickingShape_Disc*>(_shape), getData(_shape));
    }



    template <class TComponent, class TShapeData>
    bbool BaseComponentPlugin<TComponent, TShapeData>::onSetShapePosition( PickingShape* _shape, const Vec2d& _screenSpacePos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        const Vec3d worldPos = getWorldPosition(disc, _screenSpacePos);
        onSetShapePos(getActor(_shape), getComponent(_shape), disc, getData(_shape), worldPos);

        return btrue;
    }

    template <class TComponent, class TShapeData>
    bbool BaseComponentPlugin<TComponent, TShapeData>::onOffsetShapePosition( PickingShape* _shape, const Vec3d& _deltaPos )
    {
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);
        const Vec3d worldPos = getWorldPosition(disc, _deltaPos);
        onSetShapePos(getActor(_shape), getComponent(_shape), disc, getData(_shape), worldPos);

        return btrue;
    }





    // HELPERS

    template <class TComponent, class TShapeData>
    Actor* BaseComponentPlugin<TComponent, TShapeData>::getActor( PickingShape* _shape )
    {
        return static_cast<Actor*>(_shape->getOwnerPtr());
    }

    template <class TComponent, class TShapeData>
    TComponent* BaseComponentPlugin<TComponent, TShapeData>::getComponent( PickingShape* _shape )
    {
        return getActor(_shape)->GetComponent<TComponent>();
    }

    template <class TComponent, class TShapeData>
    TShapeData BaseComponentPlugin<TComponent, TShapeData>::getData( PickingShape* _shape )
    {
        TShapeData data;
        data.m_raw = _shape->getData();
        return data;
    }

    template <class TComponent, class TShapeData>
    bbool BaseComponentPlugin<TComponent, TShapeData>::isSelected( PickingShape* _shape )
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

    // if one and only one actor is selected, and it has our component type, return it
    template <class TComponent, class TShapeData>
    TComponent* ITF::BaseComponentPlugin<TComponent, TShapeData>::getSelectedComponent()
    {
        const ObjectRefList& selection = EDITOR->getEditorSelectionAsObjRefList();
        if (selection.size() == 1)
        {
            if ( Actor* actor = IRTTIObject::SafeDynamicCast<Actor>(selection[0].getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                return actor->GetComponent<TComponent>();
            }
        }

        return NULL;
    }

    template <class TComponent, class TShapeData>
    PickingShape_Disc* ITF::BaseComponentPlugin<TComponent, TShapeData>::safeCastDisc( PickingShape* _shape )
    {
        if (_shape->getShapeType() == PickingShape::ShapeType_Disc)
        {
            return static_cast<PickingShape_Disc*>(_shape);
        }

        return NULL;
    }

    // if one and only one shape is selected, and it's one of our sub-shape, return it
    template <class TComponent, class TShapeData>
    PickingShape_Disc* ITF::BaseComponentPlugin<TComponent, TShapeData>::getSelectedSubShape( TShapeData* _data /*= NULL*/ )
    {
        const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();

        if (selection.size() == 1)
        {
            if ( PickingShape_Disc* shape = safeCastDisc(selection[0]) )
            {
                if ( shape->getCreatorPickingShapePlugin() == this &&
                     shape->isSubAnchor() )
                {
                    if (_data)
                    {
                        _data->m_raw = shape->getData();
                    }
                    return shape;
                }
            }
        }

        return NULL;
    }


    template <class TComponent, class TShapeData>
    Vec3d BaseComponentPlugin<TComponent, TShapeData>::getWorldPosition( PickingShape_Disc* _shape, const Vec2d& _screenSpacePos )
    {
        Vec3d screenPos;
        GFX_ADAPTER->compute3DTo2D(_shape->getPos(), screenPos);
        screenPos.m_x = _screenSpacePos.m_x + _shape->getPickingInfoPtr()->m_anchor.m_x;
        screenPos.m_y = _screenSpacePos.m_y + _shape->getPickingInfoPtr()->m_anchor.m_y;

        Vec3d worldPos;
        GFX_ADAPTER->compute2DTo3D(screenPos, worldPos);

        return worldPos;
    }

    template <class TComponent, class TShapeData>
    Vec3d BaseComponentPlugin<TComponent, TShapeData>::getWorldPosition( PickingShape_Disc* _shape, const Vec3d& _deltaPos )
    {
        return _shape->getPos() + _deltaPos;
    }

    template <class TComponent, class TShapeData>
    PickingShape_Disc* ITF::BaseComponentPlugin<TComponent, TShapeData>::createShape( const Actor* _actor, const TShapeData _shapeData, bbool _allowSnapWithMainShape )
    {
        PickingShape_Disc* disc = newAlloc(mId_Editor, PickingShape_Disc(this));
        EDITOR->addPickingShapeForObject(_actor->getRef(), disc);

        disc->setSubAnchor(btrue);
        disc->setData(_shapeData.m_raw);
        disc->setSnapWithMainShape(_allowSnapWithMainShape);

        return disc;
    }

    template <class TComponent, class TShapeData>
    u32 ITF::BaseComponentPlugin<TComponent, TShapeData>::getShapeCount( ActorRef _actor )
    {
        u32 shapeCount = 0;

        const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_actor);
        ITF_ASSERT(shapes);
        if (shapes)
        {
            for (u32 i = 0; i < shapes->size(); i++)
            {
                PickingShape* shape = (*shapes)[i];
                if (shape->getCreatorPickingShapePlugin() == this)
                {
                    shapeCount++;
                }
            }
        }

        return shapeCount;
    }

    template <class TComponent, class TShapeData>
    void ITF::BaseComponentPlugin<TComponent, TShapeData>::recreateShapes( ActorRef _actor )
    {
        Editor::getptr()->deletePickingShapesForObject(_actor);
        ActorEditor::getInstance()->createPickingShapeForActor(_actor);
    }


} // namespace ITF


#endif // _ITF_BASECOMPONENTPLUGIN_H_
