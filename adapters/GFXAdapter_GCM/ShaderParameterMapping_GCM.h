#if defined (ITF_PS3)

#ifndef ITF_SHADER_PARAMETER_MAPPING_GCM_H_
#define ITF_SHADER_PARAMETER_MAPPING_GCM_H_

#include <Cg/cg.h>
#include "FxParameterDB.h"

namespace ITF
{
    namespace GCM
    {
        class VertexProgram;
        class FragmentProgram;
        class RenderContext;

        ITF_COMPILE_TIME_ASSERT(CGparameter_has_to_be_4_bytes, sizeof(CGparameter)==sizeof(u32));
        typedef u32 ShaderParam; // a CGparameter or a packed {register,size}

        // Note that default value of uniforms is not supported right now (at least on vertex shaders)
        class ShaderParameterMapping
        {
        public:
            ShaderParameterMapping();

            bool bind(const FxParameterDB & _paramDB, CGprogram _cgProg);
            FxParameterDB::Type getBoundDBType() const { return m_boundType; }

            void applyParams(RenderContext & _rdrCtxt, const VertexProgram & _vtxProg);
            void applyParams(RenderContext & _rdrCtxt, const FragmentProgram & _fragProg);

        private:

            bool addTypeMapping(const FxParameterDB & _paramDB, CGtype _type, CGprogram _cgProg, CGparameter _param, const String8 & _paramName);
            ShaderParam convertShaderParam(CGprogram _prog, CGparameter _cgParam);

            bool bindTex(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName);
            bool bindElement(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName);
            bool bindArray(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName);

            void applyVertexConstants(RenderContext & _rdrCtxt);

            // Internal constants are literal constants in a vertex shader
            struct InternalConstants
            {
                // the two array are not merged in 1 struct to avoid bad padding
                Vector<u16> m_regList; 
                Vector<FxParameterDB::Vector4> m_valueList; // m_regList.Size() == m_valueList.Size()
            };

            template <typename FxHdl, typename CGHdl>
            struct ParamMapElement
            {
                ParamMapElement(FxHdl _paramFxHdl, CGHdl _paramCG)
                    : paramFxHdl(_paramFxHdl)
                    , paramCG(_paramCG)
                {}

                typedef FxHdl FxHdlType;
                typedef CGHdl CGParamType;

                FxHdl paramFxHdl;
                CGHdl paramCG;
            };

            typedef Vector<ParamMapElement<FxParameterDB::TexSamplerHdl, ShaderParam> > TexSamplerMapping; // sampler Idx
            typedef Vector<ParamMapElement<FxParameterDB::Matrix4x4Hdl, ShaderParam> > Mat44Mapping;
            typedef Vector<ParamMapElement<FxParameterDB::FloatHdl, ShaderParam> > FloatMapping;
            typedef Vector<ParamMapElement<FxParameterDB::IntHdl, ShaderParam> > IntMapping;
            typedef Vector<ParamMapElement<FxParameterDB::Float4Hdl, ShaderParam> > Float4Mapping;
            typedef SafeArray<ShaderParam> ArrayShaderParams;
            typedef ITF::Vector<ParamMapElement<FxParameterDB::Float4ArrayHdl, ArrayShaderParams> > Float4ArrayFragmentMapping;
            typedef ITF::Vector<ParamMapElement<FxParameterDB::Float4ArrayHdl, ShaderParam> > Float4ArrayVertexMapping;

            InternalConstants m_constants;
            TexSamplerMapping m_samplerMapping;
            Mat44Mapping m_matMapping;
            FloatMapping m_floatMapping;
            IntMapping m_intMapping;
            Float4Mapping m_float4Mapping;
            Float4ArrayVertexMapping m_float4ArrayVertexMapping;
            Float4ArrayFragmentMapping m_float4ArrayFragmentMapping;
            
            bool m_hasCGParam; // avoid excessive processing in case of no parameter to set (for fragment)
            CGprofile m_profile; // VS or FS
            FxParameterDB::Type m_boundType;
        };
    }
}

#endif // ITF_SHADER_PARAMETER_MAPPING_GCM_H_

#endif // ITF_PS3
