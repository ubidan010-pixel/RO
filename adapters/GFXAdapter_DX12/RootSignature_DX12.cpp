#include "precompiled_GFXAdapter_DX12.h"

#include "RootSignature_DX12.h"

#include "DX12/Helpers.h"

namespace ITF::DX12
{

    static ComPtr<ID3D12RootSignature> createRootSignatureFromBinary(ComPtr<ID3D12Device> _device, ComPtr<IDxcBlob> _binaryRootSignature)
    {
        ComPtr<ID3D12RootSignature> rootSignature;
        ITF_VERIFY_HR_CALL(_device->CreateRootSignature(
            0,
            _binaryRootSignature->GetBufferPointer(),
            _binaryRootSignature->GetBufferSize(),
            DX12_IID_COMPTR_ARGS(rootSignature))
        );
        return rootSignature;
    }

    static ComPtr<ID3D12RootSignatureDeserializer> createRootSignatureDeserializerFromBinary(ComPtr<IDxcBlob> _binaryRootSignature)
    {
        ComPtr<ID3D12RootSignatureDeserializer> rootSignatureDeserializer;
        ITF_VERIFY_HR_CALL(D3D12CreateRootSignatureDeserializer(
            _binaryRootSignature->GetBufferPointer(),
            _binaryRootSignature->GetBufferSize(),
            DX12_IID_COMPTR_ARGS(rootSignatureDeserializer))
        );
        return rootSignatureDeserializer;
    }


    RootSignature::RootSignature(ComPtr<ID3D12Device> _device, ComPtr<IDxcBlob> _binaryRootSignature)
        : m_rootSignature{ createRootSignatureFromBinary(_device, _binaryRootSignature) }
        , m_rootSignatureDescriptor{ createRootSignatureDeserializerFromBinary(_binaryRootSignature) }
    {
    }

}
