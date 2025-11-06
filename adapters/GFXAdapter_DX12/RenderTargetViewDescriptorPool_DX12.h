#pragma once

#include "DX12/Helpers.h"

#include "core/utility.h"
#include "core/Color.h"
#include "core/container/ExtendedVector.h"

namespace ITF::DX12
{
    class RenderTargetViewDescriptorPool
    {
    public:

        explicit RenderTargetViewDescriptorPool(ComPtr<ID3D12Device> _device, ux _nbMaxRTV = 64);
        RenderTargetViewDescriptorPool(const RenderTargetViewDescriptorPool&) = delete;
        ~RenderTargetViewDescriptorPool();

        RenderTargetViewDescriptorPool& operator = (const RenderTargetViewDescriptorPool&) = delete;

        using DescriptorIndex = ux;
        enum : ux { INVALID_DESCRIPTOR_INDEX = UX_INVALID };
        [[nodiscard]] DescriptorIndex acquire(ID3D12Resource * _renderTarget); // return the descriptor index (to give at setRenderTarget)
        void release(DescriptorIndex _descriptorIdx); // release it (for instance at the same time as the destruction of the render target)

        class Handle
        {
        public:
            Handle() = default;
            explicit Handle(RenderTargetViewDescriptorPool* _pool, DescriptorIndex _idx)
                : m_pool(_pool)
                , m_idx(_idx)
            {
            }

            Handle(const Handle &) = delete;
            Handle(Handle &&) noexcept;
            ~Handle();

            Handle& operator=(const Handle&) = delete;
            Handle& operator=(Handle&&) noexcept;

            void set(ID3D12GraphicsCommandList * _cmdList) const;
            void clear(ID3D12GraphicsCommandList* _cmdList, Color _c) const;

            void reset(); // equivalent to h = {}

            bool operator==(const Handle&) const;
            bool operator!=(const Handle & _other) const { return !(*this == _other); }

        private:
            
            RenderTargetViewDescriptorPool* m_pool = nullptr;
            DescriptorIndex m_idx = INVALID_DESCRIPTOR_INDEX;
        };

        [[nodiscard]] Handle acquireHandle(ID3D12Resource* _renderTarget); // return a handle that is movable and will release the index automatically

        void setRenderTarget(ID3D12GraphicsCommandList * cmdList, DescriptorIndex _descriptorIdx) const;
        void clearRenderTarget(ID3D12GraphicsCommandList* _cmdList, RenderTargetViewDescriptorPool::DescriptorIndex _descriptorIdx, Color _c) const;

        [[nodiscard]] ux getNbMaxDescriptor() const { return m_nbMaxRTV; }
        [[nodiscard]] ux getNbFreeDescriptor() const { return m_freeIndices.size(); }
        [[nodiscard]] ux getNbUsedDescriptor() const { return m_nbMaxRTV - getNbFreeDescriptor(); }

        [[nodiscard]] ID3D12Device* getDevice() const { return m_device.Get(); }

    private:
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getDescriptorHandle(ux _descriptorIdx) const;

        ComPtr<ID3D12Device> m_device{};
        ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle{};
        ux m_rtvDescriptorSize = 0;
        ux m_nbMaxRTV = 0;
        Vector<DescriptorIndex> m_freeIndices;
    };
}
