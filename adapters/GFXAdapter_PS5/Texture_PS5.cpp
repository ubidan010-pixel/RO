#include "precompiled_GFXAdapter_PS5.h"

#include <agc_texture_tool/Gnf.h>

#include "core/types.h"
#include "RenderTarget_PS5.h"
#include "Texture_PS5.h"

namespace ITF
{
    namespace Gnmp
    {
        void TextureRelocHandle::relocateAddress(VRAMAddress _vramAddr)
        {
            m_texture.setBaseAddress(_vramAddr);
        }

        VRAMAddress TextureRelocHandle::resolveAddress() const
        {
            return getAddress();
        }
    }

    void PS5Texture::initFromRenderTarget(const PS5RenderTarget & rt, bbool isCubemap)
    {
        memHdl = rt.baseMemHdl;
        Gnmp::Texture::initFromRenderTarget(&rt, isCubemap);
        ITF_ASSERT(memHdl.getAddress() == rt.baseMemHdl.getAddress());
    }

    void PS5Texture::initFromMemoryGNF(const u8 * _srcRawData)
    {
        const sce::Gnf::GnfFileV5 * gnfFileInMemory = reinterpret_cast<const sce::Gnf::GnfFileV5*>(_srcRawData);

        ITF_ASSERT_CRASH(gnfFileInMemory->header.m_magicNumber == sce::Gnf::kMagic, "GNF magic data in the header is not valid");
        ITF_ASSERT_CRASH(gnfFileInMemory->contents.m_version == sce::Gnf::kVersion, "Unknown GNF format");

        if (gnfFileInMemory->header.m_magicNumber != sce::Gnf::kMagic || gnfFileInMemory->contents.m_version != sce::Gnf::kVersion)
        {
            return;
        }

        ITF_ASSERT(gnfFileInMemory->contents.m_numTextures == 1); // we take only the first one anyway, so it should not crash

        const ux idTexture = 0;

        // Copy the texture agc object data.
        this->m_agcTexture = gnfFileInMemory->m_tSharps[idTexture];
        // GNF use the metadata address (and the address) to store the information of texture size and alignment
        // The address will be assigned by the TextureRelocHandle but not the metadata address.
        // We need to clean it, otherwise the resource registration (at least) will complain.
        // This is done by sce::Gnf::patchTexturesV5 or Agx::translate in samples, but here gnfFileInMemory is const...
        this->m_agcTexture.setMetadataAddress(nullptr);

        // Ask Gnf for the right size/alignment (we do not touched gnfFileInMemory, so metadata are still there)
        Gnmp::SizeAlign texSizeAligned;
        ITF_VERIFY_SCE_CALL(sce::Gnf::getTextureDataSize(&texSizeAligned, gnfFileInMemory, idTexture));
        memHdl.alloc(texSizeAligned, Gnmp::VRAM_Texture);

        size_t offset = 0;
        ITF_VERIFY_SCE_CALL(sce::Gnf::getTextureDataOffset(&offset, gnfFileInMemory, idTexture));

        ITF_Memcpy(memHdl.getAddress(), _srcRawData + offset, texSizeAligned.m_size);
    }

    void PS5Texture::initAs2d(u32 width, u32 height, u32 numMipLevels, Gnmp::DataFormat format, sce::Agc::Core::Texture::TileMode tileMode)
    {
        Gnmp::TextureSpec spec{};
        spec.init();
        spec.m_width = width;
        spec.m_height = height;
        spec.m_numMipLevels = numMipLevels;
        spec.m_format = format;
        spec.m_tileModeHint = tileMode;
        ITF_VERIFY_SCE_CALL(this->init(&spec));
    }
}

