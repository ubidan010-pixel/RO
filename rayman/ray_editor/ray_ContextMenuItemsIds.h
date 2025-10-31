#ifndef _ITF_RAY_CONTEXTMENU_H_
#define _ITF_RAY_CONTEXTMENU_H_


namespace ITF
{
    enum RayGameEditorItemIds
    {
        RayItemId_Invalid = 0,
        RayItemId_SubMenu,

        // Misc_Plugin
        Misc_AvoidanceManager,
        Misc_PolylineIndices,
        Misc_RandomizeAnimSyncOffset,

        // SoundComponent_Plugin
        SoundComponent_Item,
        Sound_ShowDebugInfo,
        Sound_ShowMetronome,
        Sound_SetBpm60,
        Sound_SetBpm70,
        Sound_SetBpm80,
        Sound_SetBpm90,
        Sound_SetBpm100,
        Sound_SetBpm110,
        Sound_SetBpm120,
        Sound_SetBpm130,
        Sound_SetBpm140,
        Sound_SetBpm150,
        Sound_SetBpm160,

        // Camera_Plugin
        CameraComponent_Item,
        Camera_ShowDebugInfo,
        Camera_AlignL_L,
        Camera_AlignL_R,
        Camera_AlignR_R,
        Camera_AlignR_L,
        Camera_AlignWidth,
        Camera_AlignT_T,
        Camera_AlignT_B,
        Camera_AlignB_B,
        Camera_AlignB_T,
        Camera_AlignHeight,
        Camera_Stuff1,
        Camera_Stuff2,

        // Link_Plugin
        Link_Menu,
        Link_CycleDrawDebug,
        Link_RemoveObjectPathChild,
        Link_AddObjectPathChild,
        Link_StartCreate,
        Link_ClearChildren,
        Link_ClearDeadLinks,

        // Tweening_Plugin
        Tweening_Menu,
        Tweening_ToggleLock,
        Tweening_ToggleDebugAll,
        Tweening_ToggleDebugActor,
        Tweening_StartSet,
        Tweening_FxTarget,

        //PlatformTree_Plugin
        PlatformTree_SubMenu,
        PlatformTree_Open,
        PlatformTree_Close,

        //BezierTree_Plugin
        BezierTree_SubMenu,
        BezierTree_Open,
        BezierTree_Close,
        BezierTree_TweenMenu_Set,
        BezierTree_TweenMenu_Offset,
        BezierTree_SpawnMenu,

        // BossNode_Plugin
        BossNode_Plant,
        BossNode_Moray,

        // TrajectoryNode_Plugin (OLD)
        TrajectoryNode_Debug,

        // Curve_Plugin
        Curve_SubMenu,
        Curve_AddAfter,
        Curve_AddBefore,
        // BezierCurve_Plugin
        BezierCurve_SubMenu,
        BezierCurve_AddAfter,
        BezierCurve_AddBefore,

        // ...
        RayItemId_Last,
        ENUM_FORCE_SIZE_32(RayGameEditorItemIds)
    };
}

#endif // _ITF_RAY_CONTEXTMENU_H_
