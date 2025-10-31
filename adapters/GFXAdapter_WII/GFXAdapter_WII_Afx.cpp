#include "precompiled_GFXAdapter_WII.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/mtx.h>

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

namespace ITF
{

///----------------------------------------------------------------------------//

u32	GFXAdapter_WII::getAFXBufferSize()
{
    ITF_ASSERT(Rmode);
	return 2 * GXGetTexBufferSize(Rmode->fbWidth, Rmode->efbHeight, PixFormat_TO_GXTexFmt(Texture::PF_RGBA), GX_FALSE, 0);
}

u8* GFXAdapter_WII::lockAFXBuffer(u32 _size)
{
    if(!mp_AFX_RT[0] || mb_AFXBfrLocked)
        return NULL;
    
    if(_size > getAFXBufferSize())
        return NULL;

    mb_AFXBfrLocked = btrue;

	// SyncGPU: the CPU waits until the GPU has finished using the buffer.
	syncGPU();
    return (u8*) mp_AFX_RT[0]->getTexture()->m_adapterimplementationData;
}

void GFXAdapter_WII::unlockAFXBuffer()
{
	ITF_ASSERT(mb_AFXBfrLocked);
	// DCFlushRange: the CPU flushes all data in cache to RAM before letting the GPU use the buffer.
	DCFlushRange((void *)mp_AFX_RT[0]->getTexture()->m_adapterimplementationData, getAFXBufferSize());
    mb_AFXBfrLocked = bfalse;
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_startRenderTarget(Color _color)
{
    if(mb_AFXBfrLocked) return;
    
    if(!mp_AFX_RT[0])
    {
        //Create Full RenderTarget size
        f32 w = mv_StaticUtilScreenSize.m_x;
        f32 h = mv_StaticUtilScreenSize.m_y;

        // Don't use mv_StaticUtilScreenSize for alloc to have same buffer size in 4/3 than in 16/9 (for other use like HBM)
		u32 size = getAFXBufferSize();
        u8 *buffer = (u8 *)ITF::Memory::alignedMallocCategory(size, 32, MemoryId::mId_GfxAdapter);

        for(u32 i=0 ; i<AFX_MAX_FULL_RT ; ++i)
        {
            ITF_ASSERT(!mp_AFX_RT[i]);
            mp_AFX_RT[i] = getNewRenderTarget();
			mp_AFX_RT[i]->m_area[0] = 0;
			mp_AFX_RT[i]->m_area[1] = 0;
			mp_AFX_RT[i]->m_area[2] = w;
			mp_AFX_RT[i]->m_area[3] = h;

			Texture* p_Tex = mp_AFX_RT[i]->getTexture();
			p_Tex->m_adapterimplementationData = (u32) buffer;
			initializeDynamicTexture(p_Tex, w, h, Texture::PF_RGBA);

			buffer += GXGetTexBufferSize(w, h, PixFormat_TO_GXTexFmt(Texture::PF_RGBA), GX_FALSE, 0);
        }

        // Create RenderTarget of middle size using same buffer as full one
        u16 w2 = ((u16)w) >> 1;
        u16 h2 = ((u16)h) >> 1;
        u8 * refBfr = (u8 *) mp_AFX_RT[AFX_FB]->getTexture()->m_adapterimplementationData;
        for(u32 i=AFX_MAX_FULL_RT ; i<AFX_MAX_RT ; ++i)
        {
            ITF_ASSERT(!mp_AFX_RT[i]);
            mp_AFX_RT[i] = getNewRenderTarget();
            mp_AFX_RT[i]->m_area[0] = 0;
            mp_AFX_RT[i]->m_area[1] = 0;
            mp_AFX_RT[i]->m_area[2] = w2;
            mp_AFX_RT[i]->m_area[3] = h2;

            Texture* p_Tex = mp_AFX_RT[i]->getTexture();
            p_Tex->m_adapterimplementationData = (u32) refBfr;
            initializeDynamicTexture(p_Tex, w2, h2, Texture::PF_RGBA);

            refBfr += GXGetTexBufferSize(w2, h2, PixFormat_TO_GXTexFmt(Texture::PF_RGBA), GX_FALSE, 0);
        }
    }

    clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, _color.getAsU32());

    // Copy frame buffer
    copyCurrentColorBuffer(AFX_FB);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::prepareAfterFx(bbool _nocopy)
{
    init2DRender();
    if(_nocopy || mb_AFXBfrLocked) return;

    ITF_ASSERT(mp_AFX_RT[AFX_RT]);
    if(isUseInternalRT())
    {
        // NOTHING
    }
    else
    {
        // Copy frame buffer
        clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, COLOR_ZERO);
        copyCurrentColorBuffer(AFX_RT);
        m_AFXCurRT = AFX_RT;
    }

    // Disable FakeAA for AFX because it's already done with other objects render before
    // and get in AFX_FB texture
    forceDisableFakeAA(btrue);
}

void GFXAdapter_WII::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
{
    if(!(_nocopy || mb_AFXBfrLocked))
    {
        setGlobalColor(Color::white());
        disableFog();
        resetUVAnim();

        setCullMode(GFX_CULL_NONE);
        depthTest(0);
        depthMask(0);

        ITF_ASSERT(mp_AFX_RT[AFX_FB] && mp_AFX_RT[AFX_RT]);
        setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
        SetTexture(0, mp_AFX_RT[m_AFXCurRT]->getTexture());

        switch(_finalBlend)
        {
            case GFX_BLEND_COPY:
                setAlphaBlend(GFX_BLEND_COPY);
                drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);
                break;
            
            case GFX_BLEND_MUL2X:
                if(isUseInternalRT())
                {
                    setAlphaBlend(GFX_BLEND_MUL2X);
                    drawScreenQuadInternal(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE, bfalse, TEV_T);
                }
                else
                {
                    setAlphaBlend(GFX_BLEND_COPY);
                    SetTexture(2, mp_AFX_RT[AFX_FB]->getTexture());
                    drawScreenQuadInternal(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE, bfalse, TEV_AFX_2X);
                }
                break;
            
            default:
                ITF_ASSERT_MSG(0, "Blend mode (%d) not supported for AFX", (u32)_finalBlend);
                break;
        }

        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        SetTextureBind(0,0);
        setAlphaBlend(GFX_BLEND_ALPHA);

        // Restore FakeAA status
        forceDisableFakeAA(bfalse);
    }

