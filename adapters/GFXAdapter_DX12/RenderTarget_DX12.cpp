#include "precompiled_GFXAdapter_DX12.h"

#include "RenderTarget_DX12.h"
#include "Texture_DX12.h"

namespace ITF::DX12
{
    Texture& RenderTarget::getOrCreateAsTexture(TextureDescriptorPool& _texPool)
    {
        if (m_asTexture.getResource() == nullptr)
        {
            m_asTexture = Texture::createFromRenderTarget(_texPool, *this);
        }
        return m_asTexture;
    }

    void RenderTarget::syncToUseAsTexture(ID3D12GraphicsCommandList* _cmdList)
    {
        ITF_ASSERT(m_resource != nullptr);
        D3D12_RESOURCE_BARRIER barrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
            m_resource.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        _cmdList->ResourceBarrier(1, &barrierDesc);
    }

    RenderTarget RenderTarget::createFromResource(ComPtr<ID3D12Resource> _resource, RenderTargetViewDescriptorPool& _descriptorPool, const char* _debugName)
    {
        RenderTarget result{};
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

        return createFromResource(std::move(resource), _descriptorPool, _debugName);
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

        return createFromResource(std::move(resource), _descriptorPool, _debugName);
    }

}
