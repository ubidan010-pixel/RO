#pragma once

#ifdef ITF_WIN64
#include <d3d12shader.h>
#elif defined(ITF_XBOX_SERIES)
#include <d3d12shader_xs.h>
#else
#include <d3d12shader_x.h>
#endif

#include "FxParameterDB_DX12.h"

#include "DX12/Helpers.h"

#include "UniformBufferPool_DX12.h"

#include "RootSignatureDescriptor_DX12.h"

namespace ITF::DX12
{

    class VertexProgram;
    class PixelProgram;
    class SamplerDescriptorPool;

    // Note that default value of uniforms is not supported right now (at least on vertex shaders)
    class ShaderParameterMapping
    {
    public:
        ShaderParameterMapping() = default;
        ShaderParameterMapping(const ShaderParameterMapping&) = delete;
        ~ShaderParameterMapping();

        ShaderParameterMapping & operator=(const ShaderParameterMapping&) = delete;

        bool bindParameterDB(const FxParameterDB & _paramDB, const SamplerDescriptorPool& _samplerPool, ID3D12ShaderReflection* _shaderReflection, const DX12::RootSignatureDescriptor& _rootSigDesc);

        [[nodiscard]] const char * getBoundDBName() const { return m_boundDBName.cStr(); }

        [[nodiscard]] u32 getUniformBufferSize() const { return m_uniformBufferSize; }
        [[nodiscard]] u32 getNbTextureSampler() const { return m_samplerMapping.size(); }

        template <ShaderStage stage>
        void applyParams(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

        void checkCompatibilityWith(const ShaderParameterMapping& _other) const;

    private:

        bool bindUniformParameterDB(const FxParameterDB& _paramDB, ID3D12ShaderReflectionConstantBuffer* _uniformReflection);
        bool bindOneParameter(const FxParameterDB& _paramDB, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneMatrix44Parameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC & _desc, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneFloatParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneIntParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneFloat4Parameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneFloat4ArrayParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter);
        bool bindOneTexture(const FxParameterDB& _paramDB, const D3D12_SHADER_INPUT_BIND_DESC & _bindDesc, const RootSignatureDescriptor& _rootSignatureDescriptor);
        bool bindOneSampler(const FxParameterDB& _paramDB, const D3D12_SHADER_INPUT_BIND_DESC & _bindDesc, const RootSignatureDescriptor& _rootSignatureDescriptor);

        template <ShaderStage stage>
        void applyTextureParams(ID3D12GraphicsCommandList* _rdrCtxt, const FxParameterDB& _paramDB);
        template <ShaderStage stage>
        void applySamplerParams(ID3D12GraphicsCommandList* _rdrCtxt, const FxParameterDB& _paramDB);
        template <typename Mapping>
        void copyUniformBufferParams(ID3D12Resource * _uniformBuffer, u8* _uniformBufferData, const FxParameterDB& _paramDB, const Mapping& _mapping);
        template <typename Mapping>
        void checkCompatibilityWith(const Mapping& _mapping1, const Mapping& _mapping2) const;

        template <typename FxHdl, typename Binding>
        struct ParamMapElement
        {
            FxHdl fxParamHdl;
            Binding bind;

            void copyToBuffer(ID3D12Resource* _uniformBuffer, u8* _uniformBufferData, const FxParameterDB& _paramDB) const;
        };

        struct UniformBufferEntry
        {
            i32 offset; // byte offset
            i32 size; // byte size

            bool operator ==(const UniformBufferEntry& _other) const
            {
                return offset == _other.offset && size == _other.size;
            }

            bool operator !=(const UniformBufferEntry& _other) const
            {
                return !(*this == _other);
            }

        };

        template <typename FxHdl>
        ParamMapElement<FxHdl, UniformBufferEntry> createParamMapElement(FxHdl hdl, i32 offset, i32 size)
        {
            return { hdl, UniformBufferEntry{ offset, size } };
        }

        using ApiSlot = u32;

        template <typename FxHdl>
        ParamMapElement<FxHdl, ApiSlot> createParamMapElement(FxHdl hdl, ApiSlot _bind)
        {
            return { hdl, _bind };
        }

        using TexMapping = Vector<ParamMapElement<FxParameterDB::TexSamplerHdl, ApiSlot>>;
        using SamplerMapping = Vector<ParamMapElement<FxParameterDB::TexSamplerHdl, ApiSlot>>;
        using Mat44Mapping = Vector<ParamMapElement<FxParameterDB::Matrix4x4Hdl, UniformBufferEntry>>;
        using FloatMapping = Vector<ParamMapElement<FxParameterDB::FloatHdl, UniformBufferEntry>>;
        using IntMapping = Vector<ParamMapElement<FxParameterDB::IntHdl, UniformBufferEntry>>;
        using Float4Mapping = Vector<ParamMapElement<FxParameterDB::Float4Hdl, UniformBufferEntry>>;
        using Float4ArrayMapping = Vector<ParamMapElement<FxParameterDB::Float4ArrayHdl, UniformBufferEntry>>;

        const SamplerDescriptorPool * m_samplerPool = nullptr;

        TexMapping m_texMapping;
        SamplerMapping m_samplerMapping;
        Mat44Mapping m_matMapping;
        FloatMapping m_floatMapping;
        IntMapping m_intMapping;
        Float4Mapping m_float4Mapping;
        Float4ArrayMapping m_float4ArrayMapping;

        String8 m_boundDBName;
        static constexpr i32 InvalidBufferSlot = -1;
        i32 m_uniformBufferSlot = InvalidBufferSlot;
        u32 m_uniformBufferSize = 0;
    };
}
