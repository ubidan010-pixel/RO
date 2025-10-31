#include "precompiled_GFXAdapter_NVN.h"

#include "adapters/GFXAdapter_NVN/GFXAdapter_NVN.h"

#include "RenderTarget_NVN.h"

#include "core/utility.h"

namespace ITF
{

    bbool GFXAdapter_NVN::isDeviceLost()
    {
        // probably never lost...
        return bfalse;
    }

    bbool GFXAdapter_NVN::resetDevice(bbool _forceReset)
    {
        // Resolution change is rather handled by rebuildRenderTargets(..)
        return btrue;
    }

    void GFXAdapter_NVN::requestRenderTargetsRebuild(i32 _width, i32 _height)
    {
        m_newScreenWidth = _width;
        m_newScreenHeight = _height;
        m_needRebuildRenderTargets = true;
    }

    void GFXAdapter_NVN::createRenderTargets()
    {
        // set resolution before creating render targets
        ITF_ASSERT(m_newScreenWidth == getScreenWidth());
        ITF_ASSERT(m_newScreenHeight == getScreenHeight());

        m_window = nullptr; // ensure clean of the window before rebuilding it

        nvn::TextureBuilder builder = NVNRenderTarget::simpleRenderTargetBuilder(*m_nvnDevice, getScreenWidth(), getScreenHeight(), nvn::TextureFlags::COMPRESSIBLE | nvn::TextureFlags::DISPLAY);

        nvn::Texture * renderTargets[ITF_ARRAY_SIZE(m_renderTargets)];
        String8 debugName;
        for (auto & rt : m_renderTargets)
        {
            debugName.setTextFormat("MainRT_%d", distInRange(m_renderTargets, rt));
            rt = NVN::makeUnique<NVNRenderTarget>(*m_textureDescriptorPool, builder, debugName.cStr());
            renderTargets[distInRange(m_renderTargets, rt)] = &rt->getPlatfomTexture();
        }

        nvn::WindowBuilder windowBuilder;
        windowBuilder.SetDefaults()
            .SetDevice(m_nvnDevice.get())
            .SetNativeWindow(m_nativeWindow)
            .SetTextures(ITF_ARRAY_SIZE(renderTargets), renderTargets)
            .SetPresentInterval(m_waitVBL ? 1 : 0);
        m_window = NVN::makeBuilderInitialized<nvn::Window>(windowBuilder);
        m_window->SetCrop(0, 0, getScreenWidth(), getScreenHeight());
    }
} // namespace ITF
