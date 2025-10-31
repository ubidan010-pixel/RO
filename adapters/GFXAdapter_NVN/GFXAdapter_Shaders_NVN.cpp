#include "precompiled_GFXAdapter_NVN.h"

#include "core/EngineCommon.h"
#include "core/file/File.h"
#include "core/XML/PugiXMLWrap.h"
#include "engine/common/WarningManager.h"
#include "GFXAdapter_NVN.h"
#include "Technique_NVN.h"

///////////////////////////////////////////////////////////////////////////////
// Methods handling fx/technique/shaders stuff of GFXAdapter_PS5
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{

    void GFXAdapter_NVN::createShader(ITF_shader* _shader)
    {
        ITF_NOT_IMPLEMENTED();
        _shader->mp_effect = uPtr(_shader);
    }

    void GFXAdapter_NVN::releaseShader(ITF_shader* _shader)
    {
        ITF_NOT_IMPLEMENTED();
    }


    static const char* HardCodedShaders[] =
    {   // Keep the same order, call to m_shaderManager.getShaderByIndex are giving the hard-coded order of the shader
        // Keep in sync with GFXAdapter_PS5::CoreShaderGroup
        "renderPCT",
        "afterfx",
        "refract",
        "font",
        "renderOVERDRAW",
        "renderFog",
        "movie"
    };

    bool GFXAdapter_NVN::loadCoreShaders()
    {
        static_assert(static_cast<u32>(CoreShaderGroup::COUNT) == ITF_ARRAY_SIZE(HardCodedShaders));

        if (!ShaderProgramRepository::exists())
            new ShaderProgramRepository(); // create and register singleton

        ShaderProgramRepository::instance()->clear();

        for (ux i = 0; i < sizeof(HardCodedShaders) / sizeof(HardCodedShaders[0]); ++i)
        {
            if (!loadShader(HardCodedShaders[i]))
            {
                LOG("Cannot find shader %s\n", HardCodedShaders[i]);
                return false;
            }
        }

        mp_defaultShader = mp_shaderManager.getShaderByIndex(getShaderGroupIndex(CoreShaderGroup::renderPCT));

        m_texelRatioBlendHdl = m_paramDB.findFloat("g_ps_texelRatioBlend");

        return true;
    }

    // load a shader from it's name, assuming default directory
    bool GFXAdapter_NVN::loadShader(const char* _shaderName)
    {
#ifdef ITF_NX
        static const String shadersFolder("Shaders/Compiled/NX/");
#else
        static const String shadersFolder("Shaders/Compiled/OUNCE/");
#endif
        String fullPathShaderName = shadersFolder + _shaderName;
        ITF_shader* shaderFound = mp_shaderManager.getShaderByName(fullPathShaderName);
        if (shaderFound)
        {
            // a reload
            return shaderFound->loadShaderGroup() == btrue;
        }
        else
        {
            u32 nbShader = mp_shaderManager.getNumberShader();
            mp_shaderManager.loadShaderGroup(fullPathShaderName);
            return nbShader + 1 == mp_shaderManager.getNumberShader()
                && nullptr != mp_shaderManager.getShaderByIndex(nbShader);
        }
    }


    void GFXAdapter_NVN::checkShaderReload()
    {
#ifndef ITF_FINAL
        if (m_reloadShader)
        {
            m_reloadShader = false;
            m_mainContext->waitUntilGPUStalled();
            loadCoreShaders();
        }
#endif
    }


    void GFXAdapter_NVN::createShaderParametersDB()
    {
        m_paramDB.registerTexSampler("TextureSampler0", *m_samplerDescriptorPool);
        m_paramDB.registerTexSampler("TextureSampler1", *m_samplerDescriptorPool);
        m_paramDB.registerTexSampler("TextureSampler2", *m_samplerDescriptorPool);
        m_paramDB.registerTexSampler("TextureSampler3", *m_samplerDescriptorPool);

        m_paramDB.registerMatrix4x4("g_mWorldViewProjection");
        m_paramDB.registerMatrix4x4("g_mUVmat");
        m_paramDB.registerMatrix4x4("g_mView");
        m_paramDB.registerMatrix4x4("g_mPatchVx");
        m_paramDB.registerMatrix4x4("g_mPatchUv");

        m_paramDB.registerFloat("g_fconst0");
        m_paramDB.registerFloat("g_fconst1");
        m_paramDB.registerFloat("g_fconst2");
        m_paramDB.registerFloat("g_fconst3");
        m_paramDB.registerFloat("g_fconst4");
        m_paramDB.registerFloat("g_fconst5");
        m_paramDB.registerFloat("g_fconst6");
        m_paramDB.registerFloat("g_fconst7");

        m_paramDB.registerFloat("g_ps_alphaTest");

        m_paramDB.registerFloat("g_ps_texelRatioBlend");

        m_paramDB.registerFloat4Array("g_va0", 8 * 16);
        m_paramDB.registerFloat4Array("g_va1", 4 * 16);

        m_paramDB.registerInt("g_iconst0");

        m_paramDB.registerFloat4("g_vconst0");
        m_paramDB.registerFloat4("g_vconst1");
        m_paramDB.registerFloat4("g_vconst2");
        m_paramDB.registerFloat4("g_vconst3");
        m_paramDB.registerFloat4("g_vconst4");
        m_paramDB.registerFloat4("g_vconst5");
        m_paramDB.registerFloat4("g_vconst6");
        m_paramDB.registerFloat4("g_vconst7");
    }

    static bool readTechniques(nvn::Device& _device, PugiXMLWrap& _xmlParser, ITF_shader* _shader)
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
                NVN::Technique* technique = new NVN::Technique;
                i32 techId = technique->initFromXMLNode(_device, _xmlParser, _shader->m_name);
                if (techId >= 0)
                {
                    if (techId >= sizeof(_shader->m_TechHandle) / sizeof(_shader->m_TechHandle[0]))
                    {
                        // out of place for technique in the shader struct !
                        ITF_WARNING(NULL, bfalse, "Technique number %d is too high (ignored)", techId);
                    }
                    else
                    {
                        // delete the old technique (in case of reload)
                        delete reinterpret_cast<NVN::Technique*>(_shader->m_TechHandle[techId]);
                        _shader->m_TechHandle[techId] = technique;
                    }
                }
            } while (_xmlParser.nextSameElement(true));
        }
        _xmlParser.exitElement();
        return true;
    }

    static void bindShaderParameterHandles(ITF_shader* _shader, const FxParameterDB& _fxParamDB)
    {
        // bind all techniques to the fxParameterDB
        for (void* voidPTech : _shader->m_TechHandle)
        {
            NVN::Technique* tech = reinterpret_cast<NVN::Technique*>(voidPTech);
            if (tech)
            {
                if (!tech->bind(_fxParamDB))
                {
                    ITF_WARNING(NULL, bfalse, "Technique %s incorrectly bound with the engine", tech->getName8().cStr());
                }
            }
        }

        // put fxParamDB handles in hard coded slots
        _shader->m_TextureHandle[0] = _fxParamDB.findTexSampler("TextureSampler0").getAsVoidP();
        _shader->m_TextureHandle[1] = _fxParamDB.findTexSampler("TextureSampler1").getAsVoidP();
        _shader->m_TextureHandle[2] = _fxParamDB.findTexSampler("TextureSampler2").getAsVoidP();
        _shader->m_TextureHandle[3] = _fxParamDB.findTexSampler("TextureSampler3").getAsVoidP();

        /// get Matrix Handle.
        _shader->m_MatrixHandle[0] = _fxParamDB.findMatrix4x4("g_mWorldViewProjection").getAsVoidP();
        _shader->m_MatrixHandle[1] = _fxParamDB.findMatrix4x4("g_mUVmat").getAsVoidP();
        _shader->m_MatrixHandle[2] = _fxParamDB.findMatrix4x4("g_mView").getAsVoidP();
        _shader->m_MatrixHandle[3] = _fxParamDB.findMatrix4x4("g_mPatchVx").getAsVoidP();
        _shader->m_MatrixHandle[4] = _fxParamDB.findMatrix4x4("g_mPatchUv").getAsVoidP();

        _shader->m_FloatHandle[0] = _fxParamDB.findFloat("g_fconst0").getAsVoidP();
        _shader->m_FloatHandle[1] = _fxParamDB.findFloat("g_fconst1").getAsVoidP();
        _shader->m_FloatHandle[2] = _fxParamDB.findFloat("g_fconst2").getAsVoidP();
        _shader->m_FloatHandle[3] = _fxParamDB.findFloat("g_fconst3").getAsVoidP();
        _shader->m_FloatHandle[4] = _fxParamDB.findFloat("g_fconst4").getAsVoidP();
        _shader->m_FloatHandle[5] = _fxParamDB.findFloat("g_fconst5").getAsVoidP();
        _shader->m_FloatHandle[6] = _fxParamDB.findFloat("g_fconst6").getAsVoidP();
        _shader->m_FloatHandle[7] = _fxParamDB.findFloat("g_fconst7").getAsVoidP();

        _shader->m_alphaTestRef = _fxParamDB.findFloat("g_ps_alphaTest").getAsVoidP();

        _shader->m_VectorArrayHandle[0] = _fxParamDB.findFloat4Array("g_va0").getAsVoidP();
        _shader->m_VectorArrayHandle[1] = _fxParamDB.findFloat4Array("g_va1").getAsVoidP();

        _shader->m_IntHandle[0] = (void*)_fxParamDB.findInt("g_iconst0").getAsVoidP();

        _shader->m_VecHandle[0] = (void*)_fxParamDB.findFloat4("g_vconst0").getAsVoidP();
        _shader->m_VecHandle[1] = (void*)_fxParamDB.findFloat4("g_vconst1").getAsVoidP();
        _shader->m_VecHandle[2] = (void*)_fxParamDB.findFloat4("g_vconst2").getAsVoidP();
        _shader->m_VecHandle[3] = (void*)_fxParamDB.findFloat4("g_vconst3").getAsVoidP();
        _shader->m_VecHandle[4] = (void*)_fxParamDB.findFloat4("g_vconst4").getAsVoidP();
        _shader->m_VecHandle[5] = (void*)_fxParamDB.findFloat4("g_vconst5").getAsVoidP();
        _shader->m_VecHandle[6] = (void*)_fxParamDB.findFloat4("g_vconst6").getAsVoidP();
        _shader->m_VecHandle[7] = (void*)_fxParamDB.findFloat4("g_vconst7").getAsVoidP();

    }

