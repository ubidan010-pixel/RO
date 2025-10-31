#include "precompiled_engine.h"

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
#ifdef ITF_SUPPORT_XML

static void checkGameMaterial( Path& _gMat )
{
    if ( _gMat.isEmpty() )
        _gMat = GameMaterial_Template::nullMatName;
}

static void frieze_readVtxAnim( PugiXMLWrap& _XML, VertexAnim& _dest)
{
    if ( _XML.enterElement("VertexAnim") )
    {  
            _dest.m_globalSpeed = _XML.readFloatAttribute("animGlobalSpeed", 0.f );
            _dest.m_globalRotSpeed.SetDegrees( _XML.readFloatAttribute("animGlobalRotSpeed", 0.f ) );
            _dest.m_speedX = _XML.readFloatAttribute("animSpeedX", 1.f );
            _dest.m_speedY = _XML.readFloatAttribute("animSpeedY", 1.f );
            _dest.m_syncX = _XML.readFloatAttribute("animSyncX", 0.f );
            _dest.m_syncY = _XML.readFloatAttribute("animSyncY", 0.f );
            _dest.m_amplitudeX = _XML.readFloatAttribute("animAmplitudeX", 1.f );
            _dest.m_amplitudeY = _XML.readFloatAttribute("animAmplitudeY", 1.f );
            _dest.m_sync = _XML.readFloatAttribute("animSync", 0.f );
            _dest.m_angleUsed = _XML.readIntAttribute("animAngleUsed", 0 ) != 0;
            _dest.m_angle.SetDegrees( _XML.readFloatAttribute("animAngle", 0.f ) );

            _XML.exitElement();
    }
    else
        _dest.m_globalSpeed = 0.f;
}

static i32  frise_findTextureConfigIndex(FriseConfig& config, const char* _textureFriendly)
{
    u32 textureConfigCount = config.m_textureConfigs.size(); 
    for ( u32 scanT = 0; scanT < textureConfigCount; scanT++)
    {
        if (config.m_textureConfigs[scanT].m_friendly == _textureFriendly)
        {
            return scanT;
        }
    }
    return -1;
}

static i32  frise_findFirstNoFillTextureIndex(FriseConfig& config)
{
    u32 textureConfigCount = config.m_textureConfigs.size(); 
    for ( u32 scanT = 0; scanT < textureConfigCount; scanT++)
    {
        if ( scanT != (u32)config.m_fillFriseTextureConfigIndex  && !config.m_textureConfigs[scanT].getPath().isEmpty() )
            return scanT;
    }
    return -1;
}

static void frise_loadSlope(const char* slopeName, PugiXMLWrap& XML, u32 tranche, FriseConfig& config)
{
    if (!XML.enterElement(slopeName))
    {
        config.m_FriseTextureConfigIndexBySlope[tranche] = frise_findFirstNoFillTextureIndex(config);
        return;
    }

    const char* texture = XML.readAttribute("tex");            
    i32 index = frise_findTextureConfigIndex(config, texture);

    config.m_FriseTextureConfigIndexBySlope[tranche] = index;
    XML.exitElement();
}

static void frieze_setZoneCollisionCorner( FriseTextureConfig& _texConfig )
{
    Vec2d collisionCorner;

    // collision OUT
    collisionCorner = _texConfig.m_collisionSimple.m_cornerOut;
    collisionCorner.m_x = 1.f -collisionCorner.m_x;

    if ( collisionCorner.m_y > collisionCorner.m_x )
    {
        _texConfig.m_collisionSimple.m_cornerOutCur = btrue;
        _texConfig.m_collisionSimple.m_cornerOut.m_x = collisionCorner.m_y -collisionCorner.m_x;
        _texConfig.m_collisionSimple.m_cornerOut.m_y = collisionCorner.m_x;
    } 
    else
    {
        _texConfig.m_collisionSimple.m_cornerOutCur = bfalse;
        _texConfig.m_collisionSimple.m_cornerOut.m_x = collisionCorner.m_x -collisionCorner.m_y;
        _texConfig.m_collisionSimple.m_cornerOut.m_y = collisionCorner.m_y;
    }

    // collision IN
    collisionCorner = _texConfig.m_collisionSimple.m_cornerIn;

    if ( collisionCorner.m_x > collisionCorner.m_y )
    {
        _texConfig.m_collisionSimple.m_cornerInCur = btrue;
        _texConfig.m_collisionSimple.m_cornerIn.m_x = collisionCorner.m_x -collisionCorner.m_y;
        _texConfig.m_collisionSimple.m_cornerIn.m_y = 1.f -collisionCorner.m_x;
    } 
    else
    {
        _texConfig.m_collisionSimple.m_cornerInCur = bfalse;
        _texConfig.m_collisionSimple.m_cornerIn.m_x = collisionCorner.m_y -collisionCorner.m_x;
        _texConfig.m_collisionSimple.m_cornerIn.m_y = 1.f -collisionCorner.m_y;
    }
}

