#include "precompiled_GFXAdapter_DX12.h"

#include "Label_DX12.h"

#include <numeric>

namespace ITF::DX12
{

    // Manager

    LabelManager::LabelManager(ux _nbMaxLabel, ComPtr<ID3D12Device> _device)
        : m_nbMaxLabel(_nbMaxLabel)
    {
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_READBACK; // CPU-readable, GPU writes via copy/WriteBufferImmediate

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = m_nbMaxLabel * sizeof(PlatformLabel);
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        _device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            DX12_IID_COMPTR_ARGS(m_labelBuffer));

        m_freeIndices.resize(m_nbMaxLabel);
        std::iota(std::make_reverse_iterator(m_freeIndices.end()),
            std::make_reverse_iterator(m_freeIndices.begin()),
            0);

        void* rawDataPtr = nullptr;
        m_labelBuffer->Map(0, nullptr, reinterpret_cast<void**>(&rawDataPtr));
        m_labelArea = reinterpret_cast<PlatformLabel*>(rawDataPtr);
    }


    LabelManager::PlatformLabel* LabelManager::alloc()
    {
        if (m_freeIndices.empty())
        {
            ITF_ASSERT(0);
            return nullptr;
        }
        ux newIdx = m_freeIndices.back();
        m_freeIndices.pop_back();
        return m_labelArea + newIdx;
    }

    void LabelManager::free(LabelManager::PlatformLabel* _pLabel)
    {
        u32 index = u32(_pLabel - m_labelArea);
        ITF_ASSERT(index < uPtr(m_nbMaxLabel));
        // check it's not yet released
        ITF_ASSERT(m_freeIndices.end() == std::find(m_freeIndices.begin(), m_freeIndices.end(), index));
        m_freeIndices.push_back(index);
    }

    D3D12_GPU_VIRTUAL_ADDRESS LabelManager::gpuAddressOf(const PlatformLabel* p) const
    {
        const auto* cpuBase = reinterpret_cast<const u8*>(m_labelArea);
        const auto* cpuPtr = reinterpret_cast<const u8*>(p);
        const auto byteOffset = cpuPtr - cpuBase;
        return m_labelBuffer->GetGPUVirtualAddress() + byteOffset;
    }

    // Label

    void Label::initInternal(u32 _initialValue)
    {
        ITF_ASSERT(m_labelAddr == nullptr);
        m_labelAddr = LabelManager::get().alloc();
        if (m_labelAddr != nullptr)
        {
            *m_labelAddr = _initialValue;
            m_labelGpuAddr = LabelManager::get().gpuAddressOf(m_labelAddr);
        }
    }

    void Label::release()
    {
        if (m_labelAddr != nullptr)
        {
            LabelManager::get().free(m_labelAddr);
            m_labelAddr = nullptr;
            m_labelGpuAddr = {};
        }
    }

    void Label::setWriteOnEOP(ID3D12GraphicsCommandList2* commandList, u32 value)
    {
        ITF_ASSERT(commandList != nullptr);
        ITF_ASSERT(m_labelAddr != nullptr);

        D3D12_WRITEBUFFERIMMEDIATE_PARAMETER param = {};
        param.Dest = m_labelGpuAddr;
        param.Value = value;

        const D3D12_WRITEBUFFERIMMEDIATE_MODE mode = D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_OUT;

        commandList->WriteBufferImmediate(1, &param, &mode);
    }

} // ITF::DX12


