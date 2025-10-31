#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONDRAW_H_
#include    "engine/animation/AnimationDraw.h"
#endif // _ITF_ANIMATIONDRAW_H_

#ifndef _ITF_ANIMATION_H_
#include    "engine/animation/Animation.h"
#endif // _ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include    "engine/animation/AnimationManager.h"
#endif // _ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif // _ITF_GFXADAPTER_RENDERTARGET_H_

#ifdef ITF_WII
#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif // _ITF_WII_DRAWPRIM_H_
#endif // ITF_WII


namespace ITF
{
    DECLARE_RASTER(AnimDraw_ImpostorProcessPatch, RasterGroup_Animation, Color::violet());
    DECLARE_RASTER(AnimDraw_PreparePatch, RasterGroup_Animation, Color::cyan());
    DECLARE_RASTER(AnimDraw_patchToMesh, RasterGroup_Animation, Color::red());
    DECLARE_RASTER(AnimDraw_drawPatchBones, RasterGroup_Animation, Color::yellow());

    ImpostorMode::ImpostorMode() :
      m_animInfo(NULL)
    , m_renderTarget(NULL)
    {
    }

    ImpostorMode::~ImpostorMode()
    {
        if (m_animInfo && m_renderTarget)
        {
            GFX_ADAPTER->stopImpostorRendering(m_renderTarget);
            m_animInfo->m_Pos = m_savedPos;
        }
    }

    void ImpostorMode::start(AnimInfo *_animInfo, renderTarget* _renderTarget)
    {
        m_animInfo      = _animInfo;
        m_savedPos      = m_animInfo->m_Pos;
        m_renderTarget  = _renderTarget;
        
        Vec2d   minAABB = m_animInfo->m_visualAABB.getMin();
        Vec2d   maxAABB = m_animInfo->m_visualAABB.getMax();
		Vec2d   SizeAABB = maxAABB - minAABB;
/*        f32     sizex   = m_animInfo->m_visualAABB.getWidth();
        f32     sizey   = m_animInfo->m_visualAABB.getHeight();*/
        if (m_animInfo->m_globalData.m_mirror)
            minAABB.m_x = -maxAABB.m_x;
        m_animInfo->getCurrentSkeleton()->Translate(&m_animInfo->getCurrentBoneList(), -minAABB, m_animInfo->m_globalData);
		GFX_ADAPTER->startImpostorRendering(m_renderTarget, 0, 0, SizeAABB.m_x, SizeAABB.m_y);
    }


/*
#define GetInterPlus(pDst , _pos, _normal, lenght)\
    {\
		pDst.m_x = _pos.m_x + _normal.m_y * lenght;\
		pDst.m_y = _pos.m_y - _normal.m_x * lenght;\
    }

#define GetInterMinus(pDst , _pos, _normal, lenght)\
    {\
		pDst.m_x = _pos.m_x - _normal.m_y * lenght;\
		pDst.m_y = _pos.m_y + _normal.m_x * lenght;\
    }*/



#define GetInterPlus(pDst , _pos, _normal, lenght)\
	{register Vec2d mNorm = -_normal;\
	pDst = _pos + Vec2d::mergeYX(&_normal , &mNorm) * lenght;}

#define GetInterMinus(pDst , _pos, _normal, lenght)\
	{register Vec2d mNorm = -_normal;\
	pDst = _pos + Vec2d::mergeYX(&mNorm , &_normal) * lenght;}

    ITF_INLINE Vec2d GetInter1(AnimPatchPointDyn *_bone1, AnimPatchPointDyn *_bone2, f32 lenght)
    {
        return _bone1->m_Pos + Turn90(_bone1->m_Normale) * lenght;
    }

    ITF_INLINE Vec2d GetInter2(AnimPatchPointDyn * _bone1, AnimPatchPointDyn * _bone2, f32 lenght)
    {
        return _bone2->m_Pos - Turn90(_bone2->m_Normale) * lenght;
    }

    ITF_INLINE Vec2d GetInterUV1(AnimPatchPoint * _bone1, AnimPatchPoint * _bone2, f32 lenght)
    {
        return _bone1->m_PosUV - Turn90(_bone1->m_NormaleUV) * lenght;
    }