#ifndef ITF_FINAL
    static Vector<char> loadShaderDescInStringOutOfBundle(String _descFile)
    {
        Vector<char> content;
        String directAccessFileName = String("rom:/") + _descFile;

        hwFile f;
        if (f.open(directAccessFileName, ITF_FILE_ATTR_READ))
        {
            content.resize(f.getLength());
            u32 sizeRead = 0;
            f.read(content.data(), content.size(), &sizeRead);
            ITF_ASSERT(sizeRead == content.size()); // read error
            content.resize(sizeRead);
        }
        return content;
    }
#endif

    bbool GFXAdapter_NVN::loadShader(ITF_shader* _shader)
    {
        // assign the effect to the shader (the ITF_Shader is the FX object)
        _shader->mp_effect = uPtr(_shader);

        String cookedName;
        cookedName.setTextFormat("%s.fx.xml", StringToUTF8(_shader->m_name).get());

        PugiXMLWrap xmlParser;
        bool openSuccess = false;
#ifndef ITF_FINAL
        Vector<char> content = loadShaderDescInStringOutOfBundle(cookedName);
        openSuccess = !content.empty();
        if (openSuccess)
        {
            content.push_back('\0');
            openSuccess = xmlParser.openString(content.data(), PugiXMLWrap::DoNothing, btrue);
        }
#endif
        if (!openSuccess)
        {
            openSuccess = xmlParser.openFile(cookedName, bfalse);
        }

        if (!openSuccess)
        {
            return bfalse;
        }

        // load every shader in the FX
        if (!readTechniques(*m_nvnDevice, xmlParser, _shader)) // assigning _shader.techHandles
        {
            return bfalse;
        }

        // the new loaded 'effect' need to be bound with the parameter DB
        bindShaderParameterHandles(_shader, m_paramDB);

        return btrue;
    }

    void GFXAdapter_NVN::restoreDefaultShader()
    {
        setShader(mp_defaultShader);
        mp_currentShader->m_selectedTech = 0;
    }

    void GFXAdapter_NVN::setShaderForFont(bbool bOutline)
    {
        //TODO change POPO ;need a name instead of index
        setShader(mp_shaderManager.getShaderByIndex(3));
        mp_currentShader->m_selectedTech = 4 + bOutline * 1;
    }

    ///----------------------------------------------------------------------------//

    void GFXAdapter_NVN::beginShader(ITF_shader* _shader)
    {
    #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        m_currentShaderInBeginEndIndex = mp_shaderManager.getShaderIndex(_shader);
        m_currentShaderInBeginEndTech = _shader->m_selectedTech;
    #endif

        if (isDrawAllowed(false))
        {
            setShaderMatrix(_shader);
            NVN::Technique* tech = reinterpret_cast<NVN::Technique*>(_shader->getTechniqueSelected());
            tech->checkVertexDeclCompatibility(getVertexFormatDescriptor(m_currentVertexFormat));
            tech->set(*getRenderingContext(), *m_shaderProgramCache);
            tech->apply(*getRenderingContext(), *m_uniformBufferPool, m_paramDB);
        }
    }

