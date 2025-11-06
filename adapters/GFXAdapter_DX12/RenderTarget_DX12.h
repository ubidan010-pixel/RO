#pragma once

#include "DX12/Helpers.h"
#include "core/Color.h"
#include "RenderTargetViewDescriptorPool_DX12.h"
#include "Texture_DX12.h"
#include <type_traits>

namespace ITF::DX12
{
    class RenderTarget
    {
    public:

        RenderTarget() = default;
        ~RenderTarget() = default;

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget(RenderTarget&&) = default;
        RenderTarget& operator = (const RenderTarget&) = delete;
        RenderTarget& operator = (RenderTarget&&) = default;

        void set(ID3D12GraphicsCommandList * _cmdList)
        {
            m_descriptorHdl.set(_cmdList);
        }

        void clear(ID3D12GraphicsCommandList* _cmdList, Color _c)
        {
            m_descriptorHdl.clear(_cmdList, _c);
        }

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        [[nodiscard]] const char* getDebugName() const { return m_debugName.cStr(); }
    #else
        [[nodiscard]] const char* getDebugName() const { return "NoName"; }
    #endif

        Texture& getOrCreateAsTexture(TextureDescriptorPool& _texPool);

        [[nodiscard]] Texture* getAsTexture() { return m_asTexture.getResource() != nullptr ? &m_asTexture : nullptr; }

        void syncToUseAsTexture(ID3D12GraphicsCommandList* _cmdList);

        [[nodiscard]] ID3D12Resource* getResource() const { return m_resource.Get(); }

        [[nodiscard]] const D3D12_RESOURCE_DESC& getDesc() const { return m_desc; };

        static [[nodiscard]] RenderTarget createFromResource(ComPtr<ID3D12Resource> _resource, RenderTargetViewDescriptorPool & _descriptorPool, const char * _debugName);
        static [[nodiscard]] RenderTarget createCommitted(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName);
    #if !defined(ITF_WIN64)
        // On windows, we use swapchain buffers as render targets for displayable RTs
        static [[nodiscard]] RenderTarget createDisplayable(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName);
    #endif


    private:
        ComPtr<ID3D12Resource> m_resource{};
        RenderTargetViewDescriptorPool::Handle m_descriptorHdl{};
        D3D12_RESOURCE_DESC m_desc{};
        Texture m_asTexture{};
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        String8 m_debugName{};
    #endif
    };

    static_assert(std::is_default_constructible_v<RenderTarget>, "RenderTarget should be default constructible");
    static_assert(std::is_move_constructible_v<RenderTarget>, "RenderTarget should be movable");
    static_assert(std::is_move_assignable_v<RenderTarget>, "RenderTarget should be movable");
}