static void frieze_zoneCollisionCorner_Rotate90( FriseTextureConfig& _texConfig )
{
    Vec2d collisionRotate;

    // collision OUT
    collisionRotate = _texConfig.m_collisionSimple.m_cornerOut;
    _texConfig.m_collisionSimple.m_cornerOut.m_x = 1.f -collisionRotate.m_y;
    _texConfig.m_collisionSimple.m_cornerOut.m_y = collisionRotate.m_x;

    // collision IN
    collisionRotate = _texConfig.m_collisionSimple.m_cornerIn;
    _texConfig.m_collisionSimple.m_cornerIn.m_x = 1.f -collisionRotate.m_y;
    _texConfig.m_collisionSimple.m_cornerIn.m_y = collisionRotate.m_x;    
}

static void frieze_zoneCollisionCorner_Rotate180( FriseTextureConfig& _texConfig )
{
    Vec2d collisionRotate;

    // collision OUT
    collisionRotate = _texConfig.m_collisionSimple.m_cornerOut;
    _texConfig.m_collisionSimple.m_cornerOut.m_x = 1.f -collisionRotate.m_x;
    _texConfig.m_collisionSimple.m_cornerOut.m_y = 1.f -collisionRotate.m_y;

    // collision IN
    collisionRotate = _texConfig.m_collisionSimple.m_cornerIn;
    _texConfig.m_collisionSimple.m_cornerIn.m_x = 1.f -collisionRotate.m_x;
    _texConfig.m_collisionSimple.m_cornerIn.m_y = 1.f -collisionRotate.m_y;    
}

static void frieze_zoneCollisionCorner_Rotate270( FriseTextureConfig& _texConfig )
{
    Vec2d collisionRotate;

    // collision OUT
    collisionRotate = _texConfig.m_collisionSimple.m_cornerOut;
    _texConfig.m_collisionSimple.m_cornerOut.m_x = collisionRotate.m_y;
    _texConfig.m_collisionSimple.m_cornerOut.m_y = 1.f -collisionRotate.m_x;

    // collision IN
    collisionRotate = _texConfig.m_collisionSimple.m_cornerIn;
    _texConfig.m_collisionSimple.m_cornerIn.m_x = collisionRotate.m_y;
    _texConfig.m_collisionSimple.m_cornerIn.m_y = 1.f -collisionRotate.m_x;    
}

static void frieze_scanTextures(FriseConfig& config)
{
    u32 textureConfigCount = config.m_textureConfigs.size(); 
    config.m_idTexSwitch = -1;

    for ( u32 scanT = 0; scanT < textureConfigCount; scanT++)
    {
        FriseTextureConfig& texConfig = config.m_textureConfigs[scanT];

        if ( texConfig.m_friendly == "texZoneTop" )
        {
            config.m_FriseTextureConfigIndexByZone[0] = scanT;
            frieze_setZoneCollisionCorner( texConfig );
        }
        else if ( texConfig.m_friendly == "texZoneRight" )
        {
            config.m_FriseTextureConfigIndexByZone[1] = scanT;
            frieze_zoneCollisionCorner_Rotate90( texConfig );
            frieze_setZoneCollisionCorner( texConfig );
        }
        else if ( texConfig.m_friendly == "texZoneLeft" )
        {
            config.m_FriseTextureConfigIndexByZone[2] = scanT;
            frieze_zoneCollisionCorner_Rotate270( texConfig );
            frieze_setZoneCollisionCorner( texConfig );
        }
        else if ( texConfig.m_friendly == "texZoneBottom" )
        {
            config.m_FriseTextureConfigIndexByZone[3] = scanT;
            frieze_zoneCollisionCorner_Rotate180( texConfig );
            frieze_setZoneCollisionCorner( texConfig );
        }
        else if ( texConfig.m_friendly == "texSwitch" )
        {
            config.m_idTexSwitch = scanT;
        }
    }
}

