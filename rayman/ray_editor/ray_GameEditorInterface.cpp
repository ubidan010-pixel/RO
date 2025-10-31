#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_RAY_GAME_EDITOR_INTERFACE_H_
#include "rayman/ray_editor/ray_GameEditorInterface.h"
#endif //_ITF_RAY_GAME_EDITOR_INTERFACE_H_

// ALL PLUGINS

#ifndef _ITF_SOUNDCOMPONENT_PLUGIN_H_
#include "rayman/ray_editor/plugins/SoundComponent_Plugin.h"
#endif //_ITF_SOUNDCOMPONENT_PLUGIN_H_

#ifndef _ITF_SCENESETTINGS_PLUGIN_H_
#include "rayman/ray_editor/plugins/SceneSettings_Plugin.h"
#endif //_ITF_SCENESETTINGS_PLUGIN_H_

#ifndef _ITF_BOSSNODE_PLUGIN_H_
#include "rayman/ray_editor/plugins/BossNode_Plugin.h"
#endif //_ITF_BOSSNODE_PLUGIN_H_

#ifndef _ITF_CAMERA_PLUGIN_H_
#include "rayman/ray_editor/plugins/Camera_Plugin.h"
#endif //_ITF_CAMERA_PLUGIN_H_

#ifndef _ITF_LINK_PLUGIN_H_
#include "rayman/ray_editor/plugins/Link_Plugin.h"
#endif //_ITF_LINK_PLUGIN_H_

#ifndef _ITF_PLATFORMTREE_PLUGIN_H_
#include "rayman/ray_editor/plugins/PlatformTree_Plugin.h"
#endif //_ITF_PLATFORMTREE_PLUGIN_H_

#ifndef _ITF_TWEENING_PLUGIN_H_
#include "rayman/ray_editor/plugins/Tweening_Plugin.h"
#endif //_ITF_TWEENING_PLUGIN_H_

#ifndef _ITF_MISC_PLUGIN_H_
#include "rayman/ray_editor/plugins/Misc_Plugin.h"
#endif //_ITF_MISC_PLUGIN_H_

#ifndef _ITF_TRAJECTORYNODE_PLUGIN_H_
#include "rayman/ray_editor/plugins/TrajectoryNode_Plugin.h"
#endif //_ITF_TRAJECTORYNODE_PLUGIN_H_

#ifndef _ITF_NEEDSINITPOSPICKINGSHAPE_PLUGIN_H_
#include "rayman/ray_editor/plugins/NeedsInitPosPickingShape_Plugin.h"
#endif //_ITF_NEEDSINITPOSPICKINGSHAPE_PLUGIN_H_

#ifndef _ITF_CURVE_PLUGIN_H_
#include "rayman/ray_editor/plugins/Curve_Plugin.h"
#endif //_ITF_CURVE_PLUGIN_H_

#ifndef _ITF_BEZIERCURVE_PLUGIN_H_
#include "rayman/ray_editor/plugins/BezierCurve_Plugin.h"
#endif //_ITF_BEZIERCURVE_PLUGIN_H_

#ifndef _ITF_MUSICSCORE_PLUGIN_H_
#include "rayman/ray_editor/plugins/MusicScore_Plugin.h"
#endif //_ITF_MUSICSCORE_PLUGIN_H_

#ifndef _ITF_SNAPONMUSICSCORE_PLUGIN_H_
#include "rayman/ray_editor/plugins/SnapOnMusicScore_Plugin.h"
#endif //_ITF_SNAPONMUSICSCORE_PLUGIN_H_

#ifndef _ITF_GEYSERPLATFORM_PLUGIN_H_
#include "rayman/ray_editor/plugins/GeyserPlatform_Plugin.h"
#endif //_ITF_GEYSERPLATFORM_PLUGIN_H_

#ifndef _ITF_SUBANCHOR_PLUGIN_H_
#include "rayman/ray_editor/plugins/SubAnchor_Plugin.h"
#endif //_ITF_SUBANCHOR_PLUGIN_H_

#ifndef _ITF_UI_PLUGIN_H_
#include "rayman/ray_editor/plugins/UI_Plugin.h"
#endif // _ITF_UI_PLUGIN_H_

#ifndef _ITF_PROCEDURALSOFTPLATFORM_PLUGIN_H_
#include "rayman/ray_editor/plugins/ProceduralSoftPlatform_Plugin.h"
#endif //_ITF_PROCEDURALSOFTPLATFORM_PLUGIN_H_

#ifndef _ITF_RAY_BEZIERTREE_PLUGIN_H_
#include "rayman/ray_editor/plugins/Ray_BezierTree_Plugin.h"
#endif //_ITF_RAY_BEZIERTREE_PLUGIN_H_



namespace ITF
{
    void RayGameEditorInterface::registerPlugins() const
    {
        Misc_Plugin::create();
        Link_Plugin::create();
        Camera_Plugin::create();
        PlatformTree_Plugin::create();
        Tweening_Plugin::create();
        SoundComponent_Plugin::create();
        SceneSettings_Plugin::create();
        BossNode_Plugin::create();
        TrajectoryNode_Plugin::create();
        NeedsInitPosPickingShape_Plugin::create();
        Curve_Plugin::create();
        BezierCurve_Plugin::create();
        MusicScore_Plugin::create();
        SnapOnMusicScore_Plugin::create();
        GeyserPlatform_Plugin::create();
        SubAnchor_Plugin::create();
        UI_Plugin::create();
        ProceduralSoftPlatform_Plugin::create();
        Ray_BezierTree_Plugin::create();
    }
}