#pragma once

#include "NVN/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "TexturePool_NVN.h"

namespace ITF
{
    class NVNTexture
    {
    public:
        explicit NVNTexture(NVNTexturePool& _texPool, nvn::TextureBuilder _builder, const char * _debugName, NVN::VRAMResourceType _memType = NVN::VRAMResourceType::TEXTURE);
        NVNTexture(const NVNTexture&) = delete;
        ~NVNTexture() = default;

        NVNTexture & operator =(const NVNTexture&) = delete;

        static nvn::TextureBuilder createTextureBuilder(u32 _width, u32 _height, u32 _nbMipmap, nvn::Format _format);
        static NVNTexture* createFromMemoryXTX(NVNTexturePool& texPool, const u8* _srcRawData, u64 _rawDataSize, const char * _debugName); // memory of a XTX file (see XTX in NX/OUNCE doc)

        nvn::Texture& getPlatfomTexture() const { return *m_nvnTexture; }

        nvn::SeparateTextureHandle getTextureHandle() const
        {
            return m_texturePoolHandle.getNVNHandle();
        }

        void markAsUsedAtFrame(u64 _cpuFrameCount) { m_texturePoolHandle.markAsUsedAtFrame(_cpuFrameCount); }

        bool mapForUpload(void*& outPtr, u32& outRowPitch);
        void unmapAfterUpload();

    private:
        NVN::SimpleVRAMRelocHandle memHdl;
        NVN::UniquePtr<nvn::Texture> m_nvnTexture;
        NVNTexturePool::Handle m_texturePoolHandle;
    #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        String8 m_resName; // Buffer assigned to the texture object, do not change it after that.
    #endif

        void* m_uploadCpuPtr = nullptr;
        u32 m_uploadRowPitch = 0;
        u32 m_uploadWidth = 0;
        u32 m_uploadHeight = 0;
        u32 m_bytesPerPixel = 0;
        bool m_uploadInit = false;

        nvn::TextureView m_texView;
        bool m_texViewInit = false;
    };
}