void  XMLAll::loadTextureConfig( FriseConfig* pConfig, FriseTextureConfig& _texConfig, PugiXMLWrap& _XML)
{
    PathString8_t pathName;

    pConfig->m_gameMaterial.getString(pathName);

    _texConfig.m_friendly = _XML.readAttribute("friendly");
    _texConfig.m_gameMaterial = _XML.readAttribute("gameMaterial", pathName.cStr() );
    if ( _texConfig.m_gameMaterial.isEmpty() )
        _texConfig.m_gameMaterial = GameMaterial_Template::nullMatName;

    u32 red = (u32)(_XML.readIntAttribute("red", 255));
    u32 green = (u32)(_XML.readIntAttribute("green", 255));
    u32 blue = (u32)(_XML.readIntAttribute("blue", 255));
    u32 alpha = (u32)(_XML.readIntAttribute("alpha", 255));
    _texConfig.m_color = ITFCOLOR_TO_U32(alpha,red,green,blue);  

    // fill
    _texConfig.m_fillOffset = _XML.readFloatAttribute("fillingOffset", pConfig->m_fillOffSet );

    // collision.
    _texConfig.m_collisionSimple.m_build   = _XML.readIntAttribute("collision", pConfig->m_collisionFrieze.m_build ) != 0;
    _texConfig.m_collisionSimple.m_offset   = _XML.readFloatAttribute("collisionOffset", pConfig->m_collisionFrieze.m_offset );  
    _texConfig.m_collisionSimple.m_cornerIn = _XML.readVec2dAttribute("collisionCornerIn", Vec2d(0.5f,0.5f) );
    _texConfig.m_collisionSimple.m_cornerOut = _XML.readVec2dAttribute("collisionCornerOut", Vec2d(0.5f,0.5f) );

    //  scrolling texture
    _texConfig.m_scrollingSpeedTrans.m_x = _XML.readFloatAttribute("scrollU", 0.f );
    _texConfig.m_scrollingSpeedTrans.m_y = _XML.readFloatAttribute("scrollV", 0.f );
    _texConfig.m_scrollingSpeedRotate = _XML.readFloatAttribute("scrollAngle", 0.f );

    //  alpha
    _texConfig.m_alphaBorder = (u8)_XML.readIntAttribute("alphaBorder",alpha);
    _texConfig.m_alphaUp = (u8)_XML.readIntAttribute("alphaUp",alpha);

    // Blend Mode
    _texConfig.m_gfxMat.m_blendMode = GFX_BLEND_UNKNOWN;
    
    // first try to read "blending" tag
    String8 blendMode = _XML.readAttribute("blending");
    if (blendMode.getLen() > 0)
    {
        blendMode.toLower();

        #define DECL_BLEND_MODE(_mode, _value) if (strstr(blendMode.cStr(), _mode)) _texConfig.m_gfxMat.m_blendMode = _value;
        DECL_BLEND_MODE("copy", GFX_BLEND_COPY)
        DECL_BLEND_MODE("alpha", GFX_BLEND_ALPHA)
        DECL_BLEND_MODE("aplpha_premult", GFX_BLEND_ALPHAPREMULT)
        DECL_BLEND_MODE("alpha_dest", GFX_BLEND_ALPHADEST)
        DECL_BLEND_MODE("alpha_dest_premult", GFX_BLEND_ALPHADESTPREMULT)
        DECL_BLEND_MODE("add", GFX_BLEND_ADD)
        DECL_BLEND_MODE("add_alpha", GFX_BLEND_ADDALPHA)
        DECL_BLEND_MODE("sub_alpha", GFX_BLEND_SUBALPHA)
        DECL_BLEND_MODE("sub", GFX_BLEND_SUB)
        DECL_BLEND_MODE("mul", GFX_BLEND_MUL)
        DECL_BLEND_MODE("alpha_mul", GFX_BLEND_ALPHAMUL)
        DECL_BLEND_MODE("i_alpha_mul", GFX_BLEND_IALPHAMUL)
        DECL_BLEND_MODE("i_alpha", GFX_BLEND_IALPHA)
        DECL_BLEND_MODE("i_alpha_premult", GFX_BLEND_IALPHAPREMULT)
        DECL_BLEND_MODE("i_alpha_dest", GFX_BLEND_IALPHADEST)
        DECL_BLEND_MODE("i_alpha_dest_premult", GFX_BLEND_IALPHADESTPREMULT)
        DECL_BLEND_MODE("mul2x", GFX_BLEND_MUL2X)
        DECL_BLEND_MODE("alpha_to_color", GFX_BLEND_ALPHATOCOLOR)
        DECL_BLEND_MODE("i_alpha_to_color", GFX_BLEND_IALPHATOCOLOR)
        #undef DECL_BLEND_MODE

        if (GFX_BLEND_UNKNOWN == _texConfig.m_gfxMat.m_blendMode)
            ITF_ERROR("unknown value for tag \"blending\" in frise config %s", pConfig->getPath().cStr());
    }

    // if "blending" tag not found or invalid, try old "blendMode" tag
    if (GFX_BLEND_UNKNOWN == _texConfig.m_gfxMat.m_blendMode)
        _texConfig.m_gfxMat.m_blendMode = (GFX_BLENDMODE)_XML.readIntAttribute("blendMode", GFX_BLEND_UNKNOWN);

    // if "blendMode" tag failed, assign default value
    if (GFX_BLEND_UNKNOWN == _texConfig.m_gfxMat.m_blendMode)
        _texConfig.m_gfxMat.m_blendMode = GFX_BLEND_ALPHA;

    // Gfx Material.
    String matType;
    matType = _XML.readAttribute("gfxMaterial", "");
    matType.toLower();

    if (matType == "refraction")
    {
        _texConfig.m_gfxMat.m_matType = GFX_MAT_REFRACTION;
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(0, _XML.readFloatAttribute("gfxMatRefract_texmul", 1.f));
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(1, _XML.readFloatAttribute("gfxMatRefract_normalmul", 0.1f));
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(2, _XML.readFloatAttribute("gfxMatRefract_offsetx", 0.f));
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(3, _XML.readFloatAttribute("gfxMatRefract_offsety", 0.f));
        _texConfig.m_gfxMat.m_matParams.setiParamsAt(0, _XML.readIntAttribute("gfxMatRefract_downscale2x2", 0));
    }
    else if (matType == "alphafade")
    {
        _texConfig.m_gfxMat.m_matType = GFX_MAT_ALPHAFADE;
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(0, _XML.readFloatAttribute("gfxMaterialalphafadeDistMin", 0.f));
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(1, _XML.readFloatAttribute("gfxMaterialalphafadeDistMax", 10.f));
        _texConfig.m_gfxMat.m_matParams.setiParamsAt(0, _XML.readIntAttribute("gfxMaterialalphafadetype", 0));
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(2, _XML.readFloatAttribute("gfxMaterialalphafadeDensity", 0.5f));
    }
    else if (matType == "glow")
    {
        _texConfig.m_gfxMat.m_matType = GFX_MAT_GLOW;
        _texConfig.m_gfxMat.m_matParams.setfParamsAt(0, _XML.readFloatAttribute("gfxMaterialGlowSize", 10.f));
    }
    else
        _texConfig.m_gfxMat.m_matType = GFX_MAT_DEFAULT;

}


