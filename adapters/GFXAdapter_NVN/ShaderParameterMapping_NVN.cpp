#include "precompiled_GFXAdapter_NVN.h"

#include <variant>

#include "ShaderParameterMapping_NVN.h"
#include "engine/common/WarningManager.h"
#include "GLSLC/MetaData.h"

namespace ITF
{
#ifdef ITF_NX
    static const char* gs_glslcGlobalUniformBufferName = "type_$Globals.$Globals";
#else
    static const char* gs_glslcGlobalUniformBufferName = "type.$Globals.$Globals";
#endif

    ShaderParameterMapping::ShaderParameterMapping() = default;
    ShaderParameterMapping::~ShaderParameterMapping() = default;

    template <typename FxHdl, typename Binding>
    void ShaderParameterMapping::ParamMapElement<FxHdl, Binding>::copyToBuffer(
        nvn::Buffer& _uniformBuffer, u8 * _uniformBufferData, const FxParameterDB& _paramDB) const
    {
        i32 offsetInBuffer = bind.offset;
        i32 sizeInBuffer = bind.size;
        ITF_UNUSED(sizeInBuffer);
        ITF_ASSERT(offsetInBuffer >= 0);
        ITF_ASSERT(offsetInBuffer < _uniformBuffer.GetSize());
        ITF_ASSERT(offsetInBuffer + sizeInBuffer <= _uniformBuffer.GetSize());

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

    template <nvn::ShaderStage::Enum stage>
    void ShaderParameterMapping::applyTextureParams(nvn::CommandBuffer& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        for (auto& entry : m_texMapping)
        {
            ITF_ASSERT(_paramDB.getTexture(entry.fxParamHdl) != nullptr);
            _rdrCtxt.BindSeparateTexture(stage, entry.bind, _paramDB.getTextureHdl(entry.fxParamHdl));
        }
    }

    template <nvn::ShaderStage::Enum stage>
    void ShaderParameterMapping::applySamplerParams(nvn::CommandBuffer& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        for (auto & entry : m_samplerMapping)
        {
            _rdrCtxt.BindSeparateSampler(stage, entry.bind, _paramDB.getSamplerHdl(entry.fxParamHdl));
        }
    }

    template <typename Mapping>
    void ShaderParameterMapping::copyUniformBufferParams(nvn::Buffer& _uniformBuffer, u8 * _uniformBufferData, const FxParameterDB& _paramDB, const Mapping& _mapping)
    {
        for (const auto & entry : _mapping)
        {
            entry.copyToBuffer(_uniformBuffer, _uniformBufferData, _paramDB);
        }
    }


    template <nvn::ShaderStage::Enum stage>
    void ShaderParameterMapping::applyParams(nvn::CommandBuffer & _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB)
    {
        applyTextureParams<stage>(_rdrCtxt, _paramDB);
        applySamplerParams<stage>(_rdrCtxt, _paramDB);

        i32 uniformBufferSlot = stage == nvn::ShaderStage::FRAGMENT ? m_fragmentUniformBufferSlot : m_vertexUniformBufferSlot;
        if (uniformBufferSlot == InvalidBufferSlot)
            return; // no uniform buffer slot bound for this stage, nothing to do.

        // reserve an area in the uniform buffer pool at the end of the command buffer for the constant buffer.
        // It will be reusable at the same time as the command buffer.
        nvn::Buffer & uniformBuffer = _uniformPool.allocUniformBuffer(m_uniformBufferSize);

        u8* uniformBufferData = static_cast<u8*>(uniformBuffer.Map());

        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_matMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_floatMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_intMapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_float4Mapping);
        copyUniformBufferParams(uniformBuffer, uniformBufferData, _paramDB, m_float4ArrayMapping);

        _rdrCtxt.BindUniformBuffer(stage, uniformBufferSlot, uniformBuffer.GetAddress(), m_uniformBufferSize);
    }

