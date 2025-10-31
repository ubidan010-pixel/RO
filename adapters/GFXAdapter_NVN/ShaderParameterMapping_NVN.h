#pragma once

#include "FxParameterDB_NVN.h"
#include "UniformBufferPool_NVN.h"
#include "NVN/Helpers.h"

namespace ITF
{

    namespace NVN::ShaderMetaData
    {
        struct UniformBlockDesc;
        struct UniformDesc;
        struct InputDesc;
        struct OutputDesc;
    }

    class VertexProgram;
    class PixelProgram;

    // Note that default value of uniforms is not supported right now (at least on vertex shaders)
    class ShaderParameterMapping
    {
    public:
        ShaderParameterMapping();
        ~ShaderParameterMapping();

        bool bindParameterDB(const FxParameterDB & _paramDB, const Vector<NVN::ShaderMetaData::UniformBlockDesc> & _blocks, const Vector<NVN::ShaderMetaData::UniformDesc> & _uniforms);

        const char * getBoundDBName() const { return m_boundDBName.cStr(); }

        u32 getUniformBufferSize() const { return m_uniformBufferSize; }
        u32 getNbTextureSampler() const { return m_samplerMapping.size(); }

        template <nvn::ShaderStage::Enum stage>
        void applyParams(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool & _uniformPool, const FxParameterDB& _paramDB);

    private:

        bool bindOneParameter(const FxParameterDB& _paramDB, const String8& _blockName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneMatrix44Parameter(const FxParameterDB& _paramDB, const String8& _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneFloatParameter(const FxParameterDB& _paramDB, const String8& _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneIntParameter(const FxParameterDB& _paramDB, const String8& _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneFloat4Parameter(const FxParameterDB& _paramDB, const String8& _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneFloat4ArrayParameter(const FxParameterDB& _paramDB, const String8& _uniformName, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneTexture(const FxParameterDB& _paramDB, const NVN::ShaderMetaData::UniformDesc& uniformDesc);
        bool bindOneSampler(const FxParameterDB& _paramDB, const NVN::ShaderMetaData::UniformDesc& uniformDesc);

        template <nvn::ShaderStage::Enum stage>
        void applyTextureParams(nvn::CommandBuffer& _rdrCtxt, const FxParameterDB& _paramDB);
        template <nvn::ShaderStage::Enum stage>
        void applySamplerParams(nvn::CommandBuffer& _rdrCtxt, const FxParameterDB& _paramDB);
        template <typename Mapping>
        void copyUniformBufferParams(nvn::Buffer& uniformBuffer, u8* _uniformBufferData, const FxParameterDB& _paramDB, const Mapping& _mapping);

        template <typename FxHdl, typename Binding>
        struct ParamMapElement
        {
            FxHdl fxParamHdl;
            Binding bind;

            void copyToBuffer(nvn::Buffer& uniformBuffer, u8* _uniformBufferData, const FxParameterDB& _paramDB) const;
        };

        struct UniformBufferEntry
        {
            i32 offset; // byte offset
            i32 size; // byte size
        };

        template <typename FxHdl>
        ParamMapElement<FxHdl, UniformBufferEntry> createParamMapElement(FxHdl hdl, i32 offset, i32 size)
        {
            return { hdl, UniformBufferEntry{ offset, size } };
        }

        using ApiSlot = i32;

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

        TexMapping m_texMapping;
        SamplerMapping m_samplerMapping;
        Mat44Mapping m_matMapping;
        FloatMapping m_floatMapping;
        IntMapping m_intMapping;
        Float4Mapping m_float4Mapping;
        Float4ArrayMapping m_float4ArrayMapping;

        String8 m_boundDBName;
        static constexpr i32 InvalidBufferSlot = -1;
        i32 m_vertexUniformBufferSlot = InvalidBufferSlot;
        i32 m_fragmentUniformBufferSlot = InvalidBufferSlot;
        u32 m_uniformBufferSize = 0;
    };
}