    restoreDefaultShader();
    resetUVAnim();

    CAMERA->apply();
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::prepareInternalRT()
{
    ITF_ASSERT(mp_AFX_RT[AFX_RT]);
    #ifdef SUPPORT_STATS_MANAGER
    StatsManager::getptr()->renderTargetInc();
    #endif

    // Disable FakeAA for RT creation, the effect will be apply with the used of the RT
    forceDisableFakeAA(btrue);

    mp_AFX_RT[AFX_RT]->set();
    m_AFXCurRT = AFX_RT;

    // Clear
    init2DRender();
    setAlphaBlend(GFX_BLEND_COPY);
    drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, m_internRTColor.getAsU32(), btrue);

    // Restore Projection
    CAMERA->apply();
}

void GFXAdapter_WII::endInternalRT()
{
    ITF_ASSERT(mp_AFX_RT[AFX_RT]);
    mp_AFX_RT[AFX_RT]->unSet();

    // Restore FakeAA status
    forceDisableFakeAA(bfalse);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_PrepareDrawOne(AFX_RT_ID _RT, f32 _W, f32 _H, GFX_RECT *_saveViewport)
{
    ITF_ASSERT(mp_AFX_RT[AFX_FB] && mp_AFX_RT[AFX_RT]);

    // Save viewport
    getViewport(_saveViewport);

    // Init settings
    setCullMode(GFX_CULL_NONE);
    depthTest(0);
    depthMask(0);
    setAlphaBlend(GFX_BLEND_COPY);
    setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);

    // Config screen with RT
    GFX_RECT viewport;
    viewport.left   = mp_AFX_RT[_RT]->m_area[0];
    viewport.top    = mp_AFX_RT[_RT]->m_area[1];
    viewport.right  = viewport.left + mp_AFX_RT[_RT]->m_area[2];
    viewport.bottom = viewport.top  + mp_AFX_RT[_RT]->m_area[3];
    GXSetViewport(0.f, 0.f, _W, _H, -1.f, 1.f);
    GXSetScissor(0, 0, (u32)_W, (u32)_H);

    setOrthoView(0.f, _W, 0.f, _H);
}

