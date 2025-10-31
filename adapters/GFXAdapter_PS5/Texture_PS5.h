#ifndef ITF_TEXTURE_PS5_H_
#define ITF_TEXTURE_PS5_H_

#pragma once

#include "Gnmp_Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

namespace ITF
{
    namespace Gnmp
    {
        class TextureRelocHandle : public VRAMRelocHandle
        {
        public:
            TextureRelocHandle(Gnmp::Texture & _texture)
                : m_texture(_texture)
            {
                relocateAddress(nullptr);
            }

            void operator = (const VRAMRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator = (const TextureRelocHandle & hdl) { VRAMRelocHandle::operator = (hdl); }
            void operator += (VRAMChunkSize offset) { VRAMRelocHandle::operator += (offset); }

            VRAMAddress getAddress() const { return static_cast<VRAMAddress>(m_texture.getBaseAddress()); }

        protected:
            virtual void relocateAddress(VRAMAddress vramAddr);
            virtual VRAMAddress resolveAddress() const;

        private:
            Gnmp::Texture & m_texture;
        };

    }

    class PS5Texture : public Gnmp::Texture
    {
    public:

        PS5Texture()
            : Gnmp::Texture{}
            , memHdl(*this)
            , m_resHandler(&this->m_agcTexture)
        {}

        void initFromRenderTarget(const class PS5RenderTarget & rt, bbool isCubemap = bfalse);

        void initFromMemoryGNF(const u8 * _srcRawData);
        void initAs2d(u32 width, u32 height, u32 numMipLevels, Gnmp::DataFormat format, sce::Agc::Core::Texture::TileMode tileMode);

        Gnmp::TextureRelocHandle memHdl;
        Gnmp::AutoResourceHandler<sce::Agc::Core::Texture> m_resHandler;

    private:

        // The base address is changed only by the memHdl. Methods let undefined in order to get link errors.
        void setBaseAddress(void *baseAddr);
        void setBaseAddress256ByteBlocks(u32 baseAddr256);
        void initFromRenderTarget(const Gnmp::RenderTarget *rt, bool isCubemap);
    };

}


#endif // ITF_TEXTURE_PS5_H_
