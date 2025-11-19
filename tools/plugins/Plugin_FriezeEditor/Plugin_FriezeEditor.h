#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#define _ITF_PLUGIN_FRIEZEEDITOR_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor\PickingShape\PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

// Class predeclarations
class Blob;
class Frise;
class ColScene;
class PolyLine;
class PickingShape;
struct PickingInfo2D;
struct ContextMenuItem;
class EditorContextMenu;


union FriezeExtraInfo
{
    explicit FriezeExtraInfo(u32 _raw=0) : m_raw(_raw) {}
    u32 m_raw;
    struct
    {
        u32 m_subItemIndex:16;
        u32 m_reserved:14;
        u32 m_isPivot:1;
        u32 m_isEdge:1;
    } m_flags;
};
#ifdef DAN_TEXTURE_SELECTOR
    using BasePlugin = SamplerPlugin;
#else
    using BasePlugin = Plugin;
#endif

///////////////////////////////////////////////////////////////////////////////////////////
class FriezeEditor : public BasePlugin
{
public:
    //////////////////////////////////////////////////////////////////////////
    ///Internal class for picking shape of Frieze
    class PickingShapeData : public PickingShapeData_RotationScale
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        ///Construction
        PickingShapeData(PickingShape* _pParentShape, Plugin* _pPlugin);

        //////////////////////////////////////////////////////////////////////////
        ///update
        virtual void        update();

        //////////////////////////////////////////////////////////////////////////
        ///Draw component
        virtual void        draw();

        //////////////////////////////////////////////////////////////////////////
        /// Change point scale
        virtual void        handleMouseWheel(i32 _wheelDelta);

        //////////////////////////////////////////////////////////////////////////
        /// Rotate the frieze
        virtual void        rotateAroundAxisZ(f32 _deltaAngle, const Vec3d* _pRotationCenter = NULL);

        //////////////////////////////////////////////////////////////////////////
        /// Scale the frieze
        virtual void        scale(const Vec2d& _deltaScale);
		static Vec2d		scale( Frise* _target, PickingShapeData_RotationScale* _scaler, const Vec2d& _newScale, const Vec2d& _prevHelperPos, const Vec3d& _scaleCenter);


    private:
        void                drawPoint(bbool _mouseOver, const FriezeExtraInfo& _data, u32 _color = 0xffffffff);
        void                drawEdge(bbool _mouseOver, const FriezeExtraInfo& _data, u32 _color = 0xffffffff);
    };

public:
    enum ShowInfoMode
    {
        ShowInfoMode_None,
        ShowInfoMode_MainAnchorOnly,
        ShowInfoMode_LinesOnly,             // No anchor at all
        ShowInfoMode_SubAnchorsAndLines  ,   // excluding main anchor,
        ENUM_FORCE_SIZE_32(0)
    };

    static const char*      getPluginName               (   ) {return  "EditorPolyline_Plugin";}

    // beware, only powers of 2
    static const u32    FriezeInfoFlag_None = 0;
    static const u32    FriezeInfoFlag_Lines = 1;
    static const u32    FriezeInfoFlag_MainAnchor = 2;
    static const u32    FriezeInfoFlag_SubAnchors = 4;
    static const u32    FriezeInfoFlag_Any = 0xffffffff;

                            FriezeEditor          (   );
    bbool                   isPickingAllowed        (   );
    void                    createFrise             (   );
    void                    clearScene              (   );
    void                    update                  (   float _ellapsed );
    Frise*					getCurrentFrieze      (   )const;
    void                    setCurrentFrieze      ( Frise* _p, bbool _allowUnselect = btrue );
    void                    lockFrieze            ( Frise* _p, PickingInfo2D* _pPickingInfo2D = NULL );


    // Plugin inherited
    void                    receive         (   Blob& _blob, const NETPeer* _peer, u32 _senderID );
    void                    onMouseButton   (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
    void                    update          (               );
    bbool                   onPickShape     (   PickingShape* _shape  );
    bbool                   onEditorRemove  (   const SafeArray<PickingShape*>& _shapesWithSameOwner   );
    void                    onKey           (   i32 _key, InputAdapter::PressStatus _status );
    void                    onClearScene    (   );
    bbool                   bCanEnterEditMode ();
    bbool                   bCanLeaveEditMode ();
    void                    UnselectAll (bbool _force);
    virtual void            fillEditorForcedList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes)const;
    void                    fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const;
    virtual void            onFileDrop (const String& _fileName);
    virtual bbool           fillContextMenu (EditorContextMenu& _menu, ObjectRef _target, bbool _topItems);
    virtual void            onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget);
    virtual void            onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
    virtual void            onDeleteScene(const Scene* _pScene);
    virtual void            onObjectAdded(const Scene* _pScene, BaseObject* _pObj);
    virtual void            onObjectRemoved(const Scene* _pScene, BaseObject* _pObj, bbool _delete);
    virtual void            onPostSceneActivation(const Scene* _pScene, bbool _bActive);
    virtual void            onObjectSpawned(const BaseObject* _newObject);
    virtual void            cancelCurrentAction(bbool _keepEditorStuff);
    virtual void            enterSubEdit( ObjectRef _or );
    virtual bbool           isSubEditable( const BaseObject* _pObj )const;
    virtual void            onObjectCreated(BaseObject* _pObject);
    virtual void            onObjectChanged(BaseObject* _pObject);
    virtual void            onObjectsDuplicated (const SafeArray <BaseObject*>& /*_from*/, const SafeArray <BaseObject*>& /*_to*/);
    virtual bbool           onSetShapePosition(PickingShape* _pPickingShape, const Vec2d& _screenspacepos);
    virtual bbool           onOffsetShapePosition(PickingShape* _pPickingShape, const Vec3d& _deltaPos );


    static void             createPickingShapeForFrieze (const Frise* _frieze);
    PickingShapePluginData* createPickingShapeData(PickingShape *_shape);
    static void             updateFriezePickingShape(ObjectRef _or, ObjectRefList &_objects, PickingShapeVector& _orderedShapes);

    void                    switchSelectState();
    u32                     getFriezeInfoMask();
    bbool                   hasFriezeInfoFlag(u32 _flag);
    void                    setFriezeInfoFlag(u32  _flag) {m_showFriezeInfoMask |= _flag;}
    void                    switchFriezeInfoFlag(u32  _flag) {m_showFriezeInfoMask ^= _flag;}
    void                    clearFriezeInfoFlag(u32  _flag) {m_showFriezeInfoMask &= (~_flag);}
    virtual void            pushInfoFlags(bbool _clear) {m_savedFriezeInfoMask = m_showFriezeInfoMask; if (_clear) m_showFriezeInfoMask = 0;}
    virtual void            popInfoFlags() {m_showFriezeInfoMask = m_savedFriezeInfoMask;}
    virtual void            switchInfoFlags() {u32 f = m_showFriezeInfoMask; m_showFriezeInfoMask = m_savedFriezeInfoMask; m_savedFriezeInfoMask = f;}
    static FriezeEditor*    getInstance() {ITF_ASSERT_CRASH(m_instance, "no editor"); return m_instance;}
    ShowInfoMode            getShowInfoMode() const {return  m_showInfoMode;}
    void                    setShowInfoMode(ShowInfoMode _mode);
    void                    addShowInfoMode(ShowInfoMode _mode);

	ITF_INLINE const ObjectRef&		getCurFriezeRef() const {return m_curFriezeRef;}
    static void             mirror(Frise* _target, bbool _x, bbool _y);
    static Vec3d            transformFriezePoint(const Frise *_frise, const Vec3d &_p);
    static Vec3d            transformPointToLocalFriezePoint(const Frise *_frise, const Vec3d &_p);
    void                    cutFriezeAtIndex( Frise* pBase, Vec2d _pos, i32 _index );
    static bbool            isMultiCollisionEdition(const Frise* _pFrieze);
    void                    startMultiCol();

