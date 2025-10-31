#include "precompiled_GFXAdapter_NVN.h"

#include "NVN/Helpers.h"
#include "core/math/Vec2d.h"
#include "engine/display/Camera.h"
#include "engine/display/View.h"
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#include "engine/rasters/RastersManager.h"
#include "GFXAdapter_NVN.h"
#include "RenderTarget_NVN.h"
#include "Texture_NVN.h"
#include "core/utility.h"

#define IMPLEMENTED_NOT_TESTED ITF_NOT_IMPLEMENTED()

namespace ITF
{
    DECLARE_RASTER(afterFX, RasterGroup_Misc, Color::violet());

    void GFXAdapter_NVN::initAfterFx()
    {
        m_curswap = 0;

        String8 debugName;
        for (auto& renderTarget : m_afxFullRT)
        {
            debugName.setTextFormat("afxFullRT_%d", distInRange(m_afxFullRT, renderTarget));
            auto builder = NVNRenderTarget::simpleRenderTargetBuilder(*m_nvnDevice, getScreenWidth(), getScreenHeight());
            renderTarget = NVN::makeUnique<NVNRenderTarget>(*m_textureDescriptorPool, builder, debugName.cStr());
        }

        for (auto& renderTarget : m_afxHalfRT)
        {
            debugName.setTextFormat("afxHalfRT_%d", distInRange(m_afxHalfRT, renderTarget));
            auto builder = NVNRenderTarget::simpleRenderTargetBuilder(*m_nvnDevice, getScreenWidth() / 2, getScreenHeight() / 2);
            renderTarget = NVN::makeUnique<NVNRenderTarget>(*m_textureDescriptorPool, builder, debugName.cStr());
        }
    }

