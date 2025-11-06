#include "precompiled_GFXAdapter_DX12.h"

#include "ShaderParameterMapping_DX12.h"

#include "SamplerDescriptorPool_DX12.h"
#include "Texture_DX12.h"

#include "engine/common/WarningManager.h"

namespace ITF::DX12
{
    static const char* gs_d3d12GlobalConstantBufferName = "$Globals";

    ShaderParameterMapping::~ShaderParameterMapping() = default;

    template <typename FxHdl, typename Binding>
    void ShaderParameterMapping::ParamMapElement<FxHdl, Binding>::copyToBuffer(
        ID3D12Resource * _uniformBuffer, u8 * _uniformBufferData, const FxParameterDB& _paramDB) const
    {
        i32 offsetInBuffer = bind.offset;
        i32 sizeInBuffer = bind.size;
        ITF_UNUSED(sizeInBuffer);
        ITF_ASSERT(offsetInBuffer >= 0);
        ITF_ASSERT(offsetInBuffer < _uniformBuffer->GetDesc().Width);
        ITF_ASSERT(offsetInBuffer + sizeInBuffer <= _uniformBuffer->GetDesc().Width);

        u8* data = _uniformBufferData + offsetInBuffer;

        if constexpr (std::is_same_v<FxHdl, FxParameterDB::Matrix4x4Hdl>)
        {
            const auto & matrix = _paramDB.getMatrix4x4(fxParamHdl);
            ITF_ASSERT(sizeof(matrix) == sizeInBuffer);
            M44_matrixTranspose(reinterpret_cast<Matrix44*>(data), &matrix); // Failed to set matrix as row major in the sahder, we do it here
        }
        else if constexpr (std::is_same_v<FxHdl, FxParameterDB::FloatHdl>)
        {
            auto val = _paramDB.getFloat(fxParamHdl);
            ITF_ASSERT(sizeof(val) == sizeInBuffer);
            *reinterpret_cast<f32*>(data) = val;
        }
        else if constexpr (std::is_same_v<FxHdl, FxParameterDB::IntHdl>)
        {
            auto val = _paramDB.getInt(fxParamHdl);
            ITF_ASSERT(sizeof(val) == sizeInBuffer);
            *reinterpret_cast<i32*>(data) = val;
        }
        else if constexpr (std::is_same_v<FxHdl, FxParameterDB::Float4Hdl>)
        {
            const auto & vector = _paramDB.getFloat4(fxParamHdl);
            ITF_ASSERT(sizeof(vector) == sizeInBuffer);
            *reinterpret_cast<Float4*>(data) = vector;
        }
        else if constexpr (std::is_same_v<FxHdl, FxParameterDB::Float4ArrayHdl>)
        {
            const Float4* float4Array = nullptr;
            u32 _nbFloat4 = 0;
            _paramDB.getFloat4Array(fxParamHdl, float4Array, _nbFloat4);
            // The array size provided can be less than the size in the buffer.
            i32 effectiveSizeToAssign = sizeof(Float4) * _nbFloat4;
            ITF_ASSERT(effectiveSizeToAssign <= sizeInBuffer);
            memcpy(data, float4Array, Min(effectiveSizeToAssign, sizeInBuffer));
        }
    }

    template <ShaderStage stage>
    void ShaderParameterMapping::applyTextureParams(ID3D12GraphicsCommandList* _rdrCtxt, const FxParameterDB& _paramDB)
    {
        for (auto& entry : m_texMapping)
        {
            ITF_ASSERT(_paramDB.getTexture(entry.fxParamHdl) != nullptr);
            _paramDB.getTexture(entry.fxParamHdl)->set(_rdrCtxt, entry.bind);
        }
    }

    template <ShaderStage stage>
    void ShaderParameterMapping::applySamplerParams(ID3D12GraphicsCommandList* _rdrCtxt, const FxParameterDB& _paramDB)
    {
        for (auto & entry : m_samplerMapping)
        {
            FxParameterDB::SamplerMode mode = _paramDB.getSamplerMode(entry.fxParamHdl);
            m_samplerPool->setSampler(_rdrCtxt, entry.bind, mode.wrapModeU, mode.wrapModeV, mode.linearFiltering);
        }
    }

