#include "precompiled_GFXAdapter_NVN.h"

#include "NVN/Helpers.h"

#include "GFXAdapter_NVN.h"
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#include "Texture_NVN.h"
#include "RenderTarget_NVN.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "core/math/BitTweak.h"
#include "core/UnicodeTools.h"

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    /// Render Target : intern
    //////////////////////////////////////////////////////////////////////////

    void GFXAdapter_NVN::setRenderTarget(NVNRenderTarget* _rtSurface)
    {
        ITF_ASSERT(_rtSurface != nullptr);
        if (m_currentRenderTarget == _rtSurface)
            return;

        m_currentRenderTarget = _rtSurface;
        setRenderTarget(&_rtSurface->getPlatfomTexture());
    }

    void GFXAdapter_NVN::setRenderTarget(nvn::Texture * _texture)
    {
        ITF_ASSERT(_texture != nullptr);

        NVN::setMarker(getRenderingContext(), "setRenderTarget");
        nvn::Texture* nvnTargetTextureArray[1]{ _texture };
        getRenderingContext()->SetRenderTargets(1, nvnTargetTextureArray, nullptr, nullptr, nullptr);

        GFX_RECT rcViewport;
        rcViewport.left = 0;
        rcViewport.right = _texture->GetWidth();
        rcViewport.top = 0;
        rcViewport.bottom = _texture->GetHeight();
        setupViewport(&rcViewport);
    }

    void GFXAdapter_NVN::cleanupRenderTarget(renderTarget* _target)
    {
        if (_target)
        {
            if (_target->getSurface())
            {
                NVNRenderTarget* surf = reinterpret_cast<NVNRenderTarget*>(_target->getSurface());
                // NVNRenderTarget and NVNTexture are the same objects, so by calling delete surf, we also delete what is pointed by m_adapterimplementationData.
                // So to avoid dangling pointer, let's set the pointer to nullptr.
                // cleanupTexture below will early return.
                ITF_ASSERT(reinterpret_cast<NVNTexture*>(_target->getTexture()->m_adapterimplementationData) == surf);
                _target->getTexture()->m_adapterimplementationData = nullptr;
                delete surf;
                _target->setSurface(0);
            }

            cleanupTexture(_target->getTexture());
        }
    }


    void GFXAdapter_NVN::enableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0)
            return;

        if (_target->needDelayedClear())
        {
            u32 clearColor = _target->delayedClearColor();
            _target->clearDelayedClear(); // Clear before clearRenderTarget so that it can set the delay clear again.
            clearRenderTarget(_target, clearColor);
        }

        NVN::setMarker(getRenderingContext(), "enableRenderTarget");

        _target->m_prevColourSurface = uPtr(m_currentRenderTarget);
        NVNRenderTarget* newRT = reinterpret_cast<NVNRenderTarget*>(_target->getSurface());
        setRenderTarget(newRT);
    }

    void GFXAdapter_NVN::disableRenderTarget(renderTarget* _target)
    {
        if (!_target || _target->getSurface() == 0 || _target->m_prevColourSurface == 0)
            return;

        NVN::setMarker(getRenderingContext(), "disableRenderTarget");

        NVNRenderTarget* prevRT = reinterpret_cast<NVNRenderTarget*>(_target->m_prevColourSurface);
        setRenderTarget(prevRT);
    }

    void GFXAdapter_NVN::clearRenderTarget(renderTarget* _target, u32 _color)
    {
        if (!getRenderingContext()->IsRecording())
        {
            _target->setDelayedClear(_color);
            return;
        }

        NVN::setMarker(getRenderingContext(), "clearRenderTarget");

        NVNRenderTarget* nvnRT = nullptr;
        NVNTexture* nvnTex = nullptr;
        if (_target->getSurface() != 0)
        {
            nvnRT = reinterpret_cast<NVNRenderTarget*>(_target->getSurface());
            nvnTex = nvnRT;
        }
        else if (_target->getTexture() != nullptr && _target->getTexture()->m_adapterimplementationData != nullptr)
        {
            nvnTex = reinterpret_cast<NVNTexture*>(_target->getTexture()->m_adapterimplementationData);
        }

        ITF_ASSERT(nvnTex != nullptr);
        if (nvnTex == nullptr)
            return;

        NVNRenderTarget* prevRT = m_currentRenderTarget;

        if (prevRT != nvnTex)
        {
            setRenderTarget(&nvnTex->getPlatfomTexture());
        }

        Color c = _color;
        f32 colorChannelArray[4] = { c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()};
        getRenderingContext()->ClearColor(0, colorChannelArray, nvn::ClearColorMask::RGBA);

        if (prevRT != nvnTex)
        {
            setRenderTarget(prevRT);
        }
    }

    void GFXAdapter_NVN::initialiseRenderTarget(renderTarget* _target)
    {
        if (_target->getTexture()->m_adapterimplementationData != nullptr)
            return;

        u32 width = u32(_target->m_area[2] - _target->m_area[0]);
        u32 height = u32(_target->m_area[3] - _target->m_area[1]);

        auto builder = NVNRenderTarget::simpleRenderTargetBuilder(*m_nvnDevice, width, height);
        NVNRenderTarget* rt = new (MemoryId::mId_GfxAdapter) NVNRenderTarget(*m_textureDescriptorPool, builder, "RTOnTheFly");
        NVNRenderTarget* tex = rt;

        _target->setSurface(uPtr(rt));
        _target->getTexture()->m_adapterimplementationData = tex;

        // Set Original data Size.
        Texture* texture = _target->getTexture();
        texture->m_datasizeY = height;
        texture->m_datasizeX = width;
        texture->setSize(width, height);
        updateCachedScaleValues(_target->getTexture());
    }

    void GFXAdapter_NVN::resizeRenderTarget(renderTarget* _target, const Size& _size)
    {
        ITF_ASSERT(0); // Only useful for HD screenshots
    }

    void GFXAdapter_NVN::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
    {
        NVN::pushMarker(getRenderingContext(), "ImpostorRendering");
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

    void GFXAdapter_NVN::stopImpostorRendering(renderTarget* _rt)
    {
        NVN::popMarker(getRenderingContext());

        _rt->unSet();

        /// restore backbuffer and copy result of afterfx texture.
        //setBackBuffer();

        /// restore 3d View.
        CAMERA->apply();
    }

} // namespace ITF
