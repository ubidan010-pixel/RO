#include "precompiled_GFXAdapter_GCM.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_
 
#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#include "engine/rasters/RastersManager.h"

#include "GFXAdapter_GCM.h"
#include "Surface_GCM.h"
#include "Texture_GCM.h"

///////////////////////////////////////////////////////////////////////////////
// AfterFX methods
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{
    DECLARE_RASTER(afterFX, RasterGroup_Misc, Color::violet());

    void GFXAdapter_GCM::AFTERFX_startRenderTarget(Color _color)
    {
        /// already set ?
        if (m_currentRenderTarget == m_pFullRTsurf[0]) return;

        ITF_ASSERT(m_pFullRTtex[0]);
        ITF_ASSERT(m_pFullRTtex[1]);

        setRenderTarget(m_pFullRTsurf[1]);
        clear(GFX_CLEAR_COLOR, _color.getRed(), _color.getGreen(), _color.getBlue(), _color.getAlpha()); 
        setDefaultAlphaTest();
    }

    void GFXAdapter_GCM::AFTERFX_zoomCenter(f32 _factor, Vec3d _dir)
    {
        IMPLEMENTED_NOT_TESTED;
        ITF_ASSERT(0); // NI
    }

    void GFXAdapter_GCM::AFTERFX_prepareObjectGlow()
    {
        // Not supposed to be used -> DRWF_COPYTOTARGET flag is not always used
        IMPLEMENTED_NOT_TESTED;
        AFTERFX_PrepareSwapRTDown4x4();
    }

    void GFXAdapter_GCM::AFTERFX_ColorRemap(ITF::Texture * _tex)
    {
        IMPLEMENTED_NOT_TESTED;
        // setColor Ramp to shader custom texture.
        if (_tex)
        {
            SetTextureBind(3, (u32)_tex->m_adapterimplementationData, 0);
            setSamplerFilterState(3, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);
        }
        else 
            return;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 9;
        setRenderTarget( m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setAlphaBlend(GFX_BLEND_COPY);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    void GFXAdapter_GCM::prepareInternalRT()
    {
        ITF_ASSERT(m_pFullRTtex[1]);

        setRenderTarget(m_pFullRTsurf[1]);
        clear(GFX_CLEAR_COLOR, m_internRTColor.getRed(), m_internRTColor.getGreen(), m_internRTColor.getBlue(), m_internRTColor.getAlpha()); 
    }

    void GFXAdapter_GCM::endInternalRT()
    {
        setRenderTarget(m_frameBuffers[m_backBufferIndex]);
    }

    void GFXAdapter_GCM::prepareAfterFx(bbool _nocopy)
    {
        m_worldViewProj.push();

        init2DRender();

        if (_nocopy)
        {
            if (m_currentRenderTarget == m_frameBuffers[m_backBufferIndex])
            {
                // set the back buffer as source and dest (after fx job to not used it at the same time)
                m_pCurrentSwapRenderSurf = m_frameBuffers[m_backBufferIndex];
                m_pCurrentSwapSourceTexture = m_frameBuffersAsTex[m_backBufferIndex];
            }
            return;
        }

        setShader(mp_shaderManager.getShaderByIndex(1));
        setAlphaRef(1);

        ITF_ASSERT(m_pFullRTtex[1]);

        // if the current rendering target is the backbuffer then resolve it to texture.
        // to the m_pFullRTtex[0].
        if (m_currentRenderTarget == m_frameBuffers[m_backBufferIndex])
        {
            copyCurrentColorBuffer(0);
            setRenderTarget(m_pFullRTsurf[1]);
            m_pCurrentSwapSourceTexture = m_pFullRTtex[0];
            m_pCurrentSwapRenderSurf = m_pFullRTsurf[1];
        }
        else if (m_currentRenderTarget == m_pFullRTsurf[1])
        {
            setRenderTarget(m_pFullRTsurf[0]);
            //clear( GFX_CLEAR_COLOR , 0, 0, 0, 0);  // Is it really useful ?
            m_pCurrentSwapSourceTexture = m_pFullRTtex[1];
            m_pCurrentSwapRenderSurf = m_pFullRTsurf[0];
        }
    }

    void GFXAdapter_GCM::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
    {
        // restore default shader renderer.
        setShader(mp_defaultShader);

        if (!_nocopy)
        {
            setRenderTarget(m_frameBuffers[m_backBufferIndex]);

            GFX_RECT Viewport;
            Viewport.left = 0;
            Viewport.top = 0;
            Viewport.right = i32(getScreenWidth());
            Viewport.bottom = i32(getScreenHeight());
            setupViewport(&Viewport);

            SetTextureBind(0,(u32)m_pCurrentSwapSourceTexture);
            setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);
            setAlphaBlend(_finalBlend);
            drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(),0, COLOR_WHITE );

            setAlphaBlend(GFX_BLEND_ALPHA);
        }

        m_worldViewProj.pop();

        CAMERA->apply();
    }

    void GFXAdapter_GCM::AFTERFX_draw(AFX_Group* _group)
    {
        IMPLEMENTED_NOT_TESTED;

        TIMERASTER_SCOPE(afterFX);

        prepareAfterFx(bfalse);

        u32 size = _group->getNumberFx();
        for (u32 i = 0; i< size ;i++)
        {
            AFTERFX* afx = _group->getFxAt(i);
            AFTERFX_drawOne(afx);
        }

        endAfterfx(bfalse, (GFX_BLENDMODE)_group->getFinalBlend());

        setAlphaBlend(GFX_BLEND_COPY);
        setDefaultAlphaTest();
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
    {
        _quality = AFTERFX_AdjustQualityFromBufferAvailable(_quality);

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        f32 displace = 1.f*(_factor/1000.f);
        f32 ratiopixel = 1.f/W;

        /// save old Current RT.
        const GCM::Surface * currentRenderSurfSaved = m_pCurrentSwapRenderSurf;

        if ( _clearRT )
        {
            if (_quality == GFX_QUALITY_MEDIUM)
                AFTERFX_PrepareSwapRTDown2x2(false);
            else if (_quality == GFX_QUALITY_LOW)
                AFTERFX_PrepareSwapRTDown4x4(false);
        }

        mp_currentShader->m_selectedTech = 6; // Copy
        setAlphaBlend(GFX_BLEND_COPY);
        setRenderTarget(m_pCurrentSwapRenderSurf); // first, copy the source buffer
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_LINEAR, CELL_GCM_TEXTURE_LINEAR);
        drawScreenQuad(0, 0, (f32)getScreenWidth(), (f32)getScreenHeight(), 0, COLOR_WHITE); // use full res quad as the matrix is still the full res one
        
        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_SwapTargetDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_SwapTargetDown4x4();

        // the blur in action
        mp_currentShader->m_selectedTech = 0; // BigBlur

        u32 passo = 0;
        while ( displace > ratiopixel)
        {
            setRenderTarget(m_pCurrentSwapRenderSurf);

            SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);

            /// shader const.
            mp_currentShader->setFloat(0, displace);

            drawScreenQuadC(0.f, 0.f, W, H, 0.f, COLOR_WHITE, 0.f);

            displace *= 0.5f;
            passo++;

            if (_quality == GFX_QUALITY_MEDIUM)
                AFTERFX_SwapTargetDown2x2();
            else if (_quality == GFX_QUALITY_LOW)
                AFTERFX_SwapTargetDown4x4();

            if (_MaxPass && passo > _MaxPass) 
                break;
        }

        /// Go back to full RT by restoring old current RT.
        m_pCurrentSwapRenderSurf = currentRenderSurfSaved;
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode)
    {
        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        const GCM::Texture * savedSourceTex = m_pCurrentSwapSourceTexture;

        if (_finalBlendMode != GFX_BLEND_COPY) // use alpha blend to add
        {
            // copy the backbuffer
            copyCurrentColorBuffer(0); // copy the destination buffer            
        }

        // As the prepareAfterFx has a no_copy flag, the afterFx shader was not set
        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_BigBlur(_factor, 3, btrue, GFX_QUALITY_MEDIUM);

        if (_finalBlendMode == GFX_BLEND_COPY) // use alpha blend to add
        {
            /// Mul factor.
            mp_currentShader->setFloat(1, _glowfactor*_glowfactor*_addalpha);

            /// set scene texture to shader to add with BigBlur result.
            setAlphaBlend(GFX_BLEND_ADD);
            mp_currentShader->m_selectedTech = 12; // AlphaMul -> cst*Blur*Blur.a 
        }
        else
        {
            // Mul factor.
            mp_currentShader->setFloat(1, _glowfactor);
            mp_currentShader->setFloat(2, _addalpha);

            // set the fullRt copy as sampler1
            SetTextureBind(1, (u32)m_pFullRTtex[0], false);

            /// set scene texture to shader to add with big blur result.
            setAlphaBlend(_finalBlendMode);
            mp_currentShader->m_selectedTech = 1; // AddMul -> (Source + cst*Blur*Blur.a)
        }

        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = i32(getScreenWidth());
        Viewport.bottom = i32(getScreenHeight());
        setupViewport(&Viewport);

        setRenderTarget(m_pCurrentSwapRenderSurf);
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        setAlphaBlend(GFX_BLEND_ALPHA); // restore alpha blend
    }

    void GFXAdapter_GCM::AFTERFX_objectsGlow(f32 _factor)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        setShader(mp_defaultShader);

        const GCM::Surface * previousRT = m_currentRenderTarget;

        /// go back to rendertarget for apply mask
        setRenderTarget(m_pSwapTextureSurfDown4x4[0]);

        setAlphaBlend(GFX_BLEND_ALPHATOCOLOR);
        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE, btrue);

        setShader(mp_shaderManager.getShaderByIndex(1));

        /// switch rendertarget to texture and resolve it.
        AFTERFX_SwapTargetDown4x4();

        /// apply blur.
        AFTERFX_BigBlur(_factor, 3, bfalse, GFX_QUALITY_LOW);

        setShader(mp_defaultShader);

        /// go back to principal target and apply result on it.
        setAlphaBlend(GFX_BLEND_ADD);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(previousRT);
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    }

    ///----------------------------------------------------------------------------//
    void GFXAdapter_GCM::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 3;

        setRenderTarget(m_pCurrentSwapRenderSurf);
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setAlphaBlend(GFX_BLEND_COPY);

        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        mp_currentShader->setFloat(2, _sat);
        mp_currentShader->setFloat(3, _contrast);
        mp_currentShader->setFloat(4, _contrastscale);
        mp_currentShader->setFloat(5, _bright);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 10;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setAlphaBlend(GFX_BLEND_COPY);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        mp_currentShader->setFloat(1, _inBlack);
        mp_currentShader->setFloat(2, _inGamma);
        mp_currentShader->setFloat(3, _inWhite);
        mp_currentShader->setFloat(4, _outBlack);
        mp_currentShader->setFloat(5, _outWhite);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_addSceneAndMul(f32 _mul)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        /// Mul factor.
        mp_currentShader->setFloat(1, _mul);
        mp_currentShader->setFloat(2, 1.f);

        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown4x4();
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_byTech(u32 _tech)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = _tech;
        setRenderTarget( m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown4x4();        
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        depthTest(0);
        depthMask(0);

        setAlphaBlend(GFX_BLEND_COPY);
        mp_currentShader->m_selectedTech = _tech;
        setRenderTarget( m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        setSamplerFilterState(0, CELL_GCM_TEXTURE_NEAREST, CELL_GCM_TEXTURE_NEAREST);

        if (mp_currentShader->m_FloatHandle[1])
            mp_currentShader->setFloat(1, _p0);
        if (mp_currentShader->m_FloatHandle[2])
            mp_currentShader->setFloat(2, _p1);
        if (mp_currentShader->m_FloatHandle[3])
            mp_currentShader->setFloat(3, _p2);
        if (mp_currentShader->m_FloatHandle[4])
            mp_currentShader->setFloat(4, _p3);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown4x4();
    }

    void GFXAdapter_GCM::copyCurrentColorBuffer(u32 _rt)
    {
        depthMask(0);
        depthTest(0);

        const GCM::Surface * srcSurf = m_currentRenderTarget;
        const GCM::Surface * dstSurf;
        if (!UseRefract2x2)
            dstSurf = m_pFullRTsurf[_rt];
        else
            dstSurf = m_pSwapTextureSurfDown2x2[0];

        u32 srcLocation = srcSurf->colorLocation[0];
        u32 destLocation = dstSurf->colorLocation[0];

        m_rdrCtxt.gcmCtxt->SetTransferImage(srcLocation | (destLocation<<1),
                                            dstSurf->colorOffset[0],
                                            dstSurf->colorPitch[0],
                                            0,
                                            0,
                                            srcSurf->colorOffset[0],
                                            srcSurf->colorPitch[0],
                                            0,
                                            0,
                                            dstSurf->width,
                                            dstSurf->height,
                                            dstSurf->colorFormat==CELL_GCM_SURFACE_R5G6B5?2:4);
    }

    void GFXAdapter_GCM::copyTexture(int *TexHan)
    {
        ITF_ASSERT(0);
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V)
    {
        ITF_ASSERT(0);
    }

    GFX_QUALITY GFXAdapter_GCM::AFTERFX_AdjustQualityFromBufferAvailable(GFX_QUALITY _quality)
    {
        if (_quality == GFX_QUALITY_MEDIUM)
        {
            return (UseAfterFXDown2x2)?GFX_QUALITY_MEDIUM:((UseAfterFXDown4x4)?GFX_QUALITY_MEDIUM:GFX_QUALITY_FULL);
        }
        else if (_quality == GFX_QUALITY_LOW)
        {
            return (UseAfterFXDown4x4)?GFX_QUALITY_LOW:((UseAfterFXDown2x2)?GFX_QUALITY_MEDIUM:GFX_QUALITY_FULL);
        }
        else
        {
            return _quality;
        }
    }

    void   GFXAdapter_GCM::AFTERFX_RayCenter( f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
    {
        IMPLEMENTED_NOT_TESTED;

        /// Save Scene before apply mask.
        GFX_QUALITY _quality = AFTERFX_AdjustQualityFromBufferAvailable(GFX_QUALITY_LOW);

        const GCM::Texture * save = m_pCurrentSwapSourceTexture;
        const GCM::Surface * savecur = m_pCurrentSwapRenderSurf;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_PrepareSwapRTDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_PrepareSwapRTDown4x4();

        setAlphaBlend(GFX_BLEND_COPY);
        
        f32 centrof = 0.5f;
        if (_quality == GFX_QUALITY_MEDIUM) centrof = 0.5f * 2;
        else if (_quality == GFX_QUALITY_LOW) centrof = 0.5f  * 4;

        setAlphaBlend(GFX_BLEND_ALPHA);

        mp_currentShader->m_selectedTech = 8;
        f32 ratiofullscreen = (1.f / W);
        f32 value = ratiofullscreen * 4;

        mp_currentShader->setFloat(3, _v4);
        mp_currentShader->setFloat(4, _v5);
        mp_currentShader->setFloat(5, _v6);

        static const float ConvoleRevolution[16] = 
        {
            0.666666f,
            0.583333f,
            0.5416665f,
            0.52083325f,
            0.510416625f,
            0.5052083125f,
            0.50260415625f,
            0.501302078128f,
            0.5006510390625f,
            0.50032551953128f,
            0.500162789765625f,
            0.5000813798828125f,
            0.50004068994140625f,
            0.500020344970703125f,
            0.5000101724853515625f,
            0.50000508624267578125f,
        };

        u32 passo = 0;
        for (u32 i = 0; i < 8 ; i++)
        {
            setRenderTarget( m_pCurrentSwapRenderSurf );

            SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        
            /// shader const.
            mp_currentShader->setFloat(1, value);
            mp_currentShader->setInt(0, passo + 1 );
            mp_currentShader->setFloat( 6, ConvoleRevolution[passo]);

            drawScreenQuadC(0.f, 0.f, W, H, 0.f, COLOR_WHITE, centrof);

            value *= 0.5f;
            passo++;

            if (_quality == GFX_QUALITY_MEDIUM)
                AFTERFX_SwapTargetDown2x2();
            else if (_quality == GFX_QUALITY_LOW)
                AFTERFX_SwapTargetDown4x4();
        }

        /// Go back to full RT by restoring old current RT.
        m_pCurrentSwapRenderSurf = savecur;

        /// Mul factor.
        mp_currentShader->setFloat(1, _v2);
        /// Mul factor.
        mp_currentShader->setFloat(2, _v3);

        /// set scene texture to shader to add with bigblur result.
        SetTextureBind(1, (u32)save, false);

        setAlphaBlend(GFX_BLEND_COPY);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget( m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
     
        AFTERFX_SwapFullScreenTarget();
    }

    void   GFXAdapter_GCM::AFTERFX_Remanence( f32 _factor, f32 _glowfactor, f32 _addalpha)
    {   
        IMPLEMENTED_NOT_TESTED;

        /// Save Scene before apply mask.
        depthTest(0);
        depthMask(0);
        copyCurrentColorBuffer(0);
        setRenderTarget( m_pCurrentSwapRenderSurf);

        /// Write Mask to depth.
        setShader(mp_defaultShader);
        static f32 zz = -0.98f;
        depthTest(1);
        depthMask(0);//0
        depthFunc(GFX_CMP_GREATEREQUAL);
        setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(), zz, COLOR_BLACK, btrue );

        renderContext_SetDefault();

        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_SwapTargetDown2x2();

        /// do blur + add scene
        AFTERFX_BigBlur(_factor,0, btrue, GFX_QUALITY_MEDIUM);

        /// Mul factor.
        mp_currentShader->setFloat(1, _glowfactor);
        /// Mul factor.
        mp_currentShader->setFloat(2, _addalpha);

        setAlphaBlend(GFX_BLEND_ALPHA);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget( m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
    }

    void GFXAdapter_GCM::AFTERFX_SwapTargetDown4x4()
    {
        m_pCurrentSwapSourceTexture = m_pSwapTextureDown4x4[m_curswap];

        // next Target.
        m_curswap += 1;
        m_curswap %= 2;
        m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown4x4[m_curswap];
    }

    void GFXAdapter_GCM::AFTERFX_SwapTargetDown2x2()
    {
        m_pCurrentSwapSourceTexture = m_pSwapTextureDown2x2[m_curswap];

        // next Target.
        m_curswap += 1;
        m_curswap %= 2;
        m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown2x2[m_curswap];
    }

    void GFXAdapter_GCM::AFTERFX_SwapFullScreenTarget()
    {
        if (m_pCurrentSwapRenderSurf == m_pFullRTsurf[0])
        {
            m_pCurrentSwapRenderSurf = m_pFullRTsurf[1];
            m_pCurrentSwapSourceTexture = m_pFullRTtex[0];
        }
        else if (m_pCurrentSwapRenderSurf == m_pFullRTsurf[1])
        {
            m_pCurrentSwapRenderSurf = m_pFullRTsurf[0];
            m_pCurrentSwapSourceTexture = m_pFullRTtex[1];
        }
    }

    void GFXAdapter_GCM::AFTERFX_PrepareSwapRTDown2x2(bool _clear)
    {
        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = i32(getScreenWidth()/2);
        Viewport.bottom = i32(getScreenHeight()/2);
        setupViewport(&Viewport);

        ITF_ASSERT(m_pSwapTextureDown2x2[0]);
        if (_clear)
        {
            setRenderTarget(m_pSwapTextureSurfDown2x2[1]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0);
            setRenderTarget(m_pSwapTextureSurfDown2x2[0]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0);
        }
        else
        {
            setRenderTarget(m_pSwapTextureSurfDown2x2[0]);
        }
        m_curswap = 0;
        m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown2x2[0];
    }

    void GFXAdapter_GCM::AFTERFX_PrepareSwapRTDown4x4(bool _clear)
    {
        IMPLEMENTED_NOT_TESTED;

        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = i32(getScreenWidth()/4);
        Viewport.bottom = i32(getScreenHeight()/4);
        setupViewport(&Viewport);

        ITF_ASSERT(m_pSwapTextureDown4x4[0]);

        if (_clear)
        {
            setRenderTarget(m_pSwapTextureSurfDown4x4[1]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
            setRenderTarget(m_pSwapTextureSurfDown4x4[0]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
        }
        else
        {
            setRenderTarget(m_pSwapTextureSurfDown4x4[0]);
        }
        m_curswap = 0;
        m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown4x4[0];
    }

    void GFXAdapter_GCM::AFTERFX_destroyTarget()
    {
        IMPLEMENTED_NOT_TESTED;

        delete m_pSwapTextureDown2x2[0];
        m_pSwapTextureDown2x2[0] = NULL;
        delete m_pSwapTextureDown2x2[1];
        m_pSwapTextureDown2x2[1] = NULL;

        delete m_pSwapTextureSurfDown2x2[0];
        m_pSwapTextureSurfDown2x2[0] = NULL;
        delete m_pSwapTextureSurfDown2x2[1];
        m_pSwapTextureSurfDown2x2[1] = NULL;

        delete m_pSwapTextureDown4x4[0];
        m_pSwapTextureDown4x4[0] = NULL;
        delete m_pSwapTextureDown4x4[1];
        m_pSwapTextureDown4x4[1] = NULL;

        delete m_pSwapTextureSurfDown4x4[0];
        m_pSwapTextureSurfDown4x4[0] = NULL;
        delete m_pSwapTextureSurfDown4x4[1];
        m_pSwapTextureSurfDown4x4[1] = NULL;
    }

} // namespace ITF