    template <typename Mapping>
    void ShaderParameterMapping::copyUniformBufferParams(ID3D12Resource* _uniformBuffer, u8 * _uniformBufferData, const FxParameterDB& _paramDB, const Mapping& _mapping)
    {
        for (const auto & entry : _mapping)
        {
            entry.copyToBuffer(_uniformBuffer, _uniformBufferData, _paramDB);
        }
    }

    template <typename Mapping>
    void ShaderParameterMapping::checkCompatibilityWith(const Mapping& _mapping1, const Mapping& _mapping2) const
    {
        for (const auto& entry : _mapping1)
        {
            bool found = false;
            for (const auto& otherEntry : _mapping2)
            {
                if (entry.fxParamHdl == otherEntry.fxParamHdl)
                {
                    ITF_ASSERT(entry.bind == otherEntry.bind);
                    break;
                }
            }
        }
    }


    template <ShaderStage stage>
    void ShaderParameterMapping::applyParams(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        applyTextureParams<stage>(_rdrCtxt, _paramDB);
        applySamplerParams<stage>(_rdrCtxt, _paramDB);

        if (m_uniformBufferSlot == InvalidBufferSlot)
            return; // no uniform buffer slot bound for this stage, nothing to do.

        // reserve an area in the uniform buffer pool at the end of the command buffer for the constant buffer.
        // It will be reusable at the same time as the command buffer.
        ID3D12Resource* uniformBuffer = _uniformPool.allocUniformBuffer(m_uniformBufferSize);
        D3D12_RANGE range{};
        u8 * uniformBufferData = nullptr;
        ITF_VERIFY_HR_CALL(uniformBuffer->Map(0, &range, reinterpret_cast<void**>(& uniformBufferData)));

        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_matMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_floatMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_intMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_float4Mapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_float4ArrayMapping);

