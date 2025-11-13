#include "precompiled_GFXAdapter_DX12.h"

#include "GFXAdapter_DX12.h"

#include "DX12/Helpers.h"

#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#include "Texture_DX12.h"
#include "RenderTarget_DX12.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "core/math/BitTweak.h"
#include "core/UnicodeTools.h"

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    /// Render Target : intern
    //////////////////////////////////////////////////////////////////////////

    void GFXAdapter_DX12::setRenderTarget(DX12::RenderTarget& _rtSurface)
    {
        if (m_currentRenderTarget == &_rtSurface)
            return;

        m_currentRenderTarget = &_rtSurface;

        DX12::setMarker(getRenderingContext(), "setRenderTarget");

        m_currentRenderTarget->set(getRenderingContext());

        GFX_RECT rcViewport;
        rcViewport.left = 0;
        rcViewport.right = u32(m_currentRenderTarget->getDesc().Width);
        rcViewport.top = 0;
        rcViewport.bottom = m_currentRenderTarget->getDesc().Height;
        setupViewport(&rcViewport);
    }

    void GFXAdapter_DX12::cleanupRenderTarget(renderTarget* _target)
    {
        if (_target)
        {
            if (_target->getSurface())
            {
                DX12::RenderTarget* surf = reinterpret_cast<DX12::RenderTarget*>(_target->getSurface());
                ITF_ASSERT(surf != nullptr);
                ITF_ASSERT(_target->getTexture() != nullptr);
                ITF_ASSERT(_target->getTexture()->m_adapterimplementationData != nullptr);
                ITF_ASSERT(reinterpret_cast<DX12::Texture*>(_target->getTexture()->m_adapterimplementationData) == surf->getTextureView());
                _target->getTexture()->m_adapterimplementationData = nullptr;
                delete surf;
                _target->setSurface(0);
            }

            cleanupTexture(_target->getTexture());
        }
    }


    void GFXAdapter_DX12::enableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0)
            return;

        if (_target->needDelayedClear())
        {
            u32 clearColor = _target->delayedClearColor();
            _target->clearDelayedClear(); // Clear before clearRenderTarget so that it can set the delay clear again.
            clearRenderTarget(_target, clearColor);
        }

        DX12::setMarker(getRenderingContext(), "enableRenderTarget");

        _target->m_prevColourSurface = uPtr(m_currentRenderTarget);
        DX12::RenderTarget* newRT = reinterpret_cast<DX12::RenderTarget*>(_target->getSurface());
        setRenderTarget(*newRT);
    }

    void GFXAdapter_DX12::disableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0 || _target->m_prevColourSurface == 0)
            return;

        DX12::setMarker(getRenderingContext(), "disableRenderTarget");

        DX12::RenderTarget* rt = reinterpret_cast<DX12::RenderTarget*>(_target->getSurface());

        DX12::RenderTarget* prevRT = reinterpret_cast<DX12::RenderTarget*>(_target->m_prevColourSurface);

        setRenderTarget(*prevRT);

        DX12::Texture* tex = rt->transitionToTexture(getRenderingContext());
        ITF_UNUSED(tex);
        ITF_ASSERT(tex == reinterpret_cast<DX12::Texture*>(_target->getTexture()->m_adapterimplementationData));
    }

    void GFXAdapter_DX12::clearRenderTarget(renderTarget* _target, u32 _color)
    {
        if (!isFrameStarted())
        {
            _target->setDelayedClear(_color);
            return;
        }

        DX12::setMarker(getRenderingContext(), "clearRenderTarget");

        ITF_ASSERT_MSG(_target->getSurface() != 0, "Unexpected empty render target");
        if (_target->getSurface() == 0)
            return;

        DX12::RenderTarget* prevRT = m_currentRenderTarget;
        DX12::RenderTarget* newRT = reinterpret_cast<DX12::RenderTarget*>(_target->getSurface());

        if (prevRT != newRT)
        {
            setRenderTarget(*newRT);
        }

        m_currentRenderTarget->clear(getRenderingContext(), Color(_color));

        if (prevRT != newRT)
        {
            setRenderTarget(*prevRT);
        }
    }

    void GFXAdapter_DX12::initialiseRenderTarget(renderTarget* _target)
    {
        if (_target->getTexture()->m_adapterimplementationData != nullptr)
            return;

        u32 width = u32(_target->m_area[2] - _target->m_area[0]);
        u32 height = u32(_target->m_area[3] - _target->m_area[1]);

        DX12::RenderTarget* rt = new (MemoryId::mId_GfxAdapter) DX12::RenderTarget(
            DX12::RenderTarget::createCommitted(width, height, Color(0, 0, 0, 0), *m_rtvDescriptorPool, "RTOnTheFly"));

        DX12::Texture * rtAsTex = &rt->createTextureView(*m_textureDescriptorPool);

        _target->setSurface(uPtr(rt));
        _target->getTexture()->m_adapterimplementationData = rtAsTex;

        // Set Original data Size.
        Texture* texture = _target->getTexture();
        texture->m_datasizeY = height;
        texture->m_datasizeX = width;
        texture->setSize(width, height);
        updateCachedScaleValues(_target->getTexture());
    }

    void GFXAdapter_DX12::resizeRenderTarget(renderTarget* _target, const Size& _size)
    {
        ITF_ASSERT(0); // Only useful for HD screenshots
    }

    void GFXAdapter_DX12::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
    {
        DX12::pushMarker(getRenderingContext(), "ImpostorRendering");
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

    void GFXAdapter_DX12::stopImpostorRendering(renderTarget* _rt)
    {
        DX12::popMarker(getRenderingContext());

        _rt->unSet();

        /// restore backbuffer and copy result of afterfx texture.
        //setBackBuffer();

        /// restore 3d View.
        CAMERA->apply();
    }

} // namespace ITF
