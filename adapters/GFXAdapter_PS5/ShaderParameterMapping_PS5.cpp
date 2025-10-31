#include "precompiled_GFXAdapter_PS5.h"

#include <variant>

#include "ShaderParameterMapping_PS5.h"
#include "engine/common/WarningManager.h"
#include "AGS/AgsLoaderInStruct_PS5.h"

namespace ITF
{
    ShaderParameterMapping::ShaderParameterMapping() = default;
    ShaderParameterMapping::~ShaderParameterMapping() = default;


    template <typename FxHdl, typename Binding>
    void ShaderParameterMapping::ParamMapElement<FxHdl, Binding>::copyToBuffer(
        Gnmp::Buffer& _constantBuffer, const FxParameterDB& _paramDB) const
    {
        i32 offsetInBuffer = bind.offset;
        i32 sizeInBuffer = bind.size;
        ITF_UNUSED(sizeInBuffer);
        ITF_ASSERT(offsetInBuffer >= 0);
        ITF_ASSERT(offsetInBuffer < _constantBuffer.getSize());
        ITF_ASSERT(offsetInBuffer + sizeInBuffer <= _constantBuffer.getSize());

        u8* data = static_cast<u8*>(_constantBuffer.getBaseAddress()) + offsetInBuffer;

        if constexpr (std::is_same_v<FxHdl, FxParameterDB::Matrix4x4Hdl>)
        {
            const auto& matrix = _paramDB.getMatrix4x4(fxParamHdl);
            ITF_ASSERT(sizeof(matrix) == sizeInBuffer);
            *reinterpret_cast<Matrix44*>(data) = matrix;
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
            memcpy(data, float4Array, effectiveSizeToAssign);
        }
    }

    template <Gnmp::ShaderStage stage>
    void ShaderParameterMapping::applySamplerParams(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        for (auto & entry : m_samplerMapping)
        {
            auto texAndSampler = _paramDB.getTextureAndSampler(entry.fxParamHdl);
            i32 samplerIdx = entry.bind;
            _rdrCtxt.setSamplers(stage, samplerIdx, 1, &texAndSampler.sampler);
            _rdrCtxt.setTextures(stage, samplerIdx, 1, texAndSampler.texture);
        }
    }

    template <typename Mapping>
    void ShaderParameterMapping::applyConstantBufferParams(Gnmp::Buffer & _constBuffer, const FxParameterDB& _paramDB, const Mapping& _mapping)
    {
        for (const auto & entry : _mapping)
        {
            entry.copyToBuffer(_constBuffer, _paramDB);
        }
    }

    template <Gnmp::ShaderStage stage>
    void ShaderParameterMapping::applyParams(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB)
    {
        applySamplerParams<stage>(_rdrCtxt, _paramDB);

        // reserve an area at the end of the command buffer for the constant buffer.
        // It will be reusable at the same time as the command buffer.
        void* cstBufferArea = _rdrCtxt.allocateFromCommandBuffer(m_constantBufferSize, sce::Agc::Gnmp::kEmbeddedDataAlignment4);
        Gnmp::Buffer constBuffer;
        constBuffer.initAsConstantBuffer(cstBufferArea, m_constantBufferSize);

        applyConstantBufferParams(constBuffer, _paramDB, m_matMapping);
        applyConstantBufferParams(constBuffer, _paramDB, m_floatMapping);
        applyConstantBufferParams(constBuffer, _paramDB, m_intMapping);
        applyConstantBufferParams(constBuffer, _paramDB, m_float4Mapping);
        applyConstantBufferParams(constBuffer, _paramDB, m_float4ArrayMapping);

        _rdrCtxt.setConstantBuffers(stage, m_constantBufferSlot, 1, &constBuffer);
    }

