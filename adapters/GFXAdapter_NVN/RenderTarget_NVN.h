#pragma once

#include "NVN/Helpers.h"
#include "Texture_NVN.h"

#include "TexturePool_NVN.h"

namespace ITF
{
    class NVNTexture;

    class NVNRenderTarget : public NVNTexture
    {
    public:

        static nvn::TextureBuilder simpleRenderTargetBuilder(nvn::Device & _device, u32 _width, u32 _height, nvn::TextureFlags _flags = nvn::TextureFlags::COMPRESSIBLE);

        explicit NVNRenderTarget(NVNTexturePool & _texPool, nvn::TextureBuilder _builder, const char* _debugName);
        ~NVNRenderTarget() = default;

        NVNRenderTarget(const NVNRenderTarget &) = delete;
        NVNRenderTarget & operator = (const NVNRenderTarget&) = delete;
    };
}