private:
	ITF_INLINE void					setCurFriezeRef(const ObjectRef& _or) {m_curFriezeRef = _or;}

    enum state
    {
        Idle,
        AddPoints,
        CutFrieze,
        Create,
        ENUM_FORCE_SIZE_32(2)
    };

    enum HoldState
    {
        HoldState_Nothing = 0,
        HoldState_AddPtsFromMenu,
        HoldState_DelPtsFromMenu,
        HoldState_HolesFromMenu,
        HoldState_SwitchTextureFromMenu,
        HoldState_SetCustomPivot,
        ENUM_FORCE_SIZE_32(1)
    };


    static bbool            m_multiCollisionEdition, m_showAllCollidableFriezes;
    state                   m_state;
    ObjectRef               m_curFriezeRef;
    PickingInfo2D           m_currentFriezePickingInfo;
    bbool                   buildFriezeNextClick;
    bbool                   m_currentSelectionIsLocked;
    void                    closeCurrentFrieze(bbool _keepEditorStuff);
    void                    addPosToFrieze();
    void                    cutFrieze();
    static PickingShape_Disc* getDiscAtIndex(Frise * _pFrise, u32 _index);
    void                    pickColor(const Frise* _form);
    void                    pasteColor();
    void                    pickFogColor(const Frise* _form);
    void                    pasteFogColor();
    String8                 m_buildFriezeNextClickDefaultConfig;
    String                  m_lastSaveFriezePath;
    u32                     m_creationRequestFrame, m_cutRequestFrame;
    f32                     m_cutIconTimer;

    HoldState               m_holdState;
    PolyLineEdge::HoleMode  m_holeMode;

    bbool                   isHoldState(HoldState _state);
    void                    setHoldState(HoldState _state);
    void                    setHoleMode(ObjectRef _target, PolyLineEdge::HoleMode _holeMode);
    void                    setSwitchTextureMode(ObjectRef _target);

    u32                     m_showFriezeInfoMask;
    u32                     m_savedFriezeInfoMask;
    static FriezeEditor*  m_instance;
    ShowInfoMode            m_showInfoMode;
    bbool                   insertFriezeFRZ( const String& _absolutePath );
    ObjectRef               m_userPivotStateTarget;

	typedef struct RotationScaleInProgress
	{
		Vec2d		m_center;
		ObjectRef	m_target;
	} RotationScaleInProgress;
	SafeArray<RotationScaleInProgress>	m_rotationScaleInProgress;

	RotationScaleInProgress*	getRotationScaleData(const Frise* _pFrise);
	RotationScaleInProgress*	addRotationScaleData(const Frise* _pFrise);
	void						removeRotationScaleData(const Frise* _pFrise);

    ObjectRefList               m_addedActiveFriezesLastFrame; // used to create picking shapes the next frame at their creation when modified by engine process
#ifdef DAN_TEXTURE_SELECTOR
    void RecookWithSampler(Frise* target, String& sampler);;
    Vector<String> GetTexturePathFromFrise(Frise* target);
#endif
};

} // namespace ITF


#endif // _ITF_PLUGIN_FRIEZEEDITOR_H_