void XMLAll::readFluidFriseLayers(PugiXMLWrap &XML, FriseConfig* _pConfig)
{
    _pConfig->m_fluidLayers.clear();
    _pConfig->m_fluidLayers.resize(_pConfig->m_fluidLayerCount);
    if ( XML.enterElement("Layer"))
    {
        do 
        {
            i32 layerIndex = XML.readIntAttribute("index",1);
            layerIndex--;
            ITF_ASSERT(layerIndex>=0 && layerIndex<(i32)_pConfig->m_fluidLayerCount);
            if (layerIndex>=0 && layerIndex<(i32)_pConfig->m_fluidLayerCount)
            {
                FluidFriseLayer layer;
                i32 red = XML.readIntAttribute("red",255);
                i32 green = XML.readIntAttribute("green",255);
                i32 blue = XML.readIntAttribute("blue",255);
                layer.m_erosion = XML.readIntAttribute("erosion",0);
                layer.m_color = Color(1.f,red/255.f,green/255.f,blue/255.f);
                layer.m_threshold = XML.readFloatAttribute("threshold", layer.m_threshold);
                layer.m_gameMaterialPath = XML.readAttribute("gameMaterial", "");
                layer.m_gameMaterialIds[FluidFriseLayer::NormalMaterial] = layer.m_gameMaterialPath.getStringID();
                layer.m_backsideGameMaterialPath = XML.readAttribute("backsideGameMaterial", "");
                layer.m_gameMaterialIds[FluidFriseLayer::BacksideMaterial] = layer.m_backsideGameMaterialPath.getStringID();
                layer.m_collisionHeight = XML.readFloatAttribute("collisionHeight", layer.m_collisionHeight);
                layer.m_hasBacksideInversion = XML.readIntAttribute("hasBacksideInversion", layer.m_hasBacksideInversion);
                layer.m_forcedHeightWhenNotColored = XML.readFloatAttribute("forcedHeightWhenNotColored", layer.m_forcedHeightWhenNotColored);
                layer.m_hasForcedHeightWhenNotColored = (layer.m_forcedHeightWhenNotColored != -FLT_MAX);
                _pConfig->m_fluidLayers[layerIndex] = layer;
            }
        } while (XML.nextSameElement());
        
    }
}


