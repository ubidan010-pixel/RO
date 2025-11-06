#pragma once

#ifdef ITF_WIN64
    #include <dxcapi.h>
#elif defined(ITF_XBOX_SERIES)
    #include <dxcapi_xs.h>
#else
    #include <dxcapi_x.h>
#endif

#include "core/types.h"

#include "DX12/Helpers.h"
#include "core/container/extendedVector.h"

namespace ITF::DX12
{
    class RootSignatureDescriptor
    {
    public:

        RootSignatureDescriptor() = default;
        explicit RootSignatureDescriptor(ComPtr<ID3D12RootSignatureDeserializer> _rootSignatureDeserializer);

        RootSignatureDescriptor(const RootSignatureDescriptor&) = default;
        RootSignatureDescriptor(RootSignatureDescriptor&&) = default;

        ~RootSignatureDescriptor() = default;

        RootSignatureDescriptor& operator=(const RootSignatureDescriptor&) = default;
        RootSignatureDescriptor& operator=(RootSignatureDescriptor&&) = default;


        enum : u32 { InvalidSlot = U32_INVALID };

        [[nodiscard]] u32 getConstantBufferSlot(u32 registerIdx) const
        {
            ITF_ASSERT(registerIdx < m_constantBufferSlotsPerRegister.size());
            if (registerIdx < m_constantBufferSlotsPerRegister.size())
                return m_constantBufferSlotsPerRegister[registerIdx];
            return InvalidSlot;
        }

        [[nodiscard]] u32 getTextureSlot(u32 registerIdx) const
        {
            ITF_ASSERT(registerIdx < m_textureSlotsPerRegister.size());
            if (registerIdx < m_textureSlotsPerRegister.size())
                return m_textureSlotsPerRegister[registerIdx];
            return InvalidSlot;
        }

        [[nodiscard]] u32 getSamplerSlot(u32 registerIdx) const
        {
            ITF_ASSERT(registerIdx < m_samplerSlotsPerRegister.size());
            if (registerIdx < m_samplerSlotsPerRegister.size())
                return m_samplerSlotsPerRegister[registerIdx];
            return InvalidSlot;
        }

    private:

        void addSlot(u32 slot, const D3D12_ROOT_PARAMETER& param);

        Vector<u32> m_constantBufferSlotsPerRegister;
        Vector<u32> m_textureSlotsPerRegister;
        Vector<u32> m_samplerSlotsPerRegister;
    };
}