    // explicit instantiate
    template void ShaderParameterMapping::applyParams<Gnmp::kShaderStageVs>(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);
    template void ShaderParameterMapping::applyParams<Gnmp::kShaderStagePs>(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc & _member, const AGS::VectorDesc& _vectDesc)
    {
        const char* memberName = _member.name.cStr();

        if (_vectDesc.type == SceShaderFloat32)
        {
            if (_vectDesc.nbScalar == 1)
            {
                auto hdl = _paramDB.findFloat(memberName);
                if (hdl.isValid())
                {
                    m_floatMapping.emplace_back(createParamMapElement( hdl, _member.offset, _member.size));
                    return true;
                }
            }
            else if (_vectDesc.nbScalar == 4)
            {
                auto hdl = _paramDB.findFloat4(memberName);
                if (hdl.isValid())
                {
                    m_float4Mapping.emplace_back(createParamMapElement(hdl, _member.offset, _member.size));
                    return true;
                }
            }
        }
        else if (_vectDesc.type == SceShaderInt32 || _vectDesc.type == SceShaderUint32)
        {
            if (_vectDesc.nbScalar == 1)
            {
                auto hdl = _paramDB.findInt(memberName);
                if (hdl.isValid())
                {
                    m_intMapping.emplace_back(createParamMapElement(hdl, _member.offset, _member.size));
                    return true;
                }
            }
        }
        return false;
    }

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc& _member, const AGS::MatrixDesc& _matDesc)
    {
        const char* memberName = _member.name.cStr();

        if (_matDesc.type == SceShaderFloat32)
        {
            if (_matDesc.nbCol == 4 && _matDesc.nbRow == 4)
            {
                ITF_ASSERT(_matDesc.isRowMajor); // only column major is supported, add pragma in the shader
                auto hdl = _paramDB.findMatrix4x4(memberName);
                if (hdl.isValid())
                {
                    m_matMapping.emplace_back(createParamMapElement(hdl, _member.offset, _member.size));
                    return true;
                }
            }
        }
        return false;
    }

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc& _member, const AGS::ArrayDesc& _arrDesc)
    {
        const char* memberName = _member.name.cStr();

        if (_arrDesc.dimensionSizes.size() != 1) // 1 dimension array only
            return false;

        const AGS::TypeDesc::VariantType* variantType = _arrDesc.type.variantType.get();

        if (variantType != nullptr)
        {
            const AGS::VectorDesc * vector = std::get_if<AGS::VectorDesc>(variantType);

            if (   vector!=nullptr
                && vector->nbScalar == 4
                && vector->type == SceShaderFloat32)
            {
                auto hdl = _paramDB.findFloat4Array(memberName);
                if (hdl.isValid())
                {
                    ITF_ASSERT(_paramDB.getFloat4ArrayCapacity(hdl) <= _arrDesc.dimensionSizes[0]);
                    m_float4ArrayMapping.emplace_back(createParamMapElement(hdl, _member.offset, _member.size));
                    return true;
                }
            }
        }
        return false;
    }

    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const AGS::StructDesc& structDesc)
    {
        m_constantBufferSize = structDesc.size;
        for (const auto& member : structDesc.members)
        {
            const AGS::TypeDesc::VariantType* variantType = member.type.variantType.get();
            ITF_ASSERT(!member.isUsed || variantType != nullptr);
            if (member.isUsed && variantType != nullptr)
            {
                bool bindFound = false;
                if (const auto* vectorDesc = std::get_if<AGS::VectorDesc>(variantType))
                {
                    bindFound = bindParameterDB(_paramDB, member, *vectorDesc);
                }
                else if (const auto* matrixDesc = std::get_if<AGS::MatrixDesc>(variantType))
                {
                    bindFound = bindParameterDB(_paramDB, member, *matrixDesc);
                }
                else if (const auto* arrayDesc = std::get_if<AGS::ArrayDesc>(variantType))
                {
                    bindFound = bindParameterDB(_paramDB, member, *arrayDesc);
                }
                if (!bindFound)
                {
                    ITF_WARNING(NULL, bfalse, "Shader constant buffer member %s of type %s is not handled.",
                        member.name.cStr(),
                        structDesc.name.cStr());
                }
            }
        }
        return true;
    }


    bool ShaderParameterMapping::bindParameterDB(const FxParameterDB& _paramDB, const AGS::ResourceDesc& constantBufferDesc)
    {
        ITF_ASSERT(constantBufferDesc.resourceClass == SceShaderCb);
        m_boundDBName = _paramDB.getName();
        m_constantBufferSlot = constantBufferDesc.apiSlot;
        const AGS::TypeDesc::VariantType * variantofType = constantBufferDesc.type.variantType.get();
        ITF_ASSERT(variantofType != nullptr);
        if (variantofType != nullptr)
        {
            const auto * bufferDesc = std::get_if<AGS::BufferDesc>(variantofType);
            ITF_ASSERT(bufferDesc != nullptr && bufferDesc->bufferClass == SceShaderConstantBuffer);
            if (bufferDesc != nullptr && bufferDesc->bufferClass == SceShaderConstantBuffer)
            {
                ITF_ASSERT(bufferDesc->type.variantType!=nullptr);
                if (bufferDesc->type.variantType != nullptr)
                {
                    variantofType = bufferDesc->type.variantType.get();
                    const AGS::StructDesc * structDesc = std::get_if<AGS::StructDesc>(variantofType);
                    if (structDesc != nullptr)
                    {
                        return bindParameterDB(_paramDB, *structDesc);
                    }
                }
            }
        }
        return false;
    }

    bool ShaderParameterMapping::bindTextureSamplersDB(const FxParameterDB& _paramDB, const Vector<AGS::ResourceDesc>& samplerResources)
    {
        for (const auto& samplerResource : samplerResources)
        {
            if (samplerResource.resourceClass != SceShaderSrv)
                continue;

            bool bindFound = false;
            ITF_UNUSED(bindFound);
            const AGS::TypeDesc::VariantType* variantType = samplerResource.type.variantType.get();
            ITF_ASSERT(variantType != nullptr);
            if (variantType != nullptr)
            {
                const auto* textureDesc = std::get_if<AGS::TextureDesc>(variantType);
                if (textureDesc != nullptr)
                {
                    auto hdl = _paramDB.findTexSampler(samplerResource.name.cStr());
                    if (hdl.isValid())
                    {
                        m_samplerMapping.emplace_back(createParamMapElement(hdl, samplerResource.apiSlot));
                        bindFound = true;
                    }
                }
            }
            ITF_ASSERT_MSG(bindFound, "Shader texture sampler %s is not handled.", samplerResource.name.cStr());
        }
        return true;
    }

}
