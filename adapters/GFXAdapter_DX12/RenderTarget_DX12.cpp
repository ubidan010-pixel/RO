#include "precompiled_GFXAdapter_DX12.h"

#include "RenderTarget_DX12.h"
#include "Texture_DX12.h"

namespace ITF::DX12
{
#if defined(ASSERT_ENABLED)
    // This is not very clean, but it helps to catch incorrect usage of render targets vs textures
    RenderTarget* gs_currentRenderTarget = nullptr;
#endif

    void RenderTarget::set(ID3D12GraphicsCommandList* _cmdList)
    {
        transitionToRenderTarget(_cmdList);
        m_descriptorHdl.set(_cmdList);
    #if defined(ASSERT_ENABLED)
        gs_currentRenderTarget = this;
    #endif
    }

    Texture& RenderTarget::createTextureView(TextureDescriptorPool& _texPool)
    {
        ITF_ASSERT(m_asTexture.getResource() == nullptr);
        if (m_asTexture.getResource() == nullptr)
        {
            m_asTexture = Texture::createFromRenderTarget(_texPool, *this);
        }
        return m_asTexture;
    }

    Texture* RenderTarget::transitionToTexture(ID3D12GraphicsCommandList* _cmdList)
    {
        ITF_ASSERT(m_asTexture.getResource() != nullptr);
        if (m_asTexture.getResource() == nullptr)
            return nullptr;
        transitionTo(_cmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        ITF_ASSERT_MSG(gs_currentRenderTarget != this, "Render target %s transition to texture while still being set as render target.", getDebugName());
        return &m_asTexture;
    }

    void RenderTarget::transitionTo(ID3D12GraphicsCommandList* _cmdList, D3D12_RESOURCE_STATES _newState)
    {
        ITF_ASSERT(m_resource != nullptr);
        if (m_currentState != _newState)
        {
            D3D12_RESOURCE_BARRIER barrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
                m_resource.Get(),
                m_currentState,
                _newState);
            _cmdList->ResourceBarrier(1, &barrierDesc);
            m_currentState = _newState;
        }
    }

#if defined(ASSERT_ENABLED)
    const Texture* RenderTarget::getTextureView() const
    {
        ITF_ASSERT(m_asTexture.getResource() != nullptr);
        if (m_asTexture.getResource() == nullptr)
            return nullptr;
        return &m_asTexture;
    }
#endif

    RenderTarget RenderTarget::createFromResource(ComPtr<ID3D12Resource> _resource, RenderTargetViewDescriptorPool& _descriptorPool, D3D12_RESOURCE_STATES _initialState, const char* _debugName)
    {
        RenderTarget result{};
        result.m_currentState = _initialState;

        RenderTargetViewDescriptorPool::Handle hdl = _descriptorPool.acquireHandle(_resource.Get());
        if (hdl != RenderTargetViewDescriptorPool::Handle{})
        {
            result.m_resource = std::move(_resource);
            result.m_descriptorHdl = std::move(hdl);
        }

    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        result.m_debugName = _debugName ? _debugName : "NoName";
        ITF_VERIFY_HR_CALL(result.m_resource->SetName(String8ToWChar(result.m_debugName).get()));
    #endif

        result.m_desc = result.m_resource->GetDesc();

        return result;
    }

    RenderTarget RenderTarget::createCommitted(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName)
    {
        ComPtr<ID3D12Resource> resource{};

        float colorAsArray[4]{ _clearColor.getBlue(), _clearColor.getGreen(), _clearColor.getRed(), _clearColor.getAlpha() };

        ITF_VERIFY_HR_CALL(_descriptorPool.getDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, _width, _height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, colorAsArray),
            DX12_IID_COMPTR_ARGS(resource)));

        return createFromResource(std::move(resource), _descriptorPool, D3D12_RESOURCE_STATE_RENDER_TARGET, _debugName);
    }

    RenderTarget RenderTarget::createDisplayable(u32 _width, u32 _height, Color _clearColor, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName)
    {
        ComPtr<ID3D12Resource> resource{};

        float colorAsArray[4]{ _clearColor.getBlue(), _clearColor.getGreen(), _clearColor.getRed(), _clearColor.getAlpha() };

        ITF_VERIFY_HR_CALL(_descriptorPool.getDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_ALLOW_DISPLAY,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, _width, _height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
            D3D12_RESOURCE_STATE_PRESENT,
            &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, colorAsArray),
            DX12_IID_COMPTR_ARGS(resource)));

        return createFromResource(std::move(resource), _descriptorPool, D3D12_RESOURCE_STATE_PRESENT, _debugName);
    }

}