    ITF_INLINE Vec2d GetInterUV2(AnimPatchPoint * _bone1, AnimPatchPoint * _bone2, f32 lenght)
    {
        return _bone2->m_PosUV + Turn90(_bone2->m_NormaleUV) * lenght;
    }

#ifdef ANIMUSEGEOMETRYSHADER
    void appendPatchData(Patch32Data *_pData, AnimPatchPointDyn *_ppDyn[], AnimPatchPoint  *_pp[], Texture *texture)
    {
        VertexPatchAnim	* __restrict write = _pData->vertexListPatch + _pData->vertexCount;

        AnimPatchPointDyn * ppDyn0 = _ppDyn[0];
        AnimPatchPointDyn * ppDyn1 = _ppDyn[1];
        AnimPatchPointDyn * ppDyn2 = _ppDyn[2];
        AnimPatchPointDyn * ppDyn3 = _ppDyn[3];

        AnimPatchPoint * pp0 = _pp[0];
        AnimPatchPoint * pp1 = _pp[1];
        AnimPatchPoint * pp2 = _pp[2];
        AnimPatchPoint * pp3 = _pp[3];

        write->setData(ppDyn0->m_Pos, ppDyn0->m_Normale, pp0->m_PosUV, pp0->m_NormaleUV, ppDyn0->m_boneIdx);
        write++;
        write->setData(ppDyn1->m_Pos, ppDyn1->m_Normale, pp1->m_PosUV, pp1->m_NormaleUV, ppDyn1->m_boneIdx);
        write++;
        write->setData(ppDyn2->m_Pos, ppDyn2->m_Normale, pp2->m_PosUV, pp2->m_NormaleUV, ppDyn2->m_boneIdx);
        write++;
        write->setData(ppDyn3->m_Pos, ppDyn3->m_Normale, pp3->m_PosUV, pp3->m_NormaleUV, ppDyn3->m_boneIdx);
        write++;

        ITF_MeshElement &elem = _pData->mesh->m_ElementList[0];
        elem.m_material.m_textureDiffuse = texture->getID();

        ITF_MeshElement &curElem                = _pData->mesh->m_ElementList[0];
        curElem.m_count                         += 4;
        _pData->indexCount                      += 4;
        _pData->vertexCount                     += 4;

    }
#endif


