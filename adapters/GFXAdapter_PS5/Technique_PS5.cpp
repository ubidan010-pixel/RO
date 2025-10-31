#include "precompiled_GFXAdapter_PS5.h"

#include "Technique_PS5.h"
#include "ShaderProgram_PS5.h"

#include "GFXAdapter_struct_PS5.h"
#include "core/XML/PugiXMLWrap.h"
#include "engine/common/WarningManager.h"

#ifndef ITF_FINAL
#define CHECK_VERTEXDECL_SHADER_COMPATIBILITY
#endif


namespace ITF
{
    namespace Gnmp
    {
        void ShaderProgramCache::setIfChanged(Gnmx::GfxContext& _rdrCtxt, VertexProgram* _vertexProg)
        {
            if (_vertexProg != m_vertexProg)
            {
                m_vertexProg = _vertexProg;
                if (m_vertexProg!=nullptr)
                    m_vertexProg->set(_rdrCtxt);
            }
        }
        void ShaderProgramCache::setIfChanged(Gnmx::GfxContext& _rdrCtxt, PixelProgram* _pixelProg)
        {
            if (_pixelProg != m_pixelProg)
            {
                m_pixelProg = _pixelProg;
                if (m_pixelProg != nullptr)
                    m_pixelProg->set(_rdrCtxt);
            }
        }

        void ShaderProgramCache::reset()
        {
            m_vertexProg = nullptr;
            m_pixelProg = nullptr;
        }

        i32 Technique::readNode(PugiXMLWrap & _xmlParser, const String & _fxPathAndName)
        {
            const char * techName = _xmlParser.readAttribute("name");
            String techFullName;
            techFullName.setTextFormat("%ls_%s", _fxPathAndName.cStr(), techName);
            m_techFullName = techFullName;

            if (_strnicmp("Tech_", techName, strlen("Tech_")))
            {
                ITF_WARNING(NULL, bfalse, "Cannot get an ordinal number for Technique %ls", m_techFullName.cStr());
                return -1; // unknown technique
            }
            i32 techNum = atoi(techName+strlen("Tech_"));

            //vertex program
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
                    ITF_WARNING(NULL, bfalse, "Entries in fx file %ls ill formed, last taken", _fxPathAndName.cStr());
                }

                // check for a second pass ?
                if (_xmlParser.nextSameElement(false))
                {
                    ITF_WARNING(NULL, bfalse, "Too much pass used in fx file %ls (additionals ignored)", _fxPathAndName.cStr());
                }
                _xmlParser.exitElement();
            }
            else
            {
                ITF_WARNING(NULL, bfalse, "No pass found in a technique in fx file %ls", _fxPathAndName.cStr());
            }
            return techNum;
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
            vpName.setTextFormat("%ls_%s.vpo", _fxPathAndName.cStr(), _entry);

            m_vertexProg = ShaderProgramRepository::get().getVPO(vpName);
        }

        void Technique::loadFragmentProgram(const String & _fxPathAndName, const char * _entry)
        {
            String fpName;
            fpName.setTextFormat("%ls_%s.fpo", _fxPathAndName.cStr(), _entry);
            m_pixelProg = ShaderProgramRepository::get().getFPO(fpName);
        }

        bool Technique::bind(const FxParameterDB & _paramDB)
        {
            bool successVtx = m_vertexProg->bindParameterDB(_paramDB);
            bool successFrag = m_pixelProg->bindParameterDB(_paramDB);
            return successVtx && successFrag;
        }

        void Technique::set(Gnmx::GfxContext& _rdrCtxt, ShaderProgramCache & shaderCache)
        {
            if (m_vertexProg)
            {
                shaderCache.setIfChanged(_rdrCtxt, m_vertexProg);
            }
            if (m_pixelProg)
            {
                shaderCache.setIfChanged(_rdrCtxt, m_pixelProg);
            }
        }

        void Technique::apply(Gnmx::GfxContext& _rdrCtxt, const FxParameterDB& paramDB)
        {
            if (m_vertexProg)
            {
                m_vertexProg->applyParamDB(_rdrCtxt, paramDB);
            }
            if (m_pixelProg)
            {
                m_pixelProg->applyParamDB(_rdrCtxt, paramDB);
            }
        }
    }
}
