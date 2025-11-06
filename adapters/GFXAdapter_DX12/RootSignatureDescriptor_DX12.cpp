#include "precompiled_GFXAdapter_DX12.h"

#include "RootSignatureDescriptor_DX12.h"

#include "DX12/Helpers.h"

namespace ITF::DX12
{

    RootSignatureDescriptor::RootSignatureDescriptor(ComPtr<ID3D12RootSignatureDeserializer> _rootSignatureDeserializer)
    {
        const D3D12_ROOT_SIGNATURE_DESC * desc = _rootSignatureDeserializer->GetRootSignatureDesc();

        if (desc == nullptr)
        {
            ITF_ASSERT_MSG(0, "Failed to get root signature description from deserializer");
            return;
        }

        for (u32 i = 0; i < desc->NumParameters; ++i)
        {
            addSlot(i, desc->pParameters[i]);
        }
    }

    void RootSignatureDescriptor::addSlot(u32 slot, const D3D12_ROOT_PARAMETER& param)
    {
        auto assignSlot = [&](auto & vec, u32 _firstIdx, u32 _count)
            {
                size_t newSize = std::max<size_t>(vec.size(), _firstIdx + _count);
                vec.resize(newSize, InvalidSlot);
                for (uint32_t i = 0; i < _count; ++i)
                    vec[_firstIdx + i] = slot;
            };

        switch (param.ParameterType)
        {
        case D3D12_ROOT_PARAMETER_TYPE_CBV:
            assignSlot(m_constantBufferSlotsPerRegister, param.Constants.ShaderRegister, 1);
            break;
        case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
            {
                for (UINT i = 0; i < param.DescriptorTable.NumDescriptorRanges; ++i)
                {
                    const D3D12_DESCRIPTOR_RANGE& range = param.DescriptorTable.pDescriptorRanges[i];
                    if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
                    {
                        assignSlot(m_textureSlotsPerRegister, range.BaseShaderRegister, range.NumDescriptors);
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
                    {
                        assignSlot(m_samplerSlotsPerRegister, range.BaseShaderRegister, range.NumDescriptors);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "Unexpected ROOT SIGNATURE descriptor range type");
                    }
                }
            }
            break;
        case D3D12_ROOT_PARAMETER_TYPE_SRV:
        case D3D12_ROOT_PARAMETER_TYPE_UAV:
        default:
            {
                ITF_ASSERT_MSG(0, "Unexpected ROOT SIGNATURE parameter type");
            }
            break;
        }
    }
}