    //Compute control points from bone data. Output is in NSPoints[16]. Indices 0-7 are for positions, 8-15 for UVs
	 void computePatchControlPoints_Internal(Vec2d * ITF_RESTRICT _NSPoints, AnimPatchPointDyn *_ppDyn[], AnimPatchPoint  *_pp[], u32 _hdiv, u32 _vdiv, bbool _invert)
    {

        const AnimPatchPointDyn * ppDyn0 = _ppDyn[0];
        const AnimPatchPointDyn * ppDyn1 = _ppDyn[1];
        const AnimPatchPointDyn * ppDyn2 = _ppDyn[2];
        const AnimPatchPointDyn * ppDyn3 = _ppDyn[3];

        const AnimPatchPoint * pp0 = _pp[0];
        const AnimPatchPoint * pp1 = _pp[1];
        const AnimPatchPoint * pp2 = _pp[2];
        const AnimPatchPoint * pp3 = _pp[3];
        _NSPoints[0]   = ppDyn0->m_Pos;
        _NSPoints[3]   = ppDyn1->m_Pos;
        _NSPoints[4]   = ppDyn2->m_Pos;
        _NSPoints[7]   = ppDyn3->m_Pos;
#ifdef ITF_WII
		ps::Float2 T1 = (*((ps::Float2*)&ppDyn0->m_Pos) - *((ps::Float2*)&ppDyn1->m_Pos));
		ps::Float2 T2 = (*((ps::Float2*)&ppDyn2->m_Pos) - *((ps::Float2*)&ppDyn3->m_Pos));
		ps::Float2 lenghtP1P2_P3P4 = ps::Sqrt(ps::Merge(ps::Dot(T1,T1),ps::Dot(T2,T2))) * 0.5f;
		ps::Float2 UVT1 = (*((ps::Float2*)&pp0->m_PosUV) - *((ps::Float2*)&pp1->m_PosUV));
		ps::Float2 UVT2 = (*((ps::Float2*)&pp2->m_PosUV) - *((ps::Float2*)&pp3->m_PosUV));
		ps::Float2 UVlenghtP1P2_P3P4 = ps::Sqrt(ps::Merge(ps::Dot(UVT1,UVT1),ps::Dot(UVT2,UVT2)))  * 0.5f;

		if (_invert) lenghtP1P2_P3P4 = -lenghtP1P2_P3P4;

        GetInterMinus(_NSPoints[1]   , ppDyn0->m_Pos, ppDyn0->m_Normale, ps::GetX(lenghtP1P2_P3P4));
        GetInterPlus( _NSPoints[2]   , ppDyn1->m_Pos, ppDyn1->m_Normale, ps::GetX(lenghtP1P2_P3P4));
        GetInterPlus( _NSPoints[5]   , ppDyn2->m_Pos, ppDyn2->m_Normale, ps::GetY(lenghtP1P2_P3P4));
        GetInterMinus(_NSPoints[6]   , ppDyn3->m_Pos, ppDyn3->m_Normale, ps::GetY(lenghtP1P2_P3P4));


        //independent from Points[0-7]
		_NSPoints[8]   = pp0->m_PosUV;
        GetInterPlus( _NSPoints[9]  ,pp0->m_PosUV, pp0->m_NormaleUV, ps::GetX(UVlenghtP1P2_P3P4));
        GetInterMinus(_NSPoints[10] ,pp1->m_PosUV, pp1->m_NormaleUV, ps::GetX(UVlenghtP1P2_P3P4));
        _NSPoints[11]  = pp1->m_PosUV;


		_NSPoints[12]  = pp2->m_PosUV;
        GetInterMinus(_NSPoints[13] ,pp2->m_PosUV, pp2->m_NormaleUV, ps::GetY(UVlenghtP1P2_P3P4));
        GetInterPlus( _NSPoints[14] ,pp3->m_PosUV, pp3->m_NormaleUV, ps::GetY(UVlenghtP1P2_P3P4));
        _NSPoints[15]  = pp3->m_PosUV;
#else
        f32     lenghtP1P2, lenghtP3P4;
        lenghtP1P2    = (ppDyn0->m_Pos - ppDyn1->m_Pos).norm()*.5f;
        lenghtP3P4    = (ppDyn2->m_Pos - ppDyn3->m_Pos).norm()*.5f;
        if (_invert)
        {
			lenghtP1P2 = -lenghtP1P2;
			lenghtP3P4 = -lenghtP3P4;
        } 
        GetInterMinus(_NSPoints[1]   , ppDyn0->m_Pos, ppDyn0->m_Normale, lenghtP1P2);
        GetInterPlus(_NSPoints[2]   , ppDyn1->m_Pos,ppDyn1->m_Normale, lenghtP1P2);
        GetInterPlus(_NSPoints[5]   , ppDyn2->m_Pos, ppDyn2->m_Normale, lenghtP3P4);
        GetInterMinus(_NSPoints[6]   , ppDyn3->m_Pos, ppDyn3->m_Normale, lenghtP3P4);

        //independent from Points[0-7]
        lenghtP1P2    = (pp0->m_PosUV - pp1->m_PosUV).norm()*.5f;
        lenghtP3P4    = (pp2->m_PosUV - pp3->m_PosUV).norm()*.5f;
		_NSPoints[8]   = pp0->m_PosUV;
        GetInterPlus(_NSPoints[9]   ,pp0->m_PosUV, pp0->m_NormaleUV,  lenghtP1P2);
        GetInterMinus(_NSPoints[10]  ,pp1->m_PosUV, pp1->m_NormaleUV, lenghtP1P2);
        _NSPoints[11]  = pp1->m_PosUV;


		_NSPoints[12]  = pp2->m_PosUV;
        GetInterMinus(_NSPoints[13]  ,pp2->m_PosUV, pp2->m_NormaleUV, lenghtP3P4);
        GetInterPlus(_NSPoints[14]  ,pp3->m_PosUV, pp3->m_NormaleUV, lenghtP3P4);
        _NSPoints[15]  = pp3->m_PosUV;
#endif


    }

    bbool drawPatchBones(Patch32Data *_pData, Texture *texture, AnimPatchPointDyn *_ppDyn[], AnimPatchPoint  *_pp[], u32 _hdiv, u32 _vdiv, ColorInteger *lightTab, f32 _z, bbool _invert)
    {
        if (lightTab &&
            lightTab[0].getU8Alpha() == 0 &&
            lightTab[1].getU8Alpha() == 0 &&
            lightTab[2].getU8Alpha() == 0 &&
            lightTab[3].getU8Alpha() == 0)
            return bfalse;

		Vec2d   NSPoints[16];
        computePatchControlPoints_Internal(NSPoints, _ppDyn, _pp, _hdiv, _vdiv, _invert);

#ifdef ANIMUSESHADER
        GFX_ADAPTER->shaderDrawPatch32(texture, NSPoints, lightTab ? lightTab : (ColorInteger*)GFX_ADAPTER->getDefault4WhiteColorTab());
#else
        if (lightTab)  
        {
            if (lightTab[0] == lightTab[1] &&
                lightTab[0] == lightTab[2] &&
                lightTab[0] == lightTab[3]) // same colors
            {
                GFX_ADAPTER->drawPatch32PCTOneColor(_pData, texture,NSPoints, _hdiv, _vdiv, lightTab[0].getAsU32(), _z);
            } else
            {
                GFX_ADAPTER->drawPatch32PCT(_pData, texture,NSPoints, _hdiv, _vdiv, lightTab, _z);
            }
        } else
        {
            GFX_ADAPTER->drawPatch32PT(_pData, texture,NSPoints,_hdiv, _vdiv, _z);
        }
#endif
        return btrue;
    }


