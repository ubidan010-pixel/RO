#pragma once

#include "DX12/Helpers.h"
#include "VRAMAllocator/VRAMRelocHdl.h"

#include "TextureDescriptorPool_DX12.h"
#include "UploadBufferRequestManager_DX12.h"

namespace ITF::DX12
{
    class RenderTarget;

    class Texture
    {
    public:

        Texture();
        Texture(const Texture&) = delete;
        Texture(Texture&&) = default;
        ~Texture() = default;

        Texture & operator =(const Texture&) = delete;
        Texture & operator =(Texture&&) = default;

        //static DX12Texture createFromMemoryXBTC(DX12TextureDescriptorPool& _texPool, const u8* _srcRawData, u64 _rawDataSize, const char * _debugName); // memory of a XBTC file
        static [[nodiscard]] Texture createFromMemoryDDS(TextureDescriptorPool& _texPool, UploadBufferRequestManager & _uploadMgr, const u8* _srcRawData, u64 _rawDataSize, const char* _debugName);
        static [[nodiscard]] Texture createFromRenderTarget(TextureDescriptorPool& _texPool, RenderTarget& _renderTarget);
        static [[nodiscard]] Texture createCommitted(ComPtr<ID3D12Device> _device, TextureDescriptorPool& _texPool, u32 _width, u32 _height, DXGI_FORMAT _format, D3D12_HEAP_TYPE _heapType, const char* _debugName);

        void markAsUsedAtFrame(u64 _cpuFrameCount) { m_descriptorHandle.markAsUsedAtFrame(_cpuFrameCount); }

        [[nodiscard]] ID3D12Resource* getResource() const { return m_resource.Get(); }
        [[nodiscard]] const D3D12_RESOURCE_DESC & getDesc() const { return m_desc; };

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        [[nodiscard]] const char* getDebugName() const { return m_debugName.cStr(); };
    #else
        [[nodiscard]] const char* getDebugName() const { return "NoName"; };
    #endif

        void set(ID3D12GraphicsCommandList* _rdrCtx, u32 _slot)
        {
            m_descriptorHandle.set(_rdrCtx, _slot);
        }

        [[nodiscard]] u64 getUploadFenceValue() const { return m_uploadFence; }

        void ensureUploadDone(UploadBufferRequestManager& _uploadManager)
        {
            /*
            if (m_uploadFence != 0)
            {
                _uploadManager.waitFence(m_uploadFence);
                m_uploadFence = 0;
            }
            */
        }

    private:
        DefaultResourceBufferVRAMRelocHandle m_memHdl{};
        u64 m_uploadFence = 0;
        ComPtr<ID3D12Resource> m_resource{};
        D3D12_RESOURCE_DESC m_desc{};
        TextureDescriptorPool::Handle m_descriptorHandle{};
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        String8 m_debugName{};
    #endif
    };
}
