#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#include "RenderContext_GCM.h"
#include "ShaderProgram_GCM.h"
#include "ShaderParameterMapping_GCM.h"

namespace ITF
{
    namespace GCM
    {

        static inline void cleanArrayName(String8 & _paramName)
        {
            const char * bracket = strchr(_paramName.cStr(),'[');
            if (bracket)
            {
                // remove the brackets
                _paramName = _paramName.substr(0, bracket - _paramName.cStr());
            }
        }

        static inline bool isArray(const String8 & _paramName)
        {
            return NULL != strchr(_paramName.cStr(), '[');
        }


        ShaderParameterMapping::ShaderParameterMapping()
            : m_hasCGParam(false)
            , m_profile(CG_PROFILE_UNKNOWN)
            , m_boundType(FxParameterDB::TYPE_INVALID)
        {
        }

        bool ShaderParameterMapping::bind(const FxParameterDB & _paramDB, CGprogram _cgProg)
        {
            // Check if not bind several times
            // right now not supported. Rebind could be easily done, by just cleaning arrays 
            ITF_ASSERT(m_boundType == FxParameterDB::TYPE_INVALID);
            m_profile = cellGcmCgGetProgramProfile(_cgProg);
            m_boundType = _paramDB.getType();

            bool success = true;
            for(CGparameter param = cellGcmCgGetFirstLeafParameter(_cgProg, CG_PROGRAM);
                param!=0;
                )
            {
                if (0 != cellGcmCgGetParameterReferenced(_cgProg, param)) // get only used !
                {
                    String8 paramName = cellGcmCgGetParameterName(_cgProg, param);
                    CGenum variability = cellGcmCgGetParameterVariability(_cgProg, param);
                    if (CG_UNIFORM == variability)
                    {
                        CGresource res = cellGcmCgGetParameterResource(_cgProg, param);
                        if (  (res>=CG_TEX0 && res<=CG_TEX7)
                            ||(res>=CG_TEXUNIT0 && res<=CG_TEXUNIT7)) // vertex texture fetch
                        {
                            success = bindTex(_paramDB, _cgProg, param, paramName) && success;
                        }
                        else if ((res == CG_C && m_profile==CG_PROFILE_VPRSX)
                                || (res == CG_UNDEFINED && m_profile==CG_PROFILE_FPRSX)) // undefined for fragment 
                        {
                            if (isArray(paramName))
                            {
                                success = bindArray(_paramDB, _cgProg, param, paramName) && success;
                            }
                            else
                            {
                                success = bindElement(_paramDB, _cgProg, param, paramName) && success;
                            }
                        }
                        else
                        {
                            param = cellGcmCgGetNextLeafParameter(_cgProg, param);
                        }
                    }
                    else if (variability == CG_CONSTANT)
                    {
                        u32 paramIndex = cellGcmCgGetParameterResourceIndex(_cgProg, param);
                        const float * quadData = cellGcmCgGetParameterValues(_cgProg, param);
                        m_constants.m_regList.push_back(u16(paramIndex));
                        FxParameterDB::Vector4 cst;
                        cst.m_x = quadData[0];
                        cst.m_y = quadData[1];
                        cst.m_z = quadData[2];
                        cst.m_w = quadData[3];
                        m_constants.m_valueList.push_back(cst);
                        param = cellGcmCgGetNextLeafParameter(_cgProg, param);
                    }
                    else
                    {
                        param = cellGcmCgGetNextLeafParameter(_cgProg, param);
                    }
                }
                else
                {
                    param = cellGcmCgGetNextLeafParameter(_cgProg, param);
                }
            }

            m_hasCGParam = !(
                                m_matMapping.empty()
                                && m_floatMapping.empty()
                                && m_intMapping .empty()
                                && m_float4Mapping.empty()
                                && m_float4ArrayVertexMapping.empty()
                                && m_float4ArrayFragmentMapping.empty()
                                );
            return success;
        }

