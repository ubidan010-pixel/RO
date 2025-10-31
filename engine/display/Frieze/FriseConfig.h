#ifndef _ITF_FRISECONFIG_H_
#define _ITF_FRISECONFIG_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#include "core/ColorInteger.h"

#define MAX_FLUID_LAYER_COUNT 3


namespace ITF
{

class ResourceGroup;
class AnimTrackExt;
class AnimDraw;
class ArchiveMemory;
class AnimMeshScene;



struct CollisionFrieze 
{
    CollisionFrieze()
        : m_offset(0.f)
        , m_build(bfalse)
        , m_cornerInCur(bfalse)
        , m_cornerOutCur(bfalse)
        , m_extremity(Vec2d::Zero)
        , m_extremity2(Vec2d::Zero)
        , m_cornerIn(Vec2d::Zero)
        , m_cornerOut(Vec2d::Zero)
    {
//         for ( u32 i=0; i<3; i++)
//         {
// 
//         }
    };

    f32     m_offset;
    bbool   m_build;
    bbool   m_cornerInCur;
    bbool   m_cornerOutCur;    
    Vec2d   m_extremity;
    Vec2d   m_extremity2;
    Vec2d   m_cornerIn;
    Vec2d   m_cornerOut;

//     SafeArray<Vec2d, 3> m_ext;
//     SafeArray<Vec2d, 3> m_hole;

    void serialize(ArchiveMemory& _Archive)
    {
        _Archive.serialize(m_offset);
        _Archive.serialize(m_build);
        _Archive.serialize(m_cornerInCur);
        _Archive.serialize(m_cornerOutCur);
        m_extremity.serialize(_Archive);
        m_extremity2.serialize(_Archive);
        m_cornerIn.serialize(_Archive);
        m_cornerOut.serialize(_Archive);
//         m_ext.serialize(_Archive);
//         m_hole.serialize(_Archive);
    }
};


struct VertexAnim
{
    VertexAnim()
        : m_globalSpeed(0.f)
        , m_speedX(1.f)
        , m_speedY(1.f)
        , m_syncX(0.f)
        , m_syncY(0.f)
        , m_amplitudeX(1.f)
        , m_amplitudeY(1.f)
        , m_sync(0.f)
        , m_angleUsed(bfalse)
    {
        m_angle.SetRadians(0.f);
        m_globalRotSpeed.SetRadians(0.f);
    }

    f32     m_globalSpeed;
    Angle   m_globalRotSpeed;
    f32     m_speedX;
    f32     m_speedY;
    f32     m_syncX;
    f32     m_syncY;
    f32     m_amplitudeX;
    f32     m_amplitudeY;
    f32     m_sync;
    bbool   m_angleUsed;
    Angle   m_angle;

    void serialize(ArchiveMemory& _Archive)
    {
        _Archive.serialize(m_globalSpeed);
        m_globalRotSpeed.serialize(_Archive);
        _Archive.serialize(m_speedX);
        _Archive.serialize(m_speedY);
        _Archive.serialize(m_syncX);
        _Archive.serialize(m_syncY);
        _Archive.serialize(m_amplitudeX);
        _Archive.serialize(m_amplitudeY);
        _Archive.serialize(m_sync);
        _Archive.serialize(m_angleUsed);
        m_angle.serialize(_Archive);
    }
};

class FriseTextureConfig
{
public:

    void onLoaded(class FriseConfig& _owner);
    void startSerialization_load();
    void endSerialization_load();
    void serialize(ArchiveMemory& _Archive);

    FriseTextureConfig();

    enum ConfigType
    {
        FriseConfig_Texture,
        FriseConfig_Animation,
        ENUM_FORCE_SIZE_32(0)
    };

    ConfigType      m_configType;
    String          m_friendly;
    Path            m_gameMaterial;
    ResourceID      m_textureData;
    ResourceID      m_textureNormal;    
    ColorInteger    m_color;

    // Collision
    CollisionFrieze m_collisionSimple;
    CollisionFrieze m_collisionDouble;

    Vec2d   m_scrollingSpeedTrans;  
    f32     m_scrollingSpeedRotate;
    u8      m_alphaBorder;
    u8      m_alphaUp;    
    f32     m_fillOffset;

