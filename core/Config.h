#ifndef _ITF_CONFIG_H_
#define _ITF_CONFIG_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{
    typedef struct CharacterAnim
    {
        String  m_action;
        i32     m_index;
        float   m_frameSpeed;
    } CharacterAnim;

    typedef struct CharacterConfig
    {
        const CharacterAnim* findAction(const String& _action) const;
        u32     m_configIndex;
        String  m_persoType;
        String  m_persoTexture;
        float   m_persoWidth;
        float   m_persoHeight;
        i32     m_persoRank;
        float    m_defaultGravityAmountX;
        float    m_defaultGravityAmountY;
        float    m_defaultGravityScale;
        float    m_defaultTakeOffSpeed;
        float    m_MaxSpd;
        float    XAirControlLimiter;
        float   XAirControlMax;
        float   XAirControlFactor;
        float    m_JumpYImpulse;
        float   m_jumpAddedCoeff;
        float   m_jumpMax;
        float   m_jumpAddedMax;
        float   m_airJumpTimer;
        float   m_InputLimit;
        float   m_collisionRadius;
        Vec2d   m_collisionOffset;
        i32     m_AnimationSceneIndex; // -1 if no data

        float   m_speed_max_inStick;
        float   m_acceleration_inStick;
        String  m_jumpSound;
        ITF_VECTOR<CharacterAnim> m_animsConfig;

    } CharacterConfig;

    typedef struct ActorLoadPath
    {
        String m_path;
        bbool m_recursive;
    } ActorLoadPath; 

    typedef struct ActorAlias
    {
        String m_oldName;
        String m_newName;
    } ActorAlias; 

    struct XMLConfig
    {
        XMLConfig():m_X(0),m_Y(0)
            , m_noWarnings(bfalse)
            , m_waitVBL(btrue)
            , m_disableRendering(bfalse)
#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_XML) || !defined(ITF_CONSOLE_FINAL) || defined(RASTER_ENABLED)
            , m_logTimings(bfalse)
            , m_logTimings_StartFrameIndex(200)
            , m_logTimings_EndFrameIndex(300)
#endif // defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_XML) || !defined(ITF_CONSOLE_FINAL) || defined(RASTER_ENABLED)
#ifndef ITF_FINAL
            , m_showComponentPerf(bfalse)
            , m_showActorPerf(bfalse)
            , m_showActorComponentPerf(bfalse)
            , m_debugPopup(bfalse)
#endif // ITF_FINAL
#ifdef ITF_WINDOWS
            , m_XInputDevicesOnly(btrue)
#endif // ITF_WINDOWS
			, m_fullScreen(0)
			, m_screenWidth(800)
			, m_screenHeight(600)
			, m_zMin(0.0f)
			, m_zMax(100.0f)
            , m_minimalEngineVersion(0)
#ifdef ITF_SUPPORT_EDITOR
			, m_SourceControlProvider("")
#endif // ITF_SUPPORT_EDITOR
#ifdef ITF_WINDOWS
            , m_enableWiiRemoteonPC(bfalse)
#endif // ITF_WINDOWS
            , m_enableTRC(btrue)
            , m_enableRewards(btrue)
            , m_enableBootLogos(btrue)
#ifdef ITF_WII
            , m_enableEpilepsyScreen(btrue)
#endif //ITF_WII
#ifdef ITF_SUPPORT_ONLINETRACKING
			, m_enableonlineTracking(bfalse)
#endif // ITF_SUPPORT_ONLINETRACKING
#ifdef ITF_SUPPORT_NETWORKSERVICES
            , m_enableNetwork(bfalse)
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_SUPPORT_UPLAY
            , m_enableUplay(bfalse)
#endif // ITF_SUPPORT_UPLAY
/*
		_config->m_BGColor[0]       = XML.readFloatAttribute("BGColor/R", 0.1f);
		_config->m_BGColor[1]       = XML.readFloatAttribute("BGColor/G", 0.2f);
		_config->m_BGColor[2]       = XML.readFloatAttribute("BGColor/B", 0.4f);;
		_config->m_urlServer        = XML.readAttribute("Server/url", "127.0.0.1");
		_config->m_PortServer       = XML.readIntAttribute("Server/Port", 3456);
		_config->m_szMapFolder      = XML.readAttribute("DB/folder", "error", PugiXMLWrap::RaiseError);
		_config->m_BGMusic          = XML.readAttribute("BGMusic/music");
		_config->m_editorZoomFactor = XML.readFloatAttribute("Editor/ZoomFactor", 0.2f);
		_config->m_editorZoomFactor = std::max(_config->m_editorZoomFactor, 0.05f);
		_config->m_editorZoomFactor = std::min(_config->m_editorZoomFactor, 0.95f);
		_config->m_editorMoveDepthFactor = XML.readFloatAttribute("Editor/DepthFactor", 0.1f);
		_config->m_editorMoveDepthFactor = std::max(_config->m_editorMoveDepthFactor, 0.01f);
		_config->m_editorMoveDepthFactor = std::min(_config->m_editorMoveDepthFactor, 0.95f);
		_config->m_debugVideoCapture = XML.readIntAttribute("Debug/VideoCapture", 0);
		_config->m_audioVideoCapture = XML.readIntAttribute("Debug/AudioCapture", 0);
		_config->m_XInputDevicesOnly = XML.readIntAttribute("PC/XInputDevicesOnly", 1);*/
            , m_noSafeFrame(bfalse)
            , m_blackClearColorNeeded(bfalse)
        {

            m_BGColor[0] = 0.1f;
            m_BGColor[1] = 0.2f;
            m_BGColor[2] = 0.4f;

            m_pressStartToJoin = btrue;


#ifdef ITF_SUPPORT_CHEAT
            m_allowDebug          = btrue;
            m_startWithCheatsOn   = btrue;
#else
            m_allowDebug          = bfalse;
            m_startWithCheatsOn   = bfalse;
#endif 


// Special E3
#ifdef ITF_DEMO
            m_allowDebug          = bfalse;
            m_startWithCheatsOn   = btrue;
#endif


        }

        u32     m_screenWidth, m_screenHeight, m_fullScreen;
        i32     m_X,m_Y;
        float   m_zMin, m_zMax;
        String  m_BGMusic;
        float   m_BGColor[3];        // background color
        String  m_FriezeConfigLib;
        String  m_urlServer;
        int     m_PortServer;
        bbool   m_showMouseCursor;
        String  m_szDepotPath;
        String  m_szMapFolder;
        f32     m_editorZoomFactor;
        f32     m_editorMoveDepthFactor;
        String  m_projectID;
        String  m_appTitle;
        bbool   m_allowDebug;
        bbool   m_startWithCheatsOn;
        bbool   m_pressStartToJoin;
        u32     m_minimalEngineVersion;
        bbool   m_noWarnings;
        bbool   m_debugVideoCapture;
        bbool   m_audioVideoCapture;
        bbool   m_waitVBL;
        bbool   m_disableRendering;
#ifdef ITF_WINDOWS
        bbool   m_XInputDevicesOnly;
        bbool   m_enableWiiRemoteonPC;
#endif // ITF_WINDOWS
        bbool   m_enableTRC;
        bbool   m_enableRewards;
        bbool   m_enableBootLogos;
        bbool   m_noSafeFrame;
#ifdef ITF_WII
        bbool   m_enableEpilepsyScreen;
#endif //ITF_WII

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_XML) || !defined(ITF_CONSOLE_FINAL) || defined(RASTER_ENABLED)
        u32     m_logTimings_StartFrameIndex;
        u32     m_logTimings_EndFrameIndex;
        bbool   m_logTimings;