        bool ShaderParameterMapping::bindTex(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName)
        {
            bool success = false;
            CGresource res = cellGcmCgGetParameterResource(_cgProg, _cgParam);
            u32 texUnit = 0;
            if (res>=CG_TEX0 && res<=CG_TEX7)
                texUnit = res - CG_TEX0;
            else
                texUnit = res - CG_TEXUNIT0;

            FxParameterDB::TexSamplerHdl h = _paramDB.findTexSampler(_paramName.cStr());
            if (h != FxParameterDB::InvalidHandle)
            {
                m_samplerMapping.push_back(TexSamplerMapping::value_type(h, texUnit));
                success = true;
            }
            else
            {
                ITF_WARNING(NULL, bfalse, "Unknown shader tex sampler %s\n", _paramName.cStr());
            }

            _cgParam = cellGcmCgGetNextLeafParameter(_cgProg, _cgParam);            
            return success;
        }

        bool ShaderParameterMapping::bindElement(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName)
        {
            static struct CGTypeDesc
            {
                CGtype type;
                u32 nbReg;
            } arrayCGTypeDesc[] =
            {
#define CG_DATATYPE_MACRO(name, compiler_name, enum_name, base_name, ncols, nrows, pc) \
                {(enum_name), (pc)==CG_PARAMETERCLASS_MATRIX?(ncols):1},
#include <Cg/cg_datatypes.h>
#undef CG_DATATYPE_MACRO
            };

            CGtype type = cellGcmCgGetParameterType(_cgProg, _cgParam);
            u32 nbReg = arrayCGTypeDesc[u32(type) - CG_TYPE_START_ENUM - 1].nbReg;
            ITF_ASSERT(arrayCGTypeDesc[u32(type) - CG_TYPE_START_ENUM - 1].type == type);
            bool success = addTypeMapping(_paramDB, type, _cgProg, _cgParam, _paramName);

            if (nbReg>1)
            {
                // in case the number of registers is more than 1, 
                // we need to advance, as the sub elements of the params are repeated
                for(u32 i = 0; i<nbReg; ++i)
                {
                    ITF_ASSERT(_cgParam);
                    _cgParam = cellGcmCgGetNextLeafParameter(_cgProg, _cgParam);
                }
            }
            _cgParam = cellGcmCgGetNextLeafParameter(_cgProg, _cgParam);
            return success;
        }

        // this method is there to check if a handle is not already registered (for parsing error assertions)
        template <class Ctn>
        static bool findHdl(const Ctn & _ctn, typename Ctn::value_type::FxHdlType _hdl)
        {
            for(typename Ctn::const_iterator it = _ctn.begin(); it!= _ctn.end(); ++it)
            {
                if (it->paramFxHdl == _hdl)
                {
                    return true;
                }
            }
            return false;
        }

        bool ShaderParameterMapping::bindArray(const FxParameterDB & _paramDB, CGprogram _cgProg, CGparameter & _cgParam, const String8 & _paramName)
        {
            bool success = false;
            String8 cleanedParamName = _paramName;
            cleanArrayName(cleanedParamName);

            ArrayShaderParams shParamArray;

            // advance to next param, while filling the array of CGParam
            String8 newParamName;
            do 
            {
                shParamArray.push_back(ShaderParam(_cgParam));
                _cgParam = cellGcmCgGetNextLeafParameter(_cgProg, _cgParam);
                if (_cgParam!=0)
                {
                    newParamName = cellGcmCgGetParameterName(_cgProg, _cgParam);
                    cleanArrayName(newParamName);
                }
            } while(_cgParam!=0 && cleanedParamName==newParamName);

            FxParameterDB::Float4ArrayHdl h = _paramDB.findFloat4Array(cleanedParamName.cStr());
            if (h != FxParameterDB::InvalidHandle)
            {
                if (m_profile==CG_PROFILE_VPRSX)
                {
                    // just get the first register from the element 0
                    u32 firstReg = cellGcmCgGetParameterResourceIndex(_cgProg, CGparameter(shParamArray[0]));
                    ITF_ASSERT(firstReg != u32(-1));
                    ShaderParam shParam = firstReg;
                    ITF_ASSERT(!findHdl(m_float4ArrayVertexMapping, shParam));
                    m_float4ArrayVertexMapping.push_back(Float4ArrayVertexMapping::value_type(h, shParam));
                    success = true;
                }
                else
                {
                    ITF_ASSERT(!findHdl(m_float4ArrayFragmentMapping, h));
                    m_float4ArrayFragmentMapping.push_back(Float4ArrayFragmentMapping::value_type(h, shParamArray));
                    success = true;
                }
            }

            return success;
        }

