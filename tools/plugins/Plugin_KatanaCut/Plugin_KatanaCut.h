#ifndef _ITF_PLUGIN_KATANACUT_H_
#define _ITF_PLUGIN_KATANACUT_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_


#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor\PickingShape\PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

namespace ITF
{


// Class predeclarations
class   Blob;
class   PickingShape;
struct  PickingInfo2D;
struct  ContextMenuItem;
class   EditorContextMenu;

class DummyEditorObj : public Pickable
{
public:
    DECLARE_OBJECT_CHILD_RTTI(DummyEditorObj, Pickable,1606928822);
    DECLARE_SERIALIZE()

    DummyEditorObj();
    virtual void update(f32 /*_deltaTime*/);
    virtual void draw();
};

///////////////////////////////////////////////////////////////////////////////////////////
class Plugin_KatanaCut : public Plugin
{
public:
    enum CutMode
    {
        CutMode_Idle,
        CutMode_StartNextClick,
        CutMode_Active
    };

    static const char*      getPluginName           (   ) {return  "Plugin_KatanaCut";}

                            Plugin_KatanaCut        (   );
                            ~Plugin_KatanaCut       (   );

    // Plugin inherited
                            void                    receive             (   Blob& _blob, const NETPeer* _peer, u32 _senderID ) {};
    bbool                   isPickingAllowed    (   );
    void                    onMouseButton       (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    void                    update              (               );
    bbool                   onPickShape         (   PickingShape* _shape  );
    bbool                   onEditorRemove      (   const SafeArray<PickingShape*>& _shapesWithSameOwner   );
    void                    onKey               (   i32 _key, InputAdapter::PressStatus _status );
    bbool                   bCanEnterEditMode   (   ) {return btrue;}
    bbool                   bCanLeaveEditMode   (   ) {return btrue;}
    virtual void            onClearScene        (   );
    virtual void            onDeleteScene       (   );


    void                    UnselectAll         (   bbool _force    );
    virtual void            fillEditorForcedList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes)const {};
    void                    fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const;
    virtual void            onFileDrop (const String& _fileName) {};
    virtual bbool           fillContextMenu (EditorContextMenu& _menu, ObjectRef _target, bbool _topItems);
    virtual void            onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget);
    virtual void            onSwitchToWorld(World* _pWorld, bbool _bSwitchDone){};
    virtual void            onDeleteScene(const Scene* _pScene){};
    virtual void            onObjectAdded(const Scene* _pScene, BaseObject* _pObj){};
    virtual void            onPostSceneActivation(const Scene* _pScene, bbool _bActive){};
    virtual void            onObjectSpawned(const BaseObject* _newObject){};
    virtual void            cancelCurrentAction(bbool _keepEditorStuff);
    virtual void            enterSubEdit( ObjectRef _or ){};
    virtual void            onObjectCreated(BaseObject* _pObject){};
    virtual void            onObjectChanged(BaseObject* _pObject){};
    virtual void            onObjectsDuplicated (const SafeArray <BaseObject*>& /*_from*/, const SafeArray <BaseObject*>& /*_to*/){};
    virtual void            stopEditor();
    virtual bbool           onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
    virtual bbool           onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );

    static void              setMode(CutMode _mode);
    static void              drawEdge();

    // Internal

private:
    class SelectionDisc : public PickingShape_Disc
    {
    public:
        Vec3d               m_position;
        SelectionDisc() : PickingShape_Disc() {};
        virtual void        draw();
    };

    struct EdgeCutData
    {
        u32 m_edgeIndex;
        Vec2d m_worldPos;
        f32 m_ratio;
    };

    struct FriezeCutData
    {
        class Frise* m_frise;
        ITF_VECTOR<EdgeCutData> m_cutData;
    };

    enum ShapeMode
    {
        ShapeMode_Free,
        ShapeMode_Vertical,
        ShapeMode_Hozizontal
    };

    enum CutZMode
    {
        CutZMode_CurZ,
        CutZMode_AllZ,
        CutZMode_Selection,
    };

    SafeArray<class Frise*>  m_targets;

    void                    createTargetsList();
    void                    processTargetsList();
    void                    createTargetsList_CurZ();
    void                    createTargetsList_AllZ();
    void                    createTargetsList_Selection();
    void                    processKatanaCut();
    void                    buildFriezeParts( const Frise* _friezeToCut, FriezeCutData& _friezeCutData, u32 _cutDataIndex, u32 _indexEdgeLast );
    void                    addEdgeCutOnFillingVB( const Frise* _friezeToCut, Frise* frieze, f32 _ratio, u32 _indexEdgeLast );
    void                    buildFriezeCut(Frise* _friezeToCut, FriezeCutData& _friezeCutData, u32 _indexEdgeLast );
    static bbool            isSelected();
    void                    cutFrieze(class Frise* currentPolyLine, const Vec2d& _cutPoint, u32 selectedEdgeIndex);
    void                    reset();
    void                    createHolder();
    void                    splitFrieze( Frise* _friezeToCut, u32 _index, f32 _ratio );
    bbool                   isSplitable( const Frise* _friezeToCut);


    bbool                    m_go;
    static void              pushShapesToVector(PickingShapeVector& _orderedShapes);
    i32                      getShapeIndex(const PickingShape* _pick) const;
    void                     onShapeNewPos(const i32 _index, const Vec3d& _newPos);
    i32                      m_currentShapeIndex;
    static PickingInfo2D     m_currentPickingInfo;
    static ITF_VECTOR<PickingShape*> m_shapes;    // 0 : pt1, 1 : pt2
    static CutMode           m_mode;
    static Pickable*         m_pickingShapesHolder;
    ShapeMode                m_shapeMode;
    CutZMode                 m_cutMode;
    static SafeArray<class Frise*> m_selection;
    static SafeArray<ObjectRef> m_postSelection;
    ITF_VECTOR<FriezeCutData> m_cutData;
    int                     m_deleteShapes;
    static Plugin_KatanaCut *m_instance;
};

} // namespace ITF


#endif // _ITF_PLUGIN_KATANACUT_H_
