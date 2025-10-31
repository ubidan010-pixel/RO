#pragma once

#include "FxParameterDB_PS5.h"

#include "Gnmp_Helpers.h"

namespace ITF
{
    namespace AGS
    {
        struct ResourceDesc;
        struct MemberDesc;
        struct StructDesc;
        struct VectorDesc;
        struct MatrixDesc;
        struct ArrayDesc;
    }

    class VertexProgram;
    class PixelProgram;

    // Note that default value of uniforms is not supported right now (at least on vertex shaders)
    class ShaderParameterMapping
    {
    public:
        ShaderParameterMapping();
        ~ShaderParameterMapping();

        bool bindParameterDB(const FxParameterDB & _paramDB, const AGS::ResourceDesc & constantBufferDesc);
        bool bindTextureSamplersDB(const FxParameterDB& _paramDB, const Vector<AGS::ResourceDesc>& samplerResources);
        const char * getBoundDBName() const { return m_boundDBName.cStr(); }

        u32 getConstantBufferSize() const { return m_constantBufferSize; }
        u32 getNbTextureSampler() const { return m_samplerMapping.size(); }

        template <Gnmp::ShaderStage stage>
        void applyParams(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);

    private:

        bool bindParameterDB(const FxParameterDB& _paramDB, const AGS::StructDesc& strustDesc);
        bool bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc& _member, const AGS::VectorDesc& _vectDesc);
        bool bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc& _member, const AGS::MatrixDesc& _matDesc);
        bool bindParameterDB(const FxParameterDB& _paramDB, const AGS::MemberDesc& _member, const AGS::ArrayDesc& _arrDesc);

        template <Gnmp::ShaderStage stage>
        void applySamplerParams(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& _paramDB);
        template <typename Mapping>
        void applyConstantBufferParams(Gnmp::Buffer& _constBuffer, const FxParameterDB& _paramDB, const Mapping& _mapping);

        template <typename FxHdl, typename Binding>
        struct ParamMapElement
        {
            FxHdl fxParamHdl;
            Binding bind;

            void copyToBuffer(Gnmp::Buffer& _constantBuffer, const FxParameterDB& _paramDB) const;
        };

        struct ConstantBufferEntry
        {
            i32 offset; // byte offset
            i32 size; // byte size
        };

        template <typename FxHdl>
        ParamMapElement<FxHdl, ConstantBufferEntry> createParamMapElement(FxHdl hdl, i32 offset, i32 size)
        {
            return { hdl, ConstantBufferEntry{ offset, size } };
        }

        using SamplerApiSlot = i32;

        template <typename FxHdl>
        ParamMapElement<FxHdl, SamplerApiSlot> createParamMapElement(FxHdl hdl, SamplerApiSlot _bind)
        {
            return { hdl, _bind };
        }

        using TexSamplerMapping = Vector<ParamMapElement<FxParameterDB::TexSamplerHdl, SamplerApiSlot>>;
        using Mat44Mapping = Vector<ParamMapElement<FxParameterDB::Matrix4x4Hdl, ConstantBufferEntry>>;
        using FloatMapping = Vector<ParamMapElement<FxParameterDB::FloatHdl, ConstantBufferEntry>>;
        using IntMapping = Vector<ParamMapElement<FxParameterDB::IntHdl, ConstantBufferEntry>>;
        using Float4Mapping = Vector<ParamMapElement<FxParameterDB::Float4Hdl, ConstantBufferEntry>>;
        using Float4ArrayMapping = Vector<ParamMapElement<FxParameterDB::Float4ArrayHdl, ConstantBufferEntry>>;

        TexSamplerMapping m_samplerMapping;
        Mat44Mapping m_matMapping;
        FloatMapping m_floatMapping;
        IntMapping m_intMapping;
        Float4Mapping m_float4Mapping;
        Float4ArrayMapping m_float4ArrayMapping;

        String8 m_boundDBName;
        u32 m_constantBufferSlot = 0;
        u32 m_constantBufferSize = 0;
    };
}