        ShaderParam ShaderParameterMapping::convertShaderParam(CGprogram _prog, CGparameter _cgParam)
        {
            if (m_profile==CG_PROFILE_VPRSX)
            {
                u32 firstReg = cellGcmCgGetParameterResourceIndex(_prog, _cgParam);
                return ShaderParam(firstReg);
            }
            else
            {
                return ShaderParam(_cgParam);
            }
        }

        bool ShaderParameterMapping::addTypeMapping(const FxParameterDB & _paramDB, CGtype _cgType, CGprogram _cgProg, CGparameter _cgParam, const String8 & _paramName)
        {
            switch (_cgType)
            {
            case CG_FLOAT :
                {
                    FxParameterDB::FloatHdl h = _paramDB.findFloat(_paramName.cStr());
                    if (h != FxParameterDB::InvalidHandle)
                    {
                        ITF_ASSERT(!findHdl(m_floatMapping, h));
                        m_floatMapping.push_back(FloatMapping::value_type(h, ShaderParam(_cgParam)));
                        return true;
                    }
                }
                // break; // if not found, try to see if it's not an int viewed as a float
            case CG_INT :
                {
                    FxParameterDB::IntHdl h = _paramDB.findInt(_paramName.cStr());
                    if (h != FxParameterDB::InvalidHandle)
                    {
                        ITF_ASSERT(!findHdl(m_intMapping, h));
                        m_intMapping.push_back(IntMapping::value_type(h, ShaderParam(_cgParam)));
                        return true;
                    }
                }
                break;
            case CG_FLOAT4 :
                {
                    FxParameterDB::Float4Hdl h = _paramDB.findFloat4(_paramName.cStr());
                    if (h != FxParameterDB::InvalidHandle)
                    {
                        ITF_ASSERT(!findHdl(m_float4Mapping, h));
                        m_float4Mapping.push_back(Float4Mapping::value_type(h, convertShaderParam(_cgProg, _cgParam)));
                        return true;
                    }
                }
                break;
            case CG_FLOAT4x4 :
                {
                    FxParameterDB::Matrix4x4Hdl h = _paramDB.findMatrix4x4(_paramName.cStr());
                    if (h != FxParameterDB::InvalidHandle)
                    {
                        ITF_ASSERT(!findHdl(m_matMapping, h));
                        m_matMapping.push_back(Mat44Mapping::value_type(h, convertShaderParam(_cgProg, _cgParam)));
                        return true;
                    }
                }
                break;
            default:
                {
                    ITF_WARNING(NULL, bfalse, "Unknown mapping type %d on parameter %s\n", _cgType, _paramName.cStr());
                    return false;
                }
                break;
            }
            ITF_WARNING(NULL, 0, "Unknown shader parameter %s\n", _paramName.cStr());
            return false;
        }

        // templated loop
        template <typename Ctn, typename Fct, typename Prog>
        inline void applyForEachParameters(const Ctn & _container, Fct _fct, RenderContext & _rdrCtxt, const Prog & _shaderProg)
        {
            for(typename Ctn::const_iterator it = _container.begin(), itEnd = _container.end();
                it!=itEnd;
                ++it)
            {
                _fct(it->paramFxHdl, it->paramCG, _rdrCtxt, _shaderProg);
            }
        }