    // explicit instantiate
    template void ShaderParameterMapping::applyParams<nvn::ShaderStage::VERTEX>(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);
    template void ShaderParameterMapping::applyParams<nvn::ShaderStage::FRAGMENT>(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool& _uniformPool, const FxParameterDB& _paramDB);

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const Vector<NVN::ShaderMetaData::UniformBlockDesc>& _blocks, const Vector<NVN::ShaderMetaData::UniformDesc>& _uniforms)
    {
        m_boundDBName = _paramDB.getName();
        if (m_boundDBName.isEmpty())
            m_boundDBName = gs_glslcGlobalUniformBufferName;

        auto itBlock = std::find_if(_blocks.begin(), _blocks.end(), [&](const NVN::ShaderMetaData::UniformBlockDesc& blockDesc)
        {
            return (blockDesc.name == m_boundDBName);
        });

        i32 uniformBufferBlockIndex = (itBlock == _blocks.end()) ? -1 : u32(itBlock - _blocks.begin());

        if (uniformBufferBlockIndex != -1)
        {
            m_fragmentUniformBufferSlot = itBlock->fragmentBlockBinding;
            m_vertexUniformBufferSlot = itBlock->vertexBlockBinding;
            m_uniformBufferSize = itBlock->size;
        }

        bool success = true;
        for (auto& uniformDesc : _uniforms)
        {
            if (uniformDesc.kind == GLSLC_PIQ_UNIFORM_KIND_PLAIN && uniformDesc.blockNdx != -1 && uniformDesc.blockNdx == uniformBufferBlockIndex)
            {
                success &= bindOneParameter(_paramDB, m_boundDBName, uniformDesc);
            }
            else if (uniformDesc.kind == GLSLC_PIQ_UNIFORM_KIND_SAMPLER)
            {
                success &= bindOneSampler(_paramDB, uniformDesc);
            }
            else if (uniformDesc.kind == GLSLC_PIQ_UNIFORM_KIND_TEXTURE)
            {
                success &= bindOneTexture(_paramDB, uniformDesc);
            }
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

    bool ShaderParameterMapping::bindOneMatrix44Parameter(const FxParameterDB& _paramDB, const String8 & _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc)
    {
        bool bindFound = false;
        if (!uniformDesc.isArray)
        {
            auto hdl = _paramDB.findMatrix4x4(_uniformName.cStr());
            if (hdl.isValid())
            {
                m_matMapping.emplace_back(createParamMapElement(hdl, uniformDesc.blockOffset, sizeof(Matrix44)));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloatParameter(const FxParameterDB& _paramDB, const String8 & _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc)
    {
        bool bindFound = false;
        if (!uniformDesc.isArray)
        {
            auto hdl = _paramDB.findFloat(_uniformName.cStr());
            if (hdl.isValid())
            {
                m_floatMapping.emplace_back(createParamMapElement(hdl, uniformDesc.blockOffset, sizeof(float)));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneIntParameter(const FxParameterDB& _paramDB, const String8 & _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc)
    {
        bool bindFound = false;
        if (!uniformDesc.isArray)
        {
            auto hdl = _paramDB.findInt(_uniformName.cStr());
            if (hdl.isValid())
            {
                m_intMapping.emplace_back(createParamMapElement(hdl, uniformDesc.blockOffset, sizeof(int)));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloat4Parameter(const FxParameterDB& _paramDB, const String8 & _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc)
    {
        bool bindFound = false;
        if (!uniformDesc.isArray)
        {
            auto hdl = _paramDB.findFloat4(_uniformName.cStr());
            if (hdl.isValid())
            {
                m_float4Mapping.emplace_back(createParamMapElement(hdl, uniformDesc.blockOffset, sizeof(Float4)));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneFloat4ArrayParameter(const FxParameterDB& _paramDB, const String8 & _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc)
    {
        bool bindFound = false;
        if (uniformDesc.isArray)
        {
            auto hdl = _paramDB.findFloat4Array(_uniformName.cStr());
            if (hdl.isValid())
            {
                ITF_ASSERT(uniformDesc.arrayStride == sizeof(Float4));
                m_float4ArrayMapping.emplace_back(createParamMapElement(hdl, uniformDesc.blockOffset, uniformDesc.sizeOfArray * uniformDesc.arrayStride));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneParameter(const FxParameterDB& _paramDB, const String8& _blockName, const NVN::ShaderMetaData::UniformDesc& _uniformDesc)
    {
        bool bindFound = false;

        // As a convention, every uniform entry name start with the block name followed by a dot.
        String8 uniformName = cleanPrefixIfFound(_blockName + ".", _uniformDesc.name);

        switch (_uniformDesc.type)
        {
            case GLSLC_PIQ_TYPE_MAT4:
            {
                bindFound = bindOneMatrix44Parameter(_paramDB, uniformName, _uniformDesc);
                break;
            }
            case GLSLC_PIQ_TYPE_FLOAT:
            {
                bindFound = bindOneFloatParameter(_paramDB, uniformName, _uniformDesc);
                break;
            }
            case GLSLC_PIQ_TYPE_INT:
            {
                bindFound = bindOneIntParameter(_paramDB, uniformName, _uniformDesc);
                break;
            }
            case GLSLC_PIQ_TYPE_FLOAT_VEC4:
            {
                if (_uniformDesc.isArray)
                    bindFound = bindOneFloat4ArrayParameter(_paramDB, uniformName, _uniformDesc);
                else
                    bindFound = bindOneFloat4Parameter(_paramDB, uniformName, _uniformDesc);
                break;
            }
            default:
                break;
        }
        if (!bindFound)
        {
            ITF_WARNING(NULL, bfalse, "Shader uniform buffer %s of type %d is not handled.",
                _uniformDesc.name.cStr(),
                _uniformDesc.type);
        }
        return true;
    }


    bool ShaderParameterMapping::bindOneTexture(const FxParameterDB& _paramDB, const NVN::ShaderMetaData::UniformDesc& _uniformDesc)
    {
        bool bindFound = false;
        if (!_uniformDesc.isArray)
        {
            auto hdl = _paramDB.findTexSampler(_uniformDesc.name.cStr());
            if (hdl.isValid())
            {
                m_texMapping.emplace_back(createParamMapElement(hdl, _uniformDesc.textureBinding));
                bindFound = true;
            }
        }
        return bindFound;
    }

    bool ShaderParameterMapping::bindOneSampler(const FxParameterDB& _paramDB, const NVN::ShaderMetaData::UniformDesc& _uniformDesc)
    {
        bool bindFound = false;
        if (!_uniformDesc.isArray)
        {
            // we assume the sampler name starts with "samp" and then is coherent with the tex name.
            String8 cleanedName = cleanPrefixIfFound("samp", _uniformDesc.name);

            auto hdl = _paramDB.findTexSampler(cleanedName.cStr());
            ITF_ASSERT_MSG(hdl.isValid(), "Sampler %s not found, is there a different convention of sampler naming in the shaders ?", _uniformDesc.name.cStr());
            if (!hdl.isValid())
            {
                hdl = _paramDB.findTexSampler(_uniformDesc.name.cStr());
            }
            if (hdl.isValid())
            {
                m_samplerMapping.emplace_back(createParamMapElement(hdl, _uniformDesc.textureBinding));
                bindFound = true;
            }
        }
        return bindFound;
    }
}