    ColorInteger * processPatchColors(ColorInteger *resColor, AnimPatchPointDyn* _ppDyn[], SafeArray<LightInfo,2> *plights, bbool useAlpha)
    {
#ifdef USEPCT
        resColor[0]     = COLOR_BLACK;
        resColor[1]     = COLOR_BLACK;
        resColor[2]     = COLOR_BLACK;
        resColor[3]     = COLOR_BLACK;
#else
        if (!useAlpha) return NULL;

        resColor[0]     = COLOR_WHITE;
        resColor[1]     = COLOR_WHITE;
        resColor[2]     = COLOR_WHITE;
        resColor[3]     = COLOR_WHITE;
#endif 
        if (useAlpha)
        {
            resColor[0].m_rgba.m_a = ITFCOLOR_F32_TO_U8(_ppDyn[0]->m_Alpha);
            resColor[1].m_rgba.m_a = ITFCOLOR_F32_TO_U8(_ppDyn[1]->m_Alpha);
            resColor[2].m_rgba.m_a = ITFCOLOR_F32_TO_U8(_ppDyn[2]->m_Alpha);
            resColor[3].m_rgba.m_a = ITFCOLOR_F32_TO_U8(_ppDyn[3]->m_Alpha);
        }

#ifdef USEPCT
        if (plights->size() > 0)
        {
            for (u32 lightIdx=0; lightIdx<plights->size(); ++lightIdx)
            {
                for (u32 j=0; j<patch->m_nbPatchPoints; ++j)
                {
                    for (u32 cIdx=0; cIdx<3; cIdx++)
                    {
                        f32 dst = ((*plights)[lightIdx].m_position - patch->m_AnimPatchPoints[j]->m_Pos).norm();
                        if (dst < (*plights)[lightIdx].m_typeData)
                        {
                            switch ((*plights)[lightIdx].m_type)
                            {
                            case LightInfo::AMBIENT:
                                resColor[j][cIdx] += (*plights)[lightIdx].m_color[cIdx];
                                break;
                            case LightInfo::LINEAR:
                                resColor[j][cIdx] += (1.f - dst / (*plights)[lightIdx].m_typeData) * (*plights)[lightIdx].m_color[cIdx];
                                break;
                            }
                        }
                    }
                }
            }
            for (u32 j=0; j<4; ++j)
            {
                for (u32 cIdx=0; cIdx<4; cIdx++)
                {
                    if (resColor[j][cIdx] > 1.f) resColor[j][cIdx] = 1.f;
                    if (resColor[j][cIdx] < 0.f) resColor[j][cIdx] = 0.f;
                }
            }
        }
#endif
        return resColor;
    }

    Color processGeneralColor(SafeArray<LightInfo,2> *plights, const AABB& _aabb, f32 _posz)
    {
#ifdef USEPCT
        return Color::white();
#else
        // compute global color patch.
        if (plights->size() > 0)
        {
            Color pColor = Color::black();
            u32 number = plights->size();
            f32 nl = 0.f;
            for (u32 lightIdx=0; lightIdx < number; ++lightIdx)
            {
                LightInfo* light = &(*plights)[lightIdx];
                f32 sqDist = 0.f;


                if (light->m_useBV)
                {
                    f32 distx = _aabb.getDistMinPosAABB(light->m_position.truncateTo2D());
                    f32 disty = _posz - light->m_position.m_z;
                    sqDist = f32_Abs(Vec2d(distx, disty).norm());
                }
                else
                {
                    Vec3d Pos = _aabb.getCenter().to3d( _posz );
                    sqDist = f32_Abs(Vec3d(light->m_position - Pos).norm());
                }

                if (sqDist < light->m_radFar)
                {
                    switch (light->m_type)
                    {
                    case LightInfo::AMBIENT:
                        pColor += light->m_color;
                        break;
                    case LightInfo::LINEAR:
                        if (sqDist < light->m_radNear)
                        {
                            pColor += light->m_color;
                            nl = 1.f;
                        }
                        else
                        {
                            //pColor[cIdx] += (1.f - (sqDist - light->m_radNear) / (light->m_radFar - light->m_radNear)) * light->m_color[cIdx];
                            f32 lerp =  1.f - (sqDist - light->m_radNear) / (light->m_radFar - light->m_radNear);
                            nl += lerp;

                            pColor += light->m_color * lerp;
                        }
                        break;
                    }
                }
            }
            
            if (nl > 1.f)
                nl = 1.f;

            //lerp final
            Color one(1.f, 1.f, 1.f, 1.f);
            Color co = Color::colorBlend(&pColor, &one, nl);
            pColor = co;

            pColor.setAlpha(1.f);
            pColor.ClampColor();
            return pColor;
        } 
        else
        {
            return Color::white();
        }
#endif
    }