        // Vertex set params
        inline void ApplyVertexMatrixParam(FxParameterDB::Matrix4x4Hdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const VertexProgram & _prog)
        {
            _rdrCtxt.gcmCtxt->SetVertexProgramParameterBlock(_shParam, 4, (const float *)&_rdrCtxt.paramDB.getMatrix4x4(_h));
        }

        inline void ApplyVertexFloatParam(FxParameterDB::FloatHdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const VertexProgram & _prog)
        {
            f32 fValue = _rdrCtxt.paramDB.getFloat(_h);
            _rdrCtxt.gcmCtxt->SetVertexProgramParameter(CGparameter(_shParam), (const float *)&fValue);
        }

        inline void ApplyVertexIntParam(FxParameterDB::IntHdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const VertexProgram & _prog)
        {
            ITF_ASSERT(0); // TODO -> check that we need to promote it to float !
            float fInt = float(_rdrCtxt.paramDB.getInt(_h)); // promote
            _rdrCtxt.gcmCtxt->SetVertexProgramParameter(CGparameter(_shParam), (const float *)&fInt);
        }

        inline void ApplyVertexFloat4Param(FxParameterDB::Float4Hdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const VertexProgram & _prog)
        {
            _rdrCtxt.gcmCtxt->SetVertexProgramParameterBlock(_shParam, 1, (const float *)&_rdrCtxt.paramDB.getFloat4(_h));
        }

        inline void ApplyVertexFloat4ArrayParam(FxParameterDB::Float4ArrayHdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const VertexProgram & _prog)
        {
            const FxParameterDB::Vector4 * pFloat4 = NULL;
            u32 nbFloat4 = 0;
            _rdrCtxt.paramDB.getFloat4Array(_h, pFloat4, nbFloat4);
            _rdrCtxt.gcmCtxt->SetVertexProgramParameterBlock(_shParam, nbFloat4, (const float *)pFloat4);
        }

        inline void ShaderParameterMapping::applyVertexConstants(RenderContext & _rdrCtxt)
        {
            ITF_ASSERT(m_profile == CG_PROFILE_VPRSX);
            ITF_ASSERT(m_constants.m_regList.size() ==  m_constants.m_valueList.size());
            for(u32 i = 0, n = m_constants.m_regList.size(); i<n; ++i)
            {
                _rdrCtxt.gcmCtxt->SetVertexProgramParameterBlock(m_constants.m_regList[i], 1, (const float *)&m_constants.m_valueList[i]);
            }
        }

        void ShaderParameterMapping::applyParams(RenderContext & _rdrCtxt, const VertexProgram & _vtxProg)
        {
            ITF_ASSERT(m_profile == CG_PROFILE_VPRSX);
            ITF_ASSERT(m_boundType == _rdrCtxt.paramDB.getType());
            // applyForEachParameters(m_samplerMapping, ApplyVertexSamplerParam, _paramDB, _vtxProg); // not supported
            if (m_hasCGParam)
            {
                applyVertexConstants(_rdrCtxt);
                applyForEachParameters(m_matMapping, ApplyVertexMatrixParam, _rdrCtxt, _vtxProg);
                applyForEachParameters(m_floatMapping, ApplyVertexFloatParam, _rdrCtxt, _vtxProg);
                applyForEachParameters(m_intMapping, ApplyVertexIntParam, _rdrCtxt, _vtxProg);
                applyForEachParameters(m_float4Mapping, ApplyVertexFloat4Param, _rdrCtxt, _vtxProg);
                applyForEachParameters(m_float4ArrayVertexMapping, ApplyVertexFloat4ArrayParam, _rdrCtxt, _vtxProg);
            }
        }