void GFXAdapter_WII::AFTERFX_EndDrawOne(AFX_RT_ID _RT, GFX_RECT *_saveViewport)
{
    // Restore viewport
    init2DRender();
    setupViewport(_saveViewport);

    m_AFXCurRT = _RT;
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_draw(AFX_Group* _group)
{
    prepareAfterFx(bfalse);

    u32 size = _group->getNumberFx();
    for (u32 i = 0; i< size ;i++)
    {
        AFTERFX* afx = _group->getFxAt(i);
        AFTERFX_drawOne(afx);
    }

    endAfterfx(bfalse, (GFX_BLENDMODE)_group->getFinalBlend());
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
{
    ITF_ASSERT(mp_AFX_RT[AFX_FB] && mp_AFX_RT[AFX_RT]);

    // RT
    AFX_RT_ID   srcRT = m_AFXCurRT;
    AFX_RT_ID   dstRT = AFX_PART_RT_1;

    // Area size
    f32 W = (f32)(mp_AFX_RT[dstRT]->m_area[2] - mp_AFX_RT[dstRT]->m_area[0]);
    f32 H = (f32)(mp_AFX_RT[dstRT]->m_area[3] - mp_AFX_RT[dstRT]->m_area[1]);

    // Init
    GFX_RECT saveViewport;
    AFTERFX_PrepareDrawOne(dstRT, W, H, &saveViewport);

    // Blur
    Mtx tm0,tm1,tm2,tm3;
    f32 displace    = 0.001f * _factor;
    f32 ratiopixel  = 1.f * f32_Inv((f32)getScreenWidth());
    u32 passo       = 0;
    while (displace > ratiopixel)
    {
        mp_AFX_RT[dstRT]->set();

        // Each texture coordinates is a shift of the blur factor
        MTXTrans(tm0,   displace,        0.f,  0.f);
        MTXTrans(tm1,        0.f, - displace,  0.f);
        MTXTrans(tm2, - displace,        0.f,  0.f);
        MTXTrans(tm3,        0.f,   displace,  0.f);

        // Set texture coordinates
        GXLoadTexMtxImm(tm0, GX_TEXMTX0, GX_MTX2x4);
        GXLoadTexMtxImm(tm1, GX_TEXMTX1, GX_MTX2x4);
        GXLoadTexMtxImm(tm2, GX_TEXMTX2, GX_MTX2x4);
        GXLoadTexMtxImm(tm3, GX_TEXMTX3, GX_MTX2x4);

        // Draw Quad
        SetTexture(0, mp_AFX_RT[srcRT]->getTexture());
        drawScreenQuadInternal(0.f, 0.f, W, H, 0.f, COLOR_WHITE, bfalse, TEV_AFX_BIGBLUR);

        mp_AFX_RT[dstRT]->unSet();

        ++passo;
        if (_MaxPass && passo > _MaxPass) 
            break;

        displace *= 0.5f;

        // Swap RT
        if(dstRT == AFX_PART_RT_1)
        {
            srcRT = AFX_PART_RT_1;
            dstRT = AFX_PART_RT_2;
        }
        else
        {
            srcRT = AFX_PART_RT_2;
            dstRT = AFX_PART_RT_1;
        }
    }

    // Restore
    AFTERFX_EndDrawOne(dstRT, &saveViewport);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_Remanence not implemented");
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_RayCenter not implemented");
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode)
{
    ITF_ASSERT(mp_AFX_RT[AFX_FB] && mp_AFX_RT[AFX_RT]);

    // Copy frame buffer
    clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, COLOR_ZERO);
    copyCurrentColorBuffer(AFX_RT);
    m_AFXCurRT = AFX_RT;

    // Disable FakeAA for AFX because it's already done with other objects render before
    // and get in AFX_FB texture
    forceDisableFakeAA(btrue);

    // Save scene texture
    AFX_RT_ID   srcRT = m_AFXCurRT;

    // Apply Blur
    AFTERFX_BigBlur(_factor, 0, btrue, GFX_QUALITY_MEDIUM);

    // Apply Glow to blur result and add initial texture
    // srcRT = original scene
    // m_AFXCurRT = blur result

    // Init settings
    setCullMode(GFX_CULL_NONE);
    depthTest(0);
    depthMask(0);
    setAlphaBlend(GFX_BLEND_COPY);
    setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);

    // Factor
    Color col(Color::white());
    col.m_a = _addalpha * _glowfactor * 0.5f;

    // Draw Quad
    SetTexture(0, mp_AFX_RT[srcRT]->getTexture());
    SetTexture(2, mp_AFX_RT[m_AFXCurRT]->getTexture());
    switch(_finalBlendMode)
    {
    case GFX_BLEND_COPY:
        drawScreenQuadInternal(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, col.getAsU32(), bfalse, TEV_AFX_GLOW_COPY);
        break;

    case GFX_BLEND_MUL2X:
        drawScreenQuadInternal(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, col.getAsU32(), bfalse, TEV_AFX_GLOW_2X);
        break;

    default:
        ITF_ASSERT_MSG(0, "Blend mode (%d) not supported for AFX", (u32)_finalBlendMode);
        break;
    }

    // Restore FakeAA status
    forceDisableFakeAA(bfalse);
}

void GFXAdapter_WII::AFTERFX_objectsGlow(f32 _factor)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_objectsGlow not implemented");
}

void GFXAdapter_WII::AFTERFX_prepareObjectGlow()
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_prepareObjectGlow not implemented");
}

void GFXAdapter_WII::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_ColorSetting not implemented");
}

void GFXAdapter_WII::AFTERFX_ColorRemap(Texture* _tex)
{
    ITF_ASSERT(mp_AFX_RT[AFX_FB] && mp_AFX_RT[AFX_RT]);

    if(_tex)
    {
        ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_ColorRemap not implemented");
    }
    else
    {
        // NOTHING
    }
}

void GFXAdapter_WII::AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_ColorLevels not implemented");
}

void GFXAdapter_WII::AFTERFX_addSceneAndMul(f32 _mul)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_addSceneAndMul not implemented");
}   

void GFXAdapter_WII::AFTERFX_byTech(u32 _tech)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_byTech not implemented");
}

void GFXAdapter_WII::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::AFTERFX_byTechParams not implemented");
}

} // namespace ITF
