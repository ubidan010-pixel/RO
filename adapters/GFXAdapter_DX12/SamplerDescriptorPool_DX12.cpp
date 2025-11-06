#include "precompiled_GFXAdapter_DX12.h"

#include "DX12/Helpers.h"
#include "ITFToDX12_Helpers.h"

#include "SamplerDescriptorPool_DX12.h"

namespace ITF::DX12
{
    SamplerDescriptorPool::SamplerDescriptorPool(ComPtr<ID3D12Device> _device)
        : m_device(std::move(_device))
        , m_descriptorSize(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
    {
        D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
        samplerHeapDesc.NumDescriptors = NB_SAMPLERS;
        samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        ITF_VERIFY_HR_CALL(m_device->CreateDescriptorHeap(&samplerHeapDesc, DX12_IID_COMPTR_ARGS(m_heap)));

        if (m_heap != nullptr)
        {
            initializeSamplers();
            m_heapHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
            m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }
    }

    void SamplerDescriptorPool::initializeSamplers()
    {
        auto initDescriptorEntry = [&](bool _filter, GFX_TEXADRESSMODE _wrapX, GFX_TEXADRESSMODE _wrapY)
            {
                D3D12_SAMPLER_DESC samplerDesc = {};
                samplerDesc.Filter = itfToDX12Filter(_filter);
                samplerDesc.AddressU = itfToDX12(_wrapX);
                samplerDesc.AddressV = itfToDX12(_wrapY);
                samplerDesc.AddressW = itfToDX12(_wrapY);
                samplerDesc.MinLOD = 0;
                samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
                samplerDesc.MipLODBias = 0.0f;
                samplerDesc.MaxAnisotropy = 1;
                uint32_t idx = computeSamplerIndex(_wrapX, _wrapY, _filter);
                D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
                cpuHandle.ptr += idx * m_descriptorSize;
                m_device->CreateSampler(&samplerDesc, cpuHandle);
            };

        for (i32 iX = 0; iX < GFXTADDRESS_COUNT; ++iX)
        {
            GFX_TEXADRESSMODE wrapX = static_cast<GFX_TEXADRESSMODE>(iX);
            for (i32 iY = 0; iY < GFXTADDRESS_COUNT; ++iY)
            {
                GFX_TEXADRESSMODE wrapY = static_cast<GFX_TEXADRESSMODE>(iY);

                initDescriptorEntry(false, wrapX, wrapY);
                initDescriptorEntry(true, wrapX, wrapY);
            }
        }
    }

    void SamplerDescriptorPool::setSampler(ID3D12GraphicsCommandList* _cmdList, u32 _textureSlot, GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool _isBilinear) const
    {
        i32 idx = computeSamplerIndex(_clampX, _clampY, _isBilinear);
        D3D12_GPU_DESCRIPTOR_HANDLE hdl = m_heapHandle;
        hdl.ptr += idx * m_descriptorSize;
        _cmdList->SetGraphicsRootDescriptorTable(_textureSlot, hdl);
    }

}