        // Fragment set params
        inline void ApplyFragmentSamplerParam(FxParameterDB::Matrix4x4Hdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            const CellGcmTexture * texture =  (const CellGcmTexture *)_rdrCtxt.paramDB.getTexture(_h);
            if (texture)
            {
                _rdrCtxt.gcmCtxt->SetTexture(u32(_shParam), texture);
            }
        }

        inline void ApplyFragmentMatrixParam(FxParameterDB::Matrix4x4Hdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            _rdrCtxt.gcmCtxt->SetFragmentProgramParameter(_prog.cg, CGparameter(_shParam), (const float *)&_rdrCtxt.paramDB.getMatrix4x4(_h), _prog.uCodeHdl.GetOffset());
        }

        inline void ApplyFragmentFloatParam(FxParameterDB::FloatHdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            f32 fValue = _rdrCtxt.paramDB.getFloat(_h);
            _rdrCtxt.gcmCtxt->SetFragmentProgramParameter(_prog.cg, CGparameter(_shParam), (const float *)&fValue, _prog.uCodeHdl.GetOffset());
        }

        inline void ApplyFragmentIntParam(FxParameterDB::IntHdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            ITF_ASSERT(0); // TODO -> check that we need to promote it to float !
            float fInt = float(_rdrCtxt.paramDB.getInt(_h)); // promote
            _rdrCtxt.gcmCtxt->SetFragmentProgramParameter(_prog.cg, CGparameter(_shParam), (const float *)&fInt, _prog.uCodeHdl.GetOffset());
        }

        inline void ApplyFragmentFloat4Param(FxParameterDB::Float4Hdl _h, ShaderParam _shParam, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            _rdrCtxt.gcmCtxt->SetFragmentProgramParameter(_prog.cg, CGparameter(_shParam), (const float *)&_rdrCtxt.paramDB.getFloat4(_h), _prog.uCodeHdl.GetOffset());
        }

        inline void ApplyFragmentFloat4ArrayParam(FxParameterDB::Float4ArrayHdl _h, const SafeArray<ShaderParam> & _shParams, RenderContext & _rdrCtxt, const FragmentProgram & _prog)
        {
            const FxParameterDB::Vector4 * pFloat4 = NULL;
            u32 nbFloat4 = 0;
            _rdrCtxt.paramDB.getFloat4Array(_h, pFloat4, nbFloat4);
            for(u32 i = 0, n = std::min(_shParams.size(), nbFloat4); i<n; ++i)
            {
                _rdrCtxt.gcmCtxt->SetFragmentProgramParameter(_prog.cg, CGparameter(_shParams[i]), (const float *)&(pFloat4[i]), _prog.uCodeHdl.GetOffset());
            }
        }

        void ShaderParameterMapping::applyParams(RenderContext & _rdrCtxt, const FragmentProgram & _fragProg)
        {
            ITF_ASSERT(m_profile == CG_PROFILE_FPRSX);
            ITF_ASSERT(m_boundType == _rdrCtxt.paramDB.getType());
            applyForEachParameters(m_samplerMapping, ApplyFragmentSamplerParam, _rdrCtxt, _fragProg);
            if (m_hasCGParam)
            {
                applyForEachParameters(m_matMapping, ApplyFragmentMatrixParam, _rdrCtxt, _fragProg);
                applyForEachParameters(m_floatMapping, ApplyFragmentFloatParam, _rdrCtxt, _fragProg);
                applyForEachParameters(m_intMapping, ApplyFragmentIntParam, _rdrCtxt, _fragProg);
                applyForEachParameters(m_float4Mapping, ApplyFragmentFloat4Param, _rdrCtxt, _fragProg);
                applyForEachParameters(m_float4ArrayFragmentMapping, ApplyFragmentFloat4ArrayParam, _rdrCtxt, _fragProg);
                _rdrCtxt.gcmCtxt->SetUpdateFragmentProgramParameterLocation(_fragProg.uCodeHdl.GetOffset(), _fragProg.uCodeHdl.GetLocation()); // invalidate shader instruction cache
            }
        }
    }
}
