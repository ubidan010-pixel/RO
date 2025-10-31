#ifndef _ITF_RAY_BEZIERTREE_PLUGIN_H_
#define _ITF_RAY_BEZIERTREE_PLUGIN_H_

#ifndef _ITF_BASECOMPONENTPLUGIN_H_
#include "rayman/ray_editor/plugins/BaseComponentPlugin.h"
#endif //_ITF_BASECOMPONENTPLUGIN_H_

#ifndef _ITF_RAY_BEZIERTREEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BezierTreeAIComponent.h"
#endif //_ITF_RAY_BEZIERTREEAICOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{
    class TweenInstruction;

    enum ShapeType
    {
        ShapeType_NodePos,
        ShapeType_NodeTangent,
        ShapeType_NodeScale,
    };

    union ShapeData
    {
        explicit ShapeData(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
        struct 
        {
            ShapeType m_type:4;
            u32 m_branchIndex:14;
            u32 m_nodeIndex:14;
        };
    };

    union MenuData
    {
        explicit MenuData(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
        struct 
        {
            u32 m_branchIndex:12;
            u32 m_nodeIndex:10;
            i32 m_itemIndex:10;
        };
    };

    class Ray_BezierTree_Plugin : public BaseComponentPlugin<Ray_BezierTreeAIComponent, ShapeData>,
                                   public Interface_InputListener
    {
        static Ray_BezierTree_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        Ray_BezierTree_Plugin();
        virtual ~Ray_BezierTree_Plugin();

        virtual void    onUpdate();

        virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component );
        virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component );

        // Interface_InputListener
        virtual bbool   onMouseButton   (   InputAdapter::MouseButton _button, InputAdapter::PressStatus _status );
        virtual bbool   onMousePos      (   i32 /*_x*/, i32 /*_y*/ )                                            { return btrue; }
        virtual bbool   onMouseWheel    (   i32 /*_wheelValue*/, i32 /*_wheelDelta*/    )                       { return btrue; }
        virtual bbool   onKey           (   i32 /*_key*/, InputAdapter::PressStatus /*_status*/ )               { return btrue; }
        // Interface_InputListener

    protected:

        virtual void    onUpdate( Actor* _actor, Ray_BezierTreeAIComponent* _component );
        virtual void    onCreateShapes( Actor* _actor, Ray_BezierTreeAIComponent* _component );
        virtual Editor_IconStock::EditorIconID getShapeIcon( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData, f32& _iconSize );
        virtual bbool   onUpdateShape( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData );
        virtual void    onSetShapePos( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData, const Vec3d& _worldPos );
        virtual bbool   onDeleteShape( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData );

    private:

        typedef Ray_BezierTreeAIComponent::BranchList BranchList;
        typedef Ray_BezierTreeAIComponent::Branch Branch;
        typedef Ray_BezierTreeAIComponent::Node Node;

        enum State
        {
            State_None,
            State_Add,
        };
        State m_state;

        void addBranch( Actor* _actor, Ray_BezierTreeAIComponent* component );
        void addNode( Actor* _actor, Ray_BezierTreeAIComponent* _component, const ShapeData _shapeData );
        void setTweenSet( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData );
        void setTweenOffset( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData );
        void setSpawnable( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData );
        void reinit( Ray_BezierTreeAIComponent* _component );
        void reinitFull( Ray_BezierTreeAIComponent* _component );

        const TweenInstructionSetList_Template& getTweenSetList( Ray_BezierTreeAIComponent* _component );

        ActorRef m_justAddedActor;
        ShapeData m_justAddedData;
    };

} // namespace ITF


#endif // _ITF_RAY_BEZIERTREE_PLUGIN_H_
