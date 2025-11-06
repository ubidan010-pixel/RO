#pragma once

#include "core/types.h"

#include "core/memory/UniquePtr.h"

#include "DX12/Helpers.h"

#include "RootSignatureDescriptor_DX12.h"


namespace ITF::DX12
{
    class RootSignature
    {
    public:
        RootSignature() = default;
        explicit RootSignature(ComPtr<ID3D12Device> _device, ComPtr<IDxcBlob> _binaryRootSignature);

        RootSignature(const RootSignature&) = default;
        RootSignature(RootSignature&&) = default;

        ~RootSignature() = default;

        RootSignature& operator=(const RootSignature&) = default;
        RootSignature& operator=(RootSignature&&) = default;

        [[nodiscard]] const RootSignatureDescriptor& getDescriptor() const { return m_rootSignatureDescriptor; }

        [[nodiscard]] ID3D12RootSignature * get() const { return m_rootSignature.Get(); }

    private:
        ComPtr<ID3D12RootSignature> m_rootSignature{};
        RootSignatureDescriptor m_rootSignatureDescriptor;
    };
}
