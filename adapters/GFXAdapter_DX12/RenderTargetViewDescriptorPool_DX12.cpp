#include "precompiled_GFXAdapter_DX12.h"

#include "RenderTargetViewDescriptorPool_DX12.h"

#include "DX12/Helpers.h"

#include <numeric>

namespace ITF::DX12
{

    RenderTargetViewDescriptorPool::RenderTargetViewDescriptorPool(ComPtr<ID3D12Device> _device, ux _nbMaxRTV)
        : m_device(std::move(_device))
        , m_nbMaxRTV(_nbMaxRTV)
        , m_rtvDescriptorSize(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = m_nbMaxRTV;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ITF_VERIFY_HR_CALL(m_device->CreateDescriptorHeap(&rtvHeapDesc, DX12_IID_COMPTR_ARGS(m_descriptorHeap)));

        if (m_descriptorHeap != nullptr)
        {
            m_rtvHandle = D3D12_CPU_DESCRIPTOR_HANDLE{ m_descriptorHeap->GetCPUDescriptorHandleForHeapStart() };

            m_freeIndices.resize(m_nbMaxRTV);
            std::iota(std::make_reverse_iterator(m_freeIndices.end()),
                std::make_reverse_iterator(m_freeIndices.begin()),
                0);
        }
        else
        {
            m_nbMaxRTV = 0;
        }
    }

    RenderTargetViewDescriptorPool::~RenderTargetViewDescriptorPool()
    {
        ITF_ASSERT_CRASH(m_freeIndices.size() == m_nbMaxRTV, "Deleting DX12RenderTargetViewDescriptor while there are still render targets registered.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewDescriptorPool::getDescriptorHandle(ux _descriptorIdx) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE result = m_rtvHandle;
        result.ptr += _descriptorIdx * m_rtvDescriptorSize;
        return result;
    }


    RenderTargetViewDescriptorPool::DescriptorIndex RenderTargetViewDescriptorPool::acquire(ID3D12Resource * _renderTarget)
    {
        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "RTV descriptor allocation only allowed on main thread");
        ITF_ASSERT(_renderTarget != nullptr);
        ITF_ASSERT_CRASH(!m_freeIndices.empty(), "Out of RTV descriptors");
        ITF_ASSERT_CRASH(m_freeIndices.back()<m_nbMaxRTV, "RTV descriptor internal management error");
        if (m_freeIndices.empty())
            return INVALID_DESCRIPTOR_INDEX;
        ux idx = m_freeIndices.back();
        m_freeIndices.pop_back();
        m_device->CreateRenderTargetView(_renderTarget, nullptr, getDescriptorHandle(idx));
        return idx;
    }

    RenderTargetViewDescriptorPool::Handle RenderTargetViewDescriptorPool::acquireHandle(ID3D12Resource * _renderTarget)
    {
        DescriptorIndex idx = acquire(_renderTarget);
        if (idx != INVALID_DESCRIPTOR_INDEX)
            return Handle{this, idx};
        return {};
    }

    void RenderTargetViewDescriptorPool::release(RenderTargetViewDescriptorPool::DescriptorIndex _idx)
    {
        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "RTV descriptor allocation only allowed on main thread");
        ITF_ASSERT(_idx < m_nbMaxRTV);
        if (_idx < m_nbMaxRTV)
        {
            m_freeIndices.push_back(_idx);
        }
    }

    void RenderTargetViewDescriptorPool::reset()
    {
        ++m_generation;
        m_descriptorHeap.Reset();
        m_rtvHandle = {};
        m_rtvDescriptorSize = 0;

        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = (UINT)m_nbMaxRTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0;

        ITF_VERIFY_HR_CALL(m_device->CreateDescriptorHeap(&desc, DX12_IID_COMPTR_ARGS(m_descriptorHeap)));

        m_rtvHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        m_freeIndices.clear();
        m_freeIndices.reserve(m_nbMaxRTV);
        for (ux i = 0; i < m_nbMaxRTV; ++i)
            m_freeIndices.push_back((DescriptorIndex)i);
    }

    void RenderTargetViewDescriptorPool::setRenderTarget(ID3D12GraphicsCommandList* _cmdList, RenderTargetViewDescriptorPool::DescriptorIndex _descriptorIdx) const
    {
        auto rtvHandle = getDescriptorHandle(_descriptorIdx);
        _cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    }

    void RenderTargetViewDescriptorPool::clearRenderTarget(ID3D12GraphicsCommandList* _cmdList, RenderTargetViewDescriptorPool::DescriptorIndex _descriptorIdx, Color _c) const
    {
        auto rtvHandle = getDescriptorHandle(_descriptorIdx);
        const float clearColor[4] = { _c.getRed(), _c.getGreen(), _c.getBlue(), _c.getAlpha() };
        _cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    RenderTargetViewDescriptorPool::Handle::~Handle()
    {
        reset();
    }

    RenderTargetViewDescriptorPool::Handle::Handle(RenderTargetViewDescriptorPool::Handle && _other) noexcept
        : m_pool(_other.m_pool)
        , m_idx(_other.m_idx)
    {
        _other.m_pool = nullptr;
        _other.m_idx = RenderTargetViewDescriptorPool::INVALID_DESCRIPTOR_INDEX;
        _other.m_generation = 0;
    }

    RenderTargetViewDescriptorPool::Handle & RenderTargetViewDescriptorPool::Handle::operator=(RenderTargetViewDescriptorPool::Handle&& _other) noexcept
    {
        if (this == &_other)
            return *this;

        reset();

        m_pool = _other.m_pool;
        m_idx = _other.m_idx;
        m_generation = _other.m_generation;

        _other.m_pool = nullptr;
        _other.m_idx = INVALID_DESCRIPTOR_INDEX;
        _other.m_generation = 0;

        return *this;
    }

    bool RenderTargetViewDescriptorPool::Handle::operator==(const RenderTargetViewDescriptorPool::Handle & _other) const
    {
        return m_pool == _other.m_pool && m_idx == _other.m_idx;
    }

    void RenderTargetViewDescriptorPool::Handle::reset()
    {
        ITF_ASSERT((m_pool == nullptr) == (m_idx == INVALID_DESCRIPTOR_INDEX));
        if (m_pool != nullptr)
        {
            if (m_generation == m_pool->m_generation)
            {
                m_pool->release(m_idx);
            }
        }
        m_pool = nullptr;
        m_idx = INVALID_DESCRIPTOR_INDEX;
        m_generation = 0;
    }

    void RenderTargetViewDescriptorPool::Handle::set(ID3D12GraphicsCommandList* _cmdList) const
    {
        ITF_ASSERT(m_pool != nullptr);
        if (m_pool != nullptr)
        {
            m_pool->setRenderTarget(_cmdList, m_idx);
        }
    }

    void RenderTargetViewDescriptorPool::Handle::clear(ID3D12GraphicsCommandList* _cmdList, Color _c) const
    {
        ITF_ASSERT(m_pool != nullptr);
        if (m_pool != nullptr)
        {
            m_pool->clearRenderTarget(_cmdList, m_idx, _c);
        }
    }
}