    void GFXAdapter_NVN::setGfxMatRefraction(const GFX_MATERIAL& _gfxMat)
    {
        if (!getSceneResolvedFlag())
        {
            // Copy SceneTexture To TexBuffer.
            copyCurrentColorBuffer(1);
            NVN::syncRenderTargetToTexture(getRenderingContext());
            setSceneResolvedFlag(btrue);
        }

        setShader(CoreShaderGroup::refract);
        mp_currentShader->m_selectedTech = 0;

        // backbuffer_copy/refraction texture.

        setTextureAdressingMode(1, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
        internSetTextureBind(1, m_afxFullRT[1].get());

        // try normal texture.
        setTextureAdressingMode(2, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        if (!SetTextureResource(2, _gfxMat.m_textureNormal))
        {
            // if normal doesn't work
            // try texture Diffuse
            SetTextureResource(2, _gfxMat.m_textureDiffuse);
        }

        // decal factor
        mp_currentShader->setFloat(0, _gfxMat.m_matParams.getfParamsAt(0));
        mp_currentShader->setFloat(1, _gfxMat.m_matParams.getfParamsAt(1));
        mp_currentShader->setFloat(2, _gfxMat.m_matParams.getfParamsAt(2));
        mp_currentShader->setFloat(3, _gfxMat.m_matParams.getfParamsAt(3));
    }


    void GFXAdapter_NVN::AFTERFX_startRenderTarget(Color _color)
    {
        // already set ?
        if (m_currentRenderTarget == m_afxFullRT[0].get())
            return;

        NVN::setMarker(getRenderingContext(), "AFTERFX_startRenderTarget");
        setRenderTarget(m_afxFullRT[1].get());
        clear(GFX_CLEAR_COLOR, _color.getRed(), _color.getGreen(), _color.getBlue(), _color.getAlpha());
        setDefaultAlphaTest();
    }

    void GFXAdapter_NVN::AFTERFX_zoomCenter(f32 _factor, Vec3d _dir)
    {
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_NVN::AFTERFX_prepareObjectGlow()
    {
        // Not supposed to be used -> DRWF_COPYTOTARGET flag is not always used
        ITF_NOT_IMPLEMENTED();
    }

    void GFXAdapter_NVN::AFTERFX_ColorRemap(ITF::Texture* _tex)
    {
        IMPLEMENTED_NOT_TESTED; // not tested

        // setColor Ramp to shader custom texture.
        if (_tex == nullptr)
            return;

        internSetTexture(3, _tex);
        FxParameterDB::TexSamplerHdl texAndSamplerHDl{ mp_currentShader->m_TextureHandle[3] };
        m_paramDB.setSampler(texAndSamplerHDl, m_paramDB.getSamplerDesc(texAndSamplerHDl).setFiltering(*m_samplerDescriptorPool, false));

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 9;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture, false);
        setAlphaBlend(GFX_BLEND_COPY);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    void GFXAdapter_NVN::prepareInternalRT()
    {
        setRenderTarget(m_afxFullRT[1].get());
        clear(GFX_CLEAR_COLOR, m_internRTColor.getRed(), m_internRTColor.getGreen(), m_internRTColor.getBlue(), m_internRTColor.getAlpha());
    }

    void GFXAdapter_NVN::endInternalRT()
    {
        setRenderTarget(getBackBufferRT());
    }

    void GFXAdapter_NVN::prepareAfterFx(bbool _nocopy)
    {
        NVN::pushMarker(getRenderingContext(), "afterFx");
        m_worldViewProj.push();

        init2DRender();

        if (_nocopy)
        {
            if (m_currentRenderTarget == getBackBufferRT())
            {
                // set the back buffer as source and dest (after fx job to not used it at the same time)
                m_pCurrentSwapRenderSurf = getBackBufferRT();
                m_pCurrentSwapSourceTexture = m_pCurrentSwapRenderSurf;
            }
            return;
        }

        setShader(mp_shaderManager.getShaderByIndex(1));
        setAlphaRef(1);

        // if the current rendering target is the backbuffer then resolve it to texture.
        // to the m_pFullRTtex[0].
        if (m_currentRenderTarget == getBackBufferRT())
        {
            copyCurrentColorBuffer(0);
            NVN::syncRenderTargetToTexture(getRenderingContext());
            setRenderTarget(m_afxFullRT[1].get());
            m_pCurrentSwapSourceTexture = m_afxFullRT[0].get();
            m_pCurrentSwapRenderSurf = m_afxFullRT[1].get();
        }
        else if (m_currentRenderTarget == m_afxFullRT[1].get())
        {
            NVN::syncRenderTargetToTexture(getRenderingContext());
            setRenderTarget(m_afxFullRT[0].get());
            //clear( GFX_CLEAR_COLOR , 0, 0, 0, 0);  // Is it really useful ?
            m_pCurrentSwapSourceTexture = m_afxFullRT[1].get();
            m_pCurrentSwapRenderSurf = m_afxFullRT[0].get();
        }
    }

    void GFXAdapter_NVN::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
    {
        // restore default shader renderer.
        setShader(mp_defaultShader);

        if (!_nocopy)
        {
            setRenderTarget(getBackBufferRT());

            Matrix44 savedUVmat = m_UVmat;
            m_UVmat.setIdentity();

            GFX_RECT Viewport;
            Viewport.left = 0;
            Viewport.top = 0;
            Viewport.right = i32(getScreenWidth());
            Viewport.bottom = i32(getScreenHeight());
            setupViewport(&Viewport);

            internSetTextureBind(0, m_pCurrentSwapSourceTexture, true);
            setAlphaBlend(_finalBlend);
            drawScreenQuad(0, 0, (f32)getScreenWidth(), (f32)getScreenHeight(), 0, COLOR_WHITE);

            m_UVmat = savedUVmat;

            setAlphaBlend(GFX_BLEND_ALPHA);
        }

        NVN::popMarker(getRenderingContext()); // pop "afterFx"

        m_worldViewProj.pop();

        CAMERA->apply();

        end2DRender();
    }

    void GFXAdapter_NVN::AFTERFX_draw(AFX_Group* _group)
    {
        IMPLEMENTED_NOT_TESTED;

        TIMERASTER_SCOPE(afterFX);

        prepareAfterFx(bfalse);

        u32 size = _group->getNumberFx();
        for (u32 i = 0; i < size; i++)
        {
            AFTERFX* afx = _group->getFxAt(i);
            AFTERFX_drawOne(afx);
        }

        endAfterfx(bfalse, (GFX_BLENDMODE)_group->getFinalBlend());

        setAlphaBlend(GFX_BLEND_COPY);
        setDefaultAlphaTest();
    }

    void GFXAdapter_NVN::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
    {
        NVN::pushMarker(getRenderingContext(), "BigBlur");

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        f32 displace = 1.f * (_factor / 1000.f);
        f32 ratiopixel = 1.f / W;

        // save old Current RT.
        auto * currentRenderSurfSaved = m_pCurrentSwapRenderSurf;

        if (_clearRT)
        {
            if (_quality == GFX_QUALITY_MEDIUM || _quality == GFX_QUALITY_LOW)
                AFTERFX_PrepareSwapRTDown2x2(false);
        }

        mp_currentShader->m_selectedTech = 6; // Copy
        setAlphaBlend(GFX_BLEND_COPY);
        setRenderTarget(m_pCurrentSwapRenderSurf); // first, copy the source buffer
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);
        drawScreenQuad(0, 0, (f32)getScreenWidth(), (f32)getScreenHeight(), 0, COLOR_WHITE); // use full res quad as the matrix is still the full res one

        if (_quality == GFX_QUALITY_MEDIUM || _quality == GFX_QUALITY_LOW)
            AFTERFX_SwapTargetDown2x2();

        // the blur in action
        mp_currentShader->m_selectedTech = 0; // BigBlur

        u32 passo = 0;
        while (displace > ratiopixel)
        {
            setRenderTarget(m_pCurrentSwapRenderSurf);

            internSetTextureBind(0, m_pCurrentSwapSourceTexture);

            /// shader const.
            mp_currentShader->setFloat(0, displace);

            drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

            displace *= 0.5f;
            passo++;

            if (_quality == GFX_QUALITY_MEDIUM || _quality == GFX_QUALITY_LOW)
                AFTERFX_SwapTargetDown2x2();

            if (_MaxPass && passo > _MaxPass)
                break;
        }

        /// Go back to full RT by restoring old current RT.
        m_pCurrentSwapRenderSurf = currentRenderSurfSaved;

        NVN::popMarker(getRenderingContext()); // pop "BigBlur"
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_NVN::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode)
    {
        NVN::pushMarker(getRenderingContext(), "Glow");

        // A texture with the content of what was rendered until now.
        // Depending on the prepareAfterFX parameter, it is the final rt or a afx full rt
        auto* originalSceneTexture = m_pCurrentSwapSourceTexture;

        // As the prepareAfterFx has a no_copy flag, the afterFx shader was not set
        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_BigBlur(_factor, 0, btrue, GFX_QUALITY_MEDIUM);


        // As the prepareAfterFx has a no_copy flag, the afterFx shader was not set
        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_BigBlur(_factor, 3, btrue, GFX_QUALITY_MEDIUM);

        // Set AddMul_PS =>
        // bluredScene = bigBlurResult * _glowfactor;
        // result = originalSceneTexture + bluredScene * (bluredScene.a * _addAlpha);
        mp_currentShader->m_selectedTech = 1;

        mp_currentShader->setFloat(1, _glowfactor);
        mp_currentShader->setFloat(2, _addalpha);

        setAlphaBlend(GFX_BLEND_COPY);
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);
        internSetTextureBind(1, originalSceneTexture);
        drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();

        setAlphaBlend(GFX_BLEND_ALPHA); // restore alpha blend

        NVN::popMarker(getRenderingContext()); // pop "Glow"
    }

    void GFXAdapter_NVN::AFTERFX_objectsGlow(f32 _factor)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        setShader(mp_defaultShader);

        auto* previousRT = m_currentRenderTarget;

        /// go back to rendertarget for apply mask
        setRenderTarget(m_afxHalfRT[0].get());

        setAlphaBlend(GFX_BLEND_ALPHATOCOLOR);
        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE, btrue);

