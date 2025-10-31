#include "precompiled_GFXAdapter_NVN.h"

#include "core/types.h"
#include "RenderTarget_NVN.h"
#include "Texture_NVN.h"
#include "TexturePool_NVN.h"

#include "adapters/GFXAdapter_NVN/VRAMAllocator/VRAMAllocator.h"

#include <nvnTool/texpkg/nvnTool_Fd.h>
#include <nvnTool/texpkg/nvnTool_HwTexture.h>

namespace ITF
{

    static const u8* headerToContentBegin(const nvnTool::texpkg::BlockHeader& h)
    {
        return reinterpret_cast<const u8*>(&h) + h.offset;
    }

    static const u8* headerToContentEnd(const nvnTool::texpkg::BlockHeader& h)
    {
        return headerToContentBegin(h) + h.blockSize;
    }

    nvn::TextureBuilder NVNTexture::createTextureBuilder(u32 _width, u32 _height, u32 _nbMipmap, nvn::Format _format)
    {
        nvn::TextureBuilder builder;
        builder.SetDefaults()
            .SetDevice(NVN::VRAMAllocator::getDevice())
            .SetTarget(nvn::TextureTarget::TARGET_2D)
            .SetSize2D(_width, _height)
            .SetLevels(_nbMipmap)
            .SetFormat(_format);
        return builder;
    }

    NVNTexture::NVNTexture(NVNTexturePool& _texPool, nvn::TextureBuilder _builder, const char* _debugName, NVN::VRAMResourceType _memType)
    {
        // Other memory types are nonsense for textures.
        ITF_ASSERT(_memType == NVN::VRAMResourceType::RENDER_TARGET || _memType == NVN::VRAMResourceType::TEXTURE);

        auto texSize = _builder.GetStorageSize();
        auto texAlign = _builder.GetStorageAlignment();

        // $GB : the following comment was in RL NX code. I do not know the reason, but getting a 4KiB alignment seems reasonable.
        // TX1 and GM2xx need this alignment for securing other resources which have different resource type and are placed neighbor.
        texAlign = (texAlign < 4 * 1024) ? 4 * 1024 : texAlign;

        if (_memType == NVN::VRAMResourceType::RENDER_TARGET)
            memHdl.allocAligned<NVN::VRAMResourceType::RENDER_TARGET>(texSize, texAlign, NVN::VRAM_Texture);
        else
            memHdl.allocAligned<NVN::VRAMResourceType::TEXTURE>(texSize, texAlign, NVN::VRAM_RenderTarget);

        auto storage = memHdl.getStorage();
        _builder.SetStorage(storage.memoryPool, storage.offset);
        m_nvnTexture = NVN::makeBuilderInitialized<nvn::Texture>(_builder);

        m_texturePoolHandle = _texPool.registerTexture(*this);

    #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        m_resName = _debugName;
        m_nvnTexture->SetDebugLabel(m_resName.cStr());
    #endif
    }

    NVNTexture* NVNTexture::createFromMemoryXTX(NVNTexturePool& texPool, const u8 * _srcRawData, u64 _rawDataSize, const char* _debugName)
    {
        auto& arcHeader = *reinterpret_cast<const struct nvnTool::texpkg::ArchiveHeader*>(_srcRawData);
        auto& blockHeader1 = *reinterpret_cast<const struct nvnTool::texpkg::BlockHeader*>(&arcHeader + 1);
        auto& nvTextureHeader = *reinterpret_cast<const struct nvnTool::texpkg::NVNHWTextureHeader*>(headerToContentBegin(blockHeader1));
        auto& blockHeader2 = *reinterpret_cast<const struct nvnTool::texpkg::BlockHeader*>(headerToContentEnd(blockHeader1));
        const void* textureData = headerToContentBegin(blockHeader2);
        auto& blockHeader3 = *reinterpret_cast<const struct nvnTool::texpkg::BlockHeader*>(headerToContentEnd(blockHeader2));
        ITF_UNUSED(blockHeader3);

        ITF_ASSERT(arcHeader.magic == NVNFD_ARCHIVE_MAGIC);
        ITF_ASSERT(blockHeader1.magic == NVNFD_BLOCKHEADER_MAGIC);
        ITF_ASSERT(blockHeader1.blockType == nvnTool::texpkg::NVNFD_BLOCK_TEXTURE_HEADER);
        ITF_ASSERT(blockHeader2.magic == NVNFD_BLOCKHEADER_MAGIC);
        ITF_ASSERT(blockHeader2.blockType == nvnTool::texpkg::NVNFD_BLOCK_TEXTURE);
        ITF_ASSERT(blockHeader3.magic == NVNFD_BLOCKHEADER_MAGIC);
        ITF_ASSERT(blockHeader3.blockType == nvnTool::texpkg::NVNFD_BLOCK_END);

        nvn::TextureBuilder builder = createTextureBuilder(
            nvTextureHeader.width,
            nvTextureHeader.height,
            nvTextureHeader.mipmapLevels,
            nvn::Format::Enum(nvTextureHeader.format));

        NVNTexture * nvnTex = new (MemoryId::mId_Textures) NVNTexture(texPool, builder, _debugName);
        ITF_ASSERT(nvTextureHeader.dataSize == nvnTex->m_nvnTexture->GetStorageSize());

        NVN::VRAMAllocator::GPUStorage gpuStorage = nvnTex->memHdl.getStorage();
        u8* destData = reinterpret_cast<u8*>(gpuStorage.memoryPool->Map()) + gpuStorage.offset;
        ITF_Memcpy(destData, textureData, nvTextureHeader.dataSize); // just copying is ok as the memory is CPU_UNCACHED or GPU coherent

        return nvnTex;
    }
}