FriseConfig* XMLAll::loadFriseConfig(const String& _fileArg, bbool _reload, bbool _cooking )
{
    PugiXMLWrap XML;

    String file = FilePath::normalizePath(_fileArg);

    if (!XML.openFile(file, bfalse))
    {
        return NULL;
    }

    if (XML.enterElement("FriseConfig", PugiXMLWrap::LogWarning))
    {
        ResourceID ID;
        FriseConfig* pConfig;

            // Create the config from its path (or retrieve the already existing one)
            if ( RESOURCE_MANAGER->getResourceIdFromFile(ID, file, Resource::ResourceType_FriezeConfig) )
            {
                pConfig = static_cast<FriseConfig*>(ID.getResource());
                _reload = btrue;
                ResourceGroup* pGrp = pConfig->getResourceGroup();
                
                if (pGrp && _reload)
                {
                    Frise::freeResourceGroup(pGrp);

                    u32 physCount = pGrp->getPhysicalUsersCount();
                    for (u32 removePhys = 0; removePhys < physCount; removePhys++)
                        RESOURCE_MANAGER->queryRemoveUserToResourcePhysicalData(pGrp);

                    RESOURCE_MANAGER->flushPendingOps();
                    pGrp->clearAll();
                    RESOURCE_MANAGER->flushPendingOps();
                }
            }
            else
            {
                ITF_ASSERT(!_reload);
                ID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_FriezeConfig, file);
                pConfig = static_cast<FriseConfig*>(ID.getResource());
            }

        // Initialize values (and delete already existing data)
        pConfig->startSerialization_load();

        PathString8_t nullMatPathName;

        GameMaterial_Template::nullMatName.getString(nullMatPathName);

        //  Read <Global> tags and fill
        {
            pConfig->m_renderRank = XML.readFloatAttribute("global/renderRankForced", 0.f );
            pConfig->m_renderRankForced = XML.readFloatAttribute("global/renderRankForced", F32_INFINITY ) != F32_INFINITY;            
            pConfig->m_posZ = XML.readFloatAttribute("global/zForced", 0.f );
            pConfig->m_posZForced = XML.readFloatAttribute("global/zForced", F32_INFINITY ) != F32_INFINITY;

            pConfig->m_gameMaterial = XML.readAttribute("global/gameMaterial", nullMatPathName.cStr() );
            checkGameMaterial(pConfig->m_gameMaterial);

            pConfig->m_gameMaterialStartExtremity = XML.readAttribute("global/gameMaterialExtremityStart", nullMatPathName.cStr() );     
            checkGameMaterial(pConfig->m_gameMaterialStartExtremity);

            pConfig->m_gameMaterialStopExtremity = XML.readAttribute("global/gameMaterialExtremityStop", nullMatPathName.cStr() );     
            checkGameMaterial(pConfig->m_gameMaterialStopExtremity);

            pConfig->m_visualOffset = XML.readFloatAttribute("global/visualOffset", 0.5f );
            pConfig->m_regionId = XML.readAttribute("global/regionId");         
            pConfig->m_useFriezeScale = XML.readIntAttribute("global/useFriezeScale", 0 ) != 0;
            pConfig->m_cornerFactor = XML.readFloatAttribute("global/cornerFactor", 1.4142f );
            pConfig->m_height = XML.readFloatAttribute("global/height", 2.f );
            pConfig->m_width = XML.readFloatAttribute("global/width", 2.f );
            pConfig->m_methode = XML.readIntAttribute("global/methode", 1 );
            pConfig->m_snapCoeff = XML.readFloatAttribute("global/snapCoeff", 0.1f );
            pConfig->m_snapCoeffUv = XML.readFloatAttribute("global/snapCoeffUv", 0.65f );
            pConfig->m_flexibility = XML.readFloatAttribute("global/flexibility", 0.75f );     
            pConfig->m_wallAngle = XML.readFloatAttribute("global/wallAngle", 45.f ) *MTH_DEGTORAD;

            pConfig->m_isUvFlipX = XML.readIntAttribute("global/isUvFlipX", 0 ) != 0;
            pConfig->m_isUvFlipY = XML.readIntAttribute("global/isUvFlipY", 0 ) != 0;
            pConfig->m_isRatioFixed = XML.readIntAttribute("global/isRatioFixed", 1 ) != 0;

            // smooth
            pConfig->m_smoothVisual = XML.readIntAttribute("global/smoothVisual", 1 ) != 0;
            pConfig->m_smoothFilling = XML.readIntAttribute("global/smoothFilling", 0 ) != 0;
            pConfig->m_smoothCollision = XML.readIntAttribute("global/smoothCollision", 0 ) != 0;
            pConfig->m_smoothFactorVisual = XML.readFloatAttribute("global/smoothFactorVisual", 4.f );
            pConfig->m_smoothFactorFilling = XML.readFloatAttribute("global/smoothFactorFilling", 2.f );
            pConfig->m_smoothFactorCollision = XML.readFloatAttribute("global/smoothFactorCollision", 2.f );

            // overlays
            pConfig->m_scale = XML.readFloatAttribute("global/scale", 1.f );
            pConfig->m_density = XML.readFloatAttribute("global/density", 1.f ); 

			// object family
			{
				String8 family = XML.readAttribute("global/ObjectFamily");
				pConfig->m_objectFamily = Pickable::ObjectFamily_NotSet;
				if (family.getLen())
				{
					if (family.strstr("ObjectFamily_LevelDesign"))
						pConfig->m_objectFamily |= Pickable::ObjectFamily_LevelDesign;
					if (family.strstr("ObjectFamily_Art"))
						pConfig->m_objectFamily |= Pickable::ObjectFamily_Art;
					if (family.strstr("ObjectFamily_Alpha"))
						pConfig->m_objectFamily |= Pickable::ObjectFamily_Alpha;
				}
			}

            // filling
            pConfig->m_fillOffSet = XML.readFloatAttribute("global/fillingOffset", 0.5f ); 

            // collision
            pConfig->m_isCollisionFlip = XML.readIntAttribute("global/collisionFlip", 0 ) != 0;
            pConfig->m_collisionDouble = XML.readIntAttribute("global/collisionDouble", 0 );
            pConfig->m_collisionDistMaxToSkipParallelEdge = XML.readFloatAttribute("global/collisionDistMaxToSkipParallelEdge", 0.075f ); 
            pConfig->m_collisionDistMaxToSkipParallelEdge = Max( pConfig->m_collisionDistMaxToSkipParallelEdge, 0.01f); // to not have parallel edge collision

            // collision Simple
            pConfig->m_collisionFrieze.m_build = XML.readIntAttribute("global/collision", 0 ) != 0; 
            pConfig->m_collisionFrieze.m_offset = XML.readFloatAttribute("global/collisionOffset", 0.5f );                         
            pConfig->m_collisionFrieze.m_extremity = XML.readVec2dAttribute("global/collisionExtremity", Vec2d( F32_INFINITY, 0.5f ) );
            pConfig->m_collisionFrieze.m_extremity2 = XML.readVec2dAttribute("global/collisionExtremity2", Vec2d( F32_INFINITY, 0.5f ) );

            // vtx
            pConfig->m_zVtxExtrudeUp = XML.readFloatAttribute("global/zExtrudeUp", 0.f);            
            pConfig->m_zVtxExtrudeDown = XML.readFloatAttribute("global/zExtrudeDown", 0.f );

            u32 sred = (u32)(XML.readIntAttribute("global/selfIllumRed", 255));
            u32 sgreen = (u32)(XML.readIntAttribute("global/selfIllumGreen", 255));
            u32 sblue = (u32)(XML.readIntAttribute("global/selfIllumBlue", 255));
            pConfig->m_selfIllumColor = ITFCOLOR_TO_U32(255,sred,sgreen,sblue);  

            pConfig->m_updateType = (Pickable::UpdateType) XML.readIntAttribute("global/updatetype", (int)pConfig->m_updateType);
        }

        // Read <Texture> tags
        if (XML.enterElement("Texture", PugiXMLWrap::LogWarning))
        {    
            do
            {
                // Creation & initialization

                FriseTextureConfig texConfig;
                texConfig.startSerialization_load();

                // Set textures paths
                String texPath(XML.readAttribute("path"));
                if(!texPath.isEmpty())
                    texConfig.setPath(texPath);
                                           
                String Normal = XML.readAttribute("normaltexture");
                if (Normal.getLen())
                    texConfig.setNormal(Normal);          
                
                loadTextureConfig(pConfig, texConfig, XML);  
            
                texConfig.endSerialization_load();
                if (!_cooking)
                    texConfig.onLoaded(*pConfig);

                ITF_VECTOR<FriseTextureConfig> ::iterator it = pConfig->m_textureConfigs.begin();
                pConfig->m_textureConfigs.insert(it,texConfig);// push_back(texConfig);

            } while (XML.nextSameElement());
        }

        // Read <Plugin>
        pConfig->m_plugin.invalidate();
        pConfig->m_pluginSettings.m_params.clear();
        if (XML.enterElement("Plugin", PugiXMLWrap::LogWarning))
        {   
            pConfig->m_plugin = XML.readAttribute("name", "" );

            if (XML.enterFirstElement())
            {
                do
                {
                    f32 val = XML.readFloatAttribute("value", 0.f);
                    pConfig->m_pluginSettings.m_params.insert(std::pair<StringID, f32>(XML.readElementName(), val));
                } while (XML.nextElement());
            }
            XML.exitElement();
        }

        // Read <Animation> tags
        if (XML.enterElement("Animation"))
        {    
            do
            {
                FriseTextureConfig texConfig;
                texConfig.startSerialization_load();

                String texPath(XML.readAttribute("path"));
                if(!texPath.isEmpty())
                    texConfig.setAnim(texPath);

                texConfig.m_gameMaterial = XML.readAttribute("gameMaterial", nullMatPathName.cStr() );
                texConfig.m_AnimTranslationX = XML.readFloatAttribute("AnimTranslationX", 0.f);
                texConfig.m_AnimTranslationY = XML.readFloatAttribute("AnimTranslationY", 0.f);
                texConfig.m_AnimScaleX = XML.readFloatAttribute("AnimScaleX", 1.f);
                texConfig.m_AnimScaleY = XML.readFloatAttribute("AnimScaleY", 1.f);
                texConfig.m_AnimRot = XML.readFloatAttribute("AnimRot", 0.f);
                texConfig.m_AnimIndex = XML.readIntAttribute("AnimIndex", 0);
                texConfig.m_AnimTexSizeX = XML.readIntAttribute("AnimTextureSizeX", 256);
                texConfig.m_AnimTexSizeY = XML.readIntAttribute("AnimTextureSizeY", 256);

                u8 tred = (u8)(XML.readIntAttribute("AnimBGRed", 0));
                u8 tgreen = (u8)(XML.readIntAttribute("AnimBGGreen", 0));
                u8 tblue = (u8)(XML.readIntAttribute("AnimBGBlue", 0));
                u8 talpha = (u8)(XML.readIntAttribute("AnimBGAlpha", 0));
                texConfig.m_AnimBackGroundColor = ColorInteger(talpha,tred,tgreen,tblue);

                Vec2d aabbMinVal(Vec2d::Zero);
                Vec2d aabbMaxVal(Vec2d::Zero);
                aabbMinVal.m_x = XML.readFloatAttribute("AnimAABBMinX", 0.f);
                aabbMinVal.m_y = XML.readFloatAttribute("AnimAABBMinY", 0.f);
                aabbMaxVal.m_x = XML.readFloatAttribute("AnimAABBMaxX", 0.f);
                aabbMaxVal.m_y = XML.readFloatAttribute("AnimAABBMaxY", 0.f);
                texConfig.m_animAABB.setMin(aabbMinVal);
                texConfig.m_animAABB.setMax(aabbMaxVal);
                texConfig.m_animPhase = XML.readFloatAttribute("AnimPhase", 0.f);

                loadTextureConfig(pConfig, texConfig, XML);  

                texConfig.endSerialization_load();
                if (!_cooking)
                    texConfig.onLoaded(*pConfig);

                pConfig->m_textureConfigs.push_back(texConfig);
            } while (XML.nextSameElement());
        }

        // WARNING: Fill info MUST be read after texture & animation info and before slopes info          
        pConfig->m_fillFriseTextureConfigIndex = frise_findTextureConfigIndex(*pConfig, XML.readAttribute("fill/tex")); 
        pConfig->m_fillAngle.SetDegrees( XML.readFloatAttribute("fill/angle", 0.f ) );
        pConfig->m_fillScale = XML.readVec2dAttribute("fill/scale", pConfig->m_fillScale );

        

        // Read slopes
        u32 tranche = 0;

        frise_loadSlope("slope_180", XML, tranche++, *pConfig);
        frise_loadSlope("slope_202", XML, tranche++, *pConfig);
        frise_loadSlope("slope_225", XML, tranche++, *pConfig);
        frise_loadSlope("slope_247", XML, tranche++, *pConfig);
        frise_loadSlope("slope_270", XML, tranche++, *pConfig);
        frise_loadSlope("slope_292", XML, tranche++, *pConfig);
        frise_loadSlope("slope_315", XML, tranche++, *pConfig);
        frise_loadSlope("slope_337", XML, tranche++, *pConfig);
        frise_loadSlope("slope_0", XML, tranche++, *pConfig);

        frise_loadSlope("slope_22", XML, tranche++, *pConfig);
        frise_loadSlope("slope_45", XML, tranche++, *pConfig);
        frise_loadSlope("slope_67", XML, tranche++, *pConfig);
        frise_loadSlope("slope_90", XML, tranche++, *pConfig);
        frise_loadSlope("slope_112", XML, tranche++, *pConfig);
        frise_loadSlope("slope_135", XML, tranche++, *pConfig);
        frise_loadSlope("slope_157", XML, tranche++, *pConfig);
        frise_loadSlope("slope_180", XML, tranche++, *pConfig);

        // set zone
        frieze_scanTextures( *pConfig );

        // vertex anim
        frieze_readVtxAnim( XML, pConfig->m_vtxAnim);

        if ( XML.enterElement("Fluid") )
        {  
            pConfig->m_fluidElasticity       = XML.readFloatAttribute("elasticity", pConfig->m_fluidElasticity );
            pConfig->m_fluidVelocity         = XML.readFloatAttribute("velocity", pConfig->m_fluidVelocity );
            pConfig->m_fluidViscosity        = XML.readFloatAttribute("viscosity", pConfig->m_fluidViscosity );
            pConfig->m_fluidTargetHeight     = XML.readFloatAttribute("targetHeight", pConfig->m_fluidTargetHeight );
            pConfig->m_fluidAmplification    = XML.readFloatAttribute("amplification", pConfig->m_fluidAmplification );
            pConfig->m_fluidTargetMaxHeight  = XML.readFloatAttribute("maxTargetHeight", pConfig->m_fluidTargetMaxHeight );
            pConfig->m_fluidTargetMinHeight  = XML.readFloatAttribute("minTargetHeight", pConfig->m_fluidTargetMinHeight );
            pConfig->m_fluidTargetAddHeight  = XML.readFloatAttribute("addHeight", pConfig->m_fluidTargetAddHeight );
            pConfig->m_fluidUnityWidth       = XML.readFloatAttribute("unityWidth", pConfig->m_fluidUnityWidth );
            pConfig->m_fluidPolylineUnityMult= XML.readIntAttribute("polylineUnityMult", pConfig->m_fluidPolylineUnityMult );
            if (pConfig->m_fluidPolylineUnityMult < 1)
                pConfig->m_fluidPolylineUnityMult = 1;

            pConfig->m_fluidEnterMult       = XML.readFloatAttribute("enterMult", pConfig->m_fluidEnterMult );
            pConfig->m_fluidLeaveMult       = XML.readFloatAttribute("leaveMult", pConfig->m_fluidLeaveMult );
            pConfig->m_fluidMaxDstInfluence = XML.readFloatAttribute("maxDstInfluence", pConfig->m_fluidMaxDstInfluence );
            pConfig->m_fluidLevelsFront     = XML.readIntAttribute("levelsFront", pConfig->m_fluidLevelsFront );
            pConfig->m_fluidLevelsBack      = XML.readIntAttribute("levelsBack", pConfig->m_fluidLevelsBack );
            pConfig->m_fluidLevelDelta.m_x  = XML.readFloatAttribute("levelDeltaX", pConfig->m_fluidLevelDelta.m_x);
            pConfig->m_fluidLevelDelta.m_y  = XML.readFloatAttribute("levelDeltaY", pConfig->m_fluidLevelDelta.m_y);
            pConfig->m_fluidLevelDelta.m_z  = XML.readFloatAttribute("levelDeltaZ", pConfig->m_fluidLevelDelta.m_z);
            pConfig->m_fluidUVDelta.m_x     = XML.readFloatAttribute("UVDeltaX", pConfig->m_fluidUVDelta.m_x);
            pConfig->m_fluidUVDelta.m_y     = XML.readFloatAttribute("UVDeltaY", pConfig->m_fluidUVDelta.m_y);

            pConfig->m_fluidPerpendicularBack       = XML.readIntAttribute("perpendicularBack", pConfig->m_fluidPerpendicularBack );
            pConfig->m_fluidPerpendicularBackZ      = XML.readFloatAttribute("perpendicularBackZ", pConfig->m_fluidPerpendicularBackZ );
            pConfig->m_fluidPerpendicularBackScale  = XML.readFloatAttribute("perpendicularBackScale", pConfig->m_fluidPerpendicularBackScale );
            pConfig->m_fluidPerpendicularBackPos    = XML.readFloatAttribute("perpendicularBackPos", pConfig->m_fluidPerpendicularBackPos );
            pConfig->m_fluidPerpendicularBackPosZ   = XML.readFloatAttribute("perpendicularBackPosZ", pConfig->m_fluidPerpendicularBackPosZ );
            pConfig->m_fluidPerpendicularFront      = XML.readIntAttribute("perpendicularFront", pConfig->m_fluidPerpendicularFront );
            pConfig->m_fluidPerpendicularFrontZ     = XML.readFloatAttribute("perpendicularFrontZ", pConfig->m_fluidPerpendicularFrontZ );
            pConfig->m_fluidPerpendicularFrontScale = XML.readFloatAttribute("perpendicularFrontScale", pConfig->m_fluidPerpendicularFrontScale );
            pConfig->m_fluidPerpendicularFrontPos   = XML.readFloatAttribute("perpendicularFrontPos", pConfig->m_fluidPerpendicularFrontPos );
            pConfig->m_fluidPerpendicularFrontPosZ  = XML.readFloatAttribute("perpendicularFrontPosZ", pConfig->m_fluidPerpendicularFrontPosZ );

            pConfig->m_fluidWeightMultiplier        = XML.readFloatAttribute("fluidWeightMultiplier", pConfig->m_fluidWeightMultiplier);
            pConfig->m_dstInfluenceMultiplier       = XML.readFloatAttribute("dstInfluenceMultiplier", pConfig->m_dstInfluenceMultiplier);

            pConfig->m_fluidAbsorptionAtEdgeStart   = XML.readFloatAttribute("absorptionAtEdgeStart", pConfig->m_fluidAbsorptionAtEdgeStart);
            pConfig->m_fluidAbsorptionAtEdgeEnd     = XML.readFloatAttribute("absorptionAtEdgeEnd", pConfig->m_fluidAbsorptionAtEdgeEnd);
            pConfig->m_fluidAbsorptionAtEdge_Length = XML.readFloatAttribute("absorptionAtEdge_Length", pConfig->m_fluidAbsorptionAtEdge_Length);

            pConfig->m_fluidInfluenceLimit          = XML.readFloatAttribute("influenceLimit", pConfig->m_fluidInfluenceLimit);

            pConfig->m_fluidSideCount               = std::max(std::min((u32)XML.readIntAttribute("sideCount", pConfig->m_fluidSideCount),(u32)2),(u32)1);
            pConfig->m_fluidLayerCount              = std::max(std::min((u32)XML.readIntAttribute("layerCount", pConfig->m_fluidLayerCount),(u32)MAX_FLUID_LAYER_COUNT),(u32)1);

            readFluidFriseLayers(XML,pConfig);

            XML.exitElement();
        }

        pConfig->endSerialization_load(_cooking);
        if (!_cooking)
        {
            pConfig->onLoaded();
            RESOURCE_MANAGER->flushPendingOps();
        }

        return pConfig;
    }
    else
    {
        return NULL;
    }
}


#endif //ITF_SUPPORT_XML


} // namespace ITF

