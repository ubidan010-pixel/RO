#include "precompiled_GFXAdapter_GCM.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#include "GFXAdapter_GCM.h"
#include "Technique_GCM.h"

///////////////////////////////////////////////////////////////////////////////
// Methods handling fx/technique/shaders stuff of GFXAdapter_GCM
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{

    void GFXAdapter_GCM::createShader( ITF_shader* _shader )
    {
        _shader->mp_effect = u32(_shader);
        NOT_IMPLEMENTED_ON_GCM
    }

    void GFXAdapter_GCM::releaseShader( ITF_shader* _shader )
    {
        NOT_IMPLEMENTED_ON_GCM
#if 0
#ifdef ITF_X360//yo
            ((FXLEffect*)_shader->mp_effect )->Release();
#else
            SF_RELEASE( (ITF_EFFECTP&)_shader->mp_effect );
#endif

#endif //0
    }

    // 
    static bool readTechniques(PugiXMLWrap & _xmlParser, ITF_shader* _shader)
    {
        if (!_xmlParser.enterElement("fx_file"))
        {
            ITF_WARNING(NULL, bfalse, "Ill-formed shader fx descriptor file");
            return false;
        }

        memset(_shader->m_TechHandle, 0, sizeof(_shader->m_TechHandle));

        // loop on techs
        if (_xmlParser.enterElement("technique"))
        {
            do
            {
                GCM::Technique* technique = new GCM::Technique;
                i32 techId = technique->readNode(_xmlParser, _shader->m_name);
                if (techId>=0)
                {
                    if (techId >= sizeof(_shader->m_TechHandle)/sizeof(_shader->m_TechHandle[0]))
                    {
                        // out of place for technique in the shader struct !
                        ITF_WARNING(NULL, bfalse, "Technique number %d is too high (ignored)", techId);
                    }
                    else
                    {
                        _shader->m_TechHandle[techId] = technique;
                    }
                }
            } while(_xmlParser.nextSameElement(true));
        }
        _xmlParser.exitElement();
        return true;
    }

    static void bindShaderParameterHandles(ITF_shader * _shader, const FxParameterDB & _fxParamDB)
    {
        // bind all techniques to the fxParameterDB
        for(u32 i = 0; i<sizeof(_shader->m_TechHandle)/sizeof(_shader->m_TechHandle[0]); ++i)
        {
            GCM::Technique * tech = (GCM::Technique*)_shader->m_TechHandle[i];
            if (tech)
            {
                if (!tech->bind(_fxParamDB))
                {
                    ITF_WARNING(NULL, bfalse, "Technique %ls incorrectly bound with the engine", tech->getName().cStr());
                }
            }
        }

        // put fxParamDB handles in hard coded slots
        _shader->m_TextureHandle[0] = (void *)_fxParamDB.findTexSampler("TextureSampler0");
        _shader->m_TextureHandle[1] = (void *)_fxParamDB.findTexSampler("TextureSampler1");
        _shader->m_TextureHandle[2] = (void *)_fxParamDB.findTexSampler("TextureSampler2");
        _shader->m_TextureHandle[3] = (void *)_fxParamDB.findTexSampler("TextureSampler3");

        /// get Matrix Handle.
        _shader->m_MatrixHandle[0] = (void *)_fxParamDB.findMatrix4x4("g_mWorldViewProjection");
        _shader->m_MatrixHandle[1] = (void *)_fxParamDB.findMatrix4x4("g_mUVmat");
        _shader->m_MatrixHandle[2] = (void *)_fxParamDB.findMatrix4x4("g_mView");
        _shader->m_MatrixHandle[3] = (void *)_fxParamDB.findMatrix4x4("g_mPatchVx");
        _shader->m_MatrixHandle[4] = (void *)_fxParamDB.findMatrix4x4("g_mPatchUv");

        _shader->m_FloatHandle[0] = (void *)_fxParamDB.findFloat("g_fconst0");
        _shader->m_FloatHandle[1] = (void *)_fxParamDB.findFloat("g_fconst1");
        _shader->m_FloatHandle[2] = (void *)_fxParamDB.findFloat("g_fconst2");
        _shader->m_FloatHandle[3] = (void *)_fxParamDB.findFloat("g_fconst3");
        _shader->m_FloatHandle[4] = (void *)_fxParamDB.findFloat("g_fconst4");
        _shader->m_FloatHandle[5] = (void *)_fxParamDB.findFloat("g_fconst5");
        _shader->m_FloatHandle[6] = (void *)_fxParamDB.findFloat("g_fconst6");
        _shader->m_FloatHandle[7] = (void *)_fxParamDB.findFloat("g_fconst7");

        _shader->m_VectorArrayHandle[0] = (void *)_fxParamDB.findFloat4Array("g_va0");
        _shader->m_VectorArrayHandle[1] = (void *)_fxParamDB.findFloat4Array("g_va1");

        _shader->m_IntHandle[0] = (void *)_fxParamDB.findInt("g_iconst0");

        _shader->m_VecHandle[0] = (void *)_fxParamDB.findFloat4("g_vconst0");
        _shader->m_VecHandle[1] = (void *)_fxParamDB.findFloat4("g_vconst1");
        _shader->m_VecHandle[2] = (void *)_fxParamDB.findFloat4("g_vconst2");
        _shader->m_VecHandle[3] = (void *)_fxParamDB.findFloat4("g_vconst3");
        _shader->m_VecHandle[4] = (void *)_fxParamDB.findFloat4("g_vconst4");
        _shader->m_VecHandle[5] = (void *)_fxParamDB.findFloat4("g_vconst5");
        _shader->m_VecHandle[6] = (void *)_fxParamDB.findFloat4("g_vconst6");
        _shader->m_VecHandle[7] = (void *)_fxParamDB.findFloat4("g_vconst7");

    }

    bbool GFXAdapter_GCM::loadShader( ITF_shader* _shader )
    {
        // assign the effect to the shader (the ITF_Shader is the FX object)
        _shader->mp_effect = u32(_shader);

        String cookedName;
        cookedName.setTextFormat("%ls.fx.xml",_shader->m_name.cStr());

        PugiXMLWrap xmlParser;
        if (!xmlParser.openFile(cookedName, bfalse))
        {
            return bfalse;
        }

        // load every shader in the FX
        if (!readTechniques(xmlParser, _shader)) // assigning _shader.techHandles
        {
            return bfalse;
        }

        // the new loaded 'effect' need to be bound with the parameter DB
        bindShaderParameterHandles(_shader, m_rdrCtxt.paramDB);

        return btrue;
    }

    void  GFXAdapter_GCM::restoreDefaultShader()
    {
        setShader(mp_defaultShader);
        mp_currentShader->m_selectedTech = 0;
    }

    void    GFXAdapter_GCM::setShaderForFont(bbool bOutline)
    {
        //TODO change POPO ;need a name instead of index
        setShader(mp_shaderManager.getShaderByIndex(3));
        mp_currentShader->m_selectedTech = 4+bOutline*1;
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::beginShader( ITF_shader* _shader )
    {
        setShaderConstant(_shader);
        setShaderMatrix(_shader);
        GCM::Technique* tech = (GCM::Technique*) _shader->getTechniqueSelected();
        tech->checkVertexDeclCompatibility(m_currentVertexDecl);
        tech->apply(m_rdrCtxt);
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::endShader( ITF_shader* _shader )
    {    

    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::setShaderConstant(ITF_shader* _shader)
    {
        // To get same behavior as PC, the m_globalColor is assigned when calling setGlobalColor
        //m_rdrCtxt.paramDB.setFloat4(FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[0]), (FxParameterDB::Vector4 &)m_globalColor);
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::setShaderMatrix( ITF_shader* _shader )
    {
        if (m_useRotateZCamera)
            m_rdrCtxt.paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(mp_currentShader->m_MatrixHandle[0]), m_worldViewProj.getWorldViewRenderProj());
        else
            m_rdrCtxt.paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(mp_currentShader->m_MatrixHandle[0]), m_worldViewProj.getWorldViewProj());
        m_rdrCtxt.paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(mp_currentShader->m_MatrixHandle[1]), m_UVmat);
        m_rdrCtxt.paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(mp_currentShader->m_MatrixHandle[2]), m_worldViewProj.getView());
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_GCM::setShader( ITF_shader* _shader)
    {
        mp_currentShader = _shader;
    }

    void GFXAdapter_GCM::fx_setVectorArray(u32 _fx, void* _h, const GFX_Vector4* _va, u32 _count)
    {
        // ignoring 'fx' param -> using a global parameter db
        m_rdrCtxt.paramDB.setFloat4Array(FxParameterDB::Float4ArrayHdl(_h), (const FxParameterDB::Vector4*) _va, _count);
    }

    void GFXAdapter_GCM::fx_setVector(u32 _fx, void* _h, const GFX_Vector4* _v)
    {
        m_rdrCtxt.paramDB.setFloat4(FxParameterDB::Float4Hdl(_h), *(const FxParameterDB::Vector4*) _v);
    }

    void GFXAdapter_GCM::fx_setFloat(u32 _fx, void* _h, float _f)
    {
        m_rdrCtxt.paramDB.setFloat(FxParameterDB::FloatHdl(_h), _f);
    }

    void GFXAdapter_GCM::fx_setInt(u32 _fx, void* _h, i32 _i)
    {
        m_rdrCtxt.paramDB.setInt(FxParameterDB::FloatHdl(_h), _i);
    }

    void GFXAdapter_GCM::fx_commitChange(u32 _fx )
    {
        // To avoid excessive set of constants, the commit change is not doing anything :
        // this is done at the drawcall
#if 0
        ITF_shader* shader = (ITF_shader*)_fx;
        GCM::Technique* tech = (GCM::Technique*) shader->getTechniqueSelected();
        tech->checkVertexDeclCompatibility(m_currentVertexDecl);
        tech->apply(m_rdrCtxt);*/
#endif
    }


} // namespace ITF