        _rdrCtxt->SetGraphicsRootConstantBufferView(m_uniformBufferSlot, uniformBuffer->GetGPUVirtualAddress());
    }

    // explicit instantiate
    template void ShaderParameterMapping::applyParams<ShaderStage::Vertex>(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);
    template void ShaderParameterMapping::applyParams<ShaderStage::Pixel>(ID3D12GraphicsCommandList* _rdrCtxt, UniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const SamplerDescriptorPool& _samplerPool, ID3D12ShaderReflection* _shaderReflection, const DX12::RootSignatureDescriptor& _rootSignatureDescriptor)
    {
        m_samplerPool = &_samplerPool;

        m_boundDBName = _paramDB.getName();
        if (m_boundDBName.isEmpty())
            m_boundDBName = gs_d3d12GlobalConstantBufferName;

        D3D12_SHADER_DESC shaderDesc{};
        _shaderReflection->GetDesc(&shaderDesc);

        bool success = true;
        for (ux i = 0; i < shaderDesc.BoundResources && success; ++i)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            _shaderReflection->GetResourceBindingDesc(i, &bindDesc);

            if (bindDesc.Type == D3D_SIT_TEXTURE)
            {
                success &= bindOneTexture(_paramDB, bindDesc, _rootSignatureDescriptor);
            }
            else if (bindDesc.Type == D3D_SIT_SAMPLER)
            {
                success &= bindOneSampler(_paramDB, bindDesc, _rootSignatureDescriptor);
            }
            else if (bindDesc.Type == D3D_SIT_CBUFFER)
            {
                if (m_boundDBName == bindDesc.Name)
                {
                    ID3D12ShaderReflectionConstantBuffer* uniformBuffer = _shaderReflection->GetConstantBufferByName(bindDesc.Name);
                    D3D12_SHADER_BUFFER_DESC cbDesc{};
                    uniformBuffer->GetDesc(&cbDesc);
                    m_uniformBufferSlot = _rootSignatureDescriptor.getConstantBufferSlot(bindDesc.BindPoint);
                    ITF_ASSERT_MSG(m_uniformBufferSlot != InvalidBufferSlot, "Failed to find root signature slot for constant buffer %s", bindDesc.Name);
                    m_uniformBufferSize = cbDesc.Size;
                    success &= bindUniformParameterDB(_paramDB, uniformBuffer);
                }
            }
        }

        return success;
    }

    void ShaderParameterMapping::checkCompatibilityWith(const ShaderParameterMapping& _other) const
    {
        ITF_ASSERT_MSG(m_uniformBufferSize == _other.m_uniformBufferSize,
            "Incompatible uniform buffer size between shader parameter mappings (%u vs %u)",
            m_uniformBufferSize, _other.m_uniformBufferSize);

    #if defined(ASSERT_ENABLED)
        checkCompatibilityWith(m_texMapping, _other.m_texMapping);
        checkCompatibilityWith(m_samplerMapping, _other.m_samplerMapping);
        checkCompatibilityWith(m_matMapping, _other.m_matMapping);
        checkCompatibilityWith(m_floatMapping, _other.m_floatMapping);
        checkCompatibilityWith(m_intMapping, _other.m_intMapping);
        checkCompatibilityWith(m_float4Mapping, _other.m_float4Mapping);
        checkCompatibilityWith(m_float4ArrayMapping, _other.m_float4ArrayMapping);
    #endif
    }

    bool ShaderParameterMapping::bindUniformParameterDB(const FxParameterDB& _paramDB, ID3D12ShaderReflectionConstantBuffer* _uniformBufferReflection)
    {
        bool success = true;
        D3D12_SHADER_BUFFER_DESC cbDesc{};
        _uniformBufferReflection->GetDesc(&cbDesc);
        for (UINT i = 0; i < cbDesc.Variables; ++i)
        {
            ID3D12ShaderReflectionVariable* variable = _uniformBufferReflection->GetVariableByIndex(i);
            success &= bindOneParameter(_paramDB, variable);
        }
        return success;
    }

    static String8 cleanPrefixIfFound(const String8& _prefix, const String8& _str)
    {
        if (_prefix.getLen() < _str.getLen())
        {
            if (_str.substr(0, _prefix.getLen()) == _prefix)
                return _str.substr(_prefix.getLen(), _str.getLen() - _prefix.getLen());
        }
        return _str;
    }

    bool ShaderParameterMapping::bindOneMatrix44Parameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;
        auto hdl = _paramDB.findMatrix4x4(_desc.Name);
        if (hdl.isValid())
        {
            ITF_ASSERT(_desc.Size == sizeof(Matrix44));
            m_matMapping.emplace_back(createParamMapElement(hdl, _desc.StartOffset, _desc.Size));
            bindFound = true;
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloatParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;
        auto hdl = _paramDB.findFloat(_desc.Name);
        if (hdl.isValid())
        {
            ITF_ASSERT(_desc.Size == sizeof(float));
            m_floatMapping.emplace_back(createParamMapElement(hdl, _desc.StartOffset, _desc.Size));
            bindFound = true;
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneIntParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;
        auto hdl = _paramDB.findInt(_desc.Name);
        if (hdl.isValid())
        {
            ITF_ASSERT(_desc.Size == sizeof(int));
            m_intMapping.emplace_back(createParamMapElement(hdl, _desc.StartOffset, _desc.Size));
            bindFound = true;
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloat4Parameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;
        auto hdl = _paramDB.findFloat4(_desc.Name);
        if (hdl.isValid())
        {
            ITF_ASSERT(_desc.Size == sizeof(float) * 4);
            m_float4Mapping.emplace_back(createParamMapElement(hdl, _desc.StartOffset, _desc.Size));
            bindFound = true;
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloat4ArrayParameter(const FxParameterDB& _paramDB, const D3D12_SHADER_VARIABLE_DESC& _desc, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;
        auto hdl = _paramDB.findFloat4Array(_desc.Name);
        if (hdl.isValid())
        {
            m_float4ArrayMapping.emplace_back(createParamMapElement(hdl, _desc.StartOffset, _desc.Size));
            bindFound = true;
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneParameter(const FxParameterDB& _paramDB, ID3D12ShaderReflectionVariable* _parameter)
    {
        bool bindFound = false;

        ID3D12ShaderReflectionType * type = _parameter->GetType();

        if (!type)
            return false;

        D3D12_SHADER_VARIABLE_DESC desc{};
        ITF_VERIFY_HR_CALL(_parameter->GetDesc(&desc));

        D3D12_SHADER_TYPE_DESC typeDesc{};
        ITF_VERIFY_HR_CALL(type->GetDesc(&typeDesc));

        bool isScalar = typeDesc.Class == D3D10_SVC_SCALAR;
        bool isMatrix = typeDesc.Class == D3D_SVC_MATRIX_COLUMNS;
        bool isVector = typeDesc.Class == D3D10_SVC_VECTOR;
        bool isArray = typeDesc.Elements != 0;
        bool isFloat = typeDesc.Type == D3D_SVT_FLOAT;

        if (isArray)
        {
            if (isVector && isFloat)
                bindFound = bindOneFloat4ArrayParameter(_paramDB, desc, _parameter);
        }
        else if (isMatrix)
        {
            if (isFloat && typeDesc.Columns == 4 && typeDesc.Rows == 4)
                bindFound = bindOneMatrix44Parameter(_paramDB, desc, _parameter);
        }
        else if (isVector)
        {
            if (isFloat && typeDesc.Columns == 4)
                bindFound = bindOneFloat4Parameter(_paramDB, desc, _parameter);
        }
        else if (isScalar)
        {
            if (isFloat)
                bindFound = bindOneFloatParameter(_paramDB, desc, _parameter);
            else if (typeDesc.Type == D3D_SVT_INT)
                bindFound = bindOneIntParameter(_paramDB, desc, _parameter);
        }

        if (!bindFound)
        {
            ITF_ASSERT_MSG(false, "Shader uniform buffer %s of class %d type %d is not handled.", desc.Name, typeDesc.Class, typeDesc.Type);
            ITF_WARNING(NULL, bfalse, "Shader uniform buffer %s of class %d type %d is not handled.",
                desc.Name,
                typeDesc.Class,
                typeDesc.Type);
        }
        return true;
    }


    bool ShaderParameterMapping::bindOneTexture(const FxParameterDB& _paramDB, const D3D12_SHADER_INPUT_BIND_DESC& _bindDesc, const DX12::RootSignatureDescriptor& _rootSignatureDescriptor)
    {
        bool bindFound = false;
        if (_bindDesc.BindCount == 1) // no support of texture arrays for now
        {
            auto hdl = _paramDB.findTexSampler(_bindDesc.Name);
            if (hdl.isValid())
            {
                u32 rootSigSlot = _rootSignatureDescriptor.getTextureSlot(_bindDesc.BindPoint);
                ITF_ASSERT_MSG(rootSigSlot != InvalidBufferSlot, "Invalid root signature slot for texture %s", _bindDesc.Name);
                m_texMapping.emplace_back(createParamMapElement(hdl, rootSigSlot));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneSampler(const FxParameterDB& _paramDB, const D3D12_SHADER_INPUT_BIND_DESC& _bindDesc, const DX12::RootSignatureDescriptor& _rootSignatureDescriptor)
    {
        bool bindFound = false;
        if (_bindDesc.BindCount == 1) // no support of sampler arrays for now
        {
            // we assume the sampler name starts with "samp" and then is coherent with the tex name.
            String8 cleanedName = cleanPrefixIfFound("samp", _bindDesc.Name);

            auto hdl = _paramDB.findTexSampler(cleanedName.cStr());
            ITF_ASSERT_MSG(hdl.isValid(), "Sampler %s not found, is there a different convention of sampler naming in the shaders ?", _bindDesc.Name);
            if (!hdl.isValid())
            {
                hdl = _paramDB.findTexSampler(_bindDesc.Name);
            }
            if (hdl.isValid())
            {
                u32 rootSigSlot = _rootSignatureDescriptor.getSamplerSlot(_bindDesc.BindPoint);
                ITF_ASSERT_MSG(rootSigSlot != InvalidBufferSlot, "Invalid root signature slot for sampler %s", _bindDesc.Name);
                m_samplerMapping.emplace_back(createParamMapElement(hdl, rootSigSlot));
                bindFound = true;
            }
        }
        return bindFound;
    }
}
