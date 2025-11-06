#pragma once

#include "core/types.h"
#include "core/utility.h"
#include "DX12/Helpers.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"

namespace ITF::DX12
{

    class SamplerDescriptorPool
    {
    public:

        explicit SamplerDescriptorPool(ComPtr<ID3D12Device> _device);

        SamplerDescriptorPool(const SamplerDescriptorPool &) = delete;

        ~SamplerDescriptorPool() = default;

        SamplerDescriptorPool& operator=(const SamplerDescriptorPool&) = delete;

        [[nodiscard]] ID3D12DescriptorHeap* getHeap() const // should be set in the command buffer
        {
            return m_heap.Get();
        }

        void setSampler(ID3D12GraphicsCommandList* _cmdList, u32 _textureSlot, GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool _isBilinear = true) const;

    private:

        void initializeSamplers();
        [[nodiscard]] i32 computeSamplerIndex(GFX_TEXADRESSMODE _clampX, GFX_TEXADRESSMODE _clampY, bool _isBilinear) const
        {
            return (_isBilinear ? 1 : 0) * GFXTADDRESS_COUNT * GFXTADDRESS_COUNT + (toUnderlying(_clampX) * GFXTADDRESS_COUNT) + toUnderlying(_clampY);
        }

        static const u32 NB_SAMPLERS = 2 * GFXTADDRESS_COUNT * GFXTADDRESS_COUNT;

        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12DescriptorHeap> m_heap{};
        D3D12_GPU_DESCRIPTOR_HANDLE m_heapHandle{};
        ux m_descriptorSize = 0;
    };
}