        setShader(mp_shaderManager.getShaderByIndex(1));

        /// switch rendertarget to texture and resolve it.
        AFTERFX_SwapTargetDown2x2();

        /// apply blur.
        AFTERFX_BigBlur(_factor, 3, bfalse, GFX_QUALITY_LOW);

        setShader(mp_defaultShader);

        /// go back to principal target and apply result on it.
        setAlphaBlend(GFX_BLEND_ADD);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(previousRT);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    }

    ///----------------------------------------------------------------------------//
    void GFXAdapter_NVN::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 3;

        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture, false);
        setAlphaBlend(GFX_BLEND_COPY);

        mp_currentShader->setFloat(2, _sat);
        mp_currentShader->setFloat(3, _contrast);
        mp_currentShader->setFloat(4, _contrastscale);
        mp_currentShader->setFloat(5, _bright);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_NVN::AFTERFX_ColorLevels(f32 _inBlack, f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = 10;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture, false);
        setAlphaBlend(GFX_BLEND_COPY);

        mp_currentShader->setFloat(1, _inBlack);
        mp_currentShader->setFloat(2, _inGamma);
        mp_currentShader->setFloat(3, _inWhite);
        mp_currentShader->setFloat(4, _outBlack);
        mp_currentShader->setFloat(5, _outWhite);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    void GFXAdapter_NVN::AFTERFX_addSceneAndMul(f32 _mul)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        // Mul factor.
        mp_currentShader->setFloat(1, _mul);
        mp_currentShader->setFloat(2, 1.f);

        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture, false);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
    }

    void GFXAdapter_NVN::AFTERFX_byTech(u32 _tech)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        mp_currentShader->m_selectedTech = _tech;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
    }

    void GFXAdapter_NVN::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
    {
        IMPLEMENTED_NOT_TESTED;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        depthTest(0);
        depthMask(0);

        setAlphaBlend(GFX_BLEND_COPY);
        mp_currentShader->m_selectedTech = _tech;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture, false);

        if (mp_currentShader->m_FloatHandle[1])
            mp_currentShader->setFloat(1, _p0);
        if (mp_currentShader->m_FloatHandle[2])
            mp_currentShader->setFloat(2, _p1);
        if (mp_currentShader->m_FloatHandle[3])
            mp_currentShader->setFloat(3, _p2);
        if (mp_currentShader->m_FloatHandle[4])
            mp_currentShader->setFloat(4, _p3);

        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
    }

    void GFXAdapter_NVN::copyCurrentColorBuffer(u32 _rt)
    {
        NVNRenderTarget* dstSurf = m_afxFullRT[_rt].get();
        if (m_currentRenderTarget == dstSurf)
            return; // nothing to do
        NVN::pushMarker(getRenderingContext(), "copyCurrentColorBuffer");

        // First ensure that we can copy the current color buffer
        NVN::syncRenderTargetToTexture(getRenderingContext());

        depthMask(0);
        depthTest(0);

        // backup states

        m_worldViewProj.push();
        Matrix44 savedUVmat = m_UVmat;
        m_UVmat.setIdentity();

        GFX_BLENDMODE prevBlendMode = u_CurrentBlendingMode;

        ITF_shader* prevShader = mp_currentShader;
        setShader(mp_defaultShader);

        // Nothing to backup for this one?
        setOrthoView(0.f, dstSurf->getPlatfomTexture().GetWidth(), 0.f, dstSurf->getPlatfomTexture().GetHeight());

        NVNRenderTarget* previousRT = m_currentRenderTarget;
        NVNTexture * srcTex = m_currentRenderTarget;

        // copy using the default shader
        mp_currentShader->m_selectedTech = 0; // default_PCT_VS/PS
        setAlphaBlend(GFX_BLEND_COPY);
        setRenderTarget(dstSurf);
        internSetTextureBind(0, srcTex, false);
        drawScreenQuad(0, 0, dstSurf->getPlatfomTexture().GetWidth(), dstSurf->getPlatfomTexture().GetHeight(), 0, COLOR_WHITE);

        // restore states
        m_UVmat = savedUVmat;
        m_worldViewProj.pop();

        setAlphaBlend(prevBlendMode);

        setShader(prevShader);

        NVN::popMarker(getRenderingContext());

        setRenderTarget(previousRT);
    }

    void   GFXAdapter_NVN::AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
    {
        IMPLEMENTED_NOT_TESTED;

        auto* save = m_pCurrentSwapSourceTexture;
        auto* savecur = m_pCurrentSwapRenderSurf;

        f32 W = (f32)getScreenWidth();
        f32 H = (f32)getScreenHeight();

        AFTERFX_PrepareSwapRTDown2x2();

        setAlphaBlend(GFX_BLEND_COPY);

        f32 centrof = 0.5f;
        centrof = 0.5f * 2;

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
        for (u32 i = 0; i < 8; i++)
        {
            setRenderTarget(m_pCurrentSwapRenderSurf);

            internSetTextureBind(0, m_pCurrentSwapSourceTexture);

            /// shader const.
            mp_currentShader->setFloat(1, value);
            mp_currentShader->setInt(0, passo + 1);
            mp_currentShader->setFloat(6, ConvoleRevolution[passo]);

            drawScreenQuadCentroid(0.f, 0.f, W, H, 0.f, COLOR_WHITE, centrof);

            value *= 0.5f;
            passo++;

            AFTERFX_SwapTargetDown2x2();
        }

        /// Go back to full RT by restoring old current RT.
        m_pCurrentSwapRenderSurf = savecur;

        /// Mul factor.
        mp_currentShader->setFloat(1, _v2);
        /// Mul factor.
        mp_currentShader->setFloat(2, _v3);

        /// set scene texture to shader to add with bigblur result.
        internSetTextureBind(1, save, false);

        setAlphaBlend(GFX_BLEND_COPY);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

        AFTERFX_SwapFullScreenTarget();
    }

    void GFXAdapter_NVN::AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha)
    {
        IMPLEMENTED_NOT_TESTED;

        /// Save Scene before apply mask.
        depthTest(0);
        depthMask(0);
        copyCurrentColorBuffer(0);
        NVN::syncRenderTargetToTexture(getRenderingContext());

        setRenderTarget(m_pCurrentSwapRenderSurf);

        /// Write Mask to depth.
        setShader(mp_defaultShader);
        static f32 zz = -0.98f;
        depthTest(1);
        depthMask(0);//0
        depthFunc(GFX_CMP_GREATEREQUAL);
        setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(0, 0, (f32)getScreenWidth(), (f32)getScreenHeight(), zz, COLOR_BLACK, btrue);

        renderContext_SetDefault();

        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_SwapTargetDown2x2();

        /// do blur + add scene
        AFTERFX_BigBlur(_factor, 0, btrue, GFX_QUALITY_MEDIUM);

        /// Mul factor.
        mp_currentShader->setFloat(1, _glowfactor);
        /// Mul factor.
        mp_currentShader->setFloat(2, _addalpha);

        setAlphaBlend(GFX_BLEND_ALPHA);
        mp_currentShader->m_selectedTech = 1;
        setRenderTarget(m_pCurrentSwapRenderSurf);
        internSetTextureBind(0, m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
    }

    void GFXAdapter_NVN::AFTERFX_SwapTargetDown2x2()
    {
        m_pCurrentSwapSourceTexture = m_afxHalfRT[m_curswap].get();

        m_curswap = (m_curswap + 1) % AfterFXNbSwapSurface;
        m_pCurrentSwapRenderSurf = m_afxHalfRT[m_curswap].get();

        NVN::syncRenderTargetToTexture(getRenderingContext());
    }

    void GFXAdapter_NVN::AFTERFX_SwapFullScreenTarget()
    {
        if (m_pCurrentSwapRenderSurf == m_afxFullRT[0].get())
        {
            m_pCurrentSwapRenderSurf = m_afxFullRT[1].get();
            m_pCurrentSwapSourceTexture = m_afxFullRT[0].get();
        }
        else if (m_pCurrentSwapRenderSurf == m_afxFullRT[1].get())
        {
            m_pCurrentSwapRenderSurf = m_afxFullRT[0].get();
            m_pCurrentSwapSourceTexture = m_afxFullRT[1].get();
        }

        NVN::syncRenderTargetToTexture(getRenderingContext());
    }

    void GFXAdapter_NVN::AFTERFX_PrepareSwapRTDown2x2(bool _clear)
    {
        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = i32(getScreenWidth() / 2);
        Viewport.bottom = i32(getScreenHeight() / 2);
        setupViewport(&Viewport);

        if (_clear)
        {
            setRenderTarget(m_afxHalfRT[1].get());
            clear(GFX_CLEAR_COLOR, 0, 0, 0, 0);
            setRenderTarget(m_afxHalfRT[0].get());
            clear(GFX_CLEAR_COLOR, 0, 0, 0, 0);
        }
        else
        {
            setRenderTarget(m_afxHalfRT[0].get());
        }
        m_curswap = 0;
        m_pCurrentSwapRenderSurf = m_afxHalfRT[0].get();
    }

} // namespace ITF
