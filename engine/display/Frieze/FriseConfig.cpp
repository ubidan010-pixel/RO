#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

namespace ITF
{
    BEGIN_SERIALIZATION(FluidFriseLayer)
        SERIALIZE_MEMBER("threshold", m_threshold);
        SERIALIZE_MEMBER("color", m_color);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterialPath);
        SERIALIZE_MEMBER("backsideGameMaterial", m_backsideGameMaterialPath);
        m_gameMaterialIds[NormalMaterial] = m_gameMaterialPath.getStringID();
        m_gameMaterialIds[BacksideMaterial] = m_backsideGameMaterialPath.getStringID();
        SERIALIZE_MEMBER("collisionHeight", m_collisionHeight);
        SERIALIZE_MEMBER("hasBacksideInversion", m_hasBacksideInversion);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(FriseConfig)

        const char* FriseConfig::defaultFriseConfig  = "misc/defaultdata/defaultfriseconfig.fcg"; // MUST be lowercase with no backslash

    u32 FriseConfig::s_groupCount = 0;


    FriseConfig::FriseConfig(const String8& _path) 
        : Resource(_path)
        , m_renderRank(-2.f)
        , m_renderRankForced(bfalse)
        , m_posZ(0.f)
        , m_posZForced(bfalse)
        , m_isCollisionFlip(bfalse)
        , m_collisionDouble(0)
        , m_useFriezeScale(bfalse)    
        , m_cornerFactor(1.5f)
        , m_height(2.f)
        , m_width(2.f)
        , m_methode(1)
        , m_snapCoeff(0.1f)
        , m_snapCoeffUv(0.1f)
        , m_smoothVisual(btrue)
        , m_smoothFilling(bfalse)
        , m_smoothCollision(bfalse)
        , m_smoothFactorVisual(4.f)
        , m_smoothFactorFilling(2.f)
        , m_smoothFactorCollision(2.f) 
        , m_fillOffSet(0.5f)
        , m_fillScale(Vec2d(2.f,2.f))        
        , m_visualOffset(0.5f)
        , m_fluidElasticity(0.98f)
        , m_fluidVelocity(0.04f)
        , m_fluidViscosity(0.05f)
        , m_fluidAbsorptionAtEdgeStart(0)
        , m_fluidAbsorptionAtEdgeEnd(0)
        , m_fluidAbsorptionAtEdge_Length(0)
        , m_fluidTargetHeight(0.5f)
        , m_fluidAmplification(0.01f)
        , m_fluidTargetMaxHeight(1.f)
        , m_fluidTargetMinHeight(0.f)
        , m_fluidTargetAddHeight(-0.005f)
        , m_fluidUnityWidth(0.1f)
        , m_fluidPolylineUnityMult(1)
        , m_fluidEnterMult(10.f)
        , m_fluidLeaveMult(10.f)
        , m_fluidMaxDstInfluence(1.f)
        , m_fluidLevelsFront(0)
        , m_fluidLevelsBack(0)
        , m_fluidPerpendicularBack(bfalse)
        , m_fluidPerpendicularBackZ(0.f)
        , m_fluidPerpendicularBackScale(1.f)
        , m_fluidPerpendicularBackPos(1.f)
        , m_fluidPerpendicularBackPosZ(0.f)
        , m_fluidPerpendicularFront(bfalse)
        , m_fluidPerpendicularFrontZ(0.f)
        , m_fluidPerpendicularFrontScale(1.f)
        , m_fluidPerpendicularFrontPos(1.f)
        , m_fluidPerpendicularFrontPosZ(0.f)
        , m_fluidInfluenceLimit(FLT_MAX)
        , m_idTexSwitch(-1)
        , m_zVtxExtrudeUp(0.f)
        , m_zVtxExtrudeDown(0.f)
        , m_flexibility(0.75f)
        , m_wallAngle(MTH_PIBY4)
        , m_fluidWeightMultiplier(0.03f)
        , m_dstInfluenceMultiplier(1.f)
        , m_selfIllumColor(COLOR_ZERO)
        , m_objectFamily(0)
        , m_fluidLayerCount(1)
        , m_fluidSideCount(1)
        , m_isUvFlipX(bfalse)
        , m_isUvFlipY(bfalse)
        , m_isRatioFixed(btrue)
        , m_fluidLevelDelta(Vec3d::Zero)
        , m_fluidUVDelta(Vec2d::Zero)
        , m_updateType(Pickable::UpdateType_OnlyIfVisible)

        , m_scale(1.f)    
        , m_density(1.f)
        , m_collisionDistMaxToSkipParallelEdge(0.075f)
    {
        m_path = _path;
        m_pathID = _path;

        m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
        m_lastUpdateFrame = 0;

        m_fillAngle.SetRadians(0.f);
        m_gameMaterial = GameMaterial_Template::nullMatName;   
        m_gameMaterialStartExtremity = GameMaterial_Template::nullMatName;
        m_gameMaterialStopExtremity = GameMaterial_Template::nullMatName;
        m_bInitializedAfterLoading = bfalse;

    }

    FriseConfig::~FriseConfig()
    {
        unload();

        if (m_resourceGroup.isValidResourceId())
            RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);

        Frise::RemoveFromUpdateAnimList(getID());
    }

    FriseTextureConfig::FriseTextureConfig()
    {
        mp_animMeshScene = NULL;
        m_animTrackExt = NULL;
        m_gameMaterial = GameMaterial_Template::nullMatName; 
        m_configType = FriseConfig_Texture;
        m_animAABB.setMax(Vec2d::Zero);
        m_animAABB.setMin(Vec2d::Zero);
        m_animPhase = 0;
        m_color = COLOR_RED | COLOR_GREEN;

        m_alphaBorder = 0;
        m_alphaUp = 0;

        m_AnimTranslationX = m_AnimTranslationY = m_AnimScaleX = m_AnimScaleY = m_AnimRot = 0.f;
        m_AnimIndex = m_AnimTexSizeX = m_AnimTexSizeY = 0;
        m_AnimBackGroundColor = COLOR_ZERO;
        m_animPhase = 0.f;
        m_fillOffset = 0.5f;

        // scrolling
        m_scrollingSpeedTrans = Vec2d::Zero;
        m_scrollingSpeedRotate = 0.f;
    }

    void FriseConfig::load()
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        getResourceGroup()->addPhysicalUserFromMainThread();
    }

    void FriseConfig::unload()
    {
        u32 texConfCount = m_textureConfigs.size();
        for (u32 texIt = 0; texIt < texConfCount; texIt++)
        {
            FriseTextureConfig& rconf = m_textureConfigs[texIt];    
            SF_DEL(rconf.mp_animMeshScene);
            SF_DEL(rconf.m_animTrackExt);
        }

        RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_resourceGroup);
    }

    void FriseConfig::flushPhysicalData()
    {
        unload();
    }

    bbool FriseConfig::tryLoadFile ()
    {    
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
        ITF_ASSERT_MSG(bfalse, "should never be called for non physical resources. Call load() instead");
        load();
        return btrue;
    }

    void    FriseTextureConfig::setPath(const String& _path) 
    {
        m_path = FilePath::normalizePath(_path);
    }

    void    FriseTextureConfig::setNormal(const String& _path) 
    {
        m_normal = FilePath::normalizePath(_path);
    }

    void FriseTextureConfig::setAnim(const String& _path) 
    {
        m_anim = FilePath::normalizePath(_path);
        if (_path.getLen())
        {
            m_configType = FriseTextureConfig::FriseConfig_Animation;
        }
        setPath(_path);
    }

    void FriseTextureConfig::startSerialization_load()
    {
        m_configType = FriseConfig_Texture;
        SF_DEL(mp_animMeshScene);
        m_gameMaterial = GameMaterial_Template::nullMatName;
    }

    void FriseTextureConfig::endSerialization_load()
    {
    }

    void FriseTextureConfig::onLoaded(FriseConfig& _owner)
    {
        ResourceGroup* grp = _owner.getResourceGroup();

        // load resources
        if (!getPath().isEmpty() && FriseTextureConfig::FriseConfig_Texture == m_configType)
            m_textureData = grp->addResource(Resource::ResourceType_Texture, getPath());

        if (!getNormalPath().isEmpty())
            m_textureNormal = grp->addResource(Resource::ResourceType_Texture, getNormalPath());

        if (!getAnimPath().isEmpty() && FriseTextureConfig::FriseConfig_Animation == m_configType)
            m_textureData = grp->addResource(Resource::ResourceType_AnimTrack, getAnimPath());

        if (m_gameMaterial != GameMaterial_Template::nullMatName) 
            World::addGameMaterialFromFile(m_gameMaterial, grp);
    }

    void FriseConfig::startSerialization_load()
    {
        ResourceGroup* grp = getResourceGroup();

        u32 texConfCount = m_textureConfigs.size();
        for (u32 texIt = 0; texIt < texConfCount; texIt++)
        {
            FriseTextureConfig& rconf = m_textureConfigs[texIt];

            if (rconf.m_textureData.isValidResourceId())
                grp->removeResource(rconf.m_textureData);

            if (rconf.m_textureNormal.isValidResourceId())
                grp->removeResource(rconf.m_textureNormal);
        }        

        m_textureConfigs.clear();

        m_FriseTextureConfigIndexBySlope.clear();
        for (u32 preset=0; preset<17; preset++)
            m_FriseTextureConfigIndexBySlope.push_back(-1);

        m_FriseTextureConfigIndexByZone.clear();
        for (u32 preset=0; preset<4; preset++)
            m_FriseTextureConfigIndexByZone.push_back(-1);
    }

    void FriseConfig::endSerialization_load(bbool _cooking)
    {
        m_bInitializedAfterLoading = bfalse;
    }

    void FriseConfig::onLoaded()
    {
        ITF_ASSERT(!m_bInitializedAfterLoading);
        m_bInitializedAfterLoading = btrue;

        ResourceGroup* grp = getResourceGroup();

        //if(!_cooking)
        {
            u32 count = m_FriseTextureConfigIndexBySlope.size();
            for (u32 i = 0; i < count; i++)
            {
                i32 index = m_FriseTextureConfigIndexBySlope[i];
                if ( index >= 0)
                {
                    switch(m_textureConfigs[index].m_configType)
                    {
                    case FriseTextureConfig::FriseConfig_Animation:
                        AnimDependenciesMapResource::fillGroupDependencies( grp, m_textureConfigs[index].getPath(),  AnimationDependenciesMap::getDependenciesFile(m_textureConfigs[index].getPath(), FILESERVER->getPlatform()), NULL );
                        m_textureConfigs[index].m_textureData = grp->addResource(Resource::ResourceType_AnimTrack, m_textureConfigs[index].getPath());
                        break;
                    default:
                        m_textureConfigs[index].m_textureData = grp->addResource(Resource::ResourceType_Texture, m_textureConfigs[index].getPath());
                        break;
                    }
                }
            }
        }

        if ( m_gameMaterial != GameMaterial_Template::nullMatName )
            World::addGameMaterialFromFile(m_gameMaterial, grp);

        if ( m_gameMaterialStartExtremity != GameMaterial_Template::nullMatName )
            World::addGameMaterialFromFile(m_gameMaterialStartExtremity, grp);

        if ( m_gameMaterialStopExtremity != GameMaterial_Template::nullMatName )
            World::addGameMaterialFromFile(m_gameMaterialStopExtremity, grp);

        for (u32 i = 0; i < m_textureConfigs.size(); i++)
            m_textureConfigs[i].onLoaded(*this);

        for (u32 i = 0; i < m_fluidLayers.size(); i++)
        {
            const FluidFriseLayer &layer = m_fluidLayers[i];
            if (layer.m_gameMaterialPath != GameMaterial_Template::nullMatName)
            {
                World::addGameMaterialFromFile(layer.m_gameMaterialPath, grp);
            }
            if (layer.m_backsideGameMaterialPath != GameMaterial_Template::nullMatName)
            {
                World::addGameMaterialFromFile(layer.m_backsideGameMaterialPath, grp);
            }
        }
    }

    void FriseTextureConfig::serialize(ArchiveMemory& _Archive)
    {
        if (_Archive.isReading())
            startSerialization_load();

        /// Paths.
        m_path.serialize(_Archive);    
        m_normal.serialize(_Archive);
        m_anim.serialize(_Archive);

        /// Anim.
        _Archive.serialize(m_AnimTranslationX);
        _Archive.serialize(m_AnimTranslationY);
        _Archive.serialize(m_AnimScaleX);
        _Archive.serialize(m_AnimScaleY);
        _Archive.serialize(m_AnimRot);
        _Archive.serialize(m_AnimIndex);
        _Archive.serialize(m_AnimTexSizeX);
        _Archive.serialize(m_AnimTexSizeY);
        m_AnimBackGroundColor.serialize(_Archive);
        m_animAABB.serialize(_Archive);
        _Archive.serialize(m_animPhase);

        /// TextureConfig -> global.
        m_gameMaterial.serialize(_Archive);
        m_friendly.serialize(_Archive);
        m_color.serialize(_Archive);

        // Collision
        m_collisionSimple.serialize(_Archive);    
        m_collisionDouble.serialize(_Archive);  

        /// TextureConfig -> scrolling texture
        m_scrollingSpeedTrans.serialize(_Archive);
        _Archive.serialize(m_scrollingSpeedRotate);

        _Archive.serialize(m_alphaBorder);
        _Archive.serialize(m_alphaUp);

        // Material
        _Archive.serialize((u32&)(m_gfxMat.m_blendMode));
        _Archive.serialize((u32&)(m_gfxMat.m_matType));
        _Archive.serialize(m_gfxMat.m_matParams.getfParams()[0]);
        _Archive.serialize(m_gfxMat.m_matParams.getfParams()[1]);
        _Archive.serialize(m_gfxMat.m_matParams.getfParams()[2]);
        _Archive.serialize(m_gfxMat.m_matParams.getfParams()[3]);
        _Archive.serialize(m_gfxMat.m_matParams.getiParams()[0]);
        _Archive.serialize(m_gfxMat.m_matParams.getiParams()[1]);
        _Archive.serialize(m_gfxMat.m_matParams.getiParams()[2]);

        _Archive.serialize((u32&)(m_configType));

        // Filling
        _Archive.serialize(m_fillOffset);
    }

    void serializeValue(FriseTextureConfig& config, ArchiveMemory& _Archive)
    {
        config.serialize(_Archive);
    }

    void FluidFriseLayer::serialize(ArchiveMemory& _archive)
    {
        _archive.serialize(m_threshold);
        m_color.serialize(_archive);
        m_gameMaterialPath.serialize(_archive);
        m_backsideGameMaterialPath.serialize(_archive);
        m_gameMaterialIds[NormalMaterial] = m_gameMaterialPath.getStringID();
        m_gameMaterialIds[BacksideMaterial] = m_backsideGameMaterialPath.getStringID();
        _archive.serialize(m_collisionHeight);
        _archive.serialize(m_erosion);
        _archive.serialize(m_hasBacksideInversion);
        _archive.serialize(m_forcedHeightWhenNotColored);
        _archive.serialize(m_hasForcedHeightWhenNotColored);
    }

    void FriseConfig::serialize(ArchiveMemory& _Archive)
    {
        u32 ver = Versioning::friezeConfigVer();
        _Archive.serialize(ver);

        if (_Archive.isReading())
        {
            ITF_ASSERT(ver ==  Versioning::friezeConfigVer());
            startSerialization_load();
        }

        m_FriseTextureConfigIndexBySlope.serialize(_Archive);
        m_FriseTextureConfigIndexByZone.serialize(_Archive);

        m_textureConfigs.serialize(_Archive);

        _Archive.serialize(m_fillFriseTextureConfigIndex);
        m_gameMaterial.serialize(_Archive);
        m_gameMaterialStartExtremity.serialize(_Archive);
        m_gameMaterialStopExtremity.serialize(_Archive);
        m_regionId.serialize(_Archive);  
        _Archive.serialize(m_isCollisionFlip);
        _Archive.serialize(m_collisionDistMaxToSkipParallelEdge);
        _Archive.serialize(m_collisionDouble);

        m_regionId.serialize(_Archive);

        m_path.serialize(_Archive);
        m_pathID.serialize(_Archive);

        _Archive.serialize(m_useFriezeScale);   
        _Archive.serialize(m_renderRank);
        _Archive.serialize(m_renderRankForced);
        _Archive.serialize(m_visualOffset);
        _Archive.serialize(m_posZ);
        _Archive.serialize(m_posZForced);
        _Archive.serialize(m_flexibility);
        _Archive.serialize(m_wallAngle);
        _Archive.serialize(m_cornerFactor);
        _Archive.serialize(m_height);
        _Archive.serialize(m_width);
        _Archive.serialize(m_methode);   
        _Archive.serialize(m_snapCoeff);
        _Archive.serialize(m_snapCoeffUv);
        _Archive.serialize(m_density);
        _Archive.serialize(m_scale);
        // smooth
        _Archive.serialize(m_smoothVisual);
        _Archive.serialize(m_smoothFilling);
        _Archive.serialize(m_smoothCollision);
        _Archive.serialize(m_smoothFactorVisual);
        _Archive.serialize(m_smoothFactorFilling);
        _Archive.serialize(m_smoothFactorCollision);
        _Archive.serialize(m_objectFamily);

        //  collision
        m_collisionFrieze.serialize(_Archive);              

        // fill
        _Archive.serialize(m_fillOffSet);
        m_fillAngle.serialize(_Archive);
        m_fillScale.serialize(_Archive);

        //  vertex anim
        m_vtxAnim.serialize(_Archive);

        // selfillum
        m_selfIllumColor.serialize(_Archive);
        
        // fluid
        _Archive.serialize(m_fluidElasticity);
        _Archive.serialize(m_fluidVelocity);
        _Archive.serialize(m_fluidViscosity);
        _Archive.serialize(m_fluidAbsorptionAtEdgeStart);
        _Archive.serialize(m_fluidAbsorptionAtEdgeEnd);
        _Archive.serialize(m_fluidAbsorptionAtEdge_Length);
        _Archive.serialize(m_fluidTargetHeight);
        _Archive.serialize(m_fluidAmplification);
        _Archive.serialize(m_fluidTargetMaxHeight);
        _Archive.serialize(m_fluidTargetMinHeight);
        _Archive.serialize(m_fluidTargetAddHeight);
        _Archive.serialize(m_fluidUnityWidth);
        _Archive.serialize(m_fluidPolylineUnityMult);
        _Archive.serialize(m_fluidEnterMult);
        _Archive.serialize(m_fluidLeaveMult);
        _Archive.serialize(m_fluidMaxDstInfluence);
        _Archive.serialize(m_fluidLevelsFront);
        _Archive.serialize(m_fluidLevelsBack);
        m_fluidLevelDelta.serialize(_Archive);
        m_fluidUVDelta.serialize(_Archive);
        _Archive.serialize(m_fluidPerpendicularBack);
        _Archive.serialize(m_fluidPerpendicularBackZ);
        _Archive.serialize(m_fluidPerpendicularBackScale);
        _Archive.serialize(m_fluidPerpendicularBackPos);
        _Archive.serialize(m_fluidPerpendicularBackPosZ);
        _Archive.serialize(m_fluidPerpendicularFront);
        _Archive.serialize(m_fluidPerpendicularFrontZ);
        _Archive.serialize(m_fluidPerpendicularFrontScale);
        _Archive.serialize(m_fluidPerpendicularFrontPos);
        _Archive.serialize(m_fluidPerpendicularFrontPosZ);
        _Archive.serialize(m_fluidWeightMultiplier);
        _Archive.serialize(m_dstInfluenceMultiplier);
        _Archive.serialize(m_fluidInfluenceLimit);
        _Archive.serialize(m_fluidLayerCount);
        m_fluidLayers.serialize(_Archive);
        ITF_ASSERT_CRASH(m_fluidLayers.size()<=MAX_FLUID_LAYER_COUNT, "Too many layers!");
        _Archive.serialize(m_fluidSideCount);

        // tex switch
        _Archive.serialize(m_idTexSwitch);

        _Archive.serialize(m_zVtxExtrudeUp);
        _Archive.serialize(m_zVtxExtrudeDown); 
        _Archive.serialize(m_isUvFlipX); 
        _Archive.serialize(m_isUvFlipY); 
        _Archive.serialize(m_isRatioFixed); 

        m_plugin.serialize(_Archive);
        m_pluginSettings.serialize(_Archive);

        i32 tempType = m_updateType;
        _Archive.serialize(tempType);
        m_updateType = (Pickable::UpdateType)tempType;

        if (_Archive.isReading())
        {
            endSerialization_load(bfalse);

#ifdef ITF_WII
            m_fluidUnityWidth *= 2.f;
            m_fluidVelocity   *= 0.5f;
#endif // ITF_WII
        }
    }

    ResourceGroup* FriseConfig::getResourceGroup()
    { 
        return (ResourceGroup*)(m_resourceGroup.getResource());
    }

    void    FriseConfig::setStatic(bbool _static)
    {
        Super::setStatic(_static);
        if (ResourceGroup* grp = getResourceGroup())
            grp->setStatic(_static);
    }


} // namespace ITF
