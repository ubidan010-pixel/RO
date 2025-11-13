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

        Texture& createTextureView(TextureDescriptorPool& _texPool);

        void set(ID3D12GraphicsCommandList* _cmdList);

        void clear(ID3D12GraphicsCommandList* _cmdList, Color _c)
        {
            m_descriptorHdl.clear(_cmdList, _c);
        }

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        [[nodiscard]] const char* getDebugName() const { return m_debugName.cStr(); }
    #else
        [[nodiscard]] const char* getDebugName() const { return "NoName"; }
    #endif

        [[nodiscard]] Texture* transitionToTexture(ID3D12GraphicsCommandList* _cmdList);
        void transitionToCopySource(ID3D12GraphicsCommandList* _cmdList) { transitionTo(_cmdList, D3D12_RESOURCE_STATE_COPY_SOURCE); }
        void transitionToCopyDest(ID3D12GraphicsCommandList* _cmdList) { transitionTo(_cmdList, D3D12_RESOURCE_STATE_COPY_DEST); }
        void transitionToPresent(ID3D12GraphicsCommandList* _cmdList) { transitionTo(_cmdList, D3D12_RESOURCE_STATE_PRESENT); }

        [[nodiscard]] ID3D12Resource* getResource() const { return m_resource.Get(); }

        [[nodiscard]] const D3D12_RESOURCE_DESC& getDesc() const { return m_desc; };

    #if defined(ASSERT_ENABLED)
        // Allow to get the associated texture for debug purpose. In regular code, just use transitionToTexture.
        const Texture* RenderTarget::getTextureView() const;
    #endif

        static [[nodiscard]] RenderTarget createFromResource(ComPtr<ID3D12Resource> _resource, RenderTargetViewDescriptorPool & _descriptorPool, D3D12_RESOURCE_STATES _initialState, const char * _debugName);
        static [[nodiscard]] RenderTarget createCommitted(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName);
    #if !defined(ITF_WIN64)
        // On windows, we use swapchain buffers as render targets for displayable RTs
        static [[nodiscard]] RenderTarget createDisplayable(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName);
    #endif


    private:
        // transitionToRenderTarget is only called when set as render target
        void transitionToRenderTarget(ID3D12GraphicsCommandList* _cmdList) { transitionTo(_cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET); }
        void transitionTo(ID3D12GraphicsCommandList* _cmdList, D3D12_RESOURCE_STATES _newState);

        ComPtr<ID3D12Resource> m_resource{};
        RenderTargetViewDescriptorPool::Handle m_descriptorHdl{};
        D3D12_RESOURCE_DESC m_desc{};
        Texture m_asTexture{};
        D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        String8 m_debugName{};
    #endif
    };

    static_assert(std::is_default_constructible_v<RenderTarget>, "RenderTarget should be default constructible");
    static_assert(std::is_move_constructible_v<RenderTarget>, "RenderTarget should be movable");
    static_assert(std::is_move_assignable_v<RenderTarget>, "RenderTarget should be movable");
}