    /// Anims.
    AnimMeshScene* mp_animMeshScene;
    ResourceID     m_AnimTextureRef;
    ResourceID     m_AnimationTextureSourceId;

    AnimTrackExt*  m_animTrackExt;
    f32            m_AnimTranslationX, m_AnimTranslationY, m_AnimScaleX, m_AnimScaleY, m_AnimRot;
    i32            m_AnimIndex, m_AnimTexSizeX, m_AnimTexSizeY;
    ColorInteger   m_AnimBackGroundColor;
    AABB           m_animAABB;
    f32            m_animPhase;

    /// GFX Material.
    GFX_MATERIAL   m_gfxMat;

    const String&  getPath() const {return m_path;}
 //   const String&  getOverlayPath() const {return m_overlayPath;}
    const String&  getNormalPath() const {return m_normal;}
    const String&  getAnimPath() const {return m_anim;}
    void    setPath(const String& _path);    
    void    setNormal(const String& _path);
    void    setAnim(const String& _path);

private:
    String  m_path, m_normal, m_anim;

};

///////////////////////////////////////////////////////////////////////////////////////////
class FriseConfigPluginSettings
{
public:
    FriseConfigPluginSettings() {}

    void serialize(ArchiveMemory& _Archive) {m_params.serialize(_Archive);}

    f32 getValue(const StringID &_name, f32 _default) const {ITF_MAP<StringID, f32>::const_iterator iter = m_params.find(_name); if (iter==m_params.end()) return _default; return iter->second;}


    ITF_MAP<StringID, f32> m_params;
};

///////////////////////////////////////////////////////////////////////////////////////////
class FluidFriseLayer
{
    DECLARE_SERIALIZE()
public:
    static const i32 InvalidCollisionHeight = -1000;
    enum {NormalMaterial=0, BacksideMaterial, MaterialCount}; //Indices 0/1 are reserved for normal/backside materials

    FluidFriseLayer() 
        : m_color(1.f,1.f,1.f,1.f)
        , m_threshold(FLT_MAX)
        , m_collisionHeight((f32)InvalidCollisionHeight)
        , m_erosion(0)
        , m_hasBacksideInversion(bfalse)
        , m_collisionHeightMultiplier(1)
        , m_hasForcedHeightWhenNotColored(bfalse)
        , m_forcedHeightWhenNotColored(-FLT_MAX)
    {}

    void serialize(ArchiveMemory& _archive);

    f32 m_threshold; //Layer info is only applied if value is >= this threshold
    Color m_color;
    Path m_gameMaterialPath, m_backsideGameMaterialPath;
    f32 m_collisionHeight;
    f32 m_collisionHeightMultiplier;
    u32 m_erosion;
    bbool m_hasBacksideInversion;
    bbool m_hasForcedHeightWhenNotColored;
    f32 m_forcedHeightWhenNotColored;

    StringID m_gameMaterialIds[MaterialCount];
};

class FriseConfig : public Resource
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(FriseConfig,Resource,4277131661);

public:
                                    ~FriseConfig();
    void                            onLoaded();
    void                            startSerialization_load();
    void                            endSerialization_load(bbool _cooking);
    void                            serialize(ArchiveMemory& _Archive);
    virtual ResourceGroup*          getGroup() {return getResourceGroup();}

    ResourceGroup*                  getResourceGroup();
    SafeArray<i32, 17>              m_FriseTextureConfigIndexBySlope;
    SafeArray<i32, 4>               m_FriseTextureConfigIndexByZone;
    ITF_VECTOR<FriseTextureConfig>  m_textureConfigs;
    StringID                        m_regionId;
    Path                            m_gameMaterial; 
    Path                            m_gameMaterialStartExtremity; 
    Path                            m_gameMaterialStopExtremity; 
    u32                             m_lastUpdateFrame;
    bbool                           m_isCollisionFlip;            
    u32                             m_collisionDouble;    
    bbool                           m_useFriezeScale;
    f32                             m_renderRank;
    bbool                           m_renderRankForced;
    f32                             m_posZ;
    bbool                           m_posZForced;
    f32                             m_visualOffset;
    f32                             m_zVtxExtrudeUp;
    f32                             m_zVtxExtrudeDown;
    f32                             m_flexibility;
    f32                             m_wallAngle;
    bbool                           m_isUvFlipX;
    bbool                           m_isUvFlipY;    
    bbool                           m_isRatioFixed;
    Pickable::UpdateType            m_updateType;
    f32                             m_collisionDistMaxToSkipParallelEdge;