#endif // defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_XML) || !defined(ITF_CONSOLE_FINAL) || defined(RASTER_ENABLED)

#ifdef ITF_SUPPORT_ONLINETRACKING
		bool    m_enableonlineTracking;
#endif // ITF_SUPPORT_ONLINETRACKING

        // Online stuff
#ifdef ITF_SUPPORT_NETWORKSERVICES
        bbool   m_enableNetwork;
#endif // ITF_SUPPORT_NETWORKSERVICES

#ifdef ITF_SUPPORT_UPLAY
        bbool   m_enableUplay;
#endif // ITF_SUPPORT_UPLAY
        String8 m_onlineBuildId;
        String8 m_onlineLogin;
        String8 m_onlinePassword;

#ifndef ITF_FINAL
        bbool   m_showComponentPerf;
        bbool   m_showActorPerf;
        bbool   m_showActorComponentPerf;
        float   m_ProfileFPS;
        bbool   m_debugPopup;
#endif // ITF_FINAL
        bbool   m_doFreesounds;
        ITF_VECTOR<ActorLoadPath> m_actorLoadPaths;
        ITF_VECTOR<ActorAlias> m_actorAliases;
#ifdef ITF_SUPPORT_EDITOR
		String  m_SourceControlProvider;
#endif // ITF_SUPPORT_EDITOR
        String  m_HResDirectory;
        bbool   m_blackClearColorNeeded;
    };

} // namespace ITF


#endif // _ITF_CONFIG_H_