    AnimDraw::AnimDraw() :
      m_isImpostor(bfalse)
    , m_renderTargetImpostor(NULL)
    , m_width(256)
    , m_height(256)
    , m_globalMaterialBlendMode(GFX_BLEND_ALPHA)
    , m_globalMaterialMatType(GFX_MAT_DEFAULT)
    {
    }

    AnimDraw::~AnimDraw()
    {
        if (m_renderTargetImpostor)
        {
            GFX_ADAPTER->deleteRenderTarget(m_renderTargetImpostor);
            m_renderTargetImpostor = NULL;
        }
    }

    void AnimDraw::computePatchControlPoints(Vec2d *_NSPoints, AnimPatchPointDyn *_ppDyn[], AnimPatchPoint  *_pp[], u32 _hdiv, u32 _vdiv, bbool _invert)
    {
        computePatchControlPoints_Internal(_NSPoints, _ppDyn, _pp, _hdiv, _vdiv, _invert);
    }

    bbool AnimDraw::checkIfImpostorSizeMustChange()
    {
        if (m_isImpostor)
        {
            //We now support size change - this is for UHD screenshots            
            u32 w = m_renderTargetImpostor->getTexture()->m_datasizeX;
            u32 h = m_renderTargetImpostor->getTexture()->m_datasizeY;
            //
            u32 sx = m_width, sy = m_height;
#ifndef ITF_WII
            //UHD screenshot : max size
            if (GFX_ADAPTER->isDrawingForScreenshot())
            {
                u32 maxTexSize = 4096;
                if (sx<maxTexSize && sy<maxTexSize)
                {
                    if (sx>sy)
                    {
                        sy = (maxTexSize*sy)/sx;
                        sx = maxTexSize;
                    }
                    else
                    {
                        sx = (maxTexSize*sx)/sy;
                        sy = maxTexSize;
                    }
                }
            }
#endif // ITF_WII
            if (w!=sx || h!=sy)
            {
                LOG("Recreating impostor because of wrong size");
                Size m_size(sx,sy );
                GFX_ADAPTER->resizeRenderTarget(m_renderTargetImpostor, m_size);
                return btrue;
            }
        }
        return bfalse;
    }
    Texture* AnimDraw::setImpostor(u32 _sx, u32 _sy)
    {
        //
        if (m_isImpostor)
        {
            ITF_ASSERT(m_renderTargetImpostor);
            return m_renderTargetImpostor->getTexture();
        }

        m_isImpostor = btrue;

        if (m_renderTargetImpostor)
            GFX_ADAPTER->deleteRenderTarget(m_renderTargetImpostor);

        m_renderTargetImpostor = GFX_ADAPTER->getNewRenderTarget();
        m_renderTargetImpostor->create((f32)_sx, (f32)_sy);
        m_width = _sx;
        m_height = _sy;

        GFX_ADAPTER->clearRenderTarget(m_renderTargetImpostor, m_renderTargetImpostor->getBackGroundColor());

        return m_renderTargetImpostor->getTexture();
    }

    void AnimDraw::setGlobalMaterialParams(u32 _flags, GFX_MATERIAL_TYPE _mattype, GFX_BLENDMODE _blendmode)
    {
        m_globalMaterialMatType = _mattype;
        m_globalMaterialBlendMode = _blendmode;
    }