    f32     m_cornerFactor;
    f32     m_height;
    f32     m_width;
    u32     m_methode;   
    f32     m_snapCoeff;
    f32     m_snapCoeffUv;

    // smooth
    bbool        m_smoothVisual;
    bbool        m_smoothFilling;
    bbool        m_smoothCollision;
    f32         m_smoothFactorVisual;
    f32         m_smoothFactorFilling;
    f32         m_smoothFactorCollision;

    //  collision
    CollisionFrieze m_collisionFrieze;    

    // fill
    f32     m_fillOffSet;
    Angle   m_fillAngle;
    Vec2d   m_fillScale;
    i32     m_fillFriseTextureConfigIndex;

    //  vertex anim
    VertexAnim m_vtxAnim;

    ColorInteger    m_selfIllumColor;
    u32                m_objectFamily;

    StringID        m_plugin;

    // fluid
    f32     m_fluidElasticity;
    f32     m_fluidVelocity;
    f32     m_fluidViscosity;
    f32     m_fluidAbsorptionAtEdgeStart, m_fluidAbsorptionAtEdgeEnd;
    f32     m_fluidAbsorptionAtEdge_Length;
    f32     m_fluidTargetHeight; 
    f32     m_fluidAmplification;
    f32     m_fluidTargetMaxHeight;
    f32     m_fluidTargetMinHeight;
    f32     m_fluidTargetAddHeight;
    f32     m_fluidUnityWidth;
    u32     m_fluidPolylineUnityMult;
    f32     m_fluidEnterMult;
    f32     m_fluidLeaveMult;
    f32     m_fluidMaxDstInfluence;
    u32     m_fluidLevelsFront;
    u32     m_fluidLevelsBack;
    Vec3d   m_fluidLevelDelta;
    Vec2d   m_fluidUVDelta;
    u32     m_fluidPerpendicularBack;
    f32     m_fluidPerpendicularBackZ;
    f32     m_fluidPerpendicularBackScale;
    f32     m_fluidPerpendicularBackPos;
    f32     m_fluidPerpendicularBackPosZ;
    u32     m_fluidPerpendicularFront;
    f32     m_fluidPerpendicularFrontZ;
    f32     m_fluidPerpendicularFrontScale;
    f32     m_fluidPerpendicularFrontPos;
    f32     m_fluidPerpendicularFrontPosZ;
    f32     m_fluidWeightMultiplier;
    f32     m_dstInfluenceMultiplier;
    u32     m_fluidLayerCount;
    f32     m_fluidInfluenceLimit;
    u32     m_fluidSideCount;

    ITF_VECTOR<FluidFriseLayer> m_fluidLayers;
    FriseConfigPluginSettings m_pluginSettings;


    // tex Switch
    i32     m_idTexSwitch;

    // overlays
    f32         m_scale;    
    f32         m_density;

    // --- inherited from Resource
    virtual ResourceType    getType             () const {return ResourceType_FriezeConfig;}
    virtual bbool           isPhysicalExist     () { return btrue; }
    virtual void            load                ();
    virtual void            unload              ();
    virtual bbool           delayPhysicalFlush() {return bfalse;}
    virtual void            setStatic           (bbool _static);

    ITF_INLINE const String8& getPath           () const {return m_path;}
    ITF_INLINE const StringID& getPathID        () const {return m_pathID;}

    static const char*      defaultFriseConfig;

    bbool                   m_bInitializedAfterLoading;

private:
    friend class ResourceManager; // to create a FriezeConfig, call RESOURCE_MANAGER->newResourceFromFile(Resource::ResourceType_FriezeConfig, configName);

                            FriseConfig(const String8& _path);
    virtual void            flushPhysicalData  (   );
    virtual bbool           tryLoadFile         (   );
    // --- end of resource inherited block

    String8                 m_path;
    StringID                m_pathID;
    ResourceID              m_resourceGroup;

    static u32              s_groupCount;
};


} // namespace ITF

#endif  // _ITF_FRISECONFIG_H_
