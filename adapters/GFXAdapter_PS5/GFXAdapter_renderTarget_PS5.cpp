#include "precompiled_GFXAdapter_PS5.h"

#include "Gnmp_Helpers.h"
#include <agc/core.h>
#include <agc/toolkit/toolkit.h>

#include "GFXAdapter_PS5.h"
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#include "Texture_PS5.h"
#include "RenderTarget_PS5.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "core/math/BitTweak.h"
#include "core/UnicodeTools.h"

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    /// Render Target : intern
    //////////////////////////////////////////////////////////////////////////

    void GFXAdapter_PS5::setRenderTarget(PS5RenderTarget* _rtSurface)
    {
        ITF_ASSERT(_rtSurface != nullptr);
        if (m_currentRenderTarget == _rtSurface)
            return;

        m_currentRenderTarget = _rtSurface;

        Gnmp::setMarker(getRenderingContext(), "setRenderTarget");
        u32 rtSlot = 0;
        getRenderingContext()->setRenderTargetMask((0xF) << rtSlot); // 0xF, 1 bit for each of RGBA
        getRenderingContext()->setRenderTarget(rtSlot, m_currentRenderTarget);

        GFX_RECT rcViewport;
        rcViewport.left = 0;
        rcViewport.right = m_currentRenderTarget->getWidth();
        rcViewport.top = 0;
        rcViewport.bottom = m_currentRenderTarget->getHeight();
        setupViewport(&rcViewport);
    }

    void GFXAdapter_PS5::cleanupRenderTarget(renderTarget* _target)
    {
        if (_target)
        {
            if (_target->getSurface())
            {
                PS5RenderTarget* surf = reinterpret_cast<PS5RenderTarget*>(_target->getSurface());
                delete surf;
                _target->setSurface(0);
            }

            cleanupTexture(_target->getTexture());
        }
    }


    void GFXAdapter_PS5::enableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0)
            return;

        Gnmp::setMarker(getRenderingContext(), "enableRenderTarget");

        _target->m_prevColourSurface = uPtr(m_currentRenderTarget);
        PS5RenderTarget* newRT = reinterpret_cast<PS5RenderTarget*>(_target->getSurface());
        setRenderTarget(newRT);
    }

    void GFXAdapter_PS5::disableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0 || _target->m_prevColourSurface == 0)
            return;

        Gnmp::setMarker(getRenderingContext(), "disableRenderTarget");

        PS5RenderTarget* prevRT = reinterpret_cast<PS5RenderTarget*>(_target->m_prevColourSurface);
        setRenderTarget(prevRT);
    }

    void GFXAdapter_PS5::clearRenderTarget(renderTarget* _target, u32 _color)
    {
        Gnmp::setMarker(getRenderingContext(), "clearRenderTarget");

        using namespace sce::Agc::Core;
        using namespace sce::Agc::Toolkit;

        auto encodedColor = Encoder::raw(_color);

        if (_target->getSurface())
        {
            PS5RenderTarget* rt = reinterpret_cast<PS5RenderTarget*>(_target->getSurface());
            auto modifiedStates = clearRenderTargetCs(&getRenderingContext()->m_agcCtx.m_dcb, &rt->m_agcRenderTarget, encodedColor);
            getRenderingContext()->m_agcCtx.resetToolkitChangesAndSyncToGl2(modifiedStates);
            m_shaderProgramCache.reset();
            m_blendStateCache.reset();
            m_primitiveTypeCache.reset();
        }
        else if (_target->getTexture()!=nullptr && _target->getTexture()->m_adapterimplementationData != nullptr)
        {
            PS5Texture* tex = reinterpret_cast<PS5Texture*>(_target->getTexture()->m_adapterimplementationData);
            sce::Agc::CxRenderTarget rt;
            rt.init();
            ITF_VERIFY_SCE_CALL(sce::Agc::Core::translate(&rt, &tex->m_agcTexture));
            auto modifiedStates = sce::Agc::Toolkit::clearRenderTargetCs(&getRenderingContext()->m_agcCtx.m_dcb, &rt, encodedColor);
            getRenderingContext()->m_agcCtx.resetToolkitChangesAndSyncToGl2(modifiedStates);
            m_shaderProgramCache.reset();
            m_blendStateCache.reset();
            m_primitiveTypeCache.reset();
        }
    }

    void GFXAdapter_PS5::initialiseRenderTarget(renderTarget* _target)
    {
        if (_target->getTexture()->m_adapterimplementationData != nullptr)
            return;

        u32 width = u32(_target->m_area[2] - _target->m_area[0]);
        u32 height = u32(_target->m_area[3] - _target->m_area[1]);

        ITF_ASSERT(width <= (1u << BitTweak::GetLog2(width)));
        ITF_ASSERT(height <= (1u << BitTweak::GetLog2(width)));
        width = 1u << BitTweak::GetLog2(width);
        height = 1u << BitTweak::GetLog2(height);

        width = Clamp(width, getMinTextureSize(), getMaxTextureSize());
        height = Clamp(height, getMinTextureSize(), getMaxTextureSize());

        Gnmp::RenderTargetSpec rtSpec{};
        rtSpec.init();
        rtSpec.m_width = width;
        rtSpec.m_height = height;
        rtSpec.m_colorFormat = Gnmp::kDataFormatR8G8B8A8Unorm;
        rtSpec.m_colorTileModeHint = sce::Agc::CxRenderTarget::TileMode::kRenderTarget;

        PS5RenderTarget* rt = new (MemoryId::mId_GfxAdapter) PS5RenderTarget{};
        ITF_VERIFY_SCE_CALL(rt->init(&rtSpec));
        rt->baseMemHdl.alloc<Gnmp::RENDER_TARGET>(rt->getColorSizeAlign(), Gnmp::VRAM_RenderTarget);

    #if defined(ITF_ENABLE_AGC_RESOURCE_REGISTERING) && ITF_ENABLE_AGC_RESOURCE_REGISTERING
        String fileName;
        _target->getTexture()->getPath().getBasenameWithoutExtension(fileName);
        rt->m_resHandler.set(UTF16ToUTF8(fileName.cStr()).get());
    #endif

        PS5Texture* tex = new (MemoryId::mId_GfxAdapter) PS5Texture{};
        tex->initFromRenderTarget(*rt, bfalse);

        _target->setSurface(uPtr(rt));
        _target->getTexture()->m_adapterimplementationData = tex;

        // Set Original data Size.
        Texture* texture = _target->getTexture();
        texture->m_datasizeY = height;
        texture->m_datasizeX = width;
        texture->setSize(width, height);
        updateCachedScaleValues(_target->getTexture());
    }

    void GFXAdapter_PS5::resizeRenderTarget(renderTarget* _target, const Size& _size)
    {
        ITF_ASSERT(0); // Only useful for HD screenshots
    }

    void GFXAdapter_PS5::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
    {
        Gnmp::pushMarker(getRenderingContext(), "ImpostorRendering");
        _rt->set();

        if (_mustClear)
        {
            clearRenderTarget(_rt, _rt->getBackGroundColor());
        }

        setOrthoView(xmin, xmax, ymin, ymax);

        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = _rt->m_area[2];
        Viewport.bottom = _rt->m_area[3];
        setupViewport(&Viewport);

        /// Ready to go !!!.
    }

    void GFXAdapter_PS5::stopImpostorRendering(renderTarget* _rt)
    {
        Gnmp::popMarker(getRenderingContext());

        _rt->unSet();

        /// restore backbuffer and copy result of afterfx texture.
        //setBackBuffer();

        /// restore 3d View.
        CAMERA->apply();
    }

} // namespace ITF
