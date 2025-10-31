#ifndef _ITF_EDITORCONTEXTMENU_H_
#define _ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{
    struct ContextMenuItem;

    class EditorContextMenu
    {
    public:
        typedef struct ContextMenuInfo
        {
            ContextMenuInfo()
            {
                m_isExclusive = bfalse;
                m_allowDraw = bfalse;
            }
            EditorContextMenu*    m_menu;
            bbool                 m_isExclusive;
            bbool                 m_allowDraw;
        } ContextMenuInfo;

        enum    ItemId
        {
            ItemId_None = 0,

            // BACK IDs (all back ids must remain contiguous)
            ItemId_Back1,
            ItemId_Back2,
            ItemId_Back3,
            ItemId_Back4,
            ItemId_Back5,
            ItemId_Back6,
            ItemId_Back7,
            ItemId_Back8,

            // GLOBAL
            ItemId_InsertMenu,
            ItemId_NewScene,
            ItemId_LoadScene,
            ItemId_HideSelected,
            ItemId_ShowPickables,
            ItemId_ShowGrid_ObjectDepth,
            ItemId_ShowGrid_SceneDepth,
            ItemId_ChooseGridUnit,
            ItemId_ChooseAngleStep,
            ItemId_2DItems,
            ItemId_UnHideAll,
            ItemId_HideUnSelected,
            ItemId_FullScreen,
            ItemId_GroupSelection,
            ItemId_UngroupSelection,
            ItemId_ExportSelectionToScene,
            ItemId_ExportSelectionToGroup,
            ItemId_FindInExplorer,
            ItemId_SetSinglePiece,
            ItemId_SetMultiplePieces,
            ItemId_SceneExport,
            ItemId_SceneExportAsScene,
            ItemId_SceneExportAsGroup,
            ItemId_LaunchFromExplorer,
            ItemId_ReplaceActor,
            ItemId_Unselect,
            ItemId_PropertiesEditor,
            ItemId_SubEdit,
            ItemId_Edit,
#ifdef DAN_TEXTURE_SELECTOR
            ItemId_SamplerPicker,
#endif
            ItemId_File,
            ItemId_Create,
            ItemId_SceneExplorer,
            ItemId_NotImplemented,
            ItemId_SnapFriezes,
            ItemId_GenericInsert,
            ItemId_SubSceneContentShapesToggleDisplay,
            ItemId_Unpause,
            ItemId_ToggleHighlightOnMouseOver,
            ItemId_SubScenesMenu,
            ItemId_SubScenesMenuScene,
            ItemId_SubSceneLock,
            ItemId_SubSceneUnlock,
            ItemId_SubSceneGoto,
            ItemId_SubSceneOpen,
			ItemId_SetMusicTheme,
            ItemId_MuteMusic,
            ItemId_OpenTsc,


            // TOP BAR
            ItemId_TopBar_Utils,
            ItemId_TopBar_Tools,
            ItemId_TopBar_File,
            ItemId_TopBar_ImportScene,
            ItemId_TopBar_Display,
            ItemId_TopBar_Edit,
            ItemId_TopBar_Options,
            ItemId_TopBar_Save,
            ItemId_TopBar_SaveAs,
            ItemId_TopBar_Load,
            ItemId_TopBar_NewFromTemplate,
            ItemId_TopBar_Play,
            ItemId_TopBar_Pause,
            ItemId_TopBar_Reinit,
            ItemId_TopBar_Reload,
            ItemId_TopBar_Undo,
            ItemId_TopBar_Redo,
            ItemId_TopBar_ClearWarnings,
            ItemId_TopBar_WorldOrigin,
            ItemId_TopBar_CenterOnWorldOrigin,
            ItemId_TopBar_ShowWorldOrigin,
            ItemId_TopBar_HideWorldOrigin,
            ItemId_TopBar_GotoWorldOrigin,
            ItemId_TopBar_ScenesRoot,
            ItemId_TopBar_Scenes,
            ItemId_TopBar_AutoPause,
            ItemId_TopBar_PickAlphaObjects,
            ItemId_TopBar_Katana_Cut,
            ItemId_TopBar_FriezeMultiCol,
            ItemId_TopBar_Select2DActors,

            // PICKABLE
            ItemId_AlignSelection,
            ItemId_AlignSelectionX,
            ItemId_AlignSelectionY,
            ItemId_AlignSelectionZ,
            ItemId_FilterTarget,
            ItemId_UnfilterTarget,

            // ACTOR EDITOR
            ItemId_ActorDone,
            ItemId_InsertTodo,
            ItemId_ActorEditor,
            ItemId_CreateAnimatedActorFromTexture,
            ItemId_AddTextureAsActor,
            ItemId_AddTextureAsFrieze,
            ItemId_Internalize,
            ItemId_Externalize,
            ItemId_CopyObjectPathToClipboard,
            ItemId_ToggleActorIconDisplay,
            ItemId_Genanim,
            ItemId_ClearBinds,
            ItemId_ClearBindToParent,
            ItemId_ClearBindsToChildren,
            ItemId_EnterCreateBind,
            ItemId_SetSubSceneActorPivot,
            ItemId_EnterSubSceneEdition,
            ItemId_SubSceneActorZForcedToggleLock,
            ItemId_SetWorkingSubscene,
            ItemId_UnsetWorkingSubscene,
            ItemId_Actor_CopyFogProperties,
            ItemId_Actor_PasteFogProperties,
            ItemId_SubSceneActor_ToggleGroupExternal,
            ItemId_SubSceneActor_SaveAsTSC,
            ItemId_SequenceEditor,
            ItemId_ReplaceIscAtOriginAndKeepOtherPlacedRelatively,

            // color Computer
            ItemId_colorComputerFogToSelection,
            ItemId_colorComputerColorToSelection,
            ItemId_colorComputerApplyTagIdToSelection,
            ItemId_colorComputerApplyFogToTagId,
            ItemId_colorComputerApplyColorToTagId,

            // FRIEZE EDITOR
            ItemId_CutFrieze,
            ItemId_CutFrieze_CurZ,
            ItemId_CutFrieze_AllZ,
            ItemId_CutFrieze_Selection,
            ItemId_CutFrieze_Vertical,
            ItemId_CutFrieze_Horizontal,
            ItemId_CutFrieze_Free,
            ItemId_CutFrieze_Go,
            ItemId_FriezeHole,
            ItemId_FriezeHole_Collision,
            ItemId_FriezeHole_Visual,
            ItemId_FriezeHole_Both,
            ItemId_Frieze_SwitchTexture,
            ItemId_LoopFrieze,
            ItemId_FriezeDone,
            ItemId_FriezeInvertPoints,
            ItemId_EditFrieze,
            ItemId_Transform,
            ItemId_Transform_XMirror,
            ItemId_Transform_YMirror,
            ItemId_FriezePivot,
            ItemId_CenterFriezePivot,
            ItemId_TopLeftFriezePivot,
            ItemId_TopRightFriezePivot,
            ItemId_BottomLeftFriezePivot,
            ItemId_BottomRightFriezePivot,
            ItemId_UserCustomFriezePivot,
            ItemId_RepairPivot,
            ItemId_FriezeEd_CtrlPts,
            ItemId_FriezeEd_AddPts,
            ItemId_FriezeEd_DeletePts,
            ItemId_FriezeEd_CopyProperties,
            ItemId_FriezeEd_PasteProperties,
            ItemId_FriezeEd_CopyFogProperties,
            ItemId_FriezeEd_PasteFogProperties,
            ItemId_FriezeEd_Copy,
            ItemId_FriezeEd_Paste,
            ItemId_FriezeEd_Delete,
            ItemId_FriezeEd_Merge,
            ItemId_FriezeEd_Export,
            ItemId_FriezeEd_Replace,
            ItemId_FriezeEd_Replace_MP,
            ItemId_FriezeEd_Replace_MC,
            ItemId_FriezeEd_Replace_MT,
            ItemId_FriezeEd_Replace_No,
            ItemId_FriezeEd_ReplaceConfig,
            ItemId_FriezeEd_ReplaceKit,
            ItemId_AlignFriezePoints,
            ItemId_AlignFriezePointsX,
            ItemId_AlignFriezePointsY,
            ItemId_FriezeMeshToggleDisplay,
            ItemId_FriezePivotToggleDisplay,
            ItemId_FriezePointsToggleDisplay,
            ItemId_FriezeCollisionsToggleDisplay,
            ItemId_FriezeToggleDynamic,
            ItemId_FriezeSetAtFisrtPoint,

            // SEQUENCE EDITOR
            ItemId_SequenceSetPivot,

            // TRAJECTORY EDITOR
            ItemId_TrajInterpolation,
            ItemId_TrajInterConstant,
            ItemId_TrajInterLinear,
            ItemId_TrajInterSpline,
            ItemId_TrajInterBezier,
            ItemId_TrajInterBezierStandard,

            // DEBUG
            ItemId_DBG_MenuCore,
            ItemId_DBG_MenuLD,
            ItemId_DBG_MenuGameplay,
            ItemId_DBG_MenuSound,
            ItemId_DBG_MenuStats,

            ItemId_DBG_Stats,
            ItemId_DBG_Rasters,
            ItemId_DBG_UndoRedo,
            ItemId_DBG_DumpStats,
            ItemId_DBG_ReferenceIsFrame,
            ItemId_DBG_ReferenceIs5ms,
            ItemId_DBG_Sound,
            ItemId_DBG_DrawCells,
            ItemId_DBG_Camera,
            ItemId_DBG_Camera_Show,
            ItemId_DBG_CameraModifiers_Show,
            ItemId_DBG_Camera_Screen4By3,
            ItemId_DBG_Camera_HideScreen4By3,
            ItemId_DBG_Camera_ShowScreen4By3_Transparent,
            ItemId_DBG_Camera_ShowScreen4By3_Opaque,
            ItemId_DBG_Camera_Shake,
            ItemId_DBG_Physics,
            ItemId_DBG_CameraRun,
            ItemId_DBG_CameraRunStart,
            ItemId_DBG_CameraRunStop,
            ItemId_DBG_Assert,
            ItemId_DBG_Views,
            ItemId_DBG_Views_SwitchViewsLock,
            ItemId_DBG_Views_EMulatePCView,
            ItemId_DBG_Views_EMulateXBOXView,
            ItemId_DBG_Views_EMulatePS3View,
            ItemId_DBG_Views_EMulate3DSView,
            ItemId_DBG_Views_EMulateVitaView,


			// ONLINETRACKING//

			ItemId_OT_LaunchTrackingTool,
			ItemId_OT_ClearTrackingData,
			ItemId_OT_SendLevelName,

            ItemId_ShowRewards,
#ifdef DAN_TEXTURE_SELECTOR
            //EDIT_MORE FRISE
            ItemId_ReCook_RSampler,
            ItemId_ReCook_Sampler,
            ItemId_ReCook_USampler,
            ItemId_ReCook_ESRGAN,
            ItemID_FindFriseLocation,
            ItemID_ReCook_Reset,
#endif
            ItemId_DANSceneExplorer,
            ENUM_FORCE_SIZE_32(0)
        };

        EditorContextMenu();
        ~EditorContextMenu();

        void                clearItems();
        ContextMenuItem*    addItem(ItemId _Id, const String& _label, bbool _isSeparator = bfalse);
        ContextMenuItem*    addSubItem(ItemId _Id, const String& _what, ItemId _whereId, const String& _where, bbool _isSeparator = bfalse);
        ContextMenuItem*    addItemFromExternalPlugin(u32 _Id, const String& _label, bbool _isSeparator = bfalse);
        ContextMenuItem*    addSubItemFromExternalPlugin(u32 _Id, const String& _what, u32 _whereId, const String& _where, bbool _isSeparator = bfalse);
        ContextMenuItem*    getItem(const String& _label);
        ContextMenuItem*    getItem(ItemId _Id);
        void                draw(Vec2d _mousePos, EditorContextMenu* _parent = NULL, Vec2d* _forcePos = NULL);
        void                fitScreen(EditorContextMenu* _parent );
        ContextMenuItem*    getItemAtPos(const Vec2d& _pos, EditorContextMenu** _foundIn = NULL);
        void                setPos(const Vec2d& _pos);
        const Vec2d&        getPos()  const {return m_pos;}
        void                getAABB(AABB& _aabb);
        f32                 getLineHeight();
        f32                 getLineInterstice();
        f32                 getSubMenuInterstice();
        ContextMenuItem*    onLeftMB(Vec2d _mousePos);
        void                fold();

        ObjectRef           m_lastTarget;
        Vec2d               m_pos, m_launchPos;
        f32                 getWidth();
        u32                 getItemsCount() const {return m_items.size();}
        AABB                m_lastAABB;
        ContextMenuItem*    getItemAt(u32 _index) {if (_index>=m_items.size()) return NULL; return m_items[_index];}
        bbool               isFolded() const;
        static void         onOpenMenu(EditorContextMenu* _menu);
        static void         setMenuExclusive(EditorContextMenu* _menu, bbool _exclusive);
        static ContextMenuInfo* getMenuInfo(EditorContextMenu* _menu);
        static void deleteMenuInfo(EditorContextMenu* _menu);
        static void keepOnlyOneMenuOpen(EditorContextMenu* _menu);
        static void registerMenu(EditorContextMenu* _menu, bbool _isExclusive);

    private:
        ITF_VECTOR<ContextMenuItem*>   m_items;
        i32                m_maxChar;

        static SafeArray<ContextMenuInfo> m_menus;
    };

    typedef struct ContextMenuItem
    {
        ContextMenuItem() : m_userData(U64_INVALID), m_bShowSubMenu(bfalse), m_isSeparator(bfalse), m_filledFromExternal(bfalse)
        {
        }

        void        setTextColor(f32 _r, f32 _g, f32 _b) {m_r = _r; m_g = _g; m_b = _b;}
        void        setTextBackColor(f32 _r, f32 _g, f32 _b) {m_backR = _r; m_backG = _g; m_backB = _b;}
        void        setInactiveColor() {setTextColor(0.4f,0.4f,0.4f); setTextBackColor(0.1f,0.1f,0.1f);}

        String      m_label;
        Vec2d       m_pos;
        Vec2d       m_size;
        f32         m_r, m_g, m_b;
        f32         m_backR, m_backG, m_backB;
        bbool       m_bShowSubMenu;
        u64         m_userData;
        bbool       m_isSeparator;

        EditorContextMenu m_subMenu;
        void        setId(EditorContextMenu::ItemId _id) {m_Id = (u32)_id;}
        EditorContextMenu::ItemId getId() const  {ITF_ASSERT(!isExternal()); return (EditorContextMenu::ItemId)m_Id;}
        u32         getIdFromExternal() {ITF_ASSERT(isExternal()); return m_Id;}
        void        setExternal() {m_filledFromExternal = btrue;}
        bbool       isExternal() const {return m_filledFromExternal;}

    private:
        u32         m_Id;
        bbool       m_filledFromExternal;

    } ContextMenuItem;

} // namespace ITF


#endif // _ITF_EDITORCONTEXTMENU_H_
