#include "precompiled_GFXAdapter_NVN.h"

#include "adapters/GFXAdapter_NVN/RenderTarget_NVN.h"

#include "adapters/GFXAdapter_NVN/VRAMAllocator/VRAMAllocator.h"

namespace ITF
{

    nvn::TextureBuilder NVNRenderTarget::simpleRenderTargetBuilder(nvn::Device& _device, u32 _width, u32 _height, nvn::TextureFlags _flags)
    {
        nvn::TextureBuilder renderTargetBuilder;
        renderTargetBuilder.SetDefaults()
            .SetDevice(&_device)
            .SetTarget(nvn::TextureTarget::TARGET_2D)
            .SetSize2D(_width, _height)
            .SetLevels(1)
            .SetFlags(_flags)
            .SetFormat(nvn::Format::RGBA8);

        return renderTargetBuilder;
    }

    static void checkBuilder(const nvn::TextureBuilder& _builder)
    {
        ITF_ASSERT_MSG(_builder.GetStorageSize() > 0, "NVNRenderTarget: TextureBuilder has no storage size set");
        ITF_ASSERT_MSG(_builder.GetStorageAlignment() > 0, "NVNRenderTarget: TextureBuilder has no storage alignment set");
    }

    NVNRenderTarget::NVNRenderTarget(NVNTexturePool& _texPool, nvn::TextureBuilder _builder, const char* _debugName)
        : NVNTexture(_texPool, _builder, _debugName, NVN::VRAMResourceType::RENDER_TARGET)
    {
        checkBuilder(_builder);
    }
}