#if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
    bool GFXAdapter_NVN::isDrawAllowed(bool _incrementDrawCallIndex)
    {
        bool drawAllowed = m_allowedBlendMode == GFX_BLEND_UNKNOWN || u_CurrentBlendingMode == m_allowedBlendMode;
        if (m_currentShaderInBeginEndIndex != -1 && m_allowedShaderIdx != -1)
        {
            drawAllowed = drawAllowed && (m_currentShaderInBeginEndIndex == m_allowedShaderIdx);
            if (m_currentShaderInBeginEndTech != -1 && m_allowedShaderTech != -1)
            {
                drawAllowed = drawAllowed && (m_currentShaderInBeginEndTech == m_allowedShaderTech);
            }
        }
        bool inCount = drawAllowed;
        if (m_allowedDrawCallIndexInFrame != -1)
        {
            drawAllowed = drawAllowed && (m_currentDrawCallIndexInFrame == m_allowedDrawCallIndexInFrame);
        }

        if (inCount && _incrementDrawCallIndex)
            m_currentDrawCallIndexInFrame++;
        return drawAllowed;
    }
#endif

    void GFXAdapter_NVN::endShader(ITF_shader* _shader)
    {
    #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        m_currentShaderInBeginEndIndex = -1;
        m_currentShaderInBeginEndTech = -1;
    #endif
    }

    void GFXAdapter_NVN::setShaderMatrix(ITF_shader* _shader)
    {
        if (m_useRotateZCamera)
            m_paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(_shader->m_MatrixHandle[0]), m_worldViewProj.getWorldViewRenderProj());
        else
            m_paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(_shader->m_MatrixHandle[0]), m_worldViewProj.getWorldViewProj());
        m_paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(_shader->m_MatrixHandle[1]), m_UVmat);
        m_paramDB.setMatrix4x4(FxParameterDB::Matrix4x4Hdl(_shader->m_MatrixHandle[2]), m_worldViewProj.getView());
    }

    void GFXAdapter_NVN::setShader(ITF_shader* _shader)
    {
        mp_currentShader = _shader;
    }

    void GFXAdapter_NVN::setShader(CoreShaderGroup group)
    {
        setShader(mp_shaderManager.getShaderByIndex(getShaderGroupIndex(group)));
    }

    void GFXAdapter_NVN::fx_setVectorArray(uPtr _fx, void* _h, const GFX_Vector4* _va, u32 _count)
    {
        // ignoring 'fx' param -> using a global parameter db
        m_paramDB.setFloat4Array(FxParameterDB::Float4ArrayHdl(_h), reinterpret_cast<const Float4*>(_va), _count);
    }

    void GFXAdapter_NVN::fx_setVector(uPtr _fx, void* _h, const GFX_Vector4* _v)
    {
        m_paramDB.setFloat4(FxParameterDB::Float4Hdl(_h), Float4{ _v->m_x, _v->m_y, _v->m_z, _v->m_w });
    }

    void GFXAdapter_NVN::fx_setFloat(uPtr _fx, void* _h, float _f)
    {
        m_paramDB.setFloat(FxParameterDB::FloatHdl(_h), _f);
    }

    void GFXAdapter_NVN::fx_setInt(uPtr _fx, void* _h, i32 _i)
    {
        m_paramDB.setInt(FxParameterDB::FloatHdl(_h), _i);
    }

    void GFXAdapter_NVN::fx_commitChange(uPtr _fx)
    {
        // To avoid excessive set of constants, the commit change is not doing anything :
        // this is done at the drawcall (beginShader)
    #if 0
            ITF_shader* shader = (ITF_shader*)_fx;
            NVN::Technique* tech = (GCM::Technique*)shader->getTechniqueSelected();
            tech->checkVertexDeclCompatibility(m_currentVertexDecl);
            tech->apply(m_rdrCtxt); */
    #endif
    }

} // namespace ITF
