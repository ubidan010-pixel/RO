#include "precompiled_GFXAdapter_NVN.h"

#include "Technique_NVN.h"
#include "ShaderProgram_NVN.h"

#include "GFXAdapter_struct_NVN.h"
#include "core/XML/PugiXMLWrap.h"
#include "engine/common/WarningManager.h"

#ifndef ITF_FINAL
#define CHECK_VERTEXDECL_SHADER_COMPATIBILITY
#endif

namespace ITF
{
    namespace NVN
    {
        void ShaderProgramCache::setIfChanged(nvn::CommandBuffer& _rdrCtxt, Technique* _technique)
        {
            if (_technique != m_lastTechnique)
            {
                nvn::ShaderStageBits stages{0};
                if (_technique->getVertexProgram() != nullptr)
                {
                    stages |= nvn::ShaderStageBits::VERTEX;
                }
                if (_technique->getPixelProgram() != nullptr)
                {
                    stages |= nvn::ShaderStageBits::FRAGMENT;
                }
                NVN::setMarker(&_rdrCtxt, _technique->getName8().cStr());
                _rdrCtxt.BindProgram(_technique->getProgram(), stages);
                m_lastTechnique = _technique;
            }
        }

        void ShaderProgramCache::reset()
        {
            m_lastTechnique = nullptr;
        }

        i32 Technique::initFromXMLNode(nvn::Device& _device, PugiXMLWrap & _xmlParser, const String & _fxPathAndName)
        {
            const char * techName = _xmlParser.readAttribute("name");
            String techFullName;
            techFullName.setTextFormat("%s_%s", StringToUTF8(_fxPathAndName).get(), techName);
            m_techFullName = techFullName;
            m_techFullName8 = StringToUTF8(m_techFullName).get();

            if (_strnicmp("Tech_", techName, strlen("Tech_")))
            {
                ITF_WARNING(NULL, bfalse, "Cannot get an ordinal number for Technique %s", m_techFullName8.cStr());
                return -1; // unknown technique
            }
            i32 techNum = atoi(techName+strlen("Tech_"));

            if (_xmlParser.enterElement("pass"))
            {
                u32 vtxCount = 0, fragCount = 0;
                if (_xmlParser.enterElement("entry"))
                {
                    do 
                    {
                        // the first (and supposedly only) element is our entry point
                        _xmlParser.enterFirstElement();
                        const char * entry = _xmlParser.readElementValue();
                        _xmlParser.exitElement();

                        if (!_strcmpi(_xmlParser.readAttribute("type", "Vertex"), "Fragment"))
                        {
                            loadFragmentProgram(_fxPathAndName, entry);
                            fragCount++;
                        }
                        else
                        {
                            loadVertexProgram(_fxPathAndName, entry);
                            vtxCount++;
                        }
                    } while (_xmlParser.nextSameElement(true));
                }
                if (vtxCount!=1 && fragCount!=1)
                {
                    ITF_WARNING(NULL, bfalse, "Entries in fx file %s ill formed, last taken", StringToUTF8(_fxPathAndName).get());
                }

                // check for a second pass ?
                if (_xmlParser.nextSameElement(false))
                {
                    ITF_WARNING(NULL, bfalse, "Too much pass used in fx file %s (additionals ignored)", StringToUTF8(_fxPathAndName).get());
                }
                _xmlParser.exitElement();
            }
            else
            {
                ITF_WARNING(NULL, bfalse, "No pass found in a technique in fx file %s", StringToUTF8(_fxPathAndName).get());
            }
            createNVNProgram(_device);
            return techNum;
        }

        void Technique::createNVNProgram(nvn::Device& _device)
        {
            m_nvnProgram = NVN::makeInitialized<nvn::Program>(&_device);
        #if defined(ITF_ENABLE_NVN_GRAPHICS_DEBUGGING) && ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
            m_nvnProgram->SetDebugLabel(m_techFullName8.cStr());
        #endif

            Vector<nvn::ShaderData> stageData;
            if (m_vertexProg != nullptr)
            {
                stageData.push_back(m_vertexProg->getShaderData());
            }
            if (m_pixelProg)
            {
                stageData.push_back(m_pixelProg->getShaderData());
            }
            ITF_ASSERT(stageData.size() == 2);
            m_nvnProgram->SetShaders(stageData.size(), stageData.data());
        }

        void Technique::checkVertexDeclCompatibility(const VertexFormatDescriptor & _vertexDecl)
        {
    #ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
            if (m_vertexProg)
            {
                m_vertexProg->checkVertexDeclCompatibility(_vertexDecl);
            }
    #endif
        }

        void Technique::loadVertexProgram(const String & _fxPathAndName, const char * _entry)
        {
            String vpName;
            vpName.setTextFormat("%s_%s.vs.spv.glslo", StringToUTF8(_fxPathAndName).get(), _entry);

            m_vertexProg = ShaderProgramRepository::get().getVPO(vpName);
        }

        void Technique::loadFragmentProgram(const String & _fxPathAndName, const char * _entry)
        {
            String fpName;
            fpName.setTextFormat("%s_%s.fs.spv.glslo", StringToUTF8(_fxPathAndName).get(), _entry);
            m_pixelProg = ShaderProgramRepository::get().getFPO(fpName);
        }

        bool Technique::bind(const FxParameterDB & _paramDB)
        {
            bool successVtx = m_vertexProg->bindParameterDB(_paramDB);
            bool successFrag = m_pixelProg->bindParameterDB(_paramDB);
            return successVtx && successFrag;
        }

        void Technique::set(nvn::CommandBuffer& _rdrCtxt, ShaderProgramCache & shaderCache)
        {
            shaderCache.setIfChanged(_rdrCtxt, this);
        }

        void Technique::apply(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool & _uniformBufferPool, const FxParameterDB& paramDB)
        {
            if (m_vertexProg)
            {
                m_vertexProg->applyParamDB(_rdrCtxt, _uniformBufferPool, paramDB);
            }
            if (m_pixelProg)
            {
                m_pixelProg->applyParamDB(_rdrCtxt, _uniformBufferPool, paramDB);
            }
        }
    }
}