    bbool AnimDraw::Draw(AnimMeshScene* _animMeshScene)
    {
        AnimInfo        *pAnimInfo = &_animMeshScene->m_AnimInfo;
        ImpostorMode    impostor;
        u32             renderpass              = 0;

#if defined(ANIMUSEGEOMETRYSHADER)
        int             vertexFormat            = VertexFormat_PatchAnim;
#else
        int             vertexFormat            = pAnimInfo->m_vertexFormat;
#endif
        
        u32             hDiv                    = pAnimInfo->m_hDiv;
        u32             vDiv                    = pAnimInfo->m_vDiv;

        if (m_isImpostor)
        {
            TIMERASTER_SCOPE(AnimDraw_ImpostorProcessPatch);

            impostor.start(pAnimInfo, m_renderTargetImpostor);
            // need reprocess patches ...
            _animMeshScene->m_pAnimPatches.processPatchs(_animMeshScene, bfalse); // -> onWorking is false because syncro already done here
        }

        SafeArray<AnimPatchZ> * allPatches  = &_animMeshScene->m_pAnimPatches.m_allPatchs;
        u32                     patchBegin  = 0;
        u32                     patchEnd   = allPatches->size();

        if (pAnimInfo->m_renderPassFrontierPatchIndex != U32_INVALID)
        {
            ITF_ASSERT_CRASH(patchBegin <= pAnimInfo->m_renderPassFrontierPatchIndex && 
                             pAnimInfo->m_renderPassFrontierPatchIndex <= patchEnd, "renderPassFrontierPatchIndex incorrect !");
            if (pAnimInfo->m_renderPass == 0)
                patchEnd   = pAnimInfo->m_renderPassFrontierPatchIndex;
            else
                patchBegin  = pAnimInfo->m_renderPassFrontierPatchIndex;
        }

        if (patchBegin >= patchEnd)
            return bfalse;

        for (u32 p=patchBegin; p<patchEnd; p++)
        {
            AnimPatch   *patch  =(*allPatches)[p].m_patch;
            ITF_ASSERT(patch &&  patch->m_PatchBankPtr);
            if (!patch || !patch->m_PatchBankPtr) // TODO Yousri
                return bfalse;
            Texture     *pTex   = (Texture *)patch->m_PatchBankPtr->m_texture.getResource();
            if ((!pTex) || (!pTex->isPhysicallyLoaded()))
                return bfalse;
        }

        ITF_ASSERT(_animMeshScene->m_pMesh[0]->m_ElementList.size() > 0);

        Color globalColor = processGeneralColor(&pAnimInfo->m_lights, pAnimInfo->m_visualAABB, pAnimInfo->m_Pos.m_z);
        globalColor.MulAdd(globalColor,pAnimInfo->m_color,pAnimInfo->m_selfIllumColor);
		globalColor.Min(1.f);
        GFX_ADAPTER->setFogColor(pAnimInfo->m_fogColor);
        GFX_ADAPTER->setGlobalColor(globalColor);


        GFX_RECT    rcScissor;
        if (pAnimInfo->m_scissor.isValid())
        {
            Vec3d       projectedMin;
            Vec3d       projectedMax;
            GFX_ADAPTER->compute3DTo2D(pAnimInfo->m_scissor.getMin().to3d(pAnimInfo->m_Pos.m_z), projectedMin);
            GFX_ADAPTER->compute3DTo2D(pAnimInfo->m_scissor.getMax().to3d(pAnimInfo->m_Pos.m_z), projectedMax);

            rcScissor.left      = (i32)projectedMin.m_x;
            rcScissor.right     = (i32)projectedMax.m_x;
            if (projectedMin.m_y < projectedMax.m_y)
            {
                rcScissor.top       = (i32)projectedMin.m_y;
                rcScissor.bottom    = (i32)projectedMax.m_y;
            } else
            {
                rcScissor.top       = (i32)projectedMax.m_y;
                rcScissor.bottom    = (i32)projectedMin.m_y;
            }

            rcScissor.left   = Clamp(rcScissor.left,  0, (i32)GFX_ADAPTER->getScreenWidth());
            rcScissor.right  = Clamp(rcScissor.right, 0, (i32)GFX_ADAPTER->getScreenWidth());
            rcScissor.top    = Clamp(rcScissor.top,   0, (i32)GFX_ADAPTER->getScreenHeight());
            rcScissor.bottom = Clamp(rcScissor.bottom,0, (i32)GFX_ADAPTER->getScreenHeight());

            GFX_ADAPTER->setScissorRect(&rcScissor);
        }

        Patch32Data*    pData       = ANIM_MANAGER->getPatchDataBuffer();

        // material info.
        /// test Glow
        //_animMeshScene->m_pMesh[0]->m_ElementList[0].m_material.m_matType = GFX_MAT_GLOW;
        //_animMeshScene->m_pMesh[0]->m_ElementList[0].m_material.m_matParams.setfParamsAt(0, 100.f);

#if defined(ANIMUSESHADER)
        {
            TIMERASTER_SCOPE(AnimDraw_PreparePatch);

            GFX_ADAPTER->prepareShaderPatch(&_animMeshScene->m_pMesh[0]->getMatrix(), pAnimInfo->m_Pos.m_z, hDiv, vDiv);
            GFX_ADAPTER->setGfxMaterial(_animMeshScene->m_pMesh[0]->m_ElementList[0].m_material);
            pData->Init(_animMeshScene->m_pMesh[0], vertexFormat);
#if PATCH32_WORKINGBUFF == 0
            pData->vertexListPT = (VertexPT*)pData->LockVBuff();
#endif // PATCH32_WORKINGBUFF
        }
        u32 patchCount = 0;
#else // ANIMUSESHADER
        u32 curMesh         = 0;
        {
            TIMERASTER_SCOPE(AnimDraw_PreparePatch);

            pData->Init(_animMeshScene->m_pMesh[curMesh++], vertexFormat);
#if PATCH32_WORKINGBUFF == 0
            pData->vertexListPT = (VertexPT*)pData->LockVBuff();
#endif // PATCH32_WORKINGBUFF
        }
#endif // ANIMUSESHADER

        pData->mesh->m_globalColor = globalColor;
        pData->mesh->m_ElementList[0].m_material.m_matType = m_globalMaterialMatType;
        pData->mesh->m_ElementList[0].m_material.m_blendMode = m_globalMaterialBlendMode;

        for (u32 p=patchBegin; p<patchEnd; p++)
        {
            AnimPatch   *patch =(*allPatches)[p].m_patch;
            if (pData->mesh->m_ElementList[0].m_material.m_textureDiffuse.isValidResourceId() &&
                pData->mesh->m_ElementList[0].m_material.m_textureDiffuse != patch->m_PatchBankPtr->m_texture)
            {
                TIMERASTER_SCOPE(AnimDraw_patchToMesh);
#ifndef ANIMUSESHADER
#if PATCH32_WORKINGBUFF != 0
                pData->CopyToMesh();
#else
                pData->UnlockVBuff();
#endif // PATCH32_WORKINGBUFF
                if (curMesh >= _animMeshScene->m_pMesh.size())
                    if (!_animMeshScene->AllocAddMesh())
                    {
                        ITF_FATAL_ERROR("[ANIM] Too many textures or bad ordering on character path %s", /*m_GroupName.cStr()*/ "TODO");
                        exit(0);
                    }
                // use same mesh matrix
                _animMeshScene->m_pMesh[curMesh]->setMatrix(_animMeshScene->m_pMesh[0]->getMatrix());
                pData->Init(_animMeshScene->m_pMesh[curMesh++], vertexFormat);

#else
                GFX_ADAPTER->shaderPatchFlush(hDiv, vDiv, renderpass);
                GFX_ADAPTER->endShaderPatch();
                renderpass = 0;
                GFX_ADAPTER->prepareShaderPatch(&_animMeshScene->m_pMesh[0]->getMatrix(), pAnimInfo->m_Pos.m_z, hDiv, vDiv);
                pData->Init(_animMeshScene->m_pMesh[0], vertexFormat);
#endif
#if PATCH32_WORKINGBUFF == 0
                pData->vertexListPT = (VertexPT*)pData->LockVBuff();
#endif // PATCH32_WORKINGBUFF

                pData->mesh->m_globalColor = globalColor;
                pData->mesh->m_ElementList[0].m_material.m_matType = m_globalMaterialMatType;
                pData->mesh->m_ElementList[0].m_material.m_blendMode = m_globalMaterialBlendMode;
            }
            Texture             *pTexture = (Texture *)patch->m_PatchBankPtr->m_texture.getResource();
            AnimTemplate *      pTemplate = patch->m_PatchBankPtr->m_templateList[patch->m_TemplateIdx];
#ifndef ITF_SUPPORT_MULTITHREADED
            if (!pTemplate->IsBoneComputedOK())
                continue;
#endif
            AnimPatchPointDyn   *ppDyn = pAnimInfo->getTemplatePatchPointBuffer(pTemplate, bfalse);
            pData->mesh->m_ElementList[0].m_material.m_textureDiffuse   = patch->m_PatchBankPtr->m_texture;
            
            AnimPatchPointDyn   *ppDynTab[4];
            AnimPatchPoint      *ppTab[4];

            if (patch->m_nbPatchPoints == 4)
            {
                ppTab[0] = patch->m_AnimPatchPoints[1];
                ppTab[1] = patch->m_AnimPatchPoints[3];
                ppTab[2] = patch->m_AnimPatchPoints[0];
                ppTab[3] = patch->m_AnimPatchPoints[2];
            } else
            {
                ITF_ASSERT(patch->m_nbPatchPoints == 2);
                ppTab[0] = patch->m_AnimPatchPoints[0];
                ppTab[1] = patch->m_AnimPatchPoints[1];
                ppTab[2] = patch->m_AnimPatchPoints[1];
                ppTab[3] = patch->m_AnimPatchPoints[0];
            }

            ppDynTab[0] = &ppDyn[ppTab[0]->m_Index];
            ppDynTab[1] = &ppDyn[ppTab[1]->m_Index];
            ppDynTab[2] = &ppDyn[ppTab[2]->m_Index];
            ppDynTab[3] = &ppDyn[ppTab[3]->m_Index];


#if defined(ANIMUSEGEOMETRYSHADER)
            appendPatchData(pData, ppDynTab, ppTab, pTexture);

#else
            bbool drawxResult;
            {
                TIMERASTER_SCOPE(AnimDraw_drawPatchBones);
                drawxResult = drawPatchBones(pData, pTexture,
                    ppDynTab,
                    ppTab,
                    hDiv, vDiv,
                    processPatchColors(m_colorPatchTab, ppDynTab, &pAnimInfo->m_lights,vertexFormat == VertexFormat_PCT),
                    pAnimInfo->m_Pos.m_z, pAnimInfo->m_globalData.m_mirror != ppDynTab[0]->m_Mirror);
            }
#endif
            
#ifdef ANIMUSESHADER    
            if (drawxResult)
            {
                patchCount++;
                if (patchCount >= NB_ELEMENT_IN_VERTEX_BUFFER)
                {
                    TIMERASTER_SCOPE(AnimDraw_patchToMesh);
                    GFX_ADAPTER->shaderPatchFlush(hDiv, vDiv, renderpass++);
                    patchCount = 0;
                }
            }
#endif
        }

        TIMERASTER_SCOPE(AnimDraw_patchToMesh);
#if defined(ANIMUSESHADER)
        GFX_ADAPTER->shaderPatchFlush(hDiv, vDiv, renderpass);
        GFX_ADAPTER->endShaderPatch();
        GFX_ADAPTER->postGfxMaterial(_animMeshScene->m_pMesh[0]->m_ElementList[0].m_material);

#elif defined(ANIMUSEGEOMETRYSHADER)
#if PATCH32_WORKINGBUFF != 0
        pData->CopyToMesh();
#else        
        pData->UnlockVBuff();
#endif // PATCH32_WORKINGBUFF

        if (GFX_ADAPTER->prepareGeomShaderPatch(&_animMeshScene->m_pMesh[0]->getMatrix(), pAnimInfo->m_finalBoneList, pAnimInfo->m_Pos.m_z, hDiv, vDiv, pAnimInfo->m_globalData.m_mirror))
        {
            for (u32 i=0; i<curMesh; i++)
                GFX_ADAPTER->drawAnimatedPatches(*_animMeshScene->m_pMesh[i]);

            GFX_ADAPTER->endGeomShaderPatch();
        }
#else
#if PATCH32_WORKINGBUFF != 0
        pData->CopyToMesh();
#else        
        pData->UnlockVBuff();
#endif // PATCH32_WORKINGBUFF

        if (vertexFormat == VertexFormat_PCT)
        {
            for (u32 i=0; i<curMesh; i++)
                GFX_ADAPTER->drawMesh(*_animMeshScene->m_pMesh[i], false);
        }
        else
        {
            for (u32 i=0; i<curMesh; i++)
                GFX_ADAPTER->drawMeshPT(*_animMeshScene->m_pMesh[i], false);
        }

        #ifdef ITF_WII
        // Pass for separate alpha
        if (m_isImpostor)
        {
            GFX_ADAPTER_WII->startImpostorAlphaRendering(m_renderTargetImpostor);
            if (vertexFormat == VertexFormat_PCT)
            {
                for (u32 i=0; i<curMesh; i++)
                    GFX_ADAPTER->drawMesh(*_animMeshScene->m_pMesh[i], false);
            }
            else
            {
                for (u32 i=0; i<curMesh; i++)
                    GFX_ADAPTER->drawMeshPT(*_animMeshScene->m_pMesh[i], false);
            }
        }
        #endif // ITF_WII
#endif
		
        GFX_ADAPTER->setGlobalColor(0xffffffff);
        GFX_ADAPTER->setFogColor(0);

        /* draw debug bones
        SafeArray<AnimBoneDyn> & boneDynList = pAnimInfo->getCurrentBoneList();
        u32 bCount = boneDynList.size();
        for (u32 bIdx = 0; bIdx < bCount; bIdx++)
        {
            const AnimBoneDyn& bone = boneDynList[bIdx];
            GFX_ADAPTER->drawDBGLine(bone.m_Pos, bone.m_PosEnd, 1, 0, 0);
        }
        */

        if (pAnimInfo->m_scissor.isValid())
        {
            rcScissor.left= 0;
            rcScissor.top = 0;
            rcScissor.right = GFX_ADAPTER->getScreenWidth();
            rcScissor.bottom = GFX_ADAPTER->getScreenHeight();
            GFX_ADAPTER->setScissorRect(&rcScissor);
        }

        return btrue;
    }



} // namespace ITF


