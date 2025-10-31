#ifndef _ITF_TWEENING_PLUGIN_H_
#define _ITF_TWEENING_PLUGIN_H_

#ifndef _ITF_BASECOMPONENTPLUGIN_H_
#include "rayman/ray_editor/plugins/BaseComponentPlugin.h"
#endif //_ITF_BASECOMPONENTPLUGIN_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{
    class TweenInstruction;

    enum InstructionDataType
    {
        InstructionDataType_BallisticTarget = 0,
        InstructionDataType_BallisticTangent = 1,
    };

    union InstructionData
    {
        explicit InstructionData(u32 _raw = 0) : m_raw(_raw) {}
        u32 m_raw;
        struct 
        {
            u32 m_setIndex:15;
            u32 m_insIndex:15;
            InstructionDataType m_type:2;
        };
    };

    class Tweening_Plugin : public BaseComponentPlugin<TweenComponent, InstructionData>
    {
        static Tweening_Plugin* s_instance;

    public:

        static Interface_ActorEditorListener*   create();
        virtual void                            destroy();


        Tweening_Plugin();
        virtual ~Tweening_Plugin();

        virtual void    fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component );
        virtual void    onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component );
        virtual void    fillAltContextMenu( EditorContextMenu& _menu );
        virtual void    onAltContextMenuItemSelected( ContextMenuItem* _item );

        virtual void    onDraw();

    protected:

        virtual void    onCreateShapes( Actor* _actor, TweenComponent* _component );
        virtual bbool   onUpdateShape( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData );
        virtual void    onSetShapePos( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData, const Vec3d& _worldPos );
        virtual Editor_IconStock::EditorIconID getShapeIcon( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData, f32& _iconSize );

    private:

        bbool           m_drawDebugAll;
        ActorRefList    m_drawDebugActors;

        TweenInstruction* getInstruction( TweenComponent* _component, const InstructionData _shapeData );
        Pickable* getObjectInClipboard();

        void fillContextMenu_StartSet( EditorContextMenu& _menu, TweenComponent* _component );
        void onContextMenuItemSelected_StartSet( Actor* _actor, TweenComponent* component, u32 _setIndex );
        void fillContextMenu_FxTarget( EditorContextMenu& _menu, TweenComponent* _component );
        void onContextMenuItemSelected_FxTarget( Actor* _actor, TweenComponent* _component, InstructionData _menuData );
    };

} // namespace ITF


#endif // _ITF_TWEENING_PLUGIN_H_
